//  Copyright (C) 2001 Constantin Kaplinsky. All Rights Reserved.
//
//  This file is part of the rd system.
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
//  USA.
//
// Tightrd distribution homepage on the Web: http://www.tightrd.com/

// SharedData.h : Declarations for the global data in the DLL
// This file is used only when your compiler is Borland C++.

extern HWND hVeneto;
extern UINT UpdateRectMessage;
extern UINT CopyRectMessage;
extern UINT MouseMoveMessage;
extern HHOOK hCallWndHook;
extern HHOOK hGetMsgHook;
extern HHOOK hDialogMsgHook;
extern HHOOK hLLKeyboardHook;
extern HHOOK hLLMouseHook;
