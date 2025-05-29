
// rdMenu

// This class handles creation of a system-tray icon & menu

class rdMenu;

#if (!defined(_WINrd_rdMENU))
#define _WINrd_rdMENU

#include "stdhdrs.h"
#include <lmcons.h>
#include "rdServer.h"
#include "rdProperties.h"
#include "rdAbout.h"

// Constants
extern const UINT MENU_PROPERTIES_SHOW;
extern const UINT MENU_DEFAULT_PROPERTIES_SHOW;
extern const UINT MENU_ABOUTBOX_SHOW;
extern const UINT MENU_SERVICEHELPER_MSG;
extern const UINT MENU_RELOAD_MSG;
extern const UINT MENU_ADD_CLIENT_MSG;
extern const char *MENU_CLASS_NAME;

// The tray menu class itself
class rdMenu
{
public:
	rdMenu(rdServer *server);
	~rdMenu();
protected:
	// Tray icon handling
	void AddTrayIcon();
	void DelTrayIcon();
	void FlashTrayIcon(BOOL flash);
	void SendTrayMsg(DWORD msg, BOOL flash);

	// Message handler for the tray window
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

	// Fields
protected:
	// Check that the password has been set
	void CheckPassword();

	// The server that this tray icon handles
	rdServer		*m_server;

	// Properties object for this server
	rdProperties	m_properties;

	// About dialog for this server
	rdAbout		m_about;

	HWND			m_hwnd;
	HMENU			m_hmenu;
	NOTIFYICONDATA		m_nid;

	char			m_username[UNLEN+1];

	// The icon handles
	HICON			m_winrd_icon;
	HICON			m_winrd_normal_icon;
	HICON			m_winrd_disabled_icon;
	HICON			m_flash_icon;
};


#endif
