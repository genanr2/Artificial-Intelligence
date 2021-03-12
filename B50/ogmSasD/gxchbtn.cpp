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

// gxchbtn.cpp : implementation of CGXButton child class
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
// CGXButton child class

CGXButton::CGXButton(CGXControl* pComposite)
	: CGXChild(pComposite)
{
	m_bPressed = FALSE;
	m_bMouseDown = FALSE;
	m_bCanPress = TRUE;
}

void CGXButton::Draw(CDC* pDC, BOOL bActive)
{
	WORD nState = 0;

	if (bActive && HasFocus())
	{
		nState |= GX_BTNFOCUS;

		if (m_bPressed)
			nState |= GX_BTNPRESSED;
	}

	COLORREF rgbFace = GXGetSysData()->GetSysColor(COLOR_BTNFACE);
	GXPatB(pDC, m_rect, rgbFace);

	if (nState & GX_BTNPRESSED)
		GXDrawEdge(pDC, m_rect, BDR_SUNKENOUTER | BDR_SUNKENINNER);
	else
		GXDrawEdge(pDC, m_rect, BDR_RAISEDOUTER | BDR_RAISEDINNER);

	CRect faceRect(m_rect.left+1, m_rect.top+1, m_rect.right-2, m_rect.bottom-2);
	if (nState & GX_BTNPRESSED)
		faceRect += CPoint(1,1);

	GXDrawFocusText(pDC, faceRect, nState&GX_BTNFOCUS, GetText(), DT_WORDBREAK);
}

BOOL CGXButton::LButtonDown(UINT nFlags, CPoint pt)
{
	nFlags, pt;

	m_bPressed = TRUE;
	m_bMouseDown = TRUE;
	if (m_bCanPress)
		Refresh();
	if (m_pComposite->IsActive())
		GridWnd()->UpdateWindow();

	return TRUE;
}

BOOL CGXButton::LButtonUp(UINT nFlags, CPoint pt)
{
	nFlags, pt;

	if (m_bPressed && GetRect().PtInRect(pt))
		m_pComposite->OnClickedButton(this);

	m_bPressed = FALSE;
	m_bMouseDown = FALSE;
	if (m_bCanPress)
		Refresh();

	return TRUE;
}

BOOL CGXButton::MouseMove(UINT nFlags, CPoint pt)
{
	nFlags, pt;

	BOOL bState = Intersects(pt);

	if (m_bMouseDown && bState != m_bPressed)
	{
		m_bPressed = bState;
		if (m_bCanPress)
			Refresh();
	}

	return TRUE;
}

BOOL CGXButton::OnGridKeyDown(UINT nChar, UINT nRepCnt, UINT flags)
{
	// unused:
	nRepCnt, flags;

	if (nChar == 32)
	{
		// Draw pressed
		m_bPressed = TRUE;
		if (m_bCanPress)
			Refresh();
		return TRUE;
	}

	return FALSE;
}

BOOL CGXButton::OnGridKeyUp(UINT nChar, UINT nRepCnt, UINT flags)
{
	// unused:
	nRepCnt, flags;

	if (nChar == 32 && m_bPressed)
	{
		// trigger event
		m_pComposite->OnClickedButton(this);

		// Draw normal
		m_bPressed = FALSE;
		if (m_bCanPress)
			Refresh();
		return TRUE;
	}

	return FALSE;
}

void CGXButton::OnGridCancelMode()
{
	if (m_bPressed || m_bMouseDown)
	{
		m_bPressed = FALSE;
		m_bMouseDown = FALSE;
		Refresh();
	}
}
