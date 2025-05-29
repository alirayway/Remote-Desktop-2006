
#ifndef AUTHDIALOG_H__
#define AUTHDIALOG_H__

#pragma once

class AuthDialog  
{
public:
	AuthDialog();
	virtual ~AuthDialog();
	int DoDialog();
	TCHAR m_passwd[256];
	static BOOL CALLBACK DlgProc(  HWND hwndDlg,  UINT uMsg, 
		WPARAM wParam, LPARAM lParam );
};

#endif // AUTHDIALOG_H__
