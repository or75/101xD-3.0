#include <windows.h>
#include "../SDKIncludes.h"
#include "../TransInclude.h"
#include "UserMsgs.h"
#include "../locals.h"

#pragma warning( disable:4996 )

int g_PlayerTeam[33];

int hResetHUD(const char *pszName, int iSize, void *pbuf)
{
	for(int i = 1; i<33; i++)
		g_Player[i].bAlive = false;
	return pResetHUD(pszName, iSize, pbuf);
}

int hSetFOV(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );
	g_Local.iFOV = READ_BYTE();
	if( !g_Local.iFOV ) { g_Local.iFOV = 90; }
	return (*pSetFOV)( pszName, iSize, pbuf );
}

int hTeamInfo(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );
	int iIndex = READ_BYTE();
	char *szTeam = READ_STRING();
	_strlwr( szTeam );
	if( !strcmp( szTeam, "terrorist" ) )
	{
		g_Player[iIndex].iTeam = 1;
		g_PlayerTeam[iIndex] = 1;
		if( iIndex == g_Local.iIndex ) { g_Local.iTeam = 1; }
	}
	else if( !strcmp( szTeam, "ct" ) )
	{
		g_Player[iIndex].iTeam = 2;
		g_PlayerTeam[iIndex] = 2;
		if( iIndex == g_Local.iIndex ) { g_Local.iTeam = 2; }
	}
	else
	{
		g_Player[iIndex].iTeam = 0;
		g_PlayerTeam[iIndex] = 0;
		if( iIndex == g_Local.iIndex ) { g_Local.iTeam = 0; }
	}
	return pTeamInfo(pszName, iSize, pbuf);
}

int hCurWeapon(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );
	int iState = READ_BYTE();
	int iWeaponID = READ_CHAR();
	int iClip = READ_CHAR();
	if( iState )
	{
		g_Local.iClip = iClip;
		g_Local.iWeaponID = iWeaponID;
	}
	return pCurWeapon(pszName, iSize, pbuf);
}

int hDeathMsg(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );
	int iKiller=READ_BYTE();
	int iVictim=READ_BYTE();
	int iHS=READ_BYTE();
	char *szWeapon=READ_STRING();
	g_Player[iVictim].bAlive = false;
	SoundClear(g_Player[iVictim].currentSound);
	SoundClear(g_Player[iVictim].previousSound);
	return pDeathMsg(pszName, iSize, pbuf);
}

pfnUserMsgHook pResetHUD;
pfnUserMsgHook pSetFOV;
pfnUserMsgHook pTeamInfo;
pfnUserMsgHook pCurWeapon;
pfnUserMsgHook pDeathMsg;