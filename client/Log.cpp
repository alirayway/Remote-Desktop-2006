
// Log.cpp: implementation of the Log class.
//
//////////////////////////////////////////////////////////////////////

#include "stdhdrs.h"
#include "Log.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int Log::ToDebug   =  1;
const int Log::ToFile    =  2;
const int Log::ToConsole =  4;

const static int LINE_BUFFER_SIZE = 1024;

Log::Log(int mode, int level, LPTSTR filename, bool append)
{
    hlogfile = NULL;
    m_todebug = false;
    m_toconsole = false;
    m_tofile = false;
    SetMode(mode);
    if (mode & ToFile)  {
        SetFile(filename, append);
    }
}

void Log::SetMode(int mode) {
    
    if (mode & ToDebug)
        m_todebug = true;
    else
        m_todebug = false;

    if (mode & ToFile)  {
        m_tofile = true;
    } else {
        CloseFile();
        m_tofile = false;
    }
    
#ifdef _WIN32_WCE
	m_toconsole = false;
#else
    if (mode & ToConsole) {
        if (!m_toconsole)
            AllocConsole();
        m_toconsole = true;
    } else {
        m_toconsole = false;
    }
#endif
}


void Log::SetLevel(int level) {
    m_level = level;
}

void Log::SetFile(LPTSTR filename, bool append) 
{
    // if a log file is open, close it now.
    CloseFile();

    m_tofile  = true;
    
    // If filename is NULL or invalid we should throw an exception here
    
    hlogfile = CreateFile(
        filename,  GENERIC_WRITE, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL  );
    
    if (hlogfile == INVALID_HANDLE_VALUE) {
        // We should throw an exception here
        m_todebug = true;
        m_tofile = false;
        Print(0, _T("Error opening log file %s\n"), filename);
    }
    if (append) {
        SetFilePointer( hlogfile, 0, NULL, FILE_END );
    } else {
        SetEndOfFile( hlogfile );
    }
}

// if a log file is open, close it now.
void Log::CloseFile() {
    if (hlogfile != NULL) {
        CloseHandle(hlogfile);
        hlogfile = NULL;
    }
}


#ifndef UNDER_CE

// Non-CE version 

void Log::ReallyPrint(LPTSTR format, va_list ap) 
{
	TCHAR line[LINE_BUFFER_SIZE];
	_vsntprintf(line, sizeof(line) - 2 * sizeof(TCHAR), format, ap);
	line[LINE_BUFFER_SIZE-2] = (TCHAR)'\0';
	int len = _tcslen(line);
	if (len > 0 && len <= sizeof(line) - 2 * sizeof(TCHAR) && line[len-1] == (TCHAR)'\n') {
		// Replace trailing '\n' with MS-DOS style end-of-line.
		line[len-1] = (TCHAR)'\r';
		line[len] =   (TCHAR)'\n';
		line[len+1] = (TCHAR)'\0';
	}

    if (m_todebug) OutputDebugString(line);

    if (m_toconsole) {
        DWORD byteswritten;
        WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), line, _tcslen(line)*sizeof(TCHAR), &byteswritten, NULL); 
    }

    if (m_tofile && (hlogfile != NULL)) {
        DWORD byteswritten;
        WriteFile(hlogfile, line, _tcslen(line)*sizeof(TCHAR), &byteswritten, NULL); 
    }
}

#else

// CE version 

void Log::ReallyPrint(LPTSTR format, va_list ap) 
{
    TCHAR line[LINE_BUFFER_SIZE];
    _vsntprintf(line, sizeof(line) - sizeof(TCHAR), format, ap);
    if (m_todebug) OutputDebugString(line);

    if (m_tofile && (hlogfile != NULL)) {
        DWORD byteswritten;
		
		// Log file is more readable if non-unicode!
		char ansiline[LINE_BUFFER_SIZE];
		int origlen = _tcslen(line);
		int newlen = WideCharToMultiByte(
			CP_ACP,    // code page
			0,         // performance and mapping flags
			line,      // address of wide-character string
			origlen,   // number of characters in string
			ansiline,  // address of buffer for new string
			255,       // size of buffer
			NULL, NULL );
		WriteFile(hlogfile, ansiline, newlen, &byteswritten, NULL); 
    }	
}

#endif

Log::~Log()
{
    CloseFile();
}

Log theLog;
