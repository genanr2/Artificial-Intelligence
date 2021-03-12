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

// gxcordim.cpp : row and column count (CGXGridCore)
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

#ifndef _GXDELAYR_H_
#include "gxdelayr.h"
#endif

#ifndef _GXPRIV_H_
#include "gxpriv.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCORDIM")
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
// Grid Dimensions

void CGXGridCore::OnTestGridDimension(ROWCOL nLastRow, ROWCOL nLastCol)
{
	// Unused
	nLastRow, nLastCol;
}

BOOL CGXGridCore::SetRowCount(ROWCOL nRows, UINT flags /* = GX_UPDATENOW*/)
{
	// TRACE1("CGXGridCore: BEGIN SetRowCount(%u)\n", nRows);
	ROWCOL nOldRows = GetRowCount();
	if (nRows < nOldRows)
		return RemoveRows(nRows+1, nOldRows, flags);
	else if (nRows > nOldRows)
		return InsertRows(nOldRows+1, nRows-nOldRows, NULL, NULL, NULL, 0, flags);
	// TRACE1("CGXGridCore: END   SetRowCount(%u)\n", nRows);
	return TRUE;
}

BOOL CGXGridCore::SetColCount(ROWCOL nCols, UINT flags /* = GX_UPDATENOW*/)
{
	// TRACE1("CGXGridCore: BEGIN SetColCount(%u)\n", nCols);
	ROWCOL nOldCols = GetColCount();
	if (nCols < nOldCols)
		return RemoveCols(nCols+1, nOldCols, flags);
	else if (nCols > nOldCols)
		return InsertCols(nOldCols+1, nCols-nOldCols, NULL, NULL, NULL, 0, flags);
	// TRACE1("CGXGridCore: END   SetColCount(%u)\n", nCols);
	return TRUE;
}

ROWCOL CGXGridCore::GetRowCount()
{
	if (m_pParam->m_pData)
		return m_pParam->m_pData->GetRowCount();

	TRACE0("Invalid call to CGXGridCore::GetRowCount()\n");
	return 0;
}

ROWCOL CGXGridCore::GetColCount()
{
	if (m_pParam->m_pData)
		return m_pParam->m_pData->GetColCount();

	TRACE0("Invalid call to CGXGridCore::GetColCount()\n");
	return 0;
}

BOOL CGXGridCore::StoreRowCount(ROWCOL nRows)
{
	// check if grid is read-only
	if (GetParam()->m_bLockReadOnly && IsReadOnly())
		return FALSE;

	// ... now, I can store the data
	if (m_pParam->m_pData)
	{
		if (!m_pParam->m_pData->StoreRowCount(nRows))
			return FALSE;
		SetModifiedFlag();
		return TRUE;
	}

	TRACE1("Invalid call to CGXGridCore::StoreRowCount(%ld)\n", nRows);
	ASSERT(0);

	return FALSE;
}

BOOL CGXGridCore::StoreColCount(ROWCOL nCols)
{
	// check if grid is read-only
	if (GetParam()->m_bLockReadOnly && IsReadOnly())
		return FALSE;

	// ... now, I can store the data
	if (m_pParam->m_pData)
	{
		if (!m_pParam->m_pData->StoreColCount(nCols))
			return FALSE;

		SetModifiedFlag();

		return TRUE;
	}

	TRACE1("Invalid call to CGXGridCore::StoreColCount(%ld)\n", nCols);
	ASSERT(0);

	return FALSE;
}

void CGXGridCore::UpdateChangedRowCount(ROWCOL nOldRows, UINT flags, BOOL bCreateHint)
{
	// TRACE2("CGXGridCore: BEGIN UpdateChangedRowCount(%u, %05x)\n", nOldRows, flags);

	ASSERT(nOldRows < GX_MAXROWCOL);

	flags &= (GX_INVALIDATE | GX_SMART | GX_UPDATENOW);

	ROWCOL nRowCount = GetRowCount();

	ROWCOL nEditRow, nEditCol;
	if (GetCurrentCell(nEditRow, nEditCol) && nEditRow > nRowCount)
		ResetCurrentCell(FALSE);

	if (!IsLockUpdate())
	{

		nOldRows = min(nRowCount, nOldRows);
		if (flags)
			UpdateScrollbars(FALSE);

		if (flags && m_bInitDone && nOldRows <= CalcBottomRowFromRect(GetGridRect()))
		{
			InvalidateRect(CalcClientRectBottomFromRow(nOldRows));
			if (flags & GX_UPDATENOW)
				UpdateWindow();
		}
	}

#if _MFC_VER >= 0x0400
	if (m_pTooltipProxy)
		m_pTooltipProxy->Reset();
#endif

	// Create Hint
	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintUpdateChangedRowCount(this, nOldRows, flags);

	// TRACE2("CGXGridCore: END   UpdateChangedRowCount(%u, %05x)\n", nOldRows, flags);
}

void CGXGridCore::UpdateChangedColCount(ROWCOL nOldCols, UINT flags, BOOL bCreateHint)
{
	// TRACE2("CGXGridCore: BEGIN UpdateChangedColCount(%u, %05x)\n", nOldCols, flags);

	ASSERT(nOldCols < GX_MAXROWCOL);

	flags &= (GX_INVALIDATE | GX_SMART | GX_UPDATENOW);

	ROWCOL nColCount = GetColCount();
	ROWCOL nEditRow, nEditCol;
	if (GetCurrentCell(nEditRow, nEditCol) && nEditRow > nColCount)
		ResetCurrentCell(FALSE);

	if (!IsLockUpdate())
	{

		nOldCols = min(nColCount, nOldCols);
		if (flags)
			UpdateScrollbars(FALSE);

		if (flags && m_bInitDone && nOldCols <= CalcRightColFromRect(GetGridRect()))
		{
			InvalidateRect(CalcClientRectRightFromCol(nOldCols));
			if (flags & GX_UPDATENOW)
				UpdateWindow();
		}
	}

#if _MFC_VER >= 0x0400
	if (m_pTooltipProxy)
		m_pTooltipProxy->Reset();
#endif

	// Create Hint
	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintUpdateChangedColCount(this, nOldCols, flags);

	// TRACE2("CGXGridCore: END   UpdateChangedColCount(%u, %05x)\n", nOldCols, flags);
}


/////////////////////////////////////////////////////////////////////////////
// Frozen Rows at the top of the Grid (before TopRow)
// or frozen Cols at the left of the Grid (before LeftCol)

BOOL CGXGridCore::SetFrozenRows(ROWCOL nFrozenRows, ROWCOL nHeaderRows, UINT flags, GXCmdType ctCmd)
{
	ASSERT(nHeaderRows <= nFrozenRows);
	// ASSERTION-> Rows with headers must be frozen ->END

	ROWCOL nOldFrozenRows = GetFrozenRows();
	ROWCOL nOldHeaderRows = GetHeaderRows();
	if (StoreFrozenRows(nFrozenRows, nHeaderRows))
	{
		UpdateFrozenRows(nOldFrozenRows, nOldHeaderRows, flags, TRUE);

		if (m_pUndoImp && ctCmd != gxRollback && m_pParam->m_bUndoEnabled)
			AddCommand(m_pUndoImp->CreateSetFrozenRowsCmd(nOldFrozenRows, nOldHeaderRows), ctCmd);

		return TRUE;
	}

	return FALSE;
}

BOOL CGXGridCore::SetFrozenCols(ROWCOL nFrozenCols, ROWCOL nHeaderCols, UINT flags, GXCmdType ctCmd)
{
	ASSERT(nHeaderCols <= nFrozenCols);
	// ASSERTION-> Rows with headers must be frozen ->END

	ROWCOL nOldFrozenCols = GetFrozenCols();
	ROWCOL nOldHeaderCols = GetHeaderCols();
	if (StoreFrozenCols(nFrozenCols, nHeaderCols))
	{
		UpdateFrozenCols(nOldFrozenCols, nOldHeaderCols, flags, TRUE);

		if (m_pUndoImp && ctCmd != gxRollback && m_pParam->m_bUndoEnabled)
			AddCommand(m_pUndoImp->CreateSetFrozenColsCmd(nOldFrozenCols, nOldHeaderCols), ctCmd);

		return TRUE;
	}

	return FALSE;
}

BOOL CGXGridCore::StoreFrozenRows(ROWCOL nFrozenRows, ROWCOL nHeaderRows)
{
	ASSERT(nHeaderRows <= nFrozenRows);
	// ASSERTION-> Rows with headers must be frozen ->END

	// check if grid is read-only
	if (GetParam()->m_bLockReadOnly && IsReadOnly())
		return FALSE;

	// ... now, I can store the data
	m_pParam->m_nFrozenRows = nFrozenRows;
	m_pParam->m_nHeaderRows = nHeaderRows;
	SetModifiedFlag();
	return TRUE;
}

BOOL CGXGridCore::StoreFrozenCols(ROWCOL nFrozenCols, ROWCOL nHeaderCols)
{
	ASSERT(nHeaderCols <= nFrozenCols);
	// ASSERTION-> Rows with headers must be frozen ->END

	// check if grid is read-only
	if (GetParam()->m_bLockReadOnly && IsReadOnly())
		return FALSE;

	// ... now, I can store the data
	m_pParam->m_nFrozenCols = nFrozenCols;
	m_pParam->m_nHeaderCols = nHeaderCols;
	SetModifiedFlag();
	return TRUE;
}

ROWCOL CGXGridCore::GetFrozenRows()
{
	return (IsPrinting() && !m_bPrintCurSelOnly || m_nSplitRow == 0) ? m_pParam->m_nFrozenRows : 0;
}

ROWCOL CGXGridCore::GetFrozenCols()
{
	return (IsPrinting() && !m_bPrintCurSelOnly || m_nSplitCol == 0) ? m_pParam->m_nFrozenCols : 0;
}

ROWCOL CGXGridCore::GetHeaderRows()
{
	return m_pParam->m_nHeaderRows;
}

ROWCOL CGXGridCore::GetHeaderCols()
{
	return m_pParam->m_nHeaderCols;
}

BOOL CGXGridCore::IsFrozenRow(ROWCOL nRow)
{
	return nRow > 0 && nRow <= GetFrozenRows();
}

BOOL CGXGridCore::IsFrozenCol(ROWCOL nCol)
{
	return nCol > 0 && nCol <= GetFrozenCols();
}

void CGXGridCore::UpdateFrozenCols(ROWCOL nOldFrozenCols, ROWCOL nOldHeaderCols, UINT flags, BOOL bCreateHint)
{
	// if (GetFrozenCols() > 0)
	ROWCOL nfc = GetFrozenCols();
	m_nLeftCol = max(nfc+1, m_nLeftCol);

	HideCurrentCell();

	DelayMergeCells(CGXRange().SetCols(min(nfc, nOldFrozenCols), max(nfc, nOldFrozenCols)+1));
	DelayFloatCells(CGXRange().SetCols(min(nfc, nOldFrozenCols), max(nfc, nOldFrozenCols)+1));

	if (!IsLockUpdate())
		Redraw(flags, FALSE);

	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintUpdateFrozenCols(this, nOldFrozenCols, nOldHeaderCols, flags);
}

void CGXGridCore::UpdateFrozenRows(ROWCOL nOldFrozenRows, ROWCOL nOldHeaderRows, UINT flags, BOOL bCreateHint)
{
	// if (GetFrozenRows() > 0)
	ROWCOL nfr = GetFrozenRows();
	m_nTopRow = max(nfr+1, m_nTopRow);

	HideCurrentCell();

	DelayMergeCells(CGXRange().SetRows(min(nfr, nOldFrozenRows), max(nfr, nOldFrozenRows)+1));

	if (!IsLockUpdate())
		Redraw(flags, FALSE);

	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintUpdateFrozenRows(this, nOldFrozenRows, nOldHeaderRows, flags);
}

/////////////////////////////////////////////////////////////////////////////
// Cell Coordinates

void CGXGridCore::SetTopRow(ROWCOL nRow, UINT flags /* = GX_UPDATENOW */, BOOL bCreateHint)
{
	// nRow must greater than 0 and not greater than GetRowCount
	nRow = max(1, min(nRow, GetRowCount()));

	m_nLastVisibleRow = GX_INVALID;
	m_nLastVisibleCol = GX_INVALID;

	if (m_bPrintInProgress)
	{
		m_nPrintTopRow = nRow;
	}
	else if (IsLockUpdate())
	{
		// give the programmer the possibility to inhibit scrolling
		if (!OnTopRowChange(nRow))
			return;

		m_nTopRow = nRow;

		OnTopRowChanged();
	}
	else
	{
		ROWCOL nfr = GetFrozenRows();
		nRow = max(nfr+1, nRow);

		// TRACE2("CGXGridCore: BEGIN SetTopRow(%u, %05x)\n", nRow, flags);
		flags &= (GX_INVALIDATE | GX_SMART | GX_UPDATENOW);
		if (flags & GX_SMART || flags & GX_UPDATENOW)
		{
			if (nRow < m_nTopRow)
				DoScroll(GX_UP, m_nTopRow-nRow);
			else if (nRow > m_nTopRow)
				DoScroll(GX_DOWN, nRow-m_nTopRow);
			if (flags & GX_UPDATENOW)
				UpdateWindow();
		}
		else
		{
			// give the programmer the possibility to inhibit scrolling
			if (!OnTopRowChange(nRow))
				return;

			m_nTopRow = nRow;
			if (flags & GX_INVALIDATE)
			{
				UpdateScrollbars(FALSE);
				InvalidateRect(GetGridRect());
			}

			OnTopRowChanged();
		}
	}

	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintSetTopRow(this, nRow, flags);
}

void CGXGridCore::SetLeftCol(ROWCOL nCol, UINT flags /* = GX_UPDATENOW */, BOOL bCreateHint)
{
	// nCol must greater than 0 and not greater than GetColCount
	nCol = max(1, min(nCol, GetColCount()));

	m_nLastVisibleRow = GX_INVALID;
	m_nLastVisibleCol = GX_INVALID;

	if (m_bPrintInProgress)
	{
		m_nPrintLeftCol = nCol;
	}
	else if (IsLockUpdate())
	{
		// give the programmer the possibility to inhibit scrolling
		if (!OnLeftColChange(nCol))
			return;

		m_nLeftCol = nCol;
		OnLeftColChanged();
	}
	else
	{
		ROWCOL nfr = GetFrozenCols();
		nCol = max(nCol, nfr+1);

		// TRACE2("CGXGridCore: BEGIN SetLeftCol(%u, %05x)\n", nCol, flags);

		flags &= (GX_INVALIDATE | GX_SMART | GX_UPDATENOW);
		if (flags & GX_SMART || flags & GX_UPDATENOW)
		{
			if (nCol < m_nLeftCol)
				DoScroll(GX_LEFT, m_nLeftCol-nCol);
			else if (nCol > m_nLeftCol)
				DoScroll(GX_RIGHT, nCol-m_nLeftCol);
			if (flags & GX_UPDATENOW)
				UpdateWindow();
		}
		else
		{
			// give the programmer the possibility to inhibit scrolling
			if (!OnLeftColChange(nCol))
				return;

			m_nLeftCol = nCol;
			if (flags & GX_INVALIDATE)
			{
				InvalidateRect(GetGridRect());
				UpdateScrollbars(FALSE);
			}

			OnLeftColChanged();
		}
	}

	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintSetLeftCol(this, nCol, flags);

	// TRACE2("CGXGridCore: END   SetLeftCol(%u, %05x)\n", nCol, flags);
}

void CGXGridCore::SetTopLeftRowCol(ROWCOL nRow, ROWCOL nCol, UINT flags /*  GX_UPDATENOW */)
{
	SetTopRow(nRow, flags);
	SetLeftCol(nCol, flags);
}


/////////////////////////////////////////////////////////////////////////////
// Convert between real and client coordinates

ROWCOL CGXGridCore::GetClientRow(ROWCOL nRow)
{
	ROWCOL nfr = GetFrozenRows();
	if (nRow > nfr && nRow < GetTopRow())
	{
		// TRACE0("CGXGridCore: Invalid use of GetClientRow\n");
		return 0;
	}
	return nRow != GX_INVALID && nRow > nfr
		? nRow - max(GetTopRow() - nfr, 1) + 1
		: nRow;
}

ROWCOL CGXGridCore::GetClientCol(ROWCOL nCol)
{
	ROWCOL nfr = GetFrozenCols();
	if (nCol > nfr && nCol < GetLeftCol())
	{
		// TRACE0("CGXGridCore: Invalid use of GetClientCol\n");
		return 0;
	}
	return nCol != GX_INVALID && nCol > nfr
		? nCol - max(GetLeftCol() - nfr, 1) + 1
		: nCol;
}

ROWCOL CGXGridCore::GetRow(ROWCOL nClientRow)
{
	ROWCOL nfr = GetFrozenRows();
	return nClientRow != GX_INVALID && nClientRow > nfr
		? nClientRow + max(GetTopRow() - nfr, 1) - 1
		: nClientRow;
}

ROWCOL CGXGridCore::GetCol(ROWCOL nClientCol)
{
	ROWCOL nfr = GetFrozenCols();
	return nClientCol != GX_INVALID && nClientCol > nfr
		? nClientCol + max(GetLeftCol() - nfr, 1) - 1
		: nClientCol;
}

/////////////////////////////////////////////////////////////////////////////
// Inserting Rows or Colums

BOOL CGXGridCore::StoreInsertRows(ROWCOL nRow, ROWCOL nCount, BOOL bProcessed)
{
	CGXRangeList* pSelList = GetParam()->GetRangeList();

	// check if grid is read-only
	if (GetParam()->m_bLockReadOnly && IsReadOnly())
		return FALSE;

	// ... now, I can store the data
	if (m_pParam->m_pData)
		bProcessed |= m_pParam->m_pData->StoreInsertRows(nRow, nCount);

	if (bProcessed)
	{
		SetModifiedFlag();

		// hidden state
		if (m_pHideCellsImp)
			m_pHideCellsImp->InsertRows(this, nRow, nCount);

		// size
		m_pParam->m_RowHeights.Insert(nRow, nCount);

		// selection
		pSelList->InsertRows(nRow, nCount);

		// Covered cells
		m_pParam->GetCoveredCellsList()->InsertRows(nRow, nCount);
		m_pParam->UpdateCoveredCellsList();

		// Floated cells
		if (GetFloatCellsMode())
		{
			m_pParam->m_pDispFloatCells->InsertRows(nRow, nCount);
			if (m_pParam->m_pPrintFloatCells)
				m_pParam->m_pPrintFloatCells->InsertRows(nRow, nCount);
		}

		// Merged cells
		if (GetMergeCellsMode())
		{
			m_pParam->m_pHorzMergeCells->InsertRows(nRow, nCount);
				// Vertical merged cells must be recalculated
			m_pParam->m_pVertMergeCells->RemoveRows(nRow, GX_MAXROWCOL);

			DelayMergeCells(CGXRange().SetRows(nRow, GetRowCount()));
		}

		return TRUE;
	}

	TRACE2("CGXGridCore::StoreInsertRows(%ld, %ld) is not overriden!\n", nRow, nCount);
	return FALSE;
}

void CGXGridCore::UpdateInsertRows(ROWCOL nRow, ROWCOL nCount, UINT flags, BOOL bCreateHint)
{
	m_nLastVisibleRow = GX_INVALID;

	// TRACE3("CGXGridCore: BEGIN UpdateInsertRows(%u, %u, %05x)\n", nRow, nCount, flags);
	ROWCOL nEditRow, nEditCol;
	if (GetCurrentCell(nEditRow, nEditCol) && nRow <= nEditRow)
	{
		// Store in grid
		StoreCurrentCell(TRUE, nEditRow + nCount, nEditCol);

		// Sync control
		GetCurrentCell(nEditRow, nEditCol);
		CGXControl* pControl = GetControl(nEditRow, nEditCol);
		if (pControl->IsInit())
			pControl->Init(nEditRow, nEditCol);
	}

	// Adjust m_nTopRow when all rows were frozen
	if (m_nTopRow == GetFrozenRows())
		m_nTopRow++;

	// Update excel selection frame only
	m_rgLastSelectionFrame.m_bValid = FALSE;
	if (!GetParam()->GetRangeList()->IsEmpty())
		DrawSelectionFrame(NULL, FALSE, GetParam()->GetRangeList()->GetTail());

	if (!IsLockUpdate())
	{
		BOOL bLock = LockScrollbars(TRUE);

		if (m_pOptimizedUpdateImp)
			m_pOptimizedUpdateImp->UpdateInsertRows(this, nRow, nCount, flags);
		else
			Redraw(flags, FALSE);

		LockScrollbars(bLock);

		if (flags & GX_UPDATENOW || flags & GX_SMART)
			UpdateScrollbars(FALSE);
	}

#if _MFC_VER >= 0x0400
	if (m_pTooltipProxy)
		m_pTooltipProxy->Reset();
#endif

	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintUpdateInsertRows(this, nRow, nCount, flags);

	// TRACE3("CGXGridCore: END   UpdateInsertRows(%u, %u, %05x)\n", nRow, nCount, flags);
}

BOOL CGXGridCore::StoreInsertCols(ROWCOL nCol, ROWCOL nCount, BOOL bProcessed)
{
	// check if grid is read-only
	if (GetParam()->m_bLockReadOnly && IsReadOnly())
		return FALSE;

	// ... now, I can store the data
	CGXRangeList* pSelList = GetParam()->GetRangeList();

	if (m_pParam->m_pData)
		bProcessed |= m_pParam->m_pData->StoreInsertCols(nCol, nCount);

	if (bProcessed)
	{
		// hidden state
		if (m_pHideCellsImp)
			m_pHideCellsImp->InsertCols(this, nCol, nCount);

		// size
		m_pParam->m_ColWidths.Insert(nCol, nCount);

		// selection
		pSelList->InsertCols(nCol, nCount);

		// Covered cells
		m_pParam->GetCoveredCellsList()->InsertCols(nCol, nCount);
		m_pParam->UpdateCoveredCellsList();

		// Floated cells
		if (GetFloatCellsMode())
		{
			m_pParam->m_pDispFloatCells->RemoveCols(nCol, GX_MAXROWCOL);
			if (m_pParam->m_pPrintFloatCells)
				m_pParam->m_pPrintFloatCells->InsertCols(nCol, GX_MAXROWCOL);

			DelayFloatCells(CGXRange().SetCols(nCol, GetColCount()));
		}

		// Merged cells
		if (GetMergeCellsMode())
		{
			m_pParam->m_pVertMergeCells->InsertCols(nCol, nCount);
			m_pParam->m_pHorzMergeCells->RemoveCols(nCol, GX_MAXROWCOL);

			DelayMergeCells(CGXRange().SetCols(nCol, GetColCount()));
		}

		return TRUE;
	}

	TRACE2("CGXGridCore::StoreInsertCols(%ld, %ld) is not overriden!\n", nCol, nCount);
	return FALSE;
}

void CGXGridCore::UpdateInsertCols(ROWCOL nCol, ROWCOL nCount, UINT flags, BOOL bCreateHint)
{
	m_nLastVisibleCol = GX_INVALID;

	// TRACE3("CGXGridCore: BEGIN UpdateInsertCols(%u, %u, %05x)\n", nCol, nCount, flags);
	// ASSERT(!m_bPrintInProgress);
	ROWCOL nEditRow, nEditCol;
	if (GetCurrentCell(nEditRow, nEditCol) && nCol <= nEditCol)
	{
		// Store in grid
		StoreCurrentCell(TRUE, nEditRow, nEditCol + nCount);

		// Sync control
		GetCurrentCell(nEditRow, nEditCol);
		CGXControl* pControl = GetControl(nEditRow, nEditCol);
		if (pControl->IsInit())
			pControl->Init(nEditRow, nEditCol);
	}

	// Adjust m_LeftCol when all columns were frozen
	if (m_nLeftCol == GetFrozenCols())
		m_nLeftCol++;

	// Update excel selection frame only
	m_rgLastSelectionFrame.m_bValid = FALSE;
	if (!GetParam()->GetRangeList()->IsEmpty())
		DrawSelectionFrame(NULL, FALSE, GetParam()->GetRangeList()->GetTail());

	if (!IsLockUpdate())
	{
		BOOL bLock = LockScrollbars(TRUE);

		if (m_pOptimizedUpdateImp)
			m_pOptimizedUpdateImp->UpdateInsertCols(this, nCol, nCount, flags);
		else
			Redraw(flags, FALSE);

		LockScrollbars(bLock);

		if (flags & GX_UPDATENOW || flags & GX_SMART)
			UpdateScrollbars(FALSE);
	}

#if _MFC_VER >= 0x0400
	if (m_pTooltipProxy)
		m_pTooltipProxy->Reset();
#endif


	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintUpdateInsertCols(this, nCol, nCount, flags);

	// TRACE3("CGXGridCore: END   UpdateInsertCols(%u, %u, %05x)\n", nCol, nCount, flags);
}

BOOL CGXGridCore::InsertRows(
	ROWCOL nRow,
	ROWCOL nCount,
	int* anHeightArray /*=NULL*/,
	CObArray* pCellsArray /*= NULL*/,
	CObArray* pRowsArray /*= NULL*/,
	ROWCOL nColCount /*= 0*/,
	UINT flags /*= GX_UPDATENOW*/,
	GXCmdType ctCmd /*= gxDo*/)
{
#ifdef _DEBUG
	if (anHeightArray)
		ASSERT(AfxIsValidAddress(anHeightArray, (UINT) (sizeof(int)*nCount)));
	// ASSERTION-> anWidthArray is not valid, perhaps too small? ->END
//	if (pCellsArray)
//		ASSERT((DWORD) pCellsArray->GetSize() == nCount*nColCount);
	// ASSERTION-> size of pCellsArray must be equal to nCount*nColCount ->END
#endif

	CGXLongOperation theOp;

	// Transfer Current Cell's Data to grid
	if (!TransferCurrentCell())
		return FALSE;

	CGXLockSelectionFrame tmpLockSelFrame(this);

	BOOL bRefresh = FALSE;

	DWORD dwSize = nCount*nColCount;
	if (pCellsArray && dwSize < (DWORD) pCellsArray->GetSize())
	{
		CObject* pAdvUndo = (CObList*) pCellsArray->GetAt((int) dwSize);

		// Let CGXData handle this advanced undo info and
		// set GX_UPDATE flags for those cells that need
		// repainting
		if (pAdvUndo)	
			bRefresh = UndoAdvanced(pAdvUndo, ctCmd != gxRollback, TRUE);
	}

	BOOL bAdvUndo = SetAdvancedUndo(m_pParam->m_bUndoEnabled && ctCmd != gxRollback);
	BOOL bDone = StoreInsertRows(nRow, nCount);
	SetAdvancedUndo(bAdvUndo);

	if (bDone)
	{
		if (pCellsArray && dwSize < (DWORD) pCellsArray->GetSize())
		{
			CObject* pAdvUndo = (CObList*) pCellsArray->GetAt((int) dwSize);

			// Let CGXData handle this advanced undo info and
			// set GX_UPDATE flags for those cells that need
			// repainting
			if (pAdvUndo)	
				bRefresh |= UndoAdvanced(pAdvUndo, ctCmd != gxRollback, FALSE);
		}

		if (anHeightArray)
		{
			for (ROWCOL n = 0; n < nCount; n++)
				StoreRowHeight(nRow+n, anHeightArray[n]);
		}

		if (pCellsArray)
		{
			int n = 0;
			for (ROWCOL r = 0; r < nCount; r++)
				for (ROWCOL c = 0; c < nColCount; c++)
				{
					CGXStyle* pStyle;
					pStyle = (CGXStyle*) pCellsArray->GetAt(n++);
					if (pStyle)
						StoreStyleRowCol(nRow+r, c, pStyle, gxCopy, m_nExpressionValueType);

					theOp.DoMessages();
				}
		}

		if (pRowsArray)
		{
			int n = 0;
			for (ROWCOL r = 0; r < nCount; r++)
			{
				CGXStyle* pStyle;
				pStyle = (CGXStyle*) pRowsArray->GetAt(n++);
				if (pStyle)
					StoreStyleRowCol(nRow+r, 0, pStyle, gxCopy, -1);

				theOp.DoMessages();
			}
		}

		UpdateInsertRows(nRow, nCount, flags, TRUE);

		if (m_pUndoImp && ctCmd != gxRollback && m_pParam->m_bUndoEnabled)
		{
			AddCommand(m_pUndoImp->CreateRemoveRowsCmd(nRow, nRow+nCount-1), ctCmd);

			if (ctCmd == gxRedo || ctCmd == gxUndo)
				OnCellUndone(nRow, GetLeftCol());
		}

		if (bRefresh)
			RefreshViews();

		return TRUE;
	}
	else
		CleanAdvancedUndo();

	return FALSE;
}

BOOL CGXGridCore::InsertCols(
	ROWCOL nCol,
	ROWCOL nCount,
	int* anWidthArray /*=NULL*/,
	CObArray* pCellsArray /*= NULL*/,
	CObArray* pColsArray /*= NULL*/,
	ROWCOL nRowCount /*= 0*/,
	UINT flags /*= GX_UPDATENOW*/,
	GXCmdType ctCmd /*= gxDo*/)
{
#ifdef _DEBUG
	if (anWidthArray)
		ASSERT(AfxIsValidAddress(anWidthArray, (UINT) (sizeof(int)*nCount)));
	// ASSERTION-> anWidthArray is not valid, perhaps too small? ->END
	//if (pCellsArray)
	//	ASSERT((DWORD) pCellsArray->GetSize() == nCount*nRowCount);
	// ASSERTION-> size of pCellsArray must be equal to nCount*nColCount ->END
#endif

	CGXLongOperation theOp;

	// Transfer Current Cell's Data to grid
	if (!TransferCurrentCell())
		return FALSE;

	CGXLockSelectionFrame tmpLockSelFrame(this);

	BOOL bRefresh = FALSE;

	DWORD dwSize = nCount*nRowCount;
	if (pCellsArray && dwSize < (DWORD) pCellsArray->GetSize())
	{
		CObject* pAdvUndo = (CObList*) pCellsArray->GetAt((int) dwSize);

		// Let CGXData handle this advanced undo info and
		// set GX_UPDATE flags for those cells that need
		// repainting
		if (pAdvUndo)	
			bRefresh = UndoAdvanced(pAdvUndo, ctCmd != gxRollback, TRUE);
	}

	BOOL bAdvUndo = SetAdvancedUndo(m_pParam->m_bUndoEnabled && ctCmd != gxRollback);
	BOOL bDone = StoreInsertCols(nCol, nCount);
	SetAdvancedUndo(bAdvUndo);

	if (bDone)
	{
		if (pCellsArray && dwSize < (DWORD) pCellsArray->GetSize())
		{
			CObject* pAdvUndo = (CObList*) pCellsArray->GetAt((int) dwSize);

			// Let CGXData handle this advanced undo info and
			// set GX_UPDATE flags for those cells that need
			// repainting
			if (pAdvUndo)	
				bRefresh |= UndoAdvanced(pAdvUndo, ctCmd != gxRollback, FALSE);
		}

		if (anWidthArray)
		{
			for (ROWCOL n = 0; n < nCount; n++)
					StoreColWidth(nCol+n, anWidthArray[n]);
		}

		if (pCellsArray)
		{
			int n = 0;
			for (ROWCOL r = 0; r < nRowCount; r++)
				for (ROWCOL c = 0; c < nCount; c++)
				{
					CGXStyle* pStyle;
					pStyle = (CGXStyle*) pCellsArray->GetAt(n++);
					if (pStyle)
						StoreStyleRowCol(r, nCol+c, pStyle, gxCopy, m_nExpressionValueType);

					theOp.DoMessages();
				}
		}

		if (pColsArray)
		{
			int n = 0;
			for (ROWCOL c = 0; c < nCount; c++)
			{
				CGXStyle* pStyle;
				pStyle = (CGXStyle*) pColsArray->GetAt(n++);
				if (pStyle)
					StoreStyleRowCol(0, nCol+c, pStyle, gxCopy, -1);

				theOp.DoMessages();
			}
		}

		UpdateInsertCols(nCol, nCount, flags, TRUE);

		if (m_pUndoImp && ctCmd != gxRollback && m_pParam->m_bUndoEnabled)
		{
			AddCommand(m_pUndoImp->CreateRemoveColsCmd(nCol, nCol+nCount-1), ctCmd);

			if (ctCmd == gxRedo || ctCmd == gxUndo)
				OnCellUndone(GetTopRow(), nCol);
		}

		if (bRefresh)
			RefreshViews();

		return TRUE;
	}
	else
		CleanAdvancedUndo();

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Removing Rows or Colums

BOOL CGXGridCore::StoreRemoveRows(ROWCOL nFromRow, ROWCOL nToRow, BOOL bProcessed)
{
	// check if grid is read-only
	if (GetParam()->m_bLockReadOnly && IsReadOnly())
		return FALSE;

	// ... now, I can store the data
	ASSERT(nFromRow > 0 && nToRow >= nFromRow);

	CGXRangeList* pSelList = GetParam()->GetRangeList();

	if (m_pParam->m_pData)
		bProcessed |= m_pParam->m_pData->StoreRemoveRows(nFromRow, nToRow);

	if (bProcessed)
	{
		SetModifiedFlag();

		if (m_pHideCellsImp)
			m_pHideCellsImp->RemoveRows(this, nFromRow, nToRow);

		// size
		m_pParam->m_RowHeights.Remove(nFromRow, nToRow);

		ROWCOL nCount = GetRowCount();

		// selection
		pSelList->RemoveRows(nFromRow, nToRow, nCount);

		// Covered cells
		m_pParam->GetCoveredCellsList()->RemoveRows(nFromRow, nToRow, nCount);
		m_pParam->UpdateCoveredCellsList();

		// Floated cells
		if (GetFloatCellsMode())
		{
			m_pParam->m_pDispFloatCells->RemoveRows(nFromRow, nToRow);
			if (m_pParam->m_pPrintFloatCells)
				m_pParam->m_pPrintFloatCells->RemoveRows(nFromRow, nToRow);
		}

		// Merged cells
		if (GetMergeCellsMode())
		{
			m_pParam->m_pVertMergeCells->RemoveRows(nFromRow, GX_MAXROWCOL);
			m_pParam->m_pHorzMergeCells->RemoveRows(nFromRow, nToRow);

			DelayMergeCells(CGXRange().SetRows(nFromRow, GetRowCount()), GetRowCount(), GetColCount());
		}

		return TRUE;
	}

	TRACE2("CGXGridCore::StoreRemoveRows(%ld, %ld) is not overriden!\n", nFromRow, nToRow);
	return FALSE;
}

void CGXGridCore::UpdateRemoveRows(ROWCOL nFromRow, ROWCOL nToRow, int* anOldHeights, UINT flags, BOOL bCreateHint)
{
	m_nLastVisibleRow = GX_INVALID;

	// TRACE3("CGXGridCore: BEGIN UpdateRemoveRows(%u, %u, %05x)\n", nFromRow, nToRow, flags);
	// ASSERT(!m_bPrintInProgress);
	ASSERT(nFromRow > 0 && nToRow >= nFromRow);
	ROWCOL nCount = nToRow-nFromRow+1;

	// ASSERT(!IsActiveCurrentCell());
	// ASSERTION-> Did you hide the control? ->END

	ROWCOL nEditRow = 0, nEditCol = 0;
	BOOL bEdit;
	if ((bEdit = GetCurrentCell(nEditRow, nEditCol)) != 0)
	{
		if (nEditRow >= nFromRow && nEditRow <= nToRow)
		{
			StoreCurrentCell(FALSE);
			// MarkEdit(nEditRow, nEditCol);
		}
		else if (nEditRow > nToRow)
		{
			// Store in grid
			StoreCurrentCell(TRUE, nEditRow-nCount, nEditCol);

			// Sync control
			GetCurrentCell(nEditRow, nEditCol);
			CGXControl* pControl = GetControl(nEditRow, nEditCol);
			if (pControl->IsInit())
				pControl->Init(nEditRow, nEditCol);
		}
	}

	// Update excel selection frame only
	m_rgLastSelectionFrame.m_bValid = FALSE;
	if (!GetParam()->GetRangeList()->IsEmpty())
		DrawSelectionFrame(NULL, FALSE, GetParam()->GetRangeList()->GetTail());

	if (!IsLockUpdate())
	{
		BOOL bLock = LockScrollbars(TRUE);

		if (m_pOptimizedUpdateImp)
			m_pOptimizedUpdateImp->UpdateRemoveRows(this, nFromRow, nToRow, anOldHeights, flags);
		else
			Redraw(flags, FALSE);

		LockScrollbars(bLock);

		if (flags)
			UpdateScrollbars(!(flags & GX_UPDATENOW));

	}

#if _MFC_VER >= 0x0400
	if (m_pTooltipProxy)
		m_pTooltipProxy->Reset();
#endif

	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintUpdateRemoveRows(this, nFromRow, nToRow, anOldHeights, flags);

	// TRACE3("CGXGridCore: END   UpdateRemoveRows(%u, %u, %05x)\n", nFromRow, nToRow, flags);
}

BOOL CGXGridCore::StoreRemoveCols(ROWCOL nFromCol, ROWCOL nToCol, BOOL bProcessed)
{
	// check if grid is read-only
	if (GetParam()->m_bLockReadOnly && IsReadOnly())
		return FALSE;

	// ... now, I can store the data
	ASSERT(nToCol >= nFromCol && nFromCol > 0);

	CGXRangeList* pSelList = GetParam()->GetRangeList();

	if (m_pParam->m_pData)
		bProcessed |= m_pParam->m_pData->StoreRemoveCols(nFromCol, nToCol);

	if (bProcessed)
	{
		SetModifiedFlag();

		// hidden state
		if (m_pHideCellsImp)
			m_pHideCellsImp->RemoveCols(this, nFromCol, nToCol);

		// size
		ROWCOL nColCount = GetColCount();
		m_pParam->m_ColWidths.Remove(nFromCol, nToCol);

		// selection
		pSelList->RemoveCols(nFromCol, nToCol, nColCount);

		// Covered cells
		m_pParam->GetCoveredCellsList()->RemoveCols(nFromCol, nToCol, nColCount);
		m_pParam->UpdateCoveredCellsList();

		// Floated cells
		if (GetFloatCellsMode())
		{
			m_pParam->m_pDispFloatCells->RemoveCols(nFromCol, GX_MAXROWCOL);
			if (m_pParam->m_pPrintFloatCells)
				m_pParam->m_pPrintFloatCells->RemoveCols(nFromCol, GX_MAXROWCOL);

			DelayFloatCells(CGXRange().SetCols(nFromCol, GetColCount()), GetColCount());
		}

		// Merged cells
		if (GetMergeCellsMode())
		{
			m_pParam->m_pVertMergeCells->RemoveCols(nFromCol, nToCol);
			m_pParam->m_pHorzMergeCells->RemoveCols(nFromCol, GX_MAXROWCOL);

			DelayMergeCells(CGXRange().SetCols(nFromCol, GetColCount()), GetRowCount(), GetColCount());
		}

		return TRUE;
	}

	TRACE2("CGXGridDoc::StoreRemoveCols(%ld, %ld) is not overriden!\n", nFromCol, nToCol);
	return FALSE;
}

void CGXGridCore::UpdateRemoveCols(ROWCOL nFromCol, ROWCOL nToCol, int* anOldWidths, UINT flags, BOOL bCreateHint)
{
	m_nLastVisibleCol = GX_INVALID;

	// TRACE3("CGXGridCore: BEGIN UpdateRemoveCols(%u, %u, %05x)\n", nFromCol, nToCol, flags);
	// ASSERT(!m_bPrintInProgress);
	ASSERT(nToCol >= nFromCol);
	ROWCOL nCount = nToCol-nFromCol+1;

	// ASSERT(!IsActiveCurrentCell());
	// ASSERTION-> Did you hide the control? ->END

	ROWCOL nEditRow, nEditCol;
	BOOL bEdit;
	if ((bEdit = GetCurrentCell(nEditRow, nEditCol)) != 0)
	{
		if (nEditCol >= nFromCol && nEditCol <= nToCol)
		{
			StoreCurrentCell(FALSE);
			// MarkEdit(nEditRow, nEditCol);
		}
		else if (nEditCol > nToCol && nEditCol > nCount)
		{
			// Store in grid
			StoreCurrentCell(TRUE, nEditRow, nEditCol-nCount);

			// Sync control
			GetCurrentCell(nEditRow, nEditCol);
			CGXControl* pControl = GetControl(nEditRow, nEditCol);
			if (pControl->IsInit())
				pControl->Init(nEditRow, nEditCol);
		}
	}

	// Update excel selection frame only
	m_rgLastSelectionFrame.m_bValid = FALSE;
	if (!GetParam()->GetRangeList()->IsEmpty())
		DrawSelectionFrame(NULL, FALSE, GetParam()->GetRangeList()->GetTail());

	if (!IsLockUpdate())
	{
		BOOL bLock = LockScrollbars(TRUE);

		if (m_pOptimizedUpdateImp)
			m_pOptimizedUpdateImp->UpdateRemoveCols(this, nFromCol, nToCol, anOldWidths, flags);
		else
			Redraw(flags, FALSE);

		LockScrollbars(bLock);

		if (flags)
			UpdateScrollbars(!(flags & GX_UPDATENOW));

	}

#if _MFC_VER >= 0x0400
	if (m_pTooltipProxy)
		m_pTooltipProxy->Reset();
#endif

	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintUpdateRemoveCols(this, nFromCol, nToCol, anOldWidths, flags);

	// TRACE3("CGXGridCore: END   UpdateRemoveCols(%u, %u, %05x)\n", nFromCol, nToCol, flags);
}

BOOL CGXGridCore::RemoveRows(ROWCOL nFromRow, ROWCOL nToRow, UINT flags /* = GX_UPDATENOW */, GXCmdType ctCmd /*= gxDo*/)
{
	// TRACE2("CGXGridCore: BEGIN RemoveRows(%u, %u)\n", nFromRow, nToRow);
	ASSERT(nFromRow > 0 && nToRow >= nFromRow);

	// Transfer Current Cell's Data to grid
	if (!TransferCurrentCell())
		return FALSE;

	CGXLockSelectionFrame tmpLockSelFrame(this);

	CGXLongOperation theOp;
	theOp.SetStatusText(m_pAppData->strmUndoInfo, FALSE);

	// Old Heights
	ROWCOL nCount = nToRow-nFromRow+1;
	int* oldHeights = NULL;

	// Cell Styles
	CObArray* pOldCellsArray = NULL;    // will be filled with current cells
	CObArray* pOldRowsArray = NULL;    // will be filled with current row styles

	ROWCOL nColCount = GetColCount()+1;

	// generate undo info
	if (m_pUndoImp && ctCmd != gxRollback && m_pParam->m_bUndoEnabled)
	{
		if (!m_pUndoImp->CreateRemoveRowsUndoInfo(this, nFromRow, nCount, nColCount, pOldCellsArray, pOldRowsArray, theOp))
			return FALSE;
	}

	BOOL bCoverDone = FALSE;

	// Adjust covered cells
	if (m_pCoveredCellsImp)
		bCoverDone = m_pCoveredCellsImp->RemoveRows(this, nFromRow, nToRow, flags, ctCmd);

	theOp.SetStatusText(m_pAppData->strmRemoveRows, FALSE);

	//
	// old row heights are needed either for efficient
	// updating of the grid when rows inside the grid are deleted
	// (and not the last rows)
	// - Or -
	// when undo information is created to restore the old row heights
	//
	if (flags == GX_UPDATENOW && !IsLockUpdate() && nToRow+1 < GetRowCount()
		|| pOldCellsArray || pOldRowsArray)
	{
		oldHeights = new int[nCount];
		GetArrayWithRowHeights(nFromRow, nToRow, oldHeights);
	}


	BOOL bAdvUndo = SetAdvancedUndo(m_pParam->m_bUndoEnabled && ctCmd != gxRollback);
	BOOL bDone = StoreRemoveRows(nFromRow, nToRow);
	SetAdvancedUndo(bAdvUndo);

	if (bDone)
	{
		UpdateRemoveRows(nFromRow, nToRow, oldHeights, flags, TRUE);

		if (m_pUndoImp && ctCmd != gxRollback && m_pParam->m_bUndoEnabled)
		{
			// Latest member in pOldCellsArray will be assigned advanced undo info 
			// that we need to initialize here. 

			if (pOldCellsArray)
			{
				CObject* pAdvUndo = GetAdvancedUndoInfo();

				if (pAdvUndo)
					pOldCellsArray->SetAtGrow(pOldCellsArray->GetSize(), pAdvUndo);
			}
			else
				CleanAdvancedUndo();

			AddCommand(m_pUndoImp->CreateInsertRowsCmd(nFromRow, nCount, oldHeights, pOldCellsArray, pOldRowsArray, nColCount), ctCmd);

			if (ctCmd == gxRedo || ctCmd == gxUndo)
				OnCellUndone(nFromRow, GetLeftCol());
		}
		else
		{
			delete oldHeights;
			CleanAdvancedUndo();
		}

		SetSelection(0);

		// refresh formula cells 
		RefreshViews();
	}
	else
	{
		CleanAdvancedUndo();
		delete oldHeights;

		// clean up undo information
		if (pOldCellsArray)
		{
			GXDeleteAllObjects(*pOldCellsArray);
			delete pOldCellsArray;
			pOldCellsArray = NULL;
		}

		if (pOldRowsArray)
		{
			GXDeleteAllObjects(*pOldRowsArray);
			delete pOldRowsArray;
			pOldRowsArray = NULL;
		}
	}

	if (ctCmd == gxDo)
		CommitTrans();

	// TRACE2("CGXGridCore: END   RemoveRows(%u, %u)\n", nFromRow, nToRow);
	return TRUE;
}

BOOL CGXGridCore::RemoveCols(ROWCOL nFromCol, ROWCOL nToCol, UINT flags /* = GX_UPDATENOW */, GXCmdType ctCmd /*= gxDo*/)
{
	// TRACE2("CGXGridCore: BEGIN RemoveCols(%u, %u)\n", nFromCol, nToCol);
	ASSERT(nFromCol > 0 && nToCol >= nFromCol);

	// Transfer Current Cell's Data to grid
	if (!TransferCurrentCell())
		return FALSE;

	CGXLockSelectionFrame tmpLockSelFrame(this);

	CGXLongOperation theOp;
	theOp.SetStatusText(m_pAppData->strmUndoInfo, FALSE);

	// Old Widths
	ROWCOL nCount = nToCol-nFromCol+1;
	int* oldWidths = NULL;

	// Cell Styles
	CObArray* pOldCellsArray = NULL;    // will be filled with current cells
	CObArray* pOldColsArray = NULL;    // will be filled with current column styles

	ROWCOL nRowCount = GetRowCount()+1;

	// generate undo info

	if (m_pUndoImp && ctCmd != gxRollback && m_pParam->m_bUndoEnabled)
	{
		if (!m_pUndoImp->CreateRemoveColsUndoInfo(this, nFromCol, nCount, nRowCount, pOldCellsArray, pOldColsArray, theOp))
			return FALSE;
	}

	BOOL bCoverDone = FALSE;

	// Adjust covered cells
	if (m_pCoveredCellsImp)
		bCoverDone = m_pCoveredCellsImp->RemoveCols(this, nFromCol, nToCol, flags, ctCmd);

	theOp.SetStatusText(m_pAppData->strmRemoveCols, FALSE);

	//
	// old column widths are needed either for efficient
	// updating of the grid when rows inside the grid are deleted
	// (and not the last columns)
	// - Or -
	// when undo information is created to restore the old column widths
	//
	if (flags == GX_UPDATENOW && !IsLockUpdate() && nToCol+1 < GetColCount()
		|| pOldCellsArray || pOldColsArray)
	{
		oldWidths = new int[nCount];
		GetArrayWithColWidths(nFromCol, nToCol, oldWidths);
	}

	BOOL bAdvUndo = SetAdvancedUndo(m_pParam->m_bUndoEnabled && ctCmd != gxRollback);
	BOOL bDone = StoreRemoveCols(nFromCol, nToCol);
	SetAdvancedUndo(bAdvUndo);

	if (bDone)
	{
		UpdateRemoveCols(nFromCol, nToCol, oldWidths, flags, TRUE);

		if (m_pUndoImp && ctCmd != gxRollback && m_pParam->m_bUndoEnabled)
		{
			// Latest member in pOldCellsArray will be assigned advanced undo info 
			// that we need to initialize here. 
			if (pOldCellsArray)
			{
				CObject* pAdvUndo = GetAdvancedUndoInfo();

				if (pAdvUndo)
					pOldCellsArray->SetAtGrow(pOldCellsArray->GetSize(), pAdvUndo);
			}
			else
				CleanAdvancedUndo();

			AddCommand(m_pUndoImp->CreateInsertColsCmd(nFromCol, nCount, oldWidths, pOldCellsArray, pOldColsArray, nRowCount), ctCmd);

			if (ctCmd == gxRedo || ctCmd == gxUndo)
				OnCellUndone(GetTopRow(), nFromCol);
		}
		else
		{
			CleanAdvancedUndo();
			delete oldWidths;
		}

		SetSelection(0);

		// refresh formula cells 
		RefreshViews();
	}
	else
	{
		CleanAdvancedUndo();
		delete oldWidths;

		// clean up
		if (pOldCellsArray)
		{
			GXDeleteAllObjects(*pOldCellsArray);
			delete pOldCellsArray;
			pOldCellsArray = NULL;
		}

		if (pOldColsArray)
		{
			GXDeleteAllObjects(*pOldColsArray);
			delete pOldColsArray;
			pOldColsArray = NULL;
		}

	}

	if (ctCmd == gxDo)
		CommitTrans();

	// TRACE2("CGXGridCore: END   RemoveCols(%u, %u)\n", nFromCol, nToCol);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// Moving Rows or Colums

BOOL CGXGridCore::StoreMoveRows(ROWCOL nFromRow, ROWCOL nToRow, ROWCOL nDestRow, BOOL bProcessed)
{
	GX_CHECKIMP(m_pMoveRowsColsImp, "CGXGridCore::StoreMoveRows");

	if (m_pMoveRowsColsImp)
		return m_pMoveRowsColsImp->StoreMoveRows(this, nFromRow, nToRow, nDestRow, bProcessed);

	return FALSE;
}

void CGXGridCore::UpdateMoveRows(ROWCOL nFromRow, ROWCOL nToRow, ROWCOL nDestRow, UINT flags, BOOL bCreateHint)
{
	GX_CHECKIMP(m_pMoveRowsColsImp, "CGXGridCore::UpdateMoveRows");

	if (m_pMoveRowsColsImp)
		m_pMoveRowsColsImp->UpdateMoveRows(this, nFromRow, nToRow, nDestRow, flags, bCreateHint);
}

BOOL CGXGridCore::StoreMoveCols(ROWCOL nFromCol, ROWCOL nToCol, ROWCOL nDestCol, BOOL bProcessed)
{
	GX_CHECKIMP(m_pMoveRowsColsImp, "CGXGridCore::StoreMoveCols");

	if (m_pMoveRowsColsImp)
		return m_pMoveRowsColsImp->StoreMoveCols(this, nFromCol, nToCol, nDestCol, bProcessed);

	return FALSE;
}

void CGXGridCore::UpdateMoveCols(ROWCOL nFromCol, ROWCOL nToCol, ROWCOL nDestCol, UINT flags, BOOL bCreateHint)
{
	GX_CHECKIMP(m_pMoveRowsColsImp, "CGXGridCore::UpdateMoveCols");

	if (m_pMoveRowsColsImp)
		m_pMoveRowsColsImp->UpdateMoveCols(this, nFromCol, nToCol, nDestCol, flags, bCreateHint);
}

BOOL CGXGridCore::MoveRows(ROWCOL nFromRow, ROWCOL nToRow, ROWCOL nDestRow, UINT flags /* = GX_UPDATENOW */, GXCmdType ctCmd /*= gxDo*/)
{
	GX_CHECKIMP(m_pMoveRowsColsImp, "CGXGridCore::MoveRows");

	if (m_pMoveRowsColsImp)
		return m_pMoveRowsColsImp->MoveRows(this, nFromRow, nToRow, nDestRow, flags, ctCmd);

	return FALSE;
}

BOOL CGXGridCore::MoveCols(ROWCOL nFromCol, ROWCOL nToCol, ROWCOL nDestCol, UINT flags /* = GX_UPDATENOW */, GXCmdType ctCmd /*= gxDo*/)
{
	GX_CHECKIMP(m_pMoveRowsColsImp, "CGXGridCore::MoveCols");

	if (m_pMoveRowsColsImp)
		return m_pMoveRowsColsImp->MoveCols(this, nFromCol, nToCol, nDestCol, flags, ctCmd);

	return FALSE;
}

// Dragging a Selection

BOOL CGXGridCore::OnSelDragStart(ROWCOL nRow, ROWCOL nCol)
{
	GX_CHECKIMP(m_pUserDragSelectRangeImp, "CGXGridCore::OnSelDragStart");

	if (m_pUserDragSelectRangeImp)
		return m_pUserDragSelectRangeImp->OnSelDragStart(this, nRow, nCol);

	return FALSE;
}

BOOL CGXGridCore::OnSelDragRowsStart(ROWCOL, ROWCOL)
{
	BOOL bCtl = GetKeyState(VK_CONTROL) & 0x8000;
	return !bCtl;
}

BOOL CGXGridCore::OnSelDragColsStart(ROWCOL, ROWCOL)
{
	return TRUE;
}

BOOL CGXGridCore::OnSelDragMove(ROWCOL nRow1, ROWCOL nCol1, ROWCOL nRow, ROWCOL nCol)
{
	GX_CHECKIMP(m_pUserDragSelectRangeImp, "CGXGridCore::OnSelDragMove");

	if (m_pUserDragSelectRangeImp)
		return m_pUserDragSelectRangeImp->OnSelDragMove(this, nRow1, nCol1, nRow, nCol);

	return FALSE;
}

BOOL CGXGridCore::OnSelDragRowsMove(ROWCOL, ROWCOL, ROWCOL)
{
	return TRUE;
}

BOOL CGXGridCore::OnSelDragColsMove(ROWCOL, ROWCOL, ROWCOL)
{
	return TRUE;
}

BOOL CGXGridCore::OnSelDragDrop(ROWCOL nRow1, ROWCOL nCol1, ROWCOL nRow, ROWCOL nCol)
{
	GX_CHECKIMP(m_pUserDragSelectRangeImp, "CGXGridCore::OnSelDragDrop");

	if (m_pUserDragSelectRangeImp)
		return m_pUserDragSelectRangeImp->OnSelDragDrop(this, nRow1, nCol1, nRow, nCol);

	return FALSE;
}

BOOL CGXGridCore::OnSelDragRowsDrop(ROWCOL, ROWCOL, ROWCOL)
{
	return TRUE;
}

BOOL CGXGridCore::OnSelDragColsDrop(ROWCOL, ROWCOL, ROWCOL)
{
	return TRUE;
}

void CGXGridCore::OnSelDragCancel()
{
	GX_CHECKIMP(m_pUserDragSelectRangeImp, "CGXGridCore::OnSelDragCancel");

	if (m_pUserDragSelectRangeImp)
		m_pUserDragSelectRangeImp->OnSelDragCancel(this);
}

/////////////////////////////////////////////////////////////////////////////
// Predefined Menu Actions

BOOL CGXGridCore::CanFreezeCols()
{
	if (m_pFreezeMenuImp)
		return m_pFreezeMenuImp->CanFreezeCols(this);
	
	return FALSE;
}

BOOL CGXGridCore::CanUnfreezeCols()
{
	if (m_pFreezeMenuImp)
		return m_pFreezeMenuImp->CanUnfreezeCols(this);
	
	return FALSE;
}

void CGXGridCore::FreezeCols()
{
	GX_CHECKIMP(m_pFreezeMenuImp, "CGXGridCore::FreezeCols");

	if (m_pFreezeMenuImp)
		m_pFreezeMenuImp->FreezeCols(this);
}

void CGXGridCore::UnfreezeCols()
{
	GX_CHECKIMP(m_pFreezeMenuImp, "CGXGridCore::UnfreezeCols");

	if (m_pFreezeMenuImp)
		m_pFreezeMenuImp->UnfreezeCols(this);
}

BOOL CGXGridCore::CanFreezeRows()
{
	if (m_pFreezeMenuImp)
		return m_pFreezeMenuImp->CanFreezeRows(this);
	
	return FALSE;
}

BOOL CGXGridCore::CanUnfreezeRows()
{
	if (m_pFreezeMenuImp)
		return m_pFreezeMenuImp->CanUnfreezeRows(this);
	
	return FALSE;
}

void CGXGridCore::FreezeRows()
{
	GX_CHECKIMP(m_pFreezeMenuImp, "CGXGridCore::FreezeRows");

	if (m_pFreezeMenuImp)
		m_pFreezeMenuImp->FreezeRows(this);
}

void CGXGridCore::UnfreezeRows()
{
	GX_CHECKIMP(m_pFreezeMenuImp, "CGXGridCore::UnfreezeRows");

	if (m_pFreezeMenuImp)
		m_pFreezeMenuImp->UnfreezeRows(this);
}
