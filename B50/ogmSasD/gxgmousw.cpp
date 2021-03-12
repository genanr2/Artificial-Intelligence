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

// gxgmousw.cpp : intelli mouse wheel support for grid
//

#include "stdafx.h"

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

#include "gxresrc.h"
#include "gxgmouse.h"

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CGXGridCore::EnableMouseWheel()
{
	ASSERT(m_pImousePlugin == NULL);
	// ASSERTION->
	// Warning: MouseWheelPlugin already registered.
	// Did you call both EnableMouseWheel() and EnableMouseWheel()
	// If yes, remove the call to EnableMouseWheel.
	// ->END

	AddPlugin(m_pImousePlugin = new CGXGridMouseWheelPlugin(this));
}

/////////////////////////////////////////////////////////////////////////////
// CGXGridMouseWheelPlugin

CGXGridMouseWheelPlugin::CGXGridMouseWheelPlugin(CGXGridCore* pGrid)
{
	m_pGrid = pGrid;
}

CGXGridMouseWheelPlugin::~CGXGridMouseWheelPlugin()
{
}


BEGIN_MESSAGE_MAP(CGXGridMouseWheelPlugin, CGXMouseWheelPlugin)
	//{{AFX_MSG_MAP(CGXGridMouseWheelPlugin)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGXGridMouseWheelPlugin message handlers

int CGXGridMouseWheelPlugin::OnIMouseGetZoom()
{
	if (m_pGrid)
		return m_pGrid->GetZoom();
	else
		return 0;
}

void CGXGridMouseWheelPlugin::OnIMouseSetZoom(int nZoom)
{
	if (m_pGrid)
		m_pGrid->SetZoom(nZoom);
}

