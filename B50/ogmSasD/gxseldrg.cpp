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

// gxseldrg.cpp : drag selected rows and columns
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

#include "gxpriv.h" // DragLineWnd

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

class CGXGridUserDragSelectRangeImp: public CGXAbstractGridUserDragSelectRangeImp
{
public:
	virtual BOOL OnLButtonDown(CGXGridCore* pGrid, CPoint& point, UINT& flags, int& ht, ROWCOL& nRow, ROWCOL& nCol);
	virtual BOOL OnMouseMove(CGXGridCore* pGrid, CPoint& point, UINT& flags);
	virtual BOOL OnLButtonUp(CGXGridCore* pGrid, CPoint& point, UINT& flags);
	virtual BOOL OnSelDragStart(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol);
	virtual BOOL OnSelDragMove(CGXGridCore* pGrid, ROWCOL nRow1, ROWCOL nCol1, ROWCOL nRow, ROWCOL nCol);
	virtual BOOL OnSelDragDrop(CGXGridCore* pGrid, ROWCOL nRow1, ROWCOL nCol1, ROWCOL nRow, ROWCOL nCol);
	virtual void OnSelDragCancel(CGXGridCore* pGrid);
};

void CGXGridCore::ImplementUserDragSelectRange()
{
	if (m_pUserDragSelectRangeImp == NULL)
		AutoDeletePtr(m_pUserDragSelectRangeImp = new CGXGridUserDragSelectRangeImp);

	ImplementUserSelectRange();
	ImplementMoveRowsCols();
}


BOOL CGXGridUserDragSelectRangeImp::OnLButtonDown(CGXGridCore* pGrid, CPoint& point, UINT& flags, int& ht, ROWCOL& nRow, ROWCOL& nCol)
{
	GX_UNUSED_ALWAYS(ht);
	GX_UNUSED_ALWAYS(flags);
	GX_UNUSED_ALWAYS(point);

	// Some useful attributes
	CGXRangeList* pSelList = pGrid->GetParam()->GetRangeList();
	ROWCOL nhRow = pGrid->GetHeaderRows();
	ROWCOL nhCol = pGrid->GetHeaderCols();
	BOOL bSel = pSelList->IsCellInList(nRow, nCol);
	BOOL bTable = pSelList->IsCellInList(0, 0);
	BOOL bSelCol = nRow <= nhRow && nCol > nhCol && pSelList->IsCellInList(0, nCol);
	BOOL bSelRow = nCol <= nhCol && nRow > nhRow && pSelList->IsCellInList(nRow, 0);

	// If the user hit a selected row or column,
	// the grid is changed to dragging-mode.
	//
	// The programmer can inhibit this bahviour by specifying
	// GetParam()->EnableMoveCols(FALSE) and GetParam()->EnableMoveRows(FALSE)
	//
	// When dragging, there will be no calls to OnLButtonHitRowCol.
	//

	if (!bTable && bSel && bSelCol && pGrid->m_pParam->IsEnableMoveCols()
		|| !bTable && bSel && bSelRow && pGrid->m_pParam->IsEnableMoveRows())
	{
		// start dragging

		// OnSelDragStart can be overriden by the programmer
		// for changing the default behaviour
		//
		// If OnSelDragStart returns FALSE, dragging-mode is canceled
		//
		pGrid->m_bSelDrag = TRUE;

		if (pGrid->OnSelDragStart(bSelRow ? nRow : 0, bSelCol ? nCol : 0))
		{
			// Check if programmer has called GetCapture
			if (pGrid->GetCapture() != pGrid->m_pGridWnd && pGrid->m_bSelDrag)
			{
				// No, I do initialize it myself (default processing)
				if (pGrid->m_bAutoScroll)
					// I am using AutoScroll until WM_LBUTTONUP Message
					pGrid->AutoScroll(FALSE,
						bSelRow && !pGrid->IsFrozenRow(nRow),
						bSelCol && !pGrid->IsFrozenCol(nCol));

				pGrid->SetCapture();
			}

			return TRUE;
		}
		else
		{
			pGrid->m_nSelDragMode = 0;
			pGrid->m_pGridWnd->SendMessage(WM_CANCELMODE, 0, 0);
		}

		// Fall through !!!
	}

	return FALSE;
}

BOOL CGXGridUserDragSelectRangeImp::OnMouseMove(CGXGridCore* pGrid, CPoint& point, UINT& flags)
{
	GX_UNUSED_ALWAYS(flags);

	if (pGrid->m_bSelDrag)
	{
		// I am currently moving cells or columns
		ROWCOL nRow, nCol;

		if (pGrid->m_nSelDragMode == GX_SELDRAG_ROW)
		{
			nRow = pGrid->CalcClientRowFromPt(point);
			nRow = (nRow == GX_INVALID) ? pGrid->GetRowCount()+1 : pGrid->GetRow(max(nRow, 1));
			nCol = 0;
		}
		else
		{
			nCol = pGrid->CalcClientColFromPt(point);
			nCol = (nCol == GX_INVALID) ? pGrid->GetColCount()+1 : pGrid->GetCol(max(nCol, 1));
			nRow = 0;
		}

		// OnSelDragMove can be overriden by the programmer
		// for changing the default behaviour

		pGrid->OnSelDragMove(pGrid->m_nSelDragRow, pGrid->m_nSelDragCol, nRow, nCol);

		return TRUE;
	}

	return FALSE;
}

BOOL CGXGridUserDragSelectRangeImp::OnLButtonUp(CGXGridCore* pGrid, CPoint& point, UINT& flags)
{
	GX_UNUSED_ALWAYS(flags);

	if (pGrid->m_bSelDrag)
	{
		// Release Mouse Capture
		if (pGrid->m_pGridWnd->GetCapture() == pGrid->m_pGridWnd)
			ReleaseCapture();

		// User is dragging Rows or colums
		ROWCOL    nRow, nCol;

		if (pGrid->m_nSelDragMode == GX_SELDRAG_ROW)
		{
			nRow = pGrid->CalcClientRowFromPt(point);
			nRow = (nRow == GX_INVALID) ? pGrid->GetRowCount()+1 : pGrid->GetRow(max(nRow, 1));
			nCol = 0;
		}
		else
		{
			nCol = pGrid->CalcClientColFromPt(point);
			nCol = (nCol == GX_INVALID) ? pGrid->GetColCount()+1 : pGrid->GetCol(max(nCol, 1));
			nRow = 0;
		}

		// OnSelDragDrop can be overriden by the programmer
		// for changing the default behaviour.
		//
		// The base class version will call MoveRows or MoveCols.

		CGXRange* pRange = pGrid->GetParam()->GetRangeList()->GetAt(pGrid->m_SelRectId);

		pGrid->OnSelDragDrop(pRange->top, pRange->left, nRow, nCol);

		pGrid->m_bSelDrag = FALSE;

		return TRUE;
	}

	return FALSE;
}


BOOL CGXGridUserDragSelectRangeImp::OnSelDragStart(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol)
{
	pGrid->m_nSelDragRow = max(nRow, 1);
	pGrid->m_nSelDragCol = max(nCol, 1);

	CGXRangeList* pSelList = pGrid->GetParam()->GetRangeList();

	// find the Selection Rectangle
	POSITION posRect = pSelList->FindRange(nRow, nCol);
	ASSERT(posRect);

	CGXRange* pRange = pSelList->GetAt(posRect);

	// trigger selection dragging event
	// the programmer can override OnSelDragRowsStart / OnSelDragColsStart.
	// if the methods returns FALSE, dragging will not be processed

	if (nRow > 0)
	{
		pGrid->m_nSelDragMode = GX_SELDRAG_ROW;

		if (!pGrid->OnSelDragRowsStart(pRange->top, pRange->bottom))
			return FALSE;
	}
	else if (nCol > 0)
	{
		pGrid->m_nSelDragMode = GX_SELDRAG_COL;

		if (!pGrid->OnSelDragColsStart(pRange->left, pRange->right))
			return FALSE;
	}
	else
		return FALSE;

	pGrid->m_SelRectId = posRect;

	// Set Cursor
	pGrid->SetGridCursor(GX_IDC_SELDRAG);

	pGrid->m_bLockEvalFloatCells = TRUE;
	pGrid->m_bLockEvalMergeCells = TRUE;

	return TRUE;
}


BOOL CGXGridUserDragSelectRangeImp::OnSelDragMove(CGXGridCore* pGrid, ROWCOL nRow1, ROWCOL nCol1, ROWCOL nRow, ROWCOL nCol)
{
	GX_UNUSED_ALWAYS(nCol1);
	GX_UNUSED_ALWAYS(nRow1);

	CGXRange* pRange = pGrid->GetParam()->GetRangeList()->GetAt(pGrid->m_SelRectId);

	nCol = max(1, nCol);
	nRow = max(1, nRow);

	CRect rectGrid = pGrid->GetGridRect();

	// check, if nothing changed
	if (pGrid->m_nSelDragMode == GX_SELDRAG_ROW && nRow == pGrid->m_nSelDragRow
		|| pGrid->m_nSelDragMode == GX_SELDRAG_COL && nCol == pGrid->m_nSelDragCol)
		return TRUE;

	// hide dragging line
	if (pGrid->m_pWndDrag != NULL)
		pGrid->m_pWndDrag->ShowWindow(SW_HIDE);

	pGrid->m_nSelDragRow = nRow;
	pGrid->m_nSelDragCol = nCol;

	// trigger selection dragging event
	// the programmer can override OnSelDragRowsMove / OnSelDragColsMove.
	// if the methods returns FALSE, dragging will not be processed

	if (pGrid->m_nSelDragMode == GX_SELDRAG_ROW
			&& !pGrid->OnSelDragRowsMove(pRange->top, pRange->bottom, nRow)
		|| pGrid->m_nSelDragMode == GX_SELDRAG_COL
			&& !pGrid->OnSelDragColsMove(pRange->left, pRange->right, nCol))
	{
		pGrid->SetGridCursor(GX_IDC_NODROPCRSR);
		return TRUE;
	}

	// Set Cursor
	pGrid->SetGridCursor(GX_IDC_SELDRAG);

	ASSERT(pGrid->m_nSelDragRow > 0 && pGrid->m_nSelDragCol > 0);

	pGrid->UpdateWindow();

	{
		// Outline active GridLine
		if (pGrid->m_pWndDrag == NULL)
		{
			pGrid->m_pWndDrag = new CGXDragLineWnd(pGrid);
			pGrid->m_pWndDrag->CreateWnd(pGrid->m_pGridWnd);
			pGrid->m_pWndDrag->m_pen
				.SetStyle(PS_SOLID)
				.SetWidth(1)
				.SetColor(pGrid->GetParam()->GetProperties()->GetColor(gxnPropDraggingLines));
		}

		// Move the drag line
		CRect rect;
		if (pGrid->m_nSelDragMode == GX_SELDRAG_ROW)
			rect = pGrid->CalcGridLineRect(GX_HORZLINE, pGrid->GetClientRow(pGrid->m_nSelDragRow));
		else
			rect = pGrid->CalcGridLineRect(GX_VERTLINE, pGrid->GetClientCol(pGrid->m_nSelDragCol));

		pGrid->m_pWndDrag->MoveWindow(rect);
		pGrid->m_pWndDrag->ShowWindow(SW_SHOW);
		pGrid->m_pWndDrag->UpdateWindow();
	}

	return TRUE;
}

BOOL CGXGridUserDragSelectRangeImp::OnSelDragDrop(CGXGridCore* pGrid, ROWCOL nRow1, ROWCOL nCol1, ROWCOL nRow, ROWCOL nCol)
{
	GX_UNUSED_ALWAYS(nCol1);
	GX_UNUSED_ALWAYS(nRow1);

	pGrid->m_bLockEvalFloatCells = FALSE;
	pGrid->m_bLockEvalMergeCells = FALSE;

	pGrid->SetGridCursor(0);

	CRect rectGrid = pGrid->GetGridRect();

	if (pGrid->m_nSelDragMode == GX_SELDRAG_ROW && nRow == 0)
		return FALSE;

	if (pGrid->m_nSelDragMode == GX_SELDRAG_COL && nCol == 0)
		return FALSE;

	nCol = max(1, nCol);
	nRow = max(1, nRow);

	// Reset Drag Line
	if (pGrid->m_pWndDrag)
	{
		pGrid->m_pWndDrag->DestroyWindow();
		delete pGrid->m_pWndDrag;
		pGrid->m_pWndDrag = NULL;
	}

	// trigger selection dragging event
	// the programmer can override OnSelDragRowsMove / OnSelDragColsMove.
	// if the methods returns FALSE, dragging will not be processed

	CGXRange* pRange = pGrid->GetParam()->GetRangeList()->GetAt(pGrid->m_SelRectId);

	if (pGrid->m_nSelDragMode == GX_SELDRAG_ROW
			&& (!pGrid->OnSelDragRowsMove(pRange->top, pRange->bottom, nRow)
			|| !pGrid->OnSelDragRowsDrop(pRange->top, pRange->bottom, nRow))
		|| pGrid->m_nSelDragMode == GX_SELDRAG_COL
			&& (!pGrid->OnSelDragColsMove(pRange->left, pRange->right, nCol)
			|| !pGrid->OnSelDragColsDrop(pRange->left, pRange->right, nCol)))
	{
		return FALSE;
	}

	CGXRangeList* pSelList = pGrid->GetParam()->GetRangeList();

	pGrid->m_nSelDragRow = max(nRow, 1);
	pGrid->m_nSelDragCol = max(nCol, 1);
	CGXRange* r = pSelList->GetAt(pGrid->m_SelRectId);

	if (r->IsCellInRange(pGrid->m_nSelDragRow, pGrid->m_nSelDragCol)
		|| r->IsCellInRange(max(pGrid->m_nSelDragRow,1)-1, max(pGrid->m_nSelDragCol,1)-1))
		return FALSE;

	if (pGrid->m_nSelDragMode == GX_SELDRAG_ROW)
		pGrid->MoveRows(r->top, r->bottom, pGrid->m_nSelDragRow > r->top ? pGrid->m_nSelDragRow - r->GetHeight() : pGrid->m_nSelDragRow);
	else
		pGrid->MoveCols(r->left, r->right, pGrid->m_nSelDragCol > r->left ? pGrid->m_nSelDragCol - r->GetWidth() : pGrid->m_nSelDragCol);

	return TRUE;
}

void CGXGridUserDragSelectRangeImp::OnSelDragCancel(CGXGridCore* pGrid)
{
	pGrid->m_bLockEvalMergeCells = FALSE;
	pGrid->m_bLockEvalFloatCells = FALSE;

	// Reset Drag Line
	if (pGrid->m_pWndDrag)
	{
		pGrid->m_pWndDrag->DestroyWindow();
		delete pGrid->m_pWndDrag;
		pGrid->m_pWndDrag = NULL;
	}

	pGrid->SetGridCursor(0);

	pGrid->m_bSelDrag = FALSE;
}



