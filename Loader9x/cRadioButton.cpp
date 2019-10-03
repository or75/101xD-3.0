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
/*
#include "cRadioButton.h"

void cRadioBox::SetEntry(int iIndex)
{
	int iSize = m_vecRadioBox.size();
	for (int i = 0;i < iSize;i++)
	{
		if (i == iIndex)
		{
			m_vecRadioBox[i].iClickState	= BUTTON_STATE_PRESSED;
			m_vecRadioBox[i].iState			= BUTTON_STATE_PRESSED;
		}
		else
		{
			m_vecRadioBox[i].iClickState	= BUTTON_STATE_NORMAL;
			m_vecRadioBox[i].iState			= BUTTON_STATE_NORMAL;
		}
		LoadCHKBitmap(&m_vecRadioBox[i]);
	}
};

bool cRadioBox::bIsEntry(int iIndex)
{
	int iSize = m_vecRadioBox.size();

	if (iIndex >= iSize)
		return false;

	return ((m_vecRadioBox[iIndex].iClickState == BUTTON_STATE_PRESSED) ? 1 : 0);
};


void cRadioBox::LoadRDOBitmap( RADIOBOX_t* button )
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
};

void cRadioBox::OnPaint ( void )
{
	iSize = (int) m_vecRadioBox.size();

	for (int i = 0;i < iSize;i++)
	{
		switch(m_vecRadioBox[i].iClickState)
		{
		case BUTTON_STATE_PRESSED:
			LoadDlgBitmap(m_vecRadioBox[i].iIDResourcePressed,m_vecRadioBox[i].x,m_vecRadioBox[i].y);
			break;
		default:
			LoadDlgBitmap(m_vecRadioBox[i].iIDResourceNormal, m_vecRadioBox[i].x,m_vecRadioBox[i].y);
			break;
		}
	}
}


void cRadioBox::OnLButtonDown( UINT nFlags, POINT cPoint )
{
	int iSize = (int) vecButton.size();

	for ( int i = 0; i < iSize; i++ )	// button handler ;)
	{
		if ( cPoint.x > vecButton[i].x && cPoint.x < vecButton[i].x + vecButton[i].w
		&&	 cPoint.y > vecButton[i].y && cPoint.y < vecButton[i].y + vecButton[i].h )
		{
			SetEntry(i);
			OnPaint();
			return;
		}
	}
}

void cRadioBox::LoadDlgBitmap( int iIDResource, int x, int y )
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

void cRadioBox::AddRadioButton ( int x, int y, int w, int h, int iChecked, int iIDResourceNormal,int iIDResourcePressed,	int iState)
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

	RADIOBOX_t TmpCB;
	TmpCB.x = x;
	TmpCB.y = y;
	TmpCB.w = w;
	TmpCB.h = h;
	TmpCB.iIDResourceNormal  = iIDResourceNormal;
	TmpCB.iIDResourcePressed = iIDResourcePressed;
	TmpCB.iState = iState;
	TmpCB.iLastState = iState;	// no redraw
	TmpCB.iClickState = iChecked;

	m_vecRadioBox.push_back(TmpCB);
}
*/