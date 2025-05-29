// rdService

// Implementation of service-oriented functionality of Winrd

#include "stdhdrs.h"

// Header

#include "rdService.h"

#include <lmcons.h>
#include "omnithread.h"
#include "Winrd.h"
#include "rdMenu.h"
#include "rdTimedMsgBox.h"

// Error message logging
void LogErrorMsg(char *message);

// OS-SPECIFIC ROUTINES

// Create an instance of the rdService class to cause the static fields to be
// initialised properly

rdService init;

DWORD	g_platform_id;
BOOL	g_impersonating_user = 0;
DWORD	g_version_major;
DWORD	g_version_minor;

rdService::rdService()
{
    OSVERSIONINFO osversioninfo;
    osversioninfo.dwOSVersionInfoSize = sizeof(osversioninfo);

    // Get the current OS version
    if (!GetVersionEx(&osversioninfo))
	    g_platform_id = 0;
    g_platform_id = osversioninfo.dwPlatformId;
	g_version_major = osversioninfo.dwMajorVersion;
	g_version_minor = osversioninfo.dwMinorVersion;
}

// GetCurrentUser - fills a buffer with the name of the current user!
BOOL
rdService::GetCurrentUser(char *buffer, UINT size)
{
	// How to obtain the name of the current user depends upon the OS being used
	if ((g_platform_id == VER_PLATFORM_WIN32_NT) && rdService::RunningAsService())
	{
		// Windows NT, service-mode

		// -=- FIRSTLY - verify that a user is logged on

		// Get the current Window station
		HWINSTA station = GetProcessWindowStation();
		if (station == NULL)
			return FALSE;

		// Get the current user SID size
		DWORD usersize;
		GetUserObjectInformation(station,
			UOI_USER_SID, NULL, 0, &usersize);

		// Check the required buffer size isn't zero
		if (usersize == 0)
		{
			// No user is logged in - ensure we're not impersonating anyone
			RevertToSelf();
			g_impersonating_user = FALSE;

			// Return "" as the name...
			if (strlen("") >= size)
				return FALSE;
			strcpy(buffer, "");

			return TRUE;
		}

		// -=- SECONDLY - a user is logged on but if we're not impersonating
		//     them then we can't continue!
		if (!g_impersonating_user) {
			// Return "" as the name...
			if (strlen("") >= size)
				return FALSE;
			strcpy(buffer, "");
			return TRUE;
		}
	}
		
	// -=- When we reach here, we're either running under Win9x, or we're running
	//     under NT as an application or as a service impersonating a user
	// Either way, we should find a suitable user name.

	switch (g_platform_id)
	{

	case VER_PLATFORM_WIN32_WINDOWS:
	case VER_PLATFORM_WIN32_NT:
		{
			// Just call GetCurrentUser
			DWORD length = size;

			if (GetUserName(buffer, &length) == 0)
			{
				UINT error = GetLastError();

				if (error == ERROR_NOT_LOGGED_ON)
				{
					// No user logged on
					if (strlen("") >= size)
						return FALSE;
					strcpy(buffer, "");
					return TRUE;
				}
				else
				{
					// Genuine error...
					rdlog.Print(LL_INTERR, rdLOG("GetUserName() failed, error=%d\n"), GetLastError());
					return FALSE;
				}
			}
		}
		return TRUE;
	};

	// OS was not recognised!
	return FALSE;
}

BOOL
rdService::CurrentUser(char *buffer, UINT size)
{
  BOOL result = GetCurrentUser(buffer, size);
  if (result && (strcmp(buffer, "") == 0) && !rdService::RunningAsService()) {
    strncpy(buffer, "Default", size);
  }
  return result;
}

// IsWin95 - returns a BOOL indicating whether the current OS is Win95
BOOL
rdService::IsWin95()
{
	return (g_platform_id == VER_PLATFORM_WIN32_WINDOWS);
}

// IsWinNT - returns a bool indicating whether the current OS is WinNT
BOOL
rdService::IsWinNT()
{
	return (g_platform_id == VER_PLATFORM_WIN32_NT);
}

// Version info
DWORD
rdService::VersionMajor()
{
	return g_version_major;
}

DWORD
rdService::VersionMinor()
{
	return g_version_minor;
}

// Internal routine to find the Winrd menu class window and
// post a message to it!

BOOL
PostToWinrd(UINT message, WPARAM wParam, LPARAM lParam)
{
	// Locate the hidden Winrd menu window
	HWND hservwnd = FindWindow(MENU_CLASS_NAME, NULL);
	if (hservwnd == NULL)
		return FALSE;

	// Post the message to Winrd
	PostMessage(hservwnd, message, wParam, lParam);
	return TRUE;
}

// Static routines only used on Windows NT to ensure we're in the right desktop
// These routines are generally available to any thread at any time.

// - SelectDesktop(HDESK)
// Switches the current thread into a different desktop by desktop handle
// This call takes care of all the evil memory management involved

BOOL
rdService::SelectHDESK(HDESK new_desktop)
{
	// Are we running on NT?
	if (IsWinNT())
	{
		HDESK old_desktop = GetThreadDesktop(GetCurrentThreadId());

		DWORD dummy;
		char new_name[256];

		if (!GetUserObjectInformation(new_desktop, UOI_NAME, &new_name, 256, &dummy)) {
			rdlog.Print(LL_INTERR, rdLOG("GetUserObjectInformation() failed\n"));
			return FALSE;
		}

		rdlog.Print(LL_INTINFO, rdLOG("SelectHDESK() to %s (%x) from %x\n"),
					 new_name, new_desktop, old_desktop);

		// Switch the desktop
		if(!SetThreadDesktop(new_desktop)) {
			rdlog.Print(LL_INTERR, rdLOG("unable to SetThreadDesktop(), error=%d\n"), GetLastError());
			return FALSE;
		}

		// Switched successfully - destroy the old desktop
		if (!CloseDesktop(old_desktop))
			rdlog.Print(LL_INTERR, rdLOG("SelectHDESK failed to close old desktop %x, error=%d\n"), old_desktop, GetLastError());

		return TRUE;
	}

	return TRUE;
}

// - SelectDesktop(char *)
// Switches the current thread into a different desktop, by name
// Calling with a valid desktop name will place the thread in that desktop.
// Calling with a NULL name will place the thread in the current input desktop.

BOOL
rdService::SelectDesktop(char *name)
{
	// Are we running on NT?
	if (IsWinNT())
	{
		HDESK desktop;

		if (name != NULL)
		{
			// Attempt to open the named desktop
			desktop = OpenDesktop(name, 0, FALSE,
				DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
				DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
				DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
				DESKTOP_SWITCHDESKTOP | GENERIC_WRITE);
		}
		else
		{
			// No, so open the input desktop
			desktop = OpenInputDesktop(0, FALSE,
				DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
				DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
				DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
				DESKTOP_SWITCHDESKTOP | GENERIC_WRITE);
		}

		// Did we succeed?
		if (desktop == NULL) {
			rdlog.Print(LL_INTERR, rdLOG("unable to open desktop, error=%d\n"), GetLastError());
			return FALSE;
		}

		// Switch to the new desktop
		if (!SelectHDESK(desktop)) {
			// Failed to enter the new desktop, so free it!
			rdlog.Print(LL_INTERR, rdLOG("SelectDesktop() failed to select desktop\n"));
			if (!CloseDesktop(desktop))
				rdlog.Print(LL_INTERR, rdLOG("SelectDesktop failed to close desktop, error=%d\n"), GetLastError());
			return FALSE;
		}

		// We successfully switched desktops!
		return TRUE;
	}

	return (name == NULL);
}

// NT only function to establish whether we're on the current input desktop

BOOL
rdService::InputDesktopSelected()
{
	// Are we running on NT?
	if (IsWinNT())
	{
		// Get the input and thread desktops
		HDESK threaddesktop = GetThreadDesktop(GetCurrentThreadId());
		HDESK inputdesktop = OpenInputDesktop(0, FALSE,
				DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
				DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
				DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
				DESKTOP_SWITCHDESKTOP | GENERIC_WRITE);

		// Get the desktop names:
		// *** I think this is horribly inefficient but I'm not sure.
		if (inputdesktop == NULL)
		    return FALSE;

		DWORD dummy;
		char threadname[256];
		char inputname[256];

		if (!GetUserObjectInformation(threaddesktop, UOI_NAME, &threadname, 256, &dummy)) {
			if (!CloseDesktop(inputdesktop))
				rdlog.Print(LL_INTWARN, rdLOG("failed to close input desktop\n"));
			return FALSE;
		}
		_ASSERT(dummy <= 256);
		if (!GetUserObjectInformation(inputdesktop, UOI_NAME, &inputname, 256, &dummy)) {
			if (!CloseDesktop(inputdesktop))
				rdlog.Print(LL_INTWARN, rdLOG("failed to close input desktop\n"));
			return FALSE;
		}
		_ASSERT(dummy <= 256);

		if (!CloseDesktop(inputdesktop))
			rdlog.Print(LL_INTWARN, rdLOG("failed to close input desktop\n"));

		if (strcmp(threadname, inputname) != 0)
			return FALSE;
	}

	return TRUE;
}

// Static routine used to fool Winlogon into thinking CtrlAltDel was pressed

void *
SimulateCtrlAltDelThreadFn(void *context)
{
	HDESK old_desktop = GetThreadDesktop(GetCurrentThreadId());

	// Switch into the Winlogon desktop
	if (!rdService::SelectDesktop("Winlogon"))
	{
		rdlog.Print(LL_INTERR, rdLOG("failed to select logon desktop\n"));
		return FALSE;
	}

	rdlog.Print(LL_ALL, rdLOG("generating ctrl-alt-del\n"));

	// Fake a hotkey event to any windows we find there.... :(
	// Winlogon uses hotkeys to trap Ctrl-Alt-Del...
	PostMessage(HWND_BROADCAST, WM_HOTKEY, 0, MAKELONG(MOD_ALT | MOD_CONTROL, VK_DELETE));

	// Switch back to our original desktop
	if (old_desktop != NULL)
		rdService::SelectHDESK(old_desktop);

	return NULL;
}

// Static routine to simulate Ctrl-Alt-Del locally

BOOL
rdService::SimulateCtrlAltDel()
{
	rdlog.Print(LL_ALL, rdLOG("preparing to generate ctrl-alt-del\n"));

	// Are we running on NT?
	if (IsWinNT())
	{
		rdlog.Print(LL_ALL, rdLOG("spawn ctrl-alt-del thread...\n"));

		// *** This is an unpleasant hack.  Oh dear.

		// I simulate CtrAltDel by posting a WM_HOTKEY message to all
		// the windows on the Winlogon desktop.
		// This requires that the current thread is part of the Winlogon desktop.
		// But the current thread has hooks set & a window open, so it can't
		// switch desktops, so I instead spawn a new thread & let that do the work...

		omni_thread *thread = omni_thread::create(SimulateCtrlAltDelThreadFn);
		if (thread == NULL)
			return FALSE;
		thread->join(NULL);

		return TRUE;
	}

	return TRUE;
}

// Static routine to lock a 2K or above workstation

BOOL
rdService::LockWorkstation()
{
	if (!IsWinNT()) {
		rdlog.Print(LL_INTERR, rdLOG("unable to lock workstation - not NT\n"));
		return FALSE;
	}

	rdlog.Print(LL_ALL, rdLOG("locking workstation\n"));

	// Load the user32 library
	HMODULE user32 = LoadLibrary("user32.dll");
	if (!user32) {
		rdlog.Print(LL_INTERR, rdLOG("unable to load User32 DLL (%u)\n"), GetLastError());
		return FALSE;
	}

	// Get the LockWorkstation function
	typedef BOOL (*LWProc) ();
	LWProc lockworkstation = (LWProc)GetProcAddress(user32, "LockWorkStation");
	if (!lockworkstation) {
		rdlog.Print(LL_INTERR, rdLOG("unable to locate LockWorkStation - requires Windows 2000 or above (%u)\n"), GetLastError());
		FreeLibrary(user32);
		return FALSE;
	}
	
	// Attempt to lock the workstation
	BOOL result = (lockworkstation)();

	if (!result) {
		rdlog.Print(LL_INTERR, rdLOG("call to LockWorkstation failed\n"));
		FreeLibrary(user32);
		return FALSE;
	}

	FreeLibrary(user32);
	return result;
}

// Static routine to show the Properties dialog for a currently-running
// copy of Winrd, (usually a servicified version.)

BOOL
rdService::ShowProperties()
{
	// Post to the Winrd menu window
	if (!PostToWinrd(MENU_PROPERTIES_SHOW, 0, 0))
	{
		MessageBox(NULL, "No existing instance of Winrd could be contacted", szAppName, MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	}

	return TRUE;
}

// Static routine to show the Default Properties dialog for a currently-running
// copy of Winrd, (usually a servicified version.)

BOOL
rdService::ShowDefaultProperties()
{
	// Post to the Winrd menu window
	if (!PostToWinrd(MENU_DEFAULT_PROPERTIES_SHOW, 0, 0))
	{
		MessageBox(NULL, "No existing instance of Winrd could be contacted", szAppName, MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	}

	return TRUE;
}

// Static routine to show the About dialog for a currently-running
// copy of Winrd, (usually a servicified version.)

BOOL
rdService::ShowAboutBox()
{
	// Post to the Winrd menu window
	if (!PostToWinrd(MENU_ABOUTBOX_SHOW, 0, 0))
	{
		MessageBox(NULL, "No existing instance of Winrd could be contacted", szAppName, MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	}

	return TRUE;
}

// Static routine to tell a locally-running instance of the server
// to connect out to a new client

BOOL
rdService::PostAddNewClient(unsigned long ipaddress, unsigned short port)
{
	// Post to the Winrd menu window
	if (!PostToWinrd(MENU_ADD_CLIENT_MSG, (WPARAM)port, (LPARAM)ipaddress))
	{
		MessageBox(NULL, "No existing instance of Winrd could be contacted", szAppName, MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	}

	return TRUE;
}

// SERVICE-MODE ROUTINES

// Service-mode defines:

// Executable name
#define rdAPPNAME            "winrd"

// Internal service name
#define rdSERVICENAME        "winrd"

// Displayed service name
#define rdSERVICEDISPLAYNAME "rd Server"

// List of other required services ("dependency 1\0dependency 2\0\0")
// *** These need filling in properly
#define rdDEPENDENCIES       ""

// Internal service state
SERVICE_STATUS          g_srvstatus;       // current status of the service
SERVICE_STATUS_HANDLE   g_hstatus;
DWORD                   g_error = 0;
DWORD					g_servicethread = NULL;
char*                   g_errortext[256];

// Forward defines of internal service functions
void WINAPI ServiceMain(DWORD argc, char **argv);

void ServiceWorkThread(void *arg);
void ServiceStop();
void WINAPI ServiceCtrl(DWORD ctrlcode);

bool WINAPI CtrlHandler (DWORD ctrltype);

BOOL ReportStatus(DWORD state, DWORD exitcode, DWORD waithint);

// ROUTINE TO QUERY WHETHER THIS PROCESS IS RUNNING AS A SERVICE OR NOT

BOOL	g_servicemode = FALSE;

BOOL
rdService::RunningAsService()
{
	return g_servicemode;
}

BOOL
rdService::KillRunningCopy()
{
	// Locate the hidden Winrd menu window
	HWND hservwnd;

	while ((hservwnd = FindWindow(MENU_CLASS_NAME, NULL)) != NULL)
	{
		// Post the message to Winrd
		PostMessage(hservwnd, WM_CLOSE, 0, 0);

		omni_thread::sleep(1);
	}

	return TRUE;
}


// ROUTINE TO POST THE HANDLE OF THE CURRENT USER TO THE RUNNING WINrd, IN ORDER
// THAT IT CAN LOAD THE APPROPRIATE SETTINGS.  THIS IS USED ONLY BY THE SVCHELPER
// OPTION, WHEN RUNNING UNDER NT
BOOL 
rdService::PostUserHelperMessage()
{
	// - Check the platform type
	if (!IsWinNT())
		return TRUE;

	// - Get the current process ID
	DWORD processId = GetCurrentProcessId();

	// - Post it to the existing Winrd
	int retries = 6;
	while (!PostToWinrd(MENU_SERVICEHELPER_MSG, 0, (LPARAM)processId) && retries--)
		omni_thread::sleep(10);

	// - Wait until it's been used
	omni_thread::sleep(5);

	return retries;
}

BOOL
rdService::PostReloadMessage()
{
	// - Check the platform type
	if (!IsWinNT())
		return TRUE;

	// - Get the current process ID
	DWORD processId = GetCurrentProcessId();

	// - Post it to the existing Winrd
	// FIXME: Code duplication, see PostUserHelperMessage().
	int retries = 6;
	while (!PostToWinrd(MENU_RELOAD_MSG, 0, (LPARAM)processId) && retries--)
		omni_thread::sleep(10);

	// - Wait until it's been used
	omni_thread::sleep(5);

	return retries;
}


// ROUTINE TO PROCESS AN INCOMING INSTANCE OF THE ABOVE MESSAGE
BOOL
rdService::ProcessUserHelperMessage(WPARAM wParam, LPARAM lParam) {
	// - Check the platform type
	if (!IsWinNT() || !rdService::RunningAsService())
		return TRUE;

	// - Close the HKEY_CURRENT_USER key, to force NT to reload it for the new user
	// NB: Note that this is _really_ dodgy if ANY other thread is accessing the key!
	if (RegCloseKey(HKEY_CURRENT_USER) != ERROR_SUCCESS) {
		rdlog.Print(LL_INTERR, rdLOG("failed to close current registry hive\n"));
		return FALSE;
	}

	// - Revert to our own identity
	RevertToSelf();
	g_impersonating_user = FALSE;

	// - Open the specified process
	HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, (DWORD)lParam);
	if (processHandle == NULL) {
		rdlog.Print(LL_INTERR, rdLOG("failed to open specified process, error=%d\n"),
					 GetLastError());
		return FALSE;
	}

	// - Get the token for the given process
	HANDLE userToken = NULL;
	if (!OpenProcessToken(processHandle, TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_IMPERSONATE, &userToken)) {
		rdlog.Print(LL_INTERR, rdLOG("failed to get user token, error=%d\n"),
					 GetLastError());
		CloseHandle(processHandle);
		return FALSE;
	}
	CloseHandle(processHandle);

	// - Set this thread to impersonate them
	if (!ImpersonateLoggedOnUser(userToken)) {
		rdlog.Print(LL_INTERR, rdLOG("failed to impersonate user, error=%d\n"),
					 GetLastError());
		CloseHandle(userToken);
		return FALSE;
	}
	CloseHandle(userToken);

	g_impersonating_user = TRUE;
	rdlog.Print(LL_INTINFO, rdLOG("impersonating logged on user\n"));
	return TRUE;
}


// SERVICE MAIN ROUTINE
int
rdService::WinrdServiceMain()
{
	typedef DWORD (WINAPI * RegisterServiceProc)(DWORD, DWORD);
	const ULONG RSP_SIMPLE_SERVICE = 0x00000001;
	const ULONG RSP_UNREGISTER_SERVICE = 0x00000000;

	g_servicemode = TRUE;

	// How to run as a service depends upon the OS being used
	switch (g_platform_id)
	{

		// Windows 95/98
	case VER_PLATFORM_WIN32_WINDOWS:
		{
			// Obtain a handle to the kernel library
			HINSTANCE kerneldll = LoadLibrary("KERNEL32.DLL");
			if (kerneldll == NULL)
				break;

			// And find the RegisterServiceProcess function
			RegisterServiceProc RegisterService;
			RegisterService = (RegisterServiceProc) GetProcAddress(kerneldll, "RegisterServiceProcess");
			if (RegisterService == NULL)
				break;

			// Register this process with the OS as a service!
			RegisterService(NULL, RSP_SIMPLE_SERVICE);

			// Run the service itself
			WinrdAppMain();

			// Then remove the service from the system service table
			RegisterService(NULL, RSP_UNREGISTER_SERVICE);

			// Free the kernel library
			FreeLibrary(kerneldll);

			// *** If we don't kill the process directly here, then 
			// for some reason, Winrd crashes...
			// *** Is this now fixed (with the stdcall patch above)?
			//ExitProcess(0);
		}
		break;

		// Windows NT
	case VER_PLATFORM_WIN32_NT:
		{
			// Create a service entry table
			SERVICE_TABLE_ENTRY dispatchTable[] =
		    {
				{rdSERVICENAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
				{NULL, NULL}
			};

			// Call the service control dispatcher with our entry table
			if (!StartServiceCtrlDispatcher(dispatchTable))
				LogErrorMsg("StartServiceCtrlDispatcher failed.");
		}
		break;

	};

	return 0;
}

// SERVICE MAIN ROUTINE
void WINAPI ServiceMain(DWORD argc, char**argv)
{
	// Register the service control handler
    g_hstatus = RegisterServiceCtrlHandler(rdSERVICENAME, ServiceCtrl);

    if (g_hstatus == 0)
		return;

	// Set up some standard service state values
    g_srvstatus.dwServiceType = SERVICE_WIN32 | SERVICE_INTERACTIVE_PROCESS;
    g_srvstatus.dwServiceSpecificExitCode = 0;

	// Give this status to the SCM
    if (!ReportStatus(
        SERVICE_START_PENDING,	// Service state
        NO_ERROR,				// Exit code type
        15000))					// Hint as to how long Winrd should have hung before you assume error
	{
        ReportStatus(
			SERVICE_STOPPED,
			g_error,
            0);
		return;
	}

	// Now start the service for real
    omni_thread *workthread = omni_thread::create(ServiceWorkThread);
    return;
}

// SERVICE START ROUTINE - thread that calls WinrdAppMain
void ServiceWorkThread(void *arg)
{
	// Save the current thread identifier
	g_servicethread = GetCurrentThreadId();

    // report the status to the service control manager.
    //
    if (!ReportStatus(
        SERVICE_RUNNING,       // service state
        NO_ERROR,              // exit code
        0))                    // wait hint
		return;

	// RUN!
	WinrdAppMain();

	// Mark that we're no longer running
	g_servicethread = NULL;

	// Tell the service manager that we've stopped.
    ReportStatus(
		SERVICE_STOPPED,
		g_error,
		0);
}

// SERVICE STOP ROUTINE - post a quit message to the relevant thread
void ServiceStop()
{
	// Post a quit message to the main service thread
	if (g_servicethread != NULL)
	{
		rdlog.Print(LL_INTINFO, rdLOG("quitting from ServiceStop\n"));
		PostThreadMessage(g_servicethread, WM_QUIT, 0, 0);
	}
}

// SERVICE INSTALL ROUTINE
int
rdService::ReinstallService() {
	RemoveService(1);
	InstallService(0);
	return 0;
}

int
rdService::InstallService(BOOL silent)
{
	const int pathlength = 2048;
	char path[pathlength];
	char servicecmd[pathlength];

	// Get the filename of this executable
    if (GetModuleFileName(NULL, path, pathlength-(strlen(winrdRunService)+2)) == 0) {
		if (!silent) {
			MessageBox(NULL, "Unable to install Winrd service", szAppName, MB_ICONEXCLAMATION | MB_OK);
		}
		return 0;
    }

	// Append the service-start flag to the end of the path:
	if (strlen(path) + 4 + strlen(winrdRunService) < pathlength)
		sprintf(servicecmd, "\"%s\" %s", path, winrdRunService);
	else
		return 0;

	// How to add the Winrd service depends upon the OS
	switch (g_platform_id)
	{

		// Windows 95/98
	case VER_PLATFORM_WIN32_WINDOWS:
		{
			// Locate the RunService registry entry
			HKEY runservices;
			if (RegCreateKey(HKEY_LOCAL_MACHINE, 
				"Software\\Microsoft\\Windows\\CurrentVersion\\RunServices",
				&runservices) != ERROR_SUCCESS)
			{
				if (!silent) {
					MessageBox(NULL, "The SCM could not be contacted - the Winrd service was not installed", szAppName, MB_ICONEXCLAMATION | MB_OK);
				}
				break;
			}

			// Attempt to add a Winrd key
			if (RegSetValueEx(runservices, szAppName, 0, REG_SZ, (unsigned char *)servicecmd, strlen(servicecmd)+1) != ERROR_SUCCESS)
			{
				RegCloseKey(runservices);
				if (!silent) {
					MessageBox(NULL, "The Winrd service could not be registered", szAppName, MB_ICONEXCLAMATION | MB_OK);
				}
				break;
			}

			RegCloseKey(runservices);

			// We have successfully installed the service!
			rdTimedMsgBox::Do(
				"The Winrd service was successfully installed\n"
				"The service will start now and will automatically\n"
				"be run the next time this machine is reset",
				szAppName,
				MB_ICONINFORMATION | MB_OK);

			// Run the service...
			STARTUPINFO si;
			si.cb = sizeof(si);
			si.cbReserved2 = 0;
			si.lpReserved = NULL;
			si.lpReserved2 = NULL;
			si.dwFlags = 0;
			si.lpTitle = NULL;
			PROCESS_INFORMATION pi;
			if (!CreateProcess(
				NULL, servicecmd,							// Program name & path
				NULL, NULL,									// Security attributes
				FALSE,										// Inherit handles?
				NORMAL_PRIORITY_CLASS,						// Extra startup flags
				NULL,										// Environment table
				NULL,										// Current directory
				&si,
				&pi
				))
			{
				if (!silent) {
					MessageBox(NULL, "The Winrd service failed to start",
							   szAppName, MB_ICONSTOP | MB_OK);
				}
				break;
			}
		}
		break;

		// Windows NT
	case VER_PLATFORM_WIN32_NT:
		{
			SC_HANDLE   hservice;
		    SC_HANDLE   hsrvmanager;

			// Open the default, local Service Control Manager database
		    hsrvmanager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
			if (hsrvmanager == NULL)
			{
				if (!silent) {
					MessageBox(NULL,
						"The Service Control Manager could not be contacted - the Winrd service was not registered",
						szAppName,
						MB_ICONEXCLAMATION | MB_OK);
				}
				break;
			}

			// Create an entry for the Winrd service
			hservice = CreateService(
				hsrvmanager,				// SCManager database
				rdSERVICENAME,				// name of service
				rdSERVICEDISPLAYNAME,		// name to display
				SERVICE_ALL_ACCESS,			// desired access
				SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
											// service type
				SERVICE_AUTO_START,			// start type
				SERVICE_ERROR_NORMAL,		// error control type
				servicecmd,					// service's binary
				NULL,						// no load ordering group
				NULL,						// no tag identifier
				rdDEPENDENCIES,			// dependencies
				NULL,						// LocalSystem account
				NULL);						// no password
			if (hservice == NULL)
			{
				DWORD error = GetLastError();
				if (!silent) {
					if (error == ERROR_SERVICE_EXISTS) {
						MessageBox(NULL,
							"The Winrd service is already registered",
							szAppName,
							MB_ICONEXCLAMATION | MB_OK);
					} else {
						MessageBox(NULL,
							"The Winrd service could not be registered",
							szAppName,
							MB_ICONEXCLAMATION | MB_OK);
					}
				}
 				CloseServiceHandle(hsrvmanager);
				break;
			}
			CloseServiceHandle(hsrvmanager);
			CloseServiceHandle(hservice);

			// Now install the servicehelper registry setting...
			// Locate the RunService registry entry
			HKEY runapps;
			if (RegCreateKey(HKEY_LOCAL_MACHINE, 
				"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
				&runapps) != ERROR_SUCCESS)
			{
				if (!silent) {
					MessageBox(NULL, "WARNING:Unable to install the ServiceHelper hook\nGlobal user-specific registry settings will not be loaded", szAppName, MB_ICONEXCLAMATION | MB_OK);
				}
			} else {
				char servicehelpercmd[pathlength];

				// Append the service-helper-start flag to the end of the path:
				if (strlen(path) + 4 + strlen(winrdRunServiceHelper) < pathlength)
					sprintf(servicehelpercmd, "\"%s\" %s", path, winrdRunServiceHelper);
				else
					return 0;

				// Add the rdserviceHelper entry
				if (RegSetValueEx(runapps, szAppName, 0, REG_SZ,
					(unsigned char *)servicehelpercmd, strlen(servicehelpercmd)+1) != ERROR_SUCCESS)
				{
					if (!silent) {
						MessageBox(NULL, "WARNING:Unable to install the ServiceHelper hook\nGlobal user-specific registry settings will not be loaded", szAppName, MB_ICONEXCLAMATION | MB_OK);
					}
				}
				RegCloseKey(runapps);
			}

			// Everything went fine
			rdTimedMsgBox::Do(
				"The Winrd service was successfully registered\n"
				"The service may be started from the Control Panel, and will\n"
				"automatically be run the next time this machine is reset",
				szAppName,
				MB_ICONINFORMATION | MB_OK);
		}
		break;
	};

	return 0;
}

// SERVICE REMOVE ROUTINE
int
rdService::RemoveService(BOOL silent)
{
	// How to remove the Winrd service depends upon the OS
	switch (g_platform_id)
	{

		// Windows 95/98
	case VER_PLATFORM_WIN32_WINDOWS:
		{
			// Locate the RunService registry entry
			HKEY runservices;
			if (RegOpenKey(HKEY_LOCAL_MACHINE, 
				"Software\\Microsoft\\Windows\\CurrentVersion\\RunServices",
				&runservices) != ERROR_SUCCESS)
			{
				if (!silent) {
					MessageBox(NULL, "The Service Control Manager could not be contacted - the Winrd service was not unregistered", szAppName, MB_ICONEXCLAMATION | MB_OK);
				}
			}
			else
			{
				// Attempt to delete the Winrd key
				if (RegDeleteValue(runservices, szAppName) != ERROR_SUCCESS)
				{
					RegCloseKey(runservices);
					if (!silent) {
						MessageBox(NULL, "The Winrd service could not be unregistered", szAppName, MB_ICONEXCLAMATION | MB_OK);
					}
				}

				RegCloseKey(runservices);
				break;
			}

			// Try to kill any running copy of Winrd
			if (!KillRunningCopy())
			{
				if (!silent) {
					MessageBox(NULL,
						"The Winrd service could not be contacted",
						szAppName,
						MB_ICONEXCLAMATION | MB_OK);
				}
				break;
			}

			// We have successfully removed the service!
			rdTimedMsgBox::Do("The Winrd service has been unregistered", szAppName, MB_ICONINFORMATION | MB_OK);
		}
		break;

		// Windows NT
	case VER_PLATFORM_WIN32_NT:
		{
			SC_HANDLE   hservice;
			SC_HANDLE   hsrvmanager;

			// Attempt to remove the service-helper hook
			HKEY runapps;
			if (RegOpenKey(HKEY_LOCAL_MACHINE, 
				"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
				&runapps) == ERROR_SUCCESS)
			{
				// Attempt to delete the Winrd key
				if (RegDeleteValue(runapps, szAppName) != ERROR_SUCCESS)
				{
					if (!silent) {
						MessageBox(NULL, "WARNING:The ServiceHelper hook entry could not be removed from the registry", szAppName, MB_ICONEXCLAMATION | MB_OK);
					}
				}
				RegCloseKey(runapps);
			}

			// Open the SCM
		    hsrvmanager = OpenSCManager(
                        NULL,                   // machine (NULL == local)
                        NULL,                   // database (NULL == default)
                        SC_MANAGER_ALL_ACCESS   // access required
                        );
		    if (hsrvmanager)
		    {
		        hservice = OpenService(hsrvmanager, rdSERVICENAME, SERVICE_ALL_ACCESS);

				if (hservice != NULL)
				{
					SERVICE_STATUS status;

					// Try to stop the Winrd service
					if (ControlService(hservice, SERVICE_CONTROL_STOP, &status))
					{
						while(QueryServiceStatus(hservice, &status))
						{
							if (status.dwCurrentState == SERVICE_STOP_PENDING)
								Sleep(1000);
							else
								break;
						}

						if (status.dwCurrentState != SERVICE_STOPPED) {
							if (!silent) {
								MessageBox(NULL, "The Winrd service could not be stopped", szAppName, MB_ICONEXCLAMATION | MB_OK);
							}
						}
					}

					// Now remove the service from the SCM
					if(DeleteService(hservice)) {
						rdTimedMsgBox::Do("The Winrd service has been unregistered", szAppName, MB_ICONINFORMATION | MB_OK);
					} else {
						DWORD error = GetLastError();
						if (error == ERROR_SERVICE_MARKED_FOR_DELETE) {
							if (!silent)
								MessageBox(NULL, "The Winrd service is already marked to be unregistered", szAppName, MB_ICONEXCLAMATION | MB_OK);
						} else {
							if (!silent)
								MessageBox(NULL, "The Winrd service could not be unregistered", szAppName, MB_ICONEXCLAMATION | MB_OK);
						}
					}
					CloseServiceHandle(hservice);
				}
				else if (!silent)
					MessageBox(NULL, "The Winrd service could not be found", szAppName, MB_ICONEXCLAMATION | MB_OK);

				CloseServiceHandle(hsrvmanager);
			}
			else if (!silent)
				MessageBox(NULL, "The Service Control Manager could not be contacted - the Winrd service was not unregistered", szAppName, MB_ICONEXCLAMATION | MB_OK);
		}
		break;
	};
	return 0;
}

// USEFUL SERVICE SUPPORT ROUTINES

// Service control routine
void WINAPI ServiceCtrl(DWORD ctrlcode)
{
	// What control code have we been sent?
    switch(ctrlcode)
    {

	case SERVICE_CONTROL_STOP:
		// STOP : The service must stop
		g_srvstatus.dwCurrentState = SERVICE_STOP_PENDING;
        ServiceStop();
        break;

    case SERVICE_CONTROL_INTERROGATE:
		// QUERY : Service control manager just wants to know our state
		break;

	default:
		// Control code not recognised
		break;

    }

	// Tell the control manager what we're up to.
    ReportStatus(g_srvstatus.dwCurrentState, NO_ERROR, 0);
}

// Service manager status reporting
BOOL ReportStatus(DWORD state,
				  DWORD exitcode,
				  DWORD waithint)
{
	static DWORD checkpoint = 1;
	BOOL result = TRUE;

	// If we're in the start state then we don't want the control manager
	// sending us control messages because they'll confuse us.
    if (state == SERVICE_START_PENDING)
		g_srvstatus.dwControlsAccepted = 0;
	else
		g_srvstatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	// Save the new status we've been given
	g_srvstatus.dwCurrentState = state;
	g_srvstatus.dwWin32ExitCode = exitcode;
	g_srvstatus.dwWaitHint = waithint;

	// Update the checkpoint variable to let the SCM know that we
	// haven't died if requests take a long time
	if ((state == SERVICE_RUNNING) || (state == SERVICE_STOPPED))
		g_srvstatus.dwCheckPoint = 0;
	else
        g_srvstatus.dwCheckPoint = checkpoint++;

	// Tell the SCM our new status
	if (!(result = SetServiceStatus(g_hstatus, &g_srvstatus)))
		LogErrorMsg("SetServiceStatus failed");

    return result;
}

// Error reporting
void LogErrorMsg(char *message)
{
    char	msgbuff[256];
    HANDLE	heventsrc;
    char *	strings[2];

	// Save the error code
	g_error = GetLastError();

	// Use event logging to log the error
    heventsrc = RegisterEventSource(NULL, rdSERVICENAME);

	sprintf(msgbuff, "%.200s error: %d", rdSERVICENAME, g_error);
    strings[0] = msgbuff;
    strings[1] = message;

	if (heventsrc != NULL)
	{
		MessageBeep(MB_OK);

		ReportEvent(
			heventsrc,				// handle of event source
			EVENTLOG_ERROR_TYPE,	// event type
			0,						// event category
			0,						// event ID
			NULL,					// current user's SID
			2,						// strings in 'strings'
			0,						// no bytes of raw data
			(const char **)strings,	// array of error strings
			NULL);					// no raw data

		DeregisterEventSource(heventsrc);
	}
}
