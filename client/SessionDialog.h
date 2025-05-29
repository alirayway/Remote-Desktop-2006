

#ifndef SESSIONDIALOG_H__
#define SESSIONDIALOG_H__

#pragma once

#include "rdOptions.h"
#include "MRU.h"

class SessionDialog  
{
public:

	// Create a connection dialog, with the options to be
	// displayed if the options.. button is clicked.
	SessionDialog(rdOptions *pOpt);
	int DoDialog();
	int m_port;
	TCHAR m_host[256];
   	virtual ~SessionDialog();

private:
	rdOptions *m_pOpt;
	MRU *m_pMRU;
	static BOOL CALLBACK SessDlgProc(  HWND hwndDlg,  UINT uMsg, 
		WPARAM wParam, LPARAM lParam );
};

#endif // SESSIONDIALOG_H__

