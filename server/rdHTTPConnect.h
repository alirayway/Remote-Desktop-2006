
// rdHTTPConnect.h

// The rdHTTPConnect class creates a listening socket and binds
// it to the specified port number.  It then creates a listen
// thread which goes into a loop, listening on the socket.
// When the rdHTTPConnect object is destroyed, all resources are
// freed automatically, including the listen thread.
// This server allows clients to request the java classes required
// to view the desktop remotely.

class rdHTTPConnect;

#if (!defined(_WINrd_rdHTTPCONNECT))
#define _WINrd_rdHTTPCONNECT

// Includes
#include "stdhdrs.h"
#include "VSocket.h"
#include "rdServer.h"
#include <omnithread.h>

// The rdHTTPConnect class itself
class rdHTTPConnect
{
public:
	// Constructor/destructor
	rdHTTPConnect();
	~rdHTTPConnect();

	// Init
	virtual VBool Init(rdServer *server, UINT listen_port, BOOL allow_params);

	// Implementation
protected:
	// The listening socket
	VSocket m_listen_socket;

	// The port to listen on
	UINT m_listen_port;

	// The in-coming accept thread
	omni_thread *m_listen_thread;

	// Allow passing applet parameters in the URL
	BOOL m_allow_params;
};

#endif // _WINrd_rdHTTPCONNECT
