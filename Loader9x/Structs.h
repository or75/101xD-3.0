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

#ifndef _STRUCTS_H_
#define _STRUCTS_H_

// include defines
#include "Loader9x.h"
#include <string>

// struct which holds all the data of a single button
typedef struct
{
	int x;
	int y;
	int w;
	int h;
	func_t fCallback;
	int iIDResourceNormal;
	int iIDResourceHovered;
	int iIDResourcePressed;
	int iState;
	int iLastState;
} BUTTON_t;

// struct which holds all the data of a single check box
typedef struct
{
	int x;
	int y;
	int w;
	int h;
	int iIDResourceNormal;
	int iIDResourceHovered;
	int iIDResourcePressed;
	int iState;
	int iLastState;
	int iClickState;
} CHECKBOX_t;

// struct which holds all the data of a single radio box
typedef struct
{
	int x;
	int y;
	int w;
	int h;
	int iIDResourceNormal;
	int iIDResourcePressed;
	int iState;
	int iLastState;
	int iClickState;
} RADIOBOX_t;

typedef struct 
{
	int		x;
	int		y;
	std::string	StrText;
	COLORREF CRColor;
}STATICDLGTEXT_t;

#endif