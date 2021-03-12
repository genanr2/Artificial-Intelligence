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

// gxviewt.cpp : CGXTabWnd support for CGXView class
//

#include "stdafx.h"

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

// Headers

#ifndef _GXVIEW_H_
#include "gxview.h"
#endif

#ifndef _GXTWND_H_
#include "gxtwnd.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXVIEWT")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW


CString CGXView::GetTabName()
{
	CString s;
	CGXTabWnd* pTabWnd = GetParentTabWnd(this, TRUE);
	if (pTabWnd != NULL)
		s = pTabWnd->GetTabName(this);
	return s;
}

void CGXView::SetTabName(CString s)
{
	CGXTabWnd* pTabWnd = GetParentTabWnd(this, TRUE);
	if (pTabWnd != NULL)
	{
		ASSERT(pTabWnd && pTabWnd->IsKindOf(RUNTIME_CLASS(CGXTabWnd)));
		pTabWnd->SetTabName(this, s);
	}
}

