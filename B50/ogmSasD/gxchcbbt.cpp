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

// gxchcbbt.cpp : implementation of CGXComboBoxButton child class
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
// CGXComboBoxButton child class

CGXComboBoxButton::CGXComboBoxButton(CGXControl* pComposite)
	: CGXButton(pComposite)
{
}

void CGXComboBoxButton::Draw(CDC* pDC, BOOL bActive)
{
	BOOL bDrawInactiv = FALSE;

	// determine if button shall only be drawn when control is active
	CGXComboBox* pComboBox = (CGXComboBox*) m_pComposite;
	if (pComboBox->CGXControl::IsKindOf(CONTROL_CLASS(CGXComboBox)))
		bDrawInactiv = pComboBox->m_bInactiveDrawButton;

	if (!bActive && !bDrawInactiv)
		return;

	// draw the button
	BOOL bPressed = bActive && HasFocus() && m_bPressed;

	CRect r = m_rect;

	// Draw the button borders

	// Fill the face of the button (prior to loading bitmap)
	GXPatB(pDC, r, GXGetSysData()->clrBtnFace);

	// Windows 95-look
	COLORREF rgbFace = GXGetSysData()->GetSysColor(COLOR_BTNFACE);
	GXPatB(pDC, r, rgbFace);

	if (bPressed)
		GXDrawEdge(pDC, r, BDR_SUNKENINNER|BDR_SUNKENOUTER);
	else
		GXDrawEdge(pDC, r, BDR_RAISEDINNER|BDR_RAISEDOUTER);

	r.top += 2;
	r.bottom -= 2;

	// Now draw the bitmap centered on the face
	CDC memDC;
	if(memDC.CreateCompatibleDC(pDC))
	{
		CBitmap& bmp = GXGetSysData()->m_bmpDownArrow;

		BITMAP bm;
		VERIFY(bmp.GetObject(sizeof(bm), &bm) == sizeof(bm));

		int nLeft = max(0, (r.Width() - bm.bmWidth)/2) + r.left;
		int nTop  = max(0, (r.Height() - bm.bmHeight)/2) + r.top;

		if(bPressed)
			// The button is currently pressed, so offset bitmap to give
			// impression of movement.
			nLeft++, nTop++;

		CBitmap *pOldBmp = memDC.SelectObject(&bmp);
		pDC->BitBlt(nLeft, nTop, min(bm.bmWidth, r.Width()-1), min(bm.bmHeight, r.Height()-1), &memDC, 0, 0, SRCCOPY);
		memDC.SelectObject(pOldBmp);
	}
}

