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

// GXSplit.cpp : implementation of CGXSplitterWnd class
//

#include "stdafx.h"
#include "gxresrc.h"

#ifndef _GXSPLIT_H_
#include "GXSplit.h"
#endif

#include "afxpriv.h"

#if _MFC_VER >= 0x0400

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CGXSplitterWnd, CSplitterWnd);

CGXInsideWndState::CGXInsideWndState()
{
	pWnd = NULL;
	ndx = 0;
}

CGXInsideWndState::~CGXInsideWndState()
{
	delete pWnd;
}

/////////////////////////////////////////////////////////////////////////////
// Visual attributes and other constants

#define CX_BORDER   1
#define CY_BORDER   1

struct GX_SPLITTER_DATA: public CNoTrackObject
{
	GX_SPLITTER_DATA();

	int cxBorder2, cyBorder2;

	BOOL    bWin4;          // TRUE if Windows 4.0
	BOOL    bNotWin4;       // TRUE if not Windows 4.0

	// color values of system colors used for CToolBar
	COLORREF clrBtnFace, clrBtnShadow, clrBtnHilite;
	COLORREF clrBtnText, clrWindowFrame;
#ifdef _MAC
	COLORREF clr3DLight;
#endif

	// standard cursors
	HCURSOR hcurWait;
	HCURSOR hcurArrow;
	HCURSOR hcurHelp;       // cursor used in Shift+F1 help
};

PROCESS_LOCAL(GX_SPLITTER_DATA, _gxSplitData)

// Initialization code
GX_SPLITTER_DATA::GX_SPLITTER_DATA()
{
	// Cache various target platform version information
	DWORD dwVersion = ::GetVersion();
	// nWinVer = (LOBYTE(dwVersion) << 8) + HIBYTE(dwVersion);
	// bWin32s = (dwVersion & 0x80000000) != 0;
	bWin4 = (BYTE)dwVersion >= 4;
	bNotWin4 = 1 - bWin4;   // for convenience

	// cxBorder2 and cyBorder are 2x borders for Win4
	cxBorder2 = bWin4 ? CX_BORDER*2 : CX_BORDER;
	cyBorder2 = bWin4 ? CY_BORDER*2 : CY_BORDER;

	clrBtnFace = ::GetSysColor(COLOR_BTNFACE);
	clrBtnShadow = ::GetSysColor(COLOR_BTNSHADOW);
	clrBtnHilite = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	clrBtnText = ::GetSysColor(COLOR_BTNTEXT);
	clrWindowFrame = ::GetSysColor(COLOR_WINDOWFRAME);
#ifdef _MAC
	clr3DLight = ::GetSysColor(COLOR_3DLIGHT);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Visual attributes and other constants

// HitTest return values (values and spacing between values is important)
enum HitTestValue
{
	noHit                   = 0,
	vSplitterBox            = 1,
	hSplitterBox            = 2,
	bothSplitterBox         = 3,        // just for keyboard
	vSplitterBar1           = 101,
	vSplitterBar15          = 115,
	hSplitterBar1           = 201,
	hSplitterBar15          = 215,
	splitterIntersection1   = 301,
	splitterIntersection225 = 525,
	gxInsideWndBox			= 1024
};

/////////////////////////////////////////////////////////////////////////////
// CGXSplitterWnd

// make CGXSplitterWnd a friend of CView 

class CGXSplitterFriendView: public CView
{
	friend class CGXSplitterWnd;
};

CGXSplitterWnd::CGXSplitterWnd()
{
	// Standard cursors and colors
	if (_gxSplitData->hcurWait == NULL)
	{
		// Standard cursors
		_gxSplitData->hcurWait = ::LoadCursor(NULL, IDC_WAIT);
		_gxSplitData->hcurArrow = ::LoadCursor(NULL, IDC_ARROW);
		ASSERT(_gxSplitData->hcurWait != NULL);
		ASSERT(_gxSplitData->hcurArrow != NULL);
		_gxSplitData->hcurHelp = NULL;    // loaded on demand

		// colors
		_gxSplitData->clrBtnFace = ::GetSysColor(COLOR_BTNFACE);
		_gxSplitData->clrBtnShadow = ::GetSysColor(COLOR_BTNSHADOW);
		_gxSplitData->clrBtnHilite = ::GetSysColor(COLOR_BTNHIGHLIGHT);
		_gxSplitData->clrBtnText = ::GetSysColor(COLOR_BTNTEXT);
		_gxSplitData->clrWindowFrame = ::GetSysColor(COLOR_WINDOWFRAME);
#ifdef _MAC
		_gxSplitData->clr3DLight = ::GetSysColor(COLOR_3DLIGHT);
#endif
	}

	m_nvSplitterBoxPos = gxTop;
	m_nhSplitterBoxPos = gxLeft;

	for (int i = 0; i < 4; i++)
		m_apInsideWndState[i] = NULL;

	m_bTrackBox = FALSE;
}

CGXSplitterWnd::~CGXSplitterWnd()
{
	for (int i = 0; i < 4; i++)
		delete m_apInsideWndState[i];
}


BEGIN_MESSAGE_MAP(CGXSplitterWnd, CSplitterWnd)
	//{{AFX_MSG_MAP(CGXSplitterWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
#if _MFC_VER >= 0x0421
	ON_WM_MOUSEWHEEL()
#endif
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplitterWnd tracking support

CRect CGXSplitterWnd::GetSplitterBoxRect(int ht) const //%%
{
	CRect rectClient;
	GetClientRect(&rectClient);
	rectClient.InflateRect(-m_cxBorder, -m_cyBorder);

	CRect rectInside;
	GetInsideRect(rectInside);

	CRect rect;

	if (ht == vSplitterBox)
	{
		if (m_nvSplitterBoxPos == gxBottom)
		{
			rect.SetRect(rectInside.right - CX_BORDER , 
				rectInside.bottom - m_cySplitter - CY_BORDER + _gxSplitData->bWin4, 
				rectClient.right, 
				rectInside.bottom);
		}
		else
		{
			rect.SetRect(rectInside.right, rectClient.top, rectClient.right,
				rectClient.top + m_cySplitter + CY_BORDER - _gxSplitData->bWin4);
		}
	}
	else if (ht == hSplitterBox)
	{
		if (m_nhSplitterBoxPos == gxRight)
		{
			rect.SetRect(rectInside.right - m_cxSplitter - CX_BORDER + _gxSplitData->bWin4, 
				rectInside.bottom - CY_BORDER, 
				rectInside.right,
				rectClient.bottom);
		}
		else
		{
			rect.SetRect(rectClient.left, rectInside.bottom,
				 rectClient.left + m_cxSplitter + CX_BORDER - _gxSplitData->bWin4,
				 rectClient.bottom);
		}
	}

	return rect;
}

void CGXSplitterWnd::OnDrawInsideSplitter(CDC* pDC, GXSplitterBoxPos nType,
	CGXInsideWndState* pState)
{
	// Unused:
	nType;

	const CRect& rectArg = pState->rectBox;
	
	// if pDC == NULL, then just invalidate
	if (pDC == NULL)
	{
		RedrawWindow(rectArg, NULL, RDW_INVALIDATE|RDW_NOCHILDREN);
		return;
	}
	ASSERT_VALID(pDC);

	// otherwise, actually draw
	CRect rect = rectArg;
	if (pState->bResizable)
	{
		pDC->Draw3dRect(rect, _gxSplitData->clrBtnFace, _gxSplitData->clrWindowFrame);
		rect.InflateRect(-CX_BORDER, -CY_BORDER);
		pDC->Draw3dRect(rect, _gxSplitData->clrBtnHilite, _gxSplitData->clrBtnShadow);
		rect.InflateRect(-CX_BORDER, -CY_BORDER);

		// fill the middle
		COLORREF clr = _gxSplitData->clrBtnFace;
#ifdef _MAC
		// just use white for interior if less than 16 colors
		if (pDC->GetDeviceCaps(NUMCOLORS) < 16)
			clr = RGB(0xFF, 0xFF, 0xFF);
#endif
		pDC->FillSolidRect(rect, clr);
	}
	else
	{
		COLORREF rgbFace    = GXGetSysData()->GetSysColor(COLOR_BTNFACE);
		COLORREF rgbFrame   = GXGetSysData()->GetSysColor(COLOR_WINDOWFRAME);

		GXPatB(pDC, rect, GXGetSysData()->GetSysColor(COLOR_WINDOW));
		GXPatB(pDC, rect.left, rect.top, rect.Width(), 1, GXGetAppData()->bWin4 ? rgbFace : rgbFrame);

		// draw black border below info beam
		GXPatB(pDC, rect.left, rect.bottom-1, rect.Width(), 1, 0);
	}

}

		
void CGXSplitterWnd::StartTracking(int ht)
{
	// NOTE: Need to override StartTracking because GetHitRect is not virtual
	// SetSplitCursor is also not virtual
	// All code is copied from CSplitterWnd::StartTracking

	ASSERT_VALID(this);
	if (ht == noHit)
		return;

	// GetHitRect will restrict 'm_rectLimit' as appropriate
	GetInsideRect(m_rectLimit);

	if (ht >= splitterIntersection1 && ht <= splitterIntersection225)
	{
		// split two directions (two tracking rectangles)
		int row = (ht - splitterIntersection1) / 15;
		int col = (ht - splitterIntersection1) % 15;

		GetHitRect(row + vSplitterBar1, m_rectTracker);
		int yTrackOffset = m_ptTrackOffset.y;
		m_bTracking2 = TRUE;
		GetHitRect(col + hSplitterBar1, m_rectTracker2);
		m_ptTrackOffset.y = yTrackOffset;
	}
	else if (ht == bothSplitterBox)
	{
		// hit on splitter boxes (for keyboard)
		GetHitRect(vSplitterBox, m_rectTracker);
		int yTrackOffset = m_ptTrackOffset.y;
		m_bTracking2 = TRUE;
		GetHitRect(hSplitterBox, m_rectTracker2);
		m_ptTrackOffset.y = yTrackOffset;

		// center it
		m_rectTracker.OffsetRect(0, m_rectLimit.Height()/2);
		m_rectTracker2.OffsetRect(m_rectLimit.Width()/2, 0);
	}
	else if (ht >= gxInsideWndBox+gxLeft && ht <= gxInsideWndBox+gxBottom)    //%%
	{										 //%%
		m_bTrackBox = TRUE;					 //%%
	}										 //%%
	else
	{
		// only hit one bar
		GetHitRect(ht, m_rectTracker);
	}

	// allow active view to preserve focus before taking it away
//%%	CView* pView = (CView*)GetActivePane();
	CGXSplitterFriendView* pView = (CGXSplitterFriendView*)GetActivePane();
	if (pView != NULL && pView->IsKindOf(RUNTIME_CLASS(CView)))
	{
		ASSERT_VALID(pView);
		CFrameWnd* pFrameWnd = GetParentFrame();
		ASSERT_VALID(pFrameWnd);
		pView->OnActivateFrame(WA_INACTIVE, pFrameWnd);
	}

	// steal focus and capture
	SetCapture();
	SetFocus();

	// make sure no updates are pending
	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_UPDATENOW);

	// set tracking state and appropriate cursor
	if (!m_bTrackBox)	//%%
	{	//%%
		m_bTracking = TRUE;
		OnInvertTracker(m_rectTracker);
		if (m_bTracking2)
			OnInvertTracker(m_rectTracker2);
	} //%%
	m_htTrack = ht;
	SetSplitCursor(ht);
}

void CGXSplitterWnd::StopTracking(BOOL bAccept)
{
	ASSERT_VALID(this);

	if (m_bTrackBox)
	{
		RecalcLayout();
		ReleaseCapture();
		m_bTrackBox = FALSE;
		return;
	}

	CSplitterWnd::StopTracking(bAccept);
}

void CGXSplitterWnd::GetHitRect(int ht, CRect& rectHit)
{
	ASSERT_VALID(this);

	CRect rectClient;
	GetClientRect(&rectClient);
	rectClient.InflateRect(-m_cxBorder, -m_cyBorder);
	int cx = rectClient.Width();
	int cy = rectClient.Height();
	int x = rectClient.top;
	int y = rectClient.left;

	// hit rectangle does not include border
	// m_rectLimit will be limited to valid tracking rect
	// m_ptTrackOffset will be set to appropriate tracking offset
	m_ptTrackOffset.x = 0;
	m_ptTrackOffset.y = 0;

	if (ht == vSplitterBox)
	{
		cy = m_cySplitter - (2*m_cyBorder - _gxSplitData->bWin4);
		m_ptTrackOffset.y = -(cy / 2);
		y = GetSplitterBoxRect(vSplitterBox).top;  //%%
		ASSERT(m_pRowInfo[0].nCurSize > 0);
		m_rectLimit.bottom -= cy;
	}
	else if (ht == hSplitterBox)
	{
		cx = m_cxSplitter - (2*m_cxBorder - _gxSplitData->bWin4);
		x = GetSplitterBoxRect(hSplitterBox).left;  //%%
		m_ptTrackOffset.x = -(cx / 2);
		ASSERT(m_pColInfo[0].nCurSize > 0);
		m_rectLimit.right -= cx;
	}
	else if (ht >= vSplitterBar1 && ht <= vSplitterBar15)
	{
		cy = m_cySplitter - (2*m_cyBorder - _gxSplitData->bWin4);
		m_ptTrackOffset.y = -(cy / 2);
		int row;
		for (row = 0; row < ht - vSplitterBar1; row++)
			y += m_pRowInfo[row].nCurSize + m_cySplitterGap;
		m_rectLimit.top = y;
		y += m_pRowInfo[row].nCurSize + m_cyBorderShare + _gxSplitData->bWin4;
		m_rectLimit.bottom -= cy;
	}
	else if (ht >= hSplitterBar1 && ht <= hSplitterBar15)
	{
		cx = m_cxSplitter - (2*m_cxBorder - _gxSplitData->bWin4);
		m_ptTrackOffset.x = -(cx / 2);
		int col;
		for (col = 0; col < ht - hSplitterBar1; col++)
			x += m_pColInfo[col].nCurSize + m_cxSplitterGap;
		m_rectLimit.left = x;
		x += m_pColInfo[col].nCurSize + m_cxBorderShare + _gxSplitData->bWin4;
		m_rectLimit.right -= cx;
	}
	else
	{
		TRACE1("Error: GetHitRect(%d): Not Found!\n", ht);
		ASSERT(FALSE);
	}

	rectHit.right = (rectHit.left = x) + cx;
	rectHit.bottom = (rectHit.top = y) + cy;
}

int CGXSplitterWnd::HitTest(CPoint pt) const
{
	ASSERT_VALID(this);

	CRect rectClient;
	GetClientRect(&rectClient);
	rectClient.InflateRect(-m_cxBorder, -m_cyBorder);

	CRect rectInside;
	GetInsideRect(rectInside);

	for (int i = 0; i < 4; i++)
	{
		if (m_apInsideWndState[i] && m_apInsideWndState[i]->rectBox.PtInRect(pt)) 
		{
			if (m_apInsideWndState[i]->bResizable)
				return gxInsideWndBox + i;
		}
	}

	if (m_bHasVScroll && m_nRows < m_nMaxRows &&
		GetSplitterBoxRect(vSplitterBox).PtInRect(pt)) //%%
//%%		CRect(rectInside.right, rectClient.top, rectClient.right,
//%%		rectClient.top + m_cySplitter - _gxSplitData->bWin4).PtInRect(pt))
	{
		return vSplitterBox;
	}

	if (m_bHasHScroll && m_nCols < m_nMaxCols &&
		GetSplitterBoxRect(hSplitterBox).PtInRect(pt)) //%%
//%%		CRect(rectClient.left, rectInside.bottom,
//%%		 rectClient.left + m_cxSplitter - _gxSplitData->bWin4,
//%%		 rectClient.bottom).PtInRect(pt))
	{
		return hSplitterBox;
	}

	// for hit detect, include the border of splitters
	CRect rect;
	rect = rectClient;
	int col;
	for (col = 0; col < m_nCols - 1; col++)
	{
		rect.left += m_pColInfo[col].nCurSize;
		rect.right = rect.left + m_cxSplitterGap;
		if (rect.PtInRect(pt))
			break;
		rect.left = rect.right;
	}

	rect = rectClient;
	int row;
	for (row = 0; row < m_nRows - 1; row++)
	{
		rect.top += m_pRowInfo[row].nCurSize;
		rect.bottom = rect.top + m_cySplitterGap;
		if (rect.PtInRect(pt))
			break;
		rect.top = rect.bottom;
	}

	// row and col set for hit splitter (if not hit will be past end)
	if (col != m_nCols - 1)
	{
		if (row != m_nRows - 1)
			return splitterIntersection1 + row * 15 + col;
		return hSplitterBar1 + col;
	}

	if (row != m_nRows - 1)
		return vSplitterBar1 + row;

	return noHit;
}

/////////////////////////////////////////////////////////////////////////////
// Main drawing and layout

// Generic routine:
//  for X direction: pInfo = m_pColInfo, nMax = m_nMaxCols, nSize = cx
//  for Y direction: pInfo = m_pRowInfo, nMax = m_nMaxRows, nSize = cy
static void AFXAPI LayoutRowCol(CSplitterWnd::CRowColInfo* pInfoArray,
	int nMax, int nSize, int nSizeSplitter)
{
	ASSERT(pInfoArray != NULL);
	ASSERT(nMax > 0);
	ASSERT(nSizeSplitter > 0);

	CSplitterWnd::CRowColInfo* pInfo;
	int i;

	if (nSize < 0)
		nSize = 0;  // if really too small, layout as zero size

	// start with ideal sizes
	for (i = 0, pInfo = pInfoArray; i < nMax-1; i++, pInfo++)
	{
		if (pInfo->nIdealSize < pInfo->nMinSize)
			pInfo->nIdealSize = 0;      // too small to see
		pInfo->nCurSize = pInfo->nIdealSize;
	}
	pInfo->nCurSize = INT_MAX;  // last row/column takes the rest

	for (i = 0, pInfo = pInfoArray; i < nMax; i++, pInfo++)
	{
		ASSERT(nSize >= 0);
		if (nSize == 0)
		{
			// no more room (set pane to be invisible)
			pInfo->nCurSize = 0;
			continue;       // don't worry about splitters
		}
		else if (nSize < pInfo->nMinSize && i != 0)
		{
			// additional panes below the recommended minimum size
			//   aren't shown and the size goes to the previous pane
			pInfo->nCurSize = 0;

			// previous pane already has room for splitter + border
			//   add remaining size and remove the extra border
			ASSERT(_gxSplitData->cxBorder2 == _gxSplitData->cyBorder2);
			(pInfo-1)->nCurSize += nSize + _gxSplitData->cxBorder2;
			nSize = 0;
		}
		else
		{
			// otherwise we can add the second pane
			ASSERT(nSize > 0);
			if (pInfo->nCurSize == 0)
			{
				// too small to see
				if (i != 0)
					pInfo->nCurSize = 0;
			}
			else if (nSize < pInfo->nCurSize)
			{
				// this row/col won't fit completely - make as small as possible
				pInfo->nCurSize = nSize;
				nSize = 0;
			}
			else
			{
				// can fit everything
				nSize -= pInfo->nCurSize;
			}
		}

		// see if we should add a splitter
		ASSERT(nSize >= 0);
		if (i != nMax - 1)
		{
			// should have a splitter
			if (nSize > nSizeSplitter)
			{
				nSize -= nSizeSplitter; // leave room for splitter + border
				ASSERT(nSize > 0);
			}
			else
			{
				// not enough room - add left over less splitter size
				ASSERT(_gxSplitData->cxBorder2 == _gxSplitData->cyBorder2);
				pInfo->nCurSize += nSize;
				if (pInfo->nCurSize > (nSizeSplitter - _gxSplitData->cxBorder2))
					pInfo->nCurSize -= (nSizeSplitter - _gxSplitData->cyBorder2);
				nSize = 0;
			}
		}
	}
	ASSERT(nSize == 0); // all space should be allocated
}

// repositions client area of specified window
// assumes everything has WS_BORDER or is inset like it does
//  (includes scroll bars)
static void AFXAPI DeferClientPos(AFX_SIZEPARENTPARAMS* lpLayout,
	CWnd* pWnd, int x, int y, int cx, int cy, BOOL bScrollBar)
{
	ASSERT(pWnd != NULL);
	ASSERT(pWnd->m_hWnd != NULL);

	if (bScrollBar)
	{
		// if there is enough room, draw scroll bar without border
		// if there is not enough room, set the WS_BORDER bit so that
		//   we will at least get a proper border drawn
		BOOL bNeedBorder = (cx <= CX_BORDER || cy <= CY_BORDER);
		pWnd->ModifyStyle(bNeedBorder ? 0 : WS_BORDER,
			bNeedBorder ? WS_BORDER : 0);
	}
	CRect rect(x, y, x+cx, y+cy);

	// adjust for border size (even if zero client size)
	if (!_gxSplitData->bWin4)
	{
		if (bScrollBar)
			rect.InflateRect(CX_BORDER, CY_BORDER);
		else
			pWnd->CalcWindowRect(&rect);
	}

	// adjust for 3d border (splitter windows have implied border)
	if ((pWnd->GetExStyle() & WS_EX_CLIENTEDGE) ||
		  pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
		rect.InflateRect(_gxSplitData->cxBorder2, _gxSplitData->cyBorder2);

	// first check if the new rectangle is the same as the current
	CRect rectOld;
	pWnd->GetWindowRect(rectOld);
	pWnd->GetParent()->ScreenToClient(&rectOld);
	if (rect != rectOld)
		AfxRepositionWindow(lpLayout, pWnd->m_hWnd, rect);
}

void CGXSplitterWnd::RecalcLayout()
{
	ASSERT_VALID(this);
	ASSERT(m_nRows > 0 && m_nCols > 0); // must have at least one pane

	CRect rectClient;
	GetClientRect(rectClient);
	rectClient.InflateRect(-m_cxBorder, -m_cyBorder);

	CRect rectInside;
	GetInsideRect(rectInside);

	// layout columns (restrict to possible sizes)
	LayoutRowCol(m_pColInfo, m_nCols, rectInside.Width(), m_cxSplitterGap);
	LayoutRowCol(m_pRowInfo, m_nRows, rectInside.Height(), m_cySplitterGap);

	// fix a small glitch with splitter window
	if (m_nRows == 2 && m_pRowInfo[1].nCurSize == 0)
	{
		rectInside.bottom += 2;
		rectClient.bottom += 2;
	}

	if (m_nCols == 2 && m_pColInfo[1].nCurSize == 0)
	{
		rectInside.right += 2;
		rectClient.right += 2;
	}


	// adjust the panes (and optionally scroll bars)

	// give the hint for the maximum number of HWNDs
	AFX_SIZEPARENTPARAMS layout;
	layout.hDWP = ::BeginDeferWindowPos((m_nCols + 1) * (m_nRows + 1) + 1);

	// reposition size box
	if (m_bHasHScroll && m_bHasVScroll)
	{
		// size of scrollbars
		int cx = (rectClient.right - rectInside.right) - _gxSplitData->bNotWin4;
		int cy = (rectClient.bottom - rectInside.bottom) - _gxSplitData->bNotWin4;

		CWnd* pScrollBar = GetDlgItem(AFX_IDW_SIZE_BOX);
		ASSERT(pScrollBar != NULL);

		// fix style if necessary
		BOOL bSizingParent = (GetSizingParent() != NULL);
		// modifyStyle returns TRUE if style changes
		if (pScrollBar->ModifyStyle(SBS_SIZEGRIP|SBS_SIZEBOX,
				bSizingParent ? SBS_SIZEGRIP : SBS_SIZEBOX))
			pScrollBar->Invalidate();
		pScrollBar->EnableWindow(bSizingParent);

		// reposition the size box
		DeferClientPos(&layout, pScrollBar,
			rectInside.right + _gxSplitData->bNotWin4,
			rectInside.bottom + _gxSplitData->bNotWin4, cx, cy, TRUE);
	}

	// reposition scroll bars
	if (m_bHasHScroll)
	{
		int cy = (rectClient.bottom - rectInside.bottom) - _gxSplitData->bNotWin4;

		int cxSplitterBox = m_cxSplitter + _gxSplitData->bNotWin4;// split box bigger
		int x = rectClient.left;
		int y = rectInside.bottom + _gxSplitData->bNotWin4;
		for (int col = 0; col < m_nCols; col++)
		{
			CWnd* pScrollBar = GetDlgItem(AFX_IDW_HSCROLL_FIRST + col);
			ASSERT(pScrollBar != NULL);
			int cx = m_pColInfo[col].nCurSize;

			if (col == 0)
			{
				// Reserve space for left Splitter
				if (m_nCols == 1 && m_nhSplitterBoxPos == gxLeft)
					x += cxSplitterBox, cx -= cxSplitterBox;
				
				// window docked at left side
				CGXInsideWndState* pState = m_apInsideWndState[gxLeft];
				if (pState)
				{
					// When tracking, use the exact value
					pState->sizeMax.cx = cx - pState->sizeBox.cx;

					if (m_nCols == 1 && m_apInsideWndState[gxRight])
						pState->sizeMax.cx -= m_apInsideWndState[gxRight]->sizeBox.cx + m_apInsideWndState[gxRight]->size.cx;

					if (m_nCols == 1 && m_nhSplitterBoxPos == gxRight)
						pState->sizeMax.cx -= cxSplitterBox;

					if (pState->sizeMax.cx < pState->sizeMin.cx)
						pState->sizeMax.cx = cx;

					if (pState->bResizable)
					{
						pState->lRelSize = min(pState->lRelSize, 1000L);

						if (pState->lRelSize >= 0 && !m_bTrackBox)
							pState->size.cx = pState->sizeMax.cx * pState->lRelSize / 1000L;
						else 
						{
							pState->size.cx = min(pState->size.cx, pState->sizeMax.cx);

							if (pState->bResizable)
								pState->lRelSize = pState->size.cx * 1000L / pState->sizeMax.cx;
						}

						pState->size.cx = max(pState->size.cx, pState->sizeMin.cx);
					}

					// Window
					int nx = pState->ndx;
					//	if (pState->pWnd->IsKindOf(RUNTIME_CLASS(CGXRecordInfoBeam)))
					//	---> nx = 1; 

					if (pState->size.cx > 0)
						pState->rectWnd.SetRect(
							x - nx, 
							y, 
							x+min(pState->sizeMax.cx, pState->size.cx), 
							y+cy+_gxSplitData->bNotWin4
							);
					else
						pState->rectWnd.SetRectEmpty();

					DeferClientPos(&layout, pState->pWnd, pState->rectWnd.left, 
						pState->rectWnd.top, pState->rectWnd.Width(), 
						pState->rectWnd.Height(), 
						FALSE);

					x += pState->size.cx;
					cx -= pState->size.cx;
					
					// Resize Box
					if (pState->sizeBox.cx)
					{
						pState->rectBox.SetRect(
							x, 
							y, 
							x+pState->sizeBox.cx, 
							y+cy);

						InvalidateRect(pState->rectBox);

						// Box
						x += pState->sizeBox.cx;
						cx -= pState->sizeBox.cx;
					}
					else
						pState->rectBox.SetRectEmpty();

					InvalidateRect(pState->rectBox);
				}
			}

			if (col == m_nCols-1)
			{
				// Reserve space for right Splitter
				if (m_nCols == 1 && m_nhSplitterBoxPos == gxRight)
					cx -= cxSplitterBox;
				
				// window docked at right side
				CGXInsideWndState* pState = m_apInsideWndState[gxRight];
				if (pState)
				{
					// When tracking, use the exact value
					pState->sizeMax.cx = cx - pState->sizeBox.cx;

					if (pState->sizeMax.cx < pState->sizeMin.cx)
						pState->sizeMax.cx = cx;

					if (pState->bResizable)
					{
						pState->lRelSize = min(pState->lRelSize, 1000L);

						if (pState->lRelSize >= 0 && !m_bTrackBox)
							pState->size.cx = pState->sizeMax.cx * pState->lRelSize / 1000L;
						else 
						{
							pState->size.cx = min(pState->size.cx, pState->sizeMax.cx);

							if (pState->bResizable)
								pState->lRelSize = pState->size.cx * 1000L / pState->sizeMax.cx;
						}

						pState->size.cx = max(pState->size.cx, pState->sizeMin.cx);
					}

					// Window
					if (pState->size.cx > 0)
						pState->rectWnd.SetRect(
							x + cx - min(pState->sizeMax.cx, pState->size.cx), 
							y, 
							x + cx, 
							y+cy+_gxSplitData->bNotWin4
							);
					else
						pState->rectWnd.SetRectEmpty();

					DeferClientPos(&layout, pState->pWnd, pState->rectWnd.left, 
						pState->rectWnd.top, pState->rectWnd.Width(), 
						pState->rectWnd.Height(), 
						FALSE);

					cx -= pState->size.cx;
					
					// Resize Box
					if (pState->sizeBox.cx)
					{
						pState->rectBox.SetRect(
							x + cx - pState->sizeBox.cx, 
							y, 
							x + cx, 
							y+cy);
						InvalidateRect(pState->rectBox);

						// Box
						cx -= pState->sizeBox.cx;
					}
					else
						pState->rectBox.SetRectEmpty();
				}
			}

			if (m_nCols == 1)
				InvalidateRect(GetSplitterBoxRect(hSplitterBox));

			DeferClientPos(&layout, pScrollBar, x, y, cx, cy, TRUE);
			x += cx + m_cxSplitterGap;
		}
	}

	if (m_bHasVScroll)
	{
		int cx = (rectClient.right - rectInside.right) - _gxSplitData->bNotWin4;

		int cySplitterBox = m_cySplitter + _gxSplitData->bNotWin4;// split box bigger
		int x = rectInside.right + _gxSplitData->bNotWin4;
		int y = rectClient.top;
		for (int row = 0; row < m_nRows; row++)
		{
			CWnd* pScrollBar = GetDlgItem(AFX_IDW_VSCROLL_FIRST + row);
			ASSERT(pScrollBar != NULL);
			int cy = m_pRowInfo[row].nCurSize;

			if (row == 0)
			{
				// Reserve space for left Splitter
				if (m_nRows == 1 && m_nvSplitterBoxPos == gxTop)
					y += cySplitterBox, cy -= cySplitterBox;
				
				// window docked at top side
				CGXInsideWndState* pState = m_apInsideWndState[gxTop];
				if (pState)
				{
					// When tracking, use the exact value
					pState->sizeMax.cy = cy - pState->sizeBox.cy;

					if (m_nRows == 1 && m_apInsideWndState[gxBottom])
						pState->sizeMax.cy -= m_apInsideWndState[gxBottom]->sizeBox.cy + m_apInsideWndState[gxBottom]->size.cy;

					if (pState->sizeMax.cy < pState->sizeMin.cy)
						pState->sizeMax.cy = cy;

					if (pState->bResizable)
					{
						pState->lRelSize = min(pState->lRelSize, 1000L);

						if (pState->lRelSize >= 0 && !m_bTrackBox)
							pState->size.cy = pState->sizeMax.cy * pState->lRelSize / 1000L;
						else 
						{
							pState->size.cy = min(pState->size.cy, pState->sizeMax.cy);

							if (pState->bResizable)
								pState->lRelSize = pState->size.cy * 1000L / pState->sizeMax.cy;
						}

						pState->size.cy = max(pState->size.cy, pState->sizeMin.cy);
					}

					// Window
					int ny = 0;
					//if (pState->pWnd->IsKindOf(RUNTIME_CLASS(CGXRecordInfoBeam)))
					//	ny = 1;

					if (pState->size.cy > 0)
						pState->rectWnd.SetRect(
							x, 
							y - ny, 
							x+cx+_gxSplitData->bNotWin4,
							y+min(pState->sizeMax.cy, pState->size.cy)
							);
					else
						pState->rectWnd.SetRectEmpty();

					DeferClientPos(&layout, pState->pWnd, pState->rectWnd.left, 
						pState->rectWnd.top, pState->rectWnd.Width(), 
						pState->rectWnd.Height(), 
						FALSE);

					y += pState->size.cy;
					cy -= pState->size.cy;
					
					// Resize Box
					if (pState->sizeBox.cy > 0)
					{
						pState->rectBox.SetRect(
							x, 
							y, 
							x+cx,
							y+pState->sizeBox.cy
							);
						InvalidateRect(pState->rectBox);

						// Box
						y += pState->sizeBox.cy;
						cy -= pState->sizeBox.cy;
					}
					else
						pState->rectBox.SetRectEmpty();
				}
			}

			if (row == m_nRows-1)
			{
				// Reserve space for bottom Splitter
				if (m_nRows == 1 && m_nvSplitterBoxPos == gxBottom)
					cy -= cySplitterBox;
				
				// window docked at bottom side
				CGXInsideWndState* pState = m_apInsideWndState[gxBottom];
				if (pState)
				{
					// When tracking, use the exact value
					pState->sizeMax.cy = cy - pState->sizeBox.cy;

					if (pState->sizeMax.cy < pState->sizeMin.cy)
						pState->sizeMax.cy = cy;

					if (pState->bResizable)
					{
						pState->lRelSize = min(pState->lRelSize, 1000L);

						if (pState->lRelSize >= 0 && !m_bTrackBox)
							pState->size.cy = pState->sizeMax.cy * pState->lRelSize / 1000L;
						else 
						{
							pState->size.cy = min(pState->size.cy, pState->sizeMax.cy);

							if (pState->bResizable)
								pState->lRelSize = pState->size.cy * 1000L / pState->sizeMax.cy;
						}

						pState->size.cy = max(pState->size.cy, pState->sizeMin.cy);
					}

					// Window
					if (pState->size.cy > 0)
						pState->rectWnd.SetRect(
							x, 
							y + cy - min(pState->sizeMax.cy, pState->size.cy), 
							x+cx+_gxSplitData->bNotWin4,
							y + cy
							);
					else
						pState->rectWnd.SetRectEmpty();

					DeferClientPos(&layout, pState->pWnd, pState->rectWnd.left, 
						pState->rectWnd.top, pState->rectWnd.Width(), 
						pState->rectWnd.Height(), 
						FALSE);

					cy -= pState->size.cy;
					
					// Resize Box
					if (pState->sizeBox.cy)
					{
						pState->rectBox.SetRect(
							x, 
							y + cy - pState->sizeBox.cy, 
							x+cx,
							y + cy 
							);
						InvalidateRect(pState->rectBox);

						// Box
						cy -= pState->sizeBox.cy;
					}	
					else
						pState->rectBox.SetRectEmpty();
				}
			}

			if (m_nRows == 1)
				InvalidateRect(GetSplitterBoxRect(vSplitterBox));

			DeferClientPos(&layout, pScrollBar, x, y, cx, cy, TRUE);
			y += cy + m_cySplitterGap;
		}
	}

	// update all docked windows
	for (int i = 0; i < 4; i++)
	{
		if (m_apInsideWndState[i] && m_apInsideWndState[i]->pWnd)
			m_apInsideWndState[i]->pWnd->UpdateWindow();
	}

	//BLOCK: Reposition all the panes
	{
		int x = rectClient.left;
		for (int col = 0; col < m_nCols; col++)
		{
			int cx = m_pColInfo[col].nCurSize;
			int y = rectClient.top;
			for (int row = 0; row < m_nRows; row++)
			{
				int cy = m_pRowInfo[row].nCurSize;
				CWnd* pWnd = GetPane(row, col);
				DeferClientPos(&layout, pWnd, x, y, cx, cy, FALSE);
				y += cy + m_cySplitterGap;
			}
			x += cx + m_cxSplitterGap;
		}
	}

	// move and resize all the windows at once!
	if (layout.hDWP == NULL || !::EndDeferWindowPos(layout.hDWP))
		TRACE0("Warning: DeferWindowPos failed - low system resources.\n");

	// invalidate all the splitter bars (with NULL pDC)
	DrawAllSplitBars(NULL, rectInside.right, rectInside.bottom);
}

void CGXSplitterWnd::OnPaint() 
{
	ASSERT_VALID(this);
	CPaintDC dc(this);

	CRect rectClient;
	GetClientRect(&rectClient);
	rectClient.InflateRect(-m_cxBorder, -m_cyBorder);

	CRect rectInside;
	GetInsideRect(rectInside);

	// draw the splitter boxes
	if (m_bHasVScroll && m_nRows < m_nMaxRows)
	{
		OnDrawSplitter(&dc, splitBox,	//%%
			GetSplitterBoxRect(vSplitterBox));
	}

	if (m_bHasHScroll && m_nCols < m_nMaxCols)
	{
		OnDrawSplitter(&dc, splitBox,	//%%
			GetSplitterBoxRect(hSplitterBox));
	}

	for (int i = 0; i < 4; i++)
	{
		if (m_apInsideWndState[i])
			OnDrawInsideSplitter(&dc, (GXSplitterBoxPos) i, m_apInsideWndState[i]);
	}

	// extend split bars to window border (past margins)
	DrawAllSplitBars(&dc, rectInside.right, rectInside.bottom);

	if (!_gxSplitData->bWin4)
	{
		// draw splitter intersections (inside only)
		GetInsideRect(rectInside);
		dc.IntersectClipRect(rectInside);
		CRect rect;
		rect.top = rectInside.top;
		for (int row = 0; row < m_nRows - 1; row++)
		{
			rect.top += m_pRowInfo[row].nCurSize + m_cyBorderShare;
			rect.bottom = rect.top + m_cySplitter;
			rect.left = rectInside.left;
			for (int col = 0; col < m_nCols - 1; col++)
			{
				rect.left += m_pColInfo[col].nCurSize + m_cxBorderShare;
				rect.right = rect.left + m_cxSplitter;
				OnDrawSplitter(&dc, splitIntersection, rect);
				rect.left = rect.right + m_cxBorderShare;
			}
			rect.top = rect.bottom + m_cxBorderShare;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGXSplitterWnd message handlers

void CGXSplitterWnd::OnMouseMove(UINT nFlags, CPoint pt) 
{
	// Unused:
	nFlags;

	// NOTE: Need to override OnMouseMove because HitTest is not virtual
	// All code is copied from CSplitterWnd::OnMouseMove except

	if (GetCapture() != this)
		StopTracking(FALSE);

	if (m_bTracking)
	{
		// move tracker to current cursor position

		pt.Offset(m_ptTrackOffset); // pt is the upper right of hit detect
		// limit the point to the valid split range
		if (pt.y < m_rectLimit.top)
			pt.y = m_rectLimit.top;
		else if (pt.y > m_rectLimit.bottom)
			pt.y = m_rectLimit.bottom;
		if (pt.x < m_rectLimit.left)
			pt.x = m_rectLimit.left;
		else if (pt.x > m_rectLimit.right)
			pt.x = m_rectLimit.right;

		if (m_htTrack == vSplitterBox ||
			m_htTrack >= vSplitterBar1 && m_htTrack <= vSplitterBar15)
		{
			if (m_rectTracker.top != pt.y)
			{
				OnInvertTracker(m_rectTracker);
				m_rectTracker.OffsetRect(0, pt.y - m_rectTracker.top);
				OnInvertTracker(m_rectTracker);
			}
		}
		else if (m_htTrack == hSplitterBox ||
			m_htTrack >= hSplitterBar1 && m_htTrack <= hSplitterBar15)
		{
			if (m_rectTracker.left != pt.x)
			{
				OnInvertTracker(m_rectTracker);
				m_rectTracker.OffsetRect(pt.x - m_rectTracker.left, 0);
				OnInvertTracker(m_rectTracker);
			}
		}
		else if (m_htTrack == bothSplitterBox ||
		   (m_htTrack >= splitterIntersection1 &&
			m_htTrack <= splitterIntersection225))
		{
			if (m_rectTracker.top != pt.y)
			{
				OnInvertTracker(m_rectTracker);
				m_rectTracker.OffsetRect(0, pt.y - m_rectTracker.top);
				OnInvertTracker(m_rectTracker);
			}
			if (m_rectTracker2.left != pt.x)
			{
				OnInvertTracker(m_rectTracker2);
				m_rectTracker2.OffsetRect(pt.x - m_rectTracker2.left, 0);
				OnInvertTracker(m_rectTracker2);
			}
		}
	}
	else if (m_bTrackBox)	//%%
	{//%%
		int i = m_htTrack - gxInsideWndBox;

		CGXInsideWndState* pState = m_apInsideWndState[i];

		int cxSplitterBox = m_cxSplitter + _gxSplitData->bNotWin4;// split box bigger
		int cySplitterBox = m_cySplitter + _gxSplitData->bNotWin4;// split box bigger

		switch (i)
		{
			case gxLeft:
			{
				int cx = m_pColInfo[0].nCurSize;
				pState->sizeMax.cx = cx - pState->sizeBox.cx;

				if (m_nCols == 1 && m_apInsideWndState[gxRight])
					pState->sizeMax.cx -= m_apInsideWndState[gxRight]->size.cx + m_apInsideWndState[gxRight]->sizeBox.cx;

				if (m_nCols == 1)
					pState->sizeMax.cx -= cxSplitterBox;

				pt.x = max(pt.x, pState->sizeMin.cx);
				pt.x = min(pt.x, pState->sizeMax.cx);
				if (pt.x != pState->size.cx)
				{
					pState->size.cx = pt.x;
					RecalcLayout();
				}
			}
			break;

			case gxTop:
			{
				int cy = m_pRowInfo[0].nCurSize;
				pState->sizeMax.cy = cy - pState->sizeBox.cy;

				if (m_nRows == 1 && m_apInsideWndState[gxBottom])
					pState->sizeMax.cy -= m_apInsideWndState[gxBottom]->size.cy + m_apInsideWndState[gxBottom]->sizeBox.cy;

				if (m_nRows == 1)
					pState->sizeMax.cy -= cySplitterBox;

				pt.y = max(pt.y, pState->sizeMin.cy);
				pt.y = min(pt.y, pState->sizeMax.cy);

				if (pt.y != pState->size.cy)
				{
					pState->size.cy = pt.y;
					RecalcLayout();
				}
			}
			break;

			case gxRight:
			{
				int cx = m_pColInfo[m_nCols-1].nCurSize;
				pState->sizeMax.cx = cx - pState->sizeBox.cx;

				if (m_nCols == 1 && m_apInsideWndState[gxLeft])
					pState->sizeMax.cx -= m_apInsideWndState[gxLeft]->size.cx + m_apInsideWndState[gxLeft]->sizeBox.cx;

				if (m_nCols == 1)
					pState->sizeMax.cx -= cxSplitterBox;

				int x = pState->rectWnd.right;
				pt.x = min(pt.x, x - pState->sizeMin.cx);
				pt.x = max(pt.x, x - pState->sizeMax.cx);
				if (pt.x != x - pState->size.cx)
				{
					pState->size.cx = x - pt.x;
					RecalcLayout();
				}
			}
			break;

			case gxBottom:
			{
				int cy = m_pRowInfo[m_nRows-1].nCurSize;
				pState->sizeMax.cy = cy - pState->sizeBox.cy;

				if (m_nRows == 1 && m_apInsideWndState[gxTop])
					pState->sizeMax.cy -= m_apInsideWndState[gxTop]->size.cy + m_apInsideWndState[gxTop]->sizeBox.cy;

				if (m_nRows == 1)
					pState->sizeMax.cy -= cySplitterBox;

				int y = pState->rectWnd.bottom;
				pt.y = min(pt.y, y - pState->sizeMin.cy);
				pt.y = max(pt.y, y - pState->sizeMax.cy);
				if (pt.y != y - pState->size.cy)
				{
					pState->size.cy = y - pt.y;
					RecalcLayout();
				}
			}
			break;
		}

	}//%%
	else
	{
		// simply hit-test and set appropriate cursor

		int ht = HitTest(pt);
		SetSplitCursor(ht);
	}
}

void CGXSplitterWnd::SetSplitCursor(int ht)
{
	// cache of last needed cursor
	static HCURSOR hcurLast = NULL;
	static HCURSOR hcurDestroy = NULL;
	static UINT idcPrimaryLast = 0; // store the primary IDC


	UINT idcPrimary;        // app supplied cursor
	LPCTSTR idcSecondary;    // system supplied cursor (MAKEINTRESOURCE)

	//%% AfxLockGlobals(CRIT_SPLITTERWND);
	if (ht == vSplitterBox ||
		ht >= vSplitterBar1 && ht <= vSplitterBar15
		|| ht == gxInsideWndBox+gxTop || ht == gxInsideWndBox+gxBottom)  //%%
	{
		idcPrimary = AFX_IDC_VSPLITBAR;
		idcSecondary = IDC_SIZENS;
	}
	else if (ht == hSplitterBox ||
		ht >= hSplitterBar1 && ht <= hSplitterBar15
		|| ht == gxInsideWndBox+gxLeft || ht == gxInsideWndBox+gxRight)  //%%
	{
		idcPrimary = AFX_IDC_HSPLITBAR;
		idcSecondary = IDC_SIZEWE;
	}
	else if (ht == bothSplitterBox ||
		(ht >= splitterIntersection1 && ht <= splitterIntersection225))
	{
		idcPrimary = AFX_IDC_SMALLARROWS;
		idcSecondary = IDC_SIZEALL;
	}
	else
	{
		SetCursor(_gxSplitData->hcurArrow);
		idcPrimary = 0;     // don't use it
		idcSecondary = 0;   // don't use it
	}

	if (idcPrimary != 0)
	{
		HCURSOR hcurToDestroy = NULL;
		if (idcPrimary != idcPrimaryLast)
		{
			HINSTANCE hInst = AfxFindResourceHandle(
				MAKEINTRESOURCE(idcPrimary), RT_GROUP_CURSOR);

			// load in another cursor
			hcurToDestroy = hcurDestroy;

			// Note: If this LoadCursor call fails, it is likely that
			//  _AFX_NO_SPLITTER_RESOURCES is defined in your .RC file.
			// To correct the situation, remove the following line from your
			//  resource script:
			//      #define _AFX_NO_SPLITTER_RESOURCES
			// This should be done using the Resource.Set Includes... command.

			if ((hcurDestroy = hcurLast =
			   ::LoadCursor(hInst, MAKEINTRESOURCE(idcPrimary))) == NULL)
			{
				// will not look as good
				TRACE0("Warning: Could not find splitter cursor - using system provided alternative.\n");

				ASSERT(hcurDestroy == NULL);    // will not get destroyed
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
	//%% AfxUnlockGlobals(CRIT_SPLITTERWND);
}

void CGXSplitterWnd::OnLButtonDown(UINT nFlags, CPoint pt) 
{
	// Unused:
	nFlags;

	// NOTE: Need to override OnLButtonDown because HitTest is not virtual
	// All code is copied from CSplitterWnd::OnLButtonDown 

	if (m_bTracking)
		return;

    if( GetActivePane() == NULL )
        SetActivePane( 0, 0 );

	StartTracking(HitTest(pt));
}

void CGXSplitterWnd::OnLButtonDblClk(UINT nFlags, CPoint pt) 
{
	// Unused:
	nFlags;

	// NOTE: Need to override OnLButtonDblClk because HitTest is not virtual
	// All code is copied from CSplitterWnd::OnLButtonDblClk

	int ht = HitTest(pt);
	CRect rect;

	StopTracking(FALSE);

	if ((GetStyle() & SPLS_DYNAMIC_SPLIT) == 0)
		return;     // do nothing if layout is static

	if (ht == vSplitterBox)
	{
		// half split
		SplitRow(m_pRowInfo[0].nCurSize / 2);
	}
	else if (ht == hSplitterBox)
	{
		// half split
		SplitColumn(m_pColInfo[0].nCurSize / 2);
	}
	else if (ht >= vSplitterBar1 && ht <= vSplitterBar15)
	{
		int rowDelete = ht - vSplitterBar1;
		// don't delete the active row
		int row;
		if (GetActivePane(&row, NULL) != NULL && rowDelete == row)
			++rowDelete;
		DeleteRow(rowDelete);
	}
	else if (ht >= hSplitterBar1 && ht <= hSplitterBar15)
	{
		int colDelete = ht - hSplitterBar1;
		// don't delete the active column
		int col;
		if (GetActivePane(NULL, &col) != NULL && colDelete == col)
			++colDelete;
		DeleteColumn(colDelete);
	}
	else if (ht >= splitterIntersection1 && ht <= splitterIntersection225)
	{
		int rowDelete = (ht - splitterIntersection1) / 15;
		int colDelete = (ht - splitterIntersection1) % 15;
		int row, col;
		if (GetActivePane(&row, &col) != NULL)
		{
			// don't delete the active row or column
			if (col == colDelete)
				++colDelete;
			if (row == rowDelete)
				++rowDelete;
		}
		DeleteRow(rowDelete);
		DeleteColumn(colDelete);
	}
}

#if _MFC_VER >= 0x0421
BOOL CGXSplitterWnd::OnMouseWheel(UINT fFlags, short zDelta, CPoint point)
{
	fFlags, zDelta, point;

	// Base class version of CSplitterWnd::OnMouseWheel ASSERTS
	// when the panes are not of type CScrollView - therefore we
	// must avoid that the base class version gets called.

	// Each view can easily add support for mousewheel by
	// using the CGXIntelliMousePlugin class. Therefore
	// this method is not needed for CSplitterView

	return TRUE;
}
#endif


#if _MFC_VER < 0x0400

void CGXSplitterWnd::SetActivePane(int row, int col, CWnd* pWnd)
{
	// set the focus to the pane
	CWnd* pPane = pWnd == NULL ? GetPane(row, col) : pWnd;
	if (pPane->IsKindOf(RUNTIME_CLASS(CView)))
	{
		CFrameWnd* pFrameWnd = GetParentFrame();
		ASSERT_VALID(pFrameWnd);
		pFrameWnd->SetActiveView((CView*)pPane);
	}
	else
	{
		TRACE0("Warning: Next pane is not a view - calling SetFocus.\n");
		pPane->SetFocus();
	}
}

CWnd* CGXSplitterWnd::GetActivePane(int* pRow, int* pCol)
	// return active view, NULL when no active view
{
	ASSERT_VALID(this);

	// attempt to use active view of frame window
	CWnd* pView = NULL;
	CFrameWnd* pFrameWnd = GetParentFrame();
	ASSERT_VALID(pFrameWnd);
	pView = pFrameWnd->GetActiveView();

	// failing that, use the current focus
	if (pView == NULL)
		pView = GetFocus();

	// make sure the pane is a child pane of the splitter
	if (pView != NULL && !IsChildPane(pView, pRow, pCol))
		pView = NULL;

	return pView;
}

#endif

#endif // _MFC_VER

