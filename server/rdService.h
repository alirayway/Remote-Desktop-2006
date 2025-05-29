

// rdService

// SERVICE-MODE CODE

// This class provides access to service-oriented routines, under both
// Windows NT and Windows 95.  Some routines only operate under one
// OS, others operate under any OS.

class rdService;

#if (!defined(_WINrd_rdSERVICE))
#define _WINrd_rdSERVICE

#include "stdhdrs.h"

// The NT-specific code wrapper class
class rdService
{
public:
	rdService();

	// SERVICE INSTALL & START FUNCTIONS

	// Routine called by WinMain to cause Winrd to be installed
	// as a service.
	static int WinrdServiceMain();

	// Routine to install the Winrd service on the local machine
	static int InstallService(BOOL silent=0);
	static int ReinstallService();

	// Routine to remove the Winrd service from the local machine
	static int RemoveService(BOOL silent=0);

	// SERVICE SUPPORT FUNCTIONS

	// Routine to establish and return the currently logged in user name
	static BOOL GetCurrentUser(char *buffer, UINT size);
	static BOOL CurrentUser(char *buffer, UINT size);

	// Routine to post a message to the currently running Winrd server
	// to pass it a handle to the current user
	static BOOL PostUserHelperMessage();
	static BOOL PostReloadMessage();
	// Routine to process a user helper message
	static BOOL ProcessUserHelperMessage(WPARAM wParam, LPARAM lParam);

	// Routines to establish which OS we're running on
	static BOOL IsWin95();
	static BOOL IsWinNT();
	static DWORD VersionMajor();
	static DWORD VersionMinor();

	// Routine to establish whether the current instance is running
	// as a service or not
	static BOOL RunningAsService();

	// Routine to kill any other running copy of Winrd
	static BOOL KillRunningCopy();

	// Routine to set the current thread into the given desktop
	static BOOL SelectHDESK(HDESK newdesktop);

	// Routine to set the current thread into the named desktop,
	// or the input desktop if no name is given
	static BOOL SelectDesktop(char *name);

	// Routine to establish whether the current thread desktop is the
	// current user input one
	static BOOL InputDesktopSelected();

	// Routine to fake a CtrlAltDel to winlogon when required.
	// *** This is a nasty little hack...
	static BOOL SimulateCtrlAltDel();

	// Routine to lock the workstation.  Returns TRUE if successful.
	// Main cause of failure will be when locking is not supported
	static BOOL LockWorkstation();

	// Routine to make any currently running version of Winrd show its
	// Properties dialog, to allow the user to make changes to their settings
	static BOOL ShowProperties();

	// Routine to make any currently running version of Winrd show the
	// Properties dialog for the default settings, so the user can make changes
	static BOOL ShowDefaultProperties();

	// Routine to make the an already running copy of Winrd bring up its
	// About box so you can check the version!
	static BOOL ShowAboutBox();

	// Routine to make an already running copy of Winrd form an outgoing
	// connection to a new rd client
	static BOOL PostAddNewClient(unsigned long ipaddress, unsigned short port);
};

#endif
