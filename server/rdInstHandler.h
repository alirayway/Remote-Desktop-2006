

// rdInstHandler

// The WinMain procedure for Winrd produces one of these objects.
// It creates a named mutex and checks to see whether that mutex
// already existed in the system.  If it did, then there is a previous
// instance of Winrd running, which must be requested to quit cleanly.

class rdInstHandler;

#if (!defined(_WINrd_rdINSTHANDLER))
#define _WINrd_rdINSTHANDLER

// Includes
#include "stdhdrs.h"

// The rdInstHandler class itself
class rdInstHandler
{
public:
	// Constructor/destructor
	BOOL Init();
};

#endif // _WINrd_rdINSTHANDLER
