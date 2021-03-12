// Author: Stefan Hoenig
// gxgscrltp.cpp : implementation of the CGXScrollTipPlugin class
#include "stdafx.h"
#if _MFC_VER >= 0x0400
#include "gxscrltp.h"
#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXSCRLTP")
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CGXScrollTipPlugin
CGXScrollTipPlugin::CGXScrollTipPlugin(CGXGridCore* pGrid)
{
	m_pScrollTip = NULL;m_pGrid = pGrid;
}
CGXScrollTipPlugin::~CGXScrollTipPlugin(){delete m_pScrollTip;}
BEGIN_MESSAGE_MAP(CGXScrollTipPlugin, CGXPluginComponent)
	//{{AFX_MSG_MAP(CGXScrollTipPlugin)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CGXScrollTipPlugin message handlers
void CGXScrollTipPlugin::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	switch(nSBCode)
	{
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		if(m_pScrollTip==NULL)m_pScrollTip=new CGXScrollTip;
		if(!GXGetAppData()->bWin31)
		{
			// ScrollTips
			ROWCOL nRow = nPos;
			if(m_pGrid)nRow*=m_pGrid->m_nHScrollFactor;
			TCHAR szText[256];
			wsprintf(szText,_T("Строка: %u"),nRow);
			m_pScrollTip->HandleScrollMessage(this,nSBCode,SB_VERT,szText,pScrollBar);
		}
	}
}
void CGXScrollTipPlugin::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	switch(nSBCode)
	{
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		if(m_pScrollTip==NULL)m_pScrollTip=new CGXScrollTip;
		if (!GXGetAppData()->bWin31)
		{
			// ScrollTips
			ROWCOL nCol = nPos;
			if (m_pGrid)nCol *= m_pGrid->m_nHScrollFactor;
			TCHAR szText[256];
			wsprintf(szText, _T("Столбец: %u"), nCol);
			m_pScrollTip->HandleScrollMessage(this, nSBCode, SB_HORZ, szText, pScrollBar);
		}
	}
}
void CGXGridCore::EnableScrollTips()
{
	ASSERT(m_pScrollTipPlugin == NULL);
	// ASSERTION-> EnableScrollTips called more than once.->END
	AddPlugin(m_pScrollTipPlugin = new CGXScrollTipPlugin(this));
}
#else
void CGXGridCore::EnableScrollTips()
{
	TRACE("Warning: Call to ImplementScrollTips() ignored. Not supported for Win16.\n");
}
#endif
