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

// gxwait.cpp : implementation of the CGXLongOperation class
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

#ifndef _AFXPRIV_H_
#include "afxpriv.h"
#endif

#ifndef _GXEXT_H_
#include "gxext.h"
#endif

#ifndef _GXSTATE_H_
#include "gxstate.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXWAIT")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

#ifndef _GXDLL
GX_WAIT_STATE GXNEAR _gxWaitState;
#endif

/////////////////////////////////////////////////////////////////////////////
// GX_WAIT_STATE

GX_WAIT_STATE::GX_WAIT_STATE()
{
	m_dwTicksFirstTime = 750;
	m_dwTicksContinued = 1500;

	m_bAbortDialog = TRUE;
	m_bWaitCursor = FALSE;
	m_bCanceled = FALSE;
	m_bUserConfirmed = FALSE;
	m_bRollbackConfirmed = FALSE;

	m_bCanAbort = TRUE;
	m_bCanRollback = TRUE;

	m_nRefCount = 0;
	m_nLevelLocked = -1;

	m_nPercentDone = -1;

	memset((void*) m_apszStatusText, 0, sizeof(m_apszStatusText));
	memset(m_abOwnStatusText, 0, sizeof(m_abOwnStatusText));

	m_nIndicatorState = 0;
	_tcscpy(m_szStatusBuf, _T(""));

	m_hFrameWnd = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CGXLongOperation

CGXLongOperation::CGXLongOperation()
{
	GX_WAIT_STATE* pWaitState = GXGetWaitState();

	if (GXGetAppData()->m_pLongOperationImp)
		GXGetAppData()->m_pLongOperationImp->Start(this);
	else
		pWaitState->m_nRefCount++;
}

CGXLongOperation::~CGXLongOperation()
{
	GX_WAIT_STATE* pWaitState = GXGetWaitState();

	if (GXGetAppData()->m_pLongOperationImp)
		GXGetAppData()->m_pLongOperationImp->Stop(this);
	else
		--pWaitState->m_nRefCount;
}

void CGXLongOperation::FreeStatusText()
{
	if (GXGetAppData()->m_pLongOperationImp)
		GXGetAppData()->m_pLongOperationImp->FreeStatusText(this);
}

void CGXLongOperation::SetStatusText(LPCTSTR pszText, BOOL bCopyBuffer)
{
	if (GXGetAppData()->m_pLongOperationImp)
		GXGetAppData()->m_pLongOperationImp->SetStatusText(this, pszText, bCopyBuffer);
}

BOOL CGXLongOperation::DoMessages()
{
	if (GXGetAppData()->m_pLongOperationImp)
		return GXGetAppData()->m_pLongOperationImp->DoMessages(this);

	return FALSE;
}

BOOL CGXLongOperation::DoMessages(BOOL& bAbort)
{
	bAbort = FALSE;

	if (GXGetAppData()->m_pLongOperationImp)
		return GXGetAppData()->m_pLongOperationImp->DoMessages(this, bAbort);

	return FALSE;
}

// Implementation

void CGXLongOperation::ProcessMessages()
{
	if (GXGetAppData()->m_pLongOperationImp)
		GXGetAppData()->m_pLongOperationImp->ProcessMessages(this);
}

