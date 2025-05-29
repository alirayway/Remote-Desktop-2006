
#include "rdviewerApp32.h"
#include "rdviewer.h"
#include "Exception.h"

// --------------------------------------------------------------------------
rdviewerApp32::rdviewerApp32(HINSTANCE hInstance, PSTR szCmdLine) :
	rdviewerApp(hInstance, szCmdLine)
{

	m_pdaemon = NULL;
	m_pflasher = NULL;

	// Load a requested keyboard layout
	if (m_options.m_kbdSpecified) {
		HKL hkl = LoadKeyboardLayout(  m_options.m_kbdname, 
			KLF_ACTIVATE | KLF_REPLACELANG | KLF_REORDER  );
		if (hkl == NULL) {
			MessageBox(NULL, _T("Error loading specified keyboard layout"), 
				_T("rd info"), MB_OK | MB_ICONSTOP);
			exit(1);
		}
	}

	// Start listening daemons if requested
	
	if (m_options.m_listening) {
		rdlog.Print(3, _T("In listening mode - staring daemons\n"));
		
		try {
			m_pflasher = new Flasher(FLASH_PORT_OFFSET);
			m_pdaemon = new Daemon(m_options.m_listenPort);
		} catch (WarningException &e) {
			char msg[1024];
			sprintf(msg, "Error creating listening daemon:\n\r(%s)\n\r%s",
				e.m_info, "Perhaps another rdviewer is already running?");
			MessageBox(NULL, msg, "rdviewer error", MB_OK | MB_ICONSTOP);
			exit(1);
		}
		
	} 

	RegisterSounds();
	
}

	
// These should maintain a list of connections.
// FIXME: Eliminate duplicated code, see the following three functions.

void rdviewerApp32::NewConnection() {
	int retries = 0;
	ClientConnection *pcc;
	ClientConnection *old_pcc;

	pcc = new ClientConnection(this);
	while (retries < MAX_AUTH_RETRIES) {
		try {
			pcc->Run();
			return;
		} catch (AuthException &e) {
			e.Report();
			// If the connection count drops to zero, the app exits.
			old_pcc = pcc;
			pcc = new ClientConnection(this);
			// Get the previous options for the next try.
			pcc->CopyOptions(old_pcc);
			delete old_pcc;
		} catch (Exception &e) {
			e.Report();
			break;
		}
		retries++;
	}
	delete pcc;
}

void rdviewerApp32::NewConnection(TCHAR *host, int port) {
	int retries = 0;
	ClientConnection *pcc;
	ClientConnection *old_pcc;

	pcc = new ClientConnection(this, host, port);
	while (retries < MAX_AUTH_RETRIES) {
		try {
			pcc->Run();
			return;
		} catch (AuthException &e) {
			e.Report();
			// If the connection count drops to zero, the app exits.
			old_pcc = pcc;
			pcc = new ClientConnection(this, host, port);
			// Get the previous options for the next try.
			pcc->CopyOptions(old_pcc);
			delete old_pcc;
		} catch (Exception &e) {
			e.Report();	
			break;
		}
		retries++;
	}
	delete pcc;
}

void rdviewerApp32::NewConnection(SOCKET sock) {
	int retries = 0;
	ClientConnection *pcc;
	ClientConnection *old_pcc;

	pcc = new ClientConnection(this, sock);
	while (retries < MAX_AUTH_RETRIES) {
		try {
			pcc->Run();
			return;
		} catch (AuthException &e) {
			e.Report();
			// If the connection count drops to zero, the app exits.
			old_pcc = pcc;
			pcc = new ClientConnection(this, sock);
			// Get the previous options for the next try.
			pcc->CopyOptions(old_pcc);
			delete old_pcc;
		} catch (Exception &e) {
			e.Report();
			break;
		}
		retries++;
	}
	delete pcc;
}

// Register the Bell sound event

const char* BELL_APPL_KEY_NAME  = "AppEvents\\Schemes\\Apps\\rdviewer";
const char* BELL_LABEL = "rdviewerBell";

void rdviewerApp32::RegisterSounds() {
	
	HKEY hBellKey;
	char keybuf[256];
	
	sprintf(keybuf, "AppEvents\\EventLabels\\%s", BELL_LABEL);
	// First create a label for it
	if ( RegCreateKey(HKEY_CURRENT_USER, keybuf, &hBellKey)  == ERROR_SUCCESS ) {
		RegSetValue(hBellKey, NULL, REG_SZ, "Bell", 0);
		RegCloseKey(hBellKey);
		
		// Then put the detail in the app-specific area
		
		if ( RegCreateKey(HKEY_CURRENT_USER, BELL_APPL_KEY_NAME, &hBellKey)  == ERROR_SUCCESS ) {
			
			sprintf(keybuf, "%s\\%s", BELL_APPL_KEY_NAME, BELL_LABEL);
			RegCreateKey(HKEY_CURRENT_USER, keybuf, &hBellKey);
			RegSetValue(hBellKey, NULL, REG_SZ, "Bell", 0);
			RegCloseKey(hBellKey);
			
			sprintf(keybuf, "%s\\%s\\.current", BELL_APPL_KEY_NAME, BELL_LABEL);
			if (RegOpenKey(HKEY_CURRENT_USER, keybuf, &hBellKey) != ERROR_SUCCESS) {
				RegCreateKey(HKEY_CURRENT_USER, keybuf, &hBellKey);
				RegSetValue(hBellKey, NULL, REG_SZ, "ding.wav", 0);
			}
			RegCloseKey(hBellKey);
			
			sprintf(keybuf, "%s\\%s\\.default", BELL_APPL_KEY_NAME, BELL_LABEL);
			if (RegOpenKey(HKEY_CURRENT_USER, keybuf, &hBellKey) != ERROR_SUCCESS) {
				RegCreateKey(HKEY_CURRENT_USER, keybuf, &hBellKey);
				RegSetValue(hBellKey, NULL, REG_SZ, "ding.wav", 0);
			}
			RegCloseKey(hBellKey);
		}
		
	} 
	
}


rdviewerApp32::~rdviewerApp32() {
	// We don't need to clean up pcc if the thread has been joined.
	if (m_pdaemon != NULL) delete m_pdaemon;
	if (m_pflasher != NULL) delete m_pflasher;
}
	
