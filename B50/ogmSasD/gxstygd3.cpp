// This is a part of the Objective Grid C++ Library.
// Copyright (C) 1995-1998 Stingray Software, Inc.
// All rights reserved.
//
// This source code is only intended as a supplement to
// the Objective Grid Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding
// the Objective Grid product.
//
// Author: Stefan Hoenig
//

// gxstygd3.cpp : CGXPen profile support 
//

#include "stdafx.h"

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

// Headers

#ifndef _GXRESRC_H_
#include "gxresrc.h"
#endif

#ifndef _GXSTYLES_H_
#include "gxstyles.h"
#endif

#ifndef _GXSTATE_H_
#include "gxstate.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXSTYGDI")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

///////////////////////////////////////////////////////////////////

CGXPen::CGXPen(LPCTSTR szDescription)
{ 
	SetDescription(szDescription); 
}

CGXPen& CGXPen::SetDescription(LPCTSTR szDescription)
{
	DWORD style = 0;
	LONG width = 0;
	COLORREF lopnColor = 0;
	int r = 0, g = 0, b = 0, bp = 0;
	// int n = _stscanf(szDescription, szPenFormat, &style, &width, &r, &g, &b);
	// sscanf is nlot available for extension DLLs
	int n = 1;
	if (_tcsclen(szDescription) > 0)
	{
		LPCTSTR p = szDescription;
		while (*p == _T(' '))
			p++;
		style = (DWORD) _ttol(p);
		p = _tcschr(p, _T(' '));
		if (p)
		{
			n++;
			while (*p == _T(' '))
				p++;
			width = _ttol(p);
			p = _tcschr(p, _T(' '));
		}
		if (p)
		{
			n++;
			while (*p == _T(' '))
				p++;
			r = _ttoi(p);
			p = _tcschr(p, _T(' '));
		}
		if (p)
		{
			n++;
			while (*p == _T(' '))
				p++;
			g = _ttoi(p);
			p = _tcschr(p, _T(' '));
		}
		if (p)
		{
			n++;
			while (*p == _T(' '))
				p++;
			b = _ttoi(p);
			p = _tcschr(p, _T(' '));
		}
		// Support for PALETTEINDEX and PALETTERGB and system color
		if (p)
		{
			n++;
			while (*p == _T(' '))
				p++;
			bp = _ttoi(p);
		}
	}
	ASSERT(n >= 5);
	// ASSERTION-> Wrong description string ->END
	if (n >= 5)
	{
		lopnStyle = (UINT) style;
		lopnColor = RGB(r, g, b);
		lopnWidth = (unsigned) width;
	}
	if (n == 6)
	{
		lopnColor = RGB(r, g, b) | (((DWORD)(BYTE)(bp))<<24);
	}

	lopnColor1 = GXCOLOR1(lopnColor);
	lopnColor2 = GXCOLOR2(lopnColor);

	return SetInclude(n >= 5);
}

void CGXPen::GetDescription(LPTSTR s) const
{
	ASSERT(s);
	ASSERT(GetInclude());
	// ASSERTION-> Did you check GetInclude()? ->END
	COLORREF lopnColor = GXGETCOLORNA2(lopnColor1, lopnColor2);
	int pb = ((BYTE)((lopnColor)>>24)); // Support for PALETTEINDEX and PALETTERGB
	wsprintf(s, szPenFormat, (DWORD) lopnStyle, (LONG) lopnWidth, GetRValue(lopnColor),
		GetGValue(lopnColor), GetBValue(lopnColor), pb);
}

CString CGXPen::GetDescription() const
{
	ASSERT(GetInclude());
	// ASSERTION-> Did you check GetInclude()? ->END
	TCHAR s[64];
	COLORREF lopnColor = GXGETCOLORNA2(lopnColor1, lopnColor2);
	int pb = ((BYTE)((lopnColor)>>24)); // Support for PALETTEINDEX and PALETTERGB
	wsprintf(s, szPenFormat, (DWORD) lopnStyle, (LONG) lopnWidth, GetRValue(lopnColor),
		GetGValue(lopnColor), GetBValue(lopnColor), pb);
	return s;
}

