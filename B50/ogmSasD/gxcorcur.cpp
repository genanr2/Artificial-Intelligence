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

// gxcorcur.cpp : current cell (CGXGridCore)
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

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCORCUR")
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
// Current Cell

BOOL CGXGridCore::SetCurrentCell(ROWCOL nRow, ROWCOL nCol, UINT flags /* = GX_UPDATENOW */)
{
	// Allow grid to verify grid dimension first, so that
	// GetRowCount()/GetColCoun() return not too small value
	// in ODBC/DAO (or other data source) environment.
	OnTestGridDimension(nRow, nCol);

	// Position EditCell
#ifdef _DEBUG
	if (nRow != GX_INVALID && nCol != GX_INVALID)
	{
		ASSERT(nRow >= 0 && nRow <= GetRowCount());
		ASSERT(nCol >= 0 && nCol <= GetColCount());
		// ASSERTION-> Coordinates out of range ->END
	}
#endif

	// TRACE3("CGXGridCore: BEGIN SetEditCell((%ld,%ld,%05x)\n", nRow, nCol, flags);
	// TRACE2("  Old Edit: (%ld,%ld)\n", m_nEditRow, m_nEditCol);

	// Is it a covered cell?
	CGXRange rgCovered;
	if (GetCoveredCellsRowCol(nRow, nCol, rgCovered))
	{
		if (rgCovered.top != nRow || rgCovered.left != nCol)
		{
			nRow = rgCovered.top;
			nCol = rgCovered.left;
		}
	}

	// check out direction
	int direction;
	CGXControl* pOldControl = NULL;
	ROWCOL nEditRow = 0, nEditCol = 0;

	if (GetCurrentCell(nEditRow, nEditCol))
		pOldControl = GetControl(nEditRow, nEditCol);

	if (pOldControl == NULL || nEditCol <= GetHeaderCols() || nEditRow <= GetHeaderRows())
		direction = 0;
	else if (nEditRow == nRow && nEditCol == nCol)
		direction = (nRow == 0 || nCol == 0)?0:-1;
	else
		direction = (nRow==nEditRow)?GX_HORZ:(nCol==nEditCol)?GX_VERT:0;

	BOOL bAbort = FALSE;

	// deactivate current cell
	if (pOldControl)
	{
		// Retrieve Text and Hide Edit Window
		CString sEditText;
		BOOL bModified = pOldControl->GetModify();
		if (pOldControl->IsInit())
			pOldControl->GetCurrentText(sEditText);

		// ok, first I check if I can leave the current cell.
		// CanSelectCurrentCell() will call CGXControl::OnValidate()

		// The programmer should either override CanSelectCurrentCell
		// or CGXControl::OnValidate to inhibit that the user leaves the cell.

		m_bLockActivateGrid = TRUE;
		m_bKillFocusCalled = FALSE;

		HWND hWndFocus = ::GetFocus();

		// Remove any pendig Keyboard messages for the active child window
		if (pOldControl->IsActive() && IsChild(m_pGridWnd->m_hWnd, hWndFocus))
		{
			MSG msg;
			while (::PeekMessage(&msg,  // pointer to structure for message
				hWndFocus,  // handle to window
				WM_KEYFIRST, // first message
				WM_KEYLAST, // last message
				PM_REMOVE)  // removal flags
				)
				NULL;
		}

		if (CanSelectCurrentCell(FALSE, nRow, nCol, nEditRow, nEditCol))
			// Note: CanSelectCurrentCell calls CGXControl::OnValidate
		{
			// CanSelectCurrentCell returned TRUE, so I try to store the cell

			// If the cell cannot be stored, (e.g. StoreStyleRowCol returned FALSE)
			// or if OnEndEditing returns FALSE, I try to restore
			// the previous state of the cell.

			BOOL bSuccess = TRUE;
			BOOL bStored = FALSE;

			ResetError();

			// Try storing the cell
			if (bModified)
			{
				bStored = pOldControl->Store();
				bSuccess &= bStored;
				// Use ASSERT only when creating new cell types 
				if (!pOldControl->GetModify())
					TRACE(_T("Warning: Control's Store method did reset the modify flag.\n"));
				// ASSERT(pOldControl->GetModify());
			}

			// Call OnEndEditing after cell has been stored
			if (bSuccess && pOldControl->IsInit())
			{
				bSuccess &= pOldControl->OnEndEditing();

				if (bModified)
				{
					Recalc();

					RefreshViews(FALSE, NULL, TRUE);
				}
			}

			if (m_sWarningText.IsEmpty())
			{
				GetError(m_sWarningText);
				ResetError();
			}

			if (m_bKillFocusCalled)
				m_bWarningTextDisplayed = TRUE;

			// check if I can deactivate the old cell.
			if (bSuccess)
			{
				// Mark it hidden ...
				StoreCurrentCell(FALSE);

				if (pOldControl->IsInit())
					pOldControl->Hide();    // will call ShowWindow(SW_HIDE)

				pOldControl->SetActive(FALSE);

				BOOL bNeedRedraw = pOldControl->OnLeftCell(nRow, nCol);
					// OnLeftCell() normally returns TRUE. If you
					// want to hook into redrawing the cell, you can
					// override OnLeftCell() and return FALSE;

				BOOL bOld = m_bLockStyleEdit;

				if (!m_sWarningText.IsEmpty())
					DisplayWarningText();

				// ... redraw it
				if (bNeedRedraw)
				{
					m_bLockStyleEdit = TRUE;    // don't reload edit cell

					MarkEdit(nEditRow, nEditCol, direction, flags);

					// if  (GetParam()->GetHideCurrentCell() != GX_HIDE_ALWAYS)
					{
						if (bStored)
							// update all views with change
							RedrawRowCol(nEditRow, nEditCol, flags);
						else
							// only redraw the cell in the current view
							pOldControl->Refresh();
					}
				}

				// Reset it
				pOldControl->Reset();

				if (flags & (GX_UPDATENOW|GX_SMART))
					UpdateWindow();

				m_bLockStyleEdit = bOld;
			}
			else
			{
				// either the cell could not be stored or
				// OnEndEditing returned FALSE.

				if (m_sWarningText.IsEmpty())
				{
					if (pOldControl->IsActive()
						&& ::IsWindow(hWndFocus)
						&& ::IsWindowVisible(hWndFocus))
						::SetFocus(hWndFocus);

					if (m_pGridWnd->m_hWnd == hWndFocus || ::IsChild(m_pGridWnd->m_hWnd, hWndFocus))
						m_bActiveFrame = TRUE;
				}

				DisplayWarningText();

				// restore previous state of the cell

				StoreCurrentCell(TRUE, nEditRow, nEditCol);

				pOldControl->Hide();

				if (!pOldControl->IsInit())
					pOldControl->Init(nEditRow, nEditCol);

				pOldControl->SetActive(TRUE);
				pOldControl->SetModify(TRUE);
				pOldControl->SetCurrentText(sEditText);

				if (bStored)
					// update all views with change
					RedrawRowCol(nEditRow, nEditCol, flags);
				else
					// only redraw the cell in the current view
					pOldControl->Refresh(); // will set focus to control

				bAbort = TRUE;
			}
		}
		else
		{
			if (m_sWarningText.IsEmpty())
			{
				if (pOldControl->IsActive()
					&& ::IsWindow(hWndFocus)
					&& ::IsWindowVisible(hWndFocus))
					::SetFocus(hWndFocus);

				if (m_pGridWnd->m_hWnd == hWndFocus || ::IsChild(m_pGridWnd->m_hWnd, hWndFocus))
					m_bActiveFrame = TRUE;
			}

			// possibly display a message box
			DisplayWarningText();
			ASSERT(IsCurrentCell());
			bAbort = TRUE;      // ... could not deactivate it
			pOldControl->Refresh(); // will set focus to control
		}

		m_bLockActivateGrid = FALSE;
	}
	else
	{
		nEditRow = nEditCol = 0;
	}

	if (!bAbort)
		bAbort = (nRow > GetRowCount() || nCol > GetColCount());

	// activate new cell (only if nRow, nCol is valid)
	if (!bAbort)
	{
		// check, if cell was already active and had the input focus
		if (pOldControl && nEditRow == nRow && nEditCol == nCol && pOldControl->IsActive())
		{
			// so, I set the marked text and scroll the cell into the view
			if (!ScrollCellInView(nRow, nCol, flags))
				pOldControl->Refresh();
		}
		else
		{
			if (pOldControl && nEditRow == nRow && nEditCol == nCol
				 || CanSelectCurrentCell(TRUE, nRow, nCol, nEditRow, nEditCol))
			{
				CGXControl* pControl = GetControl(nRow, nCol);

				if (IsLockUpdate())
				{
					StoreCurrentCell(TRUE, nRow, nCol);
					pControl->Init(nRow, nCol);
				}
				else
				{
					BOOL bInvert = GetInvertStateRowCol(nRow, nCol, GetParam()->GetRangeList());

					// Force that grid gets the focus, unless GX_NOSETFOCUS
					// has been speicified.
					if (!(flags & GX_NOSETFOCUS) && m_pGridWnd->IsWindowVisible())
					{
						SetIgnoreFocus(TRUE);
						m_pGridWnd->SetFocus();
						SetIgnoreFocus(FALSE);
					}

					// Sets m_bActivateEditCell, m_nEditRow, m_nEditCol and m_StyleEdit
					StoreCurrentCell(TRUE, nRow, nCol);

					pControl->Init(nRow, nCol);

					if (flags & GX_DISPLAYEDITWND)
						pControl->SetActive(TRUE);

					if (GetParam()->GetHideCurrentCell() != GX_HIDE_ALWAYS)
					{
						// NOTE: The following 15 lines of code are
						// responsible for outlining the new current
						// cell. In general this means that there should
						// be a border drawn around the cell.
						//
						// Some background:
						// When the user presses the mouse button
						// on a new cell, SetCurrentCell will be called
						// and the cell will be redrawn static.
						//
						// When the user releases the mouse button, the
						// controls LButtonUp method will be called which
						// could result in redrawing the cell again (e.g.
						// a checkbox will redraw again because the value
						// changes, or if you clicked into the text, the
						// control will be redrawn active).
						//
						// When the current cell is moved with arrow keys,
						// there will always be only one call to SetCurrentCell.
						//
						// If either CGXGridCore::m_bRefreshOnSetCurrentCell or
						// CGXControl::m_bRefreshOnSetCurrentCell is TRUE,
						// the cell will be refreshed (= completely redrawn).
						//
						// Otherwise the grid tries a smarter redrawing
						// of the cell without need to fully redraw it.
						// This reduces flickering if the cells apperance
						// does not change whether it is the current cell
						// or not, because then only the border needs to be drawn.
						//
						// If you have a control which appearence is
						// depending on wheter it is a current cell
						// or not, you should set m_bRefreshOnSetCurrentCell
						// = TRUE for the specific control.

						// Check, if refreshing the cell is the prefered way
						if (pControl->m_bRefreshOnSetCurrentCell
							|| m_bRefreshOnSetCurrentCell
							|| pControl->IsActive()
							|| nEditRow == nRow && nEditCol == nCol
							|| GetParam()->GetActivateCellFlags() == GX_CAFOCUS_SETCURRENT
							)
						{
							pControl->Refresh();
						}
						// if it is not necessary, we can use a smarter
						// way to outline the cell without refreshing it.
						else if (nEditRow != nRow || nEditCol != nCol)
						{
							CClientDC dc(m_pGridWnd);
							dc.IntersectClipRect(GetGridRect());

							if (bInvert && GetParam()->GetHideCurrentCell() != GX_HIDE_ALWAYS)
								m_bInvertRect = TRUE, DrawInvertCell(&dc, nRow, nCol, CalcRectFromRowCol(nRow, nCol));

							CRect rect = CalcRectFromRowColEx(nRow, nCol);

							if (!rect.IsRectEmpty())
								pControl->InvertBorders(&dc, SubtractBorders(rect, LookupStyleRowCol(nRow, nCol)));
						}
					}

					// Scroll cell into view
					if (flags & GX_SCROLLINVIEW /*&& m_pGridWnd->IsWindowVisible()*/)
						ScrollCellInView(nRow, nCol, flags);
				}
			}
			else
			{
				// could not activate cell
				if (pOldControl
					&& CanSelectCurrentCell(TRUE, nEditRow, nEditCol, 0,0))
				{
					// Store the coordinates for the current cell
					StoreCurrentCell(TRUE, nEditRow, nEditCol);
					MarkEdit(nEditRow, nEditCol, 0, GX_UPDATENOW);
					CGXControl* pControl = GetControl(nEditRow, nEditCol);
					// ... and Initialize it
					pControl->Init(nEditRow, nEditCol);
					pControl->Refresh();
				}
				bAbort = TRUE;
			}
		}
	}

	// Update display
	if (!bAbort && flags & (GX_UPDATENOW|GX_SMART))
		UpdateWindow();

	// Mark row and column headers
	if (!bAbort && direction != -1)
		MarkEdit(nRow, nCol, direction, flags);

	// Synchronize current cell with other views.
	// CGXBrowserGrid uses this feature.
	BOOL bCreateHint = GetParam()->m_bSyncCurrentCell && !(flags & GX_NOSYNCCURRENTCELL);

	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintSetCurrentCell(this, nRow, nCol, (flags | GX_NOSETFOCUS | GX_NOSYNCCURRENTCELL | GX_NOSELECTRANGE) & ~GX_SCROLLINVIEW);

	// EXCELCURCELL
	if (GetParam()->GetExcelLikeCurrentCell()
		&& !bAbort && (flags&GX_NOSELECTRANGE) == 0)
	{
		if (!GetParam()->GetRangeList()->IsCellInList(nRow, nCol))
		{
			SetSelection(NULL);
			if (nRow > GetHeaderRows() && nCol > GetHeaderCols())
				SelectRange(CGXRange(nRow, nCol));
		}
		else
		{
		}
	}

	// TRACE3("CGXGridCore: END   SetEditCell((%lu,%lu,%05x)\n", nRow, nCol, flags);
	// TRACE1(" RET %d\n", !bAbort);

	return !bAbort;
}

BOOL CGXGridCore::TransferCurrentCell(BOOL bSaveAndValidate, UINT flags, BOOL bCreateHint)
{
	ROWCOL nRow, nCol;
	BOOL bRet = TRUE;
	if (GetCurrentCell(nRow, nCol))
	{
		if (bSaveAndValidate)
		{
			CGXControl* pControl = GetControl(nRow, nCol);
			if (pControl->IsActive())
				// Store current cell's contents and deactivate cell
				bRet = SetCurrentCell(nRow, nCol, flags);

			if (!m_bLockStyleEdit)
			{
				m_pStyleEdit->Free();
				m_pCurrentCellControl = NULL;
			}
		}
		else
		{
			if (!m_bLockStyleEdit)
			{
				m_pStyleEdit->Free();
				m_pCurrentCellControl = NULL;
			}

			CGXControl* pControl = GetControl(nRow, nCol);
			if (pControl->IsActive())
				bRet = pControl->OnCancelEditing();

			// Reinitialize current cell
			pControl->Init(nRow, nCol);
			pControl->Refresh();
			pControl->OnCanceledEditing();
		}
	}

	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		bRet &= m_pUpdateHintImp->HintTransferCurrentCell(this, bSaveAndValidate, flags);

	return bRet;
}

BOOL CGXGridCore::CanSelectCurrentCell(BOOL bSelect, ROWCOL dwSelectRow, ROWCOL dwSelectCol, ROWCOL dwOldRow, ROWCOL dwOldCol)
{
	// check, if cell can be deactivated or activated

	BOOL bDone = TRUE;

	if (bSelect && (dwSelectRow > GetRowCount() || dwSelectCol > GetColCount()))
		return FALSE;

	if (!bSelect)
	{
		CGXControl* pControl = GetControl(dwOldRow, dwOldCol);

		// Remove warning text
		SetWarningText(NULL);

		if (pControl->GetModify())
			bDone = pControl->OnValidate();

		// NOTE: You should override OnEndEditing() if you want to
		// validate non-modified cells! Changing this behavior
		// now by removing the line
		//      if (pControl->GetModify())
		// could cause problems with existing apps which
		// rely on the existing behavior that OnValidate() is
		// only called for active cells!
	}

	if (bSelect && bDone && dwSelectRow <= GetRowCount() && dwSelectCol <= GetColCount())
	{
		const CGXStyle& style = LookupStyleRowCol(dwSelectRow, dwSelectCol);
		if (style.GetIncludeEnabled() && !style.GetEnabled())
			bDone = FALSE;
	}

	return bDone;
}

BOOL CGXGridCore::GetCurrentCell(ROWCOL *pnRow, ROWCOL *pnCol, BOOL bIgnorePrint /*=FALSE*/)
{
	ASSERT(pnRow && pnCol);
	*pnRow = m_nEditRow;
	*pnCol = m_nEditCol;

	m_bActiveEditCell &= m_nEditRow <= GetRowCount() && m_nEditCol <= GetColCount();

	// nomally, it returns NULL when print-modus is active
	return m_bActiveEditCell
		&& (bIgnorePrint || !IsPrinting());
}

void CGXGridCore::StoreCurrentCell(BOOL bActive, ROWCOL nRow /*= 0*/, ROWCOL nCol /*= 0*/)
{
	if (bActive)
	{
		m_bActiveEditCell = FALSE;
		CGXControl* pControl = GetControl(nRow, nCol);

		// Cache the control
		m_pCurrentCellControl = pControl;
	}

	// Change m_bActivateEditCell, m_nEditRow, m_nEditCol and m_StyleEdit
	m_bActiveEditCell = bActive;
	if (bActive)
	{
		m_nEditRow = m_nEditCol = 0;

		// Reset StyleEdit
		m_pStyleEdit->Free();
		// Next line will initialize m_StyleEdit
		LookupStyleRowCol(nRow, nCol);

		m_nEditRow = nRow;
		m_nEditCol = nCol;

		// Also store coordinates in parameter object,
		// so that when a new view is opened on the same data
		// the current cell can easily be determined and
		// synchronized. See also CGXGridParam::SetSyncCurrentCell
		GetParam()->m_nLastSyncEditRow = nRow;
		GetParam()->m_nLastSyncEditCol = nCol;
	}
	// ... Change gets visible with next call to the control's Draw method
}

BOOL CGXGridCore::IsCurrentCell(ROWCOL nRow, ROWCOL nCol, BOOL bIgnorePrint /*=FALSE*/)
{
	if (!m_bActiveEditCell
		|| IsPrinting() && !bIgnorePrint)
		return FALSE;

	if ((nRow == GX_INVALID || nRow == m_nEditRow)
		&& (nCol == GX_INVALID || nCol == m_nEditCol)
		&& m_nEditRow <= GetRowCount()
		&& m_nEditCol <= GetColCount())
		return TRUE;

	return FALSE;
}

BOOL CGXGridCore::IsActiveCurrentCell()
{
	// check out, if EditWnd is visible
	CGXControl* pControl = GetCurrentCellControl();
	return
		!IsPrinting()
		&& pControl
		&& pControl->IsActive();
}

CGXControl* CGXGridCore::GetCurrentCellControl()
{
	ROWCOL nRow, nCol;
	if (!GetCurrentCell(nRow, nCol))
		return NULL;

	return GetControl(nRow, nCol);
}

CGXControl* CGXGridCore::GetControl(ROWCOL nRow, ROWCOL nCol)
{
	// Try if we can use the cached pointer for the current cells control
	if (nRow == m_nEditRow && nCol == m_nEditCol && m_pCurrentCellControl)
		return m_pCurrentCellControl;

	const CGXStyle& style = LookupStyleRowCol(nRow, nCol);

	return GetRegisteredControl(style.GetControl());
}

void CGXGridCore::HideCurrentCell()
{
	// Retrieve Text and Hide Edit Window
	CString sEditText;

	ROWCOL nRow, nCol;
	if (GetCurrentCell(nRow, nCol))
	{
		CGXControl* pControl = GetControl(nRow, nCol);
		pControl->Hide();
	}
}

void CGXGridCore::ResetCurrentCell(BOOL bStoreIfModified)
{
	// Retrieve Text and Hide Edit Window
	CString sEditText;

	ROWCOL nRow, nCol;
	if (GetCurrentCell(nRow, nCol))
	{
		CGXControl* pControl = GetControl(nRow, nCol);
		if (bStoreIfModified
			&& CanSelectCurrentCell(FALSE, nRow, nCol, nRow, nCol)
			&& pControl->GetModify())
		{
			pControl->Store();
		}

		pControl->Reset();
	}
}

BOOL CGXGridCore::MoveCurrentCellEx(int direction, UINT nCell /* = 1 */)
{
	if (m_pUserSelectRangeImp)
		return m_pUserSelectRangeImp->MoveCurrentCellEx(this, direction, nCell);

	// else
	BOOL bRet = MoveCurrentCell(direction, nCell);
	m_SelRectId = 0;

	return bRet;
}

// Find Cell determines if the current cell can be positioned
// onto (nSetRow,nSetCol). If not, it will search the next
// possible cell in the given directeion.
BOOL CGXGridCore::FindNextCell(int direction, ROWCOL& nSetRow, ROWCOL& nSetCol)
{
	// TRACE0("CGXGridCore::FindNextCell()\n");

	ROWCOL nCount;
	CGXRange rgCovered;
	const CGXStyle* pStyle;

	// NOTE: I call LookupStyleRowCol below instead of ComposeStyleRowCol
	// because the next time OnDrawTopLeftBottomRight is called, all
	// style objects can be reused again and don't have to be reloaded.

	ROWCOL nNewRow = nSetRow;
	ROWCOL nNewCol = nSetCol;
	BOOL bFound = FALSE;

	switch (direction)
	{
	case GX_UP:
		while (nNewRow >= 1 && !bFound)
		{
			// skip invisible and covered cells
			while (nNewRow >= 1
				&& (GetRowHeight(nNewRow) == 0
					|| GetCoveredCellsRowCol(nNewRow, nNewCol, rgCovered)
					&& rgCovered.top != nNewRow
					)
				)
				nNewRow--;

			if (nNewRow >= 1)
			{
				// determine style for the cell
				if (GetCoveredCellsRowCol(nNewRow, nNewCol, rgCovered))
					pStyle = &LookupStyleRowCol(rgCovered.top, rgCovered.left);
				else
					pStyle = &LookupStyleRowCol(nNewRow, nNewCol);

				// cell ok?
				bFound = pStyle->GetEnabled();
			}

			// ... no, I do continue search
			if (!bFound && nNewRow >= 1)
				nNewRow--;
		}
		break;

	case GX_DOWN:
		nCount = GetRowCount();
		while (nNewRow <= nCount && !bFound)
		{
			// skip invisible and covered cells
			while (nNewRow <= nCount
				&& (GetRowHeight(nNewRow) == 0
					|| GetCoveredCellsRowCol(nNewRow, nNewCol, rgCovered)
					&& rgCovered.top != nNewRow
					)
				)
				nNewRow++;

			if (nNewRow <= nCount)
			{
				// determine style for the cell
				if (GetCoveredCellsRowCol(nNewRow, nNewCol, rgCovered))
					pStyle = &LookupStyleRowCol(rgCovered.top, rgCovered.left);
				else
					pStyle = &LookupStyleRowCol(nNewRow, nNewCol);

				// cell ok?
				bFound = pStyle->GetEnabled();

				// ... no, I do continue search
				if (!bFound)
					nNewRow++;
			}
		}
		break;

	case GX_LEFT:
		while (nNewCol >= 1 && !bFound)
		{
			// skip invisible and covered cells
			while (nNewCol >= 1
				&& (GetColWidth(nNewCol) == 0
					|| GetCoveredCellsRowCol(nNewRow, nNewCol, rgCovered)
					&& rgCovered.left != nNewCol
					)
				)
				nNewCol--;

			if (nNewCol >= 1)
			{
				// determine style for the cell
				if (GetCoveredCellsRowCol(nNewRow, nNewCol, rgCovered))
					pStyle = &LookupStyleRowCol(rgCovered.top, rgCovered.left);
				else
					pStyle = &LookupStyleRowCol(nNewRow, nNewCol);

				// cell ok?
				bFound = pStyle->GetEnabled();

				// ... no, I do continue search
				if (!bFound && nNewCol >= 1)
					nNewCol--;
			}
		}
		break;

	case GX_RIGHT:
		nCount = GetColCount();
		while (nNewCol <= nCount && !bFound)
		{
			// skip invisible and covered cells
			while (nNewCol <= nCount
				&& (GetColWidth(nNewCol) == 0
					|| GetCoveredCellsRowCol(nNewRow, nNewCol, rgCovered)
					&& rgCovered.left != nNewCol
					)
				)
				nNewCol++;

			if (nNewCol <= nCount)
			{
				// determine style for the cell
				if (GetCoveredCellsRowCol(nNewRow, nNewCol, rgCovered))
					pStyle = &LookupStyleRowCol(rgCovered.top, rgCovered.left);
				else
					pStyle = &LookupStyleRowCol(nNewRow, nNewCol);

				// cell ok?
				bFound = pStyle->GetEnabled();

				// ... no, I do continue search
				if (!bFound)
					nNewCol++;
			}
		}
		break;
	}

	if (bFound)
	{
		nSetRow = nNewRow;
		nSetCol = nNewCol;
	}

	m_bNoValueNeeded = FALSE;

	// TRACE0("End CGXGridCore::FindNextCell()\n");

	return bFound;
}

BOOL CGXGridCore::MoveCurrentCell(int direction, UINT nCell /* = 1 */, BOOL bCanSelectRange)
{
	// Move the edit cell in the given direction
	ROWCOL wCell = (ROWCOL) max(nCell, 1);

	ASSERT(m_nLeftCol > 0);
	ASSERT(m_nTopRow > 0);
	ASSERT(GetColCount() == 0 || GetColCount() >= m_nLeftCol-1);
	ASSERT(GetRowCount() == 0 || GetRowCount() >= m_nTopRow-1);

	m_bLockGDIObjects = TRUE;

	BOOL bLockEdit = FALSE;
	BOOL bFound = FALSE;
	BOOL bOK = TRUE;

	// check out current position
	BOOL bActive;
	ROWCOL nEditRow, nEditCol;
	bActive = GetCurrentCell(nEditRow, nEditCol);
	if (!bActive)
	{
		nEditRow = min(m_nEditRow, GetRowCount());
		nEditCol = min(m_nEditCol, GetColCount());
	}
	else if (!GetControl(nEditRow, nEditCol)->GetModify() && m_bCacheCurrentCell && !m_bRefreshOnSetCurrentCell)
	{
		bLockEdit = TRUE;
		m_nLockRow = GX_MAXROWCOL;
		m_nLockCol = GX_MAXROWCOL;
	}

	BOOL bOld = m_bLockStyleEdit;
	m_bLockStyleEdit = bLockEdit;

	// TRACE2("CGXGridCore::MoveCurrentCell(%d, %d)\n", direction, nCell);

	ROWCOL  nFrozenRows = GetFrozenRows(),
			nFrozenCols = GetFrozenCols();

	ROWCOL  nHeaderRows = GetHeaderRows(),
			nHeaderCols = GetHeaderCols();

	ROWCOL  nSetRow = nEditRow,
			nSetCol = nEditCol;

	ROWCOL  nFoundRow = nEditRow,
			nFoundCol = nEditCol;

	ROWCOL  nClientRow = GetClientRow(nEditRow);

	// Range spanned by covered cell
	CGXRange rgCovered;
	ROWCOL  nRows = 0, nCols = 0;
	if (GetCoveredCellsRowCol(nEditRow, nEditCol, rgCovered))
	{
		nRows = rgCovered.bottom - rgCovered.top;
		nCols = rgCovered.right - rgCovered.left;
	}

	BOOL bExcel = GetParam()->IsScrollLikeExcel();

	ROWCOL  nCount;
	ROWCOL nScrollUp = 0, nScrollLeft = 0;

	switch (direction)
	{
	case GX_UP:
		// move up
		while (wCell > 0 && nSetRow > nHeaderRows)
		{
			// if there are frozen rows, move up one visible cell;
			// if current cell is at the top row, scroll the view.
			if (bExcel && (nSetRow == GetTopRow() || nScrollUp > 0))
			{
				// handle scrolling of rowscolumns similiar to MS Excel
				// if current cell is at the topmost nonfrozen row, scroll the view.
				nScrollUp++;
				nSetRow--;
			}
			else if (nSetRow <= nHeaderRows + 1 || nScrollUp > 0)
			{
				// handle scrolling of frozen columns in my own way
				// if current cell is at topmost header row, scroll the view.
				nScrollUp++;
				if (nEditRow > nFrozenRows)
					nSetRow = GetRow(nFrozenRows + 1) - nScrollUp;
			}
			else if (nFrozenRows > nHeaderRows && nSetRow >= GetTopRow() && nSetRow <= GetRow(nFrozenRows + 1))
			{
				nSetRow = GetRow(GetClientRow(nSetRow)-1);
			}
			else
				nSetRow--;

			if (!FindNextCell(GX_UP, nSetRow, nSetCol))
				break;

			// Scrolling necessary when new row is not visible
			// because it is between the frozen row and the
			// top row
			if (!(nSetRow <= nFrozenRows || nSetRow >= GetTopRow()))
				nScrollUp += GetTopRow()-nSetRow;

			wCell--;
			bFound = TRUE;
			nFoundRow = nSetRow;
			nFoundCol = nSetCol;

			if (bLockEdit)
			{
				m_nLockRow = nSetRow;
				m_nLockRow2 = m_nTopRow;
			}
		}

		break;

	case GX_DOWN:
		// move down
			// make sure that we will not go over the last row
			// if row count must be estimated (e.g. in a CRecordset)
		OnTestGridDimension(GetRowCount(), 0);
			// now, it is safe to use GetRowCount()
		nCount = GetRowCount();
		while (wCell > 0 && nSetRow < nCount)
		{
			// if there are frozen rows, move down one visible cell;
			if (nSetRow <= nFrozenRows)
				nSetRow = GetRow(nSetRow+1+nRows);
			else
				nSetRow += nRows+1;

			if (!FindNextCell(GX_DOWN, nSetRow, nSetCol))
				break;

			// cell is ok, decrease wCell
			wCell--;
			bFound = TRUE;
			nFoundRow = nSetRow;
			nFoundCol = nSetCol;
		}
		break;

	case GX_LEFT:
		// move to left
		while (wCell > 0 && nSetCol > nHeaderCols)
		{
			if (bExcel && (nSetCol == GetLeftCol() || nScrollLeft > 0))
			{
				// handle scrolling of frozen columns similiar to MS Excel
				// if current cell is at leftmost nonfrozen column, scroll the view.
				nScrollLeft++;
				nSetCol--;
			}
			else if (nSetCol <= nHeaderCols + 1 || nScrollLeft > 0)
			{
				// handle scrolling of frozen columns in my own way
				// if current cell is at leftmost header column, scroll the view.
				nScrollLeft++;
				if (nSetCol > nFrozenCols)
					nSetCol = GetCol(nHeaderCols + 1) - nScrollLeft;
			}
			else if (nFrozenCols > nHeaderCols && nSetCol >= GetLeftCol() && nSetCol <= GetCol(nFrozenCols + 1))
			{
				// if there are frozen Cols, move up one visible cell;
				nSetCol = GetCol(GetClientCol(nSetCol)-1);
			}
			else
				nSetCol--;

			if (!FindNextCell(GX_LEFT, nSetRow, nSetCol))
				break;

			// Scrolling necessary when new column is not visible
			// because it is between the frozen columns and the
			// left column
			if (!(nSetCol <= nFrozenCols || nSetCol >= GetLeftCol()))
				nScrollLeft += GetLeftCol()-nSetCol;

			wCell--;
			bFound = TRUE;
			nFoundRow = nSetRow;
			nFoundCol = nSetCol;

			if (bLockEdit)
			{
				m_nLockCol = nSetCol;
				m_nLockCol2 = m_nLeftCol;
			}
		}
		break;

	case GX_RIGHT:
		// move to right
			// make sure that we will not go over the last column
			// if column count must be estimated
		OnTestGridDimension(0, GetColCount());
			// now, it is safe to use GetColCount()
		nCount = GetColCount();
		while (wCell > 0 && nSetCol < nCount)
		{
			// if there are frozen rows, move one visible cell to the right;
			if (nSetCol <= nFrozenCols)
				nSetCol = GetCol(nSetCol+1+nCols);
			else
				nSetCol += nCols+1;

			if (!FindNextCell(GX_RIGHT, nSetRow, nSetCol))
				break;

			// cell is ok, decrease wCell
			wCell--;
			bFound = TRUE;
			nFoundRow = nSetRow;
			nFoundCol = nSetCol;
		}
		break;

	case GX_PGDOWN:
		// page down
			// make sure that we will not go over the last row
			// if row count must be estimated (e.g. in a CRecordset)
		OnTestGridDimension(GetRowCount(), 0);
			// now, it is safe to use GetRowCount()
		nCount = GetRowCount();

		if (m_nTopRow < nCount)
		{
			// calculate new cell
			ROWCOL nRow = max(m_nTopRow+1, CalcBottomRowFromRect(GetGridRect()));
			
			OnScrollBecauseOfEdit(GX_DOWN, (nRow = nRow-m_nTopRow));
			if (nClientRow > nFrozenRows)
				nSetRow = min(nEditRow+nRow, nCount);

			// find the nearest possible cell (first down. then up)
			bFound = FindNextCell(GX_DOWN, nSetRow, nSetCol);
			if (!bFound)
				bFound = FindNextCell(GX_UP, nSetRow, nSetCol);
			if (bFound)
			{
				nFoundRow = nSetRow;
				nFoundCol = nSetCol;
			}
		}
		break;

	case GX_PGUP:
		// page up
		if (m_nTopRow > nFrozenRows+1)
		{
			CRect   rect = GetGridRect();
			int    y = GetRowHeight(m_nTopRow);
			ROWCOL  ndRow = 0;
			int dy = rect.Height()-CalcSumOfRowHeights(0, nFrozenRows);

			// calculate new cell
			while (y < dy && m_nTopRow-ndRow > 1)
				y += GetRowHeight(m_nTopRow-(++ndRow));

			ASSERT(ndRow < m_nTopRow);

			OnScrollBecauseOfEdit(GX_UP, ndRow = max(1, ndRow));

			if (nClientRow > nFrozenRows)
				nSetRow = max(nEditRow, ndRow+1)-ndRow;

			// find the nearest possible cell (first up. then down)
			bFound = FindNextCell(GX_UP, nSetRow, nSetCol);

			if (!bFound)
			{
					// make sure that we will not go over the last row
					// if row count must be estimated (e.g. in a CRecordset)
				OnTestGridDimension(GetRowCount(), 0);
					// now, it is safe to use GetRowCount() (in FindNextCell())

				bFound = FindNextCell(GX_DOWN, nSetRow, nSetCol);
			}

			if (bFound)
			{
				nFoundRow = nSetRow;
				nFoundCol = nSetCol;
			}
		}
		else
		{
			// move to first row
				// make sure that we will not go over the last row
			OnTestGridDimension(GetRowCount(), 0);
				// now, it is safe to use GetRowCount() (for FindNextCell())
			nSetRow = min(GetHeaderRows()+1, GetRowCount());
			bFound = FindNextCell(GX_DOWN, nSetRow, nSetCol);
			if (bFound)
			{
				nFoundRow = nSetRow;
				nFoundCol = nSetCol;
			}
			break;
		}
		break;

	case GX_MOSTLEFT:
		// move to first column
		if (nEditCol > GetHeaderCols())
			OnScrollBecauseOfEdit(GX_LEFT, m_nEditCol-1);

			// make sure that we will not go over the last column
		OnTestGridDimension(0, GetColCount());
			// now, it is safe to use GetColCount() (for FindNextCell())

		nSetCol = min(GetHeaderCols()+1, GetColCount());

		bFound = FindNextCell(GX_RIGHT, nSetRow, nSetCol);
		if (bFound)
		{
			nFoundRow = nSetRow;
			nFoundCol = nSetCol;
		}
		break;

	case GX_MOSTRIGHT:
		// move to last column
			// make sure that column count is not only an estimated value
		OnTestGridDimension(0, GX_MAXROWCOL);
			// now, we are sure that GetColCount() returns the exact column count
		nSetCol = GetColCount();

		bFound = FindNextCell(GX_LEFT, nSetRow, nSetCol);
		if (bFound)
		{
			nFoundRow = nSetRow;
			nFoundCol = nSetCol;
		}
		break;

	case GX_TOPLEFT:
		// move to cell (1,1)
			// make sure that we will not go over the last column or row
		OnTestGridDimension(GetRowCount(), GetColCount());
			// now, it is safe to use GetColCount()/GetRowCount() (for FindNextCell())

		nSetCol = min(GetHeaderCols()+1, GetColCount());
		nSetRow = min(GetHeaderRows()+1, GetRowCount());

		bFound = FindNextCell(GX_DOWN, nSetRow, nSetCol);
		bFound |= FindNextCell(GX_RIGHT, nSetRow, nSetCol);
		if (bFound)
		{
			nFoundRow = nSetRow;
			nFoundCol = nSetCol;
		}
		break;

	case GX_BOTTOMRIGHT:
		// move to last row and last column
			// make sure that column and row count is not only an estimated value
		OnTestGridDimension(GX_MAXROWCOL, GX_MAXROWCOL);
			// now, it is sure that GetColCount() and GetRowCount() return exact values
		nSetCol = GetColCount();
		nSetRow = GetRowCount();
		bFound = FindNextCell(GX_UP, nSetRow, nSetCol);
		bFound |= FindNextCell(GX_LEFT, nSetRow, nSetCol);
		if (bFound)
		{
			nFoundRow = nSetRow;
			nFoundCol = nSetCol;
		}
		break;

	case GX_TOP:
		// move to first row
			// make sure that we will not go over the last row
		OnTestGridDimension(GetRowCount(), 0);
			// now, it is safe to use GetRowCount() (for FindNextCell())
		nSetRow = min(GetHeaderRows()+1, GetRowCount());
		bFound = FindNextCell(GX_DOWN, nSetRow, nSetCol);
		if (bFound)
		{
			nFoundRow = nSetRow;
			nFoundCol = nSetCol;
		}
		break;

	case GX_BOTTOM:
		// move to last column
			// make sure that row count is not only an estimated value
		OnTestGridDimension(GX_MAXROWCOL, 0);
			// now, it is sure that GetRowCount() returns an exact value
		nSetRow = GetRowCount();

		bFound = FindNextCell(GX_UP, nSetRow, nSetCol);
		if (bFound)
		{
			nFoundRow = nSetRow;
			nFoundCol = nSetCol;
		}
		break;

	default:
		ASSERT(0);
		// ASSERTION-> unknown direction ->END
	}

	// position edit cell
	if (bFound && nFoundRow != nEditRow && nFoundRow > nHeaderRows
		|| nFoundCol != nEditCol && nFoundCol > nHeaderCols
		|| !bActive)
	{
		// EXCELCURCELL
		if (GetParam()->GetExcelLikeCurrentCell()
			&& bCanSelectRange
			)
		{
			if (!GetParam()->GetRangeList()->IsEmpty()
				&& !GetParam()->GetRangeList()->IsCellInList(nFoundRow, nFoundCol)
				)
			SetSelection(NULL);

			bOK = SetCurrentCell(nFoundRow, nFoundCol,
				GX_UPDATENOW | GX_SCROLLINVIEW);
		}
		else
			bOK = SetCurrentCell(nFoundRow, nFoundCol,
				GX_UPDATENOW | GX_SCROLLINVIEW | GX_NOSELECTRANGE);
	}

	if (bOK)
	{
		if (nScrollUp > 0 && nFoundRow <= nFrozenRows)
			OnScrollBecauseOfEdit(GX_UP, nScrollUp);

		if (nScrollLeft > 0 && nFoundCol <= nFrozenCols)
			OnScrollBecauseOfEdit(GX_LEFT, nScrollLeft);
	}

	m_bLockGDIObjects = FALSE;
	m_nLockRow = 0;
	m_nLockCol = 0;
	m_nLockRow2 = 0;
	m_nLockCol2 = 0;

	// TRACE2("End CGXGridCore::MoveCurrentCell(%d, %d)\n", direction, nCell);

	m_bLockStyleEdit = bOld;

	return bOK;
}


/////////////////////////////////////////////////////////////////////////////
// Cell drawing and editing - Public members and Overridables
// depending on Cell-Type

void CGXGridCore::SetWarningText(LPCTSTR s)
{
	m_sWarningText = s;
}

void CGXGridCore::DisplayWarningText()
{
	// When displaying a message-box, CGXGridCore:.OnActivateGrid
	// will be called. The message-box will become activ and the
	// grid window will become inactiv which results in
	// a further call to CGXGridCore::OnActivateGrid.
	// It is necessary to avoid that OnActivateGrid calls
	// CanSelectCurrentCell(FALSE) again.
	//
	// The semaphor m_bLockActivateGrid is checked in OnActivateGrid to avoid these behavior:

	if (m_sWarningText.GetLength() > 0)
	{
		m_bLockActivateGrid = TRUE;

		HWND hWndFocus = ::GetFocus();

		// Remove any pendig Keyboard messages
		if (IsWindow(hWndFocus))
		{
			MSG msg;
			while (::PeekMessage(&msg,  // pointer to structure for message
				hWndFocus,  // handle to window
				WM_KEYFIRST, // first message
				WM_KEYLAST, // last message
				PM_REMOVE)  // removal flags
				)
				NULL;
		}

		// Make sure cell is visible
		ROWCOL nEditRow, nEditCol;
		if (GetCurrentCell(nEditRow, nEditCol))
			ScrollCellInView(nEditRow, nEditCol);

		// display box
		AfxMessageBox(m_sWarningText);

		m_bWarningTextDisplayed = TRUE;
			// indicates that this function has displayed a message-box.
			// The grid checks this variable to determine if it should
			// cancel any internal modes (e.g. if user is selecting cells)

		if (IsWindow(hWndFocus))
		{
			::SetFocus(hWndFocus);

			if (m_pGridWnd->m_hWnd == hWndFocus || ::IsChild(m_pGridWnd->m_hWnd, hWndFocus))
				m_bActiveFrame = TRUE;

		}

		m_bLockActivateGrid = FALSE;
	}

	m_sWarningText.Empty();
}

BOOL CGXGridCore::OnValidateCell(ROWCOL nRow, ROWCOL nCol)
{
	nRow, nCol;

	return TRUE;
}

BOOL CGXGridCore::OnStartEditing(ROWCOL nRow, ROWCOL nCol)
{
	nRow, nCol;

	return TRUE;
}

void CGXGridCore::OnModifyCell(ROWCOL , ROWCOL )
{
	SetModifiedFlag(TRUE);
}

BOOL CGXGridCore::OnCancelEditing(ROWCOL , ROWCOL )
{
	return TRUE;
}

void CGXGridCore::OnCanceledEditing(ROWCOL , ROWCOL )
{
}

BOOL CGXGridCore::OnEndEditing(ROWCOL nRow, ROWCOL nCol)
{
	nRow, nCol;

	return TRUE;
}

BOOL CGXGridCore::OnLeftCell(ROWCOL , ROWCOL, ROWCOL, ROWCOL )
{
	return TRUE;
}

BOOL CGXGridCore::OnDeleteCell(ROWCOL , ROWCOL )
{
	return TRUE;
}

void CGXGridCore::OnInitCurrentCell(ROWCOL nRow, ROWCOL nCol)
{
	UpdateRecordInfoWnd();

	if (m_nLastInitRow != nRow || m_nLastInitCol != nCol)
		OnMovedCurrentCell(nRow, nCol);

	m_nLastInitRow = nRow;
	m_nLastInitCol = nCol;
}

void CGXGridCore::OnMovedCurrentCell(ROWCOL , ROWCOL )
{
}

BOOL CGXGridCore::OnLButtonClickedRowCol(ROWCOL nRow, ROWCOL nCol, UINT nFlags, CPoint pt)
{
	nFlags, pt;

	if (m_pUserSelectRangeImp)
		return m_pUserSelectRangeImp->OnLButtonClickedRowCol(this, nRow, nCol, nFlags, pt);

	return FALSE;
}


BOOL CGXGridCore::OnMButtonClickedRowCol(ROWCOL nRow, ROWCOL nCol, UINT nFlags, CPoint pt)
{
	nRow, nCol, nFlags, pt;

	return FALSE;
}

BOOL CGXGridCore::OnRButtonClickedRowCol(ROWCOL nRow, ROWCOL nCol, UINT nFlags, CPoint pt)
{
	// Unused:
	nRow, nCol, nFlags, pt;

	return FALSE;
}

BOOL CGXGridCore::OnLButtonDblClkRowCol(ROWCOL nRow, ROWCOL nCol, UINT nFlags, CPoint pt)
{
	nRow, nCol, nFlags, pt;

	if (m_pSortImp)
		return m_pSortImp->OnLButtonDblClkRowCol(this, nRow, nCol, nFlags, pt);

	return FALSE;
}

BOOL CGXGridCore::OnMButtonDblClkRowCol(ROWCOL nRow, ROWCOL nCol, UINT nFlags, CPoint pt)
{
	nRow, nCol, nFlags, pt;

	return FALSE;
}

BOOL CGXGridCore::OnRButtonDblClkRowCol(ROWCOL nRow, ROWCOL nCol, UINT nFlags, CPoint pt)
{
	nRow, nCol, nFlags, pt;

	return FALSE;
}

//
// Keyboard messages
//

BOOL CGXGridCore::ProcessKeys(CWnd* pSender, UINT nMessage, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Process Keyboard input

	// NOTE: pSender is either the grid or the active control

	// Is the grid in a special mode (tracking, dragging, selecting)?
	if (GetCapture() == m_pGridWnd)
	{
		// ... give user a chance to abort this mode
		if (nChar == VK_ESCAPE)
			m_pGridWnd->SendMessage(WM_CANCELMODE, 0, 0);

		// otherwise, keys cannot be processed

		return TRUE;
	}


	ROWCOL nRow, nCol;
	CGXControl* pControl = NULL;
	if (GetCurrentCell(nRow, nCol))
		pControl = GetControl(nRow, nCol);

	// If ProcessKeys was called from the grid object
	// give active control a chance to interprete and
	// process the key first.

	if (pControl && pSender == m_pGridWnd)
	{
		if (!pControl->IsInit())
			pControl->Init(nRow, nCol);

		// KeyPressed will return TRUE if it processed
		// the message
		if (pControl->KeyPressed(nMessage, nChar, nRepCnt, nFlags))
		{
			// Make sure that cell is in visible area
			ScrollCellInView(nRow, nCol);
			UpdateWindow();

			return TRUE;
		}
	}

	// Either KeyPressed did return FALSE or the active control
	// did call ProcessKeys

	// Give CGXGridCore the chance to handle the event

	switch (nMessage)
	{
	// Keyboard messages
	case WM_KEYDOWN:
		return OnGridKeyDown(nChar, nRepCnt, nFlags);

	case WM_KEYUP:
		return OnGridKeyUp(nChar, nRepCnt, nFlags);

	case WM_CHAR:
		{
			// support for DBCS characters
			CString sChar;
			if (GXGetDoubleByteChar((TCHAR) nChar, sChar, m_bDoubleByteChar, m_nDoubleByteLeadChar))
			{
				if (sChar.IsEmpty())
					return TRUE;

				return OnGridDoubleByteChar(sChar, nRepCnt, nFlags);
			}
			else // this is a normal character
			{
				return OnGridChar(nChar, nRepCnt, nFlags);
			}
		}

	case WM_DEADCHAR:
		return OnGridDeadChar(nChar, nRepCnt, nFlags);

	case WM_SYSKEYDOWN:
		return OnGridSysKeyDown(nChar, nRepCnt, nFlags);

	case WM_SYSKEYUP:
		return OnGridSysKeyUp(nChar, nRepCnt, nFlags);

	case WM_SYSCHAR:
		return OnGridSysChar(nChar, nRepCnt, nFlags);

	case WM_SYSDEADCHAR:
		return OnGridSysDeadChar(nChar, nRepCnt, nFlags);
	}

	return FALSE;
}

BOOL CGXGridCore::OnGridKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nFlags, nRepCnt;

	BOOL bCtl = GetKeyState(VK_CONTROL) & 0x8000;
	BOOL bShift = GetKeyState(VK_SHIFT) & 0x8000;
	BOOL bAlt = GetKeyState(VK_MENU) & 0x8000;
	BOOL bRet = FALSE;

	BOOL bProcessed = FALSE, bSpeedKey = FALSE;

	// TRACE1("WM_KEYDOWN: nChar %03d\n", nChar);
	switch (nChar)
	{
	case VK_TAB:
		if (TransferCurrentCell(TRUE, GX_INVALIDATE))
			MoveCurrentCell(bShift ? GX_LEFT : GX_RIGHT, 1);
		bProcessed = TRUE;
		break;

	case VK_NEXT:
		bRet = MoveCurrentCellEx(GX_PGDOWN, max(1, nRepCnt));
		bProcessed = TRUE;
		bSpeedKey = bRet && !m_bWarningTextDisplayed;
		break;

	case VK_PRIOR:
		bRet = MoveCurrentCellEx(GX_PGUP, max(1, nRepCnt));
		bProcessed = TRUE;
		bSpeedKey = bRet && !m_bWarningTextDisplayed;
		break;

	case VK_UP:
		bRet = MoveCurrentCellEx(bCtl ? GX_TOP : GX_UP, max(1, nRepCnt));
		bProcessed = TRUE;
		bSpeedKey = bRet && !m_bWarningTextDisplayed;
		break;

	case VK_DOWN:
		bRet = MoveCurrentCellEx(bCtl ? GX_BOTTOM : GX_DOWN, max(1, nRepCnt));
		bProcessed = TRUE;
		bSpeedKey = bRet && !m_bWarningTextDisplayed;
		break;

	case VK_LEFT:
		bRet = MoveCurrentCellEx(bCtl ? GX_MOSTLEFT : GX_LEFT, max(1, nRepCnt));
		bProcessed = TRUE;
		bSpeedKey = bRet && !m_bWarningTextDisplayed;
		break;

	case VK_RIGHT:
		bRet = MoveCurrentCellEx(bCtl ? GX_MOSTRIGHT : GX_RIGHT, max(1, nRepCnt));
		bProcessed = TRUE;
		bSpeedKey = bRet && !m_bWarningTextDisplayed;
		break;

	case VK_F2:
		{
			CGXControl* pControl = GetCurrentCellControl();
			if (pControl && !pControl->IsActive())
			{
				TransferCurrentCell(FALSE);
				if (!pControl->IsReadOnly() && pControl->OnStartEditing())
				{
					pControl->SetActive(TRUE);
					pControl->Refresh();
				}
				bProcessed = TRUE;
				break;
			}
			else if (pControl && pControl->IsActive())
			{
				TransferCurrentCell();
				bProcessed = TRUE;
				break;
			}
		}

	case VK_RETURN:
		if (!bAlt && !bCtl && !bShift)
		{
			WORD nAction = GetParam()->GetEnterKeyAction();
			if (TransferCurrentCell(TRUE, GX_INVALIDATE) && nAction != 0)
				MoveCurrentCellEx(nAction, 1);
			bProcessed = TRUE;
		}
		break;

	case VK_HOME:
		if (bCtl)
		{
			MoveCurrentCellEx(GX_TOPLEFT, 1);
			bProcessed = TRUE;
		}
		break;

	case VK_END:
		if (bCtl)
		{
			MoveCurrentCellEx(GX_BOTTOMRIGHT, 1);
			bProcessed = TRUE;
		}
		break;

	case VK_INSERT:
		if (bCtl)
		{
			if (CanCopy())
				Copy();
			bProcessed = TRUE;
		}
		else if (bShift)
		{
			if (CanPaste())
				Paste();
			bProcessed = TRUE;
		}
		break;

	case VK_DELETE:
		if (bShift)
		{
			if (CanCut())
				Cut();
			bProcessed = TRUE;
		}
		else
		{
			ROWCOL nRow, nCol;
			if (GetCurrentCell(nRow, nCol) && !IsActiveCurrentCell() && CanClear() && OnDeleteCell(nRow, nCol))
				Clear(bCtl);
			bProcessed = TRUE;
		}
		break;

	case VK_BACK:
		if (bCtl)
		{
			Redo();
			bProcessed = TRUE;
		}
		break;

	case VK_ESCAPE:
		{
			if (GetParam()->m_bCut)
			{
				GetParam()->m_bCut = FALSE;
				Redraw(GX_INVALIDATE);
			}

			ROWCOL nRow, nCol;
			if (GetCurrentCell(nRow, nCol))
			{
				CGXControl* pControl = GetControl(nRow, nCol);
				if (pControl->OnCancelEditing())
				{
					pControl->Init(nRow, nCol);
					pControl->Refresh();
					pControl->OnCanceledEditing();
				}
				else
					pControl->Refresh();

				ScrollCellInView(nRow, nCol);
			}

			// EXCELCURCELL
			if (!GetParam()->GetExcelLikeCurrentCell())
				SetSelection(0);
		}
		bProcessed = TRUE;
		break;
	}

	if (bProcessed)
		UpdateWindow();


	if (m_bAccelArrowKey && bSpeedKey && !m_bSpeedKey)
	{
		m_bSpeedKey = bSpeedKey;
		m_nSpeedKeyChar = nChar;
		m_nSpeedKeyFlags = nFlags;
		AutoScroll(0, FALSE, FALSE);
	} 

	return bProcessed;  // return TRUE if you don't want default processing of this event
}

BOOL CGXGridCore::OnGridKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXGridCore::OnGridChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	BOOL bProcessed = FALSE;

	// TRACE1("WM_CHAR: nChar %03d\n", nChar);
	switch (nChar)
	{
	case VK_TAB:
	case 127:
	case VK_RETURN:
		bProcessed = TRUE;
		break;

	case 26:    // <CTRL>+Z
		Undo();
		bProcessed = TRUE;
		break;

	case 18:    // <CTRL>+R
		Redo();
		bProcessed = TRUE;
		break;

	case 3:     // <CTRL>+C
		if (CanCopy())
			Copy();
		bProcessed = TRUE;
		break;

	case 22:    // <CTRL>+V
		if (CanPaste())
			Paste();
		bProcessed = TRUE;
		break;

	case 24:    // <CTRL>+X
		if (CanCut())
			Cut();
		bProcessed = TRUE;
		break;

#ifdef TICKCOUNT
	case 20:    // <CTRL>+T
		TCHAR s[512];
		_stprintf(s, _T("Ticks: CreateIndFont %lu, CreateStdFont %lu, Lookup %lu, ")
			_T("DrawItem %lu, Draw %lu, LookupFont %lu, tickLoadFont %lu, ")
			_T("tickCompStd %lu tickComposeStyle %lu")
			_T("tick1 %lu, tick2 %lu, tick3 %lu, tick4 %lu, tick 5 %lu, ")
			_T("tick6 %lu, tick7 %lu, tick8 %lu, tick9 %lu, tick10 %lu, "),
			tickIndFont, tickStdFont, tickLookup,
			tickDrawItem, tickDraw, tickLookupFont, tickLoadFont,
			tickCompStd, tickComposeStyle,
			tick1, tick2, tick3, tick4, tick5,
			tick6, tick7, tick8, tick9, tick10);
		AfxMessageBox(s);
		bProcessed = TRUE;
		break;
#endif
	}

	return bProcessed;  // return TRUE if you don't want default processing of this event
}

BOOL CGXGridCore::OnGridDeadChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXGridCore::OnGridDoubleByteChar(const CString& sChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	sChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXGridCore::OnGridSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nFlags, nRepCnt;

	BOOL bAlt = GetKeyState(VK_MENU) & 0x8000;

	BOOL bProcessed = FALSE;

	// TRACE1("WM_SYSKEYDOWN: nChar %03d\n", nChar);
	switch (nChar)
	{
	case VK_BACK:
		if (bAlt)
		{
			Undo();
			bProcessed = TRUE;
		}
		break;
	}

	return bProcessed;  // return TRUE if you don't want default processing of this event
}

BOOL CGXGridCore::OnGridSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXGridCore::OnGridSysChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXGridCore::OnGridSysDeadChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXGridCore::OnScrollBecauseOfEdit(int direction, ROWCOL nCell)
{
	return OnAutoScroll(direction, nCell);
}

BOOL CGXGridCore::OnLButtonHitRowCol(ROWCOL nHitRow, ROWCOL nHitCol, ROWCOL nDragRow, ROWCOL nDragCol, CPoint point, UINT flags, WORD nHitState)
{
	// called from DoLButtonDown, DoMouseMove and DoLButtonUp

	// TRACE(_T("OnLButtonHitRowCol(%ld,%ld,%ld,%ld,...,%d)\n"), nHitRow, nHitCol, nDragRow, nDragCol, nHitState);

	// determine style and control of the cell
	ROWCOL nRow = nHitRow, nCol = nHitCol;

	CGXRange rgCovered;
	if (GetCoveredCellsRowCol(nRow, nCol, rgCovered))
	{
		nRow = rgCovered.top;
		nCol = rgCovered.left;
	}

	m_bHitCellScope = nHitState & GX_HITINCELL;

	if (nHitState & GX_HITSTART)
	{
		// The user has pressed the left mouse button in the grid

		// Try to move the current cell to cell (nRow, nCol).
		// If this is not possible, OnLButtonHitRowCol will return FALSE.
		//
		// OnLButtonDown checks the value returned by OnLButtonHitRowCol.
		// If it is FALSE, the current mouse operation will be canceled.
		//

		// The following block checks, if deactivating the current cell
		// will bring up a message box (because of invalid cell contents, ...).
		//
		// Block:
		// EXCELCURCELL
		if (GetParam()->GetExcelLikeCurrentCell() || !(nHitState & GX_HITCURRENTCELL))
		{
			// SetCurrentCell will possibly display a message-box.
			// In this case, SetCurrentCell will call DisplayWarningText.
			// DisplayWarningText method will set m_bWarningTextDisplayed = TRUE

			m_bWarningTextDisplayed = FALSE;

			UINT flags = GX_SMART;
			if ((m_nHitState&GX_CTRLPRESSED) != 0)
				flags |= GX_NOSELECTRANGE;

			SetCurrentCell(nRow, nCol, flags);

			// Has DisplayWarningText been called?
			if (m_bWarningTextDisplayed || nRow > GetRowCount() || nCol > GetColCount())
				return FALSE;
		}

		CGXControl* pControl = GetControl(nRow, nCol);

		// Process control specific behaviour for mouse click.

		if (IsCurrentCell(nRow, nCol) && pControl->IsInit())
			pControl->LButtonDown(flags, point, nHitState);

		m_bHitMoved = FALSE;
	}
	else if (nHitState & GX_HITEND)
	{
		CGXControl* pControl = GetControl(nRow, nCol);

		// Process control specific behaviour for mouse click.
		if (IsCurrentCell(nRow, nCol) && pControl->IsInit())
			pControl->LButtonUp(flags, point, nHitState);
			// CGXControl::LButtonUp will call OnLButtonClickedRowCol
		else
			OnLButtonClickedRowCol(nRow, nCol, flags, point);

		// check, if user moved the mouse over other cells
		if (!m_bHitMoved && IsCurrentCell(nRow, nCol))
		{
			// ... no, scroll cell into view
			if (ScrollCellInView(nRow, nCol))
				pControl->Refresh();
		}
		else if (m_pParam->IsSpecialMode(GX_MODELISTBOX))
		{
			// ... yes, and grid is used as listbox
			//
			// SetCurrentCell will move the selection bar
			if (!SetCurrentCell(nDragRow, nDragCol, GX_SMART))
				return FALSE;
		}
	}
	else if (nHitState & GX_HITMOVE)
	{
		CGXControl* pControl = GetControl(nRow, nCol);

		// Process control specific behaviour for mouse move.
		if (IsCurrentCell(nRow, nCol) && pControl->IsInit())
			pControl->MouseMove(flags, point, nHitState);

		// set internal flag which specifies if user has moved the mouse over other cells
		if (nHitRow != nDragRow || nHitCol != nDragCol)
			m_bHitMoved = TRUE;
	}

	return TRUE;
}

BOOL CGXGridCore::OnMButtonHitRowCol(ROWCOL nHitRow, ROWCOL nHitCol, ROWCOL nDragRow, ROWCOL nDragCol, CPoint point, UINT flags, WORD nHitState)
{
	// called from DoMButtonDown, DoMouseMove and DoMButtonUp

	// TRACE(_T("OnMButtonHitRowCol(%ld,%ld,%ld,%ld,...,%d)\n"), nHitRow, nHitCol, nDragRow, nDragCol, nHitState);

	// determine style and control of the cell
	ROWCOL nRow = nHitRow, nCol = nHitCol;

	CGXRange rgCovered;
	if (GetCoveredCellsRowCol(nRow, nCol, rgCovered))
	{
		nRow = rgCovered.top;
		nCol = rgCovered.left;
	}

	m_bHitCellScope = nHitState & GX_HITINCELL;

	if (nHitState & GX_HITSTART)
	{
		// The user has pressed the left mouse button in the grid

		// Try to move the current cell to cell (nRow, nCol).
		// If this is not possible, OnMButtonHitRowCol will return FALSE.
		//
		// OnLButtonDown checks the value returned by OnLButtonHitRowCol.
		// If it is FALSE, the current mouse operation will be canceled.
		//

		// The following block checks, if deactivating the current cell
		// will bring up a message box (because of invalid cell contents, ...).
		//
		// Block:
		if (!(nHitState & GX_HITCURRENTCELL))
		{
			// SetCurrentCell will possibly display a message-box.
			// In this case, SetCurrentCell will call DisplayWarningText.
			// DisplayWarningText method will set m_bWarningTextDisplayed = TRUE

			m_bWarningTextDisplayed = FALSE;

			SetCurrentCell(nRow, nCol, GX_SMART);

			// Has DisplayWarningText been called?
			if (m_bWarningTextDisplayed || nRow > GetRowCount() || nCol > GetColCount())
				return FALSE;
		}

		CGXControl* pControl = GetControl(nRow, nCol);

		// Process control specific behaviour for mouse click.

		if (IsCurrentCell(nRow, nCol) && pControl->IsInit())
			pControl->MButtonDown(flags, point, nHitState);

		m_bHitMoved = FALSE;
	}
	else if (nHitState & GX_HITEND)
	{
		CGXControl* pControl = GetControl(nRow, nCol);

		// Process control specific behaviour for mouse click.
		if (IsCurrentCell(nRow, nCol) && pControl->IsInit())
			pControl->MButtonUp(flags, point, nHitState);
			// CGXControl::MButtonUp will call OnMButtonClickedRowCol
		else
			OnMButtonClickedRowCol(nRow, nCol, flags, point);

		// check, if user moved the mouse over other cells
		if (!m_bHitMoved && IsCurrentCell(nRow, nCol))
		{
			// ... no, scroll cell into view
			if (ScrollCellInView(nRow, nCol))
				pControl->Refresh();
		}
		else if (m_pParam->IsSpecialMode(GX_MODELISTBOX))
		{
			// ... yes, and grid is used as listbox
			//
			// SetCurrentCell will move the selection bar
			if (!SetCurrentCell(nDragRow, nDragCol, GX_SMART))
				return FALSE;
		}
	}
	else if (nHitState & GX_HITMOVE)
	{
		CGXControl* pControl = GetControl(nRow, nCol);

		// Process control specific behaviour for mouse move.
		if (IsCurrentCell(nRow, nCol) && pControl->IsInit())
			pControl->MouseMove(flags, point, nHitState);

		// set internal flag which specifies if user has moved the mouse over other cells
		if (nHitRow != nDragRow || nHitCol != nDragCol)
			m_bHitMoved = TRUE;
	}

	return TRUE;
}

BOOL CGXGridCore::OnRButtonHitRowCol(ROWCOL nHitRow, ROWCOL nHitCol, ROWCOL nDragRow, ROWCOL nDragCol, CPoint point, UINT flags, WORD nHitState)
{
	// called from DoRButtonDown, DoMouseMove and DoRButtonUp

	// TRACE(_T("OnRButtonHitRowCol(%ld,%ld,%ld,%ld,...,%d)\n"), nHitRow, nHitCol, nDragRow, nDragCol, nHitState);

	// determine style and control of the cell
	ROWCOL nRow = nHitRow, nCol = nHitCol;

	CGXRange rgCovered;
	if (GetCoveredCellsRowCol(nRow, nCol, rgCovered))
	{
		nRow = rgCovered.top;
		nCol = rgCovered.left;
	}

	m_bHitCellScope = nHitState & GX_HITINCELL;

	if (nHitState & GX_HITSTART)
	{
		// The user has pressed the left mouse button in the grid

		// Try to move the current cell to cell (nRow, nCol).
		// If this is not possible, OnRButtonHitRowCol will return FALSE.
		//
		// OnLButtonDown checks the value returned by OnLButtonHitRowCol.
		// If it is FALSE, the current mouse operation will be canceled.
		//

		// The following block checks, if deactivating the current cell
		// will bring up a message box (because of invalid cell contents, ...).
		//
		// Block:
		if (!(nHitState & GX_HITCURRENTCELL))
		{
			// SetCurrentCell will possibly display a message-box.
			// In this case, SetCurrentCell will call DisplayWarningText.
			// DisplayWarningText method will set m_bWarningTextDisplayed = TRUE

			m_bWarningTextDisplayed = FALSE;

			SetCurrentCell(nRow, nCol, GX_SMART);

			// Has DisplayWarningText been called?
			if (m_bWarningTextDisplayed || nRow > GetRowCount() || nCol > GetColCount())
				return FALSE;
		}

		CGXControl* pControl = GetControl(nRow, nCol);

		// Process control specific behaviour for mouse click.

		if (IsCurrentCell(nRow, nCol) && pControl->IsInit())
			pControl->RButtonDown(flags, point, nHitState);

		m_bHitMoved = FALSE;
	}
	else if (nHitState & GX_HITEND)
	{

		CGXControl* pControl = GetControl(nRow, nCol);

		// Process control specific behaviour for mouse click.
		if (IsCurrentCell(nRow, nCol) && pControl->IsInit())
			pControl->RButtonUp(flags, point, nHitState);
			// CGXControl::RButtonUp will call OnRButtonClickedRowCol
		else
			OnRButtonClickedRowCol(nRow, nCol, flags, point);

		// check, if user moved the mouse over other cells
		if (!m_bHitMoved && IsCurrentCell(nRow, nCol))
		{
			// ... no, scroll cell into view
			if (ScrollCellInView(nRow, nCol))
				pControl->Refresh();
		}
		else if (m_pParam->IsSpecialMode(GX_MODELISTBOX))
		{
			// ... yes, and grid is used as listbox
			//
			// SetCurrentCell will move the selection bar
			if (!SetCurrentCell(nDragRow, nDragCol, GX_SMART))
				return FALSE;
		}
	}
	else if (nHitState & GX_HITMOVE)
	{

		CGXControl* pControl = GetControl(nRow, nCol);

		// Process control specific behaviour for mouse move.
		if (IsCurrentCell(nRow, nCol) && pControl->IsInit())
			pControl->MouseMove(flags, point, nHitState);

		// set internal flag which specifies if user has moved the mouse over other cells
		if (nHitRow != nDragRow || nHitCol != nDragCol)
			m_bHitMoved = TRUE;
	}

	return TRUE;
}

void CGXGridCore::OnClickedButtonRowCol(ROWCOL nRow, ROWCOL nCol)
{
	// Unreferenced:
	nRow, nCol;
}
