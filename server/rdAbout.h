
// rdAbout

// Object implementing the About dialog for Winrd.

class rdAbout;

#if (!defined(_WINrd_rdABOUT))
#define _WINrd_rdABOUT

// Includes
#include "stdhdrs.h"

// The rdAbout class itself
class rdAbout
{
public:
	// Constructor/destructor
	rdAbout();
	~rdAbout();

	// Initialisation
	BOOL Init();

	// The dialog box window proc
	static BOOL CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// General
	void Show(BOOL show);

	// Implementation
	BOOL m_dlgvisible;
};

#endif // _WINrd_rdABOUT
