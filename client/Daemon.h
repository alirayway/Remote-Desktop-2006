

#ifndef DAEMON_H__
#define DAEMON_H__

#pragma once

#include "stdhdrs.h"

class Daemon  
{
public:
	Daemon(int port);
	virtual ~Daemon();
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
protected:
	void AddTrayIcon();
	void CheckTrayIcon();
	void RemoveTrayIcon();
	bool SendTrayMsg(DWORD msg);
	SOCKET m_sock;
	HWND m_hwnd;
	HMENU m_hmenu;
	UINT m_timer;
	NOTIFYICONDATA m_nid;
	char netbuf[1024];
};

#endif // DAEMON_H__

