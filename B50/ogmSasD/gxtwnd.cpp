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

// gxtwnd.cpp : implementation of workbook classes
//

#include "stdafx.h"

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

// Headers

#ifndef _GXTWND_H_
#include "gxtwnd.h"
#endif

#ifndef _AFXPRIV_H_
#include "afxpriv.h"
#endif

#ifndef _GXSTATE_H_
#include "gxstate.h"
#endif


#ifdef _AFXDLL
// if you copy this file to your project, use the first line
// #include "resource.h"    // only needed for GX_IDC_HSPLITBAR cursor
// #include "gxresrc.h"        // only needed for GX_IDC_HSPLITBAR cursor
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXTWND")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CGXTabWnd, CWnd);

// enable use as custom control
IMPLEMENT_REGISTER(CGXTabWnd, CS_DBLCLKS | GX_GLOBALCLASS, 0, 0, 0);

// much of this code (e.g. CreateCommon, RecalcLayout, ...)
// is copied from the CSplitterWnd class and adapted to the tabwnd class

/////////////////////////////////////////////////////////////////////////////
// Win4 specific defines

#define new DEBUG_NEW

#define CX_BORDER           1
#define CY_BORDER           1
#define IDW_TABBEAM         AFX_IDW_HSCROLL_FIRST+1
#define _GXGetDlgCtrlID(hWnd)  ((UINT)(WORD)::GetDlgCtrlID(hWnd))

/////////////////////////////////////////////////////////////////////////////
// CGXTabWnd support for dialog validation

void AFXAPI DDV_GXTabWnd(CDataExchange* pDX, CGXTabWnd* pTabWnd)
{
	if (pDX->m_bSaveAndValidate)
	{
		pDX->PrepareCtrl(pTabWnd->GetDlgCtrlID());

		// if deactivation is possible, OnCanActivate returns 0;
		// otherwise it returns 0
		if (pTabWnd->m_pActivePane->SendMessage(WM_GX_CANACTIVATE, 0, 0))
			pDX->Fail();
	}
}

CGXTabWnd* GXGetParentTabWnd(const CWnd* pWnd, BOOL bAnyState)
{
	return (CGXTabWnd*) GXGetParentWnd(pWnd, RUNTIME_CLASS(CGXTabWnd), bAnyState);
}

/////////////////////////////////////////////////////////////////////////////
// CGXTabWnd

class CGXTabWndInterface: public CGXAbstractTabWndInterface
{
public:
	virtual void UpdateTabName(const CWnd* pWnd, CString& sName, BOOL bUpdateBeam);
	virtual CString GetTabName(const CWnd* pWnd, int nTab);
};

void CGXTabWndInterface::UpdateTabName(const CWnd* pWnd, CString& sName, BOOL bUpdateBeam)
{
	CGXTabWnd* pTabWnd = GXGetParentTabWnd(pWnd, TRUE);

	if (pTabWnd != NULL)
	{
		if (bUpdateBeam)
			pTabWnd->SetTabName(pWnd, sName, FALSE);
		else
			sName = pTabWnd->GetTabName(pWnd);
	}
}

CString CGXTabWndInterface::GetTabName(const CWnd* pWnd, int nTab)
{
	CGXTabWnd* pTabWnd = GXGetParentTabWnd(pWnd, TRUE);

	CString s;
	if (pTabWnd != NULL)
		s = pTabWnd->GetBeam().GetTab(nTab).sLabel;

	return s;
}

CGXTabWnd::CGXTabWnd(CRuntimeClass *pBeamClass)
{
#if _MFC_VER >= 0x0300 && defined(_DEBUG)
	ASSERT( pBeamClass && pBeamClass->IsDerivedFrom(RUNTIME_CLASS(CGXTabBeam)) );
#endif

	m_pBeamClass = pBeamClass;
	m_bCtrlsCreated = FALSE;
	m_pBeam = 0;
	m_pActivePane = 0;
	m_cxBeam = 0;
	m_lxBeam = 0;
	m_cxWnd = 0;
	m_cxSplitter = 6;
	m_cxVScroll = GetSystemMetrics(SM_CXVSCROLL);
	m_cyHScroll = GetSystemMetrics(SM_CYHSCROLL);
	m_nBarWidths = m_cyHScroll + 2*GXGetAppData()->bWin4;
	m_bTracking = FALSE;
	m_hcurArrow  = ::LoadCursor(NULL, IDC_ARROW);
	m_pEditWnd = NULL;
	m_bHasHScroll = 1;
	m_bHasVScroll = 1;

	m_brErase.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));

	m_bDrawSunken = FALSE;

	// Register in grid for sharing scrollbars
	GXGetAppData()->RegisterSharedScrollBar(GetRuntimeClass());

	// Register interface for CGXGridCore
	if (GXGetAppData()->m_pTabWndInt == NULL)
		GXGetAppData()->m_pTabWndInt = new CGXTabWndInterface;
}

CGXTabWnd::~CGXTabWnd()
{
	delete m_pBeam;
}

CGXTabBeam& CGXTabWnd::GetBeam()
{ 
	return *m_pBeam; 
}

CWnd* CGXTabWnd::GetActivePane()
{ 
	return m_pActivePane; 
}

void CGXTabWnd::SwitchTab(int nTab)
{
	ASSERT(nTab >= 0 && nTab <= m_pBeam->GetCount());
	SwitchTab((CWnd*) m_pBeam->GetTab(nTab).pExtra);
}

void CGXTabWnd::RemoveTab(int nTab)
{
	ASSERT(nTab >= 0 && nTab <= m_pBeam->GetCount());
	RemoveTab((CWnd*) m_pBeam->GetTab(nTab).pExtra);
}

// repositions client area of specified window
// assumes everything has WS_BORDER or is inset like it does
//  (includes scroll bars)
static HDWP NEAR PASCAL DeferClientPos(HDWP hDWP, CWnd* pWnd,
		int x, int y, int cx, int cy, BOOL bScrollBar)
{
	ASSERT(pWnd != NULL);
	ASSERT(pWnd->m_hWnd != NULL);

	if (bScrollBar)
	{
		DWORD dwStyle = pWnd->GetStyle();

		// if there is enough room, draw scroll bar without border
		// if there is not enough room, set the WS_BORDER bit so that
		//   we will at least get a proper border drawn
		BOOL bNeedBorder = (cx <= CX_BORDER || cy <= CY_BORDER);
		if (bNeedBorder)
			dwStyle |= WS_BORDER;
		else
			dwStyle &= ~WS_BORDER;
		::SetWindowLong(pWnd->m_hWnd, GWL_STYLE, dwStyle);
	}

	// adjust for border size (even if zero client size)
	x -= CX_BORDER;
	y -= CY_BORDER;
	cx += 2 * CX_BORDER;
	cy += 2 * CY_BORDER;

	// first check if the new rectangle is the same as the current
	CRect rectOld;
	pWnd->GetWindowRect(rectOld);
	pWnd->GetParent()->ScreenToClient(&rectOld);
	if (rectOld.left == x && rectOld.top == y &&
		rectOld.Width() == cx && rectOld.Height() == cy)
	{
		return hDWP;        // nothing to do
	}

	return ::DeferWindowPos(hDWP, pWnd->m_hWnd, NULL,
		x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
}

BOOL CGXTabWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPCHILDREN;
	return CWnd::PreCreateWindow(cs);
}

BOOL CGXTabWnd::Create(CWnd* pParentWnd,
		LPCTSTR szFirstLabel,
		CCreateContext* pContext,
		DWORD dwStyle /* ... | SPLS_DYNAMIC_SPLIT */,
		UINT nID /* = AFX_IDW_PANE_FIRST */)
{
	WNDCLASS wcls;
	if (::GetClassInfo(AfxGetInstanceHandle(), _T("GXWND"), &wcls) == 0)
	{
		ASSERT(0);
		// ASSERTION-> Did you call GXInit() in the InitInstance method
		// of your application object? ->END
		AfxThrowNotSupportedException();
	}

	ASSERT(pParentWnd != NULL);

	ASSERT(pContext != NULL);
	ASSERT(pContext->m_pNewViewClass != NULL);
	ASSERT(dwStyle & WS_CHILD);

	if (!CreateCommon(pParentWnd, dwStyle | WS_VISIBLE, nID))
		return FALSE;

	CRect rect;
	pParentWnd->GetClientRect(&rect);
	MoveWindow(0, 0, rect.Width(), rect.Height(), FALSE);

	m_pDynamicViewClass = pContext->m_pNewViewClass;
		// save for later dynamic creations

	// add the first initial pane
	CWnd* pWnd = CreateView(m_pDynamicViewClass, szFirstLabel, pContext, nID);
	if (pWnd == NULL)
	{
		DestroyWindow(); // will clean up child windows
		return FALSE;
	}

	m_bCtrlsCreated = TRUE;

	m_nFirstViewID = nID;
	m_nLastViewID = nID;
	m_pActivePane = pWnd;

	return TRUE;
}

#if _MFC_VER < 0x0300
WNDPROC* CGXTabWnd::GetSuperWndProcAddr()
{
	// Note: it is no longer necessary to override GetSuperWndProcAddr
	//  for each control class with a different WNDCLASS.
	//  This implementation now uses instance data, such that the previous
	//  WNDPROC can be anything.

	static WNDPROC NEAR pfnSuper;
	return &pfnSuper;
}
#endif

#ifndef SIF_ALL
#define SIF_ALL 0x1F
#endif

void CGXTabWnd::SaveScrollbarState(CWnd* pSheetWnd)
{
	// Save scrollbar state and store in CGXTabInfo
	CGXTabInfo* pInfo = &m_pBeam->GetTab(m_pBeam->FindTab(pSheetWnd));
#if _MFC_VER >= 0x0300
	pInfo->sbInfoVert.fMask = SIF_ALL;
	pInfo->sbInfoHorz.fMask = SIF_ALL;
	
	if (!pSheetWnd->GetScrollInfo(SB_VERT, &pInfo->sbInfoVert))
		pSheetWnd->GetScrollRange(SB_VERT, &pInfo->sbInfoVert. nMin, &pInfo->sbInfoVert.nMax);

	if (!pSheetWnd->GetScrollInfo(SB_HORZ, &pInfo->sbInfoHorz))
		pSheetWnd->GetScrollRange(SB_HORZ, &pInfo->sbInfoHorz. nMin, &pInfo->sbInfoHorz.nMax);
#else	
	pSheetWnd->GetScrollRange(SB_VERT, &pInfo->sbInfoVert. nMin, &pInfo->sbInfoVert.nMax);
	pSheetWnd->GetScrollRange(SB_HORZ, &pInfo->sbInfoHorz.nMin, &pInfo->sbInfoHorz.nMax);
#endif

	pInfo->sbInfoVert.nTrackPos = pSheetWnd->GetScrollPos(SB_VERT);
	pInfo->sbInfoHorz.nTrackPos = pSheetWnd->GetScrollPos(SB_HORZ);
}

void CGXTabWnd::RestoreScrollbarState(CWnd* pSheetWnd)
{
	// Restore scrollbar state from CGXTabInfo
	CGXTabInfo* pInfo = &m_pBeam->GetTab(m_pBeam->FindTab(pSheetWnd));
	CWnd* pHScrollBar = GetDlgItem(AFX_IDW_HSCROLL_FIRST);
	CWnd* pVScrollBar = GetDlgItem(AFX_IDW_VSCROLL_FIRST);

	if (pInfo->sbInfoVert.nMax)
	{
#if _MFC_VER >= 0x0300
		if (pVScrollBar)
			pVScrollBar->EnableScrollBar(TRUE);

		pInfo->sbInfoVert.fMask = SIF_ALL | SIF_DISABLENOSCROLL;

		if (!pSheetWnd->SetScrollInfo(SB_VERT, &pInfo->sbInfoVert))
#endif
			pSheetWnd->SetScrollRange(SB_VERT, pInfo->sbInfoVert.nMin, pInfo->sbInfoVert.nMax);
		pSheetWnd->SetScrollPos(SB_VERT, pInfo->sbInfoVert.nTrackPos);
	}
	else
		if (pVScrollBar)
			pVScrollBar->EnableScrollBar(FALSE);

	if (pInfo->sbInfoHorz.nMax)
	{
#if _MFC_VER >= 0x0300
		if (pHScrollBar)
			pHScrollBar->EnableScrollBar(TRUE);

		pInfo->sbInfoHorz.fMask = SIF_ALL | SIF_DISABLENOSCROLL;

		if (!pSheetWnd->SetScrollInfo(SB_HORZ, &pInfo->sbInfoHorz))
#endif
			pSheetWnd->SetScrollRange(SB_HORZ, pInfo->sbInfoHorz.nMin, pInfo->sbInfoHorz.nMax);
		pSheetWnd->SetScrollPos(SB_HORZ, pInfo->sbInfoHorz.nTrackPos);
	}
	else
		if (pHScrollBar)
			pHScrollBar->EnableScrollBar(FALSE);
}

BOOL CGXTabWnd::CreateCommon(CWnd* pParentWnd,
	DWORD dwStyle, UINT nID)
{
	ASSERT(pParentWnd != NULL);
	ASSERT(dwStyle & WS_CHILD);
	ASSERT(nID != 0);

	// the Windows scroll bar styles bits turn no the smart scrollbars
	m_bHasHScroll = (dwStyle & WS_HSCROLL) != 0;
	m_bHasVScroll = (dwStyle & WS_VSCROLL) != 0;
	dwStyle &= ~(WS_HSCROLL | WS_VSCROLL);

	// create with the same wnd-class as MDI-Frame (no erase bkgnd)
	if (m_hWnd == 0)
	{
		if (!CreateEx(0, _T("GxMdiFrame"), NULL, dwStyle, 0, 0, 0, 0,
			pParentWnd->m_hWnd, (HMENU)nID, NULL))
		{
			ASSERT(0);
			// ASSERTION-> Window could not be created.
			// Did you call GXInit() in you InitInstance method? ->END

			return FALSE;       // create invisible
		}
	}

	// attach the initial scrollbar parts
	TRY
	{
		// Scrollbars are always created even if m_bHasHScroll or 
		// m_bHasVScroll is FALSE.
		// If any of these flags is FALSE, the associated scrollbar
		// will be hidden.

		if (!CreateScrollBarCtrl(SBS_HORZ, AFX_IDW_HSCROLL_FIRST))
				AfxThrowResourceException();

		if (!m_bHasHScroll)
		{
			CWnd* pScrollBar = GetDlgItem(AFX_IDW_HSCROLL_FIRST);
			pScrollBar->ShowWindow(SW_HIDE);
		}

		if (!CreateScrollBarCtrl(SBS_VERT, AFX_IDW_VSCROLL_FIRST))
				AfxThrowResourceException();

		if (!m_bHasVScroll)
		{
			CWnd* pScrollBar = GetDlgItem(AFX_IDW_VSCROLL_FIRST);
			pScrollBar->ShowWindow(SW_HIDE);
		}

#if _MFC_VER >= 0x0300
		if (GXGetAppData()->bWin4 && (m_bHasHScroll || m_bHasVScroll) && GetParent()->IsFrameWnd())
			if (!CreateScrollBarCtrl(SBS_SIZEGRIP, AFX_IDW_SIZE_BOX))
					AfxThrowResourceException();
#endif

		if (m_pBeam == NULL)
		{
			m_pBeam = (CGXTabBeam*) m_pBeamClass->CreateObject();
			VERIFY(m_pBeam->Create(WS_VISIBLE, CRect(0, 0, 1, 1), this, IDW_TABBEAM));
		}

	}
	CATCH_ALL(e)
	{
		DestroyWindow(); // will clean up child windows
		return FALSE;
	}
	END_CATCH_ALL

	m_bCtrlsCreated = TRUE;

	return TRUE;
}

BOOL CGXTabWnd::CreateScrollBarCtrl(DWORD dwStyle, UINT nID)
{
	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);

	return (::CreateWindow(_T("SCROLLBAR"), NULL, dwStyle | WS_VISIBLE | WS_CHILD,
		0, 0, 1, 1, m_hWnd, (HMENU)nID,
		AfxGetInstanceHandle(), NULL) != NULL);
}

#if _MFC_VER >= 0x0300

static CWnd* GetSizingParent(CWnd* pWnd)
{
	ASSERT_VALID(pWnd);

	if (!GXGetAppData()->bWin4)
		return NULL;

	// size box is in lower right corner of this window
	CRect rectClient;
	pWnd->GetClientRect(rectClient);

	// find sizeable parent window
	CWnd* pParent = pWnd;
	if (!(pParent->GetStyle() & WS_THICKFRAME))
		pParent = pWnd->GetParent();

	// only allow if not maximized and has thick frame
	ASSERT_VALID(pParent);
	if ((pParent->GetStyle() & (WS_THICKFRAME|WS_MAXIMIZE)) == WS_THICKFRAME)
	{
		// convert client area of frame window relative to splitter
		CRect rect;
		pParent->GetClientRect(rect);
		pParent->ClientToScreen(rect);
		pWnd->ScreenToClient(rect);

		// must match exactly to get the size box
		if (rectClient.BottomRight() == rect.BottomRight())
			return pParent;
	}

	return NULL;    // no sizeable parent found
}

#endif

void CGXTabWnd::RecalcLayout(BOOL bTracking)
{
	ASSERT_VALID(this);

	if (!m_bCtrlsCreated)
	{
		// TRACE0("RecalcLayout() is creating controls\n");
		VERIFY(CreateCommon(GetParent(), WS_CHILD | WS_HSCROLL | WS_VSCROLL, _GXGetDlgCtrlID(m_hWnd)));
		m_nLastViewID = m_nFirstViewID = AFX_IDW_PANE_FIRST;
		m_bCtrlsCreated = TRUE;
	}

	CRect rectInside;
	GetInsideRect(rectInside);

	CRect rectClient;
	GetClientRect(rectClient);

	// adjust the panes and scroll bars

	// is beam size already initialized?
	if (m_lxBeam == 0)
	{
		m_lxBeam = rectClient.right/2*100L;
		m_cxBeam = (int) (m_lxBeam/100);
		m_cxWnd = rectClient.right;
	}

	// give the hint for the maximum number of HWNDs
	HDWP hDWP = ::BeginDeferWindowPos(5);

	// size of scrollbars
	int cx = (rectClient.right - rectInside.right) - 2*GXGetAppData()->bWin4;
	int cy = (rectClient.bottom - rectInside.bottom) - 2*GXGetAppData()->bWin4;

	{
		int ySB = rectInside.bottom + CY_BORDER;
		CWnd* pScrollBar = GetDlgItem(AFX_IDW_HSCROLL_FIRST);
		ASSERT(pScrollBar != NULL);
		if (m_bHasHScroll)
		{
			int cx = rectInside.right - m_cxBeam - m_cxSplitter;
			hDWP = DeferClientPos(hDWP, pScrollBar, m_cxBeam + m_cxSplitter, ySB,
				cx, cy, TRUE);
		}
		else
			hDWP = DeferClientPos(hDWP, pScrollBar, rectClient.right, ySB,
				0, cy, TRUE);
	}

	// reposition scroll bars
	if (!bTracking && m_bHasVScroll)
	{
		int xSB = rectInside.right + CX_BORDER;
		CWnd* pScrollBar = GetDlgItem(AFX_IDW_VSCROLL_FIRST);
		ASSERT(pScrollBar != NULL);
		int cy = rectInside.bottom;
		hDWP = DeferClientPos(hDWP, pScrollBar, xSB, 0,
			cx, cy, TRUE);
	}

#if _MFC_VER >= 0x0300
	// reposition size box
	if (GXGetAppData()->bWin4 && m_bHasHScroll && m_bHasVScroll && GetParent()->IsFrameWnd())
	{
		int xSB = rectInside.right + 2*CX_BORDER;
		int ySB = rectInside.bottom + 2*CY_BORDER;

		CWnd* pScrollBar = GetDlgItem(AFX_IDW_SIZE_BOX);
		ASSERT(pScrollBar != NULL);

		// fix style if necessary
		BOOL bSizingParent = (GetSizingParent(this) != NULL);
		// modifyStyle returns TRUE if style changes
		if (pScrollBar->ModifyStyle(SBS_SIZEGRIP|SBS_SIZEBOX,
				bSizingParent ? SBS_SIZEGRIP : SBS_SIZEBOX))
			pScrollBar->Invalidate();
		pScrollBar->EnableWindow(bSizingParent);

		// reposition the size box
		hDWP = DeferClientPos(hDWP, pScrollBar,
			xSB, ySB, cx, cy, TRUE);
	}
#endif

	// repositon tab beam
	{
		int ySB = rectInside.bottom + CY_BORDER;
		CWnd* pBeam = GetDlgItem(IDW_TABBEAM);
		ASSERT(pBeam != NULL);

		int cx = m_bHasHScroll?m_cxBeam:rectClient.right;
		hDWP = DeferClientPos(hDWP, pBeam, 0, ySB,
			cx, cy, FALSE);
	}


	if (!bTracking)
	{
		int x = 1;
		int y = 1;
		int cy = rectInside.Height() - CY_BORDER - 1;
		int cx = rectInside.Width() - CX_BORDER - 1;
		CWnd* pPane = m_pActivePane;
		if (pPane)
		{
			hDWP = DeferClientPos(hDWP, pPane, x, y, cx, cy, FALSE);

			// send a WM_SIZE message to the active pane, so that
			// scrollbars will be displayed correctly
			pPane->SendMessage(WM_SIZE, SIZE_MAXIMIZED, MAKELONG(cx-x, cy-y));
		}
	}

	::EndDeferWindowPos(hDWP);
}

void CGXTabWnd::ShowScrollBar(UINT nBar, BOOL bShow)
{
	BOOL bRecalc = FALSE;
	CWnd* pScrollBar;

	// Vertical Scrollbar
	if (nBar == SB_BOTH || nBar == SB_VERT)
	{
		if ((pScrollBar = GetDlgItem(AFX_IDW_VSCROLL_FIRST)) != NULL)
		{
			if (!bShow && m_bHasVScroll)
			{
				pScrollBar->ShowWindow(SW_HIDE);
				pScrollBar->MoveWindow0(FALSE);
				m_bHasVScroll = FALSE;
				bRecalc = TRUE;
			}
			else if (bShow && !m_bHasVScroll)
			{
				pScrollBar->ShowWindow(SW_SHOW);
				m_bHasVScroll = TRUE;
				bRecalc = TRUE;
			}
		}
	}

	// Horizontal Scrollbar
	if (nBar == SB_BOTH || nBar == SB_HORZ)
	{
		if ((pScrollBar = GetDlgItem(AFX_IDW_HSCROLL_FIRST)) != NULL)
		{
			if (!bShow && m_bHasHScroll)
			{
				pScrollBar->ShowWindow(SW_HIDE);
				pScrollBar->MoveWindow0(FALSE);
				m_bHasHScroll = FALSE;
				bRecalc = TRUE;
			}
			else if (bShow && !m_bHasHScroll)
			{
				pScrollBar->ShowWindow(SW_SHOW);
				m_bHasHScroll = TRUE;
				bRecalc = TRUE;
			}
		}
	}

#if _MFC_VER >= 0x0300
	// Size Box
	if (GXGetAppData()->bWin4 && (pScrollBar = GetDlgItem(AFX_IDW_SIZE_BOX)) != NULL && GetParent()->IsFrameWnd())
	{
		if (!m_bHasHScroll || !m_bHasVScroll)
		{
			pScrollBar->ShowWindow(SW_HIDE);
			pScrollBar->MoveWindow0(FALSE);
		}
		else
		{
			pScrollBar->ShowWindow(SW_SHOW);
		}
	}
#endif

	if (bRecalc)
		RecalcLayout();
}

// like GetClientRect but inset by shared scrollbars
void CGXTabWnd::GetInsideRect(CRect& rect) const
{
	ASSERT_VALID(this);

	GetClientRect(rect);
	ASSERT(rect.left == 0 && rect.top == 0);

	// subtract scrollbar clearance

	rect.bottom -= m_nBarWidths - CX_BORDER - GXGetAppData()->bWin4;

	// note:  vertical's width is same as beam's height
	if (m_bHasVScroll)
		rect.right -= m_nBarWidths - CY_BORDER - GXGetAppData()->bWin4;
}

CWnd* CGXTabWnd::CreateView(CRuntimeClass* pViewClass, LPCTSTR szLabel, CCreateContext* pContext, UINT nID)
{
	BOOL bSendInitialUpdate = FALSE;

	if (nID == 0)
		nID = ++m_nLastViewID;

	CCreateContext contextT;
	if (pContext == NULL)
	{
		// if no context specified, generate one from the currently selected
		//  client if possible
		CView* pOldView = GetParentFrame()->GetActiveView();
		if (pOldView != NULL)
		{
			// set info about last pane
			ASSERT(contextT.m_pCurrentFrame == NULL);
			contextT.m_pLastView = pOldView;
			contextT.m_pCurrentDoc = pOldView->GetDocument();
			if (contextT.m_pCurrentDoc != NULL)
				contextT.m_pNewDocTemplate =
				  contextT.m_pCurrentDoc->GetDocTemplate();
		}
		pContext = &contextT;
		bSendInitialUpdate = TRUE;
	}

	CWnd* pWnd = NULL;
	TRY
	{
		pWnd = (CWnd*)pViewClass->CreateObject();
		if (pWnd == NULL)
			AfxThrowMemoryException();
	}
	CATCH_ALL(e)
	{
		TRACE0("Out of memory creating a splitter pane\n");
		return pWnd;
	}
	END_CATCH_ALL

	ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CWnd)));
	ASSERT(pWnd->m_hWnd == NULL);       // not yet created

	// Create with the right size (wrong position)
	CRect rect;
	GetInsideRect(rect);

	DWORD dwStyle = AFX_WS_DEFAULT_VIEW & ~WS_BORDER;

	if (m_pBeam->m_nTabs > 0)
		dwStyle &= ~WS_VISIBLE;

	if (!pWnd->Create(NULL, NULL, dwStyle,
		rect, this, nID, pContext))
	{
		TRACE0("Warning: couldn't create client pane for splitter\n");
			// pWnd will be cleaned up by PostNcDestroy
		return NULL;
	}

	// send initial notification message
	HMENU hMenu = 0;
	if (pContext->m_pNewDocTemplate && pContext->m_pNewDocTemplate->IsKindOf(RUNTIME_CLASS(CMultiDocTemplate)))
		hMenu = ((CMultiDocTemplate*) pContext->m_pNewDocTemplate)->m_hMenuShared;

	m_pBeam->InsertTab(FALSE, szLabel, pWnd, FALSE, -1, hMenu);

	if (bSendInitialUpdate)
		pWnd->SendMessage(WM_INITIALUPDATE);

	// force a WM_SIZE
	pWnd->GetClientRect(rect);
	pWnd->MoveWindow(rect);

	SaveScrollbarState(pWnd);

	return pWnd;
}

UINT CGXTabWnd::GetNextID()
{
	return ++m_nLastViewID;
}

CWnd* CGXTabWnd::AttachWnd(CWnd* pWnd, LPCTSTR szLabel)
{
	ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CWnd)));
	ASSERT(pWnd->m_hWnd != NULL);       // must be created

	if (!m_bCtrlsCreated)
		RecalcLayout();

	// Create with the right size (wrong position)
	CRect rect;
	GetInsideRect(rect);
	pWnd->MoveWindow(rect);

	m_pBeam->InsertTab(FALSE, szLabel, pWnd, FALSE);

	pWnd->ShowScrollBar(SB_BOTH, FALSE);

	if (m_pActivePane == NULL)
	{
		m_pActivePane = pWnd;
		pWnd->ShowWindow(SW_SHOW);
		pWnd->SetActiveWindow();
	}

	pWnd->SendMessage(WM_GX_INITTAB, m_pBeam->FindTab(pWnd));

	SaveScrollbarState(pWnd);

	return pWnd;
}


BEGIN_MESSAGE_MAP(CGXTabWnd, CWnd)
	//{{AFX_MSG_MAP(CGXTabWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_MESSAGE(WM_GX_TABSWITCH, OnTabSwitch)
	ON_MESSAGE(WM_GX_CANTABSWITCH, OnCanTabSwitch)
	ON_MESSAGE(WM_GX_TABDBLCLK, OnTabDblClk)
	ON_MESSAGE(WM_GX_EDITCANCEL, OnEditCancel)
	ON_MESSAGE(WM_GX_EDITEND, OnEditEnd)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_SETFOCUS()
	ON_WM_ERASEBKGND()
	ON_WM_NCPAINT()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_WININICHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGXTabWnd operations

void CGXTabWnd::SwitchTab(CWnd* pWnd)
{
	int nNewTab = m_pBeam->FindTab(pWnd);

	// save scrollbar state and hide window
	if (m_pActivePane)
	{
		// check first if switching to the other sheet is possible
		if (OnCanTabSwitch((WPARAM) nNewTab, 0) == 1)
			return;

		SaveScrollbarState(m_pActivePane);
		m_pActivePane->ShowWindow(SW_HIDE);
	}

	// Update status beam
	m_pBeam->ScrollTabInView(nNewTab);
	if (m_pBeam->m_nCurrent != nNewTab)
	{
		m_pBeam->m_nCurrent = nNewTab;
		m_pBeam->Invalidate(FALSE);
	}
	m_pBeam->UpdateWindow();

	// Select new pane
	m_pActivePane = pWnd;
	pWnd->SetActiveWindow();
	m_pActivePane->ShowWindow(SW_SHOW);
	m_pActivePane->SetFocus();

	// Update menu bar
	if (pWnd->IsKindOf(RUNTIME_CLASS(CView)))
	{
		GetParentFrame()->SetActiveView((CView*) pWnd);
		((CFrameWnd*) AfxGetMainWnd())->DelayUpdateFrameMenu(NULL);
	}

	// Restore scrollbar state
	RestoreScrollbarState(pWnd);

	// force a WM_SIZE message
	CRect rect;
	m_pActivePane->GetClientRect(rect);
	m_pActivePane->Invalidate();
	m_pActivePane->MoveWindow(rect);

	RecalcLayout();
}

void CGXTabWnd::RemoveTab(CWnd* pWnd)
{
	// verify parameters
	CGXTabBeam* pBeam = &GetBeam();
	int nTabCount = pBeam->GetCount();
	int nTabToDelete = m_pBeam->FindTab(pWnd);

	if (!(nTabCount > 1 && nTabToDelete < nTabCount))
	{
		TRACE0("Warning: invalid call to CGXTabWnd::RemoveTab\n");
		return;
	}

	int nCurSel = min(pBeam->GetCurSel(), pBeam->GetCount()-2);
	int nFirstTab = min(pBeam->m_nFirstTab, nCurSel);

	// destroy window
	pWnd->DestroyWindow();
	pBeam->DeleteTab(nTabToDelete);

	// switch to (or reactivate) current pane
	m_pActivePane = NULL;
	pBeam->m_nFirstTab = nFirstTab;
	SwitchTab(nCurSel);
}

LPCTSTR CGXTabWnd::GetTabName(int nTab)
{
	ASSERT(nTab >= 0 && nTab < GetBeam().GetCount());
	return GetBeam().GetTab(nTab).sLabel;
}

LPCTSTR CGXTabWnd::GetTabName(const CWnd* pSheet)
{
	int nTab = GetBeam().FindTab(pSheet);
	if (nTab != -1)
		return GetBeam().GetTab(nTab).sLabel;

	return NULL;
}

void CGXTabWnd::SetTabName(int nTab, LPCTSTR s, BOOL bNotifySheet)
{
	ASSERT(nTab >= 0 && nTab < GetBeam().GetCount());
	GetBeam().GetTab(nTab).sLabel = s;
	GetBeam().AdjustTabSize(nTab);
	GetBeam().Invalidate();
	if (bNotifySheet)
		SendMessage(WM_GX_CHANGEDTAB, nTab);
}

void CGXTabWnd::SetTabName(const CWnd* pSheet, LPCTSTR s, BOOL bNotifySheet)
{
	int nTab = GetBeam().FindTab(pSheet);
	ASSERT(nTab != -1);
	if (nTab != -1)
		SetTabName(nTab, s, bNotifySheet);
}

/////////////////////////////////////////////////////////////////////////////
// CGXTabWnd message handlers

LRESULT CGXTabWnd::OnTabSwitch(WPARAM wParam, LPARAM )
{
	// TRACE(_T("OnTabSwitch(%d)\n"), wParam);
	CGXTabInfo& pInfo = m_pBeam->GetTab(wParam);

	if ((CWnd*) pInfo.pExtra == m_pActivePane)
	{
		MSG msg;
		// Is a double-click pending?
		if (PeekMessage(&msg, m_hWnd, WM_GX_TABDBLCLK, WM_GX_TABDBLCLK, PM_NOREMOVE))
			return 0;

		if (m_pActivePane->IsKindOf(RUNTIME_CLASS(CView)))
			GetParentFrame()->SetActiveView((CView*) m_pActivePane);
		m_pActivePane->SetFocus();
		m_pActivePane->Invalidate();
	}
	else
		SwitchTab((CWnd*) pInfo.pExtra);

	return 0;
}

LRESULT CGXTabWnd::OnCanTabSwitch(WPARAM wParam, LPARAM )
{
	// TRACE(_T("OnTabSwitch(%d)\n"), wParam);
	CGXTabInfo& pNewInfo = m_pBeam->GetTab(wParam);
	CWnd* pNewWnd = (CWnd*) pNewInfo.pExtra;

	if (pNewWnd == m_pActivePane)
		return 0;   // 0 means that switching to current pane is allowed
	else
	{
		// switch to different pane

		// 1st, check if I can deactive current pane
		// Next, check if I can active new pane

		// OnCanActivate returns 1, if activation is disabled
		if (m_pActivePane->SendMessage(WM_GX_CANACTIVATE, 0, 0)
			|| pNewWnd->SendMessage(WM_GX_CANACTIVATE, 1, 0))
		{
			m_pActivePane->SetFocus();
			return 1;
		}
	}

	return 0;   // switching is allowed

	// Please note:
	// switching is allowed when return value is 0
	// switching is disabled when return value is 1
}

LRESULT CGXTabWnd::OnTabDblClk(WPARAM wParam, LPARAM )
{
	// TRACE(_T("OnTabDlbClk(%d)\n"), wParam);
	CGXTabInfo& pInfo = m_pBeam->GetTab(wParam);
	CWnd* pWnd = (CWnd*) pInfo.pExtra;

	if (m_pEditWnd)
		return 0;

	if (pWnd->SendMessage(WM_GX_NEEDCHANGETAB, wParam, 0) == FALSE)
		return 0;

	CString s = pInfo.sLabel;

	int x = m_pBeam->CalcTabOffset(wParam);
	CRect rectInside;
	GetInsideRect(rectInside);

	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(m_pBeam->m_pFontNormal);
	CSize size = dc.GetTextExtent(_T("W"), 1);
	dc.SelectObject(pOldFont);
	size.cx = size.cx*15;

	int nBorder = GetSystemMetrics(SM_CYDLGFRAME);

	CRect rect;
	rect.left = x;
	rect.top = max(rectInside.bottom-2*size.cy-2*nBorder, 0);
	rect.right = x+size.cx,
	rect.bottom = rect.top+size.cy+4*nBorder;

	if (rect.right > rectInside.right)
	{
		rect.left = max(0, rectInside.right - size.cx);
		rect.right = rectInside.right;
	}

	if (rect.left < 0)
	{
		rect.right = min(size.cx, rectInside.right);
		rect.left = 0;
	}

	UpdateWindow();

	m_pEditWnd = new CGXEditLabelPopup(rect, this, ID_GX_EDITWND);
	m_pEditWnd->SetWindowText(s);
	m_pEditWnd->SetFont(m_pBeam->m_pFontNormal);
	m_pEditWnd->SetFocus();

	return 0;
}

LRESULT CGXTabWnd::OnEditEnd(WPARAM , LPARAM )
{
	CString s;

	m_pEditWnd->GetWindowText(s);
	m_pEditWnd->DestroyWindow();
	delete m_pEditWnd;
	m_pEditWnd = NULL;

	m_pBeam->GetTab(m_pBeam->m_nCurrent).sLabel = s;
	m_pBeam->AdjustTabSize(m_pBeam->m_nCurrent);

	CGXTabInfo& pInfo = m_pBeam->GetTab(m_pBeam->m_nCurrent);
	CWnd* pWnd = (CWnd*) pInfo.pExtra;

	pWnd->SendMessage(WM_GX_CHANGEDTAB, m_pBeam->m_nCurrent, 0);

	return 0;
}

LRESULT CGXTabWnd::OnEditCancel(WPARAM , LPARAM )
{
	if (m_pEditWnd)
	{
		m_pEditWnd->DestroyWindow();
		delete m_pEditWnd;
		m_pEditWnd = NULL;
	}

	return 0;
}

void CGXTabWnd::OnPaint()
{
	ASSERT_VALID(this);

	if (!m_bCtrlsCreated)
		RecalcLayout();

	CPaintDC dc(this);

	CRect rectClient;
	GetClientRect(&rectClient);
	CRect rectInside;
	GetInsideRect(rectInside);

	CRect rect;
	rect.top = 0;

	// draw the splitter boxes
	if (m_bHasHScroll)
	{
		CRect rect(m_cxBeam + CX_BORDER,
			rectInside.bottom,
			m_cxBeam+m_cxSplitter,
			rectClient.bottom);

		OnDrawSplitter(&dc, rect);
	}

	// Do not call CWnd::OnPaint() for painting messages
}

void CGXTabWnd::OnSize(UINT nType, int cx, int cy)
{
	// ensure that edit popup is not visible
	OnEditCancel(0, 0);

	CWnd* pPane = m_pActivePane;
	if (pPane)
		pPane->SendMessage(WM_GX_LOCKRESIZE, GX_STARTRESIZE, 0);

	if (nType != SIZE_MINIMIZED && cx > 0 && cy > 0 && cx < 32767 && cy < 32767)
	{
		// Adapt m_lxBeam, if m_cxBeam has been changed
		if ((int) (m_lxBeam/100) != m_cxBeam)
			m_lxBeam = (LONG) m_cxBeam*100;

		if (m_pBeam && m_cxWnd != 0 && m_lxBeam != 0)
		{
			LONG x = (m_pBeam->m_nMargin-20)*100L;
			m_lxBeam = (m_lxBeam-x)*(cx)/(m_cxWnd)+x;
		}
		else if (m_lxBeam == 0)
			m_lxBeam = cx/2*100L;

		m_cxBeam = (int) (m_lxBeam/100);
	
		m_cxWnd = cx;

		RecalcLayout();
	}

	CWnd::OnSize(nType, cx, cy);

	UpdateWindow();

	if (pPane)
	{
		pPane->SendMessage(WM_GX_LOCKRESIZE, GX_ENDRESIZE, 0);
		CRect r;
		m_pActivePane->GetClientRect(r);
		pPane->SendMessage(WM_SIZE, nType, MAKELONG(r.Width(), r.Height()));
	}
}

void CGXTabWnd::OnVScroll(UINT /*nSBCode*/, UINT /*nPos*/, CScrollBar* /*pScrollBar*/)
{
	// ensure that edit popup is not visible
	OnEditCancel(0, 0);

	// pass message to active pane
	MSG msg = *GetCurrentMessage();
	if (m_pActivePane)
		m_pActivePane->SendMessage(msg.message, msg.wParam, msg.lParam);
}

void CGXTabWnd::OnHScroll(UINT /*nSBCode*/, UINT /*nPos*/, CScrollBar* /*pScrollBar*/)
{
	// ensure that edit popup is not visible
	OnEditCancel(0, 0);

	// pass message to active pane
	MSG msg = *GetCurrentMessage();
	if (m_pActivePane)
		m_pActivePane->SendMessage(msg.message, msg.wParam, msg.lParam);
}

void CGXTabWnd::OnDrawSplitter(CDC* pDC, const CRect& rect)
	// if pDC == NULL then just invalidate
{
	if (pDC == NULL)
	{
		InvalidateRect(rect);
		return;
	}
	ASSERT_VALID(pDC);

	COLORREF rgbFace    = GetSysColor(COLOR_BTNFACE);
	COLORREF rgbShadow  = GetSysColor(COLOR_BTNSHADOW);
	COLORREF rgbHilite = GetSysColor(COLOR_BTNHIGHLIGHT);

	CGXTabBeam::PatB(pDC, rect.left, rect.top, 1, rect.Height(), RGB(0,0,0));
	CGXTabBeam::PatB(pDC, rect.left+1, rect.top, 1, rect.Height(), rgbHilite);
	CGXTabBeam::PatB(pDC, rect.left+2, rect.top, 1, rect.Height(), rgbFace);
	CGXTabBeam::PatB(pDC, rect.left+3, rect.top, 1, rect.Height(), rgbShadow);
	CGXTabBeam::PatB(pDC, rect.left+4, rect.top, 1, rect.Height(), RGB(0,0,0));
	                             
	// top border	                             
	if (GXGetAppData()->bNotWin4)
		CGXTabBeam::PatB(pDC, rect.left, rect.top, rect.Width(), 1, RGB(0,0,0));
}

void CGXTabWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_pActivePane->SetFocus();

	if (m_bHasHScroll)
	{
		CRect rectClient;
		GetClientRect(&rectClient);

		CRect rect(m_cxBeam+1,
			rectClient.bottom - m_cyHScroll,
			m_cxBeam+m_cxSplitter,
			rectClient.bottom);

		if (rect.PtInRect(point))
		{
			SetCapture();
			m_bTracking = TRUE;
			return;
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CGXTabWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bTracking)
	{
		ReleaseCapture();
		m_bTracking = FALSE;
		OnMouseMove(nFlags, point);
		m_pBeam->m_nxTracking = -2;
		m_pBeam->Invalidate(FALSE);
		m_pBeam->UpdateWindow();
		m_pBeam->m_nxTracking = -1;
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CGXTabWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bTracking && point.x != m_cxBeam)
	{
		if (point.x < m_pBeam->m_nMargin+20)
			return;

		CRect rectClient;
		GetClientRect(&rectClient);

		if (point.x > rectClient.right - 2 * m_nBarWidths)
			return;

		CRect rect(m_cxBeam+1,
			rectClient.bottom - m_nBarWidths,
			m_cxBeam+m_cxSplitter,
			rectClient.bottom);
		InvalidateRect(rect, FALSE);

		m_pBeam->m_nxTracking = min(m_cxBeam, point.x);

		m_cxBeam = point.x;
		RecalcLayout(TRUE);
		UpdateWindow();
		return;
	}
	else if (m_bHasHScroll)
	{
		CRect rectClient;
		GetClientRect(&rectClient);

		CRect rect(m_cxBeam+1,
			rectClient.bottom - m_nBarWidths,
			m_cxBeam+m_cxSplitter,
			rectClient.bottom);

		if (rect.PtInRect(point))
		{
			static HCURSOR NEAR hcurLast = NULL;    // ATTENTION with DLLs
			static HCURSOR NEAR hcurDestroy = NULL;
			static UINT NEAR idcPrimaryLast = 0;        // store the primary IDC

			UINT    idcPrimary = 0;        // app supplied cursor
			LPCTSTR idcSecondary = 0;      // system supplied cursor

#ifdef _AFXDLL
			idcPrimary = GX_IDC_HSPLITBAR;
			// if your compiler reports an error, you should copy the
			// AFX_IDC_HSPLITBAR cursor from AFXRES.RC to your resource file
			// and name it GX_IDC_HSPLITBAR
#else
			idcPrimary = AFX_IDC_HSPLITBAR;
#endif
			idcSecondary = IDC_SIZEWE;

			// Block:
			{
				HCURSOR hcurToDestroy = NULL;
				if (idcPrimary != idcPrimaryLast)
				{
					HINSTANCE hInst = AfxFindResourceHandle(
						MAKEINTRESOURCE(idcPrimary), RT_GROUP_CURSOR);

					// load in another cursor
					hcurToDestroy = hcurDestroy;

					// Note: If this LoadCursor call fails, it is likely that
					// gxres.rc file is not include in your resource file.
					// To correct the situation, include gxres.rc into your
					// resource script.
					// This should be done using the Resource.Set Includes... command.
					// and including gxresrc.h and gxres.rc

					if ((hcurDestroy = hcurLast =
						// AfxGetApp()->LoadCursor(idcPrimary)) == NULL
					   ::LoadCursor(hInst, MAKEINTRESOURCE(idcPrimary))) == NULL)
					{
						// will not look as good
						TRACE0("Warning: Could not find splitter cursor - using system provided alternative.\n");

						ASSERT(hcurDestroy == NULL);    // will not get destroyed
						// hcurLast = AfxGetApp()->LoadCursor(idcSecondary);
						hcurLast = ::LoadCursor(NULL, idcSecondary);
						ASSERT(hcurLast != NULL);
					}
					idcPrimaryLast = idcPrimary;
				}
				ASSERT(hcurLast != NULL);
				::SetCursor(hcurLast);
				ASSERT(hcurLast != hcurToDestroy);
				if (hcurToDestroy != NULL)
					::DestroyCursor(hcurToDestroy); // destroy after being set
			}
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

BOOL CGXTabWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (pWnd == this && !m_bTracking)
		return TRUE;    // we will handle it in the mouse move
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CGXTabWnd::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

	if (m_pActivePane && pOldWnd != m_pActivePane)
		m_pActivePane->SetFocus();
}

BOOL CGXTabWnd::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(rect);
	rect.left = rect.right - m_nBarWidths;
	pDC->FillRect(rect, &m_brErase);

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CGXEditLabelPopup

CGXEditLabelPopup::CGXEditLabelPopup(CRect rect, CWnd* pParentWnd, UINT nID)
{
	if (GXGetAppData()->bWin4)
		Create(WS_BORDER | WS_VISIBLE | ES_AUTOHSCROLL, rect, pParentWnd, nID);
	else
		Create(WS_DLGFRAME | WS_BORDER | WS_VISIBLE | ES_AUTOHSCROLL, rect, pParentWnd, nID);
}

CGXEditLabelPopup::~CGXEditLabelPopup()
{
}

BEGIN_MESSAGE_MAP(CGXEditLabelPopup, CEdit)
	//{{AFX_MSG_MAP(CGXEditLabelPopup)
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGXEditLabelPopup message handlers


void CGXEditLabelPopup::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	GetParent()->SendMessage(WM_GX_EDITCANCEL, 0, 0);
}

void CGXEditLabelPopup::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == 13)
		GetParent()->SendMessage(WM_GX_EDITEND, 0, 0);
	else if (nChar == 27)
		GetParent()->SendMessage(WM_GX_EDITCANCEL, 0, 0);
	else
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CGXEditLabelPopup::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar != 13 && nChar != 27)
		CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
}

UINT CGXEditLabelPopup::OnGetDlgCode()
{
	return CEdit::OnGetDlgCode() | DLGC_WANTALLKEYS;
}


void CGXTabWnd::OnNcPaint()
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

void CGXTabWnd::OnSysColorChange()
{
	CWnd::OnSysColorChange();

	GXGetSysData()->UpdateSysColors();

	m_brErase.DeleteObject();
	m_brErase.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));

	Invalidate();

#if _MFC_VER < 0x0300
	if (GXGetAppData()->bWin4)
		OnWinIniChange(NULL);
#endif
}

void CGXTabWnd::OnWinIniChange(LPCTSTR lpszSection)
{
	CWnd::OnWinIniChange(lpszSection);

	GXGetSysData()->UpdateSysSettings();

	m_cxVScroll = GetSystemMetrics(SM_CXVSCROLL);
	m_cyHScroll = GetSystemMetrics(SM_CYHSCROLL);
	m_nBarWidths = m_cyHScroll + 2*GXGetAppData()->bWin4;

	RecalcLayout();
}

