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

// gxcormsg.cpp : message handling in CGXGridCore
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

#ifndef _GXPLUGIN_H_
#include "gxplugin.h"
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

#ifdef TICKCOUNT
#define _TC_(f) f
#else
#define _TC_(f)
#endif

// Control CWnd IDs

#define new DEBUG_NEW


BOOL CGXGridCore::CanChangeTab()
{ 
	return GetParam()->IsEnableChangeTab(); 
}

void CGXGridCore::OnTabChanged(int nTab)
{
	if (m_pAppData->m_pTabWndInt)
	{
		m_pAppData->m_pTabWndInt->UpdateTabName(m_pGridWnd, GetParam()->m_sSheetName, FALSE);

		m_pParam->m_sSheetName = m_pAppData->m_pTabWndInt->GetTabName(m_pGridWnd, nTab);

		const BOOL bCreateHint = TRUE;
		if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
			m_pUpdateHintImp->HintOnTabChanged(this, nTab);
	}
}

void CGXGridCore::OnInfoWndMoveRecord(int nDirection, long lRecord)
{
	switch (nDirection)
	{
	case GX_MOVERECORD_FIRST:   MoveCurrentCellEx(GX_TOP); break;
	case GX_MOVERECORD_PREV:    MoveCurrentCellEx(GX_UP); break;
	case GX_MOVERECORD_NEXT:    MoveCurrentCellEx(GX_DOWN); break;
	case GX_MOVERECORD_LAST:    MoveCurrentCellEx(GX_BOTTOM); break;
	case GX_MOVERECORD_SEEK:
		OnTestGridDimension((ROWCOL) lRecord+GetHeaderRows(), GetRowCount());
		SetCurrentCell(
			min((ROWCOL) lRecord+GetHeaderRows(), GetRowCount()),
			min(GetHeaderCols()+1, GetColCount()));
		m_pGridWnd->SetFocus();
		break;
	}
}

void CGXGridCore::UpdateRecordInfoWnd()
{
	if (m_pAppData->m_pRecordInfoWndInt)
		m_pAppData->m_pRecordInfoWndInt->UpdateRecordInfoWnd(this);

	if (m_pAppData->m_pRecordInfoSplitterWndInt)
		m_pAppData->m_pRecordInfoSplitterWndInt->UpdateRecordInfoWnd(this);
}

//
// CanActivateGrid is typically called when the
// user wants to switch to a new worksheet in a
// CGXTabWnd
//

BOOL CGXGridCore::CanActivateGrid(BOOL bActivate)
{
	if (!bActivate)    // deactivate window
	{
		BOOL bRet = TRUE;

		m_bLockActivateGrid = TRUE;

		ROWCOL nRow, nCol;
		if (GetCurrentCell(nRow, nCol))
		{
			CGXControl* pControl = GetControl(nRow, nCol);

			if (pControl->IsActive())
			{
				HWND hWndFocus = ::GetFocus();

				// Remove any pendig Keyboard messages for the active child window
				if (IsChild(m_pGridWnd->m_hWnd, hWndFocus))
				{
					MSG msg;
					while (::PeekMessage(&msg,  // pointer to structure for message
						hWndFocus,  // handle to window
						WM_KEYFIRST, // first message
						WM_KEYLAST, // last message
						PM_REMOVE)  // removal flags
						)
						NULL;
				}

				if (!pControl->OnValidate())
				{
					if (m_sWarningText.IsEmpty())
					{
						if (pControl->IsActive()
							&& ::IsWindow(hWndFocus)
							&& ::IsWindowVisible(hWndFocus))
							::SetFocus(hWndFocus);

						if (m_pGridWnd->m_hWnd == hWndFocus || ::IsChild(m_pGridWnd->m_hWnd, hWndFocus))
							m_bActiveFrame = TRUE;
					}

					DisplayWarningText();

					m_bRefreshControl = TRUE;

					bRet = FALSE;
				}
			}
		}

		m_bLockActivateGrid = FALSE;

		return bRet;
	}

	return TRUE;
}

CScrollBar* CGXGridCore::DoGetScrollBarCtrl(int nBar) const
{
	ASSERT(nBar == SB_HORZ || nBar == SB_VERT);
	if (m_pGridWnd->GetStyle() & ((nBar == SB_HORZ) ? WS_HSCROLL : WS_VSCROLL))
	{
		// it has a regular windows style scrollbar (no control)
		return NULL;
	}

	CWnd* pParent = GetSharedScrollbarParentWnd();

	if (pParent == NULL)
		return NULL;            // no splitter

	// appropriate PANE id - look for sibling (splitter, or just frame)
	UINT nIDScroll;
	if (nBar == SB_HORZ)
		nIDScroll = AFX_IDW_HSCROLL_FIRST;
	else
		nIDScroll = AFX_IDW_VSCROLL_FIRST;

	// return shared scroll bars that are immediate children of splitter
	return (CScrollBar*)pParent->GetDlgItem(nIDScroll);
}

void CGXGridCore::OnGridPrepareDC(CDC* pDC, CPrintInfo* pInfo /* = NULL */)
{
	if (pInfo)
	{
		// Adjust printer DC
		pDC->SetMapMode(MM_ANISOTROPIC);
		pDC->SetWindowExt(GXGetFontState()->m_nLogPixelsX, GXGetFontState()->m_nLogPixelsY);

		int nlogx = pDC->GetDeviceCaps(LOGPIXELSX);
		int nlogy = pDC->GetDeviceCaps(LOGPIXELSY);

		pDC->SetViewportExt(nlogx, nlogy);

		// Mapping: 72 pixels/unit (like MM_TEXT)
	}
	else if (GetParam()->IsDrawWYSIWYG())
	{
		// make sure grid is drawn exactly the same
		// as it will be printed.

		CGXPrintDevice* pPD = GetParam()->GetPrintDevice();

		// GetPrintDC() will create printer device context
		// when it is called the first time. Further calls
		// will return the previously created device context.

		HDC hPrintDC = 0;
		if (pPD)
			hPrintDC = pPD->GetPrintDC();

		// Make sure, the same mapping mode is used
		if (hPrintDC != NULL)
		{
			pDC->SetAttribDC(hPrintDC);
			SIZE size;
			::SetMapMode(pDC->m_hAttribDC, MM_ANISOTROPIC);
			::SetWindowExtEx(pDC->m_hAttribDC, GXGetFontState()->m_nLogPixelsX, GXGetFontState()->m_nLogPixelsY, &size);
			int nlogx = ::GetDeviceCaps(pDC->m_hAttribDC, LOGPIXELSX);
			int nlogy = ::GetDeviceCaps(pDC->m_hAttribDC, LOGPIXELSY);

			::SetViewportExtEx(pDC->m_hAttribDC, nlogx, nlogy, &size);
		}
	}
}

BOOL CGXGridCore::OnActivateGrid( BOOL bActivate )
{
	// cancel selecting cells
	m_pGridWnd->SendMessage(WM_CANCELMODE, 0, 0);

	GX_FR_STATE* pState = GXGetLastFRState();
	if (bActivate)
		pState->m_pActiveGrid = this;

	// m_bLockActivateGrid is a semaphor which
	// tells me if I should ignore the
	// deactivation/activation of the grid

	if (bActivate || !m_bLockActivateGrid)
		m_bActiveFrame = bActivate;

	if (m_bLockActivateGrid || m_bLockCurrentCell || !m_bInitDone || GetParam() == NULL)
	{
		return FALSE;
	}

	// TRACE0("OnActivateGrid\n");

	BOOL bRet = TRUE;
	if (!bActivate)
	{
		// First, cancel all internal operations
		// such as Selecting cells, dragging, tracking
		// TRACE0("Cancel\n");
		m_pGridWnd->SendMessage(WM_CANCELMODE, 0, 0);
	}

	if (IsCurrentCell() && !GXDiscardNcActivate())
	{
		ROWCOL nRow, nCol;
		if (GetCurrentCell(nRow, nCol))
		{
			CGXControl* pControl = GetControl(nRow, nCol);
			if (bActivate)
			{
				if (GetParam()->GetActivateCellFlags() & GX_CAFOCUS_SETCURRENT)
					pControl->SetActive(TRUE);
				pControl->Refresh();
				// if the control is active, CGXControl::Draw will
				// automatically set the focus to the control
			}
			else if (pControl->IsInit())
			{
				if (CanSelectCurrentCell(FALSE, nRow, nCol, nRow, nCol))
				{
					bRet = TRUE;
					BOOL bModified = pControl->GetModify();

					if (bModified)
						bRet = pControl->Store();

					if (bRet)
						bRet = pControl->OnEndEditing();

					if (bRet)
					{
						pControl->SetActive(FALSE);
						pControl->SetModify(FALSE);
						pControl->Refresh();
					}

					if (bModified)
					{
						Recalc();

						RefreshViews(FALSE, NULL, TRUE);
					}
				}
				else
					bRet = FALSE;
			}
			else
				RedrawRowCol(nRow, nCol, GX_INVALIDATE, FALSE);
		}
	}

	if (bActivate)
	{
		UpdateFontMetrics();    // Actualizes the internally stored font metrics.
		UpdateScrollbars();     // Updates the scrollbars
	}

	m_cxOld = m_cyOld = 0;

	if (bRet && bActivate)
	{
		// Record Window support
		if (m_pAppData->m_pRecordInfoWndInt)
			m_pAppData->m_pRecordInfoWndInt->SetActivePane(this);

		if (m_pAppData->m_pRecordInfoSplitterWndInt)
			m_pAppData->m_pRecordInfoSplitterWndInt->SetActivePane(this);
	}

	return bRet;
}


/////////////////////////////////////////////////////////////////////////////
// CGXGridCore message handlers

BOOL CGXGridCore::DoNotifyDestroy()
{
	GX_FR_STATE* pState = GXGetLastFRState();

	if (pState->pFindReplaceDlg != NULL && pState->pFindReplaceDlgOwner == m_pGridWnd)
	{
		pState->pFindReplaceDlg->SendMessage(WM_CLOSE);
		pState->pFindReplaceDlg = NULL;
	}

	return FALSE;
}

BOOL CGXGridCore::DoVScroll(UINT nSBCode, UINT nPos, CScrollBar*)
{
	if (!m_bInitDone || m_nNestedDraw > 0)
		return FALSE;

	CGXControl* pControl = GetCurrentCellControl();
	if (pControl)
	{
		MSG msg;
		msg.message = WM_VSCROLL;
		pControl->OnNotifyMsg(&msg);
	}

	ROWCOL nRowCount = GetRowCount();
	ROWCOL nfr = GetFrozenRows()+1;

	if (nRowCount == 0 || nfr >= nRowCount)
		return FALSE;

	ASSERT(m_nTopRow > 0);
	ASSERT(nRowCount >= m_nTopRow-1);

	ROWCOL dwPos = (ROWCOL) (max(nfr, nPos)-nfr) * m_nVScrollFactor + nfr;

	switch (nSBCode)
	{
	case SB_BOTTOM:
		if (m_nTopRow != GetRowCount())
			DoScroll(GX_DOWN, nRowCount-m_nTopRow);
		break;

	case SB_ENDSCROLL:
		if (m_nAutoScrollDirection != -1)
			m_bAutoScrollStop = TRUE;
		break;

	case SB_LINEDOWN:
		DoScroll(GX_DOWN, 1);
		
		if (m_bAccelScrollbars && m_nAutoScrollDirection != GX_DOWN)
		{
			AutoScroll(0, FALSE, FALSE);
			m_nAutoScrollDirection = GX_DOWN;
		}
		break;

	case SB_LINEUP:
		DoScroll(GX_UP, 1);

		if (m_bAccelScrollbars && m_nAutoScrollDirection != GX_UP)
		{
			AutoScroll(0, FALSE, FALSE);
			m_nAutoScrollDirection = GX_UP;
		}
		break;

	case SB_PAGEDOWN:
		if (m_nTopRow < nRowCount)
		{
			ROWCOL    nRow = CalcBottomRowFromRect(GetGridRect());

			if (nRow < m_nTopRow)
				// All visible rows are frozen
				break;

			DoScroll(GX_DOWN, max(1, nRow-m_nTopRow));
		}
		break;

	case SB_PAGEUP:
		if (m_nTopRow > 1)
		{
			CRect   rectGrid = GetGridRect();
			int    dy = rectGrid.Height()-CalcSumOfRowHeights(0, GetFrozenRows());
			int    y = GetRowHeight(m_nTopRow);
			ROWCOL  ndRow = 0;

			while (y < dy && m_nTopRow-ndRow > 1)
				y += GetRowHeight(m_nTopRow-(++ndRow));

			ASSERT(ndRow < m_nTopRow);

			DoScroll(GX_UP, max(1, ndRow));
		}
		break;

	case SB_THUMBPOSITION:
		if (dwPos < m_nTopRow && CalcSumOfRowHeights(dwPos, m_nTopRow, 1) > 0)
			DoScroll(GX_UP, m_nTopRow-dwPos);
		else if (dwPos > m_nTopRow && CalcSumOfRowHeights(m_nTopRow, dwPos, 1) > 0)
			DoScroll(GX_DOWN, dwPos-m_nTopRow);
		break;

	case SB_THUMBTRACK:
		if (m_pParam->IsEnableThumbTrack())
		{
			if (dwPos < m_nTopRow && CalcSumOfRowHeights(dwPos, m_nTopRow, 1) > 0)
				DoScroll(GX_UP, m_nTopRow-dwPos);
			else if (dwPos > m_nTopRow && CalcSumOfRowHeights(m_nTopRow, dwPos, 1) > 0)
				DoScroll(GX_DOWN, dwPos-m_nTopRow);
		}
		break;


	case SB_TOP:
		if (m_nTopRow != 1)
			DoScroll(GX_UP, m_nTopRow-1);
		break;
	}
	return TRUE;
}

BOOL CGXGridCore::DoHScroll(UINT nSBCode, UINT nPos, CScrollBar*)
{
	if (!m_bInitDone || m_nNestedDraw > 0)
		return FALSE;

	CGXControl* pControl = GetCurrentCellControl();
	if (pControl)
	{
		MSG msg;
		msg.message = WM_HSCROLL;
		pControl->OnNotifyMsg(&msg);
	}

	ROWCOL nColCount = GetColCount();
	ROWCOL nfc = GetFrozenCols()+1;

	if (nColCount == 0 || nfc >= nColCount)
		return FALSE;

	ASSERT(m_nLeftCol > 0);
	ASSERT(nColCount >= m_nLeftCol-1);

	ROWCOL dwPos = (ROWCOL) (max(nfc, nPos)-nfc) * m_nHScrollFactor + nfc;

	switch (nSBCode)
	{
	case SB_BOTTOM:
		if (m_nLeftCol != nColCount)
			DoScroll(GX_DOWN, GetColCount()-m_nLeftCol);
		break;

	case SB_ENDSCROLL:
		if (m_nAutoScrollDirection != -1)
			m_bAutoScrollStop = TRUE;
		break;

	case SB_LINEDOWN:
		DoScroll(GX_RIGHT, 1);
		
		if (m_bAccelScrollbars && m_nAutoScrollDirection != GX_RIGHT)
		{
			AutoScroll(0, FALSE, FALSE);
			m_nAutoScrollDirection = GX_RIGHT;
		}
		break;

	case SB_LINELEFT:
		DoScroll(GX_LEFT, 1);
		
		if (m_bAccelScrollbars && m_nAutoScrollDirection != GX_LEFT)
		{
			AutoScroll(0, FALSE, FALSE);
			m_nAutoScrollDirection = GX_LEFT;
		}
		break;

	case SB_PAGEDOWN:
		if (m_nLeftCol < nColCount)
		{
			ROWCOL nCol = CalcRightColFromRect(GetGridRect());

			if (nCol < m_nLeftCol)
				// All visible columns are frozen
				break;

			DoScroll(GX_RIGHT, max(nCol-m_nLeftCol, 1));
		}
		break;

	case SB_PAGEUP:
		if (m_nLeftCol > 1)
		{
			CRect   rectGrid = GetGridRect();
			int    dx = rectGrid.Width()-CalcSumOfColWidths(0, GetFrozenCols());
			int    x = GetColWidth(m_nLeftCol);
			ROWCOL  ndCol = 0;

			while (x < dx && m_nLeftCol-ndCol > 1)
				x += GetColWidth(m_nLeftCol-(++ndCol));

			ASSERT(ndCol < m_nLeftCol);

			DoScroll(GX_LEFT, max(1, ndCol));
		}
		break;

	case SB_THUMBPOSITION:
		if (dwPos < m_nLeftCol && CalcSumOfColWidths(dwPos, m_nLeftCol, 1) > 0)
			DoScroll(GX_LEFT, m_nLeftCol-dwPos);
		else if (dwPos > m_nLeftCol && CalcSumOfColWidths(m_nLeftCol, dwPos, 1) > 0)
			DoScroll(GX_RIGHT, dwPos-m_nLeftCol);
		break;

	case SB_THUMBTRACK:
		if (m_pParam->IsEnableThumbTrack())
		{
			if (dwPos < m_nLeftCol && CalcSumOfColWidths(dwPos, m_nLeftCol, 1) > 0)
				DoScroll(GX_LEFT, m_nLeftCol-dwPos);
			else if (dwPos > m_nLeftCol && CalcSumOfColWidths(m_nLeftCol, dwPos, 1) > 0)
				DoScroll(GX_RIGHT, dwPos-m_nLeftCol);
		}
		break;

	case SB_TOP:
		if (m_nLeftCol != 1)
			DoScroll(GX_LEFT, m_nLeftCol-1);
		break;
	}

	return TRUE;
}

BOOL CGXGridCore::DoSetCursor(CWnd* pWnd, UINT nHitTest, UINT /*message*/)
{
	if (!m_bInitDone)
		return FALSE;

	if (pWnd == m_pGridWnd && nHitTest == HTCLIENT /* && !m_bTracking */)
		return TRUE;    // we will handle it in the mouse move
	return FALSE;
}

BOOL CGXGridCore::DoMouseMove(UINT flags, CPoint point)
{
	if (!m_bInitDone)
		return FALSE;
	
	// Process mouse move
	TRY
	{
		CRect rectGrid = GetGridRect();
		
#if _MFC_VER >= 0x0400
		// Forward MouseMove to ToolTip
		if (m_bTtnIsEnabled && m_pTooltipProxy && GetCapture() != m_pGridWnd)
			m_pTooltipProxy->OnMouseMove(flags, point);
#endif
		
		CPoint ptOrg = point;
		
		// Convert to valid point
		point.x = min(max(point.x, rectGrid.left), rectGrid.right-m_dyFrame);
		point.y = min(max(point.y, rectGrid.top), rectGrid.bottom-m_dyFrame);
		
		// If the user hit a selected row or column,
		// the grid was changed to dragging-mode.
		//
		// When dragging-mode, there will be no calls to OnLButtonHitRowCol.
		//
		
		if (m_pUserDragSelectRangeImp
			&& m_pUserDragSelectRangeImp->OnMouseMove(this, point, flags))
			return TRUE;
		
		// Check, if grid is in hit-mode.
		//
		// The programmer can modify the default behaviour by
		// specifiying GetParam()->EnableSelection() and/or
		// overriding OnLButtonHitRowCol
		//
		
		else if (m_nHitState)
		{
			ASSERT(GetCapture() == m_pGridWnd);
			
			if (GetCapture() != m_pGridWnd)
				return FALSE;
			
			ROWCOL nRow, nCol;
			
			// Get actual coordinates
			HitTest(point, &nRow, &nCol);
			
			//nRow = min(max(1, nRow), GetRowCount());
			//nCol = min(max(1, nCol), GetColCount());
			nRow = min(max(GetRow(GetHeaderRows()+1), nRow), GetRowCount());
			nCol = min(max(GetCol(GetHeaderCols()+1), nCol), GetColCount());
			
			// OnLButtonHitRowCol will process application specific behaviour
			// for dragging cells (and call the control's MouseMove method)
			//
			
			WORD w = (WORD) (m_nHitState | GX_HITMOVE);
			
			if (nRow == m_nHitRow && nCol == m_nHitCol)
				w |= GX_HITINCELL;
			else if (m_dwLastMoveRow == m_nHitRow && m_dwLastMoveCol == m_nHitCol)
				w |= GX_HITLEAVE;
			
			
			if (m_nMouseFlags & MK_LBUTTON)
				OnLButtonHitRowCol(m_nHitRow, m_nHitCol, nRow, nCol, point, flags, w);
			else if (m_nMouseFlags & MK_MBUTTON)
				OnMButtonHitRowCol(m_nHitRow, m_nHitCol, nRow, nCol, point, flags, w);
			else if (m_nMouseFlags & MK_RBUTTON)
				OnRButtonHitRowCol(m_nHitRow, m_nHitCol, nRow, nCol, point, flags, w);
			
			// if coordinates do not have changed, return FALSE
			
			if (nRow == m_dwLastMoveRow && nCol == m_dwLastMoveCol)
				return TRUE;
			
			// Next block contains functionality for selecting cells.
			//
			// The programmer can inhibit this behaviour by specifying
			// GetParam()->EnableSelection(FALSE).
			//
			
			if (m_pUserSelectRangeImp)
				m_pUserSelectRangeImp->OnMouseMove(this, point, flags, nRow, nCol);
			
			// save current coordinates
			m_dwLastMoveRow = nRow;
			m_dwLastMoveCol = nCol;
			
			// ... they will be checked in next MouseMove call
			
			return TRUE;
		}
		
		// check, if grid is in tracking mode
		//
		
		else if (m_pUserResizeCellsImp
			&& m_pUserResizeCellsImp->OnMouseMove(this, point, flags))
			return TRUE;
		
		else if (rectGrid.PtInRect(ptOrg) && !m_bDisableMouseMoveOver)
		{
			ROWCOL nRow, nCol;
			
			// Get actual coordinates
			HitTest(point, &nRow, &nCol);
			
			nRow = min(nRow, GetRowCount());
			nCol = min(nCol, GetColCount());
			
			// Give the control the chance to hand mouse movement
			// e.g. change cursor
			
			CGXRange rgCovered;
			if (GetCoveredCellsRowCol(nRow, nCol, rgCovered))
			{
				nRow = rgCovered.top;
				nCol = rgCovered.left;
			}
			
			CGXControl* pControl = GetControl(nRow, nCol);
			
			if (!pControl->MouseMoveOver(nRow, nCol, flags, point))
				ShowContextCursor(point);
			
			return TRUE;
		}
		
		else 
		{
			ShowContextCursor(ptOrg);
			
			return TRUE;
		}
		
	}
	CATCH(CException, e)
	{
		m_pGridWnd->SendMessage(WM_CANCELMODE, 0, 0);
	}
	END_CATCH

	return TRUE;
}

BOOL CGXGridCore::OnMouseMoveOver(ROWCOL nRow, ROWCOL nCol, UINT flags, CPoint pt)
{
	// Unused:
	nRow, nCol, flags, pt;

	return FALSE;
}

static BOOL AFXAPI _doButtonDown1(CGXGridCore* pGrid, UINT flags, CPoint point, UINT xflags, int& ht, ROWCOL& nRow, ROWCOL& nCol)
{
	if (!pGrid->m_bInitDone)
		return FALSE;

	// Process middle mouse click

	pGrid->m_nMouseFlags = flags;
	pGrid->m_bActiveFrame = TRUE;

	// If the grid is already in a special mode, cancel the current mode
	// and return without further processing.

	if (pGrid->GetCapture() == pGrid->m_pGridWnd || flags & xflags)
	{
		// If the grid is already in a special mode, cancel the current mode.
		pGrid->m_pGridWnd->SendMessage(WM_CANCELMODE, 0, 0);
		pGrid->DoCancelMode();
		return FALSE;
	}

	// Be sure, all grid contents are up to date

	pGrid->UpdateWindow();

	// determine context of click

	ht = pGrid->HitTest(point, &nRow, &nCol);

	return TRUE;
}

BOOL CGXGridCore::DoLButtonDown(UINT flags, CPoint point)
{
	TRY
	{
		ROWCOL  nRow, nCol;
		int ht;
		if (!_doButtonDown1(this, flags, point, MK_MBUTTON | MK_RBUTTON, ht, nRow, nCol))
			return FALSE;
		
		// check, if user clicked on a header or cell in the grid
		//
		// check first if OLE drag&drop shall be started
		if (ht == GX_SELEDGEHIT)
		{
			if (m_nDndFlags)
			{
				ROWCOL ncRow = nRow;
				ROWCOL ncCol = nCol;
				
				CGXRange rgCovered(nRow, nCol);
				if (GetCoveredCellsRowCol(nRow, nCol, rgCovered))
				{
					ncRow = rgCovered.top;
					ncCol = rgCovered.left;
				}
				
				if (DndStartDragDrop(ncRow, ncCol))
					return TRUE;
			}
			
			ht = GX_CELLHIT;
		}
		
		if (ht == GX_CELLHIT || ht == GX_HEADERHIT)
		{
			// If user clicked top-left cell
			if (nRow == 0 && nCol == 0)
			{
				// ... select/deselect whole table
				
				m_nHitState = GX_HIT;
				m_nHitRow   = nRow;
				m_nHitCol   = nCol;
				
				if (IsCurrentCell(m_nHitRow, m_nHitCol))
					m_nHitState |= GX_HITCURRENTCELL;
				
				if (m_pUserSelectRangeImp)
					m_pUserSelectRangeImp->OnLButtonDown(this, point, flags, ht, nRow, nCol);
				
				m_dwLastMoveRow   = nRow;
				m_dwLastMoveCol   = nCol;
				
				// OnLButtonHitRowCol will process application specific behaviour
				// for dragging cells. When OnLButtonHitRowCol returns FALSE,
				// the current mouse action will be canceled.
				//
				if (OnLButtonHitRowCol(nRow, nCol, nRow, nCol, point, flags,
					(WORD) (m_nHitState | GX_HITSTART | GX_HITINCELL)))
				{
					// The programmer can initialize autoscrolling or mouse capturing
					// in OnLButtonHitRowCol.
					//
					// If he did not I will do it myself (this is the default).
					//
					if (GetCapture() != m_pGridWnd && m_nHitState)
						SetCapture();
				}
				else
					m_pGridWnd->SendMessage(WM_CANCELMODE, 0, 0);
			}
			else
			{
				if (m_pUserDragSelectRangeImp
					&& m_pUserDragSelectRangeImp->OnLButtonDown(this, point, flags, ht, nRow, nCol))
					return TRUE;
				
				// Otherwise, the grid is changed into hit-mode.
				// The programmer can modify the default behaviour by
				// specifiying GetParam()->EnableSelection() and/or
				// overriding OnLButtonHitRowCol
				//
				
				// else
				{
					// Hit normal Cell or fixed Cell
					
					BOOL bCtl = flags & MK_CONTROL;
					
					m_nHitState = GX_HIT;
					m_nHitRow   = nRow;
					m_nHitCol   = nCol;
					
					if (IsCurrentCell(m_nHitRow, m_nHitCol) && GetControl(m_nHitRow, m_nHitCol)->IsInit())
						m_nHitState |= GX_HITCURRENTCELL;
					
					if (bCtl)
						m_nHitState |= GX_CTRLPRESSED;
					
					// Next block contains functionality for selecting cells.
					//
					// The programmer can inhibit this behaviour by specifying
					// GetParam()->EnableSelection(FALSE).
					//
					
					if (m_pUserSelectRangeImp)
						m_pUserSelectRangeImp->OnLButtonDown(this, point, flags, ht, nRow, nCol);
					
					m_dwLastMoveRow   = nRow;
					m_dwLastMoveCol   = nCol;
					
					// OnLButtonHitRowCol will process application specific behaviour
					// for dragging cells. When OnLButtonHitRowCol returns FALSE,
					// the current mouse action will be canceled.
					//
					if (OnLButtonHitRowCol(nRow, nCol, nRow, nCol, point, flags,
						(WORD) (m_nHitState | GX_HITSTART | GX_HITINCELL)))
					{
						// The programmer can initialize autoscrolling or mouse capturing
						// in OnLButtonHitRowCol.
						//
						// If he did not, I will do it myself (this is the default),
						//
						
						if (GetCapture() != m_pGridWnd && m_nHitState)
						{
							// Catch all Mouse events
							if (m_bAutoScroll)
								// I am using AutoScroll until WM_LBUTTONUP Message
								AutoScroll(FALSE,
								nRow > 0 && !IsFrozenRow(nRow),
								nCol > 0 && !IsFrozenCol(nCol));
							
							SetCapture();
						}
					}
					else
						m_pGridWnd->SendMessage(WM_CANCELMODE, 0, 0);
				}
			}
		}
	
		// check, if user clicked on a vertical line to resize the column
		// ... (behaviour can be changed with GetParam()->EnableTrackColWidth(...)
		// and GetParam()->SetRowHeaderLines(...)
		
		else if (m_pUserResizeCellsImp
			&& m_pUserResizeCellsImp->OnLButtonDown(this, point, flags, ht, nRow, nCol))
			return TRUE;
		
		// user clicked outside the grid
		
		else
			return FALSE;
		
	}
	CATCH(CException, e)
	{
		m_pGridWnd->SendMessage(WM_CANCELMODE, 0, 0);
	}
	END_CATCH
		
	return TRUE;
}


BOOL CGXGridCore::DoLButtonUp(UINT flags, CPoint point)
{
	m_bAutoScrollStop = TRUE;

	if (!(GetCapture() == m_pGridWnd)) {
		return FALSE;
	}

	// TRACE2("CGXGridCore: OnLButtonUp(%u, (%d,%d))\n", flags, point);

	TRY
	{
		CRect rectGrid = GetGridRect();
		point.x = min(max(point.x, 0), rectGrid.right-m_dyFrame);
		point.y = min(max(point.y, 0), rectGrid.bottom-m_dyFrame);

		// If the user hit a selected row or column,
		// the grid was changed to dragging-mode.
		//
		// When dragging-mode, there will be no call to OnLButtonHitRowCol.
		//

		if (m_pUserDragSelectRangeImp
			&& m_pUserDragSelectRangeImp->OnLButtonUp(this, point, flags))
			return TRUE;

		// Check, if grid is in hit-mode.
		//

		else if (m_nHitState)
		{
			// Release Mouse Capture
			if (m_pGridWnd->GetCapture() == m_pGridWnd)
				ReleaseCapture();

			// Next block contains functionality for selecting cells.
			//
			// The programmer can inhibit this behaviour by specifying
			// GetParam()->EnableSelection(FALSE).
			//

			if (m_pUserSelectRangeImp)
				m_pUserSelectRangeImp->OnLButtonUp(this, point, flags);

			// OnLButtonHitRowCol will process application specific behaviour
			// for dragging cells (and call the control's LButtonUp method)
			//

			ROWCOL nRow, nCol;
			HitTest(point, &nRow, &nCol);

			WORD w = (WORD) (m_nHitState | GX_HITEND);
			if (nRow == m_nHitRow && nCol == m_nHitCol)
				w |= GX_HITINCELL;
			else if (m_dwLastMoveRow == m_nHitRow && m_dwLastMoveCol == m_nHitCol)
				w |= GX_HITLEAVE;

			// it is important to use min(GetRowCount(), nRow) because
			// HitTest returns nRow+1 if user released the mouse button
			// outside the grid.

			OnLButtonHitRowCol(m_nHitRow, m_nHitCol, min(GetRowCount(), nRow), min(GetColCount(), nCol), point, flags, w);

			// reset semaphor for "hit-mode"

			m_nHitState = 0;
		}

		else if (m_pUserResizeCellsImp
			&& m_pUserResizeCellsImp->OnLButtonUp(this, point, flags))
			return TRUE;

	}
	CATCH(CException, e)
	{
		m_pGridWnd->SendMessage(WM_CANCELMODE, 0, 0);
	}
	END_CATCH
		
	return TRUE;
}

static BOOL AFXAPI _doButtonDown(CGXGridCore* pGrid, UINT flags, CPoint point, UINT xflags)
{
	ROWCOL  nRow, nCol;
	int ht;
	if (_doButtonDown1(pGrid, flags, point, xflags, ht, nRow, nCol))
	{
		// check, if user clicked on a header or cell in the grid
		//
		if (ht == GX_CELLHIT || ht == GX_HEADERHIT)
		{
			// Hit normal Cell or fixed Cell

			pGrid->m_nHitState = GX_HIT;
			pGrid->m_nHitRow   = nRow;
			pGrid->m_nHitCol   = nCol;

			if (pGrid->IsCurrentCell(pGrid->m_nHitRow, pGrid->m_nHitCol))
				pGrid->m_nHitState |= GX_HITCURRENTCELL;

			if (flags & MK_CONTROL)
				pGrid->m_nHitState |= GX_CTRLPRESSED;

			pGrid->m_dwLastMoveRow   = nRow;
			pGrid->m_dwLastMoveCol   = nCol;

			return TRUE;
		}
	}

	return FALSE;
}

static BOOL AFXAPI _doButtonUp(CGXGridCore* pGrid, UINT flags, CPoint point, int& ht, ROWCOL& nRow, ROWCOL& nCol, WORD& w)
{
	GX_UNUSED_ALWAYS(flags);

	// TRACE2("CGXGridCore: OnLButtonUp(%u, (%d,%d))\n", flags, point);

	CRect rectGrid = pGrid->GetGridRect();
	point.x = min(max(point.x, 0), rectGrid.right-pGrid->m_dyFrame);
	point.y = min(max(point.y, 0), rectGrid.bottom-pGrid->m_dyFrame);

	// Check, if grid is in hit-mode.
	//

	if (pGrid->m_nHitState)
	{
		// Release Mouse Capture
		if (pGrid->m_pGridWnd->GetCapture() == pGrid->m_pGridWnd)
			ReleaseCapture();

		// OnMButtonHitRowCol will process application specific behaviour
		// for dragging cells (and call the control's MButtonUp method)
		//

		ht = pGrid->HitTest(point, &nRow, &nCol);

		w = (WORD) (pGrid->m_nHitState | GX_HITEND);
		if (nRow == pGrid->m_nHitRow && nCol == pGrid->m_nHitCol)
			w |= GX_HITINCELL;
		else if (pGrid->m_dwLastMoveRow == pGrid->m_nHitRow && pGrid->m_dwLastMoveCol == pGrid->m_nHitCol)
			w |= GX_HITLEAVE;

		return TRUE;
	}

	return FALSE;
}

BOOL CGXGridCore::DoMButtonDown(UINT flags, CPoint point)
{
	TRY
	{
		if (_doButtonDown(this, flags, point, MK_LBUTTON | MK_RBUTTON))
		{
			// OnMButtonHitRowCol will process application specific behaviour
			// for dragging cells. When OnMButtonHitRowCol returns FALSE,
			// the current mouse action will be canceled.
			//

			if (OnMButtonHitRowCol(m_nHitRow, m_nHitCol, m_nHitRow, m_nHitCol, point, flags,
				(WORD) (m_nHitState | GX_HITSTART | GX_HITINCELL)))
			{
				// The programmer can initialize autoscrolling or mouse capturing
				// in OnMButtonHitRowCol.
				//
				// If he did not, I will do it myself (this is the default),
				//

				if (GetCapture() != m_pGridWnd && m_nHitState)
				{
					// Catch all Mouse events
					SetCapture();
				}
			}
			else
				m_pGridWnd->SendMessage(WM_CANCELMODE, 0, 0);

			return TRUE;
		}

	}
	CATCH(CException, e)
	{
		m_pGridWnd->SendMessage(WM_CANCELMODE, 0, 0);
	}
	END_CATCH
		
	return FALSE;
}

BOOL CGXGridCore::DoMButtonUp(UINT flags, CPoint point)
{
	TRY
	{
		ROWCOL  nRow, nCol;
		int ht;
		WORD w;
		if (_doButtonUp(this, flags, point, ht, nRow, nCol, w))
		{
			// it is important to use min(GetRowCount(), nRow) because
			// HitTest returns nRow+1 if user released the mouse button
			// outside the grid.

			OnMButtonHitRowCol(m_nHitRow, m_nHitCol, min(GetRowCount(), nRow), min(GetColCount(), nCol), point, flags, w);

			// reset semaphor for "hit-mode"

			m_nHitState = 0;

			return TRUE;
		}

	}
	CATCH(CException, e)
	{
		m_pGridWnd->SendMessage(WM_CANCELMODE, 0, 0);
	}
	END_CATCH
		
	return FALSE;
}

BOOL CGXGridCore::DoRButtonDown(UINT flags, CPoint point)
{
	TRY
	{
		if (_doButtonDown(this, flags, point, MK_LBUTTON | MK_MBUTTON))
		{
			// OnRButtonHitRowCol will process application specific behaviour
			// for dragging cells. When OnRButtonHitRowCol returns FALSE,
			// the current mouse action will be canceled.
			//
			if (OnRButtonHitRowCol(m_nHitRow, m_nHitCol, m_nHitRow, m_nHitCol, point, flags,
				(WORD) (m_nHitState | GX_HITSTART | GX_HITINCELL)))
			{
				// The programmer can initialize autoscrolling or mouse capturing
				// in OnMButtonHitRowCol.
				//
				// If he did not, I will do it myself (this is the default),
				//

				if (GetCapture() != m_pGridWnd && m_nHitState)
				{
					// Catch all Mouse events
					SetCapture();
				}
			}
			else
				m_pGridWnd->SendMessage(WM_CANCELMODE, 0, 0);

			return TRUE;
		}

	}
	CATCH(CException, e)
	{
		m_pGridWnd->SendMessage(WM_CANCELMODE, 0, 0);
	}
	END_CATCH
		
	return FALSE;
}


BOOL CGXGridCore::DoRButtonUp(UINT flags, CPoint point)
{
	TRY
	{
		ROWCOL  nRow, nCol;
		int ht;
		WORD w;
		if (_doButtonUp(this, flags, point, ht, nRow, nCol, w))
		{
			// it is important to use min(GetRowCount(), nRow) because
			// HitTest returns nRow+1 if user released the mouse button
			// outside the grid.

			OnRButtonHitRowCol(m_nHitRow, m_nHitCol, min(GetRowCount(), nRow), min(GetColCount(), nCol), point, flags, w);

			// reset semaphor for "hit-mode"

			m_nHitState = 0;

			return TRUE;
		}
	}
	CATCH(CException, e)
	{
		m_pGridWnd->SendMessage(WM_CANCELMODE, 0, 0);
	}
	END_CATCH

	return FALSE;
}

BOOL CGXGridCore::DoLButtonDblClk(UINT flags, CPoint point)
{
	TRY
	{
		ROWCOL  nRow, nCol;
		int ht;
		if (!_doButtonDown1(this, flags, point, MK_MBUTTON | MK_RBUTTON, ht, nRow, nCol))
			return FALSE;

		// check, if user clicked on a cell in the grid
		//
		if (ht == GX_CELLHIT || ht == GX_HEADERHIT)
		{
			// Hit normal Cell
			CGXRange rgCovered;
			if (GetCoveredCellsRowCol(nRow, nCol, rgCovered))
			{
				nRow = rgCovered.top;
				nCol = rgCovered.left;
			}

			CGXControl* pControl = GetControl(nRow, nCol);

			if (!IsCurrentCell(nRow, nCol) || !pControl->IsInit())
				SetCurrentCell(nRow, nCol);

			BOOL b = FALSE;

			if (pControl->IsInit() && IsCurrentCell(nRow, nCol))
				b = pControl->LButtonDblClk(flags, point);
				// ... will call OnLButtonDblClkRowCol(nRow, nCol, flags, point);
			else
				b = OnLButtonDblClkRowCol(nRow, nCol, flags, point);

			// if double-click has not been processed handle it as a normal click
			if (!b)
				return DoLButtonDown(flags, point);

			return TRUE;
		}

		// check, if user clicked on a vertical line to resize or resore the column
		// ... (behaviour can be changed with GetParam()->EnableTrackColWidth(...)
		// and GetParam()->SetRowHeaderLines(...)

		else if (m_pUserResizeCellsImp
			&& m_pUserResizeCellsImp->OnLButtonDblClk(this, point, flags, ht, nRow, nCol))
			return TRUE;

	}
	CATCH(CException, e)
	{
		m_pGridWnd->SendMessage(WM_CANCELMODE, 0, 0);
	}
	END_CATCH

	return FALSE;
}

BOOL CGXGridCore::DoMButtonDblClk(UINT flags, CPoint point)
{
	TRY
	{
		ROWCOL  nRow, nCol;
		int ht;
		if (!_doButtonDown1(this, flags, point, MK_RBUTTON | MK_LBUTTON, ht, nRow, nCol))
			return FALSE;

		// check, if user clicked on a header or cell in the grid
		//

		if (ht == GX_CELLHIT || ht == GX_HEADERHIT)
		{
			CGXRange rgCovered;
			if (GetCoveredCellsRowCol(nRow, nCol, rgCovered))
			{
				nRow = rgCovered.top;
				nCol = rgCovered.left;
			}

			// Hit normal Cell
			SetCurrentCell(nRow, nCol);
			CGXControl* pControl = GetControl(nRow, nCol);

			BOOL b = FALSE;

			if (pControl->IsInit() && IsCurrentCell(nRow, nCol))
				b = pControl->MButtonDblClk(flags, point);
				// ... will call OnMButtonDblClkRowCol(nRow, nCol, flags, point);
			else
				b = OnMButtonDblClkRowCol(nRow, nCol, flags, point);

			// if double-click has not been processed handle it as a normal click
			if (!b)
				return DoMButtonDown(flags, point);

			return TRUE;
		}
	}
	CATCH(CException, e)
	{
		m_pGridWnd->SendMessage(WM_CANCELMODE, 0, 0);
	}
	END_CATCH

	return FALSE;
}

BOOL CGXGridCore::DoRButtonDblClk(UINT flags, CPoint point)
{
	TRY
	{
		ROWCOL  nRow, nCol;
		int ht;
		if (!_doButtonDown1(this, flags, point, MK_MBUTTON | MK_LBUTTON, ht, nRow, nCol))
			return FALSE;

		// check, if user clicked on a cell in the grid
		//

		if (ht == GX_CELLHIT || ht == GX_HEADERHIT)
		{
			// Hit normal Cell
			CGXRange rgCovered;
			if (GetCoveredCellsRowCol(nRow, nCol, rgCovered))
			{
				nRow = rgCovered.top;
				nCol = rgCovered.left;
			}

			SetCurrentCell(nRow, nCol);
			CGXControl* pControl = GetControl(nRow, nCol);

			BOOL b = FALSE;

			if (pControl->IsInit() && IsCurrentCell(nRow, nCol))
				b = pControl->RButtonDblClk(flags, point);
				// ... will call OnRButtonDblClkRowCol(nRow, nCol, flags, point);
			else
				b = OnRButtonDblClkRowCol(nRow, nCol, flags, point);

			// if double-click has not been processed handle it as a normal click
			if (!b)
				return DoRButtonDown(flags, point);

			return TRUE;
		}
	}
	CATCH(CException, e)
	{
		m_pGridWnd->SendMessage(WM_CANCELMODE, 0, 0);
	}
	END_CATCH

	return FALSE;
}


void CGXGridCore::LockResize(BOOL bState)
{
	// Lock or unlock redrawing the grid for the next OnSize messages
	m_bLockResize = bState;

	// If grid gets unlocked, force redrawing the grid
	if (!bState)
		Invalidate();
}

BOOL CGXGridCore::DoSize(UINT nType, int cx, int cy)
{
	// TRACE("CGXGridCore::DoSize\n");
	// While grid resize drawing is locked validate the grid so that
	// it does not get redrawn
	CWnd* pParent = m_pGridWnd->GetParentFrame();

	m_nLastVisibleCol =
	m_nLastVisibleRow = GX_INVALID;

	if (nType == SIZE_MAXIMIZED)
	{
		// CGXTabWnd sends a SIZE_MAXIMIZED to update the scrollbars
		// when you switched a view...
		UpdateScrollbars();
	}

	if (nType != SIZE_RESTORED)
	{
		m_bZoomed = pParent && pParent->IsZoomed();
		return FALSE;
	}

	BOOL bZoomed = pParent && pParent->IsZoomed();

	if (m_bLockResize)
	{
		// TRACE("m_bLockResize = TRUE; %d %d\n", m_bZoomed, bZoomed);
		if (m_bZoomed == bZoomed)
			m_pGridWnd->ValidateRect(NULL);
		return TRUE;
	}

	// TRACE("CGXGridCore: OnSize(%u, %d, %d) (%d, %d)\n", nType, cx, cy, m_cxOld, m_cyOld);

	// Update splitter pane information
	if (m_pSplitterWnd != NULL)
	{
		m_pSplitterWnd->IsChildPane(m_pGridWnd, m_nSplitRow, m_nSplitCol);

		// Display row headers only in upper pane
		m_bDisplayHeaderRow = m_nSplitRow == 0;

		// Display column headers only in left pane
		m_bDisplayHeaderCol = m_nSplitCol == 0;

		// IsRowHidden and IsColHidden will check the m_bDisplayHeader... state
		// to determine if they should hide the header
	}

	m_nLastVisibleCol =
	m_nLastVisibleRow = GX_INVALID;

	CRect rect;
	m_pGridWnd->GetWindowRect(&rect);

	CRect rectGrid = GetGridRect();

	// is smart resizing possible?
	if (GetParam()
		&& GetParam()->m_bSmartResize
		&& !m_bRectDisplay
		&& pParent != NULL
		&& m_cxOld > 0 && m_cyOld > 0
		&& rect.top == m_rectWnd.top
		&& rect.left == m_rectWnd.left
		&& !m_bUpdateScrollbar)
	{
		if (m_bZoomed != bZoomed)
		{
			// TRACE("m_bLockResize = FALSE; %d %d\n", m_bZoomed, bZoomed);
			// window was zoomed - redraw completely
			m_pGridWnd->Invalidate();
			m_pGridWnd->UpdateWindow();
		}
		else
		{
			m_pGridWnd->ValidateRect(NULL);

			int x = cx, y = cy;
			CPoint pt(m_cxOld, m_cyOld);

			ROWCOL nRow = CalcClientRowFromPt(pt, &y);
			if (nRow == GX_INVALID)
				y = m_cyOld;

			ROWCOL nCol = CalcClientColFromPt(pt, &x);
			if (nCol == GX_INVALID)
				x = m_cxOld;
	
			if (x < cx && cx != m_cxOld)
			{
				m_pGridWnd->InvalidateRect(CRect(x, 0, cx, min(m_cyOld, y)));
				m_pGridWnd->UpdateWindow();
			}

			if (y < cy && cy != m_cyOld)
			{
				m_pGridWnd->InvalidateRect(CRect(0, y, cx, cy));
				m_pGridWnd->UpdateWindow();
			}
		}
	}

#if _MFC_VER >= 0x0400
	if (m_bInitDone && m_bTtnIsEnabled && m_pTooltipProxy)
		m_pTooltipProxy->UpdateTipArea();
#endif

	if (m_bInitDone && !m_bUpdateScrollbar)
	{
		UpdateFontMetrics();    // Actualizes the internally stored font metrics.
		if (GetParam()->m_bSmartResize)
			UpdateWindow();
		UpdateScrollbars();     // Updates the scrollbars
	}

	if (m_pGridWnd->IsWindowVisible())
	{
		m_cxOld = cx-GetSystemMetrics(SM_CXVSCROLL);
		m_cyOld = cy-GetSystemMetrics(SM_CYHSCROLL);
		m_rectWnd = rect;
	}
	else
	{
		m_cxOld = m_cyOld = 0;
	}

	// Mark window as zoomed 
	m_bZoomed = pParent && pParent->IsZoomed();

	return TRUE;
}

BOOL CGXGridCore::DoShowWindow(BOOL bShow, UINT nStatus)
{
	if (!bShow)
	{
		// Force complete redrawing when DoSize is called next
		m_cxOld = m_cyOld = 0;
		m_nLastVisibleCol =
		m_nLastVisibleRow = GX_INVALID;
	}

	// unreferenced:
	nStatus;

	return TRUE;
}

HBRUSH CGXGridCore::DoCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	// TRACE0("CGXGridCore: OnCtlColor()\n");

	ROWCOL nRow, nCol;

	HBRUSH hbr = 0;

	if (GetCurrentCell(nRow, nCol))
	{
		const CGXStyle& styleEdit = LookupStyleRowCol(nRow, nCol);

		CGXControl* pControl = GetRegisteredControl(styleEdit.GetControl());

		if (pControl)
			hbr = pControl->OnGridCtlColor(pDC, pWnd, nCtlColor);

		if (hbr == 0)
		{
			COLORREF rgbText = styleEdit.GetTextColor(),
					 rgbCell = styleEdit.GetInteriorRef().GetBkColor();
			if (styleEdit.GetInteriorRef().GetStyle() == BS_SOLID)
				rgbCell = styleEdit.GetInteriorRef().GetColor();

			pDC->SetBkColor(rgbCell);
			pDC->SetTextColor(rgbText);

			return (HBRUSH) LookupOrCreateBrush(styleEdit.GetInteriorRef())->GetSafeHandle();
		}
	}

	return NULL;
}

BOOL CGXGridCore::DoCancelMode()
{
	if (!m_bInitDone)
		return TRUE;

	// Cancel current activities like tracking, dragging or selecting cells.
	if (m_bTracking)
	{
		OnCancelTracking();
	}
	else if (m_nHitState)
	{
		SetSelection(0);
		m_nHitState = 0;
	}
	else if (m_bSelDrag)
	{
		OnSelDragCancel();
	}
	
	if (GetParam() && GetParam()->m_bCut)
	{
		GetParam()->m_bCut = FALSE;
		Redraw(GX_INVALIDATE);
	}

	// Reset cursor
	if (m_idcCursor)
		SetGridCursor(0);

	if (GetCapture() == m_pGridWnd)
		ReleaseCapture();

	ROWCOL nRow, nCol;
	if (GetCurrentCell(nRow, nCol))
		GetControl(nRow, nCol)->OnGridCancelMode();

	return TRUE;
}

BOOL CGXGridCore::DoCommand(WPARAM wParam, LPARAM lParam)
{
	ROWCOL nRow, nCol;
	if (GetCurrentCell(nRow, nCol))
		return GetControl(nRow, nCol)->OnCommand(wParam, lParam);

	return FALSE;
}

void CGXGridCore::OnGridWinIniChange(LPCTSTR lpszSection)
{
	GXGetSysData()->UpdateSysSettings();

	// Notify all controls
	CGXControl* pControl = NULL;
	POSITION pos = m_ControlMap.GetStartPosition();
	UINT nID;
	while (pos)
	{
		m_ControlMap.GetNextAssoc(pos, (void*&) nID, (void*&) pControl);
		pControl->OnGridWinIniChange(lpszSection);
	}

	Invalidate();
}

void CGXGridCore::OnGridSysColorChange()
{
	GXGetSysData()->UpdateSysColors();

	// Notify all controls
	POSITION pos = m_ControlMap.GetStartPosition();

	while (pos)
	{
		void* p = NULL;
		void* pControl = NULL;
		m_ControlMap.GetNextAssoc(pos, p, pControl);
		ASSERT(AfxIsValidAddress(pControl, sizeof(CGXControl)));
		if (pControl)
			((CGXControl*) pControl)->OnGridSysColorChange();
	}
}

UINT CGXGridCore::DoGetDlgCode(UINT uiDefaultVal)
{
	return uiDefaultVal | DLGC_WANTTAB | DLGC_WANTARROWS | DLGC_WANTCHARS;
}

BOOL CGXGridCore::DoMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message, int& retval)
{
	// MS Office products (and Visual C++) ignore the first WM_LBUTTONDOWN
	// message when the user activates a new window. The if statement is
	// needed for this behaviour.
	// You can turn off this feature by calling GetParam()->SetEatFirstMouseClick(FALSE).

	// This feature only works for CView-derived grids
	// when clicked into the client area
	BOOL bEat = GetParam()->IsEatFirstMouseClick()
				&& m_pGridWnd->IsKindOf(RUNTIME_CLASS(CView))
				&& (nHitTest == HTCLIENT);

	if (bEat)
	{
		CWnd* pWnd = m_pGridWnd->GetFocus();

		if (m_pWndLastFocus == NULL
			|| m_pWndLastFocus == m_pGridWnd
			|| m_pGridWnd->IsChild(m_pWndLastFocus)
				&& pWnd && m_pGridWnd->GetParentFrame() == pWnd->GetParentFrame()
			)
			bEat = FALSE;

		if (bEat)
		{
			m_pGridWnd->SetFocus();
			m_pWndLastFocus = m_pGridWnd;

			// discard next WM_LBUTTONDOWN messages
			retval = MA_ACTIVATEANDEAT;
			return TRUE;
		}
	}

	ROWCOL nRow, nCol;
	if (GetCurrentCell(nRow, nCol))
		return GetControl(nRow, nCol)->MouseActivate(pDesktopWnd, nHitTest, message, retval);

	return FALSE;
}

void CGXGridCore::OnGridSetFocus(CWnd* pOldWnd)
{
	if (!m_bIgnoreFocus)
	{
		if (pOldWnd == NULL || !m_pGridWnd->IsChild(pOldWnd))
			OnActivateGrid(TRUE);

		m_pWndLastFocus = pOldWnd;

		// m_bRefreshControl is set when validating cells failed
		// and the focus shall be set to the control
		if (m_bRefreshControl)
		{
			ROWCOL nRow, nCol;
			if (GetCurrentCell(nRow, nCol))
			{
				CGXControl* pControl = GetControl(nRow, nCol);
				pControl->Refresh();
			}
		}

		m_bRefreshControl = FALSE;
	}
}

void CGXGridCore::OnGridKillFocus(CWnd* pNewWnd)
{
	m_bKillFocusCalled = TRUE;

	if (!m_bIgnoreFocus)
	{
		if (/*pNewWnd != m_pGridWnd->GetParent() && */
			pNewWnd != m_pGridWnd
			&& (pNewWnd == NULL || !m_pGridWnd->IsChild(pNewWnd)))
			OnActivateGrid(FALSE);
	}
}

void CGXGridCore::OnGridTimer(UINT nIDEvent)
{
	// Unused:
	nIDEvent;
}

BOOL CGXGridCore::OnGridPreTranslateMessage(MSG* pMsg)
{
	// Override this method if you want to hook into
	// keyboard messages before they are send to
	// the active window (e.g the current cell if it owns
	// the focus)
	//

	// do nothing if not yet initialized or if grid window is hidden
	if (!m_bInitDone || !m_pGridWnd->IsWindowVisible())
		return FALSE;

/*
	switch (pMsg->message)
	{
	// Keyboard messages
	case WM_KEYDOWN:
		TRACE("CGXGridCore::WM_KEYDOWN nChar = %d\n", nChar);
		break;

	case WM_KEYUP:
		TRACE("CGXGridCore::WM_KEYUP nChar = %d\n", nChar);
		break;

	case WM_CHAR:
		TRACE("CGXGridCore::WM_CHAR nChar = %d\n", nChar);
		break;

	case WM_SYSKEYDOWN:
		TRACE("CGXGridCore::WM_SYSKEYDOWN nChar = %d\n", nChar);
		break;

	case WM_SYSKEYUP:
		TRACE("CGXGridCore::WM_SYSKEYUP nChar = %d\n", nChar);
		break;

	case WM_SYSCHAR:
		TRACE("CGXGridCore::WM_SYSCHAR nChar = %d\n", nChar);
		break;
	}
*/

#if _MFC_VER >= 0x0400
	if (m_bTtnIsEnabled && m_pTooltipProxy)
		m_pTooltipProxy->RelayEvent(pMsg);
#endif

	CGXControl* pControl = GetCurrentCellControl();
	if (pControl)
		return pControl->OnGridPreTranslateMessage(pMsg);

	return FALSE;
}

BOOL CGXGridCore::OnGridWindowProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result) 
{
	if (m_bInitDone)
	{
		BOOL bSkip = FALSE;

		POSITION pos = m_pluginList.GetHeadPosition();
		while (pos && !bSkip)
		{
			CGXPluginComponent* pPlugin = 
				GX_STATIC_DOWNCAST(CGXPluginComponent, m_pluginList.GetNext(pos));

			if (pPlugin)
			{
				pPlugin->HandleMessage(message, wParam, lParam, &result);

   				if (pPlugin->m_bExitMesssage)
					return TRUE;

				bSkip |= pPlugin->m_bSkipOtherPlugins;
			}
		}
	}

	return FALSE;
}

void CGXGridCore::AddPlugin(CGXPluginComponent* pPlugin)
{
	GX_ASSERT_KINDOF(CGXPluginComponent, pPlugin);

	m_pluginList.AddTail(pPlugin);
	pPlugin->PlugIn(m_pGridWnd);
}

					  
