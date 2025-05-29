

// rdConnDialog.cpp: implementation of the rdConnDialog class, used
// to forge outgoing connections to rd-viewer 

#include "stdhdrs.h"
#include "rdConnDialog.h"
#include "Winrd.h"

#include "resource.h"

// Constructor

rdConnDialog::rdConnDialog(rdServer *server)
{
	m_server = server;
}

// Destructor

rdConnDialog::~rdConnDialog()
{
}

// Routine called to activate the dialog and, once it's done, delete it

void rdConnDialog::DoDialog()
{
	DialogBoxParam(hAppInstance, MAKEINTRESOURCE(IDD_OUTGOING_CONN), 
		NULL, (DLGPROC) rdConnDlgProc, (LONG) this);
	delete this;
}

// Callback function - handles messages sent to the dialog box

BOOL CALLBACK rdConnDialog::rdConnDlgProc(HWND hwnd,
											UINT uMsg,
											WPARAM wParam,
											LPARAM lParam) {
	// This is a static method, so we don't know which instantiation we're 
	// dealing with. But we can get a pseudo-this from the parameter to 
	// WM_INITDIALOG, which we therafter store with the window and retrieve
	// as follows:
	rdConnDialog *_this = (rdConnDialog *) GetWindowLong(hwnd, GWL_USERDATA);

	switch (uMsg) {

		// Dialog has just been created
	case WM_INITDIALOG:
		{
			// Save the lParam into our user data so that subsequent calls have
			// access to the parent C++ object

            SetWindowLong(hwnd, GWL_USERDATA, lParam);
            rdConnDialog *_this = (rdConnDialog *) lParam;
            
            // Return success!
			return TRUE;
		}

		// Dialog has just received a command
	case WM_COMMAND:
		switch (LOWORD(wParam)) {

			// User clicked OK or pressed return
		case IDOK:
			char hostname[_MAX_PATH];
			char *portp;
			int port;

			// Get the hostname of the rdviewer
			GetDlgItemText(hwnd, IDC_HOSTNAME_EDIT, hostname, _MAX_PATH);

			// Calculate the Display and Port offset.
			port = INCOMING_PORT_OFFSET;
			portp = strchr(hostname, ':');
			if (portp) {
				*portp++ = '\0';
				if (*portp == ':') {
					port = atoi(++portp);	// Port number after "::"
				} else {
					port += atoi(portp);	// Display number after ":"
				}
			}

			// Attempt to create a new socket
			VSocket *tmpsock;
			tmpsock = new VSocket;
			if (!tmpsock)
				return TRUE;

			// Connect out to the specified host on the rdviewer listen port
			// To be really good, we should allow a display number here but
			// for now we'll just assume we're connecting to display zero
			tmpsock->Create();
			if (tmpsock->Connect(hostname, port)) {
				// Add the new client to this server
				_this->m_server->AddClient(tmpsock, TRUE, TRUE);

				// And close the dialog
                EndDialog(hwnd, TRUE);
			} else {
				// Print up an error message
				MessageBox(NULL, 
                    "Failed to connect to listening rd viewer",
                    "Outgoing Connection",
					MB_OK | MB_ICONEXCLAMATION );
				delete tmpsock;
			}
			return TRUE;

			// Cancel the dialog
		case IDCANCEL:
			EndDialog(hwnd, FALSE);
			return TRUE;
		};

		break;

	case WM_DESTROY:
		EndDialog(hwnd, FALSE);
		return TRUE;
	}
	return 0;
}

