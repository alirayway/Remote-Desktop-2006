
#include "stdhdrs.h"
#include "rdviewer.h"
#include "ClientConnection.h"

// Parameters for scrolling in full screen mode
#define BUMPSCROLLBORDER 4
#define BUMPSCROLLAMOUNTX 16
#define BUMPSCROLLAMOUNTY 4

bool ClientConnection::InFullScreenMode() 
{
	return m_opts.m_FullScreen; 
};

// You can explicitly change mode by calling this
void ClientConnection::SetFullScreenMode(bool enable)
{	
	m_opts.m_FullScreen = enable;
	RealiseFullScreenMode(false);
}

// If the options have been changed other than by calling 
// SetFullScreenMode, you need to call this to make it happen.
void ClientConnection::RealiseFullScreenMode(bool suppressPrompt)
{
	LONG style = GetWindowLong(m_hwnd, GWL_STYLE);
	if (m_opts.m_FullScreen) {

		HKEY hRegKey;
		DWORD skipprompt = (DWORD)suppressPrompt;

		if (!suppressPrompt) {
			// A bit crude here - we can skip the prompt on a registry setting.
			// We'll do this properly later.
			if ( RegCreateKey(HKEY_CURRENT_USER, SETTINGS_KEY_NAME, &hRegKey)  != ERROR_SUCCESS ) {
		        hRegKey = NULL;
			} else {
				DWORD skippromptsize = sizeof(skipprompt);
				DWORD valtype;	
				if ( RegQueryValueEx( hRegKey,  "SkipFullScreenPrompt", NULL, &valtype, 
					(LPBYTE) &skipprompt, &skippromptsize) != ERROR_SUCCESS) {
					skipprompt = 0;
				}
				RegCloseKey(hRegKey);
			}
		}

		if (!skipprompt)
			MessageBox(m_hwnd, 
				_T("To exit from full-screen mode, use Ctrl-Esc Esc and then\r\n"
				"right-click on the rdviewer taskbar icon to see the menu."),
				_T("rdviewer full-screen mode"),
				MB_OK | MB_ICONINFORMATION | MB_TOPMOST | MB_SETFOREGROUND);

		ShowWindow(m_hwnd, SW_MAXIMIZE);
		style = GetWindowLong(m_hwnd, GWL_STYLE);
		style &= ~(WS_DLGFRAME | WS_THICKFRAME);
		SetWindowLong(m_hwnd, GWL_STYLE, style);
		int cx = GetSystemMetrics(SM_CXSCREEN);
		int cy = GetSystemMetrics(SM_CYSCREEN);
		SetWindowPos(m_hwnd, HWND_TOPMOST, -1, -1, cx+3, cy+3, SWP_FRAMECHANGED);
		CheckMenuItem(GetSystemMenu(m_hwnd, FALSE), ID_FULLSCREEN, MF_BYCOMMAND|MF_CHECKED);

	} else {
		style |= WS_DLGFRAME | WS_THICKFRAME;
		SetWindowLong(m_hwnd, GWL_STYLE, style);
		SetWindowPos(m_hwnd, HWND_NOTOPMOST, 0,0,100,100, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(m_hwnd, SW_NORMAL);
		CheckMenuItem(GetSystemMenu(m_hwnd, FALSE), ID_FULLSCREEN, MF_BYCOMMAND|MF_UNCHECKED);
	}
}

bool ClientConnection::BumpScroll(int x, int y)
{
	int dx = 0;
	int dy = 0;
	int rightborder = GetSystemMetrics(SM_CXSCREEN)-BUMPSCROLLBORDER;
	int bottomborder = GetSystemMetrics(SM_CYSCREEN)-BUMPSCROLLBORDER;
	if (x < BUMPSCROLLBORDER)
		dx = -BUMPSCROLLAMOUNTX * m_opts.m_scale_num / m_opts.m_scale_den;
	if (x >= rightborder)
		dx = +BUMPSCROLLAMOUNTX * m_opts.m_scale_num / m_opts.m_scale_den;;
	if (y < BUMPSCROLLBORDER)
		dy = -BUMPSCROLLAMOUNTY * m_opts.m_scale_num / m_opts.m_scale_den;;
	if (y >= bottomborder)
		dy = +BUMPSCROLLAMOUNTY * m_opts.m_scale_num / m_opts.m_scale_den;;
	if (dx || dy) {
		if (ScrollScreen(dx,dy)) {
			// If we haven't physically moved the cursor, artificially
			// generate another mouse event so we keep scrolling.
			POINT p;
			GetCursorPos(&p);
			if (p.x == x && p.y == y)
				SetCursorPos(x,y);
			return true;
		} 
	}
	return false;
}
