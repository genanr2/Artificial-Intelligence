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

// gxcedhot.cpp : implementation of the CGXHotSpotEdit control
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
#pragma code_seg("GX_SEG_GXCEDHOT")
#endif

IMPLEMENT_CONTROL(CGXHotSpotEdit, CGXEditControl);
IMPLEMENT_DYNAMIC(CGXHotSpotEdit, CGXEditControl);

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CGXHotSpotEdit control

CGXHotSpotEdit::CGXHotSpotEdit(CGXGridCore* pGrid, UINT nID)
	: CGXEditControl(pGrid, nID)
{
	AddChild(m_pHotSpot = new CGXHotSpotButton(this));
}

void CGXHotSpotEdit::Init(ROWCOL nRow, ROWCOL nCol)
{
	CGXEditControl::Init(nRow, nCol);

	// Force drawing of button for current cell
	GridWnd()->InvalidateRect(m_pHotSpot->GetRect());
}

BEGIN_MESSAGE_MAP(CGXHotSpotEdit, CGXEditControl)
	//{{AFX_MSG_MAP(CGXHotSpotEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

const int nEditBtnWidth = 13;

CRect CGXHotSpotEdit::GetCellRect(ROWCOL nRow, ROWCOL nCol, LPRECT rectItem /* = NULL */, const CGXStyle* pStyle /*= NULL*/)
{
	// compute the interior rectangle for the text
	// without buttons and borders

	CRect rect = CGXEditControl::GetCellRect(nRow, nCol, rectItem, pStyle);

	rect.right -= nEditBtnWidth;

	return rect;
}

CSize CGXHotSpotEdit::AddBorders(CSize size, const CGXStyle& style)
{
	size.cx += nEditBtnWidth;

	return CGXEditControl::AddBorders(size, style);
}

void CGXHotSpotEdit::OnInitChildren(ROWCOL nRow, ROWCOL nCol, const CRect& rect)
{
	nRow, nCol;

	const int nEditBtnWidth = 13;

	// init hotspot button
	CRect rectBtn;
	rectBtn.IntersectRect(rect,
				CRect(rect.right-1-nEditBtnWidth, rect.top, rect.right-1, rect.top+nEditBtnWidth));

	if (rectBtn.Height() < rect.Height()-1)
	{
		rectBtn.top += 1;
		rectBtn.bottom += 1;
	}

	m_pHotSpot->SetRect(rectBtn);
}

BOOL CGXHotSpotEdit::CanFloatCell(ROWCOL, ROWCOL, const CGXStyle&, BOOL)
{
	// don't support floating
	return FALSE;
}
