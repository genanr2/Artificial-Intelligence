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

// gxcradio.cpp : implementation of the CGXRadioButton control
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

#ifndef _GXDLL_H_
#include "gxdll.h" // GXGetResourceHandle
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCRADIO")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_CONTROL(CGXRadioButton, CGXControl);

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CGXRadioButton control

const int nRadioBtnSize = 12;

static void DrawRadioButton(CDC* pDC, CRect rect, BOOL bSelected, BOOL bDraw3D, CDC* pSrcDC)
{
	// center button
	rect.top += (rect.Height()-nRadioBtnSize)/2;
	rect.bottom = rect.top + nRadioBtnSize;
	rect.left += nRadioBtnSize / 4;
	rect.right = rect.left + nRadioBtnSize;

	// TRACE(_T("%d,%d,%d,%,d\n"), rect);

	if (!bDraw3D)
	{
		// Pen
		CPen pen;
		pen.CreateStockObject(BLACK_PEN);
		CPen* pOldPen = pDC->SelectObject(&pen);

		// Brush
		CBrush brush;
		brush.CreateStockObject(WHITE_BRUSH);
		CBrush* pOldBrush = pDC->SelectObject(&brush);

		pDC->Ellipse(rect);

		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);

		if (bSelected)
		{
			rect.top += 2;
			rect.left += 2;
			rect.bottom -= 2;
			rect.right -= 2;

			CBrush brush;
			brush.CreateStockObject(BLACK_BRUSH);
			CBrush* pOldBrush = pDC->SelectObject(&brush);

			pDC->Ellipse(rect);
			pDC->SelectObject(pOldBrush);
		}

	}
	else
	{
		const int dx = 14, dy = 13;

		// normal image version
		pDC->BitBlt(rect.left, rect.top,
			dx, dy,
			pSrcDC, bSelected ? dx : 0, dy, SRCCOPY);
	}
}

int AFXAPI GXDrawRadioButtons(CDC* pDC, CRect rect, LPCTSTR pszValue, LPCTSTR pszChoices, BOOL bDraw3D)
{
	ASSERT(pszValue);
	ASSERT(pszChoices);
	ASSERT_VALID(pDC);

	// text for radio buttons
	LPTSTR pszItems = new TCHAR[_tcslen(pszChoices)+1];
	_tcscpy(pszItems, pszChoices);

	// item height
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);

	int dyHeight = max(nRadioBtnSize, tm.tmHeight + tm.tmExternalLeading);

	// Current selection
	int nCurSel = -1;
	if (pszValue && *pszValue)
		nCurSel = _ttoi(pszValue);

	// Clipping
	int nSaveDC = pDC->SaveDC();
	pDC->IntersectClipRect(rect);

	CDC memDC;
	CBitmap* pOldBitmap = NULL;

	if (bDraw3D)
	{
		// load resources from OG dll
		if (GXGetDrawState()->m_bm3dCtrl.GetSafeHandle() == NULL)
		{
			// save resource handle
			HINSTANCE hSaveResource = AfxGetResourceHandle();

			// switch it to dll
			AfxSetResourceHandle(GXGetResourceHandle());
			// ->END

			GXGetDrawState()->m_bm3dCtrl.LoadBitmap(GX_IDB_3DCTRL);

			// reset resource handle
			AfxSetResourceHandle(hSaveResource);
		}

		memDC.CreateCompatibleDC(pDC);
		pOldBitmap = memDC.SelectObject(&GXGetDrawState()->m_bm3dCtrl);
		if (pOldBitmap == NULL)
			return 0;     // destructors will clean up
	}

	// Draw each item
	int nRadio = 0;
	LPTSTR token = _tcstok(pszItems, _T("\r\n"));
	while (token != 0)
	{
		if (*token)
		{
			CRect rcItem = CRect(rect.left,
				rect.top + nRadio * dyHeight,
				rect.right,
				rect.top + (nRadio+1) * dyHeight);

			DrawRadioButton(pDC, rcItem, nRadio++ == nCurSel, bDraw3D, &memDC);

			rcItem.left += nRadioBtnSize + nRadioBtnSize/2;

			pDC->DrawText(token, _tcslen(token), rcItem, DT_SINGLELINE | DT_LEFT);
		}

		token = _tcstok(NULL, _T("\r\n"));
	}

	if (bDraw3D)
	{
		if (pOldBitmap)
			memDC.SelectObject(pOldBitmap);
	}

	// clean up
	pDC->RestoreDC(nSaveDC);

	delete pszItems;

	return nRadio;
}

int AFXAPI GXHitTestRadioButtons(CDC* pDC, CRect rect, LPCTSTR pszChoices, CPoint pt)
{
	ASSERT(pszChoices);
	ASSERT_VALID(pDC);

	// text for radio buttons
	LPTSTR pszItems = new TCHAR[_tcslen(pszChoices)+1];
	_tcscpy(pszItems, pszChoices);

	// item height
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);

	int dyHeight = max(nRadioBtnSize, tm.tmHeight + tm.tmExternalLeading);

	// check each item
	int nRadio = 0;
	BOOL bFound = FALSE;
	LPTSTR token = _tcstok(pszItems, _T("\r\n"));
	while (token != 0)
	{
		if (*token)
		{
			// Is Button.hit?
			CRect rcItem = CRect(rect.left,
				rect.top + nRadio * dyHeight,
				rect.left + nRadioBtnSize + nRadioBtnSize/2,
				rect.top + (nRadio+1) * dyHeight);

			bFound = rcItem.PtInRect(pt);
			if (bFound)
				break;

			// Is Text hit?
			rcItem.left = rect.left + nRadioBtnSize + nRadioBtnSize/2;
			rcItem.right = rect.right;

			pDC->DrawText(token, _tcslen(token), rcItem, DT_SINGLELINE | DT_LEFT | DT_CALCRECT);
			bFound = rcItem.PtInRect(pt);
			if (bFound)
				break;

			nRadio++;
		}

		token = _tcstok(NULL, _T("\r\n"));
	}

	delete pszItems;

	return bFound ? nRadio : -1;
}

CGXRadioButton::CGXRadioButton(CGXGridCore* pGrid, BOOL bDraw3d)
	: CGXControl(pGrid)
{
	m_bDraw3d = bDraw3d;
}

void CGXRadioButton::Draw(CDC* pDC, CRect rect, ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, const CGXStyle* pStandardStyle)
{
	// Select font
	CFont* pOldFont = LoadFont(pDC, style, pStandardStyle);

	// Cell-Color
	COLORREF rgbText = style.GetTextColor();
	COLORREF rgbCell = style.GetInteriorRef().GetColor();

	if (Grid()->GetParam()->GetProperties()->GetBlackWhite())
	{
		rgbText = RGB(0,0,0);
		rgbCell = RGB(255,255,255);
	}

	DrawBackground(pDC, rect, style);

	// Erase Background
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(rgbCell);
	pDC->SetTextColor(rgbText);

	LPCTSTR pszValue = _T("");
	LPCTSTR pszChoices = _T("");

	if (style.GetIncludeValue())
		pszValue = style.GetValueRef();

	if (style.GetIncludeChoiceList())
		pszChoices = style.GetChoiceListRef();

	m_nRadioBtns = GXDrawRadioButtons(pDC, rect, pszValue, pszChoices, m_bDraw3d);

	CGXControl::Draw(pDC, rect, nRow, nCol, style, pStandardStyle);

	if (pOldFont)
		pDC->SelectObject(pOldFont);
}

// const int nRadioBtnSize = 12;

CSize CGXRadioButton::CalcSize(CDC* pDC, ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, const CGXStyle* pStandardStyle, BOOL bVert)
{
	// Select font
	CFont* pOldFont = LoadFont(pDC, style, pStandardStyle);

	LPCTSTR pszChoices = _T("");

	if (style.GetIncludeChoiceList())
		pszChoices = style.GetChoiceListRef();

	// text for radio buttons
	LPTSTR pszItems = new TCHAR[_tcslen(pszChoices)+1];
	_tcscpy(pszItems, pszChoices);

	// item height
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);

	int dyHeight = max(nRadioBtnSize, tm.tmHeight + tm.tmExternalLeading);

	CRect rect;

	// check each item
	int nxWidth = 0, nyHeight = 0;

	LPTSTR token = _tcstok(pszItems, _T("\r\n"));
	while (token != 0)
	{
		if (*token)
		{
			nyHeight += dyHeight;

			CSize size = pDC->GetTextExtent(token, _tcslen(token));

			nxWidth = max(nxWidth, size.cx);
		}

		token = _tcstok(NULL, _T("\r\n"));
	}

	delete pszItems;

	nxWidth += nRadioBtnSize + nRadioBtnSize/2;

	if (pOldFont)
		pDC->SelectObject(pOldFont);

	// unused parameters:
	bVert, nRow, nCol;

	return AddBorders(CSize(nxWidth + 4, nyHeight + 4), style);
}

BOOL CGXRadioButton::LButtonDown(UINT nFlags, CPoint pt, UINT nHitState)
{
	return CGXControl::LButtonDown(nFlags, pt, nHitState);
}

BOOL CGXRadioButton::LButtonUp(UINT nFlags, CPoint pt, UINT nHitState)
{
	nHitState, pt, nFlags;

	if (nHitState & GX_HITINCELL)
	{
		int nOldValue = -1;
		LPCTSTR pszChoices = _T("");

		const CGXStyle& style = Grid()->LookupStyleRowCol(m_nRow, m_nCol);

		if (IsReadOnly())
			return FALSE;

		if (style.GetIncludeChoiceList())
			pszChoices = style.GetChoiceListRef();

		if (style.GetIncludeValue() && _tcslen(style.GetValueRef()) > 0)
			nOldValue = _ttoi(style.GetValueRef());

		CClientDC dc(GridWnd());

		// Select font
		CFont* pOldFont = LoadFont(&dc, style, NULL);

		int nValue = GXHitTestRadioButtons(&dc, GetCellRect(m_nRow, m_nCol), pszChoices, pt);
		if (nValue != -1 && nValue != nOldValue && OnStartEditing())
		{
			TCHAR szValue[10];
			wsprintf(szValue, _T("%d"), nValue);
			Grid()->SetValueRange(CGXRange(m_nRow, m_nCol), szValue, gxOverride);
		}

		if (pOldFont)
			dc.SelectObject(pOldFont);

		if (nValue != -1)
			CGXControl::OnClickedButton(NULL);
	}

	CGXControl::LButtonUp(nFlags, pt, nHitState);

	return TRUE;
}

BOOL CGXRadioButton::KeyPressed(UINT nMessage, UINT nChar, UINT nRepCnt, UINT flags)
{
	BOOL bRet = CGXControl::KeyPressed(nMessage, nChar, nRepCnt, flags);

	if (!bRet && nMessage == WM_CHAR && nChar == 32)
	{
		// trigger event
		const CGXStyle& style = Grid()->LookupStyleRowCol(m_nRow, m_nCol);

		if (IsReadOnly())
			return FALSE;

		int nOldValue = -1;
		if (style.GetIncludeValue() && _tcslen(style.GetValueRef()) > 0)
			nOldValue = _ttoi(style.GetValueRef());

		int nNewValue = nOldValue+1;
		if (nNewValue >= m_nRadioBtns)
			nNewValue = 0;

		TCHAR szValue[10];
		wsprintf(szValue, _T("%d"), nNewValue);
		Grid()->SetValueRange(CGXRange(m_nRow, m_nCol), szValue, gxOverride);

		return TRUE;
	}

	return bRet;
}
