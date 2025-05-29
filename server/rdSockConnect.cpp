
// rdSockConnect.cpp

// Implementation of the listening socket class

#include "stdhdrs.h"
#include "VSocket.h"
#include "rdSockConnect.h"
#include "rdServer.h"
#include <omnithread.h>

// The function for the spawned thread to run
class rdSockConnectThread : public omni_thread
{
public:
	// Init routine
	virtual BOOL Init(VSocket *socket, rdServer *server);

	// Code to be executed by the thread
	virtual void *run_undetached(void * arg);

	// Fields used internally
	BOOL		m_shutdown;
protected:
	VSocket		*m_socket;
	rdServer	*m_server;
};

// Method implementations
BOOL rdSockConnectThread::Init(VSocket *socket, rdServer *server)
{
	// Save the server pointer
	m_server = server;

	// Save the socket pointer
	m_socket = socket;

	// Start the thread
	m_shutdown = FALSE;
	start_undetached();

	return TRUE;
}

// Code to be executed by the thread
void *rdSockConnectThread::run_undetached(void * arg)
{
	rdlog.Print(LL_STATE, rdLOG("started socket connection thread\n"));

	// Go into a loop, listening for connections on the given socket
	for (;;) {
		// Accept an incoming connection
		VSocket *new_socket = m_socket->Accept();
		if (new_socket == NULL || m_shutdown)
			break;

		rdlog.Print(LL_CLIENTS, rdLOG("accepted connection from %s\n"),
					 new_socket->GetPeerName());

		// Successful accept - start the client unauthenticated
		m_server->AddClient(new_socket, FALSE, FALSE);
	}
	rdlog.Print(LL_STATE, rdLOG("quitting socket connection thread\n"));

	return NULL;
}

// The rdSockConnect class implementation

rdSockConnect::rdSockConnect()
{
	m_thread = NULL;
}

rdSockConnect::~rdSockConnect()
{
    m_socket.Shutdown();

    // Join with our lovely thread
    if (m_thread != NULL)
    {
	// *** This is a hack to force the listen thread out of the accept call,
	// because Winsock accept semantics are broken
	((rdSockConnectThread *)m_thread)->m_shutdown = TRUE;

	VSocket socket;
	socket.Create();
	socket.Bind(0);
	socket.Connect("localhost", m_port);
	socket.Close();

	void *returnval;
	m_thread->join(&returnval);
	m_thread = NULL;

	m_socket.Close();
    }
}

BOOL rdSockConnect::Init(rdServer *server, UINT port)
{
	// Save the port id
	m_port = port;

	// Create the listening socket
	if (!m_socket.Create())
		return FALSE;

	// Bind it
	if (!m_socket.Bind(m_port, server->LoopbackOnly()))
		return FALSE;

	// Set it to listen
	if (!m_socket.Listen())
		return FALSE;

	// Create the new thread
	m_thread = new rdSockConnectThread;
	if (m_thread == NULL)
		return FALSE;

	// And start it running
	return ((rdSockConnectThread *)m_thread)->Init(&m_socket, server);
}

