
#include "stdhdrs.h"
#include "rdviewer.h"
#include "rdviewerApp.h"
#include "Exception.h"

// For WinCE Palm, you might want to use this for debugging, since it
// seems impossible to give the command some arguments.
// #define PALM_LOG 1

rdviewerApp *pApp;

rdviewerApp::rdviewerApp(HINSTANCE hInstance, LPTSTR szCmdLine) {
	pApp = this;
	m_instance = hInstance;

	// Read the command line
	m_options.SetFromCommandLine(szCmdLine);
	
	// Logging info
	rdlog.SetLevel(m_options.m_logLevel);
	if (m_options.m_logToConsole) {
		rdlog.SetMode(Log::ToConsole | Log::ToDebug);
	}
	if (m_options.m_logToFile) {
		rdlog.SetFile(m_options.m_logFilename);
	}

#ifdef PALM_LOG
	// Hack override for WinCE Palm developers who can't give
	// options to commands, not even via shortcuts.
	rdlog.SetLevel(20);
 	rdlog.SetFile(_T("\\Temp\\log"));
#endif
 	
	// Clear connection list
	for (int i = 0; i < MAX_CONNECTIONS; i++)
		m_clilist[i] = NULL;

	// Initialise winsock
	WORD wVersionRequested = MAKEWORD(2, 0);
	WSADATA wsaData;
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		MessageBox(NULL, _T("Error initialising sockets library"), _T("rd info"), MB_OK | MB_ICONSTOP);
		PostQuitMessage(1);
	}
	rdlog.Print(3, _T("Started and Winsock (v %d) initialised\n"), wsaData.wVersion);
}


// The list of clients should fill up from the start and have NULLs
// afterwards.  If the first entry is a NULL, the list is empty.

void rdviewerApp::RegisterConnection(ClientConnection *pConn) {
	omni_mutex_lock l(m_clilistMutex);
	int i;
	for (i = 0; i < MAX_CONNECTIONS; i++) {
		if (m_clilist[i] == NULL) {
			m_clilist[i] = pConn;
			rdlog.Print(4,_T("Registered connection with app\n"));
			return;
		}
	}
	// If we've got here, something is wrong.
	rdlog.Print(-1, _T("Client list overflow!\n"));
	MessageBox(NULL, _T("Client list overflow!"), _T("rd error"),
		MB_OK | MB_ICONSTOP);
	PostQuitMessage(1);

}

void rdviewerApp::DeregisterConnection(ClientConnection *pConn) {
	omni_mutex_lock l(m_clilistMutex);
	int i;
	for (i = 0; i < MAX_CONNECTIONS; i++) {
		if (m_clilist[i] == pConn) {
			// shuffle everything above downwards
			for (int j = i; m_clilist[j] &&	j < MAX_CONNECTIONS-1 ; j++)
				m_clilist[j] = m_clilist[j+1];
			m_clilist[MAX_CONNECTIONS-1] = NULL;
			rdlog.Print(4,_T("Deregistered connection from app\n"));

			// No clients left? then we should finish, unless we're in
			// listening mode.
			if ((m_clilist[0] == NULL) && (!pApp->m_options.m_listening))
				PostQuitMessage(0);

			return;
		}
	}
	// If we've got here, something is wrong.
	rdlog.Print(-1, _T("Client not found for deregistering!\n"));
	PostQuitMessage(1);
}

// ----------------------------------------------


rdviewerApp::~rdviewerApp() {
		
	
	// Clean up winsock
	WSACleanup();
	
	rdlog.Print(2, _T("rd viewer closing down\n"));

}
