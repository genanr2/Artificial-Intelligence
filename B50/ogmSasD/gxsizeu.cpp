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

// gxsizeu.cpp : let user resize rows and columns with mouse
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

#include "gxpriv.h"

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

class CGXGridUserResizeCellsImp: public CGXAbstractGridUserResizeCellsImp
{
public:
	virtual BOOL OnMouseMove(CGXGridCore* pGrid, CPoint& point, UINT& flags);
	virtual BOOL OnLButtonDown(CGXGridCore* pGrid, CPoint& point, UINT& flags, int& ht, ROWCOL& nRow, ROWCOL& nCol);
	virtual BOOL OnLButtonUp(CGXGridCore* pGrid, CPoint& point, UINT& flags);
	virtual BOOL OnLButtonDblClk(CGXGridCore* pGrid, CPoint& point, UINT& flags, int& ht, ROWCOL& nRow, ROWCOL& nCol);
	virtual BOOL OnStartTracking(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, int nTrackingMode);
	virtual void OnMoveTracking(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, int nTrackingMode, CSize &size);
	virtual void OnCancelTracking(CGXGridCore* pGrid);
	virtual void OnEndTracking(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, int nTrackingMode, CSize& size);
};

void CGXGridCore::ImplementUserResizeCells()
{
	if (m_pUserResizeCellsImp == NULL)
		AutoDeletePtr(m_pUserResizeCellsImp = new CGXGridUserResizeCellsImp);
}

BOOL CGXGridUserResizeCellsImp::OnMouseMove(CGXGridCore* pGrid, CPoint& point, UINT& flags)
{
	GX_UNUSED_ALWAYS(flags);

	if (pGrid->m_bTracking)
	{
		// changig row or column size
		CSize size = point-pGrid->m_rectBoundTrack.TopLeft();

		if (pGrid->m_nTrackingMode == GX_TRACKHEIGHT)
			size.cx = 0;
		else
			size.cy = 0;

		// OnMoveTracking will call OnTrackColWidthMove to ensure that
		// the new size is recoginzed.
		//
		// OnMoveTracking will also check pGrid->GetParam()->m_nMinTrackColWidth
		// or pGrid->GetParam()->m_nMinTrackColWidth

		pGrid->OnMoveTracking(pGrid->m_nRowTracking, pGrid->m_nColTracking, pGrid->m_nTrackingMode, size);

		return TRUE;
	}

	return FALSE;
}

BOOL CGXGridUserResizeCellsImp::OnLButtonDown(CGXGridCore* pGrid, CPoint& point, UINT& flags, int& ht, ROWCOL& nRow, ROWCOL& nCol)
{
	GX_UNUSED_ALWAYS(flags);
	GX_UNUSED_ALWAYS(point);

	if (ht == GX_VERTLINE && pGrid->m_pParam->IsEnableTrackColWidth())
	{
		// OnStartTracking will call OnTrackColWidth(nCol) and
		// OnTrackColWidthStart(nCol) to ensure that tracking is possible.
		//
		// The programmer can override this methods to inhibit the
		// tracking feature for specific columns.

		if (pGrid->OnStartTracking(nRow, nCol, GX_TRACKWIDTH)
			&& pGrid->GetCapture() != pGrid->m_pGridWnd)

			pGrid->SetCapture();

		return TRUE;
	}

	// check, if user clicked on a horizontal line to resize the row
	// ... (behaviour can be changed with pGrid->GetParam()->EnableTrackRowHeight(...)
	// and pGrid->GetParam()->SetColHeaderLines(...)

	else if (ht == GX_HORZLINE && pGrid->m_pParam->IsEnableTrackRowHeight())
	{
		// OnStartTracking will call OnTrackRowHeight(nRow) and
		// OnTrackRowHeightStart(nRow) to ensure that tracking is possible.
		//
		// The programmer can override this methods to inhibit the
		// tracking feature for specific rows.

		if (pGrid->OnStartTracking(nRow, nCol, GX_TRACKHEIGHT)
			&& pGrid->GetCapture() != pGrid->m_pGridWnd)

			pGrid->SetCapture();

		return TRUE;
	}

	return FALSE;
}

BOOL CGXGridUserResizeCellsImp::OnLButtonUp(CGXGridCore* pGrid, CPoint& point, UINT& flags)
{
	GX_UNUSED_ALWAYS(flags);

	if (pGrid->m_bTracking)
	{
		// User has changed row height
		CSize size = point-pGrid->m_rectBoundTrack.TopLeft();
		if (pGrid->m_nTrackingMode == GX_TRACKHEIGHT)
			size.cx = 0;
		else
			size.cy = 0;

		pGrid->OnEndTracking(pGrid->m_nRowTracking, pGrid->m_nColTracking, pGrid->m_nTrackingMode, size);

		// Release Mouse Capture
		if (pGrid->m_pGridWnd->GetCapture() == pGrid->m_pGridWnd)
			ReleaseCapture();
		return TRUE;
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
// Tracking column or row heights

BOOL CGXGridUserResizeCellsImp::OnStartTracking(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, int nTrackingMode)
{
	// check for headers in case of hidden columns
	if (nTrackingMode == GX_TRACKWIDTH
		&& !(nCol > pGrid->GetHeaderCols() || !pGrid->GetParam()->IsEnableTrackColWidth(GX_TRACK_NOTHEADER))
		|| nTrackingMode == GX_TRACKHEIGHT
		&& !(nRow > pGrid->GetHeaderRows() || !pGrid->GetParam()->IsEnableTrackRowHeight(GX_TRACK_NOTHEADER)))
		return FALSE;

	// trigger events and check return values
	if (nTrackingMode == GX_TRACKWIDTH
		&& !(pGrid->OnTrackColWidth(nCol) && pGrid->OnTrackColWidthStart(nCol))
		|| nTrackingMode == GX_TRACKHEIGHT
		&& !(pGrid->OnTrackRowHeight(nRow) && pGrid->OnTrackRowHeightStart(nRow)))
		return FALSE;

	// action is allowed

	if (nTrackingMode == GX_TRACKWIDTH)
		nRow = 0;

	if (nTrackingMode == GX_TRACKHEIGHT)
		nCol = 0;

	CRect rectGrid = pGrid->GetGridRect();
	CRect rectHit = pGrid->CalcRectFromRowCol(nRow, nCol);
	pGrid->m_nTrackingMode = nTrackingMode;
	pGrid->m_nRowTracking = nRow;
	pGrid->m_nColTracking = nCol;

	if (pGrid->GetParam()->GetNewGridLineMode())
		rectHit -= CPoint(1,1);;

	CClientDC dc(pGrid->m_pGridWnd);
	pGrid->OnGridPrepareDC(&dc);

	// Tracking line and bounding rectangle
	if (pGrid->m_nTrackingMode == GX_TRACKWIDTH)
	{
		// cache the options
		pGrid->m_bTrackSmoothCells = pGrid->GetParam()->IsEnableTrackColWidth(GX_TRACK_SMOOTHCELLS);
		pGrid->m_bTrackSmoothHeader = pGrid->GetParam()->IsEnableTrackColWidth(GX_TRACK_SMOOTHHEADER)
			&& !pGrid->m_bTrackSmoothCells;
		pGrid->m_bTrackMarkLine = pGrid->GetParam()->m_pProperties->GetDisplayVertLines()
			&& !pGrid->GetParam()->IsEnableTrackColWidth(GX_TRACK_NOMARKTRACKLINE)
			&& !pGrid->m_bTrackSmoothCells;
		pGrid->m_bTrackInvertLine = !pGrid->GetParam()->IsEnableTrackColWidth(GX_TRACK_NOINVERTLINE)
			&& !pGrid->m_bTrackSmoothCells;
		pGrid->m_bTrackPressedHeader = !pGrid->GetParam()->IsEnableTrackColWidth(GX_TRACK_NOPRESSEDHEADER)
			&& !pGrid->m_bTrackSmoothCells;
		pGrid->m_sizeTrackStart = CSize(pGrid->GetColWidth(nCol), 0);

		// Check if column has an individual width
		long n;
		pGrid->m_sizeTrackStart.cy = pGrid->m_pParam->m_ColWidths.Lookup(nCol, n);

		// determine the bounding rectangle
		int yMax;
		ROWCOL nRowClient = pGrid->CalcClientRowFromPt(rectGrid.BottomRight(), &yMax);
		if (nRowClient != GX_INVALID)
			yMax += pGrid->GetRowHeight(pGrid->m_nRowTracking = pGrid->GetRow(nRowClient));
		else
			yMax += pGrid->GetRowHeight(pGrid->m_nRowTracking = pGrid->GetRowCount());
		if (pGrid->m_bTrackSmoothHeader)
			rectGrid.top += pGrid->CalcSumOfRowHeights(0, pGrid->GetHeaderRows());
		pGrid->m_rectBoundTrack.SetRect(rectHit.left, rectGrid.top, rectHit.left+1, yMax);
		pGrid->m_rectTrack.SetRect(rectHit.right, rectGrid.top, rectHit.right+1, yMax);
	}
	else if (pGrid->m_nTrackingMode == GX_TRACKHEIGHT)
	{
		// cache the options
		pGrid->m_bTrackSmoothCells = pGrid->GetParam()->IsEnableTrackRowHeight(GX_TRACK_SMOOTHCELLS);
		pGrid->m_bTrackSmoothHeader = pGrid->GetParam()->IsEnableTrackRowHeight(GX_TRACK_SMOOTHHEADER)
			&& !pGrid->m_bTrackSmoothCells;
		pGrid->m_bTrackMarkLine = pGrid->GetParam()->m_pProperties->GetDisplayHorzLines()
			&& !pGrid->GetParam()->IsEnableTrackRowHeight(GX_TRACK_NOMARKTRACKLINE)
			&& !pGrid->m_bTrackSmoothCells;
		pGrid->m_bTrackInvertLine = !pGrid->GetParam()->IsEnableTrackRowHeight(GX_TRACK_NOINVERTLINE)
			&& !pGrid->m_bTrackSmoothCells;
		pGrid->m_bTrackPressedHeader = !pGrid->GetParam()->IsEnableTrackRowHeight(GX_TRACK_NOPRESSEDHEADER)
			&& !pGrid->m_bTrackSmoothCells;
		pGrid->m_sizeTrackStart = CSize(0, pGrid->GetRowHeight(nRow));

		// Check if row has an individual height
		long n;
		pGrid->m_sizeTrackStart.cx = pGrid->m_pParam->m_RowHeights.Lookup(nRow, n);

		// determine the bounding rectangle
		int xMax;
		ROWCOL nColClient = pGrid->CalcClientColFromPt(rectGrid.BottomRight(), &xMax);
		if (nColClient != GX_INVALID)
			xMax += pGrid->GetColWidth(pGrid->GetCol(nColClient));
		else
			xMax += pGrid->GetColWidth(pGrid->GetColCount());
		if (pGrid->m_bTrackSmoothHeader)
			rectGrid.left += pGrid->CalcSumOfColWidths(0, pGrid->GetHeaderRows());
		pGrid->m_rectBoundTrack.SetRect(rectGrid.left, rectHit.top, xMax, rectHit.top+1);
		pGrid->m_rectTrack.SetRect(rectGrid.left, rectHit.bottom, xMax, rectHit.bottom+1);
	}

	// Mark the line of the column which will be sized
	if (pGrid->m_bTrackMarkLine)
	{
		if (pGrid->m_pWndDrag == NULL)
		{
			pGrid->m_pWndDrag = new CGXDragLineWnd(pGrid);
			pGrid->m_pWndDrag->CreateWnd(pGrid->m_pGridWnd);
			pGrid->m_pWndDrag->m_flags = GX_TRACKLINE;
			pGrid->m_pWndDrag->m_nClientRC = 0;
			pGrid->m_pWndDrag->m_type = (pGrid->m_nTrackingMode == GX_TRACKHEIGHT) ? GX_HORZLINE : GX_VERTLINE;
		}

		pGrid->m_pWndDrag->MoveWindow(pGrid->m_rectTrack.left, pGrid->m_rectTrack.top,
			pGrid->m_rectTrack.Width(), pGrid->m_rectTrack.Height());
		pGrid->m_pWndDrag->ShowWindow(SW_SHOW);
		pGrid->m_pWndDrag->UpdateWindow();
	}

	pGrid->m_bTrackMove = FALSE;   // will be set TRUE when user moved the line

	pGrid->m_rectInvertBar = pGrid->m_rectTrack;

	pGrid->m_bTracking = TRUE;

	if (pGrid->m_bTrackPressedHeader)
		pGrid->InvalidateRect(rectHit); // Redraw button (in pressed state)

	return TRUE;
}

// Tracking

void CGXGridUserResizeCellsImp::OnMoveTracking(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, int nTrackingMode, CSize &size)
{
	GX_UNUSED_ALWAYS(nTrackingMode);

	pGrid->m_bTrackMove = TRUE;    // indicates that user has moved the cursor

	BOOL b = FALSE;

	// trigger events
	if (pGrid->m_nTrackingMode == GX_TRACKWIDTH)
	{
		if (size.cx < pGrid->GetParam()->m_nMinTrackColWidth)
			size.cx = pGrid->GetParam()->m_nMinTrackColWidth;
		b = pGrid->OnTrackColWidthMove(nCol, size.cx);
	}
	else if (pGrid->m_nTrackingMode == GX_TRACKHEIGHT)
	{
		if (size.cy < pGrid->GetParam()->m_nMinTrackRowHeight)
			size.cy = pGrid->GetParam()->m_nMinTrackRowHeight;
		b = pGrid->OnTrackRowHeightMove(nRow, size.cy);
	}

	if (!b)
		return;

	BOOL bLock = pGrid->m_bLockedScrollbars;
	pGrid->m_bLockedScrollbars = TRUE;

	CRect    rectGrid = pGrid->GetGridRect();
	CPoint   point = pGrid->m_rectBoundTrack.TopLeft()+size;

	CClientDC dc(pGrid->m_pGridWnd);
	pGrid->OnGridPrepareDC(&dc);

	ASSERT(pGrid->GetCapture() == pGrid->m_pGridWnd);

	CRect oldRect = pGrid->m_rectInvertBar;

	if (pGrid->m_nTrackingMode & GX_TRACKWIDTH)
	{
		// Compute new grid line
		pGrid->m_rectInvertBar.SetRect(
			min(rectGrid.right-7, max(pGrid->m_rectBoundTrack.left, point.x)),
			pGrid->m_rectTrack.top,
			min(rectGrid.right-6, max(point.x+1, pGrid->m_rectBoundTrack.right)),
			pGrid->m_rectTrack.bottom);

		if (oldRect != pGrid->m_rectInvertBar)
		{
			// Undo previous inversion
			if (pGrid->m_bTrackInvertLine && oldRect != pGrid->m_rectTrack)
				dc.InvertRect(oldRect);

			// Show new size
			if (pGrid->m_bTrackInvertLine && pGrid->m_rectInvertBar != pGrid->m_rectTrack)
				dc.InvertRect(pGrid->m_rectInvertBar);

			// Draw the cells (using MemDC)
			if (pGrid->m_bTrackSmoothCells)
			{
				int dx = pGrid->m_rectInvertBar.left - pGrid->m_rectTrack.left;

				// Store drawing technique and column width
				GXDrawingTechnique dt = pGrid->SetDrawingTechnique(gxDrawUsingMemDC);
				int nNewWidth = (dx + pGrid->m_sizeTrackStart.cx);
				int nOldWidth = pGrid->GetColWidth(nCol);
				pGrid->StoreColWidth(nCol, nNewWidth);

				// Draw the cells
				pGrid->UpdateChangedColWidths(nCol, nCol, &nOldWidth, GX_UPDATENOW, FALSE);

				// Reset drawing technique and column width
				pGrid->SetDrawingTechnique(dt);
			}
			// Draw the headers (using MemDC)
			else if (pGrid->m_bTrackSmoothHeader)
			{
				int dx = pGrid->m_rectInvertBar.left - pGrid->m_rectTrack.left;

				// Store drawing technique and column width
				GXDrawingTechnique dt = pGrid->SetDrawingTechnique(gxDrawUsingMemDC);
				int nNewWidth = (dx + pGrid->GetColWidth(nCol));
				long nWidth = 0;
				pGrid->m_pParam->m_ColWidths.Lookup(nCol, nWidth);
				int nOldWidth = pGrid->GetColWidth(nCol);
				pGrid->StoreColWidth(nCol, nNewWidth);

				// Draw the headers
				CRect rect = pGrid->CalcRectFromRowColExEx(0, nCol, pGrid->GetHeaderRows(), nCol);
				rect.right = rectGrid.right;
				pGrid->InvalidateRect(rect);
				pGrid->UpdateWindow();

				// Reset drawing technique and column width
				pGrid->StoreColWidth(nCol, nOldWidth);
				if (!nWidth)
					pGrid->m_pParam->m_ColWidths.RemoveKey(nCol);
				pGrid->SetDrawingTechnique(dt);
			}
		}
	}
	else if (pGrid->m_nTrackingMode & GX_TRACKHEIGHT)
	{
		// Compute new grid line
		pGrid->m_rectInvertBar.SetRect(
			pGrid->m_rectTrack.left,
			min(rectGrid.bottom-7, max(point.y, pGrid->m_rectBoundTrack.top)),
			pGrid->m_rectTrack.right,
			min(rectGrid.bottom-6, max(point.y+1, pGrid->m_rectBoundTrack.bottom)));

		if (oldRect != pGrid->m_rectInvertBar)
		{
			// Undo previous inversion
			if (pGrid->m_bTrackInvertLine && oldRect != pGrid->m_rectTrack)
				dc.InvertRect(oldRect);

			// Show new size
			if (pGrid->m_bTrackInvertLine && pGrid->m_rectInvertBar != pGrid->m_rectTrack)
				dc.InvertRect(pGrid->m_rectInvertBar);

			// Draw the cells (using MemDC)
			if (pGrid->m_bTrackSmoothCells)
			{
				int dy = pGrid->m_rectInvertBar.top - pGrid->m_rectTrack.top;

				// Store drawing technique and column width
				GXDrawingTechnique dt = pGrid->SetDrawingTechnique(gxDrawUsingMemDC);
				int nNewHeight = (dy + pGrid->m_sizeTrackStart.cy);
				int nOldHeight = pGrid->GetRowHeight(nRow);
				pGrid->StoreRowHeight(nRow, nNewHeight);

				// Draw the cells
				pGrid->UpdateChangedRowHeights(nRow, nRow, &nOldHeight, GX_UPDATENOW, FALSE);

				// Reset drawing technique and row height
				pGrid->SetDrawingTechnique(dt);
			}
			// Draw the headers (using MemDC)
			else if (pGrid->m_bTrackSmoothHeader)
			{
				int dy = pGrid->m_rectInvertBar.top - pGrid->m_rectTrack.top;

				// Store drawing technique and column width
				GXDrawingTechnique dt = pGrid->SetDrawingTechnique(gxDrawUsingMemDC);
				int nNewHeight = (dy + pGrid->m_sizeTrackStart.cy);
				pGrid->StoreRowHeight(nRow, nNewHeight);

				// Draw the headers
				CRect rect = pGrid->CalcRectFromRowColExEx(nRow, 0, nRow, pGrid->GetHeaderCols());
				rect.bottom = rectGrid.bottom;
				pGrid->InvalidateRect(rect);
				pGrid->UpdateWindow();

				// Reset drawing technique and row height
				pGrid->SetDrawingTechnique(dt);
			}
		}
	}

	pGrid->m_bLockedScrollbars = bLock;
}

void CGXGridUserResizeCellsImp::OnCancelTracking(CGXGridCore* pGrid)
{
	pGrid->m_bTracking = FALSE;

	// Redraw the marked grid line
	if (pGrid->m_pWndDrag)
	{
		pGrid->m_pWndDrag->DestroyWindow();
		delete pGrid->m_pWndDrag;
		pGrid->m_pWndDrag = NULL;
	}

	CClientDC dc(pGrid->m_pGridWnd);
	pGrid->OnGridPrepareDC(&dc);

	CRect rectGrid = pGrid->GetGridRect();
	ROWCOL nRow = pGrid->m_nRowTracking;
	ROWCOL nCol = pGrid->m_nColTracking;

	// Undo previous inversion
	if (pGrid->m_bTrackInvertLine && pGrid->m_rectInvertBar != pGrid->m_rectTrack)
		dc.InvertRect(pGrid->m_rectInvertBar);

	if (pGrid->m_nTrackingMode == GX_TRACKWIDTH)
	{
		// Redraw the Cells
		if (pGrid->m_bTrackSmoothCells)
		{
			int nOldWidth = pGrid->GetColWidth(pGrid->m_nColTracking);
			pGrid->StoreColWidth(pGrid->m_nColTracking, pGrid->m_sizeTrackStart.cx);
			if (pGrid->m_sizeTrackStart.cy == 0)
				pGrid->m_pParam->m_ColWidths.RemoveKey(pGrid->m_nColTracking);

			// Draw the cells
			pGrid->UpdateChangedColWidths(pGrid->m_nColTracking, pGrid->m_nColTracking, &nOldWidth, GX_UPDATENOW, FALSE);
		}
		// Redraw the headers
		else if (pGrid->m_bTrackSmoothHeader)
		{
			pGrid->StoreColWidth(nCol, pGrid->m_sizeTrackStart.cx);
			if (pGrid->m_sizeTrackStart.cy == 0)
				pGrid->m_pParam->m_ColWidths.RemoveKey(nCol);

			CRect rect = pGrid->CalcRectFromRowColExEx(0, nCol, pGrid->GetHeaderRows(), nCol);
			rect.right = rectGrid.right;
			pGrid->InvalidateRect(rect);
		}
		else if (pGrid->m_bTrackPressedHeader)
			pGrid->RedrawRowCol(0, pGrid->m_nColTracking);
	}
	else if (pGrid->m_nTrackingMode == GX_TRACKHEIGHT)
	{
		// Redraw the Cells
		if (pGrid->m_bTrackSmoothCells)
		{
			int nOldHeight = pGrid->GetRowHeight(pGrid->m_nRowTracking);
			pGrid->StoreRowHeight(pGrid->m_nRowTracking, pGrid->m_sizeTrackStart.cy);
			if (pGrid->m_sizeTrackStart.cx == 0)
				pGrid->m_pParam->m_RowHeights.RemoveKey(pGrid->m_nRowTracking);

			// Draw the headers
			pGrid->UpdateChangedRowHeights(pGrid->m_nRowTracking, pGrid->m_nRowTracking, &nOldHeight, GX_UPDATENOW, FALSE);
		}
		// Redraw the headers
		else if (pGrid->m_bTrackSmoothHeader)
		{
			pGrid->StoreRowHeight(nRow, pGrid->m_sizeTrackStart.cy);
			if (pGrid->m_sizeTrackStart.cx == 0)
				pGrid->m_pParam->m_RowHeights.RemoveKey(nRow);

			CRect rect = pGrid->CalcRectFromRowColExEx(nRow, 0, nRow, pGrid->GetHeaderCols());
			rect.bottom = rectGrid.bottom;
			pGrid->InvalidateRect(rect);
		}
		else if (pGrid->m_bTrackPressedHeader)
			pGrid->RedrawRowCol(pGrid->m_nRowTracking, 0);
	}
}

void CGXGridUserResizeCellsImp::OnEndTracking(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, int /*nTrackingMode*/, CSize& size)
{
	if (pGrid->m_bTrackMove)
		pGrid->OnMoveTracking(pGrid->m_nRowTracking, pGrid->m_nColTracking, pGrid->m_nTrackingMode, size);

	// Redraw the marked grid line
	if (pGrid->m_pWndDrag)
	{
		pGrid->m_pWndDrag->DestroyWindow();
		delete pGrid->m_pWndDrag;
		pGrid->m_pWndDrag = NULL;
	}

	if (pGrid->m_nTrackingMode == GX_TRACKWIDTH)
	{
		if (size.cx < pGrid->GetParam()->m_nMinTrackColWidth)
			size.cx = pGrid->GetParam()->m_nMinTrackColWidth;
	}
	else if (pGrid->m_nTrackingMode == GX_TRACKHEIGHT)
	{
		if (size.cy < pGrid->GetParam()->m_nMinTrackRowHeight)
			size.cy = pGrid->GetParam()->m_nMinTrackRowHeight;
	}

	CPoint   point = pGrid->m_rectBoundTrack.TopLeft()+size;
	CRect rectGrid = pGrid->GetGridRect();

	CClientDC dc(pGrid->m_pGridWnd);
	pGrid->OnGridPrepareDC(&dc);

	CGXRangeList* pSelList = pGrid->GetParam()->GetRangeList();

	pGrid->m_bTracking = FALSE;

	// Undo previous inversion
	if (pGrid->m_bTrackInvertLine && pGrid->m_rectInvertBar != pGrid->m_rectTrack)
		dc.InvertRect(pGrid->m_rectInvertBar);

	GXDrawingTechnique dt = gxDrawUsingMemDC;
	if (pGrid->m_bTrackSmoothCells)
		dt = pGrid->SetDrawingTechnique(gxDrawUsingMemDC);

	UINT nUpdateFlags = GX_UPDATENOW;

	if (pGrid->m_nTrackingMode & GX_TRACKWIDTH)
	{
		// Redraw the cells
		if (pGrid->m_bTrackSmoothCells)
		{
			pGrid->StoreColWidth(nCol, pGrid->m_sizeTrackStart.cx);
			if (pGrid->m_sizeTrackStart.cy == 0)
				pGrid->m_pParam->m_ColWidths.RemoveKey(nCol);

			nUpdateFlags = GX_INVALIDATE;
		}
		// Redraw the headers
		else if (pGrid->m_bTrackSmoothHeader)
		{
			pGrid->StoreColWidth(nCol, pGrid->m_sizeTrackStart.cx);
			if (pGrid->m_sizeTrackStart.cy == 0)
				pGrid->m_pParam->m_ColWidths.RemoveKey(nCol);

			CRect rect = pGrid->CalcRectFromRowColExEx(0, nCol, pGrid->GetHeaderRows(), nCol);
			rect.right = rectGrid.right;
			pGrid->InvalidateRect(rect);
		}

		// Change the column witdths and update the display
		int dx = pGrid->m_rectInvertBar.left - pGrid->m_rectTrack.left;

		// trigger event and check return value
		if (pGrid->OnTrackColWidthEnd(nCol, size.cx)
			&& abs(dx) > 1 && pGrid->m_bTrackMove)
		{

			ASSERT(dx + pGrid->GetColWidth(pGrid->m_nColTracking) >= 0);
			int newWidth = (dx + pGrid->GetColWidth(pGrid->m_nColTracking));

			if (pGrid->m_nColTracking > 0 && pGrid->m_pParam->IsEnableTrackColWidth(GX_TRACK_ALL))
				pGrid->SetDefaultColWidth(newWidth, nUpdateFlags);

			else if (pGrid->m_nColTracking > 0 && pSelList->IsCellInList(0, pGrid->m_nColTracking))
			{
				if (pSelList->GetHead()->IsTable())
				{
					pGrid->SetDefaultColWidth(newWidth, nUpdateFlags);
				}
				else
				{
					CRowColArray  awLeftCol, awRightCol;

					pSelList->GetColArray(pGrid->GetColCount(), awLeftCol, awRightCol, TRUE);

					CGXWaitCursor theWait;

					for (int colindex = awLeftCol.GetSize()-1; colindex >= 0; colindex--)
					{
						if (newWidth > 0)
							pGrid->SetColWidth(max(awLeftCol[colindex], 1), awRightCol[colindex], newWidth, NULL, nUpdateFlags);
						else
							pGrid->HideCols(max(awLeftCol[colindex], 1), awRightCol[colindex], TRUE, NULL, nUpdateFlags);
					}
				}
			}
			else
			{
				if (newWidth > 0)
					pGrid->SetColWidth(pGrid->m_nColTracking, pGrid->m_nColTracking, newWidth, NULL, nUpdateFlags);
				else
					pGrid->HideCols(pGrid->m_nColTracking, pGrid->m_nColTracking, TRUE, NULL, nUpdateFlags);
			}
		}
		else
		{
			pGrid->InvalidateRect(pGrid->CalcRectFromRowCol(0, pGrid->m_nColTracking));
			int nOldWidth = pGrid->GetColWidth(pGrid->m_nColTracking);
			pGrid->UpdateChangedColWidths(pGrid->m_nColTracking, pGrid->m_nColTracking, &nOldWidth, nUpdateFlags, FALSE);
		}

		pGrid->UpdateWindow();

		pGrid->ScrollCellInView(pGrid->GetTopRow(), pGrid->m_nColTracking);

		pGrid->UpdateScrollbars(FALSE);
	}
	else
	{
		// Redraw the cells
		if (pGrid->m_bTrackSmoothCells)
		{
			pGrid->StoreRowHeight(nRow, pGrid->m_sizeTrackStart.cy);
			if (pGrid->m_sizeTrackStart.cx == 0)
				pGrid->m_pParam->m_RowHeights.RemoveKey(nRow);
			nUpdateFlags = GX_INVALIDATE;
		}
		// Redraw the headers
		else if (pGrid->m_bTrackSmoothHeader)
		{
			pGrid->StoreRowHeight(nRow, pGrid->m_sizeTrackStart.cy);
			if (pGrid->m_sizeTrackStart.cx == 0)
				pGrid->m_pParam->m_RowHeights.RemoveKey(nRow);

			CRect rect = pGrid->CalcRectFromRowColExEx(nRow, 0, nRow, pGrid->GetHeaderCols());
			rect.bottom = rectGrid.bottom;
			pGrid->InvalidateRect(rect);
		}

		// Change the row heights and update the display
		int dy = pGrid->m_rectInvertBar.top - pGrid->m_rectTrack.top;

		// trigger event and check return value
		if (pGrid->OnTrackRowHeightEnd(nRow, size.cy)
			&& abs(dy) > 1 && pGrid->m_bTrackMove)
		{
			ASSERT(dy + pGrid->GetRowHeight(pGrid->m_nRowTracking) >= 0);
			int newHeight = (dy + pGrid->GetRowHeight(pGrid->m_nRowTracking));

			if (pGrid->m_nRowTracking > 0 && pGrid->m_pParam->IsEnableTrackRowHeight(GX_TRACK_ALL))
				pGrid->SetDefaultRowHeight(newHeight, nUpdateFlags);

			else if (pGrid->m_nRowTracking > 0 && pSelList->IsCellInList(pGrid->m_nRowTracking, 0))
			{
				if (pSelList->GetHead()->IsTable())
				{
					pGrid->SetDefaultRowHeight(newHeight, nUpdateFlags);
				}
				else
				{
					CRowColArray  awTopRow, awBottomRow;

					pSelList->GetRowArray(pGrid->GetRowCount(), awTopRow, awBottomRow, TRUE);

					CGXWaitCursor theWait;

					for (int rowindex = awTopRow.GetSize()-1; rowindex >= 0; rowindex--)
					{
						if (newHeight > 0)
							pGrid->SetRowHeight(max(awTopRow[rowindex], 1), awBottomRow[rowindex], newHeight, NULL, nUpdateFlags);
						else
							pGrid->HideRows(max(awTopRow[rowindex], 1), awBottomRow[rowindex], TRUE, NULL, nUpdateFlags);
					}
				}
			}
			else
			{
				if (newHeight > 0)
					pGrid->SetRowHeight(pGrid->m_nRowTracking, pGrid->m_nRowTracking, newHeight, NULL, nUpdateFlags);
				else
					pGrid->HideRows(pGrid->m_nRowTracking, pGrid->m_nRowTracking, TRUE, NULL, nUpdateFlags);
			}
		}
		else
		{
			// InvalidateRect(CalcRectFromRowCol(pGrid->m_nRowTracking, 0));
			int nOldHeight = pGrid->GetRowHeight(pGrid->m_nRowTracking);
			pGrid->UpdateChangedRowHeights(pGrid->m_nRowTracking, pGrid->m_nRowTracking, &nOldHeight, nUpdateFlags, FALSE);
		}

		pGrid->UpdateWindow();

		pGrid->ScrollCellInView(pGrid->m_nRowTracking, pGrid->GetLeftCol());

		pGrid->UpdateScrollbars(FALSE);
	}

	if (pGrid->m_bTrackSmoothCells)
		pGrid->SetDrawingTechnique(dt);

	ROWCOL nEditRow, nEditCol;
	if (pGrid->m_pGridWnd->GetFocus() == pGrid->m_pGridWnd && pGrid->GetCurrentCell(nEditRow, nEditCol))
	{
		CGXControl* pControl = pGrid->GetControl(nEditRow, nEditCol);
		if (pControl->IsActive())
			pControl->Refresh();
	}

	pGrid->UpdateScrollbars();
}

BOOL CGXGridUserResizeCellsImp::OnLButtonDblClk(CGXGridCore* pGrid, CPoint& point, UINT& flags, int& ht, ROWCOL& nRow, ROWCOL& nCol)
{
	GX_UNUSED_ALWAYS(flags);
	GX_UNUSED_ALWAYS(point);

	if (ht == GX_VERTLINE && pGrid->m_pParam->IsEnableTrackColWidth())
	{
		CGXRangeList* pSelList = pGrid->GetParam()->GetRangeList();

		// Can column be tracked ?

		if (pGrid->OnTrackColWidth(nCol))
		{
			const int nDefault = -1;

			if (nCol > 0 && pGrid->m_pParam->IsEnableTrackColWidth(GX_TRACK_ALL))
				;

			else if (pGrid->IsColHidden(nCol)
				|| nCol < pGrid->GetColCount() && pGrid->IsColHidden(nCol+1))
			{
				ROWCOL nFromCol = nCol;
				ROWCOL nToCol = nCol+1;
				ROWCOL nCount = pGrid->GetColCount();
				while (nFromCol > 0 && pGrid->IsColHidden(nFromCol-1) && pGrid->OnTrackColWidth(nFromCol-1))
					nFromCol--;
				while (nToCol <= nCount && pGrid->IsColHidden(nToCol) && pGrid->OnTrackColWidth(nToCol))
					nToCol++;

				if (nToCol > nFromCol)
					pGrid->HideCols(nFromCol, nToCol-1, FALSE);
			}

			else if (nCol > 0 && pSelList->IsCellInList(0, nCol))
			{
				if (!pSelList->GetHead()->IsTable())
				{
					CRowColArray  awLeftCol, awRightCol;

					pSelList->GetColArray(pGrid->GetColCount(), awLeftCol, awRightCol, TRUE);

					CGXWaitCursor theWait;

					for (int colindex = awLeftCol.GetSize()-1; colindex >= 0; colindex--)
					{
						if (pGrid->IsColHidden(max(awLeftCol[colindex], 1)))
							pGrid->HideCols(max(awLeftCol[colindex], 1), awRightCol[colindex], FALSE);
						else
							pGrid->SetColWidth(max(awLeftCol[colindex], 1), awRightCol[colindex], nDefault);
					}
				}
			}
			else
				pGrid->SetColWidth(pGrid->m_nColTracking, pGrid->m_nColTracking, nDefault);

			return TRUE;
		}
	}

	// check, if user clicked on a horizontal line to resize or restore the row
	// ... (behaviour can be changed with GetParam()->EnableTrackRowHeight(...)
	// and GetParam()->SetColHeaderLines(...)

	else if (ht == GX_HORZLINE && pGrid->m_pParam->IsEnableTrackRowHeight())
	{
		CGXRangeList* pSelList = pGrid->GetParam()->GetRangeList();

		// Can row be tracked ?

		if (pGrid->OnTrackRowHeight(nRow))
		{
			const int nDefault = -1;

			if (nRow > 0 && pGrid->m_pParam->IsEnableTrackRowHeight(GX_TRACK_ALL))
				;

			else if (pGrid->IsRowHidden(nRow)
				|| nRow < pGrid->GetRowCount() && pGrid->IsRowHidden(nRow+1) && pGrid->OnTrackRowHeight(nRow+1))
			{
				ROWCOL nFromRow = nRow;
				ROWCOL nToRow = nRow+1;
				ROWCOL nCount = pGrid->GetRowCount();
				while (nFromRow > 0 && pGrid->IsRowHidden(nFromRow-1) && pGrid->OnTrackRowHeight(nFromRow-1))
					nFromRow--;
				while (nToRow <= nCount && pGrid->IsRowHidden(nToRow) && pGrid->OnTrackRowHeight(nToRow))
					nToRow++;

				if (nToRow > nFromRow)
					pGrid->HideRows(nFromRow, nToRow-1, FALSE);
			}

			else if (nRow > 0 && pSelList->IsCellInList(nRow, 0))
			{
				if (!pSelList->GetHead()->IsTable())
				{
					CRowColArray  awTopRow, awBottomRow;

					pSelList->GetRowArray(pGrid->GetRowCount(), awTopRow, awBottomRow, TRUE);

					CGXWaitCursor theWait;

					for (int rowindex = awTopRow.GetSize()-1; rowindex >= 0; rowindex--)
					{
						if (pGrid->IsRowHidden(max(awTopRow[rowindex], 1)))
							pGrid->HideRows(max(awTopRow[rowindex], 1), awBottomRow[rowindex], FALSE);
						else
							pGrid->SetRowHeight(max(awTopRow[rowindex], 1), awBottomRow[rowindex], nDefault);
					}
				}
			}
			else
				pGrid->SetRowHeight(pGrid->m_nRowTracking, pGrid->m_nRowTracking, nDefault);

			return TRUE;
		}
	}

	return FALSE;
}
