// 
// Game-Deception Window Class
// Copyright (c) Patrick   2004	( P47RICK@programmer.net )
// Copyright (c) Stinger   2004	( stinger@gmx.net )
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
// 

// TODO: Add Support for an Edit and Textbox
#include "cWindow.h"
#include "Loader9x.h"


// -------------------------------------------------------------------------------------------

int cWindow::iCheckBoxState	( int iIndex )
{
	if (iIndex >= (int)vecCheckBox.size())
		return 0;

	return ((vecCheckBox[iIndex].iClickState == BUTTON_STATE_PRESSED) ? 1 : 0);
}

// -------------------------------------------------------------------------------------------

HWND cWindow::hGetWindowHandle ( void )
{
	return m_hWnd;
}

// -------------------------------------------------------------------------------------------

void cWindow::AddButton( int x, int y, int w, int h, func_t fCallback, int iIDResourceNormal, int iIDResourceHovered, int iIDResourcePressed, int iState)
{
	if ((!w || !h) && iIDResourceNormal)
	{
		HBITMAP hBitmap = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(iIDResourceNormal), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
		
		SIZE sImage;
		if (GetBitmapDimensionEx(hBitmap,&sImage))
		{
			w = sImage.cx;
			h = sImage.cy;
		}
		SECURECALL(DeleteObject(hBitmap))
	}
	
	LoadDlgBitmap(iIDResourceNormal,x,y);

	BUTTON_t TmpButton;
	TmpButton.x = x;
	TmpButton.y = y;
	TmpButton.w = w;
	TmpButton.h = h;
	TmpButton.fCallback = fCallback;
	TmpButton.iIDResourceNormal  = iIDResourceNormal;
	TmpButton.iIDResourceHovered = iIDResourceHovered;
	TmpButton.iIDResourcePressed = iIDResourcePressed;
	TmpButton.iState = iState;
	TmpButton.iLastState = iState;	// no redraw

	vecButton.push_back(TmpButton);
}

// -------------------------------------------------------------------------------------------

void cWindow::AddCheckBox ( int x, int y, int w, int h, int iChecked, int iIDResourceNormal, int iIDResourceHovered,	int iIDResourcePressed,	int iState)
{
	if ((!w || !h) && iIDResourceNormal)
	{
		HBITMAP hBitmap = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(iIDResourceNormal), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);

		SIZE sImage;
		if (GetBitmapDimensionEx(hBitmap,&sImage))
		{
			w = sImage.cx;
			h = sImage.cy;
		}
		SECURECALL(DeleteObject(hBitmap))
	}

	LoadDlgBitmap(iIDResourceNormal,x,y);

	CHECKBOX_t TmpCB;
	TmpCB.x = x;
	TmpCB.y = y;
	TmpCB.w = w;
	TmpCB.h = h;
	TmpCB.iIDResourceNormal  = iIDResourceNormal;
	TmpCB.iIDResourceHovered = iIDResourceHovered;
	TmpCB.iIDResourcePressed = iIDResourcePressed;
	TmpCB.iState = iState;
	TmpCB.iLastState = iState;	// no redraw
	TmpCB.iClickState = iChecked;

	vecCheckBox.push_back(TmpCB);
}

// -------------------------------------------------------------------------------------------

HWND cWindow::hCreateWindow	(int x, int y, int w, int h, char* szCaption, int iBGImage, WndProc_t WndProc, HINSTANCE hInstance, int iIDIcon )
{
	m_WndClass.style		= CS_HREDRAW | CS_VREDRAW;
	m_WndClass.lpfnWndProc	= WndProc;
	m_WndClass.cbClsExtra	= 0;
	m_WndClass.cbWndExtra	= 0;
	m_WndClass.hInstance	= hInstance;
	m_WndClass.hIcon		= LoadIcon(hInstance, MAKEINTRESOURCE(iIDIcon));
	m_WndClass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	m_WndClass.hbrBackground= (HBRUSH)(NULL);
	m_WndClass.lpszMenuName = NULL;
	m_WndClass.lpszClassName= szCaption;

	RegisterClass(&m_WndClass);

	m_hInstance = hInstance;
	m_hWnd = CreateWindow(szCaption,szCaption,WS_POPUP, x, y, w, h, NULL, NULL, hInstance, NULL);
	SetWindowPos(m_hWnd, NULL, x, y, w, h, NULL);
	m_iWindowX		= x;
	m_iWindowY		= y;
	m_iWindowWidth	= w; 
	m_iWindowHeight	= h;
	m_iClickSpotX	= x+2;
	m_iClickSpotY	= y+2;
	m_iDragStyle	= DRAGSTYLE_NEW;
	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	m_iBackImg = iBGImage;
	UpdateWindow(m_hWnd);
	LoadDlgBitmap(iBGImage,0,0);
	return m_hWnd;
}

// -------------------------------------------------------------------------------------------

bool cWindow::bMessageHandler( HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	switch (nMessage) 
	{
		case WM_SETFOCUS:
		case WM_CREATE:
		case WM_PAINT:
		{
			OnPaint();
			return true;
		}
		case WM_MOUSEMOVE:
		{
			UINT nFlags;
			POINT cPoint;

			nFlags = (UINT)wParam;
			cPoint.x = LOWORD(lParam);
			cPoint.y = HIWORD(lParam);

			OnMouseMove(nFlags, cPoint);

			return true;
		}

		case WM_LBUTTONDOWN:
		{
			UINT nFlags;
			POINT cPoint;

			nFlags = (UINT)wParam;
			cPoint.x = LOWORD(lParam);
			cPoint.y = HIWORD(lParam);

			if (HIWORD(lParam) < 20 && nFlags == WM_MOUSELEAVE && m_iDragStyle == DRAGSTYLE_USUAL)
				SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, NULL);
			else
				OnLButtonDown(nFlags, cPoint);

			return true;
		}

		case WM_LBUTTONUP:
		{
			UINT nFlags = (UINT)wParam;
			POINT cPoint;

			cPoint.x = LOWORD(lParam);
			cPoint.y = HIWORD(lParam);

			OnLButtonUp(nFlags, cPoint);

			return true;
		}
		default:
			return false;
	}
	return false;	// stfu compiler
}

// -------------------------------------------------------------------------------------------

void cWindow::LoadDlgBitmap( int iIDResource, int x, int y )
{
	HBITMAP hBitmap;
	hBitmap = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(iIDResource), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);

	if (hBitmap == INVALID_HANDLE_VALUE)
		return;

	BITMAP Bitmap; 

	SECURECALL(GetObject(hBitmap, sizeof(Bitmap), &Bitmap))

	HDC hDC = GetDC(m_hWnd); 
	HDC hBitmapDC = CreateCompatibleDC(hDC); 

	SECURECALL(SelectObject(hBitmapDC, hBitmap))
	SECURECALL(BitBlt(hDC, x, y, Bitmap.bmWidth, Bitmap.bmHeight, hBitmapDC, 0, 0, SRCCOPY)) 

	SECURECALL(DeleteObject(hBitmap))
	SECURECALL(ReleaseDC(m_hWnd, hBitmapDC))
	SECURECALL(ReleaseDC(m_hWnd, hDC))
}

// -------------------------------------------------------------------------------------------

void cWindow::LoadBtnBitmap( BUTTON_t* button )
{
	int x = button->x;
	int y = button->y;
	int iIDResource = 0;
	HBITMAP hBitmap;

	if (button->iLastState == button->iState)
		return;

	switch(button->iState)
	{
		case BUTTON_STATE_NORMAL:
			iIDResource = button->iIDResourceNormal;
			break;

		case BUTTON_STATE_HOVERED:
			iIDResource = button->iIDResourceHovered;
			break;

		case BUTTON_STATE_PRESSED:
			iIDResource = button->iIDResourcePressed;
			break;

		default:
			iIDResource = button->iIDResourceNormal;
			break;
	}

	button->iLastState = button->iState;

	hBitmap = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(iIDResource), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);

	if (hBitmap == INVALID_HANDLE_VALUE)
		return;

	BITMAP Bitmap; 

	SECURECALL(GetObject(hBitmap, sizeof(Bitmap), &Bitmap))

	HDC hDC = GetDC(m_hWnd); 
	HDC hBitmapDC = CreateCompatibleDC(hDC); 

	SECURECALL(SelectObject(hBitmapDC, hBitmap))
	SECURECALL(BitBlt(hDC, x, y, Bitmap.bmWidth, Bitmap.bmHeight, hBitmapDC, 0, 0, SRCCOPY)) 

	SECURECALL(DeleteObject(hBitmap))
	SECURECALL(ReleaseDC(m_hWnd, hBitmapDC))
	SECURECALL(ReleaseDC(m_hWnd, hDC))
}

// -------------------------------------------------------------------------------------------

void cWindow::LoadCHKBitmap( CHECKBOX_t* button )
{
	int x = button->x;
	int y = button->y;
	int iIDResource = 0;
	HBITMAP hBitmap;

	if (button->iLastState == button->iState)
		return;

	switch(button->iState)
	{
	case BUTTON_STATE_NORMAL:
		iIDResource = button->iIDResourceNormal;
		break;

	case BUTTON_STATE_HOVERED:
		iIDResource = button->iIDResourceHovered;
		break;

	case BUTTON_STATE_PRESSED:
		iIDResource = button->iIDResourcePressed;
		break;

	default:
		iIDResource = button->iIDResourceNormal;
		break;
	}

	button->iLastState = button->iState;

	hBitmap = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(iIDResource), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);

	if (hBitmap == INVALID_HANDLE_VALUE)
		return;

	BITMAP Bitmap; 

	SECURECALL(GetObject(hBitmap, sizeof(Bitmap), &Bitmap))

	HDC hDC = GetDC(m_hWnd); 
	HDC hBitmapDC = CreateCompatibleDC(hDC); 

	SECURECALL(SelectObject(hBitmapDC, hBitmap))
	SECURECALL(BitBlt(hDC, x, y, Bitmap.bmWidth, Bitmap.bmHeight, hBitmapDC, 0, 0, SRCCOPY)) 

	SECURECALL(DeleteObject(hBitmap))
	SECURECALL(ReleaseDC(m_hWnd, hBitmapDC))
	SECURECALL(ReleaseDC(m_hWnd, hDC))
}

// -------------------------------------------------------------------------------------------

void cWindow::RedrawRadio( RADIOBOX_t* button, bool bForce )
{
	int x = button->x;
	int y = button->y;
	int iIDResource = 0;
	HBITMAP hBitmap;

	if (button->iLastState == button->iState && !bForce)
		return;

	switch(button->iState)
	{
	case BUTTON_STATE_NORMAL:
		iIDResource = button->iIDResourceNormal;
		break;

	case BUTTON_STATE_PRESSED:
		iIDResource = button->iIDResourcePressed;
		break;

	default:
		iIDResource = button->iIDResourceNormal;
		break;
	}

	button->iLastState = button->iState;

	hBitmap = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(iIDResource), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);

	if (hBitmap == INVALID_HANDLE_VALUE)
		return;

	BITMAP Bitmap; 

	SECURECALL(GetObject(hBitmap, sizeof(Bitmap), &Bitmap))

	HDC hDC = GetDC(m_hWnd); 
	HDC hBitmapDC = CreateCompatibleDC(hDC); 

	SECURECALL(SelectObject(hBitmapDC, hBitmap))
	SECURECALL(BitBlt(hDC, x, y, Bitmap.bmWidth, Bitmap.bmHeight, hBitmapDC, 0, 0, SRCCOPY)) 

	SECURECALL(DeleteObject(hBitmap))
	SECURECALL(ReleaseDC(m_hWnd, hBitmapDC))
	SECURECALL(ReleaseDC(m_hWnd, hDC))
}

// -------------------------------------------------------------------------------------------

void cWindow::OnPaint	  ( void )
{
	LoadDlgBitmap(m_iBackImg,0,0);	
	
	int iSize = (int) vecButton.size();

	for (int i = 0;i < iSize;i++)
	{
		vecButton[i].iState = BUTTON_STATE_NORMAL;
		//LoadBtnBitmap(&vecButton[i]);
		LoadDlgBitmap(vecButton[i].iIDResourceNormal,vecButton[i].x,vecButton[i].y);
	}

	iSize = (int) vecCheckBox.size();

	for (int i = 0;i < iSize;i++)
	{
		switch(vecCheckBox[i].iClickState)
		{
		case BUTTON_STATE_PRESSED:
			LoadDlgBitmap(vecCheckBox[i].iIDResourcePressed,vecCheckBox[i].x,vecCheckBox[i].y);
			break;
		default:
			LoadDlgBitmap(vecCheckBox[i].iIDResourceNormal,vecCheckBox[i].x,vecCheckBox[i].y);
			break;
		}
	}

	iSize = (int) vecDLGText.size();

	for ( int i = 0;i < iSize; i++ )
		DisplayText((char*)vecDLGText[i].StrText.c_str(),vecDLGText[i].x,vecDLGText[i].y,vecDLGText[i].CRColor);

	iSize = (int) vecRadioBox.size();

	for (int i = 0;i < iSize;i++)
	{
		for (int j = 0;j < (int) vecRadioBox[i].m_vecRadioBox.size();j++)
			RedrawRadio(&vecRadioBox[i].m_vecRadioBox[j],true);
	}
}

// -------------------------------------------------------------------------------------------

void cWindow::OnMouseMove  ( UINT nFlags, POINT cPoint )
{
	if(m_bDragging && m_iDragStyle == DRAGSTYLE_NEW)
	{
		int iWinXPos = cPoint.x + m_iWindowX + m_iClickSpotX;
		int iWinYPos = cPoint.y + m_iWindowY + m_iClickSpotY;

		iWinXPos = ((iWinXPos < 0)?0:iWinXPos);
		iWinYPos = ((iWinYPos < 0)?0:iWinYPos);

		iWinXPos = ((iWinXPos + m_iWindowWidth  > GetSystemMetrics(SM_CXSCREEN))? GetSystemMetrics(SM_CXSCREEN) - m_iWindowWidth:iWinXPos);
		iWinYPos = ((iWinYPos + m_iWindowHeight > GetSystemMetrics(SM_CYSCREEN) - TASKBARHEIGHT)? (GetSystemMetrics(SM_CYSCREEN) - m_iWindowHeight) - TASKBARHEIGHT :iWinYPos);

		SetWindowPos(m_hWnd,NULL,iWinXPos,iWinYPos,m_iWindowWidth,m_iWindowHeight,NULL);

		m_iWindowX = iWinXPos;
		m_iWindowY = iWinYPos;

		return;
	}
	else
	{
		if (m_iDragStyle == DRAGSTYLE_NEW)
		{
			POINT m_Point;

			SECURECALL(GetCursorPos(&m_Point))

			m_iClickSpotX = m_iWindowX - m_Point.x;
			m_iClickSpotY = m_iWindowY - m_Point.y;
		}
	}

	int iSize = (int) vecButton.size();

	for ( int i = 0; i < iSize; i++ )	// button handler ;)
	{
		if ( cPoint.x > vecButton[i].x && cPoint.x < vecButton[i].x + vecButton[i].w
		&&	 cPoint.y > vecButton[i].y && cPoint.y < vecButton[i].y + vecButton[i].h )
		{
			if ( nFlags == MK_LBUTTON && vecButton[i].iState != BUTTON_STATE_PRESSED )
			{
				vecButton[i].iState  = BUTTON_STATE_PRESSED;
				LoadBtnBitmap(&vecButton[i]); // update
			}
			else if ( vecButton[i].iState != BUTTON_STATE_HOVERED && nFlags != MK_LBUTTON )
			{
				vecButton[i].iState  = BUTTON_STATE_HOVERED;
				LoadBtnBitmap(&vecButton[i]); // update
			}
			return;
		}
		else
		{
			if ( vecButton[i].iState != BUTTON_STATE_NORMAL )
			{
				vecButton[i].iState  = BUTTON_STATE_NORMAL;
				LoadBtnBitmap(&vecButton[i]); // update
			}
		}
	}

	iSize = (int) vecCheckBox.size();

	for (int i = 0;i < iSize;i++)
	{
		if ( cPoint.x > vecCheckBox[i].x && cPoint.x < vecCheckBox[i].x + vecCheckBox[i].w
		&&	 cPoint.y > vecCheckBox[i].y && cPoint.y < vecCheckBox[i].y + vecCheckBox[i].h )
		{
			if ( vecCheckBox[i].iState != BUTTON_STATE_HOVERED && nFlags != MK_LBUTTON )
			{
				vecCheckBox[i].iState  = BUTTON_STATE_HOVERED;
				LoadCHKBitmap(&vecCheckBox[i]);
			}
			return;
		}
		else
		{
			if ( vecCheckBox[i].iState != vecCheckBox[i].iClickState )
			{
				vecCheckBox[i].iState  = vecCheckBox[i].iClickState;
				LoadCHKBitmap(&vecCheckBox[i]);
			}
		}
	}
}

// -------------------------------------------------------------------------------------------
void cWindow::AddStaticText		( char *szText, int x, int y, COLORREF cColor)
{
	STATICDLGTEXT_t DLGText;

	DLGText.x		=	x;
	DLGText.y		=	y;
	DLGText.CRColor	=	cColor;
	DLGText.StrText =	szText;

	vecDLGText.push_back(DLGText);

	SendMessage(m_hWnd,WM_PAINT,NULL,NULL);
}
// -------------------------------------------------------------------------------------------
void cWindow::OnLButtonDown( UINT nFlags, POINT cPoint )
{
	m_bDragging = true;

	SECURECALL(vecButton.size())
	int iSize = (int)vecButton.size();

	for ( int i = 0; i < iSize; i++ )	// button handler ;)
	{
		if ( cPoint.x > vecButton[i].x && cPoint.x < vecButton[i].x + vecButton[i].w
		&&	 cPoint.y > vecButton[i].y && cPoint.y < vecButton[i].y + vecButton[i].h )
		{
			if ( vecButton[i].iState != BUTTON_STATE_PRESSED )
			{
				vecButton[i].iState  = BUTTON_STATE_PRESSED;
				LoadBtnBitmap(&vecButton[i]); // update
				return;
			}
		}
	}
	iSize = (int) vecCheckBox.size();

	for ( int i = 0; i < iSize; i++ )	// button handler ;)
	{
		if ( cPoint.x > vecCheckBox[i].x && cPoint.x < vecCheckBox[i].x + vecCheckBox[i].w
		&&	 cPoint.y > vecCheckBox[i].y && cPoint.y < vecCheckBox[i].y + vecCheckBox[i].h )
		{
			if ( vecCheckBox[i].iClickState != BUTTON_STATE_PRESSED )
			{
				vecCheckBox[i].iState      = BUTTON_STATE_PRESSED;
				vecCheckBox[i].iClickState = BUTTON_STATE_PRESSED;
				LoadCHKBitmap(&vecCheckBox[i]);
			}
			else
			{
				vecCheckBox[i].iState      = BUTTON_STATE_NORMAL;
				vecCheckBox[i].iClickState = BUTTON_STATE_NORMAL;
				LoadCHKBitmap(&vecCheckBox[i]);
			}
			return;
		}
	}

	iSize = (int) vecRadioBox.size();
	for (int i = 0;i < iSize;i++)
	{
		for (int j = 0;j < (int) vecRadioBox[i].m_vecRadioBox.size();j++)
		{
			if ( cPoint.x > vecRadioBox[i].m_vecRadioBox[j].x && cPoint.x < vecRadioBox[i].m_vecRadioBox[j].x + vecRadioBox[i].m_vecRadioBox[j].w
			&&	 cPoint.y > vecRadioBox[i].m_vecRadioBox[j].y && cPoint.y < vecRadioBox[i].m_vecRadioBox[j].y + vecRadioBox[i].m_vecRadioBox[j].h )
			{
				FullRadioRedraw( nFlags, cPoint );
				return;
			}
		}
	}
}

// -------------------------------------------------------------------------------------------

void cWindow::AddRadioBox ( cRadioBox thisRadioBox )
{
	vecRadioBox.push_back(thisRadioBox);
}

// -------------------------------------------------------------------------------------------

void cWindow::FullRadioRedraw( UINT nFlags, POINT cPoint )
{
	int iSize = (int) vecRadioBox.size();

	for (int i = 0;i < iSize;i++)
	{
		for (int j = 0;j < (int) vecRadioBox[i].m_vecRadioBox.size();j++)
		{
			if ( cPoint.x > vecRadioBox[i].m_vecRadioBox[j].x && cPoint.x < vecRadioBox[i].m_vecRadioBox[j].x + vecRadioBox[i].m_vecRadioBox[j].w
			&&	 cPoint.y > vecRadioBox[i].m_vecRadioBox[j].y && cPoint.y < vecRadioBox[i].m_vecRadioBox[j].y + vecRadioBox[i].m_vecRadioBox[j].h )
			{
				vecRadioBox[i].m_vecRadioBox[j].iClickState = BUTTON_STATE_PRESSED;
				vecRadioBox[i].m_vecRadioBox[j].iState		= BUTTON_STATE_PRESSED;
			}
			else
			{
				vecRadioBox[i].m_vecRadioBox[j].iClickState = BUTTON_STATE_NORMAL;
				vecRadioBox[i].m_vecRadioBox[j].iState		= BUTTON_STATE_NORMAL;
			}
			RedrawRadio(&vecRadioBox[i].m_vecRadioBox[j],false);
		}
	}
}

// -------------------------------------------------------------------------------------------

void cWindow::GetDimensions	( RECT * pWindowDimensions )
{
	SECURECALL(pWindowDimensions)
	pWindowDimensions->left  = m_iWindowX; 
	pWindowDimensions->right = m_iWindowWidth;
	pWindowDimensions->top	 = m_iWindowY;
	pWindowDimensions->bottom= m_iWindowHeight;
}

// -------------------------------------------------------------------------------------------

void cWindow::SetDimensions ( RECT WindowDimensions )
{
	m_iWindowX = WindowDimensions.left; 
	m_iWindowWidth = WindowDimensions.right;
	m_iWindowY = WindowDimensions.top;
	m_iWindowHeight = WindowDimensions.bottom;
}

// -------------------------------------------------------------------------------------------

void cWindow::OnLButtonUp  ( UINT nFlags, POINT cPoint )
{
	m_bDragging = false;

	SECURECALL(vecButton.size())
	int iSize = (int)vecButton.size();

	for ( int i = 0; i < iSize; i++ )	// button handler ;)
	{
		if ( cPoint.x > vecButton[i].x && cPoint.x < vecButton[i].x + vecButton[i].w
		&&	 cPoint.y > vecButton[i].y && cPoint.y < vecButton[i].y + vecButton[i].h )
		{
			if ( vecButton[i].iState == BUTTON_STATE_PRESSED )
			{
				if (vecButton[i].fCallback)
					vecButton[i].fCallback();

				vecButton[i].iState  = BUTTON_STATE_HOVERED;
				LoadBtnBitmap(&vecButton[i]); // update
				return;
			}
		}
	}
}
// -------------------------------------------------------------------------------------------