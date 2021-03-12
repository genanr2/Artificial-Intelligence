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

// gxcstat.cpp : implementation of the CGXStatic control
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
#pragma code_seg("GX_SEG_GXCSTAT")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_CONTROL(CGXStatic, CGXControl);

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CGXStatic

CGXStatic::CGXStatic(CGXGridCore* pGrid)
	: CGXControl(pGrid)
{
}

CRect CGXStatic::GetCellRect(ROWCOL nRow, ROWCOL nCol, LPRECT rectItem /* = NULL */, const CGXStyle* pStyle /*= NULL*/)
{
	// compute the interior rectangle for the text
	// without buttons and borders

	if (pStyle == NULL)
		pStyle = &Grid()->LookupStyleRowCol(nRow, nCol);

	CRect rect = CGXControl::GetCellRect(nRow, nCol, rectItem, pStyle);

	CString str;
	HANDLE hbm = 0;
	
	// check for #bmp only for static cells (and not 
	// for cells where the user can enter the text)

	if (hbm == NULL)
	{
		// On the top and on the left side is the Grid-Line (1pt)!
		// Put the appropriate Values here (these are for normal Text-Cells)
		int xFrame = max(GetSystemMetrics(SM_CXFRAME)/2, 2);
		int yFrame = 0; // or GetSystemMetrics(SM_CXFRAME);

		rect.left += xFrame;
		if (pStyle->GetIncludeHorizontalAlignment()
			&& pStyle->GetHorizontalAlignment() == DT_RIGHT)
			rect.right -= xFrame+1;
		else
			rect.right -= xFrame/2;
		rect.top += yFrame;
		rect.bottom -= yFrame;
	}

	return rect;
}

CSize CGXStatic::AddBorders(CSize size, const CGXStyle& style)
{
	// this is the counterpart to GetCellRect

	CString str;
	HANDLE hbm = 0;
	
	// check for #bmp only for static cells (and not 
	// for cells where the user can enter the text)

	if (hbm == NULL)
	{
		// On the top and on the left side is the Grid-Line (1pt)!
		// Put the appropriate Values here (these are for normal Text-Cells)
		int xFrame = max(GetSystemMetrics(SM_CXFRAME)/2, 2);
		int yFrame = 0; // or GetSystemMetrics(SM_CXFRAME);

		size.cx += xFrame;
		size.cx += xFrame+1;

		size.cy += 2*yFrame;
	}

	return Grid()->AddBorders(size, style);
}


void CGXStatic::Draw(CDC* pDC, CRect rect, ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, const CGXStyle* pStandardStyle)
{
	ASSERT(pDC != NULL && pDC->IsKindOf(RUNTIME_CLASS(CDC)));
	// ASSERTION-> Invalid Device Context ->END
	ASSERT(nRow <= Grid()->GetRowCount() && nCol <= Grid()->GetColCount());
	// ASSERTION-> Cell coordinates out of range ->END

	ASSERT_VALID(pDC);

	GXDaFTools()->DrawStatic(this, pDC, rect, nRow, nCol, style, pStandardStyle);
}

// In OG 1.1, I have changed the behavior of CalcSize so that it does not
// resize columns any more if the row height is already large
// enough to hold the cells contents by wrapping the text.

CSize CGXStatic::CalcSize(CDC* pDC, ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, const CGXStyle* pStandardStyle, BOOL bVert)
{
	GX_CHECKIMP(Grid()->m_pAbstractCalcSizeImp, "CGXStatic::CalcSize");

	if (Grid()->m_pAbstractCalcSizeImp)
		return Grid()->m_pAbstractCalcSizeImp->CalcSize(this, pDC, nRow, nCol, style, pStandardStyle, bVert);

	return CSize(0,0);
}

BOOL CGXStatic::CanFloatCell(ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, BOOL bFloatOrFlood)
{
	if (Grid()->m_pFloatCellsImp)
		return Grid()->m_pFloatCellsImp->CanFloatCell(this, nRow, nCol, style, bFloatOrFlood);

	return FALSE;
}

