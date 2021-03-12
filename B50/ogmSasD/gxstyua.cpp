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

// gxstyua.cpp : implementation of the CGXAbstractUserAttribute classes
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

#ifndef _GXSTATE_H_
#include "gxstate.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXSTYUA")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CGXUserAttributeMap, CObArray, 0 /* schema number*/ )
IMPLEMENT_SERIAL(CGXAbstractUserAttribute, CObject, 0 /* schema number*/ )

#define new DEBUG_NEW

CString CGXAbstractUserAttribute::m_strValueCache;


/////////////////////////////////////////////////////////////////
// User Attributes

/////////////////////////////////////////////////////////////////
// CGXAbstractUserAttribute

CGXAbstractUserAttribute::CGXAbstractUserAttribute()
{
}

CGXAbstractUserAttribute::~CGXAbstractUserAttribute()
{
}

CGXAbstractUserAttribute& CGXAbstractUserAttribute::SetValue(const CString& strValue)
{
	return SetValue((LPCTSTR) strValue);
}

CGXAbstractUserAttribute::operator LPCTSTR() const
{
	return GetValue();
}

void CGXAbstractUserAttribute::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
		ar << GetValue();
	else
	{
		CString s;
		ar >> s;
		SetValue(s);
	}
}

BOOL CGXAbstractUserAttribute::IsEqual(const CGXAbstractUserAttribute& p) const
{
	// First check id so that we know this is the same attribute type
	if (p.m_nId != m_nId)
		return FALSE;

	CString s = GetValue();

	return s.Compare(p.GetValue()) == 0;
}

BOOL CGXAbstractUserAttribute::IsEmpty() const
{
	return GetValue().IsEmpty();
}

CGXAbstractUserAttribute& CGXAbstractUserAttribute::SetValue(LPCTSTR )
{
	ASSERT(0);
	// ASSERTION-> If you have registered the user attribute with
	// CGXStylesMap::AddUserAttribute this method must be implemented
	// so that the value can be changed in the user attribute page
	// and loaded from the registry ->END

	return *this;
}

const CString& CGXAbstractUserAttribute::GetValue() const
{
	m_strValueCache.Empty();
	return m_strValueCache;
}

DWORD CGXAbstractUserAttribute::GetDWordValue() const
{
	return (DWORD) _ttol(GetValue());
}

LONG CGXAbstractUserAttribute::GetLongValue() const
{
	return _ttol(GetValue());
}

double CGXAbstractUserAttribute::GetDoubleValue() const
{
	return _gxttof(GetValue());
}

CGXAbstractUserAttribute* CGXAbstractUserAttribute::Clone() const
{
	ASSERT(0);
	// ASSERTION-> This method must be implemented in your derived
	// class. You should simply call 
	// return new CYourUserAttibute(*this);
	// ->END

	return NULL;
}

/////////////////////////////////////////////////////////////////
// CGXUserAttributeMap

CGXUserAttributeMap::CGXUserAttributeMap()
{
}

CGXUserAttributeMap::~CGXUserAttributeMap()
{
	Empty();
}

void CGXUserAttributeMap::Empty()
{
	for (int i = 0; i < GetSize(); i++)
	{
		CGXAbstractUserAttribute* p = (CGXAbstractUserAttribute*) GetAt(i);
		delete p;
	}

	RemoveAll();
}

BOOL CGXUserAttributeMap::IsEmpty() const
{
	return GetSize() == NULL;
}

// Attributes (each CGXUserAttributeMap always holds its own
// attribute objects on the heap)
void CGXUserAttributeMap::SetAttribute(WORD nID, const CGXAbstractUserAttribute& attribute)
{
	SetAttributePtr(nID, attribute.Clone());
}

void CGXUserAttributeMap::SetAttributePtr(WORD nID, CGXAbstractUserAttribute* pValue)
{
	int nIndex = FindAttribute(nID);
	pValue->m_nId = (WORD) nID;

	if (nIndex != -1)
	{
		CGXAbstractUserAttribute* p = (CGXAbstractUserAttribute*) GetAt(nIndex);
		delete p;

		SetAt(nIndex, pValue);
	}
	else
	{
		SetAtGrow(GetSize(), pValue);
		Sort();
	}
}

void CGXUserAttributeMap::DeleteAttribute(WORD nID)
{
	int nIndex = FindAttribute(nID);

	if (nIndex != -1)
	{
		CGXAbstractUserAttribute* p = (CGXAbstractUserAttribute*) GetAt(nIndex);
		delete p;

		RemoveAt(nIndex);
	}
}

const CGXAbstractUserAttribute* CGXUserAttributeMap::GetAttribute(WORD nID) const
{
	int nIndex = FindAttribute(nID);

	if (nIndex != -1)
		return (CGXAbstractUserAttribute*) GetAt(nIndex);
	else
		return NULL;
}

void CGXUserAttributeMap::ChangeMap(const CGXUserAttributeMap* pSrc, GXModifyType mt)
{
	// Append new attributes
	if (mt == gxCopy)
		Empty();

	if (mt == gxExclude)
	{
		for (int nSrc = 0; nSrc < pSrc->GetSize(); nSrc++)
		{
			CGXAbstractUserAttribute* p = (CGXAbstractUserAttribute*) pSrc->GetAt(nSrc);
			DeleteAttribute(p->m_nId);
		}
	}
	else
	{
		CObArray objArray;
		for (int nSrc = 0; nSrc < pSrc->GetSize(); nSrc++)
		{
			CGXAbstractUserAttribute* p = (CGXAbstractUserAttribute*) pSrc->GetAt(nSrc);
			int nIndex = FindAttribute(p->m_nId);
			if (mt == gxApplyNew && nIndex != -1)
				continue;

			CGXAbstractUserAttribute* pClone = p->Clone();
			pClone->m_nId = p->m_nId;

			if (nIndex != -1)
			{
				CGXAbstractUserAttribute* p = (CGXAbstractUserAttribute*) GetAt(nIndex);
				delete p;

				SetAt(nIndex, pClone);
			}
			else
			{
				objArray.SetAtGrow(objArray.GetSize(), pClone);
			}
		}

		if (objArray.GetSize() > 0)
		{
			InsertAt(GetSize(), &objArray);
			Sort();
		}
	}
}

// Operations
void CGXUserAttributeMap::WriteProfile(LPCTSTR pszSection) const
{
	for (int i = 0; i < GetSize(); i++)
	{
		CGXAbstractUserAttribute* p = (CGXAbstractUserAttribute*) GetAt(i);
		TCHAR s[10];
		wsprintf(s, _T("UA%lu"), p->m_nId);
		AfxGetApp()->WriteProfileString(pszSection, s, p->GetValue());
	}
}

void CGXUserAttributeMap::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << (WORD) GetSize();

		for (int i = 0; i < GetSize(); i++)
		{
			CGXAbstractUserAttribute* p = (CGXAbstractUserAttribute*) GetAt(i);
			ar << p;
			ar << p->m_nId;
		}
	}
	else
	{
		Empty();

		WORD w;
		ar >> w;

		SetSize(w);		

		for (int i = 0; i < GetSize(); i++)
		{
			CGXAbstractUserAttribute* p = NULL;
			ar >> p;
			ar >> p->m_nId;
			SetAt(i, p);
		}
	}
}

#ifndef _WINDU_SOURCE
extern "C" static int __cdecl __gxcomparefindattr(const void* vp1, const void* vp2)
#else
extern "C" int __cdecl __gxcomparefindattr(const void* vp1, const void* vp2)
#endif
{
	WORD nId = (WORD) (LONG) vp1;
	CGXAbstractUserAttribute* p = *((CGXAbstractUserAttribute**) vp2);
	
	if (nId == p->m_nId)
		return 0;
	else if (nId > p->m_nId)
		return 1;
	else
		return -1;
}

#ifndef _WINDU_SOURCE
extern "C" static int __cdecl __gxcomparesortatt(const void* vp1, const void* vp2)
#else
extern "C" int __cdecl __gxcomparesortatt(const void* vp1, const void* vp2)
#endif
{
	CGXAbstractUserAttribute* p1 = *((CGXAbstractUserAttribute**) vp1);
	CGXAbstractUserAttribute* p2 = *((CGXAbstractUserAttribute**) vp2);
	
	if (p1->m_nId == p2->m_nId)
		return 0;
	else if (p1->m_nId > p2->m_nId)
		return 1;
	else
		return -1;
}


int CGXUserAttributeMap::FindAttribute(WORD nID) const
{                  
	if (m_pData == NULL)
		return -1;
	
	LONG id = nID;

	// search array	- NOTE: Boundschecker complains
	// at the following line but this call is checked 
	// from us and is safe! So, when running Boundschecker
	// you can simply ignore the error,
	CObject** pItem = NULL;
	if (GetSize() > 0)
		pItem = (CObject**) bsearch(
			(const void*) id, 
			m_pData, 
			(size_t) GetSize(), 
			sizeof(CObject*), 
			__gxcomparefindattr);

	if (pItem == NULL)
		return -1;
	else
		return pItem-m_pData;
}

void CGXUserAttributeMap::Sort()
{
	qsort(m_pData, (size_t) GetSize(), sizeof(void*), __gxcomparesortatt);
}

