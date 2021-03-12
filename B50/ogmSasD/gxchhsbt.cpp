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

// gxchhsbt.cpp : implementation of CGXHotSpotButton child class
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
// CGXHotSpotButton child class

CGXHotSpotButton::CGXHotSpotButton(CGXControl* pComposite)
	: CGXButton(pComposite)
{
}

void CGXHotSpotButton::Draw(CDC* pDC, BOOL bActive)
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

	if (bPressed)
	{
		r.top++;
		r.left++;
	}

	// HotSpot-Pushbutton
	if (r.Height() >= 12)
	{
		GXPatB(pDC, r.left+3, r.top+3, 6, 2, 0);
		GXPatB(pDC, r.left+6, r.top+4, 3, 3, 0);
		GXPatB(pDC, r.left+4, r.top+7, 7, 1, 0);
		GXPatB(pDC, r.left+5, r.top+8, 5, 1, 0);
		GXPatB(pDC, r.left+6, r.top+9, 3, 1, 0);
		GXPatB(pDC, r.left+7, r.top+10, 1, 1, 0);
	}
}
