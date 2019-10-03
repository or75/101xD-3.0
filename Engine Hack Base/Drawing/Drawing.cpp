#include <windows.h>
#include <math.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "Drawing.h"
#include "../SDKIncludes.h"

#pragma warning( disable:4996 )

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLu32.lib")
#pragma comment(lib, "GLaux.lib")

void DrawingBegin(void)
{
	glPushMatrix(); 
	glLoadIdentity();  
	glDisable(GL_TEXTURE_2D); 
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
}
void DrawingEnd(void)
{
	glDisable(GL_BLEND); 
	glEnable(GL_TEXTURE_2D); 
	glPopMatrix();
}
void __fastcall DrawingSetColor(int r, int g, int b)
{
	glColor3ub(r, g, b);
}
void __fastcall DrawingSetColor(int r, int g, int b, int a)
{
	glColor4ub(r, g, b, a);
}
void __fastcall DrawingSetLineWidth(float w)
{
	glLineWidth(1.0f);
}
void __fastcall DrawingDrawRectangle(int x, int y, int w, int h, bool solid)
{
	if(!solid)
		glBegin(GL_LINE_LOOP);
	else
		glBegin(GL_QUADS);

	glVertex2i(x-w/2, y-h/2);
	glVertex2i(x+w/2+1, y-h/2);
	glVertex2i(x+w/2, y+h/2);
	glVertex2i(x-w/2, y+h/2);

	glEnd();
}
void __fastcall DrawingDrawLine(int xs, int ys, int xe, int ye)
{
	glBegin(GL_LINES);
	glVertex2i(xs, ys);
	glVertex2i(xe, ye);
	glEnd();
}
void __fastcall DrawingDrawCircle(int x, int y, int r, bool solid)
{
	float vectorX, vectorY1 = (float)y + r;
	float vectorY, vectorX1 = (float)x;
	glBegin(GL_LINE_STRIP);	
	for(float angle = 0.0f; angle <= (2.0f*3.14159); angle += 0.01f)
	{		
		vectorX = (float)x + (float)r * sin(angle);
		vectorY = (float)y + (float)r * cos(angle);		
		glVertex2f(vectorX1, vectorY1);
		vectorY1 = vectorY;
		vectorX1 = vectorX;			
	}
	glEnd();
}

void DrawingDrawText(char* Text, int x, int y, int r, int g, int b, int a, char* FontName, int w, int h, bool Centered)
{
	HDC hDC = wglGetCurrentDC();
	GLuint Lists = glGenLists(256);
	wglUseFontBitmaps(hDC, 0, 256, Lists);

	HFONT hFont = CreateFont(w, 0, 0, 0, h, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_DONTCARE | DEFAULT_PITCH, FontName);
	HFONT hOldFont = (HFONT) SelectObject(hDC, hFont);

	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	SIZE s;
	char Sizes[256];
	for(int i=0; i < 256; i++)
	{
		char temp[2] = {i, '\0'};
		GetTextExtentPoint32(hDC, temp, 1, &s);
		Sizes[i] = (char)s.cx;
	}

	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);

	GLfloat curcolor[4], position[4];
	glPushAttrib(GL_LIST_BIT);
	glGetFloatv(GL_CURRENT_COLOR, curcolor);
	glGetFloatv(GL_CURRENT_RASTER_POSITION, position);
	glDisable(GL_TEXTURE_2D);

	glColor4ub(r, g, b, a);
	glRasterPos2i(x, y);

	glListBase(Lists);
	glCallLists(strlen(Text), GL_UNSIGNED_BYTE, Text);

	glPopAttrib();
	glColor4fv(curcolor);
	glRasterPos2f(position[0],position[1]);
	glEnable(GL_TEXTURE_2D);

	glDeleteLists(Lists, 256);
}

void CDrawing::FillArea( int x, int y, int w, int h, int r, int g, int b, int a )
{
	g_Engine.pfnFillRGBA( x, y, w, h, r, g, b, a );
}

void CDrawing::DrawBox( int x, int y, int w, int h, int linewidth, int r, int g, int b, int a )
{
	//        X						Y					W					H				R  G  B  A
	FillArea( x,					y,					w,					linewidth,		r, g, b, a ); // top
	FillArea( x + w - linewidth,	y + linewidth,		linewidth,			h - linewidth,	r, g, b, a ); // right
	FillArea( x,					y + linewidth,		linewidth,			h - linewidth,	r, g, b, a ); // left
	FillArea( x + linewidth,		y + h - linewidth,	w - linewidth * 2,	linewidth,		r, g, b, a ); // bottom
}

int CDrawing::iStringLen( const char *fmt, ... )
{
	va_list va_alist;
	char buf[256];
	va_start( va_alist, fmt );
	_vsnprintf( buf, sizeof( buf ), fmt, va_alist );
	va_end( va_alist );
	int iWidth, iHeight;
	g_Engine.pfnDrawConsoleStringLen( buf, &iWidth, &iHeight );
	return iWidth;
}

int CDrawing::iStringHeight( void )
{
	int iWidth, iHeight;
	g_Engine.pfnDrawConsoleStringLen( "F", &iWidth, &iHeight );
	return iHeight;
}

void CDrawing::DrawString( int x, int y, int r, int g, int b, const char *fmt, ... )
{
	va_list va_alist;
	char buf[256];
	va_start( va_alist, fmt );
	_vsnprintf( buf, sizeof( buf ), fmt, va_alist );
	va_end( va_alist );
	g_Engine.pfnDrawSetTextColor( (float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f );
	g_Engine.pfnDrawConsoleString( x, y, buf );
}

void CDrawing::DrawStringCenter( int x, int y, int r, int g, int b, const char *fmt, ... )
{
	va_list va_alist;
	char buf[256];
	va_start( va_alist, fmt );
	_vsnprintf( buf, sizeof( buf ), fmt, va_alist );
	va_end( va_alist );
	int iWidth = iStringLen( "%s", buf );
	g_Engine.pfnDrawSetTextColor( (float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f );
	g_Engine.pfnDrawConsoleString( x - iWidth / 2, y, buf );
}

void CDrawing::glFillRGBA(int x, int y, int w, int h, BYTE r, BYTE g, BYTE b, BYTE a)
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4ub(r, g, b, a);
	glBegin(GL_QUADS);
		glVertex2i(x, y);
		glVertex2i(x + w, y);
		glVertex2i(x + w, y + h);
		glVertex2i(x, y + h);
	glEnd();
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
}

CDrawing g_Drawing;