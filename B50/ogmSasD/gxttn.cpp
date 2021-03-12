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
// Authors: Daniel Jebaraj, Stefan Hoenig
//

// gxttn.cpp : implementation of Tooltip support in CGXGridCore class
//

#include "stdafx.h"

// compile this file only if OLE is supported
#if _MFC_VER >= 0x0400

// Headers

#ifndef _GXCORE_H_
#include "gxcore.h"
#endif

#ifndef _GXTTN_H_
#include "gxttn.h"
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXTTN")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Implementation of CGXToolTipCtrlProxy

CGXToolTipCtrlProxy::CGXToolTipCtrlProxy(CGXGridCore* pGrid, CWnd* pParent)
{
	m_pGrid = pGrid;
	m_pParent = pParent;
	m_nTtnRowtip = GX_INVALID;
	m_nTtnColtip = GX_INVALID;
}

CGXToolTipCtrlProxy::~CGXToolTipCtrlProxy()
{
}

void CGXToolTipCtrlProxy::RelayEvent(LPMSG lpMsg)
{
	if (m_wndTtnToolTip.m_hWnd)
		m_wndTtnToolTip.RelayEvent(lpMsg);
}

void CGXToolTipCtrlProxy::UpdateTipArea()
{
	if (m_wndTtnToolTip.m_hWnd)
	{
		m_wndTtnToolTip.SetToolRect(m_pParent, GX_IDC_TIPBUTTON, m_pGrid->GetGridRect());

		Reset();
	}
}

void CGXToolTipCtrlProxy::Reset()
{
	// Reset m_nTtnRowtip and m_nTtnColtip so that tooltip will
	// be drawn correctly with next MouseMove message
	m_nTtnRowtip = GX_INVALID;
	m_nTtnColtip = GX_INVALID;
}

BOOL CGXToolTipCtrlProxy::InitToolTip()
{
	if (m_pGrid && m_pGrid->m_pGridWnd)
		m_pParent = m_pGrid->m_pGridWnd;

	if (m_btnTtnHelper.m_hWnd != 0)
		return TRUE;
	
	// Create an invisible pushbutton
	CRect rect(0,0,0,0);
	if (!m_btnTtnHelper.Create(_T(""), WS_CHILD|WS_BORDER|BS_PUSHBUTTON, rect, m_pParent, GX_IDC_TIPBUTTON))
	{
		TRACE0("Warning! Failed to create CToolTipCtrl\n");
		TRACE0("Tooltips will not work.\n");
		return FALSE;
	}

	// Create the tooltip
	CRect recttiparea = m_pGrid->GetGridRect(); // get grid client area

	if ( !(
		m_wndTtnToolTip.Create(m_pParent, TTS_ALWAYSTIP)
		&& m_wndTtnToolTip.AddTool(m_pParent, LPSTR_TEXTCALLBACK, &recttiparea, GX_IDC_TIPBUTTON)
		))
	{
		TRACE0("Warning! Failed to create tooltip in EnableGridToolTips\n");
		TRACE0("Tooltips will not work.\n");
		return FALSE;
	}

	m_wndTtnToolTip.Activate(FALSE);

	return TRUE;
}

BOOL CGXToolTipCtrlProxy::OnMouseMove(UINT flags, CPoint pt)
{
	// Unused:
	flags;

	ROWCOL nRow, nCol;

	int ht = m_pGrid->HitTest(pt, &nRow, &nCol);

	CGXRange range;
	if (m_pGrid->GetCellRangeRowCol(nRow, nCol, range))
	{
		nRow = range.top;
		nCol = range.left;
	}

	if (ht != GX_CELLHIT && ht != GX_HEADERHIT && ht != GX_NOHIT
		|| m_nTtnRowtip == nRow && m_nTtnColtip == nCol)
		return FALSE;

	if (ht == GX_NOHIT)
		nRow = nCol = GX_INVALID;

	m_nTtnRowtip = nRow;
	m_nTtnColtip = nCol;
	m_strTtnText.Empty();

	if (nRow != GX_INVALID && nCol != GX_INVALID)
	{
		// Get the style info for the cell
		CGXStyle* pStyle = m_pGrid->CreateStyle();

		m_pGrid->m_bNoValueNeeded = TRUE;
		m_pGrid->ComposeStyleRowCol(nRow, nCol, pStyle);
		m_pGrid->m_bNoValueNeeded = FALSE;

		// and especially the tooltip-text
		pStyle->GetUserAttribute(GX_IDS_UA_TOOLTIPTEXT, m_strTtnText);

		// Release the style info again
		m_pGrid->RecycleStyle(pStyle);
	}

	if (m_strTtnText.IsEmpty())
	{
		if (m_wndTtnToolTip.m_hWnd)
			m_wndTtnToolTip.Activate(FALSE);
	}
	else
	{
		InitToolTip();		

		m_wndTtnToolTip.Activate(TRUE);
		m_wndTtnToolTip.UpdateTipText(m_strTtnText, m_pParent, GX_IDC_TIPBUTTON);

		m_pParent->UpdateWindow();
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGXGridCore
//

void CGXGridCore::ImplementToolTips()
{ 
	InitCommonControls();

	if (m_pTooltipProxy == NULL)
		m_pTooltipProxy = new CGXToolTipCtrlProxy(this, m_pGridWnd);

	m_bTtnIsEnabled = TRUE;
}

BOOL CGXGridCore::EnableGridToolTips(BOOL b)
{ 
	if (b)
	{
		ImplementToolTips();
	}
	else
		m_bTtnIsEnabled = FALSE;
	
	return TRUE;
}

#endif // _MFC_VER >= 0x0400
