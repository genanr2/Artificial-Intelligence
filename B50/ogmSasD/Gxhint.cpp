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

// gxhint.cpp: implementation of CGXGridHint class

#include "stdafx.h"

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

// Headers

#ifndef _GXCORE_H_
#include "gxcore.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXHINT")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CGXGridHint, CObject)

#define new DEBUG_NEW


/////////////////////////////////////////////////////////////////////////////
// CGXGridHint

CGXGridHint::CGXGridHint(UINT id, int viewID)
{
	m_id = id;
	nViewID = viewID;
	nRow1 = nRow2 = nRow3 = 0;
	nCol1 = nCol2 = nCol3 = 0;
	lParam = 0;
	dwParam = 0;
	vptr = 0;
	flags = 0;
	nViewID = nViewID;
}

/////////////////////////////////////////////////////////////////////////////
// CGXGridHint diagnostics

#ifdef _DEBUG
void CGXGridHint::AssertValid() const
{
	CObject::AssertValid();
}
/*
void CGXGridHint::Dump(CDumpContext& dc) const
{
	AFX_DUMP1(dc, "\nnViewID  = ", nViewID);
	AFX_DUMP1(dc, "\nm_id     = ", m_id);
	AFX_DUMP1(dc, "\nnRow1    = ", nRow1);
	AFX_DUMP1(dc, "\nnRow2    = ", nRow2);
	AFX_DUMP1(dc, "\nnRow3    = ", nRow3);
	AFX_DUMP1(dc, "\nnCol1    = ", nCol1);
	AFX_DUMP1(dc, "\nnCol2    = ", nCol2);
	AFX_DUMP1(dc, "\nnCol3    = ", nCol3);
	AFX_DUMP1(dc, "\nlParam   = ", lParam);
	AFX_DUMP1(dc, "\ndwParam  = ", dwParam);
	AFX_DUMP1(dc, "\nflags    = ", flags);
	dc << "\nRange: " << range.top << "," << range.left << ",";
	dc << range.bottom << "," << range.right;
	if (pStyle)
	{
		dc << "\nStyle " << *pStyle;
		dc << "\nModifyType " << (UINT) mt;
	}

}
*/
#endif //_DEBUG
