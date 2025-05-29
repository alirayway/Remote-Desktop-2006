// rdServer.cpp

// rdServer class implementation

// Includes
#include "stdhdrs.h"
#include <omnithread.h>
#include <string.h>
#include <lmcons.h>

// Custom
#include "Winrd.h"
#include "rdServer.h"
#include "rdSockConnect.h"
#include "rdCorbaConnect.h"
#include "rdClient.h"
#include "rdService.h"

// Constructor/destructor
rdServer::rdServer()
{
	// Initialise some important stuffs...
	m_socketConn = NULL;
	m_corbaConn = NULL;
	m_httpConn = NULL;
	m_desktop = NULL;
	m_name = NULL;
	m_port = DISPLAY_TO_PORT(0);
	m_port_http = DISPLAY_TO_HPORT(0);
	m_autoportselect = TRUE;
	m_passwd_required = TRUE;
	m_beepConnect = FALSE;
	m_beepDisconnect = FALSE;
	m_auth_hosts = 0;
	m_blacklist = 0;
	{
		rdPasswd::FromClear clearPWD;
		memcpy(m_password, clearPWD, MAXPWLEN);
		memcpy(m_password_viewonly, clearPWD, MAXPWLEN);
	}
	m_querysetting = 2;
	m_querytimeout = 10;
	m_queryaccept = FALSE;
	m_queryallownopass = FALSE;

	// Autolock settings
	m_lock_on_exit = 0;

	// Set the polling mode options
	m_poll_fullscreen = FALSE;
	m_poll_foreground = FALSE;
	m_poll_undercursor = TRUE;

	m_poll_oneventonly = FALSE;
	m_poll_consoleonly = TRUE;

	// General options
	m_loopbackOnly = FALSE;
	m_disableTrayIcon = FALSE;
	m_loopback_allowed = FALSE;
	m_httpd_enabled = TRUE;
	m_httpd_params_enabled = FALSE;
	m_lock_on_exit = 0;
	m_connect_pri = 0;

	// Set the input options
	m_enable_remote_inputs = TRUE;
	m_disable_local_inputs = FALSE;

	// Clear the client mapping table
	for (int x=0; x<MAX_CLIENTS; x++)
		m_clientmap[x] = NULL;
	m_nextid = 0;

	// Signal set when a client quits
	m_clientquitsig = new omni_condition(&m_clientsLock);
	m_clients_disabled = FALSE;
}

rdServer::~rdServer()
{
	rdlog.Print(LL_STATE, rdLOG("shutting down server object\n"));

	// If there is a socket_conn object then delete it
	if (m_socketConn != NULL)
	{
		delete m_socketConn;
		m_socketConn = NULL;
	}

	if (m_corbaConn != NULL)
	{
		delete m_corbaConn;
		m_corbaConn = NULL;
	}

	if (m_httpConn != NULL)
	{
		delete m_httpConn;
		m_httpConn = NULL;
	}

	// Remove any active clients!
	KillAuthClients();
	KillUnauthClients();

	// Wait for all the clients to die
	WaitUntilAuthEmpty();
	WaitUntilUnauthEmpty();

	// Don't free the desktop until no KillClient is likely to free it
	{	omni_mutex_lock l(m_desktopLock);

		if (m_desktop != NULL)
		{
			delete m_desktop;
			m_desktop = NULL;
		}
	}

	// Don't free the authhosts string until no more connections are possible
	if (m_auth_hosts != 0)
	{
		free(m_auth_hosts);
		m_auth_hosts = 0;
	}

	if (m_name != NULL)
	{
		free(m_name);
		m_name = NULL;
	}

	if (m_clientquitsig != NULL)
	{
		delete m_clientquitsig;
		m_clientquitsig = NULL;
	}

	rdlog.Print(LL_STATE, rdLOG("shutting down server object(4)\n"));
}

// Client handling functions
void
rdServer::DisableClients(BOOL state)
{
	m_clients_disabled = state;
}

BOOL
rdServer::ClientsDisabled()
{
	return m_clients_disabled;
}

rdClientId
rdServer::AddClient(VSocket *socket, BOOL auth, BOOL shared)
{
	return AddClient(socket, auth, shared, FALSE, 0, TRUE, TRUE); 
}

rdClientId
rdServer::AddClient(VSocket *socket, BOOL auth, BOOL shared,
					 BOOL teleport, int capability,
					 BOOL keysenabled, BOOL ptrenabled)
{
	rdClient *client;

	omni_mutex_lock l(m_clientsLock);

	// Try to allocate a client id...
	rdClientId clientid = m_nextid;
	do
	{
		clientid = (clientid+1) % MAX_CLIENTS;
		if (clientid == m_nextid)
		{
			rdlog.Print(LL_CONNERR, rdLOG("too many clients already connected\n"));
			delete socket;
			return -1;
		}
	}
	while (m_clientmap[clientid] != NULL);

	// Create a new client and add it to the relevant client list
	client = new rdClient();
	if (client == NULL) {
		rdlog.Print(LL_CONNERR, rdLOG("failed to allocate client object\n"));
		delete socket;
		return -1;
	}

	// Set the client's settings
	client->SetTeleport(teleport);
	client->SetCapability(capability);
	client->EnableKeyboard(keysenabled && m_enable_remote_inputs);
	client->EnablePointer(ptrenabled && m_enable_remote_inputs);

	// Start the client
	if (!client->Init(this, socket, auth, shared, clientid))
	{
		// The client will delete the socket for us...
		rdlog.Print(LL_CONNERR, rdLOG("failed to initialize client object\n"));
		delete client;
		return -1;
	}

	m_clientmap[clientid] = client;

	// Add the client to unauth the client list
	m_unauthClients.push_back(clientid);

	// Notify anyone interested about this event
	DoNotify(WM_SRV_CLIENT_CONNECT, 0, 0);

	rdlog.Print(LL_INTINFO, rdLOG("AddClient() done\n"));

	return clientid;
}

BOOL
rdServer::Authenticated(rdClientId clientid)
{
	rdClientList::iterator i;
	BOOL authok = TRUE;

	omni_mutex_lock l1(m_desktopLock);
	omni_mutex_lock l2(m_clientsLock);

	// Search the unauthenticated client list
	for (i = m_unauthClients.begin(); i != m_unauthClients.end(); i++)
	{
		// Is this the right client?
		if ((*i) == clientid)
		{
			rdClient *client = GetClient(clientid);

			// Yes, so remove the client and add it to the auth list
			m_unauthClients.erase(i);

			// Create the screen handler if necessary
			if (m_desktop == NULL)
			{
				m_desktop = new rdDesktop();
				if (m_desktop == NULL)
				{
					rdlog.Print(LL_CONNERR, rdLOG("failed to allocate desktop object\n"));
					client->Kill();
					authok = FALSE;
					break;
				}
				if (!m_desktop->Init(this))
				{
					rdlog.Print(LL_CONNERR, rdLOG("failed to initialize desktop object\n"));

					client->Kill();
					authok = FALSE;

					delete m_desktop;
					m_desktop = NULL;

					break;
				}
			}

			// Create a buffer object for this client
			rdBuffer *buffer = new rdBuffer(m_desktop);
			if (buffer == NULL)
			{
				rdlog.Print(LL_CONNERR, rdLOG("failed to allocate buffer object\n"));
				client->Kill();
				authok = FALSE;
				break;
			}

			// Tell the client about this new buffer
			client->SetBuffer(buffer);

			// Add the client to the auth list
			m_authClients.push_back(clientid);

			break;
		}
	}

	// Notify anyone interested of this event
	DoNotify(WM_SRV_CLIENT_AUTHENTICATED, 0, 0);

	rdlog.Print(LL_INTINFO, rdLOG("Authenticated() done\n"));

	// If so configured, beep to indicate the new connection is
	// present.
	if (authok && GetBeepConnect())
	{
		MessageBeep(MB_OK);
	}

	return authok;
}

void
rdServer::KillClient(rdClientId clientid)
{
	rdClientList::iterator i;
	BOOL done = FALSE;

	omni_mutex_lock l(m_clientsLock);

	// Find the client in one of the two lists
	for (i = m_unauthClients.begin(); i != m_unauthClients.end(); i++)
	{
		// Is this the right client?
		if ((*i) == clientid)
		{
			rdlog.Print(LL_INTINFO, rdLOG("killing unauth client\n"));

			// Ask the client to die
			rdClient *client = GetClient(clientid);
			client->Kill();

			done = TRUE;
			break;
		}
	}
	if (!done)
	{
		for (i = m_authClients.begin(); i != m_authClients.end(); i++)
		{
			// Is this the right client?
			if ((*i) == clientid)
			{
				rdlog.Print(LL_INTINFO, rdLOG("killing auth client\n"));

				// Yes, so kill it
				rdClient *client = GetClient(clientid);
				client->Kill();

				done = TRUE;
				break;
			}
		}
	}

	rdlog.Print(LL_INTINFO, rdLOG("KillClient() done\n"));
}

void
rdServer::KillAuthClients()
{
	rdClientList::iterator i;
	omni_mutex_lock l(m_clientsLock);

	// Tell all the authorised clients to die!
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{
		rdlog.Print(LL_INTINFO, rdLOG("killing auth client\n"));

		// Kill the client
		GetClient(*i)->Kill();
	}

	rdlog.Print(LL_INTINFO, rdLOG("KillAuthClients() done\n"));
}

void
rdServer::KillUnauthClients()
{
	rdClientList::iterator i;
	omni_mutex_lock l(m_clientsLock);

	// Tell all the authorised clients to die!
	for (i = m_unauthClients.begin(); i != m_unauthClients.end(); i++)
	{
		rdlog.Print(LL_INTINFO, rdLOG("killing unauth client\n"));

		// Kill the client
		GetClient(*i)->Kill();
	}

	rdlog.Print(LL_INTINFO, rdLOG("KillUnauthClients() done\n"));
}

UINT
rdServer::AuthClientCount()
{
	omni_mutex_lock l(m_clientsLock);

	return m_authClients.size();
}

UINT
rdServer::UnauthClientCount()
{
	omni_mutex_lock l(m_clientsLock);

	return m_unauthClients.size();
}

void
rdServer::WaitUntilAuthEmpty()
{
	omni_mutex_lock l(m_clientsLock);

	// Wait for all the clients to exit
	while (!m_authClients.empty())
	{
		// Wait for a client to quit
		m_clientquitsig->wait();
	}
}

void
rdServer::WaitUntilUnauthEmpty()
{
	omni_mutex_lock l(m_clientsLock);

	// Wait for all the clients to exit
	while (!m_unauthClients.empty())
	{
		// Wait for a client to quit
		m_clientquitsig->wait();
	}
}

// Client info retrieval/setup
rdClient*
rdServer::GetClient(rdClientId clientid)
{
	if ((clientid >= 0) && (clientid < MAX_CLIENTS))
		return m_clientmap[clientid];
	return NULL;
}

rdClientList
rdServer::ClientList()
{
	rdClientList clients;

	omni_mutex_lock l(m_clientsLock);

	clients = m_authClients;

	return clients;
}

void
rdServer::SetTeleport(rdClientId clientid, BOOL teleport)
{
	omni_mutex_lock l(m_clientsLock);

	rdClient *client = GetClient(clientid);
	if (client != NULL)
		client->SetTeleport(teleport);
}

void
rdServer::SetCapability(rdClientId clientid, int capability)
{
	omni_mutex_lock l(m_clientsLock);

	rdClient *client = GetClient(clientid);
	if (client != NULL)
		client->SetCapability(capability);
}

void
rdServer::SetKeyboardEnabled(rdClientId clientid, BOOL enabled)
{
	omni_mutex_lock l(m_clientsLock);

	rdClient *client = GetClient(clientid);
	if (client != NULL)
		client->EnableKeyboard(enabled);
}

void
rdServer::SetPointerEnabled(rdClientId clientid, BOOL enabled)
{
	omni_mutex_lock l(m_clientsLock);

	rdClient *client = GetClient(clientid);
	if (client != NULL)
		client->EnablePointer(enabled);
}

BOOL
rdServer::IsTeleport(rdClientId clientid)
{
	omni_mutex_lock l(m_clientsLock);

	rdClient *client = GetClient(clientid);
	if (client != NULL)
		return client->IsTeleport();
	return FALSE;
}

int
rdServer::GetCapability(rdClientId clientid)
{
	omni_mutex_lock l(m_clientsLock);

	rdClient *client = GetClient(clientid);
	if (client != NULL)
		return client->GetCapability();
	return 0;
}

BOOL
rdServer::GetKeyboardEnabled(rdClientId clientid)
{
	omni_mutex_lock l(m_clientsLock);

	rdClient *client = GetClient(clientid);
	if (client != NULL)
		return client->IsKeyboardEnabled();
	return FALSE;
}

BOOL
rdServer::GetPointerEnabled(rdClientId clientid)
{
	omni_mutex_lock l(m_clientsLock);

	rdClient *client = GetClient(clientid);
	if (client != NULL)
		return client->IsPointerEnabled();
	return FALSE;
}

const char*
rdServer::GetClientName(rdClientId clientid)
{
	omni_mutex_lock l(m_clientsLock);

	rdClient *client = GetClient(clientid);
	if (client != NULL)
		return client->GetClientName();
	return NULL;
}

// RemoveClient should ONLY EVER be used by the client to remove itself.
void
rdServer::RemoveClient(rdClientId clientid)
{
	rdClientList::iterator i;
	BOOL done = FALSE;

	omni_mutex_lock l1(m_desktopLock);
	{	omni_mutex_lock l2(m_clientsLock);

		// Find the client in one of the two lists
		for (i = m_unauthClients.begin(); i != m_unauthClients.end(); i++)
		{
			// Is this the right client?
			if ((*i) == clientid)
			{
				rdlog.Print(LL_INTINFO, rdLOG("removing unauthorised client\n"));

				// Yes, so remove the client and kill it
				m_unauthClients.erase(i);
				m_clientmap[clientid] = NULL;

				done = TRUE;
				break;
			}
		}
		if (!done)
		{
			for (i = m_authClients.begin(); i != m_authClients.end(); i++)
			{
				// Is this the right client?
				if ((*i) == clientid)
				{
					rdlog.Print(LL_INTINFO, rdLOG("removing authorised client\n"));

					// Yes, so remove the client and kill it
					m_authClients.erase(i);
					m_clientmap[clientid] = NULL;

					done = TRUE;
					break;
				}
			}
		}

		// Signal that a client has quit
		m_clientquitsig->signal();

	} // Unlock the clientLock

	// If so configured, beep to indicate the old connection is
	// gone.
	if (GetBeepDisconnect())
	{
		MessageBeep(MB_OK);
	}

	// Are there any authorised clients connected?
	if (m_authClients.empty() && (m_desktop != NULL))
	{
		rdlog.Print(LL_STATE, rdLOG("deleting desktop server\n"));

		// Are there locksettings set?
		if (LockSettings() == 1)
		{
		    // Yes - lock the machine on disconnect!
			rdService::LockWorkstation();
		} else if (LockSettings() > 1)
		{
		    char username[UNLEN+1];

		    rdService::CurrentUser((char *)&username, sizeof(username));
		    if (strcmp(username, "") != 0)
		    {
			// Yes - force a user logoff on disconnect!
			if (!ExitWindowsEx(EWX_LOGOFF, 0))
			    rdlog.Print(LL_CONNERR, rdLOG("client disconnect - failed to logoff user!\n"));
		    }
		}

		// Delete the screen server
		delete m_desktop;
		m_desktop = NULL;
	}

	// Notify anyone interested of the change
	DoNotify(WM_SRV_CLIENT_DISCONNECT, 0, 0);

	rdlog.Print(LL_INTINFO, rdLOG("RemoveClient() done\n"));
}

// NOTIFICATION HANDLING!

// Connect/disconnect notification
BOOL
rdServer::AddNotify(HWND hwnd)
{
	omni_mutex_lock l(m_clientsLock);

	// Add the window handle to the list
	m_notifyList.push_front(hwnd);

	return TRUE;
}

BOOL
rdServer::RemNotify(HWND hwnd)
{
	omni_mutex_lock l(m_clientsLock);

	// Remove the window handle from the list
	rdNotifyList::iterator i;
	for (i=m_notifyList.begin(); i!=m_notifyList.end(); i++)
	{
		if ((*i) == hwnd)
		{
			// Found the handle, so remove it
			m_notifyList.erase(i);
			return TRUE;
		}
	}

	return FALSE;
}

// Send a notification message
void
rdServer::DoNotify(UINT message, WPARAM wparam, LPARAM lparam)
{
	omni_mutex_lock l(m_clientsLock);

	// Send the given message to all the notification windows
	rdNotifyList::iterator i;
	for (i=m_notifyList.begin(); i!=m_notifyList.end(); i++)
	{
		PostMessage((*i), message, wparam, lparam);
	}
}

// Client->Desktop update signalling
void
rdServer::RequestUpdate()
{
	omni_mutex_lock l(m_desktopLock);
	if (m_desktop != NULL)
	{
		m_desktop->RequestUpdate();
	}
}

// Update handling
void
rdServer::TriggerUpdate()
{
	rdClientList::iterator i;
	
	omni_mutex_lock l(m_clientsLock);

	// Post this update to all the connected clients
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{
		// Post the update
		GetClient(*i)->TriggerUpdate();
	}
}

void
rdServer::UpdateRect(RECT &rect)
{
	rdClientList::iterator i;
	
	omni_mutex_lock l(m_clientsLock);

	// Post this update to all the connected clients
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{
		// Post the update
		GetClient(*i)->UpdateRect(rect);
	}
}

void
rdServer::UpdateRegion(rdRegion &region)
{
	rdClientList::iterator i;
	
	omni_mutex_lock l(m_clientsLock);

	// Post this update to all the connected clients
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{
		// Post the update
		GetClient(*i)->UpdateRegion(region);
	}
}

void
rdServer::CopyRect(RECT &dest, POINT &source)
{
	rdClientList::iterator i;
	
	omni_mutex_lock l(m_clientsLock);

	// Post this update to all the connected clients
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{
		// Post the update
		GetClient(*i)->CopyRect(dest, source);
	}
}

void
rdServer::UpdateMouse()
{
	rdClientList::iterator i;
	
	omni_mutex_lock l(m_clientsLock);

	// Post this mouse update to all the connected clients
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{
		// Post the update
		GetClient(*i)->UpdateMouse();
	}
}

void
rdServer::UpdateClipText(LPSTR text)
{
	rdClientList::iterator i;
	
	omni_mutex_lock l(m_clientsLock);

	// Post this update to all the connected clients
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{
		// Post the update
		GetClient(*i)->UpdateClipText(text);
	}
}

void
rdServer::UpdatePalette()
{
	rdClientList::iterator i;
	
	omni_mutex_lock l(m_clientsLock);

	// Post this update to all the connected clients
	for (i = m_authClients.begin(); i != m_authClients.end(); i++)
	{
		// Post the update
		GetClient(*i)->UpdatePalette();
	}
}

void
rdServer::UpdateLocalClipText(LPSTR text)
{
	omni_mutex_lock l(m_desktopLock);

	if (m_desktop != NULL)
		m_desktop->SetClipText(text);
}

// Name and port number handling
void
rdServer::SetName(const char * name)
{
	// Set the name of the desktop
	if (m_name != NULL)
	{
		free(m_name);
		m_name = NULL;
	}
	
	m_name = strdup(name);
}

void
rdServer::SetPorts(const UINT port_rfb, const UINT port_http)
{
	if (m_port != port_rfb || m_port_http != port_http) {
		// Set port numbers to use
		m_port = port_rfb;
		m_port_http = port_http;

		// If there is already a listening socket then close and re-open it...
		BOOL socketon = SockConnected();
		SockConnect(FALSE);
		if (socketon)
			SockConnect(TRUE);
    }
}

void
rdServer::SetPassword(const char *passwd)
{
	memcpy(m_password, passwd, MAXPWLEN);
}

void
rdServer::GetPassword(char *passwd)
{
	memcpy(passwd, m_password, MAXPWLEN);
}

void
rdServer::SetPasswordViewOnly(const char *passwd)
{
	memcpy(m_password_viewonly, passwd, MAXPWLEN);
}

void
rdServer::GetPasswordViewOnly(char *passwd)
{
	memcpy(passwd, m_password_viewonly, MAXPWLEN);
}

// Remote input handling
void
rdServer::EnableRemoteInputs(BOOL enable)
{
	m_enable_remote_inputs = enable;
}

BOOL rdServer::RemoteInputsEnabled()
{
	return m_enable_remote_inputs;
}

// Local input handling
void
rdServer::DisableLocalInputs(BOOL disable)
{
	m_disable_local_inputs = disable;
}

BOOL rdServer::LocalInputsDisabled()
{
	return m_disable_local_inputs;
}

// Socket connection handling
BOOL
rdServer::SockConnect(BOOL On)
{
	// Are we being asked to switch socket connects on or off?
	if (On)
	{
		// Is there a listening socket?
		if (m_socketConn == NULL)
		{
			m_socketConn = new rdSockConnect();
			if (m_socketConn == NULL)
				return FALSE;

			// Are we to use automatic port selection?
			if (m_autoportselect)
			{
				BOOL ok = FALSE;

				// Yes, so cycle through the ports, looking for a free one!
				for (int i = 0; i < 99; i++)
				{
					m_port = DISPLAY_TO_PORT(i);
					m_port_http = DISPLAY_TO_HPORT(i);

					rdlog.Print(LL_CLIENTS, rdLOG("trying port number %d\n"), m_port);

					// Attempt to connect to the port
					VSocket tempsock;
					if (tempsock.Create())
					{
						if (!tempsock.Connect("localhost", m_port))
						{
							// Couldn't connect, so this port is probably usable!
							if (m_socketConn->Init(this, m_port))
							{
								ok = TRUE;
								break;
							}
						}
					}
				}

				if (!ok)
				{
					delete m_socketConn;
					m_socketConn = NULL;
					return FALSE;
				}
			} else
			{
				// No autoportselect
				if (!m_socketConn->Init(this, m_port))
				{
					delete m_socketConn;
					m_socketConn = NULL;
					return FALSE;
				}
			}

			// Now let's start the HTTP connection stuff
			if (m_httpConn == NULL && m_httpd_enabled) {
				m_httpConn = new rdHTTPConnect;
				if (m_httpConn != NULL) {
					// Start up the HTTP server
					if (!m_httpConn->Init(this, m_port_http,
										  m_httpd_params_enabled)) {
						delete m_httpConn;
						m_httpConn = NULL;
						return FALSE;
					}
				}
			}
		}
	}
	else
	{
		// *** JNW - Trying to fix up a lock-up when the listening socket closes
		KillAuthClients();
		KillUnauthClients();
		WaitUntilAuthEmpty();
		WaitUntilUnauthEmpty();

		// Is there a listening socket?
		if (m_socketConn != NULL)
		{
			// Close the socket
			delete m_socketConn;
			m_socketConn = NULL;
		}

		// Is there an HTTP socket active?
		if (m_httpConn != NULL)
		{
			// Close the socket
			delete m_httpConn;
			m_httpConn = NULL;
		}
	}

	return TRUE;
}

BOOL
rdServer::SockConnected()
{
	return m_socketConn != NULL;
}

BOOL
rdServer::SetHttpdEnabled(BOOL enable_httpd, BOOL enable_params)
{
	if (enable_httpd != m_httpd_enabled) {
		m_httpd_enabled = enable_httpd;
		m_httpd_params_enabled = enable_params;
		BOOL socketConn = SockConnected();
		SockConnect(FALSE);
		SockConnect(socketConn);
	} else {
		if (enable_params != m_httpd_params_enabled) {
			m_httpd_params_enabled = enable_params;
			if (SockConnected()) {
				SockConnect(FALSE);
				SockConnect(TRUE);
			}
		}
	}
	return TRUE;
}

BOOL
rdServer::SetLoopbackOnly(BOOL loopbackOnly)
{
	if (loopbackOnly != m_loopbackOnly)
	{
		m_loopbackOnly = loopbackOnly;
		BOOL socketConn = SockConnected();
		SockConnect(FALSE);
		SockConnect(socketConn);
	}
	return TRUE;
}

BOOL
rdServer::LoopbackOnly()
{
	return m_loopbackOnly;
}

BOOL
rdServer::SetDisableTrayIcon(BOOL disableTrayIcon)
{
	if (disableTrayIcon != m_disableTrayIcon)
	{
		m_disableTrayIcon = disableTrayIcon;
	}
	return TRUE;
}

BOOL
rdServer::GetDisableTrayIcon()
{
	return m_disableTrayIcon;
}

// CORBA connection handling
BOOL
rdServer::CORBAConnect(BOOL On)
{
	// Are we being asked to switch CORBA connects on or off?
	if (On)
	{
		// Is there a CORBA object?
		if (m_corbaConn == NULL)
		{
			m_corbaConn = new rdCorbaConnect();
		}
		if (m_corbaConn == NULL)
			return FALSE;
		if (!m_corbaConn->Init(this))
		{
			delete m_corbaConn;
			m_corbaConn = NULL;
			return FALSE;
		}
	}
	else
	{
		// Is there a listening socket?
		if (m_corbaConn != NULL)
		{
			// Close the socket
			delete m_corbaConn;
			m_corbaConn = NULL;
		}
	}

	return TRUE;
}

BOOL
rdServer::CORBAConnected()
{
	return m_corbaConn != NULL;
}

void
rdServer::GetScreenInfo(int &width, int &height, int &depth)
{
	rfbServerInitMsg scrinfo;

	omni_mutex_lock l(m_desktopLock);

	rdlog.Print(LL_INTINFO, rdLOG("GetScreenInfo called\n"));

	// Is a desktop object currently active?
	if (m_desktop == NULL)
	{
		rdDesktop desktop;

		// No, so create a dummy desktop and interrogate it
		if (!desktop.Init(this))
		{
			scrinfo.framebufferWidth = 0;
			scrinfo.framebufferHeight = 0;
			scrinfo.format.bitsPerPixel = 0;
		}
		else
		{
			desktop.FillDisplayInfo(&scrinfo);
		}
	}
	else
	{
		m_desktop->FillDisplayInfo(&scrinfo);
	}

	// Get the info from the scrinfo structure
	width = scrinfo.framebufferWidth;
	height = scrinfo.framebufferHeight;
	depth = scrinfo.format.bitsPerPixel;
}

void
rdServer::SetAuthHosts(const char*hostlist) {
	omni_mutex_lock l(m_clientsLock);

	if (hostlist == 0) {
		rdlog.Print(LL_INTINFO, rdLOG("authhosts cleared\n"));
		m_auth_hosts = 0;
		return;
	}

	rdlog.Print(LL_INTINFO, rdLOG("authhosts set to \"%s\"\n"), hostlist);
	if (m_auth_hosts != 0)
		free(m_auth_hosts);

	m_auth_hosts = strdup(hostlist);
}

char*
rdServer::AuthHosts() {
	omni_mutex_lock l(m_clientsLock);

	if (m_auth_hosts == 0)
		return strdup("");
	else
		return strdup(m_auth_hosts);
}

inline BOOL
MatchStringToTemplate(const char *addr, size_t addrlen,
				      const char *filtstr, size_t filtlen) {
	if (filtlen == 0)
		return 1;
	if (addrlen < filtlen)
		return 0;
	for (size_t x = 0; x < filtlen; x++) {
		if (addr[x] != filtstr[x])
			return 0;
	}
	if ((addrlen > filtlen) && (addr[filtlen] != '.'))
		return 0;
	return 1;
}

rdServer::AcceptQueryReject
rdServer::VerifyHost(const char *hostname) {
	if (ClientsDisabled())
		return rdServer::aqrReject;

	omni_mutex_lock l(m_clientsLock);

	// -=- Is the specified host blacklisted?
	rdServer::BlacklistEntry	*current = m_blacklist;
	rdServer::BlacklistEntry	*previous = 0;
	SYSTEMTIME					systime;
	FILETIME					ftime;
	LARGE_INTEGER				now;

	// Get the current time as a 64-bit value
	GetSystemTime(&systime);
	SystemTimeToFileTime(&systime, &ftime);
	now.LowPart=ftime.dwLowDateTime;now.HighPart=ftime.dwHighDateTime;
	now.QuadPart /= 10000000; // Convert it into seconds

	while (current) {

		// Has the blacklist entry timed out?
		if ((now.QuadPart - current->_lastRefTime.QuadPart) > 0) {

			// Yes.  Is it a "blocked" entry?
			if (current->_blocked) {
				// Yes, so unblock it & re-set the reference time
				current->_blocked = FALSE;
				current->_lastRefTime.QuadPart = now.QuadPart + 10;
			} else {
				// No, so remove it
				if (previous)
					previous->_next = current->_next;
				else
					m_blacklist = current->_next;
				rdServer::BlacklistEntry *next = current->_next;
				free(current->_machineName);
				delete current;
				current = next;
				continue;
			}

		}

		// Is this the entry we're interested in?
		if ((strcmp(current->_machineName, hostname) == 0) &&
			(current->_blocked)) {
			// Machine is blocked, so just reject it
			return rdServer::aqrReject;
		}

		previous = current;
		current = current->_next;
	}

	// Has a hostname been specified?
	if (hostname == 0) {
		rdlog.Print(LL_INTWARN, rdLOG("verify failed - null hostname\n"));
		return rdServer::aqrReject;
	}

	// Set the state machine into the correct mode & process the filter
	enum vh_Mode {vh_ExpectDelimiter, vh_ExpectIncludeExclude, vh_ExpectPattern};
	vh_Mode machineMode = vh_ExpectIncludeExclude;
	
	rdServer::AcceptQueryReject verifiedHost = rdServer::aqrAccept;

	rdServer::AcceptQueryReject patternType = rdServer::aqrReject;
	UINT authHostsPos = 0;
	UINT patternStart = 0;
	UINT hostNameLen = strlen(hostname);

	// Run through the auth hosts string until we hit the end
	if (m_auth_hosts) {
		while (1) {

			// Which mode are we in?
			switch (machineMode) {

				// ExpectIncludeExclude - we should see a + or -.
			case vh_ExpectIncludeExclude:
				if (m_auth_hosts[authHostsPos] == '+') {
					patternType = rdServer::aqrAccept;
					patternStart = authHostsPos+1;
					machineMode = vh_ExpectPattern;
				} else if (m_auth_hosts[authHostsPos] == '-') {	
					patternType = rdServer::aqrReject;
					patternStart = authHostsPos+1;
					machineMode = vh_ExpectPattern;
				} else if (m_auth_hosts[authHostsPos] == '?') {	
					patternType = rdServer::aqrQuery;
					patternStart = authHostsPos+1;
					machineMode = vh_ExpectPattern;
				} else if (m_auth_hosts[authHostsPos] != '\0') {
					rdlog.Print(LL_INTWARN, rdLOG("verify host - malformed AuthHosts string\n"));
					machineMode = vh_ExpectDelimiter;
				}
				break;

				// ExpectPattern - we expect to see a valid pattern
			case vh_ExpectPattern:
				// ExpectDelimiter - we're scanning for the next ':', skipping a pattern
			case vh_ExpectDelimiter:
				if ((m_auth_hosts[authHostsPos] == ':') ||
					(m_auth_hosts[authHostsPos] == '\0')) {
					if (machineMode == vh_ExpectPattern) {
						if (patternStart == 0) {
							rdlog.Print(LL_INTWARN, rdLOG("verify host - pattern processing failed!\n"));
						} else {
							// Process the match
							if (MatchStringToTemplate(hostname, hostNameLen,
								&(m_auth_hosts[patternStart]), authHostsPos-patternStart)) {
								// The hostname matched - apply the include/exclude rule
								verifiedHost = patternType;
							}
						}
					}

					// We now expect another + or -
					machineMode = vh_ExpectIncludeExclude;
				}
				break;
			}

			// Have we hit the end of the pattern string?
			if (m_auth_hosts[authHostsPos] == '\0')
				break;
			authHostsPos++;
		}
	}

	return AdjustVerification(verifiedHost);
}


rdServer::AcceptQueryReject
rdServer::AdjustVerification(rdServer::AcceptQueryReject host)
{
	rdServer::AcceptQueryReject verifiedHost = host;

	// Based on the server's QuerySetting, adjust the verification result
	switch (host) {
	case rdServer::aqrAccept:
		if (QuerySetting() >= 3)
			verifiedHost = rdServer::aqrQuery;
		break;
	case rdServer::aqrQuery:
		if (QuerySetting() <= 1)
			verifiedHost = rdServer::aqrAccept;
		else if (QuerySetting() == 4)
			verifiedHost = rdServer::aqrReject;
		break;
	case rdServer::aqrReject:
		if (QuerySetting() == 0)
			verifiedHost = rdServer::aqrQuery;
		break;
	};

	return verifiedHost;
}

void
rdServer::AddAuthHostsBlacklist(const char *machine) {
	omni_mutex_lock l(m_clientsLock);

	// -=- Is the specified host blacklisted?
	rdServer::BlacklistEntry	*current = m_blacklist;

	// Get the current time as a 64-bit value
	SYSTEMTIME					systime;
	FILETIME					ftime;
	LARGE_INTEGER				now;
	GetSystemTime(&systime);
	SystemTimeToFileTime(&systime, &ftime);
	now.LowPart=ftime.dwLowDateTime;now.HighPart=ftime.dwHighDateTime;
	now.QuadPart /= 10000000; // Convert it into seconds

	while (current) {

		// Is this the entry we're interested in?
		if (strcmp(current->_machineName, machine) == 0) {

			// If the host is already blocked then ignore
			if (current->_blocked)
				return;

			// Set the RefTime & failureCount
			current->_lastRefTime.QuadPart = now.QuadPart + 10;
			current->_failureCount++;

			if (current->_failureCount > 5)
				current->_blocked = TRUE;
			return;
		}

		current = current->_next;
	}

	// Didn't find the entry
	current = new rdServer::BlacklistEntry;
	current->_blocked = FALSE;
	current->_failureCount = 0;
	current->_lastRefTime.QuadPart = now.QuadPart + 10;
	current->_machineName = strdup(machine);
	current->_next = m_blacklist;
	m_blacklist = current;
}

void
rdServer::RemAuthHostsBlacklist(const char *machine) {
	omni_mutex_lock l(m_clientsLock);

	// -=- Is the specified host blacklisted?
	rdServer::BlacklistEntry	*current = m_blacklist;
	rdServer::BlacklistEntry	*previous = 0;

	while (current) {

		// Is this the entry we're interested in?
		if (strcmp(current->_machineName, machine) == 0) {
			if (previous)
				previous->_next = current->_next;
			else
				m_blacklist = current->_next;
			rdServer::BlacklistEntry *next = current->_next;
			free (current->_machineName);
			delete current;
			current = next;
			continue;
		}

		previous = current;
		current = current->_next;
	}
}
