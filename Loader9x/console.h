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

#ifndef CONSOLE_H
#define CONSOLE_H
#include <Windows.h>
#include <vector>
#include <string>
#include <Mmsystem.h>
#include "cWindow.h"

using namespace std;

class cConsoleWindow : public cWindow
{
	public:
		void ThreadHandler();
		void MessageSwitch();
		void DrawHandler();
		void Print(char* szLine);

		bool bIsOpen()
		{
			return m_bIsOpen;
		};

		cConsoleWindow()
		{
			m_bIsOpen = false;
			m_dwLastTime = timeGetTime();
		};
		void	DisplayText		( char *szText, int x, int y, COLORREF cColor);

	private:
		bool m_bIsOpen;
		int m_iAplpha;
		DWORD m_dwLastTime;
		vector<string>vecStrLines;
};

#endif