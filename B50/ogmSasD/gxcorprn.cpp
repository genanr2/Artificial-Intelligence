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

// gxcorprn.cpp: Printing and print preview for grids

#include "stdafx.h"

// Headers

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

#ifndef _GXEXT_H_
#include "gxext.h"
#endif

#ifndef _GXCORE_H_
#include "gxcore.h"
#endif

#ifndef _GXABSTR_H_
#include "gxabstr.h"
#endif

#include "gxdelayr.h"

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCORPRN")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

// Debug Info

class CGXGridPrintImp: public CGXAbstractGridPrintImp
{
public:
	virtual void OnGridBeginPrinting(CGXGridCore* pGrid, CDC* pDC, CPrintInfo* pInfo);
	virtual void OnGridPrint(CGXGridCore* pGrid, CDC* pDC, CPrintInfo* pInfo);
	virtual void OnGridEndPrinting(CGXGridCore* pGrid, CDC* pDC, CPrintInfo* pInfo);

	virtual void OnPrintHeaderAndFooter(CGXProperties* pProp, CDC* pDC, CPrintInfo* pInfo, CDocument* pDoc, CGXGridParam* pParam);
	virtual int CalculateHeight(CGXProperties* pProp, CDC* pDC, CGXData &data, CGXStylesMap* pStylesMap);
	virtual void PrintHeaderFooter(CGXProperties* pProp, CDC* pDC, CPrintInfo* pInfo, CRect& rect, CGXData& data, CGXStylesMap* pStylesMap);
	virtual CString ParseText(CGXProperties* pProp, LPCTSTR sz);
	virtual CString SubstTokenText(CGXProperties* pProp, int nToken, CString& sRest);
	virtual CString GetTokenArgs(CGXProperties* pProp, CString& sRest);
};

#define new DEBUG_NEW

void CGXGridCore::ImplementPrinting()
{
	if (m_pPrintImp == NULL)
		AutoDeletePtr(m_pPrintImp = new CGXGridPrintImp);
}

void CGXGridPrintImp::OnGridBeginPrinting(CGXGridCore* pGrid, CDC* pDC, CPrintInfo* pInfo)
{
	// For some grids, the row or column count is not known in advance. 
	// (e.g. a CRecordSet query only knows the exact record count only 
	// when the cursor has been moved after the last record).

	// Printing works fine even if the record count is not
	// known. While printing the pages or stepping through
	// the pages with print preview the record count will
	// automatically adjusted until the last record in the
	// recordset has been seen.

	// However, if you want to print the page count in the footer
	// in your print outs, OnTestGridDimension should be called.
	// With the m_nPrintHandleRecordCount you can specify if
	// you need to know the page count
	// a) gxCountPrint - Only when printing but not for print preview (default)
	// b) gxCountAlways - Both for printing and print preview 
	// c) gxCountNever - Never

	switch (pGrid->m_nPrintHandleRecordCount)
	{
	case gxCountAlways: 
		pGrid->OnTestGridDimension(GX_MAXROWCOL, GX_MAXROWCOL); 
		break;

	case gxCountPrint: 
		if (!pInfo->m_bPreview)
			 pGrid->OnTestGridDimension(GX_MAXROWCOL, GX_MAXROWCOL); 
		break;

	default:
		;
		// no count
	}

	CGXGridParam* pParam = pGrid->GetParam();
	ASSERT(pParam->IsKindOf(RUNTIME_CLASS(CGXGridParam)));

	CGXProperties* pProp = pParam->GetProperties();
	ASSERT(pProp->IsKindOf(RUNTIME_CLASS(CGXProperties)));

	CGXWaitCursor theWait;

	pGrid->OnGridPrepareDC(pDC, pInfo);

	pGrid->m_awPageFirstRow.RemoveAll();
	pGrid->m_awPageFirstCol.RemoveAll();

	// TRACE0("CGXGridPrintImp::OnBeginPrinting()\n");

	CFont* pOldFont = NULL;

	ROWCOL      nCurRow = pGrid->GetTopRow(),
				nCurCol = pGrid->GetLeftCol();

	pGrid->m_bPrintPaintMsg = FALSE;

	pGrid->SetPrinting(TRUE);

	// Float Cells
	CGXAppData* pApp = GXGetAppData();
	CGXAbstractDelayedRangePoolImp* pDelayPool = (CGXAbstractDelayedRangePoolImp*) pApp->m_pDelayedRangePoolImp;
	CGXAbstractSpanCellPoolImp* pSpanPool = (CGXAbstractSpanCellPoolImp*) pApp->m_pSpanCellPoolImp;

	if (pSpanPool && pParam->m_pPrintFloatCells == NULL)
		pParam->m_pPrintFloatCells = pSpanPool->CreateSpanCellPool();

	if (pDelayPool && pParam->m_pDelayedPrintFloatCells == NULL)
		pParam->m_pDelayedPrintFloatCells = pDelayPool->CreateDelayedRangePool();

	pGrid->DelayFloatCells(CGXRange().SetTable());

	TRY
	{
		// Free all cached fonts, pens, brushes and styles
		pGrid->FreeObjectCache();

		// select default font into device context
		pOldFont = pGrid->LoadFont(pDC, pGrid->StandardStyle());

		// Default font metrics
		pGrid->GetFontMetrics(pDC, pGrid->m_dxPrintWidth, pGrid->m_dyPrintHeight);

		// Get rectangle to be drawn
		pInfo->m_rectDraw.SetRect(0, 0, pDC->GetDeviceCaps(HORZRES),
								  pDC->GetDeviceCaps(VERTRES));
		pDC->DPtoLP(&pInfo->m_rectDraw);
		pGrid->OnAdjustPrintRectangle(pDC, pInfo);
		// m_rectPrint gets returned by GetGridRect() while m_bPrintInProgress is TRUE
		pGrid->m_rectPrint = pInfo->m_rectDraw;
		CRect rectGrid = pGrid->GetGridRect();

		// compute page breaks

		CGXRangeList* pSelList = pParam->GetRangeList();

		BOOL bCurPageOnly = pInfo->m_pPD->PrintSelection() && pSelList->GetCount() == 0;
		BOOL bCurSelOnly = pInfo->m_pPD->PrintSelection() && pSelList->GetCount() > 0;

		if (bCurPageOnly)
		{
			ROWCOL      bottomRow,
						rightCol;

			// just use top left cell
			pGrid->m_nPrintTopRow = pGrid->m_nTopRow;
			pGrid->m_nPrintLeftCol = pGrid->m_nLeftCol;
			pGrid->m_awPageFirstCol.SetAtGrow(0, pGrid->m_nPrintLeftCol);
			pGrid->m_awPageFirstRow.SetAtGrow(0, pGrid->m_nPrintTopRow);

			// ... and calculate boottom right cell
			rightCol = pGrid->CalcClientColFromPt(rectGrid.BottomRight());
			ROWCOL nCol = (rightCol == GX_INVALID)
				? pGrid->GetColCount()
				: pGrid->GetCol(rightCol);

			bottomRow = pGrid->CalcClientRowFromPt(rectGrid.BottomRight());
			ROWCOL nRow = (bottomRow == GX_INVALID)
				? pGrid->GetRowCount()
				: pGrid->GetRow(bottomRow);

			pGrid->m_awPageFirstCol.SetAtGrow(1, nCol);
			pGrid->m_awPageFirstRow.SetAtGrow(1, nRow);
			pInfo->SetMaxPage(1);
		}
		else
		{

			CGXRange rangePrint;

			// ExpandRange is necessary if user wants to
			// print selected rows or columns
			if (bCurSelOnly)
				rangePrint = *pSelList->GetTail();
			else
				rangePrint.SetTable();

			rangePrint.ExpandRange(
					pGrid->GetFrozenRows()+1,
					pGrid->GetFrozenCols()+1,
					pGrid->GetRowCount(),
					pGrid->GetColCount());

			ROWCOL      bottomRow,
						rightCol;
			int         index;

			// I must take care of frozen rows and columns
			pGrid->m_nPrintTopRow = max(pGrid->GetFrozenRows()+1, rangePrint.top);
			pGrid->m_nPrintLeftCol = max(pGrid->GetFrozenCols()+1, rangePrint.left);
			pGrid->m_bPrintCurSelOnly = bCurSelOnly; 
			pGrid->m_awPageFirstCol.SetAtGrow(0, pGrid->m_nPrintLeftCol);
			pGrid->m_nCurrentPageColIndex = 0;
			pGrid->m_nCurrentPageRowIndex = 0;

			// compute left column for each page
			for (index = 1;
				(rightCol = pGrid->CalcClientColFromPt(rectGrid.BottomRight())) != GX_INVALID;
				index++)
			{
				ROWCOL nCol = max(pGrid->GetCol(rightCol), pGrid->m_nPrintLeftCol+1);
				if (nCurCol < nCol && nCurCol >= pGrid->m_nPrintLeftCol)
					pGrid->m_nCurrentPageColIndex = index-1;
				pGrid->m_awPageFirstCol.SetAtGrow(index, nCol);
				if (nCol > rangePrint.right)
				{
					rightCol = GX_INVALID;
					break;
				}
				pGrid->m_nPrintLeftCol = nCol;
			}
			pGrid->m_awPageFirstCol.SetAtGrow(index, (rightCol == GX_INVALID)
				? rangePrint.right+1
				: pGrid->GetCol(rightCol));

			pGrid->m_nPrintTopRow = max(pGrid->GetFrozenRows()+1, rangePrint.top);
			pGrid->m_nPrintLeftCol = max(pGrid->GetFrozenCols()+1, rangePrint.left);
			pGrid->m_awPageFirstRow.SetAtGrow(0, pGrid->m_nPrintTopRow);

			// compute top row for each page
			for (index = 1;
				(bottomRow = pGrid->CalcClientRowFromPt(rectGrid.BottomRight())) != GX_INVALID;
				index++)
			{
				ROWCOL nRow = max(pGrid->GetRow(bottomRow), pGrid->m_nPrintTopRow+1);
				if (nCurRow < nRow && nCurRow >= pGrid->m_nPrintTopRow)
					pGrid->m_nCurrentPageRowIndex = index-1;
				pGrid->m_awPageFirstRow.SetAtGrow(index, nRow);
				if (nRow > rangePrint.bottom)
				{
					bottomRow = GX_INVALID;
					break;
				}
				pGrid->m_nPrintTopRow = nRow;
			}
			pGrid->m_awPageFirstRow.SetAtGrow(index, (bottomRow == GX_INVALID)
				? rangePrint.bottom+1
				: pGrid->GetRow(bottomRow));

			// set no of pages
			pInfo->SetMaxPage((pGrid->m_awPageFirstRow.GetSize()-1)*(pGrid->m_awPageFirstCol.GetSize()-1));

			// page string gets displayed in the status line of the print preview
			CString s;
			wsprintf(s.GetBuffer(10), _T(" / %u"), pInfo->GetMaxPage());
			s.ReleaseBuffer();
			pInfo->m_strPageDesc += s;

			if (pInfo->m_bPreview)
			{
				if (pProp->GetPageOrder() == 0)
					pInfo->m_nCurPage = pGrid->m_nCurrentPageRowIndex*(pGrid->m_awPageFirstCol.GetSize()-1)
							+pGrid->m_nCurrentPageColIndex + 1;
				else
					pInfo->m_nCurPage = pGrid->m_nCurrentPageColIndex*(pGrid->m_awPageFirstRow.GetSize()-1)
							+pGrid->m_nCurrentPageRowIndex + 1;
			}
		}

		// restore old font
		if (pOldFont)
			pDC->SelectObject(pOldFont);

		// Reset printing mode
		pGrid->SetPrinting(FALSE);
	}
	CATCH_ALL(e)
	{
		// restore old font
		if (pOldFont)
			pDC->SelectObject(pOldFont);

		// Reset printing mode
		pGrid->SetPrinting(FALSE);

		THROW_LAST();
	}
	END_CATCH_ALL
}

void CGXGridPrintImp::OnGridPrint(CGXGridCore* pGrid, CDC* pDC, CPrintInfo* pInfo)
{
	CGXGridParam* pParam = pGrid->GetParam();
	CGXProperties* pProp = pParam->GetProperties();

	ROWCOL      bottomRow,
				rightCol;

	// TRACE1("CGXGridCore: OnPrint(%u)\n", pInfo->m_nCurPage);

	pGrid->m_bPrintPaintMsg = FALSE;
	pGrid->SetPrinting(TRUE);

	int nDC = 0;
	HFONT hOldFont = NULL;

	TRY
	{
		// Free all cached fonts, pens, brushes and styles
		pGrid->FreeObjectCache();

		// select default font into device context
		CFont* pOldFont = pGrid->LoadFont(pDC, pGrid->StandardStyle());

		// Get the HFONT handle because pOldFont is only a temporary CGdiObject object
		// and is only valid until the next time the application has idle time in its
		// event loop.
		hOldFont = (HFONT) pOldFont->GetSafeHandle();

		// Default font metrics
		pGrid->GetFontMetrics(pDC, pGrid->m_dxPrintWidth, pGrid->m_dyPrintHeight);

		// check page brak array
		if (pGrid->m_awPageFirstRow.GetSize() == 0
			|| pGrid->m_awPageFirstCol.GetSize() == 0)
		{
			TRACE0("Warning! You must call OnBeginPrinting and OnEndPrinting from your derived class!\n");
			ASSERT(0);
		}

		pGrid->OnAdjustPrintRectangle(pDC, pInfo);
		pGrid->m_rectPrint = pInfo->m_rectDraw;

		// page order
		int nPageRowIndex = 0;
		int nPageColIndex = 0;

		// Page order
		if (pProp->GetPageOrder() == 0)
		{
			nPageRowIndex = (pInfo->m_nCurPage-1)/(pGrid->m_awPageFirstCol.GetSize()-1);
			nPageColIndex = (pInfo->m_nCurPage-1)%(pGrid->m_awPageFirstCol.GetSize()-1);
		}
		else
		{
			nPageRowIndex = (pInfo->m_nCurPage-1)%(pGrid->m_awPageFirstRow.GetSize()-1);
			nPageColIndex = (pInfo->m_nCurPage-1)/(pGrid->m_awPageFirstRow.GetSize()-1);
		}

		// top left cell
		pGrid->m_nPrintTopRow = pGrid->m_awPageFirstRow[nPageRowIndex];
		pGrid->m_nPrintLeftCol = pGrid->m_awPageFirstCol[nPageColIndex];

		CRect rectGrid = pGrid->GetGridRect();

		BOOL bAgain = FALSE;

		CGXRangeList* pSelList = pParam->GetRangeList();
		BOOL bCurPageOnly = pInfo->m_pPD->PrintSelection() && pSelList->GetCount() == 0;
		BOOL bCurSelOnly = pInfo->m_pPD->PrintSelection() && pSelList->GetCount() > 0;

		if (!bCurPageOnly && !bCurSelOnly)
		{
			// All pages shall be printed ...

			//
			// Lets check if row or column count did increase. 
			pGrid->OnTestGridDimension(pGrid->m_nPrintTopRow+2*rectGrid.Height()/pGrid->GetDefaultRowHeight(), 
				pGrid->m_nPrintLeftCol+4*rectGrid.Width()/pGrid->GetDefaultColWidth());

			// Row count
			if (pGrid->m_awPageFirstRow[pGrid->m_awPageFirstRow.GetSize()-1] <= pGrid->GetRowCount())
			{
				int nPage = pGrid->m_awPageFirstRow.GetSize()-2;
				pGrid->m_nPrintTopRow = pGrid->m_awPageFirstRow[nPage];

				bottomRow = pGrid->GetClientRow(pGrid->GetRowCount());

				// compute top rows for next pages
				BOOL bEOF = FALSE;
				while (!bEOF)
				{
					bottomRow = pGrid->CalcBottomRowFromRect(rectGrid, bEOF);
					pGrid->m_awPageFirstRow.SetAtGrow(++nPage, bottomRow + bEOF);
					pGrid->m_nPrintTopRow = bottomRow+1;
				}

				bAgain = TRUE;  // check Page order again
			}

			// Column count
			if (pGrid->m_awPageFirstCol[pGrid->m_awPageFirstCol.GetSize()-1] <= pGrid->GetColCount())
			{
				int nPage = pGrid->m_awPageFirstCol.GetSize()-2;
				pGrid->m_nPrintLeftCol = pGrid->m_awPageFirstCol[nPage];

				rightCol = pGrid->GetClientCol(pGrid->GetColCount());

				// compute left cols for next pages
				BOOL bEOF = FALSE;
				while (!bEOF)
				{
					rightCol = pGrid->CalcRightColFromRect(rectGrid, bEOF);
					pGrid->m_awPageFirstCol.SetAtGrow(++nPage, rightCol + bEOF);
					pGrid->m_nPrintLeftCol = rightCol+1;
				}

				bAgain = TRUE;  // check Page order again
			}
		}

		if (bAgain)
		{
			pInfo->SetMaxPage((pGrid->m_awPageFirstRow.GetSize()-1)*(pGrid->m_awPageFirstCol.GetSize()-1));

			// Page order
			if (pProp->GetPageOrder() == 0)
			{
				nPageRowIndex = (pInfo->m_nCurPage-1)/(pGrid->m_awPageFirstCol.GetSize()-1);
				nPageColIndex = (pInfo->m_nCurPage-1)%(pGrid->m_awPageFirstCol.GetSize()-1);
			}
			else
			{
				nPageRowIndex = (pInfo->m_nCurPage-1)%(pGrid->m_awPageFirstRow.GetSize()-1);
				nPageColIndex = (pInfo->m_nCurPage-1)/(pGrid->m_awPageFirstRow.GetSize()-1);
			}

			// top left cell
			pGrid->m_nPrintTopRow = pGrid->m_awPageFirstRow[nPageRowIndex];
			pGrid->m_nPrintLeftCol = pGrid->m_awPageFirstCol[nPageColIndex];
		}

		// ... and bottom right cell
		if (nPageRowIndex+1 < pGrid->m_awPageFirstRow.GetSize())
			bottomRow = pGrid->GetClientRow(min(pGrid->GetRowCount()+1, pGrid->m_awPageFirstRow[nPageRowIndex+1]))-1;
		else
			bottomRow = pGrid->GetClientRow(pGrid->GetRowCount());

		if (nPageColIndex+1 < pGrid->m_awPageFirstCol.GetSize())
			rightCol = pGrid->GetClientCol(min(pGrid->GetColCount()+1, pGrid->m_awPageFirstCol[nPageColIndex+1]))-1;
		else
			rightCol = pGrid->GetClientCol(pGrid->GetColCount());

		// compute drawing rectangle
		int x = pGrid->CalcSumOfClientColWidths(0, rightCol);
		int y = pGrid->CalcSumOfClientRowHeights(0, bottomRow);

		// ... and center it
		if (pProp->GetCenterHorizontal())
			pGrid->m_rectPrint.left += (pGrid->m_rectPrint.Width()-x)/2;

		if (pProp->GetCenterVertical())
			pGrid->m_rectPrint.top += (pGrid->m_rectPrint.Height()-y)/2;

		pGrid->m_rectPrint.right = pGrid->m_rectPrint.left + x;
		pGrid->m_rectPrint.bottom = pGrid->m_rectPrint.top + y;

		// print header and footer
		pGrid->OnPrintHeaderAndFooter(pDC, pInfo);

		// draw border around grid
		if (pProp->GetPrintFrame())
		{
			if (pParam->GetNewGridLineMode())
				x--, y--;

			pGrid->DrawGridLine(pDC, pGrid->m_rectPrint.left-1, pGrid->m_rectPrint.top-1, x+3, 2, GX_HORZLINE, 0);
			pGrid->DrawGridLine(pDC, pGrid->m_rectPrint.left-1, pGrid->m_rectPrint.top+y, x+3, 2, GX_HORZLINE, pGrid->GetRow(bottomRow)+1);
			pGrid->DrawGridLine(pDC, pGrid->m_rectPrint.left-1, pGrid->m_rectPrint.top-1, 2, y+3, GX_VERTLINE, 0);
			pGrid->DrawGridLine(pDC, pGrid->m_rectPrint.left+x, pGrid->m_rectPrint.top-1, 2, y+3, GX_VERTLINE, pGrid->GetCol(rightCol)+1);
		}

		// clipping
		nDC = pDC->SaveDC();
		pDC->IntersectClipRect(pGrid->m_rectPrint);

		// draw page
		pGrid->OnDrawTopLeftBottomRight(0, 0, bottomRow, rightCol, pDC);

		if (nDC)
			pDC->RestoreDC(nDC);
		nDC = 0;

		// check if there are pages to go
		pInfo->m_bContinuePrinting =
			bottomRow != pGrid->GetClientRow(pGrid->GetRowCount()) ||
			rightCol != pGrid->GetClientCol(pGrid->GetColCount());

		// restore old font
		if (hOldFont)
			pDC->SelectObject(CFont::FromHandle(hOldFont));

		// Reset printing mode
		pGrid->SetPrinting(FALSE);
	}
	CATCH_ALL(e)
	{
		if (nDC)
			pDC->RestoreDC(nDC);

		// restore old font
		if (hOldFont)
			pDC->SelectObject(CFont::FromHandle(hOldFont));

		// Reset printing mode
		pGrid->SetPrinting(FALSE);

		THROW_LAST();
	}
	END_CATCH_ALL

	if (pGrid->m_bPrintPaintMsg)
		pGrid->Redraw(GX_UPDATENOW, FALSE);
}

void CGXGridPrintImp::OnGridEndPrinting(CGXGridCore* pGrid, CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	CGXGridParam* pParam = pGrid->GetParam();

	pGrid->m_awPageFirstRow.RemoveAll();
	pGrid->m_awPageFirstCol.RemoveAll();
	pGrid->m_pGridWnd->Invalidate();

	delete pParam->m_pPrintFloatCells;
	delete pParam->m_pDelayedPrintFloatCells;
	pParam->m_pPrintFloatCells = NULL;
	pParam->m_pDelayedPrintFloatCells = NULL;
}



// ============ Header&Footer Printing =======================

void CGXGridPrintImp::OnPrintHeaderAndFooter(CGXProperties* pProp, CDC* pDC, CPrintInfo* pInfo, CDocument* pDoc, CGXGridParam* pParam)
{
	// Tokens
	TCHAR sz[128];
	CString s;

	s.Empty();
	if (pDoc)
		s = pDoc->GetTitle();
	pProp->m_sSubstTokenArray.SetAtGrow(GTK_DOCUMENT, s);
	pProp->m_sSubstTokenArray.SetAtGrow(GTK_APP, AfxGetApp()->m_pszAppName);
	wsprintf(sz, _T("%d"), pInfo->GetMaxPage());
	pProp->m_sSubstTokenArray.SetAtGrow(GTK_NOOFPAGE, sz);

	if (pProp->GetFirstPage() == -1)
		wsprintf(sz, _T("%d"), (int) pInfo->m_nCurPage);
	else
		wsprintf(sz, _T("%d"), (int) pInfo->m_nCurPage-pInfo->GetMinPage()+pProp->GetFirstPage());
	pProp->m_sSubstTokenArray.SetAtGrow(GTK_PAGE, sz);

	// register name

	s.Empty();
	if (pParam)
		s = pParam->GetSheetName();
	pProp->m_sSubstTokenArray.SetAtGrow(GTK_REGISTER, s);

	// distances
	int top, bottom;
	pProp->GetDistances(top, bottom);

	// Header
	CRect rectHeader(0, 0, pDC->GetDeviceCaps(HORZRES),
							  pDC->GetDeviceCaps(VERTRES));
	pDC->DPtoLP(&rectHeader);

	// m_rectDraw is adjusted print-rectangle (top, bottom, left, right Frames subtracted)
	if (pInfo)
	{
		rectHeader.top = top;
		rectHeader.bottom = pInfo->m_rectDraw.top;
		rectHeader.left = pInfo->m_rectDraw.left;
		rectHeader.right = pInfo->m_rectDraw.right;
	}

	CGXStylesMap* pStylesMap = pParam->GetStylesMap();
	pProp->PrintHeaderFooter(pDC, pInfo, rectHeader, pProp->m_mapDataHeader, pStylesMap);

	// Footer
	CRect rectFooter(0, 0, pDC->GetDeviceCaps(HORZRES),
							  pDC->GetDeviceCaps(VERTRES));

	pDC->DPtoLP(&rectFooter);

	if (pInfo)
	{
		rectFooter.bottom -= bottom;
		rectFooter.top = max(pInfo->m_rectDraw.bottom, rectFooter.bottom-pProp->CalculateHeight(pDC, pProp->m_mapDataFooter, pStylesMap));
		rectFooter.left = pInfo->m_rectDraw.left;
		rectFooter.right = pInfo->m_rectDraw.right;
	}

	pProp->PrintHeaderFooter(pDC, pInfo, rectFooter, pProp->m_mapDataFooter, pStylesMap);
}


int CGXGridPrintImp::CalculateHeight(CGXProperties* pProp, CDC* pDC, CGXData &data, CGXStylesMap* pStylesMap)
{
	GX_UNUSED_ALWAYS(pProp);

	int nMaxHeight = 0;

	// Calculate height of all columns with used fonts
	for (ROWCOL col = 1; col <= 3; col++)
	{
		int nHeight = 0, nLastHeight = 0;
		for (ROWCOL row = 1; row <= NROWS; row++)
		{
			// Compose style
			CGXStyle style;
			data.GetStyleRowCol(row, col, style, gxCopy);

			if (style.GetIncludeValue())
			{
				// Initalize Font
				HFONT hFont = 0;
				CFont* pOldFont = 0;
				CFont* pFont = 0;

				// merge default style
				CGXStyle* pDefaultStyle;
				if (!pStylesMap->LookupStyle(pStylesMap->m_wStyleStandard, pDefaultStyle))
				{
					ASSERT(0);
					continue;
				}
				else
					style.ChangeStyle(*pDefaultStyle, gxApplyNew);

				// compare with default font

				if (!style.GetFontRef().IsEqual(pDefaultStyle->GetFontRef()))
				{
					// create font object
					// TRACE(_T("SelectObject(Font %s)\n"), (LPCTSTR) style.GetFontRef().GetDescription());
					hFont = ::CreateFontIndirect(&style.GetFontRef().GetLogFontRef());
					if (hFont == 0)
					{
						TRACE1("Failed to create font '%s' in CGXGridPrintImp::CalculateHeight\n",
							(LPCTSTR) style.GetFontRef().GetDescription());
					}
					else
					{
						// ... and select into dc
						pOldFont = pDC->SelectObject(pFont = CFont::FromHandle(hFont));
						if (pOldFont == 0)
						{
							TRACE1("Failed to select font '%s' into dc in CGXGridPrintImp::CalculateHeight\n",
								(LPCTSTR) style.GetFontRef().GetDescription());
						}
					}
				}

				// calculate height
				CSize size;
				size = pDC->GetTextExtent(_T("X"), 1);
				nHeight += (int) size.cy;
				nLastHeight = nHeight;

				// clean up
				if (pOldFont)
					pDC->SelectObject(pOldFont);

				if (pFont)
					pFont->DeleteObject();
			}

		}
		nMaxHeight = max(nLastHeight, nMaxHeight);
	}
	return nMaxHeight;
}

void CGXGridPrintImp::PrintHeaderFooter(CGXProperties* pProp, CDC* pDC, CPrintInfo* pInfo, CRect& rect, CGXData& data, CGXStylesMap* pStylesMap)
{
	GX_UNUSED_ALWAYS(pInfo);

	for (ROWCOL col = 1; col <= 3; col++)
	{
		UINT uFormat
			= (col == 1) ? DT_LEFT
			: (col == 2) ? DT_CENTER
			: DT_RIGHT;

		CRect r = rect;

		// a small margin
		r.left += 8;
		r.right -= 8;

		for (ROWCOL row = 1; row <= NROWS; row++)
		{
			// Compose style
			CGXStyle style;
			data.GetStyleRowCol(row, col, style, gxCopy);

			if (style.GetIncludeValue())
			{
				// Initalize Font
				HFONT hFont = 0;
				CFont* pOldFont = 0;
				CFont* pFont = 0;

				// merge default style
				CGXStyle *pDefaultStyle;
				if (!pStylesMap->LookupStyle(pStylesMap->m_wStyleStandard, pDefaultStyle))
				{
					ASSERT(0);
					continue;
				}
				else
					style.ChangeStyle(*pDefaultStyle, gxApplyNew);

				// compare with default font
				if (!style.GetFontRef().IsEqual(pDefaultStyle->GetFontRef()))
				{
					// create font object
					// TRACE(_T("SelectObject(Font %s)\n"), (LPCTSTR) style.GetFontRef().GetDescription());
					hFont = ::CreateFontIndirect(&style.GetFontRef().GetLogFontRef());
					if (hFont == 0)
					{
						TRACE1("Failed to create font '%s' in CGXGridPrintImp::PrintHeaderFooter\n",
							(LPCTSTR) style.GetFontRef().GetDescription());
					}
					else
					{
						// ... and select into dc
						pOldFont = pDC->SelectObject(pFont = CFont::FromHandle(hFont));
						if (pOldFont == 0)
						{
							TRACE1("Failed to select font '%s' into dc in CGXGridPrintImp::PrintHeaderFooter\n",
								(LPCTSTR) style.GetFontRef().GetDescription());
						}
					}
				}

				// Draw the cell
				CSize size;
				size = pDC->GetTextExtent(_T("X"), 1);

				CString s;
				if (style.GetIncludeValue())
				{
					s = pProp->ParseText(style.GetValueRef());
					GXDrawTextLikeMultiLineEdit(pDC, s, s.GetLength(), r, uFormat | DT_NOCLIP | DT_NOPREFIX);
				}

				r.top += size.cy;

				// clean up
				if (pOldFont)
					pDC->SelectObject(pOldFont);

				if (pFont)
					pFont->DeleteObject();
			}
		}
	}
}

CString CGXGridPrintImp::ParseText(CGXProperties* pProp, LPCTSTR sz)
{
	CString s = sz;
	CString t;
	while (s.GetLength() > 0)
	{
		BOOL bFound = FALSE;
		for (int token = 0; !bFound && token < pProp->m_sTokenArray.GetSize(); token++)
		{
			if (pProp->m_sTokenArray[token].CompareNoCase(s.Left(pProp->m_sTokenArray[token].GetLength())) == 0)
			{
				s = s.Mid(pProp->m_sTokenArray[token].GetLength());
				t += pProp->SubstTokenText(token, s);
				bFound = TRUE;
				break;
			}
		}
		if (!bFound)
		{
			t += s[0];
			s = s.Mid(1);
		}
	}
	return t;
}

CString CGXGridPrintImp::SubstTokenText(CGXProperties* pProp, int nToken, CString& sRest)
{
	CString s;
	if (nToken == GTK_DATE)
	{
		CString args = pProp->GetTokenArgs(sRest);
		if (args.IsEmpty())
			args = _T("%c");

		TCHAR szBuffer[255];

		// GetCurrentTime
		time_t ti = time(NULL);
		struct tm* ptmTemp = localtime(&ti);
		ASSERT(ptmTemp != NULL); // make sure the time has been initialized!

#if _MFC_VER < 0x0300
		if (!GXIntlStrFtime(AfxGetResourceHandle(), GX_IDS_TIME_ADAY1,
				szBuffer, sizeof(szBuffer)/sizeof(TCHAR), args, ptmTemp))
#else
		if (!_tcsftime(szBuffer, sizeof(szBuffer)/sizeof(TCHAR), args, ptmTemp))
#endif
			szBuffer[0] = _T('\0');
		return szBuffer;
	}
	else if (nToken < pProp->m_sSubstTokenArray.GetSize())
		return pProp->m_sSubstTokenArray[nToken];
	return s;
}

CString CGXGridPrintImp::GetTokenArgs(CGXProperties* pProp, CString& sRest)
{
	GX_UNUSED_ALWAYS(pProp);

	CString s;
	if (!sRest.IsEmpty() && sRest[0] == _T('{'))
	{
		int i = sRest.Find(_T('}'));
		if (i == -1)
			return s;   // Empty()
		s = sRest.Mid(1, i-1);
		sRest = sRest.Mid(i+1);
	}
	return s;
}

