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

// gxmarked.cpp: Outline row and column header for current cell
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
#pragma code_seg("GX_SEG_GXCORDRA")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

class CGXGridMarkEditHeaderImp: public CGXAbstractGridMarkEditHeaderImp
{
public:
	virtual void MarkEdit(CGXGridCore* pGrid, ROWCOL nEditRow, ROWCOL nEditCol, UINT direction, UINT flags);
	virtual BOOL GetMarkHeaderState(CGXControl* pControl, ROWCOL nRow, ROWCOL nCol, const CGXStyle& style);
};

void CGXGridCore::ImplementMarkEditHeader()
{
	if (m_pMarkEditHeaderImp == NULL)
		AutoDeletePtr(m_pMarkEditHeaderImp = new CGXGridMarkEditHeaderImp);
}

void CGXGridMarkEditHeaderImp::MarkEdit(CGXGridCore* pGrid, ROWCOL nEditRow, ROWCOL nEditCol, UINT direction, UINT flags)
{
	// Invalidate the row and column header
	if (pGrid->IsLockUpdate())
		return;

	flags &= (GX_UPDATENOW | GX_SMART | GX_INVALIDATE);

	// TRACE2("CGXGridCore: BEGIN MarkEdit(%d, %d)\n", bShow, direction);

	ROWCOL bottomRow = pGrid->GetLastVisibleRow();
	ROWCOL rightCol = pGrid->GetLastVisibleCol();

	BOOL bVert = (!direction || direction == GX_VERT);
	BOOL bHorz = (!direction || direction == GX_HORZ);

	// check, if row or column is visible
	if ((pGrid->IsFrozenRow(nEditRow) || nEditRow >= pGrid->GetTopRow()) && nEditRow <= pGrid->GetRowCount()
		|| (pGrid->IsFrozenCol(nEditCol) || nEditCol >= pGrid->GetLeftCol()) && nEditCol <= pGrid->GetColCount())
	{
		// Bottom-Right cell
		if (nEditRow <= bottomRow || nEditCol <= rightCol)
		{
			// Row Header
			if (bVert && pGrid->m_pParam->m_pProperties->GetMarkRowHeader()
				&& (pGrid->IsFrozenRow(nEditRow) || nEditRow >= pGrid->GetTopRow())
				&& nEditRow <= bottomRow)
			{
				CRect rectItem = pGrid->CalcRectFromRowColExEx(nEditRow, 0, nEditRow, pGrid->GetHeaderCols());

				ASSERT(rectItem.right >= rectItem.left && rectItem.bottom >= rectItem.top);
				if (rectItem.Width() > 1 && rectItem.Height() > 1)
				{
					pGrid->m_bLockGDIObjects = TRUE;   // avoid emptying GDI-objects-cache
					pGrid->InvalidateRect(rectItem);
					if (flags & GX_UPDATENOW || flags & GX_SMART)
						pGrid->UpdateWindow();
					pGrid->m_bLockGDIObjects = FALSE;
				}
			}

			// Column Header
			if (bHorz && pGrid->m_pParam->m_pProperties->GetMarkColHeader()
				&& (pGrid->IsFrozenCol(nEditCol) || nEditCol >= pGrid->GetLeftCol())
				&& nEditCol <= rightCol)
			{
				CRect rectItem = pGrid->CalcRectFromRowColExEx(0, nEditCol, pGrid->GetHeaderRows(), nEditCol);

				ASSERT(rectItem.right >= rectItem.left && rectItem.bottom >= rectItem.top);
				if (rectItem.Width() > 1 && rectItem.Height() > 1)
				{
					pGrid->m_bLockGDIObjects = TRUE;   // avoid emptying GDI-objects-cache
					pGrid->InvalidateRect(rectItem);
					if (flags & GX_UPDATENOW || flags & GX_SMART)
						pGrid->UpdateWindow();
					pGrid->m_bLockGDIObjects = FALSE;
				}
			}
		}

	}

	// TRACE2("CGXGridCore: END   MarkEdit(%d, %d)\n", bShow, direction);
}

BOOL CGXGridMarkEditHeaderImp::GetMarkHeaderState(CGXControl* pControl, ROWCOL nRow, ROWCOL nCol, const CGXStyle& style)
{
	// Unused:
	style;

	CGXGridCore* pGrid = pControl->Grid();
	BOOL bPressed = FALSE;
	ROWCOL ncRow, ncCol;
	BOOL bCurrent = pGrid->GetCurrentCell(ncRow, ncCol);

	CGXRange r(nRow, nCol);
	pGrid->GetCellRangeRowCol(nRow, nCol, r);

	if (ncRow <= pGrid->GetHeaderRows() || ncCol <= pGrid->GetHeaderCols())
		// headers is active Edit Cell
		//
		; // bPressed = pGrid->IsCurrentCell(nRow, nCol);
	else
	{
		// Row or Column of active Edit Cell
		bPressed |= nRow > 0
			&& !pGrid->GetParam()->IsSpecialMode(GX_MODELISTBOX)
			&& bCurrent && ncRow >= r.top && ncRow <= r.bottom
			&& pGrid->GetParam()->GetProperties()->GetMarkRowHeader();

		bPressed |= nCol > 0
			&& bCurrent && ncCol >= r.left && ncCol <= r.right
			&& pGrid->GetParam()->GetProperties()->GetMarkColHeader();
	}

	// currently Tracking size of this Row or Col ==> draw a pressed Button
	int nTrackingMode;
	ROWCOL nTracking;
	BOOL b = pGrid->IsTracking(&nTrackingMode, &nTracking);
	bPressed |= b &&
		(nTrackingMode & GX_TRACKWIDTH
			&& !pGrid->GetParam()->IsEnableTrackColWidth(GX_TRACK_NOPRESSEDHEADER)
			&& r.left <= nTracking
			&& r.right >= nTracking
		|| nTrackingMode & GX_TRACKHEIGHT
			&& !pGrid->GetParam()->IsEnableTrackRowHeight(GX_TRACK_NOPRESSEDHEADER)
			&& r.top <= nTracking
			&& r.bottom >= nTracking);

	return bPressed;
}
