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

// gxcorabs.cpp : implementation of the CGXGridCore class
//

#include "stdafx.h"

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

// Headers

#ifndef _GXABSTR_H_
#include "gxabstr.h"
#endif

#ifndef _GXCORE_H_
#include "gxcore.h"
#endif


#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCORABS")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Undo, Redo, Transactions - Implemementation can be found in gxcmd.cpp

BOOL CGXGridCore::SetAdvancedUndo(BOOL bAdvUndo)
{
	if (m_pUndoImp)
		return m_pUndoImp->SetAdvancedUndo(this, bAdvUndo);

	return FALSE;
}

CObject* CGXGridCore::GetAdvancedUndoInfo()
{
	if (m_pUndoImp)
		return m_pUndoImp->GetAdvancedUndoInfo(this);

	return NULL;
}

BOOL CGXGridCore::UndoAdvanced(CObject* pAdvUndo, BOOL bCreateUndoInfo, BOOL bOnlyToolCells)
{
	if (m_pUndoImp)
		return m_pUndoImp->UndoAdvanced(this, pAdvUndo, bCreateUndoInfo, bOnlyToolCells);

	return FALSE;
}

void CGXGridCore::CleanAdvancedUndo()
{
	if (m_pUndoImp)
		m_pUndoImp->CleanAdvancedUndo(this);
}

BOOL CGXGridCore::ExecuteCommand(CGXCommand* pCmd, CGXGridCore* pGrid, GXCmdType ctType)
{
	GX_CHECKIMP(m_pUndoImp, "CGXGridCore::ExecuteCommand");

	if (m_pUndoImp)
		return m_pUndoImp->ExecuteCommand(pCmd, pGrid, ctType);

	return FALSE;
}

void CGXGridCore::AddCommand(CGXCommand* pCmd, GXCmdType ctCmd)
{
	GX_CHECKIMP(m_pUndoImp, "CGXGridCore::AddCommand");

	if (m_pUndoImp)
		m_pUndoImp->AddCommand(this, pCmd, ctCmd);
}

void CGXGridCore::RemoveCommand(GXCmdType ctCmd)
{
	GX_CHECKIMP(m_pUndoImp, "CGXGridCore::RemoveCommand");

	if (m_pUndoImp)
		m_pUndoImp->RemoveCommand(this, ctCmd);
}

BOOL CGXGridCore::Undo()
{
	GX_CHECKIMP(m_pUndoImp, "CGXGridCore::Undo");

	if (m_pUndoImp)
		return m_pUndoImp->Undo(this);

	return FALSE;
}

BOOL CGXGridCore::Redo()
{
	GX_CHECKIMP(m_pUndoImp, "CGXGridCore::Redo");

	if (m_pUndoImp)
		return m_pUndoImp->Redo(this);

	return FALSE;
}

void CGXGridCore::BeginTrans(LPCTSTR s)
{
	if (m_pUndoImp)
		m_pUndoImp->BeginTrans(this, s);
}

void CGXGridCore::CommitTrans()
{
	if (m_pUndoImp)
		m_pUndoImp->CommitTrans(this);
}

void CGXGridCore::Rollback()
{
	if (m_pUndoImp)
		m_pUndoImp->Rollback(this);
}

void CGXGridCore::OnCellUndone(ROWCOL nRow, ROWCOL nCol, BOOL bSetEdit)
{
	if (m_pUndoImp)
		m_pUndoImp->OnCellUndone(this, nRow, nCol, bSetEdit);
}

BOOL CGXGridCore::CanUndo(CCmdUI* pCmdUI)
{
	if (m_pUndoImp)
		return m_pUndoImp->CanUndo(this, pCmdUI);

	pCmdUI->Enable(FALSE);

	return FALSE;
}

BOOL CGXGridCore::CanRedo(CCmdUI* pCmdUI)
{
	if (m_pUndoImp)
		return m_pUndoImp->CanRedo(this, pCmdUI);

	pCmdUI->Enable(FALSE);

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// Sorting rows and columns - implementation in gxsort.cpp

// Rows 

void CGXGridCore::SortRows(CGXRange sortRange, CGXSortInfoArray& sortInfo, UINT flags)
{
	GX_CHECKIMP(m_pSortImp, "CGXGridCore::SortRows");

	if (m_pSortImp)
		m_pSortImp->SortRows(this, sortRange, sortInfo, flags);
}

BOOL CGXGridCore::MoveDataRows(const CGXRange& sortRange, const CRowColArray& awRowIndex, UINT flags, GXCmdType ctCmd)
{
	GX_CHECKIMP(m_pSortImp, "CGXGridCore::MoveDataRows");

	if (m_pSortImp)
		return m_pSortImp->MoveDataRows(this, sortRange, awRowIndex, flags, ctCmd);

	return FALSE;
}

BOOL CGXGridCore::StoreMoveDataRows(const CGXRange& sortRange, const CRowColArray& awRowIndex)
{
	GX_CHECKIMP(m_pSortImp, "CGXGridCore::StoreMoveDataRows");

	if (m_pSortImp)
		return m_pSortImp->StoreMoveDataRows(this, sortRange, awRowIndex);

	return FALSE;
}

void CGXGridCore::UpdateMoveDataRows(const CGXRange& sortRange, UINT flags, BOOL bCreateHint)
{
	GX_CHECKIMP(m_pSortImp, "CGXGridCore::UpdateMoveDataRows");

	if (m_pSortImp)
		m_pSortImp->UpdateMoveDataRows(this, sortRange, flags, bCreateHint);
}

void CGXGridCore::OnGetSortRowsKey(ROWCOL nRow, const CGXSortInfoArray& sortInfo, CStringArray& keys)
{
	GX_CHECKIMP(m_pSortImp, "CGXGridCore::OnGetSortRowsKey");

	if (m_pSortImp)
		m_pSortImp->OnGetSortRowsKey(this, nRow, sortInfo, keys);
}


// Columns

void CGXGridCore::SortCols(CGXRange sortRange, CGXSortInfoArray& sortInfo, UINT flags)
{
	GX_CHECKIMP(m_pSortImp, "CGXGridCore::SortCols");

	if (m_pSortImp)
		m_pSortImp->SortCols(this, sortRange, sortInfo, flags);
}

BOOL CGXGridCore::MoveDataCols(const CGXRange& sortRange, const CRowColArray& awColIndex, UINT flags, GXCmdType ctCmd)
{
	GX_CHECKIMP(m_pSortImp, "CGXGridCore::MoveDataCols");

	if (m_pSortImp)
		return m_pSortImp->MoveDataCols(this, sortRange, awColIndex, flags, ctCmd);

	return FALSE;
}

BOOL CGXGridCore::StoreMoveDataCols(const CGXRange& sortRange, const CRowColArray& awColIndex)
{
	GX_CHECKIMP(m_pSortImp, "CGXGridCore::StoreMoveDataCols");

	if (m_pSortImp)
		return m_pSortImp->StoreMoveDataCols(this, sortRange, awColIndex);

	return FALSE;
}

void CGXGridCore::UpdateMoveDataCols(const CGXRange& sortRange, UINT flags, BOOL bCreateHint)
{
	GX_CHECKIMP(m_pSortImp, "CGXGridCore::UpdateMoveDataCols");

	if (m_pSortImp)
		m_pSortImp->UpdateMoveDataCols(this, sortRange, flags, bCreateHint);
}

void CGXGridCore::OnGetSortColsKey(ROWCOL nCol, const CGXSortInfoArray& sortInfo, CStringArray& keys)
{
	GX_CHECKIMP(m_pSortImp, "CGXGridCore::OnGetSortColsKey");

	if (m_pSortImp)
		m_pSortImp->OnGetSortColsKey(this, nCol, sortInfo, keys);
}


// general overrides

void CGXGridCore::GetSortKey(CGXControl* pControl, ROWCOL nRow, ROWCOL nCol, const CGXSortInfo& sortInfo, const CGXStyle& style, CString& sKey)
{
	pControl->GetSortKey(nRow, nCol, sortInfo, style, sKey);
}

void CGXGridCore::SortKeys(const CGXSortInfoArray& sortInfo, CPtrArray& sortKeyArray)
{
	GX_CHECKIMP(m_pSortImp, "CGXGridCore::SortKeys");

	if (m_pSortImp)
		m_pSortImp->SortKeys(this, sortInfo, sortKeyArray);

}

// Ole Drag&Drop Data Source

#if _MFC_VER >= 0x0400 && !defined(_AFX_NO_OLE_SUPPORT)

BOOL CGXGridCore::OnDndCacheGlobalData(CGXNoOleDataSourceProxy* pSrcItem, const CGXRangeList& selList, ROWCOL& nDndRowExt, ROWCOL& nDndColExt)
{
	if (m_pOleDataSourceProxy)
		return m_pOleDataSourceProxy->OnDndCacheGlobalData(this, pSrcItem, selList, nDndRowExt, nDndColExt);

	return FALSE;
}

#endif

BOOL CGXGridCore::DndStartDragDrop(ROWCOL nRow, ROWCOL nCol)
{
#if _MFC_VER >= 0x0400 && !defined(_AFX_NO_OLE_SUPPORT)
	if (m_pOleDataSourceProxy)
		return m_pOleDataSourceProxy->DndStartDragDrop(this, nRow, nCol);
#else
	nRow, nCol;
#endif

	return FALSE;
}

// Ole Drag&Drop Target

// Grid implementation for drop target

#if _MFC_VER >= 0x0400 && !defined(_AFX_NO_OLE_SUPPORT)

DROPEFFECT CGXGridCore::OnGridDragScroll(DWORD dwKeyState, CPoint point)
{
	GX_CHECKIMP(m_pOleDropTargetImp, "CGXGridCore::OnGridDragScroll");

	if (m_pOleDropTargetImp)
		return m_pOleDropTargetImp->OnGridDragScroll(this, dwKeyState, point);

	return 0;
}

DROPEFFECT CGXGridCore::OnGridDragEnter(CGXNoOleDataObjectProxy* pDataObject,
	DWORD dwKeyState, CPoint point)
{
	GX_CHECKIMP(m_pOleDropTargetImp, "CGXGridCore::OnGridDragEnter");

	if (m_pOleDropTargetImp)
		return m_pOleDropTargetImp->OnGridDragEnter(this, pDataObject, dwKeyState, point);

	return 0;
}

DROPEFFECT CGXGridCore::OnGridDragOver(CGXNoOleDataObjectProxy* pDataObject,
	DWORD dwKeyState, CPoint point)
{
	GX_CHECKIMP(m_pOleDropTargetImp, "CGXGridCore::OnGridDragOver");

	if (m_pOleDropTargetImp)
		return m_pOleDropTargetImp->OnGridDragOver(this, pDataObject, dwKeyState, point);

	return 0;
}

DROPEFFECT CGXGridCore::OnGridDropEx(CGXNoOleDataObjectProxy* pDataObject,
	DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point)
{
	GX_CHECKIMP(m_pOleDropTargetImp, "CGXGridCore::OnGridDropEx");

	if (m_pOleDropTargetImp)
		return m_pOleDropTargetImp->OnGridDropEx(this, pDataObject, dropEffect, dropEffectList, point);

	return 0;
}

void CGXGridCore::OnGridDragLeave()
{
	GX_CHECKIMP(m_pOleDropTargetImp, "CGXGridCore::OnGridDragLeave");

	if (m_pOleDropTargetImp)
		m_pOleDropTargetImp->OnGridDragLeave(this);
}

BOOL CGXGridCore::OnGridDrop(CGXNoOleDataObjectProxy* pDataObject,
	DROPEFFECT dropEffect, CPoint point)
{
	GX_CHECKIMP(m_pOleDropTargetImp, "CGXGridCore::OnGridDrop");

	if (m_pOleDropTargetImp)
		return m_pOleDropTargetImp->OnGridDrop(this, pDataObject, dropEffect, point);

	return 0;
}

BOOL CGXGridCore::OnDndDropData(CGXNoOleDataObjectProxy* pDataObject, ROWCOL nRow, ROWCOL nCol)
{
	GX_CHECKIMP(m_pOleDropTargetImp, "CGXGridCore::OnDndDropData");

	if (m_pOleDropTargetImp)
		return m_pOleDropTargetImp->OnDndDropData(this, pDataObject, nRow, nCol);

	return 0;
}

BOOL CGXGridCore::OnDndQueryAcceptData(CGXNoOleDataObjectProxy* pDataObject)
{
	GX_CHECKIMP(m_pOleDropTargetImp, "CGXGridCore::OnDndQueryAcceptData");

	if (m_pOleDropTargetImp)
		return m_pOleDropTargetImp->OnDndQueryAcceptData(this, pDataObject);

	return 0;
}

BOOL CGXGridCore::OnDndCalculateFocusRect(CGXNoOleDataObjectProxy* pOleDataObjectProxy, ROWCOL& nRowExt, ROWCOL& nColExt)
{
	GX_CHECKIMP(m_pOleDropTargetImp, "CGXGridCore::OnDndCalculateFocusRect");

	if (m_pOleDropTargetImp)
		return m_pOleDropTargetImp->OnDndCalculateFocusRect(this, pOleDataObjectProxy, nRowExt, nColExt);

	return 0;
}


void CGXGridCore::DndDrawMoveRect(CPoint point, BOOL bEraseOld, BOOL bDrawNew)
{
	if (m_pOleDropTargetImp)
		m_pOleDropTargetImp->DndDrawMoveRect(this, point, bEraseOld, bDrawNew);
}

BOOL CGXGridCore::DoDragScroll(CPoint point)
{
	GX_CHECKIMP(m_pOleDropTargetImp, "CGXGridCore::DoDragScroll");

	if (m_pOleDropTargetImp)
		return m_pOleDropTargetImp->DoDragScroll(this, point);

	return 0;
}

BOOL CGXGridCore::IsOverAnotherApplication(POINT point)
{
	GX_CHECKIMP(m_pOleDropTargetImp, "CGXGridCore::IsOverAnotherApplication");

	if (m_pOleDropTargetImp)
		return m_pOleDropTargetImp->IsOverAnotherApplication(this, point);

	return 0;
}

#endif

/////////////////////////////////////////////////////////////////////////////
// Floating Cells

void CGXGridCore::SetFloatCellsMode(WORD nMode, BOOL bRedraw /*=TRUE*/)
{
	GX_CHECKIMP(m_pFloatCellsImp, "CGXGridCore::SetFloatCellsMode");

	if (m_pFloatCellsImp)
		m_pFloatCellsImp->SetFloatCellsMode(this, nMode, bRedraw);
}

WORD CGXGridCore::GetFloatCellsMode() const
{
	return m_pParam->m_wFloatCellsMode;
}

void CGXGridCore::DelayFloatCells(const CGXRange& range, ROWCOL nMaxCols)
{
	if (m_pFloatCellsImp)
		m_pFloatCellsImp->DelayFloatCells(this, range, nMaxCols);
}

CGXSpanCellPool* CGXGridCore::GetFloatCellsPool()
{
	if (IsPrinting())
		return m_pParam->m_pPrintFloatCells;
	else
		return m_pParam->m_pDispFloatCells;
}

BOOL CGXGridCore::EvalFloatCells(const CGXRange& range,
	BOOL bUpdate, //= TRUE
	CGXRange* prgBoundary, // = NULL
	BOOL bLookupStyle) //
{
	if (m_pFloatCellsImp)
		return m_pFloatCellsImp->EvalFloatCells(this, range, bUpdate, prgBoundary, bLookupStyle);

	return FALSE;
}

BOOL CGXGridCore::CanFloatCell(CGXControl* pControl, ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, BOOL bFloatOrFlood)
{
	if (m_pFloatCellsImp)
		return m_pFloatCellsImp->CanFloatCell(this, pControl, nRow, nCol, style, bFloatOrFlood);

	return FALSE;
}

BOOL CGXGridCore::SetFloatedCellsRowCol(ROWCOL nRow, ROWCOL nCol, ROWCOL nToRow, ROWCOL nToCol, UINT flags)
{
	GX_CHECKIMP(m_pFloatCellsImp, "CGXGridCore::SetFloatedCellsRowCol");

	if (m_pFloatCellsImp)
		return m_pFloatCellsImp->SetFloatedCellsRowCol(this, nRow, nCol, nToRow, nToCol, flags);

	return FALSE;
}

CGXRange* CGXGridCore::GetCellRangeRowCol(ROWCOL nRow, ROWCOL nCol, CGXRange& range)
{
	if (GetCoveredCellsRowCol(nRow, nCol, range))
		return &range;

	if (GetMergedCellsRowCol(nRow, nCol, range))
		return &range;

	if (GetFloatedCellsRowCol(nRow, nCol, range))
		return &range;

	return NULL;
}

CGXRange* CGXGridCore::GetFloatedCellsRowCol(ROWCOL nRow, ROWCOL nCol, CGXRange& range)
{
	if (m_pFloatCellsImp)
		return m_pFloatCellsImp->GetFloatedCellsRowCol(this, nRow, nCol, range);

	return NULL;
}

void CGXGridCore::MergeFloatedCells(CGXRange& area)
{
	if (m_pFloatCellsImp)
		m_pFloatCellsImp->MergeFloatedCells(this, area);
}

void CGXGridCore::UpdateFloatedCellsRowCol(ROWCOL nRow, ROWCOL nCol, ROWCOL nOldToRow, ROWCOL nOldToCol, UINT flags, BOOL bCreateHint)
{
	if (m_pFloatCellsImp)
		m_pFloatCellsImp->UpdateFloatedCellsRowCol(this, nRow, nCol, nOldToRow, nOldToCol, flags, bCreateHint);
}



/////////////////////////////////////////////////////////////////////////////
// Merge Cells

void CGXGridCore::SetMergeCellsMode(WORD nMode, BOOL bRedraw /*=TRUE*/)
{
	GX_CHECKIMP(m_pMergeCellsImp, "CGXGridCore::SetMergeCellsMode");

	if (m_pMergeCellsImp)
		m_pMergeCellsImp->SetMergeCellsMode(this, nMode, bRedraw);
}

WORD CGXGridCore::GetMergeCellsMode() const
{
	return m_pParam->m_wMergeCellsMode;
}

void CGXGridCore::DelayMergeCells(const CGXRange& range, ROWCOL nMaxRows, ROWCOL nMaxCols)
{
	if (m_pMergeCellsImp)
		m_pMergeCellsImp->DelayMergeCells(this, range, nMaxRows, nMaxCols);
}

BOOL CGXGridCore::EvalMergeCells(const CGXRange& range,
	BOOL bUpdate, //= TRUE
	CGXRange* prgBoundary, // = NULL
	BOOL bLookupStyle) //
{
	if (m_pMergeCellsImp)
		return m_pMergeCellsImp->EvalMergeCells(this, range, bUpdate, prgBoundary, bLookupStyle);

	return FALSE;
}

BOOL CGXGridCore::CanMergeCell(CGXControl* pControl, ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, int mtType)
{
	if (m_pMergeCellsImp)
		return m_pMergeCellsImp->CanMergeCell(this, pControl, nRow, nCol, style, mtType);

	return FALSE;
}

BOOL CGXGridCore::CanMergeCells(const CGXStyle& style1, const CGXStyle& style2)
{
	if (m_pMergeCellsImp)
		return m_pMergeCellsImp->CanMergeCells(this, style1, style2);

	return FALSE;
}

BOOL CGXGridCore::SetMergedCellsRowCol(int mtType, ROWCOL nRow, ROWCOL nCol, ROWCOL nToRow, ROWCOL nToCol, UINT flags /*= GX_UPDATENOW*/)
{
	GX_CHECKIMP(m_pMergeCellsImp, "CGXGridCore::SetMergedCellsRowCol");

	if (m_pMergeCellsImp)
		return m_pMergeCellsImp->SetMergedCellsRowCol(this, mtType, nRow, nCol, nToRow, nToCol, flags);

	return FALSE;
}

CGXRange* CGXGridCore::GetMergedCellsRowCol(ROWCOL nRow, ROWCOL nCol, CGXRange& range, int mtType)
{
	if (m_pMergeCellsImp)
		return m_pMergeCellsImp->GetMergedCellsRowCol(this, nRow, nCol, range, mtType);

	return FALSE;
}

void CGXGridCore::MergeMergedCells(CGXRange& area)
{
	if (m_pMergeCellsImp)
		m_pMergeCellsImp->MergeMergedCells(this, area);
}

void CGXGridCore::UpdateMergedCellsRowCol(ROWCOL nRow, ROWCOL nCol, ROWCOL nOldToRow, ROWCOL nOldToCol, UINT flags, BOOL bCreateHint)
{
	if (m_pMergeCellsImp)
		m_pMergeCellsImp->UpdateMergedCellsRowCol(this, nRow, nCol, nOldToRow, nOldToCol, flags, bCreateHint);
}


/////////////////////////////////////////////////////////////////////////////
// Covered Cells

BOOL CGXGridCore::SetCoveredCellsRowCol(ROWCOL nRow, ROWCOL nCol, ROWCOL nToRow, ROWCOL nToCol, UINT flags /*= GX_UPDATENOW*/, GXCmdType ctCmd /*=gxDo*/)
{
	GX_CHECKIMP(m_pCoveredCellsImp, "CGXGridCore::SetCoveredCellsRowCol");

	if (m_pCoveredCellsImp)
		return m_pCoveredCellsImp->SetCoveredCellsRowCol(this, nRow, nCol, nToRow, nToCol, flags, ctCmd);

	return FALSE;
}

CGXRange* CGXGridCore::GetCoveredCellsRowCol(ROWCOL nRow, ROWCOL nCol, CGXRange& range)
{
	if (m_pCoveredCellsImp)
		return m_pCoveredCellsImp->GetCoveredCellsRowCol(this, nRow, nCol, range);

	return NULL;
}

void CGXGridCore::MergeCoveredCells(CGXRange& area)
{
	if (m_pCoveredCellsImp)
		m_pCoveredCellsImp->MergeCoveredCells(this, area);
}

BOOL CGXGridCore::StoreCoveredCellsRowCol(ROWCOL nRow, ROWCOL nCol, ROWCOL nToRow, ROWCOL nToCol, BOOL bSet)
{
	if (m_pCoveredCellsImp)
		return m_pCoveredCellsImp->StoreCoveredCellsRowCol(this, nRow, nCol, nToRow, nToCol, bSet);

	return FALSE;
}

void CGXGridCore::UpdateCoveredCellsRowCol(ROWCOL nRow, ROWCOL nCol, ROWCOL nOldToRow, ROWCOL nOldToCol, UINT flags, BOOL bCreateHint)
{
	if (m_pCoveredCellsImp)
		m_pCoveredCellsImp->UpdateCoveredCellsRowCol(this, nRow, nCol, nOldToRow, nOldToCol, flags, bCreateHint);
}

/////////////////////////////////////////////////////////////////////////////
// Printing

void CGXGridCore::OnGridBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	GX_CHECKIMP(m_pPrintImp, "CGXGridCore::OnGridBeginPrinting");

	if (m_pPrintImp)
		m_pPrintImp->OnGridBeginPrinting(this, pDC, pInfo);
}

void CGXGridCore::OnGridPrint(CDC* pDC, CPrintInfo* pInfo)
{
	GX_CHECKIMP(m_pPrintImp, "CGXGridCore::OnGridPrint");

	if (m_pPrintImp)
		m_pPrintImp->OnGridPrint(this, pDC, pInfo);
}

void CGXGridCore::OnGridEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	GX_CHECKIMP(m_pPrintImp, "CGXGridCore::OnGridEndPrinting");

	if (m_pPrintImp)
		m_pPrintImp->OnGridEndPrinting(this, pDC, pInfo);
}

/////////////////////////////////////////////////////////////////////////////
// Resize to fit

BOOL CGXGridCore::ResizeRowHeightsToFit(CGXRange range, BOOL bResizeCoveredCells /*= TRUE*/, UINT nFlags /*= GX_UPDATENOW*/)
{
	GX_CHECKIMP(m_pResizeToFitImp, "CGXGridCore::ResizeRowHeightsToFit");

	if (m_pResizeToFitImp)
		return m_pResizeToFitImp->ResizeRowHeightsToFit(this, range, bResizeCoveredCells, nFlags);

	return FALSE;
}

BOOL CGXGridCore::ResizeColWidthsToFit(CGXRange range, BOOL bResizeCoveredCells /*= TRUE*/, UINT nFlags /*= GX_UPDATENOW*/)
{
	GX_CHECKIMP(m_pResizeToFitImp, "CGXGridCore::ResizeColWidthsToFit");

	if (m_pResizeToFitImp)
		return m_pResizeToFitImp->ResizeColWidthsToFit(this, range, bResizeCoveredCells, nFlags);

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Hide Cells

BOOL CGXGridCore::HideRows(ROWCOL nFromRow, ROWCOL nToRow, BOOL bHide, BOOL* abHideArray, UINT flags, GXCmdType ctCmd)
{
	GX_CHECKIMP(m_pHideCellsImp, "CGXGridCore::HideRows");

	if (m_pHideCellsImp)
		return m_pHideCellsImp->HideRows(this, nFromRow, nToRow, bHide, abHideArray, flags, ctCmd);

	return FALSE;
}

BOOL CGXGridCore::IsRowHidden(ROWCOL nRow)
{
	if (m_pHideCellsImp)
		return m_pHideCellsImp->IsRowHidden(this, nRow);

	return FALSE;
}

BOOL CGXGridCore::StoreHideRow(ROWCOL nRow, BOOL bHide)
{
	if (m_pHideCellsImp)
		return m_pHideCellsImp->StoreHideRow(this, nRow, bHide);

	return FALSE;
}

BOOL CGXGridCore::HideCols(ROWCOL nFromCol, ROWCOL nToCol, BOOL bHide, BOOL* abHideArray, UINT flags, GXCmdType ctCmd)
{
	GX_CHECKIMP(m_pHideCellsImp, "CGXGridCore::HideCols");

	if (m_pHideCellsImp)
		return m_pHideCellsImp->HideCols(this, nFromCol, nToCol, bHide, abHideArray, flags, ctCmd);

	return FALSE;
}

BOOL CGXGridCore::IsColHidden(ROWCOL nCol)
{
	if (m_pHideCellsImp)
		return m_pHideCellsImp->IsColHidden(this, nCol);

	return FALSE;
}

BOOL CGXGridCore::StoreHideCol(ROWCOL nCol, BOOL bHide)
{
	if (m_pHideCellsImp)
		return m_pHideCellsImp->StoreHideCol(this, nCol, bHide);

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// Direct copying and moving of cells

BOOL CGXGridCore::CopyCells(CGXRange rg, ROWCOL nRow, ROWCOL nCol, UINT flags /*= GX_UPDATENOW*/)
{
	GX_CHECKIMP(m_pDirectCopyMoveCellsImp, "CGXGridCore::CopyCells");

	if (m_pDirectCopyMoveCellsImp)
		return m_pDirectCopyMoveCellsImp->CopyCells(this, rg, nRow, nCol, flags);

	return FALSE;
}

BOOL CGXGridCore::StoreCopyCells(CGXRange rg, ROWCOL nRow, ROWCOL nCol)
{
	GX_CHECKIMP(m_pDirectCopyMoveCellsImp, "CGXGridCore::StoreCopyCells");

	if (m_pDirectCopyMoveCellsImp)
		return m_pDirectCopyMoveCellsImp->StoreCopyCells(this, rg, nRow, nCol);

	return FALSE;
}

BOOL CGXGridCore::MoveCells(CGXRange rg, ROWCOL nRow, ROWCOL nCol, CObject* pUndoInfo /*=NULL*/, UINT flags /*= GX_UPDATENOW*/, GXCmdType ctCmd /*= gxDo*/)
{
	GX_CHECKIMP(m_pDirectCopyMoveCellsImp, "CGXGridCore::MoveCells");

	if (m_pDirectCopyMoveCellsImp)
		return m_pDirectCopyMoveCellsImp->MoveCells(this, rg, nRow, nCol, pUndoInfo, flags, ctCmd);

	return FALSE;
}

BOOL CGXGridCore::StoreMoveCells(CGXRange rg, ROWCOL nRow, ROWCOL nCol, CObject*& pUndoInfo, BOOL bCreateUndo, GXCmdType ctCmd)
{
	GX_CHECKIMP(m_pDirectCopyMoveCellsImp, "CGXGridCore::StoreMoveCells");

	if (m_pDirectCopyMoveCellsImp)
		return m_pDirectCopyMoveCellsImp->StoreMoveCells(this, rg, nRow, nCol, pUndoInfo, bCreateUndo, ctCmd);

	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////
// CSV Import/Export 
BOOL CGXGridCore::CopyTextToFile(CFile& destFile, const CGXRange& range)
{
	CGXRangeList selList;
	selList.AddTail(new CGXRange(range));
	return CopyTextToFile(destFile, selList);
}
BOOL CGXGridCore::CopyTextToFile(CFile& destFile, const CGXRangeList& selList)
{
	GX_CHECKIMP(m_pTextDataExchangeImp, "CGXGridCore::CopyTextToFile");
	if (m_pTextDataExchangeImp)return m_pTextDataExchangeImp->CopyTextToFile(this, destFile, selList);
	return FALSE;
}
CString CGXGridCore::GetCopyTextRowCol(ROWCOL nRow, ROWCOL nCol)
{
	GX_CHECKIMP(m_pTextDataExchangeImp, "CGXGridCore::GetCopyTextRowCol");
	if (m_pTextDataExchangeImp)return m_pTextDataExchangeImp->GetCopyTextRowCol(this, nRow, nCol);
	return _T("");
}
// paste unformatted text
BOOL CGXGridCore::PasteTextFromBuffer(LPCTSTR psz, DWORD size, const CGXRange& range)
{
	GX_CHECKIMP(m_pTextDataExchangeImp, "CGXGridCore::PasteTextFromBuffer");

	if (m_pTextDataExchangeImp)
		return m_pTextDataExchangeImp->PasteTextFromBuffer(this, psz, size, range);

	return FALSE;
}

BOOL CGXGridCore::PasteTextRowCol(ROWCOL nRow, ROWCOL nCol, const CString& str, UINT nFlags, const CGXStyle* pOldStyle)
{
	GX_CHECKIMP(m_pTextDataExchangeImp, "CGXGridCore::PasteTextRowCol");

	if (m_pTextDataExchangeImp)
		return m_pTextDataExchangeImp->PasteTextRowCol(this, nRow, nCol, str, nFlags, pOldStyle);

	return FALSE;
}

BOOL CGXGridCore::CalcBufferDimension(LPCTSTR psz, DWORD size, ROWCOL& nRows, ROWCOL& nCols)
{
	GX_CHECKIMP(m_pTextDataExchangeImp, "CGXGridCore::CalcBufferDimension");

	if (m_pTextDataExchangeImp)
		return m_pTextDataExchangeImp->CalcBufferDimension(this, psz, size, nRows, nCols);

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// Style Import/Export 

BOOL CGXGridCore::CopyCellsToArchive(CArchive& ar, const CGXRange& range, BOOL bLoadBaseStyles)
{
	CGXRangeList selList;
	selList.AddTail(new CGXRange(range));
	return CopyCellsToArchive(ar, selList, bLoadBaseStyles);
}

BOOL CGXGridCore::CopyCellsToArchive(CArchive& ar, const CGXRangeList& selList, BOOL bLoadBaseStyles)
{
	GX_CHECKIMP(m_pStyleDataExchangeImp, "CGXGridCore::CopyCellsToArchive");

	if (m_pStyleDataExchangeImp)
		return m_pStyleDataExchangeImp->CopyCellsToArchive(this, ar, selList, bLoadBaseStyles);

	return FALSE;
}

BOOL CGXGridCore::GetClipboardStyleRowCol(ROWCOL nRow, ROWCOL nCol, CGXStyle* pStyle, BOOL bLoadBaseStyles)
{
	GX_CHECKIMP(m_pStyleDataExchangeImp, "CGXGridCore::GetClipboardStyleRowCol");

	if (m_pStyleDataExchangeImp)
		return m_pStyleDataExchangeImp->GetClipboardStyleRowCol(this, nRow, nCol, pStyle, bLoadBaseStyles);

	return FALSE;
}

// paste style information

BOOL CGXGridCore::PasteCellsFromArchive(CArchive& ar, const CGXRange& range, BOOL bIgnoreDiffRange)
{
	GX_CHECKIMP(m_pStyleDataExchangeImp, "CGXGridCore::PasteCellsFromArchive");

	if (m_pStyleDataExchangeImp)
		return m_pStyleDataExchangeImp->PasteCellsFromArchive(this, ar, range, bIgnoreDiffRange);

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// Clipboard

#if _MFC_VER >= 0x0400 && !defined(_AFX_NO_OLE_SUPPORT)
ROWCOL CGXGridCore::m_nDndRowsCopied = 0;
ROWCOL CGXGridCore::m_nDndColsCopied = 0;
BOOL CGXGridCore::m_bDndGridSource = FALSE;
#endif


BOOL CGXGridCore::CanCopy()
{
	if (m_pCutPasteImp)
		return m_pCutPasteImp->CanCopy(this);

	return FALSE;
}

BOOL CGXGridCore::Copy()
{
	if (m_pCutPasteImp)
		return m_pCutPasteImp->Copy(this);

	return FALSE;
}

// Copy range of cells without having to select it.

BOOL CGXGridCore::CopyRange(const CGXRange& range)
{
	CGXRangeList selList;
	selList.AddTail(new CGXRange(range));
	return CopyRange(selList);
}

BOOL CGXGridCore::CopyRange(const CGXRangeList& selList)
{
	GX_CHECKIMP(m_pCutPasteImp, "CGXGridCore::CopyRange");

	if (m_pCutPasteImp)
		return m_pCutPasteImp->CopyRange(this, selList);

	return FALSE;
}

// copy unformatted text to clipboard

BOOL CGXGridCore::CopyTextToClipboard(const CGXRangeList& selList)
{
	GX_CHECKIMP(m_pCutPasteImp, "CGXGridCore::CopyTextToClipboard");

	if (m_pCutPasteImp)
		return m_pCutPasteImp->CopyTextToClipboard(this, selList);

	return FALSE;
}

// copy style information to clipboard

BOOL CGXGridCore::CopyCellsToClipboard(const CGXRangeList& selList, BOOL bLoadBaseStyles)
{
	GX_CHECKIMP(m_pCutPasteImp, "CGXGridCore::CopyCellsToClipboard");

	if (m_pCutPasteImp)
		return m_pCutPasteImp->CopyCellsToClipboard(this, selList, bLoadBaseStyles);

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Cut cells helpers

// Edit|Cut

BOOL CGXGridCore::CanCut()
{
	if (m_pCutPasteImp)
		return m_pCutPasteImp->CanCut(this);

	return FALSE;
}

BOOL CGXGridCore::Cut()
{
	if (m_pCutPasteImp)
		return m_pCutPasteImp->Cut(this);

	return FALSE;
}

BOOL CGXGridCore::CutRange(const CGXRange& range, BOOL bStyleOrValue)
{
	CGXRangeList selList;
	selList.AddTail(new CGXRange(range));
	return CutRange(selList, bStyleOrValue);
}

BOOL CGXGridCore::CutRange(const CGXRangeList& selList, BOOL bStyleOrValue)
{
	if (m_pCutPasteImp)
		return m_pCutPasteImp->CutRange(this, selList, bStyleOrValue);

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Paste from clipboard helpers

// Edit|Paste

BOOL CGXGridCore::CanPaste()
{
	if (m_pCutPasteImp)
		return m_pCutPasteImp->CanPaste(this);

	return FALSE;
}

BOOL CGXGridCore::OnCheckClipboardFormat()
{
	GX_CHECKIMP(m_pCutPasteImp, "CGXGridCore::OnCheckClipboardFormat");

	if (m_pCutPasteImp)
		return m_pCutPasteImp->OnCheckClipboardFormat(this);

	return FALSE;
}

BOOL CGXGridCore::Paste()
{
	if (m_pCutPasteImp)
		return m_pCutPasteImp->Paste(this);

	return FALSE;
}

BOOL CGXGridCore::OnPasteDirect(const CGXRange& range)
{
	GX_CHECKIMP(m_pCutPasteImp, "CGXGridCore::OnPasteDirect");

	if (m_pCutPasteImp)
		return m_pCutPasteImp->OnPasteDirect(this, range);

	return FALSE;
}

BOOL CGXGridCore::OnPasteFromClipboard(const CGXRange& rg)
{
	GX_CHECKIMP(m_pCutPasteImp, "CGXGridCore::OnPasteFromClipboard");

	if (m_pCutPasteImp)
		return m_pCutPasteImp->OnPasteFromClipboard(this, rg);

	return FALSE;
}

BOOL CGXGridCore::OnPasteDiffRange()
{
	if (m_pCutPasteImp)
		return m_pCutPasteImp->OnPasteDiffRange(this);

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Clear Cells

BOOL CGXGridCore::CanClear()
{
	if (m_pClearCellsImp)
		return m_pClearCellsImp->CanClear(this);

	return FALSE;
}

BOOL CGXGridCore::Clear(BOOL bStyleOrValue /* = TRUE */)
{
	GX_CHECKIMP(m_pClearCellsImp, "CGXGridCore::Clear");

	if (m_pClearCellsImp)
		return m_pClearCellsImp->Clear(this, bStyleOrValue);

	return FALSE;
}

BOOL CGXGridCore::ClearCells(const CGXRange& range, BOOL bStyleOrValue)
{
	CGXRangeList selList;
	selList.AddTail(new CGXRange(range));
	return ClearCells(selList, bStyleOrValue);
}

BOOL CGXGridCore::ClearCells(const CGXRangeList& selList, BOOL bStyleOrValue)
{
	GX_CHECKIMP(m_pClearCellsImp, "CGXGridCore::ClearCells");

	if (m_pClearCellsImp)
		return m_pClearCellsImp->ClearCells(this, selList, bStyleOrValue);

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Range Names

BOOL CGXGridCore::SetRangeName(LPCTSTR name, const CGXRange& rg, CObject* pUndoInfo, UINT flags, GXCmdType ctCmd)
{
	GX_CHECKIMP(m_pRangeNameImp, "CGXGridCore::SetRangeName");

	if (m_pRangeNameImp)
		return m_pRangeNameImp->SetRangeName(this, name, rg, pUndoInfo, flags, ctCmd);

	return FALSE;
}

BOOL CGXGridCore::GetRangeName(LPCTSTR name, CGXRange& rg)
{
	GX_CHECKIMP(m_pRangeNameImp, "CGXGridCore::GetRangeName");

	if (m_pRangeNameImp)
		return m_pRangeNameImp->GetRangeName(this, name, rg);

	return FALSE;
}

BOOL CGXGridCore::DeleteRangeName(LPCTSTR name, CObject* pUndoInfo, GXCmdType ctCmd)
{
	GX_CHECKIMP(m_pRangeNameImp, "CGXGridCore::DeleteRangeName");

	if (m_pRangeNameImp)
		return m_pRangeNameImp->DeleteRangeName(this, name, pUndoInfo, ctCmd);

	return FALSE;
}

