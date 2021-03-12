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

// gxcordra.cpp: Drawing functionality for CGXGridCore
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

#ifndef _GXEXT_H_
#include "gxext.h"
#endif

#ifndef _GXCORE_H_
#include "gxcore.h"
#endif

#ifndef _GXCTRL_H_
#include "gxctrl.h"
#endif

#ifndef _GXDELAYR_H_
#include "gxdelayr.h"
#endif

#ifndef _GXPRIV_H_
#include "gxpriv.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCORDRA")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

#ifdef TICKCOUNT
#define _TC3_(f)
#else
#define _TC3_(f)
#endif

/////////////////////////////////////////////////////////////////////////////
// CGXGridCore


/////////////////////////////////////////////////////////////////////////////
// update mechanism

void CGXGridCore::UpdateAllViews(CWnd* pSender, LPARAM lHint, CObject* pHint)
{
	CDocument* pDoc = GetGridDoc();

	if (m_bHintsEnabled && pDoc != NULL)
		pDoc->UpdateAllViews((CView*) pSender, lHint, pHint);
}

void CGXGridCore::OnGridUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* pHint)
{
	if (m_pUpdateHintImp)
		m_pUpdateHintImp->OnGridUpdate(this, pHint);
}

////////////////////////////////////////////////////////////////////////////
// Drawing cells

void CGXGridCore::RedrawRowCol(const CGXRange& range, UINT flags, BOOL bCreateHint)
{
	if (range.IsCols())
		RedrawRowCol(0, range.left, GetRowCount(), range.right, flags, bCreateHint);
	else if (range.IsRows())
		RedrawRowCol(range.top, 0, range.bottom, GetColCount(), flags, bCreateHint);
	else if (range.IsTable())
		RedrawRowCol(0, 0, GetRowCount(), GetColCount(), flags, bCreateHint);
	else
		RedrawRowCol(range.top, range.left, range.bottom, range.right, flags, bCreateHint);
}

void CGXGridCore::RedrawRowCol(ROWCOL nStartRow, ROWCOL nStartCol, ROWCOL nEndRow, ROWCOL nEndCol, UINT flags, BOOL bCreateHint)
{
	if (!flags || !m_pGridWnd->IsWindowVisible() || IsLockUpdate())
		return;

	EvalVisibleFloatMergeCells(CGXRange(nStartRow, nStartCol, nEndRow, nEndCol), FALSE);

	CRect rect = CalcRectFromRowColExEx(nStartRow, nStartCol, nEndRow, nEndCol);

	InvalidateRect(rect);
	if (flags & GX_UPDATENOW)
		UpdateWindow();

	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintRedrawRowCol(this, nStartRow, nStartCol, nEndRow, nEndCol, flags);
}

void CGXGridCore::Redraw(UINT flags, BOOL bCreateHint)
{
	m_nLastVisibleCol = GX_INVALID;
	m_nLastVisibleRow = GX_INVALID;

	// Store default character-size and width
	// Used for default Cell-Size and
	if (!flags || IsLockUpdate())
		return;

	// TRACE0("CGXGridCore: Redraw()\n");

	ROWCOL nRowCount = GetRowCount();
	ROWCOL nColCount = GetColCount();

	if (GetTopRow() > nRowCount)
	{
		SetTopRow(nRowCount, FALSE);
	}

	if (GetLeftCol() > nColCount)
	{
		SetLeftCol(nColCount, FALSE);
	}

	FreeObjectCache();

	EvalVisibleFloatMergeCells(CGXRange(0, 0, GetLastVisibleRow(), GetFrozenCols()));

	InvalidateRect(GetGridRect());
	UpdateScrollbars();

	if (flags & GX_UPDATENOW)
		UpdateWindow();

	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintRedraw(this, flags);
}

CRect CGXGridCore::SubtractBorders(const CRect& rc, const CGXStyle& style, BOOL bSubtr3dFrame)
{
	// compute interior rectangle without borders

	CRect rect = rc;

	if (bSubtr3dFrame && style.GetIncludeDraw3dFrame() && style.GetDraw3dFrame() != gxFrameNormal)
	{
		rect.left++;
		rect.top++;
		rect.bottom--;
		rect.right--;
	}

	BOOL bNewMode = GetParam()->GetNewGridLineMode();
	BOOL bHorz = GetParam()->GetProperties()->GetDisplayHorzLines();
	BOOL bVert = GetParam()->GetProperties()->GetDisplayVertLines();

	if (style.GetIncludeBorders(gxBorderTop))
	{
		if (style.GetBordersRef(gxBorderTop).GetStyle() != PS_NULL)
			rect.top += max(style.GetBordersRef(gxBorderTop).GetWidth(), 1);
	}
	else if (bSubtr3dFrame && bHorz && !bNewMode)
		rect.top++;

	if (style.GetIncludeBorders(gxBorderBottom))
	{
		if (style.GetBordersRef(gxBorderBottom).GetStyle() != PS_NULL)
			rect.bottom -= max(style.GetBordersRef(gxBorderBottom).GetWidth(), 1);
	}
	else if (bSubtr3dFrame && bHorz && bNewMode)
		rect.bottom--;

	if (style.GetIncludeBorders(gxBorderLeft))
	{
		if (style.GetBordersRef(gxBorderLeft).GetStyle() != PS_NULL)
			rect.left += max(style.GetBordersRef(gxBorderLeft).GetWidth(), 1);
	}
	else if (bSubtr3dFrame && bVert && !bNewMode)
		rect.left++;

	if (style.GetIncludeBorders(gxBorderRight))
	{
		if (style.GetBordersRef(gxBorderRight).GetStyle() != PS_NULL)
			rect.right -= max(style.GetBordersRef(gxBorderRight).GetWidth(), 1);
	}
	else if (bSubtr3dFrame && bVert && bNewMode)
		rect.right--;

	return rect;
}

CSize CGXGridCore::AddBorders(CSize size, const CGXStyle& style)
{
	// compute size with borders

	if (style.GetIncludeDraw3dFrame() && style.GetDraw3dFrame() != gxFrameNormal)
	{
		size.cx += 2;
		size.cy += 2;
	}

	BOOL bNewMode = GetParam()->GetNewGridLineMode();
	BOOL bHorz = GetParam()->GetProperties()->GetDisplayHorzLines();
	BOOL bVert = GetParam()->GetProperties()->GetDisplayVertLines();

	if (style.GetIncludeBorders(gxBorderTop)
		&& style.GetBordersRef(gxBorderTop).GetStyle() != PS_NULL)
		size.cy += max(style.GetBordersRef(gxBorderTop).GetWidth(), 1);
	else if (bHorz && !bNewMode)
		size.cy++;

	if (style.GetIncludeBorders(gxBorderBottom)
		&& style.GetBordersRef(gxBorderBottom).GetStyle() != PS_NULL)
		size.cy += max(style.GetBordersRef(gxBorderBottom).GetWidth(), 1);
	else if (bHorz && bNewMode)
		size.cy++;

	if (style.GetIncludeBorders(gxBorderLeft)
		&& style.GetBordersRef(gxBorderLeft).GetStyle() != PS_NULL)
		size.cx += max(style.GetBordersRef(gxBorderLeft).GetWidth(), 1);
	else if (bVert && !bNewMode)
		size.cy++;

	if (style.GetIncludeBorders(gxBorderRight)
		&& style.GetBordersRef(gxBorderRight).GetStyle() != PS_NULL)
		size.cx += max(style.GetBordersRef(gxBorderRight).GetWidth(), 1);
	else if (bVert && bNewMode)
		size.cy++;

	return size;
}

GXDrawingTechnique CGXGridCore::SetDrawingTechnique(GXDrawingTechnique t)
{
	GXDrawingTechnique r = m_nDrawingTechnique;
	m_nDrawingTechnique = t;
	return r;   // old drawing technique
}

void CGXGridCore::OnGridDraw(CDC* pDC)
{
#if _MFC_VER >= 0x0400
	// Drag & Drop support

	// Erase old focus rectangle
	DndDrawMoveRect(CPoint(0, 0), TRUE, FALSE);
#endif

	m_pWndLastFocus = NULL;

	if (IsLockUpdate())
		return;

	// Refresh last row and column
	CRect rect = GetGridRect();

	// drawing rectangle
	CRect rcClip;
	VERIFY(pDC->GetClipBox(&rcClip) != ERROR);

	if (rcClip.IsRectEmpty())
		return;

	// Don't draw to the screen while the grid is printing
	if (IsPrinting())
	{
		// OnGridPrint will check m_bPrintPaintMsg and redraw the
		// whole grid  later
		m_bPrintPaintMsg = TRUE;
		return;
	}

	// Test grid dimension. The programmer should override this method
	// if the row or column count is not known in advance. (e.g. a CRecordSet query
	// only knows the exact record count only when the cursor has been moved
	// after the last record).

	ROWCOL rowGuess = rect.Height();
	if (GetDefaultRowHeight() > 0)
		rowGuess = rect.Height()/GetDefaultRowHeight();

	ROWCOL colGuess = rect.Width();
	if (GetDefaultColWidth() > 0)
		colGuess = rect.Width()/GetDefaultColWidth();

	OnTestGridDimension(CalcBottomRowFromRect(rcClip) + rowGuess,
		CalcRightColFromRect(rcClip) + colGuess);

	m_bScrollbarsChanged = FALSE;

	// Record count could have been changed (when last record has been seen)
	UpdateScrollbars(TRUE, TRUE);

	// check if CGXGridCore::SetScrollRange() was called
	if (m_bScrollbarsChanged)
	{
		// Scrollbars were changed. This caused invalidation
		// of current window and therefore the grid will receive
		// another WM_PAINT message.
		//
		// In order to avoid flickering because the grid would
		// be redrawn twice, let's immediately return.
		//

		Invalidate();   // just to be sure everything is invalidated
		return;
	}

	rect = GetGridRect();

	// Now, I am sure that GetRowCount and GetColCount does not return
	// too large values.

	if (m_nDrawingTechnique == gxDrawUsingMemDC)
		DrawUsingMemDC(pDC);
	else
		DrawDirectToDC(pDC);
}

void CGXGridCore::DrawDirectToDC(CDC* pDC)
{
	// avoid recursive drawing in UpdateScrollbars
	// if (m_bUpdateScrollbar)
	//  return;

	if (!m_bInitDone)
		TRACE0("Warning! You must call CGXGridCore::Initialize() before drawing\n");
	ASSERT(m_bInitDone);
	// ASSERTION-> Did you call CGXGridCore::Initialize? ->END

	// initialize standard Styles
	if (m_pParam)
	{
		CGXStylesMap* pStyMap = m_pParam->m_pStylesMap;
		pStyMap->LookupStyle(pStyMap->m_wStyleStandard, m_pStyleStandard);
		ASSERT_VALID(m_pStyleStandard);

		pStyMap->LookupStyle(pStyMap->m_wStyleRowHeader, m_pStyleRowHeader);
		ASSERT_VALID(m_pStyleRowHeader);

		pStyMap->LookupStyle(pStyMap->m_wStyleColHeader, m_pStyleColHeader);
		ASSERT_VALID(m_pStyleColHeader);
	}

	//TRACE0("CGXGridCore: BEGIN OnDraw()\n");
#ifdef TICKCOUNT
	DWORD tk1 = GetTickCount();
	DWORD tk2 = GetTickCount();
#endif

	// Free all cached fonts, pens, brushes and styles
	FreeObjectCache();

	// select default font into device context
	CFont* pOldFont = LoadFont(pDC, StandardStyle());

	TRY
	{

		// Default font metrics  m_pParam
		GetFontMetrics(pDC, m_dxWidth, m_dyHeight);

#ifdef TICKCOUNT
		tickStdFont += (GetTickCount()-tk2);
#endif

		// Get rectangle to be drawn
		CRect rectClip;
		if (pDC->GetClipBox(&rectClip) == ERROR)
		{
			TRACE0("CGXGridCore: ERROR with GetClipBox\n");
			rectClip = GetGridRect();
		}
		else if (!rectClip.IntersectRect(rectClip, GetGridRect()))
			return; // ... nothing needs drawn

		// Draw border around grid rectangle
		if (m_bRectDisplay) // checks, if rectangle has been set with SetGridRect
		{
			CRect r = GetGridRect();
			r.left--;
			r.top--;
			GXPatB(pDC, r.left, r.top, 1, r.Height(), 0);
			GXPatB(pDC, r.right, r.top, 1, r.Height(), 0);
			GXPatB(pDC, r.left, r.top, r.Width(), 1, 0);
			GXPatB(pDC, r.left, r.bottom, r.Width()+1, 1, 0);
		}

		pDC->IntersectClipRect(GetGridRect());

		// Compute rows and colums
		ROWCOL topRow, leftCol, bottomRow, rightCol;

		CalcValidClientRowColFromPt(rectClip.TopLeft(), &topRow, &leftCol);
		rectClip.bottom--;
		rectClip.right--;
		CalcValidClientRowColFromPt(rectClip.BottomRight(), &bottomRow, &rightCol);

		if (m_bRefreshOnlyUpdatedCells)
			DrawSelectionFrame(pDC, TRUE, NULL);

		// ... and draw them
		OnDrawTopLeftBottomRight(topRow, leftCol, bottomRow, rightCol, pDC);

		DrawSelectionFrame(pDC);

		// restore old font
		if (pOldFont)
			pDC->SelectObject(pOldFont);

		POSITION pos = m_listExcludeClipRect.GetHeadPosition();
		while (pos)
		{
			CRect* pRect = (CRect*) m_listExcludeClipRect.GetNext(pos);
			delete pRect;
		}
		m_listExcludeClipRect.RemoveAll();
	}
	CATCH_ALL(e)
	{
		// restore old font
		if (pOldFont)
			pDC->SelectObject(pOldFont);

		THROW_LAST();
	}
	END_CATCH_ALL


		// ... Done
#ifdef TICKCOUNT
	tickDraw += GetTickCount() - tk1;
#endif
	// TRACE0("CGXGridCore: END   OnDraw()\n");

	// Free all cached fonts, pens, brushes and styles
	// if (m_nLockRow == 0 && m_nLockCol == 0)
	//  FreeObjectCache();
	ReleaseGridDC();
}

/*
 * This is an alternative OnDraw method
 * It looks nicer (no flickering)
 * but is slower than the original OnDraw
 * (it needs about twice ticks as the original version)
 */

void CGXGridCore::DrawUsingMemDC(CDC* pDC)
{
	// avoid recursive drawing in UpdateScrollbars
	// if (m_bUpdateScrollbar)
	//  return;

	if (!m_bInitDone)
		TRACE0("Warning! You must call CGXGridCore::Initialize() before drawing\n");
	ASSERT(m_bInitDone);
	// ASSERTION-> Did you call CGXGridCore::Initialize? ->END

	// initialize standard Styles
	if (m_pParam)
	{
		CGXStylesMap* pStyMap = m_pParam->m_pStylesMap;
		pStyMap->LookupStyle(pStyMap->m_wStyleStandard, m_pStyleStandard);
		ASSERT_VALID(m_pStyleStandard);

		pStyMap->LookupStyle(pStyMap->m_wStyleRowHeader, m_pStyleRowHeader);
		ASSERT_VALID(m_pStyleRowHeader);

		pStyMap->LookupStyle(pStyMap->m_wStyleColHeader, m_pStyleColHeader);
		ASSERT_VALID(m_pStyleColHeader);
	}

	//TRACE0("CGXGridCore: BEGIN OnDraw()\n");
#ifdef TICKCOUNT
	DWORD tk1 = GetTickCount();
	DWORD tk2 = GetTickCount();
#endif

	// Free all cached fonts, pens, brushes and styles
	FreeObjectCache();

	// initialize standard Styles

	// Get clipping rectangle and return immediately if rectangle is empty .
	CRect rectPaint;
	pDC->GetClipBox(rectPaint);

	if (rectPaint.Width() == 0 || rectPaint.Height() == 0)
		return;

	// paint into a MemDC of same size as the invalid rectangle for less flicker
	CDC dcMem;
	CBitmap bmPaint;
	CBitmap* pbmOld;
	dcMem.CreateCompatibleDC(pDC);
	OnGridPrepareDC(&dcMem);
	bmPaint.CreateCompatibleBitmap(pDC, rectPaint.Width(), rectPaint.Height());
	pbmOld = (CBitmap*) dcMem.SelectObject(&bmPaint);

	dcMem.OffsetViewportOrg(-rectPaint.left, -rectPaint.top);
	dcMem.SetBrushOrg(rectPaint.left % 8, rectPaint.top % 8);   // align dcMem with grid's dc

	// select default font into device context
	CFont* pOldFont = LoadFont(&dcMem, StandardStyle());

	// Default font metrics  m_pParam
	GetFontMetrics(&dcMem, m_dxWidth, m_dyHeight);

#ifdef TICKCOUNT
	tickStdFont += (GetTickCount()-tk2);
#endif

	// Get rectangle to be drawn
	CRect rectClip;
	if (dcMem.GetClipBox(&rectClip) == ERROR)
	{
		TRACE0("CGXGridCore: ERROR with GetClipBox\n");
		rectClip = GetGridRect();
	}
	else if (!rectClip.IntersectRect(rectClip, GetGridRect()))
		return; // ... nothing needs drawn

	dcMem.IntersectClipRect(GetGridRect());

	// Compute rows and colums
	ROWCOL topRow, leftCol, bottomRow, rightCol;

	CalcValidClientRowColFromPt(rectClip.TopLeft(), &topRow, &leftCol);
	rectClip.bottom--;
	rectClip.right--;
	CalcValidClientRowColFromPt(rectClip.BottomRight(), &bottomRow, &rightCol);

	if (m_bRefreshOnlyUpdatedCells)
		DrawSelectionFrame(&dcMem, TRUE, NULL);

	// ... and draw them
	OnDrawTopLeftBottomRight(topRow, leftCol, bottomRow, rightCol, &dcMem);

	DrawSelectionFrame(&dcMem);

	// restore old font
	if (pOldFont)
		dcMem.SelectObject(pOldFont);

	// Draw border around grid rectangle
	if (m_bRectDisplay) // checks, if rectangle has been set with SetGridRect
	{
		CRect r = GetGridRect();
		r.left--;
		r.top--;
		GXPatB(&dcMem, r.left, r.top, 1, r.Height(), 0);
		GXPatB(&dcMem, r.right-1, r.top, 1, r.Height(), 0);
		GXPatB(&dcMem, r.left, r.top, r.Width(), 1, 0);
		GXPatB(&dcMem, r.left, r.bottom-1, r.Width(), 1, 0);
	}

	// memory DC has total picture, copy it onto the real DC
	POSITION pos = m_listExcludeClipRect.GetHeadPosition();
	while (pos)
	{
		CRect* pRect = (CRect*) m_listExcludeClipRect.GetNext(pos);
		pDC->ExcludeClipRect(pRect);
		delete pRect;
	}
	m_listExcludeClipRect.RemoveAll();

	pDC->BitBlt(rectPaint.left, rectPaint.top, rectPaint.Width(), rectPaint.Height(), &dcMem,
			rectPaint.left, rectPaint.top, SRCCOPY);

	// clean-up for memory DC
	dcMem.SelectObject(pbmOld);

	// ... Done, yeah
#ifdef TICKCOUNT
	tickDraw += GetTickCount() - tk1;
#endif
	// TRACE0("CGXGridCore: END   OnDraw()\n");

	// Free all cached fonts, pens, brushes and styles
	// if (m_nLockRow == 0 && m_nLockCol == 0)
	//  FreeObjectCache();
	ReleaseGridDC();
}

void CGXGridCore::DrawGridLine(CDC *pDC, int type, ROWCOL nClientRC, UINT flags)
{
	CRect rect = CalcGridLineRect(type, nClientRC);

	DrawGridLine(pDC, rect.left, rect.top,
		rect.Width(), rect.Height(), type, nClientRC, flags);
}

void CGXGridCore::DrawGridLine(CDC *pDC,int x,int y,int dx,int dy, int type, ROWCOL nClientRC, UINT flags)
{
	// Unused:
	nClientRC;

	COLORREF rgb;

	if (flags & GX_INVERT)
	{
		pDC->InvertRect(CRect(x, y, x+dx, y+dy));
	}
	else if (flags & GX_SELDRAGLINE)
	{
		CPen
			*oldPen,
			penh(m_psSelDragLine, 1,
				GXGETCOLOR(m_pParam->m_pProperties->GetColor(gxnPropDraggingLines)));

		oldPen = pDC->SelectObject(&penh);

		pDC->SetBkMode(OPAQUE);

		GXPatB(pDC, x, y, dx, dy,
			m_pParam->m_pProperties->GetBlackWhite()
			? 0
			: GXGETCOLOR(m_pParam->m_pProperties->GetColor(gxnPropGridLines)));

		pDC->MoveTo(x, y);
		if (type == GX_VERTLINE)
			pDC->LineTo(x, y+dy);
		else
			pDC->LineTo(x+dx, y);

		pDC->SelectObject(oldPen);
		penh.DeleteObject();
	}
	// Tracking
	else if (flags & GX_TRACKLINE)
	{
		GXPatB(pDC, x, y, dx, dy,
			m_pParam->m_pProperties->GetBlackWhite()
				? 0
				: m_pParam->m_pProperties->GetColor(gxnPropTrackingLines));
	}
	else if (flags & GX_ACTIVECELL)
	{
		pDC->InvertRect(CRect(x, y, x+dx, y+dy));
	}
	else
	{
		rgb = 0;

		if (!m_pParam->m_pProperties->GetBlackWhite())
			rgb = GXGETCOLOR(m_pParam->m_pProperties->GetColor(gxnPropGridLines));

		if (pDC->IsPrinting())
		{
			CBrush* pBrush = LookupOrCreateBrush(CGXBrush(BS_SOLID, rgb, 0));
			pDC->FillRect(CRect(x, y, x+dx, y+dy), pBrush);
		}
		else
			GXPatB(pDC, x, y, dx, dy, rgb);
	}
}

CRect CGXGridCore::CalcGridLineRect(int type, ROWCOL nClientRC)
{
	CRect rect;
	
	CRect rectGrid = GetGridRect();

	if (type == GX_HORZLINE)
	{
		int y = nClientRC > 0 ? CalcSumOfClientRowHeights(0, nClientRC-1) : 0;
		ROWCOL nc = GetClientCol(GetColCount());
		int x = CalcSumOfClientColWidths(0, nc, rectGrid.Width());

		if (GetParam()->GetNewGridLineMode())
			y--;
		else
			x++;

		rect.SetRect(rectGrid.left, 
			rectGrid.top+y,
			rectGrid.left+min(x, rectGrid.Width()), 
			rectGrid.top+y+1);
	}
	else if (type == GX_VERTLINE)
	{
		int x = nClientRC > 0 ? CalcSumOfClientColWidths(0, nClientRC-1) : 0;
		ROWCOL nr = GetClientRow(GetRowCount());
		int y = CalcSumOfClientRowHeights(0, nr, rectGrid.Height());

		if (GetParam()->GetNewGridLineMode())
			x--;
		else
			y++;

		rect.SetRect(rectGrid.left+x, 
			rectGrid.top,
			rectGrid.left+x+1, 
			rectGrid.top+min(y, rectGrid.Height()));
	}

	return rect;
}


CGXGridCore::DrawStruct::DrawStruct()
{
	anHeights = NULL;
	anWidths = NULL;
	anYOffset = NULL;
	anXOffset = NULL;
	anRows = NULL;
	anCols = NULL;
	apStyles = NULL;
	abBits = NULL;
}

CGXGridCore::DrawStruct::~DrawStruct()
{
	delete[] anHeights;
	delete[] anWidths;
	delete[] anYOffset;
	delete[] anXOffset;
	delete[] anRows;
	delete[] anCols;
	delete[] abBits;

	// I am casting to (void*) because the style objects
	// referenced in the array must not be deleted
	delete (void*) apStyles;    
}

void CGXGridCore::OnDrawTopLeftBottomRight(ROWCOL topRow, ROWCOL leftCol, ROWCOL bottomRow, ROWCOL rightCol, CDC* pDC)
{
	// TRACE(_T("CGXGridCore: BEGIN OnDrawTopLeftBottomRight(%lu,%lu, %lu,%lu)\n"),
	//   GetRow(topRow), GetCol(leftCol), GetRow(bottomRow), GetCol(rightCol));

	TRY
	{
		unsigned nRow,
				 nCol;

		DrawStruct ds;

		CGXRangeList* pSelList = GetParam()->GetRangeList();

		// Make sure current cell is correctly initialized

		// increase counter for nested drawing
		// FreeObjectCache checks this counter.
		// This makes sure that FreeObjectCache will
		// not empty cached objects.
		m_nNestedDraw++;
		m_pOldDrawStruct = m_pDrawStruct;
		m_pDrawStruct = &ds;

		// But, nested drawing should be avoided when
		// possible. Therefore, I trace a warning message.

#ifdef _DEBUG
		if (m_nNestedDraw > 1)
		{
			TRACE0("Warning: You did start a redraw while the\n");
			TRACE0("grid was redrawing its contents. You might\n");
			TRACE0("consider delay this drawing by passing\n");
			TRACE0("GX_INVALIDATE as parameter if possible or\n");
			TRACE0("if you only call InvalidateRect() and do not\n");
			TRACE0("UpdateWindow()\n");

			// You should also check your GetStyleRowCol methods
			// if they redraw the grid. It normally makes
			// no sense to redraw the grid in GetStyleRowCol.
		}
#endif

		// initialize data and fill cells *******************************

		_TC3_(DWORD tk1 = GetTickCount());

		ds.topRow = topRow;
		ds.leftCol = leftCol;
		ds.bottomRow = bottomRow;
		ds.rightCol = rightCol;
		ds.pDC = pDC;
		pDC->GetClipBox(ds.rectClip);

		ds.nRows = (unsigned) (ds.bottomRow-ds.topRow+1),
		ds.nCols = (unsigned) (ds.rightCol-ds.leftCol+1);
/*		ds.nRows = 0;
		ds.nCols = 0;

        for (ROWCOL nCliRow = ds.topRow; nCliRow <= ds.bottomRow; nCliRow++)
            if (GetRowHeight(GetRow(nCliRow)) > 0)
                ds.nRows++;

        ds.nCols = 0;
        for (ROWCOL nCliCol = ds.leftCol; nCliCol <= ds.rightCol; nCliCol++)
            if (GetColWidth(GetCol(nCliCol)) > 0)
                ds.nCols++;
*/
		ds.gridTopRow = GetTopRow();
		ds.gridLeftCol = GetLeftCol();
		ds.gridFrozenCols = GetFrozenCols();
		ds.gridFrozenRows = GetFrozenRows();
		ds.gridHeaderCols = GetHeaderCols();
		ds.gridHeaderRows = GetHeaderRows();

		ds.rectItem = CalcRectFromClientRowCol(ds.topRow, ds.leftCol);
		ds.nxMin = ds.rectItem.left,
		ds.nyMin = ds.rectItem.top;
		ds.nxMax = ds.nxMin,
		ds.nyMax = ds.nyMin;

		ds.anHeights = new int[ds.nRows];
		ds.anWidths  = new int[ds.nCols];
		ds.anYOffset  = new int[ds.nRows];
		ds.anXOffset  = new int[ds.nCols];
		ds.anRows = new ROWCOL[ds.nRows+1];
		ds.anCols = new ROWCOL[ds.nCols+1];
		ds.apStyles = new LPCSTYLE[ds.nRows*ds.nCols];
		ds.abBits = new WORD[ds.nRows*ds.nCols];
		ds.pCurrentControl = NULL;
		ds.bAnyCellCovered = FALSE;
		ds.bAnyCellSelected = !ds.pDC->IsPrinting() && !pSelList->IsEmpty();

		unsigned i = 0;
		for (nRow = 0; nRow < ds.nRows; nRow++)
		{
			for (nCol = 0; nCol < ds.nCols; nCol++)
			{
				ds.apStyles[i] = 0;
				ds.abBits[i] = 0;
				i++;
			}
		}

		ds.bColor = !GetParam()->GetProperties()->GetBlackWhite();

		ds.rgbStandard = ds.bColor
				? m_pStyleStandard->GetInteriorRef().GetColor()
				: RGB(255,255,255); // white

		for (nRow = 0; nRow < ds.nRows; nRow++)
		{
			ds.anRows[nRow] = GetRow(ds.topRow+nRow);
			ds.anHeights[nRow] = GetRowHeight(ds.anRows[nRow]);
			ds.anYOffset[nRow] = ds.nyMax;
			ds.nyMax += ds.anHeights[nRow];
		}
		ds.anRows[nRow] = GetRow(ds.topRow+nRow);
		if (ds.anRows[nRow] >= GetRowCount())
			ds.anRows[nRow] = 0;
/*		
	
		ROWCOL nRowCount = GetRowCount();
        for (nRow = 0, nCliRow = ds.topRow; nCliRow <= ds.bottomRow; nCliRow++)
        {
			ROWCOL nAbsRow = GetRow(nCliRow);
            if (GetRowHeight(nAbsRow) > 0)
            {
                ds.anRows[nRow] = nAbsRow;
                ds.anHeights[nRow] = GetRowHeight(nAbsRow);
                ds.anYOffset[nRow] = ds.nyMax;
                ds.nyMax += ds.anHeights[nRow];
                nRow++;
            }
        }

        for (;; nCliRow++)
        {
			ROWCOL nAbsRow = GetRow(nCliRow);
            
			if (nAbsRow >= nRowCount)
			{   
				ds.anRows[nRow] = 0; 
				break; 
			}
            else if (GetRowHeight(nAbsRow) > 0)
            {   
				ds.anRows[nRow] = nAbsRow; 
				break; 
			}
        }
        ASSERT(nRow == ds.nRows);
*/
		for (nCol = 0; nCol < ds.nCols; nCol++)
		{
			ds.anCols[nCol] = GetCol(ds.leftCol+nCol);
			ds.anWidths[nCol] = GetColWidth(ds.anCols[nCol]);
			ds.anXOffset[nCol] = ds.nxMax;
			ds.nxMax += ds.anWidths[nCol];
		}
		ds.anCols[nCol] = GetCol(ds.leftCol+nCol);
		if (ds.anCols[nCol] >= GetColCount())
			ds.anCols[nCol] = 0;
/*	
		ROWCOL nColCount = GetColCount();
        for (nCol = 0, nCliCol = ds.leftCol; nCliCol <= ds.rightCol; nCliCol++)
        {
			ROWCOL nAbsCol = GetCol(nCliCol);
            if (GetColWidth(nAbsCol) > 0)
            {
                ds.anCols[nCol] = nAbsCol;
                ds.anWidths[nCol] = GetColWidth(nAbsCol);
                ds.anXOffset[nCol] = ds.nxMax;
                ds.nxMax += ds.anWidths[nCol];
                nCol++;
            }
        }

        for(;; nCliCol++)
        {
			ROWCOL nAbsCol = GetCol(nCliCol);
            
			if (nAbsCol >= nColCount)
			{   
				ds.anCols[nCol] = 0; 
				break; 
			}
            else if (GetColWidth(nAbsCol) > 0)
            {   
				ds.anCols[nCol] = nAbsCol; 
				break; 
			}
        }
        ASSERT(nCol == ds.nCols);
*/
		ROWCOL ncRow = GX_INVALID, ncCol = GX_INVALID;
		if (GetCurrentCell(ncRow, ncCol) && !m_bLockStyleEdit)
		{
			for (nCol = 0; nCol < ds.nCols; nCol++)
				for (nRow = 0; nRow < ds.nRows; nRow++)
					// Current Cell
					if (ds.anRows[nRow] == ncRow && ds.anCols[nCol] == ncCol)
					{
						// set internal cached current cell dirty
						m_pCurrentCellControl = NULL;
						m_pStyleEdit->Free();
					}
		}

		if (GetParam()->GetDrawOrder() == GX_DRAWORDER_COLS)
		{
			for (nCol = 0; nCol < ds.nCols; nCol++)
			{
				for (nRow = 0; nRow < ds.nRows; nRow++)
				{
					int i = nRow*ds.nCols+nCol;
					ds.apStyles[i] = &LookupStyleRowCol(ds.anRows[nRow], ds.anCols[nCol]);
				}
			}
		}

		// if (m_bRefreshOnlyUpdatedCells)
		{
			for (nRow = 0; nRow < ds.nRows; nRow++)
			{
				for (nCol = 0; nCol < ds.nCols; nCol++)
				{
					if (ds.anRows[nRow] > 0 && ds.anCols[nCol] > 0)
					{
						if (GetUpdateFlag(ds.anRows[nRow], ds.anCols[nCol]))
							DelayFloatCells(CGXRange(ds.anRows[nRow], ds.anCols[nCol]));
					}
				}
			}
		}

		// Evaluate float cell state before cells are drawn
		OnDrawTLBR_EvalFloatMergeCell(ds);

		for (nRow = 0; nRow < ds.nRows; nRow++)
		{
			BOOL bRefreshAnyCellInRow = FALSE;

			for (nCol = 0; nCol < ds.nCols; nCol++)
			{
				if (ds.anRows[nRow] == ncRow && ds.anCols[nCol] == ncCol)
					// temporarily set ds.pCurrentControl (will be used
					// in OnDrawTLBR_LoadCell)
					ds.pCurrentControl = GetControl(ncRow, ncCol);
			
				if (m_bRefreshOnlyUpdatedCells)
				{
					if (ds.anRows[nRow] > 0 && ds.anCols[nCol] > 0)
					{
						if (GetUpdateFlag(ds.anRows[nRow], ds.anCols[nCol]))
							bRefreshAnyCellInRow = TRUE;

						// problem with floating cells - borders disappear
					}

					if (bRefreshAnyCellInRow)
						OnDrawTLBR_LoadCell(ds, nRow, nCol);
					else
					{
/*						ds.pDC->ExcludeClipRect(
							ds.anXOffset[nCol], 
							ds.anYOffset[nRow], 
							ds.anXOffset[nCol]+ds.anWidths[nCol],
							ds.anYOffset[nRow]+ds.anHeights[nRow]);
*/
						// Special handling for mem dc drawing 
						m_listExcludeClipRect.AddTail(new CRect(
							ds.anXOffset[nCol], 
							ds.anYOffset[nRow], 
							ds.anXOffset[nCol]+ds.anWidths[nCol],
							ds.anYOffset[nRow]+ds.anHeights[nRow])); 
					}
				}
				else
					OnDrawTLBR_LoadCell(ds, nRow, nCol);
			}

			bRefreshAnyCellInRow = FALSE;
		}

		// Reset pCurrentControl again 
		ds.pCurrentControl = NULL;

		_TC3_(tick1 += GetTickCount()-tk1);

		// Erase Background *********************************************

		// Next, draw all Cells *****************************************

		BOOL bCCellInvert = FALSE;

		// Old Drawing (with clipping rows)

		if (m_pOldDrawingImp == NULL
			|| !m_pOldDrawingImp->OnDrawTLBR(this, pDC, &ds)
			)
		{
			// New Drawing 

			if (m_bRefreshOnlyUpdatedCells)
			{
				for (nRow = 0; nRow < ds.nRows; nRow++)
				{
					BOOL bRefreshAnyCellInRow = FALSE;

					for (nCol = 0; nCol < ds.nCols; nCol++)
					{
						if (ds.anRows[nRow] > 0 && ds.anCols[nCol] > 0)
						{
							if (GetUpdateFlag(ds.anRows[nRow], ds.anCols[nCol]))
							{
								bRefreshAnyCellInRow = TRUE;
							}
						}
					}

					if (bRefreshAnyCellInRow)
					{
						GetParam()->m_UpdatedRangeList.AddTail(new CGXRange(ds.anRows[nRow], ds.anCols[nCol]));

						// Erase the line
						OnDrawTLBR_DrawBackground(ds, nRow);

						ds.rectItem.top = ds.anYOffset[nRow];
						ds.rectItem.bottom = ds.rectItem.top + ds.anHeights[nRow];

						//if (bCanClip)
						{
							pDC->IntersectClipRect(
									CRect(ds.rectClip.left, ds.rectItem.top,
										ds.rectClip.right, ds.rectItem.bottom)
									);
						}

						// draw all columns in row
						for (nCol = 0; nCol < ds.nCols; nCol++)
						{
							ds.rectItem.left = ds.anXOffset[nCol];
							ds.rectItem.right = ds.rectItem.left + ds.anWidths[nCol];

							OnDrawTLBR_DrawCell(ds, nRow, nCol);
						}

						// restore original clipping region
						//if (bCanClip)
						{
							pDC->SelectClipRgn(NULL);
							pDC->IntersectClipRect(ds.rectClip);
						}

						// Current cell
						if (ds.pCurrentControl && !ds.rectEdit.IsRectEmpty() && !bCCellInvert)
						{
							ds.pCurrentControl->InvertBorders(ds.pDC, ds.rectEdit);
							bCCellInvert = TRUE;
						}

						// Draw top and bottom borders (or horizontal grid lines)
						OnDrawTLBR_DrawHorzBorders(ds, nRow);
					}
				}
			}
			else
			{
				for (nRow = 0; nRow < ds.nRows; nRow++)
				{
					// Erase the line
					OnDrawTLBR_DrawBackground(ds, nRow);

					// draw all columns in row
					for (nCol = 0; nCol < ds.nCols; nCol++)
					{
						ds.rectItem.left = ds.anXOffset[nCol];
						ds.rectItem.right = ds.rectItem.left + ds.anWidths[nCol];
						ds.rectItem.top = ds.anYOffset[nRow];
						ds.rectItem.bottom = ds.rectItem.top + ds.anHeights[nRow];

						OnDrawTLBR_DrawCell(ds, nRow, nCol);

						SetUpdateFlag(ds.anRows[nRow], ds.anCols[nCol], FALSE);
					}

					// Current cell
					if (ds.pCurrentControl && !ds.rectEdit.IsRectEmpty() && !bCCellInvert)
					{
						ds.pCurrentControl->InvertBorders(ds.pDC, ds.rectEdit);
						bCCellInvert = TRUE;
					}

					// Draw top and bottom borders (or horizontal grid lines)
					OnDrawTLBR_DrawHorzBorders(ds, nRow);
				}
			}

			_TC3_(tick3 += GetTickCount()-tk3);

			_TC3_(DWORD tk2 = GetTickCount());

			// Erase Background to the right and below grid
			if (!GetParam()->m_bTransparentBackground)
			{
				CRect rectClip = ds.rectClip;
				COLORREF rgb = RGB(255,255,255);

				if (!IsPrinting())
					rgb = m_pParam->m_pProperties->GetColor(gxnPropBackground);

				// Erase unused Background to the right ...
				if (GetCol(ds.rightCol) == GetColCount())
				{
					CRect rect = CalcClientRectRightFromCol(GetCol(ds.rightCol));

					GXPatB(ds.pDC, rect.left, rect.top,
						rect.Width(), rect.Height()+5, rgb);

					if (GetParam()->GetProperties()->GetDisplayVertLines()
						&& !GetParam()->GetNewGridLineMode())
						DrawGridLine(ds.pDC, GX_VERTLINE, ds.rightCol+1);

					rectClip.right = rect.left; // -1;
				}

				// ... and to the bottom
				if (GetRow(ds.bottomRow) == GetRowCount())
				{
					CRect rect = CalcClientRectBottomFromRow(GetRow(ds.bottomRow));
					GXPatB(ds.pDC, rect.left, rect.top,
						rect.Width(), rect.Height(), rgb);

					if (GetParam()->GetProperties()->GetDisplayHorzLines()
						&& !GetParam()->GetNewGridLineMode())
						DrawGridLine(ds.pDC, GX_HORZLINE, ds.bottomRow+1);

					rectClip.bottom = rect.top; //-1;
				}
			}

			// Draw Vertical Borders (and grid lines)
			for (nCol = 0; nCol < ds.nCols; nCol++)
				OnDrawTLBR_DrawVertBorders(ds, nCol);

			// Invert cells
			if (ds.bAnyCellSelected)
				OnDrawTLBR_InvertCells(ds);

			_TC3_(tick2 += GetTickCount()-tk2);
		}

		// **************************************************************

		// Drawstruct will automatically clean up

		// decrease counter for nested drawing
		m_nNestedDraw--;
		m_pDrawStruct = m_pOldDrawStruct;
	}
	CATCH_ALL(e)
	{
		// Drawstruct will automatically clean up

		// decrease counter for nested drawing
		m_nNestedDraw--;
		m_pDrawStruct = m_pOldDrawStruct;

		THROW_LAST();
	}
	END_CATCH_ALL

	// TRACE0("CGXGridCore: END   OnDrawTopLeftBottomRight\n");
}

void CGXGridCore::OnDrawTLBR_EvalFloatMergeCell(DrawStruct& ds)
{
	EvalVisibleFloatMergeCells(CGXRange(ds.anRows[0], ds.anCols[0],
				ds.anRows[ds.nRows-1], ds.anCols[ds.nCols-1]),
			ds.gridFrozenRows, ds.gridFrozenCols,
			ds.gridTopRow, ds.gridLeftCol,
			ds.anRows[ds.nRows-1], // bottom
			ds.anCols[ds.nCols-1], // right
			FALSE, // no update
			NULL,  // boundary
			TRUE); // use LookupStyleRowCol
}

void CGXGridCore::OnDrawTLBR_LoadCell(DrawStruct& ds, unsigned nRow, unsigned nCol)
{
	CGXRangeList* pSelList = GetParam()->GetRangeList();
	int i = nRow*ds.nCols+nCol;

	// Grid line settings
	BOOL bNewMode = GetParam()->GetNewGridLineMode();
	WORD nHorzFlag, nVertFlag;
	GXBorderType tVertLine, tHorzLine;

	if (bNewMode)
	{
		nVertFlag = 0x02;
		nHorzFlag = 0x08;
		tVertLine = gxBorderRight;
		tHorzLine = gxBorderBottom;
	}
	else
	{
		nVertFlag = 0x01;
		nHorzFlag = 0x04;
		tVertLine = gxBorderLeft;
		tHorzLine = gxBorderTop;
	}

	// If cell is hidden by a big spanned cell there is no need
	// to load this cell again

	if ((ds.abBits[i]&gxnFlagSpanned) == 0)
	{
		BOOL bSpanned = FALSE;
		if (m_pDrawSpannedCellsImp)
			bSpanned = m_pDrawSpannedCellsImp->OnDrawTLBR_LoadCell(this, &ds, nRow, nCol);

		if (!bSpanned)
		{
			// This is a regular cell
			if (ds.apStyles[i] == NULL)
				ds.apStyles[i] = &LookupStyleRowCol(ds.anRows[nRow], ds.anCols[nCol]);

			if (ds.anHeights[nRow] > 0 && ds.anWidths[nCol] > 0)
			{
				ASSERT(ds.apStyles[i]->GetIncludeControl());

				if (m_pStyleStandard->GetInteriorRef() != ds.apStyles[i]->GetInteriorRef())
					ds.abBits[i] |= gxnFlagColor;

				if (ds.apStyles[i]->GetIncludeBorders() )
				{
					// Left
					ds.abBits[i] |= 0x01 * ds.apStyles[i]->GetIncludeBorders(gxBorderLeft);

					// Right
					ds.abBits[i] |= 0x02 * ds.apStyles[i]->GetIncludeBorders(gxBorderRight);

					// Top
					ds.abBits[i] |= 0x04 * ds.apStyles[i]->GetIncludeBorders(gxBorderTop);

					// Bottom
					ds.abBits[i] |= 0x08 * ds.apStyles[i]->GetIncludeBorders(gxBorderBottom);
				}

				// Specifies if grid line shall be drawn for that cell

				if ((ds.abBits[i] & nVertFlag) == 0)
				{
					ds.abBits[i] |= gxnFlagVertLine;

					// Fixed column
					if (ds.anCols[nCol] > ds.gridHeaderCols && ds.anCols[nCol] == ds.gridFrozenCols)
						ds.abBits[i] |= gxnFlagFixedVertLine;
				}

				if ((ds.abBits[i] & nHorzFlag) == 0)
				{
					ds.abBits[i] |= gxnFlagHorzLine;

					// Fixed row
					if (ds.anRows[nRow] > ds.gridHeaderRows && ds.anRows[nRow] == ds.gridFrozenRows)
						ds.abBits[i] |= gxnFlagFixedHorzLine;
				}
			}
		}
	}

	// invert cell?
	if (ds.bAnyCellSelected && GetInvertStateRowCol(ds.anRows[nRow], ds.anCols[nCol], pSelList))
		ds.abBits[i] |= gxnFlagSel;
}

void CGXGridCore::OnDrawTLBR_DrawBackground(DrawStruct &ds, unsigned nRow)
{
	const COLORREF rgbWhite = RGB(255,255,255);

	CGXBrush brDesc;
	const CGXBrush* pbrDesc;
	unsigned nCol;

	if (!GetParam()->m_bTransparentBackground)
	{
		CRect rectClip = ds.rectClip;

		// Mark all cells with "background not yet drawn"
		for (nCol = 0; nCol < ds.nCols; nCol++)
			ds.abBits[nRow*ds.nCols+nCol] |= gxnFlagColor;

		// Draw Rectangles
		ds.rectItem.top = ds.anYOffset[nRow];
		ds.rectItem.bottom = ds.rectItem.top + ds.anHeights[nRow];
		ds.rectItem.left = ds.nxMin;

		for (nCol = 0; nCol < ds.nCols; nCol++)
		{
			unsigned i;
			ds.rectItem.right = ds.rectItem.left + ds.anWidths[nCol];

			// Skip spanned cells and cells where I did already
			// draw the background

			if ((ds.abBits[i = nRow*ds.nCols+nCol] & gxnFlagSpanned) == 0
				&& (ds.abBits[i] & gxnFlagColor) > 0)
			{
				if (ds.apStyles[i] == NULL) 
				{
					ds.rectItem.left = ds.rectItem.right;
					continue;
				}

				int nyHeight = ds.anHeights[nRow],
					nxWidth = ds.anWidths[nCol];

				const CGXBrush& brInterior = ds.apStyles[i]->GetInteriorRef();

				unsigned 
					nToCol = nCol+1,
					i2 = nRow*ds.nCols+nCol+1;

				// Width (No. of cells in current row with same color)
				while (nToCol < ds.nCols
					&& (ds.abBits[i2] & gxnFlagSpanned) == 0
					&& (ds.apStyles[i2] == NULL
						|| (ds.abBits[i2] & gxnFlagColor) > 0
							&& ds.apStyles[i2]->GetInteriorRef() == brInterior))
				{
					ds.abBits[i2] &= ~gxnFlagColor;  // mark cell with "background is drawn"
					nxWidth += ds.anWidths[nToCol];
					i2++, nToCol++;
				}

				// Draw cell background in advance only if
				// we can draw more than one cell in advance
				// otherwise the drawing of the background
				// until the CGXControl itself calls DrawBackground
				// in its Draw method --- this gives less flickering

				if (nToCol > nCol+1)
				{
					ds.abBits[i] &= ~gxnFlagColor;

					if (ds.bColor)
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

					DrawBrushRect(ds.pDC,
						CRect(ds.rectItem.left, ds.rectItem.top,
							ds.rectItem.left+nxWidth, ds.rectItem.top+nyHeight),
						*pbrDesc);
				}
			}

			ds.rectItem.left = ds.rectItem.right;
		}
	}
}

void CGXGridCore::OnDrawTLBR_DrawCell(DrawStruct& ds, unsigned nRow, unsigned nCol)
{
	unsigned i = nRow*ds.nCols+nCol;

	// If this is a cell which is hidden by a merge or floating cell
	// and if it is the current cell we have to outline the frame
	// around the cell

	// Special drawing for spanned cell 
	BOOL bSpanned = FALSE;

	if (m_pDrawSpannedCellsImp)
		bSpanned  = m_pDrawSpannedCellsImp->OnDrawTLBR_DrawCell(this, &ds, nRow, nCol);

	// More easy drawing for regular cell 
	if (!bSpanned && ds.apStyles[i])
	{
		if (ds.rectItem.Width() > 0 && ds.rectItem.Height() > 0)
		{
			CGXControl* pControl = NULL;

			if (IsCurrentCell(ds.anRows[nRow], ds.anCols[nCol]))
				pControl = GetRegisteredControl(ds.apStyles[i]->GetControl());

			if ((ds.abBits[i] & gxnFlagColor) > 0)
				// Force erasing the background when cell is not marked
				// with "background is drawn" - see OnDrawTLBR_DrawBackground before
				m_bForceDrawBackground = TRUE;

			OnDrawItem(ds.pDC, ds.anRows[nRow], ds.anCols[nCol],
				ds.rectItem, *ds.apStyles[i]);

			m_bForceDrawBackground = FALSE;

			// Borders will be drawn later

			if (pControl && !pControl->IsActive())
			{
				ds.pCurrentControl = pControl;
				ds.rectEdit = ds.rectItem;
				ds.rectEdit = SubtractBorders(ds.rectEdit, *ds.apStyles[i]);
			}

		}
	}
}

void CGXGridCore::OnDrawTLBR_DrawVertBorders(DrawStruct& ds, unsigned nCol)
{
	// Grid line settings
	BOOL bNewMode = GetParam()->GetNewGridLineMode();
	WORD nVertFlag;
	GXBorderType tVertLine;
	BOOL bDispVertLine = m_pParam->m_pProperties->GetDisplayVertLines();
	BOOL bFixedCol = FALSE;

	if (bDispVertLine)
	{
		m_tempPen
			.SetStyle(m_pParam->GetGridLineStyle())
			.SetWidth(1)
			.SetColor(GXGETCOLOR(m_pParam->m_pProperties->GetColor(gxnPropGridLines)));

		bFixedCol = ds.anCols[nCol] > ds.gridHeaderCols && ds.anCols[nCol] == ds.gridFrozenCols;
	}

	if (bNewMode)
	{
		nVertFlag = 0x02;
		tVertLine = gxBorderRight;
	}
	else
	{
		nVertFlag = 0x01;
		tVertLine = gxBorderLeft;
	}

	// Draw left and right borders (or vertical grid lines)
	for (int nFlag = 1; nFlag <= 2; nFlag *= 2)
	{
		GXBorderType tBorder = (nFlag == 1) ? gxBorderLeft : gxBorderRight;

		for (unsigned nRow = 0; nRow < ds.nRows; nRow++)
		{
			ds.rectItem.left = ds.anXOffset[nCol];
			ds.rectItem.right = ds.rectItem.left + ds.anWidths[nCol];
			ds.rectItem.top = ds.anYOffset[nRow];
			ds.rectItem.bottom = ds.rectItem.top + ds.anHeights[nRow];

			unsigned i = nRow*ds.nCols+nCol;

			BOOL bBorder = ds.apStyles[i] && (ds.abBits[i] & nFlag) > 0;
			BOOL bGridLine = !bBorder 
				&& bDispVertLine 
				&& ((bFixedCol && tBorder == gxBorderRight && (ds.abBits[i] & gxnFlagFixedVertLine) > 0)
					|| (tBorder == tVertLine && (ds.abBits[i] & gxnFlagVertLine) > 0));

			if (bBorder || bGridLine)
			{
				int nyHeight = ds.anHeights[nRow];

				unsigned nToRow = nRow+1,
					i2;

				if (bBorder && !ds.apStyles[i]->GetIncludeBorders())
					continue;

				const CGXPen* pPen;
				const CGXPen* pPen2;
				
				if (bBorder)
					pPen = &ds.apStyles[i]->GetBordersRef(tBorder);
				else
					pPen = &m_tempPen;

				BOOL bNoSolid = pPen->CreateCompatibleBrush(ds.brTemp1, ds.apStyles[i]->GetInteriorRef(), FALSE, !ds.bColor);

				if (bNoSolid)
				{

					// Height (No. of cells in current column with same pen)
					while (nToRow < ds.nRows)
					{
						i2 = nToRow*ds.nCols+nCol;
						BOOL bBorder2 = ds.apStyles[i2] && (ds.abBits[i2] & nFlag) > 0;
						BOOL bGridLine2 = !bBorder 
							&& bDispVertLine 
							&& ((bFixedCol && tBorder == gxBorderRight && (ds.abBits[i2] & gxnFlagFixedVertLine) > 0)
								|| (tBorder == tVertLine && (ds.abBits[i2] & gxnFlagVertLine) > 0));

						if (!bBorder2 && !bGridLine2)
							break;

						if (bBorder2)
							pPen2 = &ds.apStyles[i2]->GetBordersRef(tBorder);
						else
							pPen2 = &m_tempPen;

						if (! (pPen2->CreateCompatibleBrush(
								ds.brTemp2, ds.apStyles[i2]->GetInteriorRef(), 
								FALSE, !ds.bColor)
							&& ds.brTemp1 == ds.brTemp2) )
							break;

						// same pen, same brush
						if (bBorder2)
							ds.abBits[i2] &= ~nFlag;
						else
							ds.abBits[i2] &= ~gxnFlagVertLine;

						nyHeight += ds.anHeights[nToRow];
						nToRow++;
					}
				}
				else
				{
					// Height (No. of cells in current column with same pen)
					while (nToRow < ds.nRows)
					{
						i2 = nToRow*ds.nCols+nCol;
						BOOL bBorder2 = ds.apStyles[i2] && (ds.abBits[i2] & nFlag) > 0;
						BOOL bGridLine2 = !bBorder 
							&& bDispVertLine 
							&& ((bFixedCol && tBorder == gxBorderRight && (ds.abBits[i2] & gxnFlagFixedVertLine) > 0)
								|| (tBorder == tVertLine && (ds.abBits[i2] & gxnFlagVertLine) > 0));

						if (!bBorder2 && !bGridLine2)
							break;

						if (bBorder2)
							pPen2 = &ds.apStyles[i2]->GetBordersRef(tBorder);
						else
							pPen2 = &m_tempPen;

						if (! (*pPen2 == *pPen) )
							break;

						// same pen, (solid - no need to take care on brush)
						if (bBorder2)
							ds.abBits[i2] &= ~nFlag;
						else
							ds.abBits[i2] &= ~gxnFlagVertLine;

						nyHeight += ds.anHeights[nToRow];
						nToRow++;
					}
				}

				if (bNoSolid || bBorder)
				{
					// Border must always be drawn with DrawBorder
					// Grid lines will be drawn with DrawBorder when not solid
					DrawBorder(*pPen,
						CRect(ds.rectItem.left, ds.rectItem.top, ds.rectItem.right, ds.rectItem.top+nyHeight),
						tBorder, ds.pDC, &ds.apStyles[i]->GetInteriorRef());
				}
				else
				{
					// call DrawGridLine for solid grid lines - 
					// for compatibilty with 1.x version
					DrawGridLine(ds.pDC,
						bNewMode ? ds.rectItem.right-1 : ds.rectItem.left,
						ds.rectItem.top,
						1,
						nyHeight,
						GX_VERTLINE,
						nCol+ds.leftCol);
				}
			}
		}

		// Draw right border with fixed line color if this is frozen column
		if (bFixedCol)
			m_tempPen.SetColor(m_pParam->m_pProperties->GetColor(gxnPropFixedLines));
	}
}

void CGXGridCore::OnDrawTLBR_DrawHorzBorders(DrawStruct& ds, unsigned nRow)
{
	// Grid line settings
	BOOL bNewMode = GetParam()->GetNewGridLineMode();
	WORD nHorzFlag;
	GXBorderType tHorzLine;
	BOOL bDispHorzLine = m_pParam->m_pProperties->GetDisplayHorzLines();

	BOOL bFixedRow = FALSE;
	
	if (bDispHorzLine)
	{
		m_tempPen
			.SetStyle(m_pParam->GetGridLineStyle())
			.SetWidth(1)
			.SetColor(GXGETCOLOR(m_pParam->m_pProperties->GetColor(gxnPropGridLines)));

		bFixedRow = ds.anRows[nRow] > ds.gridHeaderRows && ds.anRows[nRow] == ds.gridFrozenRows;
	}

	if (bNewMode)
	{
		nHorzFlag = 0x08;
		tHorzLine = gxBorderBottom;
	}
	else
	{
		nHorzFlag = 0x04;
		tHorzLine = gxBorderTop;
	}

	// Draw top and bottom borders (or horizontal grid lines)
	for (int nFlag = 4; nFlag <= 8; nFlag *= 2)
	{
		GXBorderType tBorder = (nFlag == 4) ? gxBorderTop : gxBorderBottom;

		for (unsigned nCol = 0; nCol < ds.nCols; nCol++)
		{
			ds.rectItem.top = ds.anYOffset[nRow];
			ds.rectItem.bottom = ds.rectItem.top + ds.anHeights[nRow];
			ds.rectItem.left = ds.anXOffset[nCol];
			ds.rectItem.right = ds.rectItem.left + ds.anWidths[nCol];

			unsigned i = nRow*ds.nCols+nCol;

			BOOL bBorder = ds.apStyles[i] && (ds.abBits[i] & nFlag) > 0;
			BOOL bGridLine = !bBorder 
				&& bDispHorzLine 
				&& ((bFixedRow && tBorder == gxBorderBottom && (ds.abBits[i] & gxnFlagFixedHorzLine) > 0)
					|| (tBorder == tHorzLine && (ds.abBits[i] & gxnFlagHorzLine) > 0));

			if (bBorder || bGridLine)
			{
				int nxWidth = ds.anWidths[nCol];

				unsigned nToCol = nCol+1,
					i2;

				if (bBorder && !ds.apStyles[i]->GetIncludeBorders())
					continue;

				const CGXPen* pPen;
				const CGXPen* pPen2;
				
				if (bBorder)
					pPen = &ds.apStyles[i]->GetBordersRef(tBorder);
				else
					pPen = &m_tempPen;

				BOOL bNoSolid = pPen->CreateCompatibleBrush(ds.brTemp1, ds.apStyles[i]->GetInteriorRef(), FALSE, !ds.bColor);

				if (bNoSolid)
				{
					// Width (No. of cells in current row with same pen)
					while (nToCol < ds.nCols)
					{
						i2 = nRow*ds.nCols+nToCol;
						BOOL bBorder2 = ds.apStyles[i2] && (ds.abBits[i2] & nFlag) > 0;
						BOOL bGridLine2 = !bBorder 
							&& bDispHorzLine 
							&& ((bFixedRow && tBorder == gxBorderBottom && (ds.abBits[i2] & gxnFlagFixedHorzLine) > 0)
								|| (tBorder == tHorzLine && (ds.abBits[i2] & gxnFlagHorzLine) > 0));

						if (!bBorder2 && !bGridLine2)
							break;

						if (bBorder2)
							pPen2 = &ds.apStyles[i2]->GetBordersRef(tBorder);
						else
							pPen2 = &m_tempPen;

						if (! (pPen2->CreateCompatibleBrush(
								ds.brTemp2, ds.apStyles[i2]->GetInteriorRef(), 
								FALSE, !ds.bColor)
							&& ds.brTemp1 == ds.brTemp2) )
							break;

						// same pen, same brush
						if (bBorder2)
							ds.abBits[i2] &= ~nFlag;
						else
							ds.abBits[i2] &= ~gxnFlagHorzLine;

						nxWidth += ds.anWidths[nToCol];
						nToCol++;
					}
				}
				else
				{
					// Width (No. of cells in current row with same pen)
					while (nToCol < ds.nCols)
					{
						i2 = nRow*ds.nCols+nToCol;
						BOOL bBorder2 = ds.apStyles[i2] && (ds.abBits[i2] & nFlag) > 0;
						BOOL bGridLine2 = !bBorder 
							&& bDispHorzLine 
							&& ((bFixedRow && tBorder == gxBorderBottom && (ds.abBits[i2] & gxnFlagFixedHorzLine) > 0)
								|| (tBorder == tHorzLine && (ds.abBits[i2] & gxnFlagHorzLine) > 0));

						if (!bBorder2 && !bGridLine2)
							break;

						if (bBorder2)
							pPen2 = &ds.apStyles[i2]->GetBordersRef(tBorder);
						else
							pPen2 = &m_tempPen;

						if (! (*pPen2 == *pPen) )
							break;

						// same pen, (solid - no need to take care on brush)
						if (bBorder2)
							ds.abBits[i2] &= ~nFlag;
						else
							ds.abBits[i2] &= ~gxnFlagHorzLine;

						nxWidth += ds.anWidths[nToCol];
						nToCol++;
					}
				}

				if (bBorder || bNoSolid)
				{
					// Border must always be drawn with DrawBorder
					// Grid lines will be drawn with DrawBorder when not solid
					DrawBorder(*pPen,
						CRect(ds.rectItem.left, ds.rectItem.top, ds.rectItem.left+nxWidth, ds.rectItem.bottom),
						tBorder, ds.pDC, &ds.apStyles[i]->GetInteriorRef());
				}
				else
				{
					// call DrawGridLine for solid grid lines - 
					// for compatibilty with 1.x version
					DrawGridLine(ds.pDC,
						ds.rectItem.left,
						bNewMode ? ds.rectItem.bottom-1 : ds.rectItem.top,
						nxWidth,
						1,
						GX_HORZLINE,
						nRow+ds.topRow);
				}
			}
		}

		// Draw bottom border with fixed line color if this is frozen row
		if (bFixedRow)
			m_tempPen.SetColor(m_pParam->m_pProperties->GetColor(gxnPropFixedLines));
	}
}

void CGXGridCore::OnDrawTLBR_InvertCells(DrawStruct& ds, unsigned nRow)
{
	ds.rectItem.top = ds.anYOffset[nRow];
	ds.rectItem.bottom = ds.rectItem.top + ds.anHeights[nRow];

	ds.rectItem.left = ds.nxMin;

	for (unsigned nCol = 0; nCol < ds.nCols; nCol++)
	{
		ds.rectItem.right = ds.rectItem.left + ds.anWidths[nCol];

		if ((ds.abBits[nRow*ds.nCols+nCol] & gxnFlagSel) > 0)
		{
			unsigned nToCol = nCol+1;
			while (nToCol < ds.nCols && (ds.abBits[nRow*ds.nCols+nToCol] & gxnFlagSel) > 0)
			{
				ds.rectItem.right += ds.anWidths[nToCol];
				nToCol++;
			}

			DrawInvertCell(ds.pDC, ds.anRows[nRow], ds.anCols[nCol], ds.rectItem);
			nCol = nToCol-1;
		}

		ds.rectItem.left = ds.rectItem.right;
	}

	ds.rectItem.top = ds.rectItem.bottom;
}

// Old Drawing (with clipping)

void CGXGridCore::OnDrawTLBR_DrawBackground(DrawStruct &ds)
{
	if (m_pOldDrawingImp)
		m_pOldDrawingImp->OnDrawTLBR_DrawBackground(this, &ds);
}

void CGXGridCore::OnDrawTLBR_InvertCells(DrawStruct& ds)
{
	unsigned nRow;

	for (ds.rectItem.top = ds.nyMin, nRow = 0; nRow < ds.nRows; nRow++)
	{
		ds.rectItem.bottom = ds.rectItem.top + ds.anHeights[nRow];

		ds.rectItem.left = ds.nxMin;

		for (unsigned nCol = 0; nCol < ds.nCols; nCol++)
		{
			ds.rectItem.right = ds.rectItem.left + ds.anWidths[nCol];

			if ((ds.abBits[nRow*ds.nCols+nCol] & gxnFlagSel) > 0)
				DrawInvertCell(ds.pDC, ds.anRows[nRow], ds.anCols[nCol], ds.rectItem);

			ds.rectItem.left = ds.rectItem.right;
		}

		ds.rectItem.top = ds.rectItem.bottom;
	}
}

void CGXGridCore::DrawInvertCell(CDC *pDC, ROWCOL nRow, ROWCOL nCol, CRect rectItem)
{
	// Invert cell (selected cells)

	// Don't invert current cell when m_bInvertRect is FALSE. SetCurrentCell
	// will set m_bInvertRect = FALSE

	BOOL bCCell = IsCurrentCell(nRow, nCol);

	if (!bCCell || m_bInvertRect || m_pParam->GetHideCurrentCell() == GX_HIDE_ALWAYS)
		pDC->InvertRect(&rectItem);

	if (bCCell && GetParam()->GetExcelLikeCurrentCell())
	{
		// TRACE(_T("m_bExcelLikeCurrentCell\n"));
		CRect r = rectItem;

		if (GetParam()->GetNewGridLineMode())
		{
			pDC->InvertRect(CRect(r.left, r.top, r.right, r.top+2));
			pDC->InvertRect(CRect(r.left, r.top+2, r.left+2, r.bottom));
			pDC->InvertRect(CRect(r.right-3, r.top+2, r.right, r.bottom-3));
			pDC->InvertRect(CRect(r.left+2, r.bottom-3, r.right, r.bottom));
		}
		else
		{
			pDC->InvertRect(CRect(r.left, r.top, r.right-2, r.top+3));
			pDC->InvertRect(CRect(r.left, r.top+3, r.left+3, r.bottom-2));
			pDC->InvertRect(CRect(r.left, r.bottom-2, r.right-2, r.bottom));
			pDC->InvertRect(CRect(r.right-2, r.top, r.right, r.bottom));
		}
	}

	m_bInvertRect = FALSE;
}

void CGXGridCore::OnDrawBorders(CDC *pDC, CRect rectItem, const CGXStyle& style)
{
	// Draw Borders

	if (style.GetIncludeBorders())
	{
		if (style.GetIncludeBorders(gxBorderTop))
			DrawBorder(style.GetBordersRef(gxBorderTop), rectItem, gxBorderTop, pDC, &style.GetInteriorRef());
		if (style.GetIncludeBorders(gxBorderBottom))
			DrawBorder(style.GetBordersRef(gxBorderBottom), rectItem, gxBorderBottom, pDC, &style.GetInteriorRef());
		if (style.GetIncludeBorders(gxBorderLeft))
			DrawBorder(style.GetBordersRef(gxBorderLeft), rectItem, gxBorderLeft, pDC, &style.GetInteriorRef());
		if (style.GetIncludeBorders(gxBorderRight))
			DrawBorder(style.GetBordersRef(gxBorderRight), rectItem, gxBorderRight, pDC, &style.GetInteriorRef());
	}
}

void CGXGridCore::DrawBorder(const CGXPen& pen, const CRect& rc, GXBorderType bt, CDC* pDC, const CGXBrush* pbrInterior)
{
	ASSERT(pen.GetInclude());
	// ASSERTION-> Did you check pen.GetInclude()? ->END
	ASSERT(bt >= 0 && bt <= CGXStyle::nBorders);
	// ASSERTION-> BorderType out of range ->END
	ASSERT(pDC != NULL && pDC->IsKindOf(RUNTIME_CLASS(CDC)));
	// ASSERTION-> Invalid Device Context ->END

	if (pen.GetStyle() == PS_NULL)
		return;

	int w = max(pen.GetWidth(), 1);

	if (pbrInterior && pen.CreateCompatibleBrush(m_tempBrush, *pbrInterior, bt == gxBorderTop || bt == gxBorderBottom, GetParam()->GetProperties()->GetBlackWhite()))
	{
		CRect rect;

		switch (bt)
		{
		case gxBorderTop:
			rect.SetRect(rc.left, rc.top, rc.right, rc.top+w);
			break;
		case gxBorderLeft:
			rect.SetRect(rc.left, rc.top, rc.left+w, rc.bottom);
			break;
		case gxBorderBottom:
			rect.SetRect(rc.left, rc.bottom-w, rc.right, rc.bottom);
			break;
		case gxBorderRight:
			rect.SetRect(rc.right-w, rc.top, rc.right, rc.bottom);
			break;
		}

		DrawBrushRect(pDC, rect, m_tempBrush);
	}
	else if (w == 1 && pen.lopnStyle > 0)
	{
		// Select Pen
		CPen* pPen;
		if (GetParam()->GetProperties()->GetBlackWhite())
			pPen = LookupOrCreatePen(CGXPen(pen).SetColor(RGB(0,0,0)));
		else
			pPen = LookupOrCreatePen(pen);

		if (pPen == 0)
		{
			TRACE1("Failed to create pen '%s' in CGXGridCore::DrawFrameLine\n",
				(LPCTSTR) pen.GetDescription());
			return;
		}

		CPen* pOldPen = pDC->SelectObject(pPen);
		if (pOldPen == 0)
		{
			TRACE1("Failed to select pen '%s' into dc in CGXGridCore::DrawFrameLine\n",
				(LPCTSTR) pen.GetDescription());
			return;
		}

		// Draw
		switch (bt)
		{
		case gxBorderTop:
			pDC->MoveTo(rc.left, rc.top);
			pDC->LineTo(rc.right-1, rc.top);
			break;
		case gxBorderLeft:
			pDC->MoveTo(rc.left, rc.top);
			pDC->LineTo(rc.left, rc.bottom-1);
			break;
		case gxBorderBottom:
			pDC->MoveTo(rc.left, rc.bottom-1);
			pDC->LineTo(rc.right-1, rc.bottom-1);
			break;
		case gxBorderRight:
			pDC->MoveTo(rc.right-1, rc.top);
			pDC->LineTo(rc.right-1, rc.bottom-1);
			break;
		}

		if (pOldPen)
			pDC->SelectObject(pOldPen);
	}
	else if (pDC->IsPrinting())
	{
		COLORREF rgb = pen.GetColor();
		if (GetParam()->GetProperties()->GetBlackWhite())
			rgb = 0;

		CBrush* pBrush = LookupOrCreateBrush(CGXBrush(BS_SOLID, rgb, 0));
		switch (bt)
		{
		case gxBorderTop:
			pDC->FillRect(CRect(rc.left, rc.top, rc.right, rc.top+w), pBrush);
			break;
		case gxBorderLeft:
			pDC->FillRect(CRect(rc.left, rc.top, rc.left+w, rc.bottom), pBrush);
			break;
		case gxBorderBottom:
			pDC->FillRect(CRect(rc.left, rc.bottom-w, rc.right, rc.bottom), pBrush);
			break;
		case gxBorderRight:
			pDC->FillRect(CRect(rc.right-w, rc.top, rc.right, rc.bottom), pBrush);
			break;
		}
	}
	else // normal display
	{
		COLORREF rgb = pen.GetColor();
		if (GetParam()->GetProperties()->GetBlackWhite())
			rgb = 0;

		// Draw
		switch (bt)
		{
		case gxBorderTop:
			GXPatB(pDC, rc.left, rc.top, rc.Width(), w, rgb);
			break;
		case gxBorderLeft:
			GXPatB(pDC, rc.left, rc.top, w, rc.Height(), rgb);
			break;
		case gxBorderBottom:
			GXPatB(pDC, rc.left, rc.bottom-w, rc.Width(), w, rgb);
			break;
		case gxBorderRight:
			GXPatB(pDC, rc.right-w, rc.top, w, rc.Height(), rgb);
			break;
		}
	}
}

void CGXGridCore::DrawBrushRect(CDC* pDC, const CRect& rect, const CGXBrush& brush)
{
	if (brush.GetStyle() == BS_NULL)
	{
		GXPatB(pDC, rect, brush.GetBkColor());
	}
	else
	{
		// Make sure patterns are printed as they appear on the screen
		// (by using a bitmap dc)
		if (pDC->IsPrinting() && brush.GetStyle() > BS_HATCHED)
		{
			// Draw
			CDC dcMem;
			CBitmap bmPaint;
			CBitmap* pbmOld;
			dcMem.CreateCompatibleDC(pDC);
			bmPaint.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
			pbmOld = (CBitmap*) dcMem.SelectObject(&bmPaint);

			dcMem.SetTextColor(brush.GetColor());
			dcMem.SetBkColor(brush.GetBkColor());
			dcMem.SetBkMode(OPAQUE);
			dcMem.FillRect(CRect(0, 0, rect.Width(), rect.Height()),
				LookupOrCreateBrush(brush));

			pDC->StretchBlt(rect.left, rect.top,
				rect.Width(), rect.Height(),
				&dcMem,
				0, 0,
				rect.Width(), rect.Height(),
				SRCCOPY);

			dcMem.SelectObject(pbmOld);
		}
		else
		{
			// Just use the FillRect method for the screen (is faster)
			pDC->SetBkColor(brush.GetBkColor());
			pDC->SetTextColor(brush.GetColor());
			pDC->SetBkMode(OPAQUE);
			pDC->FillRect(rect, LookupOrCreateBrush(brush));
		}
	}
}

void CGXGridCore::OnDrawItem(CDC *pDC, ROWCOL nRow, ROWCOL nCol, const CRect& rectItem, const CGXStyle& style)
{
	ASSERT(pDC != NULL && pDC->IsKindOf(RUNTIME_CLASS(CDC)));
	// ASSERTION-> Invalid Device Context ->END
	ASSERT(nRow <= GetRowCount() && nCol <= GetColCount());
	// ASSERTION-> Cell coordinates out of range ->END

	// TRACE(_T("CGXGridCore::OnDrawItem(%lu, %lu, (%d,%d,%d,%d), pDC)\n"),
	//    nRow, nCol, (RECT) rectItem);

#ifdef TICKCOUNT
	DWORD tick1 = GetTickCount();
#endif

	CGXControl* pControl = GetRegisteredControl(style.GetControl());
	ASSERT(pControl && pControl->IsKindOf(CONTROL_CLASS(CGXControl)));
	// ASSERTION-> The control returned by GetControl is not derived from CGXControl. ->END

	// if this is the current cell, is control initialized correctly?
	if (nRow && nCol 
		&& IsCurrentCell(nRow, nCol) 
		&& !pControl->IsInit(nRow, nCol))
	{
		// TRACE0("Warning! Current cell has been  reinitialized in OnDrawItem\n");
		// This is ok and will happen after you change the cell type of
		// the current cell with SetStyleRange, ..!
		
		pControl->Init(nRow, nCol);
	}

	pControl->Draw(pDC, rectItem, nRow, nCol, style, m_pStyleStandard);

#ifdef TICKCOUNT
	tickDrawItem += GetTickCount()-tick1;
#endif
}

void CGXGridCore::MarkEdit(ROWCOL nEditRow, ROWCOL nEditCol, UINT direction, UINT flags)
{
	if (m_pMarkEditHeaderImp)
		m_pMarkEditHeaderImp->MarkEdit(this, nEditRow, nEditCol, direction, flags);
}

/////////////////////////////////////////////////////////////////////////////
// CGXGridCore printing

void CGXGridCore::OnAdjustPrintRectangle(CDC* /*pDC*/, CPrintInfo* pInfo)
{
	CGXGridParam* pParam = GetParam();
	CGXProperties* pProp = pParam->GetProperties();

	// subtract left, right, bottom and top Frame
	int nTop, nLeft, nBottom, nRight;

	pProp->GetMargins(nTop, nLeft, nBottom, nRight);

	pInfo->m_rectDraw.top += nTop;
	pInfo->m_rectDraw.left += nLeft;
	pInfo->m_rectDraw.bottom -= nBottom;
	pInfo->m_rectDraw.right -= nRight;
}

void CGXGridCore::OnPrintHeaderAndFooter(CDC* pDC, CPrintInfo* pInfo)
{
	CGXGridParam* pParam = GetParam();
	CGXProperties* pProp = pParam->GetProperties();

	CDocument* pDoc = NULL;
	if (m_pGridWnd->IsKindOf(RUNTIME_CLASS(CView)))
		pDoc = ((CView*) m_pGridWnd)->GetDocument();

	pProp->OnPrintHeaderAndFooter(pDC, pInfo, pDoc, pParam);
}


/////////////////////////////////////////////////////////////////////////////
// Common members

BOOL CGXGridCore::CalcClientRowColFromPt(CPoint& pt, ROWCOL* nRow, ROWCOL* nCol, CRect* rect /* = NULL */)
{
	BOOL    bvalid;
	int    x = 0,
			y = 0;

	if (pt.x < 0 || pt.y < 0)
	{
		TRACE3("CGXGridCore: File %s, line %d: CPoint (%d,%d) invalid\n", __FILE__, __LINE__, pt);
		*nCol = 0;
		*nRow = 0;
		return FALSE;
	}

	*nCol = CalcClientColFromPt(pt, &x);
	*nRow = CalcClientRowFromPt(pt, &y);

	bvalid = *nRow != GX_INVALID && *nCol != GX_INVALID;
	if (bvalid && rect)
	{
		rect->SetRect(x, y, x+GetColWidth(GetCol(*nCol)), y+GetRowHeight(GetRow(*nRow)));
	}

	return bvalid;
}

CRect CGXGridCore::CalcRectFromClientRowCol(ROWCOL nRow, ROWCOL nCol)
{
	CRect rectGrid = GetGridRect();

	rectGrid.right *= 2;
	rectGrid.bottom *= 2;

	for (m_rectRC.top = rectGrid.top, m_RCnRowClient = 0;
		m_rectRC.top <= rectGrid.bottom && m_RCnRowClient < nRow;
		m_RCnRowClient++)
		m_rectRC.top += GetRowHeight(GetRow(m_RCnRowClient));

	for (m_rectRC.left = rectGrid.left, m_RCnColClient = 0;
		m_rectRC.left <= rectGrid.right && m_RCnColClient < nCol;
		m_RCnColClient++)
		m_rectRC.left += GetColWidth(GetCol(m_RCnColClient));

	// Exclude Frame
	m_rectRC.bottom = m_rectRC.top + GetRowHeight(GetRow(nRow)) ;
	m_rectRC.right = m_rectRC.left + GetColWidth(GetCol(nCol)) ;

	return m_rectRC;
}

CRect CGXGridCore::CalcNextRectFromClientRowCol(ROWCOL nRow, ROWCOL nCol)
{
	CRect rectGrid = GetGridRect();

	rectGrid.right *= 2;
	rectGrid.bottom *= 2;

	if (m_RCnRowClient > nRow && m_RCnRowClient > nRow)
	{
		TRACE2("CGXGridCore: Row %d, Col %d: Invalid use of CalcNextRectFromClientRowCol\n", nRow, nCol);
	}

	if (m_RCnRowClient > nRow)
	{
		m_RCnRowClient = 0;
		m_rectRC.top = rectGrid.top;
	}

	for (; m_rectRC.top <= rectGrid.bottom && m_RCnRowClient < nRow; m_RCnRowClient++)
		m_rectRC.top += GetRowHeight(GetRow(m_RCnRowClient));

	if (m_RCnColClient > nCol)
	{
		m_RCnColClient = 0;
		m_rectRC.left = rectGrid.left;
	}
	for (; m_rectRC.left <= rectGrid.right && m_RCnColClient < nCol; m_RCnColClient++)
		m_rectRC.left += GetColWidth(GetCol(m_RCnColClient));

	// Exclude Frame
	m_rectRC.bottom = m_rectRC.top + GetRowHeight(GetRow(nRow)) ;
	m_rectRC.right = m_rectRC.left + GetColWidth(GetCol(nCol)) ;

	return m_rectRC;
}


/////////////////////////////////////////////////////////////////////////////
// CGXGridCore coordinate calculating member-functions

void CGXGridCore::CalcValidClientRowColFromPt(CPoint& pt, ROWCOL* nRow, ROWCOL* nCol)
{
	if (!CalcClientRowColFromPt(pt, nRow, nCol, NULL))
	{
		if (*nRow == GX_INVALID)
			*nRow = GetClientRow(GetRowCount());
		if (*nCol == GX_INVALID)
			*nCol = GetClientCol(GetColCount());
	}
}

/////////////////////////////////////////////////////////////////////////////
// Optimized redrawing and recalculation (only marked cells)

BOOL CGXGridCore::GetUpdateFlag(ROWCOL nRow, ROWCOL nCol)
{
	// is update flag set
	CGXData* pData = GetParam()->GetData();

	CGXRange rg(nRow, nCol);
	GetCoveredCellsRowCol(nRow, nCol, rg);
	
	return pData && pData->GetUpdateFlag(rg.top, rg.left);
}

void CGXGridCore::SetUpdateFlag(ROWCOL nRow, ROWCOL nCol, BOOL bToggle)
{
	// set update flag (TRUE if cell shall be updated later)
	CGXData* pData = GetParam()->GetData();
	
	if (pData)
		pData->SetUpdateFlag(nRow, nCol, bToggle);
}

class __CGXData: public CGXData
{
	friend class CGXGridCore;
};

BOOL CGXGridCore::RefreshViews(BOOL bRecalc, CString* pErrorStr, BOOL bCreateHint)
{
	// Test if SetUpdateFlag and GetUpdateFlag is implemented.
	// Otherwise return immediately

	if (GetRowCount() < 1 || GetColCount() < 1)
		return FALSE;

	__CGXData* pData = (__CGXData*) GetParam()->GetData();

	pData->m_tmpValueStyle.SetIncludeValue(FALSE);

	// test if GetUpdateFlag is overridden (CGXData::GetUpdateFlag modifies m_tmpValueStyle)
	GetUpdateFlag(1, 1);

	if (pData->m_tmpValueStyle.GetIncludeValue())
		return FALSE; // CGXData::GetUpdateFlag does nothing

	// Only the first view needs to call pData->Recalc
	if (bRecalc)
	{
		ResetError();
		Recalc();
	}

	BOOL b = m_bLockGDIObjects;
	m_bLockGDIObjects = TRUE;

	UpdateWindow();  // make sure there are no pending invalid areas

	m_bRefreshOnlyUpdatedCells = TRUE;

	Redraw(GX_UPDATENOW, FALSE);
		// loop through visible cells and redraw only cells with update flag

	m_bRefreshOnlyUpdatedCells = FALSE;
	m_bLockGDIObjects = b;

	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintRefreshViews(this, pErrorStr);

	CString s;
	BOOL bRet = TRUE;
	if (bRecalc && GetError(s))
	{
		if (pErrorStr)
			*pErrorStr = s;
		else
			AfxMessageBox(s);

		ResetError();

		bRet = FALSE;
	}

	POSITION pos = GetParam()->m_UpdatedRangeList.GetHeadPosition();
	
	while (pos)
	{
		CGXRange* pRange = GetParam()->m_UpdatedRangeList.GetNext(pos);
		SetUpdateFlag(pRange->top, pRange->left, FALSE);
		delete pRange;								 
	}

	GetParam()->m_UpdatedRangeList.RemoveAll();

	return bRet;
}

void CGXGridCore::AddRecalcRange(const CGXRange& range)
{
	CGXData* pData = GetParam()->GetData();

	if (pData && !m_bLockAddRecalcRange)
		pData->AddRecalcRange(range);
}

void CGXGridCore::Recalc(BOOL bForceRecalc)
{
	CGXData* pData = GetParam()->GetData();

	if (pData)
		pData->Recalc(bForceRecalc);
}

void CGXGridCore::ResetError()
{
	CGXData* pData = GetParam()->GetData();

	if (pData)
		pData->ResetError();
}

BOOL CGXGridCore::GetError(CString& strError)
{
	CGXData* pData = GetParam()->GetData();

	return pData && pData->GetError(strError);
}

/////////////////////////////////////////////////////////////////////////////
// Floating and Merging Cells

BOOL CGXGridCore::EvalVisibleFloatMergeCells(
	const CGXRange& rg,
	ROWCOL nfr,
	ROWCOL nfc,
	ROWCOL nTopRow,
	ROWCOL nLeftCol,
	ROWCOL nLastRow,
	ROWCOL nLastCol,
	BOOL bUpdate, //= TRUE
	CGXRange* prgBoundary, // = NULL
	BOOL bLookupStyle) //
{
	if (m_pDrawSpannedCellsImp)
		return m_pDrawSpannedCellsImp->EvalVisibleFloatMergeCells(
			this, rg, nfr, nfc,
			nTopRow, nLeftCol, nLastRow, nLastCol,
			bUpdate, prgBoundary, bLookupStyle);

	return FALSE;
}

