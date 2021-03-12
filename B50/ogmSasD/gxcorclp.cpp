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

// gxcorclp.cpp : Clipboard and OLE Drag&Drop (CGXGridCore)
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

class CGXGridCutPasteImp: public CGXAbstractGridCutPasteImp
{
public:
	virtual BOOL CanCopy(CGXGridCore* pGrid);
	virtual BOOL Copy(CGXGridCore* pGrid);
	virtual BOOL CopyRange(CGXGridCore* pGrid, const CGXRangeList& selList);
	virtual BOOL CopyTextToClipboard(CGXGridCore* pGrid, const CGXRangeList& selList);
	virtual BOOL CopyCellsToClipboard(CGXGridCore* pGrid, const CGXRangeList& selList, BOOL bLoadBaseStyles);
	virtual BOOL CanCut(CGXGridCore* pGrid);
	virtual BOOL Cut(CGXGridCore* pGrid);
	virtual BOOL CutRange(CGXGridCore* pGrid, const CGXRangeList& selList, BOOL bStyleOrValue);
	virtual BOOL CanPaste(CGXGridCore* pGrid);
	virtual BOOL OnCheckClipboardFormat(CGXGridCore* pGrid);
	virtual BOOL Paste(CGXGridCore* pGrid);
	virtual BOOL OnPasteDirect(CGXGridCore* pGrid, const CGXRange& range);
	virtual BOOL OnPasteFromClipboard(CGXGridCore* pGrid, const CGXRange& rg);
	virtual BOOL OnPasteDiffRange(CGXGridCore* pGrid);
};

void CGXGridCore::ImplementCutPaste()
{
	if (m_pCutPasteImp == NULL)
		AutoDeletePtr(m_pCutPasteImp = new CGXGridCutPasteImp);
}

/////////////////////////////////////////////////////////////////////////////
// Copy to clipboard helpers

// Edit|Copy

BOOL CGXGridCutPasteImp::CanCopy(CGXGridCore* pGrid)
{
	// return FALSE if clipboard is already opened
	if (GetOpenClipboardWindow() != NULL)
		return FALSE;

	CGXControl* pControl = pGrid->GetCurrentCellControl();
	CGXGridParam* pParam = pGrid->GetParam();

	return pParam && !pParam->GetRangeList()->IsEmpty()
		|| pControl && pControl->CanCopy();
}

BOOL CGXGridCutPasteImp::Copy(CGXGridCore* pGrid)
{
	CGXGridParam* pParam = pGrid->GetParam();

	// Empty clipboard
	VERIFY(pGrid->OpenClipboard());
	BOOL bEmpty = EmptyClipboard();
	VERIFY(CloseClipboard());
	if (!bEmpty)
	{
		TRACE0("Warning: Cannot empty clipboard!\n");
		return FALSE;
	}

	// return FALSE if clipboard is already opened
	if (GetOpenClipboardWindow() != NULL)
		return FALSE;

	CGXControl* pControl = pGrid->GetCurrentCellControl();
	BOOL bActive = pControl && pControl->IsActive();
	BOOL bSucess = FALSE;

	BOOL bRangeSel = pGrid->CopyRangeList(pParam->m_CopyRangeList, FALSE);

	// If there are no selections, give current cell the
	// chance to copy its prefered clipboard format
	if ((!bRangeSel || bActive) && pControl && pControl->Copy())

		bSucess = TRUE;

	// If there are no selections add current cell
	else if (bRangeSel || pGrid->CopyRangeList(pParam->m_CopyRangeList, TRUE))
	{
		CGXLongOperation theOp;

		// loop through all selected ranges and expand them.
		// row and column headers will be excluded from expanded range.
		ROWCOL nFirstRow = 0;
		ROWCOL nFirstCol = 0;
		ROWCOL nRowCount = pGrid->GetRowCount();
		ROWCOL nColCount = pGrid->GetColCount();

		if ((pGrid->m_nClipboardFlags & GX_DNDROWHEADER) == 0)
			nFirstCol = pGrid->GetHeaderCols()+1;

		if ((pGrid->m_nClipboardFlags & GX_DNDCOLHEADER) == 0)
			nFirstRow = pGrid->GetHeaderRows()+1;

		if (nFirstRow || nFirstCol)
		{
			// I am expanding ranges only when GX_DNDCOLHEADER
			// or GX_DNDROWHEADER has been specified because
			// after ranges have been expanded they are
			// not marked as "row range" or "column range"
			// any more.

			POSITION pos = pParam->m_CopyRangeList.GetHeadPosition( );
			while (pos)
			{
				pParam->m_CopyRangeList.GetNext(pos)->ExpandRange(nFirstRow, 
						nFirstCol,
						nRowCount,
						nColCount);
			}
		}

		bSucess = pGrid->CopyRange(pParam->m_CopyRangeList);
	}

	if (pParam->m_bCut)
	{
		pParam->m_bCut = FALSE;
		pGrid->Redraw();
	}

	return bSucess;
}

// Copy range of cells without having to select it.

#define GX_DIRECTCOPY "GX_DIRECTCOPY"

static UINT s_cfDIRECTCOPY = 0xffff;

inline UINT AFXAPI cfDirectCopy() 
{
	if( s_cfDIRECTCOPY == 0xffff)
#if !defined(_UNICODE)
		s_cfDIRECTCOPY = RegisterClipboardFormat(GX_DIRECTCOPY);
#else
		s_cfDIRECTCOPY= RegisterClipboardFormat(_T(GX_DIRECTCOPY));
#endif

	return s_cfDIRECTCOPY;
}

BOOL CGXGridCutPasteImp::CopyRange(CGXGridCore* pGrid, const CGXRangeList& selList)
{
	CGXLongOperation theOp;

	BOOL bSuccess = FALSE;
	CGXGridParam* pParam = pGrid->GetParam();

	if (pGrid->m_nClipboardFlags & GX_DNDTEXT)
		bSuccess |= pGrid->CopyTextToClipboard(selList);

	if (pGrid->m_nClipboardFlags & GX_DNDSTYLES)
		bSuccess |= pGrid->CopyCellsToClipboard(selList, FALSE); //(BOOL) (m_nClipboardFlags & GX_DNDCOMPOSE));

	if (pParam->m_bDirectCopyPaste || pParam->m_bDirectCutPaste)
	{
		HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE|GMEM_ZEROINIT, sizeof(CGXGridParam*));

		// Now, hand over memory block to clipboard
		if (hData)
		{
			void** lpCopy = (void**) GlobalLock(hData); 
			void* p1 = pParam;
			*lpCopy = p1;
			GlobalUnlock(hData);          // Place the handle on the clipboard.  

			VERIFY(pGrid->OpenClipboard());
	        SetClipboardData(cfDirectCopy(), hData);
			VERIFY(CloseClipboard());
		}
	}

	// Check if canceled by user or failed for other reasons
	return bSuccess;
}

// copy unformatted text to clipboard

BOOL CGXGridCutPasteImp::CopyTextToClipboard(CGXGridCore* pGrid, const CGXRangeList& selList)
{
	HGLOBAL hData = 0;
	HANDLE hResult = 0;

	// open memory file
	CSharedFile clipb (GMEM_MOVEABLE|GMEM_DDESHARE|GMEM_ZEROINIT);

	BOOL bSuccess = pGrid->CopyTextToFile(clipb, selList);

	if (bSuccess)
		hData = clipb.Detach();

	// Now, hand over memory block to clipboard
	if (hData)
	{
		VERIFY(pGrid->OpenClipboard());

#ifdef _UNICODE
		hResult = SetClipboardData(CF_UNICODETEXT, hData);
#else
		hResult = SetClipboardData(CF_TEXT, hData);
#endif

		VERIFY(CloseClipboard());
	}

	return hResult != 0;    // Successful if hResult is non zero
}

// copy style information to clipboard

BOOL CGXGridCutPasteImp::CopyCellsToClipboard(CGXGridCore* pGrid, const CGXRangeList& selList, BOOL bLoadBaseStyles)
{
	HGLOBAL hData = 0;
	HANDLE hResult = 0;

	// open memory file
	CSharedFile clipb (GMEM_MOVEABLE|GMEM_DDESHARE|GMEM_ZEROINIT);
	CArchive ar(&clipb, CArchive::store);

	BOOL bSuccess = pGrid->CopyCellsToArchive(ar, selList, bLoadBaseStyles);

	ar.Close();

	if (bSuccess)
		hData = clipb.Detach();

	// Now, hand over memory block to clipboard
	if (hData)
	{
		VERIFY(pGrid->OpenClipboard());

		hResult = SetClipboardData(pGrid->m_pAppData->m_uiStylesClipboardFormat, hData);

		VERIFY(CloseClipboard());
	}

	return hResult != 0;    // Successful if hResult is non zero
}

/////////////////////////////////////////////////////////////////////////////
// Cut cells helpers

// Edit|Cut

BOOL CGXGridCutPasteImp::CanCut(CGXGridCore* pGrid)
{
	// return FALSE if clipboard is already opened
	if (GetOpenClipboardWindow() != NULL || pGrid->IsReadOnly())
		return FALSE;

	CGXControl* pControl = pGrid->GetCurrentCellControl();
	CGXGridParam* pParam = pGrid->GetParam();

	return pParam && !pParam->GetRangeList()->IsEmpty()
		|| pControl && pControl->CanCut();
}

BOOL CGXGridCutPasteImp::Cut(CGXGridCore* pGrid)
{
	CGXGridParam* pParam = pGrid->GetParam();

	VERIFY(pGrid->OpenClipboard());
	EmptyClipboard();
	VERIFY(CloseClipboard());

	CGXLongOperation theOp;

	CGXControl* pControl = pGrid->GetCurrentCellControl();
	BOOL bActive = pControl && pControl->IsActive();
	BOOL bSucess = FALSE;

	BOOL bRangeSel = pGrid->CopyRangeList(pParam->m_CopyRangeList, FALSE);

	// If there are no selections, give current cell the
	// chance to copy its prefered clipboard format
	if ((!bRangeSel || bActive) && pControl && pControl->Cut())
		bSucess = TRUE;

	else if (pParam->m_bDirectCutPaste)
	{
		// Copy will set up CopyRangeList
		// Data will be cut later when user gives Paste Command
		pGrid->Copy();
		pParam->m_bCut = TRUE;

		// this will redraw the cells greyed out
		pGrid->Redraw();

		bSucess = TRUE;
	}
	else
	{
		CGXLongOperation theOp;

		// Determine agian list of selections (if necessary add current cell)
		VERIFY(pGrid->CopyRangeList(pParam->m_CopyRangeList, TRUE));

		bSucess = pGrid->CutRange(pParam->m_CopyRangeList, (BOOL) (pGrid->m_nClipboardFlags & GX_DNDSTYLES));
	}

	return bSucess;
}

BOOL CGXGridCutPasteImp::CutRange(CGXGridCore* pGrid, const CGXRangeList& selList, BOOL bStyleOrValue)
{
	pGrid->BeginTrans(pGrid->m_pAppData->strmCutData);

	// First, copy data
	if (pGrid->Copy() && pGrid->ClearCells(selList, bStyleOrValue))
	{
		pGrid->CommitTrans();
		return TRUE;
	}
	else
	{
		// if failed, rollback and do not clear any cells
		pGrid->Rollback();
		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Paste from clipboard helpers

// Edit|Paste

BOOL CGXGridCutPasteImp::CanPaste(CGXGridCore* pGrid)
{
	// return FALSE if clipboard is already opened
	CGXGridParam* pParam = pGrid->GetParam();
	if (pGrid->IsReadOnly() && pParam->IsLockReadOnly())
		return FALSE;

	if (!pParam->m_CopyRangeList.IsEmpty() && 
		(pParam->m_bDirectCutPaste && pParam->m_bCut 
			|| pParam->m_bDirectCopyPaste && !pParam->m_bCut))
		return TRUE;

	// First, give current cell a chance
	CGXControl* pControl = pGrid->GetCurrentCellControl();
	if (pControl && pControl->CanPaste())
		return TRUE;

	// Next, check other clipboard format
//  OpenClipboard();
	// Review! MAKE A NOTE IN DOCU: OpenClipboard is not called any more
	BOOL bCanPaste = pGrid->OnCheckClipboardFormat();
//  CloseClipboard();

	return bCanPaste;
}

BOOL CGXGridCutPasteImp::OnCheckClipboardFormat(CGXGridCore* pGrid)
{
	BOOL bCanPaste = FALSE;

	// Check if clipboard is opened
#ifdef _UNICODE
	bCanPaste = IsClipboardFormatAvailable(CF_UNICODETEXT);
#else
	bCanPaste = IsClipboardFormatAvailable(CF_TEXT);

	if (!bCanPaste)
		bCanPaste = IsClipboardFormatAvailable(CF_OEMTEXT);
#endif

	if (!bCanPaste)
		bCanPaste = IsClipboardFormatAvailable(pGrid->m_pAppData->m_uiStylesClipboardFormat);

	return bCanPaste;
}

BOOL CGXGridCutPasteImp::Paste(CGXGridCore* pGrid)
{
	if (!pGrid->CanPaste())
		return FALSE;

	// Verify that only one range or current cell is selected
	CGXRangeList selList;
	if (!pGrid->CopyRangeList(selList, TRUE))
		return FALSE;

	// Give the current cell a chance
	CGXControl* pControl = pGrid->GetCurrentCellControl();
	if (pControl && pControl->CanPaste() && pControl->Paste())
		return TRUE;

	CGXGridParam* pParam = pGrid->GetParam();

	if (selList.GetCount() > 1)
	{
		// only one selected range is allowed
		AfxMessageBox(GX_IDM_PASTENOTMULTI);
		return FALSE;
	}

	ROWCOL nFirstRow = 0;
	ROWCOL nFirstCol = 0;
	ROWCOL nRowCount = pGrid->GetRowCount();
	ROWCOL nColCount = pGrid->GetColCount();

	if (!(pGrid->m_nClipboardFlags & GX_DNDROWHEADER))
		nFirstRow = pGrid->GetHeaderRows()+1;

	if (!(pGrid->m_nClipboardFlags & GX_DNDCOLHEADER))
		nFirstCol = pGrid->GetHeaderCols()+1;

	CGXRange range = *selList.GetHead();
	range.ExpandRange(nFirstRow, nFirstCol, nRowCount, nColCount);

	BOOL bPasteDirect = FALSE;

	// Cut/Paste directly in sheet (no clipboard)
	if (!pParam->m_CopyRangeList.IsEmpty() 
		&& (pParam->m_bDirectCutPaste && pParam->m_bCut 
			|| pParam->m_bDirectCopyPaste && !pParam->m_bCut))
	{
		if (pParam->m_bDirectCutPaste && pParam->m_bCut)
			bPasteDirect = TRUE;

		else if (IsClipboardFormatAvailable(cfDirectCopy()))
		{
			VERIFY(pGrid->OpenClipboard());
			HGLOBAL handle = GetClipboardData(cfDirectCopy());

			if (handle)
			{
				void** psz = (void**) GlobalLock(handle);
				void* p1 = pParam;
				void* p2 = *psz;
				bPasteDirect = p1 == p2;

				GlobalUnlock(handle);
			}

			CloseClipboard();
		}
	}

	if (bPasteDirect)
		return pGrid->OnPasteDirect(range);
	else
	{
		VERIFY(pGrid->OpenClipboard());

		// if you want to support additional clipboard format,
		// you should override the following method

		BOOL bSuccess = pGrid->OnPasteFromClipboard(range);

		CloseClipboard();

		return bSuccess;
	}
}

BOOL CGXGridCutPasteImp::OnPasteDirect(CGXGridCore* pGrid, const CGXRange& range)
{
	CGXGridParam* pParam = pGrid->GetParam();

	if (!pParam->m_CopyRangeList.IsEmpty()
		&& (pParam->m_bDirectCutPaste && pParam->m_bCut 
			|| pParam->m_bDirectCopyPaste && !pParam->m_bCut))
	{
		// store rows/columns indizes to process in an array
		CRowColArray    awLeftCol, awRightCol,
						awTopRow, awBottomRow;

		pParam->m_CopyRangeList.GetRowArray(pGrid->GetRowCount(), awTopRow, awBottomRow, FALSE);
		pParam->m_CopyRangeList.GetColArray(pGrid->GetColCount(), awLeftCol, awRightCol, FALSE);

		// status message, let the user abort the operation
		BOOL bLock = pGrid->IsLockUpdate();
		BOOL bTrans = pParam->m_CopyRangeList.GetCount() > 1;

		CGXLongOperation theOp;
		if (pParam->m_bCut)
		{
			theOp.SetStatusText(pGrid->m_pAppData->strmDragDropMove, FALSE);
			if (bTrans)
				pGrid->BeginTrans(pGrid->m_pAppData->strmDragDropMove);
			pGrid->LockUpdate(TRUE);
		}
		else
		{
			theOp.SetStatusText(pGrid->m_pAppData->strmDragDropCopy, FALSE);
			if (bTrans)
			{
				pGrid->BeginTrans(pGrid->m_pAppData->strmDragDropCopy);
				pGrid->LockUpdate(TRUE);
			}
		}

		ROWCOL nRows = 0;
		ROWCOL nCols = 0;
		
		TRY
		{
			CGXRange rg;
			BOOL bCanceled = FALSE;

			BOOL bCurCell = FALSE;

			for (int rowindex = 0; !bCanceled && rowindex < awTopRow.GetSize(); rowindex++)
			{										 
				nCols = 0;

				for (int colindex = 0; !bCanceled && colindex < awLeftCol.GetSize(); colindex++)
				{
					rg.SetCells(awTopRow[rowindex], awLeftCol[colindex], awBottomRow[rowindex], awRightCol[colindex]);

					if (pParam->m_bCut)
						bCanceled |= !pGrid->MoveCells(rg, range.top+nRows, range.left+nCols);
					else
						bCanceled |= !pGrid->CopyCells(rg, range.top+nRows, range.left+nCols);

					CGXRange r(range.top, range.left, range.top+rg.GetHeight()-1, range.left+rg.GetWidth()-1);

					bCurCell |= r.IsCellInRange(pGrid->m_nEditRow, pGrid->m_nEditCol);

					nCols += rg.GetWidth();
				}

				nRows += rg.GetHeight();
			}

			if (bTrans)
			{
				if (bCanceled)
					pGrid->Rollback();
				else
					pGrid->CommitTrans();
			}
			  
			if (pParam->m_bCut)
			{
				pParam->m_bCut = FALSE;
				pParam->m_CopyRangeList.DeleteAll();
			}

			if (pGrid->IsLockUpdate())
			{
				pGrid->LockUpdate(bLock);
				pGrid->Recalc();
				pGrid->Redraw();
			}

			if (bCurCell)
			{
				pGrid->DelayFloatCells(CGXRange(pGrid->m_nEditRow, pGrid->m_nEditCol));
				pGrid->SetCurrentCell(pGrid->m_nEditRow, pGrid->m_nEditCol);
			}

			return !bCanceled;
		}
		CATCH(CException, e)
		{
			if (e->IsKindOf(RUNTIME_CLASS(CMemoryException)))
				AfxMessageBox(GX_IDM_RANGETOOBIG, MB_OK);
		}
		END_CATCH
	}

	return TRUE;
}

BOOL CGXGridCutPasteImp::OnPasteFromClipboard(CGXGridCore* pGrid, const CGXRange& rg)
{
	ROWCOL nFirstRow = 0;
	ROWCOL nFirstCol = 0;

	if (!(pGrid->m_nClipboardFlags & GX_DNDROWHEADER))
		nFirstRow = pGrid->GetHeaderRows()+1;

	if (!(pGrid->m_nClipboardFlags & GX_DNDCOLHEADER))
		nFirstCol = pGrid->GetHeaderCols()+1;

	CGXRange range(max(nFirstRow, rg.top),
			max(nFirstCol, rg.left),
			rg.bottom,
			rg.right);

	if (pGrid->m_nClipboardFlags & GX_DNDSTYLES)
	{
		// try internal format
		HGLOBAL hStyles = GetClipboardData(pGrid->m_pAppData->m_uiStylesClipboardFormat);

		// if CF_INTERNAL clipboard format found
		// create the archive and get the data.
		if (hStyles)
		{
			CSharedFile mf;
			mf.SetHandle(hStyles);
			CArchive ar(&mf, CArchive::load);
			BOOL bSuccess = pGrid->PasteCellsFromArchive(ar, range, !(pGrid->m_nClipboardFlags & GX_DNDDIFFRANGEDLG));
			ar.Close();
			mf.Detach();
			return bSuccess;
		}
	}

	// if no CF_INTERNAL clipboard format found

	if (pGrid->m_nClipboardFlags & GX_DNDTEXT)
	{
		// check for CF_TEXT clipboard format
		HGLOBAL handle = 0;

#ifdef _UNICODE
		handle = GetClipboardData(CF_UNICODETEXT);
#else
		handle = GetClipboardData(CF_TEXT);

		if (handle == 0)
			handle = GetClipboardData(CF_OEMTEXT);
#endif

		// if CF_TEXT clipboard format found
		if (handle)
		{
			LPTSTR psz = (LPTSTR) GlobalLock(handle);
			DWORD size = GlobalSize(handle);

			BOOL bSuccess = pGrid->PasteTextFromBuffer(psz, size, range);

			GlobalUnlock(handle);

			return bSuccess;
		}
	}

	// clipboard format unknown
	return FALSE;
}

BOOL CGXGridCutPasteImp::OnPasteDiffRange(CGXGridCore* pGrid)
{
	// This method is called when the selected range of
	// cells differs from the range of cells currently
	// available in the cliboard.
	//
	// Override this method if you don't want that this
	// message box is displayed.

	// return FALSE if paste-operation should be canceld
	return AfxMessageBox(pGrid->m_pAppData->strmPasteDiffRange, MB_OKCANCEL) == IDOK;
}

