

#ifndef rdOPTIONS_H__
#define rdOPTIONS_H__

#pragma once

// The idea here was to use a slider control.  However, that seems
// to require MFC, which has not been used up to this point.  I 
// wonder why not?  WinCE compatibility maybe?
// #include <afxcmn.h>


#define LASTENCODING rfbEncodingZlibHex

#define NOCURSOR 0
#define DOTCURSOR 1
#define NORMALCURSOR 2

class rdOptions  
{
public:
	rdOptions();
	rdOptions& operator=(rdOptions& s);
	virtual ~rdOptions();
	
	// Save and load a set of options from a config file
	void Save(char *fname);
	void Load(char *fname);

	// process options
	bool	m_listening;
	int		m_listenPort;
	bool	m_connectionSpecified;
	bool	m_configSpecified;
	TCHAR   m_configFilename[_MAX_PATH];
	bool	m_restricted;

	// default connection options - can be set through Dialog
	bool	m_ViewOnly;
	bool	m_FullScreen;
	bool	m_Use8Bit;
	int		m_PreferredEncoding;
	bool	m_SwapMouse;
	bool    m_Emul3Buttons; 
 	int     m_Emul3Timeout;
 	int     m_Emul3Fuzz;
	bool	m_Shared;
	bool	m_DeiconifyOnBell;
	bool	m_DisableClipboard;
	int     m_localCursor;
	bool	m_scaling;
	int		m_scale_num, m_scale_den; // Numerator & denominator
	bool	m_useCompressLevel;
	int		m_compressLevel;
	bool	m_enableJpegCompression;
	int		m_jpegQualityLevel;
	bool	m_requestShapeUpdates;
	bool	m_ignoreShapeUpdates;

	// Keyboard can be specified on command line as 8-digit hex
	TCHAR	m_kbdname[9];
	bool	m_kbdSpecified;

	// Connection options we don't do through the dialog
	// Which encodings do we allow?
	bool	m_UseEnc[LASTENCODING+1];

	TCHAR   m_host[256];
	int     m_port;

    // Logging
    int     m_logLevel;
    bool    m_logToFile, m_logToConsole;
    TCHAR   m_logFilename[_MAX_PATH];
    
	// for debugging purposes
	int m_delay;

#ifdef UNDER_CE
	// WinCE screen format for dialogs (Palm vs HPC)
	int	m_palmpc;
	// Use slow GDI rendering, but more accurate colours.
	int m_slowgdi;
#endif

	int DoDialog(bool running = false);
	void SetFromCommandLine(LPTSTR szCmdLine);


	static BOOL CALLBACK OptDlgProc(  HWND hwndDlg,  UINT uMsg, 
		WPARAM wParam, LPARAM lParam );

	// Register() makes this viewer the app invoked for .rd files
	static void Register();

private:
    void ShowUsage(LPTSTR info = NULL);
	void FixScaling();

	// Just for temporary use
	bool m_running;

};

#endif // rdOPTIONS_H__

