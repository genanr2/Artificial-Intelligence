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

// gxcvrdra.cpp: Drawing for covered (merge and floating) cells
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

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCVRDRA")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

class CGXDrawSpannedCellsImp: public CGXAbstractDrawSpannedCellsImp
{
public:
	virtual BOOL OnDrawTLBR_LoadCell(CGXGridCore* pGrid, void* pds, unsigned nRow, unsigned nCol);
	virtual BOOL OnDrawTLBR_DrawCell(CGXGridCore* pGrid, void* pds, unsigned nRow, unsigned nCol);
	virtual BOOL EvalVisibleFloatMergeCells(CGXGridCore* pGrid, const CGXRange& range, ROWCOL nfr, ROWCOL nfc, ROWCOL nTopRow, ROWCOL nLefCol, ROWCOL nLastRow, ROWCOL nLastCol, BOOL bUpdate, CGXRange* prgBoundary, BOOL bLookupStyle);
};

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Special grid drawing for covered, floating and merge cells

// This code will be linked in when user calls 
// EnableCoveredCells,
// EnableFloatCells
// or EnableMergeCells
//
// with the following code snippet:

void CGXGridCore::ImplementDrawSpannedCells()
{
	if (m_pDrawSpannedCellsImp == NULL)
	 	AutoDeletePtr(m_pDrawSpannedCellsImp = new CGXDrawSpannedCellsImp);
}

BOOL CGXDrawSpannedCellsImp::EvalVisibleFloatMergeCells(
	CGXGridCore* pGrid,
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
	CGXRange range = rg;

	if (!range.IsCells())
		range.ExpandRange(0, 0, pGrid->GetRowCount(), pGrid->GetColCount());

	BOOL bFrozenTL, bFrozenTop, bFrozenLeft, bCells;
	CGXRange rgFrozenTL, rgFrozenTop, rgFrozenLeft, rgCells;

	bFrozenTL = rgFrozenTL.IntersectRange(CGXRange(0, 0, nfr, nfc), range);
	bFrozenTop = rgFrozenTop.IntersectRange(CGXRange(0, nLeftCol, nfr, nLastCol), range);
	bFrozenLeft = rgFrozenLeft.IntersectRange(CGXRange(nTopRow, 0, nLastRow, nfc), range);
	bCells = rgCells.IntersectRange(CGXRange(nTopRow, nLeftCol, nLastRow, nLastCol), range);

	BOOL bEval = FALSE;

	if (bFrozenTL)
		bEval |= pGrid->EvalMergeCells(rgFrozenTL, bUpdate, prgBoundary, bLookupStyle);

	if (bFrozenTop)
		bEval |= pGrid->EvalMergeCells(rgFrozenTop, bUpdate, prgBoundary, bLookupStyle);

	if (bFrozenLeft)
		bEval |= pGrid->EvalMergeCells(rgFrozenLeft, bUpdate, prgBoundary, bLookupStyle);

	if (bCells)
		bEval |= pGrid->EvalMergeCells(rgCells, bUpdate, prgBoundary, bLookupStyle);

	if (bFrozenTL)
		bEval |= pGrid->EvalFloatCells(rgFrozenTL, bUpdate, prgBoundary, bLookupStyle);

	if (bFrozenTop)
		bEval |= pGrid->EvalFloatCells(rgFrozenTop, bUpdate, prgBoundary, bLookupStyle);

	if (bFrozenLeft)
		bEval |= pGrid->EvalFloatCells(rgFrozenLeft, bUpdate, prgBoundary, bLookupStyle);

	if (bCells)
		bEval |= pGrid->EvalFloatCells(rgCells, bUpdate, prgBoundary, bLookupStyle);

	return bEval;
}

BOOL CGXDrawSpannedCellsImp::OnDrawTLBR_LoadCell(CGXGridCore* pGrid, void* pds, unsigned nRow, unsigned nCol)
{
	CGXGridCore::DrawStruct& ds = *((CGXGridCore::DrawStruct*) pds);

	int i = nRow*ds.nCols+nCol;
	CGXRange rgCovered;

	// Grid line settings
	BOOL bNewMode = pGrid->GetParam()->GetNewGridLineMode();
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

	// Covered cells
	BOOL bCovered = pGrid->GetCoveredCellsRowCol(ds.anRows[nRow], ds.anCols[nCol], rgCovered) != NULL;
	BOOL bSpanned = bCovered;

	//
	// Check if cell is a floating or merge cell and make sure it
	// is not an active cell in the floating or merge cell.
	//
	// We have to check for active current cell because with 
	// floating or merge cells, the user should still be able
	// to edit the hidden cells.
	//
	if (!bCovered)
	{
		// ds.pCurrentControl has been set in OnDrawTopLeftBottomRight
		BOOL bActive = pGrid->IsCurrentCell(ds.anRows[nRow], ds.anCols[nCol]) && ds.pCurrentControl && ds.pCurrentControl->IsActive();

		// Horizontal merged cells
		BOOL bHorzMerge = bSpanned || (pGrid->GetMergedCellsRowCol(ds.anRows[nRow], ds.anCols[nCol], rgCovered, CGXGridCore::mtHorz)
				// make sure this is the merging cell (all other cells will not be drawn)
				&& (ds.anCols[nCol] == rgCovered.left 
					// only the current cell will be drawn when active
					|| !bActive)
					);
		bSpanned |= bHorzMerge;

		// Vertically merged cells
		BOOL bVertMerge = bSpanned || (pGrid->GetMergedCellsRowCol(ds.anRows[nRow], ds.anCols[nCol], rgCovered, CGXGridCore::mtVert)
				&& (ds.anRows[nRow] == rgCovered.top 
					|| !bActive)
					);
		bSpanned |= bVertMerge;

		// Floated cells
		BOOL bFloated = bSpanned || (pGrid->GetFloatedCellsRowCol(ds.anRows[nRow], ds.anCols[nCol], rgCovered)
				&& (ds.anCols[nCol] == rgCovered.left 
					|| !bActive)
					);
		bSpanned |= bFloated;
	}

	// If we found a spanned cell, initialize settings
	// for all those cells which are hidden by this spanned cell

	// - For each hidden cell, the style pointer from the spanned
	//   cell will be copied - each hidden cell will have the same
	//   style pointer.

	// - The first cell will be marked with gxnFlagSpanned. All 
	//   subsequent hidden cells will be marked with gxnFlagSpannedCont.
	//   gxnFlagSpannedCont will be checked in DrawCell - if it is set
	//   the cell must not be drawn
	//
	//   An exception for Merge and float cells is: If an active current cell 
	//   is in the range of hidden cell this cell will be ignored (and
	//   not marked with gxnFlagSpannedCont flag).
	// 
	// - The border setting will be initialized for all hidden cells.
	//   This helps the DrawBorders methods too find out if a grid line
	//   should be drawn above or below the cell or if a normal
	//   border shall be drawn.


	if (bSpanned)
	{
		if (ds.apStyles[i] == NULL)
			ds.apStyles[i] = &pGrid->LookupStyleRowCol(rgCovered.top, rgCovered.left);

		// Block: mark all cells of covered range
		{
			unsigned row1, col1, row2, col2;

			ROWCOL rgRight = rgCovered.right;
			ROWCOL rgBottom = rgCovered.bottom;
			if (rgBottom < ds.gridTopRow && rgBottom > ds.gridFrozenRows)
				rgBottom = ds.gridFrozenRows;

			if (rgRight < ds.gridLeftCol && rgRight > ds.gridFrozenCols)
				rgRight = ds.gridFrozenCols;

			row2 = min((unsigned) (pGrid->GetClientRow(rgBottom)-ds.topRow), ds.nRows-1),
			col2 = min((unsigned) (pGrid->GetClientCol(rgRight)-ds.leftCol), ds.nCols-1);

			ds.abBits[nRow*ds.nCols+nCol] = gxnFlagSpanned;
			int n1 = 0;
			for (row1 = nRow; row1 <= row2; row1++)
			{
				for (col1 = nCol; col1 <= col2; col1++)
				{
					// Covered or hidden cell (active cells will 
					// be ignored for floating and merge cell)
					if (bCovered)
					{
						unsigned i2 = row1*ds.nCols+col1;
						ds.apStyles[i2] = ds.apStyles[i];
						ds.abBits[i2] = (WORD) (gxnFlagSpanned | gxnFlagCovered | n1*gxnFlagSpannedCont);
						n1 = 1;
					}
					else if ( !(pGrid->IsCurrentCell(ds.anRows[row1], ds.anCols[col1]) 
							&& ds.pCurrentControl 
							&& ds.pCurrentControl->IsActive()))
					{
						unsigned i2 = row1*ds.nCols+col1;
						ds.apStyles[i2] = ds.apStyles[i];
						ds.abBits[i2] = (WORD) (gxnFlagSpanned | n1*gxnFlagSpannedCont);
						n1 = 1;
					}
				}
			}

			// determine for which cells the grid line
			// shall be drawn 

			if (bNewMode)
			{
				if (ds.anCols[col2] == rgCovered.right)
				{
					for (row1 = nRow; row1 <= row2; row1++)
					{
						int i2 = row1*ds.nCols+col2;

						if (ds.apStyles[i2])
						{
							if (ds.apStyles[i2]->GetIncludeBorders() ) 
							{
								// Left (or right)
								ds.abBits[i2] |= nVertFlag * ds.apStyles[i2]->GetIncludeBorders(tVertLine);
							}

							if ((ds.abBits[i2] & nVertFlag) == 0)
								ds.abBits[i2] |= gxnFlagVertLine;
						}
					}
				}

				if (ds.anRows[row2] == rgCovered.bottom)
				{
					for (col1 = nCol; col1 <= col2; col1++)
					{
						int i2 = row2*ds.nCols+col1;

						if (ds.apStyles[i2])
						{
							if (ds.apStyles[i2]->GetIncludeBorders() )
							{
								// Top
								ds.abBits[i2] |= nHorzFlag * ds.apStyles[i2]->GetIncludeBorders(tHorzLine);
							}

							if ((ds.abBits[i2] & nHorzFlag) == 0)
								ds.abBits[i2] |= gxnFlagHorzLine;
						}
					}
					
				}	
			}
			else
			{
				if (ds.anCols[nCol] == rgCovered.left)
				{
					for (row1 = nRow; row1 <= row2; row1++)
					{
						int i2 = row1*ds.nCols+nCol;

						if (ds.apStyles[i2])
						{
							if (ds.apStyles[i2]->GetIncludeBorders() ) 
							{
								// Left (or right)
								ds.abBits[i2] |= nVertFlag * ds.apStyles[i2]->GetIncludeBorders(tVertLine);
							}

							if ((ds.abBits[i2] & nVertFlag) == 0)
								ds.abBits[i2] |= gxnFlagVertLine;
						}
					}
				}

				if (ds.anRows[nRow] == rgCovered.top)
				{
					for (col1 = nCol; col1 <= col2; col1++)
					{
						int i2 = nRow*ds.nCols+col1;

						if (ds.apStyles[i2])
						{
							if (ds.apStyles[i2]->GetIncludeBorders() )
							{
								// Top
								ds.abBits[i2] |= nHorzFlag * ds.apStyles[i2]->GetIncludeBorders(tHorzLine);
							}

							if ((ds.abBits[i2] & nHorzFlag) == 0)
								ds.abBits[i2] |= gxnFlagHorzLine;
						}
					}
					
				}	
			}

			// Fixed column
			if (ds.anCols[col2] > ds.gridHeaderCols && ds.anCols[col2] == ds.gridFrozenCols
				&& ds.anCols[col2] == rgCovered.right)
			{
				for (row1 = nRow; row1 <= row2; row1++)
				{
					int i2 = row1*ds.nCols+col2;

					if (ds.apStyles[i2] && !ds.apStyles[i2]->GetIncludeBorders(gxBorderRight) ) 
						ds.abBits[i2] |= gxnFlagFixedVertLine;
				}
			}

			// Fixed row
			if (ds.anRows[row2] > ds.gridHeaderRows && ds.anRows[row2] == ds.gridFrozenRows
				&& ds.anRows[row2] == rgCovered.bottom)
			{
				for (col1 = nCol; col1 <= col2; col1++)
				{
					int i2 = row2*ds.nCols+col1;

					if (ds.apStyles[i2] && !ds.apStyles[i2]->GetIncludeBorders(gxBorderBottom) ) 
						ds.abBits[i2] |= gxnFlagFixedHorzLine;
				}
			}
		}

		ds.bAnyCellCovered = TRUE;
		if (ds.bColor)
			ds.abBits[i] |= gxnFlagColor;

		return TRUE;
	}

	return FALSE;
}

BOOL CGXDrawSpannedCellsImp::OnDrawTLBR_DrawCell(CGXGridCore* pGrid, void* pds, unsigned nRow, unsigned nCol)
{
	CGXGridCore::DrawStruct& ds = *((CGXGridCore::DrawStruct*) pds);
	CGXRange rgCovered;

	unsigned i;

	// If this is a cell which is hidden by a merge or floating cell
	// and if it is the current cell we have to outline the frame
	// around the cell

	if (ds.apStyles[i = nRow*ds.nCols+nCol] && (ds.abBits[i]&gxnFlagSpannedCont) != 0)
	{
		// Get control only for inverting its borders when a cell
		// is hidden by floated or merged cell

		if ((ds.abBits[i]&gxnFlagCovered) == 0 && 
			ds.rectItem.Width() > 0 && ds.rectItem.Height() > 0)
		{
			CGXControl* pControl = NULL;

			if (pGrid->IsCurrentCell(ds.anRows[nRow], ds.anCols[nCol]))
			{
				pControl = pGrid->GetRegisteredControl(ds.apStyles[i]->GetControl());
		
				// Borders will be drawn later
				if (pControl && !pControl->IsActive())
				{
					if (pControl->IsInit(ds.anRows[nRow], ds.anCols[nCol]))
						ds.pCurrentControl = pControl;
					ds.rectEdit = ds.rectItem;
					ds.rectEdit = pGrid->SubtractBorders(ds.rectEdit, *ds.apStyles[i]);
				}
			}
		}

		return TRUE;
	}

	// This is either a spanned cell or normal cell
	else if (ds.apStyles[i])
	{
		if ((ds.abBits[i]&gxnFlagSpanned) != 0)
		{
			CGXRange* pr = pGrid->GetCellRangeRowCol(ds.anRows[nRow], ds.anCols[nCol], rgCovered);
			ASSERT(pr != NULL);

			CGXRange r[4];

			// I have to deal with frozen rows and columns:
			//    +--|-----------+
			//    |r0|r2         |
			//   ======================= top row
			//    |r1|r3         |
			//    |  |           |r
			//    +--|-----------+
			//       |
			//      left column

			ROWCOL nLastFixedRow = pGrid->GetRow(pGrid->GetFrozenRows());
			ROWCOL nLastFixedCol = pGrid->GetCol(pGrid->GetFrozenCols());

			r[0].top = pr->top;
			r[0].left = pr->left;
			r[0].bottom = min(nLastFixedRow, pr->bottom);
			r[0].right = min(nLastFixedCol, pr->right);

			r[1].top = max(pGrid->GetTopRow(), pr->top);
			r[1].left = pr->left;
			r[1].bottom = pr->bottom;
			r[1].right = min(nLastFixedCol, pr->right);

			r[2].top = pr->top;
			r[2].left = max(pGrid->GetLeftCol(), pr->left);
			r[2].bottom = min(nLastFixedRow, pr->bottom);
			r[2].right = pr->right;

			r[3].top = max(pGrid->GetTopRow(), pr->top);
			r[3].left = max(pGrid->GetLeftCol(), pr->left);
			r[3].bottom = pr->bottom;
			r[3].right = pr->right;

			CRect rectVisible, rect;
			CGXControl* pControl = NULL;

			const CGXStyle& style = pGrid->LookupStyleRowCol(pr->top, pr->left);

			if (pGrid->IsCurrentCell(ds.anRows[nRow], ds.anCols[nCol])
				|| (ds.abBits[i]&gxnFlagCovered) && pGrid->IsCurrentCell(pr->top, pr->left))
				pControl = pGrid->GetRegisteredControl(style.GetControl());

			// Draw each subrectangle, take care on clipping
			BOOL bInvert = FALSE;
			for (int j = 0; j <= 3; j++)
			{
				if (r[j].right < r[j].left || r[j].bottom < r[j].top
					|| r[j].bottom < ds.anRows[nRow]
					|| r[j].right < ds.anCols[nCol])
					continue;

				BOOL bOldLimitColWidth = pGrid->m_bLimitColWidth;
				BOOL bOldLimitRowHeight = pGrid->m_bLimitRowHeight;
				pGrid->m_bLimitColWidth = FALSE;
				pGrid->m_bLimitRowHeight = FALSE;

				rect = rectVisible = pGrid->CalcRectFromRowCol(r[j].top, r[j].left, r[j].bottom, r[j].right, TRUE);

				// clipping
				int nDC;
				
				nDC = 0;

				// Temporarily make sure m_bLimitColWidth is FALSE
				// so that GetRowHeight and GetColWidth do not return
				// adjusted values
				
				TRY
				{
					BOOL bCanClip = TRUE;

					// Metafiles don't support SelectClipRgn!
					if (ds.pDC->IsKindOf(RUNTIME_CLASS(CMetaFileDC)) && !ds.pDC->IsPrinting())
						bCanClip = FALSE;

					// 16-Bit device drivers don't support SelectClipRgn!
					if (pGrid->m_pAppData->bWin31 && ds.pDC->IsPrinting())
						bCanClip = FALSE;

					BOOL bClip = FALSE;

					// full rectangle (can contain negative top and left value)
					if (pr->top < r[j].top)
					{
						bClip = TRUE;
						rect.top -= pGrid->CalcSumOfRowHeights(pr->top, r[j].top-1);
					}

					if (pr->bottom > r[j].bottom)
					{
						bClip = TRUE;
						rect.bottom += pGrid->CalcSumOfRowHeights(r[j].bottom+1, pr->bottom);
					}

					if (pr->left < r[j].left)
					{
						bClip = TRUE;
						rect.left -= pGrid->CalcSumOfColWidths(pr->left, r[j].left-1);
					}

					if (pr->right > r[j].right)
					{
						bClip = TRUE;
						rect.right += pGrid->CalcSumOfColWidths(r[j].right+1, pr->right);
					}

					if (pGrid->m_bRefreshOnlyUpdatedCells)
					{
						bClip = TRUE;
					}

					pGrid->m_bDrawCoveredCell = TRUE;

					if (bClip || pGrid->m_bForceOldDrawing)
					{
						// clip area if cell is only partly visible
						CRect rectIntersect;
						nDC = ds.pDC->SaveDC();
						if (nDC != 0)
						{
							if (pGrid->m_bForceOldDrawing || pGrid->m_bRefreshOnlyUpdatedCells)
								ds.pDC->SelectClipRgn(NULL);
							rectIntersect.IntersectRect(rectVisible, ds.rectClip);
							ds.pDC->IntersectClipRect(rectIntersect);

							// draw it with full rectangle
							pGrid->OnDrawItem(ds.pDC, pr->top, pr->left, rect, style);
						}
						else // error - no clipping possible
							pGrid->OnDrawItem(ds.pDC, pr->top, pr->left, rect, style);
					}
					else
						// draw it with full rectangle
						pGrid->OnDrawItem(ds.pDC, pr->top, pr->left, rect, style);

					pGrid->m_bDrawCoveredCell = FALSE;

					// Draw Borders
					if (style.GetIncludeBorders())
						pGrid->OnDrawBorders(ds.pDC, rect, style);

					if (pControl && !pControl->IsActive() && !bInvert)
					{
						if ((ds.abBits[i]&gxnFlagCovered) == 0)
							rect = ds.rectItem;
						rect = pGrid->SubtractBorders(rect, style);

						if (!rect.IsRectEmpty())
							pControl->InvertBorders(ds.pDC, rect);

						if ((ds.abBits[i]&gxnFlagCovered) == 0)
							bInvert = TRUE;
					}

					if (nDC)
						ds.pDC->RestoreDC(nDC);

					pGrid->m_bLimitColWidth = bOldLimitColWidth;
					pGrid->m_bLimitRowHeight = bOldLimitRowHeight;

				}
				CATCH_ALL(e)
				{
					if (nDC)
						ds.pDC->RestoreDC(nDC);

					pGrid->m_bLimitColWidth = bOldLimitColWidth;
					pGrid->m_bLimitRowHeight = bOldLimitRowHeight;

					THROW_LAST();
				}
				END_CATCH_ALL
			}
				
			return TRUE;
		}
	}

	return FALSE;
}

