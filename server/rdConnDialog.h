
class rdConnDialog;

#if (!defined(_WINrd_rdCONNDIALOG))
#define _WINrd_rdCONNDIALOG

#pragma once

#include "rdServer.h"

// Outgoing connection dialog.  This allows people running rd servers on
// Win32 platforms to _push_ their displays out to other people's screens
// rather than having to _pull_ them across.

class rdConnDialog  
{
public:

	// Create an outgoing-connection dialog
	rdConnDialog(rdServer *server);

	// Destructor
	virtual ~rdConnDialog();

	// Once a dialog object is created, either delete it again, or
	// call DoDialog.  DoDialog will run the object and delete it when done
	void DoDialog();

	// Internal stuffs
private:

	// Routine to call when a dialog event occurs
	static BOOL CALLBACK rdConnDlgProc(HWND hwndDlg,
										UINT uMsg, 
										WPARAM wParam,
										LPARAM lParam);

	// Pointer back to the server object
	rdServer *m_server;
};

#endif
