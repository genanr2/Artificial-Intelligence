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

// gxcvedit.cpp : implementation of the CGXVScrollEdit control
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

#ifndef _GXCTRL_H_
#include "gxctrli.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCVEDIT")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_CONTROL(CGXVScrollEdit, CGXEditControl);
IMPLEMENT_DYNAMIC(CGXVScrollEdit, CGXEditControl);

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CGXVScrollEditControl control

CGXVScrollEdit::CGXVScrollEdit(CGXGridCore* pGrid, UINT nID)
	: CGXEditControl(pGrid, nID)
{
}

BEGIN_MESSAGE_MAP(CGXVScrollEdit, CGXEditControl)
	//{{AFX_MSG_MAP(CGXVScrollEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

DWORD CGXVScrollEdit::CalcEditStyle(LPRECT rectNP, int& dyHeight)
{
	SetVertScrollBar(TRUE);
	return CGXEditControl::CalcEditStyle(rectNP, dyHeight);
}

BOOL CGXVScrollEdit::CanFloatCell(ROWCOL, ROWCOL, const CGXStyle&, BOOL)
{
	// don't support floating
	return FALSE;
}
