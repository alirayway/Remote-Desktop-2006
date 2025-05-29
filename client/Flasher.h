

#ifndef FLASHER_H__
#define FLASHER_H__

#pragma once

// Flasher is used with the AT&T internal version of rd, when the user may need to 
// choose which monitor in a room should be used to display his desktop. A server can
// connect to a listening viewer and flash the boundary of the screen, typically displaying
// the userid of the person doing the flashing at the same time.
// Not used in the external version.

class Flasher  
{
public:
	Flasher(int port);
	virtual ~Flasher();
	static void CloseScreenSaver();
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
protected:
	SOCKET m_sock;
	HWND m_hwnd;
	HFONT m_hfont;
};

#endif // FLASHER_H__

