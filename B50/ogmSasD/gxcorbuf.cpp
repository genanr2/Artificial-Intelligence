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

// gxcorbuf.cpp : caching of GDI and style objects (CGXGridCore)
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

#ifndef _GXCORE_H_
#include "gxcore.h"
#endif

#ifndef _GXCTRL_H_
#include "gxctrl.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCORBUF")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

#ifdef TICKCOUNT
#define _TC_(f) f
#else
#define _TC_(f)
#endif

#define USENDX 1

/////////////////////////////////////////////////////////////////////////////
// CGXGridCore

/////////////////////////////////////////////////////////////////////////////
// Caching Styles and GDI Objects

// The cache will be emptied each time OnDraw or OnPrint is called

CFont* CGXGridCore::LookupOrCreateFont(const CGXFont& font)
{
	CFont* pFont = NULL;

	GXBYTE8 key = font.GetFontKey();

	if (!m_mapFontHandles.GetHashTableSize())
		m_mapFontHandles.InitHashTable(257);

	if (!m_mapFontHandles.Lookup(key, (void*&) pFont) || !pFont->GetSafeHandle())
	{
		if (!pFont)
			pFont = new CFont;

		m_mapFontHandles.SetAt(key, (void*) pFont);
		
		// TRACE("Create Font %d - %d\n", font.GetFontKey(), font.GetSize());

		if (!pFont->CreateFontIndirect(&font.GetLogFontRef()))
		{
			TRACE0("Failed to create font in CGXGridCore::LookupOrCreateFont\n");
			// delete pFont;
		}
		ASSERT(pFont->GetSafeHandle());
	}
	else
	{
		ASSERT(pFont->GetSafeHandle());
	}

	return pFont;
}

CBrush* CGXGridCore::LookupOrCreateBrush(const CGXBrush& brush)
{
	CBrush* pBrush = NULL;
	
	GXBYTE8 key = brush.GetBrushKey();

	if (!m_mapBrushHandles.GetHashTableSize())
		m_mapBrushHandles.InitHashTable(257);

	if (!m_mapBrushHandles.Lookup(key, (void*&) pBrush)  || !pBrush->GetSafeHandle())
	{
		LOGBRUSH logbrush = brush;

		// convert system colors to RGB value
		logbrush.lbColor = GXGETCOLOR(logbrush.lbColor);

		if (!pBrush)
			pBrush = new CBrush;

		m_mapBrushHandles[key] = (void*) pBrush;

		if (!pBrush->CreateBrushIndirect((LOGBRUSH*) &logbrush) )
		{
			TRACE0("Failed to create brush in CGXGridCore::LookupOrCreateBrush\n");
			// delete pBrush;
		}
	}
	return pBrush;
}

CPen* CGXGridCore::LookupOrCreatePen(const CGXPen& pen)
{
	CPen* pPen = NULL;
	
	GXBYTE8 key = pen.GetPenKey();

	if (!m_mapPenHandles.GetHashTableSize())
		m_mapPenHandles.InitHashTable(257);

	if (!m_mapPenHandles.Lookup(key, (void*&) pPen) || !pPen->GetSafeHandle())
	{
		LOGPEN logpen = pen;

		// convert system colors to RGB value
		logpen.lopnColor = GXGETCOLOR(logpen.lopnColor);

		if (!pPen)
			pPen = new CPen;
#if _MFC_VER >= 0x0300 && !defined(_MAC) && !defined(_UNIX_)
		CGXBrush brush;
		brush.SetColor(logpen.lopnColor);
		if ( !m_pAppData->bWin31 )
		{
			if ( pPen->CreatePen(pen.lopnStyle | PS_GEOMETRIC | PS_ENDCAP_SQUARE, logpen.lopnWidth.x, brush, 0) )
				m_mapPenHandles[key] = (void*) pPen;
		}
		else
#endif
		m_mapPenHandles[key] = (void*) pPen;

		if (!pPen->CreatePenIndirect((LOGPEN*) &logpen))
		{
			TRACE0("Failed to create pen in CGXGridCore::LookupOrCreatePen\n");
			// delete pPen;
		}
	}
	return pPen;
}

const CGXStyle& CGXGridCore::LookupStyleRowCol(ROWCOL nRow, ROWCOL nCol)
{
	ASSERT(nRow <= GetRowCount() && nCol <= GetColCount());
	// -> ASSERTION Coordinates out of range ->END

//  _TC_(tick10++);
	_TC_(DWORD tkLookup = GetTickCount());

	CGXStyle* pStyle;

#if USENDX
	if (m_bCacheCurrentCell && m_pStyleEdit)
	{
		// is current cell initialized?
		if (nRow == m_nEditRow && nCol == m_nEditCol && m_pStyleEdit->GetStyleBits())
			return *m_pStyleEdit;
	}

	GXNDX ndx(nRow, nCol);

	if (IsCurrentCell(nRow, nCol) && !m_bLockStyleEdit // 
		|| !m_mapStyles.Lookup(ndx, (CObject*&) pStyle))
#else
	// The second alternative uses a WORD value as key for the
	// style (instead of two DWORDs for nRow and nCol). This
	// should increase performance for Lookup.
	// The key is based on the coordinates of the topleft cell.
	// The topleft cell is determined each time the cache has
	// been emptied.

	// one time initialization of cache
	if (m_papAltStyles == NULL)
	{
		m_papAltStyles = new LPVOID[m_nAltStylesCount];
		for (unsigned ui = 0; ui < m_nAltStylesCount; ui++)
			m_papAltStyles[ui] = NULL;
		m_nAltStylesTopRow = 0;
		m_nAltStylesLeftCol = 0;
	}

	if (m_bCacheCurrentCell)
	{
		// is current cell initialized?
		if (nRow == m_nEditRow && nCol == m_nEditCol && m_pStyleEdit->GetStyleBits())
			return *m_pStyleEdit;
	}

	/*
	if (m_nEditRow == nRow && m_nEditCol == nCol)
		TRACE("Reload Edit %d, %d\n", m_pStyleEdit->GetStyleBits(), m_bCacheCurrentCell);
	*/

	// determine base coordinates
	if (m_nAltStylesTopRow == 0)
	{
		//TRACE0("Empty\n");
		m_nAltStylesTopRow = GetTopRow();
		//if (m_nAltStylesTopRow > m_nAltStylesRowOffs)
		//  m_nAltStylesTopRow -= m_nAltStylesRowOffs;
		m_nAltStylesTopRow = max(m_nAltStylesRowOffs+1, m_nAltStylesTopRow)-m_nAltStylesRowOffs;

		m_nAltStylesLeftCol = GetLeftCol();
		//if (m_nAltStylesLeftCol > m_nAltStylesColOffs)
		//  m_nAltStylesLeftCol -= m_nAltStylesColOffs;
		m_nAltStylesLeftCol = max(m_nAltStylesColOffs+1, m_nAltStylesLeftCol)-m_nAltStylesColOffs;
	}

	// determine nRowIndex and nColIndex based on
	// m_nAltStylesTopRow and m_nAltStylesLeftCol

	ROWCOL nfr = GetFrozenRows();
	BYTE nRowIndex = 255;
	if (nRow <= nfr)
		nRowIndex = (BYTE) nRow;
	else if (nRow >= m_nAltStylesTopRow+nfr && nRow + nfr < m_nAltStylesTopRow+m_nAltStylesRows)
		nRowIndex = (BYTE) (nRow - m_nAltStylesTopRow + nfr + 1);

	ROWCOL nfc = GetFrozenCols();
	BYTE nColIndex = 255;
	if (nCol <= nfc)
		nColIndex = (BYTE) nCol;
	else if (nCol >= m_nAltStylesLeftCol+nfc && nCol + nfc < m_nAltStylesLeftCol+m_nAltStylesCols)
		nColIndex = (BYTE) (nCol - m_nAltStylesLeftCol + nfc + 1);

	// Now, I can create the key
	// WORD wKey = MAKEWORD(nRowIndex, nColIndex);
	unsigned wKey = (unsigned) (nRowIndex*m_nAltStylesCols+nColIndex);

	if (nRowIndex >= m_nAltStylesRows
		|| nColIndex >= m_nAltStylesCols
		|| IsCurrentCell(nRow, nCol) && !m_bLockStyleEdit // 
		|| (pStyle = (CGXStyle*) m_papAltStyles[wKey]) == NULL)
#endif
	{
		_TC_(DWORD tkCompose = GetTickCount());

#ifdef _CCHIT
	  {
		LONG l = 1;
		DWORD dw = GXRowColToDWord(nRow, nCol);
		if (m_mapCellHit.Lookup(dw, l))
			l++;

		m_mapCellHit[dw] = l;
	  }
#endif

		// Compose style
		_TC_(DWORD ti1 = GetTickCount());
		pStyle = CreateStyle();
		_TC_(tick1 += GetTickCount()-ti1);

		// TRACE(_T("Compose(%ld, %ld)   (IsCurrentCell() = %d)\n"), nRow, nCol, IsCurrentCell(nRow, nCol));

		ComposeStyleRowCol(nRow, nCol, pStyle);

		if (IsReadOnly())
			pStyle->SetReadOnly(TRUE);

		_TC_(DWORD ti8 = GetTickCount());
#if USENDX
		if (!IsCurrentCell(nRow, nCol))
		{
			m_mapStyles[ndx] = pStyle;
			// if (m_mapStyles.GetCount() % 100 == 0)
			//	TRACE("m_mapStyles: %d el,\n", m_mapStyles.GetCount());
		}
		else
		{
		 	m_listAltStyles.AddHead(pStyle);
			// if (m_listAltStyles.GetCount() % 100 == 0)
			//	TRACE("m_listAltStyles: %d el,\n", m_listAltStyles.GetCount());
		}
#else
		// The cache is a simple array with "m_nAltStylesRows" rows
		// and "m_nAltStylesCols" columns. If the cell coordinates
		// can be mapped to the array, store it.
		if (!IsCurrentCell(nRow, nCol) && nRowIndex < m_nAltStylesRows && nColIndex < m_nAltStylesCols)
		{
			if (m_papAltStyles[wKey])
				RecycleStyle((CGXStyle*) m_papAltStyles[wKey]);
			m_papAltStyles[wKey] = pStyle;
		}
		else
		{
			// ... Otherwise, the style must be stored temporary.
			// FreeObjectCache will free m_listAltStyles.
			m_listAltStyles.AddHead(pStyle);
			// static LONG l = 0;
			// TRACE(_T("AddHead(pStyle); %ld\n"), l++);
		}
#endif
		_TC_(tick8 += GetTickCount()-ti8);

		_TC_(tickComposeStyle += (GetTickCount()-tkCompose));
//      _TC_(tick9++);
	}
//  else
//      ASSERT(b);


	if (nRow == m_nEditRow && nCol == m_nEditCol)
	{
		m_pCurrentCellControl = GetRegisteredControl(pStyle->GetControl());
		*m_pStyleEdit = *pStyle;
		
	}

	_TC_(tickLookup += GetTickCount()-tkLookup);

	ASSERT_VALID(pStyle);

	return *pStyle;
}

CGXStyle* CGXGridCore::CreateStyle()
{
	if (m_StyleBuffer.IsEmpty())
	{
		CGXStyle* pStyle = new CGXStyle;

		return pStyle;
	}

	return (CGXStyle*) m_StyleBuffer.RemoveHead();
}

void CGXGridCore::RecycleStyle(CGXStyle* pStyle)
{
	if (m_StyleBuffer.GetCount() > 500)
	{
		delete pStyle;
		return;
	}
	else
	{
		pStyle->Free();
		m_StyleBuffer.AddHead(pStyle);
	}
}

void CGXGridCore::FreeObjectCache()
{
	if (m_nNestedDraw)
		return;

	// TRACE0("FreeObjectCache()\n");

	_TC_(DWORD tk1 = GetTickCount());

	if (!m_bLockGDIObjects)
	{
		// TRACE0("Freeing GDI-Objects\n");

		ReleaseGridDC();

		// Font
		GXBYTE8 b8key;
		for (POSITION pos = m_mapFontHandles.GetStartPosition(); pos; )
		{
			CFont* pFont;
			m_mapFontHandles.GetNextAssoc(pos, b8key, (void*&) pFont);
			pFont->DeleteObject();
			delete pFont;
		}

		for (POSITION pos = m_mapBrushHandles.GetStartPosition(); pos; )
		{
			CBrush* pBrush;
			m_mapBrushHandles.GetNextAssoc(pos, b8key, (void*&) pBrush);
			pBrush->DeleteObject();
			delete pBrush;
		}

		for (POSITION pos = m_mapPenHandles.GetStartPosition(); pos; )
		{
			CPen* pPen;
			m_mapPenHandles.GetNextAssoc(pos, b8key, (void*&) pPen);
			pPen->DeleteObject();
			delete pPen;
		}

		m_mapFontHandles.RemoveAll();
		m_mapBrushHandles.RemoveAll();
		m_mapPenHandles.RemoveAll();

		ASSERT(m_mapFontHandles.GetCount() == 0);
		ASSERT(m_mapBrushHandles.GetCount() == 0);
		ASSERT(m_mapPenHandles.GetCount() == 0);
	}
	// else
		// TRACE0("Keeping GDI-Objects\n");

#if USENDX
	GXNDX ndx;

	if (m_nLockRow || m_nLockCol || m_nLockRow2 || m_nLockCol2)
	{
		for (POSITION pos = m_mapStyles.GetStartPosition(); pos; )
		{
			CGXStyle* pStyle;
			m_mapStyles.GetNextAssoc(pos, ndx, (CObject*&) pStyle);
			if (!(m_nLockCol && ndx.nCol == m_nLockCol
					|| m_nLockRow && ndx.nRow == m_nLockRow
					|| m_nLockCol2 && ndx.nCol == m_nLockCol2
					|| m_nLockRow2 && ndx.nRow == m_nLockRow2
					|| ndx.nRow == m_nEditRow && ndx.nCol == m_nEditCol)) // current cell
			{
				// Instead of deleting the style, I do reinitialize it,
				// and store it in a buffer. This is a kind of recycling.
				// delete pStyle;
				pStyle->Free();
				m_StyleBuffer.AddHead(pStyle);
				m_mapStyles.RemoveKey(ndx);
			}
		}
	}
	else
	{
		for (POSITION pos = m_mapStyles.GetStartPosition(); pos; )
		{
			CGXStyle* pStyle;
			m_mapStyles.GetNextAssoc(pos, ndx, (CObject*&) pStyle);
			// delete pStyle;
			pStyle->Free();
			m_StyleBuffer.AddHead(pStyle);
		}

		m_mapStyles.RemoveAll();

		if (!m_bLockStyleEdit)
		{
			if (m_pStyleEdit)
				m_pStyleEdit->Free();
			m_pCurrentCellControl = NULL;
		}
	}
#else
	if (m_papAltStyles == NULL)
		return;

	if (m_nLockRow == GX_MAXROWCOL && m_nLockCol == GX_MAXROWCOL)
		; // Do nothing
	else if (m_nLockRow || m_nLockCol)
	{
		ROWCOL nfr = GetFrozenRows();
		BYTE nRowIndex = 0;
		if (m_nLockRow <= nfr)
			nRowIndex = (BYTE) m_nLockRow;
		else if (m_nLockRow >= m_nAltStylesTopRow+nfr && m_nLockRow + nfr < m_nAltStylesTopRow+m_nAltStylesRows)
			nRowIndex = (BYTE) (m_nLockRow - m_nAltStylesTopRow + nfr + 1);

		ROWCOL nfc = GetFrozenCols();
		BYTE nColIndex = 0;
		if (m_nLockCol <= nfc)
			nColIndex = (BYTE) m_nLockCol;
		else if (m_nLockCol >= m_nAltStylesLeftCol+nfc && m_nLockCol + nfc < m_nAltStylesLeftCol+m_nAltStylesCols)
			nColIndex = (BYTE) (m_nLockCol - m_nAltStylesLeftCol + nfc + 1);

		BOOL bEmpty = TRUE;
		for (unsigned w = 0; w < m_nAltStylesCount; w++)
		{
			if (!(nColIndex && w%m_nAltStylesCols == nColIndex
					|| nRowIndex && w/m_nAltStylesCols == nRowIndex))
			{
				CGXStyle* pStyle = (CGXStyle*) m_papAltStyles[w];
				if (pStyle)
				{
					m_papAltStyles[w] = NULL;
					delete pStyle;
				}
			}
			else if (m_papAltStyles[w])
				bEmpty = FALSE;
		}

		if (bEmpty)
		{
			nRowIndex = nColIndex = 0;
			m_nAltStylesTopRow = m_nAltStylesLeftCol = 0;
		}

	}
	else
	{
		//ACE0("Freeing Style-Objects\n");

		if (m_nAltStylesTopRow != 0)
		{
			// Only loop through the list of style object pointers
			// if there was recently a LookupStyleRowCol

			for (unsigned w = 0; w < m_nAltStylesCount; w++)
			{
				CGXStyle* pStyle = (CGXStyle*) m_papAltStyles[w];
				if (pStyle)
				{
					m_papAltStyles[w] = NULL;
					delete pStyle;
				}
			}
		}

		m_nAltStylesTopRow = m_nAltStylesLeftCol = 0;

		if (!m_bLockStyleEdit)
		{
			m_pStyleEdit->Free();
			m_pCurrentCellControl = NULL;
		}
	}
#endif

	// Temporary styles
	int n = m_listAltStyles.GetCount();

	while (n-- > 0)
	{
		CGXStyle* pStyle = (CGXStyle*) m_listAltStyles.RemoveHead();
		delete pStyle;
	}

	_TC_(tick1 += GetTickCount()-tk1);
}

CDC* CGXGridCore::GetGridDC()
{
	if (m_pDrawStruct)
		return m_pDrawStruct->pDC;

	if (m_pGridDC == NULL)
	{
		m_pGridDC = new CClientDC(GridWnd());
		OnGridPrepareDC(m_pGridDC);
	}                  
	
	if (m_hOldFont == 0)
		m_hOldFont = (HFONT) LoadFont(m_pGridDC, StandardStyle().GetFontRef())->GetSafeHandle();

	return m_pGridDC;
}

void CGXGridCore::ReleaseGridDC()
{
	// Skip this if the grid is in drawing mode
	// or busy doing a long operation
	if (m_pDrawStruct || GXGetWaitState()->m_nRefCount > 0)
		return;

	if (m_pGridDC)
	{
		if (m_hOldFont)
			m_pGridDC->SelectObject(CFont::FromHandle(m_hOldFont));
		m_hOldFont = 0;
		delete m_pGridDC;
		m_pGridDC = NULL;
	}
}

CFont* CGXGridCore::LoadFont(CDC* pDC, const CGXStyle& style, const CGXStyle* pStandardStyle)
{
	// Select font
	CFont* pOldFont = NULL;

#ifdef TICKCOUNT
	DWORD tick2 = GetTickCount();
#endif

	// Compare font with the font of the default style
	BOOL bIndividualFont = TRUE;

	if (pStandardStyle)
		bIndividualFont = style.GetIncludeFont() 
			&& memcmp(&style.GetFontRef().GetLogFontRef(),
				&pStandardStyle->GetFontRef().GetLogFontRef(),
				sizeof(LOGFONT)) != 0;

	//TRACE("individual font('%s' %d) = %d\n",
	//	(LPCTSTR) style.GetFontRef().GetDescription(), style.GetFontRef().GetLogFontRef().lfHeight, bIndividualFont);

#ifdef TICKCOUNT
	tickCompStd += (GetTickCount()-tick2);
#endif

	if (bIndividualFont)
		pOldFont = LoadFont(pDC, style.GetFontRef());

	return pOldFont;
}

CFont* CGXGridCore::LoadFont(CDC* pDC, const CGXFont& font)
{
	_TC_(DWORD tk2 = GetTickCount());

	// Font
	CFont* pOldFont = 0;

	// TRACE(_T("SelectObject(Font %s)\n"), (LPCTSTR) style.GetFontRef().GetDescription());

	_TC_(DWORD t = GetTickCount());

	CGXFont fontZoom(font);
	fontZoom.SetSize(font.GetSize()*GetZoom()/100);

	CFont* pFont = LookupOrCreateFont(fontZoom);

	_TC_(tickLookupFont += (GetTickCount()-t));

	if (pFont == 0)
	{
		TRACE1("Failed to create font '%s' in CGXGridCore::LoadFont\n",
			(LPCTSTR) font.GetDescription());
	}
	else
	{

		_TC_(DWORD t = GetTickCount());

		pOldFont = pDC->SelectObject(pFont);

		_TC_(tickLoadFont += (GetTickCount()-t));

		if (pOldFont == 0)
		{
			TRACE1("Failed to select font '%s' into dc in CGXGridCore::LoadFont\n",
				(LPCTSTR) font.GetDescription());
		}
	}

	_TC_(tickIndFont += (GetTickCount()-tk2));

	return pOldFont;
}

