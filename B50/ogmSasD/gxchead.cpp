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

// gxchead.cpp : implementation of the CGXHeader control
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

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_CONTROL(CGXHeader, CGXStatic);

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CGXHeader

CGXHeader::CGXHeader(CGXGridCore* pGrid)
	: CGXStatic(pGrid)
{
}

void CGXHeader::Draw(CDC* pDC, CRect rect, ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, const CGXStyle* pStandardStyle)
{
	// Rectangle
	CRect rectItem = rect;

	CRect rectText = GetCellRect(nRow, nCol, rect);

	if (rect.right <= rect.left || rect.Width() <= 1 || rect.Height() <= 1)
		return;

	// Select font
	CFont* pOldFont = LoadFont(pDC, style, pStandardStyle);

	// Cell-Color
	COLORREF rgbText = style.GetTextColor();

	if (Grid()->GetParam()->GetProperties()->GetBlackWhite())
		rgbText = RGB(0,0,0);

	BOOL bPressed = GetMarkHeaderState(nRow, nCol, style);

	// Background
	DrawBackground(pDC, rect, style);

	CRect rc = Grid()->SubtractBorders(rect, style, TRUE);

	CGXProperties* pProp = Grid()->GetParam()->GetProperties();
	// Draw button look (if not pressed)
	if (style.GetDraw3dFrame() == gxFrameNormal
		&& pProp->GetDisplay3dButtons())
	{
		COLORREF rgb3dDkShadow = RGB(0, 0, 0);

		if (!Grid()->IsPrinting())
		{
			COLORREF rgb3dHilight = GXGetSysData()->GetSysColor(COLOR_BTNHIGHLIGHT);

			if (!bPressed)
			{
				// Draw Raised like a normal CGXHeader (looks better
				// when grid lines are turned on)
				GXDraw3dFrame(pDC, rc.left, rc.top, rc.right-1, rc.bottom-1, 1,
					rgb3dHilight, rgb3dDkShadow);
			}
			else
			{
				GXPatB(pDC, CRect(rc.left, rc.bottom-1, rc.right-1, rc.bottom), rgb3dDkShadow);
				GXPatB(pDC, CRect(rc.right-1, rc.top, rc.right, rc.bottom), rgb3dDkShadow);
			}
		}
		else
		{
			// border between headers and cells when printing
			ROWCOL nhr = Grid()->GetHeaderRows();
			ROWCOL nhc = Grid()->GetHeaderCols();

			CGXPen pen(PS_SOLID, 1, RGB(0,0,0));

			if (nCol <= nhc || nRow <= nhr)
			{
				if (style.GetIncludeBorders(gxBorderBottom)
					&& style.GetBordersRef(gxBorderBottom).GetStyle() == PS_NULL
					&& pProp->GetDisplayHorzLines())
					Grid()->DrawBorder(pen, rect, gxBorderBottom, pDC);

				if (style.GetIncludeBorders(gxBorderRight)
					&& style.GetBordersRef(gxBorderRight).GetStyle() == PS_NULL
					&& pProp->GetDisplayVertLines())
					Grid()->DrawBorder(pen, rect, gxBorderRight, pDC);
			}
		}
	}

	if (bPressed)
	{
		// text will be moved to the bottom-right corner a bit
		rectText.top++;
		rectText.left++;
	}

	pDC->SetBkMode(TRANSPARENT);

	DWORD dtAlign = style.GetHorizontalAlignment() | style.GetVerticalAlignment();

	if (style.GetWrapText())
		dtAlign |= DT_NOPREFIX | DT_WORDBREAK;
	else
		dtAlign |= DT_NOPREFIX | DT_SINGLELINE;

	pDC->SetTextColor(rgbText);

	CString sOutput;
	GetControlText(sOutput, nRow, nCol, NULL, style);

	HANDLE hbm = GXGetDIBState()->GetPool()->LookupOrCreateDIB(sOutput);
	if (hbm)
	{
		// Draw bitmap
		rect = GetCellRect(nRow, nCol, rect, &style);

		DWORD dtAlign = style.GetHorizontalAlignment() | style.GetVerticalAlignment();

		CGXDIB::Draw(pDC, hbm, rect, CGXDIB::none, (UINT) dtAlign);

		// child Controls: spin-buttons, hotspot, combobox btn, ...
		CGXControl::Draw(pDC, rect, nRow, nCol, style, pStandardStyle);
	}
	else
	{
		if (!sOutput.IsEmpty())
		{
			if (style.GetIncludeFont() 
				&& style.GetFontRef().GetIncludeOrientation() 
				&& style.GetFontRef().GetOrientation() != 0)
			{
				// draw vertical text
				GXDaFTools()->GXDrawRotatedText(pDC, 
					sOutput,
					sOutput.GetLength(),
					rectText,
					(UINT) dtAlign,
					style.GetFontRef().GetOrientation(), 
					&rect);
			}
			else
			{
				GXDrawTextLikeMultiLineEdit(pDC,
					sOutput,
					sOutput.GetLength(),
					rectText,
					(UINT) dtAlign,
					&rect);
			}
		}
	}

	CGXControl::Draw(pDC, rect, nRow, nCol, style, pStandardStyle);

	if (pOldFont)
		pDC->SelectObject(pOldFont);
}

void CGXHeader::InvertBorders(CDC* pDC, const CRect& r)
{
	// Mark the header as current cell
	pDC->DrawFocusRect(r);
}
