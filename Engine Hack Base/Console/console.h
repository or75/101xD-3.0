#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <vector>
#include <windows.h>
#include "../cFont/font.h"
#include "../SDKIncludes.h"

extern bool steam_hwnd;

typedef void (*CONSOLE_CALLBACK)(int, char **);

typedef struct _FUNCTION_CALLBACK
{
	char				cCommand[256];
	CONSOLE_CALLBACK	pCallback;
	bool				bVisible;
}
FUNCTION_CALLBACK, *PFUNCTION_CALLBACK;

class CConsole
{
	public:
				CConsole(void);
				~CConsole();

		void	Init(void);
		bool	Toggle(void);
		bool	IsOpen(void);

		void	Printf(const char *cString, ...);
		void	Clear(void);
		void	ClearInput();

		void	AddFunction(const char *cName, CONSOLE_CALLBACK pCallback, bool bVisible = true);

		void	HUD_Redraw(SCREENINFO *sScreen);

	private:
		void	OnOpen(void);
		void	OnClose(void);

		void	ProceedInput(void);

		char	TranslateKeyInput(int nVirtKey);
		void	CheckKeys(void);

		static	LRESULT CALLBACK MessageHandler(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);

		void	DrawRect(int x, int y, int w, int h, BYTE r, BYTE g, BYTE b, BYTE a);

		bool	p_bActive;

		HWND	p_hWnd;
		WNDPROC	p_pOrigProc;

		bool	p_bKeyboardBuffer[256];
		bool	p_bKeyboardOldBuffer[256];

		char	p_cInputBuffer[1024];
		char	p_cConsoleBuffer[2048];

		std::vector<FUNCTION_CALLBACK> p_vFunctions;

		int		p_nHelpItems;
		int		p_nHelpSelection;
		char	p_cCurrentHelp[256];
};

typedef struct _SEARCH_WINDOW
{
	DWORD	dwProcessId;
	HWND	hWnd;
	char	cClassName[MAX_PATH];
}
SEARCH_WINDOW, *PSEARCH_WINDOW;

typedef struct _KEY_INFO
{
	bool	bPressed;
	DWORD	dwStartTime;
}
KEY_INFO, *PKEY_INFO;

static BOOL CALLBACK SearchWindow(HWND hWnd, LPARAM lParam);
HWND FindProcessWindow(DWORD dwProcessId, const char *cClassName);

#endif
