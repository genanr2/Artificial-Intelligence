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

// gxautscr.cpp : Auto/Speed scrolling 
//

#include "stdafx.h"

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

#include "gxcore.h"
#include "gxautscr.h"
#include "gxmsg.h"

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Drag Scrolling
#define GX_DEFSCROLLSPEED		80  // slowest scroll speed
#define GX_DEFSCROLLACCEL		2    // Accelerate scroll speed
#define GX_DEFSCROLLBYACCEL		2
#define GX_DEFSCROLLBYMAX		80

// Mouse move messages
#define GX_DEFMOUSEMOVESPEED	100
#define GX_DEFMOUSEMOVEACCEL	3

// Speed scrolling when user clicked on scrollbar
#define GX_SPEEDSCROLLDIST		2	// Distance factor (emulates drag scroll)

// Speed scrolling when user clicked arrow key
#define GX_SPEEDKEYDIST			8	// Distance factor (emulates drag scroll)

void CGXGridCore::ImplementAutoScroll()
{
	if (m_pAutoScrollPlugin)
		return;

	// else
	AddPlugin(m_pAutoScrollPlugin = new CGXAutoScrollPlugin(this));
}

/////////////////////////////////////////////////////////////////////////////
// CGXAutoScrollPlugin

CGXAutoScrollPlugin::CGXAutoScrollPlugin(CGXGridCore* pGrid)
{
	GX_ZERO_INIT_OBJECT(CGXPluginComponent)

	m_pGrid = pGrid;
	m_pGrid->m_bAutoScroll = TRUE;
}

CGXAutoScrollPlugin::~CGXAutoScrollPlugin()
{
}


BEGIN_MESSAGE_MAP(CGXAutoScrollPlugin, CGXPluginComponent)
	//{{AFX_MSG_MAP(CGXAutoScrollPlugin)
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_CANCELMODE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGXAutoScrollPlugin message handlers

void CGXAutoScrollPlugin::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == GX_AUTOSCROLLTIMER)
	{
		if (m_pGrid->m_bAutoScrollStop)
		{
			KillAutoScroll();
			return;
		}

		if (m_pGrid->m_bAutoScrollStart)
		{
			// start autoscroll operation
			m_pGrid->m_bAutoScrollStart = FALSE;
				// will be checked and reset in autoscroll plugin component's
				// WM_TIMER messeage

			m_bScrollVert = m_pGrid->m_bAutoScrollVert;
			m_bScrollHorz = m_pGrid->m_bAutoScrollHorz; 
			m_nScrollDirection = m_pGrid->m_nAutoScrollDirection;
			m_nSpeedKeyChar = m_pGrid->m_nSpeedKeyChar;
			m_nSpeedKeyFlags = m_pGrid->m_nSpeedKeyFlags;
			m_nTimer = m_pGrid->m_nAutoScrollTimer;

			// Calculate scrolling area and store it in m_rectScroll
			m_rectScroll = m_pGrid->GetGridRect();      
			ROWCOL nFrozenRows = m_pGrid->GetFrozenRows();
			ROWCOL nFrozenCols = m_pGrid->GetFrozenCols();

			m_rectScroll.top += m_pGrid->CalcSumOfRowHeights(0, nFrozenRows);
			m_rectScroll.left += m_pGrid->CalcSumOfColWidths(0, nFrozenCols);

			m_dwCurrentTick = GetTickCount();
			m_nNextScrollTicks = GX_DEFSCROLLSPEED;
			m_nNextMouseMoveTicks = GX_DEFSCROLLSPEED;
			m_dwLastTick = 0;
			m_nScrollBy = 1;
			m_nLastDistance = 0;

			GetCursorPos(&m_ptOrgPos);
			ScreenToClient(&m_ptOrgPos);
		}

		DWORD dwTick = GetTickCount();

		CPoint ptCursorPos;
		GetCursorPos(&ptCursorPos);
		ScreenToClient(&ptCursorPos);

		// scroll if necessary
		if (dwTick - m_dwLastTick > (DWORD) m_nNextScrollTicks)
		{
			m_dwLastTick = dwTick;

			int nDistance = 0, nDirection = 0;
			BOOL bScrolled = FALSE;
			int nFact = 1;

			// Vertikal Scrolling

			// if mouse cursor is dragging outside the client area, scrolling occurs
			if (m_pGrid->m_bSpeedKey)
			{
				if (m_nNextScrollTicks < GX_DEFSCROLLSPEED/2)
					SendMessage(WM_KEYDOWN, m_nSpeedKeyChar, MAKELONG(max(1, m_nScrollBy/5), m_nSpeedKeyFlags)); 
				nDistance = GX_SPEEDKEYDIST;
				bScrolled = TRUE;
			}

			if (!bScrolled && m_nScrollDirection != -1)
			{
				if (m_nNextScrollTicks < GX_DEFSCROLLSPEED/2)
					m_pGrid->OnAutoScroll(nDirection = m_nScrollDirection, max(1, m_nScrollBy/10));
				nDistance = GX_SPEEDSCROLLDIST;
				bScrolled = TRUE;
			}

			if (!bScrolled && m_bScrollVert)
			{
				nFact = 5;

				if (ptCursorPos.y < m_rectScroll.top)
				{
					// Cursor is above client area
					nDistance = m_rectScroll.top - ptCursorPos.y;
					nDirection = GX_UP;
				}
				else if (ptCursorPos.y > m_rectScroll.bottom)
				{
					// Cursor is below client area
					nDistance = ptCursorPos.y - m_rectScroll.bottom;
					nDirection = GX_DOWN;
				}

				bScrolled = m_pGrid->OnAutoScroll(nDirection, max(1, m_nScrollBy/10));
			}

			// Horizontal Scrolling

			if (!bScrolled && m_bScrollHorz)
			{
				nFact = 10;

				if (ptCursorPos.x < m_rectScroll.left)
				{
					// Cursor is on the left of the client area
					nDistance = m_rectScroll.left-ptCursorPos.x;
					nDirection = GX_LEFT;
				}
				else if (ptCursorPos.x > m_rectScroll.right)
				{
					// Cursor is on the right of the client area
					nDistance = ptCursorPos.x - m_rectScroll.right;
					nDirection = GX_RIGHT;
				}

				bScrolled = m_pGrid->OnAutoScroll(nDirection, max(1, m_nScrollBy/10));
			}


			if (!bScrolled || nDistance < m_nLastDistance)
			{
				m_nNextScrollTicks = GX_DEFSCROLLSPEED;
				m_nScrollBy = 1;
			}
			else if (nDistance > 1)
			{
				m_nNextScrollTicks -= nDistance/nFact/GX_DEFSCROLLACCEL;

				if (m_nNextScrollTicks < 0)
				{
					m_nNextScrollTicks = 0;
					m_nScrollBy += GX_DEFSCROLLBYACCEL;
					m_nScrollBy = min(m_nScrollBy, GX_DEFSCROLLBYMAX);
				}
			}

			m_nLastDistance = nDistance;

			// peridically do OnMouseMove
			if (m_nScrollDirection == -1)
				SendMessage(WM_MOUSEMOVE, 0, MAKELONG(ptCursorPos.x, ptCursorPos.y));
		}
		else if (abs(ptCursorPos.x-m_ptOrgPos.x) + abs(ptCursorPos.y-m_ptOrgPos.y) > 5)
		{
			// if the user drags the mouse immediately
			if (m_nScrollDirection == -1 && !m_pGrid->m_bSpeedKey)
				SendMessage(WM_MOUSEMOVE, 0, MAKELONG(ptCursorPos.x, ptCursorPos.y));

			// reset frequency of mouse move message 
			m_nNextMouseMoveTicks = GX_DEFMOUSEMOVESPEED;
		}
		else if (dwTick - m_dwLastTick > (DWORD) m_nNextMouseMoveTicks)
		{
			if (m_nScrollDirection == -1 && !m_pGrid->m_bSpeedKey)
				SendMessage(WM_MOUSEMOVE, 0, MAKELONG(ptCursorPos.x, ptCursorPos.y));
			m_nNextMouseMoveTicks -= GX_DEFMOUSEMOVEACCEL;
			// increase frequency of mouse move message (usefull for spin buttons etc. )
		}
	}
}

void CGXAutoScrollPlugin::OnLButtonUp(UINT , CPoint ) 
{
	KillAutoScroll();
}

void CGXAutoScrollPlugin::OnMButtonUp(UINT , CPoint ) 
{
	KillAutoScroll();
}

void CGXAutoScrollPlugin::OnRButtonUp(UINT , CPoint ) 
{
	KillAutoScroll();
}

void CGXAutoScrollPlugin::OnCancelMode() 
{
	KillAutoScroll();
}

void CGXAutoScrollPlugin::OnKeyDown(UINT nChar, UINT , UINT ) 
{
	switch (nChar)
	{
	case VK_ESCAPE:
		KillAutoScroll();
		break;
	default:
		if (nChar != m_pGrid->m_nSpeedKeyChar)
			KillAutoScroll();
		break;
	}
}

void CGXAutoScrollPlugin::OnKeyUp(UINT , UINT , UINT ) 
{
	KillAutoScroll();
}

void CGXAutoScrollPlugin::KillAutoScroll()
{
	if (m_nTimer)
	{
		// Parent window will call ReleaseCapture
		KillTimer(m_nTimer);
		m_nTimer = 0;
	}
	m_pGrid->m_nAutoScrollDirection = -1;
	m_pGrid->m_bSpeedKey = FALSE;
	m_pGrid->m_bAutoScrollStop = TRUE;
}



void CGXAutoScrollPlugin::OnKillFocus(CWnd* ) 
{
	if (!m_pGrid->m_bIgnoreFocus)
		KillAutoScroll();
}
