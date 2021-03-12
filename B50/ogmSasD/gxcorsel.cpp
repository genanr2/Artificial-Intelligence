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

// gxcorsel.cpp : cell selections (CGXGridCore)
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

#ifndef _GXPRIV_H_
#include "gxpriv.h"
#endif


#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCORSEL")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef TICKCOUNT
#define _TC_(f) f
#else
#define _TC_(f)
#endif


#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Selected Cells

CGXLockSelectionFrame::CGXLockSelectionFrame(CGXGridCore* pGrid)
{
	m_pGrid = pGrid;   
	m_bLock = m_pGrid->LockSelectionFrame(TRUE);
}

CGXLockSelectionFrame::~CGXLockSelectionFrame()
{
	m_pGrid->LockSelectionFrame(m_bLock);
}

BOOL CGXGridCore::CopyRangeList(CGXRangeList& list, BOOL bCanUseCurrentCell)
{
	CGXRangeList* pSelList = GetParam()->GetRangeList();

	list.DeleteAll();
	POSITION pos = pSelList->GetHeadPosition();
	while (pos)
		list.AddTail(new CGXRange(*pSelList->GetNext(pos)));

	if (pSelList->IsEmpty() && bCanUseCurrentCell)
	{
		ROWCOL nRow, nCol;
		if (GetCurrentCell(&nRow, &nCol))
			list.AddTail(new CGXRange(nRow, nCol, nRow, nCol));
	}

	return !list.IsEmpty();
}

BOOL CGXGridCore::GetSelectedCols(CRowColArray& awLeft, CRowColArray& awRight, BOOL bRangeColsOnly /*=FALSE*/, BOOL bCanUseCurrentCell)
{
	CGXRangeList* pSelList = GetParam()->GetRangeList();

	BOOL bFound
		= pSelList->GetColArray(GetColCount(), awLeft, awRight, bRangeColsOnly);
	if (!bFound && bCanUseCurrentCell)
	{
		ROWCOL nRow, nCol;
		bFound = GetCurrentCell(&nRow, &nCol);
		if (bFound)
		{
			awLeft.SetAtGrow(0, nCol);
			awRight.SetAtGrow(0, nCol);
		}
	}
	return bFound;
}

ROWCOL CGXGridCore::GetSelectedCols(CRowColArray& awCols, BOOL bRangeColsOnly /*=FALSE*/, BOOL bCanUseCurrentCell)
{
	CRowColArray awLeft, awRight;
	int nCount = 0;

	awCols.SetSize(0);

	if (GetSelectedCols(awLeft, awRight, bRangeColsOnly, bCanUseCurrentCell))
	{
		for (int i = 0; i < awLeft.GetSize(); i++)
		{
			for (ROWCOL nCol = awLeft[i]; nCol <= awRight[i]; nCol++)
				awCols.SetAtGrow(nCount++, nCol);
		}
	}

	return nCount;
}

BOOL CGXGridCore::GetSelectedRows(CRowColArray& awTop, CRowColArray& awBottom, BOOL bRangeRowsOnly/*=FALSE*/, BOOL bCanUseCurrentCell)
{
	CGXRangeList* pSelList = GetParam()->GetRangeList();

	BOOL bFound
		= pSelList->GetRowArray(GetRowCount(), awTop, awBottom, bRangeRowsOnly);
	if (!bFound && bCanUseCurrentCell)
	{
		ROWCOL nRow, nCol;
		bFound = GetCurrentCell(&nRow, &nCol);
		if (bFound)
		{
			awTop.SetAtGrow(0, nRow);
			awBottom.SetAtGrow(0, nRow);
		}
	}
	return bFound;
}

ROWCOL CGXGridCore::GetSelectedRows(CRowColArray& awRows, BOOL bRangeRowsOnly /*=FALSE*/, BOOL bCanUseCurrentCell)
{
	CRowColArray awTop, awBottom;
	int nCount = 0;

	awRows.SetSize(0);

	if (GetSelectedRows(awTop, awBottom, bRangeRowsOnly, bCanUseCurrentCell))
	{
		for (int i = 0; i < awTop.GetSize(); i++)
		{
			for (ROWCOL nRow = awTop[i]; nRow <= awBottom[i]; nRow++)
				awRows.SetAtGrow(nCount++, nRow);
		}
	}

	return nCount;
}

BOOL CGXGridCore::GetInvertStateRowCol(ROWCOL nRow, ROWCOL nCol, const CGXRangeList* pSelList)
{
	return pSelList->IsCellInList(nRow, nCol);
}

void CGXGridCore::SelectRange(const CGXRange& range, BOOL bSelect /* = TRUE */, BOOL bUpdate )
{
	GX_CHECKIMP(m_pUserSelectRangeImp, "CGXGridCore::SelectRange");

	if (m_pUserSelectRangeImp)
		m_pUserSelectRangeImp->SelectRange(this, range, bSelect, bUpdate);
}

BOOL CGXGridCore::StoreSelectRange(const CGXRange& range, BOOL bSelect)
{
	GX_CHECKIMP(m_pUserSelectRangeImp, "CGXGridCore::StoreSelectRange");

	if (m_pUserSelectRangeImp)
		return m_pUserSelectRangeImp->StoreSelectRange(this, range, bSelect);

	return FALSE;
}

void CGXGridCore::UpdateSelectRange(const CGXRange& range, const CGXRangeList* pOldRangeList, BOOL bCreateHint)
{
	GX_CHECKIMP(m_pUserSelectRangeImp, "CGXGridCore::UpdateSelectRange");

	if (m_pUserSelectRangeImp)
		m_pUserSelectRangeImp->UpdateSelectRange(this, range, pOldRangeList, bCreateHint);
}

void CGXGridCore::SetSelection(POSITION pos, ROWCOL nTop, ROWCOL nLeft, ROWCOL nBottom, ROWCOL nRight)
{
	GX_CHECKIMP(m_pUserSelectRangeImp, "CGXGridCore::SetSelection");

	if (m_pUserSelectRangeImp)
		m_pUserSelectRangeImp->SetSelection(this, pos, nTop, nLeft, nBottom, nRight);
}

void CGXGridCore::PrepareChangeSelection(POSITION oldPos, ROWCOL top, ROWCOL left, ROWCOL bottom, ROWCOL right, BOOL bCreateHint)
{
	GX_CHECKIMP(m_pUserSelectRangeImp, "CGXGridCore::PrepareChangeSelection");

	if (m_pUserSelectRangeImp)
		m_pUserSelectRangeImp->PrepareChangeSelection(this, oldPos, top, left, bottom, right, bCreateHint);
}

void CGXGridCore::PrepareClearSelection(BOOL bCreateHint)
{
	GX_CHECKIMP(m_pUserSelectRangeImp, "CGXGridCore::PrepareClearSelection");

	if (m_pUserSelectRangeImp)
		m_pUserSelectRangeImp->PrepareClearSelection(this, bCreateHint);
}

// Events

void CGXGridCore::OnChangedSelection(const CGXRange* /*changedRect*/, BOOL /*bIsDragging*/, BOOL /*bKey*/)
{
}

BOOL CGXGridCore::OnStartSelection(ROWCOL nRow, ROWCOL nCol, UINT flags, CPoint point)
{
	nRow, nCol, point, flags;

	return TRUE;
}

// just for compatibility with OG 1.0. It is recommended that
// you override CanChangeSelection instead.
void CGXGridCore::PreChangeSelection(CGXRange* /*range*/, BOOL /*bIsDragging*/, BOOL /*bKey*/)
{
}

BOOL CGXGridCore::CanChangeSelection(CGXRange* pRange, BOOL bIsDragging, BOOL bKey)
{
	// just for compatibility with OG 1.0
	PreChangeSelection(pRange, bIsDragging, bKey);

	return TRUE; // return TRUE unless you want to prevent selecting the range
}

void CGXGridCore::DrawInvertFrame(CDC* pDC, const CRect& rc, const CRect& rectClip, BOOL bTopVisible, BOOL bLeftVisible, int nMarker, BOOL bOnlyMarker)
{
	GX_CHECKIMP(m_pExcelLikeFrameImp, "CGXGridCore::DrawInvertFrame");

	if (m_pExcelLikeFrameImp)
		m_pExcelLikeFrameImp->DrawInvertFrame(this, pDC, rc, rectClip, bTopVisible, bLeftVisible, nMarker, bOnlyMarker);
}

void CGXGridCore::DrawSelectionRangeFrame(CDC* pDC, const CGXRange& rg, int nMarker, BOOL bOnlyMarker)
{
	GX_CHECKIMP(m_pExcelLikeFrameImp, "CGXGridCore::DrawSelectionRangeFrame");

	if (m_pExcelLikeFrameImp)
		m_pExcelLikeFrameImp->DrawSelectionRangeFrame(this, pDC, rg, nMarker, bOnlyMarker);
}

void CGXGridCore::DrawSelectionFrame(CDC* pDC, BOOL bDrawOld, const CGXRange* pNewRange)
{
	if (m_pExcelLikeFrameImp)
		m_pExcelLikeFrameImp->DrawSelectionFrame(this, pDC, bDrawOld, pNewRange);
}

void CGXGridCore::ToggleSelectionFrameMarker()
{
	if (m_pExcelLikeFrameImp)
		m_pExcelLikeFrameImp->ToggleSelectionFrameMarker(this);
}

BOOL CGXGridCore::LockSelectionFrame(BOOL bLock, BOOL bCreateHint)
{                 
	if (m_pExcelLikeFrameImp)
		return m_pExcelLikeFrameImp->LockSelectionFrame(this, bLock, bCreateHint);

	return FALSE;
}

