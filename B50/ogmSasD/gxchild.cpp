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

// gxchild.cpp : implementation of child class
//

#include "stdafx.h"

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

// Headers

#ifndef _GXCORE_H_
#include "gxcore.h"
#endif

#ifndef _GXCTRLI_H_
#include "gxctrli.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCHILD")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CGXChild

CGXChild::CGXChild(CGXControl* pComposite)
{
	m_pComposite = pComposite;
	m_bDoubleByteChar = FALSE;
}

CGXChild::~CGXChild()
{
}

void CGXChild::SetRect(const CRect& r)
{
	m_rect = r;
}

CRect CGXChild::GetRect()
{
	return m_rect;
}

BOOL CGXChild::Intersects(CPoint pt)
{
	return m_rect.PtInRect(pt);
}

BOOL CGXChild::HasFocus()
{
	return m_pComposite->ChildWithFocus() == this;
}

void CGXChild::SetText(LPCTSTR s)
{
	m_sText = s;
}

CString CGXChild::GetText()
{
	return m_sText;
}

void CGXChild::Refresh()
{
	m_pComposite->GridWnd()->InvalidateRect(m_rect);
}

BOOL CGXChild::LButtonDown(UINT nFlags, CPoint pt)
{
	// Unreferenced:
	nFlags, pt;

	return FALSE;
}

BOOL CGXChild::LButtonUp(UINT nFlags, CPoint pt)
{
	// Unreferenced:
	nFlags, pt;

	return FALSE;
}

BOOL CGXChild::MButtonDown(UINT nFlags, CPoint pt)
{
	// Unreferenced:
	nFlags, pt;

	return FALSE;
}

BOOL CGXChild::MButtonUp(UINT nFlags, CPoint pt)
{
	// Unreferenced:
	nFlags, pt;

	return FALSE;
}

BOOL CGXChild::RButtonDown(UINT nFlags, CPoint pt)
{
	// Unreferenced:
	nFlags, pt;

	return FALSE;
}

BOOL CGXChild::RButtonUp(UINT nFlags, CPoint pt)
{
	// Unreferenced:
	nFlags, pt;

	return FALSE;
}

BOOL CGXChild::RButtonDblClk(UINT nFlags, CPoint pt)
{
	// Unreferenced:
	nFlags, pt;

	return FALSE;
}

BOOL CGXChild::LButtonDblClk(UINT nFlags, CPoint pt)
{
	// Unreferenced:
	nFlags, pt;

	return FALSE;
}

BOOL CGXChild::MButtonDblClk(UINT nFlags, CPoint pt)
{
	// Unreferenced:
	nFlags, pt;

	return FALSE;
}

BOOL CGXChild::MouseMove(UINT nFlags, CPoint pt)
{
	// Unreferenced:
	nFlags, pt;

	return FALSE;
}

BOOL CGXChild::MouseMoveOver(ROWCOL nRow, ROWCOL nCol, UINT nFlags, CPoint pt)
{
	// Unreferenced:
	nRow, nCol, nFlags, pt;

	return FALSE;
}

BOOL CGXChild::KeyPressed(UINT nMessage, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nMessage)
	{
	// Keyboard messages
	case WM_KEYDOWN:
		return OnGridKeyDown(nChar, nRepCnt, nFlags);

	case WM_KEYUP:
		return OnGridKeyUp(nChar, nRepCnt, nFlags);

	case WM_CHAR:
		{
			// support for DBCS characters
			CString sChar;
			if (GXGetDoubleByteChar((TCHAR) nChar, sChar, m_bDoubleByteChar, m_nDoubleByteLeadChar))
			{
				if (sChar.IsEmpty())
					return TRUE;

				return OnGridDoubleByteChar(sChar, nRepCnt, nFlags);
			}
			else // this is a normal character
			{
				return OnGridChar(nChar, nRepCnt, nFlags);
			}
		}

	case WM_DEADCHAR:
		return OnGridDeadChar(nChar, nRepCnt, nFlags);

	case WM_SYSKEYDOWN:
		return OnGridSysKeyDown(nChar, nRepCnt, nFlags);

	case WM_SYSKEYUP:
		return OnGridSysKeyUp(nChar, nRepCnt, nFlags);

	case WM_SYSCHAR:
		return OnGridSysChar(nChar, nRepCnt, nFlags);

	case WM_SYSDEADCHAR:
		return OnGridSysDeadChar(nChar, nRepCnt, nFlags);
	}

	return FALSE;
}

BOOL CGXChild::OnGridKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXChild::OnGridKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXChild::OnGridChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXChild::OnGridDoubleByteChar(const CString& sChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	sChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXChild::OnGridDeadChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXChild::OnGridSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXChild::OnGridSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXChild::OnGridSysChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXChild::OnGridSysDeadChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

void CGXChild::OnGridWinIniChange(LPCTSTR lpszSection)
{
	// Unused:
	lpszSection;
}

void CGXChild::OnGridSysColorChange()
{
}

void CGXChild::OnGridCancelMode()
{
}

