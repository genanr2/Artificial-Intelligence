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

// gxcorfr.cpp : Find and Replace (CGXGridCore)
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


/////////////////////////////////////////////////////////////////////////////
// GX_FR_STATE : last find/replace state

GX_FR_STATE::GX_FR_STATE()
{
	pFindReplaceDlg = NULL;
	bCase = FALSE;
	bNext = TRUE;
	bTransaction = TRUE;
#ifdef _UNICODE
	lpszMultiByteFindString = NULL;
#endif
	m_pActiveGrid = NULL;
	pFindReplaceDlgOwner = NULL;
	m_pStyle = NULL;
}

GX_FR_STATE::~GX_FR_STATE()
{
	strFind.Empty();
	strFindUpper.Empty();
	strReplace.Empty();
#ifdef _UNICODE
	delete[] lpszMultiByteFindString;
#endif
}

void GX_FR_STATE::PrepareFindReplace()
{
	strFindUpper = strFind;
	if (!bCase)
		strFindUpper.MakeUpper();

#ifdef _UNICODE
	// provide also a non UNICODE string
	delete[] lpszMultiByteFindString;

	int nLen = _tcsclen(strFind) + 1;
	lpszMultiByteFindString = new char[nLen];
	WideCharToMultiByte (CP_ACP, 0, (LPCWSTR) strFind, -1,
		lpszMultiByteFindString, nLen, NULL, NULL);
#endif
}

#if !defined(_GXDLL)
GX_FR_STATE GXNEAR _gxLastFRState;
#endif


/////////////////////////////////////////////////////////////////////////////
// GX_FRSTATE : last find/replace state

void CGXGridCore::GetSelectedText(CString& strResult)
{
	ROWCOL nRow, nCol;

	if (GetCurrentCell(nRow, nCol))
	{
		CGXControl* pControl = GetControl(nRow, nCol);

		pControl->GetSelectedText(strResult);
	}
	else
		strResult.Empty();
}

void CGXGridCore::OnShowFindReplaceDialog(BOOL bFindOnly)
{
	GX_CHECKIMP(m_pFindReplaceImp, "CGXGridCore::OnShowFindReplaceDialog");

	if (m_pFindReplaceImp)
		m_pFindReplaceImp->OnShowFindReplaceDialog(this, bFindOnly);
}

void CGXGridCore::OnFindNext(LPCTSTR lpszFind, BOOL bNext, BOOL bCase)
{
	GX_CHECKIMP(m_pFindReplaceImp, "CGXGridCore::OnFindNext");

	if (m_pFindReplaceImp)
		m_pFindReplaceImp->OnFindNext(this, lpszFind, bNext, bCase);
}


BOOL CGXGridCore::FindText(BOOL bSetCell /*=TRUE*/)
{
	GX_CHECKIMP(m_pFindReplaceImp, "CGXGridCore::FindText");

	if (m_pFindReplaceImp)
		return m_pFindReplaceImp->FindText(this, bSetCell);

	return FALSE;
}

void CGXGridCore::OnReplaceSel(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, LPCTSTR lpszReplace)
{
	GX_CHECKIMP(m_pFindReplaceImp, "CGXGridCore::OnReplaceSel");

	if (m_pFindReplaceImp)
		m_pFindReplaceImp->OnReplaceSel(this, lpszFind, bNext, bCase, lpszReplace);
}

void CGXGridCore::OnReplaceAll(LPCTSTR lpszFind, LPCTSTR lpszReplace, BOOL bCase)
{
	GX_CHECKIMP(m_pFindReplaceImp, "CGXGridCore::OnReplaceAll");

	if (m_pFindReplaceImp)
		m_pFindReplaceImp->OnReplaceAll(this, lpszFind, lpszReplace, bCase);
}

void CGXGridCore::OnFindReplace(CFindReplaceDialog* pDialog)
{
	GX_CHECKIMP(m_pFindReplaceImp, "CGXGridCore::OnFindReplace");

	if (m_pFindReplaceImp)
		m_pFindReplaceImp->OnFindReplace(this, pDialog);
}

BOOL CGXGridCore::FindText(ROWCOL& nRow, ROWCOL& nCol, BOOL bSetCell)
{
	GX_CHECKIMP(m_pFindReplaceImp, "CGXGridCore::FindText");

	if (m_pFindReplaceImp)
		return m_pFindReplaceImp->FindText(this, nRow, nCol, bSetCell);

	return FALSE;
}

void CGXGridCore::OnTextNotFound(LPCTSTR)
{
	MessageBeep(0);
}

