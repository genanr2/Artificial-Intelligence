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

// gxccbbox.cpp : implementation of the CGXComboBox control
//

#include "stdafx.h"

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

// Headers

#ifndef _GXCORE_H_
#include "gxcore.h"
#endif

#ifndef _GXCTRLI_H_
#include "gxctrli.h"
#endif

#ifndef _GXMSG_H_
#include "gxmsg.h"
#endif

#include <ctype.h>

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCCBBOX")
#endif

IMPLEMENT_CONTROL(CGXComboBox, CGXEditControl);
IMPLEMENT_DYNAMIC(CGXComboBox, CGXEditControl);
IMPLEMENT_DYNAMIC(CGXComboListBox, CListBox);

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CGXComboBox control

// when the user presses the dropdown button,
// a CGXGridCombo window is displayed with WS_POPUP style
// and a CGXComboListBox window is embedded into the CGXGridCombo window.
//
// CGXComboListBox sends messages WM_GX_LBOXEND, WM_GX_LBOXCANCEL
// and WM_GX_LBOXCHANGED to the CGXComboBox control.
//
// When CGXComboBox receives a WM_GX_LBOXEND message, the current
// selection is stored and the listbox is destroyed.
//
// When CGXComboBox receives a WM_GX_LBOXCHANGE, the edit text is
// updated.
//
// When CGXComboBox receives a WM_GX_LBOXCANCEL, the listbox will
// be destroyed.
//

// static helpers

// CGXComboListBox

CGXComboListBox::CGXComboListBox(CWnd* pMsgWnd)
{
	m_bKeyDown = FALSE;
	m_bLButtonDown = FALSE;
	m_pMsgWnd = pMsgWnd;
	m_bColor = FALSE;
	m_nRepCnt = 0;
}

BOOL CGXComboListBox::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
#ifdef _UNIX_
	VERIFY(CWnd::Create(_T("LISTBOX"), _T(""),
		dwStyle | WS_VISIBLE | WS_BORDER,
		rect, pParentWnd, nID));
#else
	VERIFY(CWnd::Create(_T("COMBOLBOX"), _T(""),
		dwStyle | WS_VISIBLE | WS_BORDER,
		rect, pParentWnd, nID));
#endif
	return TRUE;
}

CGXComboListBox::~CGXComboListBox()
{
}

BEGIN_MESSAGE_MAP(CGXComboListBox, CListBox)
	//{{AFX_MSG_MAP(CGXComboListBox)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_KEYUP()
	ON_WM_KILLFOCUS()
	ON_WM_SYSKEYUP()
	ON_WM_SYSKEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// ============ CGXComboListBox message handlers

void CGXComboListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_bKeyDown =TRUE;
	if (m_nRepCnt++ == 0)
		m_nOldSel = GetCurSel();
	m_bAlNum = _istalnum((_TXCHAR) nChar);

	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CGXComboListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bLButtonDown = TRUE;
	m_nOldSel = GetCurSel();

	CListBox::OnLButtonDown(nFlags, point);
}

void CGXComboListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	CListBox::OnLButtonUp(nFlags, point);

	if (m_bLButtonDown && GetCurSel() != LB_ERR)
		m_pMsgWnd->SendMessage(WM_GX_LBOXEND, 0, 0);
}

BOOL CGXComboListBox::OnEraseBkgnd(CDC* pDC)
{
#if _MFC_VER >= 0x0300
	HBRUSH hBrush = (HBRUSH) GetParent()->SendMessage(
		WM_CTLCOLORLISTBOX,
		(WPARAM) pDC->GetSafeHdc(),
		(LPARAM) m_hWnd);
#else
	HBRUSH hBrush = (HBRUSH) GetParent()->SendMessage(
		WM_CTLCOLOR,
		(WPARAM) pDC->GetSafeHdc(),
		MAKELONG((WORD) m_hWnd, (WORD) CTLCOLOR_LISTBOX));
#endif

	if (hBrush)
	{
		CRect rect;
		GetClientRect(rect);
		::FillRect(pDC->GetSafeHdc(), rect, hBrush);
		return TRUE;
	}

	return CListBox::OnEraseBkgnd(pDC);
}

void CGXComboListBox::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CListBox::OnKeyUp(nChar, nRepCnt, nFlags);

	if (m_bKeyDown)
	{
		if (nChar == 9)
		{
			m_pMsgWnd->PostMessage(WM_KEYDOWN, 9);
			m_pMsgWnd->PostMessage(WM_KEYUP, 9);
			m_pMsgWnd->SendMessage(WM_GX_LBOXEND, 0, 0);
		}
		else if (nChar == 13)
			m_pMsgWnd->SendMessage(WM_GX_LBOXEND, 0, 0);
		else if (nChar == 27)
			m_pMsgWnd->SendMessage(WM_GX_LBOXCANCEL, 0, 0);
		else if (m_nOldSel != GetCurSel())
		{
			m_pMsgWnd->SendMessage(WM_GX_LBOXCHANGED, 0, 0);
			m_nOldSel = GetCurSel();
		}

		m_nRepCnt = 0;
	}
}

void CGXComboListBox::OnKillFocus(CWnd* pNewWnd)
{
	// TRACE0("CGXComboListBox::OnKillFocus\n");

	CListBox::OnKillFocus(pNewWnd);

	if (m_pMsgWnd)
		m_pMsgWnd->PostMessage(WM_GX_LBOXKILLFOCUS, 0, 0);
}

void CGXComboListBox::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_bKeyDown && nChar == VK_DOWN)
		m_pMsgWnd->SendMessage(WM_GX_LBOXCANCEL, 0, 0);
	else
		CListBox::OnSysKeyUp(nChar, nRepCnt, nFlags);
}

void CGXComboListBox::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_bKeyDown =TRUE;

	CListBox::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

// CGXComboBox

const int _nComboBtnWidth = 15;
const int _nComboBtnHeight = 18;


CGXComboBox::CGXComboBox(CGXGridCore* pGrid, UINT nEditID, UINT nListBoxID, UINT nFlags)
	: CGXEditControl(pGrid, nEditID)
{
	AddChild(m_pButton = new CGXComboBoxButton(this));
	m_nListBoxID = nListBoxID;
	m_pDropDownWnd = NULL;
	m_nFlags = nFlags;
	m_bFillWithChoiceList = TRUE;
	m_bInactiveDrawAllCell = FALSE; // draw text over pushbutton when inactive
	m_bInactiveDrawButton = FALSE;  // draw pushbutton when inactive
	m_dwListBoxStyle = 0;
	m_bDropdownOnly = FALSE;        // set this TRUE if text shall not be editable
	m_bSizeToContent = TRUE;        // set this TRUE if combobox button shall only be as large as
									// the text (OG 1.1 standard behavior),
									// FALSE will draw the button over the whole cell height

	if (m_nFlags & GXCOMBO_DISPLAYCHOICE)
	{
		// must be combined with index flag
		ASSERT(m_nFlags & (GXCOMBO_ONEBASED | GXCOMBO_ZEROBASED));
	}

	// don't change this!
	m_nDefaultHeight = 0;
	m_nExtraFrame = 1;
}

CGXComboBox::~CGXComboBox()
{
	if (m_pDropDownWnd)
	{
		CGXGridCombo::SetComboBoxDropDown(TRUE);

		m_pDropDownWnd->DestroyWindow();
		delete m_pDropDownWnd;
		m_pDropDownWnd  = NULL;

		CGXGridCombo::SetComboBoxDropDown(FALSE);
	}
}

void CGXComboBox::Init(ROWCOL nRow, ROWCOL nCol)
{
	CGXEditControl::Init(nRow, nCol);

	// Force drawing of buttons for current cell
	GridWnd()->InvalidateRect(m_pButton->GetRect());
}

void CGXComboBox::OnNotifyMsg(MSG* pMsg)
{
	if (pMsg->message == WM_VSCROLL
		|| pMsg->message == WM_HSCROLL)
		ShowDropDown(FALSE);

	CGXEditControl::OnNotifyMsg(pMsg);
}

void CGXComboBox::Reset()
{
	ShowDropDown(FALSE);
	CGXEditControl::Reset();
}

void CGXComboBox::SetActive(BOOL bActive)
{
	if (!bActive && GetDroppedState())
	{
		PostMessage(WM_GX_LBOXCANCEL, 0, 0);
		// will call ShowDropDown(FALSE);
	}

	CGXEditControl::SetActive(bActive);
}

BEGIN_MESSAGE_MAP(CGXComboBox, CGXEditControl)
	//{{AFX_MSG_MAP(CGXComboBox)
	ON_MESSAGE(WM_GX_LBOXEND, OnListBoxEnd)
	ON_MESSAGE(WM_GX_LBOXCANCEL, OnListBoxCancel)
	ON_MESSAGE(WM_GX_LBOXCHANGED, OnListBoxChanged)
	ON_MESSAGE(WM_GX_LBOXKILLFOCUS, OnListBoxKillFocus)
	ON_WM_SYSKEYDOWN()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CRect CGXComboBox::GetCellRect(ROWCOL nRow, ROWCOL nCol, LPRECT rectItem /* = NULL */, const CGXStyle* pStyle /*= NULL*/)
{
	// compute the interior rectangle for the text
	// without buttons and borders

	CRect rect = CGXEditControl::GetCellRect(nRow, nCol, rectItem, pStyle);

	if (!m_bInactiveDrawAllCell
		|| m_bInactiveDrawButton
		|| Grid()->IsCurrentCell(nRow, nCol))
		rect.right -= _nComboBtnWidth;

	return rect;
}

CSize CGXComboBox::AddBorders(CSize size, const CGXStyle& style)
{
	size.cx += _nComboBtnWidth;

	return CGXEditControl::AddBorders(size, style);
}

void CGXComboBox::OnInitChildren(ROWCOL nRow, ROWCOL nCol, const CRect& rect)
{
	nRow, nCol;

	if (m_pButton)
	{
		int dy = rect.Height();

		// initialize combobox button only if
		// not the current cell
		CRect rectBtn(
				max(rect.left+1, rect.right-1-_nComboBtnWidth),
				rect.top,
				rect.right-1,
				rect.top + dy -1);

		// Determine the default height for the combobox edit part
		const CGXStyle* pStyle = NULL;
		if (IsInit() && m_nRow == nRow && m_nCol == nCol)
		{
			NeedStyle();
			pStyle = m_pStyle;
		}
		else if (m_bInactiveDrawButton)
			pStyle = &Grid()->LookupStyleRowCol(nRow, nCol);

		if (pStyle)
		{
			if (m_nDefaultHeight == 0)
			{
				// Get font metrics
				TEXTMETRIC tm;
				CClientDC dc(GridWnd());
				Grid()->OnGridPrepareDC(&dc);
				CFont* pOldFont = Grid()->LoadFont(&dc, *pStyle);
				dc.GetTextMetrics(&tm);
				if (pOldFont)
					dc.SelectObject(pOldFont);

				m_nDefaultHeight = tm.tmHeight;
			}
		}

		if (m_bSizeToContent)
		{
			if (pStyle)
			{
				dy = min(m_nDefaultHeight+2, rect.Height() - 2*m_nExtraFrame);

				if (pStyle->GetIncludeVerticalAlignment())
				{
					// vertical alignment for single line text
					// is done by computing the window rectangle for the CComboBox
					switch (pStyle->GetVerticalAlignment())
					{
					case DT_VCENTER:
						rectBtn.top += (rect.Height() - dy) / 2 - m_nExtraFrame;
						break;
					case DT_BOTTOM:
						rectBtn.top = rect.bottom - dy - 2 * m_nExtraFrame;
					break;
					}
				}
			}

			rectBtn.bottom = rectBtn.top + dy + 2*m_nExtraFrame;
			rectBtn.bottom--;
		}

		m_pButton->SetRect(rectBtn);
	}
}

CSize CGXComboBox::CalcSize(CDC* pDC, ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, const CGXStyle* pStandardStyle, BOOL bVert)
{
	// Size of edit portion
	CSize sizeEdit = CGXEditControl::CalcSize(pDC, nRow, nCol, style, pStandardStyle, bVert);

	// Determine size of listbox entries
	CSize sizeCombo(0, 0);  

	// Select font
	CFont* pOldFont = LoadFont(pDC, style, pStandardStyle);

	if (bVert)
	{
		// row height
		TEXTMETRIC    tm;
		pDC->GetTextMetrics(&tm);
		sizeCombo.cy = tm.tmHeight + tm.tmExternalLeading + 1;
	}
	else
	{
		// BLOCK:
		{
			CSize sz;

			int width = 0;

			// check each item
			if (m_bFillWithChoiceList && style.GetIncludeChoiceList())
			{
				CString s = style.GetChoiceListRef();
				CString sItem;
				int n = 0;
				while (!s.IsEmpty() && n != -1)
				{
					n = GXGetNextLine(s, sItem);

					sz = pDC->GetTextExtent(sItem, sItem.GetLength());
					width = max(width, sz.cx);
				}
			}

			sizeCombo.cx = width;
		}
	}

	if (pOldFont)
		pDC->SelectObject(pOldFont);

	// Add space for borders, left and right margin, 3d frame etc.
	sizeCombo = AddBorders(sizeCombo, style);

	// Return the size which fits for both listbox entries and edit portion
	return CSize(max(sizeCombo.cx, sizeEdit.cx), max(sizeCombo.cy, sizeEdit.cy));
}

BOOL CGXComboBox::GetValue(CString& sResult)
{
	if (!CGXEditControl::GetValue(sResult))
		return FALSE;

	BOOL bTextFit = m_nFlags & GXCOMBO_TEXTFIT;
	BOOL bDispChoice = m_nFlags & GXCOMBO_DISPLAYCHOICE;
	int nIndexOffset = (m_nFlags & GXCOMBO_ONEBASED) ? 1 : 0;

	if (m_bFillWithChoiceList && (bTextFit || bDispChoice))
	{
		if (m_pStyle && m_pStyle->GetIncludeChoiceList())
		{
			CString sMatch = sResult;

			// finds exact string or the first string with the same prefix
			int nIndex = FindStringInChoiceList(sMatch, sResult,
				m_pStyle->GetChoiceListRef(), FALSE);

			if (bDispChoice)
			{
				if (nIndex != -1)
				{
					nIndex += nIndexOffset;
					wsprintf(sResult.GetBuffer(20), _T("%d"), nIndex);
					sResult.ReleaseBuffer();
				}
				else
					sResult.Empty();
			}
			else
				// assign the exact choice or the choice with the same prefix
				sResult = sMatch;
		}
	}

	return TRUE;
}

BOOL CGXComboBox::GetControlText(CString& strResult, ROWCOL nRow, ROWCOL nCol, LPCTSTR pszRawValue, const CGXStyle& style)
{
	CString sItem;

	// First, we need to convert the raw value into an choice list entry
	// when CGXComboBox is used with GXCOMBO_DISPLAYCHOICE flag.
	if (m_nFlags & GXCOMBO_DISPLAYCHOICE
		&& m_bFillWithChoiceList
		&& style.GetIncludeChoiceList())
	{
		if (pszRawValue == NULL && style.GetIncludeValue())
			pszRawValue = style.GetValueRef();

		if (pszRawValue == NULL || _tcslen(pszRawValue) == 0)
			return FALSE;

		// determine index
		short nIndex = (short) _ttoi(pszRawValue);

		// the following operation needs a zero-based index
		if (nIndex != -1 && m_nFlags & GXCOMBO_ONEBASED)
			nIndex--;

		GetChoiceListItem(sItem, style.GetChoiceListRef(), nIndex);
			// base class version will format the text as specified in mask
		pszRawValue = sItem;
	}

	// Now, we can format this entry.
	return CGXEditControl::GetControlText(strResult, nRow, nCol, pszRawValue, style);
}

BOOL CGXComboBox::ConvertControlTextToValue(CString& str, ROWCOL nRow, ROWCOL nCol, const CGXStyle* pOldStyle)
{
	BOOL bTextFit = m_nFlags & GXCOMBO_TEXTFIT;
	BOOL bDispChoice = m_nFlags & GXCOMBO_DISPLAYCHOICE;
	int nIndexOffset = (m_nFlags & GXCOMBO_ONEBASED) ? 1 : 0;

	// check if we need to convert to an index
	if (m_bFillWithChoiceList && (bTextFit || bDispChoice))
	{
		CGXStyle* pStyle = NULL;
		if (pOldStyle == NULL)
		{
			pStyle = Grid()->CreateStyle();
			Grid()->ComposeStyleRowCol(nRow, nCol, pStyle);
			pOldStyle = pStyle;
		}

		if (pOldStyle && pOldStyle->GetIncludeChoiceList())
		{
			CString sMatch;

			// finds exact string or the first string with the same prefix
			int nIndex = FindStringInChoiceList(sMatch, str,
				pOldStyle->GetChoiceListRef(), FALSE);

			if (bDispChoice)
			{
				if (nIndex != -1)
				{
					nIndex += nIndexOffset;
					wsprintf(str.GetBuffer(20), _T("%d"), nIndex);
					str.ReleaseBuffer();
				}
				else
					str.Empty();
			}
			else
				// assign the exact choice or the choice with the same prefix
				str = sMatch;
		}

		if (pStyle)
			Grid()->RecycleStyle(pStyle);

	}

	return TRUE;
}

BOOL CGXComboBox::Store()
{
	return CGXEditControl::Store();
}

BOOL CGXComboBox::ValidateString(const CString& sEdit)
{
	if (sEdit.IsEmpty() || !m_bFillWithChoiceList)
		return TRUE;

	BOOL bValid = TRUE;

	BOOL bTextFit = m_nFlags & GXCOMBO_TEXTFIT;
	BOOL bDispChoice = m_nFlags & GXCOMBO_DISPLAYCHOICE;

	CString sMatch;

	if (bTextFit || bDispChoice)
	{
		if (!m_pStyle->GetIncludeChoiceList())
			return FALSE;

		// search choice which exactly fits the given text
		int nIndex = FindStringInChoiceList(sMatch, sEdit,
			 m_pStyle->GetChoiceListRef(), FALSE);

		if (nIndex == -1)
			bValid = FALSE;
	}

	return bValid;
}

void CGXComboBox::ReplaceSel(LPCTSTR pszReplaceText)
{
	if (IsActive())
	{
		CEdit::ReplaceSel(pszReplaceText);
		SetModify(TRUE);
		OnModifyCell();
	}
	else
		CGXControl::ReplaceSel(pszReplaceText);
}

// WM_MOUSEACTIVATE message
BOOL CGXComboBox::MouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message, int& retval)
{
	if (GetDroppedState())
	{
		retval = MA_ACTIVATEANDEAT;
		SendMessage(WM_GX_LBOXCANCEL, 0, TRUE);
		return TRUE;
	}

	return CGXEditControl::MouseActivate(pDesktopWnd, nHitTest, message, retval);
}

BOOL CGXComboBox::OnGridSysKeyDown(UINT nChar, UINT nRepCnt, UINT flags)
{
	if (nChar == VK_DOWN)
	{
		// trigger OnStartEditing
		if (!OnStartEditing())
			return FALSE;

		SetActive(TRUE);

		// Redraw the cell
		if (!Grid()->ScrollCellInView(m_nRow, m_nCol))
			Refresh();

		GridWnd()->UpdateWindow();
		ShowDropDown(TRUE);

		return TRUE;
	}

	return CGXEditControl::OnGridSysKeyDown(nChar, nRepCnt, flags);
}

BOOL CGXComboBox::OnGridChar(UINT nChar, UINT nRepCnt, UINT flags)
{
	if (m_bDropdownOnly && nChar == VK_DOWN)
	{
		// search choice which exactly fits the given text
		CString sMatch, sEdit = (TCHAR) nChar;
		FindStringInChoiceList(sMatch, sEdit,
			 m_pStyle->GetChoiceListRef(), FALSE);

		SetCurrentText(sMatch);

		return TRUE;
	}

	return CGXEditControl::OnGridChar(nChar, nRepCnt, flags);
}

void CGXComboBox::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_DOWN)
	{
		// trigger OnStartEditing
		if (!OnStartEditing())
			return;

		Grid()->ScrollCellInView(m_nRow, m_nCol);
		GridWnd()->UpdateWindow();
		ShowDropDown(TRUE);
		return;
	}

	CGXEditControl::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CGXComboBox::OnClickedButton(CGXChild* pChild)
{
	pChild;

	if (!IsActive())
	{
		// trigger OnStartEditing
		if (!OnStartEditing())
			return;

		SetActive(TRUE);

		Refresh();
	}

	GridWnd()->UpdateWindow();

	ShowDropDown(!GetDroppedState());

	CGXEditControl::OnClickedButton(pChild);
}

BOOL CGXComboBox::GetDroppedState()
{
	return m_pDropDownWnd != NULL;
}

void CGXComboBox::ShowDropDown(BOOL bShow)
{
	// TRACE(_T("CGXComboBox::ShowDropDown(BOOL bShow = %d)\n"), bShow);

	if (!bShow)
	{
		// Hide dropdown-listbox
		if (m_pDropDownWnd)
			SendMessage(WM_GX_LBOXCANCEL, 0, IsActive());
		return;
	}
	else if (m_pDropDownWnd)
		// dropdown-listbox alread visible
		return;

	// else display dropdown-listbox
	Grid()->SetDropDownCell(TRUE);

	// Style
	CGXStyle style;
	Grid()->ComposeStyleRowCol(m_nRow, m_nCol, &style);

	CRect rect = CGXControl::GetCellRect(m_nRow, m_nCol, NULL, &style);

	// calculate rectangle for popup window
	CWnd* desktopWnd = CWnd::GetDesktopWindow();

	CRect desktopRect;
	desktopWnd->GetClientRect(&desktopRect);

	// hook for changing the default listbox width/height
	Grid()->SetIgnoreFocus(TRUE);
	Grid()->m_bLockActivateGrid = TRUE;

	// create popup window
	m_pDropDownWnd = CreateDropDown(GridWnd(), CRect(0,0,1,1));

	// create listbox
	CWnd* pListBox = CreateListBox(m_pDropDownWnd, m_nListBoxID);
	ASSERT_VALID(pListBox);
	// ASSERTION-> Failed to create listbox ->END

	Grid()->SetIgnoreFocus(FALSE);

	// embedd listbox in popup window
	m_pDropDownWnd->AttachListBox(pListBox);

	// listbox can be referenced with m_pDropDownWnd->GetLBox()

	// create GDI-font-object
	if (m_fontCombo.GetSafeHandle() != 0)
		m_fontCombo.DeleteObject();

	const LOGFONT& _lf = style.GetFontRef().GetLogFont(Grid()->GetZoom());
	if ( !m_fontCombo.CreateFontIndirect(&_lf) )
	{
		TRACE1("Failed to create font '%s' in CGXGridCore::OnDropDownListRowCol\n",
			(LPCTSTR) style.GetFontRef().GetDescription());
	}

	// set font in listbox
	m_pDropDownWnd->GetLBox().SetFont(&m_fontCombo);

	COLORREF rgbText = style.GetTextColor(),
			 rgbBack = style.GetInteriorRef().GetColor();

	// set colors in m_pDropDownWnd
	// (listbox colors are determined with OnCtlColor in CGXGridCombo)
	if (Grid()->GetParam()->GetProperties()->GetBlackWhite())
	{
		rgbText = RGB(0,0,0);
		rgbBack = RGB(255,255,255);
	}

	m_pDropDownWnd->SetColor(rgbText, rgbBack);

	// Fill entries (also a hook for changing the color)
	OnFillDroppedList(&m_pDropDownWnd->GetLBox());

	CRect rectComboLBox = OnGetListboxRect(rect);

	GridWnd()->ClientToScreen(rectComboLBox);

	// Scroll selected item into view
	CListBox& lBox = m_pDropDownWnd->GetLBox();
	int nOptHeight = m_nDefaultHeight*lBox.GetCount() + lBox.GetItemHeight(0)/2;
	if (nOptHeight < rectComboLBox.Height())
		rectComboLBox.bottom = rectComboLBox.top + nOptHeight;

	if (rectComboLBox.right > desktopRect.right)
		rectComboLBox -= CPoint(rectComboLBox.Width() - rect.Width(), 0);

	if (rectComboLBox.bottom > desktopRect.bottom)
		rectComboLBox -= CPoint(0, rectComboLBox.Height() + rect.Height());

	CGXGridCombo::SetComboBoxDropDown(TRUE);

	// I could add a hook for Title row here (CGXTabbedComboBox)

	rectComboLBox.IntersectRect(rectComboLBox, desktopRect);

	// Display dropdown list box
	m_pDropDownWnd->SetWindowPos(&wndTopMost, rectComboLBox.left,
		rectComboLBox.top,
		rectComboLBox.Width(),
		rectComboLBox.Height(), SWP_SHOWWINDOW);

	lBox.SetWindowPos(NULL, 0, 0, rectComboLBox.Width(), rectComboLBox.Height(), SWP_SHOWWINDOW);

	// Set Focus
	m_pDropDownWnd->GetLBox().SetFocus();
	if (m_pDropDownWnd->GetLBox().GetCurSel() == LB_ERR)
		m_pDropDownWnd->GetLBox().SetCurSel(0);
	m_pDropDownWnd->GetLBox().UpdateWindow();

	if (m_pDropDownWnd->GetLBox().GetCurSel() != LB_ERR)
		m_pDropDownWnd->GetLBox().SetTopIndex(m_pDropDownWnd->GetLBox().GetCurSel());

	CGXGridCombo::SetComboBoxDropDown(FALSE);
	Grid()->m_bLockActivateGrid = FALSE;
}

CRect CGXComboBox::OnGetListboxRect(const CRect& rcCellRect)
{
	// override this method for example if you want to have a
	// listbox which is wider than the cell

	return CRect(rcCellRect.left,
		rcCellRect.bottom,
		rcCellRect.right,
		rcCellRect.bottom+Grid()->GetParam()->GetDefaultComboBoxHeight());
}

CGXGridCombo* CGXComboBox::CreateDropDown(CWnd* pWnd, const CRect& rect)
{
	return new CGXGridCombo(pWnd, this, rect);
}

CWnd* CGXComboBox::CreateListBox(CWnd* pParentWnd, UINT nID)
{
	// Creates the listbox

	// if you want to create an owner-drawn listbox, you should
	// derive a class from CGXComboListBox, override CreateListBox
	// and return a pointer to it.

	CGXComboListBox* pListBox = new CGXComboListBox(this);

	CRect rect;
	pParentWnd->GetClientRect(&rect);
	rect.left++;
	rect.right--;

	if (m_dwListBoxStyle == 0)
		m_dwListBoxStyle = WS_VSCROLL|LBS_NOTIFY;

	pListBox->Create(m_dwListBoxStyle, rect, pParentWnd, nID);

	// CGXComboBox will delete the listbox object.

	return pListBox;
}

void CGXComboBox::OnFillDroppedList(CListBox* lbox)
{
	const CGXStyle& style = Grid()->LookupStyleRowCol(m_nRow, m_nCol);

	// fill with Choices
	if (m_bFillWithChoiceList && style.GetIncludeChoiceList())
	{
		CString s = style.GetChoiceListRef();
		int n = 0;
		CString sItem;
		while (!s.IsEmpty() && n != -1)
		{
			n = GXGetNextLine(s, sItem);

			lbox->AddString(sItem);
		}
	}

	BOOL bTextFit = m_nFlags & GXCOMBO_TEXTFIT;
	BOOL bDispChoice = m_nFlags & GXCOMBO_DISPLAYCHOICE;

	// Set Window Text and select item
	CString s;
	if (IsActive())
	{
		GetWindowText(s);
		bTextFit |= bDispChoice;    // call FindStringExact when bDispChoice is TRUE
	}
	else
		s = style.GetValueRef();    // call SetCurSel when bDispChoice is TRUE

	// Select item
	if (bTextFit)
	{
		int nIndex = lbox->FindStringExact(-1, s);
		if (nIndex == -1)
			lbox->SelectString(-1, s);
		else
			lbox->SetCurSel(nIndex);
	}
	else if (m_nFlags & GXCOMBO_ZEROBASED)
		lbox->SetCurSel(_ttoi(s));
	else if (m_nFlags & GXCOMBO_ONEBASED)
		lbox->SetCurSel(_ttoi(s)-1);
	else
		lbox->SelectString(-1, s);
}

void CGXComboBox::OnStoreDroppedList(CListBox* lbox)
{
	// Save the selection
	int index = lbox->GetCurSel();
	if (index != LB_ERR)
	{
		CString s;
		TCHAR sz[20];

		if (m_nFlags & GXCOMBO_ZEROBASED)
		{
			wsprintf(sz, _T("%d"), index);
			s = sz;
		}
		else if (m_nFlags & GXCOMBO_ONEBASED)
		{
			wsprintf(sz, _T("%d"), index+1);
			s = sz;
		}
		else
			lbox->GetText(index, s);

		SetActive(TRUE);
		SetValue(s);
		SetSel(0,-1);
		SetModify(TRUE);
		if (m_bDropdownOnly)
			Refresh();
		else
			Invalidate();
	}
}

LRESULT CGXComboBox::OnListBoxEnd(WPARAM, LPARAM )
{
	// TRACE0("CGXComboBox::OnListBoxEnd(WPARAM, LPARAM )\n");
	ASSERT(m_pStyle);

	Grid()->SetIgnoreFocus(TRUE);
	Grid()->SetDropDownCell(FALSE);

	// User has selected an item
	if (m_pDropDownWnd)
	{
		CGXGridCombo::SetComboBoxDropDown(TRUE);

		NeedStyle();

		// empty cell, when user pressed alpahnumeric key
		if (!IsReadOnly())
			OnStoreDroppedList(&m_pDropDownWnd->GetLBox());

		// I need to set m_pDropDownWnd = NULL before I destroy the window
		// This avoids problems when killing the focus results in a call to ListBoxCancel
		CWnd* pDropDownWnd = m_pDropDownWnd;
		m_pDropDownWnd  = NULL;

		CWnd* pOldFocus = GetFocus();
		BOOL bSetFocusAgain = FALSE;
		if(pOldFocus == this || pDropDownWnd->IsChild(pOldFocus))
			bSetFocusAgain = TRUE; 

		if (pDropDownWnd)
			DestroyDropDownWnd(pDropDownWnd);

		CGXGridCombo::SetComboBoxDropDown(FALSE);

		// Fire OnModifyCell event after Dropdown-window has been destroyed

		OnModifyCell();

#ifdef _WIN32
		if (bSetFocusAgain || GetFocus() == GridWnd())
			SetFocus();
#else
		if (bSetFocusAgain || ::IsChild(AfxGetMainWnd()->m_hWnd,::GetFocus()));
			SetFocus();
#endif

		Grid()->SetIgnoreFocus(FALSE);
	}

	return 0;
}

void CGXComboBox::DestroyDropDownWnd(CWnd* pDropDownWnd)
{
	if (pDropDownWnd)
	{
		pDropDownWnd->DestroyWindow();
		delete pDropDownWnd;
	}

	m_fontCombo.DeleteObject();
}

LRESULT CGXComboBox::OnListBoxCancel(WPARAM, LPARAM bSetFocus)
{
	// TRACE0("CGXComboBox::OnListBoxCancel(WPARAM, LPARAM )\n");
	// User has canceled listbox selection
	Grid()->SetDropDownCell(FALSE);

	if (m_pDropDownWnd)
	{
		Grid()->SetIgnoreFocus(TRUE);
		CGXGridCombo::SetComboBoxDropDown(TRUE);

		BOOL bFocus =  bSetFocus ||
			&m_pDropDownWnd->GetLBox() == GetFocus() && Grid()->IsActiveFrame();

		// I need to set m_pDropDownWnd = NULL before I destroy the window
		// This avoids problems when killing the focus results in a further call to ListBoxCancel
		CWnd* pDropDownWnd = m_pDropDownWnd;
		m_pDropDownWnd  = NULL;

		if (pDropDownWnd)
			DestroyDropDownWnd(pDropDownWnd);

		CGXGridCombo::SetComboBoxDropDown(FALSE);

		if (bFocus)
		{
			SetFocus();
			if (GetModify())
				OnModifyCell();
		}
		else
		{
			SetActive(FALSE);
			SetModify(FALSE);
			Refresh();
		}

		Grid()->SetIgnoreFocus(FALSE);
	}

	return 0;
}

LRESULT CGXComboBox::OnListBoxChanged(WPARAM, LPARAM)
{
	// User has changed selection

	if (!IsReadOnly())
	{
		OnStoreDroppedList(&m_pDropDownWnd->GetLBox());
		OnModifyCell();
	}

	return 0;
}

LRESULT CGXComboBox::OnListBoxKillFocus(WPARAM, LPARAM)
{
	// User has changed selection

	CWnd* pWnd = GetFocus();

	if (pWnd == NULL)
		SendMessage(WM_GX_LBOXCANCEL, 0, 0);

	else if (!(GridWnd()->IsChild(pWnd) || GridWnd() == pWnd))
	{
		Grid()->SetIgnoreFocus(TRUE);
		GridWnd()->SetFocus();
		SendMessage(WM_GX_LBOXCANCEL, 0, 0);
		// update the titlebar
		pWnd->SetFocus();
		Grid()->SetIgnoreFocus(FALSE);
	}

	return 0;
}

void CGXComboBox::OnSetFocus(CWnd* pOldWnd)
{
	// TRACE0("CGXComboBox::OnSetFocus\n");

	if (GetDroppedState())
		SendMessage(WM_GX_LBOXCANCEL, 0, TRUE /* set focus to cotrol */);

	CGXEditControl::OnSetFocus(pOldWnd);
}

void CGXComboBox::Draw(CDC* pDC, CRect rect, ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, const CGXStyle* pStandardStyle)
{
	ASSERT(pDC != NULL && pDC->IsKindOf(RUNTIME_CLASS(CDC)));
	// ASSERTION-> Invalid Device Context ->END
	ASSERT(nRow <= Grid()->GetRowCount() && nCol <= Grid()->GetColCount());
	// ASSERTION-> Cell coordinates out of range ->END

	ASSERT_VALID(pDC);

	// if (nRow == m_nRow && nCol == m_nCol && m_bIsActive)
	//     initialize CWnd, make it visible and set focus
	if (nRow == m_nRow && nCol == m_nCol
		&& IsActive() && !Grid()->IsPrinting()
		&& m_bDropdownOnly)
	{
		CGXStyle* pStyle = style.Clone();

		CString sValue;
		GetValue(sValue);

		pStyle->SetValue(sValue);

		CGXStatic::Draw(pDC, rect, nRow, nCol, *pStyle, pStandardStyle);

		delete pStyle;
	}
	else
		CGXEditControl::Draw(pDC, rect, nRow, nCol, style, pStandardStyle);
}

BOOL CGXComboBox::CanFloatCell(ROWCOL, ROWCOL, const CGXStyle&, BOOL)
{
	// combobox don't support floating
	return FALSE;
}
