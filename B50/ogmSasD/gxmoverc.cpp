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

// gxmoverc.cpp : Implementation of MoveRows and MoveCols API
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

class CGXGridMoveRowsColsImp: public CGXAbstractGridMoveRowsColsImp
{
public:
	virtual BOOL StoreMoveRows(CGXGridCore* pGrid, ROWCOL nFromRow, ROWCOL nToRow, ROWCOL nDestRow, BOOL bProcessed);
	virtual void UpdateMoveRows(CGXGridCore* pGrid, ROWCOL nFromRow, ROWCOL nToRow, ROWCOL nDestRow, UINT flags, BOOL bCreateHint);
	virtual BOOL StoreMoveCols(CGXGridCore* pGrid, ROWCOL nFromCol, ROWCOL nToCol, ROWCOL nDestCol, BOOL bProcessed);
	virtual void UpdateMoveCols(CGXGridCore* pGrid, ROWCOL nFromCol, ROWCOL nToCol, ROWCOL nDestCol, UINT flags, BOOL bCreateHint);
	virtual BOOL MoveRows(CGXGridCore* pGrid, ROWCOL nFromRow, ROWCOL nToRow, ROWCOL nDestRow, UINT flags, GXCmdType ctCmd);
	virtual BOOL MoveCols(CGXGridCore* pGrid, ROWCOL nFromCol, ROWCOL nToCol, ROWCOL nDestCol, UINT flags, GXCmdType ctCmd);
};

void CGXGridCore::ImplementMoveRowsCols()
{
	if (m_pMoveRowsColsImp == NULL)
		AutoDeletePtr(m_pMoveRowsColsImp = new CGXGridMoveRowsColsImp);
}


BOOL CGXGridMoveRowsColsImp::StoreMoveRows(CGXGridCore* pGrid, ROWCOL nFromRow, ROWCOL nToRow, ROWCOL nDestRow, BOOL bProcessed)
{
	if (nFromRow == nDestRow)
		// nothing to do!
		return FALSE;

	// ... now, I can store the data
	CGXRangeList* pSelList = pGrid->GetParam()->GetRangeList();

	if (pGrid->m_pParam->m_pData)
		bProcessed |= pGrid->m_pParam->m_pData->StoreMoveRows(nFromRow, nToRow, nDestRow);

	if (bProcessed)
	{
		pGrid->SetModifiedFlag();

		// hidden state
		if (pGrid->m_pHideCellsImp)
			pGrid->m_pHideCellsImp->MoveRows(pGrid, nFromRow, nToRow, nDestRow);

		// size
		ROWCOL nRowCount = pGrid->GetRowCount();
		pGrid->m_pParam->m_RowHeights.Move(nFromRow, nToRow, nDestRow);

		// selection
		pSelList->MoveRows(nFromRow, nToRow, nDestRow, nRowCount);

		// Covered cells
		pGrid->m_pParam->GetCoveredCellsList()->MoveRows(nFromRow, nToRow, nDestRow, nRowCount);
		pGrid->m_pParam->UpdateCoveredCellsList();

		CGXAbstractSpanCellPoolImp* pSpanPool = (CGXAbstractSpanCellPoolImp*) pGrid->m_pAppData->m_pSpanCellPoolImp;

		// Floated cells
		if (pGrid->GetFloatCellsMode() && pSpanPool)
		{
			pSpanPool->MoveRows(pGrid->m_pParam->m_pDispFloatCells, nFromRow, nToRow, nDestRow);
			if (pGrid->m_pParam->m_pPrintFloatCells)
				pSpanPool->MoveRows(pGrid->m_pParam->m_pPrintFloatCells, nFromRow, nToRow, nDestRow);
		}

		// Merged cells
		if (pGrid->GetMergeCellsMode() && pSpanPool)
		{
			pSpanPool->RemoveRows(pGrid->m_pParam->m_pVertMergeCells, min(nFromRow, nDestRow), GX_MAXROWCOL);
			pSpanPool->MoveRows(pGrid->m_pParam->m_pHorzMergeCells, nFromRow, nToRow, nDestRow);

			pGrid->DelayMergeCells(CGXRange().SetRows(min(nFromRow, nDestRow), pGrid->GetRowCount()));
		}

		return TRUE;
	}

	TRACE2("CGXGridMoveRowsColsImp::StoreMoveRows(%ld, %ld", nFromRow, nToRow);
	TRACE1(", %ld) is not overriden!\n", nDestRow);

	return FALSE;
}

void CGXGridMoveRowsColsImp::UpdateMoveRows(CGXGridCore* pGrid, ROWCOL nFromRow, ROWCOL nToRow, ROWCOL nDestRow, UINT flags, BOOL bCreateHint)
{
	pGrid->m_nLastVisibleRow = GX_INVALID;

	// TRACE3("CGXGridCore: BEGIN UpdateMoveRows(%u, %u, %u)\n", nFromRow, nToRow, nDestRow);
	// ASSERT(!m_bPrintInProgress);
	ASSERT(nToRow >= nFromRow);

	ROWCOL nCount = nToRow-nFromRow+1;
	ROWCOL nDest  = nDestRow;
	if (nDest > nFromRow)
		nDest += nCount;

	if (pGrid->m_nEditRow >= nFromRow && pGrid->m_nEditRow <= nToRow && pGrid->m_nEditRow >= nDest)
		pGrid->m_nEditRow = nDest + (pGrid->m_nEditRow - nFromRow);
	else if (pGrid->m_nEditRow >= nFromRow && pGrid->m_nEditRow <= nToRow && pGrid->m_nEditRow < nDest)
		pGrid->m_nEditRow = nDest - nCount + (pGrid->m_nEditRow - nFromRow);
	else if (pGrid->m_nEditRow >= nFromRow && pGrid->m_nEditRow <= nDest)
		pGrid->m_nEditRow = pGrid->m_nEditRow - nCount;
	else if (pGrid->m_nEditRow <= nFromRow && pGrid->m_nEditRow >= nDest)
		pGrid->m_nEditRow = pGrid->m_nEditRow + nCount;

	if (pGrid->IsCurrentCell())
	{
		// Store in grid
		pGrid->StoreCurrentCell(TRUE, pGrid->m_nEditRow, pGrid->m_nEditCol);

		// Sync control
		CGXControl* pControl = pGrid->GetControl(pGrid->m_nEditRow, pGrid->m_nEditCol);
		if (pControl->IsInit())
			pControl->Init(pGrid->m_nEditRow, pGrid->m_nEditCol);
	}

	// Update excel selection frame only
	pGrid->m_rgLastSelectionFrame.m_bValid = FALSE;
	if (!pGrid->GetParam()->GetRangeList()->IsEmpty())
		pGrid->DrawSelectionFrame(NULL, FALSE, pGrid->GetParam()->GetRangeList()->GetTail());

	if (!pGrid->IsLockUpdate())
	{

		if ((flags & GX_INVALIDATE)
			|| pGrid->IsFrozenRow(nFromRow) || pGrid->IsFrozenRow(nToRow) || pGrid->IsFrozenRow(nDest)
			|| pGrid->GetFloatCellsMode()
			|| pGrid->GetMergeCellsMode())
		{
			pGrid->Redraw(flags, FALSE);
		}
		else
		{

			flags &= (GX_INVALIDATE | GX_SMART | GX_UPDATENOW);

			ROWCOL nRow     = min(nFromRow, nDest);
			CRect rectGrid = pGrid->GetGridRect();
			ROWCOL rightRow = pGrid->CalcBottomRowFromRect(rectGrid);
			ROWCOL nNewFrom = (nFromRow > nDest)?nDest:nDest-nCount;
			ROWCOL nNewTo   = nNewFrom+nCount-1;
			BOOL bBottom   = nDest > nFromRow;
			BOOL bTop    = nDest < nFromRow;

			// All rows were above top row
			if (nToRow < pGrid->m_nTopRow && nDest < pGrid->m_nTopRow)
			{
				; // nothing to do
			}
			// Rows have been moved from above the top row
			// to below the top row.
			else if (nToRow < pGrid->m_nTopRow && nDest >= pGrid->m_nTopRow)
			{
				pGrid->m_nTopRow -= nCount;
				ROWCOL rightRow = pGrid->CalcBottomRowFromRect(rectGrid);
				// Are new rows visible in currently displayed page?
				if (nNewFrom <= rightRow)
				{
					CRect rect = pGrid->CalcClientRectBottomFromRow(pGrid->GetRow(pGrid->GetClientRow(nNewFrom)-1));
					int nInsHeight = pGrid->CalcSumOfRowHeights(nNewFrom, nNewTo, rect.right-rect.left);
					if (flags & GX_INVALIDATE
						|| rect.left+nInsHeight > rect.right)
						pGrid->InvalidateRect(&rect);
					else if (flags)
					{
						pGrid->ScrollWindowEx(0, nInsHeight, &rect, &rectGrid,
							NULL, NULL, SW_INVALIDATE | SW_SCROLLCHILDREN);
						rect.right = rect.left+nInsHeight;
						pGrid->InvalidateRect(&rect);
					}
				}
			}
			// Rows which were previously visible in the current
			// page have been moved below the bottom row.
			else if (nFromRow < pGrid->m_nTopRow && nToRow >= pGrid->m_nTopRow && bBottom)
			{
				CRect rect = rectGrid;
				rect.top += pGrid->GetRowHeight(0);
				if (flags)
					pGrid->InvalidateRect(&rect);
			}
			else if (nFromRow < pGrid->m_nTopRow && nToRow >= pGrid->m_nTopRow && bTop)
			{
				CRect rect = rectGrid;
				rect.top += pGrid->GetRowHeight(0);

				if (flags)
					pGrid->InvalidateRect(&rect);
			}
			else if (flags && nRow <= rightRow)
			{
				if (nRow > pGrid->m_nTopRow)
					pGrid->InvalidateRect(pGrid->CalcClientRectBottomFromRow(pGrid->GetRow(pGrid->GetClientRow(nRow)-1)));
				else
					pGrid->InvalidateRect(pGrid->GetGridRect());
			}

			if (flags & GX_UPDATENOW)
				pGrid->UpdateWindow();

			if (flags)
				pGrid->UpdateScrollbars(!(flags & GX_UPDATENOW));

			if (pGrid->GetParam()->m_bNumberedRowHeaders)
				pGrid->RedrawRowCol(
					min(nFromRow, nDest), 0,
					pGrid->GetRowCount(), 0, flags, FALSE);
		}
	}

#if _MFC_VER >= 0x0400
	if (pGrid->m_pTooltipProxy)
		pGrid->m_pTooltipProxy->Reset();
#endif

	if (pGrid->m_pUpdateHintImp && bCreateHint && pGrid->m_bHintsEnabled)
		pGrid->m_pUpdateHintImp->HintUpdateMoveRows(pGrid, nFromRow, nToRow, nDestRow, flags);

	// TRACE3("CGXGridCore: END   UpdateMoveRows(%u, %u, %u)\n", nFromRow, nToRow, nDestRow);
}

BOOL CGXGridMoveRowsColsImp::StoreMoveCols(CGXGridCore* pGrid, ROWCOL nFromCol, ROWCOL nToCol, ROWCOL nDestCol, BOOL bProcessed)
{
	if (nFromCol == nDestCol)
		// nothing to do!
		return FALSE;

	// ... now, I can store the data
	CGXRangeList* pSelList = pGrid->GetParam()->GetRangeList();

	if (pGrid->m_pParam->m_pData)
		bProcessed |= pGrid->m_pParam->m_pData->StoreMoveCols(nFromCol, nToCol, nDestCol);

	if (bProcessed)
	{
		pGrid->SetModifiedFlag();

		// hidden state
		if (pGrid->m_pHideCellsImp)
			pGrid->m_pHideCellsImp->MoveCols(pGrid, nFromCol, nToCol, nDestCol);

		// size
		ROWCOL nColCount = pGrid->GetColCount();
		pGrid->m_pParam->m_ColWidths.Move(nFromCol, nToCol, nDestCol);

		// selection
		pSelList->MoveCols(nFromCol, nToCol, nDestCol, nColCount);

		// Covered cells
		pGrid->m_pParam->GetCoveredCellsList()->MoveCols(nFromCol, nToCol, nDestCol, nColCount);
		pGrid->m_pParam->UpdateCoveredCellsList();
		
		CGXAbstractSpanCellPoolImp* pSpanPool = (CGXAbstractSpanCellPoolImp*) pGrid->m_pAppData->m_pSpanCellPoolImp;

		// Floated cells
		if (pGrid->GetFloatCellsMode() && pSpanPool)
		{
			pSpanPool->RemoveCols(pGrid->m_pParam->m_pDispFloatCells, min(nFromCol, nDestCol), GX_MAXROWCOL);
			if (pGrid->m_pParam->m_pPrintFloatCells)
				pSpanPool->RemoveCols(pGrid->m_pParam->m_pPrintFloatCells, min(nFromCol, nDestCol), GX_MAXROWCOL);

			pGrid->DelayFloatCells(CGXRange().SetCols(min(nFromCol, nDestCol), pGrid->GetColCount()));
		}

		if (pGrid->GetMergeCellsMode() && pSpanPool)
		{
			// Merged cells
			pSpanPool->MoveCols(pGrid->m_pParam->m_pVertMergeCells, nFromCol, nToCol, nDestCol);
			pSpanPool->RemoveCols(pGrid->m_pParam->m_pHorzMergeCells, min(nFromCol, nDestCol), GX_MAXROWCOL);

			pGrid->DelayMergeCells(CGXRange().SetCols(min(nFromCol, nDestCol), pGrid->GetColCount()));
		}

		return TRUE;
	}

	TRACE2("CGXGridDoc::StoreMoveRows(%ld, %ld", nFromCol, nToCol);
	TRACE1(", %ld) is not overriden!\n", nDestCol);

	return FALSE;
}

void CGXGridMoveRowsColsImp::UpdateMoveCols(CGXGridCore* pGrid, ROWCOL nFromCol, ROWCOL nToCol, ROWCOL nDestCol, UINT flags, BOOL bCreateHint)
{
	pGrid->m_nLastVisibleCol = GX_INVALID;

	// TRACE3("CGXGridCore: BEGIN UpdateMoveCols(%u, %u, %u)\n", nFromCol, nToCol, nDestCol);
	// ASSERT(!m_bPrintInProgress);
	ASSERT(nToCol >= nFromCol);

	ROWCOL nCount = nToCol-nFromCol+1;
	ROWCOL nDest = nDestCol;
	if (nDest > nFromCol)
		nDest += nCount;

	if (pGrid->m_nEditCol >= nFromCol && pGrid->m_nEditCol <= nToCol && pGrid->m_nEditCol >= nDest)
		pGrid->m_nEditCol = nDest + (pGrid->m_nEditCol - nFromCol);
	else if (pGrid->m_nEditCol >= nFromCol && pGrid->m_nEditCol <= nToCol && pGrid->m_nEditCol < nDest)
		pGrid->m_nEditCol = nDest - (nToCol - nFromCol + 1) + (pGrid->m_nEditCol - nFromCol);
	else if (pGrid->m_nEditCol >= nFromCol && pGrid->m_nEditCol <= nDest)
		pGrid->m_nEditCol = pGrid->m_nEditCol - (nToCol - nFromCol + 1);
	else if (pGrid->m_nEditCol <= nFromCol && pGrid->m_nEditCol >= nDest)
		pGrid->m_nEditCol = pGrid->m_nEditCol + (nToCol - nFromCol + 1);

	if (pGrid->IsCurrentCell())
	{
		// Store in grid
		pGrid->StoreCurrentCell(TRUE, pGrid->m_nEditRow, pGrid->m_nEditCol);

		// Sync control
		CGXControl* pControl = pGrid->GetControl(pGrid->m_nEditRow, pGrid->m_nEditCol);
		if (pControl->IsInit())
			pControl->Init(pGrid->m_nEditRow, pGrid->m_nEditCol);
	}

	// Update excel selection frame only
	pGrid->m_rgLastSelectionFrame.m_bValid = FALSE;
	if (!pGrid->GetParam()->GetRangeList()->IsEmpty())
		pGrid->DrawSelectionFrame(NULL, FALSE, pGrid->GetParam()->GetRangeList()->GetTail());

	if (!pGrid->IsLockUpdate())
	{
		if ((flags & GX_INVALIDATE)
			|| pGrid->IsFrozenCol(nFromCol) || pGrid->IsFrozenCol(nToCol) || pGrid->IsFrozenCol(nDest)
			|| pGrid->GetFloatCellsMode()
			|| pGrid->GetMergeCellsMode())
		{
			pGrid->Redraw(flags, FALSE);
		}
		else
		{
			flags &= (GX_INVALIDATE | GX_SMART | GX_UPDATENOW);

			ROWCOL nCol     = min(nFromCol, nDest);
			CRect rectGrid = pGrid->GetGridRect();
			ROWCOL rightCol = pGrid->CalcRightColFromRect(rectGrid);
			ROWCOL nNewFrom = (nFromCol > nDest)?nDest:nDest-nCount;
			ROWCOL nNewTo   = nNewFrom+nCount-1;
			BOOL bRight   = nDest > nFromCol;
			BOOL bLeft    = nDest < nFromCol;

			// All columns were before pGrid->m_nLeftCol
			if (nToCol < pGrid->m_nLeftCol && nDest < pGrid->m_nLeftCol)
			{
				; // nothing to do
			}
			// Columns have been moved from left to pGrid->m_nLeftCol
			// to the right.
			else if (nToCol < pGrid->m_nLeftCol && nDest >= pGrid->m_nLeftCol)
			{
				pGrid->m_nLeftCol -= nCount;
				ROWCOL rightCol = pGrid->CalcRightColFromRect(rectGrid);

				// Are new columns visible in currently displayed page?
				if (nNewFrom <= rightCol)
				{
					CRect rect = pGrid->CalcClientRectRightFromCol(pGrid->GetCol(pGrid->GetClientCol(nNewFrom)-1));
					int nInsWidth = pGrid->CalcSumOfColWidths(nNewFrom, nNewTo, rect.right-rect.left);
					if (flags & GX_INVALIDATE
						|| rect.left+nInsWidth > rect.right)
					{
						pGrid->InvalidateRect(&rect);
					}
					else if (flags)
					{
						pGrid->ScrollWindowEx(nInsWidth, 0, &rect, &rectGrid,
							NULL, NULL, SW_INVALIDATE | SW_SCROLLCHILDREN);
						rect.right = rect.left+nInsWidth;
						pGrid->InvalidateRect(&rect);
					}
				}
			}
			// Columns which were previously visible in the current
			// page have been moved to the right of the last visible
			// column.
			else if (nFromCol < pGrid->m_nLeftCol && nToCol >= pGrid->m_nLeftCol && bRight)
			{
				CRect rect = rectGrid;
				rect.left += pGrid->GetColWidth(0);
				if (flags)
					pGrid->InvalidateRect(&rect);
			}
			else if (nFromCol < pGrid->m_nLeftCol && nToCol >= pGrid->m_nLeftCol && bLeft)
			{
				CRect rect = rectGrid;
				rect.left += pGrid->GetColWidth(0);

				if (flags)
					pGrid->InvalidateRect(&rect);
			}
			else if (flags && nCol <= rightCol)
			{
				if (nCol > pGrid->m_nLeftCol)
					pGrid->InvalidateRect(pGrid->CalcClientRectRightFromCol(pGrid->GetCol(pGrid->GetClientCol(nCol)-1)));
				else
					pGrid->InvalidateRect(pGrid->GetGridRect());
			}

			if (flags & GX_UPDATENOW)
				pGrid->UpdateWindow();

			if (flags)
				pGrid->UpdateScrollbars(!(flags & GX_UPDATENOW));

			if (pGrid->GetParam()->m_bNumberedColHeaders)
				pGrid->RedrawRowCol(
					0, min(nFromCol, nDest),
					0, pGrid->GetColCount(), flags, FALSE);
		}
	}

#if _MFC_VER >= 0x0400
	if (pGrid->m_pTooltipProxy)
		pGrid->m_pTooltipProxy->Reset();
#endif

	if (pGrid->m_pUpdateHintImp && bCreateHint && pGrid->m_bHintsEnabled)
		pGrid->m_pUpdateHintImp->HintUpdateMoveCols(pGrid, nFromCol, nToCol, nDestCol, flags);

	// TRACE3("CGXGridCore: END   UpdateMoveCols(%u, %u, %u)\n", nFromCol, nToCol, nDestCol);
}

BOOL CGXGridMoveRowsColsImp::MoveRows(CGXGridCore* pGrid, ROWCOL nFromRow, ROWCOL nToRow, ROWCOL nDestRow, UINT flags /* = GX_UPDATENOW */, GXCmdType ctCmd /*= gxDo*/)
{
	// TRACE3("CGXGridCore: BEGIN MoveRows(%u, %u, %u)\n", nFromRow, nToRow, nDestRow);

	// Transfer Current Cell's Data to grid
	if (!pGrid->TransferCurrentCell())
		return FALSE;

	CGXLockSelectionFrame tmpLockSelFrame(pGrid);

	// Undo information for Covered Cells
	if (ctCmd == gxDo)
	{
		pGrid->BeginTrans(pGrid->m_pAppData->strmMoveRows);

		POSITION pos = pGrid->GetParam()->GetCoveredCellsList()->GetHeadPosition();

		while (pos)
		{
			CGXRange r = pGrid->GetParam()->GetCoveredCellsList()->GetNext(pos);

			BOOL bChange = FALSE;
			if (r.top >= nFromRow && r.top <= nToRow && nToRow < nDestRow)
			{
				/* from top to dest */
				bChange = TRUE;
			}
			else if (r.top >= nFromRow && r.top <= nToRow && nDestRow < nFromRow)
			{
				// dest from top to
				bChange = TRUE;
			}
			else if (r.top > nToRow && r.top < nDestRow)
			{
				// from to top dest
				bChange = TRUE;
			}
			else if (r.top >= nDestRow && r.top < nFromRow)
			{
				// dest top from to
				bChange = TRUE;
			}

			if (bChange)
			{
				if (pGrid->m_pUndoImp && ctCmd != gxRollback && pGrid->m_pParam->m_bUndoEnabled)
				{
					pGrid->AddCommand(pGrid->m_pUndoImp->CreateSetCoveredCellsCmd(r.top, r.left, r.bottom, r.right), ctCmd);
					pGrid->AddCommand(pGrid->m_pUndoImp->CreateSetCoveredCellsCmd(r.top, r.left, r.top, r.left), ctCmd);
				}

				flags |= GX_INVALIDATE;
				flags &= ~(GX_UPDATENOW|GX_SMART);
			}
		}
	}

	// Allow engine to create undo info for matrix and tool worksheet functions
	// that overwrite other cells. AddCommand will check for this kind of undo info
	BOOL bAdvUndo = pGrid->SetAdvancedUndo(pGrid->m_pParam->m_bUndoEnabled && ctCmd != gxRollback);
	BOOL bDone = pGrid->StoreMoveRows(nFromRow, nToRow, nDestRow);
	pGrid->SetAdvancedUndo(bAdvUndo);

	if (bDone)
	{
		pGrid->UpdateMoveRows(nFromRow, nToRow, nDestRow, flags, TRUE);

		if (pGrid->m_pUndoImp && ctCmd != gxRollback && pGrid->m_pParam->m_bUndoEnabled)
		{
			ROWCOL  nCount = nToRow-nFromRow+1;

			pGrid->AddCommand(pGrid->m_pUndoImp->CreateMoveRowsCmd(nDestRow, nDestRow+nCount-1,
				nFromRow),
				ctCmd);

			if (ctCmd == gxRedo || ctCmd == gxUndo)
			{
				pGrid->OnCellUndone(
					nDestRow,
					pGrid->GetLeftCol());
			}
		}

		if (ctCmd == gxDo)
			pGrid->CommitTrans();

		// refresh formula cells 
		// pGrid->RefreshViews();

		return TRUE;
	}
	else
		pGrid->CleanAdvancedUndo();

	// TRACE3("CGXGridCore: END   MoveRows(%u, %u, %u)\n", nFromRow, nToRow, nDestRow);
	return FALSE;

}

BOOL CGXGridMoveRowsColsImp::MoveCols(CGXGridCore* pGrid, ROWCOL nFromCol, ROWCOL nToCol, ROWCOL nDestCol, UINT flags /* = GX_UPDATENOW */, GXCmdType ctCmd /*= gxDo*/)
{
	// TRACE3("CGXGridCore: BEGIN MoveCols(%u, %u, %u)\n", nFromCol, nToCol, nDestCol);

	// Transfer Current Cell's Data to grid
	if (!pGrid->TransferCurrentCell())
		return FALSE;

	CGXLockSelectionFrame tmpLockSelFrame(pGrid);

	// Undo information for Covered Cells
	if (ctCmd == gxDo)
	{
		pGrid->BeginTrans(pGrid->m_pAppData->strmMoveCols);

		POSITION pos = pGrid->GetParam()->GetCoveredCellsList()->GetHeadPosition();

		while (pos)
		{
			CGXRange r = pGrid->GetParam()->GetCoveredCellsList()->GetNext(pos);

			BOOL bChange = FALSE;
			if (r.left >= nFromCol && r.left <= nToCol && nToCol < nDestCol)
			{
				/* from left to dest */
				bChange = TRUE;
			}
			else if (r.left >= nFromCol && r.left <= nToCol && nDestCol < nFromCol)
			{
				// dest from left to
				bChange = TRUE;
			}
			else if (r.left > nToCol && r.left < nDestCol)
			{
				// from to left dest
				bChange = TRUE;
			}
			else if (r.left >= nDestCol && r.left < nFromCol)
			{
				// dest left from to
				bChange = TRUE;
			}

			if (bChange)
			{
				if (pGrid->m_pUndoImp && ctCmd != gxRollback && pGrid->m_pParam->m_bUndoEnabled)
				{
					pGrid->AddCommand(pGrid->m_pUndoImp->CreateSetCoveredCellsCmd(r.top, r.left, r.bottom, r.right), ctCmd);
					pGrid->AddCommand(pGrid->m_pUndoImp->CreateSetCoveredCellsCmd(r.top, r.left, r.top, r.left), ctCmd);
				}

				flags |= GX_INVALIDATE;
				flags &= ~(GX_UPDATENOW|GX_SMART);
			}
		}
	}

	// Allow engine to create undo info for matrix and tool worksheet functions
	// that overwrite other cells. AddCommand will check for this kind of undo info
	BOOL bAdvUndo = pGrid->SetAdvancedUndo(pGrid->m_pParam->m_bUndoEnabled && ctCmd != gxRollback);
	BOOL bDone = pGrid->StoreMoveCols(nFromCol, nToCol, nDestCol);
	pGrid->SetAdvancedUndo(bAdvUndo);

	if (bDone)
	{
		pGrid->UpdateMoveCols(nFromCol, nToCol, nDestCol, flags, TRUE);

		if (pGrid->m_pUndoImp && ctCmd != gxRollback && pGrid->m_pParam->m_bUndoEnabled)
		{
			ROWCOL  nCount = nToCol-nFromCol+1;

			pGrid->AddCommand(pGrid->m_pUndoImp->CreateMoveColsCmd(nDestCol, nDestCol+nCount-1,
				nFromCol),
				ctCmd);

			if (ctCmd == gxRedo || ctCmd == gxUndo)
			{
				pGrid->OnCellUndone(
					pGrid->GetTopRow(),
					nDestCol);
			}
		}

		if (ctCmd == gxDo)
			pGrid->CommitTrans();

		// refresh formula cells 
		// pGrid->RefreshViews();

		return TRUE;
	}
	else
		pGrid->CleanAdvancedUndo();

	// TRACE3("CGXGridCore: END   MoveCols(%u, %u, %u)\n", nFromCol, nToCol, nDestCol);
	return FALSE;
}
