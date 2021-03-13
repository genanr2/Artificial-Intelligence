#include "stdafx.h"
#include "b54mdll.h"
#include "ListButton.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// ListButton
ListButton::ListButton(){}
ListButton::~ListButton(){}
BEGIN_MESSAGE_MAP(ListButton, CButton)
	//{{AFX_MSG_MAP(ListButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// ListButton message handlers
void ListButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CButton::OnLButtonDown(nFlags, point);
}
void ListButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CButton::OnLButtonDblClk(nFlags, point);
}
void ListButton::OnSize(UINT nType, int cx, int cy) 
{
	CButton::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
}
void ListButton::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CButton::OnPaint() for painting messages
}
void ListButton::OnSysColorChange() 
{
	CButton::OnSysColorChange();
	// TODO: Add your message handler code here
}
HBRUSH ListButton::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CButton::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO: Change any attributes of the DC here
	// TODO: Return a different brush if the default is not desired
	return hbr;
}
void ListButton::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	CButton::PreSubclassWindow();
	ModifyStyle(0,BS_OWNERDRAW);
	CRect rect;
	GetClientRect(rect);
	// Resize the window to make it square
//	rect.bottom=rect.right=min(rect.bottom,rect.right);
	// Get the vital statistics of the window
	m_ptCentre=rect.CenterPoint();
	m_nRadius=rect.bottom/2-1;
	// Set the window region so mouse clicks only activate the round section of the button
	m_rgn.DeleteObject(); 
	SetWindowRgn(NULL, FALSE);
	m_rgn.CreateEllipticRgnIndirect(rect);
	SetWindowRgn(m_rgn, TRUE);
	// Convert client coords to the parents client coords
	ClientToScreen(rect);
	CWnd* pParent = GetParent();
	if (pParent) pParent->ScreenToClient(rect);
	// Resize the window
	MoveWindow(rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
//	CButton::PreSubclassWindow();
}
void ListButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	ASSERT(lpDrawItemStruct!=NULL);
	CDC*pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rect=lpDrawItemStruct->rcItem;
	UINT state=lpDrawItemStruct->itemState;
	UINT nStyle=GetStyle();
	int nRadius=m_nRadius;
	CBrush*br = new TBrush(dc->GetBkColor());/////////////
	int nSavedDC=pDC->SaveDC();
	pDC->SelectStockObject(NULL_BRUSH);
	pDC->FillSolidRect(rect, ::GetSysColor(COLOR_BTNFACE));
	// Draw the focus circle around the button
	if((state&ODS_FOCUS)&&m_bDrawDashedFocusCircle)DrawCircle(pDC,m_ptCentre,nRadius--,RGB(0,0,0));
	// Draw the raised/sunken edges of the button (unless flat)
	if (nStyle & BS_FLAT) 
	{
		DrawCircle(pDC, m_ptCentre, nRadius--, RGB(0,0,0));
		DrawCircle(pDC, m_ptCentre, nRadius--, ::GetSysColor(COLOR_3DHIGHLIGHT));
	} 
	else 
	{
		if((state&ODS_SELECTED)){DrawCircle(pDC, m_ptCentre, nRadius--, 
					   ::GetSysColor(COLOR_3DDKSHADOW), ::GetSysColor(COLOR_3DHIGHLIGHT));
			DrawCircle(pDC,m_ptCentre,nRadius--,::GetSysColor(COLOR_3DSHADOW),::GetSysColor(COLOR_3DLIGHT));
		} 
		else 
		{
			DrawCircle(pDC,m_ptCentre, nRadius--,::GetSysColor(COLOR_3DHIGHLIGHT),::GetSysColor(COLOR_3DDKSHADOW));
			DrawCircle(pDC,m_ptCentre, nRadius--,::GetSysColor(COLOR_3DLIGHT),::GetSysColor(COLOR_3DSHADOW));
		}
	}
	// draw the text if there is any
	CString strText;
	GetWindowText(strText);
	if (!strText.IsEmpty())
	{
		CRgn rgn;
		rgn.CreateEllipticRgn(m_ptCentre.x-nRadius,m_ptCentre.y-nRadius,m_ptCentre.x+nRadius, m_ptCentre.y+nRadius);
		pDC->SelectClipRgn(&rgn);
		CSize Extent = pDC->GetTextExtent(strText);
		CPoint pt = CPoint( m_ptCentre.x - Extent.cx/2, m_ptCentre.x - Extent.cy/2 );
		if (state & ODS_SELECTED) pt.Offset(1,1);
		pDC->SetBkMode(TRANSPARENT);
		if(state&ODS_DISABLED)pDC->DrawState(pt,Extent,strText,DSS_DISABLED,TRUE,0,(HBRUSH)NULL);
		else pDC->TextOut(pt.x, pt.y, strText);
		pDC->SelectClipRgn(NULL);rgn.DeleteObject();
	}
	// Draw the focus circle on the inside of the button
	if((state&ODS_FOCUS)&&m_bDrawDashedFocusCircle)DrawCircle(pDC,m_ptCentre,nRadius-2,RGB(0,0,0),TRUE);
	pDC->RestoreDC(nSavedDC);
}
void ListButton::SetButtonState( UINT)
{
	TCurrentEvent& Msg = GetCurrentEvent();
	int not = LOWORD(Msg.LParam);
	switch(not)
	{
	 case TBUTTON_DARK_STATE:  light = DARK; InvalidateRgn(NULL); break;
	 case TBUTTON_LIGHT_STATE: light = LIGHT; InvalidateRgn(NULL); break;
	 case TBUTTON_GRAY_STATE:  light = GRAY; InvalidateRgn(NULL); break;
	 default: break;
	}
}
