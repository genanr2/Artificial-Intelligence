#if !defined(AFX_LISTBUTTON_H__F73936A2_53DF_11D2_B0AD_00A024384FCF__INCLUDED_)
#define AFX_LISTBUTTON_H__F73936A2_53DF_11D2_B0AD_00A024384FCF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ListButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ListButton window

class ListButton : public CButton
{
// Construction
public:
	ListButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ListButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~ListButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(ListButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSysColorChange();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTBUTTON_H__F73936A2_53DF_11D2_B0AD_00A024384FCF__INCLUDED_)
