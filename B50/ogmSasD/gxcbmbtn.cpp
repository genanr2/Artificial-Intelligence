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

// gxcbmbtn.cpp : implementation of the CGXBitmapButton control
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
#pragma code_seg("GX_SEG_GXCBMBTN")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_CONTROL(CGXBitmapButton, CGXControl);

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CGXBitmapButton

CGXBitmapButton::CGXBitmapButton(CGXGridCore* pGrid,
		UINT nIDBitmapResource,
		UINT nIDBitmapResourceSel,
		UINT nIDBitmapResourceFocus)
		: CGXControl(pGrid)
{
	ASSERT(pGrid);
	ASSERT(nIDBitmapResource);

	AddChild(m_pButton = new CGXBitmapButtonChild(this));

	m_bInvertBorders = (nIDBitmapResourceFocus == 0);

	m_pButton->LoadBitmaps(MAKEINTRESOURCE(nIDBitmapResource),
		MAKEINTRESOURCE(nIDBitmapResourceSel),
		MAKEINTRESOURCE(nIDBitmapResourceFocus));
}

void CGXBitmapButton::OnInitChildren(ROWCOL nRow, ROWCOL nCol, const CRect& rect)
{
	FocusOnChild(m_pButton);
	m_pButton->SetRect(rect);

	// Unreferenced:
	nRow, nCol;
};

void CGXBitmapButton::InvertBorders(CDC* pDC, const CRect& r)
{
	if (m_bInvertBorders)
		CGXControl::InvertBorders(pDC, r);
}

void CGXBitmapButton::Draw(CDC* pDC, CRect rect, ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, const CGXStyle* pStandardStyle)
{
	// Background and 3d frame
	DrawBackground(pDC, rect, style);

	CGXControl::Draw(pDC, rect, nRow, nCol, style, pStandardStyle);
}

CSize CGXBitmapButton::CalcSize(CDC* pDC, ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, const CGXStyle* pStandardStyle, BOOL bVert)
{
	// Unused: 
	pDC, nRow, nCol, pStandardStyle, bVert;

	return 	AddBorders(m_pButton->GetSize(), style);
}

