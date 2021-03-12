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

// gxgmouse.cpp : intelli mouse panning and dragging for grid
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

void CGXGridCore::EnableIntelliMouse()
{
	ASSERT(m_pImousePlugin == NULL);
	// ASSERTION->
	// Warning: IntelliMousePlugin already registered.
	// Did you call both EnableIntelliMouse() and EnableMouseWheel()
	// If yes, remove the call to EnableMouseWheel.
	// ->END

	AddPlugin(m_pImousePlugin = new CGXGridIntelliMousePlugin(this));
}

/////////////////////////////////////////////////////////////////////////////
// CGXGridIntelliMousePlugin

CGXGridIntelliMousePlugin::CGXGridIntelliMousePlugin(CGXGridCore* pGrid)
{
	m_pGrid = pGrid;
}

CGXGridIntelliMousePlugin::~CGXGridIntelliMousePlugin()
{
}


BEGIN_MESSAGE_MAP(CGXGridIntelliMousePlugin, CGXIntelliMousePlugin)
	//{{AFX_MSG_MAP(CGXGridIntelliMousePlugin)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGXGridIntelliMousePlugin message handlers

int CGXGridIntelliMousePlugin::OnIMouseGetZoom()
{
	if (m_pGrid)
		return m_pGrid->GetZoom();
	else
		return 0;
}

void CGXGridIntelliMousePlugin::OnIMouseSetZoom(int nZoom)
{
	if (m_pGrid)
		m_pGrid->SetZoom(nZoom);
}

void CGXGridIntelliMousePlugin::OnIMouseVScroll(UINT nSBCode, UINT nScroll)
{
	BOOL b = m_pGrid->m_bDoScrollNoUpdateWindow;
	m_pGrid->OnAutoScroll(nSBCode == SB_LINEUP ? GX_UP : GX_DOWN, nScroll);
	m_pGrid->m_bDoScrollNoUpdateWindow = b;
}

void CGXGridIntelliMousePlugin::OnIMouseHScroll(UINT nSBCode, UINT nScroll)
{
	BOOL b = m_pGrid->m_bDoScrollNoUpdateWindow;
	m_pGrid->OnAutoScroll(nSBCode == SB_LINEUP ? GX_LEFT : GX_RIGHT, nScroll);
	m_pGrid->m_bDoScrollNoUpdateWindow = b;
}
