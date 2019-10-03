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

#ifndef _cCRC32_H
#define _cCRC32_H
// #define WIN32_LEAN_AND_MEAN // exclude rarely used stuff
#include <Windows.h>

typedef struct 
{
	DWORD dwResultHash1;
	DWORD dwResultHash2;
	DWORD dwResultHash3;
	DWORD dwResultHash4;
}CRC32HASH_t;// if we crc larger amounts of bytes crc32 is a bit inaccurate so here is my solution

class cCRC32
{
	/*
	public:
		 cCRC32();			// creates us the polynomial table
//		~cCRC32();			// not used at the moment
		DWORD			dwCRCString( const char* szString );				// gets us the hash of a string
		CRC32HASH_t*	CRCArray   ( const PBYTE pbArray, const int iSize );	// gets us the hash of an array, use the double size IF you happen to crc a 4byte element array
		CRC32HASH_t*	CRCModule  ( HMODULE hModule );				// gets us the extended hash of a PE Module

	private:
		DWORD m_dwCRCTable[256];
		*/
};

extern cCRC32 gCRC32;	// makes us the class available everywhere (b.t.w. dom1n1k is available too :P )

#endif