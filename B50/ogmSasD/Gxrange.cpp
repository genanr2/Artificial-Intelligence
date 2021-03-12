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

// gxrange.cpp : implementation of the CGXRange and CGXRangeList class
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

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXRANGE")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CGXRangeList, CPtrList, 1)

#define new DEBUG_NEW


/////////////////////////////////////////////////////////////////////////////
// CGXRange

CGXRange::CGXRange()
{
	memset(this, 0, sizeof(CGXRange));
	// top = left = bottom = right = 0;
	// rtType = rtCells;
	// m_bValid = FALSE;
}

CGXRange::CGXRange(const CGXRange &r)
{
	*this = r;
}

CGXRange::CGXRange(const CGXRange *r)
{
	if (r)
		*this = *r;
	else
		memset(this, 0, sizeof(CGXRange));
}

CGXRange::CGXRange(ROWCOL nTop, ROWCOL nLeft, ROWCOL nBottom, ROWCOL nRight)
{
	SetCells(nTop, nLeft, nBottom, nRight);
}

const CGXRange& CGXRange::operator=(const CGXRange& r)
{
	memcpy(this, &r, sizeof(CGXRange));
	return *this;
}

CGXRange& CGXRange::SetCells(ROWCOL nTop, ROWCOL nLeft, ROWCOL nBottom, ROWCOL nRight)
{
	rtType = rtCells;
	top =    min(nTop, nBottom);
	left =   min(nLeft, nRight);
	bottom = max(nTop, nBottom);
	right =  max(nLeft, nRight);
	m_bValid = TRUE;

	return *this;
}

CGXRange& CGXRange::SetRows(ROWCOL nFrom, ROWCOL nTo)
{
	rtType = rtRows;
	top    = min(nFrom, nTo);
	bottom = max(nFrom, nTo);
	left = right = 0;
	m_bValid = TRUE;

	return *this;
}

CGXRange& CGXRange::SetCols(ROWCOL nFrom, ROWCOL nTo)
{
	rtType = rtCols;
	left   = min(nFrom, nTo);
	right  = max(nFrom, nTo);
	top = bottom = 0;
	m_bValid = TRUE;

	return *this;
}

CGXRange& CGXRange::SetTable()
{
	rtType = rtTable;
	m_bValid = TRUE;
	return *this;
}

BOOL CGXRange::GetFirstCell(ROWCOL &nRow, ROWCOL &nCol) const
{
	if (!IsValid())
		return FALSE;

	nRow = top;
	nCol = left;
	return TRUE;
}

BOOL CGXRange::GetNextCell(ROWCOL &nRow, ROWCOL &nCol, BOOL bSortByRow /* = TRUE */) const
{
	if (!IsValid())
		return FALSE;

	if (bSortByRow && ++nCol > right)
	{
		if (++nRow > bottom)
			nRow = nCol = 0;
		else
			nCol = left;
	}
	else if (!bSortByRow && ++nRow > bottom)
	{
		if (++nCol > right)
			nRow = nCol = 0;
		else
			nRow = top;
	}

	return nRow > 0 || nCol > 0;
}

CGXRange& CGXRange::ExpandRange(ROWCOL nFirstRow, ROWCOL nFirstCol, ROWCOL nRowCount, ROWCOL nColCount)
{
	switch (rtType)
	{
	case rtCols:
		top = nFirstRow;
		bottom = nRowCount;
		break;
	case rtRows:
		left = nFirstCol;
		right = nColCount;
		break;
	case rtTable:
		top = nFirstRow;
		bottom = nRowCount;
		left = nFirstCol;
		right = nColCount;
		break;
	}

	rtType = rtCells;

	return *this;
}

BOOL CGXRange::IsCellInRange(ROWCOL nRow, ROWCOL nCol) const
{
	return m_bValid
		&& ( (rtType == rtCols || (top <= nRow && nRow <= bottom))
				&& (rtType == rtRows || (left <= nCol && nCol <= right))
			|| rtType == rtTable );
}

BOOL CGXRange::IntersectRange(const CGXRange& r1, const CGXRange& r2)
{
	left = max(r1.left, r2.left);
	right = min(r1.right, r2.right);
	top = max(r1.top, r2.top);
	bottom = min(r1.bottom, r2.bottom);
	return m_bValid = left <= right && top <= bottom;
}

BOOL CGXRange::UnionRange(const CGXRange& r1, const CGXRange& r2)
{
	left = min(r1.left, r2.left);
	right = max(r1.right, r2.right);
	top = min(r1.top, r2.top);
	bottom = max(r1.bottom, r2.bottom);
	return m_bValid = left <= right && top <= bottom;
}

CArchive& AFXAPI operator<<(CArchive& ar, const CGXRange& range)
{
	ar << range.top;
	ar << range.left;
	ar << range.bottom;
	ar << range.right;
	ar << (BYTE) range.rtType;
	ar << (BYTE) range.m_bValid;
	return ar;
}

CArchive& AFXAPI operator>>(CArchive& ar, CGXRange& range)
{
	ar >> range.top;
	ar >> range.left;
	ar >> range.bottom;
	ar >> range.right;
	BYTE b;
	ar >> b;
	range.rtType = (CGXRange::RangeType) b;
	ar >> b;
	range.m_bValid = (BOOL) b;
	return ar;
}

/////////////////////////////////////////////////////////////////////////////
// CGXRangeList

CGXRangeList::~CGXRangeList()
{
	DeleteAll();
}

/////////////////////////////////////////////////////////////////////////////
// CGXRangeList serialization

void CGXRangeList::Serialize(CArchive& ar)
{
//  TRACE0("entering CGXRangeList::Serialize()\n");

	static const WORD wVersion = 1;
	WORD wActualVersion = wVersion;

	ASSERT_VALID( this );

	if (ar.IsStoring())
	{
		ar << wVersion;
	}
	else
	{
		// Check for version first
		ar >> wActualVersion;
		if( wActualVersion != wVersion )
		{
			// Wrong version
#ifdef _DEBUG
			TRACE0("Incompatible format while reading CGXRangeList" );
			TRACE2("in %s at line %d\n", __FILE__, __LINE__);
#endif
			AfxThrowArchiveException(CArchiveException::badSchema);
			return;
		}
	}

	if (ar.IsStoring())
	{
		DeleteGarbage();
		LONG n = GetCount();
		ar << n;
		POSITION pos = GetHeadPosition();
		while (pos)
		{
			const CGXRange* r = GetNext(pos);
			if (r->IsValid())
				ar << *r;
		}
	}
	else
	{
		LONG n;
		ar >> n;

		for (int i = 0; i < n; i++)
		{
			CGXRange* r = new CGXRange;
			ar >> *r;
			AddTail(r);
		}
	}
}

void CGXRangeList::DeleteGarbage()
{
	POSITION pos = GetHeadPosition();
	while (pos)
	{
		POSITION pos2 = pos;
		CGXRange* r = GetNext(pos);
		if (!r->IsValid())
			DeleteAt(pos2);
	}
}

CGXRangeList& CGXRangeList::operator=(const CGXRangeList& l)
{
	ASSERT_VALID( this );

	if (&l == this)
		return *this;
	
	//clean up
	DeleteAll();

	POSITION pos = l.GetHeadPosition();
		
	while (pos)
	{
			const CGXRange* r = l.GetNext(pos);
			if (r->IsValid())
				AddHead(new CGXRange(r));
	}

	ASSERT_VALID( this );

	return *this;
}

BOOL CGXRangeList::IsCellInList(ROWCOL nRow, ROWCOL nCol, POSITION posWithOut) const
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		if (pos == posWithOut)
		{
			GetNext(pos);
			continue;
		}
		const CGXRange* r = GetNext(pos);
		if (r->IsCellInRange(nRow, nCol))
			return TRUE;
	}
	return FALSE;
}

void CGXRangeList::DeleteAll()
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		CGXRange* r = GetNext(pos);
		delete r;
	}
	RemoveAll();
}

void CGXRangeList::DeleteTail()
{
	CGXRange* r = GetTail();
	delete r;
	RemoveTail();
}

POSITION CGXRangeList::FindRange(ROWCOL nRow, ROWCOL nCol)
{
	POSITION pos = GetHeadPosition();
	POSITION pos2 = pos;
	while (pos)
	{
		CGXRange* r = GetNext(pos);
		if (r->IsCellInRange(nRow, nCol))
			break;
		pos2 = pos;
	}
	return pos2;
}

void CGXRangeList::DeleteAt(POSITION position)
{
	CGXRange* r = GetAt(position);
	delete r;
	RemoveAt(position);
}

void CGXRangeList::InsertRows(ROWCOL nRow, ROWCOL nCount)
{
	POSITION pos = GetHeadPosition();
	while (pos)
	{
		CGXRange* r = GetNext(pos);
		if (r->top >= nRow)
			r->top += nCount;
		if (r->bottom >= nRow)
			r->bottom += nCount;
	}
}

void CGXRangeList::InsertCols(ROWCOL nCol, ROWCOL nCount)
{
	POSITION pos = GetHeadPosition();
	while (pos)
	{
		CGXRange* r = GetNext(pos);
		if (r->left >= nCol)
			r->left += nCount;
		if (r->right >= nCol)
			r->right += nCount;
	}
}

void CGXRangeList::RemoveRows(ROWCOL nFromRow, ROWCOL nToRow, ROWCOL nRowCount)
{
	ROWCOL nCount = nToRow-nFromRow+1;
	POSITION pos = GetHeadPosition();
	while (pos)
	{
		CGXRange* r = GetNext(pos);
		if (r->IsValid())
		{
			if (r->top >= nFromRow && r->bottom <= nToRow)
				r->m_bValid = FALSE;
			else
			{
				if (r->top >= nFromRow)
					r->top = max(nFromRow, max(nToRow, r->top)-nCount);
				if (r->bottom >= nFromRow)
					r->bottom = max(nToRow, r->bottom)-nCount;
				r->m_bValid = r->bottom >= r->top;
			}

			r->bottom = min(r->bottom, nRowCount);
		}
	}
}

void CGXRangeList::RemoveCols(ROWCOL nFromCol, ROWCOL nToCol, ROWCOL nColCount)
{
	ROWCOL nCount = nToCol-nFromCol+1;
	POSITION pos = GetHeadPosition();
	while (pos)
	{
		CGXRange* r = GetNext(pos);
		if (r->IsValid())
		{
			if (r->left >= nFromCol && r->right <= nToCol)
				r->m_bValid = FALSE;
			else
			{
				if (r->left >= nFromCol)
					r->left = max(nFromCol, max(nToCol, r->left)-nCount);
				if (r->right >= nFromCol)
					r->right = max(nToCol, r->right)-nCount;
				r->m_bValid = r->right >= r->left;
			}
			
			r->right = min(r->right, nColCount);
		}
	}
}

void CGXRangeList::MoveRows(ROWCOL nFromRow, ROWCOL nToRow, ROWCOL nDestRow, ROWCOL nRowCount)
{
	ROWCOL nCount = nToRow-nFromRow+1;
	if (nDestRow > nFromRow)
		nDestRow += nCount;

	// verschieben
	POSITION pos = GetHeadPosition();
	while (pos)
	{
		CGXRange* r = GetNext(pos);

		if (r->top >= nFromRow && r->top <= nToRow && nToRow < nDestRow)
		{
			// from top to dest
			r->top = nDestRow - nCount + (r->top-nFromRow);
			r->bottom = nDestRow - nCount + (r->bottom-nFromRow);
		}
		else if (r->top >= nFromRow && r->top <= nToRow && nDestRow < nFromRow)
		{
			// dest from top to
			r->top = nDestRow + (r->top-nFromRow);
			r->bottom = nDestRow + (r->bottom-nFromRow);
		}
		else if (r->top > nToRow && r->top < nDestRow)
		{
			// from to top dest
			r->top  = r->top - nCount;
			r->bottom = r->bottom - nCount;
		}
		else if (r->top >= nDestRow && r->top < nFromRow)
		{
			// dest top from to
			r->top = r->top + nCount;
			r->bottom = r->bottom + nCount;
		}

		r->bottom = min(r->bottom, nRowCount);
	}
}

void CGXRangeList::MoveCols(ROWCOL nFromCol, ROWCOL nToCol, ROWCOL nDestCol, ROWCOL nColCount)
{
	ROWCOL nCount = nToCol-nFromCol+1;

	if (nDestCol > nFromCol)
		nDestCol += nCount;

	// verschieben
	POSITION pos = GetHeadPosition();
	while (pos)
	{
		CGXRange* r = GetNext(pos);

		if (r->left >= nFromCol && r->left <= nToCol && nToCol < nDestCol)
		{
			/* from left to dest */
			r->left = nDestCol - nCount + (r->left-nFromCol);
			r->right = nDestCol - nCount + (r->right-nFromCol);
		}
		else if (r->left >= nFromCol && r->left <= nToCol && nDestCol < nFromCol)
		{
			// dest from left to
			r->left = nDestCol + (r->left-nFromCol);
			r->right = nDestCol + (r->right-nFromCol);
		}
		else if (r->left > nToCol && r->left < nDestCol)
		{
			// from to left dest
			r->left  = r->left - nCount;
			r->right = r->right - nCount;
		}
		else if (r->left >= nDestCol && r->left < nFromCol)
		{
			// dest left from to
			r->left = r->left + nCount;
			r->right = r->right + nCount;
		}

		r->right = min(r->right, nColCount);

		// TRACE("Range %2d,%2d,%2d,%2d\n", r->left, r->right, r->top, r->bottom);
	}
// #endif
}

#if OLDFUNC
BOOL CGXRangeList::FindFixedRows(POSITION& pos, ROWCOL nCount, ROWCOL& nStart, ROWCOL& nEnd)
{
	BOOL bFound = FALSE;
	while (pos)
	{
		CGXRange* r = GetNext(pos);
		if (!r->IsValid())
			continue;

		if (r->rtType == CGXRange::rtRows)
		{
			nStart = r->top;
			nEnd   = r->bottom;
			bFound = TRUE;
			break;
		}
		else if (r->rtType == CGXRange::rtTable)
		{
			nStart = 1;
			nEnd   = nCount;
			bFound = TRUE;
			break;
		}
	}

	// Check if there is another Rectangle
	POSITION pos2 = pos;
	while (pos2)
	{
		CGXRange* r = GetNext(pos2);
		if (r->IsValid()
			&& (r->rtType == CGXRange::rtRows
			|| r->rtType == CGXRange::rtTable))
			break;
		pos = pos2;
	}
	return bFound;
}

BOOL CGXRangeList::FindFixedCols(POSITION& pos, ROWCOL nCount, ROWCOL& nStart, ROWCOL& nEnd)
{
	BOOL bFound = FALSE;
	while (pos)
	{
		CGXRange* r = GetNext(pos);
		if (!r->IsValid())
			continue;

		if (r->rtType == CGXRange::rtCols)
		{
			nStart = r->left;
			nEnd   = r->right;
			bFound = TRUE;
			break;
		}
		else if (r->rtType == CGXRange::rtTable)
		{
			nStart = 1;
			nEnd   = nCount;
			bFound = TRUE;
			break;
		}
	}
	// Check if there is another Rectangle
	POSITION pos2 = pos;
	while (pos2)
	{
		CGXRange* r = GetNext(pos2);
		if (r->IsValid()
			&& (r->rtType == CGXRange::rtCols
			|| r->rtType == CGXRange::rtTable))
			break;
		pos = pos2;
	}
	return bFound;
}
#endif

BOOL CGXRangeList::IsAnyCellFromCol(ROWCOL nCol)
{
	POSITION pos = GetHeadPosition();
	while (pos)
	{
		CGXRange* r = GetNext(pos);
		if (r->IsValid()
			&& (r->rtType == CGXRange::rtRows
				|| r->rtType == CGXRange::rtTable
				|| (nCol >= r->left && nCol <= r->right)))
			return TRUE;
	}
	return FALSE;
}

BOOL CGXRangeList::IsAnyCellFromRow(ROWCOL nRow)
{
	POSITION pos = GetHeadPosition();
	while (pos)
	{
		CGXRange* r = GetNext(pos);
		if (r->IsValid()
			&& (r->rtType == CGXRange::rtCols
				|| r->rtType == CGXRange::rtTable
				|| (nRow >= r->top && nRow <= r->bottom)))
			return TRUE;
	}
	return FALSE;
}

BOOL CGXRangeList::GetColArray(ROWCOL nColCount, CRowColArray& awLeft, CRowColArray& awRight, BOOL bFixed/*=FALSE*/) const
{
	POSITION pos = GetHeadPosition();
	BOOL bFound = FALSE;

	awLeft.RemoveAll();
	awRight.RemoveAll();

	// create sorted arrays
	while (pos)
	{
		CGXRange* r = GetNext(pos);
		if (r->IsValid()
			&& (!bFixed || r->rtType == CGXRange::rtCols || r->rtType == CGXRange::rtTable))
		{
			BOOL bDone = FALSE;
			ROWCOL left = r->left, right = r->right;

			if (r->IsRows() || r->IsTable())
				right = nColCount;
			int index;
			for (index = 0; index < awLeft.GetSize() && !bDone; index++)
			{
				if (left >= awLeft[index] && left <= awRight[index]
					|| right >= awLeft[index] && right <= awRight[index]
					|| awLeft[index] >= left && awLeft[index] <= right
					|| awRight[index] >= left && awRight[index] <= right)
				{
					awLeft[index] = min(left, awLeft[index]);
					awRight[index] = max(right, awRight[index]);
					bDone = TRUE;
				}
				else if (awLeft[index] > left)
				{
					awLeft.InsertAt(index, left);
					awRight.InsertAt(index, right);
					bDone = TRUE;
				}
			}
			if (!bDone)
			{
				awLeft.InsertAt(index, left);
				awRight.InsertAt(index, right);
			}
			bFound = TRUE;
		}
	}

	// remove intersections
	if (bFound)
	{
		for (int index = 0; index+1 < awLeft.GetSize(); index++)
		{
			while (index+1 < awLeft.GetSize() && awRight[index]+1 >= awLeft[index+1])
			{
				awRight[index] = max(awRight[index], awRight[index+1]);
				awRight.RemoveAt(index+1);
				awLeft.RemoveAt(index+1);
			}
		}
	}

	return bFound;
}

BOOL CGXRangeList::GetRowArray(ROWCOL nRowCount, CRowColArray& awTop, CRowColArray& awBottom, BOOL bFixed/*=FALSE*/) const
{
	POSITION pos = GetHeadPosition();
	BOOL bFound = FALSE;

	awTop.RemoveAll();
	awBottom.RemoveAll();

	// create sorted arrays
	while (pos)
	{
		CGXRange* r = GetNext(pos);

		if (r->IsValid()
			&& (!bFixed || r->rtType == CGXRange::rtRows || r->rtType == CGXRange::rtTable))
		{
			BOOL bDone = FALSE;
			ROWCOL top = r->top, bottom = r->bottom;

			if (r->IsCols() || r->IsTable())bottom = nRowCount;
			int index;
			for (index = 0; index < awTop.GetSize() && !bDone; index++)
			{
				if (top >= awTop[index] && top <= awBottom[index]
					|| bottom >= awTop[index] && bottom <= awBottom[index]
					|| awTop[index] >= top && awTop[index] <= bottom
					|| awBottom[index] >= top && awBottom[index] <= bottom)
				{
					awTop[index] = min(top, awTop[index]);
					awBottom[index] = max(bottom, awBottom[index]);
					bDone = TRUE;
				}
				else if (awTop[index] > top)
				{
					awTop.InsertAt(index, top);
					awBottom.InsertAt(index, bottom);
					bDone = TRUE;
				}
			}
			if (!bDone)
			{
				awTop.InsertAt(index, top);
				awBottom.InsertAt(index, bottom);
			}
			bFound = TRUE;
		}
	}

	// remove intersections
	if (bFound)
	{
		for (int index = 0; index+1 < awTop.GetSize(); index++)
		{
			while (index+1 < awTop.GetSize() && awBottom[index]+1 >= awTop[index+1])
			{
				awBottom[index] = max(awBottom[index], awBottom[index+1]);
				awBottom.RemoveAt(index+1);
				awTop.RemoveAt(index+1);
			}
		}
	}

	return bFound;
}


/////////////////////////////////////////////////////////////////////////////
// CGXRangeList diagnostics
/*
#ifdef _DEBUG
void CGXRangeList::Dump(CDumpContext& dc) const
{

	POSITION pos = GetHeadPosition();
	while (pos)
	{
		CGXRange* r = GetNext(pos);
		dc << _T("(");
		dc << (UINT) r->rtType;
		dc << _T(":");
		dc << r->top;
		dc << _T(",");
		dc << r->left;
		dc << _T(",");
		dc << r->bottom;
		dc << _T(",");
		dc << r->right;
		dc << _T(":");
		dc << r->m_bValid;
		dc << _T(")\n");
	}
}
#endif //_DEBUG
*/