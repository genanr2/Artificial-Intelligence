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

// gxprof.cpp : profile and registry support for CGXStyle, 
//              CGXGridParam, CGXProperty and CGXStylesMap class
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

#if _MFC_VER >= 0x0300
#ifndef _WINNLS_
#include "winnls.h"
#endif
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXSTYLES")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

// Colors
const TCHAR BASED_CODE szClrFormat[] = _T("%d %d %d");

const DWORD ExcludeAll = 0;
const DWORD IncludeAll = 0xffff;

class CGXGridProfileImp: public CGXAbstractGridProfileImp
{
public:
	virtual void WriteProfile(const CGXProperties* pProp);
	virtual BOOL ReadProfile(CGXProperties* pProp);
	virtual void WriteProfile(const CGXStyle* pStyle, LPCTSTR pszSection, const CGXStylesMap* pStylesMap);
	virtual BOOL ReadProfile(CGXStyle* pStyle, LPCTSTR pszSection, const CGXStylesMap* pStylesMap);
	virtual void WriteProfile(const CGXFont* pFont, LPCTSTR pszSection, LPCTSTR pszEntry);
	virtual BOOL ReadProfile(CGXFont* pFont, LPCTSTR pszSection, LPCTSTR pszEntry);
	virtual BOOL ReadProfile(CGXStylesMap* pStyMap);
	virtual void WriteProfile(const CGXStylesMap* pStyMap);
};

void AFXAPI CGXStyle::ImplementProfile()
{
	if (GXGetAppData()->m_pGridProfileImp == NULL)
		GXGetAppData()->AutoDeletePtr(GXGetAppData()->m_pGridProfileImp = new CGXGridProfileImp);
}

// Profile helper functions

void AFXAPI GXWriteProfileLong(LPCTSTR szSection, LPCTSTR szEntry, LONG val)
{
	TCHAR sr[128];

	wsprintf(sr, _T("%ld"), (LONG) val);
	AfxGetApp()->WriteProfileString(szSection, szEntry, sr);
}

LONG AFXAPI GXGetProfileLong(LPCTSTR szSection, LPCTSTR szEntry, LONG val)
{
	CString sr = AfxGetApp()->GetProfileString(szSection, szEntry);
	if (sr.GetLength() > 0)
		val = _ttol((LPCTSTR) sr);
	return val;
}


void CGXGridProfileImp::WriteProfile(const CGXProperties* pProp) 
{
	ASSERT_VALID(pProp);

	LPCTSTR szSection = pProp->GetSection();

	// Colors
	for (int i = 0; i < pProp->m_ColorArray.GetSize(); i++)
	{
		TCHAR sr[20];
		wsprintf(sr, _T("%lu"), pProp->m_ColorArray[i]);
		AfxGetApp()->WriteProfileString(szSection, pProp->m_ColorNameArray[i], sr);
	}

	GXWriteProfileLong(szSection, CGXProperties::szMarkColHeader, pProp->m_bMarkColHeader);
	GXWriteProfileLong(szSection, CGXProperties::szMarkRowHeader, pProp->m_bMarkRowHeader);
	GXWriteProfileLong(szSection, CGXProperties::szDisplay3DButtons, pProp->m_b3dButtons);
	GXWriteProfileLong(szSection, CGXProperties::szDisplayHorzLines, pProp->m_bDisplayHorzLines);
	GXWriteProfileLong(szSection, CGXProperties::szDisplayVertLines, pProp->m_bDisplayVertLines);

	// user properties
	if (pProp->m_UserPropertyInfoMap.GetCount() > 0)
	{
		WORD nID;
		CGXProperties::UserPropertyInfo* pInfo;
		POSITION pos = pProp->m_UserPropertyInfoMap.GetStartPosition();
		while (pos)
		{
			TCHAR szKey[128];
			pProp->m_UserPropertyInfoMap.GetNextAssoc(pos, nID, (void*&) pInfo);
			wsprintf(szKey, _T("UP%u"), pInfo->nID);
			if (pInfo->pStyle && pInfo->pStyle->GetIncludeValue())
				AfxGetApp()->WriteProfileString(szSection, szKey, (LPCTSTR) pInfo->pStyle->GetValueRef());
		}
	}

	// print settings

	TCHAR sr[128];

	GXWriteProfileLong(szSection, CGXProperties::szPageOrder, pProp->m_nPageOrder);
	GXWriteProfileLong(szSection, CGXProperties::szPrintVertLines, pProp->m_bPrintVertLines);
	GXWriteProfileLong(szSection, CGXProperties::szPrintHorzLines, pProp->m_bPrintHorzLines);
	GXWriteProfileLong(szSection, CGXProperties::szBlackWhite, pProp->m_bBlackWhite);
	GXWriteProfileLong(szSection, CGXProperties::szRowHeaders, pProp->m_bRowHeaders);
	GXWriteProfileLong(szSection, CGXProperties::szColHeaders, pProp->m_bColHeaders);
	GXWriteProfileLong(szSection, CGXProperties::szCenterVertical, pProp->m_bCenterVertical);
	GXWriteProfileLong(szSection, CGXProperties::szCenterHorizontal, pProp->m_bCenterHorizontal);
	GXWriteProfileLong(szSection, CGXProperties::szPrintFrame, pProp->m_bPrintFrame);

	wsprintf(sr, _T("%ld,%ld,%ld,%ld"), (LONG) pProp->m_nTopMargin, (LONG) pProp->m_nLeftMargin,
		(LONG) pProp->m_nBottomMargin, (LONG) pProp->m_nRightMargin);

	AfxGetApp()->WriteProfileString(szSection, CGXProperties::szMargins, sr);

	// Header&Footer
	GXWriteProfileLong(szSection, CGXProperties::szFirstPage, pProp->m_nFirstPage);

	wsprintf(sr, _T("%ld,%ld"), (LONG) pProp->m_nDistTop, (LONG) pProp->m_nDistBottom);

	AfxGetApp()->WriteProfileString(szSection, CGXProperties::szDistances, sr);
}

BOOL CGXGridProfileImp::ReadProfile(CGXProperties* pProp)
{
	LPCTSTR szSection = pProp->GetSection();

	// check, if section is in profile
#if _MFC_VER >= 0x0300 && !defined(_MAC)
	if (AfxGetApp()->m_pszRegistryKey != NULL)
	{
		// Retrieve all entries from key [szSection]
		HKEY hSecKey = AfxGetApp()->GetSectionKey(szSection);
		if (hSecKey == NULL)
			return FALSE;
		RegCloseKey(hSecKey);
	}
	else
#endif
	{
		TCHAR buf[1024];
		TCHAR szDefault[127];
		_tcscpy(szDefault, _T(""));
		_tcscpy(buf, _T(""));
		::WritePrivateProfileString(NULL, NULL, NULL, AfxGetApp()->m_pszProfileName);

		if (::GetPrivateProfileString(szSection, NULL, szDefault, buf, sizeof(buf)/sizeof(TCHAR)-1, AfxGetApp()->m_pszProfileName) == 0)
			return FALSE;
	}

	CString sr;

	// Colors
	for (int i = 0; i < pProp->m_ColorArray.GetSize(); i++)
	{
		CString sr = AfxGetApp()->GetProfileString(szSection, pProp->m_ColorNameArray[i]);
		if (sr.GetLength() > 0)
			pProp->m_ColorArray[i] = _ttol((LPCTSTR) sr);
	}

	pProp->m_bDisplayVertLines
		= (BOOL) GXGetProfileLong(szSection, CGXProperties::szDisplayVertLines, pProp->m_bDisplayVertLines);
	pProp->m_bDisplayHorzLines
		= (BOOL) GXGetProfileLong(szSection, CGXProperties::szDisplayHorzLines, pProp->m_bDisplayHorzLines);
	pProp->m_b3dButtons
		= (BOOL) GXGetProfileLong(szSection, CGXProperties::szDisplay3DButtons, pProp->m_b3dButtons);
	pProp->m_bMarkRowHeader
		= (BOOL) GXGetProfileLong(szSection, CGXProperties::szMarkRowHeader, pProp->m_bMarkRowHeader);
	pProp->m_bMarkColHeader
		= (BOOL) GXGetProfileLong(szSection, CGXProperties::szMarkColHeader, pProp->m_bMarkColHeader);

	// user properties
	if (pProp->m_UserPropertyInfoMap.GetCount() > 0)
	{
		WORD nID;
		CGXProperties::UserPropertyInfo* pInfo;
		POSITION pos = pProp->m_UserPropertyInfoMap.GetStartPosition();
		while (pos)
		{
			TCHAR szKey[128];
			CString s;
			pProp->m_UserPropertyInfoMap.GetNextAssoc(pos, nID, (void*&) pInfo);
			wsprintf(szKey, _T("UP%d"), pInfo->nID);
			s = AfxGetApp()->GetProfileString(szSection, szKey);
			if (pInfo->pStyle)
				pInfo->pStyle->SetValue(s);
		}
	}

	// Print settings

	pProp->m_nPageOrder
		= (int) GXGetProfileLong(szSection, CGXProperties::szPageOrder, pProp->m_nPageOrder);

	pProp->m_bPrintVertLines
		= (GBOOL) GXGetProfileLong(szSection, CGXProperties::szPrintVertLines, pProp->m_bPrintVertLines);

	pProp->m_bPrintHorzLines
		= (GBOOL) GXGetProfileLong(szSection, CGXProperties::szPrintHorzLines, pProp->m_bPrintHorzLines);

	pProp->m_bBlackWhite
		= (GBOOL) GXGetProfileLong(szSection, CGXProperties::szBlackWhite, pProp->m_bBlackWhite);

	pProp->m_bRowHeaders
		= (GBOOL) GXGetProfileLong(szSection, CGXProperties::szRowHeaders, pProp->m_bRowHeaders);

	pProp->m_bColHeaders
		= (GBOOL) GXGetProfileLong(szSection, CGXProperties::szColHeaders, pProp->m_bColHeaders);

	pProp->m_bCenterVertical
		= (GBOOL) GXGetProfileLong(szSection, CGXProperties::szCenterVertical, pProp->m_bCenterVertical);

	pProp->m_bCenterHorizontal
		= (GBOOL) GXGetProfileLong(szSection, CGXProperties::szCenterHorizontal, pProp->m_bCenterHorizontal);

	pProp->m_bPrintFrame
		= (GBOOL) GXGetProfileLong(szSection, CGXProperties::szPrintFrame, pProp->m_bPrintFrame);

	sr = AfxGetApp()->GetProfileString(szSection, CGXProperties::szMargins);
	if (sr.GetLength() > 0)
	{
		LPCTSTR p = (LPCTSTR) sr;
		while (*p == _T(' '))
			p = _tcsinc(p);
		pProp->m_nTopMargin = _ttoi(p);
		p = _tcschr(p, _T(','));
		if (p)
		{
			while (*(p = _tcsinc(p)) == _T(' '))
				NULL;
			pProp->m_nLeftMargin = _ttoi(p);
			p = _tcschr(p, _T(','));
		}
		if (p)
		{
			while (*(p = _tcsinc(p)) == _T(' '))
				NULL;
			pProp->m_nBottomMargin = _ttoi(p);
			p = _tcschr(p, _T(','));
		}
		if (p)
		{
			while (*(p = _tcsinc(p)) == _T(' '))
				NULL;
			pProp->m_nRightMargin = _ttoi(p);
		}
	}

	// Header&Footer

	pProp->m_nFirstPage
		= (int) GXGetProfileLong(szSection, CGXProperties::szFirstPage, pProp->m_nFirstPage);

	sr = AfxGetApp()->GetProfileString(szSection, CGXProperties::szDistances);
	if (sr.GetLength() > 0)
	{
		LPCTSTR p = (LPCTSTR) sr;
		while (*p == _T(' '))
			p = _tcsinc(p);
		pProp->m_nDistTop = _ttoi(p);
		p = _tcschr(p, _T(','));
		if (p)
		{
			while (*(++p) == _T(' '))
				NULL;
			pProp->m_nDistBottom = _ttoi(p);
			p = _tcschr(p, _T(','));
		}
	}

	ASSERT_VALID(pProp);

	return TRUE;
}

static void _WP(LPCTSTR pszSection, LPCTSTR pszEntry, BOOL bInclude, DWORD val)
{
	ASSERT(pszSection != NULL && pszEntry != NULL);

	if (pszSection == NULL || pszEntry == NULL)
		return;

	if (bInclude)
	{
		TCHAR sr[20];
		wsprintf(sr, _T("%lu"), val);
		AfxGetApp()->WriteProfileString(pszSection, pszEntry, sr);
	}
	else
		AfxGetApp()->WriteProfileString(pszSection, pszEntry, NULL);
}

#define WP(pszSection, pszEntry, bInclude, val) \
	if (bInclude) \
		_WP(pszSection, pszEntry, bInclude, val); \
	else \
		_WP(pszSection, pszEntry, bInclude, 0);


void CGXGridProfileImp::WriteProfile(const CGXStyle* pStyle, LPCTSTR pszSection, const CGXStylesMap* pStylesMap)
{
	ASSERT(pszSection != NULL);
	// ASSERTION-> pszSection must not be NULL ->END

	ASSERT_VALID(pStyle);
									 
	if (pszSection == NULL)
		return;

	// HorizontalAlignment
	WP(pszSection, CGXStyle::szHorizontalAlignment,
		pStyle->GetIncludeHorizontalAlignment(), pStyle->GetHorizontalAlignment());

	// VerticalAlignment
	WP(pszSection, CGXStyle::szVerticalAlignment,
		pStyle->GetIncludeVerticalAlignment(), pStyle->GetVerticalAlignment());

	// ReadOnly
	WP(pszSection, CGXStyle::szReadOnly,
		pStyle->GetIncludeReadOnly(), pStyle->boolbits.m_bReadOnly);

	// FloatCell
	WP(pszSection, CGXStyle::szFloatCell,
		pStyle->GetIncludeFloatCell(), pStyle->boolbits.m_bFloatCell);

	// FloodCell
	WP(pszSection, CGXStyle::szFloodCell,
		pStyle->GetIncludeFloodCell(), pStyle->boolbits.m_bFloodCell);

	// MergeCell
	WP(pszSection, CGXStyle::szMergeCell,
		pStyle->GetIncludeMergeCell(), pStyle->boolbits.m_nMergeCell);

	// Draw3dFrame
	WP(pszSection, CGXStyle::szDraw3dFrame,
		pStyle->GetIncludeDraw3dFrame(), pStyle->boolbits.m_nDraw3dFrame);

	// Control
	WP(pszSection, CGXStyle::szControl,
		pStyle->GetIncludeControl(), pStyle->GetControl());

	// Max Length
	WP(pszSection, CGXStyle::szMaxLength,
		pStyle->GetIncludeMaxLength(), pStyle->m_nMaxLength);

	// Vert Scrollbar
	WP(pszSection, CGXStyle::szVertScroll,
		pStyle->GetIncludeVertScrollBar(), pStyle->boolbits.m_bVertScroll);

	// Base Style
	if (pStyle->GetIncludeBaseStyle())
	{
		UINT nResourceID;
		LPCTSTR szName = pStylesMap->GetStyleName(pStyle->GetBaseStyle(), nResourceID);
		if (nResourceID > 0)
		{
			TCHAR s[99];
			wsprintf(s, _T("#%lu"), nResourceID);
			AfxGetApp()->WriteProfileString(pszSection, CGXStyle::szBaseStyle, s);
		}
			AfxGetApp()->WriteProfileString(pszSection, CGXStyle::szBaseStyle, szName);
	}
	else
		AfxGetApp()->WriteProfileString(pszSection, CGXStyle::szBaseStyle, NULL);

	// Textcolor
	if (pStyle->GetIncludeTextColor())
	{
		COLORREF color = pStyle->GetTextColorNA();
		TCHAR s[99];
		int r = GetRValue(color), g = GetGValue(color), b = GetBValue(color);
		wsprintf(s, szClrFormat, r, g, b);
		AfxGetApp()->WriteProfileString(pszSection, CGXStyle::szTextColor, s);
	}
	else
		AfxGetApp()->WriteProfileString(pszSection, CGXStyle::szTextColor, NULL);

	// WrapText
	WP(pszSection, CGXStyle::szWrapText,
		pStyle->GetIncludeWrapText(), pStyle->boolbits.m_bWrapText);

	// AutoSize
	WP(pszSection, CGXStyle::szAutoSize,
		pStyle->GetIncludeAutoSize(), pStyle->boolbits.m_bAutoSize);

	// AllowEnter
	WP(pszSection, CGXStyle::szAllowEnter,
		pStyle->GetIncludeAllowEnter(), pStyle->boolbits.m_bAllowEnter);

	// Enabled
	WP(pszSection, CGXStyle::szEnabled,
		pStyle->GetIncludeEnabled(), pStyle->boolbits.m_bEnabled);

	// TriState
	WP(pszSection, CGXStyle::szTriState,
		pStyle->GetIncludeTriState(), pStyle->boolbits.m_bTriState);

	// Value
	if (pStyle->GetIncludeValue())
		AfxGetApp()->WriteProfileString(pszSection, CGXStyle::szValue, pStyle->GetValueRef());
	else
		AfxGetApp()->WriteProfileString(pszSection, CGXStyle::szValue, NULL);

	// Interior
	if (pStyle->GetIncludeInterior())
		AfxGetApp()->WriteProfileString(pszSection, CGXStyle::szInterior, pStyle->GetInterior().GetDescription());
	else
		AfxGetApp()->WriteProfileString(pszSection, CGXStyle::szInterior, NULL);

	// Format
	WP(pszSection, CGXStyle::szFormat,
		pStyle->GetIncludeFormat(), pStyle->GetFormat());

	// Places
	WP(pszSection, CGXStyle::szPlaces,
		pStyle->GetIncludePlaces(), pStyle->GetPlaces());

	// Choice List
	if (pStyle->GetIncludeChoiceList())
	{
		CString s = pStyle->GetChoiceListRef();
		CString r;

		for (int n = 0; n < s.GetLength(); n++)
		{
			if (s[n] == _T('\r'))
				;
			else if (s[n] == _T('\n'))
				r += _T("\\n");
			else if (s[n] == _T('\\'))
				r += _T("\\\\");
			else
				r += s[n];
		}
		AfxGetApp()->WriteProfileString(pszSection, CGXStyle::szChoiceList, r);
	}
	else
		AfxGetApp()->WriteProfileString(pszSection, CGXStyle::szChoiceList, NULL);

	// Font
	if (pStyle->GetIncludeFont())
		pStyle->GetFontRef().WriteProfile(pszSection, CGXStyle::szFont);
	else
		AfxGetApp()->WriteProfileString(pszSection, CGXStyle::szFont, NULL);

	// Borders
	AfxGetApp()->WriteProfileString(pszSection, CGXStyle::szBorders,
		pStyle->GetIncludeBorders() ? _T("1") : NULL);
	if (pStyle->GetIncludeBorders())
	{
		for (int i = 0; i < CGXStyle::nBorders; i++)
		{
			GXBorderType t;
			t = (GXBorderType) i;
			if (pStyle->GetIncludeBorders(t))
				AfxGetApp()->WriteProfileString(pszSection, CGXStyle::szBorder[t], pStyle->GetBordersRef(t).GetDescription());
			else
				AfxGetApp()->WriteProfileString(pszSection, CGXStyle::szBorder[t], NULL);
		}
	}

	// User attributes
	if (pStyle->m_pmapUserAttributes)
		pStyle->m_pmapUserAttributes->WriteProfile(pszSection);
}

BOOL CGXGridProfileImp::ReadProfile(CGXStyle* pStyle, LPCTSTR pszSection, const CGXStylesMap* pStylesMap)
{
	ASSERT(pszSection != NULL);
	// ASSERTION-> pszSection must not be NULL ->END

	if (pszSection == NULL)
		return FALSE;

	pStyle->Free();

	CString r;

	// VerticalAlignment
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szVerticalAlignment);
	if (r.GetLength() > 0)
		pStyle->SetVerticalAlignment(_ttol(r));

	// HorizontalAlignment
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szHorizontalAlignment);
	if (r.GetLength() > 0)
		pStyle->SetHorizontalAlignment(_ttol(r));

	// ReadOnly
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szReadOnly);
	if (r.GetLength() > 0)
		pStyle->SetReadOnly((BOOL) _ttol(r));

	// FloatCell
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szFloatCell);
	if (r.GetLength() > 0)
		pStyle->SetFloatCell((BOOL) _ttol(r));

	// FloodCell
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szFloodCell);
	if (r.GetLength() > 0)
		pStyle->SetFloodCell((BOOL) _ttol(r));

	// MergeCell
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szMergeCell);
	if (r.GetLength() > 0)
		pStyle->SetMergeCell((unsigned) _ttol(r));

	// Draw3dFrame
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szDraw3dFrame);
	if (r.GetLength() > 0)
		pStyle->SetDraw3dFrame((GXDraw3dFrameType) _ttol(r));

	// MaxLength
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szMaxLength);
	if (r.GetLength() > 0)
		pStyle->SetMaxLength((WORD) _ttoi(r));

	// Vertical Scrollbar
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szVertScroll);
	if (r.GetLength() > 0)
		pStyle->SetVertScrollBar((BOOL) _ttol(r));

	// Control
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szControl);
	if (r.GetLength() > 0)
		pStyle->SetControl((WORD) _ttol(r));

	// Textcolor
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szTextColor);
	if (r.GetLength() > 0)
	{
		int rv = 0, gv = 0, bv = 0;
		// if (_stscanf((LPCTSTR) r, CGXStyle::szClrFormat, &rv, &gv, &bv) == 3)
		// sscanf is not available for extension DLLs
		int n = 1;
		if (r.GetLength() > 0)
		{
			LPCTSTR p = (LPCTSTR) r;
			while (*p == _T(' '))
				p++;
			rv = _ttoi(p);
			p = _tcschr(p, _T(' '));
			if (p)
			{
				n++;
				while (*p == _T(' '))
					p++;
				gv = _ttoi(p);
				p = _tcschr(p, _T(' '));
			}
			if (p)
			{
				n++;
				while (*p == _T(' '))
					p++;
				bv = _ttoi(p);
			}
		}
		if (n == 3)
			pStyle->SetTextColor(RGB(rv, gv, bv));
		else
			ASSERT(0);
			// ASSERTION-> Entry does not have three values ->END
	}

	// WrapText
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szWrapText);
	if (r.GetLength() > 0)
		pStyle->SetWrapText((BOOL) _ttol(r));

	// AutoSize
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szAutoSize);
	if (r.GetLength() > 0)
		pStyle->SetAutoSize((BOOL) _ttol(r));

	// AllowEnter
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szAllowEnter);
	if (r.GetLength() > 0)
		pStyle->SetAllowEnter((BOOL) _ttol(r));

	// Enabled
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szEnabled);
	if (r.GetLength() > 0)
		pStyle->SetEnabled((BOOL) _ttol(r));

	// TriState
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szTriState);
	if (r.GetLength() > 0)
		pStyle->SetTriState((BOOL) _ttol(r));

	// Value
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szValue);
	if (r.GetLength() > 0)
		pStyle->SetValue(r);

	// Base Style
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szBaseStyle);
	if (pStylesMap && r.GetLength() > 0)
	{
		if (r[0] == _T('#'))
			pStyle->SetBaseStyle(pStylesMap->GetBaseStyleId((UINT) _ttol(r.Mid(1))));
		else
			pStyle->SetBaseStyle(pStylesMap->GetBaseStyleId(r));
	}

	// Format
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szFormat);
	if (r.GetLength() > 0)
		pStyle->SetFormat((BOOL) _ttol(r));

	// Places
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szPlaces);
	if (r.GetLength() > 0)
		pStyle->SetPlaces((BOOL) _ttol(r));

	// Choice List
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szChoiceList);
	if (r.GetLength() > 0)
	{
		CString s;

		for (int n = 0; n < r.GetLength(); n++)
		{
			if (r.Mid(n, 2) == _T("\\\\"))
			{
				s += _T("\\");
				n++;
			}
			else if (r.Mid(n, 2) == _T("\\n"))
			{
				s += _T("\r\n");
				n++;
			}
			else
				s += r[n];
		}
		pStyle->SetChoiceList(s);
	}

	// Font
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szFont);
	if (r.GetLength() > 0)
		pStyle->SetFont(CGXFont(r));

	// Interior
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szInterior);
	if (r.GetLength() > 0)
		pStyle->SetInterior(CGXBrush(r));

	// Borders
	r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szBorders);
	if (r == _T("1"))
	{
		for (int i = 0; i < CGXStyle::nBorders; i++)
		{
			GXBorderType t;
			t = (GXBorderType) i;
			r = AfxGetApp()->GetProfileString(pszSection, CGXStyle::szBorder[t]);
			if (r.GetLength() > 0)
				pStyle->SetBorders(t, CGXPen(r));
		}
	}

	// User attributes
	if (pStylesMap)
	{
		for (POSITION pos = pStylesMap->GetUserAttrInfoMap().GetStartPosition(); pos; )
		{
			WORD w;
			CGXStylesMap::UserAttrInfo* pInfo;
			TCHAR s[10];
			pStylesMap->GetUserAttrInfoMap().GetNextAssoc(pos, w, (void*&) pInfo);
			wsprintf(s, _T("UA%u"), w);
			r = AfxGetApp()->GetProfileString(pszSection, s);
			if (r.GetLength() > 0)
				pStyle->SetUserAttribute(w, r);
		}
	}

	ASSERT_VALID(pStyle);

	return TRUE;
}

void CGXGridProfileImp::WriteProfile(const CGXFont* pFont, LPCTSTR pszSection, LPCTSTR pszEntry)
{
	ASSERT(pszSection != NULL && pszEntry != NULL);
	// ASSERTION-> Passed NULL Pointer to CGXFont::WriteProfile ->END

	if (pszSection == NULL || pszEntry == NULL)
		return;

	AfxGetApp()->WriteProfileString(pszSection, pszEntry, pFont->GetDescription());
}

BOOL CGXGridProfileImp::ReadProfile(CGXFont* pFont, LPCTSTR pszSection, LPCTSTR pszEntry)
{
	ASSERT(pszSection != NULL && pszEntry != NULL);
	// ASSERTION-> Passed NULL Pointer to CGXFont::ReadProfile ->END

	if (pszSection == NULL || pszEntry == NULL)
		return FALSE;

	const CString sFont = AfxGetApp()->GetProfileString(pszSection, pszEntry);

	if (sFont.GetLength() == 0)
		return FALSE;

	pFont->SetDescription(sFont);

	return TRUE;
}

void CGXGridProfileImp::WriteProfile(const CGXStylesMap* pStyMap)
{
	ASSERT_VALID(pStyMap);

	LPCTSTR szSection = pStyMap->GetSection();

	// Delete all entries in [szSection]
#if _MFC_VER >= 0x0300
  #ifndef _MAC
	if (AfxGetApp()->m_pszRegistryKey != NULL)
	{
		CStringArray nameArray;
		DWORD dwSize = 1024;
		LPTSTR buf = new TCHAR[dwSize];
		DWORD iSubKey = 0;

		// Retrieve all entries from key [szSection]
		HKEY hSecKey = AfxGetApp()->GetSectionKey(szSection);
		if (hSecKey != NULL)
		{
			LONG lRes = ::RegEnumKey(
				hSecKey,    // handle of key to enumerate
				iSubKey,    // index of subkey to enumerate
				buf,        // address of buffer for subkey name
				dwSize      // size of subkey buffer
				);

			while (lRes == ERROR_SUCCESS)
			{
				nameArray.SetAtGrow(iSubKey++, buf);

				lRes = RegEnumKey(hSecKey, iSubKey, buf, dwSize);
			}

			::RegCloseKey(hSecKey);
		}

		delete buf;

		HKEY hAppKey = AfxGetApp()->GetAppRegistryKey();

		for (int n = 0; n < nameArray.GetSize(); n++)
			::RegDeleteKey(hAppKey, (CString) szSection + _T('\\') + nameArray[n]);
	}
	else
  #endif
		AfxGetApp()->WriteProfileString(szSection, NULL, NULL);
#else
	::WritePrivateProfileString(szSection, NULL, NULL, AfxGetApp()->m_pszProfileName);
#endif

	// Styles
	POSITION pos = pStyMap->StartStylesIteration();
	if (pos)
	{
		WORD wStyleId;
		CString sName;
		UINT nResourceID;
		const CGXStyle* pStyle = NULL;

		do
		{
			pStyMap->GetNextStyle(pos, wStyleId, sName, nResourceID, pStyle);

			ASSERT_VALID(pStyle);

			// Append style name in [szSection]
			if (nResourceID > 0)
			{
				wsprintf(sName.GetBufferSetLength(99), _T("#%u"), nResourceID);
				sName.ReleaseBuffer();
			}

			// Write style to section [szSection.Name]
#if _MFC_VER >= 0x0300 && !defined(_MAC)
			if (AfxGetApp()->m_pszRegistryKey != NULL)
				pStyle->WriteProfile((CString) szSection + _T('\\') + sName, pStyMap);
			else
#endif
			{
				AfxGetApp()->WriteProfileString(szSection, sName, _T("1"));
				pStyle->WriteProfile((CString) szSection + _T('.') + sName, pStyMap);
			}
		}
		while (pos);
	}
}

BOOL CGXGridProfileImp::ReadProfile(CGXStylesMap* pStyMap)
{
	ASSERT(pStyMap->m_nLastId > 0);
	// ASSERTION-> No styles registered.
	// Did you call CreateStandardStyles()? ->END

	LPCTSTR szSection = pStyMap->GetSection();

	CStringArray nameArray;
#if _MFC_VER >= 0x0300 && !defined(_MAC)
	if (AfxGetApp()->m_pszRegistryKey != NULL)
	{
		DWORD dwSize = 1024;
		LPTSTR buf = new TCHAR[dwSize];
		DWORD iSubKey = 0;

		// Retrieve all entries from key [szSection]
		HKEY hSecKey = AfxGetApp()->GetSectionKey(szSection);
		if (hSecKey != NULL)
		{
			LONG lRes = ::RegEnumKey(
				hSecKey,    // handle of key to enumerate
				iSubKey,    // index of subkey to enumerate
				buf,        // address of buffer for subkey name
				dwSize      // size of subkey buffer
				);

			while (lRes == ERROR_SUCCESS)
			{
				nameArray.SetAtGrow(iSubKey++, buf);

				lRes = RegEnumKey(hSecKey, iSubKey, buf, dwSize);
			}

			::RegCloseKey(hSecKey);
		}

		delete buf;
	}
	else
		// Retrieve all entries from [szSection]
#endif
	{
		// check, if section is in profile
		LPTSTR buf = new TCHAR[1024];
		TCHAR szDefault[127];
		_tcscpy(szDefault, _T(""));
		_tcscpy(buf, _T(""));

		::WritePrivateProfileString(NULL, NULL, NULL, AfxGetApp()->m_pszProfileName);

		// check out style names from [szSection]
		int nBufSize = ::GetPrivateProfileString(szSection, NULL, szDefault, buf, 1023, AfxGetApp()->m_pszProfileName);

		// parse entries
		for (int n = 0, nBOL = 0; n < nBufSize; n += _tclen(buf+n))
		{
			// entry syntax: entry=xxx\n
			if (buf[n] == _T('\0'))
			{
				nameArray.SetAtGrow(nameArray.GetSize(), buf+nBOL);
				nBOL = n+1;
			}
		}

		delete buf;
	}

	// entries found?
	if (nameArray.GetSize() == 0)
		return FALSE;

	// check out Styles
	for (int n = 0; n < nameArray.GetSize(); n++)
	{
		CGXStyle* pStyle = (CGXStyle*) pStyMap->m_pStyleClass->CreateObject();

		// read style from section [szSection.Name]
#if _MFC_VER >= 0x0300 && !defined(_MAC)
		if (AfxGetApp()->m_pszRegistryKey != NULL)
			pStyle->ReadProfile((CString) szSection + _T('\\') + nameArray[n], pStyMap);
		else
#endif
			pStyle->ReadProfile((CString) szSection + _T('.') + nameArray[n], pStyMap);

		CString s = nameArray.GetAt(n);
		if (s[0] == _T('#'))
			pStyMap->RegisterStyle((UINT) _ttol(s.Mid(1)), *pStyle);
		else
			pStyMap->RegisterStyle(nameArray[n], *pStyle);

		delete pStyle;
	}

	pStyMap->m_wStyleStandard = pStyMap->GetBaseStyleId(GX_IDS_STYLE_STANDARD);
	pStyMap->m_wStyleColHeader = pStyMap->GetBaseStyleId(GX_IDS_STYLE_COLHEADER);
	pStyMap->m_wStyleRowHeader = pStyMap->GetBaseStyleId(GX_IDS_STYLE_ROWHEADER);

	// fill up standard style
	CGXStyle* pStyle;
	VERIFY(pStyMap->LookupStyle(pStyMap->m_wStyleStandard, pStyle));
	CGXStyle* pDefStyle = (CGXStyle*) pStyle->GetRuntimeClass()->CreateObject();
	pDefStyle->SetDefault();
	pStyle->ChangeStyle(*pDefStyle, gxApplyNew);
	delete pDefStyle;

	ASSERT_VALID(pStyMap);

	return TRUE;
}

