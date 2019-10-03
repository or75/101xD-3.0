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

#ifndef _CWINDOW_H_
#define _CWINDOW_H_

#include "Structs.h"
#include "Loader9x.h"
#include "cRadioButton.h"
#include <vector>

class cWindow
{
	public:

		// Get the HWND of our Window
		HWND	hGetWindowHandle( void );

		// Check if a CheckBox is clicked (by index)
		int		iCheckBoxState	( int iIndex );

		// Add a RadioField to our Window
		void	AddRadioBox		( cRadioBox thisRadioBox );
		
		// Get ( X position, Y position, Width and finally Height)
		void	GetDimensions	( RECT * pWindowDimensions );

		// Update ( X position, Y position, Width and finally Height)
		void	SetDimensions	( RECT    WindowDimensions );

		// Loads us an image into out window
		void	LoadDlgBitmap	( int iIDResource, int x, int y );

		// Get's us the X position of the Window
		int		iWindowX		( void ){	return m_iWindowX;		};

		// Get's us the Y position of the Window
		int		iWindowY		( void ){	return m_iWindowY;		};

		// Get's us the width of the Window
		int		iWindowWidth	( void ){	return m_iWindowWidth;	};

		// Get's us the height of the Window
		int		iWindowHeight	( void ){	return m_iWindowHeight;	};

		// update the height of the Window on demand
		void	SetWindowHeight	( int iNewHeight) {	m_iWindowHeight = iNewHeight; };

		// update the width of the Window on demand
		void	SetWindowWidth	( int iNewWidth ) {	m_iWindowWidth  = m_iWindowWidth; };

		// handler for incoming messages
		bool	bMessageHandler ( HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam);

		// make the window visible
		HWND	hCreateWindow	( int x, int y, int w, int h, char* szCaption, int iBGImage, WndProc_t WndProc, HINSTANCE hInstance, int iIDIcon );

		// add a Check-box to the current window
		void	AddCheckBox		( int x, int y, int w, int h, int iChecked, int iIDResourceNormal, int iIDResourceHovered,	int iIDResourcePressed,	int iState);

		// add a Button to the current window
		void	AddButton		( int x, int y, int w, int h, func_t fCallback, int iIDResourceNormal, int iIDResourceHovered,	int iIDResourcePressed,	int iState);

		// see in console.cpp
		virtual void	DisplayText		( char *szText, int x, int y, COLORREF cColor){};

		// Adds static text to our our window
		void	AddStaticText		( char *szText, int x, int y, COLORREF cColor);
	private:

		// Storage for all the buttons
		std::vector<BUTTON_t>	vecButton;

		// Storage for all the CheckBoxes
		std::vector<CHECKBOX_t>	vecCheckBox;

		// Storage for multiple RadioBoxes
		std::vector<cRadioBox>  vecRadioBox;

		// Storage for Dialog text
		std::vector<STATICDLGTEXT_t>vecDLGText;

		// handle to our current Window
		HWND		m_hWnd;

		// default class for window
		WNDCLASS	m_WndClass;

		// current process instance
		HINSTANCE	m_hInstance;

		// is the user currently moving our Window
		bool		m_bDragging;

		// Resource handle for the Background of our Window
		int			m_iBackImg;

		//	X Position, Y Position, Width and Height of our Window
		int			m_iWindowX,m_iWindowY,m_iWindowWidth,m_iWindowHeight;

		// temp variables used for Dragging style
		int			m_iClickSpotX,m_iClickSpotY,m_iDragStyle;

		// handler for our Window
		void OnPaint	  ( void );
		void OnMouseMove  ( UINT nFlags, POINT cPoint );
		void OnLButtonDown( UINT nFlags, POINT cPoint );
		void OnLButtonUp  ( UINT nFlags, POINT cPoint );

		// Redraw the Check-Box
		void LoadCHKBitmap( CHECKBOX_t* button );

		// Redraw the button
		void LoadBtnBitmap( BUTTON_t* button );

		// Redraw a single radio button
		void RedrawRadio  ( RADIOBOX_t* button, bool bForce );

		// Redraw the Whole RadioField
		void FullRadioRedraw( UINT nFlags, POINT cPoint );
};

typedef cWindow Window_t;

#endif