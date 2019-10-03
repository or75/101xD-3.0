#ifndef __DRAWING__
#define __DRAWING__

#include <Windows.h>
#include <gl/GL.h>
#include "../SDKIncludes.h"

void __fastcall DrawingDrawCircle(int x, int y, int r, bool solid);
void __fastcall DrawingDrawLine(int xs, int ys, int xe, int ye);
void __fastcall DrawingDrawRectangle(int x, int y, int w, int h, bool solid);
void __fastcall DrawingSetLineWidth(float w);
void __fastcall DrawingSetColor(int r, int g, int b, int a);
void DrawingBegin(void);
void inline DrawingEnd(void);

class CDrawing
{
public:
	void FillArea( int x, int y, int w, int h, int r, int g, int b, int a );
	void DrawBox( int x, int y, int w, int h, int linewidth, int r, int g, int b, int a );
	int iStringLen( const char *fmt, ... );
	int iStringHeight( void );
	void DrawString( int x, int y, int r, int g, int b, const char *fmt, ... );
	void DrawStringCenter( int x, int y, int r, int g, int b, const char *fmt, ... );
	void glFillRGBA(int x, int y, int w, int h, BYTE r, BYTE g, BYTE b, BYTE a);
private:
};

extern CDrawing g_Drawing;

#endif