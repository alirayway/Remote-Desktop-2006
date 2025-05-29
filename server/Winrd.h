

// Winrd header file

#include "stdhdrs.h"
#include "resource.h"

// Application specific messages

// Message used for system tray notifications
#define WM_TRAYNOTIFY				WM_USER+1

// Messages used for the server object to notify windows of things
#define WM_SRV_CLIENT_CONNECT		WM_USER+2
#define WM_SRV_CLIENT_AUTHENTICATED	WM_USER+3
#define WM_SRV_CLIENT_DISCONNECT	WM_USER+4

// Export the application details
extern HINSTANCE	hAppInstance;
extern const char	*szAppName;
extern DWORD		mainthreadId;

// Main rd server routine
extern int WinrdAppMain();

// Standard command-line flag definitions
const char winrdRunService[]		= "-service";
const char winrdRunServiceHelper[]	= "-servicehelper";
const char winrdRunAsUserApp[]		= "-run";

const char winrdInstallService[]	= "-install";
const char winrdRemoveService[]	= "-remove";
const char winrdReinstallService[]	= "-reinstall";

const char winrdReload[]			= "-reload";
const char winrdShowProperties[]	= "-settings";
const char winrdShowDefaultProperties[]	= "-defaultsettings";
const char winrdShowAbout[]		= "-about";
const char winrdKillRunningCopy[]	= "-kill";

const char winrdAddNewClient[]		= "-connect";

const char winrdShowHelp[]			= "-help";

// Usage string
const char winrdUsageText[]		= "winrd [-run] [-kill] [-connect [host[:display]]]"
									  " [-connect [host[::port]]]\n"
									  " [-install] [-remove] [-reinstall] [-settings]"
									  " [-defaultsettings] [-about]\n";
