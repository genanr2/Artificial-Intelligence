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

// gxcoret.cpp : CGXTabWnd support for CGXGridCore
//

#include "stdafx.h"

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

// Headers

#ifndef _GXRESRC_H_
#include "gxresrc.h"
#endif

#ifndef _GXCORE_H_
#include "gxcore.h"
#endif

#ifndef _GXTWND_H_
#include "gxtwnd.h"
#endif


#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCORET")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

CGXTabWnd* CGXGridCore::GetTabWnd() const
{
	return (CGXTabWnd*) GXGetParentWnd(m_pGridWnd, RUNTIME_CLASS(CGXTabWnd), TRUE);
}

CString CGXGridCore::GetTabName()
{
	CString s;

	CWnd* pWnd = m_pGridWnd;
	while ((pWnd = pWnd->GetParent()) != NULL &&
		!pWnd->IsKindOf(RUNTIME_CLASS(CGXTabWnd)))
		;

	if (pWnd)
	{
		CGXTabWnd* pTabWnd = (CGXTabWnd*) pWnd;
		ASSERT(pTabWnd && pTabWnd->IsKindOf(RUNTIME_CLASS(CGXTabWnd)));

		s = pTabWnd->GetTabName(m_pGridWnd);
	}

	return s;
}

void CGXGridCore::SetTabName(CString s)
{
	CWnd* pWnd = m_pGridWnd;
	while ((pWnd = pWnd->GetParent()) != NULL &&
		!pWnd->IsKindOf(RUNTIME_CLASS(CGXTabWnd)))
		;

	if (pWnd)
	{
		CGXTabWnd* pTabWnd = (CGXTabWnd*) pWnd;
		ASSERT(pTabWnd && pTabWnd->IsKindOf(RUNTIME_CLASS(CGXTabWnd)));

		pTabWnd->SetTabName(m_pGridWnd, s);
	}
}

