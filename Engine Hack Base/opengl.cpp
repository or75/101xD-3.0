#include <Windows.h>
#include "cFont/cFont.h"
#include "SDKIncludes.h"
#include "TransInclude.h"
#include "cVars/cvars.h"

typedef void ( APIENTRY *glBegin_t )( GLenum );
typedef void ( APIENTRY *glEnd_t )( void );
typedef void ( APIENTRY *glVertex3fv_t )( const GLfloat *v );
typedef void ( APIENTRY *glClear_t )( GLbitfield );

glBegin_t pglBegin = NULL;
glEnd_t pglEnd = NULL;
glVertex3fv_t pglVertex3fv = NULL;
glClear_t pglClear = NULL;

GLenum g_glMode;

bool bWallFix = true;

void APIENTRY Hooked_glBegin( GLenum mode )
{
	g_glMode = mode;
	cl_entity_t *pEnt = g_Studio.GetCurrentEntity();
	GLboolean bGL_BLEND = glIsEnabled( GL_BLEND );
	GLboolean bGL_TEXTURE_2D = glIsEnabled( GL_TEXTURE_2D );
	static bool InitFont = false;

	if ( cvar.wallhack_xqz->value )
	{
		if (mode == GL_TRIANGLE_STRIP || mode == GL_TRIANGLE_FAN) 
			(*glDisable)(GL_DEPTH_TEST);
	}

	if( cvar.whitewalls->value )
	{
		if (mode != GL_TRIANGLES && mode != GL_TRIANGLE_STRIP  && mode != GL_TRIANGLE_FAN  && mode != GL_QUADS)
		{ 
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); 
		}
	}

	if(!InitFont)
	{
		g_Info.InitText();
		g_Arial.InitText();
		InitFont = true;
	}
	(*pglBegin)( mode );
}

void APIENTRY Hooked_glEnd( void )
{
	if( bWallFix )
	{
		bWallFix = false;
		glDisable( GL_BLEND );
		glEnable( GL_DEPTH_TEST );
		//glEnable( GL_TEXTURE_2D );
	}
	(*pglEnd)();
}

void APIENTRY Hooked_glVertex3fv( GLfloat *v )
{
	cl_entity_t *pLocal = g_Engine.GetLocalPlayer();
	(*pglVertex3fv)( v );
}

void APIENTRY Hooked_glClear( GLbitfield mask )
{
	(*pglClear)( mask );
}

void *DetourFunc( BYTE *src, const BYTE *dst, const int len )
{
	BYTE *jmp = (BYTE*)malloc( len + 5 );
	DWORD dwback;
	VirtualProtect( src, len, PAGE_READWRITE, &dwback );
	memcpy( jmp, src, len );
	jmp += len;
	jmp[0] = 0xE9;
	*(DWORD*)( jmp + 1 ) = (DWORD)( src + len - jmp ) - 5;
	src[0] = 0xE9;
	*(DWORD*)( src + 1 ) = (DWORD)( dst - src ) - 5;
	VirtualProtect( src, len, dwback, &dwback );
	return ( jmp - len );
}

void HookOpenGL( void )
{
	HMODULE hmOpenGL = GetModuleHandle( "opengl32.dll" );
	pglBegin = (glBegin_t)DetourFunc( (LPBYTE)GetProcAddress( hmOpenGL, "glBegin" ), (LPBYTE)&Hooked_glBegin, 6 );
	pglEnd = (glEnd_t)DetourFunc( (LPBYTE)GetProcAddress( hmOpenGL, "glEnd" ), (LPBYTE)&Hooked_glEnd, 6 );
	pglVertex3fv = (glVertex3fv_t)DetourFunc( (LPBYTE)GetProcAddress( hmOpenGL, "glVertex3fv" ), (LPBYTE)&Hooked_glVertex3fv, 6 );
	pglClear = (glClear_t)DetourFunc( (LPBYTE)GetProcAddress( hmOpenGL, "glClear" ), (LPBYTE)&Hooked_glClear, 7 );
}