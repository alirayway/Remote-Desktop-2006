
#ifndef rdVIEWERAPP_H__
#define rdVIEWERAPP_H__

#pragma once

// The state of the application as a whole is contained in the app object
class rdviewerApp;

// I doubt we'll ever want more simultaneous connections than this
#define MAX_CONNECTIONS    (128)

#define MAX_AUTH_RETRIES   (3)

#include "ClientConnection.h"

class rdviewerApp {
public:
	rdviewerApp(HINSTANCE hInstance, LPTSTR szCmdLine);

	virtual void NewConnection() = 0;
	virtual void NewConnection(TCHAR *host, int port) = 0;
	virtual void NewConnection(SOCKET sock) = 0;
		
	~rdviewerApp();

	// This should be used by Connections to register and deregister 
	// themselves.  When the last connection is deregistered, the app
	// will close unless in listening mode.
	void RegisterConnection(ClientConnection *pConn);
	void DeregisterConnection(ClientConnection *pConn);
	
	rdOptions m_options;
	HINSTANCE  m_instance;

private:
	ClientConnection *m_clilist[MAX_CONNECTIONS];
	omni_mutex m_clilistMutex;
};

#endif // rdVIEWERAPP_H__

