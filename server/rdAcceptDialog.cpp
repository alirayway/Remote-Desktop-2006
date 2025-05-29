// rdAcceptDialog.cpp: implementation of the rdAcceptDialog class, used
// to query whether or not to accept incoming connections.

#include "stdhdrs.h"
#include "rdAcceptDialog.h"
#include "Winrd.h"
#include "rdService.h"

#include "resource.h"

// Constructor

rdAcceptDialog::rdAcceptDialog(UINT timeoutSecs,
								 BOOL acceptOnTimeout,
								 BOOL allowNoPass,
								 const char *ipAddress)
{
	m_timeoutSecs = timeoutSecs;
	m_acceptOnTimeout = acceptOnTimeout;
	m_allowNoPass = allowNoPass;
	m_ipAddress = strdup(ipAddress);
}

// Destructor

rdAcceptDialog::~rdAcceptDialog()
{
	if (m_ipAddress)
		free(m_ipAddress);
}

// Routine called to activate the dialog

int rdAcceptDialog::DoDialog()
{
	int retVal = DialogBoxParam(hAppInstance,
								MAKEINTRESOURCE(IDD_ACCEPT_CONN),
								NULL,
								(DLGPROC)rdAcceptDlgProc,
								(LPARAM)this);
	switch (retVal) {
	case IDREJECT:
		return 0;
	case IDACCEPT:
		return 1;
	case IDACCEPT_NOPASS:
		return 2;
	}

	// If the dialog box could not be shown, return the default action code
	return (m_acceptOnTimeout) ? 1 : 0;
}

// Callback function - handles messages sent to the dialog box

BOOL CALLBACK rdAcceptDialog::rdAcceptDlgProc(HWND hwnd,
											UINT uMsg,
											WPARAM wParam,
											LPARAM lParam) {
	// This is a static method, so we don't know which instantiation we're 
	// dealing with. But we can get a pseudo-this from the parameter to 
	// WM_INITDIALOG, which we therafter store with the window and retrieve
	// as follows:
	rdAcceptDialog *_this = (rdAcceptDialog *) GetWindowLong(hwnd, GWL_USERDATA);

	switch (uMsg) {

		// Dialog has just been created
	case WM_INITDIALOG:
		{
			// Save the lParam into our user data so that subsequent calls have
			// access to the parent C++ object

            SetWindowLong(hwnd, GWL_USERDATA, lParam);
            rdAcceptDialog *_this = (rdAcceptDialog *) lParam;

			// Disable the "Accept without password" button if needed
			HWND hNoPass = GetDlgItem(hwnd, IDACCEPT_NOPASS);
			EnableWindow(hNoPass, _this->m_allowNoPass);

			// Set the IP-address string
			SetDlgItemText(hwnd, IDC_ACCEPT_IP, _this->m_ipAddress);
			if (SetTimer(hwnd, 1, 1000, NULL) == 0)
			{
				if (_this->m_acceptOnTimeout)
					EndDialog(hwnd, IDACCEPT);
				else
				EndDialog(hwnd, IDREJECT);
			}
			_this->m_timeoutCount = _this->m_timeoutSecs;
			// Update the displayed count
			char temp[256];
			if (_this->m_acceptOnTimeout)
				sprintf(temp, "AutoAccept:%u", (_this->m_timeoutCount));
			else
				sprintf(temp, "AutoReject:%u", (_this->m_timeoutCount));
			SetDlgItemText(hwnd, IDC_ACCEPT_TIMEOUT, temp);

			SetForegroundWindow(hwnd);

			// Attempt to mimic Win98/2000 dialog behaviour
			if ((rdService::IsWinNT() && (rdService::VersionMajor() <= 4)) ||
				(rdService::IsWin95() && (rdService::VersionMinor() == 0)))
			{
				// Perform special hack to display the dialog safely
				if (GetWindowThreadProcessId(GetForegroundWindow(), NULL) != GetCurrentProcessId())
				{
					// We can't set our dialog as foreground if the foreground window
					// doesn't belong to us - it's unsafe!
					SetActiveWindow(hwnd);
					_this->m_foreground_hack = TRUE;
					_this->m_flash_state = FALSE;
				}
			}
			if (!_this->m_foreground_hack) {
				SetForegroundWindow(hwnd);
			}

			// Beep
			MessageBeep(MB_ICONEXCLAMATION);
            
			SetForegroundWindow(hwnd);

            // Return false to prevent accept button from gaining
			// focus.
			return FALSE;
		}

		// Timer event
	case WM_TIMER:
		if ((_this->m_timeoutCount) == 0) {
			if ( _this->m_acceptOnTimeout ) {
				EndDialog(hwnd, IDACCEPT);
			} else {
				EndDialog(hwnd, IDREJECT);
			}
		}
		_this->m_timeoutCount--;

		// Flash if necessary
		if (_this->m_foreground_hack) {
			if (GetWindowThreadProcessId(GetForegroundWindow(), NULL) != GetCurrentProcessId())
			{
				_this->m_flash_state = !_this->m_flash_state;
				FlashWindow(hwnd, _this->m_flash_state);
			} else {
				_this->m_foreground_hack = FALSE;
			}
		}

		// Update the displayed count
		char temp[256];
		if ( _this->m_acceptOnTimeout )
			sprintf(temp, "AutoAccept: %u", (_this->m_timeoutCount));
		else
			sprintf(temp, "AutoReject: %u", (_this->m_timeoutCount));
		SetDlgItemText(hwnd, IDC_ACCEPT_TIMEOUT, temp);
		break;

		// Dialog has just received a command
	case WM_COMMAND:
		switch (LOWORD(wParam)) {

			// User clicked Accept or pressed return
		case IDACCEPT:
		case IDOK:
			EndDialog(hwnd, IDACCEPT);
			return TRUE;

		case IDACCEPT_NOPASS:
			EndDialog(hwnd, IDACCEPT_NOPASS);
			return TRUE;

		case IDREJECT:
		case IDCANCEL:
			EndDialog(hwnd, IDREJECT);
			return TRUE;
		};

		break;

		// Window is being destroyed!  (Should never happen)
	case WM_DESTROY:
		EndDialog(hwnd, IDREJECT);
		return TRUE;
	}
	return 0;
}

