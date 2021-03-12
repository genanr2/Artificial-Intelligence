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

// gxclear.cpp : Clearing cells
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

// Edit|Clear

class CGXGridClearCellsImp: public CGXAbstractGridClearCellsImp
{
public:
	virtual BOOL CanClear(CGXGridCore* pGrid);
	virtual BOOL Clear(CGXGridCore* pGrid, BOOL bStyleOrValue /* = TRUE */);
	virtual BOOL ClearCells(CGXGridCore* pGrid, const CGXRangeList& selList, BOOL bStyleOrValue);
};

void CGXGridCore::ImplementClearCells()
{
	if (m_pClearCellsImp == NULL)
		AutoDeletePtr(m_pClearCellsImp = new CGXGridClearCellsImp);
}

BOOL CGXGridClearCellsImp::CanClear(CGXGridCore* pGrid)
{
	if (pGrid->IsReadOnly())
		return FALSE;

	return pGrid->GetParam() && !pGrid->GetParam()->GetRangeList()->IsEmpty() || pGrid->IsCurrentCell();
}

BOOL CGXGridClearCellsImp::Clear(CGXGridCore* pGrid, BOOL bStyleOrValue /* = TRUE */)
{
	CGXLongOperation theOp;

	CGXRangeList selList;
	BOOL bSucess = FALSE;

	VERIFY(pGrid->CopyRangeList(selList, TRUE));

	bSucess = pGrid->ClearCells(selList, bStyleOrValue);

	return bSucess;
}

BOOL CGXGridClearCellsImp::ClearCells(CGXGridCore* pGrid, const CGXRangeList& selList, BOOL bStyleOrValue)
{
	pGrid->BeginTrans(pGrid->m_pAppData->strmClearData);

	CGXLongOperation theOp;

	BOOL bCanceled = FALSE;

	theOp.SetStatusText(pGrid->m_pAppData->strmClearData, FALSE);
	theOp.SetCanRollback(TRUE);
	theOp.SetLockedState(TRUE);

	ROWCOL nFirstRow = 0;
	ROWCOL nFirstCol = 0;
	ROWCOL nRowCount = pGrid->GetRowCount();
	ROWCOL nColCount = pGrid->GetColCount();

	nFirstRow = pGrid->GetHeaderRows()+1;
	nFirstCol = pGrid->GetHeaderCols()+1;

	POSITION pos = selList.GetHeadPosition( );
	while (pos)
	{
		selList.GetNext(pos)->ExpandRange(nFirstRow, 
				nFirstCol,
				nRowCount,
				nColCount);
	}

	CRowColArray    awLeftCol, awRightCol,
					awTopRow, awBottomRow;

	selList.GetRowArray(pGrid->GetRowCount(), awTopRow, awBottomRow, FALSE);
	selList.GetColArray(pGrid->GetColCount(), awLeftCol, awRightCol, FALSE);

	BOOL bOldLockAddRecalcRange = pGrid->m_bLockAddRecalcRange;

	TRY
	{
		for (int rowindex = 0; !bCanceled && rowindex < awTopRow.GetSize(); rowindex++)
		{
			for (int colindex = 0; !bCanceled && colindex < awLeftCol.GetSize(); colindex++)
			{
				// SetStyleRange possibly throws an User Exception

				if (!pGrid->m_bLockAddRecalcRange)
					pGrid->AddRecalcRange(CGXRange(awTopRow[rowindex], awLeftCol[colindex],
							awBottomRow[rowindex], awRightCol[colindex]));

				pGrid->m_bLockAddRecalcRange = TRUE;

				if (bStyleOrValue)
				{
					// Remove all style information
					bCanceled = !pGrid->SetStyleRange(
						CGXRange(awTopRow[rowindex], awLeftCol[colindex],
							awBottomRow[rowindex], awRightCol[colindex]),
						NULL,
						gxRemove);

					if (bCanceled || theOp.DoMessages(bCanceled) && bCanceled)
						AfxThrowUserException();
				}
				else
				{
					CString s;

					// Only the value
					for (ROWCOL nRow = awTopRow[rowindex]; !bCanceled && nRow <= awBottomRow[rowindex]; nRow++)
					{
						for (ROWCOL nCol = awLeftCol[colindex]; !bCanceled && nCol <= awRightCol[colindex]; nCol++)
						{
							CGXStyle* pStyle = pGrid->CreateStyle();
							pGrid->ComposeStyleRowCol(nRow, nCol, pStyle);

							CGXControl* pControl = pGrid->GetRegisteredControl(pStyle->GetControl());

							// Give the control the chance to validate
							// and change the pasted text
							if (pControl->GetControlText(s, nRow, nCol, NULL, *pStyle)
								&& !s.IsEmpty())
								bCanceled = !pGrid->SetControlTextRowCol(nRow, nCol, _T(""), GX_INVALIDATE, pStyle);

							pGrid->RecycleStyle(pStyle);

							if (bCanceled || theOp.DoMessages(bCanceled) && bCanceled)
								AfxThrowUserException();
						}
					}
				}

				pGrid->m_bLockAddRecalcRange = bOldLockAddRecalcRange;
			}
		}
	}
	CATCH(CUserException, e)
	{
		pGrid->m_bLockAddRecalcRange = bOldLockAddRecalcRange;

		if (theOp.GetRollbackConfirmedState())
			pGrid->Rollback();
		else
			pGrid->CommitTrans();
		return FALSE;
	}
	END_CATCH

	pGrid->CommitTrans();

	pGrid->RefreshViews();
	
	return TRUE;
}

