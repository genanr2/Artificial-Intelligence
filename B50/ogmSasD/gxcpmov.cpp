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

// gxcpmov.cpp : CGGXGridCore support for direct copy/move cells
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

#ifndef _AFXPRIV_H_
#include "afxpriv.h"
#endif

#ifndef _GXCORE_H_
#include "gxcore.h"
#endif

#ifndef _GXCTRL_H_
#include "gxctrl.h"
#endif

#ifndef _GXDELAYR_H_
#include "gxdelayr.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCORCLP")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

class CGXGridDirectCopyMoveCellsImp: public CGXAbstractGridDirectCopyMoveCellsImp
{
public:
	virtual BOOL CopyCells(CGXGridCore* pGrid, CGXRange& rg, ROWCOL nRow, ROWCOL nCol, UINT flags);
	virtual BOOL StoreCopyCells(CGXGridCore* pGrid, CGXRange& rg, ROWCOL nRow, ROWCOL nCol);
	virtual BOOL MoveCells(CGXGridCore* pGrid, CGXRange& rg, ROWCOL nRow, ROWCOL nCol, CObject* pUndoInfo /*=NULL*/, UINT flags /*= GX_UPDATENOW*/, GXCmdType ctCmd /*= gxDo*/);
	virtual BOOL StoreMoveCells(CGXGridCore* pGrid, CGXRange& rg, ROWCOL nRow, ROWCOL nCol, CObject*& pUndoInfo, BOOL bCreateUndo, GXCmdType ctCmd);
};

void CGXGridCore::ImplementDirectCopyMoveCells()
{
	if (m_pDirectCopyMoveCellsImp == NULL)
		AutoDeletePtr(m_pDirectCopyMoveCellsImp = new CGXGridDirectCopyMoveCellsImp);

	CGXData::ImplementCopyMoveCells();
}

/////////////////////////////////////////////////////////////////////////////
// Copying and moving cells directly in sheet
// (see also GetParam()->m_bDirectCopyPaste)

// CopyCells will never be called for undo purposes - therefore
// it has no GXCmdType parameter. Undoing a CopyCells operation
// is done through CGXSetStyleRangeCmd

BOOL CGXGridDirectCopyMoveCellsImp::CopyCells(CGXGridCore* pGrid, CGXRange& rg, ROWCOL nRow, ROWCOL nCol, UINT flags)
{
	if (pGrid->IsReadOnly())
		return FALSE;

	pGrid->BeginTrans(pGrid->m_pAppData->strmDragDropCopy);

	// start op, generate undo info
	CGXLongOperation theOp;

	theOp.SetStatusText(pGrid->m_pAppData->strmDragDropCopy, FALSE);

	theOp.SetCanRollback(TRUE);

	CObArray* pOldCellsArray = NULL;    // will be filled with style setting

	CGXRange range(nRow, nCol, nRow+rg.GetHeight()-1, nCol+rg.GetWidth()-1);

	DWORD dwSize = range.GetWidth()*range.GetHeight();

	// generate undo info
	if (pGrid->m_pParam->m_bUndoEnabled && pGrid->m_pUndoImp)
	{
		// Uurgh ... too much!
		if (dwSize >= INT_MAX || range.GetWidth() > SHRT_MAX || range.GetHeight() > SHRT_MAX)
		{
			if (AfxMessageBox(GX_IDS_CANNOTUNDO, MB_OKCANCEL) == IDCANCEL)
				return FALSE;
		}
		else
		{
			// Get style info for cells
			TRY
			{
				pOldCellsArray = pGrid->GetStyleArray(range, pGrid->m_nExpressionValueType, &theOp);
			}
			CATCH(CMemoryException, e)
			{
				// shall I continue?
				if (AfxMessageBox(GX_IDS_CANNOTUNDO, MB_OKCANCEL) == IDCANCEL)
					return FALSE;
			}
			AND_CATCH(CUserException, e)
			{
				// user aborted, clean up
				return FALSE;
			}
			END_CATCH
		}
	}

	pGrid->FreeObjectCache();

	pGrid->AddRecalcRange(range);

	// Let engine or CGXData copy cells in memory (and also
	// create undo info for tools and matrix cells)
	BOOL bAdvUndo = pGrid->SetAdvancedUndo(pGrid->m_pParam->m_bUndoEnabled);
	BOOL bDone = pGrid->StoreCopyCells(rg, nRow, nCol);
	pGrid->SetAdvancedUndo(bAdvUndo);

	if (bDone)
	{
		pGrid->CommitTrans();

		// Delay float cell evaluation of this range
		pGrid->DelayFloatCells(range);
		pGrid->DelayMergeCells(range);

		pGrid->SetModifiedFlag();

		// Latest member in pOldCellsArray will be assigned advanced undo info 
		// that we need to initialize here. 
		CObject* pAdvUndo = pGrid->GetAdvancedUndoInfo();

		if (pAdvUndo)
			pOldCellsArray->SetAtGrow((int) dwSize, pAdvUndo);

		if (pOldCellsArray)
			pGrid->AddCommand(pGrid->m_pUndoImp->CreateSetStyleRangeCmd(range, pGrid->m_nExpressionValueType, pOldCellsArray), gxDo);
	}
	else if (pOldCellsArray)
	{
		// nothing changed, free undo information
		for (int i = 0; i < pOldCellsArray->GetSize(); i++)
			delete pOldCellsArray->GetAt(i);

		delete pOldCellsArray;

		pGrid->CleanAdvancedUndo();

		pGrid->Rollback();
	}
	else
	{
		pGrid->CleanAdvancedUndo();

		pGrid->Rollback();
	}

#if defined(_DETECTFALSE) && defined(_DEBUG)
	if (!bDone)
	{
		TRACE0("Warning: CopyCells() returned FALSE.\n");
		TRACE(_T("range: (%lu,%lu,%lu,%lu)\n"), range.top, range.left, range.bottom, range.right);
	}
#endif

	// Calling Recalc makes sure that also cells that depended
	// on cells in the destintation area get updated. The expression
	// and the float/merge cell state will be recalculated 
	pGrid->Recalc();
	pGrid->Redraw(flags);

	return bDone;
}

BOOL CGXGridDirectCopyMoveCellsImp::StoreCopyCells(CGXGridCore* pGrid, CGXRange& rg, ROWCOL nRow, ROWCOL nCol)
{
	// NOTE: If you need a more SetStyleRange-like behavior (CGXControl::StoreStyle
	// called for each cell) override this method and use SetStyleRange as example.

	// In 99% it will fine if the engine (either CGXData or CGXFormulaSheet)
	// does the copy which is several times faster than SetStyleRange

	CGXData* pData = pGrid->GetParam()->GetData();

	if (pData == NULL)
		return TRUE;

	return pData->StoreCopyCells(rg, nRow, nCol);
}

BOOL CGXGridDirectCopyMoveCellsImp::MoveCells(CGXGridCore* pGrid, CGXRange& rg, ROWCOL nRow, ROWCOL nCol, CObject* pUndoInfo /*=NULL*/, UINT flags /*= GX_UPDATENOW*/, GXCmdType ctCmd /*= gxDo*/)
{
	if (pGrid->IsReadOnly())
		return FALSE;

	CGXData* pData = pGrid->GetParam()->GetData();

	if (pData == NULL)
		return FALSE;

	CGXLongOperation theOp;
	theOp.SetStatusText(pGrid->m_pAppData->strmDragDropMove);

	CGXRange range(nRow, nCol, nRow+rg.GetHeight()-1, nCol+rg.GetWidth()-1);

	// bulk add range so that StoreStyleRowCol does not 
	// call AddRecalcRange for each cell.
	if (!pGrid->m_bLockAddRecalcRange)
	{
		pGrid->AddRecalcRange(range);
		pGrid->AddRecalcRange(rg);
	}

	// Restore old cells and cell references if this is an undo
	// and let engine or CGXData move cells in memory
	BOOL bCreateUndo = pGrid->m_pUndoImp && pGrid->m_pParam->m_bUndoEnabled && ctCmd != gxRollback;

	// StoreMoveCells will allocate pUndoInfo or reuse
	// existing object. CGXMoveCellsCmd will delete pUndoInfo
	BOOL bDone = pGrid->StoreMoveCells(rg, nRow, nCol, pUndoInfo, bCreateUndo, ctCmd);

	if (bDone)
	{
		// Delay float cell evaluation of this range
		pGrid->DelayFloatCells(range);
		pGrid->DelayMergeCells(range);
		pGrid->DelayFloatCells(rg);
		pGrid->DelayMergeCells(rg);

		pGrid->SetModifiedFlag();

		if (ctCmd != gxRollback)
			pGrid->AddCommand(pGrid->m_pUndoImp->CreateMoveCellsCmd(rg, nRow, nCol, pUndoInfo), ctCmd);
	}

	// Calling Recalc makes sure that also cells that depended
	// on cells in the destintation area get updated. The expression
	// and the float/merge cell state will be recalculated 
	pGrid->Recalc();
	pGrid->Redraw(flags);

	return TRUE;
}

BOOL CGXGridDirectCopyMoveCellsImp::StoreMoveCells(CGXGridCore* pGrid, CGXRange& rg, ROWCOL nRow, ROWCOL nCol, CObject*& pUndoInfo, BOOL bCreateUndo, GXCmdType ctCmd)
{
	// NOTE: If you need a more SetStyleRange-like behavior (CGXControl::StoreStyle
	// called for each cell) override this method and use SetStyleRange as example.

	// In 99% it will fine if the engine (either CGXData or CGXFormulaSheet)
	// does the copy which is several times faster than SetStyleRange

	CGXData* pData = pGrid->GetParam()->GetData();

	if (pData == NULL)
		return TRUE;

	return pData->StoreMoveCells(rg, nRow, nCol, pUndoInfo, bCreateUndo, ctCmd);
}

