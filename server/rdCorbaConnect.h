

// rdCorbaConnect

// The rdCorbaConnect object makes the Winrd server available through
// the CORBA desktop control interface

class rdCorbaConnect;

#if (!defined(_WINrd_rdCORBACONNECT))
#define _WINrd_rdCORBACONNECT

// Includes
#include "stdhdrs.h"

// The following class definition is only used if CORBA control
// is to be enabled in the final executable
#if(defined(_CORBA))

#include <omniorb2/CORBA.h>
#include <omnithread.h>
#include "rd.hh"
#include "rdServer.h"

// The rdCorbaConnect class itself
class rdCorbaConnect
{
public:
	// Constructor/destructor
	rdCorbaConnect();
	~rdCorbaConnect();

	// Init
	virtual BOOL Init(rdServer *server);

	// Implementation
protected:
	// Internal methods
	virtual BOOL InitQuiet(rdServer *server);
	virtual BOOL InitCorba(int argc, char *argv[]);
	virtual void ShutdownCorba(void);
	virtual CORBA::Boolean BindDesktop(CORBA::Object_ptr obj);

	// General
	rdServer			*m_server;

	// The actual CORBA stuff;
	CORBA::ORB_ptr		m_orb;						// The overall ORB object
	CORBA::BOA_ptr		m_boa;

	rd::_sk_controller	*m_controller;

	char				*m_username;
	char				*m_desktop;

	CORBA::ULong		m_lastconntime;

	omni_mutex			m_updateLock;
	
	UINT				m_port;
};

#else // _CORBA

#include "rdServer.h"

// The rdCorbaConnect class itself

class rdCorbaConnect
{
public:
	// Init
	virtual BOOL Init(rdServer *server) {return FALSE;};
};

#endif // _CORBA

#endif // _WINrd_rdCORBACONNECT
