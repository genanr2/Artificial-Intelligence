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

// gxcvrcel.cpp: implementation of covered cells

#include "stdafx.h"

// Headers

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
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

#ifndef _GXDELAYR_H_
#include "gxdelayr.h"
#endif

#ifndef _GXCVRCEL_H_
#include "gxcvrcel.h"
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXSPAN")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

class CGXCoveredCellPoolImp: public CGXAbstractCoveredCellPoolImp
{
public:
	virtual CGXCoveredCellPool* CreateCoveredCellPool();
	virtual void InitFromRangeList(CGXCoveredCellPool*, CGXRangeList&);
};

/////////////////////////////////////////////////////////////////////////////
// Covered Cells

class CGXGridCoveredCellsImp: public CGXAbstractGridCoveredCellsImp
{
public:
	virtual BOOL SetCoveredCellsRowCol(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, ROWCOL nToRow, ROWCOL nToCol, UINT flags, GXCmdType ctCmd);
	virtual CGXRange* GetCoveredCellsRowCol(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, CGXRange& range);
	virtual BOOL StoreCoveredCellsRowCol(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, ROWCOL nToRow, ROWCOL nToCol, BOOL bSet);
	virtual void UpdateCoveredCellsRowCol(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, ROWCOL nOldToRow, ROWCOL nOldToCol, UINT flags, BOOL bCreateHint);
	virtual void MergeCoveredCells(CGXGridCore* pGrid, CGXRange& range);
	virtual BOOL RemoveRows(CGXGridCore* pGrid, ROWCOL nFromRow, ROWCOL nToRow, UINT& flags, GXCmdType ctCmd);
	virtual BOOL RemoveCols(CGXGridCore* pGrid, ROWCOL nFromCol, ROWCOL nToCol, UINT& flags, GXCmdType ctCmd);
};

IMPLEMENT_DYNCREATE(CGXCoveredCellPool, CObject)

// Debug Info

#define new DEBUG_NEW

void AFXAPI CGXCoveredCellPool::ImplementCoveredCellPool()
{
	if (GXGetAppData()->m_pCoveredCellPoolImp == NULL)
		GXGetAppData()->AutoDeletePtr(GXGetAppData()->m_pCoveredCellPoolImp = new CGXCoveredCellPoolImp);
}

CGXCoveredCellPool* CGXCoveredCellPoolImp::CreateCoveredCellPool()
{
	return new CGXCoveredCellPool;
}

void CGXCoveredCellPoolImp::InitFromRangeList(CGXCoveredCellPool* pPool, CGXRangeList& rangelist)
{
	pPool->InitFromRangeList(&rangelist);
}


void CGXGridCore::ImplementCoveredCells()
{
	if (m_pCoveredCellsImp == NULL)
		AutoDeletePtr(m_pCoveredCellsImp = new CGXGridCoveredCellsImp);

	CGXCoveredCellPool::ImplementCoveredCellPool();

	CGXGridParam* pParam = GetParam();
	if (pParam)
		pParam->InitCoveredCells();

	ImplementDrawSpannedCells();
}

/////////////////////////////////////////////////////////////////////////////
// Covered Cells

BOOL CGXGridCoveredCellsImp::SetCoveredCellsRowCol(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, ROWCOL nToRow, ROWCOL nToCol, UINT flags /*= GX_UPDATENOW*/, GXCmdType ctCmd /*=gxDo*/)
{
	CGXGridParam* pParam = pGrid->GetParam();

	ASSERT(nToRow >= nRow && nToCol >= nCol);

	ROWCOL nOldToRow = nRow,
		   nOldToCol = nCol;

	CGXRange rgCovered;
	if (pGrid->GetCoveredCellsRowCol(nRow, nCol, rgCovered))
	{
		nOldToRow = rgCovered.bottom;
		nOldToCol = rgCovered.right;
	}

	BOOL bSet = (nToRow != nRow || nToCol != nCol);

	// Command
	if (pGrid->StoreCoveredCellsRowCol(nRow, nCol, nToRow, nToCol, bSet))
	{
		CGXRange rg(nRow, nCol, max(nOldToRow, nToRow), max(nOldToCol, nToCol));

		pGrid->DelayFloatCells(rg);
		pGrid->DelayMergeCells(rg);

		CGXRange rgBoundary;

		if (pGrid->EvalMergeCells(rg, FALSE, &rgBoundary))
			rg.UnionRange(rg, rgBoundary);

		if (pGrid->EvalFloatCells(rg, FALSE, &rgBoundary))
			rg.UnionRange(rg, rgBoundary);

		pGrid->UpdateCoveredCellsRowCol(rg.top, rg.left, rg.bottom, rg.right, flags, TRUE);

		if (pGrid->m_pUndoImp && ctCmd != gxRollback && pParam->m_bUndoEnabled)
		{
			pGrid->AddCommand(pGrid->m_pUndoImp->CreateSetCoveredCellsCmd(nRow, nCol, nOldToRow, nOldToCol), ctCmd);

			if (ctCmd == gxUndo || ctCmd == gxRedo)
				pGrid->OnCellUndone(nRow, nCol, TRUE);
		}

		return TRUE;
	}

	return FALSE;
}

CGXRange* CGXGridCoveredCellsImp::GetCoveredCellsRowCol(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, CGXRange& range)
{
	CGXGridParam* pParam = pGrid->GetParam();

	if (nRow >= INT_MAX || nCol >= INT_MAX)
		return FALSE;

	CGXRangeList* pCoveredCells = pParam->GetCoveredCellsList();

	POSITION pos = ((CGXCoveredCellPool*) pParam->m_pCoveredCells)->GetSpanCellsRowCol(nRow, nCol);

	if (pos)
	{
		range = *pCoveredCells->GetAt(pos);
		return &range;
	}

	return NULL;
}

void CGXGridCoveredCellsImp::MergeCoveredCells(CGXGridCore* pGrid, CGXRange& area)
{
	CGXGridParam* pParam = pGrid->GetParam();

	CGXRangeList* pCoveredCells = pParam->GetCoveredCellsList();

	CGXRange rgCovered;
	CGXRange t;

	if (!pCoveredCells->IsEmpty())
	{
		for (ROWCOL nRow = area.top; nRow <= area.bottom; nRow++)
		{
			for (ROWCOL nCol = area.left; nCol <= area.right; nCol++)
			{
				if (pGrid->GetCoveredCellsRowCol(nRow, nCol, rgCovered))
				{
					nCol = rgCovered.right+1;

					area.UnionRange(rgCovered, area);
				}
			}
		}
	}
}

BOOL CGXGridCoveredCellsImp::StoreCoveredCellsRowCol(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, ROWCOL nToRow, ROWCOL nToCol, BOOL bSet)
{
	CGXGridParam* pParam = pGrid->GetParam();

	if (nToRow >= INT_MAX || nToCol >= INT_MAX)
		return FALSE;

	// check if grid is read-only
	if (pParam->m_bLockReadOnly && (pGrid->IsReadOnly() || pGrid->StandardStyle().GetIncludeReadOnly() && pGrid->StandardStyle().GetReadOnly()))
		return FALSE;

	// ... now, I can store the data
	POSITION pos;
	CGXRangeList* pCoveredCells = pParam->GetCoveredCellsList();

	TRY
	{
		if (!bSet)
		{
			// Remove existing area
			pos = ((CGXCoveredCellPool*) pParam->m_pCoveredCells)->GetSpanCellsRowCol(nRow, nCol);
			if (pos)
			{
				CGXRange* pr = pCoveredCells->GetAt(pos);
				if (pr->top == nRow && pr->left == nCol)
				{
					((CGXCoveredCellPool*) pParam->m_pCoveredCells)->StoreSpanCellsRowCol(pr->top, pr->left, pr->bottom, pr->right, NULL);
					delete pCoveredCells->GetAt(pos);
					pCoveredCells->RemoveAt(pos);
					return TRUE;
				}
			}
			return FALSE;
		}
		else
		{
			// add new area
			CGXRange r = CGXRange(nRow, nCol, nToRow, nToCol);
			CGXRange rgCovered(r);

			pGrid->MergeCoveredCells(rgCovered);
			if (rgCovered != r)
				return FALSE;

			POSITION pos = ((CGXCoveredCellPool*) pParam->m_pCoveredCells)->GetSpanCellsRowCol(nRow, nCol);

			CGXRange* pr = NULL;
			if (pos != NULL)
			{
				pr = pCoveredCells->GetAt(pos);

				if (*pr == r)
					return FALSE;
				else if (pr->left == r.left && pr->top == r.top)
				{
					// change existing area
					*pr = r;
					((CGXCoveredCellPool*) pParam->m_pCoveredCells)->StoreSpanCellsRowCol(r.top, r.left, r.bottom, r.right, pos);
					return TRUE;
				}
				else
				{
					ASSERT(0); // something went wrong in MergeCoveredCells
					// cannot change existing area
					return FALSE;
				}
			}

			// There was no intersection. So, I can add the area to the list
			pos = pCoveredCells->AddTail(new CGXRange(r));
			((CGXCoveredCellPool*) pParam->m_pCoveredCells)->StoreSpanCellsRowCol(r.top, r.left, r.bottom, r.right, pos);

			// Fall through
		}
	}
	CATCH(CMemoryException, e)
	{
		return FALSE;
	}
	END_CATCH

	return TRUE;
}

void CGXGridCoveredCellsImp::UpdateCoveredCellsRowCol(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCol, ROWCOL nOldToRow, ROWCOL nOldToCol, UINT flags, BOOL bCreateHint)
{
	if (!pGrid->IsLockUpdate())
	{

		ROWCOL nToRow = nRow,
			   nToCol = nCol;

		CGXRange rgCovered;
		if (pGrid->GetCoveredCellsRowCol(nRow, nCol, rgCovered))
		{
			nToRow = rgCovered.bottom;
			nToCol = rgCovered.right;

			ROWCOL nEditRow, nEditCol;
			// if current cell is hidden by covered cells range,
			// move current cell to the covered cell
			if (pGrid->GetCurrentCell(&nEditRow, &nEditCol)
				&& !(rgCovered.top == nEditRow && rgCovered.left == nEditCol)
				&& rgCovered.IsCellInRange(nEditRow, nEditCol))
				pGrid->SetCurrentCell(nRow, nCol, flags | GX_SCROLLINVIEW);
		}

		pGrid->RedrawRowCol(nRow, nCol, max(nToRow, nOldToRow), max(nToCol, nOldToCol), flags, FALSE);
	}

	// Create Hint
	if (pGrid->m_pUpdateHintImp && bCreateHint && pGrid->m_bHintsEnabled)
		pGrid->m_pUpdateHintImp->HintUpdateCoveredCellsRowCol(pGrid, nRow, nCol, nOldToRow, nOldToCol, flags);
}


BOOL CGXGridCoveredCellsImp::RemoveRows(CGXGridCore* pGrid, ROWCOL nFromRow, ROWCOL nToRow, UINT& flags, GXCmdType ctCmd)
{
	BOOL bCoverDone = FALSE;

	// Covered Cells
	if (ctCmd == gxDo)
	{
		BOOL bLock = pGrid->LockUpdate(TRUE);

		pGrid->BeginTrans(pGrid->m_pAppData->strmRemoveRows);

		// Block: Covered Cells
		{
			POSITION pos = pGrid->GetParam()->GetCoveredCellsList()->GetHeadPosition();
			while (pos)
			{
				CGXRange r = pGrid->GetParam()->GetCoveredCellsList()->GetNext(pos);
				if (r.top >= nFromRow && r.top <= nToRow)
					bCoverDone |= pGrid->SetCoveredCellsRowCol(r.top, r.left, r.top, r.left, flags, ctCmd);
				else if (r.top < nFromRow && r.bottom >= nFromRow && r.bottom <= nToRow)
				{
					bCoverDone |= pGrid->SetCoveredCellsRowCol(r.top, r.left, r.top, r.left, flags, ctCmd);
					bCoverDone |= pGrid->SetCoveredCellsRowCol(r.top, r.left, nFromRow-1, r.right, flags, ctCmd);
				}
			}
		}

		if (bCoverDone)
			flags = GX_INVALIDATE;

		pGrid->LockUpdate(bLock);
	}

	return bCoverDone;
}

BOOL CGXGridCoveredCellsImp::RemoveCols(CGXGridCore* pGrid, ROWCOL nFromCol, ROWCOL nToCol, UINT& flags, GXCmdType ctCmd)
{
	BOOL bCoverDone = FALSE;

	// Covered Cells
	if (ctCmd == gxDo)
	{
		BOOL bLock = pGrid->LockUpdate(TRUE);

		pGrid->BeginTrans(pGrid->m_pAppData->strmRemoveCols);

		// Block: Covered Cells
		{
			POSITION pos = pGrid->GetParam()->GetCoveredCellsList()->GetHeadPosition();
			while (pos)
			{
				CGXRange r = pGrid->GetParam()->GetCoveredCellsList()->GetNext(pos);
				if (r.left >= nFromCol && r.left <= nToCol)
					bCoverDone |= pGrid->SetCoveredCellsRowCol(r.top, r.left, r.top, r.left, flags, ctCmd);
				else if (r.left < nFromCol && r.right >= nFromCol && r.right <= nToCol)
				{
					bCoverDone |= pGrid->SetCoveredCellsRowCol(r.top, r.left, r.top, r.left, flags, ctCmd);
					bCoverDone |= pGrid->SetCoveredCellsRowCol(r.top, r.left, r.bottom, nFromCol-1, flags, ctCmd);
				}
			}
		}

		if (bCoverDone)
			flags = GX_INVALIDATE;

		pGrid->LockUpdate(bLock);
	}

	return bCoverDone;
}

/////////////////////////////////////////////////////////////////////////////
// class CGXCoveredCellPool

CGXCoveredCellPool::CGXCoveredCellPool()
{
}

POSITION CGXCoveredCellPool::GetSpanCellsRowCol(ROWCOL nRow, ROWCOL nCol)
{
	int row = (int) nRow, col = (int) nCol;

	CPtrArray* pPtrArray = NULL;

	if (row < m_SpanPool.GetSize())
		pPtrArray = m_SpanPool.GetAt(row);

	// PtrArray members contains pointer to range cell
	if (pPtrArray && col < pPtrArray->GetSize())
		return (POSITION) pPtrArray->GetAt(col);

	return NULL;
}

BOOL CGXCoveredCellPool::StoreSpanCellsRowCol(ROWCOL nFromRow, ROWCOL nFromCol, ROWCOL nToRow, ROWCOL nToCol, POSITION pos)
{
	BOOL bDone = FALSE;

	if (pos && nToCol == nFromCol && nToRow == nFromRow)
		return FALSE;

	TRY
	{
		int row, col;
		for (ROWCOL nRow = nFromRow; nRow <= nToRow; nRow++)
		{
			row = (int) nRow;
			col = (int) nFromCol;

			CPtrArray* pPtrArray = NULL;

			if (row < m_SpanPool.GetSize())
				pPtrArray = m_SpanPool.GetAt(row);

			if (pPtrArray == NULL)
			{
				if (pos == NULL)
					continue;
				else
				{
					pPtrArray = new CPtrArray;
					m_SpanPool.SetAtGrow(row, pPtrArray);
				}
			}

			POSITION p = 0;

			// PtrArray members contains pointer to float cell
			// e.g. 0 0 0 0 4 4 4 4 0 0 10 10 10 ...
			if (pPtrArray && col < pPtrArray->GetSize())
				p = (POSITION) pPtrArray->GetAt(col);

			if (!pos)
			{
				// Remove existing area
				if (p)
				{
					while (col < pPtrArray->GetSize()
						&& pPtrArray->GetAt(col) == p)
						pPtrArray->SetAt(col++, 0);

					bDone = TRUE;
				}
			}
			else
			{
				if (p && p != pos)
					return FALSE;

				// add new area
				while (col <= (int) nToCol)
					pPtrArray->SetAtGrow(col++, pos);

				while (col < pPtrArray->GetSize()
					&& pPtrArray->GetAt(col) == pos)
					pPtrArray->SetAt(col++, 0);

				bDone = TRUE;
			}
		}
	}
	CATCH(CMemoryException, e)
	{
	}
	END_CATCH

	return bDone;
}

void CGXCoveredCellPool::InitFromRangeList(const CGXRangeList* pRangeList)
{
	MakeEmpty();

	if (!pRangeList->IsEmpty())
	{
		CGXRange t;
		// check, if intersections with existing areas
		POSITION pos = pRangeList->GetHeadPosition();
		POSITION p;
		while (pos != NULL)
		{
			p = pos;
			CGXRange& r = *pRangeList->GetNext(pos);

			StoreSpanCellsRowCol(r.top, r.left, r.bottom, r.right, p);
		}
	}
}

void CGXCoveredCellPool::MakeEmpty()
{
	m_SpanPool.DeleteAll();
}

