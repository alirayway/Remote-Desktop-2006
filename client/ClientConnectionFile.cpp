#include "stdhdrs.h"
#include "rdviewer.h"
#include "ClientConnection.h"
#include "Exception.h"

#include "rdauth.h"

// This file contains the code for saving and loading connection info.

static OPENFILENAME ofn;

void ofnInit()
{
	static char filter[] = "rd files (*.rd)\0*.rd\0" \
						   "All files (*.*)\0*.*\0";
	memset((void *) &ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = filter;
	ofn.nMaxFile = _MAX_PATH;
	ofn.nMaxFileTitle = _MAX_FNAME + _MAX_EXT;
	ofn.lpstrDefExt = "rd";
}

//
// SaveConnection
// Save info about current connection to a file
//

void ClientConnection::SaveConnection()
{
	rdlog.Print(2, _T("Saving connection info\n"));	
	char fname[_MAX_PATH];
	char tname[_MAX_FNAME + _MAX_EXT];
	ofnInit();
	int disp = PORT_TO_DISPLAY(m_port);
	sprintf(fname, "%.10s-%d.rd", m_host, (disp > 0 && disp < 100) ? disp : m_port);
	ofn.hwndOwner = m_hwnd;
	ofn.lpstrFile = fname;
	ofn.lpstrFileTitle = tname;
	ofn.Flags = OFN_HIDEREADONLY;
	if (!GetSaveFileName(&ofn)) {
		DWORD err = CommDlgExtendedError();
		char msg[1024]; 
		switch(err) {
		case 0:	// user cancelled
			break;
		case FNERR_INVALIDFILENAME:
			strcpy(msg, "Invalid filename");
			MessageBox(m_hwnd, msg, "Error saving file", MB_ICONERROR | MB_OK);
			break;
		default:
			rdlog.Print(0, "Error %d from GetSaveFileName\n", err);
			break;
		}
		return;
	}
	rdlog.Print(1, "Saving to %s\n", fname);	
	int ret = WritePrivateProfileString("connection", "host", m_host, fname);
	char buf[32];
	sprintf(buf, "%d", m_port);
	WritePrivateProfileString("connection", "port", buf, fname);
	if (MessageBox(m_hwnd,
		"Do you want to save the password in this file?\n\r"
		"If you say Yes, anyone with access to this file could access your session\n\r"
		"and (potentially) discover your rd password.",  
		"Security warning", 
		MB_YESNO | MB_ICONWARNING) == IDYES) 
	{
		for (int i = 0; i < MAXPWLEN; i++) {
			sprintf(buf+i*2, "%02x", (unsigned int) m_encPasswd[i]);
		}
	} else
		buf[0] = '\0';
	WritePrivateProfileString("connection", "password", buf, fname);
	m_opts.Save(fname);
	m_opts.Register();
}

// returns zero if successful
int ClientConnection::LoadConnection(char *fname)
{
	if (GetPrivateProfileString("connection", "host", "", m_host, MAX_HOST_NAME_LEN, fname) == 0) {
		MessageBox(m_hwnd, "Error reading host name from file", "Config file error", MB_ICONERROR | MB_OK);
		return -1;
	};
	if ( (m_port = GetPrivateProfileInt("connection", "port", 0, fname)) == 0) {
		MessageBox(m_hwnd, "Error reading port number from file", "Config file error", MB_ICONERROR | MB_OK);
		return -1;
	};
	char buf[32];
	m_encPasswd[0] = '\0';
	if (GetPrivateProfileString("connection", "password", "", buf, 32, fname) > 0) {
		for (int i = 0; i < MAXPWLEN; i++)	{
			int x = 0;
			sscanf(buf+i*2, "%2x", &x);
			m_encPasswd[i] = (unsigned char) x;
		}
	}
	return 0;
}
