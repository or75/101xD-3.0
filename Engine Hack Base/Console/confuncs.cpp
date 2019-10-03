#include "confuncs.h"
#include "../TransInclude.h"

CConsole g_cConsole;

//CON_CVAR(bhop);

CON_FUNCTION(cvarlist)
{
g_cConsole.Printf("\
\n----------------\n\
cvarlist\n\
clear\n\
quit\n\
----------------\n\
");
}

CON_FUNCTION(clear)
{
	g_cConsole.Clear();
	g_cConsole.Printf("console by _or_75\n----------------\n");
}

CON_FUNCTION(quit)
{
	g_Engine.pfnClientCmd("quit");
}

void InitConsole(void)
{
	g_cConsole.Init();
	cmd_clear(0, NULL);

	CON_REGISTER(cvarlist);
	CON_REGISTER(clear);
	CON_REGISTER(quit);
}
