#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <fstream>
#include "detours.h"
#include "SDKIncludes.h"
#include "TransInclude.h"
#include "NT_DDK.h"

using namespace std;

#include "UserMsgs/UserMsgs.h"
#include "Offset Scan/OffsetScan.h"
#include "ClientDll/ClientDll.h"
#include "locals.h"

HINSTANCE NewhinstDLL;

char g_szHackDir[256]={};

ClientDll_t				g_Client;
ClientDll_t*			g_pClient;
cl_enginefunc_t			g_Engine;
cl_enginefunc_t*		g_pEngine;
engine_studio_api_t		g_Studio;
engine_studio_api_t*	g_pStudio;
net_status_t			g_nStatus;

HANDLE PreSHandler;
ofstream ofile;

extern DWORD SpeedPtr;
extern DWORD SoundPtr;
extern DWORD VgBasePtr;

#pragma warning( disable:4018 )
#pragma warning( disable:4996 )

string szDirFile( char* pszName )
{
	string szRet = g_szHackDir;
	return (szRet + pszName);
}

void __cdecl add_log (const char *fmt, ...)
{
	//if(ofile != NULL)
	//{
		if(!fmt)
		{ 
			return;
		}
		va_list va_alist;
		char logbuf[256] = {0};
		va_start (va_alist, fmt);
		_vsnprintf (logbuf+strlen(logbuf), sizeof(logbuf) - strlen(logbuf), fmt, va_alist);
		va_end (va_alist);
		ofile << logbuf << endl;
	//}
}
int AddCommand ( char *cmd_name, void (*function)(void) )
{
	//add_log("AddCommand: %s",cmd_name);
	return 0;
}
cvar_t* RegisterVariable ( char *szName, char *szValue, int flags )
{
	//add_log("RegisterVariable: %s",szName);
	cvar_t* pResult = g_Engine.pfnGetCvarPointer(szName);
	if(pResult != NULL)
		return pResult;
	return g_Engine.pfnRegisterVariable(szName, szValue, flags);
}
int HookUserMsg(char *szMsgName, pfnUserMsgHook pfn)
{
	#define HOOK_MSG(n)	if(!stricmp(#n, szMsgName)){p##n = pfn;return g_Engine.pfnHookUserMsg(#n, h##n);}

	HOOK_MSG(ResetHUD);
	HOOK_MSG(SetFOV);
	HOOK_MSG(TeamInfo);
	HOOK_MSG(CurWeapon);
	HOOK_MSG(DeathMsg);

	return g_Engine.pfnHookUserMsg(szMsgName, pfn);

}
void SetupHooks(void)
{
	while(!gOffsets.Initialize())
		Sleep(100);

	Sleep(2500);

	DWORD ClientBase = gOffsets.ClientBase();
	g_pClient = (ClientDll_t*)gOffsets.ClientFuncs();
	g_pEngine = (cl_enginefunc_t*)gOffsets.EngineFuncs();
	g_pStudio = (engine_studio_api_t*)gOffsets.EngineStudio();
	SoundPtr = (DWORD)gOffsets.Sound();
	SpeedPtr = (DWORD)gOffsets.SpeedHackPtr();	

	if(!g_pClient || !g_pEngine || !g_pStudio || !SoundPtr || !SpeedPtr)
	{
		MessageBox(0,"[101xD] Error Search Offset","Error",MB_OK|MB_ICONERROR);
		return;
	}

	add_log("g_pClient: 0x%X", g_pClient);
	add_log("g_pEngine: 0x%X", g_pEngine);
	add_log("g_pStudio: 0x%X", g_pStudio);
	add_log("ClientBase: 0x%X",ClientBase);
	add_log("SpeedPtr: 0x%X",SpeedPtr);
	add_log("SoundPtr: 0x%X",SoundPtr);

	add_log("Start Hook ... ");

	RtlCopyMemory(&g_Client, g_pClient, sizeof(ClientDll_t));
	RtlCopyMemory(&g_Engine, g_pEngine, sizeof(cl_enginefunc_t));
	RtlCopyMemory(&g_Studio, g_pStudio, sizeof(engine_studio_api_t));

	add_log("Start Hook God ");

	PreS_DynamicSound_s = (PreS_DynamicSound_t)DetourFunction((PBYTE)SoundPtr,(PBYTE)PreS_DynamicSound);

	g_pStudio->StudioEntityLight = StudioEntityLight;

	HookClient();
		
	add_log("Start Hook HookUserMsg ... ");
	g_pEngine->pfnHookUserMsg = &HookUserMsg;
	add_log("Start Hook HookUserMsg God ");

	add_log("Start Hook AddCommand ... ");
	g_pEngine->pfnAddCommand = &AddCommand;
	add_log("Start Hook AddCommand God ");

	add_log("Start Hook RegisterVariable ... ");
	g_pEngine->pfnRegisterVariable = &RegisterVariable;
	add_log("Start Hook RegisterVariable God ");

	add_log("Call Initialize ... ");
	g_Client.Initialize(g_pEngine, CLDLL_INTERFACE_VERSION);
	add_log("Call Initialize God ");

	add_log("Call HUD_Init ... ");
	g_Client.HUD_Init();
	add_log("Call HUD_Init GoD ");

	g_pEngine->pfnHookEvent = g_Engine.pfnHookEvent;
	g_pEngine->pfnHookUserMsg = g_Engine.pfnHookUserMsg;
	g_pEngine->pfnAddCommand = g_Engine.pfnAddCommand;
	g_pEngine->pfnRegisterVariable = g_Engine.pfnRegisterVariable;
}
void HideModule( HANDLE hModule )
{
	PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hModule;
		if( pDOSHeader->e_magic == IMAGE_DOS_SIGNATURE )
		{
			PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)( (DWORD)pDOSHeader + (DWORD)pDOSHeader->e_lfanew );
			if( pNTHeader->Signature == IMAGE_NT_SIGNATURE )
			{
				DWORD dwOld, dwSize = pNTHeader->OptionalHeader.SizeOfHeaders;
				if( VirtualProtect( (LPVOID)pDOSHeader, dwSize, PAGE_READWRITE, &dwOld ) )
				{
					memset( (void*)pDOSHeader, 0, dwSize );
					VirtualProtect( (LPVOID)pDOSHeader, dwSize, dwOld, &dwOld );
				}
			}
			_TEB *pTeb = NULL;
			_asm
			{
				mov eax, fs:[0x18]
				mov pTeb, eax
			}
			PLIST_ENTRY pList = &pTeb->Peb->LoaderData->InLoadOrderModuleList;
			for( PLIST_ENTRY pEntry = pList->Flink; pEntry != pList; pEntry = pEntry->Flink )
			{
				PLDR_MODULE pModule = (PLDR_MODULE)pEntry;
				if( pModule->BaseAddress == hModule )
				{
					pEntry = &pModule->InLoadOrderModuleList; pEntry->Blink->Flink = pEntry->Flink; pEntry->Flink->Blink = pEntry->Blink;
					memset( pEntry, 0, sizeof( LIST_ENTRY ) );
					pEntry = &pModule->InMemoryOrderModuleList; pEntry->Blink->Flink = pEntry->Flink; pEntry->Flink->Blink = pEntry->Blink;
					memset( pEntry, 0, sizeof( LIST_ENTRY ) );
					pEntry = &pModule->InInitializationOrderModuleList; pEntry->Blink->Flink = pEntry->Flink; pEntry->Flink->Blink = pEntry->Blink;
					memset( pEntry, 0, sizeof( LIST_ENTRY ) );
					pEntry = &pModule->HashTableEntry; pEntry->Blink->Flink = pEntry->Flink; pEntry->Flink->Blink = pEntry->Blink;
					memset( pEntry, 0, sizeof( LIST_ENTRY ) );
					memset( pModule->FullDllName.Buffer, 0, pModule->FullDllName.Length );
					memset( pModule, 0, sizeof( LDR_MODULE ) );
					break;
			}
		}
	}
}
void HideModuleXta( HINSTANCE hModule )
{
	DWORD dwPEB_LDR_DATA = 0;

	_asm
	{
		pushad;
		pushfd;
		mov eax, fs:[30h]					// PEB
		mov eax, [eax+0Ch]					// PEB->ProcessModuleInfo
		mov dwPEB_LDR_DATA, eax				// Save ProcessModuleInfo
		mov esi, [eax+0Ch]					// ProcessModuleInfo->InLoadOrderModuleList[FORWARD]
		mov edx, [eax+10h]					//  ProcessModuleInfo->InLoadOrderModuleList[BACKWARD]

LoopInLoadOrderModuleList: 
		lodsd								//  Load First Module
			mov esi, eax		    			//  ESI points to Next Module
			mov ecx, [eax+18h]		    		//  LDR_MODULE->BaseAddress
		cmp ecx, hModule		    		//  Is it Our Module ?
			jne SkipA		    		    	//  If Not, Next Please (@f jumps to nearest Unamed Lable @@:)
			mov ebx, [eax]						//  [FORWARD] Module 
		mov ecx, [eax+4]    		    	//  [BACKWARD] Module
		mov [ecx], ebx						//  Previous Module's [FORWARD] Notation, Points to us, Replace it with, Module++
			mov [ebx+4], ecx					//  Next Modules, [BACKWARD] Notation, Points to us, Replace it with, Module--
			jmp InMemoryOrderModuleList			//  Hidden, so Move onto Next Set
SkipA:
		cmp edx, esi						//  Reached End of Modules ?
			jne LoopInLoadOrderModuleList		//  If Not, Re Loop

InMemoryOrderModuleList:
		mov eax, dwPEB_LDR_DATA				//  PEB->ProcessModuleInfo
			mov esi, [eax+14h]					//  ProcessModuleInfo->InMemoryOrderModuleList[START]
		mov edx, [eax+18h]					//  ProcessModuleInfo->InMemoryOrderModuleList[FINISH]

LoopInMemoryOrderModuleList: 
		lodsd
			mov esi, eax
			mov ecx, [eax+10h]
		cmp ecx, hModule
			jne SkipB
			mov ebx, [eax] 
		mov ecx, [eax+4]
		mov [ecx], ebx
			mov [ebx+4], ecx
			jmp InInitializationOrderModuleList
SkipB:
		cmp edx, esi
			jne LoopInMemoryOrderModuleList

InInitializationOrderModuleList:
		mov eax, dwPEB_LDR_DATA				     //  PEB->ProcessModuleInfo
			mov esi, [eax+1Ch]						 //  ProcessModuleInfo->InInitializationOrderModuleList[START]
		mov edx, [eax+20h]						 //  ProcessModuleInfo->InInitializationOrderModuleList[FINISH]

LoopInInitializationOrderModuleList: 
		lodsd
			mov esi, eax		
			mov ecx, [eax+08h]
		cmp ecx, hModule		
			jne SkipC
			mov ebx, [eax] 
		mov ecx, [eax+4]
		mov [ecx], ebx
			mov [ebx+4], ecx
			jmp Finished
SkipC:
		cmp edx, esi
			jne LoopInInitializationOrderModuleList

Finished:
		popfd;
		popad;
	}
}

unsigned int hashing(unsigned char* message, int len)
{
	int i = 0;
	unsigned int hash = 0x2929AEAF;
	for(int i = 1024;i < len;i++)
	{
		hash = hash ^ ((hash >> 2) + hash * 32 + (unsigned int)message[i]);
	}
	return hash;
}

unsigned int ReadCRC32(char* cFile)
{
	unsigned char code[1024];
	int codelen = 0;
	FILE* pFILE = fopen(cFile,"rb");
	fseek(pFILE,0,SEEK_END);
	codelen = ftell(pFILE);
	fseek(pFILE,0,SEEK_SET);
	fread(code,1024,1,pFILE);
	unsigned int CRC;
	memcpy(&CRC,&code[1000],4);
	fclose(pFILE);
	return (unsigned int)CRC;
}
DWORD CRC32(char* cFile)
{
	unsigned char* code;
	int codelen = 0;
	FILE* pFILE = fopen(cFile,"rb");
	fseek(pFILE,0,SEEK_END);
	codelen = ftell(pFILE);
	code = new unsigned char[codelen];
	fseek(pFILE,0,SEEK_SET);
	fread(code,codelen,1,pFILE);
	DWORD CRC = 0;
	CRC = hashing(code,codelen);	
	fclose(pFILE);
	return CRC;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, PVOID unused)
{
	HMODULE OGL;
	HANDLE mutex;

	if(fdwReason == DLL_PROCESS_ATTACH)
	{
		LPSTR mtxname = "101xD 2.9";
		if(OpenMutex(MUTEX_ALL_ACCESS, false, mtxname) != 0)
			ExitProcess(0);
		else
			mutex = CreateMutex(0, false, mtxname);

		NewhinstDLL = hinstDLL;
		DisableThreadLibraryCalls( hinstDLL );

		GetModuleFileName( (HMODULE)hinstDLL, g_szHackDir, 256 );

		//DWORD CRC_X = ReadCRC32(g_szHackDir);
		//DWORD CRC_Y = CRC32(g_szHackDir);

		//if ( CRC_X != CRC_Y )
		//	*((int*)(0x3a03cdab+rand()%50000))=0;

		char *p = g_szHackDir + strlen( g_szHackDir );
		while( p >= g_szHackDir && *p != '\\' ) { --p; }
		p[1] = '\0';

		char LogFile[1024];
		strcpy(LogFile,g_szHackDir);
		strcat(LogFile,"\\LogSignature.txt");
		remove(LogFile);
		ofile.open(LogFile, ios::app);

		OGL = LoadLibrary("Opengl32.dll"); // fix opengl32 for cFont by _or_75

		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SetupHooks, NULL, 0, NULL);
	}
	if(fdwReason == DLL_PROCESS_DETACH)
	{
		ofile.close();
		FreeLibrary(OGL);
	}
	return TRUE;
}