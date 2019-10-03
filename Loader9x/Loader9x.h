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
#pragma once

#ifndef _WIN32_IE
#define _WIN32_IE		0x0600
#endif

// a macro which returns if an API fails
#define SECURECALL(pfn) if ( ! pfn ) return;

// identifier number for the single tray button
#define M_EXIT			0x1337

#define DLL_CHECKSUM	0xF0710C7C	// not used ATM

// cast's down our threads
#define THREADCAST		(DWORD (__stdcall *)(void *))

// default PROCESS ACCESS VALUES
#define PROCESS_ACCESS  PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ 

// default THREAD ACCESS VALUES
#define THREAD_ACCESS	THREAD_SUSPEND_RESUME|THREAD_GET_CONTEXT|THREAD_SET_CONTEXT| THREAD_SET_INFORMATION|THREAD_QUERY_INFORMATION

// our Drag styles:
#define DRAGSTYLE_USUAL			0	// default windows Drag style
#define DRAGSTYLE_NEW			1	// makes the whole window a Dragarea

// button states
#define BUTTON_STATE_NORMAL		0
#define BUTTON_STATE_HOVERED	1
#define BUTTON_STATE_PRESSED	2

// identifies the Taskbar-height
#define TASKBARHEIGHT			30

// Windows inside :D
// #define WIN32_LEAN_AND_MEAN // exclude rarely used stuff
#include <Windows.h>

// some typedefs we need :)

// for the Window Callback Routine
typedef LRESULT (CALLBACK* WndProc_t)(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam);

// 


// Callback identifier on click
typedef void (*func_t) ( void );


#include "resource.h"

// Prototypes
bool bInjectLibrary ( DWORD dwPID, const char * szPath );
char* szGetDllPath ( void );
DWORD dwGetHLPID( void );