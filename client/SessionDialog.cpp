

// SessionDialog.cpp: implementation of the SessionDialog class.

#include "stdhdrs.h"
#include "rdviewer.h"
#include "SessionDialog.h"
#include "Exception.h"

#define SESSION_MRU_KEY_NAME _T("Software\\ORL\\rdviewer\\MRU")
#define NUM_MRU_ENTRIES 8

SessionDialog::SessionDialog(rdOptions *pOpt)
{
	m_pOpt = pOpt;
    m_pMRU = new MRU(SESSION_MRU_KEY_NAME);
}

SessionDialog::~SessionDialog()
{
    delete m_pMRU;
}

// It's exceedingly unlikely, but possible, that if two modal dialogs were
// closed at the same time, the static variables used for transfer between 
// window procedure and this method could overwrite each other.
int SessionDialog::DoDialog()
{
 	return DialogBoxParam(pApp->m_instance, DIALOG_MAKEINTRESOURCE(IDD_SESSION_DLG), 
		NULL, (DLGPROC) SessDlgProc, (LONG) this);
}

BOOL CALLBACK SessionDialog::SessDlgProc(  HWND hwnd,  UINT uMsg,  WPARAM wParam, LPARAM lParam ) {
	// This is a static method, so we don't know which instantiation we're 
	// dealing with. But we can get a pseudo-this from the parameter to 
	// WM_INITDIALOG, which we therafter store with the window and retrieve
	// as follows:
	SessionDialog *_this = (SessionDialog *) GetWindowLong(hwnd, GWL_USERDATA);

	switch (uMsg) {

	case WM_INITDIALOG:
		{
            SetWindowLong(hwnd, GWL_USERDATA, lParam);
            SessionDialog *_this = (SessionDialog *) lParam;
            CentreWindow(hwnd);
            // Set up recently-used list
            HWND hcombo = GetDlgItem(  hwnd, IDC_HOSTNAME_EDIT);
            TCHAR valname[256];

            for (int i = 0; i < _this->m_pMRU->NumItems(); i++) {
                _this->m_pMRU->GetItem(i, valname, 255);
                int pos = SendMessage(hcombo, CB_ADDSTRING, 0, (LPARAM) valname);

            }
            SendMessage(hcombo, CB_SETCURSEL, 0, 0);
            return TRUE;
		}

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
            TCHAR tmphost[256];
            TCHAR display[256];
            TCHAR fulldisplay[256];

			GetDlgItemText(hwnd, IDC_HOSTNAME_EDIT, display, 256);
            _tcscpy(fulldisplay, display);
            if (!ParseDisplay(display, tmphost, 255, &_this->m_port)) {
                MessageBox(NULL, 
                    _T("Invalid rd server specified.\n\r")
                    _T("Server should be of the form host:display."), 
                    _T("Connection setup"), MB_OK | MB_ICONEXCLAMATION );
            } else {
                _tcscpy(_this->m_host, tmphost);
                _this->m_pMRU->AddItem(fulldisplay);
                EndDialog(hwnd, TRUE);
            }
			return TRUE;
		case IDCANCEL:
			EndDialog(hwnd, FALSE);
			return TRUE;
		case IDC_OPTIONBUTTON:
			{
				_this->m_pOpt->DoDialog();
				return TRUE;
			}
		}
		break;

	case WM_DESTROY:
		EndDialog(hwnd, FALSE);
		return TRUE;
	}
	return 0;
}

