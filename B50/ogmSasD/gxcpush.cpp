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

// gxcpush.cpp : implementation of the CGXPushbutton control
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
#pragma code_seg("GX_SEG_GXCPUSH")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_CONTROL(CGXPushbutton, CGXControl);

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CGXPushbutton control

CGXPushbutton::CGXPushbutton(CGXGridCore* pGrid)
	: CGXControl(pGrid)
{
	AddChild(m_pButton = new CGXButton(this));
}

CGXPushbutton::CGXPushbutton(CGXGridCore* pGrid, CGXButton* pChild)
	: CGXControl(pGrid)
{
	m_pButton = pChild;
}

void CGXPushbutton::Init(ROWCOL nRow, ROWCOL nCol)
{
	CGXControl::Init(nRow, nCol);

	GridWnd()->InvalidateRect(m_pButton->GetRect());
}

void CGXPushbutton::OnInitChildren(ROWCOL nRow, ROWCOL nCol, const CRect& rect)
{
	const CGXStyle& style = Grid()->LookupStyleRowCol(nRow, nCol);

	FocusOnChild(m_pButton);
	m_pButton->SetRect(rect);
	m_pButton->SetText(style.GetIncludeChoiceList() ? (LPCTSTR) style.GetChoiceListRef() : _T(""));
};


void CGXPushbutton::Draw(CDC* pDC, CRect rect, ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, const CGXStyle* pStandardStyle)
{
	if (rect.right <= rect.left || rect.Width() <= 1 || rect.Height() <= 1)
		return;

	// Select font
	CFont* pOldFont = LoadFont(pDC, style, pStandardStyle);

	// Borders
	DrawFrame(pDC, rect, style);

	pDC->SetTextColor(style.GetTextColor());

	CGXControl::Draw(pDC, rect, nRow, nCol, style, pStandardStyle);

	if (pOldFont)
		pDC->SelectObject(pOldFont);
}

CSize CGXPushbutton::CalcSize(CDC* pDC, ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, const CGXStyle* pStandardStyle, BOOL bVert)
{
	bVert, nRow, nCol;

	// Select font
	CFont* pOldFont = LoadFont(pDC, style, pStandardStyle);

	LPCTSTR szText = style.GetIncludeChoiceList() ? (LPCTSTR) style.GetChoiceListRef() : _T("");
#if _MFC_VER >= 0x0300
	CSize size;
	VERIFY(::GetTextExtentPoint32(pDC->m_hAttribDC, szText, _tcslen(szText), &size));
#else
	CSize size = pDC->GetTextExtent(szText, _tcslen(szText));
#endif

	size.cx += 8;
	size.cy += 4;

	if (pOldFont)
		pDC->SelectObject(pOldFont);

	return AddBorders(size, style);
}

void CGXPushbutton::InvertBorders(CDC* pDC, const CRect& r)
{
	pDC, r;
}
