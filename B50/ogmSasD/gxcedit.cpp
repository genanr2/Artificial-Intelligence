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

// gxcedit.cpp : implementation of the CGXEditControl control
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

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCEDIT")
#endif

IMPLEMENT_CONTROL(CGXEditControl, CGXStatic);
IMPLEMENT_DYNAMIC(CGXEditControl, CEdit);

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CGXEditControl

CGXEditControl::CGXEditControl(CGXGridCore* pGrid, UINT nID)
	: CGXStatic(pGrid)
{
	m_nID = nID;

	m_bIsActive = FALSE;

	m_bUpdateEditStyle = FALSE;
	m_dyHeight = 0;
	m_dxMaxCharWidth = 0;
	m_rgbWndHilightText   = GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_bAutoSize = FALSE;
	m_bSizeChanged = FALSE;
	m_bAlwaysLeftJustified = FALSE;
	m_bDoNotEmptyLastChar = FALSE;
	m_nLimitText = 0;
	m_bFloatDone = FALSE;
	m_bWrapText = FALSE;


	// NOTE: There is a problem with Win32s. It is
	// not possible to support the automatic switching
	// from right-aligned to left-aligned text on the
	// fly while the user is entering data. Therefore
	// I have turned off this method for Win32s.
	//
	// So, if the user wants to type in text in a right-aligned
	// or centered cell, and the CEdit window gets the focus
	// the text will be displayed left-aligned.
	//
	// A note to MAC and UNIX developers: If you encounter
	// weird things when the user enters large text into
	// right-aligned or centered cells, you should also
	// turn off this feature. Please let us know if you
	// think we should turn this feature off for MAC
	// or UNIX.

#if _MFC_VER >= 0x0300 && _MFC_VER < 0x0420
	if (GXGetAppData()->bWin31)
		m_bAlwaysLeftJustified = TRUE;
#endif
}

CGXEditControl::~CGXEditControl()
{
	DestroyWindow();
}

BOOL CGXEditControl::CreateControl(DWORD dwStyle, LPRECT lpRect)
{
#if defined(_UNIX_)
	CRect r(-100,-100,10,10);
#else
	CRect r(0,0,0,0);
#endif

	if (lpRect)
		r = *lpRect;

	return CWnd::Create(_T("GXEDIT"),
		NULL,
		dwStyle,
		r,
		GridWnd(),
		m_nID);
}

CWnd* CGXEditControl::GetWndPtr() const
{
	return (CWnd*) this;
}

BEGIN_MESSAGE_MAP(CGXEditControl, CEdit)
	//{{AFX_MSG_MAP(CGXEditControl)
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_ERASEBKGND()
	ON_WM_SHOWWINDOW()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_KEYUP()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSCHAR()
	ON_WM_SYSKEYUP()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
#if _MFC_VER >= 0x0400
	ON_WM_CONTEXTMENU()
	ON_MESSAGE(WM_IME_STARTCOMPOSITION, OnImeStartComposition)
	ON_MESSAGE(WM_IME_ENDCOMPOSITION, OnImeEndComposition)
#endif
END_MESSAGE_MAP()

static BOOL NEAR s_bImeOpen = FALSE;

#if _MFC_VER >= 0x0400
LRESULT CGXEditControl::OnImeStartComposition(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	s_bImeOpen = TRUE;

	return Default();
}

LRESULT CGXEditControl::OnImeEndComposition(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	s_bImeOpen = FALSE;

	return Default();
}
#endif

// Status
void CGXEditControl::SetActive(BOOL bActive)
{
	if (!m_bIsActive && bActive && m_hWnd)
	{				 
		m_bIsActive = TRUE;
		// Force a call to GetStyleRowCol() because
		// GetStyleRowCol() might set style attributes
		// depending on the IsActive() flag
		FreeStyle();
		NeedStyle();
	}
	else
		m_bIsActive = bActive && m_hWnd;

	if (m_hWnd)
	{
		Grid()->DelayFloatCells(CGXRange(m_nRow, m_nCol));
		Grid()->EvalFloatCells(CGXRange(m_nRow, m_nCol));
	//	Grid()->EvalVisibleFloatMergeCells(CGXRange(m_nRow, m_nCol, Grid()->GetLastVisibleRow(), Grid()->GetLastVisibleCol()), TRUE);
	}
}

BOOL CGXEditControl::IsActive()
{
	return m_bIsActive;
}

void CGXEditControl::SetModify(BOOL bModified)
{
	if (m_hWnd)
		CEdit::SetModify(bModified);
}

BOOL CGXEditControl::GetModify()
{
	return m_hWnd && CEdit::GetModify();
}

// Operations
void CGXEditControl::Init(ROWCOL nRow, ROWCOL nCol)
{
	s_bImeOpen = FALSE;

	// Stores the cell coordinates, resets the style and sets the window text
	CGXStatic::Init(nRow, nCol);

	if (!m_hWnd)
		CreateControl();

	NeedStyle();

	CString s = Grid()->GetExpressionRowCol(nRow, nCol);
	//if (m_pStyle->GetIncludeValue())
	//	 s = m_pStyle->GetValueRef();

	m_bAutoSize = FALSE;

	if (m_pStyle->GetIncludeAutoSize())
		 m_bAutoSize = m_pStyle->GetAutoSize();

	// hook for setting the window text
	// optimal override for masked edit
	SetValue(s);

	if (m_hWnd)
	{
		SetSel(0, -1, FALSE);   // Select all

		if (/*m_bAutoSize && */IsActive())
			LineScroll(255, 0);
	}

	SetModify(FALSE);
}

void CGXEditControl::SetCurrentText(const CString& str)
{
	if (!IsInit() || !m_hWnd)
		return;

	if (!IsActive())
		SetActive(TRUE);

	if (ValidateString(str))
	{
		SetWindowText(str);
		SetModify(TRUE);
		OnModifyCell();
	}

	Grid()->DelayFloatCells(CGXRange(m_nRow, m_nCol));
	Grid()->EvalFloatCells(CGXRange(m_nRow, m_nCol));
}

// GetCurrentText
//
// Get masked text which is displayed in the cell

void CGXEditControl::GetCurrentText(CString& sMaskedResult)
{
	if (IsActive())
	{
		GetWindowText(sMaskedResult);
	}
	else if (IsInit())
	{
		// Convert raw value to masked string
		NeedStyle();
		GetControlText(sMaskedResult, m_nRow, m_nCol, NULL, *m_pStyle);
	}
	else
	{
		TRACE0("Warning: GetCurrentText was called for an unitialized control!\n");

		// Control is not initialized - Unable to determine text
		sMaskedResult.Empty();
	}
}

BOOL CGXEditControl::GetValue(CString& strResult)
{
	// Reads out the window text and converts it into
	// the plain value which should be stored in the style object.

	if (!IsInit())
		return FALSE;

	GetCurrentText(strResult);

	return TRUE;
}

BOOL CGXEditControl::CanFloatCell(ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, BOOL bFloatOrFlood)
{
	nRow, nCol;

	//if (IsActive())
	//	return FALSE;

	// static cells can float over other cells if value is not empty
	if (bFloatOrFlood)
		return _tcslen(style.GetValueRef()) > 0;
	else
		// static cells can be flooded by other floatable cells if value is empty
		return _tcslen(style.GetValueRef()) == 0;
}

BOOL CGXEditControl::OnEndEditing()
{
	if (CGXControl::OnEndEditing())
	{
		Grid()->DelayFloatCells(CGXRange(m_nRow, m_nCol));
		return TRUE;
	}

	return FALSE;
}

void CGXEditControl::SetValue(LPCTSTR pszRawValue)
{
	// Convert the value to the text which should be
	// displayed in the current cell and show this
	// text.

	if (m_hWnd == NULL)
		return;

	if (Grid()->GetParam()->m_nDisplayExpression & GX_EXPR_DISPLAYACTIVE)
	{
		if (pszRawValue)
			SetWindowText(pszRawValue);
		else
			SetWindowText(Grid()->GetExpressionRowCol(m_nRow, m_nCol));
	}
	else
	{
		CString sText;
		GetControlText(sText, m_nRow, m_nCol, pszRawValue, *m_pStyle);
		SetWindowText(sText);
	}

	if (IsActive())
	{
		Grid()->DelayFloatCells(CGXRange(m_nRow, m_nCol));
		Grid()->EvalFloatCells(CGXRange(m_nRow, m_nCol));
		Grid()->EvalVisibleFloatMergeCells(CGXRange(m_nRow, m_nCol, Grid()->GetLastVisibleRow(), Grid()->GetLastVisibleCol()), TRUE);
	}
}

BOOL CGXEditControl::Store()
{
	// Calls SetStyleRange() and resets the modify flag
	ASSERT(m_pStyle);

	CString sValue;
	if (m_pStyle && GetModify() && GetValue(sValue))
	{
		SetActive(FALSE);
		FreeStyle();

		return Grid()->SetExpressionRowCol(
			m_nRow,  m_nCol,
			sValue,
			GX_INVALIDATE);
			// Cell will be automatically redrawn inactive

		// If text is "=xxxx" this will become a formula cell
	}

	return TRUE;
}

void CGXEditControl::Draw(CDC* pDC, CRect rect, ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, const CGXStyle* pStandardStyle)
{
	ASSERT(pDC != NULL && pDC->IsKindOf(RUNTIME_CLASS(CDC)));
	// ASSERTION-> Invalid Device Context ->END
	ASSERT(nRow <= Grid()->GetRowCount() && nCol <= Grid()->GetColCount());
	// ASSERTION-> Cell coordinates out of range ->END

	// Notes on Draw:
	// if (nRow == m_nRow && nCol == m_nCol && m_bIsActive)
	//     initialize CWnd, make it visible and set focus
	// else {
	//     lookup style and draw static, e.g.:
	//     - edit control with GXDrawTextLikeMultiLineEdit
	// }

	ASSERT_VALID(pDC);

	// if (nRow == m_nRow && nCol == m_nCol && m_bIsActive)
	//     initialize CWnd, make it visible and set focus
	if (nRow == m_nRow && nCol == m_nCol && IsActive() && !Grid()->IsPrinting())
	{
		// ------------------ Active Window ---------------------------

		if (s_bImeOpen)
			return;

		NeedStyle();

		// Font

		if (m_bFloatDone && IsWindowVisible())
		{
			CRect rectText = GetCellRect(nRow, nCol, rect);

			// pDC->ExcludeClipRect(rectText); // ?? is it necessary?

			MoveWindow(rectText);

			// Background + Frame
			DrawBackground(pDC, rect, *m_pStyle, TRUE);

			ExcludeClipRect(rectText);

			// child Controls: spin-buttons, hotspot, combobox btn, ...
			CGXControl::Draw(pDC, rect, nRow, nCol, *m_pStyle, pStandardStyle);

			m_bFloatDone = FALSE;
		}
		else
		{
			Hide();

			if (m_font.GetSafeHandle())
				m_font.DeleteObject();

			CFont* pOldFont = 0;

			CGXFont font(m_pStyle->GetFontRef());
			font.SetSize(font.GetSize()*Grid()->GetZoom()/100);
			font.SetOrientation(0);

			// no vertical font for editing the cell
			const LOGFONT& _lf = font.GetLogFontRef();
			if ( !m_font.CreateFontIndirect(&_lf) )
			{
				TRACE1("Failed to create font '%s' in CGXEditControl::Draw\n",
					(LPCTSTR) m_pStyle->GetFontRef().GetDescription());
			}

			// Cell-Color
			COLORREF rgbText = m_rgbWndHilightText;
			COLORREF rgbCell = m_pStyle->GetInteriorRef().GetColor();

			if (Grid()->GetParam()->GetProperties()->GetBlackWhite())
			{
				rgbText = RGB(0,0,0);
				rgbCell = RGB(255,255,255);
			}

			pOldFont = pDC->SelectObject(&m_font);

			TEXTMETRIC    tm;
			pDC->GetTextMetrics(&tm);
			m_dyHeight = tm.tmHeight + tm.tmExternalLeading;
			m_dxMaxCharWidth = tm.tmMaxCharWidth;

			if (pOldFont)
				pDC->SelectObject(pOldFont);

			CRect rectText = GetCellRect(nRow, nCol, rect);

			if (!m_pStyle->GetWrapText()) //  && !m_pStyle->GetAllowEnter())
			{
				// vertical alignment for single line text
				// is done by computing the window rectangle for the CEdit
				if (m_pStyle->GetIncludeVerticalAlignment())
				{
					switch (m_pStyle->GetVerticalAlignment())
					{
					case DT_VCENTER:
						rectText.top += max(0, (rectText.Height() - m_dyHeight) /2);
						break;
					case DT_BOTTOM:
						rectText.top = max(rectText.top, rectText.bottom-m_dyHeight);
					break;
					}
				}
				rectText.bottom = min(rectText.bottom, rectText.top + m_dyHeight);
			}

			DWORD ntAlign = m_pStyle->GetHorizontalAlignment() | m_pStyle->GetVerticalAlignment();

			if (ntAlign & DT_RIGHT)
				rectText.right++;

			// Background + Frame
			DrawBackground(pDC, rect, *m_pStyle, TRUE);

			SetFont(&m_font);

			MoveWindow(rectText, FALSE);
			m_rgbCell = rgbCell;
			m_ntAlign = ntAlign;

			SetCellColor(rgbCell);
			SetTextAlign((UINT) ntAlign);

			if (m_pStyle->GetIncludeVertScrollBar())
				SetVertScrollBar(m_pStyle->GetVertScrollBar());

			if (m_pStyle->GetIncludeAutoSize())
				 m_bAutoSize = m_pStyle->GetAutoSize();

			SetAllowEnter(m_pStyle->GetAllowEnter());
			SetWrapText(m_pStyle->GetWrapText());

			SetActive(TRUE);

#ifdef _UNIX_
			// Wind/U-specific: ShowWindow was being called from within OnShowWindow
			// if the edit style has changed because the edit control
			// is destroyed and recreated if it's styles has changed.
			// This caused a crash on Solaris. I call UpdateEditStyle
			// before ShowWindow to prevent this.
			UpdateEditStyle();
#endif

			if (GetFocus() == GridWnd())
			{
				Grid()->SetIgnoreFocus(TRUE);
				SetFocus();
				Grid()->SetIgnoreFocus(FALSE);
			}

			m_nLimitText = m_pStyle->GetIncludeMaxLength()
				? (int) m_pStyle->GetMaxLength()
				: 0;
			LimitText(m_nLimitText);

#if _MFC_VER < 0x0300
			// scroll caret into view
			int nStart, nEnd;
			GetSel(nStart, nEnd);
			SetSel(nStart, nEnd, FALSE);
#endif

			ShowWindow(SW_SHOW);

			SetReadOnly(IsReadOnly());

			if (nRow > Grid()->GetFrozenRows() && (Grid()->GetTopRow() > nRow 
				|| nCol > Grid()->GetFrozenCols() && Grid()->GetLeftCol() > nCol))
			// Ensure that the window cannot draw outside the clipping area!
			{
				CRect rectClip;
				if (pDC->GetClipBox(&rectClip) != ERROR)
				{
					CRect r = rect & Grid()->GetGridRect();
					GridWnd()->ClientToScreen(&r);
					ScreenToClient(&r);
					GridWnd()->ClientToScreen(&rectClip);
					ScreenToClient(&rectClip);
					ValidateRect(r); 
					InvalidateRect(rectClip); 
					HideCaret();
				}
			}

			UpdateWindow();

			// child Controls: spin-buttons, hotspot, combobox btn, ...
			CGXControl::Draw(pDC, rect, nRow, nCol, *m_pStyle, pStandardStyle);

			ExcludeClipRect(rectText);
		}
	}
	else
	// else {
	//     lookup style and draw static:
	//     - edit control with GXDrawTextLikeMultiLineEdit
	// }
	{

		// ------------------ Static Text ---------------------------

		CGXStatic::Draw(pDC, rect, nRow, nCol, style, pStandardStyle);
	}
}

void CGXEditControl::Hide()
{
	// Hides the CEdit without changing the m_bIsActive flag
	// and without invalidating the screen
	if (m_hWnd && IsWindowVisible())
	{
		Grid()->SetIgnoreFocus(TRUE);
/*
#if _MFC_VER >= 0x0400
		CGXNoImeProxy* pIme = GXGetImeState()->m_pImeProxy;
		if (pIme)
		{
			HIMC hImc = pIme->ImmGetContext(m_hWnd);
			if (hImc)
				pIme->ImmSetOpenStatus(hImc, FALSE);
		}
		s_bImeOpen = FALSE;
#endif
*/
		HideCaret();
		MoveWindow0(FALSE);
		ShowWindow(SW_HIDE);
		Grid()->SetIgnoreFocus(FALSE);
	}
}

// Mouse hit
BOOL CGXEditControl::LButtonUp(UINT nFlags, CPoint pt, UINT nHitState)
{
	if (m_hWnd)
	{
		// Sets the caret position
		int n = CalcTextPosUnderPt(pt);

		if (n != -1 && (Grid()->GetParam()->GetActivateCellFlags() & GX_CAFOCUS_CLICKINTEXT) > 0
			|| Grid()->GetParam()->GetActivateCellFlags()&(GX_CAFOCUS_CLICKONCELL|GX_CAFOCUS_SETCURRENT))


		{
			if (IsActive())
				SetSel(n == -1 ? 0 : n, n, FALSE);
			else if (OnStartEditing())
			{
				SetActive(TRUE);
				SetSel(n == -1 ? 0 : n, n, FALSE);
			}
		}

		if (IsActive() && GetFocus() != this)
			Refresh();
	}

	// check child buttons
	CGXStatic::LButtonUp(nFlags, pt, nHitState);

	return TRUE;
}

BOOL CGXEditControl::LButtonDblClk(UINT nFlags, CPoint pt)
{
	if (m_hWnd)
	{
		if (!IsActive() &&
			(Grid()->GetParam()->GetActivateCellFlags() & GX_CAFOCUS_DBLCLICKONCELL) > 0
			)
		{
			// Set the caret position
			int n = CalcTextPosUnderPt(pt);

			if (IsActive())
				SetSel(n == -1 ? 0 : n, n, FALSE);
			else if (OnStartEditing())
			{
				SetActive(TRUE);
				SetSel(n == -1 ? 0 : n, n, FALSE);
				Refresh();
			}

			// check child buttons
			CGXStatic::LButtonDblClk(nFlags, pt);

			return TRUE;
		}
	}

	return CGXStatic::LButtonDblClk(nFlags, pt);
}

// Keyboard

// called from CGXGridCore when current cell is incative
// (does not have the focus) and before CGXGridCore
// interpretes the key.

BOOL CGXEditControl::OnGridKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!m_hWnd)
		return FALSE;

	BOOL bProcessed = FALSE;

	BOOL bCtl = GetKeyState(VK_CONTROL) & 0x8000;
	BOOL bShift = GetKeyState(VK_SHIFT) & 0x8000;

	// Pass all CTRL keys to the grid
	if (!bCtl)
	{
		switch (nChar)
		{
		case VK_DELETE:
			if (Grid()->GetParam()->GetRangeList()->IsEmpty() // no ranges of cells selected
				&& !IsReadOnly()        // cell is not readonly
				&& !bShift
				&& OnDeleteCell()       // OnDeleteCell notification returns TRUE
				&& OnStartEditing())    // OnStartEditing notification returns TRUE
			{
				SetActive(TRUE);

				// Delete text
				SetModify(TRUE);
				SetCurrentText(_T(""));

				bProcessed = TRUE;
			}
			break;

		case VK_END:
			if (OnStartEditing())
			{
				if (!IsInit())
					Init(m_nRow, m_nCol);
				SetActive(TRUE);

				CString s;
				GetWindowText(s);
				int nLen = _tcslen(s);

				// position caret
				SetSel(nLen, nLen, FALSE);

				bProcessed = TRUE;
			}
			break;

		case VK_HOME:
			if (OnStartEditing())
			{
				if (!IsInit())
					Init(m_nRow, m_nCol);
				SetActive(TRUE);

				// position caret
				SetSel(0, 0, FALSE);

				bProcessed = TRUE;
			}
			break;
		}
	}

	if (bProcessed)
	{
		// eventually destroys and creates CEdit with appropriate window style
		UpdateEditStyle();

		if (!Grid()->ScrollCellInView(m_nRow, m_nCol))
			Refresh();

		return TRUE;
	}

	return CGXStatic::OnGridKeyDown(nChar, nRepCnt, nFlags);

}

BOOL CGXEditControl::OnGridChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!m_hWnd)
		return FALSE;

	// is this a valid character (no Ctrl-Key)
	CString s = (TCHAR) nChar;

	// pass valid characters as string to OnGridDoubleByteChar
	if (nChar >= 0x020 && nChar != 0x7f)
		return OnGridDoubleByteChar((CString) (TCHAR) nChar, nRepCnt, nFlags);

	// default processing
	return CGXControl::OnGridChar(nChar, nRepCnt, nFlags);
}

BOOL CGXEditControl::OnGridDoubleByteChar(const CString& sChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nRepCnt, nFlags;

	if (IsReadOnly())
		return FALSE;

	if (IsActive())
	{
		CString sEdit;
		GetWindowText(sEdit);

		int nStartChar, nEndChar;
		GetSel(nStartChar, nEndChar);

		if (nEndChar == -1)
			sEdit = sChar;
		// else if (nEndChar > nStartChar && nEndChar+1 < sEdit.GetLength())
		// 	sEdit = sEdit.Left(nStartChar) + sChar + sEdit.Mid(nEndChar+1);
		else
			sEdit = sEdit.Left(nStartChar) + sChar + sEdit.Mid(nEndChar);

		if (ValidateString(sEdit))
		{

			m_bDoNotEmptyLastChar = TRUE;
			ReplaceSel(sChar);
			m_bDoNotEmptyLastChar = FALSE;
			nStartChar += sChar.GetLength();
			SetSel(nStartChar, nStartChar, FALSE);
		}

		return TRUE;
	}
	else
	{
		// discard key if cell is read only
		if (IsReadOnly() || !ValidateString(sChar) || !OnStartEditing())
			return FALSE;

		SetActive(TRUE);

		SetCurrentText(sChar);
		SetSel(sChar.GetLength(), sChar.GetLength(), FALSE);
		Refresh();

#if _MFC_VER >= 0x0300 && _MFC_VER < 0x0420
		// Fix some weird problems with Win32s
		if (GXGetAppData()->bWin31)
		{
			CString s;
			GridWnd()->UpdateWindow();
			GetWindowText(s);
			SetSel(s.GetLength(), s.GetLength(), FALSE);
		}
#endif

		return TRUE;
	}
}

BOOL CGXEditControl::Cut()
{
	return CGXControl::Cut();
}

BOOL CGXEditControl::Copy()
{
	return CGXControl::Copy();
}

BOOL CGXEditControl::Paste()
{
	// Paste
	BOOL bSuccess = CGXControl::Paste();

	return bSuccess;
}

void CGXEditControl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT flags)
{
	if (!IsActive())
	{
		Grid()->ProcessKeys(this, WM_KEYDOWN, nChar, nRepCnt, flags);
		return;
	}

	BOOL bProcessed = FALSE;
	BOOL bCallDefault = TRUE;

	// TRACE("CGXEditControl::OnKeyDown nChar = %d\n", nChar);

	BOOL bCtl = GetKeyState(VK_CONTROL) & 0x8000;
	BOOL bShift = GetKeyState(VK_SHIFT) & 0x8000;

	Grid()->ScrollCellInView(m_nRow, m_nCol);

	// Pass all CTRL keys to the grid
	if (!bCtl)
	{
		// window text and caret position
		CString s;
		int ns, nr;

		GetWindowText(s);
		GetSel(ns, nr);

		switch (nChar)
		{
		case VK_RIGHT:
			// process right arrow if not at end of string
			bCallDefault = ns < (int) s.GetLength();
			break;

		case VK_LEFT:
			// process left arrow if not at start of string
			bCallDefault = nr > 0;
			break;

		case VK_DOWN:
			// process down arrow if not at last line
			bCallDefault = LineFromChar(nr)+1 < GetLineCount();
			break;

		case VK_UP:
			// process up arrow if not at first line
			bCallDefault = LineFromChar(ns) > 0;
			break;

		case VK_RETURN:
			// does edit control want to handle the RETURN key itsself?
			bCallDefault = (GetStyle() & ES_WANTRETURN) > 0;
			break;

		case VK_F1:
		case VK_F2:
		case VK_F3:
		case VK_F4:
		case VK_F5:
		case VK_F6:
		case VK_F7:
		case VK_F8:
		case VK_F9:
		case VK_F10:
		case VK_F11:
		case VK_F12:
		case VK_F13:
		case VK_F14:
		case VK_F15:
		case VK_F16:
		case VK_F17:
		case VK_F18:
		case VK_F19:
		case VK_F20:
		case VK_F21:
		case VK_F22:
		case VK_F23:
		case VK_F24:
		case VK_TAB:
		case VK_NEXT:
		case VK_PRIOR:
		case VK_ESCAPE:
			// let grid handle escape key
			bCallDefault = FALSE;
			break;

		case VK_DELETE:
			{
				// if we have a multiple selection, handle it differently
				if (ns != nr && bShift)
				{
					OpenClipboard();
					EmptyClipboard();
					CloseClipboard();

					Cut();
					bCallDefault = FALSE;
					bProcessed = TRUE;
				}
				else
					bCallDefault = TRUE;
				break;
			}

		case VK_INSERT:
			{
				if (bShift)
				{
					Paste();
					bCallDefault = FALSE;
					bProcessed = TRUE;
				}
				else
					bCallDefault = TRUE;
				break;
			}

		case VK_HOME:
		case VK_END:
		case VK_SHIFT:
		default:
			// let CEdit handle insert key
			// let CEdit handle delete and Insert key
			// let CEdit handle all other keys
			bCallDefault = TRUE;
			break;
		}

		if (bCallDefault)
		{
			CEdit::OnKeyDown(nChar, nRepCnt, flags);
			bProcessed = TRUE;
		}

	}

	if (!bProcessed)
		Grid()->ProcessKeys(this, WM_KEYDOWN, nChar, nRepCnt, flags);
}

void CGXEditControl::OnChar(UINT nChar, UINT nRepCnt, UINT flags)
{
	m_bFloatDone = FALSE;
	if (OnProcessChar(nChar, nRepCnt, flags))
	{
		CEdit::OnChar(nChar, nRepCnt, flags);
		Grid()->ProcessKeys(this, WM_CHAR, nChar, nRepCnt, flags);
	}
	else if (!m_sLastChar.IsEmpty())
	{
		int ns, ne;
		GetSel(ns, ne);
		ns += m_sLastChar.GetLength();
		m_bDoNotEmptyLastChar = TRUE;
		ReplaceSel(m_sLastChar);
		m_bDoNotEmptyLastChar = FALSE;
		if (m_bFloatDone)
		{
			SetSel(0, 0, FALSE);
			GridWnd()->UpdateWindow();
			SetSel(ns, ns, TRUE);
		}
		else
			SetSel(ns, ns);
	}
	m_bFloatDone = FALSE;
}

BOOL CGXEditControl::OnProcessChar(UINT nChar, UINT nRepCnt, UINT flags)
{
	// UNUSED:
	nRepCnt, flags;

	NeedStyle();

	if (!IsInit()
		|| !IsActive()
		|| IsReadOnly()
		|| (GetStyle() & ES_READONLY)
#if _MFC_VER >= 0x0300
		// special handling for pen windows ...
		|| nChar != 8 && nRepCnt == 0 && !::GetSystemMetrics(SM_PENWINDOWS)
#endif
		)
	{
		m_sLastChar.Empty();
		return FALSE;
	}

	BOOL bCallDefault = TRUE;

	// TRACE(_T("CGXEditControl::OnChar nChar = %d\n"), nChar);

	BOOL bCtl = GetKeyState(VK_CONTROL) & 0x8000;

	int ns, ne;
	GetSel(ns, ne);

	Grid()->ScrollCellInView(m_nRow, m_nCol);

	if (!m_bDoNotEmptyLastChar)
		m_sLastChar.Empty();

	if (bCtl)
	{
		switch (nChar)
		{
		case 24:    // CTRL+X
			OpenClipboard();
			EmptyClipboard();
			CloseClipboard();

			Cut();
			bCallDefault = FALSE;
			break;

		case 3:     // CTRL+C
			OpenClipboard();
			EmptyClipboard();
			CloseClipboard();

			Copy();
			bCallDefault = FALSE;
			break;

		case 22:    // CTRL+V
			Paste();
			bCallDefault = FALSE;
			break;
		}

		// Pass all other CTRL keys to the grid
	}

	if (!bCtl)
	{
		// support for DBCS characters
		BOOL bChar = FALSE;
		CString sChar;

#if _MFC_VER >= 0x0400
		// Korean Win95 behaves a bit different
		// Therefore we have to check for LANG_KOREAN 

		BOOL    bDoubleByteChar = m_bDoubleByteChar;
		TCHAR   nDoubleByteLeadChar = m_nDoubleByteLeadChar;

		if (GXGetDoubleByteChar((TCHAR) nChar, sChar, bDoubleByteChar, nDoubleByteLeadChar))
		{
			WORD langId = LANGIDFROMLCID(::GetThreadLocale());

			if (PRIMARYLANGID(langId) != LANG_KOREAN || !bDoubleByteChar)
			{
				m_bDoubleByteChar = bDoubleByteChar;
				m_nDoubleByteLeadChar = nDoubleByteLeadChar;

				if (sChar.IsEmpty())
					return FALSE;   // leading byte

				// double byte character in sChar
				bChar = TRUE;
			}
		}
#else
		if (GXGetDoubleByteChar((TCHAR) nChar, sChar, m_bDoubleByteChar, m_nDoubleByteLeadChar))
		{
			if (sChar.IsEmpty())
				return FALSE;   // leading byte

			// double byte character in sChar
			bChar = TRUE;
		}
#endif
		else if (nChar >= 0x020) // this is a normal character
		{
			sChar = (TCHAR) nChar;
			bChar = TRUE;
		}

		// Is it an alphanumeric or double byte character?
		if (bChar)
		{
			m_sLastChar = sChar;

			// validate input
			CString sEdit;
			GetWindowText(sEdit);

			if (ne == -1)
				sEdit = m_sLastChar;
			else
				sEdit = sEdit.Left(ns) + m_sLastChar + sEdit.Mid(ne);

			if (!ValidateString(sEdit))
			{
				OnInvalidKeyPressed(m_sLastChar);
				m_sLastChar.Empty();
			}

			bCallDefault = FALSE;
		}
	}
	else if (
		GetStyle() & ES_WANTRETURN && nChar == 13
		|| nChar == 10 && m_pStyle->GetWrapText())
	{
		// insert a cr/lf
		m_sLastChar = _T("\r\n");
	}
	else if (nChar == 13 || nChar == 10)
	{
		m_sLastChar.Empty();
		bCallDefault = FALSE;
	}

	return bCallDefault && m_sLastChar.IsEmpty();
}

void CGXEditControl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (!Grid()->ProcessKeys(this, WM_KEYUP, nChar, nRepCnt, nFlags))
    {
        CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
        if (nChar != VK_SHIFT  &&
            nChar != VK_HOME  &&  nChar != VK_END  &&  
            nChar != VK_DOWN  &&  nChar != VK_UP)
        {
            UpdateEditStyle();
        }
    }
}

void CGXEditControl::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!Grid()->ProcessKeys(this, WM_SYSKEYDOWN, nChar, nRepCnt, nFlags))
		CEdit::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CGXEditControl::OnSysChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!Grid()->ProcessKeys(this, WM_SYSCHAR, nChar, nRepCnt, nFlags))
		CEdit::OnSysChar(nChar, nRepCnt, nFlags);
}

void CGXEditControl::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!Grid()->ProcessKeys(this, WM_SYSKEYUP, nChar, nRepCnt, nFlags))
		CEdit::OnSysKeyUp(nChar, nRepCnt, nFlags);
}

void CGXEditControl::OnLButtonDown(UINT nFlags, CPoint point)
{
	ASSERT_VALID(this);

	int nOldStartSel, nOldEndSel;
	GetSel(nOldStartSel, nOldEndSel);

	// call the base class so the caret gets set right
	CEdit::OnLButtonDown(nFlags, point);
	           
	if (!IsWindowVisible())
	{
		SetActive(TRUE);
		Refresh();      
	}

	// check if user clicked into selectect text
	int nStartSel, nEndSel;
	GetSel(nStartSel, nEndSel);

	if (nOldStartSel != nOldEndSel
		&& nStartSel == nEndSel
		&& nStartSel >= nOldStartSel
		&& nStartSel <= nOldEndSel)
	{
		// Keep old selection
		SetSel(nOldStartSel, nOldEndSel);

		// Start Drag&Drop
		Grid()->DndStartDragDrop(m_nRow, m_nCol);

		// if internal state is still valid
		if (!IsInit() || !IsActive() || !Grid()->IsCurrentCell(m_nRow, m_nCol))
			return;

		// ... continue
		SetSel(nStartSel, nEndSel);
	}
}

BOOL CGXEditControl::OnCommand(WPARAM wParam, LPARAM lParam)
{
#if _MFC_VER < 0x0300
	UINT nNotification = HIWORD(lParam);
	HWND hCtl = (HWND) LOWORD(lParam);
#else
	UINT nNotification = HIWORD(wParam);
	HWND hCtl = (HWND) lParam;
#endif

	if (hCtl == m_hWnd && IsActive())
	{
		if (nNotification == EN_CHANGE)
		{
			OnModifyCell();

			Grid()->DelayFloatCells(CGXRange(m_nRow, m_nCol));
			if (Grid()->EvalFloatCells(CGXRange(m_nRow, m_nCol)))
			{
				m_bFloatDone = TRUE;
			}
			else if (m_bAutoSize)
			{
				CGXRange rgCovered(m_nRow, m_nCol);
				Grid()->GetCellRangeRowCol(m_nRow, m_nCol, rgCovered);
#if _MFC_VER >= 0x0300
				if (m_bWrapText)
					Grid()->ResizeRowHeightsToFit(rgCovered, TRUE, GX_UPDATENOW|GX_NOSHRINKSIZE);
				else
					Grid()->ResizeColWidthsToFit(rgCovered, TRUE, GX_UPDATENOW|GX_NOSHRINKSIZE);
				LineScroll(255, 0);
#else
				BOOL bChanged = FALSE;

				if (m_bWrapText)
				{				
					int nOldHeight = Grid()->GetRowHeight(rgCovered.bottom);
					Grid()->ResizeRowHeightsToFit(rgCovered, TRUE, GX_UPDATENOW|GX_NOSHRINKSIZE);
					bChanged = Grid()->GetRowHeight(rgCovered.bottom) != nOldHeight;
				}
				else
				{
					int nOldWidth = Grid()->GetColWidth(rgCovered.right);
					Grid()->ResizeColWidthsToFit(rgCovered, TRUE, GX_UPDATENOW|GX_NOSHRINKSIZE);
					bChanged = Grid()->GetColWidth(rgCovered.right) != nOldWidth;
				}
                 
				if (bChanged)
				{                 
					int nStart, nEnd;
					GetSel(nStart, nEnd);
					SetSel(0, 0);
					SetSel(nStart, nEnd);
				}
#endif
			}

			return TRUE;
		}
	
		CEdit::OnCommand(wParam, lParam);
	}

	return CGXControl::OnCommand(wParam, lParam);
}

// Find and Replace
BOOL CGXEditControl::FindText(const GX_FR_STATE& find, ROWCOL nRow, ROWCOL nCol, BOOL bCurrentPos, BOOL bSetCell)
{
	LPCTSTR szValue = NULL;
	LPCTSTR pszFound = NULL;
	LPCTSTR szFind = find.strFindUpper;
		// strFindUpper contains the string to search.
		// if bCase is FALSE (case insensitive search),
		// the text in strFindUpper is converted to uppercase

	// if the active cell contains the text, mark it and return TRUE
	// if not, do a find and return FALSE
	if (nRow == m_nRow && nCol == m_nCol && IsActive())
	{
		CString s;

		int nStart, nEnd;

		GetSel(nStart, nEnd);
		GetWindowText(s);

		// Convert text in s to upper case if we are doing a case insensitive search
		if (!find.bCase)
			s.MakeUpper();

		LPCTSTR pv = szValue = s;

		if (nStart != -1 && nEnd != -1)
			szValue += nStart + (bCurrentPos ? 0 : _tclen(szValue + nStart));

		pszFound = _tcsstr(szValue, szFind);

		if (pszFound != NULL)
		{
			if (bSetCell)
			{
				int nStart = (pszFound - pv);

				// mark the text I have found
				Grid()->ScrollCellInView(nRow, nCol);
				SetSel(nStart, nStart+_tcslen(szFind));
			}
			return TRUE;
		}
	}
	else
	{
		CString s;
		BOOL b;
		if (find.m_pStyle)
		{
			b = GetControlText(s, nRow, nCol, NULL, *find.m_pStyle);
		}
		else
		{
			// request a style object
			CGXStyle* pStyle = Grid()->CreateStyle();

			Grid()->ComposeStyleRowCol(nRow, nCol, pStyle);

			b = GetControlText(s, nRow, nCol, NULL, *pStyle);

			// free style object
			Grid()->RecycleStyle(pStyle);
		}

		if (b)
		{
			// Convert text to upper case if we are doing a case insensitive search
			if (!find.bCase)
				s.MakeUpper();

			szValue = s;
		}
		else
			szValue = _T("");

		pszFound = _tcsstr(szValue, szFind);

		if (pszFound != NULL)
		{
			if (bSetCell)
			{
//				int nStart = (pszFound - szValue);

				// mark the text I have found
				Grid()->ScrollCellInView(nRow, nCol);
				Grid()->SetCurrentCell(nRow, nCol, GX_SMART|GX_NOSETFOCUS);
				SetActive(TRUE);
				FindText(find, nRow, nCol, TRUE, TRUE);
//				SetSel(nStart, nStart+_tcslen(szFind));
				Refresh();
			}
			return TRUE;
		}
	}

	return FALSE;
}

void CGXEditControl::ReplaceSel(LPCTSTR pszReplaceText)
{
	if (IsActive())
	{
		if (!IsReadOnly())
		{
			SetModify(TRUE);

			CEdit::ReplaceSel(pszReplaceText);
		}
	}
	else
		CGXControl::ReplaceSel(pszReplaceText);
}

BOOL CGXEditControl::ReplaceAll(const GX_FR_STATE& find, ROWCOL nRow, ROWCOL nCol)
{
	BOOL bFound = FALSE;
	LPCTSTR szValue = NULL;
	LPCTSTR szTemp = NULL;
	LPCTSTR pszFound = NULL;
	LPCTSTR szReplace = find.strReplace;
	LPCTSTR szFind = find.strFindUpper;
		// strFindUpper contains the string to search.
		// if bCase is FALSE (case insensitive search),
		// the text in strFindUpper is converted to uppercase

	CString s, sTemp;

	int nStart = 0, nEnd = -1;

	// request a style object
	CGXStyle* pStyle = NULL;

	// if the active cell contains the text, mark it and return TRUE
	// if not, do a find and return FALSE
	if (nRow == m_nRow && nCol == m_nCol && IsActive())
	{
		GetSel(nStart, nEnd);
		GetWindowText(s);
	}
	else if (find.m_pStyle)
	{
		pStyle = find.m_pStyle;
		GetControlText(s, nRow, nCol, NULL, *find.m_pStyle);
	}
	else
	{
		// request a style object
		pStyle = Grid()->CreateStyle();

		Grid()->ComposeStyleRowCol(nRow, nCol, pStyle);

		GetControlText(s, nRow, nCol, NULL, *pStyle);
	}

	// Convert text in s to upper case if we are doing a case insensitive search
	sTemp = s;
	if (!find.bCase)
		sTemp.MakeUpper();
	szValue = s;
	szTemp = sTemp;

	nStart = max(0, nStart);

	int nReplace = _tcslen(szReplace);
	int nFind = _tcslen(szFind);

	// result text
	TCHAR szResult[1024];
	_tcsncpy(szResult, szValue, nStart);
	LPTSTR pszResult = szResult + nStart;

	// original text
	LPCTSTR pszValue = szValue + nStart;

	// text to compare (upper case if case insensitive search)
	LPCTSTR pszTemp  = szTemp + nStart;

	do
	{
		pszFound = _tcsstr(pszTemp, szFind);

		if (pszFound == NULL)
			// copy remaining after last match
			_tcscpy(pszResult, pszValue);
		else
		{
			bFound = TRUE;

			// copy text before match
			_tcsncpy(pszResult, pszValue, pszFound-pszTemp);
			pszResult += pszFound-pszTemp;

			// append replaced text
			_tcsncpy(pszResult, szReplace, nReplace);
			pszResult += nReplace;
			*pszResult = _T('\0');

			// increase pointers in original text (skip matching text)
			pszValue += pszFound-pszTemp+nFind;
			pszTemp = pszFound+nFind;

			// just to be sure ...
			ASSERT(pszValue == szValue + (pszTemp-szTemp));
		}
	}
	while (pszFound && *pszTemp);

	if (bFound)
		Grid()->SetControlTextRowCol(nRow, nCol, szResult, GX_UPDATENOW, pStyle ? pStyle : m_pStyle);

	if (find.m_pStyle == NULL && pStyle)
	{
		// free style object
		Grid()->RecycleStyle(pStyle);
	}

	return bFound;
}

BOOL CGXEditControl::GetSelectedText(CString& sWindowText)
{
	if (IsActive())
	{
		// Get selected text
		int nStartChar = 0,
			 nEndChar = -1;

		GetSel(nStartChar, nEndChar);
		GetWindowText(sWindowText);

		// if no text is selected use all text
		if (nEndChar == nStartChar)
			return FALSE;   // no text selected

		else
		{
			// if nEndChar is -1 get whole text
			int nLength = (nEndChar == -1) ? GetWindowTextLength() : nEndChar-nStartChar;

			// Copy text to strResult
			sWindowText = sWindowText.Mid(nStartChar, nLength);

			return TRUE;    // text selected
		}
	}
	else
	{
		// No caret visible. Get text which is displayed in the cell
		GetCurrentText(sWindowText);

		return FALSE;   // no text selected
	}
}

// Edit specific

int CGXEditControl::CalcTextPosUnderPt(CPoint point)
{
	// used to compute textposition after mouseclick

	NeedStyle();
	CRect rect = GetCellRect(m_nRow, m_nCol);

	CString s;
	GetWindowText(s);

	DWORD ntAlign = m_pStyle->GetHorizontalAlignment() | m_pStyle->GetVerticalAlignment();

	if (m_pStyle->GetWrapText())
		ntAlign |= DT_NOPREFIX | DT_WORDBREAK;
	else
		ntAlign |= DT_NOPREFIX | DT_SINGLELINE;

	CClientDC dc(GridWnd());

	CFont* pOldFont = Grid()->LoadFont(&dc, *m_pStyle);

	int nHitChar = GXGetMultiLineTextPosUnderPt(
		&dc, s, -1, rect,
		(UINT) ntAlign, point);

	if (pOldFont)
	{
		if ((pOldFont = dc.SelectObject(pOldFont)) != NULL)
			pOldFont->DeleteObject();
	}

	return nHitChar;
}

void CGXEditControl::SetWrapText(BOOL b)
{
	m_bWrapText = b;
}

void CGXEditControl::SetAllowEnter(BOOL b)
{
	m_bAllowEnter = b;
}

void CGXEditControl::SetVertScrollBar(BOOL b)
{
	m_bVertScrollBar = b;
}

void CGXEditControl::SetCellColor(COLORREF rgb)
{
	m_rgbCell = rgb;
}

void CGXEditControl::SetTextAlign(UINT ntAlign)
{
	m_ntAlign = AlignTAToES(AlignDTToTA(ntAlign));
}

BOOL CGXEditControl::UpdateEditStyle()
{
	if (m_bUpdateEditStyle || !m_hWnd)
		// semaphor for not recursively re-entering UpdateEditStyle()
		return FALSE;

	// TRACE0("CGXEditControl: BEGIN UpdateEditStyle()\n");

	int nLine;
	RECT rect;
	GetClientRect(&rect);

	RECT rectNP;
	DWORD dwEditStyle = CalcEditStyle(&rectNP, m_dyHeight);
	BOOL bChanged = m_dwEditStyle != dwEditStyle;

	int ns, ne;
	GetSel(ns, ne);

	if (bChanged)
	{
		m_bUpdateEditStyle = TRUE;

		BOOL bModify   = GetModify();
		BOOL bReadOnly = (BOOL) (GetStyle() & ES_READONLY);

		MapWindowPoints(GridWnd(), &rect);

		CString s;
		GetWindowText(s);

		CFont *pFont = GetFont();

		HideCaret();
		MoveWindow0(FALSE);

		Grid()->SetIgnoreFocus(TRUE);

		DestroyWindow();

		// TRACE0("CGXEditControl: Destroying and recreating EDIT-Window\n");

		VERIFY(CreateControl(m_dwEditStyle = dwEditStyle, &rect));

#if !defined(_UNIX_) && !defined(_MAC)
		// not supported by Wind/U and Mac
		SetRect(&rectNP);
#endif
		SetFont(pFont);
		SetWindowText(s);
		SetSel(ns, ne, FALSE);
		SetModify(bModify);
		SetReadOnly(bReadOnly);
		LimitText(m_nLimitText);

		m_bUpdateEditStyle = FALSE;
		nLine = LineFromChar(ne);

		HideCaret();
		UpdateWindow();
		ShowCaret();
#if _MFC_VER >= 0x0300
		SendMessage(EM_SCROLLCARET, 0, 0);
#endif

		SetFocus();

		Grid()->SetIgnoreFocus(FALSE);
	}
	else
	{
		nLine = LineFromChar();
#if !defined(_UNIX_) && !defined(_MAC)
		// not supported by Wind/U and Mac
		SetRectNP(&rectNP);
#endif
	}

	int nLast = GetLastVisibleLine(&rect);
	int nFirst = GetFirstVisibleLine();

	// TRACE(_T("CGXEditControl: FirstVisible=%d,LineFromChar=%d,LastVisible=%d\n"),
	//  nFirst, nLine, nLast);

	if (nLine > nLast || nLine < nFirst)
	{
		HideCaret();

		if (nLine > nLast)
			LineScroll(nLine - nLast);
		else
			LineScroll(nLine - nFirst);
#if _MFC_VER < 0x0300
		SetSel(ns, ne, FALSE);
#else
		SendMessage(EM_SCROLLCARET, 0, 0);
#endif
		UpdateWindow();
		ShowCaret();
	}
	else
	{
#if _MFC_VER >= 0x0300
		SendMessage(EM_SCROLLCARET, 0, 0);
#endif
		;
	}

	// TRACE1("CGXEditControl: END   UpdateEditStyle() RET %d\n", bChanged);

	return bChanged;
}

int CGXEditControl::GetLastVisibleLine(LPRECT lprect)
{
	RECT rect;

	if (lprect)
		rect = *lprect;
	else
		GetClientRect(&rect);

	if (m_dyHeight == 0)
	{
		CDC *pDC = GetDC();
		CFont *pOldFont = pDC->SelectObject(GetFont());

		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);
		m_dyHeight = tm.tmHeight + tm.tmExternalLeading;
		m_dxMaxCharWidth = tm.tmMaxCharWidth;

		ReleaseDC(pDC);
		if (pOldFont)
			pDC->SelectObject(pOldFont);
	}

	return GetFirstVisibleLine()+max(0, (rect.bottom-rect.top)/m_dyHeight);
}

DWORD CGXEditControl::CalcEditStyle(LPRECT rectNP, int& dyHeight)
{
	// TRACE0("CGXEditControl: BEGIN CalcEditStyle()\n");

	// figure out edit control style
	DWORD dwStyle   = WS_VISIBLE | WS_CHILD | WS_TABSTOP;
	LONG  ntAlign   = m_bAlwaysLeftJustified ? ES_LEFT : m_ntAlign;

	CRect rect;
	GetClientRect(&rect);

	// Device Context needed
	CDC* pDC = GetDC();

	CFont *pOldFont = NULL;
	CFont* pFont = GetFont();
	if (pFont)
		pOldFont = pDC->SelectObject(pFont);

	CString s;
	GetWindowText(s);

	CSize size = pDC->GetTextExtent(s, s.GetLength());

	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	m_dyHeight = tm.tmHeight + tm.tmExternalLeading;
	m_dxMaxCharWidth = tm.tmMaxCharWidth;

	UINT nFormat = 0;
	if (m_bWrapText)
		nFormat |= DT_NOPREFIX | DT_WORDBREAK;
	else
		nFormat |= DT_NOPREFIX | DT_SINGLELINE;

	// Calculate no of lines needed to display text
	GXGetMultiLineTextBreakCount(pDC, s, -1, rect, nFormat);

	if (pOldFont)
		pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);

	// ES_MULTILINE | ES_AUTOVSCROLL is needed to display the editwnd
	// with horizontal Alignment
	DWORD dwEditStyle = ES_NOHIDESEL | ES_MULTILINE;

	// if (!m_bAutoSize)
		dwEditStyle |= ES_AUTOVSCROLL;
	// using ES_AUTOHSCROLL only would always result in left-aligned text

	if (m_bAllowEnter)
		dwEditStyle |= ES_WANTRETURN;

	if (!m_bWrapText || !m_bVertScrollBar && (rect.Height()-dyHeight/3) <= dyHeight)
	{
		// cell is only a singe line

//      if (!m_bAutoSize)
			dwEditStyle |= ES_AUTOHSCROLL;

		// switch right aligned or centered edit control
		// into a left aligned one, so that autoscrolling
		// works correctly.
		if (size.cx > rect.Width()-m_dxMaxCharWidth)
			ntAlign = ES_LEFT;
	}
	else
	{
		// display Scrollbar if text needs more lines than visible
		if (m_bVertScrollBar)
			dwStyle |= WS_VSCROLL;
	}

	if (rectNP)
		*rectNP = rect;

	rectNP->bottom = max(rectNP->bottom, dyHeight+1);

	dwEditStyle = MAKELONG(((WORD) ntAlign | (WORD) dwEditStyle), HIWORD(dwStyle));

	// TRACE1("CGXEditControl: END   CalcEditStyle() RET %08lx\n", dwEditStyle);

	return dwEditStyle;
}


void CGXEditControl::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	CGXControl::OnKillFocus(pNewWnd);
}

UINT CGXEditControl::OnGetDlgCode()
{
	return CEdit::OnGetDlgCode() | DLGC_WANTTAB | DLGC_WANTARROWS | DLGC_WANTCHARS;
}

BOOL CGXEditControl::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(rect);

	// Erase Background
	// GXPatB(pDC, rect, m_rgbCell);
	GXPatB(pDC, rect, RGB(255,255,255));
	NeedStyle();
	if (m_pStyle)
		pDC->FillRect(rect, Grid()->LookupOrCreateBrush(m_pStyle->GetInteriorRef()));

	return TRUE;
}

void CGXEditControl::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (bShow)
		UpdateEditStyle();

	CEdit::OnShowWindow(bShow, nStatus);
}

void CGXEditControl::OnSetFocus(CWnd* pOldWnd)
{
	CEdit::OnSetFocus(pOldWnd);

	UpdateEditStyle();
}

#if _MFC_VER >= 0x0400
void CGXEditControl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	pWnd, point;
}
#endif
