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

// console...
#include "console.h"
#include "cWindow.h"
#include "resource.h"
#include "Loader9x.h"

#define STEPTIME 100


void cConsoleWindow::ThreadHandler()
{
	if(timeGetTime() > STEPTIME + m_dwLastTime)
	{
		m_dwLastTime = timeGetTime();
		// todo_ add an opacity handler here
	}
}

void cConsoleWindow::MessageSwitch()
{
	m_bIsOpen = !m_bIsOpen;

	if (m_bIsOpen)
	{
		SetWindowHeight(iWindowHeight()*2);
		SetWindowPos (hGetWindowHandle(),NULL,iWindowX(),iWindowY(),iWindowWidth(),iWindowHeight(),NULL);
		LoadDlgBitmap(IDB_BITMAP10,0,iWindowHeight()/2);
		DrawHandler();
	}
	else
	{
		SetWindowHeight(iWindowHeight()/2);
		SetWindowPos (hGetWindowHandle(),NULL,iWindowX(),iWindowY(),iWindowWidth(),iWindowHeight(),NULL);
	}
}

void cConsoleWindow::DisplayText(char *szText, int x, int y, COLORREF cColor)
{
	HDC hDC = GetDC(hGetWindowHandle());
	SECURECALL(SetBkMode(hDC, TRANSPARENT))
	SetTextColor(hDC, cColor);

	RECT Rect;
	SECURECALL(GetClientRect(hGetWindowHandle(), &Rect))

	Rect.left += x;
	Rect.top += y;
	DrawText(hDC, szText,(int)strlen(szText), &Rect, DT_EDITCONTROL | DT_WORDBREAK);

	ReleaseDC(hGetWindowHandle(), hDC); 
}

void cConsoleWindow::DrawHandler()
{
	if (!vecStrLines.size() || !m_bIsOpen)
		return;

	RECT RCTD;
	GetDimensions(&RCTD);

	LoadDlgBitmap(IDB_BITMAP10,0,RCTD.bottom/2);

	int y = 475;
	COLORREF ccrTextcolor = RGB(255,255,255);

	for (int i =(int)(vecStrLines.size() -1); i >= 0 && 250 < y;i--,y -= 15)
	{
		DisplayText((char*)vecStrLines[i].c_str(),12,y,ccrTextcolor);
	}
}

void cConsoleWindow::Print(char* szLine)
{
	if (szLine)
	{
		string tmpstr(szLine);
		vecStrLines.push_back(tmpstr);
	}
}