// 
// Game-Deception Window Class
// Copyright (c) Patrick   2004	( P47RICK@programmer.net )
// Copyright (c) Stinger   2004	( stinger@gmx.net )
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
// 

#ifndef CTRAYICON_H
#define CTRAYICON_H

#ifndef _WIN32_IE
#define _WIN32_IE		0x0600
#endif

#include <windows.h>
#include <stdio.h>
#include <vector>
#include <shellapi.h>

typedef void (*func_t) ( void );

typedef struct {
	UINT	m_uiIdentNumber;
	func_t	m_OnClick;
} ClickBox_t;

class cTrayIcon
{
public:
	// Prepare all the required data for our Tray Icon
	void Initialize		( char *szName,HWND hWnd,HICON hIcon );

	// remove our icon from the system tray
	void Remove			( void );

	// add our icon to the system tray
	void Add			( void );

	// update our icon
	void Update			( void );

	// change the icon
	void ChangeIcon		( HICON hIcon );

	// change the icon name & description
	void ChangeName		( char *szName );

	// show a balloon tip
	bool ShowBalloonTip	( LPCTSTR szMsg, LPCTSTR szTitle, UINT uTimeout, DWORD dwInfoFlags );
	void AddClickBox	( UINT	m_uiIdentNumber, func_t	m_OnClick, char * szName);

	// handle the incoming messages so we can react on user input
	bool bMessageHandler( HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam);	// handler for incoming messages
private:
	// storage for our buttons
	std::vector < ClickBox_t > m_vecClickBox;

	// Handle for our selection menu
	HMENU			m_hPopupMenu;

	// Struct that con tains our icon data
	NOTIFYICONDATA	m_nidTrayIcon;

	// identifier iof we even have a menu entry
	bool			m_bGotMenu;

	// stoage for mouse coordinates
	POINT			m_pPoint;
};

extern cTrayIcon gTrayIcon;
#endif