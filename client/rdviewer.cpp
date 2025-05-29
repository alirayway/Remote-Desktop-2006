

#include "stdhdrs.h"
#include "rdviewer.h"
#include "Exception.h"
#ifdef UNDER_CE
#include "omnithreadce.h"
#else
#include "omnithread.h"
#include "rdviewerApp32.h"
#endif

// All logging is done via the log object
Log rdlog;

#ifdef UNDER_CE
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR szCmdLine, int iCmdShow)
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
#endif
{
	// The state of the application as a whole is contained in the one app object
	#ifdef _WIN32_WCE
		rdviewerApp app(hInstance, szCmdLine);
	#else
		rdviewerApp32 app(hInstance, szCmdLine);
	#endif

	// Start a new connection if specified on command line, 
	// or if not in listening mode
	
	if (app.m_options.m_connectionSpecified) {
		app.NewConnection(app.m_options.m_host, app.m_options.m_port);
	} else if (!app.m_options.m_listening) {
		// This one will also read from config file if specified
		app.NewConnection();
	}

	MSG msg;
	
	try {
		while ( GetMessage(&msg, NULL, 0,0) ) {
			DispatchMessage(&msg);
		} 
	} catch (WarningException &e) {
		e.Report();
	} catch (QuietException &e) {
		e.Report();
	}
	
	// Clean up winsock
	WSACleanup();

	rdlog.Print(3, _T("Exiting\n"));

	return msg.wParam;
}


// Move the given window to the centre of the screen
// and bring it to the top.
void CentreWindow(HWND hwnd)
{
	RECT winrect, workrect;
	
	// Find how large the desktop work area is
	SystemParametersInfo(SPI_GETWORKAREA, 0, &workrect, 0);
	int workwidth = workrect.right -  workrect.left;
	int workheight = workrect.bottom - workrect.top;
	
	// And how big the window is
	GetWindowRect(hwnd, &winrect);
	int winwidth = winrect.right - winrect.left;
	int winheight = winrect.bottom - winrect.top;
	// Make sure it's not bigger than the work area
	winwidth = min(winwidth, workwidth);
	winheight = min(winheight, workheight);

	// Now centre it
	SetWindowPos(hwnd, 
		HWND_TOP,
		workrect.left + (workwidth-winwidth) / 2,
		workrect.top + (workheight-winheight) / 2,
		winwidth, winheight, 
		SWP_SHOWWINDOW);
	SetForegroundWindow(hwnd);
}

// Convert "host:display" or "host::port" into host and port
// Returns true if valid format, false if not.
// Takes initial string, addresses of results and size of host buffer in wchars.
// If the display info passed in is longer than the size of the host buffer, it
// is assumed to be invalid, so false is returned.
bool ParseDisplay(LPTSTR display, LPTSTR phost, int hostlen, int *pport) 
{
    if (hostlen < (int)_tcslen(display))
        return false;

    int tmp_port;
    TCHAR *colonpos = _tcschr(display, L':');
    if (colonpos == NULL) {
		// No colon -- use default port number
        tmp_port = RFB_PORT_OFFSET;
		_tcsncpy(phost, display, MAX_HOST_NAME_LEN);
	} else {
		_tcsncpy(phost, display, colonpos - display);
		phost[colonpos - display] = L'\0';
		if (colonpos[1] == L':') {
			// Two colons -- interpret as a port number
			if (_stscanf(colonpos + 2, TEXT("%d"), &tmp_port) != 1) 
				return false;
		} else {
			// One colon -- interpret as a display number
			if (_stscanf(colonpos + 1, TEXT("%d"), &tmp_port) != 1) 
				return false;
			tmp_port += RFB_PORT_OFFSET;
		}
	}
    *pport = tmp_port;
    return true;
}


