#ifndef __USERMSG__
#define __USERMSG__

int hResetHUD(const char *pszName, int iSize, void *pbuf);
int hSetFOV(const char *pszName, int iSize, void *pbuf);
int hTeamInfo(const char *pszName, int iSize, void *pbuf);
int hNVGToggle(const char *pszName, int iSize, void *pbuf);
int hCurWeapon(const char *pszName, int iSize, void *pbuf);
int hDeathMsg(const char *pszName, int iSize, void *pbuf);

extern pfnUserMsgHook pResetHUD;
extern pfnUserMsgHook pSetFOV;
extern pfnUserMsgHook pTeamInfo;
extern pfnUserMsgHook pCurWeapon;
extern pfnUserMsgHook pDeathMsg;

#endif