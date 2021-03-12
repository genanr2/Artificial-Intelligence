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

// gxcharro.cpp : implementation of arrow button child class
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

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CGXArrowButton child class

CGXArrowButton::CGXArrowButton(CGXControl* pComposite)
	: CGXButton(pComposite)
{
}

BOOL CGXArrowButton::MouseMove(UINT nFlags, CPoint pt)
{
	nFlags, pt;

	CGXButton::MouseMove(nFlags, pt);

	if (m_bPressed && GetRect().PtInRect(pt) && GetTickCount() >= dwLastTick+dwTicksNeeded)
	{
		// Increase speed
		if (dwTicksNeeded > 150)
			dwTicksNeeded = 150;
		else if (dwTicksNeeded > 50)
			dwTicksNeeded -= 5;

		dwLastTick = GetTickCount();
		m_pComposite->OnClickedButton(this);
	}

	return TRUE;
}

BOOL CGXArrowButton::LButtonDown(UINT nFlags, CPoint pt)
{
	nFlags, pt;

	CGXButton::LButtonDown(nFlags, pt);

	dwLastTick = GetTickCount();
	dwTicksNeeded = 500;

	m_pComposite->OnClickedButton(this);

	return TRUE;
}

BOOL CGXArrowButton::LButtonUp(UINT nFlags, CPoint pt)
{
	nFlags, pt;

	m_bPressed = FALSE;
	Refresh();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGXUpArrowButton child class

CGXUpArrowButton::CGXUpArrowButton(CGXControl* pComposite)
	: CGXArrowButton(pComposite)
{
}

void CGXUpArrowButton::Draw(CDC* pDC, BOOL bActive)
{
	if (!bActive)
		return;

	BOOL bPressed = bActive && HasFocus() && m_bPressed;

	CRect r = m_rect;

	COLORREF rgbFace = GXGetSysData()->GetSysColor(COLOR_BTNFACE);
	GXPatB(pDC, r, rgbFace);

	if (bPressed)
		GXDrawEdge(pDC, r, BDR_SUNKENINNER|BDR_SUNKENOUTER);
	else
		GXDrawEdge(pDC, r, BDR_RAISEDINNER|BDR_RAISEDOUTER);

	int x1, y1;

	if (r.Height() >= 8)
	{
		y1 = r.top + max((r.Height()-5) / 2, 0);
		x1 = r.left + max((r.Width()-7) / 2, 0);
	}
	else if (r.Height() >= 5)
	{
		y1 = r.top + max((r.Height()-3) / 2, 0);
		x1 = r.left + max((r.Width()-5) / 2, 0);
	}
	else
		return;
		 
	if (bPressed)
	{
		y1++;
		x1++;
	}

	x1 += 3;

	if (y1 < r.bottom) GXPatB(pDC, x1--, y1++, 1, 1, 0);
	if (y1 < r.bottom) GXPatB(pDC, x1--, y1++, 3, 1, 0);
	if (y1 < r.bottom) GXPatB(pDC, x1--, y1++, 5, 1, 0);
	if (r.Height() >= 8)
		if (y1 < r.bottom) GXPatB(pDC, x1--, y1++, 7, 1, 0);
}

/////////////////////////////////////////////////////////////////////////////
// CGXDownArrowButton child class

CGXDownArrowButton::CGXDownArrowButton(CGXControl* pComposite)
	: CGXArrowButton(pComposite)
{
}

void CGXDownArrowButton::Draw(CDC* pDC, BOOL bActive)
{
	if (!bActive)
		return;

	BOOL bPressed = bActive && HasFocus() && m_bPressed;

	CRect r = m_rect;

	COLORREF rgbFace = GXGetSysData()->GetSysColor(COLOR_BTNFACE);
	GXPatB(pDC, r, rgbFace);

	if (bPressed)
		GXDrawEdge(pDC, r, BDR_SUNKENINNER|BDR_SUNKENOUTER);
	else
		GXDrawEdge(pDC, r, BDR_RAISEDINNER|BDR_RAISEDOUTER);

	int x1, y1;

	if (r.Height() >= 8)
	{
		y1 = r.top + max((r.Height()-4) / 2, 0);
		x1 = r.left + max((r.Width()-7) / 2, 0);
	}
	else if (r.Height() >= 5)
	{
		y1 = r.top + max((r.Height()-2) / 2, 0);
		x1 = r.left + max((r.Width()-5) / 2, 0);
	}
	else
		return;
		 
	if (bPressed)
	{
		y1++;
		x1++;
	}

	if (y1 < r.bottom) GXPatB(pDC, x1++, y1++, 7, 1, 0);
	if (y1 < r.bottom) GXPatB(pDC, x1++, y1++, 5, 1, 0);
	if (y1 < r.bottom) GXPatB(pDC, x1++, y1++, 3, 1, 0);
	if (r.Height() >= 8)
		if (y1 < r.bottom) GXPatB(pDC, x1++, y1++, 1, 1, 0);
}
