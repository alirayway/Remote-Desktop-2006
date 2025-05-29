//

#include "stdhdrs.h"
#include "rdviewer.h"
#include "ClientConnection.h"
#include "Exception.h"

// This file contains the code for getting text from, and putting text into
// the Windows clipboard.

//
// ProcessClipboardChange
// Called by ClientConnection::WndProc.
// We've been informed that the local clipboard has been updated.
// If it's text we want to send it to the server.
//

void ClientConnection::ProcessLocalClipboardChange()
{
	rdlog.Print(2, _T("Clipboard changed\n"));
	
	HWND hOwner = GetClipboardOwner();
	if (hOwner == m_hwnd) {
		rdlog.Print(2, _T("We changed it - ignore!\n"));
	} else if (!m_initialClipboardSeen) {
		rdlog.Print(2, _T("Don't send initial clipboard!\n"));
		m_initialClipboardSeen = true;
	} else if (!m_opts.m_DisableClipboard) {
		
		// The clipboard should not be modified by more than one thread at once
		omni_mutex_lock l(m_clipMutex);
		
		if (OpenClipboard(m_hwnd)) { 
			HGLOBAL hglb = GetClipboardData(CF_TEXT); 
			if (hglb == NULL) {
				CloseClipboard();
			} else {
				LPSTR lpstr = (LPSTR) GlobalLock(hglb);  
				
				char *contents = new char[strlen(lpstr) + 1];
				char *unixcontents = new char[strlen(lpstr) + 1];
				strcpy(contents,lpstr);
				GlobalUnlock(hglb); 
				CloseClipboard();       		
				
				// Translate to Unix-format lines before sending
				int j = 0;
				for (int i = 0; contents[i] != '\0'; i++) {
					if (contents[i] != '\x0d') {
						unixcontents[j++] = contents[i];
					}
				}
				unixcontents[j] = '\0';
				try {
					SendClientCutText(unixcontents, strlen(unixcontents));
				} catch (WarningException &e) {
					rdlog.Print(0, _T("Exception while sending clipboard text : %s\n"), e.m_info);
					DestroyWindow(m_hwnd);
				}
				delete [] contents; 
				delete [] unixcontents;
			}
		}
	}
	// Pass the message to the next window in clipboard viewer chain
	::SendMessage(m_hwndNextViewer, WM_DRAWCLIPBOARD , 0,0); 
}

// We've read some text from the remote server, and
// we need to copy it into the local clipboard.
// Called by ClientConnection::ReadServerCutText()

void ClientConnection::UpdateLocalClipboard(char *buf, int len) {
	
	if (m_opts.m_DisableClipboard)
		return;

	// Copy to wincontents replacing LF with CR-LF
	char *wincontents = new char[len * 2 + 1];
	int j = 0;
	for (int i = 0; m_netbuf[i] != 0; i++, j++) {
        if (buf[i] == '\x0a') {
			wincontents[j++] = '\x0d';
            len++;
        }
		wincontents[j] = buf[i];
	}
	wincontents[j] = '\0';

    // The clipboard should not be modified by more than one thread at once
    {
        omni_mutex_lock l(m_clipMutex);

        if (!OpenClipboard(m_hwnd)) {
	        throw WarningException("Failed to open clipboard\n");
        }
        if (! ::EmptyClipboard()) {
	        throw WarningException("Failed to empty clipboard\n");
        }

        // Allocate a global memory object for the text. 
        HGLOBAL hglbCopy = GlobalAlloc(GMEM_DDESHARE, (len +1) * sizeof(TCHAR));
        if (hglbCopy != NULL) { 
	        // Lock the handle and copy the text to the buffer.  
	        LPTSTR lptstrCopy = (LPTSTR) GlobalLock(hglbCopy); 
	        memcpy(lptstrCopy, wincontents, len * sizeof(TCHAR)); 
	        lptstrCopy[len] = (TCHAR) 0;    // null character 
	        GlobalUnlock(hglbCopy);          // Place the handle on the clipboard.  
	        SetClipboardData(CF_TEXT, hglbCopy); 
        }

        delete [] wincontents;

        if (! ::CloseClipboard()) {
	        throw WarningException("Failed to close clipboard\n");
        }
    }
}
