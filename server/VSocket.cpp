

// VSocket.cpp

// The VSocket class provides a platform-independent socket abstraction
// with the simple functionality required for an RFB server.

class VSocket;

////////////////////////////////////////////////////////
// System includes

#include "stdhdrs.h"
#include <iostream.h>

#include <stdio.h>
#ifdef __WIN32__
#include <io.h>
#include <winsock.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#endif
#include <sys/types.h>

////////////////////////////////////////////////////////
// Custom includes

#include "VTypes.h"

////////////////////////////////////////////////////////
// *** Lovely hacks to make Win32 work.  Hurrah!

#ifdef __WIN32__
#define EWOULDBLOCK WSAEWOULDBLOCK
#endif

////////////////////////////////////////////////////////
// Socket implementation

#include "VSocket.h"

// The socket timeout value (currently 5 seconds, for no reason...)
// *** THIS IS NOT CURRENTLY USED ANYWHERE
const VInt rfbMaxClientWait = 5000;

////////////////////////////
// Socket implementation initialisation

static WORD winsockVersion = 0;

VSocketSystem::VSocketSystem()
{
  // Initialise the socket subsystem
  // This is only provided for compatibility with Windows.

#ifdef __WIN32__
  // Initialise WinPoxySockets on Win32
  WORD wVersionRequested;
  WSADATA wsaData;
	
  wVersionRequested = MAKEWORD(2, 0);
  if (WSAStartup(wVersionRequested, &wsaData) != 0)
  {
    m_status = VFalse;
	return;
  }

  winsockVersion = wsaData.wVersion;
 
#else
  // Disable the nasty read/write failure signals on UNIX
  signal(SIGPIPE, SIG_IGN);
#endif

  // If successful, or if not required, then continue!
  m_status = VTrue;
}

VSocketSystem::~VSocketSystem()
{
	if (m_status)
	{
		WSACleanup();
	}
}

////////////////////////////

VSocket::VSocket()
{
  // Clear out the internal socket fields
  sock = -1;
  out_queue = NULL;
}

////////////////////////////

VSocket::~VSocket()
{
  // Close the socket
  Close();
}

////////////////////////////

VBool
VSocket::Create()
{
  const int one = 1;

  // Check that the old socket was closed
  if (sock >= 0)
    Close();

  // Create the socket
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      return VFalse;
    }

  // Set the socket options:
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one)))
    {
      return VFalse;
    }
  if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&one, sizeof(one)))
	{
	  return VFalse;
	}

  return VTrue;
}

////////////////////////////

VBool
VSocket::Close()
{
  if (sock >= 0)
    {
	  rdlog.Print(LL_SOCKINFO, rdLOG("closing socket\n"));

	  shutdown(sock, SD_BOTH);
#ifdef __WIN32__
	  closesocket(sock);
#else
	  close(sock);
#endif
      sock = -1;
    }
  while (out_queue)
	{
	  AIOBlock *next = out_queue->next;
	  delete out_queue;
	  out_queue = next;
	}

  return VTrue;
}

////////////////////////////

VBool
VSocket::Shutdown()
{
  if (sock >= 0)
    {
	  rdlog.Print(LL_SOCKINFO, rdLOG("shutdown socket\n"));

	  shutdown(sock, SD_BOTH);
    }
  while (out_queue)
	{
	  AIOBlock *next = out_queue->next;
	  delete out_queue;
	  out_queue = next;
	}

  return VTrue;
}

////////////////////////////

VBool
VSocket::Bind(const VCard port, const VBool localOnly,
			  const VBool checkIfInUse)
{
  struct sockaddr_in addr;

  // Check that the socket is open!
  if (sock < 0)
    return VFalse;

  // If a specific port is being set then check it's not already used!
  if (port != 0 && checkIfInUse)
  {
	VSocket dummy;

	if (dummy.Create())
	{
		// If we're able to connect then the port number is in use...
		if (dummy.Connect("localhost", port))
			return VFalse;
	}
  }

  // Set up the address to bind the socket to
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if (localOnly)
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  else
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

  // And do the binding
  if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
      return VFalse;

  return VTrue;
}

////////////////////////////

VBool
VSocket::Connect(const VString address, const VCard port)
{
  // Check the socket
  if (sock < 0)
    return VFalse;

  // Create an address structure and clear it
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));

  // Fill in the address if possible
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(address);

  // Was the string a valid IP address?
  if (addr.sin_addr.s_addr == -1)
    {
      // No, so get the actual IP address of the host name specified
      struct hostent *pHost;
      pHost = gethostbyname(address);
      if (pHost != NULL)
	  {
		  if (pHost->h_addr == NULL)
			  return VFalse;
		  addr.sin_addr.s_addr = ((struct in_addr *)pHost->h_addr)->s_addr;
	  }
	  else
	    return VFalse;
    }

  // Set the port number in the correct format
  addr.sin_port = htons(port);

  // Actually connect the socket
  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    return VFalse;

  // Put the socket into non-blocking mode
#ifdef __WIN32__
  u_long arg = 1;
  if (ioctlsocket(sock, FIONBIO, &arg) != 0)
	return VFalse;
#else
  if (fcntl(sock, F_SETFL, O_NDELAY) != 0)
	return VFalse;
#endif

  return VTrue;
}

////////////////////////////

VBool
VSocket::Listen()
{
  // Check socket
  if (sock < 0)
    return VFalse;

	// Set it to listen
  if (listen(sock, 5) < 0)
    return VFalse;

  return VTrue;
}

////////////////////////////

VSocket *
VSocket::Accept()
{
  const int one = 1;

  int new_socket_id;
  VSocket * new_socket;

  // Check this socket
  if (sock < 0)
    return NULL;

  // Accept an incoming connection
  if ((new_socket_id = accept(sock, NULL, 0)) < 0)
    return NULL;

  // Create a new VSocket and return it
  new_socket = new VSocket;
  if (new_socket != NULL)
    {
      new_socket->sock = new_socket_id;
    }
  else
    {
	  shutdown(new_socket_id, SD_BOTH);
	  closesocket(new_socket_id);
	  return NULL;
    }

  // Attempt to set the new socket's options
  setsockopt(new_socket->sock, IPPROTO_TCP, TCP_NODELAY, (char *)&one, sizeof(one));

  // Put the socket into non-blocking mode
#ifdef __WIN32__
  u_long arg = 1;
  if (ioctlsocket(new_socket->sock, FIONBIO, &arg) != 0) {
	delete new_socket;
	new_socket = NULL;
  }
#else
  if (fcntl(new_socket->sock, F_SETFL, O_NDELAY) != 0) {
	delete new_socket;
	new_socket = NULL;
  }
#endif

  return new_socket;
}

////////////////////////////

VString
VSocket::GetPeerName()
{
	struct sockaddr_in	sockinfo;
	struct in_addr		address;
	int					sockinfosize = sizeof(sockinfo);
	VString				name;

	// Get the peer address for the client socket
	getpeername(sock, (struct sockaddr *)&sockinfo, &sockinfosize);
	memcpy(&address, &sockinfo.sin_addr, sizeof(address));

	name = inet_ntoa(address);
	if (name == NULL)
		return "<unavailable>";
	else
		return name;
}

////////////////////////////

VString
VSocket::GetSockName()
{
	struct sockaddr_in	sockinfo;
	struct in_addr		address;
	int					sockinfosize = sizeof(sockinfo);
	VString				name;

	// Get the peer address for the client socket
	getsockname(sock, (struct sockaddr *)&sockinfo, &sockinfosize);
	memcpy(&address, &sockinfo.sin_addr, sizeof(address));

	name = inet_ntoa(address);
	if (name == NULL)
		return "<unavailable>";
	else
		return name;
}

////////////////////////////

VCard32
VSocket::Resolve(const VString address)
{
  VCard32 addr;

  // Try converting the address as IP
  addr = inet_addr(address);

  // Was it a valid IP address?
  if (addr == 0xffffffff)
    {
      // No, so get the actual IP address of the host name specified
      struct hostent *pHost;
      pHost = gethostbyname(address);
      if (pHost != NULL)
	  {
		  if (pHost->h_addr == NULL)
			  return 0;
		  addr = ((struct in_addr *)pHost->h_addr)->s_addr;
	  }
	  else
		  return 0;
    }

  // Return the resolved IP address as an integer
  return addr;
}

////////////////////////////

VBool
VSocket::SetTimeout(VCard32 secs)
{
	if (LOBYTE(winsockVersion) < 2)
		return VFalse;
	int timeout=secs;
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == SOCKET_ERROR)
	{
		return VFalse;
	}
	if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) == SOCKET_ERROR)
	{
		return VFalse;
	}
	return VTrue;
}

////////////////////////////

VInt
VSocket::Send(const char *buff, const VCard bufflen)
{
	errno = 0;

	VInt bytes = send(sock, buff, bufflen, 0);
#ifdef __WIN32__
	if (bytes < 0 && WSAGetLastError() == WSAEWOULDBLOCK)
		errno = EWOULDBLOCK;
#endif

	return bytes;
}

////////////////////////////

VBool
VSocket::SendExact(const char *buff, const VCard bufflen)
{
	struct fd_set write_fds;
	struct timeval tm;
	int count;

	// Put the data into the queue
	SendQueued(buff, bufflen);

	while (out_queue) {
		// Wait until some data can be sent
		do {
			FD_ZERO(&write_fds);
			FD_SET((unsigned int)sock, &write_fds);
			tm.tv_sec = 1;
			tm.tv_usec = 0;
			count = select(sock + 1, NULL, &write_fds, NULL, &tm);
		} while (count == 0);
		if (count < 0 || count > 1) {
			rdlog.Print(LL_SOCKERR, rdLOG("socket error in select()\n"));
			return VFalse;
		}
		// Actually send some data
		if (FD_ISSET((unsigned int)sock, &write_fds)) {
			if (!SendFromQueue())
				return VFalse;
		}
    }

	return VTrue;
}

////////////////////////////

VBool
VSocket::SendQueued(const char *buff, const VCard bufflen)
{
	omni_mutex_lock l(queue_lock);

	// Just append new bytes to the output queue
	if (!out_queue) {
		out_queue = new AIOBlock(bufflen, buff);
		bytes_sent = 0;
	} else {
		AIOBlock *last = out_queue;
		while (last->next)
			last = last->next;
		last->next = new AIOBlock(bufflen, buff);
	}

	return VTrue;
}

////////////////////////////

VBool
VSocket::SendFromQueue()
{
	omni_mutex_lock l(queue_lock);

	// Is there something to send?
	if (!out_queue)
		return VTrue;

	// Maximum data size to send at once
	size_t portion_size = out_queue->data_size - bytes_sent;
	if (portion_size > 32768)
		portion_size = 32768;

	// Try to send some data
	int bytes = Send(out_queue->data_ptr + bytes_sent, portion_size);
	if (bytes > 0) {
		bytes_sent += bytes;
	} else if (bytes < 0 && errno != EWOULDBLOCK) {
		rdlog.Print(LL_SOCKERR, rdLOG("socket error\n"));
		return VFalse;
	}

	// Remove block if all its data has been sent
	if (bytes_sent == out_queue->data_size) {
		AIOBlock *sent = out_queue;
		out_queue = sent->next;
		bytes_sent = 0;
		delete sent;
	}

	return VTrue;
}

////////////////////////////

VInt
VSocket::Read(char *buff, const VCard bufflen)
{
	errno = 0;

	VInt bytes = recv(sock, buff, bufflen, 0);

#ifdef __WIN32__
	if (bytes < 0 && WSAGetLastError() == WSAEWOULDBLOCK)
		errno = EWOULDBLOCK;
#endif

	return bytes;
}

////////////////////////////

VBool
VSocket::ReadExact(char *buff, const VCard bufflen)
{
	int bytes;
	VCard currlen = bufflen;
	struct fd_set read_fds, write_fds;
	struct timeval tm;
	int count;

	while (currlen > 0) {
		// Wait until some data can be read or sent
		do {
			FD_ZERO(&read_fds);
			FD_SET((unsigned int)sock, &read_fds);
			FD_ZERO(&write_fds);
			if (out_queue)
				FD_SET((unsigned int)sock, &write_fds);
			tm.tv_sec = 0;
			tm.tv_usec = 50;
			count = select(sock + 1, &read_fds, &write_fds, NULL, &tm);
		} while (count == 0);
		if (count < 0 || count > 2) {
			rdlog.Print(LL_SOCKERR, rdLOG("socket error in select()\n"));
			return VFalse;
		}
		if (FD_ISSET((unsigned int)sock, &write_fds)) {
			// Try to send some data
			if (!SendFromQueue())
				return VFalse;
		}
		if (FD_ISSET((unsigned int)sock, &read_fds)) {
			// Try to read some data in
			bytes = Read(buff, currlen);
			if (bytes > 0) {
				// Adjust the buffer position and size
				buff += bytes;
				currlen -= bytes;
			} else if (bytes < 0 && errno != EWOULDBLOCK) {
				rdlog.Print(LL_SOCKERR, rdLOG("socket error\n"));
				return VFalse;
			} else if (bytes == 0) {
				rdlog.Print(LL_SOCKERR, rdLOG("zero bytes read\n"));
				return VFalse;
			}
		}
    }

	return VTrue;
}

