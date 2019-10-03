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
#include "ctrayicon.h"
#include <Windows.h>
#include <shellapi.h>
#include "resource.h"
#include <tchar.h>

// credits: dom1n1k...

void cTrayIcon::Initialize (char *szName,HWND hWnd,HICON hIcon)
{
    m_nidTrayIcon.cbSize			= sizeof(m_nidTrayIcon);
    m_nidTrayIcon.hIcon				= hIcon;
    m_nidTrayIcon.hWnd				= hWnd;  
	m_nidTrayIcon.uCallbackMessage	= (WM_USER + 1);
    m_nidTrayIcon.uFlags			= NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_nidTrayIcon.uID				= 0x0200;
    strcpy(m_nidTrayIcon.szTip,szName);
    m_nidTrayIcon.szTip[strlen(szName)] = '\0';

	Add();
}

void cTrayIcon::Add( void )
{
	Shell_NotifyIcon(NIM_ADD, &m_nidTrayIcon);
}

void cTrayIcon::Remove( void )
{
	Shell_NotifyIcon(NIM_DELETE, &m_nidTrayIcon);
}

void cTrayIcon::Update( void )
{
	Shell_NotifyIcon(NIM_MODIFY, &m_nidTrayIcon);
}

void cTrayIcon::ChangeIcon(HICON hIcon)
{
	m_nidTrayIcon.hIcon = hIcon;
	Update();
}

void cTrayIcon::ChangeName(char *szName)
{
	strcpy(m_nidTrayIcon.szTip,szName);
	Update();
}

void cTrayIcon::AddClickBox	( UINT	m_uiIdentNumber, func_t	m_OnClick, char * szName)
{
	if (!m_bGotMenu)
	{
		m_hPopupMenu	=	CreatePopupMenu();
		m_bGotMenu		=	true;
	}
	ClickBox_t ClickBox;
	ClickBox.m_uiIdentNumber	= m_uiIdentNumber;
	ClickBox.m_OnClick			= m_OnClick;
	AppendMenu(m_hPopupMenu, MF_STRING, ClickBox.m_uiIdentNumber,szName);
	m_vecClickBox.push_back(ClickBox);
}

bool cTrayIcon::bMessageHandler( HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	if (nMessage == WM_USER + 1)
	{
		switch(LOWORD(lParam))
		{
			case WM_LBUTTONUP:
			{
				ShowWindow( hWnd, true);
				return true;
			}

			case WM_RBUTTONUP:
			{
				if(GetCursorPos(&m_pPoint))
					TrackPopupMenu(m_hPopupMenu, TPM_LEFTALIGN, m_pPoint.x, m_pPoint.y, 0, hWnd, NULL);
				return true;
			}
		}
		return true;
	}
	else
	{
		if (nMessage == WM_COMMAND)
		{
			for (int i = 0; i < (int)m_vecClickBox.size();i++)
			{
				if (m_vecClickBox[i].m_uiIdentNumber == LOWORD(wParam))
				{
					if (m_vecClickBox[i].m_OnClick)
					{
						m_vecClickBox[i].m_OnClick();
						return true;
					}
				}
			}
			return true;
		}
	}
	return false;
}

bool cTrayIcon::ShowBalloonTip(LPCTSTR szMsg,LPCTSTR szTitle, UINT uTimeout, DWORD dwInfoFlags)
{
	m_nidTrayIcon.cbSize		= sizeof(NOTIFYICONDATA);
	m_nidTrayIcon.uFlags		= NIF_INFO;
	m_nidTrayIcon.uTimeout		= uTimeout;
	m_nidTrayIcon.dwInfoFlags	= dwInfoFlags;
	m_nidTrayIcon.hIcon			= LoadIcon(NULL, MAKEINTRESOURCE(IDI_LOADER9X));

	strcpy(m_nidTrayIcon.szInfo,szMsg ? szMsg : _TEXT(""));
	strcpy(m_nidTrayIcon.szInfoTitle,szTitle ? szTitle : _TEXT(""));
	return (Shell_NotifyIcon(NIM_MODIFY, &m_nidTrayIcon) > 0);
}