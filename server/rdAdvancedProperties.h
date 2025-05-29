// rdAdvancedProperties

// Object implementing the Advanced Properties dialog for Winrd.
// The Properties dialog is displayed whenever the user selects the
// Properties option from the system tray menu.
// The Properties dialog also takes care of loading the program
// settings and saving them on exit.

class rdAdvancedProperties;

#if (!defined(_WINrd_rdADVANCEDPROPERTIES))
#define _WINrd_rdADVANCEDPROPERTIES

// Includes
#include "stdhdrs.h"
#include "rdServer.h"

// The rdProperties class itself
class rdAdvancedProperties
{
public:
	// Constructor/destructor
	rdAdvancedProperties();
	~rdAdvancedProperties();

	// Initialisation
	BOOL Init(rdServer *server);

	// The dialog box window proc
	static BOOL CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	//BOOL DoDialog();

	// Display the properties dialog
	// If usersettings is TRUE then the per-user settings come up
	// If usersettings is FALSE then the default system settings come up
	void Show(BOOL show, BOOL usersettings);

	// Loading & saving of preferences
	void Load(BOOL usersettings);
	void Save();

	// Implementation
protected:
	// The server object to which this properties object is attached.
	rdServer *			m_server;

	// Flag to indicate whether the currently loaded settings are for
	// the current user, or are default system settings
	BOOL				m_usersettings;

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
	UINT m_pref_QuerySetting;
	UINT m_pref_QueryTimeout;
	BOOL m_pref_QueryAccept;
	BOOL m_pref_QueryAllowNoPass;

	BOOL m_pref_AllowLoopback;
	BOOL m_pref_OnlyLoopback;
	BOOL m_pref_RequireAuth;
	BOOL m_pref_Log;
	BOOL m_pref_LogLots;
	int m_pref_Priority;
};

#endif // _WINrd_rdADVANCEDPROPERTIES
