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

// gxcoll.cpp : implementation of collection classes
//

#include "stdafx.h"

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

// Headers

#ifndef _GXCOLL_H_
#include "gxcoll.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCOLL")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CGXCollMapDWordToLong, CObject, 0)
IMPLEMENT_SERIAL(CMapNdxToOb, CObject, 0)
IMPLEMENT_DYNAMIC(CGXMapFontData, CObject)
IMPLEMENT_DYNAMIC(CGXMapByte8ToPtr, CObject)
IMPLEMENT_DYNAMIC(CGXPtrArrayPtrArray, CPtrArray)

IMPLEMENT_SERIAL(CGXMapDWordToLong, CGXCollMapDWordToLong, 0)
IMPLEMENT_SERIAL(CGXMapCellToOb, CMapNdxToOb, 0)

/////////////////////////////////////////////////////////////////////////////

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CGXMapDWordToLong

void CGXMapDWordToLong::Insert(DWORD nId, DWORD nCount)
{
	LONG lVal;
	DWORD dw;

	CGXCollMapDWordToLong*
		newmap = new CGXCollMapDWordToLong;

	POSITION pos = GetStartPosition();
	while (pos)
	{
		GetNextAssoc(pos, dw, lVal);
		if (dw < nId)
			newmap->SetAt(dw, lVal);
		else
			newmap->SetAt(dw+nCount, lVal);
	}

	RemoveAll();

	pos = newmap->GetStartPosition();
	while (pos)
	{
		newmap->GetNextAssoc(pos, dw, lVal);
		SetAt(dw, lVal);
	}

	delete newmap;
}

void CGXMapDWordToLong::Remove(DWORD nFrom, DWORD nTo)
{
	ASSERT(nTo >= nFrom);
	DWORD nCount = nTo-nFrom+1;

	LONG lVal;
	DWORD dw;

	CGXCollMapDWordToLong*
		newmap = new CGXCollMapDWordToLong;

	POSITION pos = GetStartPosition();
	while (pos)
	{
		GetNextAssoc(pos, dw, lVal);
		if (dw < nFrom)
			newmap->SetAt(dw, lVal);
		else if (dw > nTo)
			newmap->SetAt(dw-nCount, lVal);
	}

	RemoveAll();

	pos = newmap->GetStartPosition();
	while (pos)
	{
		newmap->GetNextAssoc(pos, dw, lVal);
		SetAt(dw, lVal);
	}

	delete newmap;
}

void CGXMapDWordToLong::Move(DWORD nFrom, DWORD nTo, DWORD nDest)
{
	ASSERT(nTo >= nFrom);
	DWORD nCount = nTo-nFrom+1;

	if (nDest > nFrom)
		nDest += nCount;

	LONG lVal;
	DWORD dw;

	CGXCollMapDWordToLong*
		newmap = new CGXCollMapDWordToLong;

	POSITION pos = GetStartPosition();
	while (pos)
	{
		GetNextAssoc(pos, dw, lVal);
		if (dw < nDest && dw < nFrom
			|| dw >= nDest && dw > nTo)
			newmap->SetAt(dw, lVal);
		else if (dw >= nFrom && dw <= nTo)
		{
			if (nDest < nFrom)
				newmap->SetAt(dw - nFrom + nDest, lVal);
			else
				newmap->SetAt(dw - nFrom + nDest - nCount, lVal);
		}
		else if (dw >= nDest && dw < nFrom)
			newmap->SetAt(dw + nCount, lVal);
		else if (dw < nDest && dw > nTo)
			newmap->SetAt(dw - nCount, lVal);
		else
			ASSERT(0);

	}

	RemoveAll();

	pos = newmap->GetStartPosition();
	while (pos)
	{
		newmap->GetNextAssoc(pos, dw, lVal);
		SetAt(dw, lVal);
	}

	delete newmap;
}


CGXMapDWordToLong& CGXMapDWordToLong::operator=(const CGXMapDWordToLong& map)
{
	if (&map == this)
		return *this;

	RemoveAll();

	LONG lVal;
	DWORD dw;

	POSITION pos = map.GetStartPosition();
	while (pos)
	{
		map.GetNextAssoc(pos, dw, lVal);
		SetAt(dw, lVal);
	}

	return *this;
}


/////////////////////////////////////////////////////////////////////////////
// CGXMapCellToOb

void CGXMapCellToOb::InsertRows(DWORD nRow, DWORD nCount)
{
	GXNDX ndx;
	CObject* pObject;

	CMapNdxToOb*
		newmap = new CMapNdxToOb;

	POSITION pos = GetStartPosition();
	while (pos)
	{
		GetNextAssoc(pos, ndx, pObject);
		if (ndx.nRow < nRow)
			newmap->SetAt(ndx, pObject);
		else
			newmap->SetAt(GXNDX(ndx.nRow+nCount, ndx.nCol), pObject);
	}

	RemoveAll();

	pos = newmap->GetStartPosition();
	while (pos)
	{
		newmap->GetNextAssoc(pos, ndx, pObject);
		SetAt(ndx, pObject);
	}

	delete newmap;
}

void CGXMapCellToOb::InsertCols(DWORD nCol, DWORD nCount)
{
	// ASSERT(nCol+nCount < USHRT_MAX);

	GXNDX    ndx;
	CObject* pObject;

	CMapNdxToOb*
		newmap = new CMapNdxToOb;

	POSITION pos = GetStartPosition();
	while (pos)
	{
		GetNextAssoc(pos, ndx, pObject);
		if (ndx.nCol < nCol)
			newmap->SetAt(ndx, pObject);
		else
			newmap->SetAt(GXNDX(ndx.nRow, ndx.nCol+nCount), pObject);
	}

	RemoveAll();

	pos = newmap->GetStartPosition();
	while (pos)
	{
		newmap->GetNextAssoc(pos, ndx, pObject);
		SetAt(ndx, pObject);
	}

	delete newmap;
}

void CGXMapCellToOb::RemoveRows(DWORD nFromRow, DWORD nToRow)
{
	ASSERT(nToRow >= nFromRow);
	// ASSERT(nToRow < USHRT_MAX);

	GXNDX    ndx;
	CObject* pObject;
	DWORD nCount = nToRow-nFromRow+1;

	CMapNdxToOb*
		newmap = new CMapNdxToOb;

	POSITION pos = GetStartPosition();
	while (pos)
	{
		GetNextAssoc(pos, ndx, pObject);
		if (ndx.nRow < nFromRow)
			newmap->SetAt(ndx, pObject);
		else if (ndx.nRow > nToRow)
			newmap->SetAt(GXNDX(ndx.nRow-nCount, ndx.nCol), pObject);
		else
			delete pObject;
	}

	RemoveAll();

	pos = newmap->GetStartPosition();
	while (pos)
	{
		newmap->GetNextAssoc(pos, ndx, pObject);
		SetAt(ndx, pObject);
	}

	delete newmap;
}


void CGXMapCellToOb::RemoveCols(DWORD nFromCol, DWORD nToCol)
{
	ASSERT(nToCol >= nFromCol);

	GXNDX ndx;
	CObject* pObject;
	DWORD nCount = nToCol-nFromCol+1;

	CMapNdxToOb*
		newmap = new CMapNdxToOb;

	POSITION pos = GetStartPosition();
	while (pos)
	{
		GetNextAssoc(pos, ndx, pObject);
		if (ndx.nCol < nFromCol)
			newmap->SetAt(ndx, pObject);
		else if (ndx.nCol > nToCol)
			newmap->SetAt(GXNDX(ndx.nRow, ndx.nCol-nCount), pObject);
		else
			delete pObject;
	}

	RemoveAll();

	pos = newmap->GetStartPosition();
	while (pos)
	{
		newmap->GetNextAssoc(pos, ndx, pObject);
		SetAt(ndx, pObject);
	}

	delete newmap;
}

void CGXMapCellToOb::MoveRows(DWORD nFromRow, DWORD nToRow, DWORD nDestRow)
{
	ASSERT(nToRow >= nFromRow);
	ASSERT(nDestRow > nToRow || nDestRow <= nFromRow);

	GXNDX    ndx;
	CObject* pObject;
	DWORD nCount = nToRow-nFromRow+1;

	CMapNdxToOb*
		newmap = new CMapNdxToOb;

	POSITION pos = GetStartPosition();
	while (pos)
	{
		GetNextAssoc(pos, ndx, pObject);
		DWORD nRow = ndx.nRow;
		if (nRow < nDestRow && nRow < nFromRow
			|| nRow >= nDestRow && nRow > nToRow)
			newmap->SetAt(ndx, pObject);
		else if (nRow >= nFromRow && nRow <= nToRow)
		{
			if (nDestRow < nFromRow)
				newmap->SetAt(GXNDX(nRow - nFromRow + nDestRow, ndx.nCol), pObject);
			else
				newmap->SetAt(GXNDX(nRow - nFromRow + nDestRow - nCount, ndx.nCol), pObject);
		}
		else if (nRow >= nDestRow && nRow < nFromRow)
			newmap->SetAt(GXNDX(nRow + nCount, ndx.nCol), pObject);
		else if (nRow < nDestRow && nRow > nToRow)
			newmap->SetAt(GXNDX(nRow - nCount, ndx.nCol), pObject);
		else
			ASSERT(0);
	}

	RemoveAll();

	pos = newmap->GetStartPosition();
	while (pos)
	{
		newmap->GetNextAssoc(pos, ndx, pObject);
		SetAt(ndx, pObject);
	}

	delete newmap;
}


void CGXMapCellToOb::MoveCols(DWORD nFromCol, DWORD nToCol, DWORD nDestCol)
{
	ASSERT(nToCol >= nFromCol);
	ASSERT(nDestCol > nToCol || nDestCol <= nFromCol);

	GXNDX   ndx;
	CObject* pObject;
	DWORD nCount = nToCol-nFromCol+1;

	CMapNdxToOb*
		newmap = new CMapNdxToOb;

	POSITION pos = GetStartPosition();
	while (pos)
	{
		GetNextAssoc(pos, ndx, pObject);
		DWORD nCol = ndx.nCol;
		if (nCol < nDestCol && nCol < nFromCol
			|| nCol >= nDestCol && nCol > nToCol)
			newmap->SetAt(ndx, pObject);
		else if (nCol >= nFromCol && nCol <= nToCol)
		{
			if (nDestCol < nFromCol)
				newmap->SetAt(GXNDX(ndx.nRow, nCol - nFromCol + nDestCol), pObject);
			else
				newmap->SetAt(GXNDX(ndx.nRow, nCol - nFromCol + nDestCol - nCount), pObject);
		}
		else if (nCol >= nDestCol && nCol < nFromCol)
			newmap->SetAt(GXNDX(ndx.nRow, nCol + nCount), pObject);
		else if (nCol < nDestCol && nCol > nToCol)
			newmap->SetAt(GXNDX(ndx.nRow, nCol - nCount), pObject);
		else
			ASSERT(0);
	}

	RemoveAll();

	pos = newmap->GetStartPosition();
	while (pos)
	{
		newmap->GetNextAssoc(pos, ndx, pObject);
		SetAt(ndx, pObject);
	}

	delete newmap;
}

/////////////////////////////////////////////////////////////////////////////
// classes created with templdef tool


// Workaround for afxEmptyString
// Using afxEmptyString generates a GPF in the _AFXDLL version
#if _MFC_VER < 0x0300
static const CString NEAR gxEmptyString;
#define afxEmptyString gxEmptyString
#endif
static const CString NEAR gxEmptyString;
#define afxEmptyString gxEmptyString

CGXPlex* PASCAL CGXPlex::Create(CGXPlex*& pHead, UINT nMax, UINT cbElement)
{
	ASSERT(nMax > 0 && cbElement > 0);
	CGXPlex* p = (CGXPlex*) new BYTE[sizeof(CGXPlex) + nMax * cbElement];
			// may throw exception
	p->nMax = nMax;
	p->nCur = 0;
	p->pNext = pHead;
	pHead = p;  // change head (adds in reverse order for simplicity)
	return p;
}

void CGXPlex::FreeDataChain()     // free this one and links
{
	CGXPlex* p = this;
	while (p != NULL)
	{
		BYTE* bytes = (BYTE*) p;
		CGXPlex* pNext = p->pNext;
		delete bytes;
		p = pNext;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Special implementations for CStrings
// it is faster to bit-wise copy a CString than to call an official
//   constructor - since an empty CString can be bit-wise copied

static inline void ConstructElement(CString* pNewData)
{
	memcpy(pNewData, &afxEmptyString, sizeof(CString));
}

static inline void DestructElement(CString* pOldData)
{
	pOldData->~CString();
}

static inline void CopyElement(CString* pSrc, CString* pDest)
{
	*pSrc = *pDest;
}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

CGXCollMapDWordToLong::CGXCollMapDWordToLong(int nBlockSize)
{
	ASSERT(nBlockSize > 0);

	m_pHashTable = NULL;
	m_nHashTableSize = 17;  // default size
	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

inline UINT CGXCollMapDWordToLong::HashKey(DWORD key) const
{
	// default identity hash - works for most primitive values
#ifdef _WIN32
	return ((UINT)(void*)(DWORD)key) >> 4;
#else
	return _AFX_FP_OFF(key) >> 4;
#endif
}


void CGXCollMapDWordToLong::InitHashTable(UINT nHashSize)
//
// Used to force allocation of a hash table or to override the default
//   hash table size of (which is fairly small)
{
	ASSERT_VALID(this);
	ASSERT(m_nCount == 0);
	ASSERT(nHashSize > 0);

	// free hash table
	delete[] m_pHashTable;
	m_pHashTable = NULL;

	m_pHashTable = new CAssoc* [nHashSize];
	memset(m_pHashTable, 0, sizeof(CAssoc*) * nHashSize);
	m_nHashTableSize = nHashSize;
}

void CGXCollMapDWordToLong::RemoveAll()
{
	ASSERT_VALID(this);



	// free hash table
	delete[] m_pHashTable;
	m_pHashTable = NULL;

	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

CGXCollMapDWordToLong::~CGXCollMapDWordToLong()
{
	RemoveAll();
	ASSERT(m_nCount == 0);
}

/////////////////////////////////////////////////////////////////////////////
// Assoc helpers
// same as CList implementation except we store CAssoc's not CNode's
//    and CAssoc's are singly linked all the time

CGXCollMapDWordToLong::CAssoc* CGXCollMapDWordToLong::NewAssoc()
{
	if (m_pFreeList == NULL)
	{
		// add another block
		CGXPlex* newBlock = CGXPlex::Create(m_pBlocks, m_nBlockSize, sizeof(CGXCollMapDWordToLong::CAssoc));
		// chain them into free list
		CGXCollMapDWordToLong::CAssoc* pAssoc = (CGXCollMapDWordToLong::CAssoc*) newBlock->data();
		// free in reverse order to make it easier to debug
		pAssoc += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pAssoc--)
		{
			pAssoc->pNext = m_pFreeList;
			m_pFreeList = pAssoc;
		}
	}
	ASSERT(m_pFreeList != NULL);  // we must have something

	CGXCollMapDWordToLong::CAssoc* pAssoc = m_pFreeList;
	m_pFreeList = m_pFreeList->pNext;
	m_nCount++;
	ASSERT(m_nCount > 0);  // make sure we don't overflow
	memset(&pAssoc->key, 0, sizeof(DWORD));

	memset(&pAssoc->value, 0, sizeof(LONG));

	return pAssoc;
}

void CGXCollMapDWordToLong::FreeAssoc(CGXCollMapDWordToLong::CAssoc* pAssoc)
{

	pAssoc->pNext = m_pFreeList;
	m_pFreeList = pAssoc;
	m_nCount--;
	ASSERT(m_nCount >= 0);  // make sure we don't underflow
}

CGXCollMapDWordToLong::CAssoc*
CGXCollMapDWordToLong::GetAssocAt(DWORD key, UINT& nHash) const
// find association (or return NULL)
{
	nHash = HashKey(key) % m_nHashTableSize;

	if (m_pHashTable == NULL)
		return NULL;

	// see if it exists
	CAssoc* pAssoc;
	for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (pAssoc->key == key)
			return pAssoc;
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CGXCollMapDWordToLong::Lookup(DWORD key, LONG& rValue) const
{
	ASSERT_VALID(this);

	UINT nHash;
	CAssoc* pAssoc = GetAssocAt(key, nHash);
	if (pAssoc == NULL)
		return FALSE;  // not in map

	rValue = pAssoc->value;
	return TRUE;
}

LONG& CGXCollMapDWordToLong::operator[](DWORD key)
{
	ASSERT_VALID(this);

	UINT nHash;
	CAssoc* pAssoc;
	if ((pAssoc = GetAssocAt(key, nHash)) == NULL)
	{
		if (m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);

		// it doesn't exist, add a new Association
		pAssoc = NewAssoc();
		pAssoc->nHashValue = nHash;
		pAssoc->key = key;
		// 'pAssoc->value' is a constructed object, nothing more

		// put into hash table
		pAssoc->pNext = m_pHashTable[nHash];
		m_pHashTable[nHash] = pAssoc;
	}
	return pAssoc->value;  // return new reference
}


BOOL CGXCollMapDWordToLong::RemoveKey(DWORD key)
// remove key - return TRUE if removed
{
	ASSERT_VALID(this);

	if (m_pHashTable == NULL)
		return FALSE;  // nothing in the table

	CAssoc** ppAssocPrev;
	ppAssocPrev = &m_pHashTable[HashKey(key) % m_nHashTableSize];

	CAssoc* pAssoc;
	for (pAssoc = *ppAssocPrev; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (pAssoc->key == key)
		{
			// remove it
			*ppAssocPrev = pAssoc->pNext;  // remove from list
			FreeAssoc(pAssoc);
			return TRUE;
		}
		ppAssocPrev = &pAssoc->pNext;
	}
	return FALSE;  // not found
}


/////////////////////////////////////////////////////////////////////////////
// Iterating

void CGXCollMapDWordToLong::GetNextAssoc(POSITION& rNextPosition,
	DWORD& rKey, LONG& rValue) const
{
	ASSERT_VALID(this);
	ASSERT(m_pHashTable != NULL);  // never call on empty map

	CAssoc* pAssocRet = (CAssoc*)rNextPosition;
	ASSERT(pAssocRet != NULL);

	if (pAssocRet == (CAssoc*) BEFORE_START_POSITION)
	{
		// find the first association
		for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
				break;
		ASSERT(pAssocRet != NULL);  // must find something
	}

	// find next association
	ASSERT(AfxIsValidAddress(pAssocRet, sizeof(CAssoc)));
	CAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = pAssocRet->nHashValue + 1;
		  nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	rNextPosition = (POSITION) pAssocNext;

	// fill in return data
	rKey = pAssocRet->key;
	rValue = pAssocRet->value;
}

/////////////////////////////////////////////////////////////////////////////
// Serialization


void CGXCollMapDWordToLong::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << (WORD) m_nCount;
		if (m_nCount == 0)
			return;  // nothing more to do

		ASSERT(m_pHashTable != NULL);
		for (UINT nHash = 0; nHash < m_nHashTableSize; nHash++)
		{
			CAssoc* pAssoc;
			for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL;
			  pAssoc = pAssoc->pNext)
			{
				ar << pAssoc->key;
				ar << pAssoc->value;
			}
		}
	}
	else
	{
		WORD wNewCount;
		ar >> wNewCount;
		while (wNewCount--)
		{
			DWORD newKey;
			LONG newValue;
			ar >> newKey;
			ar >> newValue;
			SetAt(newKey, newValue);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Diagnostics

#ifdef _DEBUG
/*
void CGXCollMapDWordToLong::Dump(CDumpContext& dc) const
{
	ASSERT_VALID(this);

	CObject::Dump(dc);
	dc << "with " << m_nCount << " elements";
	if (dc.GetDepth() > 0)
	{
		// Dump in format "[key] -> value"
		POSITION pos = GetStartPosition();
		DWORD key;
		LONG val;

		AFX_DUMP0(dc, "\n");
		while (pos != NULL)
		{
			GetNextAssoc(pos, key, val);
			AFX_DUMP1(dc, "\n\t[", key);
			AFX_DUMP1(dc, "] = ", val);
		}
	}
	dc << "\n";

}
*/
void CGXCollMapDWordToLong::AssertValid() const
{
	CObject::AssertValid();

	ASSERT(m_nHashTableSize > 0);
	ASSERT(m_nCount == 0 || m_pHashTable != NULL);
		// non-empty map should have hash table
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

CMapNdxToOb::CMapNdxToOb(int nBlockSize)
{
	ASSERT(nBlockSize > 0);

	m_pHashTable = NULL;
	m_nHashTableSize = 17;  // default size
	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

inline UINT CMapNdxToOb::HashKey(GXNDX key) const
{
	// default identity hash - works for most primitive values
#ifdef _WIN32
	return ((UINT)(void*)(DWORD)key) >> 4;
#else
	return _AFX_FP_OFF(key) >> 4;
#endif
}

void CMapNdxToOb::InitHashTable(
	UINT nHashSize, BOOL bAllocNow)
//
// Used to force allocation of a hash table or to override the default
//   hash table size of (which is fairly small)
{
	ASSERT_VALID(this);
	ASSERT(m_nCount == 0);
	ASSERT(nHashSize > 0);

	ASSERT(bAllocNow || m_pHashTable == NULL);
	if (m_pHashTable != NULL)
	{
		// free hash table
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}

	if (bAllocNow)
	{
		m_pHashTable = new CAssoc* [nHashSize];
		memset(m_pHashTable, 0, sizeof(CAssoc*) * nHashSize);
	}
	m_nHashTableSize = nHashSize;
}

void CMapNdxToOb::RemoveAll()
{
	ASSERT_VALID(this);



	if (m_pHashTable != NULL)
	{
		// free hash table
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}

	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

CMapNdxToOb::~CMapNdxToOb()
{
	RemoveAll();
	ASSERT(m_nCount == 0);
}

/////////////////////////////////////////////////////////////////////////////
// Assoc helpers
// same as CList implementation except we store CAssoc's not CNode's
//    and CAssoc's are singly linked all the time

CMapNdxToOb::CAssoc*
CMapNdxToOb::NewAssoc()
{
	if (m_pFreeList == NULL)
	{
		// add another block
		CGXPlex* newBlock = CGXPlex::Create(m_pBlocks, m_nBlockSize, sizeof(CMapNdxToOb::CAssoc));
		// chain them into free list
		CMapNdxToOb::CAssoc* pAssoc = (CMapNdxToOb::CAssoc*) newBlock->data();
		// free in reverse order to make it easier to debug
		pAssoc += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pAssoc--)
		{
			pAssoc->pNext = m_pFreeList;
			m_pFreeList = pAssoc;
		}
	}
	ASSERT(m_pFreeList != NULL);  // we must have something

	CMapNdxToOb::CAssoc* pAssoc = m_pFreeList;
	m_pFreeList = m_pFreeList->pNext;
	m_nCount++;
	ASSERT(m_nCount > 0);  // make sure we don't overflow
	memset(&pAssoc->key, 0, sizeof(GXNDX));

	memset(&pAssoc->value, 0, sizeof(CObject*));

	return pAssoc;
}

void CMapNdxToOb::FreeAssoc(CMapNdxToOb::CAssoc* pAssoc)
{

	pAssoc->pNext = m_pFreeList;
	m_pFreeList = pAssoc;
	m_nCount--;
	ASSERT(m_nCount >= 0);  // make sure we don't underflow
}

CMapNdxToOb::CAssoc*
CMapNdxToOb::GetAssocAt(GXNDX key, UINT& nHash) const
// find association (or return NULL)
{
	nHash = HashKey(key) % m_nHashTableSize;

	if (m_pHashTable == NULL)
		return NULL;

	// see if it exists
	CAssoc* pAssoc;
	for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (pAssoc->key == key)
			return pAssoc;
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CMapNdxToOb::Lookup(GXNDX key, CObject*& rValue) const
{
	ASSERT_VALID(this);

	UINT nHash;
	CAssoc* pAssoc = GetAssocAt(key, nHash);
	if (pAssoc == NULL)
		return FALSE;  // not in map

	rValue = pAssoc->value;
	return TRUE;
}

CObject*& CMapNdxToOb::operator[](GXNDX key)
{
	ASSERT_VALID(this);

	UINT nHash;
	CAssoc* pAssoc;
	if ((pAssoc = GetAssocAt(key, nHash)) == NULL)
	{
		if (m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);

		// it doesn't exist, add a new Association
		pAssoc = NewAssoc();
		pAssoc->nHashValue = nHash;
		pAssoc->key = key;
		// 'pAssoc->value' is a constructed object, nothing more

		// put into hash table
		pAssoc->pNext = m_pHashTable[nHash];
		m_pHashTable[nHash] = pAssoc;
	}
	return pAssoc->value;  // return new reference
}


BOOL CMapNdxToOb::RemoveKey(GXNDX key)
// remove key - return TRUE if removed
{
	ASSERT_VALID(this);

	if (m_pHashTable == NULL)
		return FALSE;  // nothing in the table

	CAssoc** ppAssocPrev;
	ppAssocPrev = &m_pHashTable[HashKey(key) % m_nHashTableSize];

	CAssoc* pAssoc;
	for (pAssoc = *ppAssocPrev; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (pAssoc->key == key)
		{
			// remove it
			*ppAssocPrev = pAssoc->pNext;  // remove from list
			FreeAssoc(pAssoc);
			return TRUE;
		}
		ppAssocPrev = &pAssoc->pNext;
	}
	return FALSE;  // not found
}


/////////////////////////////////////////////////////////////////////////////
// Iterating

void CMapNdxToOb::GetNextAssoc(POSITION& rNextPosition,
	GXNDX& rKey, CObject*& rValue) const
{
	ASSERT_VALID(this);
	ASSERT(m_pHashTable != NULL);  // never call on empty map

	CAssoc* pAssocRet = (CAssoc*)rNextPosition;
	ASSERT(pAssocRet != NULL);

	if (pAssocRet == (CAssoc*) BEFORE_START_POSITION)
	{
		// find the first association
		for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
				break;
		ASSERT(pAssocRet != NULL);  // must find something
	}

	// find next association
	ASSERT(AfxIsValidAddress(pAssocRet, sizeof(CAssoc)));
	CAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = pAssocRet->nHashValue + 1;
		  nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	rNextPosition = (POSITION) pAssocNext;

	// fill in return data
	rKey = pAssocRet->key;
	rValue = pAssocRet->value;
}


/////////////////////////////////////////////////////////////////////////////
// Serialization

void CMapNdxToOb::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << (WORD) m_nCount;
		if (m_nCount == 0)
			return;  // nothing more to do

		ASSERT(m_pHashTable != NULL);
		for (UINT nHash = 0; nHash < m_nHashTableSize; nHash++)
		{
			CAssoc* pAssoc;
			for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL;
			  pAssoc = pAssoc->pNext)
			{
				ar << pAssoc->key;
				ar << pAssoc->value;
			}
		}
	}
	else
	{
		WORD wNewCount;
		ar >> wNewCount;

		GXNDX newKey;
		CObject* newValue;
		while (wNewCount--)
		{
			ar >> newKey;
			ar >> newValue;
			SetAt(newKey, newValue);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Diagnostics

#ifdef _DEBUG
/*
void CMapNdxToOb::Dump(CDumpContext& dc) const
{

	CObject::Dump(dc);
	dc << "with " << m_nCount << " elements";
	if (dc.GetDepth() > 0)
	{
		// Dump in format "[key] -> value"
		GXNDX key;
		CObject* val;

		POSITION pos = GetStartPosition();
		while (pos != NULL)
		{
			GetNextAssoc(pos, key, val);
			dc << "\n\t[" << key << "] = " << val;
		}
	}
	dc << "\n";

}
*/
void CMapNdxToOb::AssertValid() const
{
	CObject::AssertValid();

	ASSERT(m_nHashTableSize > 0);
	ASSERT(m_nCount == 0 || m_pHashTable != NULL);
		// non-empty map should have hash table
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////

CGXMapByte8ToPtr::CGXMapByte8ToPtr(int nBlockSize)
{
	ASSERT(nBlockSize > 0);

	m_pHashTable = NULL;
	m_nHashTableSize = 17;  // default size
	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

inline UINT CGXMapByte8ToPtr::HashKey(GXBYTE8 key) const
{
	// default identity hash - works for most primitive values
	return *((UINT*)(void*)&key);
}

void CGXMapByte8ToPtr::InitHashTable(
	UINT nHashSize, BOOL bAllocNow)
//
// Used to force allocation of a hash table or to override the default
//   hash table size of (which is fairly small)
{
	ASSERT_VALID(this);
	ASSERT(m_nCount == 0);
	ASSERT(nHashSize > 0);

	if (m_pHashTable != NULL)
	{
		// free hash table
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}

	if (bAllocNow)
	{
		m_pHashTable = new CAssoc* [nHashSize];
		memset(m_pHashTable, 0, sizeof(CAssoc*) * nHashSize);
	}
	m_nHashTableSize = nHashSize;
}

void CGXMapByte8ToPtr::RemoveAll()
{
	ASSERT_VALID(this);



	if (m_pHashTable != NULL)
	{
		// free hash table
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}

	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

CGXMapByte8ToPtr::~CGXMapByte8ToPtr()
{
	RemoveAll();
	ASSERT(m_nCount == 0);
}

/////////////////////////////////////////////////////////////////////////////
// Assoc helpers
// same as CList implementation except we store CAssoc's not CNode's
//    and CAssoc's are singly linked all the time

CGXMapByte8ToPtr::CAssoc*
CGXMapByte8ToPtr::NewAssoc()
{
	if (m_pFreeList == NULL)
	{
		// add another block
		CGXPlex* newBlock = CGXPlex::Create(m_pBlocks, m_nBlockSize, sizeof(CGXMapByte8ToPtr::CAssoc));
		// chain them into free list
		CGXMapByte8ToPtr::CAssoc* pAssoc = (CGXMapByte8ToPtr::CAssoc*) newBlock->data();
		// free in reverse order to make it easier to debug
		pAssoc += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pAssoc--)
		{
			pAssoc->pNext = m_pFreeList;
			m_pFreeList = pAssoc;
		}
	}
	ASSERT(m_pFreeList != NULL);  // we must have something

	CGXMapByte8ToPtr::CAssoc* pAssoc = m_pFreeList;
	m_pFreeList = m_pFreeList->pNext;
	m_nCount++;
	ASSERT(m_nCount > 0);  // make sure we don't overflow

	memset(&pAssoc->key, 0, sizeof(GXBYTE8));
	memset(&pAssoc->value, 0, sizeof(void*));

	return pAssoc;
}

void CGXMapByte8ToPtr::FreeAssoc(CGXMapByte8ToPtr::CAssoc* pAssoc)
{

	pAssoc->pNext = m_pFreeList;
	m_pFreeList = pAssoc;
	m_nCount--;
	ASSERT(m_nCount >= 0);	// make sure we don't underflow

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

CGXMapByte8ToPtr::CAssoc*
CGXMapByte8ToPtr::GetAssocAt(GXBYTE8 key, UINT& nHash) const
// find association (or return NULL)
{
	nHash = HashKey(key) % m_nHashTableSize;

	if (m_pHashTable == NULL)
		return NULL;

	// see if it exists
	CAssoc* pAssoc;
	for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
#if _MFC_VER >= 0x0300
		if (pAssoc->key == key)
#else
		if (memcmp(&pAssoc->key, &key, sizeof(GXBYTE8)) == 0)
#endif
			return pAssoc;
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CGXMapByte8ToPtr::Lookup(GXBYTE8 key, void*& rValue) const
{
	ASSERT_VALID(this);

	UINT nHash;
	CAssoc* pAssoc = GetAssocAt(key, nHash);
	if (pAssoc == NULL)
		return FALSE;  // not in map

	rValue = pAssoc->value;
	return TRUE;
}

void*& CGXMapByte8ToPtr::operator[](GXBYTE8 key)
{
	ASSERT_VALID(this);

	UINT nHash;
	CAssoc* pAssoc;
	if ((pAssoc = GetAssocAt(key, nHash)) == NULL)
	{
		if (m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);

		// it doesn't exist, add a new Association
		pAssoc = NewAssoc();
		pAssoc->nHashValue = nHash;
		pAssoc->key = key;
		// 'pAssoc->value' is a constructed object, nothing more

		// put into hash table
		pAssoc->pNext = m_pHashTable[nHash];
		m_pHashTable[nHash] = pAssoc;
	}
	return pAssoc->value;  // return new reference
}


BOOL CGXMapByte8ToPtr::RemoveKey(GXBYTE8 key)
// remove key - return TRUE if removed
{
	ASSERT_VALID(this);

	if (m_pHashTable == NULL)
		return FALSE;  // nothing in the table

	CAssoc** ppAssocPrev;
	ppAssocPrev = &m_pHashTable[HashKey(key) % m_nHashTableSize];

	CAssoc* pAssoc;
	for (pAssoc = *ppAssocPrev; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
#if _MFC_VER >= 0x0300
		if (pAssoc->key == key)
#else
		if (memcmp(&pAssoc->key, &key, sizeof(GXBYTE8)) == 0)
#endif
		{
			// remove it
			*ppAssocPrev = pAssoc->pNext;  // remove from list
			FreeAssoc(pAssoc);
			return TRUE;
		}
		ppAssocPrev = &pAssoc->pNext;
	}
	return FALSE;  // not found
}


/////////////////////////////////////////////////////////////////////////////
// Iterating

void CGXMapByte8ToPtr::GetNextAssoc(POSITION& rNextPosition,
	GXBYTE8& rKey, void*& rValue) const
{
	ASSERT_VALID(this);
	ASSERT(m_pHashTable != NULL);  // never call on empty map

	CAssoc* pAssocRet = (CAssoc*)rNextPosition;
	ASSERT(pAssocRet != NULL);

	if (pAssocRet == (CAssoc*) BEFORE_START_POSITION)
	{
		// find the first association
		for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
				break;
		ASSERT(pAssocRet != NULL);  // must find something
	}

	// find next association
	ASSERT(AfxIsValidAddress(pAssocRet, sizeof(CAssoc)));
	CAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = pAssocRet->nHashValue + 1;
		  nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	rNextPosition = (POSITION) pAssocNext;

	// fill in return data
	rKey = pAssocRet->key;
	rValue = pAssocRet->value;
}


/////////////////////////////////////////////////////////////////////////////
// Diagnostics

#ifdef _DEBUG
/*
void CGXMapByte8ToPtr::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
	dc << "with " << m_nCount << " elements";
	if (dc.GetDepth() > 0)
	{
		// Dump in format "[key] -> value"
		GXBYTE8 key;
		void* val;
		POSITION pos = GetStartPosition();
		while (pos != NULL)
		{
			GetNextAssoc(pos, key, val);
			// dc << "\n\t[" << key << "] = " << val;
		}
	}
	dc << "\n";

}
*/
void CGXMapByte8ToPtr::AssertValid() const
{
	CObject::AssertValid();

	ASSERT(m_nHashTableSize > 0);
	ASSERT(m_nCount == 0 || m_pHashTable != NULL);
		// non-empty map should have hash table
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////

CGXMapFontData::CGXMapFontData(int nBlockSize)
{
	ASSERT(nBlockSize > 0);

	m_pHashTable = NULL;
	m_nHashTableSize = 257;  // default size
	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

inline UINT CGXMapFontData::HashKey(GXBYTE8 key) const
{
	// default identity hash - works for most primitive values
	return *((UINT*)(void*)&key);
}

void CGXMapFontData::InitHashTable(
	UINT nHashSize, BOOL bAllocNow)
//
// Used to force allocation of a hash table or to override the default
//   hash table size of (which is fairly small)
{
	ASSERT_VALID(this);
	ASSERT(m_nCount == 0);
	ASSERT(nHashSize > 0);

	if (m_pHashTable != NULL)
	{
		// free hash table
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}

	if (bAllocNow)
	{
		m_pHashTable = new CAssoc* [nHashSize];
		memset(m_pHashTable, 0, sizeof(CAssoc*) * nHashSize);
	}
	m_nHashTableSize = nHashSize;
}

void CGXMapFontData::RemoveAll()
{
	ASSERT_VALID(this);



	if (m_pHashTable != NULL)
	{
		// free hash table
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}

	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

CGXMapFontData::~CGXMapFontData()
{
	RemoveAll();
	ASSERT(m_nCount == 0);
}

/////////////////////////////////////////////////////////////////////////////
// Assoc helpers
// same as CList implementation except we store CAssoc's not CNode's
//    and CAssoc's are singly linked all the time

CGXMapFontData::CAssoc*
CGXMapFontData::NewAssoc()
{
	if (m_pFreeList == NULL)
	{
		// add another block
		CGXPlex* newBlock = CGXPlex::Create(m_pBlocks, m_nBlockSize, sizeof(CGXMapFontData::CAssoc));
		// chain them into free list
		CGXMapFontData::CAssoc* pAssoc = (CGXMapFontData::CAssoc*) newBlock->data();
		// free in reverse order to make it easier to debug
		pAssoc += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pAssoc--)
		{
			pAssoc->pNext = m_pFreeList;
			m_pFreeList = pAssoc;
		}
	}
	ASSERT(m_pFreeList != NULL);  // we must have something

	CGXMapFontData::CAssoc* pAssoc = m_pFreeList;
	m_pFreeList = m_pFreeList->pNext;
	m_nCount++;
	ASSERT(m_nCount > 0);  // make sure we don't overflow

	memset(&pAssoc->key, 0, sizeof(GXBYTE8));
	memset(&pAssoc->value, 0, sizeof(CGXFontData));

	return pAssoc;
}

void CGXMapFontData::FreeAssoc(CGXMapFontData::CAssoc* pAssoc)
{

	pAssoc->pNext = m_pFreeList;
	m_pFreeList = pAssoc;
	m_nCount--;
	ASSERT(m_nCount >= 0);	// make sure we don't underflow

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

CGXMapFontData::CAssoc*
CGXMapFontData::GetAssocAt(GXBYTE8 key, UINT& nHash) const
// find association (or return NULL)
{
	nHash = HashKey(key) % m_nHashTableSize;

	if (m_pHashTable == NULL)
		return NULL;

	// see if it exists
	CAssoc* pAssoc;
	for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
#if _MFC_VER >= 0x0300
		if (pAssoc->key == key)
#else
		if (memcmp(&pAssoc->key, &key, sizeof(GXBYTE8)) == 0)
#endif
			return pAssoc;
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CGXMapFontData::Lookup(GXBYTE8 key, CGXFontData& rValue) const
{
	ASSERT_VALID(this);

	UINT nHash;
	CAssoc* pAssoc = GetAssocAt(key, nHash);
	if (pAssoc == NULL)
		return FALSE;  // not in map

	rValue = pAssoc->value;
	return TRUE;
}

CGXFontData& CGXMapFontData::operator[](GXBYTE8 key)
{
	ASSERT_VALID(this);

	UINT nHash;
	CAssoc* pAssoc;
	if ((pAssoc = GetAssocAt(key, nHash)) == NULL)
	{
		if (m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);

		// it doesn't exist, add a new Association
		pAssoc = NewAssoc();
		pAssoc->nHashValue = nHash;
		pAssoc->key = key;
		// 'pAssoc->value' is a constructed object, nothing more

		// put into hash table
		pAssoc->pNext = m_pHashTable[nHash];
		m_pHashTable[nHash] = pAssoc;
	}
	return pAssoc->value;  // return new reference
}


BOOL CGXMapFontData::RemoveKey(GXBYTE8 key)
// remove key - return TRUE if removed
{
	ASSERT_VALID(this);

	if (m_pHashTable == NULL)
		return FALSE;  // nothing in the table

	CAssoc** ppAssocPrev;
	ppAssocPrev = &m_pHashTable[HashKey(key) % m_nHashTableSize];

	CAssoc* pAssoc;
	for (pAssoc = *ppAssocPrev; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
#if _MFC_VER >= 0x0300
		if (pAssoc->key == key)
#else
		if (memcmp(&pAssoc->key, &key, sizeof(GXBYTE8)) == 0)
#endif
		{
			// remove it
			*ppAssocPrev = pAssoc->pNext;  // remove from list
			FreeAssoc(pAssoc);
			return TRUE;
		}
		ppAssocPrev = &pAssoc->pNext;
	}
	return FALSE;  // not found
}


/////////////////////////////////////////////////////////////////////////////
// Iterating

void CGXMapFontData::GetNextAssoc(POSITION& rNextPosition,
	GXBYTE8& rKey, CGXFontData& rValue) const
{
	ASSERT_VALID(this);
	ASSERT(m_pHashTable != NULL);  // never call on empty map

	CAssoc* pAssocRet = (CAssoc*)rNextPosition;
	ASSERT(pAssocRet != NULL);

	if (pAssocRet == (CAssoc*) BEFORE_START_POSITION)
	{
		// find the first association
		for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
				break;
		ASSERT(pAssocRet != NULL);  // must find something
	}

	// find next association
	ASSERT(AfxIsValidAddress(pAssocRet, sizeof(CAssoc)));
	CAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = pAssocRet->nHashValue + 1;
		  nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	rNextPosition = (POSITION) pAssocNext;

	// fill in return data
	rKey = pAssocRet->key;
	rValue = pAssocRet->value;
}


/////////////////////////////////////////////////////////////////////////////
// Diagnostics

#ifdef _DEBUG
/*
void CGXMapFontData::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
	dc << "with " << m_nCount << " elements";
	if (dc.GetDepth() > 0)
	{
		// Dump in format "[key] -> value"
		GXBYTE8 key;
		CGXFontData val;
		POSITION pos = GetStartPosition();
		while (pos != NULL)
		{
			GetNextAssoc(pos, key, val);
			// dc << "\n\t[" << key << "] = " << val;
		}
	}
	dc << "\n";
}
*/
void CGXMapFontData::AssertValid() const
{
	CObject::AssertValid();

	ASSERT(m_nHashTableSize > 0);
	ASSERT(m_nCount == 0 || m_pHashTable != NULL);
		// non-empty map should have hash table
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// class CGXPtrArrayPtrArray

CGXPtrArrayPtrArray::~CGXPtrArrayPtrArray()
{
	DeleteAll();
}

void CGXPtrArrayPtrArray::DeleteAt(int nIndex, int nCount)
{
	int nUpper = min(GetUpperBound(), nIndex+nCount-1);
	if (nUpper >= nIndex)
	{
		for (int index = nIndex; index <= nUpper; index++)
			delete GetAt(index);

		RemoveAt(nIndex, nUpper-nIndex+1);
	}
}

void CGXPtrArrayPtrArray::DeleteAll()
{
	for (int index = 0; index <= GetUpperBound(); index++)
		delete GetAt(index);

	RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
// Some helper methods, classes

void AFXAPI GXDeleteAllObjects(CMapPtrToPtr& ptmap)
{
	// Parameter Object Local Data
	POSITION pos = ptmap.GetStartPosition();
	while (pos)
	{
		CObject* pObject;
		void* pClassId;
		
		ptmap.GetNextAssoc(pos, pClassId, (void*&) pObject);
		delete pObject;
	}

	ptmap.RemoveAll();
}

void AFXAPI GXDeleteAllObjects(CObList& oblist)
{
	POSITION pos = oblist.GetHeadPosition();
	while (pos)
	{
		CObject* pObject = oblist.GetNext(pos);
		delete pObject;
	}
	oblist.RemoveAll();
}

void AFXAPI GXDeleteAllObjects(CObArray& obarray)
{
	for (int i = 0; i < obarray.GetSize(); i++)
		delete obarray.GetAt(i);
	obarray.RemoveAll();
}

CGXAutoDeleteObList::~CGXAutoDeleteObList()
{
	 GXDeleteAllObjects(*this);
}

