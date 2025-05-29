

// rdProperties

// Object implementing the Properties dialog for Winrd.
// The Properties dialog is displayed whenever the user selects the
// Properties option from the system tray menu.
// The Properties dialog also takes care of loading the program
// settings and saving them on exit.

class rdProperties;

#if (!defined(_WINrd_rdPROPERTIES))
#define _WINrd_rdPROPERTIES

// Includes
#include "stdhdrs.h"
#include "rdServer.h"

// The rdProperties class itself
class rdProperties
{
public:
	// Constructor/destructor
	rdProperties();
	~rdProperties();

	// Initialisation
	BOOL Init(rdServer *server);

	// The dialog box window proc
	static BOOL CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Display the properties dialog
	// If usersettings is TRUE then the per-user settings come up
	// If usersettings is FALSE then the default system settings come up
	void Show(BOOL show, BOOL usersettings);

	// Loading & saving of preferences
	void Load(BOOL usersettings);
	void Save();

	// TRAY ICON MENU SETTINGS
	BOOL AllowProperties() {return m_allowproperties;};
	BOOL AllowShutdown() {return m_allowshutdown;};
	BOOL AllowEditClients() {return m_alloweditclients;};

	// Implementation
protected:
	// The server object to which this properties object is attached.
	rdServer *			m_server;

	// Flag to indicate whether the currently loaded settings are for
	// the current user, or are default system settings
	BOOL				m_usersettings;

	// Tray icon menu settings
	BOOL				m_allowproperties;
	BOOL				m_allowshutdown;
	BOOL				m_alloweditclients;

	BOOL				m_inadvanced;

	// Password handling
	void LoadPassword(HKEY k, char *buffer, const char *entry_name);
	void SavePassword(HKEY k, const char *buffer, const char *entry_name);

	// String handling
	char * LoadString(HKEY k, LPCSTR valname);
	void SaveString(HKEY k, LPCSTR valname, const char *buffer);

	// Manipulate the registry settings
	LONG LoadInt(HKEY key, LPCSTR valname, LONG defval);
	void SaveInt(HKEY key, LPCSTR valname, LONG val);

	// Loading/saving all the user prefs
	void LoadUserPrefs(HKEY appkey);
	void SaveUserPrefs(HKEY appkey);

	// Making the loaded user prefs active
	void ApplyUserPrefs();
	
	BOOL m_returncode_valid;
	BOOL m_dlgvisible;

	// STORAGE FOR THE PROPERTIES PRIOR TO APPLICATION
	BOOL m_pref_SockConnect;
	BOOL m_pref_AutoPortSelect;
	LONG m_pref_PortNumber;
	LONG m_pref_HttpPortNumber;
	BOOL m_pref_BeepConnect;
	BOOL m_pref_BeepDisconnect;
	char m_pref_passwd[MAXPWLEN];
	char m_pref_passwd_viewonly[MAXPWLEN];
	BOOL m_pref_CORBAConn;
	UINT m_pref_QuerySetting;
	UINT m_pref_QueryTimeout;
	BOOL m_pref_QueryAccept;
	BOOL m_pref_QueryAllowNoPass;
	UINT m_pref_IdleTimeout;
	BOOL m_pref_RemoveWallpaper;
	BOOL m_pref_EnableRemoteInputs;
	int m_pref_LockSettings;
	BOOL m_pref_DisableLocalInputs;
	BOOL m_pref_PollUnderCursor;
	BOOL m_pref_PollForeground;
	BOOL m_pref_PollFullScreen;
	BOOL m_pref_PollConsoleOnly;
	BOOL m_pref_PollOnEventOnly;

private:
	void InitPortSettings(HWND hwnd);
};

#endif // _WINrd_rdPROPERTIES
