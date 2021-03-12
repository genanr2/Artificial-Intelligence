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

// gxvw.cpp : implementation of the CGXGridView class
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

#ifndef _GXVW_H_
#include "gxvw.h"
#endif

#ifndef _GXMSG_H_
#include "gxmsg.h"
#endif

#ifndef _GXSTATE_H_
#include "gxstate.h"
#endif

#ifndef _GXCTRL_H_
#include "gxctrl.h"
#endif

#ifndef _GXEXT_H_
#include "gxext.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXVW")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CGXGridView, CGXView)

static UINT NEAR wm_Find = RegisterWindowMessage( FINDMSGSTRING );

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CGXGridView

BEGIN_MESSAGE_MAP(CGXGridView, CGXView)
	//{{AFX_MSG_MAP(CGXGridView)
	ON_WM_DESTROY()
	ON_WM_SETCURSOR()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_KEYDOWN()
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_REPLACE, OnEditReplace)
	ON_COMMAND(ID_EDIT_REPEAT, OnEditRepeat)
	ON_REGISTERED_MESSAGE(wm_Find, OnFindReplaceCmd)
	ON_WM_SETFOCUS()
	ON_MESSAGE(WM_GX_NEEDCHANGETAB, OnNeedChangeTab)
	ON_MESSAGE(WM_GX_CHANGEDTAB, OnChangedTab)
	ON_MESSAGE(WM_GX_CANACTIVATE, OnCanActivate)
	ON_MESSAGE(WM_GX_MOVERECORD, OnMoveRecord)
	ON_MESSAGE(WM_GX_LOCKRESIZE, OnLockResize)
	ON_WM_CHAR()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_CANCELMODE()
	ON_WM_SYSKEYDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_MOUSEACTIVATE()
	ON_WM_KEYUP()
	ON_WM_SYSKEYUP()
	ON_WM_SYSCHAR()
	ON_WM_PAINT()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_TIMER()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_WM_WININICHANGE()
	ON_COMMAND(ID_FILE_PRINT, CGXView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CGXView::OnFilePrintPreview)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGXGridView construction/destruction

CGXGridView::CGXGridView()
{
	SetGridWnd(this);
}

CGXGridView::~CGXGridView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CGXGridView initialization

void CGXGridView::OnInitialUpdate()
{
	SetGridWnd(this, GetDocument(), TRUE);

	m_pSplitterWnd = GetParentDynamicSplitter(this, TRUE);

	// Commment: I have move the following lines before
	// CGXGridCore::OnGridInitialUpdate() so that scrollbars
	// can be initialized correctly.
	if (m_pSplitterWnd || GetSharedScrollbarParentWnd())
		SetScrollBarMode(SB_BOTH, gxnShared | gxnEnhanced);
	else
		SetScrollBarMode(SB_BOTH, gxnAutomatic | gxnEnhanced);

	OnGridInitialUpdate();
	CGXView::OnInitialUpdate();

	CGXGridParam* pParam = GetParam();

	// check print device
	if (GetPrintDevice() == NULL)
	{
		// is print device in parameter object?
		if (pParam->GetPrintDevice() == NULL)
			pParam->SetPrintDevice(new CGXPrintDevice);

		SetPrintDevice(pParam->GetPrintDevice(), FALSE);
	}

	// if this is a new pane in a dynamic splitter window
	// I will initialize top row or left column

	if (m_pSplitterWnd != NULL)
	{
		ASSERT(m_pSplitterWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)));
		VERIFY(m_pSplitterWnd->IsChildPane(this, m_nSplitRow, m_nSplitCol));

		ASSERT(m_nSplitRow < 2);
		ASSERT(m_nSplitCol < 2);

		if (m_nSplitRow > 0 || m_nSplitCol > 0)
		{
			// copy settings from other pane
			CGXGridView *pView;

			pView = (CGXGridView *) m_pSplitterWnd->GetPane(0, 0);

			ASSERT(pView != NULL);
			if (pView->IsKindOf(RUNTIME_CLASS(CGXGridView)))
			{
				if (m_nSplitRow > 0)
				{
					pView = (CGXGridView *) m_pSplitterWnd->GetPane(0, m_nSplitCol);

					ASSERT(pView != NULL);
					if (pView->IsKindOf(RUNTIME_CLASS(CGXGridView)))
					{
						m_nLeftCol = pView->m_nLeftCol;
						m_nTopRow = pView->m_nTopRow;
						m_bDisplayHeaderRow = FALSE;

						// disable smart redrawing of WM_SIZE message
						pView->m_cxOld = pView->m_cyOld = 0;
					}
				}

				if (m_nSplitCol > 0)
				{
					pView = (CGXGridView *) m_pSplitterWnd->GetPane(m_nSplitRow, 0);

					ASSERT(pView != NULL);
					if (pView->IsKindOf(RUNTIME_CLASS(CGXGridView)))
					{
						m_nTopRow = pView->m_nTopRow;
						if (m_nSplitRow == 0)
							m_nLeftCol = pView->m_nLeftCol;
						m_bDisplayHeaderCol = FALSE;

						// disable smart redrawing of WM_SIZE message
						pView->m_cxOld = pView->m_cyOld = 0;
					}
				}
			}

		}

		UpdateScrollbars();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGXGridView support for CGXTabWnd/CGXRecordInfoWnd

LRESULT CGXGridView::OnChangedTab(WPARAM wParam, LPARAM /*lParam*/)
{
	OnTabChanged((int) wParam);

	return 0;
}

LRESULT CGXGridView::OnCanActivate(WPARAM wParam, LPARAM /*lParam*/)
{
	BOOL bCanActivate = CanActivateGrid((BOOL) wParam);

	// Please note:
	// activation is allowed when return value is 0
	// activation is disabled when return value is 1
	return bCanActivate ? 0 : 1;
}

LRESULT CGXGridView::OnNeedChangeTab(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	return CanChangeTab();
}

LRESULT CGXGridView::OnLockResize(WPARAM wParam, LPARAM )
{
	LockResize((BOOL) wParam);
	return 0;
}

LRESULT CGXGridView::OnMoveRecord(WPARAM wParam, LPARAM lParam)
{
	OnInfoWndMoveRecord((int) wParam, (long) lParam);
	return 0;
}

CScrollBar* CGXGridView::GetScrollBarCtrl(int nBar) const
{
	CScrollBar* pBar = DoGetScrollBarCtrl(nBar);
	if (pBar != NULL)
		return pBar;

	return CGXView::GetScrollBarCtrl(nBar);
}

/////////////////////////////////////////////////////////////////////////////
// CView overridable member functions

void CGXGridView::OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView )
{
	if (bActivate != m_bActiveFrame && !GXDiscardNcActivate())
		OnActivateGrid(bActivate);
	CGXView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CGXGridView::OnDraw(CDC* pDC)
{
	OnGridDraw(pDC);
}

void CGXGridView::OnPrepareDC( CDC* pDC, CPrintInfo* pInfo /* = NULL */)
{
	OnGridPrepareDC(pDC, pInfo);
}

void CGXGridView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	OnGridUpdate(pSender, lHint, pHint);
}

BOOL CGXGridView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (DoCommand(wParam, lParam))
		return TRUE;

	return CGXView::OnCommand(wParam, lParam);
}

BOOL CGXGridView::PreTranslateMessage(MSG* pMsg)
{
	if (OnGridPreTranslateMessage(pMsg))
		return TRUE;

	return CGXView::PreTranslateMessage(pMsg);
}

LRESULT CGXGridView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT result;

	if (OnGridWindowProc(message, wParam, lParam, result))
		return result;

	return CGXView::WindowProc(message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
// CGXGridView printing

BOOL CGXGridView::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->SetMaxPage(0xffff);

	pInfo->m_pPD->m_pd.Flags &= ~PD_NOSELECTION;
	pInfo->m_pPD->m_pd.hInstance = AfxGetInstanceHandle();

	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGXGridView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	OnGridBeginPrinting(pDC, pInfo);
}

void CGXGridView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	OnGridEndPrinting(pDC, pInfo);
}

void CGXGridView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	OnGridPrint(pDC, pInfo);
}


/////////////////////////////////////////////////////////////////////////////
// CGXGridView diagnostics

#ifdef _DEBUG
void CGXGridView::AssertValid() const
{
	CGXView::AssertValid();
}
/*
void CGXGridView::Dump(CDumpContext& dc) const
{
//	CGXView::Dump(dc);
}
*/
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CGXGridView message handlers

void CGXGridView::OnPaint()
{
	// standard paint routine
	if (!m_bInitDone)
	{
		TRACE0("Warning! You must call CGXGridView::OnInitialUpdate()\n");
		ASSERT(0);        // You must call Initialize() in OnInitDialog!
		AfxThrowNotSupportedException();
	}

	CPaintDC dc(this);
	OnPrepareDC(&dc);
	OnDraw(&dc);
}

void CGXGridView::OnDestroy()
{
	if (!DoNotifyDestroy())
		CGXView::OnDestroy();
}

void CGXGridView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pBar)
{
	if (!DoVScroll(nSBCode, nPos, pBar))
		CGXView::OnVScroll(nSBCode, nPos, pBar);
}

void CGXGridView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pBar)
{
	if (!DoHScroll(nSBCode, nPos, pBar))
		CGXView::OnHScroll(nSBCode, nPos, pBar);
}

BOOL CGXGridView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (DoSetCursor(pWnd, nHitTest, message))
		return TRUE;
	return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CGXGridView::OnMouseMove(UINT flags, CPoint point)
{
	if (!DoMouseMove(flags, point))
		CGXView::OnMouseMove(flags, point);
}

int CGXGridView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// First, check out current control's
	int retval;

	if (DoMouseActivate(pDesktopWnd, nHitTest, message, retval))
		return retval;

	return CGXView::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CGXGridView::OnLButtonDown(UINT flags, CPoint point)
{
	if (!DoLButtonDown(flags, point))
		CGXView::OnLButtonDown(flags, point);
}

void CGXGridView::OnLButtonUp(UINT flags, CPoint point)
{
	if (!DoLButtonUp(flags, point))
		CGXView::OnLButtonUp(flags, point);
}


void CGXGridView::OnRButtonDown(UINT flags, CPoint point)
{
	if (!DoRButtonDown(flags, point))
		CGXView::OnRButtonDown(flags, point);
}

void CGXGridView::OnRButtonUp(UINT flags, CPoint point)
{
	if (!DoRButtonUp(flags, point))
		CGXView::OnRButtonUp(flags, point);
}

void CGXGridView::OnLButtonDblClk(UINT flags, CPoint point)
{
	if (!DoLButtonDblClk(flags, point))
		CGXView::OnLButtonDblClk(flags, point);
}

void CGXGridView::OnRButtonDblClk(UINT flags, CPoint point)
{
	if (!DoRButtonDblClk(flags, point))
		CGXView::OnRButtonDblClk(flags, point);
}

void CGXGridView::OnMButtonDown(UINT flags, CPoint point)
{
	if (!DoMButtonDown(flags, point))
		CGXView::OnLButtonDown(flags, point);
}

void CGXGridView::OnMButtonUp(UINT flags, CPoint point)
{
	if (!DoMButtonUp(flags, point))
		CGXView::OnMButtonUp(flags, point);
}

void CGXGridView::OnMButtonDblClk(UINT flags, CPoint point)
{
	if (!DoMButtonDblClk(flags, point))
		CGXView::OnMButtonDblClk(flags, point);
}

BOOL CGXGridView::OnEraseBkgnd(CDC* /*pDC*/)
{
	// I don't want the Screen to flicker!
	// Also, ...
	return TRUE;
}

void CGXGridView::OnSize(UINT nType, int cx, int cy)
{
	if (!DoSize(nType, cx, cy))
		CGXView::OnSize(nType, cx, cy);
}

void CGXGridView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CGXView::OnShowWindow(bShow, nStatus);
	DoShowWindow(bShow, nStatus);
}

HBRUSH CGXGridView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return DoCtlColor(pDC, pWnd, nCtlColor);
}

void CGXGridView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT flags)
{
	if (!ProcessKeys(this, WM_KEYDOWN, nChar, nRepCnt, flags))
		CGXView::OnKeyDown(nChar, nRepCnt, flags);
}

void CGXGridView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT flags)
{
	if (!ProcessKeys(this, WM_KEYUP, nChar, nRepCnt, flags))
		CGXView::OnKeyUp(nChar, nRepCnt, flags);
}

void CGXGridView::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!ProcessKeys(this, WM_SYSKEYDOWN, nChar, nRepCnt, nFlags))
		CGXView::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CGXGridView::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!ProcessKeys(this, WM_SYSKEYUP, nChar, nRepCnt, nFlags))
		CGXView::OnSysKeyUp(nChar, nRepCnt, nFlags);
}

void CGXGridView::OnSysChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!ProcessKeys(this, WM_SYSCHAR, nChar, nRepCnt, nFlags))
		CGXView::OnSysChar(nChar, nRepCnt, nFlags);
}

void CGXGridView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!ProcessKeys(this, WM_CHAR, nChar, nRepCnt, nFlags))
		CGXView::OnChar(nChar, nRepCnt, nFlags);
}

void CGXGridView::OnSetFocus(CWnd* pOldWnd)
{
	CGXView::OnSetFocus(pOldWnd);
	OnGridSetFocus(pOldWnd);
}

void CGXGridView::OnCancelMode()
{
	// TRACE0("OnCancelMode\n");
	DoCancelMode();
}

void CGXGridView::OnWinIniChange(LPCTSTR lpszSection)
{
	OnGridWinIniChange(lpszSection);
	CGXView::OnWinIniChange(lpszSection);
}

void CGXGridView::OnSysColorChange()
{
	OnGridSysColorChange();
	CGXView::OnSysColorChange();
}

void CGXGridView::OnTimer(UINT nIDEvent)
{
	OnGridTimer(nIDEvent);

	CGXView::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
// Menu_Actions

void CGXGridView::OnUpdateEditCopy(CCmdUI* pCmdUI)
	{ pCmdUI->Enable(CanCopy()); }

void CGXGridView::OnEditCopy()
	{ Copy(); }

void CGXGridView::OnEditCut()
	{ Cut(); }

void CGXGridView::OnUpdateEditCut(CCmdUI* pCmdUI)
	{ pCmdUI->Enable(CanCut()); }

void CGXGridView::OnEditPaste()
	{ Paste(); }

void CGXGridView::OnUpdateEditPaste(CCmdUI* pCmdUI)
	{ pCmdUI->Enable(CanPaste()); }

void CGXGridView::OnEditUndo()
	{ Undo(); }

void CGXGridView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	CanUndo(pCmdUI);
}

void CGXGridView::OnEditRedo()
{
	Redo();
}

void CGXGridView::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	CanRedo(pCmdUI);
}

/////////////////////////////////////////////////////////////////////////////
// CEditView Find & Replace

void CGXGridView::OnEditFind()
{
	ASSERT_VALID(this);
	OnShowFindReplaceDialog(TRUE);
	ASSERT_VALID(this);
}

void CGXGridView::OnEditReplace()
{
	ASSERT_VALID(this);
	OnShowFindReplaceDialog(FALSE);
	ASSERT_VALID(this);
}

void CGXGridView::OnEditRepeat()
{
	ASSERT_VALID(this);
	if (!FindText())
		OnTextNotFound(GXGetLastFRState()->strFind);
	ASSERT_VALID(this);
}

LRESULT CGXGridView::OnFindReplaceCmd(WPARAM, LPARAM lParam)
{
	ASSERT_VALID(this);
	CFindReplaceDialog* pDialog = CFindReplaceDialog::GetNotifier(lParam);

	OnFindReplace(pDialog);

	ASSERT_VALID(this);
	return 0;
}

void CGXGridView::OnKillFocus(CWnd* pNewWnd) 
{
	CGXView::OnKillFocus(pNewWnd);

	// Set m_bIgnoreFocus = FALSE so that OnGridKillFocus
	// will not fire a OnActivateGrid event. OnActivateGrid 
	// already gets called from OnActivateView.
	
	BOOL b = m_bIgnoreFocus;
	m_bIgnoreFocus = TRUE;
	OnGridKillFocus(pNewWnd);
	m_bIgnoreFocus = b;
}
