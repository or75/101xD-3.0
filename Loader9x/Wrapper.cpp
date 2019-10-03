// 
// Game-Deception Injector
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

#include <Windows.h>
#include <tlhelp32.h>
#include "cTrayIcon.h"
#include "console.h"
#include "Loader9x.h"

char* szGetDllPath ( void )
{
	static char szDllPath[1024] = { 0 };

	GetModuleFileName(GetModuleHandle(NULL),szDllPath,sizeof (char [1024]));

	int iPathLen = (int) strlen(szDllPath);

	szDllPath[iPathLen - 1 ] = 'l';
	szDllPath[iPathLen - 2 ] = 'l';
	szDllPath[iPathLen - 3 ] = 'd';

	return szDllPath;
}

void StringToLowerCase(char* szInput)
{
	int iLen = (int) strlen(szInput);

	for(int i = 0;i < iLen;i++)
	{
		if((szInput[i] >= 65)&&(szInput[i] <= 90)) // is it a lower char ?
		{
			szInput[i] += 32;
		}
	}
}

bool StringCompareEx(char* szPath)
{
	char* szPos = szPath + strlen(szPath);
	while(szPos >= szPath && *szPos != '\\')--szPos;

	StringToLowerCase(szPos);

	if((!strcmp(szPath,"hl.exe"))||(!strcmp(szPath,"cstrike.exe")))
	{
		return true;
	}

	return false;
}

DWORD dwGetHLPID( void )
{
	HANDLE			hProc;
	PROCESSENTRY32  hProcEntry;
	hProcEntry.dwSize = sizeof(PROCESSENTRY32);
	hProc			  = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);


	if( hProc == INVALID_HANDLE_VALUE )
	{
		return NULL;
	}
	if( Process32First(hProc, &hProcEntry) )
	{
		if( StringCompareEx(strlwr(hProcEntry.szExeFile)))
		{
			CloseHandle( hProc );
			return hProcEntry.th32ProcessID;
		}
		while( Process32Next(hProc, &hProcEntry) )
		{
			if( StringCompareEx(strlwr(hProcEntry.szExeFile)))
			{
				CloseHandle( hProc );
				return hProcEntry.th32ProcessID;
			}
		}
	}

	CloseHandle( hProc );
	return NULL;
}
// Credits: Y0DA
bool bCallRemoteAPI(HANDLE hRemoteProcess,PTHREAD_START_ROUTINE pfnThread,LPVOID pParams)
{
	// create the remote thread
	HANDLE hThread = CreateRemoteThread(hRemoteProcess,NULL,0,pfnThread,pParams,0,NULL);

	// check if the thread was created
	if (hThread == NULL || hThread == INVALID_HANDLE_VALUE)
		return false;

	// wait for the thread to execute
	WaitForSingleObject(hThread,INFINITE);

	// close the handle once the thread finished
	CloseHandle(hThread);

	// inform the caller about success
	return true;
}

// this function is UNTESTED! if it doesn't work use another one and don't cry
// also: if you hand over a Path of a non existant DLL the function will report success even though it failed
// if the szPath string is NOT NULL Terminated the function might crash
bool bInjectLibrary ( DWORD dwPID, const char * szPath )
{
	// Handle to the Remote Process
	HANDLE		hProcess		= NULL;

	// Pointer to the RemotePath
	char *		pszRemotePath	= NULL;

	// Open a Handle to the RemoteThread
	hProcess = OpenProcess(PROCESS_ACCESS,FALSE,dwPID);

	// check if the handle could be obtained
	if (hProcess == NULL || hProcess == INVALID_HANDLE_VALUE)
		return false;

	// Calculate the required space in the RemoteProcess
	int iPathSize = (1 + (int)strlen(szPath)) * sizeof(char);

	// allocate it
	pszRemotePath = (char *) VirtualAllocEx(hProcess,NULL,iPathSize,MEM_COMMIT,PAGE_EXECUTE_READWRITE);

	// check if we were successful
	if (pszRemotePath == NULL)
		return false;

	// write the path into the remote Process
	if (!WriteProcessMemory(hProcess,pszRemotePath,szPath,iPathSize,NULL))
		return false;

	// get the API Address
#ifdef UNICODE
	PTHREAD_START_ROUTINE pfnThread = (PTHREAD_START_ROUTINE) GetProcAddress(::GetModuleHandle("Kernel32.dll"),"LoadLibraryW");
#else
	PTHREAD_START_ROUTINE pfnThread = (PTHREAD_START_ROUTINE) GetProcAddress(::GetModuleHandle("Kernel32.dll"),"LoadLibraryA");
#endif

	if (pfnThread == NULL)// unlikely to happen...
		return false;

	// call the API
	bool bRet = bCallRemoteAPI(hProcess,pfnThread,(LPVOID)pszRemotePath);

	// Free the allocated space
	if(!VirtualFreeEx(hProcess,pszRemotePath,iPathSize,MEM_RELEASE))
		SetLastError(0xC0DEFEED);

	// Close the Handle
	if (!CloseHandle(hProcess))
		SetLastError(0xC0DEF00D);

	// return the Result
	return bRet;
}