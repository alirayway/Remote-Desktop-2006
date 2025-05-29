

// rdAbout.cpp

// Implementation of the About dialog!

#include "stdhdrs.h"

#include "Winrd.h"
#include "rdAbout.h"

// Constructor/destructor
rdAbout::rdAbout()
{
	m_dlgvisible = FALSE;
}

rdAbout::~rdAbout()
{
}

// Initialisation
BOOL
rdAbout::Init()
{
	return TRUE;
}

// Dialog box handling functions
void
rdAbout::Show(BOOL show)
{
	if (show)
	{
		if (!m_dlgvisible)
		{
			DialogBoxParam(hAppInstance,
				MAKEINTRESOURCE(IDD_ABOUT), 
				NULL,
				(DLGPROC) DialogProc,
				(LONG) this);
		}
	}
}

BOOL CALLBACK
rdAbout::DialogProc(HWND hwnd,
					 UINT uMsg,
					 WPARAM wParam,
					 LPARAM lParam )
{
	// We use the dialog-box's USERDATA to store a _this pointer
	// This is set only once WM_INITDIALOG has been recieved, though!
	rdAbout *_this = (rdAbout *) GetWindowLong(hwnd, GWL_USERDATA);

	switch (uMsg)
	{

	case WM_INITDIALOG:
		{
			// Retrieve the Dialog box parameter and use it as a pointer
			// to the calling rdProperties object
			SetWindowLong(hwnd, GWL_USERDATA, lParam);
			_this = (rdAbout *) lParam;

			// Show the dialog
			SetForegroundWindow(hwnd);

			_this->m_dlgvisible = TRUE;

			return TRUE;
		}

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{

		case IDCANCEL:
		case IDOK:
			// Close the dialog
			EndDialog(hwnd, TRUE);

			_this->m_dlgvisible = FALSE;

			return TRUE;
		}

		break;

	case WM_DESTROY:
		EndDialog(hwnd, FALSE);
		_this->m_dlgvisible = FALSE;
		return TRUE;
	}
	return 0;
}
