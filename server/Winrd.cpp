
// Winrd.cpp

// 24/11/97		WEZ

// WinMain and main WndProc for the new version of Winrd

////////////////////////////
// System headers
#include "stdhdrs.h"

////////////////////////////
// Custom headers
#include "VSocket.h"
#include "Winrd.h"

#include "rdServer.h"
#include "rdMenu.h"
#include "rdInstHandler.h"
#include "rdService.h"

// Application instance and name
HINSTANCE	hAppInstance;
const char	*szAppName = "Winrd";
DWORD		mainthreadId;

// WinMain parses the command line and either calls the main App
// routine or, under NT, the main service routine.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
#ifdef _DEBUG
	{
		// Get current flag
		int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );

		// Turn on leak-checking bit
		tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

		// Set flag to the new value
		_CrtSetDbgFlag( tmpFlag );
	}
#endif

	// Save the application instance and main thread id
	hAppInstance = hInstance;
	mainthreadId = GetCurrentThreadId();

	// Initialise the VSocket system
	VSocketSystem socksys;
	if (!socksys.Initialised())
	{
		MessageBox(NULL, "Failed to initialise the socket system", szAppName, MB_OK);
		return 0;
	}
	rdlog.Print(LL_STATE, rdLOG("sockets initialised\n"));

	// Make the command-line lowercase and parse it
	size_t i;
	for (i = 0; i < strlen(szCmdLine); i++)
	{
		szCmdLine[i] = tolower(szCmdLine[i]);
	}

	BOOL argfound = FALSE;
	for (i = 0; i < strlen(szCmdLine); i++)
	{
		if (szCmdLine[i] <= ' ')
			continue;
		argfound = TRUE;

		// Now check for command-line arguments
		if (strncmp(&szCmdLine[i], winrdRunServiceHelper, strlen(winrdRunServiceHelper)) == 0)
		{
			// NB : This flag MUST be parsed BEFORE "-service", otherwise it will match
			// the wrong option!  (This code should really be replaced with a simple
			// parser machine and parse-table...)

			// Run the Winrd Service Helper app
			rdService::PostUserHelperMessage();
			return 0;
		}
		if (strncmp(&szCmdLine[i], winrdRunService, strlen(winrdRunService)) == 0)
		{
			// Run Winrd as a service
			return rdService::WinrdServiceMain();
		}
		if (strncmp(&szCmdLine[i], winrdRunAsUserApp, strlen(winrdRunAsUserApp)) == 0)
		{
			// Winrd is being run as a user-level program
			return WinrdAppMain();
		}
		if (strncmp(&szCmdLine[i], winrdInstallService, strlen(winrdInstallService)) == 0)
		{
			// Install Winrd as a service
			rdService::InstallService();
			i+=strlen(winrdInstallService);
			continue;
		}
		if (strncmp(&szCmdLine[i], winrdReinstallService, strlen(winrdReinstallService)) == 0)
		{
			// Silently remove Winrd, then re-install it
			rdService::ReinstallService();
			i+=strlen(winrdReinstallService);
			continue;
		}
		if (strncmp(&szCmdLine[i], winrdRemoveService, strlen(winrdRemoveService)) == 0)
		{
			// Remove the Winrd service
			rdService::RemoveService();
			i+=strlen(winrdRemoveService);
			continue;
		}
		if (strncmp(&szCmdLine[i], winrdReload, strlen(winrdReload)) == 0)
		{
			// Reload Properties from the registry
			rdService::PostReloadMessage();
			i+=strlen(winrdReload);
			continue;
		}
		if (strncmp(&szCmdLine[i], winrdShowProperties, strlen(winrdShowProperties)) == 0)
		{
			// Show the Properties dialog of an existing instance of Winrd
			rdService::ShowProperties();
			i+=strlen(winrdShowProperties);
			continue;
		}
		if (strncmp(&szCmdLine[i], winrdShowDefaultProperties, strlen(winrdShowDefaultProperties)) == 0)
		{
			// Show the Properties dialog of an existing instance of Winrd
			rdService::ShowDefaultProperties();
			i+=strlen(winrdShowDefaultProperties);
			continue;
		}
		if (strncmp(&szCmdLine[i], winrdShowAbout, strlen(winrdShowAbout)) == 0)
		{
			// Show the About dialog of an existing instance of Winrd
			rdService::ShowAboutBox();
			i+=strlen(winrdShowAbout);
			continue;
		}
		if (strncmp(&szCmdLine[i], winrdKillRunningCopy, strlen(winrdKillRunningCopy)) == 0)
		{
			// Kill any already running copy of Winrd
			rdService::KillRunningCopy();
			i+=strlen(winrdKillRunningCopy);
			continue;
		}
		if (strncmp(&szCmdLine[i], winrdAddNewClient, strlen(winrdAddNewClient)) == 0)
		{
			// Add a new client to an existing copy of winrd
			i+=strlen(winrdAddNewClient);

			// First, we have to parse the command line to get the hostname to use
			int start, end;
			start=i;
			while (szCmdLine[start] && szCmdLine[start] <= ' ') start++;
			end = start;
			while (szCmdLine[end] > ' ') end++;

			// Was there a hostname (and optionally a port number) given?
			if (end-start > 0) {
				char *name = new char[end-start+1];
				if (name != NULL) {
					strncpy(name, &(szCmdLine[start]), end-start);
					name[end-start] = 0;

					int port = INCOMING_PORT_OFFSET;
					char *portp = strchr(name, ':');
					if (portp != NULL) {
						*portp++ = '\0';
						if (*portp == ':') {
							port = atoi(++portp);
						} else {
							port += atoi(portp);
						}
					}

					VCard32 address = VSocket::Resolve(name);
					if (address != 0) {
						// Post the IP address to the server
						rdService::PostAddNewClient(address, port);
					}
					delete [] name;
				}
			} else {
				// Tell the server to show the Add New Client dialog
				rdService::PostAddNewClient(0, 0);
			}
			i = end;
			continue;
		}

		// Either the user gave the -help option or there is something odd on the cmd-line!

		// Show the usage dialog
		MessageBox(NULL, winrdUsageText, "Winrd Usage", MB_OK | MB_ICONINFORMATION);
		break;
	};

	// If no arguments were given then just run
	if (!argfound)
		return WinrdAppMain();

	return 0;
}

// This is the main routine for Winrd when running as an application
// (under Windows 95 or Windows NT)
// Under NT, Winrd can also run as a service.  The WinrdServerMain routine,
// defined in the rdService header, is used instead when running as a service.

int WinrdAppMain()
{
	// Set this process to be the last application to be shut down.
	SetProcessShutdownParameters(0x100, 0);
	
	// Check for previous instances of Winrd!
	rdInstHandler instancehan;
	if (!instancehan.Init())
	{
		// We don't allow multiple instances!
		MessageBox(NULL, "Another instance of Winrd is already running", szAppName, MB_OK);
		return 0;
	}

	// CREATE SERVER
	rdServer server;

	// Set the name and port number
	server.SetName(szAppName);
	rdlog.Print(LL_STATE, rdLOG("server created ok\n"));

	// Create tray icon & menu if we're running as an app
	rdMenu *menu = new rdMenu(&server);
	if (menu == NULL)
	{
		rdlog.Print(LL_INTERR, rdLOG("failed to create tray menu\n"));
		PostQuitMessage(0);
	}

	// Now enter the message handling loop until told to quit!
	MSG msg;
	while (GetMessage(&msg, NULL, 0,0) ) {
		rdlog.Print(LL_INTINFO, rdLOG("message %d received\n"), msg.message);

		TranslateMessage(&msg);  // convert key ups and downs to chars
		DispatchMessage(&msg);
	}

	rdlog.Print(LL_STATE, rdLOG("shutting down server\n"));

	if (menu != NULL)
		delete menu;

	return msg.wParam;
};
