#ifndef __FONT_H__
#define __FONT_H__

#include <stdio.h>
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "../glaux.h"
#include "colors.h"

#define NUM_GLYPHS 256

typedef enum _ALIGNMENT
{
	VA_LEFT,
	VA_CENTER,
	VA_RIGHT
}
ALIGNMENT, *PALIGNMENT;

class CGLFont
{
	public:
		CGLFont(void);
		~CGLFont();
		void	Init(const char *ccFontName, int iSize, bool bBold = false, bool bItalic = false, bool bUnderline = false, bool bStrikeout = false);
		void	Print(int x, int y, color_s *color, ALIGNMENT align, char *cText, ...);
		void	WrappedPrint(int x, int y, int w, int h, color_s *color, char *cText, ...);
		int		TextLen(const char *ccText);
		int		WrappedTextHeight(int x, int y, int w, char *cText, ...);
	private:
		void	Delete(void);
		HDC		p_hCurrentDC;
		GLuint	p_iBase;
		char	p_cWidth[NUM_GLYPHS];
		char	p_cHeight;
};

#endif
