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

// gxtbeam.cpp : implementation of CGXTabBeam class
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

#ifndef _GXSTATE_H_
#include "gxstate.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXTBEAM")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CGXTabBeam, CWnd);
IMPLEMENT_DYNAMIC(CGXEditLabelPopup, CEdit);

// enable use as custom control
IMPLEMENT_REGISTER(CGXTabBeam, CS_DBLCLKS | GX_GLOBALCLASS, 0, 0, 0);

// much of this code (e.g. CreateCommon, RecalcLayout, ...)
// is copied from the CSplitterWnd class and adapted to the tabwnd class

/////////////////////////////////////////////////////////////////////////////
// Win4 specific defines

#define new DEBUG_NEW

// Scrolling
const GX_SCRLMOSTLEFT  = 0;
const GX_SCRLLEFT  = 1;
const GX_SCRLRIGHT = 2;
const GX_SCRLMOSTRIGHT = 3;

// Colors for Arrows
const COLORREF rgbBlack = RGB(0,0,0);
const COLORREF rgbGrey  = RGB(96,96,96);

/////////////////////////////////////////////////////////////////////////////
// CGXTabInfo

CGXTabInfo::CGXTabInfo()
{
	pExtra = 0;
	bSel = FALSE;
	nWidth = 60;
	bDeleteExtra = FALSE;

	// Scrollbar Info
	memset(&sbInfoVert, 0, sizeof(SCROLLINFO));
	memset(&sbInfoHorz, 0, sizeof(SCROLLINFO));
}

CGXTabInfo::~CGXTabInfo()
{
	if (bDeleteExtra)
		delete pExtra;
}


/////////////////////////////////////////////////////////////////////////////
// CGXTabBeam

BEGIN_MESSAGE_MAP(CGXTabBeam, CWnd)
	//{{AFX_MSG_MAP(CGXTabBeam)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_WININICHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
CGXTabBeam::CGXTabBeam()
{
	m_pFontNormal = 0;
	m_pFontCurrent = 0;
	m_nHeight = GetSystemMetrics(SM_CYHSCROLL);
	m_bScrollBtns = TRUE;
	m_nScrollBtns = 2;
	m_nButtonWidth = m_nHeight;
	m_nMargin = m_nScrollBtns * m_nButtonWidth;
	m_nWidthEx = 7;
	m_nxTracking = -1;
	m_nTabs = 0;
	m_nCurrent = -1;
	m_nFirstTab = 0;
	m_pNotifyWnd = 0;
	m_hcurArrow  = ::LoadCursor(NULL, IDC_ARROW);
}
BOOL CGXTabBeam::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if(CWnd::Create(NULL,NULL,dwStyle|WS_CHILD,rect,pParentWnd, nID) )
	{
		CreateFonts();
		SetWindowPos(NULL,0,0,1,m_nHeight,SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
		UpdateColors();
		return TRUE;
	}return FALSE;
}
CGXTabBeam::~CGXTabBeam(){DestroyFonts();for(int i=0;i<m_nTabs;i++)delete m_TabInfo[i];}
void CGXTabBeam::CreateBrushes()
{
	m_brFaceSel.DeleteObject();
	m_brFaceNormal.DeleteObject();
	m_brFaceSel.CreateSolidBrush(m_rgbFaceSel);
	m_brFaceNormal.CreateSolidBrush(m_rgbFaceNormal);
}
void CGXTabBeam::CreateFonts()
{
	DestroyFonts();
	LOGFONT lf;
	// use "Arial" as default font
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfClipPrecision = CLIP_LH_ANGLES | CLIP_STROKE_PRECIS;
	_tcscpy(lf.lfFaceName, _T("Arial Cyr"));
#if _MFC_VER >= 0x0400
	if (GetSystemMetrics(SM_DBCSENABLED))
	{
		// Win95/4.0 only supports DEFAULT_GUI_FONT.  Using this
		// font makes it more easy to port the app to
		// international markets. Load default system font
		CFont font;
		font.CreateStockObject(DEFAULT_GUI_FONT);
		if(font.GetSafeHandle()==0)font.CreateStockObject(SYSTEM_FONT);
		// Retrieve logical font information
		if(font.GetSafeHandle())font.GetObject(sizeof(lf),&lf);
	}
#endif
	lf.lfCharSet = GXGetFontState()->GetFontCharset(GXGetFontState()->LookupFontInfo(lf.lfFaceName));
	lf.lfItalic = 0;
	lf.lfUnderline = 0;
	lf.lfHeight = (m_nHeight-2);	// font size based on tab beam height
	// Create the normal font
	lf.lfWeight = FW_NORMAL;
	m_pFontNormal = new CFont;
	m_pFontNormal->CreateFontIndirect(&lf);
	// Create the bold font
	lf.lfWeight = FW_BOLD;
	m_pFontCurrent = new CFont;
	m_pFontCurrent->CreateFontIndirect(&lf);
}
void CGXTabBeam::DestroyFonts()
{
	if(m_pFontNormal)delete m_pFontNormal;
	if(m_pFontCurrent)delete m_pFontCurrent;
}
void CGXTabBeam::SetTabBkColor(int nTab, COLORREF cr)
{
	CGXTabInfo *pInfo = &GetTab(nTab);
	pInfo->rgbFaceNormal = cr;
	pInfo->brFaceNormal.DeleteObject();
	pInfo->brFaceNormal.CreateSolidBrush(cr);
	// set flagUseOwnColorFaceNormal to be true
	pInfo->flagUseOwnColorFaceNormal = 1;
}
void CGXTabBeam::SetTabSelBkColor(int nTab, COLORREF cr)
{
	CGXTabInfo *pInfo = &GetTab(nTab);
	pInfo->rgbFaceSel = cr;
	pInfo->brFaceSel.DeleteObject();
	pInfo->brFaceSel.CreateSolidBrush(cr);
	// set flagUseOwnColorFaceSel to be true
	pInfo->flagUseOwnColorFaceSel = 1;
}
void CGXTabBeam::DisplayScrollBtns(BOOL bSet, int nScrollBtns)
{
	if (bSet)
	{
		ASSERT(nScrollBtns==2||nScrollBtns==4);
		m_nScrollBtns=nScrollBtns;
		m_nMargin=m_nScrollBtns*m_nButtonWidth;
	}
	else{m_nScrollBtns=0;m_nMargin=0;}
	m_bScrollBtns = bSet;
	Invalidate(FALSE);
}
void CGXTabBeam::SetNotifyWnd(CWnd*pNotify){m_pNotifyWnd = pNotify;}
// CGXTabBeam Tabs
// Insert Tab or append Tab if bInsert is FALSE
// pExtra can be used for CWnd-Object
// bDelExtra must not be TRUE for a CWnd, because a CWnd normally gets destroyed itself
// (bDelExtra should be TRUE for buffers etc., which should get
// destroyed when the Tab gets destroyed)
void CGXTabBeam::InsertTab(BOOL bInsert, LPCTSTR szLabel, CObject* pExtra, BOOL bDelExtra, int nWidth, HMENU hMenu)
{
	CGXTabInfo* pInfo = new CGXTabInfo;
	pInfo->sLabel = szLabel;
	pInfo->nWidth = nWidth;
	pInfo->pExtra = pExtra;
	pInfo->bDeleteExtra = bDelExtra;
	pInfo->hMenu = hMenu;

	// Use the CGXTabBeam's default color and brush
	pInfo->flagUseOwnColorFaceNormal= 0;
	pInfo->flagUseOwnColorFaceSel=  0;

	// Initialize the color and brush anyway though
	// they are not used. This is to prevent some user go to
	// set the color flags to true in CGXTabInfo directly
	// and get unexpected results.
	pInfo->rgbFaceNormal= m_rgbFaceNormal;
	pInfo->rgbFaceSel= m_rgbFaceSel;
	pInfo->brFaceNormal.CreateSolidBrush(m_rgbFaceNormal);
	pInfo->brFaceSel.CreateSolidBrush(m_rgbFaceSel);

	// Add info and adjust the tab size

	if (m_nCurrent == -1 || !bInsert)
	{
		m_TabInfo.SetAtGrow(m_nTabs, pInfo);
		if (nWidth < 0)
			AdjustTabSize(m_nTabs);
	}
	else
	{
		m_TabInfo.InsertAt(m_nCurrent, pInfo, 1);
		if (nWidth < 0)
			AdjustTabSize(m_nCurrent);
	}
	m_nTabs++;

	Invalidate();

	// Make sure tab is visible

	if (m_nCurrent == -1)
	{
		m_nCurrent = m_nTabs-1;
		ScrollTabInView(m_nCurrent);
	}
}

void CGXTabBeam::DeleteTab(int nTab)
{
	if (m_nCurrent == nTab)
	{
		m_nCurrent = 0;
		m_nFirstTab = 0;
	}

	delete m_TabInfo[nTab];
	m_TabInfo.RemoveAt(nTab);
	m_nTabs--;

	if (m_nFirstTab >= m_nTabs)
		m_nFirstTab = max(0, m_nTabs-1);

	Invalidate();
}

CGXTabInfo& CGXTabBeam::GetTab(int nTab)
{
	ASSERT(nTab >= 0 && nTab < m_TabInfo.GetSize());
	return *((CGXTabInfo*) m_TabInfo[nTab]);
}

// Calculate the left x-coordinate of the tab
int CGXTabBeam::CalcTabOffset(int nTab)
{
	int offset = m_bScrollBtns?m_nMargin+1:0;
	for (int i = m_nFirstTab; i < nTab; i++)
		offset += GetTab(i).nWidth + m_nWidthEx;
	if (nTab < m_nFirstTab)
	{
		for (int i = nTab; i < m_nFirstTab; i++)
			offset -= GetTab(i).nWidth + m_nWidthEx;
	}
	return offset;
}

// Get the tab for offset (x-coordinate)
int CGXTabBeam::CalcTabFromOffset(int nOffset)
{
	int nTab = m_nFirstTab;
	int offset;
	for (offset = m_bScrollBtns?m_nMargin+1:0;
		nTab < m_nTabs && offset < nOffset;
		offset += GetTab(nTab++).nWidth + m_nWidthEx)
		;

	return nTab < m_nTabs || offset > nOffset ? nTab-1 : -1;
}

int CGXTabBeam::CalcLastVisibleTab()
{
	CRect rect;
	GetClientRect(&rect);
	int i;
	int offset = m_bScrollBtns?m_nMargin+1:0;
	offset += GetTab(m_nFirstTab).nWidth + 2*m_nWidthEx;
	for ( i = m_nFirstTab+1; i < m_nTabs && offset < rect.right; i++)
		offset += GetTab(i).nWidth + m_nWidthEx;

	if (offset > rect.right)
		return max(m_nFirstTab, i-2);

	return m_nTabs-1;
}


// Tab has been pressed
void CGXTabBeam::SwitchToTab(CPoint point)
{
	int n = 0;
	if (point.x < m_nMargin)
		return;

	if (point.x - m_nMargin < m_nWidthEx)
		n = max(0, m_nFirstTab - 1);
	else
		n = CalcTabFromOffset(point.x);

	if (n == -1)
		return;

	BOOL bCanSwitch;

	if (m_pNotifyWnd)
		bCanSwitch = !m_pNotifyWnd->SendMessage(WM_GX_CANTABSWITCH, (WPARAM) n, 0);
	else
		bCanSwitch = !GetParent()->SendMessage(WM_GX_CANTABSWITCH, (WPARAM) n, 0);

	if (!bCanSwitch)
		return;

	BOOL bSwitch = (n != m_nCurrent);

	m_nCurrent = n;
	if (!GetTab(m_nCurrent).bSel)
	{
		for (int i = 0; i < m_nTabs; i++)
			GetTab(i).bSel = 0;
	}

	ScrollTabInView(m_nCurrent);

	if (bSwitch)
	{
		Invalidate();
		UpdateWindow();
	}

	// TabSwitch will always be sent, to give the parent window
	// the ability to set the focus to the sheet
	{
		if (m_pNotifyWnd)
			m_pNotifyWnd->PostMessage(WM_GX_TABSWITCH, (WPARAM) m_nCurrent, 0);
		else
			GetParent()->PostMessage(WM_GX_TABSWITCH, (WPARAM) m_nCurrent, 0);
	}
}

void CGXTabBeam::ScrollTabInView(int nTab)
{
	if (nTab < m_nFirstTab)
	{
		Invalidate();
		m_nFirstTab = nTab;
	}
	else if (nTab > CalcLastVisibleTab())
	{
		Invalidate();
		do {
			m_nFirstTab++;
		} while (nTab > CalcLastVisibleTab());
	}
}

// Double Clicked a tab
void CGXTabBeam::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (point.x < m_nMargin || CalcTabFromOffset(point.x) == -1)
	{
		OnLButtonDown(nFlags, point);
		return;
	}

	int n = m_nCurrent;
	if (n < m_nFirstTab)
	{
		OnLButtonDown(nFlags, point);
		return;
	}

	if (m_pNotifyWnd)
		m_pNotifyWnd->PostMessage(WM_GX_TABDBLCLK, (WPARAM) n, 0);
	else
		GetParent()->PostMessage(WM_GX_TABDBLCLK, (WPARAM) n, 0);
}

// Adjust tab size to content
void CGXTabBeam::AdjustTabSize(int nTab)
{
	CClientDC dc(this);

	CFont* pOldFont = dc.SelectObject(m_pFontCurrent);

	CSize size = dc.GetTextExtent(
		GetTab(nTab).sLabel,
		GetTab(nTab).sLabel.GetLength());

	dc.SelectObject(pOldFont);

	GetTab(nTab).nWidth = size.cx + m_nWidthEx;
	Invalidate();
}

int CGXTabBeam::FindTab(const CObject *pObject)
{
	for (int i = 0; i < m_nTabs; i++)
		if (GetTab(i).pExtra == pObject)
			return i;
	return -1;
}

// Current Selection, TabCount inlines

int CGXTabBeam::GetCurSel()
{ 
	return m_nCurrent; 
}

int CGXTabBeam::GetCount()
{ 
	return m_nTabs; 
}


/////////////////////////////////////////////////////////////////////////////
// CGXTabBeam Painting

void CGXTabBeam::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	int i;
	CRect rectPaint;
	dc.GetClipBox(&rectPaint);

	if (m_nxTracking >= 0)
		rectPaint.left = max(m_nxTracking-1, 0);

	CDC dcMem;
	CBitmap bmPaint;
	CBitmap* pbmOld;

	GetClientRect(rectPaint);
	dcMem.CreateCompatibleDC(&dc);
	bmPaint.CreateCompatibleBitmap(&dc, rectPaint.Width(), rectPaint.Height());
	pbmOld = (CBitmap*) dcMem.SelectObject(&bmPaint);

	dcMem.OffsetViewportOrg(-rectPaint.left, -rectPaint.top);
	dcMem.SetBrushOrg(rectPaint.left % 8, rectPaint.top % 8);   // align dcMem with grid's dc

	// default font
	CFont* pOldFont = dcMem.SelectObject(m_pFontNormal);

	CRect rect;
	dcMem.GetClipBox(&rect);

	// Scroll Buttons
	dcMem.SelectClipRgn(NULL);
	if (m_bScrollBtns && (m_nxTracking == -2 || rect.left <= m_nMargin))
	{
		for (i = 0; i < m_nScrollBtns; i++)
			DrawButton(&dcMem, i + ((m_nScrollBtns==2)?1:0), FALSE);
	}

	// Tabs
	{
		rect.left = max(rect.left, m_bScrollBtns?m_nMargin+1:0);
		dcMem.IntersectClipRect(rect);

		// Grey Beam
		PatB(&dcMem, rect.left, 1, rect.Width(), rect.Height(), m_rgbBeam);

		// Black Border
		PatB(&dcMem, rect.left, 0, rect.Width(), 1, rgbBlack);

		// Which tabs need painting?
		int nLast = CalcTabFromOffset(rect.right+m_nWidthEx);
		if (nLast == -1)
			nLast = m_nTabs-1;

		int nFirst = max(CalcTabFromOffset(rect.left)-1, 0);

		// Draw tabs
		for (i = nLast; i >= nFirst; i--)
			if (i != m_nCurrent)
				DrawTab(&dcMem, i);

		// Current tab
		if (m_nCurrent >= nFirst && m_nCurrent <= nLast)
			DrawTab(&dcMem, m_nCurrent);
	}

	// Clean up
	dcMem.SelectObject(pOldFont);

	dc.BitBlt(rectPaint.left, rectPaint.top, rectPaint.Width(), rectPaint.Height(), &dcMem,
			rectPaint.left, rectPaint.top, SRCCOPY);

	// clean-up for memory DC
	dcMem.SelectObject(pbmOld);
}


void CGXTabBeam::DrawTab(CDC* pDC, int nTab)
{
	int x = CalcTabOffset(nTab);
	BOOL bSel = (GetTab(nTab).bSel);
	BOOL bCur = (nTab == m_nCurrent);
	
	int nHeight = m_nHeight - GXGetAppData()->bNotWin4;

	// Tab Polygon
	CRgn rgn;
	POINT pts[4];
	pts[0].x = x;                                   pts[0].y = 0;
	pts[1].x = x+GetTab(nTab).nWidth+2*m_nWidthEx;  pts[1].y = 0;
	pts[2].x = x+GetTab(nTab).nWidth+m_nWidthEx;    pts[2].y = nHeight;
	pts[3].x = x+m_nWidthEx;                        pts[3].y = nHeight;
	rgn.CreatePolygonRgn(pts, 4, ALTERNATE);

	if (bSel || bCur)
	{
		if (!GetTab(nTab).flagUseOwnColorFaceSel)
			pDC->FillRgn(&rgn, &m_brFaceSel);
		else
			pDC->FillRgn(&rgn, &GetTab(nTab).brFaceSel);
	}
	else
	{
		if (!GetTab(nTab).flagUseOwnColorFaceNormal)
			pDC->FillRgn(&rgn, &m_brFaceNormal);
		else
			pDC->FillRgn(&rgn, &GetTab(nTab).brFaceNormal);
	}

	// Frame
	pDC->FrameRgn(&rgn, &m_brBlack, 1, 1);

	if (bCur)
		PatB(pDC, pts[0].x, 0, pts[1].x-pts[0].x, 1, m_rgbFaceSel);

	// shadow
	CPen* pOldPen = pDC->SelectObject(&m_penShadow);
	pDC->MoveTo(x+m_nWidthEx+1, nHeight-1);
	pDC->LineTo(x+GetTab(nTab).nWidth+m_nWidthEx-1, nHeight-1);
	pDC->LineTo(x+GetTab(nTab).nWidth+2*m_nWidthEx-2, 0);
	pDC->SelectObject(pOldPen);

	// Draw Text
	COLORREF rgbText = bCur ? m_rgbTextCurrent : m_rgbTextNormal;
	COLORREF rgbBack = bSel ? m_rgbFaceSel : m_rgbFaceNormal;

	pDC->SetTextColor(rgbText);
	pDC->SetBkColor(rgbBack);
	pDC->SetBkMode(TRANSPARENT);

	// Inside rect
	CRect rect;
	rect.left = x+m_nWidthEx;
	rect.right = rect.left+GetTab(nTab).nWidth;
	rect.top = 0;
	rect.bottom = m_nHeight;

	// check if current tab
	CFont* pOldFont = NULL;
	if (bCur)
		pOldFont = pDC->SelectObject(m_pFontCurrent);

	// center vertically
	CSize size = pDC->GetTextExtent(_T("t"), 1);
	rect.top += max(m_nHeight-size.cy, 0)/2;

	pDC->DrawText(GetTab(nTab).sLabel, -1, rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	// clean up
	if (bCur)
		pDC->SelectObject(pOldFont);
}

void CGXTabBeam::DrawButton(CDC* pDC, int nButton, BOOL bPressed)
{
	int i = nButton;
	if (m_nScrollBtns == 2)i--;
	CRect rect( i * m_nButtonWidth, 0, (i + 1) * m_nButtonWidth, m_nHeight);
	DrawBlankButton(pDC, rect.left, rect.top, rect.Width(), rect.Height(), bPressed);
	// Black border
	if (GXGetAppData()->bWin4)PatB(pDC, rect.left, rect.bottom-1, rect.Width(), 1, rgbBlack);
	// Draw arrows
	int x, y;
	int cx = m_nHeight > 19 ? 6 : 5;
	if (GXGetAppData()->bWin4)cx += 2;
	int cy = m_nHeight > 19 ? 9 : 7;
	// center the 9-pixel high arrow vertically
	y = rect.top + ((rect.Height() - 9) / 2);
	int y2 = rect.top + ((rect.Height() - cy) / 2);
	// if pressed, image gets moved
	if (bPressed)y++;
	if (m_nScrollBtns == 2)i++;
	COLORREF rgb = rgbBlack;    // Black
	CRect rectClient;
	GetClientRect(rectClient);
	if (GetCount() <= 1 || i <= 1 && m_nFirstTab == 0 || i >= 2 && CalcTabFromOffset(rectClient.right) == -1)
		rgb = rgbGrey;
	switch(i)
	{
	case GX_MOVERECORD_FIRST:
		// Most left
		x = rect.left + ((rect.Width() - cx) / 2) - 1;
		if (bPressed)x++;
		PatB(pDC, x, y2, 1, cy, rgb);
	case GX_MOVERECORD_PREV:
		// Left Arrow
		x = rect.left + ((rect.Width() - cx) / 2) - 1;
		if (bPressed)x++;
		PatB(pDC, x+2, y+4, 1, 1, rgb);
		PatB(pDC, x+3, y+3, 1, 3, rgb);
		PatB(pDC, x+4, y+2, 1, 5, rgb);
		PatB(pDC, x+5, y+1, 1, 7, rgb);
		if (m_nHeight > 19)PatB(pDC, x+6,   y, 1, 9, rgb);
		break;
	case GX_MOVERECORD_LAST:
		// Most Right
		x = rect.right - ((rect.Width() - cx) / 2);
		if (bPressed)x++;
		PatB(pDC, x, y2, 1, cy, rgb);
	case GX_MOVERECORD_NEXT:
		// Right Arrow
		x = rect.right - ((rect.Width() - cx) / 2);
		if (bPressed)x++;
		PatB(pDC, x-2, y+4, 1, 1, rgb);
		PatB(pDC, x-3, y+3, 1, 3, rgb);
		PatB(pDC, x-4, y+2, 1, 5, rgb);
		PatB(pDC, x-5, y+1, 1, 7, rgb);
		if (m_nHeight > 19)PatB(pDC, x-6,   y, 1, 9, rgb);
		break;
	}
}
BOOL CGXTabBeam::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(rect);
	if (m_nxTracking >= 0)
	{
		rect.left = max(m_nxTracking-1, 0);
		PatB(pDC, rect.left, 1, rect.Width(), rect.Height(), m_rgbBeam);
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CGXTabBeam Mouse Actions
// Replacement for SetCapture
// Sends WM_MOUSEMOVE Messages, when a Button is pressed
void CGXTabBeam::AutoMove(UINT nRefMessage)
{
	MSG   msgLoop;          // dummmy message structure to process incoming
							// messages while autoscrolling.
	CPoint ptCursorPos;
	msgLoop.message = 0;    // forces at least one loop.
	SetCapture();
	DWORD nLastTicks = GetTickCount();
	DWORD nTicksNeeded = 200;
	// Process all messages until the relevant mouse button has been released.
	while (msgLoop.message != nRefMessage) // nRefMessage depends on which button
										// has been used to trigger autoscrolling.
	{
		// Process incoming messages until autoscroll button is released
		if (PeekMessage(&msgLoop, 0, 0, 0, PM_REMOVE))
		{
			if(msgLoop.message!=WM_MOUSEMOVE)
				{TranslateMessage(&msgLoop);DispatchMessage(&msgLoop);}continue;
		}
		GetCursorPos(&ptCursorPos);
		ScreenToClient(&ptCursorPos);
		if (GetTickCount() - nLastTicks > nTicksNeeded)
		{
			// peridically do OnMouseMove
			OnMouseMove(0, ptCursorPos);
			if (nTicksNeeded > 100)nTicksNeeded -= 20;
		}
	}
	ReleaseCapture();
}
// Button is hit
// 0 = most left, 1 = scroll left, 2 = scroll right, 3 = most right
// if nButton is not -1, it is checked if the mouse has left
// the pressed button, (the button will get unpressed in OnMouseMove)
int CGXTabBeam::ScrollBtnHit(CPoint point, int nButton /*= -1*/)
{
	if(point.y<0||point.y>m_nHeight||point.x<0||point.x>=m_nMargin)return-1;
	// get Button
	int nBtn = point.x / m_nButtonWidth;
	if (m_nScrollBtns == 2)nBtn++;
	CRect rectClient;
	GetClientRect(rectClient);
	if (nBtn <= 1 && m_nFirstTab == 0 || nBtn >= 2 && CalcTabFromOffset(rectClient.right) == -1)
		return -1;
	if (nButton != -1 && nBtn != nButton)return -1;
	// Button is pressed
	Scroll(nBtn);
	return nBtn;
}
void CGXTabBeam::Scroll(int nDirection)
{
	// ensure that edit-popup is not visible
	GetParent()->SendMessage(WM_GX_EDITCANCEL, 0, 0);
	int n = m_nFirstTab;
	switch (nDirection)
	{
	case GX_SCRLMOSTLEFT:  n = 0; break;
	case GX_SCRLLEFT:  n = m_nFirstTab-1; break;
	case GX_SCRLRIGHT: n = m_nFirstTab+1; break;
	case GX_SCRLMOSTRIGHT: n = m_nTabs-1; break;
	default:return;
	}
	// Calculate last visible tabs
	CRect rect;
	GetClientRect(rect);
	rect.left = m_nMargin+1;

	int nLast = m_nTabs-1;
	int dx = GetTab(nLast).nWidth+2*m_nWidthEx;
	while (nLast > m_nFirstTab && dx+GetTab(nLast-1).nWidth+m_nWidthEx < rect.Width())
		dx += GetTab(--nLast).nWidth+m_nWidthEx;
	// Adjust n (this the new value for m_nFirstTab)
	n = max(0, min(nLast, n));
	// Check if n is different
	if (n != m_nFirstTab)
	{
		CRect rect;
		GetClientRect(rect);
		rect.left = m_nMargin+1;
		// Scroll
		if (n > m_nFirstTab)
		{
			int scrolled = 0;
			for (int i = m_nFirstTab; i < n; i++)scrolled += GetTab(i).nWidth+m_nWidthEx;
			ScrollWindow(-scrolled, 0, rect, rect);
		}
		else
		{
			int scrolled = 0;
			for (int i = n; i < m_nFirstTab; i++)
				scrolled += GetTab(i).nWidth+m_nWidthEx;
			ScrollWindow(scrolled, 0, rect, rect);
			rect.right = rect.left+scrolled+m_nWidthEx;
			InvalidateRect(rect, FALSE);
		}m_nFirstTab = n;
	}
}
void CGXTabBeam::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ((m_nBtnDown = ScrollBtnHit(point)) != -1)
	{
		// Scrolling
		CClientDC dc(this);
		DrawButton(&dc, m_nBtnDown, m_bBtnDown = TRUE);

		m_bMouseCapture = TRUE;
		AutoMove(WM_LBUTTONUP); // (SetCapture until WM_LBUTTONUP)
	}
	else if (nFlags & MK_CONTROL || nFlags & MK_SHIFT)
	{
		// Select Tabs
		CRect rect;
		GetClientRect(rect);
		rect.left = m_bScrollBtns?m_nMargin+1:0;
		int n = CalcTabFromOffset(point.x);
		if (n == -1)return;
		if (nFlags & MK_CONTROL)
		{
			GetTab(n).bSel = !GetTab(n).bSel;InvalidateRect(rect, FALSE);
		}
		else if (m_nCurrent != -1)
		{
			while (n != m_nCurrent)
			{
				GetTab(n).bSel = 1;n = (n > m_nCurrent)?n-1:n+1;
			}
			InvalidateRect(rect, FALSE);
		}
	}
	else
		SwitchToTab(point);

	CWnd::OnLButtonDown(nFlags, point);
}
void CGXTabBeam::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bMouseCapture)
	{
		m_bMouseCapture = FALSE;
		if (m_nBtnDown != -1 && m_bBtnDown)
		{
			UpdateWindow();
			CClientDC dc(this);
			for(int i=0;i<m_nScrollBtns; i++)DrawButton(&dc, i + ((m_nScrollBtns==2)?1:0), FALSE);
		}
	}
	m_bBtnDown = FALSE;
	CWnd::OnLButtonUp(nFlags, point);
}
void CGXTabBeam::OnMouseMove(UINT /*nFlags*/, CPoint point)
{
	SetCursor(m_hcurArrow);
	if (!m_bMouseCapture)return;
	if (m_nBtnDown != -1)
	{
		// Check if mouse has left the button,
		// ScrollBtnHit does scrolling
		BOOL bDraw = FALSE;
		if(ScrollBtnHit(point, m_nBtnDown)==-1){bDraw=m_bBtnDown;m_bBtnDown = FALSE;}
		else{bDraw = !m_bBtnDown;m_bBtnDown = TRUE;}
		if (bDraw)
		{
			UpdateWindow();
			CClientDC dc(this);
			for (int i = 0; i < m_nScrollBtns; i++)
			{
				int n = i + ((m_nScrollBtns==2)?1:0);
				DrawButton(&dc, n, n == m_nBtnDown && m_bBtnDown);
			}
		}
	}
}

void CGXTabBeam::UpdateColors()
{
	// Colors
	m_rgbTextCurrent = GetSysColor(COLOR_WINDOWTEXT);
	m_rgbTextNormal = GetSysColor(COLOR_WINDOWTEXT);
	m_rgbFaceNormal = GetSysColor(COLOR_BTNFACE);
	m_rgbFaceSel = GetSysColor(COLOR_WINDOW); // RGB(255,255,255);
	m_rgbBeam = GetSysColor(COLOR_SCROLLBAR);

	// Brushes
	m_brFaceSel.DeleteObject();
	m_brFaceSel.CreateSolidBrush(m_rgbFaceSel);
	m_brFaceNormal.DeleteObject();
	m_brFaceNormal.CreateSolidBrush(m_rgbFaceNormal);
	m_brBlack.DeleteObject();
	m_brBlack.CreateSolidBrush(RGB(0,0,0));

	// Pens
	m_penShadow.DeleteObject();
	m_penShadow.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
}
void CGXTabBeam::OnSysColorChange()
{
	CWnd::OnSysColorChange();
	GXGetSysData()->UpdateSysColors();
	UpdateColors();
	Invalidate();
#if _MFC_VER < 0x0300
	// Also check scrollbar size
	if(GXGetAppData()->bWin4)OnWinIniChange(NULL);
#endif
}
void CGXTabBeam::OnWinIniChange(LPCTSTR lpszSection)
{
	CWnd::OnWinIniChange(lpszSection);
	GXGetSysData()->UpdateSysSettings();
	int cy = GetSystemMetrics(SM_CYHSCROLL);
	if (m_nHeight != cy)
	{
		// scrollbar size did change
		m_nButtonWidth = m_nHeight = cy;
		m_nMargin = m_nScrollBtns * m_nButtonWidth;
		CreateFonts();
		for (int i = 0; i < m_nTabs; i++)AdjustTabSize(i);
	}Invalidate();
}
// Drawing helpers
void AFXAPI CGXTabBeam::PatB(CDC* pDC,int x,int y,int dx,int dy, COLORREF rgb)
{
	RECT    rc;
	rc.left   = x;
	rc.top    = y;
	rc.right  = x + dx;
	rc.bottom = (y + dy);
	COLORREF rgbOld = pDC->GetBkColor();
	pDC->SetBkColor(rgb);
	pDC->ExtTextOut(0,0,ETO_OPAQUE,&rc,NULL,0,NULL);
	pDC->SetBkColor(rgbOld);
}
void AFXAPI CGXTabBeam::DrawBlankButton(CDC* pDC, int x, int y, int dx, int dy, BOOL bPressed)
{
	// face color
	COLORREF rgbFace    = GXGetSysData()->GetSysColor(COLOR_BTNFACE);
	COLORREF rgbShadow  = GXGetSysData()->GetSysColor(COLOR_BTNSHADOW);
	COLORREF rgbHilite = GXGetSysData()->GetSysColor(COLOR_BTNHIGHLIGHT);
	COLORREF rgbFrame   = GXGetSysData()->GetSysColor(COLOR_WINDOWFRAME);
	x++;y++;dx--;dy--;
	// border around button
	PatB(pDC,x-1,y-1,dx+1,1,rgbFrame);
	PatB(pDC,x,y+dy,dx,1,rgbFrame);
	PatB(pDC,x-1,y,1,dy,rgbFrame);
	PatB(pDC,x+dx,y,1,dy,rgbFrame);
	// interior grey
	PatB(pDC, x, y, dx, dy, rgbFace);
	if (bPressed)
	{
		// pressed in or down or down disabled
		PatB(pDC, x, y, 1, dy, rgbShadow);
		PatB(pDC, x, y, dx, 2, rgbShadow);
		PatB(pDC, x + dx - 1, y, 1, dy, rgbShadow);
		PatB(pDC, x, y + dy-1, dx, 1,   rgbShadow);
	}
	else
	{
		// regular button look
		if (GXGetAppData()->bWin4)
		{
			PatB(pDC, x+1, y+1, 1, dy - 2, rgbHilite);
			PatB(pDC, x+1, y+1, dx - 2, 1, rgbHilite);
			PatB(pDC, x + dx - 1, y+1, 1, dy-1, rgbShadow);
			PatB(pDC, x+1, y + dy-2, dx-1, 1,   rgbShadow);
		}
		else
		{
			PatB(pDC, x, y, 1, dy - 1, rgbHilite);
			PatB(pDC, x, y, dx - 1, 1, rgbHilite);
			PatB(pDC, x + dx - 1, y, 1, dy, rgbShadow);
			PatB(pDC, x, y + dy-2, dx, 1,   rgbShadow);
			PatB(pDC, x + 1 + dx - 3, y + 1, 1, dy-2, rgbShadow);
			PatB(pDC, x + 1, y + dy - 3, dx - 2, 1, rgbShadow);
		}
	}
}
