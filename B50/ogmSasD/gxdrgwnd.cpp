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

// gxdrgwnd.cpp : CGXDragLineWnd window
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

#ifndef _GXPRIV_H_
#include "gxpriv.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCORDIM")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


#define new DEBUG_NEW


/////////////////////////////////////////////////////////////////////////////
// CGXDragLineWnd

CGXDragLineWnd::CGXDragLineWnd(CGXGridCore* pGrid)
{
	m_pGrid = pGrid;
	m_flags = 0;
	m_type = 0;
	m_nClientRC = 0;
}

BOOL CGXDragLineWnd::CreateWnd(CWnd* pParent)
{
	if (!CreateEx(
			0, GXGetAppData()->sComboClassName, NULL, WS_CHILD,
			0, 0, 1, 1,
			pParent->GetSafeHwnd(), NULL))
	{
		TRACE0("Failed to create window in CreateWnd\n");
		ASSERT(0);
		// ASSERTION-> Failed to create window
		// Did you register the windowclass with CGXGridCombo::RegisterClass? ->END

		return FALSE;
	}

	return TRUE;
};

BEGIN_MESSAGE_MAP(CGXDragLineWnd, CWnd)
	//{{AFX_MSG_MAP(CGXDragLineWnd)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CGXDragLineWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rect;
	GetClientRect(rect);

	if (m_flags == 0)
	{
		CPen pen;
		pen.CreatePenIndirect(m_pen);
		CPen* pOldPen = dc.SelectObject(&pen);

		dc.MoveTo(rect.left, rect.top);
		dc.LineTo(rect.right-1, rect.bottom-1);

		dc.SelectObject(pOldPen);
	}
	else
	{
		m_pGrid->DrawGridLine(&dc, rect.left, rect.top, rect.Width(), rect.Height(), 
			m_type, m_nClientRC, m_flags);
	}
}

