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

// gxcorfit.cpp : fit cells to contents (CGXGridCore)
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

#ifndef _GXCORE_H_
#include "gxcore.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCORFIT")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef TICKCOUNT
#define _TC_(f) f
#else
#define _TC_(f)
#endif


#define new DEBUG_NEW

// CGXAbstractGridResizeToFitImp* m_pResizeToFitImp;
// friend class CGXGridResizeToFitImp;
// void EnableResizeToFit();

class CGXGridResizeToFitImp: public CGXAbstractGridResizeToFitImp
{
public:
	virtual BOOL ResizeRowHeightsToFit(CGXGridCore* pGrid, CGXRange& range, BOOL bResizeCoveredCells, UINT nFlags);
	virtual BOOL ResizeColWidthsToFit(CGXGridCore* pGrid, CGXRange& range, BOOL bResizeCoveredCells, UINT nFlags);
};

void CGXGridCore::ImplementResizeToFit()
{
	if (m_pResizeToFitImp == NULL)
		AutoDeletePtr(m_pResizeToFitImp = new CGXGridResizeToFitImp);

	ImplementCalcSize();
}

BOOL CGXGridResizeToFitImp::ResizeRowHeightsToFit(CGXGridCore* pGrid, CGXRange& range, BOOL bResizeCoveredCells, UINT nFlags)
{
	CClientDC dc(pGrid->m_pGridWnd);
	pGrid->OnGridPrepareDC(&dc);

	// select default font into device context
	CGXStyle* pStyleStandard = &pGrid->StandardStyle();
	CFont* pOldFont = pGrid->LoadFont(&dc, *pStyleStandard);

	// Get the HFONT handle because pOldFont is only a temporary CGdiObject object 
	// and is only valid until the next time the application has idle time in its 
	// event loop.
	HFONT hOldFont = (HFONT) pOldFont->GetSafeHandle();

	range.ExpandRange(0, 0, pGrid->GetRowCount(), pGrid->GetColCount());

	pGrid->EvalMergeCells(range);
	pGrid->EvalFloatCells(range);

	CGXLongOperation theOp;
	theOp.SetStatusText(pGrid->m_pAppData->strmResizeRows);
	pGrid->BeginTrans(pGrid->m_pAppData->strmResizeRows);

	int nMaxHeight;
	ROWCOL nfr = pGrid->GetFrozenRows();
	CRect rectGrid = pGrid->GetGridRect();

	if (rectGrid.Height() == 0)
	{
		TRACE(
			_T("Warning: ResizeRowHeightsToFit called for a GridWnd which client's area is CSize(0,0)\n")
			_T("It is possible that sized rows do not fit into the grid area\n"));
		CWnd* pWnd = pGrid->m_pGridWnd->GetParent();
		if (pWnd == NULL)
			pWnd = pGrid->m_pGridWnd->GetTopWindow();
		if (pWnd)
			pWnd->GetClientRect(&rectGrid);
		else
			rectGrid.SetRect(0,0,32767,32767);
	}

	TRY
	{
		BOOL bAbort;
		CGXRange rgCovered;
		for (ROWCOL nRow = range.top; nRow <= range.bottom; nRow++)
		{
			if (nRow == 0)
				nMaxHeight = max(0, rectGrid.Height() - 1);
			else if (nRow <= nfr)
				nMaxHeight = max(0, rectGrid.Height() - pGrid->CalcSumOfRowHeights(0, nRow-1) - 1);
			else
				nMaxHeight = max(0, rectGrid.Height() - pGrid->CalcSumOfRowHeights(0, nfr) - 1);

			int nHeight;
			nHeight = 0;
			for (ROWCOL nCol = range.left; nCol <= range.right; nCol++)
			{
				BOOL bCovered = pGrid->GetCellRangeRowCol(nRow, nCol, rgCovered) != NULL;

				// Skip invisible columns
				BOOL bCanResize = FALSE;

				// Covered cells
				if (bCovered)
				{
					if (bResizeCoveredCells)
					{
						bCanResize =
							nCol == rgCovered.left // must be the first covered column
							&& nRow == rgCovered.bottom // and the last covered row
							// all cells of covered cell must be with in range to be resized
							&& range.top <= rgCovered.top && range.right >= rgCovered.right;
					}
				}
				else
					// skip invisible columns
					bCanResize = pGrid->GetColWidth(nCol) > 0;

				if (bCanResize)
				{
					CGXStyle* pStyle = pGrid->CreateStyle();

					CSize size;

					if (bCovered)
					{
						pGrid->ComposeStyleRowCol(rgCovered.top, rgCovered.left, pStyle);
						CGXControl* pControl = pGrid->GetRegisteredControl(pStyle->GetControl());
						pControl->LoadStyle(rgCovered.top, rgCovered.left, pStyle);

						size = pControl->CalcSize(&dc, rgCovered.top, rgCovered.left, *pStyle, pStyleStandard, TRUE);

						// Subtract row heights of previous rows (only the last row can be resized)
						if (nRow > rgCovered.top)
							size.cy -= pGrid->CalcSumOfRowHeights(rgCovered.top, nRow-1);
					}
					else
					{
						pGrid->ComposeStyleRowCol(nRow, nCol, pStyle);
						CGXControl* pControl = pGrid->GetRegisteredControl(pStyle->GetControl());
						pControl->LoadStyle(nRow, nCol, pStyle);

						size = pControl->CalcSize(&dc, nRow, nCol, *pStyle, pStyleStandard, TRUE);
					}

					nHeight = max(nHeight, size.cy);

					// Style object recycling (CreateStyle will use this style-object again)
					pGrid->RecycleStyle(pStyle);
				}

				theOp.DoMessages(bAbort);
				if (bAbort)
					AfxThrowUserException();

				if (bCovered)
					// Skip covered cells
					nCol = rgCovered.right;
			}

			nHeight = min(nMaxHeight, nHeight);

			if (nFlags & GX_NOSHRINKSIZE)
			{
				if (nHeight > pGrid->GetRowHeight(nRow))
					pGrid->SetRowHeight(nRow, nRow, nHeight, NULL, nFlags);
			}
			else
			{
				if (nHeight != pGrid->GetRowHeight(nRow) && nHeight > 0)
					pGrid->SetRowHeight(nRow, nRow, nHeight, NULL, nFlags);
			}
		}

		if (hOldFont)
			dc.SelectObject(CFont::FromHandle(hOldFont));

		pGrid->CommitTrans();
	}
	CATCH(CUserException, e)
	{
		if (hOldFont)
			dc.SelectObject(CFont::FromHandle(hOldFont));

		pGrid->Rollback();
		return FALSE;
	}
	END_CATCH

	return TRUE;
}

BOOL CGXGridResizeToFitImp::ResizeColWidthsToFit(CGXGridCore* pGrid, CGXRange& range, BOOL bResizeCoveredCells, UINT nFlags)
{
	CClientDC dc(pGrid->m_pGridWnd);
	pGrid->OnGridPrepareDC(&dc);

	// select default font into device context
	CGXStyle* pStyleStandard = &pGrid->StandardStyle();
	CFont* pOldFont = pGrid->LoadFont(&dc, *pStyleStandard);

	// Get the HFONT handle because pOldFont is only a temporary CGdiObject object 
	// and is only valid until the next time the application has idle time in its 
	// event loop.
	HFONT hOldFont = (HFONT) pOldFont->GetSafeHandle();

	range.ExpandRange(0, 0, pGrid->GetRowCount(), pGrid->GetColCount());

	pGrid->EvalMergeCells(range);
	pGrid->EvalFloatCells(range);

	CGXLongOperation theOp;
	theOp.SetStatusText(pGrid->m_pAppData->strmResizeCols);
	pGrid->BeginTrans(pGrid->m_pAppData->strmResizeCols);

	int nMaxWidth;
	ROWCOL nfr = pGrid->GetFrozenCols();
	CRect rectGrid = pGrid->GetGridRect();

	if (rectGrid.Width() == NULL)
	{
		TRACE(
			_T("Warning: ResizeColWidthsToFit called for a GridWnd which client's area is CSize(0,0)\n")
			_T("It is possible that sized columns do not fit into the grid area\n"));
		CWnd* pWnd = pGrid->m_pGridWnd->GetParent();
		if (pWnd == NULL)
			pWnd = pGrid->m_pGridWnd->GetTopWindow();
		if (pWnd)
			pWnd->GetClientRect(&rectGrid);
		else
			rectGrid.SetRect(0,0,32767,32767);
	}

	TRY
	{
		BOOL bAbort;
		CGXRange rgCovered;
		for (ROWCOL nCol = range.left; nCol <= range.right; nCol++)
		{
			if (nCol == 0)
				nMaxWidth = max(0, rectGrid.Width() - 1);
			else if (nCol <= nfr)
				nMaxWidth = max(0, rectGrid.Width() - pGrid->CalcSumOfColWidths(0, nCol-1) - 1);
			else
				nMaxWidth = max(0, rectGrid.Width() - pGrid->CalcSumOfColWidths(0, nfr) - 1);

			int nWidth;
			nWidth = 0;
			for (ROWCOL nRow = range.top; nRow <= range.bottom; nRow++)
			{
				BOOL bCovered = pGrid->GetCellRangeRowCol(nRow, nCol, rgCovered) != NULL;

				// Skip invisible rows
				BOOL bCanResize = FALSE;

				// Covered cells
				if (bCovered)
				{
					if (bResizeCoveredCells)
					{
						bCanResize =
							nRow == rgCovered.top // must be the first covered row
							&& nCol == rgCovered.right // and the last covered column
							// all cells of covered cell must be with in range to be resized
							&& range.left <= rgCovered.left && range.bottom >= rgCovered.bottom;
					}
				}
				else
					// skip invisible rows
					bCanResize = pGrid->GetRowHeight(nRow) > 0;

				if (bCanResize)
				{
					CGXStyle* pStyle = pGrid->CreateStyle();

					CSize size;

					if (bCovered)
					{
						pGrid->ComposeStyleRowCol(rgCovered.top, rgCovered.left, pStyle);
						CGXControl* pControl = pGrid->GetRegisteredControl(pStyle->GetControl());
						pControl->LoadStyle(rgCovered.top, rgCovered.left, pStyle);

						size = pControl->CalcSize(&dc, rgCovered.top, rgCovered.left, *pStyle, pStyleStandard, FALSE);

						// Subtract row heights of previous rows (only the last row can be resized)
						if (nCol > rgCovered.left)
							size.cx -= pGrid->CalcSumOfColWidths(rgCovered.left, nCol-1);
					}
					else
					{
						pGrid->ComposeStyleRowCol(nRow, nCol, pStyle);
						CGXControl* pControl = pGrid->GetRegisteredControl(pStyle->GetControl());
						pControl->LoadStyle(nRow, nCol, pStyle);

						size = pControl->CalcSize(&dc, nRow, nCol, *pStyle, pStyleStandard, FALSE);
					}

					nWidth = max(nWidth, size.cx);

					// Style object recycling (CreateStyle will use this style-object again)
					pGrid->RecycleStyle(pStyle);
				}

				theOp.DoMessages(bAbort);
				if (bAbort)
					AfxThrowUserException();

				if (bCovered)
					// Skip covered cells
					nRow = rgCovered.bottom;
			}

			nWidth = min(nMaxWidth, nWidth);

			if (nFlags & GX_NOSHRINKSIZE)
			{
				if (nWidth > pGrid->GetColWidth(nCol))
					pGrid->SetColWidth(nCol, nCol, nWidth, NULL, nFlags);
			}
			else
			{
				if (nWidth != pGrid->GetColWidth(nCol) && nWidth > 0)
					pGrid->SetColWidth(nCol, nCol, nWidth, NULL, nFlags);
			}
		}

		if (hOldFont)
			dc.SelectObject(CFont::FromHandle(hOldFont));

		pGrid->CommitTrans();
	}
	CATCH(CUserException, e)
	{
		if (hOldFont)
			dc.SelectObject(CFont::FromHandle(hOldFont));

		pGrid->Rollback();
		return FALSE;
	}
	END_CATCH

	return TRUE;
}
