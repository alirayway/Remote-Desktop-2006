
// AuthDialog.cpp: implementation of the AuthDialog class.

#include "stdhdrs.h"
#include "rdviewer.h"
#include "AuthDialog.h"
#include "Exception.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AuthDialog::AuthDialog()
{
	m_passwd[0]=__T('\0');
}

AuthDialog::~AuthDialog()
{
}

int AuthDialog::DoDialog()
{
	return DialogBoxParam(pApp->m_instance, DIALOG_MAKEINTRESOURCE(IDD_AUTH_DIALOG), 
		NULL, (DLGPROC) DlgProc, (LONG) this);
}

BOOL CALLBACK AuthDialog::DlgProc(  HWND hwnd,  UINT uMsg,  
									   WPARAM wParam, LPARAM lParam ) {
	// This is a static method, so we don't know which instantiation we're 
	// dealing with. But we can get a pseudo-this from the parameter to 
	// WM_INITDIALOG, which we therafter store with the window and retrieve
	// as follows:
	AuthDialog *_this = (AuthDialog *) GetWindowLong(hwnd, GWL_USERDATA);

	switch (uMsg) {

	case WM_INITDIALOG:
		{
			SetWindowLong(hwnd, GWL_USERDATA, lParam);
			_this = (AuthDialog *) lParam;
			CentreWindow(hwnd);
			return TRUE;
		}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			{
				UINT res= GetDlgItemText( hwnd,  IDC_PASSWD_EDIT,
					_this->m_passwd, 256);
				
				EndDialog(hwnd, TRUE);

				return TRUE;
			}
		case IDCANCEL:
			EndDialog(hwnd, FALSE);
			throw QuietException("User canceled connection.");
			return TRUE;
		}
		break;
	case WM_DESTROY:
		EndDialog(hwnd, FALSE);
		return TRUE;
	}
	return 0;
}

