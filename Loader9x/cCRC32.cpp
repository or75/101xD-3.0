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

// credits: who ever made that algorithm up & OGC ;)
#include "cCRC32.h"

/*
cCRC32::cCRC32 ( )
{
	DWORD dwPolynomial = 0xEDB88320;

	DWORD x;
	
	for(int i = 0; i < 256; i++)
	{
		x = i;
		for(int j = 8; j > 0; j--)
		{
			if(x & 1) { x = (x >> 1) ^ dwPolynomial; }
			else      { x >>= 1; }
		}
		m_dwCRCTable[i] = x;
	}
}

DWORD cCRC32::dwCRCString ( const char* szString )
{
	register DWORD dwReturnCrc32 = 0xFFFFFFFF;
	int iLen = (int) strlen(szString);

	for (int i = 0;i < iLen;i++)
		dwReturnCrc32 = ((dwReturnCrc32) >> 8) ^ m_dwCRCTable[ szString[i] ^ ((dwReturnCrc32) & 0xFF)];

	return ~dwReturnCrc32;
}

CRC32HASH_t* cCRC32::CRCModule  ( HMODULE hModule )
{
	if (hModule == (HMODULE) 0xFFFFFFFF || hModule == (HMODULE)0xFFFFFF ||hModule == (HMODULE)NULL)
		hModule =  GetModuleHandle(NULL);	// we have a pseudo handle here...

	PIMAGE_DOS_HEADER pDOSHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);

	if (pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return NULL;

	PIMAGE_NT_HEADERS pNtHeader  = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<PBYTE>(pDOSHeader) + pDOSHeader->e_lfanew);

	if (pNtHeader->Signature != IMAGE_NT_SIGNATURE)
		return NULL;

	return CRCArray( (const PBYTE) pNtHeader->OptionalHeader.BaseOfCode, pNtHeader->OptionalHeader.SizeOfCode * 4 );
}

CRC32HASH_t* cCRC32::CRCArray ( PBYTE pbArray, int iSize )
{
	static CRC32HASH_t CRC32RET;
	memset(&CRC32RET,0xFFFFFFFF,sizeof CRC32HASH_t);
	
	int iTemporalLen = iSize / 4;

	
	for (int i = 0;i < iTemporalLen;i++)
		CRC32RET.dwResultHash1 = ((CRC32RET.dwResultHash1) >> 8) ^ m_dwCRCTable[ pbArray [i] ^ ((CRC32RET.dwResultHash1) & 0xFF)];

	pbArray += iTemporalLen;

	for (int i = 0;i < iTemporalLen;i++)
		CRC32RET.dwResultHash2 = ((CRC32RET.dwResultHash2) >> 8) ^ m_dwCRCTable[ pbArray [i] ^ ((CRC32RET.dwResultHash2) & 0xFF)];

		pbArray += iTemporalLen;

	for (int i = 0;i < iTemporalLen;i++)
		CRC32RET.dwResultHash3 = ((CRC32RET.dwResultHash3) >> 8) ^ m_dwCRCTable[ pbArray [i] ^ ((CRC32RET.dwResultHash3) & 0xFF)];

	pbArray += iTemporalLen;

	for (int i = 0;i < iTemporalLen;i++)
		CRC32RET.dwResultHash4 = ((CRC32RET.dwResultHash4) >> 8) ^ m_dwCRCTable[ pbArray [i] ^ ((CRC32RET.dwResultHash4) & 0xFF)];

	return &CRC32RET;
}*/