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

// gxcorscr.cpp : Scrolling (CGXGridCore)
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

#include "gxmsg.h"

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCORSCR")
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

/////////////////////////////////////////////////////////////////////////////
// Scrolling

BOOL CGXGridCore::ScrollCellInView(ROWCOL nRow, ROWCOL nCol, UINT flags /* = GX_UPDATENOW */, BOOL bFloatCell)
{
	// ASSERT(!m_bPrintInProgress);

	CRect rectGrid = GetGridRect();
	ROWCOL bottomRow,
		 rightCol;
	BOOL bScrolled = FALSE;

	ROWCOL nRowCount = GetRowCount();
	ROWCOL nColCount = GetColCount();

	CGXRange rgCovered;
	if (GetCoveredCellsRowCol(nRow, nCol, rgCovered))
	{
		// if user clicked on a covered cell which is not
		// completely visible and is used in the frozen
		// cells range, scroll the grid to the most-left
		// or most-top cell.

		if (rgCovered.bottom > GetFrozenRows())
			nRow = max(GetFrozenRows()+1, nRow);

		if (rgCovered.right > GetFrozenCols())
			nCol = max(GetFrozenCols()+1, nCol);
	}

	if (bFloatCell)
	{
		CGXRange rgFloated;
		if (GetFloatedCellsRowCol(nRow, nCol, rgFloated))
		{
			// if user clicked on a Floated cell which is not
			// completely visible and is used in the frozen
			// cells range, scroll the grid to the most-left
			// or most-top cell.

			if (rgFloated.bottom > GetFrozenRows())
				nRow = max(GetFrozenRows()+1, nRow);

			if (rgFloated.right > GetFrozenCols())
				nCol = max(GetFrozenCols()+1, nCol);
		}
	}

	CalcBottomRightRowColFromRect(rectGrid, &bottomRow, &rightCol);

	if (nRow > GetFrozenRows() && nRow < m_nTopRow) // above visible area
	{
		SetTopRow(nRow, flags);
		bScrolled = TRUE;
	}
	else if (nRow > m_nTopRow
		&& nRow >= bottomRow
		&& (bottomRow != nRowCount
		|| CalcTopFromClientRow(GetClientRow(bottomRow+1)) > rectGrid.bottom)
		)
	{
		CGXControl* pControl = GetCurrentCellControl();
		if (pControl)
		{
			MSG msg;
			msg.message = WM_VSCROLL;
			pControl->OnNotifyMsg(&msg);
		}

		ROWCOL topRow = nRow;
		ROWCOL nFrozen = GetFrozenRows();
		int iHeight = GetRowHeight(topRow)+GetRowHeight(0);
		for (ROWCOL i = 0; i < nFrozen; i++)
			iHeight += GetRowHeight(i+1);
		while (iHeight < rectGrid.Height() && topRow > 1)
			iHeight += GetRowHeight(--topRow);
		if (iHeight >= rectGrid.Height() && topRow < nRow)
			topRow++;
		SetTopRow(min(topRow, nRowCount), flags);

		if (flags & (GX_UPDATENOW | GX_SMART))
			UpdateWindow();
		bScrolled = TRUE;
	}

	if (nCol > GetFrozenCols() && nCol < m_nLeftCol) // left of visible area
	{
		SetLeftCol(nCol, flags);
		bScrolled = TRUE;
	}
	else if (nCol > m_nLeftCol
		&& nCol >= rightCol
		&& (rightCol != nColCount
		|| CalcLeftFromClientCol(GetClientCol(rightCol+1)) > rectGrid.right)
		)
	{
		CGXControl* pControl = GetCurrentCellControl();
		if (pControl)
		{
			MSG msg;
			msg.message = WM_HSCROLL;
			pControl->OnNotifyMsg(&msg);
		}

		ROWCOL leftCol = nCol;
		ROWCOL nFrozen = GetFrozenCols();
		int iWidth = GetColWidth(leftCol)+GetColWidth(0);
		for (ROWCOL i = 0; i < nFrozen; i++)
			iWidth += GetColWidth(i+1);
		while (iWidth < rectGrid.Width() && leftCol > 1)
			iWidth += GetColWidth(--leftCol);
		if (iWidth >= rectGrid.Width() && leftCol < nCol)
			leftCol++;
		SetLeftCol(min(leftCol, nColCount), flags);

		bScrolled = TRUE;
	}

	if (flags & (GX_UPDATENOW | GX_SMART))
		UpdateWindow();

	return bScrolled;
}

/////////////////////////////////////////////////////////////////////////////
// ScrollBars

const DWORD maxScrollRange       = SHRT_MAX;

void CGXGridCore::SetScrollBarMode(int nBar, int nSetting, BOOL bRedraw)
{
	if (nBar == SB_BOTH || nBar == SB_VERT)
		m_nVScrollSetting = nSetting;
	if (nBar == SB_BOTH || nBar == SB_HORZ)
		m_nHScrollSetting = nSetting;

	if (bRedraw)
	{
		if (nSetting == gxnDisabled)
			ShowScrollBar(SB_BOTH, FALSE);
		else if (nSetting & gxnEnabled)
			ShowScrollBar(SB_BOTH, TRUE);
		else if (nSetting & gxnAutomatic)
			UpdateScrollbars();
	}
}

int CGXGridCore::GetScrollBarMode(int nBar)
{
	if (nBar == SB_VERT)
		return m_nVScrollSetting;
	else if (nBar == SB_HORZ)
		return m_nHScrollSetting;

	return 0;
}

ROWCOL CGXGridCore::SetScrollPos(int nBar, ROWCOL nPos, BOOL bRedraw /* = TRUE */)
{
	// return if disabled scrollbar
	if (GetScrollBarMode(nBar) == gxnDisabled)
		return 0;

	ROWCOL nFactor = (nBar == SB_VERT) ? m_nVScrollFactor : m_nHScrollFactor;

	if (nPos < nFactor)
		nPos = 0;

	return (ROWCOL) m_pGridWnd->CWnd::SetScrollPos(nBar, (int) (nPos/nFactor), bRedraw)*nFactor;
}

#ifndef SIF_ALL
#define SIF_ALL             0x001F
#endif

void CGXGridCore::SetScrollRange(int nBar, ROWCOL nMinPos, ROWCOL nMaxPos, BOOL bRedraw /* = TRUE */)
{
	// return if disabled scrollbar
	if (GetScrollBarMode(nBar) == gxnDisabled)
		return;

	// Determine if Win95 scrollbars are supported
	BOOL bWin4Scrollbars = FALSE;
#if _MFC_VER >= 0x0300
	SCROLLINFO info;
	info.fMask = SIF_ALL;
	bWin4Scrollbars = m_pGridWnd->CWnd::GetScrollInfo(nBar, &info);
#endif

	int nPage = 0;
	CRect rect = GetGridRect();

	if (nBar == SB_VERT)
	{
		ASSERT(nMinPos > 0);
		m_nVScrollFactor = nMaxPos/maxScrollRange+1;
		if (m_nVScrollSetting & gxnEnhanced && bWin4Scrollbars)
		{
			nPage = (int) ((GetRowCount() - nMaxPos + 1) / m_nVScrollFactor);
			nMaxPos = GetRowCount();
		}
		nMaxPos = nMaxPos/m_nVScrollFactor;
	}
	else if (nBar == SB_HORZ)
	{
		ASSERT(nMinPos > 0);
		m_nHScrollFactor = nMaxPos/maxScrollRange+1;
		if (m_nHScrollSetting & gxnEnhanced && bWin4Scrollbars)
		{
			nPage = (int) ((GetColCount() - nMaxPos + 1) / m_nHScrollFactor);
			nMaxPos = GetColCount();
		}
		nMaxPos = nMaxPos/m_nHScrollFactor;
	}

#if _MFC_VER >= 0x0300
	if (nMaxPos < nMinPos)
	{
		info.fMask = SIF_PAGE|SIF_RANGE;
		info.nMin = nMaxPos;
		info.nMax = nMaxPos;
		info.nPage = 1;
	}
	else
	{
		info.fMask = SIF_PAGE|SIF_RANGE;
		info.nMin = nMinPos;
		info.nMax = nMaxPos;
		info.nPage = nPage;
	}
	if (!bWin4Scrollbars || !m_pGridWnd->CWnd::SetScrollInfo(nBar, &info, bRedraw))
		m_pGridWnd->CWnd::SetScrollRange(nBar, (int) nMinPos, (int) nMaxPos, bRedraw);
#else
	m_pGridWnd->CWnd::SetScrollRange(nBar, (int) nMinPos, (int) nMaxPos, bRedraw);
#endif
	m_bScrollbarsChanged = TRUE;
}

ROWCOL CGXGridCore::GetScrollPos(int nBar) const
{
	ROWCOL nFactor = (nBar == SB_VERT) ? m_nVScrollFactor : m_nHScrollFactor;
	return (ROWCOL) m_pGridWnd->CWnd::GetScrollPos(nBar) * nFactor;
}

void CGXGridCore::GetScrollRange(int nBar, ROWCOL* lpMinPos, ROWCOL* lpMaxPos) const
{
	int     nMinPos,
			nMaxPos;
	ROWCOL nFactor = (nBar == SB_VERT) ? m_nVScrollFactor : m_nHScrollFactor;

	m_pGridWnd->CWnd::GetScrollRange(nBar, &nMinPos, &nMaxPos);
	*lpMinPos = (ROWCOL) nMinPos * nFactor;
	*lpMaxPos = (ROWCOL) nMaxPos * nFactor;
}

BOOL CGXGridCore::LockScrollbars(BOOL bLock)
{
	BOOL bOldLock = m_bLockedScrollbars;
	m_bLockedScrollbars = bLock;
	if (!bLock)
		UpdateScrollbars();
	return bOldLock;
}

void CGXGridCore::UpdateScrollbars(BOOL bRedraw, BOOL bOnlyIfDimensionChanged)
{
	if (!m_bInitDone // OnInitialUpdate must have been called
		|| m_bUpdateScrollbar // Avoid infinite recursion (UpdateScrollbars() will set m_bUpdateScrollbar
		|| m_bLockedScrollbars // LockScrollbars(TRUE) has been called
		|| IsLockUpdate())  // no scrollbar update when drawing is locked
		return;

	// Don't update scrollbars when window is inactive in a workbook
	// so that the scrollbars of the current sheet don't get changed
	if (GetSharedScrollbarParentWnd()
		&& !m_bActiveFrame
		&& !m_pGridWnd->IsWindowVisible())
		return;

	// no scrollbars while printing
	if (IsPrinting())
	{
		ShowScrollBar(SB_HORZ, FALSE);
		ShowScrollBar(SB_VERT, FALSE);
		return;
	}

	// Are both scrollbars disabled?
	if (m_nVScrollSetting == gxnDisabled
			&& m_nHScrollSetting == gxnDisabled)
		return;

	// Make sure that grid window is correctly visible
	CRect   rect;
	GetClientRect(&rect);
	if (rect.right <= rect.left || rect.bottom <= rect.top)
		return;

	// TRACE(_T("CGXGridCore: UpdateScrollBars(%d, %d)\n"), bRedraw, bOnlyIfDimensionChanged);

	// Determine row/column count only if the corresponding
	// scrollbar is not disabled.
	ROWCOL nRowCount = m_nVScrollSetting == gxnDisabled ? 0 : GetRowCount();
	ROWCOL nColCount = m_nHScrollSetting == gxnDisabled ? 0 : GetColCount();

	if (bOnlyIfDimensionChanged)
	{
		// Return immediately if row/column colum count did not change
		// since last call to UpdateScrollbars()
		if (nRowCount == m_nLastSBRowCount && nColCount == m_nLastSBColCount)
			return;
	}

	// Reset last visible row/col so that they will be recalculated again
	m_nLastVisibleRow = GX_INVALID;
	m_nLastVisibleCol = GX_INVALID;

	ROWCOL  nFirstTopRow = GetFrozenRows()+1,
			nFirstLeftCol = GetFrozenCols()+1;

	ROWCOL nLastTopRow = 0, nLastLeftCol = 0;

	// BLOCK: Calculate last possible top-row and left-column
	{
		CRect   rect = GetGridRect();

		// check right col
		nLastLeftCol = nColCount;
		if (m_nHScrollSetting)
		{
			int x = 0;
			rect.left += CalcSumOfClientColWidths(0, max(GetClientCol(GetLeftCol()), 1)-1);
			x = GetColWidth(nLastLeftCol);
			while (x < rect.Width() && nLastLeftCol > nFirstLeftCol)
				x += GetColWidth(--nLastLeftCol);
			if (x >= rect.Width())
				nLastLeftCol++;
		}

		// check bottom Row
		nLastTopRow = nRowCount;
		if (m_nVScrollSetting)
		{
			int y = 0;
			rect.top += CalcSumOfClientRowHeights(0, max(GetClientRow(GetTopRow()), 1)-1);
			y = GetRowHeight(nLastTopRow);
			while (y < rect.Height() && nLastTopRow > nFirstTopRow)
				y += GetRowHeight(--nLastTopRow);
			if (y >= rect.Height())
				nLastTopRow++;
		}
	}

	// Automatic scrolling of the grid so that as many cell as
	// possibile are visible. This is usefull if user did for
	// example enlarge the grid window, so we can scroll some
	// rows into view (gxnEnhanced flag must be set).

#if defined(_MAC) || defined(_UNIX_)
	// For Macintosh I have to set this semaphor
	// before calling UpdateWindow(), because it
	// is more safe to do it before calling UpdateWindow()!

	// I assume that is also more safe for Wind/U to do it
	// here.

	// It would also be safe for MS Windows to set the semaphor
	// at this place but it looks smoother if I set the
	// semaphor after adapting the toprow and/or left column
	// (less flickering).

	// Semaphor avoids infinite recursion
	m_bUpdateScrollbar = TRUE;
#endif

	// Vertikal scrolling
	if ((m_nVScrollSetting & gxnEnhanced) > 0 && GetTopRow() > nFirstTopRow && nLastTopRow < GetTopRow())
	{
		SetTopRow(nLastTopRow);
		UpdateWindow();
	}

	// Horizontal scrolling
	if ((m_nHScrollSetting & gxnEnhanced) > 0 && GetLeftCol() > nFirstLeftCol && nLastLeftCol < GetLeftCol())
	{
		SetLeftCol(nLastLeftCol);
		UpdateWindow();
	}

	// Semaphor avoids infinite recursion
	m_bUpdateScrollbar = TRUE;

	ROWCOL  topRow,
			leftCol;

	GetTopLeftRowCol(&topRow, &leftCol);

	BOOL bHorz = FALSE,
		 bVert = FALSE;

	DWORD dwStyle = m_pGridWnd->GetStyle();

	// Show or Hide scrollbars and mark bVert or bHorz TRUE
	// if the corresponding scrollbar were hidden or shown

	// Vertical scrollbar
	switch (m_nVScrollSetting & ~gxnEnhanced)
	{
	case gxnAutomatic:
		if (topRow > nFirstTopRow || nLastTopRow > nFirstTopRow) // enable scrollbar
		{
			// Set bVert TRUE, when scrollbar was disabled
			bVert = (DWORD) (dwStyle & WS_VSCROLL) == 0;

			// Display and update scrollbar
			SetScrollRange(SB_VERT, nFirstTopRow, nLastTopRow, FALSE);
			SetScrollPos(SB_VERT, topRow, FALSE);
			ShowScrollBar(SB_VERT, TRUE);
		}
		else // disable scrollbar
		{
			// Set bVert TRUE, when scrollbar was enabled
			bVert = (DWORD) (dwStyle & WS_VSCROLL) > 0;
			ShowScrollBar(SB_VERT, FALSE);
		}
		break;

	case gxnEnabled:
		bVert = (DWORD) (dwStyle & WS_VSCROLL) == 0;

		// Display and update scrollbar
		SetScrollRange(SB_VERT, nFirstTopRow, nLastTopRow, FALSE);
		SetScrollPos(SB_VERT, topRow, FALSE);
		ShowScrollBar(SB_VERT, TRUE);
		break;

	case gxnShared:
		SetScrollRange(SB_VERT, nFirstTopRow, nLastTopRow, FALSE);
		SetScrollPos(SB_VERT, topRow, TRUE);
		break;

	case gxnDisabled:
		// Set bVert TRUE, when scrollbar was enabled
		bVert = (DWORD) (dwStyle & WS_VSCROLL) != 0;
		if (bVert)
			ShowScrollBar(SB_VERT, FALSE);
		break;
	}

	// Horizontal scrollbar
	switch (m_nHScrollSetting & ~gxnEnhanced)
	{
	case gxnAutomatic:
		if (leftCol > nFirstLeftCol || nLastLeftCol > nFirstLeftCol)    // enable scrollbar
		{
			// Set bHorz TRUE, when scrollbar was disabled
			bHorz = (DWORD) (dwStyle & WS_HSCROLL) == 0;

			// Display and update scrollbar
			SetScrollRange(SB_HORZ, nFirstLeftCol, nLastLeftCol, FALSE);
			SetScrollPos(SB_HORZ, leftCol, FALSE);
			ShowScrollBar(SB_HORZ, TRUE);
		}
		else // disable scrollbar
		{
			// Set bHorz TRUE, when scrollbar was enabled
			bHorz = (DWORD) (dwStyle & WS_HSCROLL) > 0;
			ShowScrollBar(SB_HORZ, FALSE);
		}
		break;

	case gxnEnabled:
		bHorz = (DWORD) (dwStyle & WS_HSCROLL) == 0;

		// Display and update scrollbar
		SetScrollRange(SB_HORZ, nFirstLeftCol, nLastLeftCol, FALSE);
		SetScrollPos(SB_HORZ, leftCol, FALSE);
		ShowScrollBar(SB_HORZ, TRUE);
		break;

	case gxnShared:
		SetScrollRange(SB_HORZ, nFirstLeftCol, nLastLeftCol, FALSE);
		SetScrollPos(SB_HORZ, leftCol, TRUE);
		break;

	case gxnDisabled:
		// Set bHorz TRUE, when scrollbar was enabled
		bHorz = (DWORD) (dwStyle & WS_HSCROLL) != 0;
		if (bHorz)
			ShowScrollBar(SB_HORZ, FALSE);
		break;
	}

	// Optimized updating of grid window when
	// only the scrollbar regions shall be redrawn
	// and other parts of the window shall not
	// be redrawn.

	if (!bRedraw)
	{
		if (bHorz || bVert)
			m_cxOld = m_cyOld = 0;

		UpdateWindow();
		m_bUpdateScrollbar = FALSE;

		if (bHorz || bVert)
		{
			m_cxOld = m_cyOld = 0;
			CRect rect;
			GetClientRect(&rect);
			if (bHorz)
			{
				// Cancel mouse interactions with the scrollbar
				// ... (don't know hot to do this)

				// Refresh the area previously hidden by the scrollbar
				InvalidateRect(CRect(rect.left,
					rect.bottom-2*GetSystemMetrics(SM_CYHSCROLL),
					rect.right,
					rect.bottom));
				UpdateWindow();
			}
			if (bVert)
			{
				// Cancel mouse interactions with the scrollbar
				// ...

				// Refresh the area previously hidden by the scrollbar
				InvalidateRect(CRect(rect.right-2*GetSystemMetrics(SM_CXVSCROLL),
					rect.top,
					rect.right,
					rect.bottom));
				UpdateWindow();
			}
		}
	}

	// Update scrollbar positions
	if (m_nVScrollSetting)
		SetScrollPos(SB_VERT, topRow);

	if (m_nHScrollSetting)
		SetScrollPos(SB_HORZ, leftCol);

	// Remember row count (see check for bOnlyIfDimensionChanged above)
	m_nLastSBRowCount = nRowCount;
	m_nLastSBColCount = nColCount;

	// Reset semaphor
	m_bUpdateScrollbar = FALSE;
}

BOOL CGXGridCore::DoScroll(int direction, ROWCOL nCell)
{
	// TRACE2("CGXGridCore: BEGIN DoScroll(%d,%u)\n", direction, (WORD) nCell);

	BOOL    retcode = FALSE;
	ROWCOL  wCell;

	ROWCOL  nLeftCol = GetLeftCol(),
			nTopRow = GetTopRow();

	m_nLastVisibleRow = GX_INVALID;
	m_nLastVisibleCol = GX_INVALID;

	CRect   rectEdit;

	ROWCOL nEditRow, nEditCol;
	CGXControl* pControl = NULL;
	if (GetCurrentCell(nEditRow, nEditCol))
	{
		pControl = GetControl(nEditRow, nEditCol);
		rectEdit = CalcRectFromRowCol(nEditRow, nEditCol);
	}

	wCell = (ROWCOL) nCell;

	m_bLockGDIObjects = TRUE;   // Prevents freeing GDIObjects

	switch (direction)
	{
	case GX_LEFT:
		{
			ROWCOL  nLeftClient = GetClientCol(nLeftCol);
			int     x = 0;
			ROWCOL  nPos = max(nLeftCol, nLeftClient+wCell)-wCell;  // nPos > 1
			CRect   rectMove = GetGridRect();
			rectMove.left += CalcSumOfClientColWidths(0, GetCol(nLeftClient-1));
			ROWCOL nfc = GetFrozenCols();

			CRect   rectClip = rectMove;

			if (nLeftCol != nPos)
			{
				while (x < rectMove.Width() && (nLeftCol > nPos || nLeftCol > nfc+1 && GetColWidth(nLeftCol-1) == 0))
					x += GetColWidth(--nLeftCol);

				nLeftCol = max(1, min(nLeftCol, nPos));

				// give the programmer the possibility to inhibit scrolling
#if _MFC_VER >= 0x0400 && !defined(_AFX_NO_OLE_SUPPORT)
				DndDrawMoveRect(CPoint(0,0), TRUE, FALSE);
#endif
				if (!OnLeftColChange(nLeftCol))
					break;

				ToggleSelectionFrameMarker();

				m_nLockCol = m_nLeftCol;  // do not remove cached styles-objects of this column
										  // when calling FreeObjectCache
				m_nLeftCol = nLeftCol;

				rectMove.right -= x-1;

				if (pControl && pControl->IsActive() && !IsFrozenCol(nEditCol) && !rectMove.PtInRect(rectEdit.TopLeft()))
					pControl->Hide();

				if (rectMove.left >= rectMove.right)
					InvalidateRect(rectClip);
				else
				{
					ScrollWindow(x, 0, rectClip, rectClip);
					if (m_pParam->m_pProperties->GetDisplayVertLines()
						&& !GetParam()->GetNewGridLineMode())
					{
						rectClip.right = rectClip.left + x;
						InvalidateRect(rectClip);
					}
				}

				SetScrollPos(SB_HORZ, nLeftCol, TRUE);

				if (m_nHScrollSetting & gxnAutomatic
					&& nLeftCol == nfc+1)
					UpdateScrollbars();

				ToggleSelectionFrameMarker();

				// let the programmer know that grid has scrolled
				OnLeftColChanged();

				retcode = TRUE;
			}
		}
		break;

	case GX_RIGHT:
		{
			ROWCOL  nLeftClient = GetClientCol(nLeftCol);
			int     x = 0, right = 0;
			ROWCOL  nScroll = 0;
			CRect   rectMove =  CalcClientRectRightFromCol(GetCol(nLeftClient-1));

			CRect   rectClip = rectMove;

			// check right col
				// make sure that GetColCount() will not estimate a too big value
			OnTestGridDimension(0, GetColCount());
				// now, it is safe to use GetColCount()
			ROWCOL nLastLeftCol = GetColCount();

			x = GetColWidth(nLastLeftCol);
			while (x < rectMove.Width() && nLastLeftCol > nLeftCol)
				x += GetColWidth(--nLastLeftCol);
			if (x >= rectMove.Width() && nLastLeftCol < GetColCount())
				nLastLeftCol++;

			ROWCOL nCols = nLastLeftCol - nLeftCol;

			while (nLeftCol + wCell -1 < nCols && GetColWidth(nLeftCol + wCell - 1) == 0)
				wCell++;

			wCell = min(nCols, wCell);

			if (nCols > 0)
			{
				for (x = 0; x < rectMove.Width() && nScroll < wCell; )
					x += GetColWidth(nLeftCol+nScroll++);

				nLeftCol += wCell;

				// give the programmer the possibility to inhibit scrolling
#if _MFC_VER >= 0x0400 && !defined(_AFX_NO_OLE_SUPPORT)
				DndDrawMoveRect(CPoint(0,0), TRUE, FALSE);
#endif
				if (!OnLeftColChange(nLeftCol))
					break;

				ToggleSelectionFrameMarker();

				ROWCOL rightCol = CalcClientColFromPt(rectMove.BottomRight(), &right);

				m_nLockCol = GetCol(rightCol);  // do not remove cached styles-objects of this column
												// when calling FreeObjectCache
				m_nLeftCol = nLeftCol;

				rectClip = rectMove;
				rectMove.left += x;

				if (pControl && pControl->IsActive() && !IsFrozenCol(nEditCol) && !rectMove.PtInRect(rectEdit.TopLeft()))
					pControl->Hide();

				if (rectMove.left >= rectMove.right)
					InvalidateRect(rectClip);
				else
				{
					rectClip.right = right;
					ScrollWindow(-x, 0, rectClip, rectClip);
					rectMove.left = right-x;
					rectMove.right = GetGridRect().right;
					InvalidateRect(rectMove);
				}

				SetScrollPos(SB_HORZ, GetLeftCol(), TRUE);

				ToggleSelectionFrameMarker();

				// let the programmer know that grid has scrolled
				OnLeftColChanged();

				retcode = TRUE;
			}
		}
		break;

	case GX_UP:
		{
			ROWCOL  nTopClient = GetClientRow(nTopRow);
			int     y = 0;
			ROWCOL  nPos = max(nTopRow, nTopClient+wCell)-wCell;   // nPos > 1
			CRect   rectMove =  CalcClientRectBottomFromRow(GetRow(nTopClient-1));
			ROWCOL  nfr = GetFrozenRows();

			CRect   rectClip = rectMove;


			if (nTopRow != nPos)
			{
				while (y < rectMove.Height() && (nTopRow > nPos || nTopRow > nfr+1 && GetRowHeight(nTopRow-1) == 0))
					y += GetRowHeight(--nTopRow);

				nTopRow = max(1, min(nTopRow, nPos));

				// give the programmer the possibility to inhibit scrolling
#if _MFC_VER >= 0x0400 && !defined(_AFX_NO_OLE_SUPPORT)
				DndDrawMoveRect(CPoint(0,0), TRUE, FALSE);
#endif
				if (!OnTopRowChange(nTopRow))
					break;

				ToggleSelectionFrameMarker();

				m_nLockRow = m_nTopRow;       // do not remove cached styles-objects of this row
											// when calling FreeObjectCache
				m_nTopRow = nTopRow;

				rectClip = rectMove;
				rectMove.bottom -= y-1;

				if (pControl && pControl->IsActive() && !IsFrozenRow(nEditRow) && !rectMove.PtInRect(rectEdit.TopLeft()))
					pControl->Hide();

				if (rectMove.top >= rectMove.bottom)
					InvalidateRect(rectClip);
				else
				{
					ScrollWindow(0, y, rectClip, rectClip);

					if (m_pParam->m_pProperties->GetDisplayHorzLines()
						&& !GetParam()->GetNewGridLineMode())
					{
						rectClip.bottom = rectClip.top + y;
						InvalidateRect(rectClip);
					}
				}

				SetScrollPos(SB_VERT, GetTopRow(), TRUE);

				if (m_nVScrollSetting & gxnAutomatic
					&& GetTopRow() == nfr+1)
					UpdateScrollbars();

				ToggleSelectionFrameMarker();

				// let the programmer know that grid has scrolled
				OnTopRowChanged();

				retcode = TRUE;
			}
		}
		break;

	case GX_DOWN:
		{
			// TRACE("TopRow = %5lu: ", nTopRow);

			ROWCOL  nTopClient = GetClientRow(nTopRow);
			int     y = 0, bottom = 0;
			ROWCOL  nScroll = 0;
			CRect   rectMove =  CalcClientRectBottomFromRow(GetRow(nTopClient-1));
			CRect   rectClip = rectMove;

			// check bottom row
				// make sure that GetRowCount() will not estimate a too big value
			OnTestGridDimension(GetRowCount(), 0);
				// now, it is safe to use GetRowCount()
			ROWCOL nLastTopRow = GetRowCount();

			y = GetRowHeight(nLastTopRow);
			while (y < rectMove.Height() && nLastTopRow > nTopRow)
				y += GetRowHeight(--nLastTopRow);
			if (y >= rectMove.Height() && nLastTopRow < GetRowCount())
				nLastTopRow++;

			ROWCOL nRows = nLastTopRow - nTopRow;

			while (nTopRow + wCell - 1 < nRows && GetRowHeight(nTopRow + wCell - 1) == 0)
				wCell++;

			wCell = min(nRows, wCell);

			if (nRows > 0)
			{
				for (y = 0; y < rectMove.Height() && nScroll < wCell; )
					y += GetRowHeight(nTopRow+nScroll++);

				nTopRow += wCell;

				// give the programmer the possibility to inhibit scrolling
#if _MFC_VER >= 0x0400 && !defined(_AFX_NO_OLE_SUPPORT)
				DndDrawMoveRect(CPoint(0,0), TRUE, FALSE);
#endif
				if (!OnTopRowChange(nTopRow))
					break;

				ToggleSelectionFrameMarker();

				ROWCOL bottomRow = CalcClientRowFromPt(rectMove.BottomRight(), &bottom);

				m_nLockRow = GetRow(bottomRow); // do not remove cached styles-objects of this row
												// when calling FreeObjectCache
				m_nTopRow = nTopRow;

				rectClip = rectMove;
				rectMove.top += y;

				if (pControl && pControl->IsActive() && !IsFrozenRow(nEditRow) && !rectMove.PtInRect(rectEdit.TopLeft()))
					pControl->Hide();

				if (rectMove.top >= rectMove.bottom)
					InvalidateRect(rectClip);
				else
				{
					rectClip.bottom = bottom;
					ScrollWindow(0, -y, rectClip, rectClip);
					rectMove.top = bottom-y;
					rectMove.bottom = GetGridRect().bottom;
					InvalidateRect(rectMove);
				}

				SetScrollPos(SB_VERT, GetTopRow(), TRUE);

				ToggleSelectionFrameMarker();
				// let the programmer know that grid has scrolled
				OnTopRowChanged();

				retcode = TRUE;
			}
		}
		break;
	}

	// TRACE3("CGXGridCore: END   DoScroll(%d,%u) RET %d\n", direction, nCell, retcode);

	if (!m_bDoScrollNoUpdateWindow)
		UpdateWindow();

	m_bLockGDIObjects = FALSE;
	m_nLockRow = m_nLockCol = 0;

	// TRACE("%5lu Ticks\n", GetTickCount()-dwTickStart);

	return retcode;   // no Scrolling: FALSE
}

BOOL CGXGridCore::OnLeftColChange(ROWCOL nNewLeftCol)
{
	nNewLeftCol;

	return TRUE;
}

BOOL CGXGridCore::OnTopRowChange(ROWCOL nNewTopRow)
{
	nNewTopRow;

	return TRUE;
}


void CGXGridCore::OnLeftColChanged()
{
}

void CGXGridCore::OnTopRowChanged()
{
}

void CGXGridCore::ScrollWindow(int xAmount, int yAmount,
		LPCRECT lpRect /* = NULL */,
		LPCRECT lpClipRect /* = NULL */)
{
	ScrollWindowEx(xAmount, yAmount, lpRect, lpClipRect, NULL, NULL, SW_ERASE | SW_INVALIDATE | SW_SCROLLCHILDREN);
}


void CGXGridCore::AutoScroll(BOOL bSetCapture, BOOL bVertikal, BOOL bHorizontal)
{
	m_bAutoScrollStart = TRUE;
		// will be checked and reset in autoscroll plugin component's
		// WM_TIMER messeage

	m_bAutoScrollVert = bVertikal;
	m_bAutoScrollHorz = bHorizontal; 
	m_bAutoScrollStop = FALSE;

	if (bSetCapture)
		m_pGridWnd->SetCapture();

	m_nAutoScrollTimer = m_pGridWnd->SetTimer(GX_AUTOSCROLLTIMER, 10, NULL);

	if (m_nAutoScrollTimer)
		m_pGridWnd->PostMessage(WM_TIMER, GX_AUTOSCROLLTIMER, 0);
	else
		m_bAutoScrollStart = FALSE;
}

BOOL CGXGridCore::OnAutoScroll(int direction, ROWCOL nCell)
{
	if (DoScroll(direction, nCell))
	{
		const BOOL bCreateHint = 1;
		if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
			m_pUpdateHintImp->HintOnAutoScroll(this, direction, nCell);

		return TRUE;
	}

	return FALSE;
}
