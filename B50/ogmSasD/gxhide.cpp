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

// gxhide.cpp : Hiding rows and/or columns
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

#ifndef _GXSTYLES_H_
#include "gxstyles.h"
#endif

#ifndef _GXDLL_H_
#include "gxdll.h"
#endif

#ifndef _GXPRIV_H_
#include "gxpriv.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCORSIZ")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

class CGXGridHideCellsImp: public CGXAbstractGridHideCellsImp
{
public:
	virtual void RemoveRows(CGXGridCore* pGrid, ROWCOL nFromRow, ROWCOL nToRow);
	virtual void RemoveCols(CGXGridCore* pGrid, ROWCOL nFromCol, ROWCOL nToCol);
	virtual void InsertRows(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCount);
	virtual void InsertCols(CGXGridCore* pGrid, ROWCOL nCol, ROWCOL nCount);
	virtual void MoveRows(CGXGridCore* pGrid, ROWCOL nFromRow, ROWCOL nToRow, ROWCOL nDestRow);
	virtual void MoveCols(CGXGridCore* pGrid, ROWCOL nFromCol, ROWCOL nToCol, ROWCOL nDestCol);
	virtual BOOL HideRows(CGXGridCore* pGrid, ROWCOL nFromRow, ROWCOL nToRow, BOOL bHide, BOOL* abHideArray, UINT flags, GXCmdType ctCmd);
	virtual BOOL IsRowHidden(CGXGridCore* pGrid, ROWCOL nRow);
	virtual BOOL StoreHideRow(CGXGridCore* pGrid, ROWCOL nRow, BOOL bHide);
	virtual BOOL HideCols(CGXGridCore* pGrid, ROWCOL nFromCol, ROWCOL nToCol, BOOL bHide, BOOL* abHideArray, UINT flags, GXCmdType ctCmd);
	virtual BOOL IsColHidden(CGXGridCore* pGrid, ROWCOL nCol);
	virtual BOOL StoreHideCol(CGXGridCore* pGrid, ROWCOL nCol, BOOL bHide);
};

void CGXGridCore::ImplementHideCells()
{
	if (m_pHideCellsImp == NULL)
		AutoDeletePtr(m_pHideCellsImp = new CGXGridHideCellsImp);
}

/////////////////////////////////////////////////////////////////////////////
// CGXGridHideCellsImp

void CGXGridHideCellsImp::RemoveRows(CGXGridCore* pGrid, ROWCOL nFromRow, ROWCOL nToRow)
{
	CGXBitArray* pArray = (CGXBitArray*) &pGrid->m_pParam->m_abRowHidden;
	pArray->RemoveBits(nFromRow, nToRow);
}

void CGXGridHideCellsImp::RemoveCols(CGXGridCore* pGrid, ROWCOL nFromCol, ROWCOL nToCol)
{
	CGXBitArray* pArray = (CGXBitArray*) &pGrid->m_pParam->m_abColHidden;
	pArray->RemoveBits(nFromCol, nToCol);
}

void CGXGridHideCellsImp::InsertRows(CGXGridCore* pGrid, ROWCOL nRow, ROWCOL nCount)
{
	CGXBitArray* pArray = (CGXBitArray*) &pGrid->m_pParam->m_abRowHidden;
	pArray->InsertBits(nRow, nCount);
}


void CGXGridHideCellsImp::InsertCols(CGXGridCore* pGrid, ROWCOL nCol, ROWCOL nCount)
{
	CGXBitArray* pArray = (CGXBitArray*) &pGrid->m_pParam->m_abColHidden;
	pArray->InsertBits(nCol, nCount);
}

void CGXGridHideCellsImp::MoveRows(CGXGridCore* pGrid, ROWCOL nFromRow, ROWCOL nToRow, ROWCOL nDestRow)
{
	CGXBitArray* pArray = (CGXBitArray*) &pGrid->m_pParam->m_abRowHidden;
	pArray->MoveBits(nFromRow, nToRow, nDestRow);
}

void CGXGridHideCellsImp::MoveCols(CGXGridCore* pGrid, ROWCOL nFromCol, ROWCOL nToCol, ROWCOL nDestCol)
{
	CGXBitArray* pArray = (CGXBitArray*) &pGrid->m_pParam->m_abColHidden;
	pArray->MoveBits(nFromCol, nToCol, nDestCol);
}

BOOL CGXGridHideCellsImp::HideRows(CGXGridCore* pGrid, ROWCOL nFromRow, ROWCOL nToRow, BOOL bHide, BOOL* abHideArray, UINT flags, GXCmdType ctCmd)
{
	if (nToRow == 0)
		nToRow = nFromRow;

	ASSERT(nToRow >= nFromRow);

#ifdef _DEBUG
	if (abHideArray)
		ASSERT(AfxIsValidAddress(abHideArray, (UINT) (sizeof(BOOL)*(nToRow-nFromRow+1))));
	// ASSERTION-> abHideArray is not valid, perhaps too small? ->END
#endif

	CGXLongOperation theOp;

	int* anOldHeights = new int[nToRow-nFromRow+1];
	pGrid->GetArrayWithRowHeights(nFromRow, nToRow, anOldHeights);

	BOOL* abOldHiddenState = new BOOL[nToRow-nFromRow+1];

	BOOL b = FALSE;
	for (ROWCOL nRow = nFromRow; nRow <= nToRow; nRow++)
	{
		abOldHiddenState[nRow-nFromRow] = pGrid->IsRowHidden(nRow);
		if (abHideArray)
			b |= pGrid->StoreHideRow(nRow, abHideArray[nRow-nFromRow]);
		else
			b |= pGrid->StoreHideRow(nRow, bHide);

		theOp.DoMessages();
	}

	if (b)
	{
		pGrid->DelayMergeCells(CGXRange().SetRows(nFromRow, nToRow));
		pGrid->DelayFloatCells(CGXRange().SetRows(nFromRow, nToRow));

		pGrid->UpdateChangedRowHeights(nFromRow, nToRow, anOldHeights, flags, TRUE);

		if (pGrid->m_pUndoImp && ctCmd != gxRollback && pGrid->m_pParam->m_bUndoEnabled)
		{
			pGrid->AddCommand(pGrid->m_pUndoImp->CreateHideRowsCmd(nFromRow, nToRow, abOldHiddenState), ctCmd);

			if (ctCmd == gxRedo || ctCmd == gxUndo)
				pGrid->OnCellUndone(nFromRow, pGrid->GetLeftCol());
		}
		else
			delete abOldHiddenState;

		delete anOldHeights;
		return TRUE;
	}

	// clean up undo info
	delete abOldHiddenState;
	delete anOldHeights;

	return FALSE;
}

BOOL CGXGridHideCellsImp::IsRowHidden(CGXGridCore* pGrid, ROWCOL nRow)
{
	// lower pane in dynamic splitter view
	if (pGrid->m_pSplitterWnd != NULL && !pGrid->IsPrinting() && !pGrid->m_bDisplayHeaderRow && nRow <= pGrid->GetHeaderRows())
		return TRUE;

	// check if "Row Headers" are enabled for printing
	if (nRow == 0 && pGrid->IsPrinting() && !pGrid->m_pParam->m_pProperties->GetDisplayColHeaders())
		return TRUE;

	// otherwise, let's see what is stored in the CGXBitArray
	return ((CGXBitArray*)&pGrid->m_pParam->m_abRowHidden)->GetBitAt(nRow);
}

BOOL CGXGridHideCellsImp::StoreHideRow(CGXGridCore* pGrid, ROWCOL nRow, BOOL bHide)
{
	return ((CGXBitArray*)&pGrid->m_pParam->m_abRowHidden)->SetBitAt(nRow, bHide);
}

BOOL CGXGridHideCellsImp::HideCols(CGXGridCore* pGrid, ROWCOL nFromCol, ROWCOL nToCol, BOOL bHide, BOOL* abHideArray, UINT flags, GXCmdType ctCmd)
{
	if (nToCol == 0)
		nToCol = nFromCol;

	ASSERT(nToCol >= nFromCol);

#ifdef _DEBUG
	if (abHideArray)
		ASSERT(AfxIsValidAddress(abHideArray, (UINT) (sizeof(BOOL)*(nToCol-nFromCol+1))));
	// ASSERTION-> abHideArray is not valid, perhaps too small? ->END
#endif

	CGXLongOperation theOp;

	int* anOldWidths = new int[nToCol-nFromCol+1];
	pGrid->GetArrayWithColWidths(nFromCol, nToCol, anOldWidths);

	BOOL* abOldHiddenState = new BOOL[nToCol-nFromCol+1];

	BOOL b = FALSE;
	for (ROWCOL nCol = nFromCol; nCol <= nToCol; nCol++)
	{
		abOldHiddenState[nCol-nFromCol] = pGrid->IsColHidden(nCol);
		if (abHideArray)
			b |= pGrid->StoreHideCol(nCol, abHideArray[nCol-nFromCol]);
		else
			b |= pGrid->StoreHideCol(nCol, bHide);

		theOp.DoMessages();
	}

	if (b)
	{
		pGrid->DelayMergeCells(CGXRange().SetCols(nFromCol, nToCol));
		pGrid->DelayFloatCells(CGXRange().SetCols(nFromCol, nToCol));

		pGrid->UpdateChangedColWidths(nFromCol, nToCol, anOldWidths, flags, TRUE);

		if (pGrid->m_pUndoImp && ctCmd != gxRollback && pGrid->m_pParam->m_bUndoEnabled)
		{
			pGrid->AddCommand(pGrid->m_pUndoImp->CreateHideColsCmd(nFromCol, nToCol, abOldHiddenState), ctCmd);

			if (ctCmd == gxRedo || ctCmd == gxUndo)
				pGrid->OnCellUndone(nFromCol, pGrid->GetLeftCol());
		}
		else
			delete abOldHiddenState;

		delete anOldWidths;
		return TRUE;
	}

	// clean up undo info
	delete abOldHiddenState;
	delete anOldWidths;

	return FALSE;
}

BOOL CGXGridHideCellsImp::IsColHidden(CGXGridCore* pGrid, ROWCOL nCol)
{
	// right pane in dynamic splitter view
	if (pGrid->m_pSplitterWnd && !pGrid->IsPrinting() && !pGrid->m_bDisplayHeaderCol && nCol <= pGrid->GetHeaderCols())
		return TRUE;

	// check if "Column Headers" are enabled for printing
	if (nCol == 0 && pGrid->IsPrinting() && !pGrid->m_pParam->m_pProperties->GetDisplayRowHeaders())
		return TRUE;

	// otherwise, let's see what is stored in the CGXBitArray
	return ((CGXBitArray*)&pGrid->m_pParam->m_abColHidden)->GetBitAt(nCol);
}

BOOL CGXGridHideCellsImp::StoreHideCol(CGXGridCore* pGrid, ROWCOL nCol, BOOL bHide)
{
	return ((CGXBitArray*)&pGrid->m_pParam->m_abColHidden)->SetBitAt(nCol, bHide);
}



/////////////////////////////////////////////////////////////////////////////
// CGXBitArray

BOOL CGXBitArray::SetBitAt(DWORD dwIndex, BOOL bValue)
{
	DWORD n = dwIndex/8;

	if (n >= (DWORD) INT_MAX)
		return FALSE;

	int nIndex = (int) n;

	BYTE b = 0;
	if (nIndex < GetSize())
		b =  GetAt(nIndex);
	else
	{
		int nOldSize = GetSize();
		SetSize(nIndex+1);
		while (nOldSize <= nIndex)
			SetAt(nOldSize++, 0);
	}

	// 1 << n == 1*2^n
	if (bValue)
		b |= 1<<(dwIndex%8);
	else
		b &= ~(1<<(dwIndex%8));

	SetAt(nIndex, b);

	return TRUE;
}

BOOL CGXBitArray::GetBitAt(DWORD dwIndex)
{
	DWORD n = dwIndex/8;

	if (n >= (DWORD) INT_MAX)
		return FALSE;

	int nIndex = (int) n;

	// 1 << n == 1*2^n
	if (nIndex < GetSize())
		return (GetAt(nIndex) & (1<<(dwIndex%8))) > 0;

	return FALSE;
}

void CGXBitArray::RemoveBits(DWORD dwFrom, DWORD dwTo)
{
	ASSERT(dwTo >= dwFrom);

	int nFrom = (int) min(dwFrom/8, (DWORD) INT_MAX);
	int nTo = (int) min((dwTo+1)/8, (DWORD) INT_MAX);

	// nothing to do?
	if (nFrom >= GetSize())
		return;

	// 1 << n == 1*2^n

	// << means shift to high bit
	// >> means shift to lower bit

	BYTE b1 = GetAt(nFrom);
	BYTE b2 = nTo >= GetSize() ? (BYTE) 0 : GetAt(nTo);

	int n1 = (int) (dwFrom%8);
	int n2 = (int) ((dwTo+1)%8);
	int nShift = (int) ((dwTo-dwFrom+1)%8);

	// Bits left from range to be removed
		// shift out higher bits
	BYTE bLeftBits = (BYTE) ((BYTE) (b1 << (8-n1)) >> (8-n1));

	// Bits right from range to be removed (in the nTo byte)
		// shift out lower bits and then shift them to the new position
	BYTE bRightBits = (BYTE) ((BYTE) (b2 >> n2) << n1);

	// Bits from next byte which will shift into first byte
	BYTE bNextBits = nTo+1 >= GetSize() ? (BYTE) 0 : GetAt(nTo+1);

	BYTE bShiftInto = 0;
	if (n2 > n1)
	{
			// shift out higher bits
		bShiftInto = (BYTE) (bNextBits << (8-nShift));
		nTo++;
	}
	if(n2 == n1)
		nTo++;

	// Now, combine this bits together
	BYTE bFirstByte = (BYTE) (bLeftBits | bRightBits | bShiftInto);
	SetAt(nFrom, bFirstByte);

	// Now, loop through next bytes
	while (nTo < GetSize() && ++nFrom < GetSize())
	{
		// Get last bits from first byte and shift left
		bLeftBits = GetAt(nTo++);
			// shift out lower bits (higher bits (n2-n1) to 8 will become 0)

		// Get first bits from next byte and shift right
		bRightBits = nTo >= GetSize() ? (BYTE) 0 : GetAt(nTo);
			// shift out higher bits  (lower bits 0 - (n2-n1) will become 0)

		// Now, combine this bits together
		BYTE bCombinedByte = (BYTE) (bLeftBits >> nShift | bRightBits << (8-nShift));

		SetAt(nFrom, bCombinedByte);
	}

	// Remove obsolete bytes
	SetSize(nFrom+1);
}

void CGXBitArray::MoveBits(DWORD dwFrom, DWORD dwTo, DWORD dwDest)
{
	DWORD dwCount = dwTo-dwFrom+1;

	if (dwDest == dwFrom || dwCount == 0)
		return;

	if (dwDest < dwFrom)
		dwFrom += dwCount;
	else if (dwDest > dwFrom)
		dwDest += dwCount;

	// Insert bits at destination
	InsertBits(dwDest, dwCount);

	// Copy bits to be moved
	for (DWORD dw = 0; dwFrom+dw <= (DWORD) GetUpperBound()*8 && dw < dwCount; dw++)
	{
		if (GetBitAt(dwFrom+dw))
			SetBitAt(dwDest+dw, GetBitAt(dwFrom+dw));
	}

	// Remove bits from original position
	if (dwFrom <= (DWORD) GetUpperBound()*8)
		RemoveBits(dwFrom, min(dwFrom+dwCount-1, (DWORD) GetUpperBound()*8));
}

void CGXBitArray::InsertBits(DWORD dwFrom, DWORD dwCount)
{
	int nFrom = (int) min(dwFrom/8, (DWORD) INT_MAX);
	int nTo = (int) min((dwFrom+dwCount)/8, (DWORD) INT_MAX);

	// nothing to do?
	if (nFrom >= GetSize())
		return;

	// 1 << n == 1*2^n

	// << means shift to high bit
	// >> means shift to lower bit

	BYTE b1 = GetAt(nFrom);
	BYTE b2 = nTo >= GetSize() ? (BYTE) 0 : GetAt(nTo);

	int n1 = (int) (dwFrom%8);
	int n2 = (int) ((dwFrom+dwCount)%8);
	int nShift = (int) (dwCount%8);

	int nBytes = (int) ((dwCount-1)/8+1);
	SetSize(GetSize() + nBytes);

	// Shift and combine bits which move to the right
	int nDest;
	for (nDest = GetSize()-1; nDest > nTo; nDest--)
	{
		BYTE bRightBits = GetAt(nDest-nBytes+1);
		BYTE bLeftBits = GetAt(nDest-nBytes);

		BYTE bCombinedByte = (BYTE) (bLeftBits >> (8-nShift) | bRightBits << nShift);

		SetAt(nDest, bCombinedByte);
	}

	// Bits left from range
		// shift out higher bits
	BYTE bLeftBits = (BYTE) ((BYTE) (b1 << (8-n1)) >> (8-n1));

	// Special case for the first one or two bytes
	if (nFrom == nTo)
	{
		// shift out lower bits
		BYTE bRightBits = (BYTE) ((BYTE) (b1 >> n1) << n1);
		SetAt(nFrom, (BYTE) (bLeftBits | bRightBits << nShift));
	}
	else
	{
		// First Byte
		SetAt(nFrom, bLeftBits);

		// Next Byte
		BYTE bLeftBits = (BYTE) ((BYTE) (b1 >> n1) << n2);
		BYTE bRightBits = (BYTE) (b2 << nShift);
		SetAt(nTo, (BYTE) (bLeftBits | bRightBits));
	}

	// Empty all inserted bytes
	for (int nIndex = nFrom+1; nIndex < nDest; nIndex++)
		SetAt(nIndex, 0);
}



