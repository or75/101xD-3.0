#include "font.h"

#pragma warning( disable:4996 )

CGLFont::CGLFont(void){}

CGLFont::~CGLFont()
{
	Delete();
}

void CGLFont::Init(const char *ccFontName, int iSize, bool bBold, bool bItalic, bool bUnderline, bool bStrikeout)
{
	p_hCurrentDC = wglGetCurrentDC();

	p_iBase = glGenLists(NUM_GLYPHS);
	HFONT hFont = CreateFont(
		iSize,
		0,
		0,
		0,
		(bBold) ? FW_BOLD : FW_NORMAL,
		(bItalic) ? 1 : 0,
		(bUnderline) ? 1 : 0,
		(bStrikeout) ? 1 : 0,
		ANSI_CHARSET,
		OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY,
		FF_DONTCARE | DEFAULT_PITCH,
		ccFontName);

	if (!hFont) return;

	SelectObject(p_hCurrentDC, hFont);
	wglUseFontBitmaps(p_hCurrentDC, 0, NUM_GLYPHS, p_iBase);

	for (int i = 0; i < NUM_GLYPHS; i++)
	{
		SIZE s;
		char temp[2] = { '\0', '\0' };

		temp[0] = (char)i;
		GetTextExtentPoint32(p_hCurrentDC, temp, 1, &s);
		p_cWidth[i] = (char)s.cx;
		p_cHeight = (char)s.cy;
	}

	DeleteObject(hFont);
}

void CGLFont::Print(int x, int y, color_s *color, ALIGNMENT align, char *cText, ...)
{
	if (!p_iBase || !cText)
		return;

	TCHAR cBuffer[1024];
	va_list args;
	va_start(args, cText);
	vsprintf(cBuffer, cText, args);
	va_end(args);

	if (align != VA_LEFT)
	{
		int iLen = TextLen(cBuffer);
		if (align == VA_CENTER) x -= iLen / 2;
		else x -= iLen;
	}
	y += (p_cHeight / 2) + 3;

	GLfloat curcolor[4], position[4];

	glPushAttrib(GL_LIST_BIT);
		glGetFloatv(GL_CURRENT_COLOR, curcolor);
		glGetFloatv(GL_CURRENT_RASTER_POSITION, position);
		glDisable(GL_TEXTURE_2D);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4ub(color->r, color->g, color->b, color->a);
		glRasterPos2i(x, y);

		glListBase(p_iBase);
		glCallLists((GLsizei)strlen(cBuffer), GL_UNSIGNED_BYTE, cBuffer);
	glPopAttrib();

	glColor4fv(curcolor);
	glRasterPos2f(position[0], position[1]);
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
}

void CGLFont::WrappedPrint(int x, int y, int w, int h, color_s *color, char *cText, ...)
{
	if (!p_iBase || !cText)
		return;

	TCHAR cBuffer[2048];
	va_list args;
	va_start(args, cText);
	vsprintf(cBuffer, cText, args);
	va_end(args);

	/*
	linebreak erfolgt nur wenn
	\n
	if (iCurrentTextSize >= w)
	*/

	size_t sSize = strlen(cBuffer);
	size_t sGap = 0;
	int tempy = y;
	for (size_t i = 0; i < sSize; i++)
	{
		if (cBuffer[i] == '\n')
		{
			char cTemp[1024];
			strcpy(cTemp, &cBuffer[sGap]);	// backup text from last gap to current position!
//			strcpy(cBuffer, cTemp);
			cTemp[(i - sGap)] = '\0';
			Print(x, tempy, color, VA_LEFT, cTemp);
			sGap = (i + 1);

			tempy += (int)p_cHeight;
			if ((tempy + (int)p_cHeight) > (y + h))
				break;
		}
	}

	Print(x, tempy, color, VA_LEFT, &cBuffer[sGap]);

	/*
	int nLineBreak = 0;
	size_t sLineBreakPosition[24] = { 0 };

	size_t nNewWord = 0;
	char cTemp[1024];
	size_t sSize = strlen(cBuffer);

	int iCurrentTextSize = 0;
	for (size_t i = 0; i < sSize; i++)
	{
		iCurrentTextSize += (int)p_cWidth[cBuffer[i]];

		if (cBuffer[i] == '\n')
		{
			sLineBreakPosition[nLineBreak] = i;
			cBuffer[i] = '\0';
			iCurrentTextSize = 0;
			nLineBreak++;
		}
		else
		{
			if (iCurrentTextSize >= w)
			{
				sLineBreakPosition[nLineBreak] = nNewWord;
				cBuffer[nNewWord] = '\0';
				iCurrentTextSize = 0;
				nLineBreak++;
			}

			if (cBuffer[i] == ' ')
			{
				nNewWord = i;
			}
		}
	}

	int tempy = y;
	char *cPointer = cBuffer;
	for (size_t i = 0; i < 24; i++)
	{
		if (sLineBreakPosition[i] == 0)
			break;

		strcpy(cTemp, cPointer);
		cPointer = (char *)((DWORD)cBuffer + sLineBreakPosition[i] + 1);

		Print(x, tempy, color, VA_LEFT, cTemp);

		tempy += (int)p_cHeight;
		if ((tempy + (int)p_cHeight) > (y + h))
			break;
	}

	Print(x, tempy, color, VA_LEFT, cPointer);
	*/
}

int CGLFont::TextLen(const char *ccText)
{
	int Return = 0;
	for (int i = 0; ccText[i]; i++)
	{
		if (ccText[i] < NUM_GLYPHS)
			Return += p_cWidth[ccText[i]];
	}

	return Return;
}

int CGLFont::WrappedTextHeight(int x, int y, int w, char *cText, ...)
{
	if (!p_iBase || !cText)
		return -1;

	TCHAR cBuffer[1024];
	va_list args;
	va_start(args, cText);
	vsprintf(cBuffer, cText, args);
	va_end(args);

	int iHeight = (int)p_cHeight;

	size_t nNewWord = 0;
	size_t sSize = strlen(cBuffer);

	int iCurrentTextSize = 0;
	for (size_t i = 0; i < sSize; i++)
	{
		iCurrentTextSize += (int)p_cWidth[cBuffer[i]];

		if (cBuffer[i] == '\n')
		{
			iHeight += (int)p_cHeight;
			iCurrentTextSize = 0;
		}
		else
		{
			if (iCurrentTextSize >= w)
			{
				iHeight += (int)p_cHeight;
				iCurrentTextSize = 0;
			}

			if (cBuffer[i] == ' ')
			{
				nNewWord = i;
			}
		}
	}

	return iHeight;
}

void CGLFont::Delete(void)
{
	glDeleteLists(p_iBase, NUM_GLYPHS);
}
