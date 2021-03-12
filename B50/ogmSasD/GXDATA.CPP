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

// gxdata.cpp : implementation of the CGXData class
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

#ifndef _GXSTYLES_H_
#include "gxstyles.h"
#endif

#ifndef _GXEXT_H_
#include "gxext.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXDATA")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CGXData, CObject, 0 /* schema number*/ )
IMPLEMENT_DYNAMIC(CGXStylePtrArray, CPtrArray)
IMPLEMENT_DYNAMIC(CGXStylePtrArrayPtrArray, CPtrArray)

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// class CGXStylePtrArray

CGXStylePtrArray::~CGXStylePtrArray()
{
	DeleteAll();
}

void CGXStylePtrArray::DeleteAll()
{
	for (int index = 0; index <= GetUpperBound(); index++)
		delete GetAt(index);

	RemoveAll();
}

void CGXStylePtrArray::DeleteAt(int nIndex, int nCount)
{
	int nUpper = min(GetUpperBound(), nIndex+nCount-1);
	if (nUpper >= nIndex)
	{
		for (int index = nIndex; index <= nUpper; index++)
			delete GetAt(index);

		RemoveAt(nIndex, nUpper-nIndex+1);
	}
}

void CGXStylePtrArray::Move(int nIndex, int nCount, int nDest)
{
	if (nDest == nIndex || nCount == 0
		|| GetUpperBound() < nIndex && GetUpperBound() < nDest)
		return;
	else if (nDest < nIndex)
		nIndex += nCount;
	else if (nDest > nIndex)
		nDest += nCount;

	InsertAt(nDest, NULL, nCount);
	for (int n = 0; nIndex+n <= GetUpperBound() && n < nCount; n++)
	{
		if (GetAt(nIndex+n))
			SetAt(nDest+n, GetAt(nIndex+n));
	}

	if (nIndex <= GetUpperBound())
		RemoveAt(nIndex, min(nCount, GetUpperBound()-nIndex+1));
}

const CGXStylePtrArray& CGXStylePtrArray::operator=(const CGXStylePtrArray& other)
{
	if (&other == this)
		return *this;

	DeleteAll();

	SetSize(other.GetSize());
	for (int index = 0; index <= other.GetUpperBound(); index++)
	{
		CGXStyle* pStyle = other.GetAt(index);
		if (pStyle)
			SetAt(index, pStyle->Clone());
	}

	return *this;
}

void CGXStylePtrArray::Serialize(CArchive& ar)
{
	GX_CHECKIMP(GXGetAppData()->m_pDocSerializeImp, "CGXStylePtrArray::Serialize");

	if (GXGetAppData()->m_pDocSerializeImp)
		GXGetAppData()->m_pDocSerializeImp->Serialize(this, ar);
}

/////////////////////////////////////////////////////////////////////////////
// class CGXStylePtrArrayPtrArray

CGXStylePtrArrayPtrArray::~CGXStylePtrArrayPtrArray()
{
	DeleteAll();
}

void CGXStylePtrArrayPtrArray::DeleteAt(int nIndex, int nCount)
{
	int nUpper = min(GetUpperBound(), nIndex+nCount-1);
	if (nUpper >= nIndex)
	{
		for (int index = nIndex; index <= nUpper; index++)
			delete GetAt(index);

		RemoveAt(nIndex, nUpper-nIndex+1);
	}
}

void CGXStylePtrArrayPtrArray::DeleteAll()
{
	for (int index = 0; index <= GetUpperBound(); index++)
		delete GetAt(index);

	RemoveAll();
}

void CGXStylePtrArrayPtrArray::Move(int nIndex, int nCount, int nDest)
{
	if (nDest == nIndex || nCount == 0
		|| GetUpperBound() < nIndex && GetUpperBound() < nDest)
		return;
	else if (nDest < nIndex)
		nIndex += nCount;
	else if (nDest > nIndex)
		nDest += nCount;

	InsertAt(nDest, NULL, nCount);
	for (int n = 0; nIndex+n <= GetUpperBound() && n < nCount; n++)
	{
		if (GetAt(nIndex+n))
			SetAt(nDest+n, GetAt(nIndex+n));
	}

	if (nIndex <= GetUpperBound())
		RemoveAt(nIndex, min(nCount, GetUpperBound()-nIndex+1));
}

const CGXStylePtrArrayPtrArray& CGXStylePtrArrayPtrArray::operator=(const CGXStylePtrArrayPtrArray& other)
{
	if (&other == this)
		return *this;

	DeleteAll();

	SetSize(other.GetSize());
	for (int index = 0; index <= other.GetUpperBound(); index++)
	{
		if (other.GetAt(index))
			SetAt(index, new CGXStylePtrArray(*other.GetAt(index)));
	}

	return *this;
}

void CGXStylePtrArrayPtrArray::Serialize(CArchive& ar)
{
	GX_CHECKIMP(GXGetAppData()->m_pDocSerializeImp, "CGXStylePtrArrayPtrArray::Serialize");

	if (GXGetAppData()->m_pDocSerializeImp)
		GXGetAppData()->m_pDocSerializeImp->Serialize(this, ar);
}

/////////////////////////////////////////////////////////////////////////////
// class CGXData

CGXData::CGXData()
{
	m_pAppData = GXGetAppData();

	ASSERT(m_pAppData->m_bInitDone);
	// ASSERTION-> This constructor is called before the GXInit()
	// method has been called. This can be problematic because
	// at this time no factory object is available.
	//
	// Make sure you didn't simply forget to call GXInit() from
	// your application's InitInstance method. 
	//
	// Another reason for this problem can be that the object is 
	// constructed at the same time that your application object 
	// is instantiated, for example if you embedded this class 
	// as member variable in the application class. 
	// 
	// To resolve the problem we recommend you instantiate the
	// object on the heap. Change your class declaration and
	// declare a pointer to this class. After GXInit() has been
	// called you can call m_pData = new CGXData. In your
	// class destructor don't forget to delete m_pData.
	// ->END

	m_nRows = 0;
	m_nCols = 0;
}

CGXData::~CGXData()
{
	DeleteContents();
}

void CGXData::DeleteContents()
{
	m_apapCellStyle.DeleteAll();
	m_apRowBaseStyle.DeleteAll();
	m_apColBaseStyle.DeleteAll();
}

const CGXData& CGXData::operator=(const CGXData& p)
{
	if (&p == this)
		return *this;

	m_apapCellStyle = p.m_apapCellStyle;
	m_apRowBaseStyle = p.m_apRowBaseStyle;
	m_apColBaseStyle = p.m_apColBaseStyle;
	m_nRows = p.m_nRows;
	m_nCols = p.m_nCols;

	return *this;
}

void CGXData::InitParam(CGXGridParam* pParam)
{
	/*
	pParam->m_bDirectCopyPaste = FALSE;
	pParam->m_bDirectCutPaste = FALSE;
	pParam->m_bDirectDragDrop = FALSE;
	*/
	pParam;
}

/////////////////////////////////////////////////////////////////////////////
// serialization


void CGXData::Serialize(CArchive& ar)
{
	GX_CHECKIMP(m_pAppData->m_pDocSerializeImp, "CGXData::Serialize");

	if (m_pAppData->m_pDocSerializeImp)
		m_pAppData->m_pDocSerializeImp->Serialize(this, ar);
}

/////////////////////////////////////////////////////////////////////////////
// Number of Rows and Colums

ROWCOL CGXData::GetRowCount()
{
	return (ROWCOL) m_nRows;
}

ROWCOL CGXData::GetColCount()
{
	return (ROWCOL) m_nCols;
}

BOOL CGXData::StoreRowCount(ROWCOL nRows)
{
	if (nRows < m_nRows)
		StoreRemoveRows(nRows+1, m_nRows);
	else if (nRows > m_nRows)
		StoreInsertRows(m_nRows+1, nRows-m_nRows);
	return TRUE;
}

BOOL CGXData::StoreColCount(ROWCOL nCols)
{
	if (nCols < m_nCols)
		StoreRemoveCols(nCols+1, m_nCols);
	else if (nCols > m_nCols)
		StoreInsertCols(m_nCols+1, nCols-m_nCols);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// Insert, Remove and Move Cells

BOOL CGXData::StoreInsertRows(ROWCOL nRow, ROWCOL nCount)
{
	ASSERT(nRow <= INT_MAX && nCount <= INT_MAX);

	int row = (int) nRow;
	int count = (int) nCount;

	m_apRowBaseStyle.InsertAt(row, NULL, count);
	m_apapCellStyle.InsertAt(row, NULL, count);

	m_nRows += nCount;

	return TRUE;
}

BOOL CGXData::StoreInsertCols(ROWCOL nCol, ROWCOL nCount)
{
	ASSERT(nCol <= INT_MAX && nCount <= INT_MAX);

	int col = (int) nCol;
	int count = (int) nCount;

	m_apColBaseStyle.InsertAt(col, NULL, count);

	// insert columns for each row
	for (int row = 0; row <= m_apapCellStyle.GetUpperBound(); row++)
	{
		if (m_apapCellStyle.GetAt(row))
			m_apapCellStyle.GetAt(row)->InsertAt(col, NULL, count);
	}

	m_nCols += nCount;

	return TRUE;
}


BOOL CGXData::StoreRemoveRows(ROWCOL nFromRow, ROWCOL nToRow)
{
	ASSERT(nFromRow <= INT_MAX && nToRow <= INT_MAX);

	if (nFromRow > m_nRows)
		return TRUE;

	int row = (int) nFromRow;
	int count = (int) (nToRow-nFromRow+1);

	m_apRowBaseStyle.DeleteAt(row, count);
	m_apapCellStyle.DeleteAt(row, count);

	m_nRows -= min(nToRow-nFromRow+1, m_nRows);

	return TRUE;
}


BOOL CGXData::StoreRemoveCols(ROWCOL nFromCol, ROWCOL nToCol)
{
	ASSERT(nFromCol <= INT_MAX && nToCol <= INT_MAX);

	if (nFromCol > m_nCols)
		return TRUE;

	int col = (int) nFromCol;
	int count = (int) (nToCol-nFromCol+1);

	m_apColBaseStyle.DeleteAt(col, count);

	// delete columns for each row
	for (int row = 0; row <= m_apapCellStyle.GetUpperBound(); row++)
	{
		if (m_apapCellStyle.GetAt(row))
			m_apapCellStyle.GetAt(row)->DeleteAt(col, count);
	}

	m_nCols -= min(nToCol-nFromCol+1, m_nCols);

	return TRUE;
}

BOOL CGXData::StoreMoveRows(ROWCOL nFromRow, ROWCOL nToRow, ROWCOL nDestRow)
{
	ASSERT(nFromRow <= INT_MAX && nToRow <= INT_MAX && nDestRow <= INT_MAX);

	int row = (int) nFromRow;
	int count = (int) (nToRow-nFromRow+1);
	int dest = (int) nDestRow;

	m_apRowBaseStyle.Move(row, count, dest);
	m_apapCellStyle.Move(row, count, dest);

	return TRUE;
}

BOOL CGXData::StoreMoveCols(ROWCOL nFromCol, ROWCOL nToCol, ROWCOL nDestCol)
{
	ASSERT(nFromCol <= INT_MAX && nToCol <= INT_MAX && nDestCol <= INT_MAX);

	int col = (int) nFromCol;
	int count = (int) (nToCol-nFromCol+1);
	int dest = (int) nDestCol;

	m_apColBaseStyle.Move(col, count, dest);

	// move columns for each row
	for (int row = 0; row <= m_apapCellStyle.GetUpperBound(); row++)
	{
		if (m_apapCellStyle.GetAt(row))
			m_apapCellStyle.GetAt(row)->Move(col, count, dest);
	}

	return TRUE;
}

BOOL CGXData::StoreCopyCells(CGXRange rg, ROWCOL nRow, ROWCOL nCol)
{
	GX_CHECKIMP(GXGetAppData()->m_pDataCopyMoveCellsImp, "CGXData::StoreCopyCells");

	if (GXGetAppData()->m_pDataCopyMoveCellsImp)
		return GXGetAppData()->m_pDataCopyMoveCellsImp->StoreCopyCells(this, rg, nRow, nCol);

	return FALSE;
}

BOOL CGXData::StoreMoveCells(CGXRange rg, ROWCOL nRow, ROWCOL nCol, CObject*& pUndoInfo, BOOL bCreateUndoInfo, int ctCmd)
{
	GX_CHECKIMP(GXGetAppData()->m_pDataCopyMoveCellsImp, "CGXData::StoreMoveCells");

	if (GXGetAppData()->m_pDataCopyMoveCellsImp)
		return GXGetAppData()->m_pDataCopyMoveCellsImp->StoreMoveCells(this, rg, nRow, nCol, pUndoInfo, bCreateUndoInfo, ctCmd);

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Cell Style

BOOL CGXData::StoreStyleRowCol(ROWCOL nRow, ROWCOL nCol, const CGXStyle* pStyle, GXModifyType mt, int nType)
{
	m_nRows = max(nRow, m_nRows);
	m_nCols = max(nCol, m_nCols);

	int row = (int) nRow;
	int col = (int) nCol;

	if (pStyle == NULL)
	{
		ASSERT(mt == gxRemove);
		// ASSERTION-> Do you want to remove the style or is it a bug that pStyle is NULL? ->END
	}
	else
	{
		ASSERT_VALID(pStyle); ASSERT(pStyle->IsKindOf(RUNTIME_CLASS(CGXStyle)));
		// ASSERTION-> style is invalid ->END

		ASSERT(mt != gxRemove);
		// ASSERTION-> If you want to remove the style you don't need a style! ->END
	}

	CGXStyle *pCellStyle = NULL;

	if (nType == -1)    // nType is -1 for row, column and table ranges
	{
		ASSERT(row == 0 || col == 0);

		// either change existing cell

		if (row == 0)
		{
			// base styles for columns
			if (col <= m_apColBaseStyle.GetUpperBound())
				pCellStyle = m_apColBaseStyle.GetAt(col);

			if (pStyle == NULL)
			{
				// remove cell
				if (pCellStyle)
				{
					delete pCellStyle;
					m_apColBaseStyle.SetAtGrow(col, NULL);
				}
			}
			else
			{
				// store style
				if (pCellStyle)
					pCellStyle->ChangeStyle(*pStyle, mt);
				else if (mt != gxExclude)
					m_apColBaseStyle.SetAtGrow(col, pStyle->Clone());
			}
		}
		else
		{
			// base styles for rows
			if (row <= m_apRowBaseStyle.GetUpperBound())
				pCellStyle = m_apRowBaseStyle.GetAt(row);

			if (pStyle == NULL)
			{
				// remove cell
				if (pCellStyle)
				{
					delete pCellStyle;
					m_apRowBaseStyle.SetAtGrow(row, NULL);
				}
			}
			else
			{
				// store style
				if (pCellStyle)
					pCellStyle->ChangeStyle(*pStyle, mt);
				else if (mt != gxExclude)
					m_apRowBaseStyle.SetAtGrow(row, pStyle->Clone());
			}
		}
	}
	else
	{
		if (pStyle == NULL)
		{
			// remove cell
			if (row <= m_apapCellStyle.GetUpperBound())
			{
				CGXStylePtrArray* paCells = m_apapCellStyle.GetAt(row);
				if (paCells && col <= paCells->GetUpperBound())
				{
					delete paCells->GetAt(col);
					paCells->SetAt(col, NULL);
				}
			}
		}
		else
		{
			// store style

			// get cell-array for row
			CGXStylePtrArray* paCells = NULL;
			if (row <= m_apapCellStyle.GetUpperBound())
				paCells = m_apapCellStyle.GetAt(row);
			else
				// increase array size
				m_apapCellStyle.SetSize((int) m_nRows+1);

			// check if row must be created
			if (paCells == NULL)
			{
				paCells = new CGXStylePtrArray;
				m_apapCellStyle.SetAtGrow(row, paCells);
			}

			if (paCells && col <= paCells->GetUpperBound())
				pCellStyle = paCells->GetAt(col);
			else
				paCells->SetSize((int) m_nCols+1);

			BOOL bNum = FALSE;
			unsigned nValFormat;
			unsigned nValPlaces;
				
			if (GXGetAppData()->m_pNumberFormattingImp
				&& mt != gxExclude && nType == GX_VALUE_EXPRESSION && pStyle->GetIncludeValue() 
				&& pStyle->GetValueType() != GX_VT_NUMERIC)
			{
				double d;

				bNum = GXDeval(pStyle->GetValueRef(), &d, &nValFormat, &nValPlaces);
			}

			if (pCellStyle)
			{
				pCellStyle->ChangeStyle(*pStyle, mt);

				if (bNum)
					pCellStyle->SetValueType(GX_VT_NUMERIC);
			}
			else if (mt != gxExclude)
			{
				CGXStyle* pNewStyle;
				pNewStyle = pStyle->Clone();
				if (bNum)
					pNewStyle->SetValueType(GX_VT_NUMERIC);
				paCells->SetAtGrow(col, pNewStyle);
			}
		}
	}

	return TRUE;
}

BOOL CGXData::GetStyleRowCol(ROWCOL nRow, ROWCOL nCol, CGXStyle& style, GXModifyType mt, int nType)
{
	if (nRow > m_nRows || nCol > m_nCols)
		return FALSE;

	int row = (int) nRow;
	int col = (int) nCol;

	ASSERT(style.IsKindOf(RUNTIME_CLASS(CGXStyle)));
	ASSERT_VALID(&style);

	CGXStyle* pCellStyle = NULL;

	if (nType == -1)    // nType is -1 for row, column and table ranges
	{
		ASSERT(row == 0 || col == 0);

		// either change existing cell

		if (row == 0)
		{
			// base styles for columns
			if (col <= m_apColBaseStyle.GetUpperBound())
				pCellStyle = m_apColBaseStyle.GetAt(col);
		}
		else
		{
			// base styles for rows
			if (row <= m_apRowBaseStyle.GetUpperBound())
				pCellStyle = m_apRowBaseStyle.GetAt(row);
		}
	}
	else
	{
		// get cell-array for row
		CGXStylePtrArray* paCells = NULL;
		if (row <= m_apapCellStyle.GetUpperBound())
			paCells = m_apapCellStyle.GetAt(row);

		// check if column is there
		if (paCells && col <= paCells->GetUpperBound())
			pCellStyle = paCells->GetAt(col);
	}

	if (pCellStyle)
	{
		if (mt == gxCopy)
			style = *pCellStyle;
		else
			style.ChangeStyle(*pCellStyle, mt);
		return TRUE;
	}

	// ... no style found
	return FALSE;
}

BOOL CGXData::StoreValueRowCol(ROWCOL nRow, ROWCOL nCol, LPCTSTR pszValue, GXModifyType mt, int nType, CGXStyle* pValueStyle)
{
	if (pValueStyle == NULL)
		// I have to use a CGXStyle object because I have no
		// access to CGXGridCore::CreateStyle from here
		pValueStyle = &m_tmpValueGetStyle;

	pValueStyle->Free();
	pValueStyle->SetValue(pszValue);

	return StoreStyleRowCol(nRow, nCol, pValueStyle, mt, nType);
}

LPCTSTR CGXData::GetValueRowCol(ROWCOL nRow, ROWCOL nCol, int nType, CGXStyle* pValueStyle)
{
	if (pValueStyle == NULL)
		// I have to use a CGXStyle object because I have no
		// access to CGXGridCore::CreateStyle from here
		pValueStyle = &m_tmpValueGetStyle;

	pValueStyle->Free();
	GetStyleRowCol(nRow, nCol, *pValueStyle, gxOverride, nType);

	return pValueStyle->GetValueRef();
}

// Sorting (rearranging) CGXData

BOOL CGXData::StoreMoveDataRows(const CGXRange& sortRange, const CRowColArray& anRowIndex)
{
	GX_CHECKIMP(GXGetAppData()->m_pSortMoveDataImp, "CGXData::StoreMoveDataRows");

	if (GXGetAppData()->m_pSortMoveDataImp)
		return GXGetAppData()->m_pSortMoveDataImp->StoreMoveDataRows(this, sortRange, anRowIndex);

	return FALSE;
}

BOOL CGXData::StoreMoveDataCols(const CGXRange& sortRange, const CRowColArray& anColIndex)
{
	GX_CHECKIMP(GXGetAppData()->m_pSortMoveDataImp, "CGXData::StoreMoveDataCols");

	if (GXGetAppData()->m_pSortMoveDataImp)
		return GXGetAppData()->m_pSortMoveDataImp->StoreMoveDataCols(this, sortRange, anColIndex);

	return FALSE;
}

CGXGridCore* CGXData::Grid() const
{
	return GXGetAppData()->GetFirstGrid((void*) this);
}

BOOL CGXData::GetUpdateFlag(ROWCOL nRow, ROWCOL nCol)
{
	nRow, nCol;

	m_tmpValueStyle.SetValue(_T(""));

	return FALSE;
}

void CGXData::SetUpdateFlag(ROWCOL nRow, ROWCOL nCol, BOOL bToggle)
{
	nRow, nCol, bToggle;
}

void CGXData::AddRecalcRange(const CGXRange& range)
{
	range;
}

void CGXData::Recalc(BOOL bForceRecalc)
{
	bForceRecalc;
}

void CGXData::ResetError()
{
	;
}

BOOL CGXData::GetError(CString& strError)
{
	strError;

	return FALSE;
}

BOOL CGXData::SetAdvancedUndo(BOOL b)
{
	b;

	return FALSE;
}

CObject* CGXData::GetAdvancedUndoInfo()
{
	return NULL;
}

BOOL CGXData::UndoAdvanced(CObject* pAdvUndo, BOOL bCreateUndoInfo, BOOL bOnlyToolCells)
{
	pAdvUndo, bCreateUndoInfo, bOnlyToolCells;

	return FALSE;
}

BOOL CGXData::SetRangeName(LPCTSTR name, const CGXRange& rg, CObject* pUndoInfo)
{
	name, rg, pUndoInfo;

	return FALSE;
}

BOOL CGXData::GetRangeName(LPCTSTR name, CGXRange& rg)
{
	name, rg;

	return FALSE;
}

BOOL CGXData::DeleteRangeName(LPCTSTR name, CObject*& pUndoInfo, BOOL bCreateUndoInfo)
{
	name, pUndoInfo, bCreateUndoInfo;

	return FALSE;
}

