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

// gxcalcsi.cpp : implementation of CalcSize functionality
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

#ifndef _GXCTRL_H_
#include "gxctrl.h"
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

class CGXCalcSizeImp: public CGXAbstractCalcSizeImp
{
public:
	virtual CSize CalcSize(CGXControl* pControl, CDC* pDC, ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, const CGXStyle* pStandardStyle, BOOL bVert);
};

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CGXStatic

void CGXGridCore::ImplementCalcSize()
{
	if (m_pAbstractCalcSizeImp == NULL)
		AutoDeletePtr(m_pAbstractCalcSizeImp = new CGXCalcSizeImp);
}

CSize CGXCalcSizeImp::CalcSize(CGXControl* pControl, CDC* pDC, ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, const CGXStyle* pStandardStyle, BOOL bVert)
{
	CGXGridCore* pGrid = pControl->Grid();

	CString sOutput;
	if (pGrid->GetParam()->m_nDisplayExpression & GX_EXPR_DISPLAYALWAYS)
		sOutput = pGrid->GetExpressionRowCol(nRow, nCol);
	else
		pControl->GetControlText(sOutput, nRow, nCol, NULL, style);

	HANDLE hbm = 0;
	
	// check for #bmp only for static cells (and not 
	// for cells where the user can enter the text)

	if (!pControl->IsKindOf(RUNTIME_CLASS(CEdit)))
		hbm = GXGetDIBState()->GetPool()->LookupOrCreateDIB(sOutput);

	if (hbm != NULL)
	{
		// adjust cell to bitmap size

		CGXDIB dib;
		dib.Attach(hbm);
		CSize size = dib.GetSize();
		dib.Detach();

		// Don't add margins - only add borders 
		// Therefore we call CGXControl::AddBorders
		return pControl->CGXControl::AddBorders(size, style);
	}
	else
	{
		// text only

		// Select font
		CFont* pOldFont = pControl->LoadFont(pDC, style, pStandardStyle);

		// Determine current size of cell (and take care if cell
		// is a covered, merged or float cell) and subtract space 
		// needed for borders, 3d-Frame or grid-lines with GetCellRect
		//
		// Don't call CalcRectFromRowCol because the calculation
		// of CalcRectFromRowCol is based on the current top
		// row and last visible row. If the cell is not visible,
		// the CRect would then be empty and that's not what we
		// want!

		CGXRange area = CGXRange(nRow, nCol);
		pGrid->GetCellRangeRowCol(nRow, nCol, area);
		CRect rc(0, 0, pGrid->CalcSumOfColWidths(area.left, area.right), pGrid->CalcSumOfRowHeights(area.top, area.bottom));
		rc = pControl->GetCellRect(nRow, nCol, rc, &style);
		CSize size(rc.Width(), rc.Height());

		// BLOCK
		{
			DWORD dtAlign = style.GetHorizontalAlignment() | style.GetVerticalAlignment();

			if (!sOutput.IsEmpty())
			{
				dtAlign |= DT_NOPREFIX | DT_WORDBREAK;

				TEXTMETRIC    tm;
				pDC->GetTextMetrics(&tm);
				int dyHeight = tm.tmHeight + tm.tmExternalLeading;

				BOOL bWrap = style.GetIncludeWrapText() && style.GetWrapText();

				int nLineBreaks = 1;

				CRect rcTemp(rc.left, rc.top, rc.right, 32767);

				int LineBreaks[32], LineLengths[32];

				if (bWrap)
					nLineBreaks = GXComputeLineBreaks(pDC,
						sOutput,
						-1,
						rcTemp,
						(UINT) dtAlign,
						LineBreaks,
						LineLengths,
						32);

				if (bVert)
				{
					if (bWrap)
						size.cy = nLineBreaks*dyHeight+1;
					else
						size.cy = (dyHeight+1);
				}
				else
				{
					CSize sz;

					int height = 0;
					int width = 0;

					// Optimize the width to be large enough for a single line
					// only if the cell is not large enough or if wraptext is FALSE
					if (!bWrap || nLineBreaks*dyHeight+1 > rc.Height())
					{
						// The following loop determines the width of
						// each line of text (separated with a newline).

						CString s = sOutput;
						CString sItem;
						int n = 0;
						while (!s.IsEmpty() && n != -1)
						{
							n = GXGetNextLine(s, sItem);

							sz = pDC->GetTextExtent(sItem, sItem.GetLength());
							width = max(width, sz.cx);
							height += sz.cy;
						}
						size.cx = width;
					}
					else
					{
						// Find the minimum width which is large enough for all
						// wrapped lines in a multline cell where the cells
						// row height is high enough.

						LPCTSTR lpszString = sOutput;

						for (int nBreak = 0; nBreak < nLineBreaks; nBreak++)
						{
							sz = pDC->GetTextExtent(lpszString+LineBreaks[nBreak], LineLengths[nBreak]);
							width = max(width, sz.cx);
							height += sz.cy;
						}
						size.cx = width;
					}
				}
			}
			else
			{
				// no value
				size.cx = size.cy = 0;

				TEXTMETRIC    tm;
				pDC->GetTextMetrics(&tm);
				int dyHeight = tm.tmHeight + tm.tmExternalLeading;

				size.cy = dyHeight+1;
				size.cx = tm.tmMaxCharWidth;
			}
		}

		if (pOldFont)
			pDC->SelectObject(pOldFont);

		return pControl->AddBorders(size, style);
	}
}

