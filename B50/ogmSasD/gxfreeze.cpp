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

// gxfreeze.cpp: Freezing rows and columns
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

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCORDRA")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

// CGXAbstractGridFreezeMenuHelperImp* m_pFreezeMenuImp
// friend class CGXGridFreezeMenuHelperImp;
// void EnableFreezeMenuHelper();

class CGXGridFreezeMenuHelperImp: public CGXAbstractGridFreezeMenuHelperImp
{
public:
	virtual BOOL CanFreezeCols(CGXGridCore* pGrid);
	virtual BOOL CanUnfreezeCols(CGXGridCore* pGrid);
	virtual void FreezeCols(CGXGridCore* pGrid);
	virtual void UnfreezeCols(CGXGridCore* pGrid);
	virtual BOOL CanFreezeRows(CGXGridCore* pGrid);
	virtual BOOL CanUnfreezeRows(CGXGridCore* pGrid);
	virtual void FreezeRows(CGXGridCore* pGrid);
	virtual void UnfreezeRows(CGXGridCore* pGrid);
};

void CGXGridCore::ImplementFreezeMenuHelper()
{
	if (m_pFreezeMenuImp == NULL)
		AutoDeletePtr(m_pFreezeMenuImp = new CGXGridFreezeMenuHelperImp);
}

BOOL CGXGridFreezeMenuHelperImp::CanFreezeCols(CGXGridCore* pGrid)
{
	CGXGridParam* pParam = pGrid->GetParam();
	if (pParam == NULL || pGrid->GetFrozenCols() != 0)
		return FALSE;

	CRowColArray  awLeftCol, awRightCol;
	CGXRangeList* pSelList = pParam->GetRangeList();

	pSelList->GetColArray(pGrid->GetColCount(), awLeftCol, awRightCol, TRUE);

	return pSelList
		&& awLeftCol.GetSize() == 1;    // only one block of columns can be fixed
}

BOOL CGXGridFreezeMenuHelperImp::CanUnfreezeCols(CGXGridCore* pGrid)
{
	return pGrid->GetParam() && pGrid->GetFrozenCols() > 0;
}

void CGXGridFreezeMenuHelperImp::FreezeCols(CGXGridCore* pGrid)
{
	if (!pGrid->CanFreezeCols())
		return;

	ROWCOL nHeaderCols = pGrid->GetHeaderCols();
	CRowColArray  awLeftCol, awRightCol;

	CGXGridParam* pParam = pGrid->GetParam();

	CGXRangeList* pSelList = pParam->GetRangeList();

	pSelList->GetColArray(pGrid->GetColCount(), awLeftCol, awRightCol, TRUE);

	CGXWaitCursor theWait;
	pGrid->BeginTrans(pGrid->m_pAppData->strmSetFrozenCols);

	BOOL b = pGrid->LockUpdate();
	pGrid->SetSelection(0);

	awLeftCol[0] = max(awLeftCol[0], nHeaderCols+1);

	if (awLeftCol[0] != 1)
		pGrid->MoveCols(awLeftCol[0], awRightCol[0], 1);

	ROWCOL nCols = awRightCol[0]-awLeftCol[0]+1;
	pParam->m_nUndoFreezeFromCol = nHeaderCols+1;
	pParam->m_nUndoFreezeToCol = nCols;
	pParam->m_nUndoFreezeDestCol = awLeftCol[0];

	pGrid->SetFrozenCols(nCols);
	pGrid->LockUpdate(b);
	pGrid->Redraw();
	pGrid->CommitTrans();
}

void CGXGridFreezeMenuHelperImp::UnfreezeCols(CGXGridCore* pGrid)
{
	CGXGridParam* pParam = pGrid->GetParam();

	if (pParam && pGrid->GetFrozenCols() > 0)
	{
		BOOL b = pGrid->LockUpdate();
		pGrid->BeginTrans(pGrid->m_pAppData->strmSetFrozenCols);

		if (pParam->m_nUndoFreezeFromCol != pParam->m_nUndoFreezeDestCol)
			pGrid->MoveCols(pParam->m_nUndoFreezeFromCol,
				pParam->m_nUndoFreezeToCol,
				pParam->m_nUndoFreezeDestCol);
		pGrid->SetFrozenCols(0);
		pGrid->LockUpdate(b);
		pGrid->Redraw();
		pGrid->CommitTrans();
	}
}

BOOL CGXGridFreezeMenuHelperImp::CanFreezeRows(CGXGridCore* pGrid)
{
	CGXGridParam* pParam = pGrid->GetParam();
	if (pParam == NULL || pGrid->GetFrozenRows() != 0)
		return FALSE;

	CRowColArray  awTopRow, awBottomRow;
	CGXRangeList* pSelList = pParam->GetRangeList();

	pSelList->GetRowArray(pGrid->GetRowCount(), awTopRow, awBottomRow, TRUE);

	return pSelList
		&& awTopRow.GetSize() == 1; // only one block of Rows can be fixed
}

BOOL CGXGridFreezeMenuHelperImp::CanUnfreezeRows(CGXGridCore* pGrid)
{
	return pGrid->GetParam() && pGrid->GetFrozenRows() > 0;
}

void CGXGridFreezeMenuHelperImp::FreezeRows(CGXGridCore* pGrid)
{
	if (!pGrid->CanFreezeRows())
		return;

	ROWCOL nHeaderRows = pGrid->GetHeaderRows();
	CRowColArray  awTopRow, awBottomRow;

	CGXGridParam* pParam = pGrid->GetParam();

	CGXRangeList* pSelList = pParam->GetRangeList();

	pSelList->GetRowArray(pGrid->GetRowCount(), awTopRow, awBottomRow, TRUE);

	CGXWaitCursor theWait;
	pGrid->BeginTrans(pGrid->m_pAppData->strmSetFrozenRows);

	BOOL b = pGrid->LockUpdate();
	pGrid->SetSelection(0);

	awTopRow[0] = max(awTopRow[0], nHeaderRows+1);

	if (awTopRow[0] > nHeaderRows+1)
		pGrid->MoveRows(awTopRow[0], awBottomRow[0], nHeaderRows+1);

	ROWCOL nRows = awBottomRow[0]-awTopRow[0]+1;
	pParam->m_nUndoFreezeFromRow = nHeaderRows+1;
	pParam->m_nUndoFreezeToRow = nRows;
	pParam->m_nUndoFreezeDestRow = awTopRow[0];

	pGrid->SetFrozenRows(nRows);
	pGrid->LockUpdate(b);
	pGrid->Redraw();
	pGrid->CommitTrans();
}

void CGXGridFreezeMenuHelperImp::UnfreezeRows(CGXGridCore* pGrid)
{
	CGXGridParam* pParam = pGrid->GetParam();

	if (pParam && pGrid->GetFrozenRows() > 0)
	{
		pGrid->SetFrozenRows(0);
		if (pParam->m_nUndoFreezeFromRow != pParam->m_nUndoFreezeDestRow)
			pGrid->MoveRows(pParam->m_nUndoFreezeFromRow,
				pParam->m_nUndoFreezeToRow,
				pParam->m_nUndoFreezeDestRow);
	}
}
