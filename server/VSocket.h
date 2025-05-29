

// VSocket.h

// RFB V3.0

// The VSocket class provides simple socket functionality,
// independent of platform.  Hurrah.

class VSocket;

#if (!defined(_ATT_VSOCKET_DEFINED))
#define _ATT_VSOCKET_DEFINED

#include <omnithread.h>
#include "VTypes.h"

// This class is used as a part of output queue
class AIOBlock
{
public:
	size_t data_size;		// Data size in this block
	char *data_ptr;			// Beginning of the data buffer
	AIOBlock *next;			// Next block or NULL for the last block

	AIOBlock(int size, const char *data = NULL) {
		next = NULL;
		data_size = size;
		data_ptr = new char[size];
		if (data_ptr && data)
			memcpy(data_ptr, data, size);
	}
	~AIOBlock() {
		if (data_ptr)
			delete[] data_ptr;
	}
};

////////////////////////////
// Socket implementation

// Create one or more VSocketSystem objects per application
class VSocketSystem
{
public:
	VSocketSystem();
	~VSocketSystem();
	VBool Initialised() {return m_status;};
private:
	VBool m_status;
};

// The main socket class
class VSocket
{
public:
  // Constructor/Destructor
  VSocket();
  virtual ~VSocket();

  ////////////////////////////
  // Socket implementation

  // Create
  //        Create a socket and attach it to this VSocket object
  VBool Create();

  // Shutdown
  //        Shutdown the currently attached socket
  VBool Shutdown();

  // Close
  //        Close the currently attached socket
  VBool Close();

  // Bind
  //        Bind the attached socket to the specified port
  //		If localOnly is VTrue then the socket is bound only
  //        to the loopback adapter. If checkIfInUse is VTrue,
  //        then the socket won't be bound to an address which
  //        is already in use (i.e. accepts connections).
  VBool Bind(const VCard port, const VBool localOnly = VFalse,
			 const VBool checkIfInUse = VFalse);

  // Connect
  //        Make a stream socket connection to the specified port
  //        on the named machine.
  VBool Connect(const VString address, const VCard port);

  // Listen
  //        Set the attached socket to listen for connections
  VBool Listen();

  // Accept
  //        If the attached socket is set to listen then this
  //        call blocks waiting for an incoming connection, then
  //        returns a new socket object for the new connection
  VSocket *Accept();

  // GetPeerName
  //        If the socket is connected then this returns the name
  //        of the machine to which it is connected.
  //        This string MUST be copied before the next socket call...
  VString GetPeerName();

  // GetSockName
  //		If the socket exists then the name of the local machine
  //		is returned.  This string MUST be copied before the next
  //		socket call!
  VString GetSockName();

  // Resolve
  //        Uses the Winsock API to resolve the supplied DNS name to
  //        an IP address and returns it as an Int32
  static VCard32 Resolve(const VString name);

  // SetTimeout
  //        Sets the socket timeout on reads and writes.
  VBool SetTimeout(VCard32 secs);

  // I/O routines

  // Send and Read return the number of bytes sent or recieved.
  VInt Send(const char *buff, const VCard bufflen);
  VInt Read(char *buff, const VCard bufflen);

  // SendExact and ReadExact attempt to send and recieve exactly
  // the specified number of bytes.
  VBool SendExact(const char *buff, const VCard bufflen);
  VBool ReadExact(char *buff, const VCard bufflen);

  // SendQueued sends as much data as possible immediately,
  // and puts remaining bytes in a queue, to be sent later.
  VBool SendQueued(const char *buff, const VCard bufflen);

  ////////////////////////////
  // Internal structures
protected:
  // The internal socket id
  int sock;

  // Output queue
  size_t bytes_sent;
  AIOBlock *out_queue;
  omni_mutex queue_lock;

  VBool SendFromQueue();
};

#endif // _ATT_VSOCKET_DEFINED
