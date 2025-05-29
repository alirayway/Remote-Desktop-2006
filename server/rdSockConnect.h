

// rdSockConnect.h

// The rdSockConnect class creates a listening socket and binds
// it to the specified port.  It then creates a listen thread which
// goes into a loop, listening on the socket.
// When the rdSockConnect object is destroyed, all resources are
// freed automatically, including the listen thread.

class rdSockConnect;

#if (!defined(_WINrd_rdSOCKCONNECT))
#define _WINrd_rdSOCKCONNECT

// Includes
#include "stdhdrs.h"
#include "VSocket.h"
#include "rdServer.h"
#include <omnithread.h>

// The rdSockConnect class itself
class rdSockConnect
{
public:
	// Constructor/destructor
	rdSockConnect();
	~rdSockConnect();

	// Init
	virtual VBool Init(rdServer *server, UINT port);

	// Implementation
protected:
	// The listening socket
	VSocket m_socket;

	// The port to listen on
	UINT m_port;

	// The in-coming accept thread
	omni_thread *m_thread;
};

#endif // _WINrd_rdSOCKCONNECT
