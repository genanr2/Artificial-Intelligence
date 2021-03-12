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

// gxcpmovd.cpp : CGXData support for direct copy/move cells
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

#ifndef _GXEXT_H_
#include "gxext.h"
#endif

#ifndef _GXSTYLES_H_
#include "gxstyles.h"
#endif

#ifndef _GXDELAYR_H_
#include "gxdelayr.h"
#endif

#include "gxpriv.h"

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXPARAM")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


#define new DEBUG_NEW

// CGXAbstractDataCopyMoveCellsImp* m_pDataCopyMoveCellsImp;
class CGXDataCopyMoveCellsImp: public CGXAbstractDataCopyMoveCellsImp
{
public:
	virtual BOOL StoreCopyCells(CGXData* pData, CGXRange rg, ROWCOL nRow, ROWCOL nCol);
	virtual BOOL StoreMoveCells(CGXData* pData, CGXRange& rg, ROWCOL nRow, ROWCOL nCol, CObject*& pUndoInfo, BOOL bCreateUndoInfo, int ctCmd);
};

void AFXAPI CGXData::ImplementCopyMoveCells()
{
	if (GXGetAppData()->m_pDataCopyMoveCellsImp == NULL)
		GXGetAppData()->AutoDeletePtr(GXGetAppData()->m_pDataCopyMoveCellsImp = new CGXDataCopyMoveCellsImp);
}

BOOL CGXDataCopyMoveCellsImp::StoreCopyCells(CGXData* pData, CGXRange rg, ROWCOL nRow, ROWCOL nCol)
{
	for (ROWCOL nSrcRow = rg.top; nSrcRow <= rg.bottom; nSrcRow++)
	{
		// Source row
		int srcrow = (int) nSrcRow;
		CGXStylePtrArray* paSrcCells; 
		if (srcrow <= pData->m_apapCellStyle.GetUpperBound())
			paSrcCells = pData->m_apapCellStyle.GetAt(srcrow);
		else
			paSrcCells = NULL;

		// Dest Row
		ROWCOL nDestRow = nRow + (nSrcRow - rg.top);
		int destrow = (int) nDestRow;
		CGXStylePtrArray* paDestCells = NULL; 
		if (destrow <= pData->m_apapCellStyle.GetUpperBound())
			paDestCells = pData->m_apapCellStyle.GetAt(destrow);
		else
		{
			if (paSrcCells == NULL)
				continue; // both dest and src are empty

			// increase array size
			paDestCells = NULL;
			pData->m_apapCellStyle.SetSize((int) pData->m_nRows+1);
		}

		// check if row must be created
		if (paDestCells == NULL)
		{
			paDestCells = new CGXStylePtrArray;
			pData->m_apapCellStyle.SetAtGrow(destrow, paDestCells);
		}

		for (ROWCOL nSrcCol = rg.left; nSrcCol <= rg.right; nSrcCol++)
		{
			// Source Style
			int col = (int) nSrcCol;
			CGXStyle *pSrcStyle;
			if (paSrcCells && col <= paSrcCells->GetUpperBound())
				pSrcStyle = paSrcCells->GetAt(col);
			else
				pSrcStyle = NULL;

			// Dest Style
			ROWCOL nDestCol = nCol + (nSrcCol - rg.left);
			int destcol = (int) nDestCol;
			CGXStyle *pDestStyle;

			// delete target style
			if (paDestCells && destcol <= paDestCells->GetUpperBound())
			{
				delete paDestCells->GetAt(destcol);
				if (pSrcStyle == NULL)
					paDestCells->SetAt(destcol, NULL);
			}
			pDestStyle = NULL;

			// copy style
			if (pSrcStyle)
				paDestCells->SetAtGrow(destcol, pSrcStyle->Clone());
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////
// CGXUndoEngineCellsInfo
// holds undo info moving cells (cut/paste, ole dnd)

struct CGXStylePtrList
{
	CGXStylePtrList* next;
	ROWCOL nRow;
	ROWCOL nCol;
	CGXStyle* pStyle;
};

class CGXDataUndoMoveCellsInfo: public CObject
{
	DECLARE_DYNAMIC(CGXDataUndoMoveCellsInfo);

public:
	CGXDataUndoMoveCellsInfo()
		{ m_pCells = NULL; }

	~CGXDataUndoMoveCellsInfo()
	{     
		CGXStylePtrList* next_cell = NULL;
		for (CGXStylePtrList* cp = m_pCells; cp != NULL; cp = next_cell)
		{
			next_cell = cp->next;
			delete cp->pStyle;
			delete cp;
		}
		m_pCells = NULL; 
	}

	CGXStylePtrList* m_pCells;
};

// move ranges in one step - CGXGridCore expects CGXFormulaSheet
// to create efficient undo info and store it in pUndoInfo

IMPLEMENT_DYNAMIC(CGXDataUndoMoveCellsInfo, CObject);

BOOL CGXDataCopyMoveCellsImp::StoreMoveCells(CGXData* pData, CGXRange& rg, ROWCOL nRow, ROWCOL nCol, CObject*& pUndoInfo, BOOL bCreateUndoInfo, int ctCmd)
{
	// Unused:
	ctCmd;

	CGXStylePtrList* pPreserveList = NULL;
	CGXStylePtrList* pMoveList = NULL;

	for (ROWCOL nSrcRow = rg.top; nSrcRow <= rg.bottom; nSrcRow++)
	{
		// Source row
		int srcrow = (int) nSrcRow;
		CGXStylePtrArray* paSrcCells; 
		if (srcrow <= pData->m_apapCellStyle.GetUpperBound())
			paSrcCells = pData->m_apapCellStyle.GetAt(srcrow);
		else
			paSrcCells = NULL;

		for (ROWCOL nSrcCol = rg.left; nSrcCol <= rg.right; nSrcCol++)
		{
			// Source Style
			int col = (int) nSrcCol;
			CGXStyle *pSrcStyle;
			if (paSrcCells && col <= paSrcCells->GetUpperBound())
				pSrcStyle = paSrcCells->GetAt(col);
			else
				pSrcStyle = NULL;

			// move style
			if (pSrcStyle)
			{
				ROWCOL nDestRow = nRow + (nSrcRow - rg.top);
				ROWCOL nDestCol = nCol + (nSrcCol - rg.left);

				// Preserve style info with row and col
				CGXStylePtrList* cp = new CGXStylePtrList;
				cp->nRow = nDestRow;
				cp->nCol = nDestCol;
				cp->pStyle = pSrcStyle;
				cp->next = pMoveList;
				pMoveList = cp;
				paSrcCells->SetAt(col, NULL);
			}
		}
	}

	for (ROWCOL nSrcRow = rg.top; nSrcRow <= rg.bottom; nSrcRow++)
	{
		// Dest Row
		ROWCOL nDestRow = nRow + (nSrcRow - rg.top);
		int destrow = (int) nDestRow;
		CGXStylePtrArray* paDestCells = NULL; 
		if (destrow <= pData->m_apapCellStyle.GetUpperBound())
			paDestCells = pData->m_apapCellStyle.GetAt(destrow);
		else
		{
			// increase array size
			paDestCells = NULL;
			pData->m_apapCellStyle.SetSize((int) pData->m_nRows+1);
		}

		// check if row must be created
		if (paDestCells == NULL)
		{
			paDestCells = new CGXStylePtrArray;
			pData->m_apapCellStyle.SetAtGrow(destrow, paDestCells);
		}

		for (ROWCOL nSrcCol = rg.left; nSrcCol <= rg.right; nSrcCol++)
		{
			// Dest Style
			ROWCOL nDestCol = nCol + (nSrcCol - rg.left);
			int destcol = (int) nDestCol;
			CGXStyle *pDestStyle;

			// delete target style (or add to undo list)
			if (paDestCells && destcol <= paDestCells->GetUpperBound())
			{
				pDestStyle = paDestCells->GetAt(destcol);

				if (bCreateUndoInfo && pDestStyle)
				{
					// Preserve style info with row and col
					CGXStylePtrList* cp = new CGXStylePtrList;
					cp->nRow = nDestRow;
					cp->nCol = nDestCol;
					cp->pStyle = pDestStyle;
				    cp->next = pPreserveList;
					pPreserveList = cp;
				}
				else
					delete pDestStyle;

				paDestCells->SetAt(destcol, NULL);
			}
			pDestStyle = NULL;
		}
	}

	// Move Cells from move_list
	CGXStylePtrList* next_cell = NULL;
	for (CGXStylePtrList* cp = pMoveList; cp != NULL; cp = next_cell)
	{
		next_cell = cp->next;

		int row = (int) cp->nRow;
		int col = (int) cp->nCol;

		// get cell-array for row
		CGXStylePtrArray* paCells = NULL;
		if (row <= pData->m_apapCellStyle.GetUpperBound())
			paCells = pData->m_apapCellStyle.GetAt(row);
		else
			// increase array size
			pData->m_apapCellStyle.SetSize((int) pData->m_nRows+1);

		// check if row must be created
		if (paCells == NULL)
		{
			paCells = new CGXStylePtrArray;
			pData->m_apapCellStyle.SetAtGrow(row, paCells);
		}		 

		if (paCells && col > paCells->GetUpperBound())
			paCells->SetSize((int) pData->m_nCols+1);

		paCells->SetAt(col, cp->pStyle);
		cp->pStyle = NULL;

		delete cp;
	}

	CGXDataUndoMoveCellsInfo* pInfo = NULL;
	if (pUndoInfo && pUndoInfo->IsKindOf(RUNTIME_CLASS(CGXDataUndoMoveCellsInfo)))
		pInfo = (CGXDataUndoMoveCellsInfo*) pUndoInfo;

	if (pInfo)
	{
		CGXStylePtrList* next_cell = NULL;
	    for (CGXStylePtrList* cp = pInfo->m_pCells; cp != NULL; cp = next_cell)
		{
			next_cell = cp->next;

			int row = (int) cp->nRow;
			int col = (int) cp->nCol;

			// get cell-array for row
			CGXStylePtrArray* paCells = NULL;
			if (row <= pData->m_apapCellStyle.GetUpperBound())
				paCells = pData->m_apapCellStyle.GetAt(row);
			else
				// increase array size
				pData->m_apapCellStyle.SetSize((int) pData->m_nRows+1);

			// check if row must be created
			if (paCells == NULL)
			{
				paCells = new CGXStylePtrArray;
				pData->m_apapCellStyle.SetAtGrow(row, paCells);
			}		 

			if (paCells && col > paCells->GetUpperBound())
				paCells->SetSize((int) pData->m_nCols+1);

			paCells->SetAtGrow(col, cp->pStyle);
			cp->pStyle = NULL;

			delete cp;
		}

		pInfo->m_pCells = NULL;
	}

	if (bCreateUndoInfo)
	{
		if (pUndoInfo == NULL)
			pUndoInfo = pInfo = new CGXDataUndoMoveCellsInfo;

		pInfo->m_pCells = pPreserveList;
	}


	return TRUE;
}
