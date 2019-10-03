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

#include "stdafx.h"
#include "Loader9x.h"
#include "console.h"
#include "cTrayIcon.h"
#include "cCRC32.h"

cConsoleWindow	gConsoleWindow;
cTrayIcon		gTrayIcon;
MSG				Msg;
DWORD			dwThreadID;
RECT			Rect;
bool			g_bMemoryCorrupted = false;

void ON_Exit()
{
	gTrayIcon.Remove();	// remove the tray icon
	ExitProcess(0);		// goodbye galaxy
}

void ON_MINIMIZE()
{
	ShowWindow(gConsoleWindow.hGetWindowHandle(),SW_HIDE);	// minimize the window
}

void ON_CONSOLE()
{
	gConsoleWindow.MessageSwitch();	// perform switch
}
DWORD dwMonitoringThread( LPVOID * lpArgs )
{
	DWORD dwPID = NULL;

//	if (g_bMemoryCorrupted)	// just if the CRC check failed
//		return 0;

	for (;true;Sleep(20))
	{
		if (gConsoleWindow.iCheckBoxState(0) == 0)
		{
			Sleep(210);
			continue;
		}

		//	gConsoleWindow.ThreadHandler();
		dwPID = dwGetHLPID();

		if (dwPID)
		{
			gConsoleWindow.Print("101xD: injection ...");
			Sleep(3000);
			if(!bInjectLibrary(dwPID,szGetDllPath()))
			{
				// the function failed so we print an error
				gConsoleWindow.Print("injection failed.");
			}
			else
			{
				// show the message balloon
				gConsoleWindow.Print("101xD: injection successful.");
				gTrayIcon.ShowBalloonTip("injection successful.","Shutting 101xD Loader",2000,IDI_LOADER9X);
				Sleep(2500);	// give the message balloon some time
				ON_Exit();		// shut down so V.A.C. can't find us
			}
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	// default windows message handler
	if (gConsoleWindow.bMessageHandler(hWnd,nMessage,wParam,lParam))
	{
		// console draw function
		if ((nMessage == WM_PAINT || nMessage == WM_SETFOCUS ) && gConsoleWindow.bIsOpen())
			gConsoleWindow.DrawHandler();
	}
	else
	{
		// the handle for our icon
		if(gTrayIcon.bMessageHandler(hWnd,nMessage,wParam,lParam)){ }
	}

	return DefWindowProc(hWnd, nMessage, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow)
{
	HANDLE hMutex = CreateMutex( 0 , 0, "101XD_LOADER_MUTEX");

	// the process is already running
	if( GetLastError() == ERROR_ALREADY_EXISTS && hMutex == INVALID_HANDLE_VALUE)
	{
		MessageBox( NULL,"Already active","Error",MB_ICONERROR);
		return 1;
	}

	// create our console window
	HWND hWnd = gConsoleWindow.hCreateWindow(200,200,350,250,"101xD Loader",129,(WndProc_t)WndProc,hInstance,IDI_SMALL);

	// sometimes the function fails on Windows 2000 so we check for it
	if (!GetClientRect(hWnd, &Rect))
		return 0;

	// create our injection thread
	HANDLE hThread = CreateThread(NULL,NULL,THREADCAST dwMonitoringThread,NULL,NULL,&dwThreadID);

	// if the injection thread fails to create we exit
	if (hThread == INVALID_HANDLE_VALUE)
		ON_Exit();

	// create the try icon (I.E. 5.0 and above)
	gTrayIcon.Initialize("101xD Loader",hWnd,LoadIcon(hInstance,MAKEINTRESOURCE(IDI_LOADER9X)));

	// add one click box (this is just an Example)
	gTrayIcon.AddClickBox(M_EXIT,ON_Exit,"EXIT");

	// add all the buttons
	gConsoleWindow.AddButton(Rect.right - 30, Rect.top    + 5 , 14, 13, ON_Exit, IDB_BITMAP4 ,IDB_BITMAP5 ,IDB_BITMAP3 ,BUTTON_STATE_NORMAL);
	gConsoleWindow.AddButton(Rect.right - 48, Rect.top    + 5 , 14, 13, ON_MINIMIZE,IDB_BITMAP12,IDB_BITMAP13,IDB_BITMAP11,BUTTON_STATE_NORMAL);
	gConsoleWindow.AddButton(Rect.right - 88, Rect.bottom - 19, 69, 13, ON_CONSOLE,IDB_BITMAP7,IDB_BITMAP8,IDB_BITMAP6,BUTTON_STATE_NORMAL);

	//	add a checkbox ;)
	gConsoleWindow.AddCheckBox(Rect.right - 161,Rect.bottom - 19,69,14,0,IDB_BITMAP15,IDB_BITMAP16,IDB_BITMAP14,BUTTON_STATE_NORMAL);

	// print the default console messages
	gConsoleWindow.Print("101xD: Site: csx-script.ru");
	gConsoleWindow.Print("101xD: Made by _or_75");
	gConsoleWindow.Print("101xD: CSX Community :3");

	// and finally the message loop
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	// mmmmh blah
	return (int) Msg.wParam;
}