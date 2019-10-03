#ifndef __CONFUNCS_H__
#define __CONFUNCS_H__

#include <windows.h>
#include "console.h"
#include "../cVars/cvars.h"

extern CConsole g_cConsole;

#define CON_FUNCTION(x) \
	void cmd_##x(int argc, char **argv)

#define CON_CVAR(x)																				\
	CON_FUNCTION(x)																				\
	{																							\
		if (argc >= 2) { cvar.##x = (float)atof(argv[1]); }					\
		float fValue = cvar.##x;								\
		if (fValue == (int)fValue) g_cConsole.Printf("\n%s = \"%d\"", TEXT(#x), (int)fValue);	\
		else g_cConsole.Printf("\n%s = \"%2.2f\"", TEXT(#x), fValue);							\
	}

#define CON_REGISTER(x) \
	g_cConsole.AddFunction(TEXT(#x), cmd_##x);

#define CON_REGISTER_SILENT(x) \
	g_cConsole.AddFunction(TEXT(#x), cmd_##x, false);

void InitConsole(void);

#endif
