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

// gxhinti.cpp: implementation of update/hint mechanism class

#include "stdafx.h"

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

// Headers

#ifndef _GXCORE_H_
#include "gxcore.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXHINT")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

class CGXGridUpdateHintImp: public CGXAbstractGridUpdateHintImp
{
public:
	virtual void OnGridUpdate(CGXGridCore * pGrid, CObject* pHint);
	virtual void HintUpdateReadOnly(CGXGridCore* pGrid, BOOL bOldReadOnly);
	virtual void HintSetCurrentCell(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, UINT flags);
	virtual BOOL HintTransferCurrentCell(CGXGridCore* pGrid, BOOL bSaveAndValidate, UINT flags);
	virtual void HintUpdateChangedRowCount(CGXGridCore* pGrid, ROWCOL nOldRows, UINT flags);
	virtual void HintUpdateChangedColCount(CGXGridCore* pGrid, ROWCOL nOldCols, UINT flags);
	virtual void HintUpdateFrozenCols(CGXGridCore* pGrid, ROWCOL nOldFrozenCols, ROWCOL nOldHeaderCols, UINT flags);
	virtual void HintUpdateFrozenRows(CGXGridCore* pGrid, ROWCOL nOldFrozenRows, ROWCOL nOldHeaderRows, UINT flags);
	virtual void HintSetTopRow(CGXGridCore* pGrid, ROWCOL nRow, UINT flags);
	virtual void HintSetLeftCol(CGXGridCore* pGrid, ROWCOL nCol, UINT flags);
	virtual void HintUpdateInsertRows(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCount, UINT flags);
	virtual void HintUpdateInsertCols(CGXGridCore* pGrid, ROWCOL nCol, ROWCOL nCount, UINT flags);
	virtual void HintRedrawRowCol(CGXGridCore* pGrid, ROWCOL nStartRow, ROWCOL nStartCol, ROWCOL nEndRow, ROWCOL nEndCol, UINT flags);
	virtual void HintRedraw(CGXGridCore* pGrid, UINT flags);
	virtual void HintRefreshViews(CGXGridCore* pGrid, CString* pErrorStr);
	virtual void HintOnTabChanged(CGXGridCore* pGrid, int nTab);
	virtual void HintOnAutoScroll(CGXGridCore* pGrid, int direction, ROWCOL nCell);
	virtual void HintUpdateSelectRange(CGXGridCore* pGrid, const CGXRange& range, const CGXRangeList* pOldRangeList);
	virtual void HintLockSelectionFrame(CGXGridCore* pGrid, BOOL bLock);
	virtual void HintPrepareChangeSelection(CGXGridCore* pGrid, POSITION oldPos, ROWCOL top, ROWCOL left, ROWCOL bottom, ROWCOL right);
	virtual void HintPrepareClearSelection(CGXGridCore* pGrid);
	virtual void HintUpdateChangedRowHeights(CGXGridCore* pGrid, ROWCOL nFromRow, ROWCOL nToRow, int* nOldHeights, UINT flags);
	virtual void HintUpdateChangedColWidths(CGXGridCore* pGrid, ROWCOL nFromCol, ROWCOL nToCol, int* nOldWidths, UINT flags);
	virtual void HintStoreZoom(CGXGridCore* pGrid, int nZoom);
	virtual void HintUpdateZoom(CGXGridCore* pGrid, int nOldZoom, UINT flags);
	virtual void HintUpdateStyleRange(CGXGridCore* pGrid, const CGXRange& range, const CGXStyle* pStyle, GXModifyType mt, int nType, UINT flags);
	virtual void HintUpdateStylesMap(CGXGridCore* pGrid, UINT flags);
	virtual void HintUpdateProperties(CGXGridCore* pGrid, UINT flags);
	virtual void HintUpdateBaseStyle(CGXGridCore* pGrid, WORD wStyleId, UINT flags);
	virtual void HintUpdateCoveredCellsRowCol(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, ROWCOL nOldToRow, ROWCOL nOldToCol, UINT flags);
	virtual void HintUpdateFloatedCellsRowCol(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, ROWCOL nOldToRow, ROWCOL nOldToCol, UINT flags);
	virtual void HintUpdateMergedCellsRowCol(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, ROWCOL nOldToRow, ROWCOL nOldToCol, UINT flags);
	virtual void HintUpdateMoveDataRows(CGXGridCore* pGrid, const CGXRange& sortRange, UINT flags);
	virtual void HintUpdateMoveDataCols(CGXGridCore* pGrid, const CGXRange& sortRange, UINT flags);
	virtual void HintUpdateRemoveRows(CGXGridCore* pGrid, ROWCOL nFromRow, ROWCOL nToRow, int* anOldHeights, UINT flags);
	virtual void HintUpdateRemoveCols(CGXGridCore* pGrid, ROWCOL nFromCol, ROWCOL nToCol, int* anOldWidths, UINT flags);
	virtual void HintUpdateMoveRows(CGXGridCore* pGrid, ROWCOL nFromRow, ROWCOL nToRow, ROWCOL nDestRow, UINT flags);
	virtual void HintUpdateMoveCols(CGXGridCore* pGrid, ROWCOL nFromCol, ROWCOL nToCol, ROWCOL nDestCol, UINT flags);
};

	
// CGXAbstractGridUpdateHintImp* m_pUpdateHintImp;
void CGXGridCore::ImplementUpdateHint()
{
	if (m_pUpdateHintImp == NULL)
		AutoDeletePtr(m_pUpdateHintImp = new CGXGridUpdateHintImp);
}

//////////////////////////////////////////////////////////////////////////////

void CGXGridUpdateHintImp::OnGridUpdate(CGXGridCore* pGrid, CObject* pHint)
{
	if (pGrid->GetParam() == NULL
		|| pHint == NULL
		|| !pHint->IsKindOf(RUNTIME_CLASS(CGXGridHint)))
		return;

	CGXGridHint &info = *((CGXGridHint*) pHint);

	// same document, but other view-id
	if (info.nViewID != -1 && info.nViewID != pGrid->m_nViewID)
		return;

	switch (info.m_id)
	{
	case gxHintRedraw:
		pGrid->Redraw(
			info.flags,
			FALSE
			);
		break;

	case gxHintRedrawRowCol:
		pGrid->RedrawRowCol(
			info.nRow1,
			info.nCol1,
			info.nRow2,
			info.nCol2,
			info.flags,
			FALSE
			);
		break;

	case gxHintUpdateChangedRowCount:
		pGrid->UpdateChangedRowCount(
			info.nRow1,
			info.flags,
			FALSE
			);
		break;

	case gxHintUpdateChangedColCount:
		pGrid->UpdateChangedColCount(
			info.nCol1,
			info.flags,
			FALSE
			);
		break;

	case gxHintUpdateChangedRowHeights:
		pGrid->UpdateChangedRowHeights(
			info.nRow1,
			info.nRow2,
			(int*) info.vptr,
			info.flags,
			FALSE
			);
		break;

	case gxHintUpdateChangedColWidths:
		pGrid->UpdateChangedColWidths(
			info.nCol1,
			info.nCol2,
			(int*) info.vptr,
			info.flags,
			FALSE
			);
		break;

	case gxHintUpdateInsertCols:
		pGrid->UpdateInsertCols(
			info.nCol1,
			(ROWCOL) info.dwParam,
			info.flags,
			FALSE
			);
		break;

	case gxHintUpdateInsertRows:
		pGrid->UpdateInsertRows(
			info.nRow1,
			(ROWCOL) info.dwParam,
			info.flags,
			FALSE
			);
		break;

	case gxHintUpdateRemoveCols:
		pGrid->UpdateRemoveCols(
			info.nCol1,
			info.nCol2,
			(int*) info.vptr,
			info.flags,
			FALSE
			);
		break;

	case gxHintUpdateRemoveRows:
		pGrid->UpdateRemoveRows(
			info.nRow1,
			info.nRow2,
			(int*) info.vptr,
			info.flags,
			FALSE
			);
		break;

	case gxHintUpdateMoveCols:
		pGrid->UpdateMoveCols(
			info.nCol1,
			info.nCol2,
			info.nCol3,
			info.flags,
			FALSE
			);
		break;

	case gxHintUpdateMoveRows:
		pGrid->UpdateMoveRows(
			info.nRow1,
			info.nRow2,
			info.nRow3,
			info.flags,
			FALSE
			);
		break;

	case gxHintPrepareChangeSelection:
		pGrid->PrepareChangeSelection(
			(POSITION) info.vptr,
			info.nRow1,
			info.nCol1,
			info.nRow2,
			info.nCol2,
			FALSE);
		break;

	case gxHintLockSelectionFrame:
		pGrid->LockSelectionFrame((BOOL) info.flags, FALSE);
		break;

	case gxHintUpdateSelectRange:
		pGrid->UpdateSelectRange(
			info.range,
			(const CGXRangeList*) info.vptr,
			FALSE);
		break;

	case gxHintPrepareClearSelection:
		pGrid->PrepareClearSelection(FALSE);
		break;

	case gxHintUpdateFrozenRows:
		pGrid->UpdateFrozenRows(
			info.nRow1,
			info.nRow2,
			info.flags,
			FALSE
			);
		break;

	case gxHintUpdateFrozenCols:
		pGrid->UpdateFrozenCols(
			info.nCol1,
			info.nCol2,
			info.flags,
			FALSE
			);
		break;

	case gxHintUpdateStyleRange:
		pGrid->UpdateStyleRange(
			info.range,
			info.pStyle,
			info.mt,
			(int) info.lParam,
			info.flags,
			FALSE
			);
		break;

	case gxHintUpdateCoveredCellsRowCol:
		pGrid->UpdateCoveredCellsRowCol(
			info.nRow1,
			info.nCol1,
			info.nRow2,
			info.nCol2,
			info.flags,
			FALSE
			);
		break;

	case gxHintUpdateFloatedCellsRowCol:
		pGrid->UpdateFloatedCellsRowCol(
			info.nRow1,
			info.nCol1,
			info.nRow2,
			info.nCol2,
			info.flags,
			FALSE
			);
		break;

	case gxHintUpdateMergedCellsRowCol:
		pGrid->UpdateMergedCellsRowCol(
			info.nRow1,
			info.nCol1,
			info.nRow2,
			info.nCol2,
			info.flags,
			FALSE
			);
		break;

	case gxHintMoveDataRows:
		pGrid->UpdateMoveDataRows(info.range, info.flags, FALSE);
		break;

	case gxHintMoveDataCols:
		pGrid->UpdateMoveDataCols(info.range, info.flags, FALSE);
		break;

	case gxHintUpdateProperties:
		pGrid->UpdateProperties(info.flags, FALSE);
		break;

	case gxHintUpdateStylesMap:
		pGrid->UpdateStylesMap(info.flags, FALSE);
		break;

	case gxHintUpdateBaseStyle:
		pGrid->UpdateBaseStyle((WORD) info.dwParam, info.flags, FALSE);
		break;

	case gxHintRefreshViews:
		pGrid->RefreshViews(FALSE, (CString*) info.vptr, FALSE);
		break;

	case gxHintUpdateZoom:
		if (pGrid->GetParam()->m_wFloatCellsMode != gxnFloatDisable
			|| info.vptr == pGrid->m_pSplitterWnd)
		{
			pGrid->UpdateZoom(
				(int) info.lParam,
				info.flags,
				FALSE
				);
		}
		break;

	case gxHintStoreZoom:
		if (pGrid->GetParam()->m_wFloatCellsMode != gxnFloatDisable
			|| info.vptr == pGrid->m_pSplitterWnd)
		{
			pGrid->StoreZoom(
				(int) info.lParam,
				FALSE
				);
		}
		break;

	case gxHintUpdateReadOnly:
		pGrid->UpdateReadOnly(
			(BOOL) info.lParam, FALSE);
		break;

	case gxHintTransferCurrentCell:
		info.dwParam &= (DWORD) pGrid->TransferCurrentCell(
			(BOOL) info.lParam, info.flags, FALSE);
		break;

	case gxHintSetTopRow:
		if (pGrid->m_pSplitterWnd && pGrid->m_pSplitterWnd->GetColumnCount() > 1 && pGrid->m_nSplitRow < pGrid->m_pSplitterWnd->GetRowCount())
		{
			CWnd* pPane = pGrid->m_pSplitterWnd->GetPane(pGrid->m_nSplitRow, pGrid->m_nSplitCol);
			if (info.vptr == pPane)
				pGrid->SetTopRow(info.nRow1, info.flags, FALSE);
		}
		break;

	case gxHintSetLeftCol:
		if (pGrid->m_pSplitterWnd && pGrid->m_pSplitterWnd->GetRowCount() > 1 && pGrid->m_nSplitCol < pGrid->m_pSplitterWnd->GetColumnCount())
		{
			CWnd* pPane = pGrid->m_pSplitterWnd->GetPane(pGrid->m_nSplitRow, pGrid->m_nSplitCol);
			if (info.vptr == pPane)
				pGrid->SetLeftCol(info.nCol1, info.flags, FALSE);
		}
		break;

	case gxHintChangedTab:
		{
			if (pGrid->m_pAppData->m_pTabWndInt)
				pGrid->m_pAppData->m_pTabWndInt->UpdateTabName(pGrid->m_pGridWnd, pGrid->GetParam()->m_sSheetName, TRUE);
		}
		break;

	case gxHintSyncCurrentCell:
		pGrid->SetCurrentCell(info.nRow1, info.nCol1, info.flags);
		break;

	default:
		return;
	}
}

void CGXGridUpdateHintImp::HintUpdateReadOnly(CGXGridCore* pGrid, BOOL bOldReadOnly)
{
	// Create Hint
	CGXGridHint hint(gxHintUpdateReadOnly, pGrid->m_nViewID);
	hint.lParam = bOldReadOnly;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintSetCurrentCell(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, UINT flags)
{
	CGXGridHint hint(gxHintSyncCurrentCell, pGrid->m_nViewID);
	hint.nRow1 = nRow;
	hint.nCol1 = nCol;
	hint.flags = flags ;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

BOOL CGXGridUpdateHintImp::HintTransferCurrentCell(CGXGridCore* pGrid, BOOL bSaveAndValidate, UINT flags)
{
	CGXGridHint hint(gxHintTransferCurrentCell, pGrid->m_nViewID);
	hint.lParam = bSaveAndValidate;
	hint.dwParam = TRUE;
	hint.flags = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);

	return (BOOL) hint.dwParam;
}

void CGXGridUpdateHintImp::HintUpdateChangedRowCount(CGXGridCore* pGrid, ROWCOL nOldRows, UINT flags)
{
	CGXGridHint hint(gxHintUpdateChangedRowCount, pGrid->m_nViewID);
	hint.nRow1 = nOldRows;
	hint.flags = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintUpdateChangedColCount(CGXGridCore* pGrid, ROWCOL nOldCols, UINT flags)
{
	CGXGridHint hint(gxHintUpdateChangedColCount, pGrid->m_nViewID);
	hint.nCol1 = nOldCols;
	hint.flags = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintUpdateFrozenCols(CGXGridCore* pGrid, ROWCOL nOldFrozenCols, ROWCOL nOldHeaderCols, UINT flags)
{
	CGXGridHint hint(gxHintUpdateFrozenCols, pGrid->m_nViewID);
	hint.nCol1 = nOldFrozenCols;
	hint.nCol2 = nOldHeaderCols;
	hint.flags    = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintUpdateFrozenRows(CGXGridCore* pGrid, ROWCOL nOldFrozenRows, ROWCOL nOldHeaderRows, UINT flags)
{
	CGXGridHint hint(gxHintUpdateFrozenRows, pGrid->m_nViewID);
	hint.nRow1 = nOldFrozenRows;
	hint.nRow2 = nOldHeaderRows;
	hint.flags    = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintSetTopRow(CGXGridCore* pGrid, ROWCOL nRow, UINT flags)
{
	if (pGrid->m_pSplitterWnd && pGrid->m_pSplitterWnd->GetColumnCount() > 1)
	{
		CWnd* pPane = pGrid->m_pSplitterWnd->GetPane(pGrid->m_nSplitRow, 1-pGrid->m_nSplitCol);

		CGXGridHint hint(gxHintSetTopRow, pGrid->m_nViewID);
		hint.nRow1 = nRow;
		hint.flags = flags;
		hint.vptr = pPane;

		pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
	}
}

void CGXGridUpdateHintImp::HintSetLeftCol(CGXGridCore* pGrid, ROWCOL nCol, UINT flags)
{
	if (pGrid->m_pSplitterWnd && pGrid->m_pSplitterWnd->GetRowCount() > 1)
	{
		CWnd* pPane = pGrid->m_pSplitterWnd->GetPane(1-pGrid->m_nSplitRow, pGrid->m_nSplitCol);

		CGXGridHint hint(gxHintSetLeftCol, pGrid->m_nViewID);
		hint.nCol1 = nCol;
		hint.flags = flags;
		hint.vptr = pPane;

		pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
	}
}

void CGXGridUpdateHintImp::HintUpdateInsertRows(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCount, UINT flags)
{
	CGXGridHint hint(gxHintUpdateInsertRows, pGrid->m_nViewID);
	hint.nRow1 = nRow;
	hint.dwParam = nCount;
	hint.flags = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintUpdateInsertCols(CGXGridCore* pGrid, ROWCOL nCol, ROWCOL nCount, UINT flags)
{
	CGXGridHint hint(gxHintUpdateInsertCols, pGrid->m_nViewID);
	hint.nCol1 = nCol;
	hint.dwParam = nCount;
	hint.flags = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintRedrawRowCol(CGXGridCore* pGrid, ROWCOL nStartRow, ROWCOL nStartCol, ROWCOL nEndRow, ROWCOL nEndCol, UINT flags)
{
	CGXGridHint hint(gxHintRedrawRowCol, pGrid->m_nViewID);
	hint.nRow1 = nStartRow;
	hint.nCol1 = nStartCol;
	hint.nRow2 = nEndRow;
	hint.nCol2 = nEndCol;
	hint.flags = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintRedraw(CGXGridCore* pGrid, UINT flags)
{
	CGXGridHint hint(gxHintRedraw, pGrid->m_nViewID);
	hint.flags = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintRefreshViews(CGXGridCore* pGrid, CString* pErrorStr)
{
	CGXGridHint hint(gxHintRefreshViews, pGrid->m_nViewID);

	hint.vptr = pErrorStr;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintOnTabChanged(CGXGridCore* pGrid, int nTab)
{
	GX_UNUSED_ALWAYS(nTab);

	CGXGridHint hint(gxHintChangedTab, pGrid->m_nViewID);
	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintOnAutoScroll(CGXGridCore* pGrid, int direction, ROWCOL nCell)
{
	GX_UNUSED_ALWAYS(nCell);

	switch (direction)
	{
	case GX_UP:
	case GX_DOWN:
		if (pGrid->m_pSplitterWnd && pGrid->m_pSplitterWnd->GetColumnCount() > 1)
		{
			// create hint when used in a dynamic splitter window and send it
			// to the other pane
			CWnd* pPane = pGrid->m_pSplitterWnd->GetPane(pGrid->m_nSplitRow, 1-pGrid->m_nSplitCol);

			CGXGridHint hint(gxHintSetTopRow, pGrid->m_nViewID);
			hint.nRow1 = pGrid->GetTopRow();
			hint.flags = GX_UPDATENOW;
			hint.vptr = pPane;

			pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
		}

		break;

	case GX_LEFT:
	case GX_RIGHT:
		if (pGrid->m_pSplitterWnd && pGrid->m_pSplitterWnd->GetRowCount() > 1)
		{
			// create hint when used in a dynamic splitter window and send it
			// to the other pane
			CWnd* pPane = pGrid->m_pSplitterWnd->GetPane(1-pGrid->m_nSplitRow, pGrid->m_nSplitCol );

			CGXGridHint hint(gxHintSetLeftCol, pGrid->m_nViewID);
			hint.nCol1 = pGrid->GetLeftCol();
			hint.flags = GX_UPDATENOW;
			hint.vptr = pPane;

			pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
		}
		break;
	}
}


void CGXGridUpdateHintImp::HintUpdateSelectRange(CGXGridCore* pGrid, const CGXRange& range, const CGXRangeList* pOldRangeList)
{
	CGXGridHint hint(gxHintUpdateSelectRange, pGrid->m_nViewID);
	hint.range = range;
	hint.vptr = (void*) pOldRangeList;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintLockSelectionFrame(CGXGridCore* pGrid, BOOL bLock)
{
	CGXGridHint hint(gxHintLockSelectionFrame, pGrid->m_nViewID);
	hint.flags =  bLock;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintPrepareChangeSelection(CGXGridCore* pGrid, POSITION oldPos, ROWCOL top, ROWCOL left, ROWCOL bottom, ROWCOL right)
{
	CGXGridHint hint(gxHintPrepareChangeSelection, pGrid->m_nViewID);
	hint.vptr =  oldPos;
	hint.nRow1 = top;
	hint.nCol1 = left;
	hint.nRow2 = bottom;
	hint.nCol2 = right;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintPrepareClearSelection(CGXGridCore* pGrid)
{
	CGXGridHint hint(gxHintPrepareClearSelection, pGrid->m_nViewID);

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintUpdateChangedRowHeights(CGXGridCore* pGrid, ROWCOL nFromRow, ROWCOL nToRow, int* nOldHeights, UINT flags)
{
	CGXGridHint hint(gxHintUpdateChangedRowHeights, pGrid->m_nViewID);
	hint.nRow1 = nFromRow;
	hint.nRow2 = nToRow;
	hint.vptr  = nOldHeights;
	hint.flags = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintUpdateChangedColWidths(CGXGridCore* pGrid, ROWCOL nFromCol, ROWCOL nToCol, int* nOldWidths, UINT flags)
{
	CGXGridHint hint(gxHintUpdateChangedColWidths, pGrid->m_nViewID);
	hint.nCol1 = nFromCol;
	hint.nCol2 = nToCol;
	hint.vptr  = nOldWidths;
	hint.flags = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintStoreZoom(CGXGridCore* pGrid, int nZoom)
{
	CGXGridHint hint(gxHintStoreZoom, pGrid->m_nViewID);
	hint.lParam = nZoom;
	hint.vptr = pGrid->m_pSplitterWnd;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintUpdateZoom(CGXGridCore* pGrid, int nOldZoom, UINT flags)
{
	CGXGridHint hint(gxHintUpdateZoom, pGrid->m_nViewID);
	hint.lParam = nOldZoom;
	hint.vptr = pGrid->m_pSplitterWnd;
	hint.flags        = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintUpdateStyleRange(CGXGridCore* pGrid, const CGXRange& rgBoundary, const CGXStyle* pStyle, GXModifyType mt, int nType, UINT flags)
{
	CGXGridHint hint(gxHintUpdateStyleRange, pGrid->m_nViewID);
	hint.range = rgBoundary;
	hint.pStyle = pStyle;
	hint.mt    = mt;
	hint.lParam = nType;
	hint.flags = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintUpdateStylesMap(CGXGridCore* pGrid, UINT flags)
{
	CGXGridHint hint(gxHintUpdateStylesMap, pGrid->m_nViewID);
	hint.flags    = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintUpdateProperties(CGXGridCore* pGrid, UINT flags)
{
	CGXGridHint hint(gxHintUpdateProperties, pGrid->m_nViewID);
	hint.flags    = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintUpdateBaseStyle(CGXGridCore* pGrid, WORD wStyleId, UINT flags)
{
	CGXGridHint hint(gxHintUpdateBaseStyle, pGrid->m_nViewID);
	hint.dwParam  = wStyleId;
	hint.flags    = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintUpdateCoveredCellsRowCol(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, ROWCOL nOldToRow, ROWCOL nOldToCol, UINT flags)
{
	CGXGridHint hint(gxHintUpdateCoveredCellsRowCol, pGrid->m_nViewID);
	hint.nRow1      = nRow;
	hint.nCol1      = nCol;
	hint.nRow2 = nOldToRow;
	hint.nCol2 = nOldToCol;
	hint.flags     = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintUpdateFloatedCellsRowCol(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, ROWCOL nOldToRow, ROWCOL nOldToCol, UINT flags)
{
	CGXGridHint hint(gxHintUpdateFloatedCellsRowCol, pGrid->m_nViewID);
	hint.nRow1      = nRow;
	hint.nCol1      = nCol;
	hint.nRow2 = nOldToRow;
	hint.nCol2 = nOldToCol;
	hint.flags     = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintUpdateMergedCellsRowCol(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, ROWCOL nOldToRow, ROWCOL nOldToCol, UINT flags)
{
	CGXGridHint hint(gxHintUpdateMergedCellsRowCol, pGrid->m_nViewID);
	hint.nRow1      = nRow;
	hint.nCol1      = nCol;
	hint.nRow2 = nOldToRow;
	hint.nCol2 = nOldToCol;
	hint.flags     = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintUpdateMoveDataRows(CGXGridCore* pGrid, const CGXRange& sortRange, UINT flags)
{
	CGXGridHint hint(gxHintMoveDataRows, pGrid->m_nViewID);
	hint.flags = flags;
	hint.range = sortRange;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintUpdateMoveDataCols(CGXGridCore* pGrid, const CGXRange& sortRange, UINT flags)
{
	CGXGridHint hint(gxHintMoveDataCols, pGrid->m_nViewID);
	hint.flags = flags;
	hint.range = sortRange;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintUpdateRemoveRows(CGXGridCore* pGrid, ROWCOL nFromRow, ROWCOL nToRow, int* anOldHeights, UINT flags)
{
	// Create Hint
	CGXGridHint hint(gxHintUpdateRemoveRows, pGrid->m_nViewID);
	hint.nRow1 = nFromRow;
	hint.nRow2 = nToRow;
	hint.vptr = anOldHeights;
	hint.flags = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintUpdateRemoveCols(CGXGridCore* pGrid, ROWCOL nFromCol, ROWCOL nToCol, int* anOldWidths, UINT flags)
{
	CGXGridHint hint(gxHintUpdateRemoveCols, pGrid->m_nViewID);
	hint.nCol1 = nFromCol;
	hint.nCol2 = nToCol;
	hint.vptr = anOldWidths;
	hint.flags = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintUpdateMoveRows(CGXGridCore* pGrid, ROWCOL nFromRow, ROWCOL nToRow, ROWCOL nDestRow, UINT flags)
{
	CGXGridHint hint(gxHintUpdateMoveRows, pGrid->m_nViewID);
	hint.nRow1 = nFromRow;
	hint.nRow2 = nToRow;
	hint.nRow3 = nDestRow;
	hint.flags = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

void CGXGridUpdateHintImp::HintUpdateMoveCols(CGXGridCore* pGrid, ROWCOL nFromCol, ROWCOL nToCol, ROWCOL nDestCol, UINT flags)
{
	CGXGridHint hint(gxHintUpdateMoveCols, pGrid->m_nViewID);
	hint.nCol1 = nFromCol;
	hint.nCol2 = nToCol;
	hint.nCol3 = nDestCol;
	hint.flags = flags;

	pGrid->UpdateAllViews(pGrid->m_pGridWnd, 0, &hint);
}

