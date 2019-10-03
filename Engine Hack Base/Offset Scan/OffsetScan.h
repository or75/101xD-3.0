#ifndef __OFFSETSCAN__
#define __OFFSETSCAN__

#pragma once

class COffsets
{
public:
	BYTE HLType;
	DWORD ClBase, ClSize, ClEnd;
	DWORD HwBase, HwSize, HwEnd;
	DWORD VgBase, VgSize;
	
	bool Initialize(void);
	void *GameConsole(void);
	void *SpeedHackPtr(void);
	void *ClientFuncs(void);
	void *EngineFuncs(void);
	DWORD EngineStudio(void);
	void GameInfo(void);
	void *Sound(void);
	void Error(const char* Msg);
	DWORD GetModuleSize(const DWORD Address);
	DWORD FarProc(const DWORD Address, DWORD LB, DWORD HB);
	void ConsoleColorInitalize();
	DWORD ClientBase(void);
};

extern COffsets gOffsets;

#endif