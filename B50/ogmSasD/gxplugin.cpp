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

// gxplugin.cpp : implementation of the CGXPluginComponent class
//

#include "stdafx.h"

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

#include "gxresrc.h"

#include "gxplugin.h"

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CGXPluginComponent, CWnd);

CGXPluginComponent::CGXPluginComponent()
{
	m_bSkipOtherPlugins = FALSE;
	m_bExitMesssage = FALSE;
}

CGXPluginComponent::~CGXPluginComponent()
{
	// make sure Detach won't get called
	m_hWnd = NULL;
}

BEGIN_MESSAGE_MAP(CGXPluginComponent, CWnd)
	//{{AFX_MSG_MAP(CGXPluginComponent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CGXPluginComponent::PlugIn(CWnd* pParentWnd)
{
	m_hWnd = pParentWnd->GetSafeHwnd();

	return TRUE;
}

BOOL CGXPluginComponent::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	m_bSkipOtherPlugins = FALSE;
	m_bExitMesssage = FALSE;

#if _MFC_VER >= 0x0400
	return CWnd::OnWndMsg(message, wParam, lParam, pResult);
#else
	*pResult = CWnd::WindowProc(message, wParam, lParam);
	return TRUE;
#endif
}

LRESULT CGXPluginComponent::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// do nothing - this makes sure that calls to Default() 
	// will have no effect (and thus make sure that the same
	// message is not process twice).

	// Unused:
	message, wParam, lParam;

	return 0;
}

#ifdef _DEBUG
void CGXPluginComponent::AssertValid() const
{
	if (m_hWnd == NULL)
		return;     // null (unattached) windows are valid

	// should be a normal window
	ASSERT(::IsWindow(m_hWnd));

	// Regular CWnd's check the permanent or temporary handle map
	// and compare the pointer to this.
	// This will fail for a CGXPluginComponent because several
	// Plugin objects share the same HWND. Therefore we must not
	// call CWnd::AsssertValid.
}
/*
void CGXPluginComponent::Dump(CDumpContext& dc) const
{
//	dc << "PluginComponent";
	// It is safe to call CWnd::Dump
//	CWnd::Dump(dc);
}
*/
#endif
