

#ifndef rdVIEWER_H__
#define rdVIEWER_H__

#pragma once

#include "res\resource.h"
#include "rdviewerApp.h"
#include "Log.h"

#define WM_SOCKEVENT WM_USER+1
#define WM_TRAYNOTIFY WM_SOCKEVENT+1
#define WM_REGIONUPDATED WM_TRAYNOTIFY+1

// The Application
extern rdviewerApp *pApp;

// Global logger - may be used by anything
extern Log rdlog;

// Display given window in centre of screen
void CentreWindow(HWND hwnd);

// Convert "host:display" into host and port
// Returns true if valid.
bool ParseDisplay(LPTSTR display, LPTSTR phost, int hostlen, int *port);

// Macro DIALOG_MAKEINTRESOURCE is used to allow both normal windows dialogs
// and the selectable aspect ratio dialogs under WinCE (PalmPC vs HPC).
#ifndef UNDER_CE
#define DIALOG_MAKEINTRESOURCE MAKEINTRESOURCE
#else
// Under CE we pick dialog resource according to the 
// screen format selected or determined.
#define DIALOG_MAKEINTRESOURCE(res) SELECT_MAKEINTRESOURCE(res ## _PALM, res)
inline LPTSTR SELECT_MAKEINTRESOURCE(WORD res_palm, WORD res_hpc)
{
	if (pApp->m_options.m_palmpc)
		return MAKEINTRESOURCE(res_palm);
	else
		return MAKEINTRESOURCE(res_hpc);
}
#endif

#endif // rdVIEWER_H__

