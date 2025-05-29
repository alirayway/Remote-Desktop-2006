
class rdAcceptDialog;

#if (!defined(_WINrd_rdACCEPTDIALOG))
#define _WINrd_rdACCEPTDIALOG

#pragma once

// Incoming connection-rejection dialog.  rdClient creates an AcceptDialog
// if it needs to query whether or not to accept a connection.

class rdAcceptDialog  
{
public:

	// Create an outgoing-connection dialog
	rdAcceptDialog(UINT timeoutSecs,
					BOOL acceptOnTimeout,
					BOOL allowNoPass,
					const char *ipAddress);

	// Destructor
	virtual ~rdAcceptDialog();

	// Once a dialog object is created, either delete it again, or
	// call DoDialog.  DoDialog will run the dialog and return
	// TRUE (Accept) or FALSE (Reject).
	// 1: Accept, 2: Accept w/o Password
	// The function will also return false (or true, if set to accept at timeout)
	// if the dialog times out.
	BOOL DoDialog();

	// Internal stuffs
private:

	// Routine to call when a dialog event occurs
	static BOOL CALLBACK rdAcceptDlgProc(HWND hwndDlg,
										UINT uMsg, 
										WPARAM wParam,
										LPARAM lParam);

	// Storage for the timeout value
	UINT m_timeoutSecs;
	UINT m_timeoutCount;

	// Flashing hack
	BOOL m_foreground_hack;
	BOOL m_flash_state;

	// Address of the offending machine
	char *m_ipAddress;

	// Whether to accept or reject on default/timeout
	BOOL m_acceptOnTimeout;
	BOOL m_allowNoPass;

};

#endif
