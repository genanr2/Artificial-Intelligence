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

// gxccombo.cpp : implementation of the CGXGridCombo class
//

#include "stdafx.h"

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

// Headers

#ifndef _GXCORE_H_
#include "gxcore.h"
#endif

#ifndef _GXCTRLI_H_
#include "gxctrli.h"
#endif

#ifndef _GXDLL_H_
#include "gxdll.h" // GXGetResourceHandle
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCCOMBO")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

#ifndef WS_EX_TOOLWINDOW
#define WS_EX_TOOLWINDOW (GXGetAppData()->bWin4?0x00000080L:0L)
#endif

CGXGridCombo::CGXGridCombo(CWnd* pGridWnd, CWnd* pWnd, const CRect& rect)
{
	// Unused
	pWnd;

	m_pGridWnd = pGridWnd;
	m_pListBox = NULL;
	m_bColor = FALSE;

	SetComboBoxDropDown(TRUE);

	if (!CreateEx(
		WS_EX_TOOLWINDOW, GetClassName(), NULL, WS_POPUP | WS_VISIBLE,
		rect.left-1, rect.top, rect.Width()+1, rect.Height(),
		::GetDesktopWindow(), NULL))
	{
		TRACE0("Failed to create popup window in CGXGridCombo\n");
		ASSERT(0);
		// ASSERTION-> Failed to create popup window
		// Did you register the windowclass with CGXGridCombo::RegisterClass? ->END
	}

	SetComboBoxDropDown(FALSE);

	ASSERT_VALID(this);
}

void CGXGridCombo::AttachListBox(CWnd* pListBox)
{
	ASSERT_VALID(pListBox);

	m_pListBox = pListBox;
}

CGXGridCombo::~CGXGridCombo()
{
	if (m_pListBox)
		delete m_pListBox;
}

void AFXAPI CGXGridCombo::RegisterClass()
{
	GXGetAppData()->sComboClassName = AfxRegisterWndClass(CS_SAVEBITS);
}

void AFXAPI CGXGridCombo::UnregisterClass()
{
	if (GXGetAppData()->sComboClassName.GetLength() > 0)
		::UnregisterClass(GXGetAppData()->sComboClassName, GXGetResourceHandle());
	GXGetAppData()->sComboClassName.Empty();
}

BOOL AFXAPI CGXGridCombo::GetComboBoxDropDown()
{
	return GXGetAppData()->m_bDiscardNcActivate;
}

CString AFXAPI CGXGridCombo::GetClassName()
{
	return GXGetAppData()->sComboClassName;
}

void AFXAPI CGXGridCombo::SetComboBoxDropDown(BOOL b)
{
	GXGetAppData()->m_bDiscardNcActivate = b;
}

void CGXGridCombo::SetColor(COLORREF rgbText, COLORREF rgbBack)
{
	br.DeleteObject();
	m_bColor = TRUE;
	m_rgbText = rgbText;
	m_rgbBack = rgbBack;
	br.CreateSolidBrush(rgbBack);
}

BEGIN_MESSAGE_MAP(CGXGridCombo, CWnd)
	//{{AFX_MSG_MAP(CGXGridCombo)
	ON_WM_CTLCOLOR()
	ON_WM_ACTIVATE()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ======== CGXGridCombo message handlers

HBRUSH CGXGridCombo::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (m_bColor)
	{
		pDC->SetBkColor(m_rgbBack);
		pDC->SetTextColor(m_rgbText);
		return (HBRUSH) br.GetSafeHandle();
	}

	return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL CGXGridCombo::OnCommand(WPARAM wParam, LPARAM lParam)
{
#if _MFC_VER < 0x0300
	// UINT nNotification = HIWORD(lParam);
	HWND hCtl = (HWND) LOWORD(lParam);
#else
	// UINT nNotification = HIWORD(wParam);
	HWND hCtl = (HWND) lParam;
#endif

	if (hCtl == m_pListBox->GetSafeHwnd())
		return (BOOL) m_pGridWnd->SendMessage(WM_COMMAND, wParam, lParam);

	return CWnd::OnCommand(wParam, lParam);
}

void CGXGridCombo::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	ASSERT_VALID(this);

	CWnd::OnActivate(nState, pWndOther, bMinimized);
}

void CGXGridCombo::OnSysCommand(UINT nID, LPARAM lParam) 
{
	// Don't allow WM_SYSCOMMAND messages while dropdown list 
	// is visible

	// Unused:
	nID, lParam;

	return; 
}
