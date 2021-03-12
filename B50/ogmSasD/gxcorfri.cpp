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

// gxcorfri.cpp : Find and Replace Implementation (CGXGridCore)
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

#ifndef _GXCTRL_H_
#include "gxctrl.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCORFR")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

class CGXGridFindReplaceImp: public CGXAbstractGridFindReplaceImp
{
public:
	virtual void OnShowFindReplaceDialog(CGXGridCore* pGrid, BOOL bFindOnly);
	virtual void OnFindNext(CGXGridCore * pGrid, LPCTSTR lpszFind, BOOL bNext, BOOL bCase);
	virtual BOOL FindText(CGXGridCore* pGrid, BOOL bSetCell);
	virtual void OnReplaceSel(CGXGridCore* pGrid, LPCTSTR lpszFind, BOOL bNext, BOOL bCase, LPCTSTR lpszReplace);
	virtual void OnReplaceAll(CGXGridCore* pGrid, LPCTSTR lpszFind, LPCTSTR lpszReplace, BOOL bCase);
	virtual void OnFindReplace(CGXGridCore* pGrid, CFindReplaceDialog* pDialog);
	virtual BOOL FindText(CGXGridCore* pGrid, ROWCOL& nRow, ROWCOL& nCol, BOOL bSetCell);
	virtual BOOL FindText(CGXControl* pControl, const GX_FR_STATE& find, ROWCOL nRow, ROWCOL nCol, BOOL bCurrentPos, BOOL bSetCell);
	virtual BOOL ReplaceAll(CGXControl* pControl, const GX_FR_STATE& find, ROWCOL nRow, ROWCOL nCol);
};

void CGXGridCore::ImplementFindReplace()
{
	if (m_pFindReplaceImp == NULL)
		AutoDeletePtr(m_pFindReplaceImp = new CGXGridFindReplaceImp);
}
 
// show commdlg

void AFXAPI GXShowFindReplaceDialog(BOOL bFindOnly, const CString& strFind, CWnd* pParent)
{
	GX_FR_STATE* pState = GXGetLastFRState();

	if (pState->pFindReplaceDlg != NULL)
	{
		if (pState->bFindOnly == bFindOnly)
		{
			pState->pFindReplaceDlg->SetActiveWindow();
			pState->pFindReplaceDlg->ShowWindow(SW_SHOW);
			return;
		}
		else
		{
			ASSERT(pState->bFindOnly != bFindOnly);
			pState->pFindReplaceDlg->SendMessage(WM_CLOSE);
			ASSERT(pState->pFindReplaceDlg == NULL);
		}
	}
	else
		pState->pFindReplaceDlgOwner = pParent;

	CString strReplace = pState->strReplace;
	pState->pFindReplaceDlg = new CFindReplaceDialog;
	ASSERT(pState->pFindReplaceDlg != NULL);
	DWORD dwFlags = FR_HIDEWHOLEWORD;
	if (pState->bNext)
		dwFlags |= FR_DOWN;
	if (pState->bCase)
		dwFlags |= FR_MATCHCASE;
	if (!pState->pFindReplaceDlg->Create(bFindOnly, strFind,
		strReplace, dwFlags, pParent))
	{
		pState->pFindReplaceDlg = NULL;
		return;
	}
	ASSERT(pState->pFindReplaceDlg != NULL);
	pState->bFindOnly = bFindOnly;
}

void CGXGridFindReplaceImp::OnShowFindReplaceDialog(CGXGridCore* pGrid, BOOL bFindOnly)
{
	if (pGrid->GetRowCount() == 0 || pGrid->GetColCount() == 0)
		return;

	GX_FR_STATE* pState = GXGetLastFRState();
	pState->m_pActiveGrid = pGrid;

	if (!pGrid->IsCurrentCell())
		pGrid->SetCurrentCell(
			min(pGrid->GetTopRow(), pGrid->GetRowCount()),
			min(pGrid->GetLeftCol(), pGrid->GetColCount()),
			GX_SCROLLINVIEW | GX_UPDATENOW | GX_NOSETFOCUS);
	else
	{
		ROWCOL nRow, nCol;
		pGrid->GetCurrentCell(&nRow, &nCol);
		pGrid->ScrollCellInView(nRow, nCol);
	}

	CString strFind;
	pGrid->GetSelectedText(strFind);
	if (strFind.IsEmpty())
		strFind = pState->strFind;

	GXShowFindReplaceDialog(bFindOnly, strFind, pGrid->m_pGridWnd);
}

void CGXGridFindReplaceImp::OnFindNext(CGXGridCore * pGrid, LPCTSTR lpszFind, BOOL bNext, BOOL bCase)
{
	GX_FR_STATE* pState = GXGetLastFRState();

	if (pGrid->GetRowCount() == 0 || pGrid->GetColCount() == 0)
		return;

	// initialize searchparameter structure
	pState->strFind = lpszFind;
	pState->bCase = bCase;
	pState->bNext = bNext;
	pState->PrepareFindReplace();

	pGrid->FindText(TRUE);

#ifdef _UNICODE
	// Clean up non UNICODE text buffer
	delete[] pState->lpszMultiByteFindString;
	pState->lpszMultiByteFindString = NULL;
#endif
}

BOOL CGXGridFindReplaceImp::FindText(CGXGridCore* pGrid, BOOL bSetCell)
{
	// active cell coordinates
	ROWCOL nRow, nCol;
	if (!pGrid->GetCurrentCell(nRow, nCol))
		return FALSE;

	CGXLongOperation theOp;

	CGXControl* pControl = pGrid->GetControl(nRow, nCol);

	pGrid->m_nFirstRow = nRow;

	// check active cell or search next cell
	BOOL bFound = pControl->FindText(*GXGetLastFRState(), nRow, nCol, FALSE, bSetCell);

	// .... or search next cell
	if (!bFound)
		bFound = pGrid->FindText(nRow, nCol, bSetCell);

	// ... not found
	if (!bFound && bSetCell)
		pGrid->OnTextNotFound(GXGetLastFRState()->strFind);

	return bFound;
}

void CGXGridFindReplaceImp::OnReplaceSel(CGXGridCore* pGrid, LPCTSTR lpszFind, BOOL bNext, BOOL bCase, LPCTSTR lpszReplace)
{
	if (pGrid->GetRowCount() == 0 || pGrid->GetColCount() == 0)
		return;

	// active cell coordinates
	ROWCOL nRow, nCol;
	if (!pGrid->GetCurrentCell(&nRow, &nCol))
		return;

	GX_FR_STATE* pState = GXGetLastFRState();

	// initialize searchparameter structure
	pState->strFind = lpszFind;
	pState->strReplace = lpszReplace;
	pState->bCase = bCase;
	pState->bNext = bNext;
	pState->PrepareFindReplace();

	CGXLongOperation theOp;

	pGrid->m_nFirstRow = nRow;

	CGXControl* pControl = pGrid->GetControl(nRow, nCol);

	// check active cell or search next cell
	BOOL bFound = pControl->FindText(*pState, nRow, nCol, TRUE /* current pos */, TRUE /* do SetSel */);

	if (bFound)
		pControl->ReplaceSel(GXGetLastFRState()->strReplace);

	if (bFound && pControl->FindText(*pState, nRow, nCol, TRUE /* current pos */, FALSE /* do SetSel */))
		bFound = pControl->FindText(*pState, nRow, nCol, TRUE /* current pos */, TRUE /* do SetSel */);
	else
		// .... or search next cell
		bFound = pGrid->FindText(nRow, nCol, TRUE /* set current cell */);

	// ... not found
	if (!bFound)
		pGrid->OnTextNotFound(pState->strFind);

#ifdef _UNICODE
	// Clean up non UNICODE text buffer
	delete[] pState->lpszMultiByteFindString;
	pState->lpszMultiByteFindString = NULL;
#endif
}

void CGXGridFindReplaceImp::OnReplaceAll(CGXGridCore* pGrid, LPCTSTR lpszFind, LPCTSTR lpszReplace, BOOL bCase)
{
	GX_FR_STATE* pState = GXGetLastFRState();

	if (pGrid->GetRowCount() == 0 || pGrid->GetColCount() == 0)
		return;

	// active cell coordinates
	ROWCOL nRow, nCol;
	if (!pGrid->GetCurrentCell(&nRow, &nCol))
		return;

	// initialize searchparameter structure
	pState->strFind = lpszFind;
	pState->strReplace = lpszReplace;
	pState->bCase = bCase;
	pState->bNext = TRUE;
	pState->PrepareFindReplace();

	CGXLongOperation theOp;
	theOp.SetStatusText(pGrid->m_pAppData->strmReplacingAll, FALSE);
	theOp.SetLockedState(TRUE);
	theOp.SetCanRollback(TRUE);

	if (pState->bTransaction)
		pGrid->BeginTrans(pGrid->m_pAppData->strmReplaceAll);
	pGrid->m_bCanceled = FALSE;

	// check active cell and replace

	pGrid->m_nFirstRow = nRow;

	ROWCOL dwStartRow = nRow;
	ROWCOL dwRowCount = nRow;
	ROWCOL dwRows = 0;

	BOOL bFound = FALSE;

	while (nRow <= pGrid->GetRowCount() && nRow > 0)
	{
		dwRows++;
		if (theOp.NeedMessages())
		{
			if (pState->bNext)
				dwRowCount = pGrid->GetRowCount()-dwStartRow;
			theOp.SetPercentDone((int) (dwRows*100 / dwRowCount));
			theOp.DoMessages(pGrid->m_bCanceled);
			if (pGrid->m_bCanceled)
				break;
		}

		CGXStyle* pStyle = pGrid->CreateStyle();
		pGrid->ComposeStyleRowCol(nRow, nCol, pStyle);
		CGXControl* pControl = pGrid->GetRegisteredControl(pStyle->GetControl());
		ASSERT(pControl != 0);

		pState->m_pStyle = pStyle; // cache style object so that ReplaceAll can resuse it

		if (pControl->ReplaceAll(*pState, nRow, nCol))
			bFound = TRUE;

		pState->m_pStyle = NULL;

		// free style object
		pGrid->RecycleStyle(pStyle);

		nRow = nRow + (pState->bNext ? 1 : -1);

		pGrid->OnTestGridDimension(nRow+1, 0);
	}

	theOp.SetPercentDone(-1);
	theOp.SetLockedState(FALSE);

	if (pGrid->m_bCanceled && !pState->bTransaction)
	{
		AfxThrowUserException();
	}
	else if (pGrid->m_bCanceled && theOp.GetRollbackConfirmedState())
	{
		if (pState->bTransaction)
			pGrid->Rollback();
	}
	else
	{
		if (!bFound)
			pGrid->OnTextNotFound(GXGetLastFRState()->strFind);

		if (pState->bTransaction)
			pGrid->CommitTrans();
	}

	pGrid->RefreshViews();

#ifdef _UNICODE
	// Clean up non UNICODE text buffer
	delete[] pState->lpszMultiByteFindString;
	pState->lpszMultiByteFindString = NULL;
#endif
}

void CGXGridFindReplaceImp::OnFindReplace(CGXGridCore* pGrid, CFindReplaceDialog* pDialog)
{
	GX_FR_STATE* pState = GXGetLastFRState();

	ASSERT(pDialog != NULL);
	ASSERT(pDialog == GXGetLastFRState()->pFindReplaceDlg);

	if (pState->m_pActiveGrid != pGrid)
	{
		// delegate to active grid view
		pState->m_pActiveGrid->OnFindReplace(pDialog);
		return;
	}

	if (pDialog->IsTerminating())
	{
		pState->pFindReplaceDlg = NULL;
	}
	else if (pDialog->FindNext())
	{
		pGrid->OnFindNext(pDialog->GetFindString(),
			pDialog->SearchDown(), pDialog->MatchCase());
	}
	else if (pDialog->ReplaceCurrent())
	{
		ASSERT(!pState->bFindOnly);
		pGrid->OnReplaceSel(pDialog->GetFindString(),
			pDialog->SearchDown(), pDialog->MatchCase(),
			pDialog->GetReplaceString());
	}
	else if (pDialog->ReplaceAll())
	{
		pState->bTransaction = TRUE;
			// Programmer should set this variable = FALSE if he
			// wants to handle his own transaction-like mechanism.
			// E.g. CGXDaoGrid and CGXODBCGrid override OnReplaceAll
			// and set pState->bTransaction = FALSE.
		ASSERT(!pState->bFindOnly);
		pGrid->OnReplaceAll(pDialog->GetFindString(), pDialog->GetReplaceString(),
			pDialog->MatchCase());
	}
}

BOOL CGXGridFindReplaceImp::FindText(CGXGridCore* pGrid, ROWCOL& nRow, ROWCOL& nCol, BOOL bSetCell)
{
	GX_FR_STATE* pState = GXGetLastFRState();

	// search text in cells next to given coordinates (nRow, nCol)

	nRow = nRow + (pState->bNext ? 1 : -1);

	CGXLongOperation theOp;
	theOp.SetStatusText(pGrid->m_pAppData->strmSearching, FALSE);

	ROWCOL dwStartRow = nRow;
	ROWCOL dwRowCount = nRow;
	ROWCOL dwRows = 0;
	BOOL bFound = FALSE;

	while (nRow <= pGrid->GetRowCount() && nRow > 0)
	{
		dwRows++;
		if (theOp.NeedMessages())
		{
			if (pState->bNext)
				dwRowCount = pGrid->GetRowCount()-dwStartRow;
			theOp.SetPercentDone((int) (dwRows*100 / dwRowCount));
			theOp.DoMessages(pGrid->m_bCanceled);
			if (pGrid->m_bCanceled)
				return FALSE;
		}

		CGXStyle* pStyle = pGrid->CreateStyle();
		pGrid->ComposeStyleRowCol(nRow, nCol, pStyle);
		CGXControl* pControl = pGrid->GetRegisteredControl(pStyle->GetControl());
		ASSERT(pControl != 0);

		pState->m_pStyle = pStyle; // cache style object so that ReplaceAll can reuse it

		if (pStyle->GetEnabled() && pControl->FindText(*pState, nRow, nCol, FALSE, bSetCell))
			bFound = TRUE;

		pState->m_pStyle = NULL;

		// free style object
		pGrid->RecycleStyle(pStyle);

		if (bFound)
			break;

		nRow = nRow + (pState->bNext ? 1 : -1);

		pGrid->OnTestGridDimension(nRow+1, 0);
	}

	return bFound;
}

BOOL CGXGridFindReplaceImp::FindText(CGXControl* pControl, const GX_FR_STATE& find, ROWCOL nRow, ROWCOL nCol, BOOL bCurrentPos, BOOL bSetCell)
{
	// Unreferenced parameters:
	find, nRow, nCol, bCurrentPos, bSetCell;

	CGXGridCore* pGrid = pControl->Grid();

	// skip current cell
	if (!bCurrentPos && pGrid->IsCurrentCell(nRow, nCol))
		return FALSE;

	LPCTSTR szValue = NULL;
	LPCTSTR pszFound = NULL;
	LPCTSTR szFind = find.strFindUpper;
		// strFindUpper contains the string to search.
		// if bCase is FALSE (case insensitive search),
		// the text in strFindUpper is converted to uppercase

	CString s;
	BOOL b;
	if (find.m_pStyle)
	{
		b = pControl->GetControlText(s, nRow, nCol, NULL, *find.m_pStyle);
	}
	else
	{
		// request a style object
		CGXStyle* pStyle = pGrid->CreateStyle();

		pGrid->ComposeStyleRowCol(nRow, nCol, pStyle);

		b = pControl->GetControlText(s, nRow, nCol, NULL, *pStyle);

		// free style object
		pGrid->RecycleStyle(pStyle);
	}

	if (b)
	{
		// Convert text to upper case if we are doing a case insensitive search
		if (!find.bCase)
			s.MakeUpper();

		szValue = s;
	}
	else
		szValue = _T("");

	pszFound = _tcsstr(szValue, szFind);

	if (pszFound != NULL)
	{
		if (bSetCell)
			pGrid->SetCurrentCell(nRow, nCol);
		return TRUE;
	}

	return FALSE;
}

BOOL CGXGridFindReplaceImp::ReplaceAll(CGXControl* pControl, const GX_FR_STATE& find, ROWCOL nRow, ROWCOL nCol)
{
	BOOL bFound = FALSE;
	LPCTSTR szValue = NULL;
	LPCTSTR szTemp = NULL;
	LPCTSTR pszFound = NULL;
	LPCTSTR szReplace = find.strReplace;
	LPCTSTR szFind = find.strFindUpper;
		// strFindUpper contains the string to search.
		// if bCase is FALSE (case insensitive search),
		// the text in strFindUpper is converted to uppercase

	CGXGridCore* pGrid = pControl->Grid();

	CString s, sTemp;

	int nStart = 0;

	// request a style object
	CGXStyle* pStyle = find.m_pStyle;

	if (find.m_pStyle)
	{
		pControl->GetControlText(s, nRow, nCol, NULL, *find.m_pStyle);
	}
	else
	{
		// request a style object
		pStyle = pGrid->CreateStyle();

		pGrid->ComposeStyleRowCol(nRow, nCol, pStyle);

		pControl->GetControlText(s, nRow, nCol, NULL, *pStyle);
	}

	// Convert text in s to upper case if we are doing a case insensitive search
	sTemp = s;
	if (!find.bCase)
		sTemp.MakeUpper();
	szValue = s;
	szTemp = sTemp;

	nStart = max(0, nStart);

	int nReplace = _tcslen(szReplace);
	int nFind = _tcslen(szFind);

	// result text
	TCHAR szResult[1024];
	_tcsncpy(szResult, szValue, nStart);
	LPTSTR pszResult = szResult + nStart;

	// original text
	LPCTSTR pszValue = szValue + nStart;

	// text to compare (upper case if case insensitive search)
	LPCTSTR pszTemp  = szTemp + nStart;

	do
	{
		pszFound = _tcsstr(pszTemp, szFind);

		if (pszFound == NULL)
			// copy remaining after last match
			_tcscpy(pszResult, pszValue);
		else
		{
			bFound = TRUE;

			// copy text before match
			_tcsncpy(pszResult, pszValue, pszFound-pszTemp);
			pszResult += pszFound-pszTemp;

			// append replaced text
			_tcsncpy(pszResult, szReplace, nReplace);
			pszResult += nReplace;
			*pszResult = _T('\0');

			// increase pointers in original text (skip matching text)
			pszValue += pszFound-pszTemp+nFind;
			pszTemp = pszFound+nFind;

			// just to be sure ...
			ASSERT(pszValue == szValue + (pszTemp-szTemp));
		}
	}
	while (pszFound && *pszTemp);

	if (bFound)
		pGrid->SetControlTextRowCol(nRow, nCol, szResult, GX_UPDATENOW, pStyle);


	if (find.m_pStyle == NULL && pStyle)
	{
		// free style object
		pGrid->RecycleStyle(pStyle);
	}

	return bFound;
}

