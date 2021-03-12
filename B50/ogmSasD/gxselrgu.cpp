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

// gxselgru.cpp : let user select cell ranges with mouse
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

class CGXGridUserSelectRangeImp: public CGXAbstractGridUserSelectRangeImp
{
public:
	virtual BOOL MoveCurrentCellEx(CGXGridCore* pGrid, int direction, UINT nCell);
	virtual void OnLButtonDown(CGXGridCore* pGrid, CPoint& point, UINT& flags, int& ht, ROWCOL& nRow, ROWCOL& nCol);
	virtual BOOL OnMouseMove(CGXGridCore* pGrid, CPoint& point, UINT& flags, ROWCOL& nRow, ROWCOL& nCol);
	virtual void OnLButtonUp(CGXGridCore* pGrid, CPoint& point, UINT& flags);
	virtual void SelectRange(CGXGridCore* pGrid, const CGXRange& range, BOOL bSelect /* = TRUE */, BOOL bUpdate );
	virtual BOOL StoreSelectRange(CGXGridCore* pGrid, const CGXRange& range, BOOL bSelect);
	virtual void UpdateSelectRange(CGXGridCore* pGrid, const CGXRange& range, const CGXRangeList* pOldRangeList, BOOL bCreateHint);
	virtual void SetSelection(CGXGridCore* pGrid, POSITION pos, ROWCOL nTop, ROWCOL nLeft, ROWCOL nBottom, ROWCOL nRight);
	virtual void PrepareChangeSelection(CGXGridCore* pGrid, POSITION oldPos, ROWCOL top, ROWCOL left, ROWCOL bottom, ROWCOL right, BOOL bCreateHint);
	virtual void PrepareClearSelection(CGXGridCore* pGrid, BOOL bCreateHint);
	virtual BOOL OnLButtonClickedRowCol(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, UINT nFlags, CPoint pt);
};

void CGXGridCore::ImplementUserSelectRange()
{
	if (m_pUserSelectRangeImp == NULL)
		AutoDeletePtr(m_pUserSelectRangeImp = new CGXGridUserSelectRangeImp);
}


BOOL CGXGridUserSelectRangeImp::MoveCurrentCellEx(CGXGridCore* pGrid, int direction, UINT nCell /* = 1 */)
{
	// Wrapper for MoveCurrentCell.
	// if SHIFT- or CONTROL-Key is pressed, select the cells
	ROWCOL nRow = 0, nCol = 0;

	BOOL bShift =
		GetKeyState(VK_SHIFT) &  0x8000
		&& pGrid->m_pParam->IsEnableSelection(GX_SELKEYBOARD);

	CGXRangeList* pSelList = pGrid->GetParam()->GetRangeList();

	if (pGrid->m_pParam->IsSpecialMode(GX_MODELISTBOX))
	{
		// Multiple Selection Listbox
		if (pGrid->m_pParam->IsSpecialMode(GX_MODELBOX_MS) && pGrid->GetCurrentCell(&nRow, &nCol))
		{
			// Delete previous selection if SHIFT-Key is not pressed
			if (!pGrid->m_SelRectId || !bShift)
			{
				if (pGrid->CanChangeSelection(0, TRUE, FALSE))
				{
					pGrid->SetSelection(0);
					pGrid->OnChangedSelection(0, TRUE, FALSE);
					pGrid->m_SelRectId = pSelList->AddTail(new CGXRange);

					pGrid->m_dwButtonDownRow = nRow;
					pGrid->m_dwButtonDownCol = nCol;
				}
				else
					return FALSE;
			}

			// allow programmer to change the range
			if (pGrid->CanChangeSelection(pSelList->GetAt(pGrid->m_SelRectId), FALSE, TRUE))
			{
				// Move Edit and set selection to new row
				pGrid->MoveCurrentCell(direction, nCell);
				pGrid->GetCurrentCell(&nRow, &nCol);
				if (pGrid->m_SelRectId == NULL)
					pGrid->m_SelRectId = pSelList->AddTail(new CGXRange);
				pGrid->SetSelection(pGrid->m_SelRectId, bShift?pGrid->m_dwButtonDownRow:nRow, 0, nRow, 0);
				pGrid->m_dwLastMoveRow = nRow;
				pGrid->m_dwLastMoveCol = nCol;

				// Notify derived classed
				pGrid->OnChangedSelection(pGrid->m_SelRectId?pSelList->GetAt(pGrid->m_SelRectId):NULL, FALSE, TRUE);

				if (!bShift)
				{
					pGrid->m_dwButtonDownRow = nRow;
					pGrid->m_dwButtonDownCol = nCol;
				}

				return TRUE;
			}
			else
				return FALSE;
		}
		// Single Selection Listbox
		else if (pGrid->m_pParam->IsSpecialMode(GX_MODELBOX_SS) && pGrid->GetCurrentCell(nRow, nCol))
		{
			// Remove previos selection
			if (!pGrid->m_SelRectId) // || !bShift)
			{
				if (pGrid->CanChangeSelection(0, TRUE, FALSE))
				{
					pGrid->SetSelection(0);
					pGrid->m_SelRectId = pSelList->AddTail(new CGXRange);
					pGrid->OnChangedSelection(0, TRUE, FALSE);
				}
				else
					return FALSE;
			}

			// allow programmer to change the range
			if (pGrid->CanChangeSelection(pSelList->GetAt(pGrid->m_SelRectId), FALSE, TRUE))
			{

				// Move Edit and set selection to new row
				pGrid->MoveCurrentCell(direction, nCell);
				pGrid->GetCurrentCell(nRow, nCol);
				if (pGrid->m_SelRectId == NULL)
					pGrid->m_SelRectId = pSelList->AddTail(new CGXRange);
				pGrid->SetSelection(pGrid->m_SelRectId, nRow, 0, nRow, 0);
				pGrid->m_dwLastMoveRow = nRow;
				pGrid->m_dwLastMoveCol = nCol;
				pGrid->m_dwButtonDownRow = nRow;
				pGrid->m_dwButtonDownCol = nCol;

				// Notify derived classed
				pGrid->OnChangedSelection(pGrid->m_SelRectId?pSelList->GetAt(pGrid->m_SelRectId):NULL, FALSE, TRUE);
				return TRUE;
			}
			else
				return FALSE;
		}
	}
	else if (bShift)
	{
		// Normal grid
		if (pGrid->GetCurrentCell(&nRow, &nCol))
		{
			// Delete previous selection
			if (!pGrid->m_SelRectId)
			{
				// trigger event for selectin cells
				// the programmer can override OnStartSelection and return FALSE
				// if he don't want to let the user start selecting cells at this cell
				if (!pGrid->OnStartSelection(nRow, nCol, 0, CPoint(0,0)))
					return FALSE;

				if (pGrid->CanChangeSelection(0, TRUE, FALSE))
				{
					pGrid->SetSelection(0);
					pGrid->OnChangedSelection(0, TRUE, FALSE);
					pGrid->m_SelRectId = pSelList->AddTail(new CGXRange);

					pGrid->m_dwButtonDownRow = nRow;
					pGrid->m_dwButtonDownCol = nCol;
				}
				else
					return FALSE;
			}

			// allow programmer to change the range
			if (pGrid->CanChangeSelection(pSelList->GetAt(pGrid->m_SelRectId), FALSE, TRUE)
				&& // Move Edit and expand selection to new cell
				pGrid->MoveCurrentCell(direction, nCell, FALSE))
			{
				pGrid->GetCurrentCell(&nRow, &nCol);
				if (pGrid->m_SelRectId == NULL)
					pGrid->m_SelRectId = pSelList->AddTail(new CGXRange);
				pGrid->SetSelection(pGrid->m_SelRectId, pGrid->m_dwButtonDownRow, pGrid->m_dwButtonDownCol, nRow, nCol);
				pGrid->m_dwLastMoveRow = nRow;
				pGrid->m_dwLastMoveCol = nCol;

				// Notify derived classed
				pGrid->OnChangedSelection(pGrid->m_SelRectId?pSelList->GetAt(pGrid->m_SelRectId):NULL, FALSE, TRUE);
				return TRUE;
			}
			else
				return FALSE;
		}
	}
	else // No SHIFT-Key pressed
	{
		BOOL bRet = pGrid->MoveCurrentCell(direction, nCell);
		pGrid->m_SelRectId = 0;
		return bRet;
	}

	return FALSE;
}

void CGXGridUserSelectRangeImp::OnLButtonDown(CGXGridCore* pGrid, CPoint& point, UINT& flags, int& ht, ROWCOL& nRow, ROWCOL& nCol)
{
	GX_UNUSED_ALWAYS(ht);

	CGXRangeList* pSelList = pGrid->GetParam()->GetRangeList();
	
	if (nRow == 0 && nCol == 0)
	{
		// selecting cells
		if (pGrid->m_pParam->IsEnableSelection(GX_SELTABLE) && pGrid->OnStartSelection(0, 0, flags, point))
		{
			// ... yes, select/deselect table

			pGrid->SelectRange(
				CGXRange().SetTable(),
				!pSelList->IsCellInList(0, 0));

			// ... SelectRange will trigger OnChangedSelection event
		}

		return;
	}

	ROWCOL nhRow = pGrid->GetHeaderRows();
	ROWCOL nhCol = pGrid->GetHeaderCols();

	BOOL bCol = (nRow <= pGrid->GetHeaderCols( ) && nCol > nhCol);
	BOOL bRow = (nCol <= pGrid->GetHeaderRows( ) && nRow > nhRow);
	// BOOL bCol = (nRow == 0 && nCol > nhCol);
	// BOOL bRow = (nCol == 0 && nRow > nhRow);
	BOOL bCell = (nRow > nhRow && nCol > nhCol);
	BOOL bCtl = flags & MK_CONTROL && pGrid->m_pParam->IsEnableSelection(GX_SELMULTIPLE);
	BOOL bShift = flags & MK_SHIFT && pGrid->m_pParam->IsEnableSelection(GX_SELSHIFT);

	if (pGrid->GetParam()->IsSpecialMode(GX_MODELBOX_MS))
	{
		bCtl = flags & MK_CONTROL;
		bShift = flags & MK_SHIFT;
	}
	else if (pGrid->GetParam()->IsSpecialMode(GX_MODELBOX_SS))
	{
		bCtl = bShift = FALSE;
	}

	if (pGrid->m_pParam->IsEnableSelection(GX_SELFULL) && pGrid->OnStartSelection(nRow, nCol, flags, point))
	{
		ROWCOL ncRow, ncCol;

		// First, remove all selections
		if (!bShift && !bCtl)
		{
			if (pGrid->CanChangeSelection(0, TRUE, FALSE))
			{
				pGrid->SetSelection(0);
				pGrid->OnChangedSelection(0, TRUE, FALSE);
			}
		}
		// If user pressed CTRL-Key while no range was selected
		// but the current cell was visible, select the current
		// cell.
		else if (bCtl && !bShift && pSelList->IsEmpty() && pGrid->GetCurrentCell(ncRow, ncCol)
			&& !pGrid->IsCurrentCell(nRow, nCol) && !pGrid->m_pParam->IsSpecialMode(GX_MODELISTBOX))
		{
			CGXRange range(ncRow, ncCol);
			if (pGrid->CanChangeSelection(&range, TRUE, FALSE))
			{
				pGrid->SelectRange(range, TRUE);
				pGrid->OnChangedSelection(&range, TRUE, FALSE);
			}
		}

		// Check, if selection is allowed
		if (bRow && pGrid->m_pParam->IsEnableSelection(GX_SELROW)
			|| bCol && pGrid->m_pParam->IsEnableSelection(GX_SELCOL)
			|| bCell && pGrid->m_pParam->IsEnableSelection(GX_SELCELL))
		{
			// start selection

			// if user pressed <SHIFT>, extend last used range

			if (bShift)
			{
				if (pSelList->IsEmpty())
				{
					// select cells starting from current cell
					pGrid->m_SelRectId = pSelList->AddTail(new CGXRange);
					pGrid->m_dwButtonDownRow = 1;
					pGrid->m_dwButtonDownCol = 1;
					pGrid->GetCurrentCell(pGrid->m_dwButtonDownRow, pGrid->m_dwButtonDownCol);
				}
				else
					// last used range
					pGrid->m_SelRectId = pSelList->GetTailPosition();
				
				CGXRange* pRange = pSelList->GetAt(pGrid->m_SelRectId);
				CGXRange range  =  *pRange;
				range.top = pGrid->m_dwButtonDownRow;
				range.left = pGrid->m_dwButtonDownCol;
				range.bottom = nRow;
				range.right = nCol;

				// allow programmer to change the range
				if (pGrid->CanChangeSelection(&range, TRUE, FALSE))
				//if (CanChangeSelection(pSelList->GetAt(m_SelRectId), TRUE, FALSE))
				{

					pGrid->SetSelection(pGrid->m_SelRectId,
						pGrid->m_dwButtonDownRow, pGrid->m_dwButtonDownCol,
						nRow, nCol);

					// trigger OnChangedSelection event
					pGrid->OnChangedSelection(pGrid->m_SelRectId?pSelList->GetAt(pGrid->m_SelRectId):NULL, TRUE, FALSE);
				}

				pGrid->m_nHitState |= GX_SELECT;
			}

			// Otherwise, add a new range
			else
			{
				pGrid->m_SelRectId = pSelList->AddTail(new CGXRange);

				// m_dwButtonDownRow and m_dwButtonDownCol
				// will be used for further selection,
				// (e.g. when shift key was pressed, or user
				// selects rows with arrow-keys.)

				pGrid->m_dwButtonDownRow = nRow;
				pGrid->m_dwButtonDownCol = nCol;

				pGrid->m_nHitState |= GX_SELECT;

				// If the user hit a row or column header, the whole
				// row/column will be selected immediately.

				// EXCELCURCELL
				if (/*m_bExcelLikeCurrentCell && bCtl
					||*/ bRow || bCol || pGrid->m_pParam->IsSpecialMode(GX_MODELISTBOX))
				{
					// allow programmer to change the range
					CGXRange range;

					// single selection listbox

					if (pGrid->m_pParam->IsSpecialMode(GX_MODELBOX_SS))
						range.SetRows(nRow);
					else if (pGrid->m_dwButtonDownRow <= nhRow)
						range.SetCols(min(pGrid->m_dwButtonDownCol, nCol), max(pGrid->m_dwButtonDownCol, nCol));
					else if (pGrid->m_dwButtonDownCol <= nhCol)
						range.SetRows(min(pGrid->m_dwButtonDownRow, nRow), max(pGrid->m_dwButtonDownRow, nRow));
					else
					{
						nRow = max(nRow, nhRow);
						nCol = max(nCol, nhCol);
						range.SetCells(min(pGrid->m_dwButtonDownRow, nRow),
							min(pGrid->m_dwButtonDownCol, nCol),
							max(pGrid->m_dwButtonDownRow, nRow),
							max(pGrid->m_dwButtonDownCol, nCol));
					}

					// allow programmer to change the range
					if (pGrid->CanChangeSelection(&range, TRUE, FALSE))
					{

						pGrid->SetSelection(pGrid->m_SelRectId, range.top, range.left, range.bottom, range.right);

						// trigger OnChangedSelection event
						pGrid->OnChangedSelection(pGrid->m_SelRectId?pSelList->GetAt(pGrid->m_SelRectId):NULL, TRUE, FALSE);
					}
				}
			}
		}
	}
}

BOOL CGXGridUserSelectRangeImp::OnMouseMove(CGXGridCore* pGrid, CPoint& point, UINT& flags, ROWCOL& nRow, ROWCOL& nCol)
{
	GX_UNUSED_ALWAYS(flags);
	GX_UNUSED_ALWAYS(point);

	if (pGrid->m_pParam->IsEnableSelection(GX_SELFULL) // selecting cells enabled?
		&& pGrid->m_nHitState & GX_SELECT)
	{
		CGXRangeList* pSelList = pGrid->GetParam()->GetRangeList();

		ROWCOL  nhRow = pGrid->GetHeaderRows(),
				nhCol = pGrid->GetHeaderCols();

		CGXRange range;

		// single selection listbox

		if (pGrid->m_pParam->IsSpecialMode(GX_MODELBOX_SS))
		{
			range.SetRows(nRow);
		}
		else if (pGrid->m_dwButtonDownRow <= nhRow)
			range.SetCols(min(pGrid->m_dwButtonDownCol, nCol), max(pGrid->m_dwButtonDownCol, nCol));
		else if (pGrid->m_dwButtonDownCol <= nhCol)
			range.SetRows(min(pGrid->m_dwButtonDownRow, nRow), max(pGrid->m_dwButtonDownRow, nRow));
		else
		{
			nRow = max(nRow, nhRow+1);
			nCol = max(nCol, nhCol+1);
			range.SetCells(min(pGrid->m_dwButtonDownRow, nRow),
				min(pGrid->m_dwButtonDownCol, nCol),
				max(pGrid->m_dwButtonDownRow, nRow),
				max(pGrid->m_dwButtonDownCol, nCol));
		}

		// allow programmer to change the range
		if (pGrid->m_SelRectId && pGrid->CanChangeSelection(&range, TRUE, FALSE))
		{
			pGrid->SetSelection(pGrid->m_SelRectId, range.top, range.left, range.bottom, range.right);

			// trigger OnChangedSelection event
			pGrid->OnChangedSelection(pGrid->m_SelRectId?pSelList->GetAt(pGrid->m_SelRectId):NULL, TRUE, FALSE);
		}

		return TRUE;
	}

	return FALSE;
}

void CGXGridUserSelectRangeImp::OnLButtonUp(CGXGridCore* pGrid, CPoint& point, UINT& flags)
{
	GX_UNUSED_ALWAYS(flags);
	GX_UNUSED_ALWAYS(point);

	CGXRangeList* pSelList = pGrid->GetParam()->GetRangeList();

	if (pGrid->m_pParam->IsEnableSelection(GX_SELFULL)
		&& pGrid->m_nHitState & GX_SELECT
		&& (pGrid->m_dwButtonDownCol != 0 || pGrid->m_dwButtonDownRow != 0))
	{
		if (!pGrid->m_pParam->IsSpecialMode(GX_MODELISTBOX)
			&& pGrid->m_dwButtonDownCol == pGrid->m_dwLastMoveCol
			&& pGrid->m_dwButtonDownRow == pGrid->m_dwLastMoveRow
			&& pGrid->m_dwButtonDownRow > 0 && pGrid->m_dwButtonDownCol > 0)
		{
			if (pGrid->m_nHitState & GX_CTRLPRESSED)
			{
				// allow programmer to change the range
				if (pGrid->m_SelRectId && pGrid->CanChangeSelection(pSelList->GetAt(pGrid->m_SelRectId), FALSE, FALSE))
				{
					// User pressed ctrl-key. I do select the current cell
					pGrid->SetSelection(pGrid->m_SelRectId,
						pGrid->m_dwButtonDownRow, pGrid->m_dwButtonDownCol,
						pGrid->m_dwButtonDownRow, pGrid->m_dwButtonDownCol);

					// trigger OnChangedSelection event
					pGrid->OnChangedSelection(pGrid->m_SelRectId?pSelList->GetAt(pGrid->m_SelRectId):NULL, FALSE, FALSE);
				}
			}
			else
			{
				// User did move the current cell but didn't select any cells.
				// EXCELCURCELL
				if (!pGrid->GetParam()->GetExcelLikeCurrentCell())
				{
					pGrid->SetSelection(NULL);
					/*
					PrepareClearSelection(TRUE);
					m_SelRectId = NULL;
					if (!pSelList->IsEmpty())
						pSelList->DeleteTail();
					*/
				}
				pGrid->m_nHitState &= (WORD) ~GX_SELECT;
			}
		}
		else
		{
			// trigger OnChangedSelection event to let user know that selection
			// ended.
			if (pGrid->m_SelRectId)
				pGrid->OnChangedSelection(pSelList->GetAt(pGrid->m_SelRectId), FALSE, FALSE);
			else
				pGrid->OnChangedSelection(NULL, FALSE, FALSE);

			pGrid->m_nHitState &= (WORD) ~GX_SELECT;
		}
	}
}


void CGXGridUserSelectRangeImp::SelectRange(CGXGridCore* pGrid, const CGXRange& range, BOOL bSelect /* = TRUE */, BOOL bUpdate )
{
	// Save old state
	CGXRangeList oldRangeList;
	pGrid->CopyRangeList(oldRangeList, FALSE);

	// Store new range
	if (pGrid->StoreSelectRange(range, bSelect))
	{
		pGrid->OnChangedSelection((CGXRange*) &range, FALSE, FALSE);
		if (bUpdate)
			pGrid->UpdateSelectRange(range, &oldRangeList, TRUE);
	}
}

BOOL CGXGridUserSelectRangeImp::StoreSelectRange(CGXGridCore* pGrid, const CGXRange& range, BOOL bSelect)
{
	// Check
	CGXRangeList* pSelList = pGrid->GetParam()->GetRangeList();

	if (bSelect)
	{
		// Simply add it to the list of selections
		pSelList->AddTail(new CGXRange(range));
	}
	else
	{
		// Remove it from the list of selections
		CGXRange* pRangeItem;
		CGXRange r;
		CGXRange rangeEx = range;
		rangeEx.ExpandRange(1, 1, pGrid->GetRowCount(), pGrid->GetColCount());
		POSITION pos = pSelList->GetHeadPosition();
		POSITION posOld;
		while (pos)
		{
			posOld = pos;
			pRangeItem = pSelList->GetNext(pos);
			CGXRange rangeItemEx = pRangeItem;
			rangeItemEx.ExpandRange(1, 1, pGrid->GetRowCount(), pGrid->GetColCount());
			if (r.IntersectRange(rangeItemEx, rangeEx))
			{
				// check if is a interior rectangle of a given range
				// if yes, exclude the area from the existing range and
				// break the existing range into max. 4 pieces

				if (r.top > rangeItemEx.top)
				{
					CGXRange* pNewRange = new CGXRange;

					if (range.IsRows() && (pRangeItem->IsRows() || pRangeItem->IsTable()))
						pNewRange->SetRows(rangeItemEx.top, r.top-1);
					else
						pNewRange->SetCells(rangeItemEx.top, rangeItemEx.left, r.top-1, rangeItemEx.right);

					pSelList->AddTail(pNewRange);
				}

				if (r.bottom < rangeItemEx.bottom)
				{
					CGXRange* pNewRange = new CGXRange;

					if (range.IsRows() && (pRangeItem->IsRows() || pRangeItem->IsTable()))
						pNewRange->SetRows(r.bottom+1, rangeItemEx.bottom);
					else
						pNewRange->SetCells(r.bottom+1, rangeItemEx.left, rangeItemEx.bottom, rangeItemEx.right);

					pSelList->AddTail(pNewRange);
				}

				if (r.left > rangeItemEx.left)
				{
					CGXRange* pNewRange = new CGXRange;

					if (range.IsCols() && (pRangeItem->IsCols() || pRangeItem->IsTable()))
						pNewRange->SetCols(rangeItemEx.left, r.left-1);
					else
						pNewRange->SetCells(rangeItemEx.top, rangeItemEx.left, rangeItemEx.bottom, r.left-1);

					pSelList->AddTail(pNewRange);
				}

				if (r.right < rangeItemEx.right)
				{
					CGXRange* pNewRange = new CGXRange;

					if (range.IsCols() && (pRangeItem->IsCols() || pRangeItem->IsTable()))
						pNewRange->SetCols(r.right+1, rangeItemEx.right);
					else
						pNewRange->SetCells(rangeItemEx.top, r.right+1, rangeItemEx.bottom, rangeItemEx.right);

					pSelList->AddTail(pNewRange);
				}

				// remove the original range
				delete pRangeItem;
				pSelList->RemoveAt(posOld);
			}
		}
	}

	return TRUE;
}

void CGXGridUserSelectRangeImp::UpdateSelectRange(CGXGridCore* pGrid, const CGXRange& range, const CGXRangeList* pOldRangeList, BOOL bCreateHint)
{
	CGXRangeList* pSelList = pGrid->GetParam()->GetRangeList();

	pGrid->m_bLockEvalMergeCells = TRUE;
	pGrid->m_bLockEvalFloatCells = TRUE;           // Don't evaluate float cell state

	if (pGrid->IsLockUpdate())
	{
		// Update excel selection frame only
		pGrid->m_rgLastSelectionFrame.m_bValid = FALSE;
		if (!pGrid->GetParam()->GetRangeList()->IsEmpty())
			pGrid->DrawSelectionFrame(NULL, FALSE, pGrid->GetParam()->GetRangeList()->GetTail());
	}
	else
	{

		// visible range of cells
		ROWCOL bottomRow = pGrid->GetLastVisibleRow();
		ROWCOL rightCol = pGrid->GetLastVisibleCol();

		// Expand fixed Cols and Rows (..Ex are absolute index in Grid)
		// and sort them (left < right, top < bottom)

		CGXRange rangeEx = range;
		rangeEx.ExpandRange(0, 0, pGrid->GetRowCount(), pGrid->GetColCount());

		// Intersect with displayable Rect  (absolute Index)
		ROWCOL nTop    = (rangeEx.top<=pGrid->GetFrozenRows()) ? rangeEx.top: max(rangeEx.top, pGrid->GetTopRow());
		ROWCOL nLeft   = (rangeEx.left<=pGrid->GetFrozenCols()) ? rangeEx.left : max(rangeEx.left, pGrid->GetLeftCol());
		ROWCOL nBottom = min(rangeEx.bottom, bottomRow);
		ROWCOL nRight  = min(rangeEx.right, rightCol);

		// Get Client Rows and Cols (relative Index to TopLeft)
		nTop    = pGrid->GetClientRow(nTop);
		nLeft   = pGrid->GetClientCol(nLeft);
		nBottom = pGrid->GetClientRow(nBottom);
		nRight  = pGrid->GetClientCol(nRight);

		if (nBottom == 0)
			nBottom = pGrid->GetFrozenRows();

		if (nRight == 0)
			nRight = pGrid->GetFrozenCols();

		if (pOldRangeList && pOldRangeList->GetCount() > 0 && pOldRangeList->GetHead()->IsTable())
			nTop = nLeft = 0;

		CClientDC dc(pGrid->m_pGridWnd);
		pGrid->OnGridPrepareDC(&dc);
		dc.IntersectClipRect(pGrid->GetGridRect());

		CRect rectItem = pGrid->CalcRectFromClientRowCol(0, 0);

		pGrid->DrawSelectionFrame(&dc, TRUE);

		pGrid->m_rgLastSelectionFrame.m_bValid = FALSE;
		if (!pGrid->GetParam()->GetRangeList()->IsEmpty())
			pGrid->DrawSelectionFrame(&dc, FALSE, pGrid->GetParam()->GetRangeList()->GetTail());

		// Check each cell if it should be inverted
		for (ROWCOL nClientRow = nTop; nClientRow <= nBottom; nClientRow++)
		{
			for (ROWCOL nClientCol = nLeft; nClientCol <= nRight; nClientCol++)
			{
				ROWCOL nCol = pGrid->GetCol(nClientCol), // absolute Index
					 nRow = pGrid->GetRow(nClientRow);

				BOOL bIsSel = pGrid->GetInvertStateRowCol(nRow, nCol, pOldRangeList);
				BOOL bNewSel =pGrid->GetInvertStateRowCol(nRow, nCol, pSelList);

				if (bNewSel != bIsSel)
				{
					rectItem = pGrid->CalcNextRectFromClientRowCol(nClientRow, nClientCol);
					pGrid->DrawInvertCell(&dc, nRow, nCol, rectItem);
				}
			}
		}
	}

	if (pSelList->IsEmpty())
	{
		pGrid->m_SelRectId = NULL;
	}
	else
	{
		// last used range
		pGrid->m_SelRectId = pSelList->GetTailPosition();
		CGXRange* pRange = pSelList->GetAt(pGrid->m_SelRectId);
		pGrid->m_dwButtonDownRow = pRange->top;
		pGrid->m_dwButtonDownCol = pRange->left;
		pGrid->m_dwLastMoveRow = pRange->top;
		pGrid->m_dwLastMoveCol = pRange->left;
	}

	// Create Hint
	if (pGrid->m_pUpdateHintImp && bCreateHint && pGrid->m_bHintsEnabled)
		pGrid->m_pUpdateHintImp->HintUpdateSelectRange(pGrid, range, pOldRangeList);

	pGrid->m_bLockEvalMergeCells = FALSE;
	pGrid->m_bLockEvalFloatCells = FALSE;
}

void CGXGridUserSelectRangeImp::SetSelection(CGXGridCore* pGrid, POSITION pos, ROWCOL nTop, ROWCOL nLeft, ROWCOL nBottom, ROWCOL nRight)
{
	// ASSERT(!m_bPrintInProgress);
	// TRACE(_T("CGXGridCore: BEGIN SetSelection(%u,%u,%u,%u)\n"),
	//  nTop, nLeft, nBottom, nRight);

	CGXRangeList* pSelList = pGrid->GetParam()->GetRangeList();

	if (pos == NULL)
	{
		// Clear all Selection
		if (!pSelList->IsEmpty())
			pGrid->PrepareClearSelection(TRUE);
		pSelList->DeleteAll();
		pGrid->m_SelRectId = NULL;
	}
	else
	{
		if (pGrid->m_pParam->IsSpecialMode(GX_MODELISTBOX))
			nLeft = nRight = 0;

		// Change the actual Selection
		pGrid->UpdateWindow();

		ROWCOL l;

		if (nTop > nBottom)
			l = nTop, nTop = nBottom, nBottom = l;

		if (nLeft > nRight)
			l = nLeft, nLeft = nRight, nRight = l;

		pGrid->PrepareChangeSelection(pos, nTop, nLeft, nBottom, nRight, TRUE);

		CGXRange* pRange = pSelList->GetAt(pos);

		if (nTop == 0 && nLeft == 0)
			pRange->SetTable();
		else if (nLeft == 0)
			pRange->SetRows(nTop, nBottom);
		else if (nTop == 0)
			pRange->SetCols(nLeft, nRight);
		else
			pRange->SetCells(nTop, nLeft, nBottom, nRight);
	}
	// TRACE0("CGXGridCore: END   SetSelection()\n");
}

void CGXGridUserSelectRangeImp::PrepareChangeSelection(CGXGridCore* pGrid, POSITION oldPos, ROWCOL top, ROWCOL left, ROWCOL bottom, ROWCOL right, BOOL bCreateHint)
{
	// ASSERT(!m_bPrintInProgress);
	ROWCOL bottomRow = pGrid->GetLastVisibleRow();
	ROWCOL rightCol = pGrid->GetLastVisibleCol();

	ROWCOL nTop = top, nLeft = left, nBottom = bottom, nRight = right;

	pGrid->m_bLockEvalMergeCells = TRUE;
	pGrid->m_bLockEvalFloatCells = TRUE;           // Don't evaluate float cell state

	CGXRangeList* pSelList = pGrid->GetParam()->GetRangeList();

	ROWCOL nRowCount = pGrid->GetRowCount();
	ROWCOL nColCount = pGrid->GetColCount();

	// Get current rectangle (to be changed)
	CGXRange oldRect;
	if (oldPos)
		oldRect = *(pSelList->GetAt(oldPos));
	if (oldRect.IsValid())
		oldRect.ExpandRange(0, 0, nRowCount, nColCount);
	else
		oldPos = NULL;

	ROWCOL nFrozenCols = pGrid->GetFrozenCols();
	ROWCOL nFrozenRows = pGrid->GetFrozenRows();

	// Expand fixed Cols and Rows (..Ex are absolute index in Grid)
	// and sort them (left < right, top < bottom)
	ROWCOL nTopEx    = min(nTop, nBottom);
	ROWCOL nLeftEx   = min(nLeft, nRight);
	ROWCOL nBottomEx = (nTopEx>0)?max(nTop, nBottom):nRowCount;
	ROWCOL nRightEx  = (nLeftEx>0)?max(nLeft, nRight):nColCount;

	// Union both Rectangles (absolute index)
	nTop    = oldPos?min(nTopEx,    oldRect.top)   :nTopEx;
	nLeft   = oldPos?min(nLeftEx,   oldRect.left)  :nLeftEx;
	nBottom = oldPos?max(nBottomEx, oldRect.bottom):nBottomEx;
	nRight  = oldPos?max(nRightEx,  oldRect.right) :nRightEx;

	// TRACE(_T("OldRect %d => %u,%u,%u,%u\n"), oldPos?1:0, oldRect);

	// Intersect with displayable Rect  (absolute Index)
	nTop    = (nTop<=nFrozenRows) ? nTop : max(nTop, pGrid->GetTopRow());
	nLeft   = (nLeft<=nFrozenCols) ? nLeft : max(nLeft, pGrid->GetLeftCol());
	nBottom = min(nBottom, bottomRow);
	nRight  = min(nRight, rightCol);

	// Get Client Rows and Cols (relative Index to TopLeft)
	nTop    = pGrid->GetClientRow(nTop);
	nLeft   = pGrid->GetClientCol(nLeft);
	nBottom = pGrid->GetClientRow(nBottom);
	nRight  = pGrid->GetClientCol(nRight);

	if (nBottom == 0)
		nBottom = pGrid->GetFrozenRows();

	if (nRight == 0)
		nRight = pGrid->GetFrozenCols();

	CClientDC dc(pGrid->m_pGridWnd);
	pGrid->OnGridPrepareDC(&dc);
	dc.IntersectClipRect(pGrid->GetGridRect());

	CRect rectItem = pGrid->CalcRectFromClientRowCol(0, 0);

	// Erase old and draw new rectangle
	CGXRange rgNew(top, left, bottom, right);

	pGrid->DrawSelectionFrame(&dc, TRUE, &rgNew);

	// Check each cell if it should be inverted
	for (ROWCOL nClientRow = nTop; nClientRow <= nBottom; nClientRow++)
	{
		for (ROWCOL nClientCol = nLeft; nClientCol <= nRight; nClientCol++)
		{
			ROWCOL nCol = pGrid->GetCol(nClientCol), // absolute Index
				 nRow = pGrid->GetRow(nClientRow);

			BOOL bIsSel = pGrid->GetInvertStateRowCol(nRow, nCol, pSelList);

			BOOL bNewSel =
				((nCol >= nLeftEx && nCol <= nRightEx
				&& nRow >= nTopEx && nRow <= nBottomEx) // True, if should be inverted
				|| pSelList->IsCellInList(nRow, nCol, oldPos))
				;
			//&& (!IsCurrentCell(nRow, nCol)
			//          || GetParam()->GetHideCurrentCell() == GX_HIDE_ALWAYS);

			// TRACE(_T("PrepareChangeSelection: %d/%d IsSel=%d, NewSel=%d\n"),
			//  nRow, nCol, bIsSel, bNewSel);
			if (bNewSel != bIsSel)
			{
				rectItem = pGrid->CalcNextRectFromClientRowCol(nClientRow, nClientCol);
				pGrid->DrawInvertCell(&dc, nRow, nCol, rectItem);
				// dc.InvertRect(rectItem);
			}
		}
	}

	// Create Hint
	if (pGrid->m_pUpdateHintImp && bCreateHint && pGrid->m_bHintsEnabled)
		pGrid->m_pUpdateHintImp->HintPrepareChangeSelection(pGrid, oldPos, top, left, bottom, right);

	pGrid->m_bLockEvalMergeCells = FALSE;
	pGrid->m_bLockEvalFloatCells = FALSE;
}

void CGXGridUserSelectRangeImp::PrepareClearSelection(CGXGridCore* pGrid, BOOL bCreateHint)
{
	// ASSERT(!m_bPrintInProgress);
	ROWCOL bottomRow = pGrid->GetLastVisibleRow();
	ROWCOL rightCol = pGrid->GetLastVisibleCol();

	pGrid->m_bLockEvalMergeCells = TRUE;
	pGrid->m_bLockEvalFloatCells = TRUE;           // Don't evaluate float cell state

	bottomRow = pGrid->GetClientRow(bottomRow);
	rightCol  = pGrid->GetClientCol(rightCol);

	CClientDC dc(pGrid->m_pGridWnd);
	pGrid->OnGridPrepareDC(&dc);
	dc.IntersectClipRect(pGrid->GetGridRect());

	// Erase old rectangle
	pGrid->DrawSelectionFrame(&dc, TRUE, NULL);
	pGrid->m_rgLastSelectionFrame.m_bValid = FALSE;

	CGXRangeList* pSelList = pGrid->GetParam()->GetRangeList();

	CRect rectItem = pGrid->CalcRectFromClientRowCol(0, 0);

	for (ROWCOL nClientRow = 0; nClientRow <= bottomRow; nClientRow++)
	{
		for (ROWCOL nClientCol = 0; nClientCol <= rightCol; nClientCol++)
		{
			ROWCOL nCol = pGrid->GetCol(nClientCol), // absolute Index
				 nRow = pGrid->GetRow(nClientRow);

			BOOL bIsSel = pGrid->GetInvertStateRowCol(nRow, nCol, pSelList);

			if (bIsSel)
			{
				rectItem = pGrid->CalcNextRectFromClientRowCol(nClientRow, nClientCol);
				// dc.InvertRect(rectItem);
				pGrid->DrawInvertCell(&dc, nRow, nCol, rectItem);
			}
		}
	}

	// Create Hint
	if (pGrid->m_pUpdateHintImp && bCreateHint && pGrid->m_bHintsEnabled)
		pGrid->m_pUpdateHintImp->HintPrepareClearSelection(pGrid);

	pGrid->m_bLockEvalMergeCells = FALSE;
	pGrid->m_bLockEvalFloatCells = FALSE;          // Don't evaluate float cell state
}

BOOL CGXGridUserSelectRangeImp::OnLButtonClickedRowCol(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, UINT nFlags, CPoint pt)
{
	nFlags, pt;

	// Let Ctrl-Click on a selected row deselect the row
	CGXRangeList* pSelList = pGrid->GetParam()->GetRangeList();
	BOOL bCtl = GetKeyState(VK_CONTROL) & 0x8000;

	if (bCtl && nCol <= pGrid->GetHeaderCols() && pSelList->IsCellInList(nRow, nCol, pGrid->m_SelRectId))
	{
		pGrid->m_SelRectId = NULL;
		pGrid->SelectRange(CGXRange().SetRows(nRow), FALSE);
		return TRUE;
	}
	else if (bCtl && nRow <= pGrid->GetHeaderRows() && pSelList->IsCellInList(nRow, nCol, pGrid->m_SelRectId))
	{
		pGrid->m_SelRectId = NULL;
		pGrid->SelectRange(CGXRange().SetCols(nCol), FALSE);
		return TRUE;
	}

	return FALSE;
}

