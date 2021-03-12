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

// gxcorsiz.cpp : row heights and column widths (CGXGridCore)
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

#ifndef _GXSTYLES_H_
#include "gxstyles.h"
#endif

#ifndef _GXDLL_H_
#include "gxdll.h"
#endif

#ifndef _GXPRIV_H_
#include "gxpriv.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCORSIZ")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CGXGridCore

// this file contains following methods from CGXGridCore:

// - Cell width and height
// - Hidden rows/cols
// - Zoom
// - Font Metrics
// - Cursor
// - Tracking

/////////////////////////////////////////////////////////////////////////////
// Cell width and height

BOOL CGXGridCore::SetRowHeight(ROWCOL nFromRow, ROWCOL nToRow, int nHeight, int* anHeightArray, UINT flags, GXCmdType ctCmd)
{
	if (nToRow == 0)
		nToRow = nFromRow;

	ASSERT(nToRow >= nFromRow);

#ifdef _DEBUG
	ASSERT(anHeightArray == NULL || nHeight == 0);
	// ASSERTION-> Ambigous command call ->END

	if (anHeightArray)
		ASSERT(AfxIsValidAddress(anHeightArray, (UINT) (sizeof(int)*(nToRow-nFromRow+1))));
	// ASSERTION-> anWidthArray is not valid, perhaps too small? ->END
#endif

	CGXLongOperation theOp;

	int* nOldHeights = new int[nToRow-nFromRow+1];
	GetArrayWithRowHeights(nFromRow, nToRow, nOldHeights);
	BOOL b = FALSE;
	for (ROWCOL nRow = nFromRow; nRow <= nToRow; nRow++)
	{
		if (anHeightArray)
			b |= StoreRowHeight(nRow, anHeightArray[nRow-nFromRow]);
		else
			b |= StoreRowHeight(nRow, nHeight);

		theOp.DoMessages();
	}

	if (b)
	{
		DelayFloatCells(CGXRange().SetRows(nFromRow, nToRow));

		UpdateChangedRowHeights(nFromRow, nToRow, nOldHeights, flags, TRUE);

		if (m_pUndoImp && ctCmd != gxRollback && m_pParam->m_bUndoEnabled)
		{
			AddCommand(m_pUndoImp->CreateSetRowHeightCmd(nFromRow, nToRow, nOldHeights), ctCmd);

			if (ctCmd == gxRedo || ctCmd == gxUndo)
				OnCellUndone(nFromRow, GetLeftCol());
		}
		else
			delete nOldHeights;

		return TRUE;
	}

	// clean up undo info
	delete nOldHeights;

	return FALSE;
}

BOOL CGXGridCore::SetColWidth(ROWCOL nFromCol, ROWCOL nToCol, int nWidth, int* anWidthArray, UINT flags, GXCmdType ctCmd)
{
	if (nToCol == 0)
		nToCol = nFromCol;

	ASSERT(nToCol >= nFromCol);

#ifdef _DEBUG
	ASSERT(anWidthArray == NULL || nWidth == 0);
	// ASSERTION-> Ambigous command call ->END

	if (anWidthArray)
		ASSERT(AfxIsValidAddress(anWidthArray, (UINT) (sizeof(int)*(nToCol-nFromCol+1))));
	// ASSERTION-> anWidthArray is not valid, perhaps too small? ->END
#endif

	CGXLongOperation theOp;

	int* nOldWidths = new int[nToCol-nFromCol+1];
	GetArrayWithColWidths(nFromCol, nToCol, nOldWidths);
	BOOL b = FALSE;
	for (ROWCOL nCol = nFromCol; nCol <= nToCol; nCol++)
	{
		if (anWidthArray)
			b |= StoreColWidth(nCol, anWidthArray[nCol-nFromCol]);
		else
			b |= StoreColWidth(nCol, nWidth);

		theOp.DoMessages();
	}

	if (b)
	{
		DelayFloatCells(CGXRange().SetCols(nFromCol, nToCol+1));

		UpdateChangedColWidths(nFromCol, nToCol, nOldWidths, flags, TRUE);

		if (m_pUndoImp && ctCmd != gxRollback && m_pParam->m_bUndoEnabled)
		{
			AddCommand(m_pUndoImp->CreateSetColWidthCmd(nFromCol, nToCol, nOldWidths), ctCmd);
			if (ctCmd == gxRedo || ctCmd == gxUndo)
				OnCellUndone(GetTopRow(), nFromCol);
		}
		else
			delete nOldWidths;
		return TRUE;
	}

	// clean up undo info
	delete nOldWidths;

	return FALSE;
}

BOOL CGXGridCore::SetDefaultRowHeight(int nHeight, UINT flags /*= GX_UPDATENOW*/, GXCmdType ctCmd /*=gxDo*/)
{
	int nOldHeight = GetDefaultRowHeight();

	if (StoreDefaultRowHeight(nHeight))
	{
		DelayMergeCells(CGXRange().SetTable());
		DelayFloatCells(CGXRange().SetTable());

		UpdateChangedRowHeights(1, GetRowCount(), NULL, flags, TRUE);

		if (m_pUndoImp && ctCmd != gxRollback && m_pParam->m_bUndoEnabled)
			AddCommand(m_pUndoImp->CreateSetDefaultRowHeightCmd(nOldHeight), ctCmd);

		return TRUE;
	}
	return FALSE;
}

BOOL CGXGridCore::SetDefaultColWidth(int nWidth, UINT flags /*= GX_UPDATENOW*/, GXCmdType ctCmd /*=gxDo*/)
{
	int nOldWidth = GetDefaultColWidth();

	if (StoreDefaultColWidth(nWidth))
	{
		DelayMergeCells(CGXRange().SetTable());
		DelayFloatCells(CGXRange().SetTable());

		UpdateChangedColWidths(1, GetColCount(), NULL, flags, TRUE);

		if (m_pUndoImp && ctCmd != gxRollback && m_pParam->m_bUndoEnabled)
			AddCommand(m_pUndoImp->CreateSetDefaultColWidthCmd(nOldWidth), ctCmd);

		return TRUE;
	}
	return FALSE;
}

BOOL CGXGridCore::StoreDefaultRowHeight(int nHeight)
{
	if (nHeight <= 0)
		return FALSE;

	m_pParam->m_nDefaultHeight = Height_DPtoLP(nHeight);
	SetModifiedFlag();
	return TRUE;
}

BOOL CGXGridCore::StoreDefaultColWidth(int nWidth)
{
	if (nWidth <= 0)
		return FALSE;

	m_pParam->m_nDefaultWidth = Width_DPtoLP(nWidth);
	SetModifiedFlag();
	return TRUE;
}

int CGXGridCore::GetDefaultRowHeight()
{
	return Height_LPtoDP(m_pParam->m_nDefaultHeight);
}

int CGXGridCore::GetDefaultColWidth()
{
	return Width_LPtoDP(m_pParam->m_nDefaultWidth);
}

int CGXGridCore::GetRowHeight(ROWCOL nRow)
{
	if (IsRowHidden(nRow))
		return 0;

	long nHeight;
	if (m_pParam->m_RowHeights.Lookup(nRow, nHeight))
		nHeight = Height_LPtoDP(nHeight);
	else
		nHeight = GetDefaultRowHeight();

	// The following lines make sure that the row height
	// will not be larger than the height of the grid.
	// If this new OG 1.1 feature causes compatibilty problems
	// with your existing 1.0 application, you may turn
	// it off by setting
	//  m_bLimitRowHeight = FALSE;
	// in your grid initialization routine.
	if (m_bLimitRowHeight && m_pGridWnd->IsWindowVisible())
	{
		ROWCOL nfr = GetFrozenRows();
		if (nRow > nfr)
			nHeight = min(nHeight,
				GetGridRect().Height()-CalcSumOfRowHeights(0, nfr));
	}

	return (int) nHeight;
}


int CGXGridCore::GetColWidth(ROWCOL nCol)
{
	if (IsColHidden(nCol))
		return 0;

	long nWidth;
	if (m_pParam->m_ColWidths.Lookup(nCol, nWidth))
		nWidth = Width_LPtoDP(nWidth);
	else
		nWidth = GetDefaultColWidth();

	// The following lines make sure that the column width
	// will not be larger than the width of the grid.
	// If this new OG 1.1 feature causes compatibilty problems
	// with your existing 1.0 application, you may turn
	// it off by setting
	//  m_bLimitColWidth = FALSE;
	// in your grid initialization routine.
	if (m_bLimitColWidth && m_pGridWnd->IsWindowVisible())
	{
		ROWCOL nfc = GetFrozenCols();
		if (nCol > nfc)
			nWidth = min(nWidth,
				GetGridRect().Width()-CalcSumOfColWidths(0, nfc));
	}

	return (int) nWidth;
}

BOOL CGXGridCore::StoreRowHeight(ROWCOL nRow, int nHeight /* = 0 */)
{
	if (nHeight >= 0)
		m_pParam->m_RowHeights[nRow] = Height_DPtoLP(nHeight);
	else
		m_pParam->m_RowHeights.RemoveKey(nRow);
	SetModifiedFlag();
	return TRUE;
}

BOOL CGXGridCore::StoreColWidth(ROWCOL nCol, int nWidth /* = 0 */)
{
	if (nWidth >= 0)
		m_pParam->m_ColWidths[nCol] = Width_DPtoLP(nWidth);
	else
		m_pParam->m_ColWidths.RemoveKey(nCol);
	SetModifiedFlag();
	return TRUE;
}

void CGXGridCore::UpdateChangedRowHeights(ROWCOL nFromRow, ROWCOL nToRow, int* nOldHeights, UINT flags, BOOL bCreateHint)
{
	m_nLastVisibleRow = GX_INVALID;

	ROWCOL ncRow, ncCol;
	if (GetRowHeight(nFromRow) == 0 && GetCurrentCell(ncRow,  ncCol) && ncRow >= nFromRow && ncRow <= nToRow)
		ResetCurrentCell();

	// TRACE3("CGXGridCore: BEGIN UpdateChangedRowHeights(%u, %u, %05x)\n", nFromRow, nToRow, flags);
	if (!IsLockUpdate())
	{
		if (m_pOptimizedUpdateImp)
			m_pOptimizedUpdateImp->UpdateChangedRowHeights(this, nFromRow, nToRow, nOldHeights, flags);
		else
			Redraw(flags);
	}

	// Create Hint
	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintUpdateChangedRowHeights(this, nFromRow, nToRow, nOldHeights, flags);

	// TRACE3("CGXGridCore: END   UpdateChangedRowHeights(%u, %u, %05x)\n", nFromRow, nToRow, flags);
}

void CGXGridCore::UpdateChangedColWidths(ROWCOL nFromCol, ROWCOL nToCol, int* nOldWidths, UINT flags, BOOL bCreateHint)
{
	m_nLastVisibleCol = GX_INVALID;

	// TRACE3("CGXGridCore: BEGIN UpdateChangedColWidths(%u, %u, %05x)\n", nFromCol, nToCol, flags);

	ROWCOL ncRow, ncCol;
	if (GetColWidth(nFromCol) == 0 && GetCurrentCell(ncRow,  ncCol) && ncCol >= nFromCol && ncCol <= nToCol)
		ResetCurrentCell();

	if (!IsLockUpdate())
	{
		if (m_pOptimizedUpdateImp)
			m_pOptimizedUpdateImp->UpdateChangedColWidths(this, nFromCol, nToCol, nOldWidths, flags);
		else
			Redraw(flags);
	}

	// Create Hint
	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintUpdateChangedColWidths(this, nFromCol, nToCol, nOldWidths, flags);

	// TRACE3("CGXGridCore: END   UpdateChangedColWidths(%u, %u, %05x)\n", nFromCol, nToCol, flags);
}

/////////////////////////////////////////////////////////////////////////////
// Get array with row heights/column widths

void CGXGridCore::GetArrayWithRowHeights(ROWCOL nFromRow, ROWCOL nToRow, int* pArray)
{
	ASSERT(nToRow >= nFromRow);
	// ASSERTION-> nToRow < nFromRow ->END

	nToRow = max(nFromRow, nToRow);
	ROWCOL nCount = nToRow-nFromRow+1;

	ASSERT(pArray);
	// ASSERTION-> pArray must not be NULL. ->END

	ASSERT(AfxIsValidAddress(pArray, (UINT) (sizeof(int)*nCount)));
	// ASSERTION-> pArray is not valid, perhaps too small? ->END

	for (ROWCOL n = 0; n < nCount; n++)
		pArray[n] = GetRowHeight(nFromRow+n);
}

void CGXGridCore::GetArrayWithColWidths(ROWCOL nFromCol, ROWCOL nToCol, int* pArray)
{
	ASSERT(nToCol >= nFromCol);
	// ASSERTION-> nToCol < nFromCol ->END

	nToCol = max(nFromCol, nToCol);
	ROWCOL nCount = nToCol-nFromCol+1;

	ASSERT(pArray);
	// ASSERTION-> pArray must not be NULL. ->END

	ASSERT(AfxIsValidAddress(pArray, (UINT) (sizeof(int)*nCount)));
	// ASSERTION-> pArray is not valid, perhaps too small? ->END

	for (ROWCOL n = 0; n < nCount; n++)
		pArray[n] = GetColWidth(nFromCol+n);
}

/////////////////////////////////////////////////////////////////////////////
// Get rectangle from row, col

CRect CGXGridCore::CalcRectFromRowColEx(ROWCOL nRow, ROWCOL nCol)
{
	return CalcRectFromRowColEx(nRow, nCol, nRow, nCol);
}

CRect CGXGridCore::CalcRectFromRowColExEx(ROWCOL nRow, ROWCOL nCol)
{
	return CalcRectFromRowColExEx(nRow, nCol, nRow, nCol);
}

CRect CGXGridCore::CalcRectFromRowColEx(ROWCOL nStartRow, ROWCOL nStartCol, ROWCOL nEndRow, ROWCOL nEndCol)
{
	CGXRange area = CGXRange(nStartRow, nStartCol, nEndRow, nEndCol);

	MergeCoveredCells(area);

	return CalcRectFromRowCol(area.top, area.left, area.bottom, area.right);
}

CRect CGXGridCore::CalcRectFromRowColExEx(ROWCOL nStartRow, ROWCOL nStartCol, ROWCOL nEndRow, ROWCOL nEndCol)
{
	CGXRange area = CGXRange(nStartRow, nStartCol, nEndRow, nEndCol);

	MergeMergedCells(area);
	MergeFloatedCells(area);

	return CalcRectFromRowColEx(area.top, area.left, area.bottom, area.right);
}

CRect CGXGridCore::CalcRectFromRowCol(ROWCOL nStartRow, ROWCOL nStartCol, ROWCOL nEndRow, ROWCOL nEndCol, BOOL bOutsideClientArea)
{
	ROWCOL    leftCol   = min(nStartCol, nEndCol),
			topRow    = min(nStartRow, nEndRow),
			rightCol  = max(nStartCol, nEndCol),
			bottomRow = max(nStartRow, nEndRow),
			nColClient,
			nRowClient;
	ROWCOL  nfc = GetFrozenCols();
	ROWCOL  nfr = GetFrozenRows();
	CRect   rect,
			rectGrid = GetGridRect();

	rectGrid.right *= 2;
	rectGrid.bottom *= 2;

	// absolutely out of Client area
	if (leftCol > nfc && rightCol < GetLeftCol()
		|| topRow > nfr && bottomRow < GetTopRow()
		// || leftCol && leftCol > CalcRightColFromRect(rectGrid)
		// || topRow && topRow > CalcBottomRowFromRect(rectGrid)
		)
		return CRect(0, 0, 0, 0);

	if (leftCol > 0)
	{
		leftCol = GetClientCol(leftCol);
		if (leftCol == 0)
			leftCol = GetClientCol(GetLeftCol());
	}

	if (topRow > 0)
	{
		topRow  = GetClientRow(topRow);
		if (topRow == 0)
			topRow = GetClientRow(GetTopRow());
	}

	// under client area
	if (!IsPrinting()
		&& (m_nLastVisibleRow != GX_INVALID && topRow > m_nLastVisibleRow
			|| m_nLastVisibleCol != GX_INVALID && leftCol > m_nLastVisibleCol
		   )
		)
		return CRect(0, 0, 0, 0);

	if (rightCol > nfc)
		rightCol = max(nfc, GetClientCol(rightCol));
	if (bottomRow > nfr)
		bottomRow = max(nfr, GetClientRow(bottomRow));

	for (rect.left = rectGrid.left, nColClient = 0;
		(bOutsideClientArea || rect.left <= rectGrid.right) && nColClient < leftCol;
		nColClient++)
		rect.left += GetColWidth(GetCol(nColClient));

	for (rect.top = rectGrid.top, nRowClient = 0;
		(bOutsideClientArea || rect.top <= rectGrid.bottom) && nRowClient < topRow;
		nRowClient++)
		rect.top += GetRowHeight(GetRow(nRowClient));

	for (rect.right = rect.left;
		(bOutsideClientArea || rect.right <= rectGrid.right) && nColClient <= rightCol;
		nColClient++)
		rect.right += GetColWidth(GetCol(nColClient));

	for (rect.bottom = rect.top;
		(bOutsideClientArea || rect.bottom <= rectGrid.bottom) && nRowClient <= bottomRow;
		nRowClient++)
		rect.bottom += GetRowHeight(GetRow(nRowClient));

	// TRACE(_T("CalcRectFromRowCol(%u,%u,%u,%u) returns (%d,%d,%d,%d)\n"),
	//  nStartRow, nStartCol, nEndRow, nEndCol, rect);

	return rect;
}


/////////////////////////////////////////////////////////////////////////////
// Get rows and cols from Points

ROWCOL CGXGridCore::CalcClientColFromPt(CPoint& pt, int* xLeft /* = NULL */)
{
	int      x = GetGridRect().left;  // Left x-Coordinate of RowColRect
	ROWCOL    c = 0;  // current Col

	ASSERT(pt.x >= 0);
	ASSERT(pt.y >= 0);

	if (xLeft)
		*xLeft = x;

	ROWCOL nColCount = GetColCount();

	while (x <= pt.x && GetCol(c) <= nColCount)
	{
		if (xLeft)
			*xLeft = x;
		x += GetColWidth(GetCol(c++));
	}

	if (x == GetGridRect().left && GetCol(c) <= nColCount)
		return 0;
	else if (x < pt.x)
		return GX_INVALID;

	return c-1;
}

ROWCOL CGXGridCore::CalcClientRowFromPt(CPoint& pt, int* yUp /* = NULL */)
{
	int      y = GetGridRect().top;  // top y-Coordinate of RowColRect
	ROWCOL    r = 0;  // current Col

	ASSERT(pt.x >= 0);
	ASSERT(pt.y >= 0);

	if (yUp)
		*yUp = y;

	ROWCOL nRowCount = GetRowCount();

	while (y <= pt.y && GetRow(r) <= nRowCount)
	{
		if (yUp)
			*yUp = y;
		y += GetRowHeight(GetRow(r++));
	}

	if (y == GetGridRect().top && GetRow(r) <= nRowCount)
		return 0;
	else if (y < pt.y)
		return GX_INVALID;

	return r-1;
}

/////////////////////////////////////////////////////////////////////////////
// BottomRight Cell (could use GetGridRect)

ROWCOL CGXGridCore::CalcRightColFromRect(const CRect& rect)
{
	CPoint pt(rect.right, rect.bottom);
	ROWCOL nCol = CalcClientColFromPt(pt);
	return (nCol == GX_INVALID) ? GetColCount() : GetCol(nCol);
}

ROWCOL CGXGridCore::CalcBottomRowFromRect(const CRect& rect)
{
	CPoint pt(rect.right, rect.bottom);
	ROWCOL nRow = CalcClientRowFromPt(pt);
	return (nRow == GX_INVALID) ? GetRowCount() : GetRow(nRow);
}

ROWCOL CGXGridCore::CalcRightColFromRect(const CRect& rect, BOOL& bLastColVisible)
{
	CPoint pt(rect.right, rect.bottom);
	ROWCOL nCol = CalcClientColFromPt(pt);
	return (bLastColVisible = (nCol == GX_INVALID)) != FALSE ? GetColCount() : GetCol(nCol);
}

ROWCOL CGXGridCore::CalcBottomRowFromRect(const CRect& rect, BOOL& bLastRowVisible)
{
	CPoint pt(rect.right, rect.bottom);
	ROWCOL nRow = CalcClientRowFromPt(pt);
	return (bLastRowVisible = (nRow == GX_INVALID)) != FALSE ? GetRowCount() : GetRow(nRow);
}

void CGXGridCore::CalcBottomRightRowColFromRect(const CRect& rect, ROWCOL *nRow, ROWCOL *nCol)
{
	*nRow = CalcBottomRowFromRect(rect);
	*nCol = CalcRightColFromRect(rect);
}

CRect CGXGridCore::CalcClientRectRightFromCol(ROWCOL nCol)
{
	CRect rectGrid = GetGridRect();
	if (nCol > 0)
		rectGrid.left = CalcLeftFromClientCol(GetClientCol(nCol)) + GetColWidth(nCol);
	else
		rectGrid.left += GetColWidth(0);
	return rectGrid;
}

CRect CGXGridCore::CalcClientRectBottomFromRow(ROWCOL nRow)
{
	CRect rectGrid = GetGridRect();
	if (nRow > 0)
		rectGrid.top = CalcTopFromClientRow(GetClientRow(nRow)) + GetRowHeight(nRow);
	else
		rectGrid.top += GetRowHeight(0);
	return rectGrid;
}

/////////////////////////////////////////////////////////////////////////////
// Get coordinates rectangle from row, col

int CGXGridCore::CalcLeftFromClientCol(ROWCOL nCol)
{
	CRect   rectGrid = GetGridRect();
	return  rectGrid.left + (nCol > 0 ? CalcSumOfClientColWidths(0, nCol-1) : 0);
}

int CGXGridCore::CalcTopFromClientRow(ROWCOL nRow)
{
	CRect   rectGrid = GetGridRect();
	return  rectGrid.top + (nRow > 0 ? CalcSumOfClientRowHeights(0, nRow-1) : 0);
}

/////////////////////////////////////////////////////////////////////////////
// Sum of Heights and Widths

int CGXGridCore::CalcSumOfRowHeights(ROWCOL nFromRow, ROWCOL nToRow, int nAbortAt /* = 0 */)
{
	int r = 0;
	for (ROWCOL n = nFromRow; n <= nToRow && (!nAbortAt || r <= nAbortAt); n++)
		r += GetRowHeight(n);
	return r;
}

int CGXGridCore::CalcSumOfColWidths(ROWCOL nFromCol, ROWCOL nToCol, int nAbortAt /* = 0 */)
{
	int r = 0;
	for (ROWCOL n = nFromCol; n <= nToCol && (!nAbortAt || r <= nAbortAt); n++)
		r += GetColWidth(n);
	return r;
}

int CGXGridCore::CalcSumOfClientRowHeights(ROWCOL nFromRow, ROWCOL nToRow, int nAbortAt /* = 0 */)
{
	int r = 0;
	for (ROWCOL n = nFromRow; n <= nToRow && (!nAbortAt || r <= nAbortAt); n++)
		r += GetRowHeight(GetRow(n));
	return r;
}

int CGXGridCore::CalcSumOfClientColWidths(ROWCOL nFromCol, ROWCOL nToCol, int nAbortAt /* = 0 */)
{
	int r = 0;
	for (ROWCOL n = nFromCol; n <= nToCol && (!nAbortAt || r <= nAbortAt); n++)
		r += GetColWidth(GetCol(n));
	return r;
}

/////////////////////////////////////////////////////////////////////////////
// Tracking column or row heights

BOOL CGXGridCore::OnStartTracking(ROWCOL nRow, ROWCOL nCol, int nTrackingMode)
{
	if (m_pUserResizeCellsImp)
		return m_pUserResizeCellsImp->OnStartTracking(this, nRow, nCol, nTrackingMode);

	return FALSE;
}

// Tracking

void CGXGridCore::OnMoveTracking(ROWCOL nRow, ROWCOL nCol, int nTrackingMode, CSize &size)
{
	GX_CHECKIMP(m_pUserResizeCellsImp, "CGXGridCore::OnMoveTracking");

	if (m_pUserResizeCellsImp)
		m_pUserResizeCellsImp->OnMoveTracking(this, nRow, nCol, nTrackingMode, size);
}

void CGXGridCore::OnCancelTracking()
{
	GX_CHECKIMP(m_pUserResizeCellsImp, "CGXGridCore::OnCancelTracking");

	if (m_pUserResizeCellsImp)
		m_pUserResizeCellsImp->OnCancelTracking(this);
}

void CGXGridCore::OnEndTracking(ROWCOL nRow, ROWCOL nCol, int nTrackingMode, CSize& size)
{
	GX_CHECKIMP(m_pUserResizeCellsImp, "CGXGridCore::OnEndTracking");

	if (m_pUserResizeCellsImp)
		m_pUserResizeCellsImp->OnEndTracking(this, nRow, nCol, nTrackingMode, size);
}

// Events

BOOL CGXGridCore::OnTrackColWidth(ROWCOL /*nCol*/)
	{ return m_pUserResizeCellsImp != NULL; }

BOOL CGXGridCore::OnTrackColWidthStart(ROWCOL /*nCol*/)
	{ return TRUE; }

BOOL CGXGridCore::OnTrackColWidthMove(ROWCOL /*nCol*/, int /*nWidth*/)
	{ return TRUE; }

BOOL CGXGridCore::OnTrackColWidthEnd(ROWCOL /*nCol*/, int /*nWidth*/)
	{ return TRUE; }


BOOL CGXGridCore::OnTrackRowHeight(ROWCOL /*nRow*/)
	{ return m_pUserResizeCellsImp != NULL; }

BOOL CGXGridCore::OnTrackRowHeightStart(ROWCOL /*nRow*/)
	{ return TRUE; }

BOOL CGXGridCore::OnTrackRowHeightMove(ROWCOL /*nRow*/, int /*nHeight*/)
	{ return TRUE; }

BOOL CGXGridCore::OnTrackRowHeightEnd(ROWCOL /*nRow*/, int /*nHeight*/)
	{ return TRUE; }


/////////////////////////////////////////////////////////////////////////////
// Hit Testing

int CGXGridCore::HitTest(CPoint &pt, ROWCOL* pnRow, ROWCOL* pnCol, CRect* pRectHit)
{
	// TRACE0("entering CGXGridCore::HitTest()\n");
	int nFrame = m_nHitTestFrame;

	int retValue = GX_NOHIT;
	CPoint  ptTest(
		max(0, pt.x),
		max(0, pt.y));

	int x = 0,
		y = 0;

	// Calculate ClientRow/pnCol
	ROWCOL nRow = CalcClientRowFromPt(ptTest, &y);
	ROWCOL nCol = CalcClientColFromPt(ptTest, &x);

	if (!GetGridRect().PtInRect(ptTest) || !GetGridRect().PtInRect(pt))
		return GX_NOHIT;

	CRect rect;
	BOOL bValid = nRow != GX_INVALID && nCol != GX_INVALID;
	if (bValid)
		rect.SetRect(x, y, x+GetColWidth(GetCol(nCol)), y+GetRowHeight(GetRow(nRow)));

	BOOL bTrackCol = m_pParam->IsEnableTrackColWidth()
			&& (nRow <= GetHeaderRows() || GetParam()->IsEnableTrackColWidth(GX_TRACK_EXTHITTEST));

	BOOL bTrackRow = m_pParam->IsEnableTrackRowHeight()
			&& (nCol <= GetHeaderCols() || GetParam()->IsEnableTrackRowHeight(GX_TRACK_EXTHITTEST));

	// Generate Hittest-Value
	if (bTrackCol && bValid && abs(rect.right-ptTest.x) <= nFrame/2)
	{
		if (nCol > GetHeaderCols() || !GetParam()->IsEnableTrackColWidth(GX_TRACK_NOTHEADER))
			retValue = GX_VERTLINE;
	}
	else if (bTrackCol && bValid && abs(rect.left-ptTest.x) <= nFrame/2 && nCol > 0)
	{
		if (nCol > GetHeaderCols()+1 || !GetParam()->IsEnableTrackColWidth(GX_TRACK_NOTHEADER))
		{
			while (nCol > 0 && GetColWidth(GetCol(--nCol)) == 0)
			{
				rect.right = rect.left;
				rect.left = rect.right-GetColWidth(GetCol(nCol));
			}
			retValue = GX_VERTLINE;
		}
	}
	else if (bTrackRow && bValid && abs(rect.bottom-ptTest.y) <= nFrame/2)
	{
		if (nRow > GetHeaderRows() || !GetParam()->IsEnableTrackRowHeight(GX_TRACK_NOTHEADER))
			retValue = GX_HORZLINE;
	}
	else if (bTrackRow && bValid && abs(rect.top-ptTest.y) <= nFrame/2 && nRow > 0)
	{
		if (nRow > GetHeaderRows()+1 || !GetParam()->IsEnableTrackRowHeight(GX_TRACK_NOTHEADER))
		{
			while (nRow > 0 && GetRowHeight(GetRow(--nRow)) == 0)
			{
				rect.bottom = rect.top;
				rect.top = rect.bottom-GetRowHeight(GetRow(nRow));
			}
			retValue = GX_HORZLINE;
		}
	}

	if (bValid && retValue == GX_NOHIT)
	{
		// OLE Drag&Drop (or let the user add his own hit-test values)
		retValue = OnExtHitTest(GetRow(nRow), GetCol(nCol), ptTest, rect);
	}

	if (bValid && retValue == GX_NOHIT)
	{
		if (nCol > GetHeaderCols() && nRow > GetHeaderRows())
			retValue = GX_CELLHIT;
		else if (bValid)
			retValue = GX_HEADERHIT;
		else
			retValue = GX_NOHIT;
	}

	// Convert pnRow/pnCol to absolute pnRow/pnCol
	if (nRow == GX_INVALID && ptTest.y > nFrame/2+1)
		nRow = GetRowCount()+1;
	else
		nRow = GetRow(nRow);

	if (nCol == GX_INVALID && ptTest.x > nFrame/2+1)
		nCol = GetColCount()+1;
	else
		nCol = GetCol(nCol);

	if (pRectHit)
		*pRectHit = rect;
	if (pnRow)
		*pnRow = nRow;
	if (pnCol)
		*pnCol = nCol;

	return retValue;
}

int CGXGridCore::OnExtHitTest(ROWCOL ncRow, ROWCOL ncCol, CPoint& ptTest, CRect& rect)
{
	if (!m_bHitTestSelEdge)
		return GX_NOHIT;

	int nEdge = 4;

	BOOL bSelEdge = FALSE;
	POSITION pos = 0; // use pos as a recheck that FindRange did return a valid position
	BOOL bValid = TRUE;
	CGXRangeList* pSelList = GetParam()->GetRangeList();
	CGXRange rg;
	CGXRange* pRange = NULL;

	// check if whole table is selected
	BOOL bTableSel = pSelList->FindRange(0,0) != 0;

	if (bTableSel)
	{
		bSelEdge = ncRow == 0 && ncCol == 0
			// check left border
			&& (ptTest.x >= rect.left && ptTest.x-rect.left <= nEdge
			// check top border
			|| ptTest.y >= rect.top && ptTest.y-rect.top <= nEdge);
	}
	else
	{
		if (pSelList->IsEmpty())
		{
			rg.SetCells(ncRow, ncCol);
			GetCoveredCellsRowCol(ncRow, ncCol, rg);

			if (IsCurrentCell(rg.top, rg.left))
				pRange = &rg;
		}

		// check left border
		if (bValid && !bSelEdge && ptTest.x >= rect.left && ptTest.x-rect.left <= nEdge)
		{
			if (!pRange)
			{
				pos = pSelList->FindRange(ncRow, ncCol);
				if (pos != 0)
					pRange = pSelList->GetAt(pos);
				else
					bValid = FALSE;
			}
			bSelEdge = pRange && !pRange->IsRows() && pRange->left == ncCol;
		}

		// check right border
		if (bValid && !bSelEdge && ptTest.x <= rect.right && rect.right-ptTest.x <= nEdge)
		{
			if (!pRange)
			{
				pos = pSelList->FindRange(ncRow, ncCol);
				if (pos != 0)
					pRange = pSelList->GetAt(pos);
				else
					bValid = FALSE;
			}
			bSelEdge = pRange && !pRange->IsRows() && pRange->right == ncCol;
		}

		// check top border
		if (bValid && !bSelEdge && ptTest.y >= rect.top && ptTest.y-rect.top <= nEdge)
		{
			if (!pRange)
			{
				pos = pSelList->FindRange(ncRow, ncCol);
				if (pos != 0)
					pRange = pSelList->GetAt(pos);
				else
					bValid = FALSE;
			}
			bSelEdge = pRange && !pRange->IsCols() && pRange->top == ncRow;
		}

		// check bottom border
		if (bValid && !bSelEdge && ptTest.y <= rect.bottom && rect.bottom-ptTest.y <= nEdge)
		{
			if (!pRange)
			{
				pos = pSelList->FindRange(ncRow, ncCol);
				if (pos != 0)
					pRange = pSelList->GetAt(pos);
				else
					bValid = FALSE;
			}
			bSelEdge = pRange && !pRange->IsCols() && pRange->bottom == ncRow;
		}
	}

	if (bSelEdge)
		return GX_SELEDGEHIT;
	else
		return GX_NOHIT;
}

/////////////////////////////////////////////////////////////////////////////
// Cursor

void CGXGridCore::SetGridCursor(UINT idcCursor)
{
	m_idcCursor = idcCursor;
	CPoint pt(0,0);
	ShowContextCursor(pt);
}

void CGXGridCore::ShowContextCursor(CPoint &point)
{
	UINT    idcPrimary = 0;        // app supplied cursor
	LPCTSTR idcSecondary = 0;      // system supplied cursor

	CWnd* pWnd = m_pGridWnd->GetFocus();

	if (m_idcCursor)
	{
		idcPrimary = m_idcCursor;
		idcSecondary = IDC_ARROW;
	}
	else if (GetParam()->m_bEatFirstMouseClick
		&& m_pGridWnd->IsKindOf(RUNTIME_CLASS(CView))
		&& !(pWnd
			&& m_pGridWnd->GetParentFrame() == pWnd->GetParentFrame()
			&& GXGetActiveView() == m_pGridWnd)
		)
	{
		// MS Office products (and Visual C++) ignore the first WM_LBUTTONDOWN
		// message when the user activates a new window. The if statement is
		// needed for this behaviour.
		//
		// This feature only works for CView-derived grids
		//
		// You can turn off this feature by calling GetParam()->SetEatFirstMouseClick(FALSE).

		idcPrimary = 0;
		idcSecondary = 0;
	}
	else
	{
		ROWCOL    nRow, nCol;
		int     ht = HitTest(point, &nRow, &nCol);

		if (ht == GX_HORZLINE)
		{
			if (m_pParam->IsEnableTrackRowHeight()
				&& OnTrackRowHeight(nRow))
			{
				idcPrimary = GX_IDC_TRACKHEIGHT;
				idcSecondary = IDC_SIZENS;
			}
		}
		else if (ht == GX_VERTLINE)
		{
			if (m_pParam->IsEnableTrackColWidth()
				&& OnTrackColWidth(nCol))
			{
				idcPrimary = GX_IDC_TRACKWIDTH;
				idcSecondary = IDC_SIZEWE;
			}
		}
		else if (ht == GX_SELEDGEHIT)
		{
			// OLE drag&drop cursor
			idcPrimary = GX_IDC_SELDRAG;
			idcSecondary = IDC_ARROW;
		}

		CGXRangeList* pSelList = GetParam()->GetRangeList();

		if (ht == GX_HEADERHIT
			&& nRow == 0 && nCol > 0)
		{
			BOOL bSel = pSelList->IsCellInList(nRow, nCol);
			if (bSel && m_pParam->IsEnableMoveCols())
			{
				idcPrimary = 0;
				idcSecondary = 0;
			}
			else if (m_pParam->IsEnableSelection(GX_SELCOL))
			{
				idcPrimary = GX_IDC_COLHEADER;
				idcSecondary = IDC_ARROW;
			}
		}
		else if (ht == GX_HEADERHIT
			&& nRow > 0 && nCol == 0)
		{
			BOOL bSel = pSelList->IsCellInList(nRow, nCol);
			if (bSel && m_pParam->IsEnableMoveRows())
			{
				idcPrimary = 0;
				idcSecondary = 0;
			}
			else if (m_pParam->IsEnableSelection(GX_SELROW))
			{
				idcPrimary = GX_IDC_ROWHEADER;
				idcSecondary = IDC_ARROW;
			}
		}
	}

	ShowContextCursor(idcPrimary, idcSecondary);
}

void CGXGridCore::ShowContextCursor(UINT idcPrimary, LPCTSTR idcSecondary)
{
	if (idcPrimary != 0)
	{
		HCURSOR hcurToDestroy = NULL;
		if (idcPrimary != GXGetCursorState()->m_idcPrimaryLast)
		{
			HINSTANCE hInst = AfxFindResourceHandle(
				MAKEINTRESOURCE(idcPrimary), RT_GROUP_CURSOR);

			// load in another cursor
			hcurToDestroy = GXGetCursorState()->m_hcurDestroy;

			// Note: If this LoadCursor call fails, it is likely that
			// gxres.rc file is not included in your resource file.
			// To correct the situation, include gxres.rc into your
			// resource script.
			//
			// This should be done using the Resource.Set Includes... command.
			// and including gxresrc.h and gxres.rc

			if ((GXGetCursorState()->m_hcurDestroy = GXGetCursorState()->m_hcurLast =
			   ::LoadCursor(hInst, MAKEINTRESOURCE(idcPrimary))) == NULL)
			{
				// will not look as good
				if (!GXGetCursorState()->m_bNotFound)
				{
					// Display warning message only first time
					TRACE0("Warning: Could not find default cursor - using system provided alternative.\n");
					GXGetCursorState()->m_bNotFound = TRUE;
				}

				ASSERT(GXGetCursorState()->m_hcurDestroy == NULL);    // will not get destroyed
				GXGetCursorState()->m_hcurLast = ::LoadCursor(NULL, idcSecondary);
				ASSERT(GXGetCursorState()->m_hcurLast != NULL);
			}
			GXGetCursorState()->m_idcPrimaryLast = idcPrimary;
		}
		ASSERT(GXGetCursorState()->m_hcurLast != NULL);
		if (GXGetCursorState()->m_hcurLast)
		{
			SetCursor(GXGetCursorState()->m_hcurLast);
			ASSERT(GXGetCursorState()->m_hcurLast != hcurToDestroy);
			if (hcurToDestroy != NULL)
				DestroyCursor(hcurToDestroy); // destroy after being set
		}
	}
	else
	{
		SetCursor(m_hcurArrow);
	}
}

// Zoom

void CGXGridCore::SetZoom(int nZoom, UINT flags /* = GX_UPDATENOW */)
{
	int nOldZoom = GetZoom();

	if (StoreZoom(nZoom))
		UpdateZoom(nOldZoom, flags, TRUE);
}

int CGXGridCore::GetZoom()
{
	if (IsPrinting()
		|| GetParam() == NULL
		|| GetParam()->IsDrawWYSIWYG())
		return 100;

	return m_nZoom;
}

BOOL CGXGridCore::StoreZoom(int nZoom, BOOL bCreateHint)
{
	m_nZoom = nZoom;
	m_pParam->m_pProperties->SetLastZoom(nZoom);

	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintStoreZoom(this, nZoom);

	return TRUE;
}

void CGXGridCore::UpdateZoom(int nOldZoom, UINT flags, BOOL bCreateHint)
{
	FreeObjectCache();
	UpdateFontMetrics();
	DelayFloatCells(CGXRange().SetTable());

	if (!IsLockUpdate())
	{
		if (nOldZoom != GetZoom())
			Redraw(flags, FALSE);
	}

	// Create Hint
	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintUpdateZoom(this, nOldZoom, flags);
}

/////////////////////////////////////////////////////////////////////////////
// Font Metrics

void CGXGridCore::UpdateFontMetrics()
{
	// Default font metrics
	GetDefaultFontSize(m_dxWidth, m_dyHeight);
}

void CGXGridCore::GetDefaultFontSize(int& dxWidth, int& dyHeight)
{
	CGXStyle* pStyle;

	if (!GetParam()->GetStylesMap()->LookupStyle(GetParam()->GetStylesMap()->m_wStyleStandard, pStyle))
	{
		ASSERT(0);
		// ASSERTION-> Standard style not found ->END
	}
	else
	{
		CGXStyle& style = *pStyle;

		if (!style.GetIncludeFont())
		{
			ASSERT(0);
			// ASSERTION-> Standard style without font description ->END

			dyHeight = 15;
			dxWidth = 10;
			return;
		}

		CClientDC dc(m_pGridWnd);
		OnGridPrepareDC(&dc);

		CFont font;
		CFont* pOldFont = NULL;

		if (style.GetIncludeFont())
		{
			pOldFont = LoadFont(&dc, style);
		}

		GetFontMetrics(&dc, dxWidth, dyHeight);

		if (pOldFont)
			dc.SelectObject(pOldFont);

		// Empty the dc which is cached for floating cells, merge cells, ...			
		ReleaseGridDC();
	}
}

void CGXGridCore::GetFontMetrics(CDC* pDC, int& dxWidth, int& dyHeight)
{
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	dyHeight = tm.tmHeight + tm.tmExternalLeading;
	// dxWidth  = tm.tmAveCharWidth * 10 / 7; //  * 2; // tm.tmMaxCharWidth;
	dxWidth  = (tm.tmPitchAndFamily & 1 ? 3 : 2) * tm.tmAveCharWidth / 2;
}

int CGXGridCore::GetFontHeight()
{
	ASSERT(m_bInitDone);    // OnInitialUpdate must have been called!

	return m_bPrintInProgress
		? m_dyPrintHeight
		: m_dyHeight;
}

int CGXGridCore::GetFontWidth()
{
	ASSERT(m_bInitDone);    // OnInitialUpdate must have been called!

	return m_bPrintInProgress
		? m_dxPrintWidth
		: m_dxWidth;
}
