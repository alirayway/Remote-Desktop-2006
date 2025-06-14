
// rdServer.h

// rdServer class handles the following functions:
// - Allowing clients to be dynamically added and removed
// - Propagating updates from the local rdDesktop object
//   to all the connected clients
// - Propagating mouse movements and keyboard events from
//   clients to the local rdDesktop
// It also creates the rdSockConnect and rdCORBAConnect
// servers, which respectively allow connections via sockets
// and via the ORB interface

class rdServer;

#if (!defined(_WINrd_rdSERVER))
#define _WINrd_rdSERVER

// Custom
#include "rdCORBAConnect.h"
#include "rdSockConnect.h"
#include "rdHTTPConnect.h"
#include "rdClient.h"
#include "rdRegion.h"
#include "rdPasswd.h"

// Includes
#include "stdhdrs.h"
#include <omnithread.h>
#include <list>

// Define a datatype to handle lists of windows we wish to notify
typedef std::list<HWND> rdNotifyList;

// Some important constants;
const int MAX_CLIENTS = 128;

// The rdServer class itself

class rdServer
{
public:
	// Constructor/destructor
	rdServer();
	~rdServer();

	// Client handling functions
	virtual void DisableClients(BOOL state);
	virtual BOOL ClientsDisabled();
	virtual rdClientId AddClient(VSocket *socket, BOOL auth, BOOL shared);
	virtual rdClientId AddClient(VSocket *socket,
		BOOL auth, BOOL shared, BOOL teleport, int capability,
		BOOL keysenabled, BOOL ptrenabled);
	virtual BOOL Authenticated(rdClientId client);
	virtual void KillClient(rdClientId client);

	virtual UINT AuthClientCount();
	virtual UINT UnauthClientCount();

	virtual void KillAuthClients();
	virtual void WaitUntilAuthEmpty();

	virtual void KillUnauthClients();
	virtual void WaitUntilUnauthEmpty();

	// Client info retrieval/setup
	virtual rdClient* GetClient(rdClientId clientid);
	virtual rdClientList ClientList();

	virtual void SetTeleport(rdClientId client, BOOL teleport);
	virtual void SetCapability(rdClientId client, int capability);
	virtual void SetKeyboardEnabled(rdClientId client, BOOL enabled);
	virtual void SetPointerEnabled(rdClientId client, BOOL enabled);

	virtual BOOL IsTeleport(rdClientId client);
	virtual int GetCapability(rdClientId client);
	virtual BOOL GetKeyboardEnabled(rdClientId client);
	virtual BOOL GetPointerEnabled(rdClientId client);
	virtual const char* GetClientName(rdClientId client);

	// Let a client remove itself
	virtual void RemoveClient(rdClientId client);

	// Connect/disconnect notification
	virtual BOOL AddNotify(HWND hwnd);
	virtual BOOL RemNotify(HWND hwnd);

protected:
	// Send a notification message
	virtual void DoNotify(UINT message, WPARAM wparam, LPARAM lparam);

public:
	// Update handling, used by clients to signal the screen server
	virtual void RequestUpdate();

	// Update handling, used by the screen server
	virtual void TriggerUpdate();
	virtual void UpdateRect(RECT &rect);
	virtual void UpdateRegion(rdRegion &region);
	virtual void CopyRect(RECT &dest, POINT &source);
	virtual void UpdateMouse();
	virtual void UpdateClipText(LPSTR text);
	virtual void UpdatePalette();

	// Polling mode handling
	virtual void PollUnderCursor(BOOL enable) {m_poll_undercursor = enable;};
	virtual BOOL PollUnderCursor() {return m_poll_undercursor;};
	virtual void PollForeground(BOOL enable) {m_poll_foreground = enable;};
	virtual BOOL PollForeground() {return m_poll_foreground;};
	virtual void PollFullScreen(BOOL enable) {m_poll_fullscreen = enable;};
	virtual BOOL PollFullScreen() {return m_poll_fullscreen;};

	virtual void PollConsoleOnly(BOOL enable) {m_poll_consoleonly = enable;};
	virtual BOOL PollConsoleOnly() {return m_poll_consoleonly;};
	virtual void PollOnEventOnly(BOOL enable) {m_poll_oneventonly = enable;};
	virtual BOOL PollOnEventOnly() {return m_poll_oneventonly;};

	// Client manipulation of the clipboard
	virtual void UpdateLocalClipText(LPSTR text);

	// Name and port number handling
	virtual void SetName(const char * name);
	virtual void SetPorts(const UINT port_rfb, const UINT port_http);
	virtual UINT GetPort() { return m_port; };
	virtual UINT GetHttpPort() { return m_port_http; };
	virtual void SetAutoPortSelect(const BOOL autoport) {
	    if (autoport && !m_autoportselect)
	    {
		BOOL sockconnect = SockConnected();
		SockConnect(FALSE);
		m_autoportselect = autoport;
		SockConnect(sockconnect);
	    }
		else
		{
			m_autoportselect = autoport;
		}
	};
	virtual BOOL AutoPortSelect() {return m_autoportselect;};

	// Password set/retrieve.  Note that these functions now handle the encrypted
	// form, not the plaintext form.  The buffer passwed MUST be MAXPWLEN in size.
	virtual void SetPassword(const char *passwd);
	virtual void GetPassword(char *passwd);
	virtual void SetPasswordViewOnly(const char *passwd);
	virtual void GetPasswordViewOnly(char *passwd);

	// Remote input handling
	virtual void EnableRemoteInputs(BOOL enable);
	virtual BOOL RemoteInputsEnabled();

	// Local input handling
	virtual void DisableLocalInputs(BOOL disable);
	virtual BOOL LocalInputsDisabled();

	// General connection handling
	virtual void SetConnectPriority(UINT priority) {m_connect_pri = priority;};
	virtual UINT ConnectPriority() {return m_connect_pri;};

	// Socket connection handling
	virtual BOOL SockConnect(BOOL on);
	virtual BOOL SockConnected();
	virtual BOOL SetLoopbackOnly(BOOL loopbackOnly);
	virtual BOOL LoopbackOnly();

	// Tray icon disposition
	virtual BOOL SetDisableTrayIcon(BOOL disableTrayIcon);
	virtual BOOL GetDisableTrayIcon();

	// CORBA connection handling
	virtual BOOL CORBAConnect(BOOL on);
	virtual BOOL CORBAConnected();
	virtual void GetScreenInfo(int &width, int &height, int &depth);

	// Allow connections if no password is set?
	virtual void SetAuthRequired(BOOL reqd) {m_passwd_required = reqd;};
	virtual BOOL AuthRequired() {return m_passwd_required;};

	// Beep on connect/disconnect?
	virtual void SetBeepConnect(BOOL beepConn) {m_beepConnect = beepConn;};
	virtual BOOL GetBeepConnect() {return m_beepConnect;};
	virtual void SetBeepDisconnect(BOOL beepDisconn) {m_beepDisconnect = beepDisconn;};
	virtual BOOL GetBeepDisconnect() {return m_beepDisconnect;};

	// Handling of per-client connection authorisation
	virtual void SetAuthHosts(const char *hostlist);
	virtual char *AuthHosts();
	enum AcceptQueryReject {aqrAccept, aqrQuery, aqrReject};
	virtual AcceptQueryReject VerifyHost(const char *hostname);
	virtual AcceptQueryReject AdjustVerification(AcceptQueryReject host);

	// Blacklisting of machines which fail connection attempts too often
	// Such machines will fail VerifyHost for a short period
	virtual void AddAuthHostsBlacklist(const char *machine);
	virtual void RemAuthHostsBlacklist(const char *machine);

	// Connection querying settings
	virtual void SetQuerySetting(const UINT setting) {m_querysetting = setting;};
	virtual UINT QuerySetting() {return m_querysetting;};
	virtual void SetQueryTimeout(const UINT setting) {m_querytimeout = setting;};
	virtual UINT QueryTimeout() {return m_querytimeout;};
	virtual void SetQueryAccept(const BOOL setting) {m_queryaccept = setting;};
	virtual BOOL QueryAccept() {return m_queryaccept;};
	virtual void SetQueryAllowNoPass(const BOOL setting) {m_queryallownopass = setting;};
	virtual BOOL QueryAllowNoPass() {return m_queryallownopass;};

	// Whether or not to allow incoming HTTP connections
	virtual BOOL SetHttpdEnabled(BOOL enable_httpd, BOOL enable_params);
	virtual BOOL HttpdEnabled() {return m_httpd_enabled;};
	virtual BOOL HttpdParamsEnabled() {return m_httpd_params_enabled;};

	// Whether or not to allow connections from the local machine
	virtual void SetLoopbackOk(BOOL ok) {m_loopback_allowed = ok;};
	virtual BOOL LoopbackOk() {return m_loopback_allowed;};

	// Whether or not to shutdown or logoff when the last client leaves
	virtual void SetLockSettings(int ok) {m_lock_on_exit = ok;};
	virtual int LockSettings() {return m_lock_on_exit;};

	// Timeout for automatic disconnection of idle connections
	virtual void SetAutoIdleDisconnectTimeout(const UINT timeout) {m_idle_timeout = timeout;};
	virtual UINT AutoIdleDisconnectTimeout() {return m_idle_timeout;};

	// Removal of desktop wallpaper, etc
	virtual void EnableRemoveWallpaper(const BOOL enable) {m_remove_wallpaper = enable;};
	virtual BOOL RemoveWallpaperEnabled() {return m_remove_wallpaper;};

	// Internal stuffs
protected:
	// Connection servers
	rdSockConnect		*m_socketConn;
	rdCorbaConnect		*m_corbaConn;
	rdHTTPConnect		*m_httpConn;

	// The desktop handler
	rdDesktop			*m_desktop;

	// General preferences
	UINT				m_port;
	UINT				m_port_http;
	BOOL				m_autoportselect;
	char				m_password[MAXPWLEN];
	char				m_password_viewonly[MAXPWLEN];
	BOOL				m_passwd_required;
	BOOL				m_loopback_allowed;
	BOOL				m_httpd_enabled;
	BOOL				m_httpd_params_enabled;
	BOOL				m_loopbackOnly;
	BOOL				m_disableTrayIcon;
	char				*m_auth_hosts;
	BOOL				m_enable_remote_inputs;
	BOOL				m_disable_local_inputs;
	int					m_lock_on_exit;
	int					m_connect_pri;
	BOOL				m_beepConnect;
	BOOL				m_beepDisconnect;
	UINT				m_querysetting;
	UINT				m_querytimeout;
	BOOL				m_queryaccept;
	BOOL				m_queryallownopass;
	BOOL				m_clients_disabled;
	UINT				m_idle_timeout;

	BOOL				m_remove_wallpaper;

	// Polling preferences
	BOOL				m_poll_fullscreen;
	BOOL				m_poll_foreground;
	BOOL				m_poll_undercursor;

	BOOL				m_poll_oneventonly;
	BOOL				m_poll_consoleonly;

	// Name of this desktop
	char				*m_name;

	// Blacklist structures
	struct BlacklistEntry {
		BlacklistEntry *_next;
		char *_machineName;
		LARGE_INTEGER _lastRefTime;
		UINT _failureCount;
		BOOL _blocked;
	};
	BlacklistEntry		*m_blacklist;
	
	// The client lists - list of clients being authorised and ones
	// already authorised
	rdClientList		m_unauthClients;
	rdClientList		m_authClients;
	rdClient			*m_clientmap[MAX_CLIENTS];
	rdClientId			m_nextid;

	// Lock to protect the client list from concurrency - lock when reading/updating client list
	omni_mutex			m_clientsLock;
	// Lock to protect the desktop object from concurrency - lock when updating client list
	omni_mutex			m_desktopLock;

	// Signal set when a client removes itself
	omni_condition		*m_clientquitsig;

	// Set of windows to send notifications to
	rdNotifyList		m_notifyList;
};

#endif
