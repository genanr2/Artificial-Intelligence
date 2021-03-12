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

// gxctrl.cpp : implementation of controls
//

#include "stdafx.h"

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

// Headers

#ifndef _GXCTRL_H_
#include "gxctrl.h"
#endif

#ifndef _GXCORE_H_
#include "gxcore.h"
#endif

#ifndef _GXEXT_H_
#include "gxext.h"
#endif

#ifndef _AFXPRIV_H_
#include "afxpriv.h"    // for CSharedFile
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCTRL")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

// Global methods

BOOL AFXAPI GXGetDoubleByteChar(TCHAR nChar, CString& sChar, BOOL& bDoubleByteChar, TCHAR& nDoubleByteLeadChar)
{
	// support for DBCS characters

	// is it a leading byte?
	if (!bDoubleByteChar && _istlead(nChar))
	{
		// buffer the leading byte
		bDoubleByteChar = TRUE;
		nDoubleByteLeadChar = (TCHAR) nChar;
		sChar.Empty();

		return TRUE;
	}

	// is it a trailing char?
	else if (bDoubleByteChar)
	{
		// combine this trailing char with the buffered leading byte
		bDoubleByteChar = FALSE;
		TCHAR sz[3];
		sz[0] = nDoubleByteLeadChar;
		sz[1] = (TCHAR) nChar;
		sz[2] = _T('\0');
		sChar = sz;

		return TRUE;
	}

	else // this is a normal character
	{
		sChar = nChar;

		return FALSE;
	}
}

// ------------------------ Control Base Classes ----------------------------

/////////////////////////////////////////////////////////////////////////////
// CGXControl class

CGXControl::CGXControl(CGXGridCore* pGrid)
{
	ASSERT(pGrid != NULL);

	m_pGrid = pGrid;
	m_nRow = GX_INVALID;
	m_nCol = GX_INVALID;
	m_pStyle = NULL;
	m_pChildList = NULL;
	m_pChildFocus = NULL;
	m_bInit = FALSE;

	m_bRefreshOnSetCurrentCell = FALSE;
		// If your derived controls appearence is depending
		// on whether it is a current cell or not, you should set
		// m_bRefreshOnSetCurrentCell = TRUE. Otherwise
		// a smarter redrawing of the cell will be used.

	m_bDoubleByteChar = FALSE;
}

CGXControl::~CGXControl()
{
	if (m_pStyle)
		delete m_pStyle;

	if (m_pChildList)
	{
		POSITION pos = m_pChildList->GetHeadPosition();
		while (pos)
		{
			// pChild has a virtual destructor
			// so, I have to cast the void* to a CGXChild*
			// and assign it to pointer-variable
			CGXChild* pChild = (CGXChild*) m_pChildList->GetNext(pos);
			delete pChild;
		}
		delete m_pChildList;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Runtime Typing (mainly copied from CObject)

// special runtime-class structure for CGXControl (no base class)
#ifdef _GXDLL
CGXControlClass* PASCAL CGXControl::_GetControlBaseClass()
	{ return NULL; }
GX_DATADEF struct CGXControlClass CGXControl::ctrlCGXControl =
	{ "CGXControl", NULL };
#else
struct CGXControlClass GXNEAR CGXControl::ctrlCGXControl =
	{ "CGXControl", NULL };
#endif

CGXControlClass* CGXControl::GetControlClass() const
{
	return &CGXControl::ctrlCGXControl;
}

CRuntimeClass* CGXControl::GetRuntimeClass() const
{
	return NULL;
}

BOOL CGXControl::IsKindOf(const CGXControlClass* pClass) const
{
	ASSERT(this != NULL);
	// it better be in valid memory, at least for CObject size
	ASSERT(AfxIsValidAddress(this, sizeof(CGXControl)));

	// simple SI case
	CGXControlClass* pClassThis = GetControlClass();
	ASSERT(pClass != NULL);
	ASSERT(pClassThis != NULL);
	while (pClassThis != NULL)
	{
		if (pClassThis == pClass || strcmp(pClassThis->m_lpszClassName,
				pClass->m_lpszClassName) == 0)
			return TRUE;

#ifdef _GXDLL
		if (pClassThis->m_pfnGetBaseClass == NULL)
			break;

		pClassThis = (*pClassThis->m_pfnGetBaseClass)();
#else
		pClassThis = pClassThis->m_pBaseClass;
#endif
	}
	return FALSE;       // walked to the top, no match
}

BOOL CGXControl::IsKindOf(const CRuntimeClass* pClass) const
{
	CWnd* pWnd = GetWndPtr();

	return pWnd != NULL && pWnd->IsKindOf(pClass);
}

/////////////////////////////////////////////////////////////////////////////

CGXControl::operator CWnd*()
{
	return GetWndPtr();
}

CWnd* CGXControl::GetWndPtr() const
{
	return NULL;
}

void CGXControl::Hide()
{
}

void AFXAPI CGXControl::AddUserAttributes(CGXStylesMap* pStylesMap)
{
	CGXStyle styleSpin;
	styleSpin.SetControl(GX_IDS_CTRL_SPINEDIT).SetWrapText(FALSE);

	// user attributes
		// Validation: warning message
	pStylesMap->AddUserAttribute(GX_IDS_UA_VALID_MSG,
		CGXStyle().SetWrapText(TRUE).SetAutoSize(TRUE));

		// Validation: maximum value
	pStylesMap->AddUserAttribute(GX_IDS_UA_VALID_MAX,
		&styleSpin);

		// Validation: Minimum value
	pStylesMap->AddUserAttribute(GX_IDS_UA_VALID_MIN,
		&styleSpin);
}

void CGXControl::Refresh()
{
	if (IsInit())
	{
		if (GetModify() || IsActive())
			Grid()->DelayFloatCells(CGXRange(m_nRow, m_nCol));

		Hide();

		if (IsActive())
			GridWnd()->InvalidateRect(Grid()->CalcRectFromRowColEx(m_nRow, m_nCol));
		else
			GridWnd()->InvalidateRect(Grid()->CalcRectFromRowColExEx(m_nRow, m_nCol));
	}
}

void CGXControl::SetActive(BOOL bActive)
{
	bActive;
}

BOOL CGXControl::IsActive()
{
	return FALSE;
}

void CGXControl::SetModify(BOOL bModified)
{
	bModified;
}

BOOL CGXControl::GetModify()
{
	return FALSE;
}

BOOL CGXControl::Store()
{
	return FALSE;
}

BOOL CGXControl::OnStartEditing()
{
	if (IsInit() && Grid()->OnStartEditing(m_nRow, m_nCol))
	{
		Grid()->ScrollCellInView(m_nRow, m_nCol, GX_UPDATENOW, TRUE);
		return TRUE;
	}

	return FALSE;
}

void CGXControl::OnModifyCell()
{
	if (GetModify())
	{
		Grid()->OnModifyCell(m_nRow, m_nCol);
	}
}

BOOL CGXControl::OnCancelEditing()
{
	if (Grid()->OnCancelEditing(m_nRow, m_nCol))
	{
		Grid()->DelayFloatCells(CGXRange(m_nRow, m_nCol));
		return TRUE;
	}

	return FALSE;
}

void CGXControl::OnCanceledEditing()
{
	Grid()->OnCanceledEditing(m_nRow, m_nCol);
	Grid()->EvalFloatCells(CGXRange(m_nRow, m_nCol));
}

BOOL CGXControl::OnEndEditing()
{
	return Grid()->OnEndEditing(m_nRow, m_nCol);
}

BOOL CGXControl::OnLeftCell(ROWCOL nNewRow, ROWCOL nNewCol)
{
	return Grid()->OnLeftCell(m_nRow, m_nCol, nNewRow, nNewCol);
}

BOOL CGXControl::OnValidate()
{
	BOOL bValid = TRUE;

	CString sText;

	if (IsActive() && GetModify())
	{
		GetCurrentText(sText);

		if (!sText.IsEmpty())
		{
			CString sMin;

			const CGXStyle& style = Grid()->LookupStyleRowCol(m_nRow, m_nCol);

			style.GetUserAttribute(GX_IDS_UA_VALID_MIN, sMin);

			if (!sMin.IsEmpty() && _gxttof(sMin) > _gxttof(sText))
				bValid = FALSE;

			if (bValid)
			{
				CString sMax;

				style.GetUserAttribute(GX_IDS_UA_VALID_MAX, sMax);

				if (!sMax.IsEmpty() && _gxttof(sMax) < _gxttof(sText))
					bValid = FALSE;
			}

			if (!bValid)
			{
				CString sMsg;
				style.GetUserAttribute(GX_IDS_UA_VALID_MSG, sMsg);
				Grid()->SetWarningText(sMsg);
				return FALSE;
			}
		}

		if (!ValidateString(sText))
			return FALSE;
	}

	return Grid()->OnValidateCell(m_nRow, m_nCol);
}

BOOL CGXControl::ValidateString(const CString&)
{
	return TRUE;
}

void CGXControl::OnInvalidKeyPressed(LPCTSTR pszChar)
{
	pszChar;
}

BOOL CGXControl::OnDeleteCell()
{
	return Grid()->OnDeleteCell(m_nRow, m_nCol);
}

void CGXControl::Init(ROWCOL nRow, ROWCOL nCol)
{
	// Stores the cell coordinates, resets the style and sets the window text
	m_nRow = nRow;
	m_nCol = nCol;
	m_bInit = TRUE;
	SetActive(FALSE);
	SetModify(FALSE);

	FreeStyle(FALSE);

	FocusOnChild(NULL);
	OnInitChildren(nRow, nCol, CGXControl::GetCellRect(nRow, nCol));
	Grid()->OnInitCurrentCell(nRow, nCol);

	if (Grid()->GetParam()->GetActivateCellFlags() & GX_CAFOCUS_SETCURRENT && OnStartEditing())
		SetActive(TRUE);
}

BOOL CGXControl::IsInit() const
{
	return m_bInit;
}

BOOL CGXControl::IsInit(ROWCOL nRow, ROWCOL nCol) const
{
	return m_bInit && m_nRow == nRow && m_nCol == nCol;
}

void CGXControl::Reset()
{
	if (GetModify() || IsActive())
		Grid()->DelayFloatCells(CGXRange(m_nRow, m_nCol));
	Hide();
	SetModify(FALSE);
	SetActive(FALSE);
	FreeStyle();
	m_bInit = FALSE;
}

void CGXControl::FreeStyle(BOOL bGrid)
{
	if (m_pStyle)
		delete m_pStyle;
	m_pStyle = NULL;

	if (bGrid)
		Grid()->m_pStyleEdit->Free();
}

CGXStyle* CGXControl::NeedStyle()
{
	if (!m_pStyle)
	{
		const CGXStyle& style = Grid()->LookupStyleRowCol(m_nRow, m_nCol);
		// duplicate the style
		m_pStyle = style.Clone();
	}

	return m_pStyle;
}

BOOL CGXControl::IsReadOnly()
{
	NeedStyle();

	return Grid()->GetParam()->IsLockReadOnly()
		&& (Grid()->IsReadOnly() || m_pStyle && m_pStyle->GetIncludeReadOnly() && m_pStyle->GetReadOnly());
}

CGXGridCore* CGXControl::Grid()
{
	return m_pGrid;
}

CWnd* CGXControl::GridWnd()
{
	return m_pGrid->m_pGridWnd;
}

CRect CGXControl::GetCellRect(ROWCOL nRow, ROWCOL nCol, LPRECT rectItem /* = NULL */, const CGXStyle* pStyle /*= NULL*/)
{
	// compute the interior rectangle for the text
	// without buttons and borders
	RECT rect;

	if (pStyle == NULL)
		pStyle = &Grid()->LookupStyleRowCol(nRow, nCol);

	if (rectItem == NULL)
		rect = Grid()->CalcRectFromRowColEx(nRow, nCol);
	else
		rect = *rectItem;

	// subtract space needed for borders, 3d-Frame or grid-lines
	rect = Grid()->SubtractBorders(rect, *pStyle);

	if (!Grid()->GetParam()->GetNewGridLineMode())
	{
		// With old drawing mode, we have to reserve one
		// pixel for the frozen line color
		BOOL bHorz = Grid()->GetParam()->GetProperties()->GetDisplayHorzLines();
		BOOL bVert = Grid()->GetParam()->GetProperties()->GetDisplayVertLines();

		BOOL bFixedCol = bHorz && nCol > Grid()->GetHeaderCols() && nCol == Grid()->GetFrozenCols();
		BOOL bFixedRow = bVert && nRow > Grid()->GetHeaderRows() && nRow == Grid()->GetFrozenRows();

		if (bFixedRow && !pStyle->GetIncludeBorders(gxBorderBottom))
			rect.bottom--;

		if (bFixedCol && !pStyle->GetIncludeBorders(gxBorderRight))
			rect.right--;
	}

	return rect;
}

CSize CGXControl::AddBorders(CSize size, const CGXStyle& style)
{
	// Override this method if you have overridden GetCellRect
	return Grid()->AddBorders(size, style);
}

CSize CGXControl::CalcSize(CDC* pDC, ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, const CGXStyle* pStandardStyle, BOOL bVert)
{
	bVert, nRow, nCol, pDC, style, pStandardStyle;

	return CSize(0, 0);
}

BOOL CGXControl::CanFloatCell(ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, BOOL bFloatOrFlood)
{
	nRow, nCol, style, bFloatOrFlood;

	// By default, cells can not float over other cells.

	return FALSE;
}

BOOL CGXControl::CanMergeCell(ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, int mtType)
{
	// unused:
	nRow, nCol, mtType;

	// By default, cells can be merged when value is not empty

	return !style.GetValueRef().IsEmpty();
}

void CGXControl::InvertBorders(CDC* pDC, const CRect& r)
{
	if (Grid()->GetParam()->GetExcelLikeCurrentCell()
		|| Grid()->GetParam()->GetActivateCellFlags() == GX_CAFOCUS_SETCURRENT
			&& IsKindOf(RUNTIME_CLASS(CWnd))
		)
		return;

	int nHide = Grid()->GetParam()->GetHideCurrentCell();

	if (!nHide || nHide == GX_HIDE_INACTIVEFRAME && Grid()->IsActiveFrame())
	{
		CGXProperties* pProp = Grid()->GetParam()->GetProperties();

		const CGXStyle& style =
			pProp->GetUserProperty(GX_IDS_OUTLINECURRENTCELL);

		int nThick = 1;
		if (style.GetValueRef() == pProp->sInvertDrawBorder)
		{
			CGXPen pen;
			pen.SetWidth(nThick = 1);
			Grid()->DrawBorder(pen, r, gxBorderTop, pDC);
			Grid()->DrawBorder(pen, r, gxBorderBottom, pDC);
			Grid()->DrawBorder(pen, r, gxBorderLeft, pDC);
			Grid()->DrawBorder(pen, r, gxBorderRight, pDC);
		}
		else if (style.GetValueRef() == pProp->sInvertThickBorder)
		{
			CGXPen pen;
			pen.SetWidth(nThick = 2);
			Grid()->DrawBorder(pen, r, gxBorderTop, pDC);
			Grid()->DrawBorder(pen, r, gxBorderBottom, pDC);
			Grid()->DrawBorder(pen, r, gxBorderLeft, pDC);
			Grid()->DrawBorder(pen, r, gxBorderRight, pDC);
		}
		else if (!(style.GetValueRef() == pProp->sInvertNoBorder))
		{
			if (style.GetValueRef() == pProp->sInvertThick)
				nThick = 2;

			// left border
			pDC->InvertRect(CRect(r.left, r.top, r.left+nThick, r.bottom));
			// right border
			pDC->InvertRect(CRect(r.right-nThick, r.top, r.right, r.bottom));
			// top border
			pDC->InvertRect(CRect(r.left+nThick, r.top, r.right-nThick, r.top+nThick));
			// bottom border
			pDC->InvertRect(CRect(r.left+nThick, r.bottom-nThick, r.right-nThick, r.bottom));
		}
	}
}

BOOL CGXControl::GetChoiceListItem(CString& strResult, LPCTSTR pszChoiceList, int nIndex)
{
	// Try to find the nth line in the choice list

	if (pszChoiceList == NULL || nIndex < 0)
	{
		strResult.Empty();
		return FALSE;
	}
	else
	{
		// loop through choice list
		CString s = pszChoiceList;
		int n = 0;
		int nLine;
		for (nLine = 0; nLine <= nIndex && !s.IsEmpty() && n != -1; nLine++)
			n = GXGetNextLine(s, strResult);

		if (nLine <= nIndex)
		{
			// choice list has fewer rows than the value of index
			strResult.Empty();
			return FALSE;
		}
	}

	return TRUE;
}

int CGXControl::FindStringInChoiceList(CString& sMatch, LPCTSTR pszEdit, LPCTSTR pszChoiceList, BOOL bExact)
{
	// finds exact string or the first string with the same prefix
	int nIndex = -1;

	int nEdit = _tcslen(pszEdit);

	// search choice which fits the given text
	if (nEdit > 0)
	{
		// First, check if we find a line which completely fits the text
		CString s = pszChoiceList;
		int n = 0;
		BOOL bFound = FALSE;

		// First guess
		int nIndexGuess = -1;
		CString sGuess;

		nIndex = 0;
		CString sItem;
		while (!s.IsEmpty() && n != -1)
		{
			n = GXGetNextLine(s, sItem);

			// is text matching the item?
			if (_tcsicmp(pszEdit, sItem) == 0)
			{
				sMatch = sItem;
				bFound = TRUE;
				break;
			}
			else if (!bExact && sGuess.IsEmpty() && _tcsnicmp(pszEdit, sItem, nEdit) == 0)
			{
				// first choice with the same prefix
				sGuess = sItem;
				nIndexGuess = nIndex;
			}

			nIndex++;
		}

		if (!bFound && !bExact)
		{
			// first choice with the same prefix
			sMatch = sGuess;
			nIndex = nIndexGuess;
		}
		else if (!bFound)
		{
			sMatch.Empty();
			nIndex = -1;
		}
	}

	return nIndex;
}

void CGXControl::Draw(CDC* pDC, CRect rect, ROWCOL nRow, ROWCOL nCol, const CGXStyle& style, const CGXStyle* pStandardStyle)
{
	pStandardStyle, style;

	OnInitChildren(nRow, nCol, CGXControl::GetCellRect(nRow, nCol, &rect, &style));

	for (int i = 0; i < GetCount(); i++)
	{
		CGXChild* pChild;
		if ((pChild = GetChild(i)) != NULL)
		{
			pChild->Draw(pDC, Grid()->IsCurrentCell(nRow, nCol) && !Grid()->IsPrinting()
				&& Grid()->IsActiveFrame());
		}
	}
}

BOOL CGXControl::GetValue(CString& strResult)
{
	// Reads out the window text and converts it into
	// the plain value which should be stored in the style object.

	if (!IsInit())
		return FALSE;

	NeedStyle();

	if (!m_pStyle->GetIncludeValue())
		return FALSE;

	strResult = m_pStyle->GetValue();

	return TRUE;
}


void CGXControl::SetValue(LPCTSTR pszRawValue)
{
	// Convert the value which should be stored in the
	// style object to the text which should be displayed
	// in the cell and display it in the active
	// current cell (e.g. with SetWindowText()).

	// Default behavior: store value in m_pStyle
	VERIFY(IsInit());
	if (IsInit())
	{
		NeedStyle();
		m_pStyle->SetValue(pszRawValue);

		Grid()->DelayFloatCells(CGXRange(m_nRow, m_nCol));
		Grid()->EvalFloatCells(CGXRange(m_nRow, m_nCol));
	}
}

void CGXControl::GetCurrentText(CString& strResult)
{
	// override this method to return the text which
	// is displayed in the current cell

	if (m_pStyle && m_pStyle->GetIncludeValue())
		strResult = m_pStyle->GetValueRef();
	else if (IsInit())
	{
		NeedStyle();

		// Get the formatted string from the style object
		// and also give the programmer the chance to adjust
		// the string before it is displayed in the cell
		GetControlText(strResult, m_nRow, m_nCol, NULL, *m_pStyle);
	}
	else
		strResult.Empty();
}

void CGXControl::SetCurrentText(const CString& str)
{
	// override this method to set the text as it should
	// be displayed in the current cell

	// simply change value in cell

	if (IsInit())
	{
		Grid()->SetControlTextRowCol(m_nRow, m_nCol, str, GX_UPDATENOW, m_pStyle);
	}
}

BOOL CGXControl::GetControlText(CString& sResult, ROWCOL nRow, ROWCOL nCol, LPCTSTR pszRawValue, const CGXStyle& style)
{
	// Override this method to convert the plain value (specified
	// either with pszRawValue or style.GetValue) into the
	// specific format which should be displayed in the cell.

	// if value is specified, simply copy this value
	if (pszRawValue)
	{
		if (style.GetIncludeFormat() && style.GetFormat() != GX_FMT_DEFAULT)
			sResult = GXFormatText(style, pszRawValue, GX_VT_STRING);
		else
			sResult = pszRawValue;
		return TRUE;
	}
	// check the style object
	if (style.GetIncludeValue())
	{
		if (style.GetIncludeFormat() && style.GetFormat() != GX_FMT_DEFAULT)
			sResult = GXFormatText(style);
		else
			sResult = style.GetValueRef();

		return TRUE;
	}

	// unused:
	nRow, nCol;

	// no value found
	return FALSE;
}

// SetControlText
//
// Override this method to convert the string into the
// value which should be stored in the style.
//
// SetControlText will be called when you paste
// several cells from the clipboard with CF_TEXT
// format and lets you validate the pasted contents.
//
// See the MaskEdit implementation for an example. It
// validates the input before calling SetValueRange.
//
// This default implementation only checks for readonly
// and maximum length.

BOOL CGXControl::SetControlText(ROWCOL nRow, ROWCOL nCol, const CString& str, UINT nFlags, const CGXStyle* pOldStyle)
{
	BOOL bSuccess = FALSE;
	CGXStyle* pStyle = NULL;

	if (pOldStyle == NULL)
	{
		pStyle = Grid()->CreateStyle();
		Grid()->ComposeStyleRowCol(nRow, nCol, pStyle);
		pOldStyle = pStyle;
	}

	Grid()->m_pOldStyle = pOldStyle;

	CString sValue = str;
	bSuccess = ConvertControlTextToValue(sValue, nRow, nCol, pOldStyle);

	if (bSuccess)
	{
		int nMaxLength = 0;

		// check for read-only
		if (Grid()->GetParam()->IsLockReadOnly() &&
			(Grid()->IsReadOnly() || pOldStyle->GetIncludeReadOnly() && pOldStyle->GetReadOnly()))
			bSuccess = FALSE;

		// check for maximum length
		else if (pOldStyle->GetIncludeMaxLength()
			&& (nMaxLength = (int) pOldStyle->GetMaxLength()) > 0
			&& sValue.GetLength() > nMaxLength)
			bSuccess = Grid()->SetExpressionRowCol(nRow, nCol, sValue.Left(nMaxLength), nFlags);

		// default
		else
			bSuccess = Grid()->SetExpressionRowCol(nRow, nCol, sValue, nFlags);
	}

	if (pStyle)
		Grid()->RecycleStyle(pStyle);

	Grid()->m_pOldStyle = NULL;

	return bSuccess;
}

BOOL CGXControl::ConvertControlTextToValue(CString& str, ROWCOL nRow, ROWCOL nCol, const CGXStyle* pOldStyle)
{
	// Unused
	nRow, nCol, str, pOldStyle;

	return TRUE;
}

BOOL CGXControl::StoreStyle(ROWCOL nRow, ROWCOL nCol, const CGXStyle* pStyle, GXModifyType mt, int nType, const CGXStyle* pOldStyle)
{
	// Unused:
	nType;

	ASSERT(nRow <= Grid()->GetRowCount() && nCol <= Grid()->GetColCount());
	// ASSERTION-> cell coordinates out of range ->END

	BOOL bReadOnly = FALSE;

	// do not overwrite read-only cells when GetParam()->m_bLockReadOnly is set.
	if (Grid()->GetParam()->IsLockReadOnly())
	{
		bReadOnly = Grid()->IsReadOnly();

		if (!bReadOnly)
		{
			//
			// This is the check if the cell is read-only
			//
			bReadOnly = pOldStyle && pOldStyle->GetIncludeReadOnly()
				? pOldStyle->GetReadOnly()
				: FALSE;
		}
	}

	if (bReadOnly)
		return FALSE;

	// New style seems ok, let the grid store the style

	BOOL bStored = Grid()->StoreStyleRowCol(nRow, nCol, pStyle, mt, nType);

	if (bStored && Grid()->IsCurrentCell(nRow, nCol))
	{
		if (IsInit() && m_nRow == nRow && m_nCol == nCol && mt != gxRemove)
		{
			FreeStyle();
			m_nRow = nRow;
			m_nCol = nCol;
			NeedStyle();

			// if value has changed, update window text
			BOOL bModified = GetModify();

			if (pStyle && pStyle->GetIncludeControl() && mt != gxRemove)
			{
				Grid()->m_pCurrentCellControl = NULL;
				Grid()->m_pStyleEdit->Free();
			}
			else if (pStyle && pStyle->GetIncludeValue() && mt != gxRemove)
			{
				CString s;
				s = pStyle->GetValueRef();
				SetValue(s);
			}

			Refresh();

			if (mt == gxCopy)
				SetModify(FALSE);
			else
				SetModify(bModified);
		}
		else
			Init(nRow, nCol);
	}

	return bStored;
}

BOOL CGXControl::LoadStyle(ROWCOL nRow, ROWCOL nCol, CGXStyle* pStyle)
{
	// Unused:
	nRow, nCol, pStyle;

	if (IsInit() && Grid()->IsCurrentCell(nRow, nCol))
	{
		CString s;
		if (GetValue(s))
			pStyle->SetValue(s);
	}

	return FALSE; // no change made to the style
}

BOOL CGXControl::GetMarkHeaderState(ROWCOL nRow, ROWCOL nCol, const CGXStyle& style)
{
	if (Grid()->m_pMarkEditHeaderImp)
		return Grid()->m_pMarkEditHeaderImp->GetMarkHeaderState(this, nRow, nCol, style);
	
	return FALSE;
}

void CGXControl::OnClickedButton(CGXChild* pChild)
{
	// Unreferenced parameters:
	pChild;

	Grid()->OnClickedButtonRowCol(m_nRow, m_nCol);
}

// Mouse hit
BOOL CGXControl::LButtonDown(UINT nFlags, CPoint pt, UINT nHitState)
{
	// Unreferenced parameters:
	nHitState;

	OnInitChildren(m_nRow, m_nCol, CGXControl::GetCellRect(m_nRow, m_nCol));

	for (int i = 0; i < GetCount(); i++)
	{
		CGXChild* pChild;
		if ((pChild = GetChild(i))->Intersects(pt))
		{
			FocusOnChild(pChild);
			return pChild->LButtonDown(nFlags, pt);
		}
	}

	return FALSE;
}

BOOL CGXControl::MouseMove(UINT nFlags, CPoint pt, UINT nHitState)
{
	// Unreferenced parameters:
	nHitState;

	OnInitChildren(m_nRow, m_nCol, CGXControl::GetCellRect(m_nRow, m_nCol));

	CGXChild* pChild;
	if ((pChild = ChildWithFocus()) != NULL)
		return pChild->MouseMove(nFlags, pt);

	return FALSE;
}

BOOL CGXControl::LButtonUp(UINT nFlags, CPoint pt, UINT nHitState)
{
	// Unreferenced parameters:
	nHitState;

	OnInitChildren(m_nRow, m_nCol, CGXControl::GetCellRect(m_nRow, m_nCol));

	BOOL bState = FALSE;

	CGXChild* pChild;
	if ((pChild = ChildWithFocus()) != NULL)
		bState |= pChild->LButtonUp(nFlags, pt);

	if (CGXControl::GetCellRect(m_nRow, m_nCol).PtInRect(pt))
		bState |= Grid()->OnLButtonClickedRowCol(m_nRow, m_nCol, nFlags, pt);

	return bState;
}

BOOL CGXControl::MButtonDown(UINT nFlags, CPoint pt, UINT nHitState)
{
	// Unreferenced parameters:
	nHitState;

	OnInitChildren(m_nRow, m_nCol, CGXControl::GetCellRect(m_nRow, m_nCol));

	for (int i = 0; i < GetCount(); i++)
	{
		CGXChild* pChild;
		if ((pChild = GetChild(i))->Intersects(pt))
		{
			FocusOnChild(pChild);
			return pChild->MButtonDown(nFlags, pt);
		}
	}

	return FALSE;
}

BOOL CGXControl::MButtonUp(UINT nFlags, CPoint pt, UINT nHitState)
{
	// Unreferenced parameters:
	nHitState;

	OnInitChildren(m_nRow, m_nCol, CGXControl::GetCellRect(m_nRow, m_nCol));

	BOOL bState = FALSE;

	CGXChild* pChild;
	if ((pChild = ChildWithFocus()) != NULL)
		bState |= pChild->MButtonUp(nFlags, pt);

	if (CGXControl::GetCellRect(m_nRow, m_nCol).PtInRect(pt))
		bState |= Grid()->OnMButtonClickedRowCol(m_nRow, m_nCol, nFlags, pt);

	return bState;
}


BOOL CGXControl::RButtonDown(UINT nFlags, CPoint pt, UINT nHitState)
{
	// Unreferenced parameters:
	nHitState;

	OnInitChildren(m_nRow, m_nCol, CGXControl::GetCellRect(m_nRow, m_nCol));

	for (int i = 0; i < GetCount(); i++)
	{
		CGXChild* pChild;
		if ((pChild = GetChild(i))->Intersects(pt))
		{
			FocusOnChild(pChild);
			return pChild->RButtonDown(nFlags, pt);
		}
	}

	return FALSE;
}

BOOL CGXControl::RButtonUp(UINT nFlags, CPoint pt, UINT nHitState)
{
	// Unreferenced parameters:
	nHitState;

	OnInitChildren(m_nRow, m_nCol, CGXControl::GetCellRect(m_nRow, m_nCol));

	BOOL bState = FALSE;

	CGXChild* pChild;
	if ((pChild = ChildWithFocus()) != NULL)
		bState |= pChild->RButtonUp(nFlags, pt);

	if (CGXControl::GetCellRect(m_nRow, m_nCol).PtInRect(pt))
		bState |= Grid()->OnRButtonClickedRowCol(m_nRow, m_nCol, nFlags, pt);

	return bState;
}

BOOL CGXControl::LButtonDblClk(UINT nFlags, CPoint pt)
{
	BOOL bState = FALSE;

	OnInitChildren(m_nRow, m_nCol, CGXControl::GetCellRect(m_nRow, m_nCol));

	for (int i = 0; !bState && i < GetCount(); i++)
	{
		CGXChild* pChild;
		if ((pChild = GetChild(i))->Intersects(pt))
		{
			FocusOnChild(pChild);
			bState |= pChild->LButtonDblClk(nFlags, pt);
		}
	}

	bState |= Grid()->OnLButtonDblClkRowCol(m_nRow, m_nCol, nFlags, pt);

	return bState;
}

BOOL CGXControl::MButtonDblClk(UINT nFlags, CPoint pt)
{
	BOOL bState = FALSE;

	OnInitChildren(m_nRow, m_nCol, CGXControl::GetCellRect(m_nRow, m_nCol));

	for (int i = 0; !bState && i < GetCount(); i++)
	{
		CGXChild* pChild;
		if ((pChild = GetChild(i))->Intersects(pt))
		{
			FocusOnChild(pChild);
			bState |= pChild->MButtonDblClk(nFlags, pt);
		}
	}

	bState |= Grid()->OnMButtonDblClkRowCol(m_nRow, m_nCol, nFlags, pt);

	return bState;
}

BOOL CGXControl::RButtonDblClk(UINT nFlags, CPoint pt)
{
	BOOL bState = FALSE;

	OnInitChildren(m_nRow, m_nCol, CGXControl::GetCellRect(m_nRow, m_nCol));

	for (int i = 0; !bState && i < GetCount(); i++)
	{
		CGXChild* pChild;
		if ((pChild = GetChild(i))->Intersects(pt))
		{
			FocusOnChild(pChild);
			bState |= pChild->RButtonDblClk(nFlags, pt);
		}
	}

	bState |= Grid()->OnRButtonDblClkRowCol(m_nRow, m_nCol, nFlags, pt);

	return bState;
}

BOOL CGXControl::MouseMoveOver(ROWCOL nRow, ROWCOL nCol, UINT nFlags, CPoint pt)
{
	BOOL bState = FALSE;

	OnInitChildren(nRow, nCol, CGXControl::GetCellRect(nRow, nCol));

	for (int i = 0; !bState && i < GetCount(); i++)
	{
		CGXChild* pChild;
		if ((pChild = GetChild(i))->Intersects(pt))
		{
			bState |= pChild->MouseMoveOver(nRow, nCol, nFlags, pt);
		}
	}

	bState |= Grid()->OnMouseMoveOver(nRow, nCol, nFlags, pt);

	return bState;
}

BOOL CGXControl::MouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message, int& retval)
{
	pDesktopWnd, nHitTest, message, retval;

	return FALSE;   // not processed
}

// KeyPressed
//
// this method will be called from CGXGridCore::ProcessKeys
// when the current cell is inactive and the user did press
// a key.
//
// The return value is TRUE if the key has been processed.
// and FALSE if the default action shall be performed.
//
// NOTE: If you have a CWnd-derived class (e.g. CGXEditControl),
// KeyPressed will not be called when the control has the focus.
// Once the focus is set to the CWnd, you have to override
// the WM_KEYDOWN and other keyboard messages with ClassWizard
// for processing the keyboard message (see
// CGXEditControl::OnKeyDown for example).
//


BOOL CGXControl::KeyPressed(UINT nMessage, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CGXChild* pChild = ChildWithFocus();
	if (pChild && pChild->KeyPressed(nMessage, nChar, nRepCnt, nFlags))
		return TRUE;

	// dispatch message to key the following virtuals
	// if the derived control class does not override
	// the event, the CGXGridCore will process the key
	// (e.g. move the current cell)

	// sample calling sqeuence
	// CGXGridWnd:OnKeyDown
	// CGXGridCore::ProcessKeys
	// CGXControl::KeyPressed
	// CGXControl::OnGridKeyDown
	// CGXGridCore::OnGridKeyDown

	switch (nMessage)
	{
	// Keyboard messages
	case WM_KEYDOWN:
		return OnGridKeyDown(nChar, nRepCnt, nFlags);

	case WM_KEYUP:
		return OnGridKeyUp(nChar, nRepCnt, nFlags);

	case WM_CHAR:
		{
			// support for DBCS characters
			CString sChar;
			if (GXGetDoubleByteChar((TCHAR) nChar, sChar, m_bDoubleByteChar, m_nDoubleByteLeadChar))
			{
				if (sChar.IsEmpty())
					return TRUE;

				return OnGridDoubleByteChar(sChar, nRepCnt, nFlags);
			}
			else // this is a normal character
			{
				return OnGridChar(nChar, nRepCnt, nFlags);
			}
		}

	case WM_DEADCHAR:
		return OnGridDeadChar(nChar, nRepCnt, nFlags);

	case WM_SYSKEYDOWN:
		return OnGridSysKeyDown(nChar, nRepCnt, nFlags);

	case WM_SYSKEYUP:
		return OnGridSysKeyUp(nChar, nRepCnt, nFlags);

	case WM_SYSCHAR:
		return OnGridSysChar(nChar, nRepCnt, nFlags);

	case WM_SYSDEADCHAR:
		return OnGridSysDeadChar(nChar, nRepCnt, nFlags);
	}

	return FALSE;
}

BOOL CGXControl::OnGridKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXControl::OnGridKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXControl::OnGridChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXControl::OnGridDoubleByteChar(const CString& sChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	sChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXControl::OnGridDeadChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXControl::OnGridSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXControl::OnGridSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXControl::OnGridSysChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

BOOL CGXControl::OnGridSysDeadChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Unused:
	nChar, nRepCnt, nFlags;

	return FALSE;   // return TRUE if you don't want default processing of this event
}

//
// Sorting

void CGXControl::GetSortKey(ROWCOL nRow, ROWCOL nCol, const CGXSortInfo& sortInfo, const CGXStyle& style, CString& sKey)
{
	// Unused:
	nCol, nRow, sortInfo;

	sKey = style.GetValueRef();
}

//
// Find & Replace

BOOL CGXControl::FindText(const GX_FR_STATE& find, ROWCOL nRow, ROWCOL nCol, BOOL bCurrentPos, BOOL bSetCell)
{
	GX_CHECKIMP(Grid()->m_pFindReplaceImp, "CGXControl::FindText");

	if (Grid()->m_pFindReplaceImp)
		return Grid()->m_pFindReplaceImp->FindText(this, find, nRow, nCol, bCurrentPos, bSetCell);

	return FALSE;
}

void CGXControl::ReplaceSel(LPCTSTR pszReplaceText)
{
	if (IsInit() && Grid()->IsCurrentCell(m_nRow, m_nCol))
		Grid()->SetControlTextRowCol(m_nRow, m_nCol, pszReplaceText, GX_UPDATENOW, m_pStyle);
}

BOOL CGXControl::ReplaceAll(const GX_FR_STATE& find, ROWCOL nRow, ROWCOL nCol)
{
	GX_CHECKIMP(Grid()->m_pFindReplaceImp, "CGXControl::ReplaceAll");

	if (Grid()->m_pFindReplaceImp)
		return Grid()->m_pFindReplaceImp->ReplaceAll(this, find, nRow, nCol);

	return FALSE;
}

BOOL CGXControl::GetSelectedText(CString& strResult)
{
	GetCurrentText(strResult);

	return FALSE;
}

// Copy
//
// Copy will only copy data to clipboard if cell is active (has the focus)
// If you also want to support Copy for inactive controls, or if you
// want to support additional clipboard formats, you should
// override these methods.

BOOL CGXControl::CanCopy()
{
	return IsInit();
}

// Copy()
//
// Called when user calls Edit|Copy and the grid
// has a current cell but no range is selected.
//
// Copy() performs the following job:
// a) Copy Selected Text as formatted in the cell
// b) Only if cell is inactive copy the CGXStyle info for the cell

BOOL CGXControl::Copy()
{
	HGLOBAL hData = 0;
	HANDLE hResult = 0;

	if (!IsActive())
	{
		// just perform standard grid copy
		return IsInit()
			&& Grid()->IsCurrentCell(m_nRow, m_nCol)
			&& Grid()->CopyRange(CGXRange(m_nRow, m_nCol));
	}
	else
	{
		// open memory file
		CSharedFile clipb (GMEM_MOVEABLE|GMEM_DDESHARE|GMEM_ZEROINIT);

		CString s;
		GetSelectedText(s);
		if (_tcslen(s) > 0)
		{
			clipb.Write(s, _tcslen(s)*sizeof(TCHAR));
			clipb.Write(_T("\0"), sizeof(TCHAR));

			hData = clipb.Detach();
		}

		// Now, hand over memory block to clipboard
		if (hData)
		{
			VERIFY(GridWnd()->OpenClipboard());

	#ifdef _UNICODE
			hResult = SetClipboardData(CF_UNICODETEXT, hData);
	#else
			hResult = SetClipboardData(CF_TEXT, hData);
	#endif

			VERIFY(CloseClipboard());
		}

		return hResult != 0;   // Successful if hResult is non zero
	}
}

// Cut
//
// Cut will only cut  data to clipboard if cell is active (has the focus)
// If you also want to support Cut for inactive controls, or if you
// want to support additional clipboard formats, you should
// override these methods.

BOOL CGXControl::CanCut()
{
	if (IsInit())
	{
		NeedStyle();
		return !(m_pStyle->GetIncludeReadOnly() && m_pStyle->GetReadOnly());
	}

	return FALSE;
}

BOOL CGXControl::Cut()
{
	if (!IsActive())
	{
		// just perform standard grid cut
		return IsInit()
			&& Grid()->IsCurrentCell(m_nRow, m_nCol)
			&& Grid()->CutRange(CGXRange(m_nRow, m_nCol), (BOOL) (Grid()->m_nClipboardFlags & GX_DNDSTYLES));
	}
	else
	{
		if (!Copy())
			return FALSE;

		ReplaceSel(_T(""));

		return TRUE;
	}
}

// Paste
//
// Paste will only paste data from clipboard if cell is active (has the focus)
// If you also want to support Paste for inactive controls, or if you
// want to support additional clipboard formats, you should
// override these methods.

BOOL CGXControl::CanPaste()
{
	BOOL bCanPaste = FALSE;

	if (IsInit())
	{
		NeedStyle();

		BOOL bStyles =
			IsClipboardFormatAvailable(GXGetAppData()->m_uiStylesClipboardFormat);


		if ((!bStyles || Grid()->GetParam()->GetRangeList()->GetCount() == 0)
			&& IsInit() && IsActive()
			&& !(m_pStyle->GetIncludeReadOnly() && m_pStyle->GetReadOnly()))
		{
#ifdef _UNICODE
			bCanPaste = IsClipboardFormatAvailable(CF_UNICODETEXT);
#else
			bCanPaste = IsClipboardFormatAvailable(CF_TEXT);

			if (!bCanPaste)
				bCanPaste = IsClipboardFormatAvailable(CF_OEMTEXT);
#endif
		}
	}

	return bCanPaste;
}

BOOL CGXControl::Paste()
{
	BOOL bStyles =
		IsClipboardFormatAvailable(GXGetAppData()->m_uiStylesClipboardFormat);

	if ((!bStyles || Grid()->GetParam()->GetRangeList()->GetCount() == 0)
		&& IsInit() && IsActive()
		&& !(m_pStyle->GetIncludeReadOnly() && m_pStyle->GetReadOnly()))
	{
		// Check if clipboard is opened
		BOOL bOpen = FALSE;
		if (CWnd::GetOpenClipboardWindow() == NULL)
			bOpen = GridWnd()->OpenClipboard();

		HGLOBAL handle;
#ifdef _UNICODE
		handle = GetClipboardData(CF_UNICODETEXT);
#else
		handle = GetClipboardData(CF_TEXT);

		if (handle == 0)
			handle = GetClipboardData(CF_OEMTEXT);
#endif

		// insert text into active current cell
		if (handle)
		{
			LPTSTR psz = (LPTSTR) GlobalLock(handle);
			ReplaceSel(psz);
			GlobalUnlock(handle);
		}

		// If we did open the clipboard, we also need to close it again
		if (bOpen)
			CloseClipboard();

		return handle != 0;
	}

	return FALSE;
}

BOOL CGXControl::OnCommand(WPARAM, LPARAM)
{
	return FALSE;
}

CFont* CGXControl::LoadFont(CDC* pDC, const CGXStyle& style, const CGXStyle* pStandardStyle)
{
	return Grid()->LoadFont(pDC, style, pStandardStyle);
}

void CGXControl::OnNotifyMsg(MSG* pMsg)
{
	// Called from CGXGridCore::OnVScroll, OnHScroll
	pMsg;

	// NOTE: It is essential to deactivate the current cell
	// when the user scrolls to the right and the current cell
	// is a floating or covered cell which spans
	// over the freeze columns/row line

	if (pMsg->message == WM_VSCROLL
		|| pMsg->message == WM_HSCROLL)
	{
		if (IsActive())
		{
			CGXGridCore* pGrid = Grid();
			CGXRange rg;
			if (pGrid->GetCellRangeRowCol(m_nRow, m_nCol, rg)
				&&
					(rg.left <= pGrid->GetFrozenCols() && rg.right > pGrid->GetFrozenCols()
					|| (rg.top <= pGrid->GetFrozenRows() && rg.bottom > pGrid->GetFrozenRows()))
				)
			{
				if (GetModify())
					Store();
				SetActive(FALSE);
				Refresh();
			}
		}
	}
}

void CGXControl::OnGridWinIniChange(LPCTSTR lpszSection)
{
	// Called from CGXGridCore::OnGridWinIniChange

	// notify children
	for (int i = 0; i < GetCount(); i++)
	{
		CGXChild* pChild;
		if ((pChild = GetChild(i)) != NULL)
		{
			pChild->OnGridWinIniChange(lpszSection);
		}
	}
}

void CGXControl::OnGridSysColorChange()
{
	// Called from CGXGridCore::OnGridSysColorChange

	// notify children
	for (int i = 0; i < GetCount(); i++)
	{
		CGXChild* pChild;
		if ((pChild = GetChild(i)) != NULL)
		{
			pChild->OnGridSysColorChange();
		}
	}
}

HBRUSH CGXControl::OnGridCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	// Called from CGXGridCore::DoCtlColor
	pDC, pWnd, nCtlColor;

	return NULL;
}

void CGXControl::OnKillFocus(CWnd* pNewWnd)
{
	if (pNewWnd == NULL)
		return;

	if (pNewWnd != GridWnd() && !Grid()->m_bIgnoreFocus)
	{
		GridWnd()->SendMessage(WM_KILLFOCUS, (WPARAM) pNewWnd->GetSafeHwnd(), 0);
	}
}

BOOL CGXControl::OnGridPreTranslateMessage(MSG* pMsg)
{
	pMsg;

	return FALSE;
}

void CGXControl::ExcludeClipRect(const CRect& rect)
{
	Grid()->m_listExcludeClipRect.AddTail(new CRect(rect));
}

void CGXControl::AddChild(CGXChild* pChild)
{
	if (m_pChildList == NULL)
		m_pChildList = new CPtrList;

	m_pChildList->AddTail(pChild);
}

CGXChild* CGXControl::GetChild(int i)
{
	ASSERT(m_pChildList);
	return (CGXChild*) m_pChildList->GetAt(m_pChildList->FindIndex(i));
}

int CGXControl::GetCount()
{
	if (m_pChildList)
		return m_pChildList->GetCount();

	return 0;
}

void CGXControl::OnInitChildren(ROWCOL nRow, ROWCOL nCol, const CRect& rect)
{
	nRow, nCol, rect;
}


void CGXControl::FocusOnChild(CGXChild* pChild)
{
	m_pChildFocus = pChild;
}

CGXChild* CGXControl::ChildWithFocus()
{
	return m_pChildFocus;
}

void CGXControl::DrawBackground(CDC* pDC, const CRect& rect, const CGXStyle& style, BOOL bErase)
{
	const COLORREF rgbWhite = RGB(255,255,255);
	BOOL bColor = !Grid()->GetParam()->GetProperties()->GetBlackWhite();

	const CGXBrush& brInterior = style.GetInteriorRef();
	CGXBrush brDesc;
	const CGXBrush* pbrDesc;

	// check if background needs to be drawn
	if (bErase || !Grid()->GetParam()->IsTransparentBackground()
			&& (Grid()->m_bDrawCoveredCell       // this flag is set in OnDrawTopLeftBottomRight when a covered cell is drawn
				|| Grid()->m_bForceDrawBackground)
		)
	{
		// No background needed (when printing)?
		if (brInterior.GetStyle() == BS_NULL)
			GXPatB(pDC, rect, bColor ? brInterior.GetBkColor() : rgbWhite);
		else
		{
			if (bColor)
				pbrDesc = &brInterior;
			else
			{
				brDesc = brInterior;
				if (brDesc.GetStyle() == BS_SOLID)
					brDesc.SetColor(rgbWhite).SetBkColor(0);
				else
					brDesc.SetBkColor(rgbWhite).SetColor(0);

				pbrDesc = &brDesc;
			}


			// Make sure patterns are printed as they appear on the screen
			// (by using a bitmap dc)
			Grid()->DrawBrushRect(pDC, rect, *pbrDesc);
		}
	}

	DrawFrame(pDC, rect, style);
}

void CGXControl::DrawFrame(CDC* pDC, const CRect& rect, const CGXStyle& style)
{
	BOOL bColor = !Grid()->GetParam()->GetProperties()->GetBlackWhite();

	if (bColor)
	{
		CRect r = Grid()->SubtractBorders(rect, style, TRUE);

		int n3dFrame = 0;

		if (style.GetIncludeDraw3dFrame())
			n3dFrame = style.GetDraw3dFrame();

		COLORREF rgb3dDkShadow = RGB(0, 0, 0);
		COLORREF rgb3dHilight = GXGetSysData()->GetSysColor(COLOR_BTNHIGHLIGHT);

		switch (n3dFrame)
		{
		case gxFrameRaised:
			GXDraw3dFrame(pDC, r.left-1, r.top-1, r.right, r.bottom, 1,
				  rgb3dHilight, rgb3dDkShadow);
			break;

		case gxFrameInset:
			GXDraw3dFrame(pDC, r.left-1, r.top-1, r.right, r.bottom, 1,
				  rgb3dDkShadow, rgb3dHilight);
			break;
		}
	}
}

void CGXControl::OnGridCancelMode()
{
	// Called from CGXGridCore::DoCancelMode

	// notify children
	for (int i = 0; i < GetCount(); i++)
	{
		CGXChild* pChild;
		if ((pChild = GetChild(i)) != NULL)
		{
			pChild->OnGridCancelMode();
		}
	}
}
