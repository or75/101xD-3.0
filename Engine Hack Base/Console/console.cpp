#include <windows.h>
#include "console.h"
#include "../cFont/colors.h"
#include "../Drawing/Drawing.h"

bool steam_hwnd;

#pragma warning( disable:4996 )

CGLFont g_cVerdana16;
CGLFont g_cVerdana12;

CConsole *g_pConsole = NULL;

static int maxchar = 45;
static int charcount = 0;

CConsole::CConsole(void)
{
	g_pConsole = this;
	p_bActive = false;
	p_hWnd = FindProcessWindow(GetCurrentProcessId(), "Valve001");
	if(p_hWnd == 0)
	{
		p_hWnd = FindProcessWindow(GetCurrentProcessId(), "SDL_app"); // For Update Steam By _or_75
		if(p_hWnd != 0)
			steam_hwnd = true;
	}
	else
		steam_hwnd = false;
	p_pOrigProc = (WNDPROC)GetWindowLongPtr(p_hWnd, GWLP_WNDPROC);
}

CConsole::~CConsole(){}

void CConsole::Init(void)
{
	g_cVerdana16.Init("Verdana", 16, true);
	g_cVerdana12.Init("Verdana", 12);
}

bool CConsole::Toggle(void)
{
	p_bActive = !p_bActive;
	if (p_bActive)
		OnOpen();
	else
		OnClose();
	return p_bActive;
}

bool CConsole::IsOpen(void)
{
	return p_bActive;
}

void CConsole::Printf(const char *cString, ...)
{
	TCHAR cBuffer[1024];
	va_list args;
	va_start(args, cString);
	vsprintf(cBuffer, cString, args);
	va_end(args);

	strcat(p_cConsoleBuffer, cBuffer);

	// determine maximum number of lines to be printed
	int nMaxCount = 19;
	for (int i = sizeof(p_cConsoleBuffer); i >= 0; i--)
	{
		if (p_cConsoleBuffer[i] == '\n')
		{
			nMaxCount--;
			if (nMaxCount <= 0)
			{
				char cTemp[2048];
				strcpy(cTemp, &p_cConsoleBuffer[(i + 1)]);

				memset(p_cConsoleBuffer, 0, sizeof(p_cConsoleBuffer));
				strcpy(p_cConsoleBuffer, cTemp);
				break;
			}
		}
	}
}

void CConsole::ClearInput()
{
	memset(p_cInputBuffer, 0, sizeof(p_cInputBuffer));
	charcount = 0;
}

void CConsole::Clear(void)
{
	memset(p_cConsoleBuffer, 0, sizeof(p_cConsoleBuffer));
}

void CConsole::AddFunction(const char *cName, CONSOLE_CALLBACK pCallback, bool bVisible)
{
	FUNCTION_CALLBACK sFunction;

	strncpy(sFunction.cCommand, cName, 256);
	sFunction.pCallback = pCallback;
	sFunction.bVisible = bVisible;

	p_vFunctions.push_back(sFunction);
}

void CConsole::HUD_Redraw(SCREENINFO *sScreen)
{
	if (!p_bActive)
		return;

	int x = 140;
	int y = 140;
	int w = 500;
	int h = 295;
	int b = 7;		// border

	// resize?
	if (w < 70)		w = 70;
	if (h < 100)	h = 100;

	DrawRect(x, y, w, h, 35, 35, 35, 200);											// background
	DrawRect((x + b), (x + b + 20), (w - 2*b), (h - 20 - 3*b - 20), 9, 9, 9, 180);	// main text
	DrawRect((x + b), (x + h - b - 20), (w - 2*b), 20, 9, 9, 9, 180);				// editbox

	// render caption
	g_cVerdana12.Print((x + b + 4), (x + 10), &color_orange, VA_LEFT, "Engine Hack Base, Count: %i",charcount);

	// render input text
	g_cVerdana12.Print((x + b + 5), (x + h - b - 15), &color_orange, VA_LEFT, p_cInputBuffer);

	// render main text
	g_cVerdana12.WrappedPrint((x + b + 5), (x + b + 23), (w - 2*b - 8), (h - 20 - 3*b - 33), &color_orange, p_cConsoleBuffer);

	// render orange |
	if ((int)(GetTickCount() / 400) % 2)
		g_Drawing.glFillRGBA((x + b + 5 + g_cVerdana12.TextLen(p_cInputBuffer)), (x + h - b - 15), 1, 11, 136, 68, 0, 255);

	// render help
	if (p_cInputBuffer[1] != 0)
	{
		p_nHelpItems = 0;
		for (size_t i = 0; i < (size_t)p_vFunctions.size(); i++)
		{
			if (p_nHelpItems >= 6)
				break;

			if (!p_vFunctions[i].bVisible)
				continue;

			if (!strncmp(p_vFunctions[i].cCommand, p_cInputBuffer, strlen(p_cInputBuffer)))
				p_nHelpItems++;
		}

		if (p_nHelpItems > 0)
		{
			// help box
			DrawRect((x + b), (x + h + 2), 120, (3 + 16 * p_nHelpItems), 9, 9, 9, 200);

			if (p_nHelpSelection)
				g_Drawing.glFillRGBA((x + b + 1), (x + h + 3 + 16 * (p_nHelpSelection - 1)), 118, 17, 53, 53, 53, 170);

			p_nHelpItems = 0;
			for (size_t i = 0; i < (size_t)p_vFunctions.size(); i++)
			{
				if (p_nHelpItems >= 6)
					break;

				if (!p_vFunctions[i].bVisible)
					continue;

				if (!strncmp(p_vFunctions[i].cCommand, p_cInputBuffer, strlen(p_cInputBuffer)))
				{
					if (p_nHelpItems == (p_nHelpSelection - 1))
						strncpy(p_cCurrentHelp, p_vFunctions[i].cCommand, sizeof(p_cCurrentHelp));

					g_cVerdana12.Print((x + b + 5), (x + h - b + 13 + 16 * p_nHelpItems), &color_orange, VA_LEFT, p_vFunctions[i].cCommand);
					p_nHelpItems++;
				}
			}
		}
	}

	CheckKeys();
}

void CConsole::OnOpen(void)
{
	SetWindowLongPtr(p_hWnd, GWLP_WNDPROC, (LONG_PTR)MessageHandler);
	p_nHelpSelection = 0;
	for (int i = 0; i < 256; i++)
		p_bKeyboardOldBuffer[i] = false;
}

void CConsole::OnClose(void)
{
	SetWindowLongPtr(p_hWnd, GWLP_WNDPROC, (LONG_PTR)p_pOrigProc);
	ClearInput();
}

void CConsole::ProceedInput(void)
{
	if (p_cInputBuffer[0] == '\0')
		return;

	Printf("\n] %s", p_cInputBuffer);

	// remove all multiple whitespaces!
	size_t sWhite = 0;
	for (size_t i = 0; i < sizeof(p_cInputBuffer); i++)
	{
		if ((p_cInputBuffer[i] == ' ' || p_cInputBuffer[i] == 9) && sWhite == 0)
		{
			sWhite = i;
		}
		if (!(p_cInputBuffer[i] == ' ' || p_cInputBuffer[i] == 9) && sWhite != 0)
		{
			strcpy(&p_cInputBuffer[(sWhite + 1)], &p_cInputBuffer[i]);
			i = sWhite;
			sWhite = 0;
		}
	}

	// put all arguments into an array
	int argc = 1;
	for (int i = 1; i < sizeof(p_cInputBuffer); i++)
		if (p_cInputBuffer[i] != ' ' && p_cInputBuffer[(i - 1)] == ' ')
			argc++;

	char **argv = new char* [argc];

	int nArg = 0;
	int nLastGap = 0;
	for (int i = 0; i < sizeof(p_cInputBuffer); i++)
	{
		if (p_cInputBuffer[i] == ' ' || p_cInputBuffer[i] == '\0')
		{
			p_cInputBuffer[i] = '\0';
			argv[nArg] = &p_cInputBuffer[nLastGap];

			nLastGap = (i + 1);
			nArg++;

			if (p_cInputBuffer[(i + 1)] == '\0')
				break;
		}
	}

	// now proceed the fuck!
	for (size_t i = 0; i < (size_t)p_vFunctions.size(); i++)
	{
		if (!strcmpi(argv[0], p_vFunctions[i].cCommand))
			(*(p_vFunctions[i].pCallback))(argc, argv);
	}

	delete argv;
	ClearInput();
}
char CConsole::TranslateKeyInput(int nVirtKey)
{
	BYTE bKeyboardState[256];
	GetKeyboardState(bKeyboardState);

	WCHAR cReturn[2] = { 0 };
	ToUnicode(nVirtKey, MapVirtualKey(nVirtKey, 0), bKeyboardState, cReturn,2, 0);

	if (cReturn[0] == '%')
		cReturn[0] = ' ';

	return (char)cReturn[0];
}

void CConsole::CheckKeys(void)
{
	for (int i = 0x08; i < 223; i++)
	{
		//p_bKeyboardBuffer[i] = (HIWORD(GetKeyState(i)) != 0);
		if (!p_bKeyboardOldBuffer[i] && p_bKeyboardBuffer[i])
		{
			if (i == VK_RETURN)
			{
				if (p_nHelpSelection > 0)
				{
					strncpy(p_cInputBuffer, p_cCurrentHelp, sizeof(p_cCurrentHelp));
					charcount = strlen(p_cInputBuffer)-1;
					p_nHelpSelection = 0;
				}
				else
				{
					ProceedInput();
				}
			}
			else if (i == VK_BACK)
			{
				p_nHelpSelection = 0;
				p_cInputBuffer[(strlen(p_cInputBuffer) - 1)] = '\0';
				if(charcount > 0)
					charcount--;
			}
			else if (i == VK_DELETE)
			{
				p_nHelpSelection = 0;
				ClearInput();
			}
			else if (i == VK_TAB)
			{
				p_nHelpSelection++;
				if (p_nHelpSelection > p_nHelpItems)
					p_nHelpSelection = 1;
			}
			else if (i == VK_UP)
			{
				p_nHelpSelection--;
				if (p_nHelpSelection <= 0)
					p_nHelpSelection = p_nHelpItems;
			}
			else if (i == VK_DOWN)
			{
				p_nHelpSelection++;
				if (p_nHelpSelection > p_nHelpItems)
					p_nHelpSelection = 1;
			}
			else
			{
				p_nHelpSelection = 0;
				char cKey[2] = "";
				cKey[0] = TranslateKeyInput(i);
				if(charcount >= maxchar){goto mxch;}
				if (cKey != 0)
				{
					char tempBuff[1024];
					strcpy(tempBuff,p_cInputBuffer);
					strcat(p_cInputBuffer, cKey);
					if(strlen(tempBuff) != strlen(p_cInputBuffer))
						charcount++;
				}
			}
		}
		mxch:
		p_bKeyboardOldBuffer[i] = p_bKeyboardBuffer[i];
	}
}

LRESULT CALLBACK CConsole::MessageHandler(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	if (g_pConsole->p_bActive)
	{
		switch (wMsg)
		{
			case WM_KEYDOWN:
			{
				if(wParam == VK_INSERT)
				{
					g_pConsole->Toggle();
				}
				if (wParam == VK_ESCAPE)
				{
					g_pConsole->Toggle();
				}
				else
				{
					g_pConsole->p_bKeyboardBuffer[wParam] = true;
					return NULL;
				}
			}
			break;

			case WM_KEYUP:
			{
				g_pConsole->p_bKeyboardBuffer[wParam] = false;
			}
			break;
		}
	}

	if (g_pConsole->p_pOrigProc)
		return CallWindowProc(g_pConsole->p_pOrigProc, hWnd, wMsg, wParam, lParam);
	else
		return DefDlgProc(hWnd, wMsg, wParam, lParam);
}

void CConsole::DrawRect(int x, int y, int w, int h, BYTE r, BYTE g, BYTE b, BYTE a)
{
	g_Drawing.glFillRGBA((x + 1), (y + 1), (w - 2), (h - 2), r, g, b, a);		// background
	g_Drawing.glFillRGBA(x, y, w, 1, 105, 105, 105, 255);						// top outline
	g_Drawing.glFillRGBA(x, y, 1, h, 105, 105, 105, 255);						// left outline
	g_Drawing.glFillRGBA((x + w - 1), y, 1, h, 105, 105, 105, 255);			// right outline
	g_Drawing.glFillRGBA(x, (y + h - 1), w, 1, 105, 105, 105, 255);			// bottom outline
}

static BOOL CALLBACK SearchWindow(HWND hWnd, LPARAM lParam)
{
	PSEARCH_WINDOW pSearchWindow;
	DWORD dwWindowProcessId;

	if (!(pSearchWindow = (PSEARCH_WINDOW)lParam))
		return FALSE;

	GetWindowThreadProcessId(hWnd, &dwWindowProcessId);
	if (pSearchWindow->dwProcessId == dwWindowProcessId)
	{
		char cClassName[MAX_PATH];
		if (!GetClassName(hWnd, cClassName, sizeof(cClassName)))
			return TRUE;

		if (!strcmpi(pSearchWindow->cClassName, cClassName))
		{
			pSearchWindow->hWnd = hWnd;
			return FALSE;
		}
	}
	return TRUE;
}

HWND FindProcessWindow(DWORD dwProcessId, const char *cClassName)
{
	SEARCH_WINDOW sSearchWindow;
	sSearchWindow.hWnd = 0;
	memset(&sSearchWindow, 0, sizeof(sSearchWindow));
	sSearchWindow.dwProcessId = dwProcessId;
	strcpy(sSearchWindow.cClassName, cClassName);
	EnumWindows(SearchWindow, (LPARAM)&sSearchWindow);
	return sSearchWindow.hWnd;
}