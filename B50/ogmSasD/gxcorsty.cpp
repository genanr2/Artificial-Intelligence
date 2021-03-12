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

// gxcorsty.cpp : styles and controls (CGXGridCore)
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
#pragma code_seg("GX_SEG_GXCORSTY")
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


void CGXGridCore::RegisterControl(WORD nID, CGXControl* pControl, BOOL bDefault, BOOL bShowInStyleSheet)
{
	ASSERT(pControl);
	// ASSERTION-> passed NULL pointer for control ->END

	ASSERT(pControl->IsKindOf(CONTROL_CLASS(CGXControl)));
	// ASSERTION-> pControl is not derived from CGXControl ->END

	ASSERT(nID > 0);
	// ASSERTION-> Invalid id ->END

	ASSERT(GetParam());
	// ASSERTION-> parameter-object is not yet linked to the view ->END

	CGXControl* pRegistered = NULL;

	void* vptr = (void*) (DWORD) nID;

	if (m_ControlMap.Lookup(vptr, (void*&) pRegistered))
	{
		if (pRegistered==m_pLastControlHit)
		{
			m_nLastControlHit = 0;
			m_pLastControlHit = NULL;
		}

		delete pRegistered;
	}

	m_ControlMap.SetAt(vptr, pControl);

	if (bShowInStyleSheet)
		GetParam()->GetStylesMap()->AddControl(nID);

	if (bDefault || m_pDefaultControl == NULL)
		m_pDefaultControl = pControl;
}

CGXControl* CGXGridCore::GetRegisteredControl(WORD nID)
{
	ASSERT(m_pDefaultControl);
	// ASSERTION-> no default control defined! ->END

	if (nID == 0)
		return m_pDefaultControl;
	else if (nID == m_nLastControlHit)
		return m_pLastControlHit;

	CGXControl* pControl = NULL;

	void* vptr = (void*) (DWORD) nID;

	// check if control is registered
	if (!m_ControlMap.Lookup(vptr, (void*&) pControl))
	{
		CGXAbstractControlFactory* pControlFactory = m_pAppData->m_pControlFactory;

		if (pControlFactory)
			pControl = pControlFactory->CreateControl(nID, this);

		if (pControl)
			RegisterControl(nID, pControl, FALSE, FALSE);
	}

	if (pControl)
	{
		// cache the last hit so that we can limit the
		// number of calls to m_ControlMap.Lookup 
		m_nLastControlHit = nID;
		m_pLastControlHit = pControl;
		return pControl;
	}
	else
		return m_pDefaultControl;
}

CGXStyle& CGXGridCore::StandardStyle()
{
	if (m_pStyleStandard == NULL)
	{
		if (!GetParam() || !GetParam()->GetStylesMap())
		{
			ASSERT(0);
			// ASSERTION-> Did you already call CGXGridCore::OnGridInitialUpdate
			// or did you attach a CGXStylesMap to CGXGridParam ? ->END
			AfxThrowNotSupportedException();
		}
		CGXStylesMap* pStyMap = m_pParam->m_pStylesMap;
		VERIFY(pStyMap->LookupStyle(pStyMap->m_wStyleStandard, m_pStyleStandard));
	}

	return *m_pStyleStandard;
}

CGXStyle& CGXGridCore::RowHeaderStyle()
{
	if (m_pStyleRowHeader == NULL)
	{
		if (!GetParam() || !GetParam()->GetStylesMap())
		{
			ASSERT(0);
			// ASSERTION-> Did you already call CGXGridCore::OnGridInitialUpdate
			// or did you attach a CGXStylesMap to CGXGridParam ? ->END
			AfxThrowNotSupportedException();
		}
		CGXStylesMap* pStyMap = m_pParam->m_pStylesMap;
		VERIFY(pStyMap->LookupStyle(pStyMap->m_wStyleRowHeader, m_pStyleRowHeader));
	}

	return *m_pStyleRowHeader;
}

CGXStyle& CGXGridCore::ColHeaderStyle()
{
	if (m_pStyleColHeader == NULL)
	{
		if (!GetParam() || !GetParam()->GetStylesMap())
		{
			ASSERT(0);
			// ASSERTION-> Did you already call CGXGridCore::OnGridInitialUpdate
			// or did you attach a CGXStylesMap to CGXGridParam ? ->END
			AfxThrowNotSupportedException();
		}
		CGXStylesMap* pStyMap = m_pParam->m_pStylesMap;
		VERIFY(pStyMap->LookupStyle(pStyMap->m_wStyleColHeader, m_pStyleColHeader));
	}

	return *m_pStyleColHeader;
}

CGXStyle& CGXGridCore::BaseStyle(WORD wStyleID)
{
	CGXStyle* pStyle;
	if (GetParam()->GetStylesMap()->LookupStyle(wStyleID, pStyle))
		return *pStyle;

	ASSERT(0);
	// ASSERTION-> Did you already register the style
	// with CGXStylesMap::RegisterStyle? ->END

	AfxThrowNotSupportedException();

	return *pStyle;
}

/////////////////////////////////////////////////////////////////////////////
// Cell Attributes/Styles

CObArray* CGXGridCore::GetStyleArray(const CGXRange& range, int nType, CGXLongOperation* pTheOp) 
{
	DWORD dwSize = range.GetWidth()*range.GetHeight();
	BOOL bAbort = FALSE;

	// Try, if array fits in memory
	CObArray* pOldCellsArray = NULL;

	TRY
	{
		pOldCellsArray = new CObArray;

		pOldCellsArray->SetSize((int) dwSize);

		// Store styles in array, but allow user to abort
		for (ROWCOL nRow = range.top; nRow <= range.bottom; nRow++)
		{
			for (ROWCOL nCol = range.left; nCol <= range.right; nCol++)
			{
				CGXStyle* pStyle;
				DWORD dwIndex;

				pStyle = CreateStyle();
				dwIndex = (nRow-range.top)*range.GetWidth()+(nCol-range.left);

				if (GetStyleRowCol(nRow, nCol, *pStyle, gxCopy, nType))
					pOldCellsArray->SetAt((int) dwIndex, pStyle);
				else
					m_StyleBuffer.AddHead(pStyle);

				// check, if user pressed ESC to cancel
				if (pTheOp && pTheOp->NeedMessages())
				{
					pTheOp->SetPercentDone((int) (dwIndex * 100 / dwSize));

					pTheOp->DoMessages(bAbort);

					if (bAbort)
						AfxThrowUserException();
				}
			}
		}
	}
	CATCH(CMemoryException, e)
	{
		// clean up
		if (pOldCellsArray)
		{
			for (int i = 0; i < pOldCellsArray->GetSize(); i++)
				delete pOldCellsArray->GetAt(i);
		}
		delete pOldCellsArray;
		pOldCellsArray = NULL;

		THROW_LAST();
	}
	AND_CATCH(CUserException, e)
	{
		// user aborted, clean up
		if (pOldCellsArray)
		{
			for (int i = 0; i < pOldCellsArray->GetSize(); i++)
				delete pOldCellsArray->GetAt(i);
		}
		delete pOldCellsArray; 
		pOldCellsArray = NULL;

		THROW_LAST();
	}
	END_CATCH

	return pOldCellsArray;
}


BOOL CGXGridCore::SetStyleRange(
	const CGXRange& range,
	const CGXStyle* pStyle,
	GXModifyType mt,
	int nType /*= 0*/,
	const CObArray* pCellsArray /*= NULL*/,
	UINT flags /*= GX_UPDATENOW*/,
	GXCmdType ctCmd /*= gxDo*/)
{
	// Is it called to execute the command
	if (ctCmd == gxDo)
	{
		// ... then pCellsArray must be 0

		if (pStyle == NULL)
		{
			ASSERT(mt == gxRemove || pCellsArray != NULL);
			// ASSERTION-> Do you want to remove the style or is it a bug that pStyle is NULL? ->END
		}
		else
		{
			ASSERT_VALID(pStyle); ASSERT(pStyle->IsKindOf(RUNTIME_CLASS(CGXStyle)));
			// ASSERTION-> style is invalid ->END

			ASSERT(mt != gxRemove);
			// ASSERTION-> If you want to remove the style, pStyle must be NULL! ->END

			ASSERT(pCellsArray == NULL);
		}
	}
	else // undo, redo or rollback the cmd
	{
		// ... pCellsArray contains cells to be restored, pStyle must be 0

		ASSERT(pStyle == NULL);

		ASSERT(pCellsArray != NULL);
		ASSERT_VALID(pCellsArray);
		ASSERT(pCellsArray->IsKindOf(RUNTIME_CLASS(CObArray)));
	}

	if ((range.IsRows() || range.IsCols() || range.IsTable()) && nType == 0)
		nType = -1;
	
	int nUndoType = nType;

	if (nType == 0)
		nUndoType = m_nExpressionValueType;

	// start op, generate undo info
	CGXLongOperation theOp;

	BOOL bIsMouseAction = GetHitState() > 0;

	theOp.SetStatusText(m_pAppData->strmUndoInfo, FALSE);

	theOp.SetCanRollback(ctCmd == gxDo);

	BOOL bDone = FALSE;

	CObArray* pOldCellsArray = NULL;    // will be filled with style setting

	// generate undo info
	if (m_pUndoImp && ctCmd != gxRollback && m_pParam->m_bUndoEnabled)
	{
		DWORD dwSize = range.GetWidth()*range.GetHeight();

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
				pOldCellsArray = GetStyleArray(range, nUndoType, bIsMouseAction ? NULL : &theOp);
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

	// Now, pOldCellsArray contains the old cell's data

	CGXRange RollbackRange;

	if (ctCmd == gxDo)
		theOp.SetStatusText(m_pAppData->strmStoreCells, FALSE);
	else
		theOp.SetStatusText(m_pAppData->strmRestoreCells, FALSE);

	theOp.SetCanRollback(ctCmd == gxDo && pOldCellsArray != NULL);

	// make sure all cached data are released.
	// BOOL b = m_bLockGDIObjects;
	// m_bLockGDIObjects = TRUE;
	FreeObjectCache();
	// m_bLockGDIObjects = b;

	// bulk add range so that StoreStyleRowCol does not 
	// call AddRecalcRange for each cell.
	if (!m_bLockAddRecalcRange)
		AddRecalcRange(range);

	BOOL bOldLockAddRecalcRange = m_bLockAddRecalcRange;
	m_bLockAddRecalcRange = TRUE;

	// Let engine or CGXData copy cells in memory (and also
	// create undo info for tools and matrix cells)
	BOOL bAdvUndo = SetAdvancedUndo(m_pParam->m_bUndoEnabled && ctCmd != gxRollback);

	DWORD dwSize = range.GetWidth()*range.GetHeight();

	// Latest member in pCellsArray array might contain advanced undo info 
	if (pCellsArray && dwSize < (DWORD) pCellsArray->GetSize())
	{
		CObject* pAdvUndo = (CObject*) pCellsArray->GetAt((int) dwSize);

		// Make sure this is a ObList
		if (pAdvUndo)
		{
			// Let engine handle this advanced undo info and
			// set GX_UPDATE flags for those cells that need
			// repainting
			if (UndoAdvanced(pAdvUndo, ctCmd != gxRollback, FALSE))
				RefreshViews();
		}
	}

	// Store styles
	TRY
	{
		ROWCOL ncRow = GX_MAXROWCOL, ncCol = GX_MAXROWCOL;
		GetCurrentCell(ncRow, ncCol);

		// store styles
		BOOL bAbort = FALSE;
		for (ROWCOL nRow = range.top; nRow <= range.bottom; nRow++)
		{
			for (ROWCOL nCol = range.left; nCol <= range.right; nCol++)
			{
				DWORD dwIndex;

				dwIndex = (nRow-range.top)*range.GetWidth()+(nCol-range.left);

				// process cells array
				if (pCellsArray)
				{
					if (dwIndex >= (DWORD) pCellsArray->GetSize())
						break;

					CGXStyle* pStyle = (CGXStyle*) pCellsArray->GetAt((int) dwIndex);

					if (nType >= 0)
					{
						CGXStyle* pOldStyle = CreateStyle();

						ComposeStyleRowCol(nRow, nCol, pOldStyle);

						// base styles (column, row ?)
						WORD nControlId = pStyle && pStyle->GetIncludeControl()
							? pStyle->GetControl()
							: pOldStyle->GetControl();

						CGXControl* pControl = GetRegisteredControl(nControlId);

						//
						// CGXControl::StoreStyle will call CGXGridCore::StoreStyleRowCol
						// and check if cell is read-only.
						//
						// In OG 1.1, CGXGridCore::StoreStyleRowCol was directly called. This has
						// changed in version 1.2, in order to make the behavior more cell-oriented
						// and allow specific controls to modify the style object before it is stored
						// in the datastructure. The new RTF control will use the passed style object
						// and change the value depending on the new formattings. If the style object
						// is for example "Bold" and StoreStyle is called for an active RTF
						// control, the RTF control will make the paragraph bold (by changing
						// the cell's value which contains rich text formattings).
						//
						// NOTE: The check whether a cell is read-only has been moved from
						// CGXGridCore::StoreStyleRowCol to CGXControl::StoreStyle
						//

						if (pStyle)
							bDone |= pControl->StoreStyle(nRow, nCol, pStyle, mt, nType, pOldStyle);
						else
							bDone |= pControl->StoreStyle(nRow, nCol, NULL, gxRemove, nType, pOldStyle);

						RecycleStyle(pOldStyle);
					}
					else
					{
						if (pStyle)
							bDone |= StoreStyleRowCol(nRow, nCol, pStyle, mt, nType);
						else
							bDone |= StoreStyleRowCol(nRow, nCol, NULL, gxRemove, nType);
					}
				}
				else
				{
					if (nType >= 0)
					{
						if (m_pOldStyle)
						{
							WORD nControlId = pStyle && pStyle->GetIncludeControl()
								? pStyle->GetControl()
								: m_pOldStyle->GetControl();

							CGXControl* pControl = GetRegisteredControl(nControlId);

							bDone |= pControl->StoreStyle(nRow, nCol, pStyle, mt, nType, m_pOldStyle);

						}
						else
						{
							CGXStyle* pOldStyle = CreateStyle();

							ComposeStyleRowCol(nRow, nCol, pOldStyle);

							WORD nControlId = pStyle && pStyle->GetIncludeControl()
								? pStyle->GetControl()
								: pOldStyle->GetControl();

							CGXControl* pControl = GetRegisteredControl(nControlId);

							bDone |= pControl->StoreStyle(nRow, nCol, pStyle, mt, nType, pOldStyle);

							RecycleStyle(pOldStyle);
						}

						if (ncRow == nRow && ncCol == nCol)
						{
							// set internal cached current cell dirty
							m_pStyleEdit->Free();
							m_pCurrentCellControl = NULL;
						}
					}
					else
					{
						// base styles (column, row and table style)

						// ... or one style only
						bDone |= StoreStyleRowCol(nRow, nCol, pStyle, mt, nType);

						if (nRow == 0 && nCol == 0 || nRow == ncRow || nCol == ncCol)
						{
							// set internal cached current cell dirty
							m_pStyleEdit->Free();
							m_pCurrentCellControl = NULL;
						}

					}
				}

				// check, if user pressed ESC to cancel
				if (!bIsMouseAction && theOp.NeedMessages())
				{
					theOp.SetPercentDone((int) (dwIndex * 100 / dwSize));

					theOp.DoMessages(bAbort);

					if (bAbort)
					{
						if (nRow == range.top)
							RollbackRange.SetCells(nRow, range.left, nRow, nCol);
						else
							RollbackRange.SetCells(range.top, range.left, nRow, range.right);

						AfxThrowUserException();
					}
				}
			}
		}

		// update
		if (bDone)
		{
			SetAdvancedUndo(bAdvUndo);

			// Delay float cell evaluation of this range
			DelayFloatCells(range);
			DelayMergeCells(range);

			UpdateStyleRange(range, pStyle, mt, nType, flags, TRUE);

			SetModifiedFlag();

			if (pOldCellsArray)
			{
				// Latest member in pOldCellsArray will be assigned advanced undo info (a CObList with CObjects)
				// that we need to initialize here. 
				CObject* pAdvUndo = GetAdvancedUndoInfo();

				if (pAdvUndo)
					pOldCellsArray->SetAtGrow((int) dwSize, pAdvUndo);

				AddCommand(m_pUndoImp->CreateSetStyleRangeCmd(range, nUndoType, pOldCellsArray), ctCmd);
			}

			if (ctCmd == gxRedo || ctCmd == gxUndo)
				OnCellUndone(range.top, range.left, TRUE);
		}
		else if (pOldCellsArray)
		{
			CleanAdvancedUndo();

			// nothing changed, free undo information
			for (int i = 0; i < pOldCellsArray->GetSize(); i++)
				delete pOldCellsArray->GetAt(i);

			delete pOldCellsArray;
		}

	}
	CATCH(CUserException, e)
	{
		// operation aborted
		// is it possible to rollback the command?
		if (pOldCellsArray && theOp.GetRollbackConfirmedState())
		{	

			if (UndoAdvanced(GetAdvancedUndoInfo(), FALSE, FALSE))
				RefreshViews();

			// Undo
			SetStyleRange(RollbackRange, NULL, gxCopy, nUndoType, pOldCellsArray, flags, gxRollback);

			for (int i = 0; i < pOldCellsArray->GetSize(); i++)
				delete pOldCellsArray->GetAt(i);

			delete pOldCellsArray;
		}
		else if (pOldCellsArray)
		{
			// Latest member in pOldCellsArray will be assigned advanced undo info (a CObList with CObjects)
			// that we need to initialize here. 
			CObject* pAdvUndo = GetAdvancedUndoInfo();

			if (pAdvUndo)
				pOldCellsArray->SetAtGrow((int) dwSize, pAdvUndo);

			AddCommand(m_pUndoImp->CreateSetStyleRangeCmd(range, nUndoType, pOldCellsArray), ctCmd);
		}
		else
		{
			// Undo is not possible
			UpdateStyleRange(RollbackRange, pStyle, mt, nUndoType, flags, TRUE);
		}

		bDone = FALSE;
	}
	END_CATCH

#if defined(_DETECTFALSE) && defined(_DEBUG)
	if (!bDone)
	{
		TRACE0("Warning: SetStyleRange(range,pStyle) returned FALSE.\n");
		TRACE(_T("range: (%lu,%lu,%lu,%lu)\n"), range.top, range.left, range.bottom, range.right);
	}
#endif

	SetAdvancedUndo(bAdvUndo);

	// Reset bulk addrange
	m_bLockAddRecalcRange = bOldLockAddRecalcRange;

	return bDone;
}

BOOL CGXGridCore::StoreStyleRowCol(ROWCOL nRow, ROWCOL nCol, const CGXStyle* pStyle, GXModifyType mt, int nType)
{
	// Unused:
	nType;

	ASSERT(nRow <= GetRowCount() && nCol <= GetColCount());
	// ASSERTION-> cell coordinates out of range ->END

	BOOL bSuccess = FALSE;
	BOOL bReadOnly = FALSE;

	if (pStyle == NULL)
	{
		ASSERT(mt == gxRemove);
		// ASSERTION-> Do you want to remove the style or is it a bug that pStyle is NULL? ->END
	}
	else
	{
		ASSERT_VALID(pStyle); ASSERT(pStyle->IsKindOf(RUNTIME_CLASS(CGXStyle)));
		// ASSERTION-> style is invalid ->END

		ASSERT(mt != gxRemove);
		// ASSERTION-> If you want to remove the style you don't need a style! ->END
	}

	// do not overwrite read-only cells when GetParam()->m_bLockReadOnly is set.
	if (GetParam()->m_bLockReadOnly)
		bReadOnly = IsReadOnly();

	if (GetParam()->m_pData && !bReadOnly)
	{
		bSuccess = GetParam()->m_pData->StoreStyleRowCol(nRow, nCol, pStyle, mt, nType);

		if (!m_bLockAddRecalcRange)
			AddRecalcRange(CGXRange(nRow, nCol));
	}

	if (IsCurrentCell(nRow, nCol))
	{
		// set internal cached current cell dirty
		m_pStyleEdit->Free();
		m_pCurrentCellControl = NULL;
	}


	m_bLockStyleEdit= FALSE;

	return bSuccess;
}

BOOL CGXGridCore::GetStyleRowCol(ROWCOL nRow, ROWCOL nCol, CGXStyle& style, GXModifyType mt, int nType)
{
	// Unused:
	nType;

	if (GetParam()->m_pData)
		return GetParam()->m_pData->GetStyleRowCol(nRow, nCol, style, mt, nType);

	return FALSE;
}

ROWCOL CGXGridCore::GetLastVisibleRow(BOOL* pbLastRowVisible)
{
	if (m_nLastVisibleRow == GX_INVALID || m_nLastVisibleTopRow != m_nTopRow)
	{
		m_nLastVisibleTopRow = m_nTopRow;
		m_nLastVisibleRow = CalcBottomRowFromRect(GetGridRect(), m_bLastRowVisible);

		if (pbLastRowVisible)
			*pbLastRowVisible = m_bLastRowVisible;
	}

	return m_nLastVisibleRow;
}

ROWCOL CGXGridCore::GetLastVisibleCol(BOOL* pbLastColVisible)
{
	if (m_nLastVisibleCol == GX_INVALID || m_nLastVisibleLeftCol != m_nLeftCol)
	{
		m_nLastVisibleLeftCol = m_nLeftCol;
		m_nLastVisibleCol = CalcRightColFromRect(GetGridRect(), m_bLastColVisible);

		if (pbLastColVisible)
			*pbLastColVisible = m_bLastColVisible;
	}

	return m_nLastVisibleCol;
}

void CGXGridCore::UpdateStyleRange(const CGXRange& range, const CGXStyle* pStyle, GXModifyType mt, int nType, UINT flags, BOOL bCreateHint)
{
	CGXRange rgBoundary = range;

	// Float cell state: Check currently visible cells
	if (!IsLockUpdate() && m_pGridWnd->IsWindowVisible())
	{
		if (EvalVisibleFloatMergeCells(range,   FALSE, &rgBoundary, FALSE))
			rgBoundary.UnionRange(range, rgBoundary);

		RedrawRowCol(rgBoundary, flags, FALSE);
	}

	// Create Hint
	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintUpdateStyleRange(this, rgBoundary, pStyle, mt, nType, flags);
}

void CGXGridCore::ComposeStyleRowCol(ROWCOL nRow, ROWCOL nCol, CGXStyle* pStyle, BOOL bApplyStandard)
{
	ASSERT(nRow <= GetRowCount() && nCol <= GetColCount());
	// -> ASSERTION Coordinates out of range ->END

//  TRACE2("ComposeStyleRowCol(%ld, %ld)\n", nRow, nCol);

	_TC_(DWORD ti2 = GetTickCount());
	GetStyleRowCol(nRow, nCol, *pStyle, gxCopy, 0);
	_TC_(tick2 += GetTickCount()-ti2);

	// Check if you only need the value
	if (m_bOnlyValueNeeded && pStyle->GetIncludeValue())
		return;

	_TC_(DWORD ti3 = GetTickCount());

	ROWCOL nhCols = GetHeaderCols(),
		   nhRows = GetHeaderRows();

	// Row and Column styles

	// Apply Row style
	if (nCol > nhCols)
		GetStyleRowCol(nRow, 0, *pStyle, gxApplyNew, -1);   // -1 defines range instead of cell value

	// Apply Column style
	if (nRow > nhRows)
		GetStyleRowCol(0, nCol, *pStyle, gxApplyNew, -1);

	// Apply table style
	if (nRow > nhRows && nCol > nhCols)
		GetStyleRowCol(0, 0, *pStyle, gxApplyNew, -1);

	_TC_(tick3 += GetTickCount()-ti3);

	// inherit attributes from base styles
	_TC_(DWORD ti4 = GetTickCount());
	pStyle->LoadBaseStyle(*m_pParam->m_pStylesMap);
	_TC_(tick4 += GetTickCount()-ti4);

	_TC_(DWORD ti5 = GetTickCount());
	// column headers and row headers
	if (nRow <= nhRows)
		pStyle->ChangeStyle(*m_pStyleColHeader, gxApplyNew);

	if (nCol <= nhCols)
		pStyle->ChangeStyle(*m_pStyleRowHeader, gxApplyNew);
	_TC_(tick5 += GetTickCount()-ti5);

	_TC_(DWORD ti6 = GetTickCount());
	// set value, if column or row header
	if ((nRow == 0 || nCol == 0) && !pStyle->GetIncludeValue())
	{
		if (nRow == 0 && nCol > 0 && GetParam()->IsNumberedColHeaders())
		{
			// Column header in format A, B, ..., AA, ..., AAA, ... ZZZZZ
			
			TCHAR s[10], t[10];
			int n;
			for (n = 0; n < 9; n++)
			{
				nCol--;
				s[n] = (TCHAR) ((nCol%26) + _T('A'));
				if ((nCol = nCol/26) == 0)
					break;
			}
			
			// reverse string
			int i;
			for ( i = 0; i < n+1; i++)
				t[i] = s[n-i];
			t[i] = 0;
			
			pStyle->SetValue(t);
		}
		else if (nCol == 0 && nRow > 0 && GetParam()->IsNumberedRowHeaders())
		{
			// Row header is simple a number
			pStyle->SetValue(nRow);
		}
	}
	_TC_(tick6 += GetTickCount()-ti6);

	_TC_(DWORD ti7 = GetTickCount());
	if (bApplyStandard)
		pStyle->ChangeStyle(*m_pStyleStandard, gxApplyNew);

	if (GetParam()->m_bDirectCutPaste && GetParam()->m_bCut && GetParam()->m_CopyRangeList.IsCellInList(nRow, nCol))
	{
		pStyle->SetInterior(CGXBrush().SetPattern(17).SetColor(RGB(192,192,192)));
	}

	_TC_(tick7 += GetTickCount()-ti7);
}

const CString& CGXGridCore::GetValueRowCol(ROWCOL nRow, ROWCOL nCol)
{
	m_pStyleGetValue->SetIncludeValue(FALSE);

	m_bOnlyValueNeeded = TRUE;
		// you can check m_bOnlyValueNeeded this attribute in
		// your derived GetStyleRowCol method for performance reasons
		// It is TRUE, when only the value for a cell is needed and
		// not any other formattings

	ComposeStyleRowCol(nRow, nCol, m_pStyleGetValue);

	m_bOnlyValueNeeded = FALSE;

	return m_pStyleGetValue->GetValueRef();
}

const CString& CGXGridCore::GetExpressionRowCol(ROWCOL nRow, ROWCOL nCol)
{ 
	m_pStyleGetValue->SetIncludeValue(FALSE);

	m_bOnlyValueNeeded = TRUE;
		// you can check m_bOnlyValueNeeded this attribute in
		// your derived GetStyleRowCol method for performance reasons
		// It is TRUE, when only the value for a cell is needed and
		// not any other formattings

	GetStyleRowCol(nRow, nCol, *m_pStyleGetValue, gxOverride, m_nExpressionValueType);

	m_bOnlyValueNeeded = FALSE;

	return m_pStyleGetValue->GetValueRef();
}

// Styles Map

BOOL CGXGridCore::ChangeStylesMap(const CGXStylesMap& newStylesMap, UINT flags, GXCmdType ctCmd)
{
	if (GetParam()->IsLockReadOnly() && IsReadOnly())
		return FALSE;

	ASSERT(GetParam()->GetStylesMap());

	CGXStylesMap& stylesmap = *GetParam()->GetStylesMap();
	
	if (m_pUndoImp && ctCmd != gxRollback && m_pParam->m_bUndoEnabled)
		AddCommand(m_pUndoImp->CreateChangeStylesMapCmd(stylesmap), ctCmd);

	stylesmap = newStylesMap;

	SetModifiedFlag();
	DelayFloatCells(CGXRange().SetTable());

	UpdateStylesMap(flags);

	return TRUE;
}

void CGXGridCore::UpdateStylesMap(UINT flags, BOOL bCreateHint)
{
	CGXStylesMap& stylesmap = *GetParam()->GetStylesMap();

	// initialize standard Styles
	stylesmap.LookupStyle(stylesmap.m_wStyleStandard, m_pStyleStandard);
	stylesmap.LookupStyle(stylesmap.m_wStyleRowHeader, m_pStyleRowHeader);
	stylesmap.LookupStyle(stylesmap.m_wStyleColHeader, m_pStyleColHeader);

	Redraw(flags, FALSE);

	// Create Hint
	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintUpdateStylesMap(this, flags);
}

// Properties

BOOL CGXGridCore::ChangeProperties(const CGXProperties& properties, UINT flags, GXCmdType ctCmd)
{
	if (GetParam()->IsLockReadOnly() && IsReadOnly())
		return FALSE;

	CGXProperties* pPropertyObj = GetParam()->GetProperties();
	ASSERT(pPropertyObj->IsKindOf(RUNTIME_CLASS(CGXProperties)));

	if (m_pUndoImp && ctCmd != gxRollback && m_pParam->m_bUndoEnabled)
		AddCommand(m_pUndoImp->CreateChangePropertiesCmd(*pPropertyObj), ctCmd);

	*pPropertyObj = properties;
	SetModifiedFlag();

	UpdateProperties(flags);

	return TRUE;
}

void CGXGridCore::UpdateProperties(UINT flags, BOOL bCreateHint)
{
	Redraw(flags, FALSE);

	// Create Hint
	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintUpdateProperties(this, flags);
}

// Base Style

BOOL CGXGridCore::ChangeStandardStyle(const CGXStyle& style, GXModifyType mt, UINT flags, GXCmdType ctCmd)
{
	return ChangeBaseStyle(GetParam()->GetStylesMap()->m_wStyleStandard, style, mt, flags, ctCmd);
}

BOOL CGXGridCore::ChangeRowHeaderStyle(const CGXStyle& style, GXModifyType mt, UINT flags, GXCmdType ctCmd)
{
	return ChangeBaseStyle(GetParam()->GetStylesMap()->m_wStyleRowHeader, style, mt, flags, ctCmd);
}

BOOL CGXGridCore::ChangeColHeaderStyle(const CGXStyle& style, GXModifyType mt, UINT flags, GXCmdType ctCmd)
{
	return ChangeBaseStyle(GetParam()->GetStylesMap()->m_wStyleColHeader, style, mt, flags, ctCmd);
}

BOOL CGXGridCore::ChangeBaseStyle(WORD wStyleID, const CGXStyle& style, GXModifyType mt, UINT flags, GXCmdType ctCmd)
{
	CGXGridParam* pParam = GetParam();

	ASSERT(pParam != NULL && pParam->GetStylesMap() != NULL);
	// ASSERTION-> Don't call this method before 
	// you have attached a parameter and a stylesmap object
	// to the grid ->END

	if (pParam == NULL || pParam->GetStylesMap() == NULL 
		|| pParam->IsLockReadOnly() && IsReadOnly())
		return FALSE;

	CGXStylesMap& stylesmap = *GetParam()->GetStylesMap();
	CGXStyle& basestyle = BaseStyle(wStyleID);

	if (m_pUndoImp && m_bInitDone && ctCmd != gxRollback && m_pParam->m_bUndoEnabled)
		AddCommand(m_pUndoImp->CreateChangeBaseStyleCmd(
			wStyleID, basestyle, stylesmap.GetStyleName(wStyleID)), ctCmd);
			
	basestyle.ChangeStyle(style, mt);

	SetModifiedFlag();
	DelayFloatCells(CGXRange().SetTable());

	UpdateBaseStyle(wStyleID, flags);

	return TRUE;
}

void CGXGridCore::UpdateBaseStyle(WORD wStyleId, UINT flags, BOOL bCreateHint)
{
	CGXStylesMap& stylesmap = *GetParam()->GetStylesMap();

	if (m_bInitDone)
	{
		// initialize standard Styles
		stylesmap.LookupStyle(stylesmap.m_wStyleStandard, m_pStyleStandard);
		stylesmap.LookupStyle(stylesmap.m_wStyleRowHeader, m_pStyleRowHeader);
		stylesmap.LookupStyle(stylesmap.m_wStyleColHeader, m_pStyleColHeader);

		ReleaseGridDC();  // make sure dc does not use the old font any more
		UpdateFontMetrics();
		Redraw(flags, FALSE);
	}

	// Create Hint
	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintUpdateBaseStyle(this, wStyleId, flags);
}

BOOL CGXGridCore::SetControlTextRowCol(ROWCOL nRow, ROWCOL nCol, const CString& str, UINT nFlags, const CGXStyle* pOldStyle)
{
	// override this method if you want to validate (or change) the text
	// which should be displayed in the cell.
	//
	// pControl->SetControlText will convert the text into the raw
	// value which should be stored in the cell and will then
	// call SetControlValue to store that value in the grid
	//
	// NOTE: If you want to validate the value as it will be 
	// stored in the cell, override StoreStyleRowCol

	CGXStyle* pStyle = NULL;
	
	if (pOldStyle == NULL)
	{
		pStyle = CreateStyle();
		ComposeStyleRowCol(nRow, nCol, pStyle);
		pOldStyle = pStyle;
	}

	CGXControl* pControl = GetRegisteredControl(pOldStyle->GetControl());

	// Give the control the chance to validate
	// and change the pasted text
	BOOL bRet = pControl->SetControlText(nRow, nCol, str, nFlags, pOldStyle);

	if (pStyle)
		RecycleStyle(pStyle);

	return bRet;
}

