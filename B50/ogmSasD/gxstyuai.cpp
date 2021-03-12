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

// gxstyuai.cpp : implementation of the CGXUserAttribute class
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
#pragma code_seg("GX_SEG_GXSTYUAI")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CGXUserAttribute, CGXAbstractUserAttribute, 0 /* schema number*/ )

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////
// CGXStyle

// User attributs

CGXStyle& CGXStyle::SetIncludeUserAttribute(WORD nID, BOOL b)
{
	const CGXAbstractUserAttribute* pValue = NULL;

	BOOL bExist = m_pmapUserAttributes && (pValue = m_pmapUserAttributes->GetAttribute(nID)) != 0;

	// Check if state is changing
	if (b && !bExist || !b && bExist)
	{
		if (b)
		{
			// Allocate user attribues map
			if (m_pmapUserAttributes == NULL)
				m_pmapUserAttributes = new CGXUserAttributeMap;
			// Assign empty string
			m_pmapUserAttributes->SetAttributePtr(nID, new CGXUserAttribute());
			stylebits.userattr = 1;
		}
		else
		{
			// Clear out attribute and delete string
			// delete pValue;
			m_pmapUserAttributes->DeleteAttribute(nID);
			if (m_pmapUserAttributes->IsEmpty())
				stylebits.userattr = 0;
		}
	}

	return *this;
}

/////////////////////////////////////////////////////////////////
// CGXUserAttribute

static BOOL _deval(const CString& s, double *d)
{
	if (GXGetAppData()->m_pNumberFormattingImp && !GXDeval(s, d))
		return FALSE;

	// Make sure we return the same string that we got
	// (this fixes problems if we should store leading zeros. e.g.
	// 000333)

	CString t;
	t.Format(_T("%.13g"), *d);

	return t == s;
}

CGXUserAttribute::CGXUserAttribute()
{
	m_vtType = vtEmpty;
	memset(&m_value, 0, sizeof(m_value));
}

CGXUserAttribute::CGXUserAttribute(DWORD value)
{
	m_vtType = vtDWord;
	m_value.dwValue = value;
}

CGXUserAttribute::CGXUserAttribute(LONG value)
{
	m_vtType = vtLong;
	m_value.lValue = value;
}

CGXUserAttribute::CGXUserAttribute(const CString& value)
{
	double d;
	if (_deval(value, &d))
	{
		m_vtType = vtDouble;
		m_value.dValue = d;
	}
	else if (value.GetLength() <= 8/sizeof(TCHAR))
	{
		m_vtType = vtShortString;
		_tcsncpy(m_value.szShortString, value, 8/sizeof(TCHAR));
	}
	else
	{
		m_vtType = vtString;
		m_value.pszValue = new TCHAR[value.GetLength()+1];
		_tcscpy(m_value.pszValue, value);
	}
}

CGXUserAttribute::CGXUserAttribute(LPCTSTR pszValue)
{
	double d;
	if (_deval(pszValue, &d))
	{
		m_vtType = vtDouble;
		m_value.dValue = d;
	}
	else if (_tcslen(pszValue) <= 8/sizeof(TCHAR))
	{
		m_vtType = vtShortString;
		_tcsncpy(m_value.szShortString, pszValue, 8/sizeof(TCHAR));
	}
	else
	{
		m_vtType = vtString;
		m_value.pszValue = new TCHAR[_tcslen(pszValue)+1];
		_tcscpy(m_value.pszValue, pszValue);
	}
}

CGXUserAttribute::CGXUserAttribute(double value)
{
	m_vtType = vtDouble;
	m_value.dValue = value;
}

CGXUserAttribute::CGXUserAttribute(GXBYTE8 value)
{
	m_vtType = vtByte8;
	m_value.b8Value = value;
}

CGXUserAttribute::CGXUserAttribute(const CGXUserAttribute& p)
{
	m_vtType = p.m_vtType;

	if (m_vtType == vtString)
	{
		m_value.pszValue = new TCHAR[_tcslen(p.m_value.pszValue)+1];
		_tcscpy(m_value.pszValue, p.m_value.pszValue);
	}
	else
		// binary copy
		m_value.b8Value = p.m_value.b8Value;
}

CGXUserAttribute::~CGXUserAttribute()
{
	if (m_vtType == vtString)
		// Strings must be emptied because they allocated heap space
		delete m_value.pszValue;
}

const CString& CGXUserAttribute::GetValue() const
{
	switch (m_vtType)
	{
	case vtEmpty: 
		m_strValueCache.Empty(); 
		return m_strValueCache;

	case vtDWord: 
		m_strValueCache.Format(_T("%lu"), m_value.dwValue); 
		return m_strValueCache;

	case vtLong:  
		m_strValueCache.Format(_T("%ld"), m_value.lValue);
		return m_strValueCache;

	case vtShortString: 
		{
			LPTSTR p = m_strValueCache.GetBuffer(8/sizeof(TCHAR)+1);
			_tcsncpy(p, m_value.szShortString, 8/sizeof(TCHAR));
			p[8/sizeof(TCHAR)] = 0;
			m_strValueCache.ReleaseBuffer();
		}
		return m_strValueCache;

	case vtString:
		m_strValueCache = m_value.pszValue;
		return m_strValueCache;

	case vtDouble:
		m_strValueCache.Format(_T("%.13g"), m_value.dValue);
		return m_strValueCache;

	case vtByte8:
		{
			DWORD* pd = (DWORD*) &m_value.b8Value;
			m_strValueCache.Format(_T("%08x%08x"), pd[0], pd[1]);
			return m_strValueCache;
		}

	default:
		m_strValueCache = _T("Value?");
		return m_strValueCache;
	}
}

DWORD CGXUserAttribute::GetDWordValue() const
{
	switch (m_vtType)
	{
	case vtEmpty: 
	case vtByte8:
	default:
		return 0;

	case vtDWord: 
		return m_value.dwValue; 

	case vtLong:  
		return (DWORD) m_value.lValue;

	case vtDouble:
		return (DWORD) m_value.dValue;

	case vtShortString: 
	case vtString:
		return (DWORD) _ttol(GetValue());
	}
}

LONG CGXUserAttribute::GetLongValue() const
{
	switch (m_vtType)
	{
	case vtEmpty: 
	case vtByte8:
	default:
		return 0;

	case vtDWord: 
		return (LONG) m_value.dwValue; 

	case vtLong:  
		return m_value.lValue;

	case vtDouble:
		return (LONG) m_value.dValue;

	case vtShortString: 
	case vtString:
		return _ttol(GetValue());
	}
}

double CGXUserAttribute::GetDoubleValue() const
{
	switch (m_vtType)
	{
	case vtEmpty: 
	case vtByte8:
	default:
		return 0;

	case vtDWord: 
		return (double) m_value.dwValue; 

	case vtLong:  
		return (double) m_value.lValue;

	case vtDouble:
		return m_value.dValue;

	case vtShortString: 
	case vtString:
		return _gxttof(GetValue());
	}
}

CGXAbstractUserAttribute& CGXUserAttribute::SetValue(LPCTSTR pszValue)
{
	if (m_vtType == vtString)
		// Strings must be emptied because they allocated heap space
		delete m_value.pszValue;

	double d;
	if (_tcslen(pszValue) == 0)
	{
		m_vtType = vtEmpty;
	}
	else if (_deval(pszValue, &d))
	{
		m_vtType = vtDouble;
		m_value.dValue = d;
	}
	else if (_tcslen(pszValue) <= 8/sizeof(TCHAR))
	{
		m_vtType = vtShortString;
		_tcsncpy(m_value.szShortString, pszValue, 8/sizeof(TCHAR));
	}
	else
	{
		m_vtType = vtString;
		m_value.pszValue = new TCHAR[_tcslen(pszValue)+1];
		_tcscpy(m_value.pszValue, pszValue);
	}

	return *this;
}

void CGXUserAttribute::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << (BYTE) m_vtType;
		if (m_vtType != vtString && m_vtType != vtShortString)
			ar << (BYTE)m_value.b8Value;
		else
			ar << GetValue();
	}
	else
	{
		BYTE b;
		ar >> b, m_vtType = (CGXUserAttribute::ValueType) b;
		if (m_vtType != vtString && m_vtType != vtShortString);
//			ar >> m_value.b8Value;
		else
		{
			CString s;
			ar >> s;
			if (s.GetLength() <= 8/sizeof(TCHAR))
			{
				m_vtType = vtShortString;
				_tcsncpy(m_value.szShortString, s, 8/sizeof(TCHAR));
			}
			else
			{
				m_vtType = vtString;
				m_value.pszValue = new TCHAR[s.GetLength()+1];
				_tcscpy(m_value.pszValue, s);
			}
		}
	}
}

// Copying attribute (e.g. from one style object to another)
CGXAbstractUserAttribute* CGXUserAttribute::Clone() const
{
	return new CGXUserAttribute(*this);
}

// Comparision
BOOL CGXUserAttribute::IsEqual(const CGXAbstractUserAttribute& pOther) const
{
	// First check id so that we know this is the same attribute type
	if (pOther.m_nId != m_nId)
		return FALSE;

	// Now it is save to assume that other object is also a CGXUserAttribute
	CGXUserAttribute& p = (CGXUserAttribute&) pOther;

	if (m_vtType != p.m_vtType)
		return FALSE;

	switch (m_vtType)
	{
	case vtEmpty:		return TRUE;
	case vtDWord:		return p.m_value.dwValue == m_value.dwValue; 
	case vtLong:		return p.m_value.lValue == m_value.lValue;
	case vtShortString: 
	case vtString:		{ CString s = GetValue(); return s.Compare(p.GetValue()) == 0; }
	case vtDouble:		return p.m_value.dValue == m_value.dValue;
	case vtByte8:		return memcmp(&p.m_value.b8Value, &m_value.b8Value, 8) == 0;
	default:			return FALSE;
	}
}

