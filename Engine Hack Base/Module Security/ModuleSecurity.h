#ifndef __MOD_SEC__
#define __MOD_SEC__

void HideModuleFromPEB(HINSTANCE hInstance);
void RemovePeHeader(DWORD ModuleBase);
bool DestroyModuleHeader(HMODULE hModule);

#endif