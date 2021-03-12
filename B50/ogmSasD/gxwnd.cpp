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

// gxwnd.cpp : implementation of CGXGridWnd class
//

#include "stdafx.h"

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

// Headers

#ifndef _GXWND_H_
#include "gxwnd.h"
#endif

#ifndef _GXMSG_H_
#include "gxmsg.h"
#endif

#ifndef _GXCTRL_H_
#include "gxctrl.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXWND")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CGXGridWnd, CWnd)
IMPLEMENT_REGISTER(CGXGridWnd, CS_DBLCLKS | GX_GLOBALCLASS, 0, 0, 0)

static UINT NEAR wm_Find = RegisterWindowMessage( FINDMSGSTRING );

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CGXGridWnd support for dialog validation

void AFXAPI DDV_GXGridWnd(CDataExchange* pDX, CGXGridCore* pGrid)
{
	if (pDX->m_bSaveAndValidate)
	{
		pDX->PrepareCtrl(pGrid->GridWnd()->GetDlgCtrlID());
		if (!pGrid->CanActivateGrid(FALSE))
			pDX->Fail();
		else
			pGrid->TransferCurrentCell();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGXGridWnd

CGXGridWnd::CGXGridWnd()
{
	SetGridWnd(this);
	m_bDrawSunken = FALSE;
}

BOOL CGXGridWnd::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::Create(NULL, NULL,
		dwStyle | WS_CHILD,
		rect, pParentWnd, nID);
}

#if _MFC_VER < 0x0300
WNDPROC* CGXGridWnd::GetSuperWndProcAddr()
{
	// Note: it is no longer necessary to override GetSuperWndProcAddr
	//  for each control class with a different WNDCLASS.
	//  This implementation now uses instance data, such that the previous
	//  WNDPROC can be anything.

	static WNDPROC NEAR pfnSuper;
	return &pfnSuper;
}
#endif

BEGIN_MESSAGE_MAP(CGXGridWnd, CWnd)
	//{{AFX_MSG_MAP(CGXGridWnd)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_HSCROLL()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_GETDLGCODE()
	ON_WM_SETFOCUS()
	ON_MESSAGE(WM_GX_CHANGEDTAB, OnChangedTab)
	ON_MESSAGE(WM_GX_NEEDCHANGETAB, OnNeedChangeTab)
	ON_MESSAGE(WM_GX_CANACTIVATE, OnCanActivate)
	ON_MESSAGE(WM_GX_MOVERECORD, OnMoveRecord)
	ON_MESSAGE(WM_GX_LOCKRESIZE, OnLockResize)
	ON_WM_CHAR()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSCHAR()
	ON_WM_SYSKEYUP()
	ON_WM_KILLFOCUS()
	ON_WM_CANCELMODE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SHOWWINDOW()
	ON_WM_KEYUP()
	ON_WM_MOUSEACTIVATE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_NCPAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(wm_Find, OnFindReplaceCmd)
	ON_WM_WININICHANGE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGXGridWnd initialization

void CGXGridWnd::Initialize()
{
	// is it embedded in a CGXTabWnd or CGXRecordInfoWnd?
	if (GetSharedScrollbarParentWnd())
		// use shared scrollbars
		SetScrollBarMode(SB_BOTH, gxnShared | gxnEnhanced, FALSE);

	// no, turn on scroll bars depending on the style-bits of the window
	// (the programmer can specify them in AppStudio or directly when
	// calling the Create method)
	else
	{
		DWORD dwStyle = GetStyle();
		if ((DWORD) (dwStyle & WS_VSCROLL))
			SetScrollBarMode(SB_VERT, gxnAutomatic | gxnEnhanced, FALSE);
		else
			SetScrollBarMode(SB_VERT, gxnDisabled, FALSE);

		if ((DWORD) (dwStyle & WS_HSCROLL))
			SetScrollBarMode(SB_HORZ, gxnAutomatic | gxnEnhanced, FALSE);
		else
			SetScrollBarMode(SB_HORZ, gxnDisabled, FALSE);
	}

	OnInitialUpdate();
}

void CGXGridWnd::OnInitialUpdate()
{
	OnGridInitialUpdate();
}

#define _GXGetDlgCtrlID(hWnd)          ((UINT)(WORD)::GetDlgCtrlID(hWnd))

UINT AFXAPI CGXGridWnd::GetParentTabViewID(const CWnd* pWnd)
{
	UINT nID = _GXGetDlgCtrlID(pWnd->m_hWnd);
	ASSERT(nID >= AFX_IDW_PANE_FIRST && nID < AFX_IDW_PANE_LAST);
	return nID-AFX_IDW_PANE_FIRST;
}

CScrollBar* CGXGridWnd::GetScrollBarCtrl(int nBar) const
{
	CScrollBar* pBar = DoGetScrollBarCtrl(nBar);
	if (pBar != NULL)
		return pBar;

	return CWnd::GetScrollBarCtrl(nBar);
}

LRESULT CGXGridWnd::OnChangedTab(WPARAM wParam, LPARAM /*lParam*/)
{
	OnTabChanged((int) wParam);

	return 0;
}

LRESULT CGXGridWnd::OnNeedChangeTab(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	return CanChangeTab();
}

LRESULT CGXGridWnd::OnCanActivate(WPARAM wParam, LPARAM /*lParam*/)
{
	BOOL bCanActivate = CanActivateGrid((BOOL) wParam);

	// Please note:
	// activation is allowed when return value is 0
	// activation is disabled when return value is 1
	return bCanActivate ? 0 : 1;
}

LRESULT CGXGridWnd::OnLockResize(WPARAM wParam, LPARAM )
{
	LockResize((BOOL) wParam);
	return 0;
}

LRESULT CGXGridWnd::OnMoveRecord(WPARAM wParam, LPARAM lParam)
{
	SetFocus();
	OnInfoWndMoveRecord((int) wParam, (long) lParam);
	return 0;
}

LRESULT CGXGridWnd::OnFindReplaceCmd(WPARAM, LPARAM lParam)
{
	CFindReplaceDialog* pDialog = CFindReplaceDialog::GetNotifier(lParam);

	OnFindReplace(pDialog);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CGXGridWnd message handlers

void CGXGridWnd::OnPaint()
{
	// standard paint routine
	if (!m_bInitDone)
	{
		TRACE0("Warning! You must call CGXGridWnd::Initialize() in OnInitDialog!\n");
		ASSERT(0);        // You must call Initialize() in OnInitDialog!
		AfxThrowNotSupportedException();
	}

	CPaintDC dc(this);
	OnGridPrepareDC(&dc);
	OnGridDraw(&dc);
}

void CGXGridWnd::OnNcPaint()
{
	// Draw scrollbars and other non-client area
	Default();

	if (m_bDrawSunken
#if _MFC_VER >= 0x0400
		|| (GetStyle() & WS_EX_CLIENTEDGE) > 0
#endif
		)
	{
		// Check if we have a valid parent window
		CWnd* pParentWnd = GetParent();
		if (pParentWnd == NULL)
			return;

		// Get device context for painting in parent window
		CClientDC dc(pParentWnd);

		// Grid rectangle
		CRect r;
		GetWindowRect( &r );
		pParentWnd->ScreenToClient( &r );

		// Add space for sunken border
		r.left -= 2;
		r.top -= 2;
		r.right += 3;
		r.bottom += 3;

		// Draw sunken border
		_GXDrawSunkenBorders(&dc, r, TRUE, TRUE);
	}
}

HBRUSH CGXGridWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return DoCtlColor(pDC, pWnd, nCtlColor);
}

void CGXGridWnd::OnDestroy()
{
	if (!DoNotifyDestroy())
		CWnd::OnDestroy();
}

void CGXGridWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (!DoHScroll(nSBCode, nPos, pScrollBar))
		CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CGXGridWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!ProcessKeys(this, WM_KEYDOWN, nChar, nRepCnt, nFlags))
		CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CGXGridWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!ProcessKeys(this, WM_KEYUP, nChar, nRepCnt, nFlags))
		CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CGXGridWnd::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!ProcessKeys(this, WM_SYSKEYDOWN, nChar, nRepCnt, nFlags))
		CWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CGXGridWnd::OnSysChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!ProcessKeys(this, WM_SYSCHAR, nChar, nRepCnt, nFlags))
		CWnd::OnSysChar(nChar, nRepCnt, nFlags);
	nChar, nRepCnt, nFlags;
}

void CGXGridWnd::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!ProcessKeys(this, WM_SYSKEYUP, nChar, nRepCnt, nFlags))
		CWnd::OnSysKeyUp(nChar, nRepCnt, nFlags);
	nChar, nRepCnt, nFlags;
}

void CGXGridWnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!ProcessKeys(this, WM_CHAR, nChar, nRepCnt, nFlags))
		CWnd::OnChar(nChar, nRepCnt, nFlags);
}

void CGXGridWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd* pFocus = GetFocus();
	if (pFocus != this && (pFocus == NULL || !IsChild(pFocus)))
		SetFocus();

	if (!DoLButtonDown(nFlags, point))
		CWnd::OnLButtonDown(nFlags, point);
}

void CGXGridWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!DoLButtonUp(nFlags, point))
		CWnd::OnLButtonUp(nFlags, point);
}

void CGXGridWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!DoMouseMove(nFlags, point))
		CWnd::OnMouseMove(nFlags, point);
}

void CGXGridWnd::OnLButtonDblClk(UINT flags, CPoint point)
{
	if (!DoLButtonDblClk(flags, point))
		CWnd::OnLButtonDblClk(flags, point);
}

void CGXGridWnd::OnMButtonDblClk(UINT flags, CPoint point)
{
	if (!DoMButtonDblClk(flags, point))
		CWnd::OnMButtonDblClk(flags, point);
}

void CGXGridWnd::OnMButtonDown(UINT flags, CPoint point)
{
	CWnd* pFocus = GetFocus();
	if (pFocus != this && (pFocus == NULL || !IsChild(pFocus)))
		SetFocus();

	if (!DoMButtonDown(flags, point))
		CWnd::OnMButtonDown(flags, point);
}

void CGXGridWnd::OnMButtonUp(UINT flags, CPoint point)
{
	if (!DoMButtonUp(flags, point))
		CWnd::OnMButtonUp(flags, point);
}

void CGXGridWnd::OnRButtonDblClk(UINT flags, CPoint point)
{
	if (!DoRButtonDblClk(flags, point))
		CWnd::OnRButtonDblClk(flags, point);
}

void CGXGridWnd::OnRButtonDown(UINT flags, CPoint point)
{
	CWnd* pFocus = GetFocus();
	if (pFocus != this && (pFocus == NULL || !IsChild(pFocus)))
		SetFocus();

	if (!DoRButtonDown(flags, point))
		CWnd::OnRButtonDown(flags, point);
}

void CGXGridWnd::OnRButtonUp(UINT flags, CPoint point)
{
	if (!DoRButtonUp(flags, point))
		CWnd::OnRButtonUp(flags, point);
}

BOOL CGXGridWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

BOOL CGXGridWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (DoSetCursor(pWnd, nHitTest, message))
		return TRUE;
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CGXGridWnd::OnSize(UINT nType, int cx, int cy)
{
	if (!DoSize(nType, cx, cy))
		CWnd::OnSize(nType, cx, cy);
}

void CGXGridWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);
	DoShowWindow(bShow, nStatus);
}

void CGXGridWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (!DoVScroll(nSBCode, nPos, pScrollBar))
		CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CGXGridWnd::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);
	OnGridSetFocus(pOldWnd);
}

void CGXGridWnd::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);
	OnGridKillFocus(pNewWnd);
}

UINT CGXGridWnd::OnGetDlgCode()
{
	return DoGetDlgCode(CWnd::OnGetDlgCode());
}

void CGXGridWnd::OnCancelMode()
{
	// TRACE0("OnCancelMode\n");
	DoCancelMode();
}

int CGXGridWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	int retval;

	if (DoMouseActivate(pDesktopWnd, nHitTest, message, retval))
		return retval;

	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

BOOL CGXGridWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (DoCommand(wParam, lParam))
		return TRUE;

	return CWnd::OnCommand(wParam, lParam);
}

BOOL CGXGridWnd::PreTranslateMessage(MSG* pMsg)
{
	if (OnGridPreTranslateMessage(pMsg))
		return TRUE;

	return CWnd::PreTranslateMessage(pMsg);
}

LRESULT CGXGridWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT result;

	if (OnGridWindowProc(message, wParam, lParam, result))
		return result;

	return CWnd::WindowProc(message, wParam, lParam);
}

void CGXGridWnd::OnWinIniChange(LPCTSTR lpszSection)
{
	OnGridWinIniChange(lpszSection);
	CWnd::OnWinIniChange(lpszSection);
}

void CGXGridWnd::OnSysColorChange()
{
	OnGridSysColorChange();
	CWnd::OnSysColorChange();
}

void CGXGridWnd::OnTimer(UINT nIDEvent)
{
	OnGridTimer(nIDEvent);

	CWnd::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
// CGXGridWnd printing

void CGXGridWnd::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	OnGridBeginPrinting(pDC, pInfo);
}

void CGXGridWnd::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	OnGridEndPrinting(pDC, pInfo);
}

void CGXGridWnd::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	OnGridPrint(pDC, pInfo);
}

/////////////////////////////////////////////////////////////////////////////
// CGXGridWnd diagnostics

#ifdef _DEBUG
void CGXGridWnd::AssertValid() const
{
	CGXGridCore::AssertValid();
	CWnd::AssertValid();
}
/*
void CGXGridWnd::Dump(CDumpContext& dc) const
{
//	CGXGridCore::Dump(dc);
//	CWnd::Dump(dc);
}
*/
#endif //_DEBUG
