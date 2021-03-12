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

// gxstygd2.cpp : CGXFont, CGXBrush profile support
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

CGXFont::CGXFont(LPCTSTR pszDescription)
{
	SetDescription(pszDescription);
}

void CGXFont::GetDescription(LPTSTR s) const
{
	LPTSTR p = s;
	LPCTSTR szd = _T("%d");

	ASSERT(s);
	if (!s)
		return;

	if (GetIncludeFaceName())
	{
#if !defined(_UNIX_) && !defined(_MAC)
		wsprintf(s, _T("%s"), GetFaceName());
		p = s + _tcslen(s);
#else
		p = s + _stprintf(s, _T("%s"), GetFaceName());
		// I have used _stprintf instead of wsprint because
		// I need the number of bytes and not the number of
		// characters when dealing with DBCS strings.
#endif
	}
	*p++ = _T(',');
	if (GetIncludeSize())
		p += wsprintf(p, szd, (int) GetSize());
	*p++ = _T(',');
	if (GetIncludeWeight())
		p += wsprintf(p, szd, (int) GetWeight());
	*p++ = _T(',');
	if (GetIncludeItalic())
		*p++ = (TCHAR) (GetItalic() + _T('0'));
	*p++ = _T(',');
	if (GetIncludeUnderline())
		*p++ = (TCHAR) (GetUnderline() + _T('0'));
	*p++ = _T(',');
	if (GetIncludeStrikeOut())
		*p++ = (TCHAR) (GetStrikeOut() + _T('0'));
	if (GetIncludeOrientation())
		p += wsprintf(p, szd, (int) GetOrientation());
	*p = _T('\0');
}

CString CGXFont::GetDescription() const
{
	TCHAR s[128];

	GetDescription(s);

	return s;
}

CGXFont& CGXFont::SetDescription(LPCTSTR s)
{
	ASSERT(s != NULL);
	// ASSERTION-> Passed NULL Pointer to CGXFont::ReadProfile ->END

	if (s == NULL)
		return *this;

	TCHAR t[128];
	Init();

	// Facename
	LPTSTR p = (char*)_tcschr(s, _T(','));
	if (*s != _T(','))
	{
		_tcsncpy(t, s, p-s);
		t[p-s] = _T('\0');
		SetFaceName(t);
	}
	if (p == NULL)
		return *this;

	// Size
	s = p+1;
	p =(char*) _tcschr(s, _T(','));
	if (*s != _T(','))
		SetSize(_ttoi(s));
	if (p == NULL)
		return *this;

	// Bold
	s = p+1;
	p = (char*)_tcschr(s, _T(','));
	if (*s != _T(','))
		SetWeight((BOOL) _ttoi(s));
	if (p == NULL)
		return *this;

	// Italic
	s = p+1;
	p = (char*)_tcschr(s, _T(','));
	if (*s != _T(','))
		SetItalic((BOOL) _ttoi(s));
	if (p == NULL)
		return *this;

	// Underline
	s = p+1;
	p =(char*) _tcschr(s, _T(','));
	if (*s != _T(','))
		SetUnderline((BOOL) _ttoi(s));
	if (p == NULL)
		return *this;

	// StrikeOut
	s = p+1;
	p = (char*)_tcschr(s, _T(','));
	if (*s != _T(','))
		SetStrikeOut((BOOL) _ttoi(s));
	if (p == NULL)
		return *this;

	// Orientation
	s = p+1;
	if (*s)
		SetOrientation((BOOL) _ttoi(s));

	return *this;
}

///////////////////////////////////////////////////////////////////

CGXBrush::CGXBrush(LPCTSTR pszDescription)
{ 
	SetDescription(pszDescription); 
}

CGXBrush& CGXBrush::SetDescription(LPCTSTR szDescription)
{
	DWORD style = 0;
	LONG hatch = 0;
	int r = 0, g = 0, b = 0;
	int rb = 0, gb = 0, bb = 0, bp = 0;
	COLORREF lbColor = 0, lbBkColor = 0;

	// int n = _stscanf(szDescription, szBrushFormat, &style, &hatch, &r, &g, &b);
	// sscanf is not available for extension DLLs
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
			hatch = _ttol(p);
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
		if (p)
		{
			n++;
			while (*p == _T(' '))
				p++;
			rb = _ttoi(p);
			p = _tcschr(p, _T(' '));
		}
		if (p)
		{
			n++;
			while (*p == _T(' '))
				p++;
			gb = _ttoi(p);
			p = _tcschr(p, _T(' '));
		}
		if (p)
		{
			n++;
			while (*p == _T(' '))
				p++;
			bb = _ttoi(p);
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
		lbStyle = (UINT) style;
		lbColor = RGB(r, g, b) | (((DWORD)(BYTE)(bp))<<24);

		if (lbStyle == BS_PATTERN)
			lbHatch = (int) GXGetPatternBitmap(lbPattern = (int) hatch)->GetSafeHandle();
		else
			lbHatch = (int) hatch;
		if (n >= 8)
			lbBkColor = RGB(rb, gb, bb);

		// Support for PALETTEINDEX and PALETTERGB
		if (n >= 9 && bp == 1)
			lbColor = PALETTEINDEX(MAKEWORD(r, g));
		else if (n >= 9 && bp == 2)
			lbColor = PALETTERGB(r, g, b);
	}

	lbColor1 = GXCOLOR1(lbColor);
	lbColor2 = GXCOLOR2(lbColor);
	lbBkColor1 = GXCOLOR1(lbBkColor);
	lbBkColor2 = GXCOLOR2(lbBkColor);

	return *this;
}

void CGXBrush::GetDescription(LPTSTR s) const
{
	ASSERT(s);
	wsprintf(s, szBrushFormat, (DWORD) lbStyle, (LONG) lbHatch,
		GetRValue(lbColor2), GetGValue(lbColor2), GetBValue(lbColor2),
		GetRValue(lbBkColor2), GetGValue(lbBkColor2), GetBValue(lbBkColor2), 
		((BYTE)((GXGETCOLORNA2(lbColor1, lbColor2))>>24)));
}

CString CGXBrush::GetDescription() const
{
	DWORD style = lbStyle;
	LONG hatch = lbHatch;
	if (lbStyle == BS_PATTERN)
		hatch = lbPattern;
	int r = GetRValue(lbColor2), g = GetGValue(lbColor2), b = GetBValue(lbColor2);
	int rb = GetRValue(lbBkColor2), gb = GetGValue(lbBkColor2), bb = GetBValue(lbBkColor2);
	int pb = ((BYTE)((GXGETCOLORNA2(lbColor1, lbColor2))>>24));   // Support for PALETTEINDEX and PALETTERGB
	TCHAR s[64];
	wsprintf(s, szBrushFormat, style, hatch, r, g, b, rb, gb, bb, pb);
	return s;
}

