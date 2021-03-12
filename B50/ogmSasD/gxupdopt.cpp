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

// gxupdopt.cpp : optimized updating for insert, removing rows
//				  and columns and resizing rows and column

#include "stdafx.h"

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

// Headers

#ifndef _GXRESRC_H_
#include "gxresrc.h"
#endif

#ifndef _GXEXT_H_
#include "gxext.h"
#endif

#ifndef _GXCORE_H_
#include "gxcore.h"
#endif

#ifndef _GXCTRL_H_
#include "gxctrl.h"
#endif

#ifndef _GXMSG_H_
#include "gxmsg.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCORMSG")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

class CGXGridOptimizedUpdateImp: public CGXAbstractGridOptimizedUpdateImp
{
public:
	virtual void UpdateInsertRows(CGXGridCore* pGrid, ROWCOL& nRow, ROWCOL& nCount, UINT& flags);
	virtual void UpdateInsertCols(CGXGridCore* pGrid, ROWCOL& nCol, ROWCOL& nCount, UINT& flags);
	virtual void UpdateRemoveRows(CGXGridCore* pGrid, ROWCOL& nFromRow, ROWCOL& nToRow, int* anOldHeights, UINT flags);
	virtual void UpdateRemoveCols(CGXGridCore* pGrid, ROWCOL& nFromCol, ROWCOL& nToCol, int* anOldWidths, UINT& flags);
	virtual void UpdateChangedRowHeights(CGXGridCore* pGrid, ROWCOL& nFromRow, ROWCOL& nToRow, int* nOldHeights, UINT& flags);
	virtual void UpdateChangedColWidths(CGXGridCore* pGrid, ROWCOL& nFromCol, ROWCOL& nToCol, int* nOldWidths, UINT& flags);
};

void CGXGridCore::ImplementOptimizedUpdate()
{
	if (m_pOptimizedUpdateImp == NULL)
		AutoDeletePtr(m_pOptimizedUpdateImp = new CGXGridOptimizedUpdateImp);
}

void CGXGridOptimizedUpdateImp::UpdateInsertRows(CGXGridCore* pGrid, ROWCOL& nRow, ROWCOL& nCount, UINT& flags)
{
	if (pGrid->IsFrozenRow(nRow)
		|| pGrid->GetFloatCellsMode()
		|| pGrid->GetMergeCellsMode())
	{
		pGrid->Redraw(flags, FALSE);
	}
	else
	{
		flags &= (GX_INVALIDATE | GX_SMART | GX_UPDATENOW);
		CRect rect = pGrid->GetGridRect();

		if (nRow < pGrid->m_nTopRow)
		{
			pGrid->m_nTopRow += nCount;
		}
		else if (nRow <= pGrid->CalcBottomRowFromRect(pGrid->GetGridRect()))
		{
			// Insertion is Visible
			rect.top += pGrid->CalcSumOfClientRowHeights(0, pGrid->GetClientRow(nRow)-1, rect.Height());
			int nInsHeight = pGrid->CalcSumOfRowHeights(nRow, nRow+nCount-1, rect.bottom-rect.top);
			if (flags & GX_INVALIDATE
				|| rect.top+nInsHeight > rect.bottom
				|| nRow+nCount >= pGrid->GetRowCount())
			{
				rect = pGrid->CalcRectFromRowColExEx(nRow, 0, pGrid->GetRowCount(), pGrid->GetColCount());
				rect.bottom++;
				rect.right++;
				pGrid->InvalidateRect(&rect);
			}
			else if (flags & (GX_SMART | GX_UPDATENOW))
			{
				pGrid->ScrollWindowEx(0, nInsHeight, &rect, pGrid->GetGridRect(),
				NULL, NULL, SW_INVALIDATE | SW_SCROLLCHILDREN);
				rect = pGrid->CalcRectFromRowColExEx(nRow, 0, nRow+nCount, pGrid->GetColCount());
				pGrid->InvalidateRect(&rect);
			}
		}
		if (flags & GX_UPDATENOW)
			pGrid->UpdateWindow();

		if (pGrid->GetParam()->m_bNumberedRowHeaders)
			pGrid->RedrawRowCol(nRow+nCount, 0, pGrid->GetRowCount(), 0, flags, FALSE);
	}
}

void CGXGridOptimizedUpdateImp::UpdateInsertCols(CGXGridCore* pGrid, ROWCOL& nCol, ROWCOL& nCount, UINT& flags)
{
	if (pGrid->IsFrozenCol(nCol)
		|| pGrid->GetFloatCellsMode()
		|| pGrid->GetMergeCellsMode())
	{
		pGrid->Redraw(flags, FALSE);
	}
	else
	{
		flags &= (GX_INVALIDATE | GX_SMART | GX_UPDATENOW);
		CRect rect = pGrid->GetGridRect();

		if (nCol < pGrid->m_nLeftCol)
		{
			pGrid->m_nLeftCol += nCount;
		}
		else if (nCol <= pGrid->CalcRightColFromRect(rect))
		{
			// Insertion is Visible
			rect.left += pGrid->CalcSumOfClientColWidths(0, pGrid->GetClientCol(nCol)-1, rect.Width());
			int nInsWidth = pGrid->CalcSumOfColWidths(nCol, nCol+nCount-1, rect.Width());

			if (flags & GX_INVALIDATE
				|| rect.left+nInsWidth > rect.right
				|| nCol+nCount >= pGrid->GetColCount())
			{
				rect = pGrid->CalcRectFromRowColExEx(0, nCol, pGrid->GetRowCount(), pGrid->GetColCount());
				rect.bottom++;
				rect.right++;
				pGrid->InvalidateRect(&rect);
			}
			else if (flags & (GX_SMART | GX_UPDATENOW))
			{
				pGrid->ScrollWindowEx(nInsWidth, 0, rect, rect,
				NULL, NULL, SW_INVALIDATE | SW_SCROLLCHILDREN);
				rect = pGrid->CalcRectFromRowColExEx(0, nCol, pGrid->GetRowCount(), nCol+nCount);
				pGrid->InvalidateRect(&rect);
			}
		}
		if (flags & GX_UPDATENOW)
			pGrid->UpdateWindow();

		if (pGrid->GetParam()->m_bNumberedColHeaders)
			pGrid->RedrawRowCol(0, nCol+nCount, 0, pGrid->GetColCount(), flags, FALSE);
	}
}

void CGXGridOptimizedUpdateImp::UpdateRemoveRows(CGXGridCore* pGrid, ROWCOL& nFromRow, ROWCOL& nToRow, int* anOldHeights, UINT flags)
{
	if (flags == GX_INVALIDATE
		|| pGrid->IsFrozenRow(nFromRow)
		|| pGrid->GetFloatCellsMode()
		|| pGrid->GetMergeCellsMode())
	{
		pGrid->Redraw(flags, FALSE);
	}
	else
	{
		ROWCOL nCount = nToRow-nFromRow+1;
		flags &= (GX_INVALIDATE | GX_SMART | GX_UPDATENOW);

		CRect rect     = pGrid->GetGridRect();
		ROWCOL rightRow = pGrid->CalcBottomRowFromRect(rect);

		// All rows in grid were removed
		if (pGrid->GetRowCount() == 0)
		{
			pGrid->m_nTopRow = 1;
			if (flags)
				pGrid->InvalidateRect(pGrid->GetGridRect());
		}
		// All displayed rows were removed
		else if (pGrid->m_nTopRow > pGrid->GetRowCount())
		{
			pGrid->m_nTopRow = max(1, pGrid->GetRowCount());
			if (flags)
				pGrid->InvalidateRect(pGrid->GetGridRect());
		}
		// Rows above display were removed
		else if (nToRow < pGrid->m_nTopRow)
		{
			pGrid->m_nTopRow -= nCount;
		}
		// Bottom rows in display were removed
		else if (nFromRow > rightRow && rightRow == pGrid->GetRowCount())
		{
			if (flags)
				pGrid->InvalidateRect(pGrid->CalcClientRectBottomFromRow(rightRow));
		}
		// Rows below display were removed
		else if (nFromRow > rightRow /* && rightRow < pGrid->GetRowCount() */)
		{
			// nothing to do
		}
		// Rows above display and from display were removed
		else if (nToRow >= pGrid->m_nTopRow && nFromRow < pGrid->m_nTopRow)
		{
			rect.top += pGrid->CalcSumOfClientRowHeights(0, pGrid->GetFrozenRows(), rect.Height());

			if (flags & GX_INVALIDATE || anOldHeights == NULL)
				pGrid->InvalidateRect(&rect);
			else
			{
				int nOldHeight = 0;
				for (ROWCOL n = pGrid->m_nTopRow-nFromRow; n < nCount && nOldHeight < rect.Height(); n++)
					nOldHeight += anOldHeights[n];
				pGrid->m_nTopRow = max(1, nFromRow);

				if (nOldHeight > rect.Height())
					pGrid->InvalidateRect(&rect);
				else if (flags & (GX_SMART | GX_UPDATENOW))
				{
					pGrid->ScrollWindowEx(0, -nOldHeight, &rect, &rect,
						NULL, NULL, SW_INVALIDATE | SW_SCROLLCHILDREN);

					// avoid only partial invalid cells
					CPoint pt(0, pGrid->GetGridRect().bottom-nOldHeight);
					ROWCOL nRow = pGrid->CalcClientRowFromPt(pt);
					if (nRow != GX_INVALID)
					{
						CRect r = pGrid->GetGridRect();
						r.top += pGrid->CalcSumOfClientRowHeights(0, max(1,nRow)-1);
						pGrid->InvalidateRect(r);
						pGrid->UpdateWindow();
					}
				}
			}
		}
		// Rows were visible
		else if (nFromRow >= pGrid->m_nTopRow && nFromRow <= pGrid->CalcBottomRowFromRect(pGrid->GetGridRect()))
		{
			rect.top += pGrid->CalcSumOfClientRowHeights(0, pGrid->GetClientRow(nFromRow)-1, rect.Height());

			if (flags & GX_INVALIDATE || anOldHeights == NULL)
				pGrid->InvalidateRect(&rect);
			else
			{
				int nOldHeight = 0;
				for (ROWCOL n = 0; n < nCount && nOldHeight < rect.Height(); n++)
					nOldHeight += anOldHeights[n];

				if (nOldHeight > rect.Height())
					pGrid->InvalidateRect(&rect);
				else if (flags & (GX_SMART | GX_UPDATENOW))
				{
					pGrid->ScrollWindowEx(0, -nOldHeight, &rect, &rect,
						NULL, NULL, SW_INVALIDATE | SW_SCROLLCHILDREN);

					// avoid only partial invalid cells
					CPoint pt(0, pGrid->GetGridRect().bottom-nOldHeight);
					ROWCOL nRow = pGrid->CalcClientRowFromPt(pt);
					if (nRow != GX_INVALID)
					{
						CRect r = pGrid->GetGridRect();
						r.top += pGrid->CalcSumOfClientRowHeights(0, max(1,nRow)-1);
						pGrid->InvalidateRect(r);
						pGrid->UpdateWindow();
					}
				}
			}
		}

		POSITION pos = pGrid->GetParam()->GetCoveredCellsList()->GetHeadPosition();
		while (pos)
		{
			CGXRange* r = pGrid->GetParam()->GetCoveredCellsList()->GetNext(pos);
			if (r->top < nFromRow && r->bottom >= nFromRow)
				pGrid->InvalidateRect(pGrid->CalcRectFromRowCol(r->top, r->left, r->bottom, r->right));
		}

		if (flags & GX_UPDATENOW)
			pGrid->UpdateWindow();

		if (pGrid->GetParam()->m_bNumberedRowHeaders)
			pGrid->RedrawRowCol(nFromRow, 0, pGrid->GetRowCount(), 0, flags, FALSE);
	}
}

void CGXGridOptimizedUpdateImp::UpdateRemoveCols(CGXGridCore* pGrid, ROWCOL& nFromCol, ROWCOL& nToCol, int* anOldWidths, UINT& flags)
{
	if (flags == GX_INVALIDATE
		|| pGrid->IsFrozenCol(nFromCol)
		|| pGrid->GetFloatCellsMode()
		|| pGrid->GetMergeCellsMode())
	{
		pGrid->Redraw(flags, FALSE);
	}
	else
	{
		ROWCOL nCount = nToCol-nFromCol+1;
		flags &= (GX_INVALIDATE | GX_SMART | GX_UPDATENOW);

		CRect rect     = pGrid->GetGridRect();
		CRect rectGrid = pGrid->GetGridRect();
		ROWCOL rightCol = pGrid->CalcRightColFromRect(rect);

		// All columns were removed
		if (pGrid->GetColCount() == 0)
		{
			pGrid->m_nLeftCol = 1;
			if (flags)
				pGrid->InvalidateRect(pGrid->GetGridRect());
		}
		// All visible columns were removed
		else if (pGrid->m_nLeftCol > pGrid->GetColCount())
		{
			pGrid->m_nLeftCol = max(1, pGrid->GetColCount());
			if (flags)
				pGrid->InvalidateRect(pGrid->GetGridRect());
		}
		// Columns left from visible area were removed
		else if (nToCol < pGrid->m_nLeftCol)
		{
			pGrid->m_nLeftCol -= nCount;
		}
		// Right Columns from visible area were removed
		else if (nFromCol > rightCol && rightCol == pGrid->GetColCount())
		{
			if (flags)
				pGrid->InvalidateRect(pGrid->CalcClientRectRightFromCol(rightCol));
		}
		// Columns right of display were removed
		else if (nFromCol > rightCol /* && rightCol < pGrid->GetColCount() */)
		{
			// nothing to do
		}
		// Removed columns were left of and in display area
		else if (nToCol >= pGrid->m_nLeftCol && nFromCol <= pGrid->m_nLeftCol)
		{
			rect.left += pGrid->CalcSumOfClientColWidths(0, pGrid->GetFrozenCols(), rect.Width());

			if (flags & GX_INVALIDATE || anOldWidths == NULL)
				pGrid->InvalidateRect(&rect);
			else
			{
				int nOldWidth = 0;
				for (ROWCOL n = pGrid->m_nLeftCol-nFromCol; n < nCount && nOldWidth < rect.Width(); n++)
					nOldWidth += anOldWidths[n];
				pGrid->m_nLeftCol = max(1, nFromCol);

				if (nOldWidth > rect.Width())
					pGrid->InvalidateRect(&rect);
				else if (flags)
				{
					pGrid->ScrollWindowEx(-nOldWidth, 0, &rect, &rect,
						NULL, NULL, SW_INVALIDATE | SW_SCROLLCHILDREN);

					// avoid only partial invalid cells
					CPoint pt(pGrid->GetGridRect().right-nOldWidth, 0);
					ROWCOL nCol = pGrid->CalcClientColFromPt(pt);
					if (nCol != GX_INVALID)
					{
						CRect r = pGrid->GetGridRect();
						r.left += pGrid->CalcSumOfClientColWidths(0, max(1,nCol-1));
						pGrid->InvalidateRect(r);
						pGrid->UpdateWindow();
					}
				}
			}
		}
		// Columns were visible
		else if (nFromCol > pGrid->m_nLeftCol && nFromCol <= pGrid->CalcRightColFromRect(pGrid->GetGridRect()))
		{
			rect.left += pGrid->CalcSumOfClientColWidths(0, pGrid->GetClientCol(nFromCol)-1, rect.Width());

			if (flags & GX_INVALIDATE || anOldWidths == NULL)
				pGrid->InvalidateRect(&rect);
			else
			{
				int nOldWidth = 0;
				for (ROWCOL n = 0; n < nCount && nOldWidth < rect.Width(); n++)
					nOldWidth += anOldWidths[n];

				if (nOldWidth > rect.Width())
					pGrid->InvalidateRect(&rect);
				else if (flags)
				{
					pGrid->ScrollWindowEx(-nOldWidth, 0, &rect, &rect,
						NULL, NULL, SW_INVALIDATE | SW_SCROLLCHILDREN);

					// avoid only partial invalid cells
					CPoint pt(pGrid->GetGridRect().right-nOldWidth, 0);
					ROWCOL nCol = pGrid->CalcClientColFromPt(pt);
					if (nCol != GX_INVALID)
					{
						CRect r = pGrid->GetGridRect();
						r.left += pGrid->CalcSumOfClientColWidths(0, max(1,nCol-1));
						pGrid->InvalidateRect(r);
						pGrid->UpdateWindow();
					}
				}
			}
		}

		POSITION pos = pGrid->GetParam()->GetCoveredCellsList()->GetHeadPosition();
		while (pos)
		{
			CGXRange* r = pGrid->GetParam()->GetCoveredCellsList()->GetNext(pos);
			if (r->left < nFromCol && r->right >= nFromCol)
				pGrid->InvalidateRect(pGrid->CalcRectFromRowCol(r->top, r->left, r->bottom, r->right));
		}

		if (flags & GX_UPDATENOW)
			pGrid->UpdateWindow();

		if (pGrid->GetParam()->m_bNumberedColHeaders)
			pGrid->RedrawRowCol(0, nFromCol, 0, pGrid->GetColCount(), flags, FALSE);
	}
}

void CGXGridOptimizedUpdateImp::UpdateChangedRowHeights(CGXGridCore* pGrid, ROWCOL& nFromRow, ROWCOL& nToRow, int* nOldHeights, UINT& flags)
{
	BOOL bRedrawAll = pGrid->EvalVisibleFloatMergeCells(CGXRange().SetRows(nFromRow, nToRow), FALSE);

	if (bRedrawAll)
		pGrid->Redraw(flags, FALSE);
	else
	{
		if (nToRow == 0)
			nToRow = nFromRow;

		flags &= (GX_INVALIDATE | GX_SMART | GX_UPDATENOW);

		ASSERT(nToRow >= nFromRow);

		int nOldHeight = 0,
			 nNewHeight = 0;
		BOOL bAbort = FALSE;
		CRect rect = pGrid->GetGridRect();

		if (pGrid->m_pSplitterWnd && !pGrid->IsPrinting() && !pGrid->m_bDisplayHeaderRow && nFromRow <= pGrid->GetHeaderRows())
		{
			// Lower pane in dynamic splitter view normally has no headers.
			nOldHeights = NULL; // don't use previous row heights

			if (nToRow <= pGrid->GetHeaderRows())
				bAbort = TRUE; // No update necessary
		}

		if (!bAbort)
		{
			// New Right Row calculated of new Heights
			ROWCOL nBottomRow = pGrid->CalcBottomRowFromRect(rect);

			// Not in visible Area
			ROWCOL nfc = pGrid->GetFrozenRows();
			ROWCOL nTopRow = pGrid->GetTopRow();
			if (nFromRow > nfc && nFromRow < nTopRow
					&& nToRow > nfc && nToRow < nTopRow
					|| nFromRow > nBottomRow)
				bAbort = TRUE;
			else
			{

				ROWCOL nEditRow, nEditCol;
				if (pGrid->GetCurrentCell(nEditRow, nEditCol) && nEditRow >= nFromRow && nEditRow <= nToRow)
				{
					CGXControl* pControl = pGrid->GetControl(nEditRow, nEditCol);
					if (pControl->IsActive())
						pControl->Hide();
				}


				ROWCOL nFirstVisible = GX_INVALID;
				if (nFromRow > pGrid->m_nTopRow)
					nFirstVisible = pGrid->GetClientRow(nFromRow);
				else if (nFromRow <= pGrid->GetFrozenRows())
					nFirstVisible = nFromRow;

				if (nFirstVisible > 0 && nFirstVisible != GX_INVALID)
					rect.top += pGrid->CalcSumOfClientRowHeights(0, nFirstVisible-1, rect.Height());

				if (flags & GX_INVALIDATE || nOldHeights == NULL || nFirstVisible == GX_INVALID)
				{
					pGrid->InvalidateRect(&rect);
					bAbort = TRUE;
				}
				else
				{
					// Sum of Old Heights of visible Rows
					nOldHeight = 0;
					for (ROWCOL n = nFirstVisible; n <= pGrid->GetClientRow(nToRow); n++)
						nOldHeight += nOldHeights[pGrid->GetRow(n)-nFromRow];

					nNewHeight = pGrid->CalcSumOfClientRowHeights(nFirstVisible, pGrid->GetClientRow(nToRow), rect.Height());
				}
			}
		}

		// Smarter Redrawing
		if (!bAbort && flags)
		{
			if (nOldHeight < rect.Height())
			{
				CRect rectScroll = pGrid->GetGridRect();
				rectScroll.top = rect.top + nOldHeight;
				pGrid->ScrollWindowEx(0, nNewHeight-nOldHeight, &rectScroll, &rect,
					NULL, NULL, SW_INVALIDATE | SW_SCROLLCHILDREN);
				if (flags & (GX_SMART | GX_UPDATENOW) && nNewHeight < nOldHeight
					&& nOldHeight - nNewHeight < rectScroll.bottom)
				{
					// this rectangle is scrolled into the view
					CPoint pt(0, rectScroll.bottom-(nOldHeight-nNewHeight));
					ROWCOL nRow = pGrid->CalcClientRowFromPt(pt);

					if (nRow == GX_INVALID)
						nRow = pGrid->GetClientRow(pGrid->GetRowCount());

					CRect r = pGrid->GetGridRect();
					r.top += pGrid->CalcSumOfClientRowHeights(0, max(1,nRow)-1);
					pGrid->InvalidateRect(r);
					pGrid->UpdateWindow();
				}

				// this rectangle is the changed row
				rect = pGrid->CalcRectFromRowColExEx(nFromRow, 0, nToRow, pGrid->GetColCount());
				// rect.bottom = min(rectScroll.top, rect.top + nNewHeight);
			}

			pGrid->InvalidateRect(&rect);
		}

		if (flags & GX_UPDATENOW)
		{
			pGrid->UpdateWindow();
			pGrid->UpdateScrollbars(FALSE);
		}
	}
}

void CGXGridOptimizedUpdateImp::UpdateChangedColWidths(CGXGridCore* pGrid, ROWCOL& nFromCol, ROWCOL& nToCol, int* nOldWidths, UINT& flags)
{
	BOOL bRedrawAll = pGrid->EvalVisibleFloatMergeCells(CGXRange().SetCols(nFromCol, nToCol+1), FALSE);

	if (bRedrawAll)
		pGrid->Redraw(flags, FALSE);
	else
	{
		if (nToCol == 0)
			nToCol = nFromCol;

		flags &= (GX_INVALIDATE | GX_SMART | GX_UPDATENOW);

		ASSERT(nToCol >= nFromCol);

		int nOldWidth = 0,
			 nNewWidth = 0;
		BOOL bAbort = FALSE;
		CRect rect = pGrid->GetGridRect();

		if (pGrid->m_pSplitterWnd && !pGrid->IsPrinting() && !pGrid->m_bDisplayHeaderCol && nFromCol <= pGrid->GetHeaderCols())
		{
			// Right pane in dynamic splitter view normally has no headers.
			nOldWidths = NULL;  // don't use previous row heights

			if (nToCol <= pGrid->GetHeaderCols())
				bAbort = TRUE; // No update necessary
		}

		if (!bAbort)
		{
			// New Right Col calculated of new Widths
			ROWCOL nRightCol = pGrid->CalcRightColFromRect(rect);

			// Not in visible Area
			ROWCOL nfc = pGrid->GetFrozenCols();
			ROWCOL nLeftCol = pGrid->GetLeftCol();

			if (nFromCol > nfc && nFromCol < nLeftCol
					&& nToCol > nfc && nToCol < nLeftCol
					|| nFromCol > nRightCol)
				bAbort = TRUE;
			else
			{
				ROWCOL nEditRow, nEditCol;
				if (pGrid->GetCurrentCell(nEditRow, nEditCol) && nEditCol >= nFromCol && nEditCol <= nToCol)
				{
					CGXControl* pControl = pGrid->GetControl(nEditRow, nEditCol);
					if (pControl->IsActive())
						pControl->Hide();
				}

				ROWCOL nFirstVisible = GX_INVALID;
				if (nFromCol > pGrid->m_nLeftCol)
					nFirstVisible = pGrid->GetClientCol(nFromCol);
				else if (nFromCol <= pGrid->GetFrozenCols())
					nFirstVisible = nFromCol;

				if (nFirstVisible > 0 && nFirstVisible != GX_INVALID)
					rect.left += pGrid->CalcSumOfClientColWidths(0, nFirstVisible-1, rect.Width());

				if (flags & GX_INVALIDATE || nOldWidths == NULL || nFirstVisible == GX_INVALID)
				{
					pGrid->InvalidateRect(&rect);
					bAbort = TRUE;
				}
				else
				{
					// Sum of Old Widths of visible Cols
					ROWCOL nTo = max(pGrid->GetClientCol(nToCol), nFirstVisible);
					nOldWidth = 0;
					for (ROWCOL n = nFirstVisible; n <= nTo; n++)
						nOldWidth += nOldWidths[pGrid->GetCol(n)-nFromCol];

					nNewWidth = pGrid->CalcSumOfClientColWidths(nFirstVisible, nTo, rect.Width());
				}
			}
		}

		if (!bAbort && flags)
		{
			if (nOldWidth < rect.Width())
			{
				// Smarter Redrawing
				CRect rectScroll = pGrid->GetGridRect();
				rectScroll.left = rect.left + nOldWidth;
				pGrid->ScrollWindowEx(nNewWidth-nOldWidth, 0, &rectScroll, &rect,
					NULL, NULL, SW_INVALIDATE | SW_SCROLLCHILDREN);

				if (flags & (GX_SMART | GX_UPDATENOW) && nNewWidth < nOldWidth
					&& nOldWidth - nNewWidth < rectScroll.right)
				{
					// this rectangle is scrolled into the view
					CPoint pt(rectScroll.right-(nOldWidth-nNewWidth), 0);
					ROWCOL nCol = pGrid->CalcClientColFromPt(pt);

					if (nCol == GX_INVALID)
						nCol = pGrid->GetClientCol(pGrid->GetColCount());

					CRect r = pGrid->GetGridRect();
					r.left += pGrid->CalcSumOfClientColWidths(0, max(1,nCol)-1);
					pGrid->InvalidateRect(r);
					pGrid->UpdateWindow();
				}

				// this rectangle is the column
				rect = pGrid->CalcRectFromRowColExEx(0, nFromCol, pGrid->GetRowCount(), nToCol);
			}
			// rect.right = min(rectScroll.left, rect.left+nNewWidth);
			pGrid->InvalidateRect(&rect);
		}

		if (flags & GX_UPDATENOW)
		{
			pGrid->UpdateWindow();
			pGrid->UpdateScrollbars(FALSE);
		}
	}
}

