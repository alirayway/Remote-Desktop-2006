

// rdMenu

// Implementation of a system tray icon & menu for Winrd

#include "stdhdrs.h"
#include "Winrd.h"
#include "rdService.h"
#include "rdConnDialog.h"
#include <lmcons.h>
#include <wininet.h>
#include <shlobj.h>

// Header

#include "rdMenu.h"

// Constants
const UINT MENU_PROPERTIES_SHOW = RegisterWindowMessage("Winrd.Properties.User.Show");
const UINT MENU_DEFAULT_PROPERTIES_SHOW = RegisterWindowMessage("Winrd.Properties.Default.Show");
const UINT MENU_ABOUTBOX_SHOW = RegisterWindowMessage("Winrd.AboutBox.Show");
const UINT MENU_SERVICEHELPER_MSG = RegisterWindowMessage("Winrd.ServiceHelper.Message");
const UINT MENU_RELOAD_MSG = RegisterWindowMessage("Winrd.Reload.Message");
const UINT MENU_ADD_CLIENT_MSG = RegisterWindowMessage("Winrd.AddClient.Message");
const UINT MENU_REMOVE_CLIENTS_MSG = RegisterWindowMessage("Winrd.RemoveClients.Message");
const char *MENU_CLASS_NAME = "Winrd Tray Icon";

bool g_restore_ActiveDesktop = false;

static void
KillActiveDesktop()
{
  rdlog.Print(LL_INTERR, rdLOG("KillActiveDesktop\n"));

  // Contact Active Desktop if possible
  HRESULT result;
  IActiveDesktop* active_desktop = 0;
  result = CoCreateInstance(CLSID_ActiveDesktop, NULL, CLSCTX_INPROC_SERVER,
    IID_IActiveDesktop, (void**)&active_desktop);
  if (result != S_OK) {
    rdlog.Print(LL_INTERR, rdLOG("unable to access Active Desktop object:%x\n"), result);
    return;
  }

  // Get Active Desktop options
  COMPONENTSOPT options;
  options.dwSize = sizeof(options);
  result = active_desktop->GetDesktopItemOptions(&options, 0);
  if (result != S_OK) {
    rdlog.Print(LL_INTERR, rdLOG("unable to fetch Active Desktop options:%x\n"), result);
    active_desktop->Release();
    return;
  }

  // Disable if currently active
  g_restore_ActiveDesktop = (options.fActiveDesktop != 0);
  if (options.fActiveDesktop) {
    rdlog.Print(LL_INTINFO, rdLOG("attempting to disable Active Desktop\n"));
    options.fActiveDesktop = FALSE;
    result = active_desktop->SetDesktopItemOptions(&options, 0);
    if (result != S_OK) {
      rdlog.Print(LL_INTERR, rdLOG("unable to disable Active Desktop:%x\n"), result);
      active_desktop->Release();
      return;
    }
  } else {
    rdlog.Print(LL_INTINFO, rdLOG("Active Desktop not enabled - ignoring\n"));
  }

  active_desktop->ApplyChanges(AD_APPLY_REFRESH);
  active_desktop->Release();
}

static void
KillWallpaper()
{
	KillActiveDesktop();

	// Tell all applications that there is no wallpaper
	// Note that this doesn't change the wallpaper registry setting!
	SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, "", SPIF_SENDCHANGE);
}

static void
RestoreActiveDesktop()
{
  // Contact Active Desktop if possible
  HRESULT result;
  IActiveDesktop* active_desktop = 0;
  result = CoCreateInstance(CLSID_ActiveDesktop, NULL, CLSCTX_INPROC_SERVER,
    IID_IActiveDesktop, (void**)&active_desktop);
  if (result != S_OK) {
    rdlog.Print(LL_INTERR, rdLOG("unable to access Active Desktop object:%x\n"), result);
    return;
  }

  // Get Active Desktop options
  COMPONENTSOPT options;
  options.dwSize = sizeof(options);
  result = active_desktop->GetDesktopItemOptions(&options, 0);
  if (result != S_OK) {
    rdlog.Print(LL_INTERR, rdLOG("unable to fetch Active Desktop options:%x\n"), result);
    active_desktop->Release();
    return;
  }

  // Re-enable if previously disabled
  if (g_restore_ActiveDesktop) {
    g_restore_ActiveDesktop = false;
    rdlog.Print(LL_INTINFO, rdLOG("attempting to re-enable Active Desktop\n"));
    options.fActiveDesktop = TRUE;
    result = active_desktop->SetDesktopItemOptions(&options, 0);
    if (result != S_OK) {
      rdlog.Print(LL_INTERR, rdLOG("unable to re-enable Active Desktop:%x\n"), result);
      active_desktop->Release();
      return;
    }
  }

  active_desktop->ApplyChanges(AD_APPLY_REFRESH);
  active_desktop->Release();
}

static void
RestoreWallpaper()
{
	RestoreActiveDesktop();
	SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, NULL, SPIF_SENDCHANGE);
}

// Implementation

rdMenu::rdMenu(rdServer *server)
{
	CoInitialize(0);

	// Save the server pointer
	m_server = server;

	// Set the initial user name to something sensible...
	rdService::CurrentUser((char *)&m_username, sizeof(m_username));

	// Create a dummy window to handle tray icon messages
	WNDCLASSEX wndclass;

	wndclass.cbSize			= sizeof(wndclass);
	wndclass.style			= 0;
	wndclass.lpfnWndProc	= rdMenu::WndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= hAppInstance;
	wndclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName	= (const char *) NULL;
	wndclass.lpszClassName	= MENU_CLASS_NAME;
	wndclass.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);

	m_hwnd = CreateWindow(MENU_CLASS_NAME,
				MENU_CLASS_NAME,
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				200, 200,
				NULL,
				NULL,
				hAppInstance,
				NULL);
	if (m_hwnd == NULL)
	{
		rdlog.Print(LL_INTERR, rdLOG("unable to CreateWindow:%d\n"), GetLastError());
		PostQuitMessage(0);
		return;
	}

	// Timer to trigger icon updating
	SetTimer(m_hwnd, 1, 5000, NULL);

	// record which client created this window
	SetWindowLong(m_hwnd, GWL_USERDATA, (LONG) this);

	// Ask the server object to notify us of stuff
	server->AddNotify(m_hwnd);

	// Initialise the properties dialog object
	if (!m_properties.Init(m_server))
	{
		rdlog.Print(LL_INTERR, rdLOG("unable to initialise Properties dialog\n"));
		PostQuitMessage(0);
		return;
	}

	// Load the icons for the tray
	m_winrd_normal_icon = LoadIcon(hAppInstance, MAKEINTRESOURCE(IDI_WINrd));
	m_winrd_disabled_icon = LoadIcon(hAppInstance, MAKEINTRESOURCE(IDI_DISABLED));
	m_flash_icon = LoadIcon(hAppInstance, MAKEINTRESOURCE(IDI_FLASH));
	m_winrd_icon = m_winrd_normal_icon;

	// Load the popup menu
	m_hmenu = LoadMenu(hAppInstance, MAKEINTRESOURCE(IDR_TRAYMENU));

	// Install the tray icon!
	AddTrayIcon();
}

rdMenu::~rdMenu()
{
	// Remove the tray icon
	DelTrayIcon();
	
	// Destroy the loaded menu
	if (m_hmenu != NULL)
		DestroyMenu(m_hmenu);

	// Tell the server to stop notifying us!
	if (m_server != NULL)
		m_server->RemNotify(m_hwnd);
}

void
rdMenu::AddTrayIcon()
{
	// If the user name is non-null then we have a user!
	if (strcmp(m_username, "") != 0)
	{
		// Make sure the server has not been configured to
		// suppress the tray icon.
		if ( ! m_server->GetDisableTrayIcon())
		{
			SendTrayMsg(NIM_ADD, FALSE);
		}
	}
}

void
rdMenu::DelTrayIcon()
{
	SendTrayMsg(NIM_DELETE, FALSE);
}

void
rdMenu::FlashTrayIcon(BOOL flash)
{
	SendTrayMsg(NIM_MODIFY, flash);
}

// Get the local ip addresses as a human-readable string.
// If more than one, then with \n between them.
// If not available, then gets a message to that effect.
void GetIPAddrString(char *buffer, int buflen) {
    char namebuf[256];

    if (gethostname(namebuf, 256) != 0) {
	strncpy(buffer, "Host name unavailable", buflen);
	return;
    };

    HOSTENT *ph = gethostbyname(namebuf);
    if (!ph) {
	strncpy(buffer, "IP address unavailable", buflen);
	return;
    };

    *buffer = '\0';
    char digtxt[5];
    for (int i = 0; ph->h_addr_list[i]; i++) {
    	for (int j = 0; j < ph->h_length; j++) {
	    sprintf(digtxt, "%d.", (unsigned char) ph->h_addr_list[i][j]);
	    strncat(buffer, digtxt, (buflen-1)-strlen(buffer));
	}	
	buffer[strlen(buffer)-1] = '\0';
	if (ph->h_addr_list[i+1] != 0)
	    strncat(buffer, ", ", (buflen-1)-strlen(buffer));
    }
}

void
rdMenu::SendTrayMsg(DWORD msg, BOOL flash)
{
	// Create the tray icon message
	m_nid.hWnd = m_hwnd;
	m_nid.cbSize = sizeof(m_nid);
	m_nid.uID = IDI_WINrd;			// never changes after construction
	m_nid.hIcon = flash ? m_flash_icon : m_winrd_icon;
	m_nid.uFlags = NIF_ICON | NIF_MESSAGE;
	m_nid.uCallbackMessage = WM_TRAYNOTIFY;

	// Use resource string as tip if there is one
	if (LoadString(hAppInstance, IDI_WINrd, m_nid.szTip, sizeof(m_nid.szTip)))
	{
	    m_nid.uFlags |= NIF_TIP;
	}
	
	// Try to add the server's IP addresses to the tip string, if possible
	if (m_nid.uFlags & NIF_TIP)
	{
	    strncat(m_nid.szTip, " - ", (sizeof(m_nid.szTip)-1)-strlen(m_nid.szTip));

	    if (m_server->SockConnected())
	    {
		unsigned long tiplen = strlen(m_nid.szTip);
		char *tipptr = ((char *)&m_nid.szTip) + tiplen;

		GetIPAddrString(tipptr, sizeof(m_nid.szTip) - tiplen);
	 		if (m_server->ClientsDisabled())
				strncat(m_nid.szTip, " (Not listening)", (sizeof(m_nid.szTip)-1)-strlen(m_nid.szTip));
	    }
	    else
	    {
		strncat(m_nid.szTip, "Not listening", (sizeof(m_nid.szTip)-1)-strlen(m_nid.szTip));
	    }
	}

	// Send the message
	if (Shell_NotifyIcon(msg, &m_nid))
	{
		// Set the enabled/disabled state of the menu items
		rdlog.Print(LL_INTINFO, rdLOG("tray icon added ok\n"));

		EnableMenuItem(m_hmenu, ID_PROPERTIES,
			m_properties.AllowProperties() ? MF_ENABLED : MF_GRAYED);
		EnableMenuItem(m_hmenu, ID_CLOSE,
			m_properties.AllowShutdown() ? MF_ENABLED : MF_GRAYED);
		EnableMenuItem(m_hmenu, ID_DISABLE_CONN,
			m_properties.AllowShutdown() ? MF_ENABLED : MF_GRAYED);
		EnableMenuItem(m_hmenu, ID_KILLCLIENTS,
			m_properties.AllowEditClients() ? MF_ENABLED : MF_GRAYED);
		EnableMenuItem(m_hmenu, ID_OUTGOING_CONN,
			m_properties.AllowEditClients() ? MF_ENABLED : MF_GRAYED);
	} else {
		if (!rdService::RunningAsService())
		{
			if (msg == NIM_ADD)
			{
				// The tray icon couldn't be created, so use the Properties dialog
				// as the main program window
				rdlog.Print(LL_INTINFO, rdLOG("opening dialog box\n"));
				m_properties.Show(TRUE, TRUE);
				rdlog.Print(LL_INTERR, rdLOG("unable to add tray icon\n"), GetLastError());
				PostQuitMessage(0);
			}
		}
	}
}

// Process window messages
LRESULT CALLBACK rdMenu::WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// This is a static method, so we don't know which instantiation we're 
	// dealing with. We use Allen Hadden's (ahadden@taratec.com) suggestion 
	// from a newsgroup to get the pseudo-this.
	rdMenu *_this = (rdMenu *) GetWindowLong(hwnd, GWL_USERDATA);

	switch (iMsg)
	{

		// Every five seconds, a timer message causes the icon to update
	case WM_TIMER:
	    	// *** HACK for running servicified
		if (rdService::RunningAsService()) {
		    // Attempt to add the icon if it's not already there
		    _this->AddTrayIcon();
		    // Trigger a check of the current user
		    PostMessage(hwnd, WM_USERCHANGED, 0, 0);
		}

		// Update the icon
		_this->FlashTrayIcon(_this->m_server->AuthClientCount() != 0);
		break;

		// DEAL WITH NOTIFICATIONS FROM THE SERVER:
	case WM_SRV_CLIENT_AUTHENTICATED:
	case WM_SRV_CLIENT_DISCONNECT:
		// Adjust the icon accordingly
		_this->FlashTrayIcon(_this->m_server->AuthClientCount() != 0);

		if (_this->m_server->AuthClientCount() != 0) {
			if (_this->m_server->RemoveWallpaperEnabled())
				KillWallpaper();
		} else {
			RestoreWallpaper();
		}
		return 0;

		// STANDARD MESSAGE HANDLING
	case WM_CREATE:
		return 0;

	case WM_COMMAND:
		// User has clicked an item on the tray menu
		switch (LOWORD(wParam))
		{

		case ID_DEFAULT_PROPERTIES:
			// Show the default properties dialog, unless it is already displayed
			rdlog.Print(LL_INTINFO, rdLOG("show default properties requested\n"));
			_this->m_properties.Show(TRUE, FALSE);
			_this->FlashTrayIcon(_this->m_server->AuthClientCount() != 0);
			break;
		

		case ID_PROPERTIES:
			// Show the properties dialog, unless it is already displayed
			rdlog.Print(LL_INTINFO, rdLOG("show user properties requested\n"));
			_this->m_properties.Show(TRUE, TRUE);
			_this->FlashTrayIcon(_this->m_server->AuthClientCount() != 0);
			break;
		
		case ID_OUTGOING_CONN:
			// Connect out to a listening rd viewer
			{
				rdConnDialog *newconn = new rdConnDialog(_this->m_server);
				if (newconn)
					newconn->DoDialog();
			}
			break;

		case ID_KILLCLIENTS:
			// Disconnect all currently connected clients
			_this->m_server->KillAuthClients();
			break;

		case ID_DISABLE_CONN:
			// Disallow incoming connections (changed to leave existing ones)
			if (GetMenuState(_this->m_hmenu, ID_DISABLE_CONN, MF_BYCOMMAND) & MF_CHECKED)
			{
				//_this->m_server->SockConnect(TRUE);
				_this->m_server->DisableClients(FALSE);
				CheckMenuItem(_this->m_hmenu, ID_DISABLE_CONN, MF_UNCHECKED);
				_this->m_winrd_icon = _this->m_winrd_normal_icon;
			} else
			{
				//_this->m_server->SockConnect(FALSE);
				_this->m_server->DisableClients(TRUE);
				CheckMenuItem(_this->m_hmenu, ID_DISABLE_CONN, MF_CHECKED);
				_this->m_winrd_icon = _this->m_winrd_disabled_icon;
			}
			// Update the icon
			_this->FlashTrayIcon(_this->m_server->AuthClientCount() != 0);
			break;

		case ID_ABOUT:
			// Show the About box
			_this->m_about.Show(TRUE);
			break;

		case ID_CLOSE:
			// User selected Close from the tray menu
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;

		}
		return 0;

	case WM_TRAYNOTIFY:
		// User has clicked on the tray icon or the menu
		{
			// Get the submenu to use as a pop-up menu
			HMENU submenu = GetSubMenu(_this->m_hmenu, 0);

			// What event are we responding to, RMB click?
			if (lParam==WM_RBUTTONUP)
			{
				if (submenu == NULL)
				{ 
					rdlog.Print(LL_INTERR, rdLOG("no submenu available\n"));
					return 0;
				}

				// Make the first menu item the default (bold font)
				SetMenuDefaultItem(submenu, 0, TRUE);
				
				// Get the current cursor position, to display the menu at
				POINT mouse;
				GetCursorPos(&mouse);

				// There's a "bug"
				// (Microsoft calls it a feature) in Windows 95 that requires calling
				// SetForegroundWindow. To find out more, search for Q135788 in MSDN.
				//
				SetForegroundWindow(_this->m_nid.hWnd);

				// Display the menu at the desired position
				TrackPopupMenu(submenu,
						0, mouse.x, mouse.y, 0,
						_this->m_nid.hWnd, NULL);

				return 0;
			}
			
			// Or was there a LMB double click?
			if (lParam==WM_LBUTTONDBLCLK)
			{
				// double click: execute first menu item
				SendMessage(_this->m_nid.hWnd,
							WM_COMMAND, 
							GetMenuItemID(submenu, 0),
							0);
			}

			return 0;
		}

	case WM_CLOSE:

		// Only accept WM_CLOSE if the logged on user has AllowShutdown set
		if (!_this->m_properties.AllowShutdown())
			return 0;
		break;

	case WM_DESTROY:
		// The user wants Winrd to quit cleanly...
		rdlog.Print(LL_INTERR, rdLOG("quitting from WM_DESTROY\n"));
		PostQuitMessage(0);
		return 0;

	case WM_QUERYENDSESSION:
		rdlog.Print(LL_INTERR, rdLOG("WM_QUERYENDSESSION\n"));
		break;

	case WM_ENDSESSION:
		rdlog.Print(LL_INTERR, rdLOG("WM_ENDSESSION\n"));
		break;

/*
	case WM_ENDSESSION:
		// Are we running as a system service, or shutting the system down?
		if (wParam && (lParam == 0))
		{
			// Shutdown!
			// Try to clean ourselves up nicely, if possible...

			// Firstly, disable incoming CORBA or socket connections
			_this->m_server->SockConnect(FALSE);
			_this->m_server->CORBAConnect(FALSE);

			// Now kill all the server's clients
			_this->m_server->KillAuthClients();
			_this->m_server->KillUnauthClients();
			_this->m_server->WaitUntilAuthEmpty();
			_this->m_server->WaitUntilUnauthEmpty();
		}

		// Tell the OS that we've handled it anyway
		return 0;
		*/

	case WM_USERCHANGED:
		// The current user may have changed.
		{
			char newuser[UNLEN+1];

			if (rdService::CurrentUser((char *) &newuser, sizeof(newuser)))
			{
				rdlog.Print(LL_INTINFO,
					rdLOG("usernames : old=\"%s\", new=\"%s\"\n"),
					_this->m_username, newuser);

				// Check whether the user name has changed!
				if (strcmp(newuser, _this->m_username) != 0)
				{
					rdlog.Print(LL_INTINFO,
						rdLOG("user name has changed\n"));

					// User has changed!
					strcpy(_this->m_username, newuser);

					// Redraw the tray icon and set it's state
					_this->DelTrayIcon();
					_this->AddTrayIcon();
					_this->FlashTrayIcon(_this->m_server->AuthClientCount() != 0);

					// We should load in the prefs for the new user
					_this->m_properties.Load(TRUE);
				}
			}
		}
		return 0;
	
	default:
		// Deal with any of our custom message types
		if (iMsg == MENU_PROPERTIES_SHOW)
		{
			// External request to show our Properties dialog
			PostMessage(hwnd, WM_COMMAND, MAKELONG(ID_PROPERTIES, 0), 0);
			return 0;
		}
		if (iMsg == MENU_DEFAULT_PROPERTIES_SHOW)
		{
			// External request to show our Properties dialog
			PostMessage(hwnd, WM_COMMAND, MAKELONG(ID_DEFAULT_PROPERTIES, 0), 0);
			return 0;
		}
		if (iMsg == MENU_ABOUTBOX_SHOW)
		{
			// External request to show our About dialog
			PostMessage(hwnd, WM_COMMAND, MAKELONG(ID_ABOUT, 0), 0);
			return 0;
		}
		if (iMsg == MENU_SERVICEHELPER_MSG)
		{
			// External ServiceHelper message.
			// This message holds a process id which we can use to
			// impersonate a specific user.  In doing so, we can load their
			// preferences correctly
			rdService::ProcessUserHelperMessage(wParam, lParam);

			// - Trigger a check of the current user
			PostMessage(hwnd, WM_USERCHANGED, 0, 0);
			return 0;
		}
		if (iMsg == MENU_RELOAD_MSG)
		{
			// Reload the preferences
			_this->m_properties.Load(TRUE);

			// Redraw the tray icon and set it's state
			_this->DelTrayIcon();
			_this->AddTrayIcon();
			_this->FlashTrayIcon(_this->m_server->AuthClientCount() != 0);

			return 0;
		}
		if (iMsg == MENU_ADD_CLIENT_MSG)
		{
			// Add Client message.  This message includes an IP address and
			// a port numbrt of a listening client, to which we should connect.

			// If there is no IP address then show the connection dialog
			if (!lParam) {
				rdConnDialog *newconn = new rdConnDialog(_this->m_server);
				if (newconn) newconn->DoDialog();
				return 0;
			}

			// Get the IP address stringified
			struct in_addr address;
			address.S_un.S_addr = lParam;
			char *name = inet_ntoa(address);
			if (name == 0)
				return 0;
			char *nameDup = strdup(name);
			if (nameDup == 0)
				return 0;

			// Get the port number
			unsigned short nport = (unsigned short)wParam;
			if (nport == 0)
				nport = INCOMING_PORT_OFFSET;

			// Attempt to create a new socket
			VSocket *tmpsock;
			tmpsock = new VSocket;
			if (tmpsock) {

				// Connect out to the specified host on the rdviewer listen port
				tmpsock->Create();
				if (tmpsock->Connect(nameDup, nport)) {
					// Add the new client to this server
					_this->m_server->AddClient(tmpsock, TRUE, TRUE);
				} else {
					delete tmpsock;
				}
			}

			// Free the duplicate name
			free(nameDup);
			return 0;
		}
		if (iMsg == MENU_REMOVE_CLIENTS_MSG)
		{
			// Kill all connected clients
			_this->m_server->KillAuthClients();
			return 0;
		}
	}

	// Message not recognised
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}
