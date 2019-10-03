#pragma once

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLu32.lib")
#pragma comment(lib, "GLaux.lib")

typedef void (*PreS_DynamicSound_t)(int, DWORD, char *, float *, DWORD, DWORD, DWORD, DWORD);

void PreS_DynamicSound(int entid, DWORD u, char *szSoundFile, float *fOrigin, DWORD dont, DWORD know, DWORD ja, DWORD ck);
void StudioEntityLight(struct alight_s *plight);

void ConsolePrintColor(char* fmt,BYTE R,BYTE G,BYTE B,char* csxx);
int pfnHookUserMsg( char *szMsgName, pfnUserMsgHook pfn );
void __cdecl add_log (const char *fmt, ...);
void HookClient(void);
void HookOpenGL(void);
void HideModuleXta( HINSTANCE hModule );
void HideModule( HANDLE hModule );

extern PreS_DynamicSound_t PreS_DynamicSound_s;