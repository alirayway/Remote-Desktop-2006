

#include "stdhdrs.h"
#include "rdviewer.h"

// Process the About dialog.
static LRESULT CALLBACK AboutDlgProc(HWND hwnd, UINT iMsg, 
										   WPARAM wParam, LPARAM lParam) {
	switch (iMsg) {
	case WM_INITDIALOG:
		{
			CentreWindow(hwnd);
			return TRUE;
		}
	case WM_CLOSE:
		EndDialog(hwnd, TRUE);
		return TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK) {
			EndDialog(hwnd, TRUE);
		}
	}
	return FALSE;
}

void ShowAboutBox()
{
	int res = DialogBox(pApp->m_instance, 
 		DIALOG_MAKEINTRESOURCE(IDD_APP_ABOUT),
		NULL, (DLGPROC) AboutDlgProc);
}
	
