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

// gxstymap.cpp : implementation of the CGXStylesMap class 
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
#pragma code_seg("GX_SEG_GXSTYMAP")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CGXStylesMap, CObject, 0 /* schema number*/ )

#define new DEBUG_NEW

// default section
LPCTSTR CGXStylesMap::szDefaultSection      = _T("Styles");

/////////////////////////////////////////////////////////////////////////////
// CGXStylesMap

CGXStylesMap::CGXStylesMap(CRuntimeClass* pStyleClass)
{
	if (pStyleClass == NULL)
		pStyleClass = RUNTIME_CLASS(CGXStyle);

	m_pStyleClass = pStyleClass;
	m_sSection = szDefaultSection;
	m_nLastId = 0;
}

CGXStylesMap::CGXStylesMap(const CGXStylesMap& p)
{
	*this = p;
}

void CGXStylesMap::CreateStandardStyles()
{
	CGXAppData* pAppData = GXGetAppData();

	ASSERT(pAppData->m_bInitDone);
	// ASSERTION-> This constructor is called before the GXInit()
	// method has been called. This can be problematic because
	// at this time no factory object is available. ->END

	// Styles
	CGXStyle* pStyle = (CGXStyle*) m_pStyleClass->CreateObject();

	pStyle->SetDefault()
		.SetFont( CGXFont().SetDefault().SetSize(8) );

	m_wStyleStandard = RegisterStyle(GX_IDS_STYLE_STANDARD, *pStyle, TRUE);

	pStyle->Free();

	// Check if borders shall be removed for headers
	if (GXGetAppData()->m_bEnableNewGridLineMode && GXGetAppData()->m_bNoHeaderBorders)
		pStyle->SetBorders(gxBorderAll, CGXPen().SetStyle(PS_NULL));

	m_wStyleColHeader = RegisterStyle(GX_IDS_STYLE_COLHEADER, (*pStyle)
		.SetFont(CGXFont().SetBold(TRUE))
		.SetInterior(GXSYSCOLOR(COLOR_BTNFACE))
		.SetHorizontalAlignment(DT_CENTER)
		.SetVerticalAlignment(DT_VCENTER)
		.SetControl(GX_IDS_CTRL_HEADER)
		, TRUE);


	pStyle->Free();

	// Check if borders shall be removed for headers
	if (GXGetAppData()->m_bEnableNewGridLineMode && GXGetAppData()->m_bNoHeaderBorders)
		pStyle->SetBorders(gxBorderAll, CGXPen().SetStyle(PS_NULL));

	m_wStyleRowHeader = RegisterStyle(GX_IDS_STYLE_ROWHEADER, (*pStyle)
		.SetControl(GX_IDS_CTRL_HEADER)
		.SetInterior(GXSYSCOLOR(COLOR_BTNFACE))
		, TRUE);

	delete pStyle;
}

CGXStylesMap::~CGXStylesMap()
{
	Free();
}

void CGXStylesMap::Free()
{
	FreeStylesMap();
	FreeControlMap();

	// UserAttributs
	for (POSITION pos = m_UserAttrInfoMap.GetStartPosition(); pos; )
	{
		WORD w;
		UserAttrInfo* pInfo;
		m_UserAttrInfoMap.GetNextAssoc(pos, w, (void*&) pInfo);
		delete pInfo;
	}

	m_UserAttrInfoMap.RemoveAll();
}

void CGXStylesMap::FreeStylesMap()
{
	// BaseStyleInfo
	WORD wStyleId;
	BaseStyleInfo* pSrcInfo;
	POSITION pos = m_mwpIdInfo.GetStartPosition();
	while (pos)
	{
		m_mwpIdInfo.GetNextAssoc(pos, wStyleId, (void*&) pSrcInfo);
		delete pSrcInfo;
	}

	m_mwpIdInfo.RemoveAll();
	m_mswNameId.RemoveAll();

	CGXStyle* pStyle;
	pos = m_mwpIdStyle.GetStartPosition();
	while (pos)
	{
		m_mwpIdStyle.GetNextAssoc(pos, wStyleId, (void*&) pStyle);
		delete pStyle;
	}

	m_mwpIdStyle.RemoveAll();

	m_nLastId = 0;
}

void CGXStylesMap::FreeControlMap()
{
	// Controls
	for (POSITION pos = m_ControlNameMap.GetStartPosition(); pos; )
	{
		WORD w;
		CString* pStrName;
		m_ControlNameMap.GetNextAssoc(pos, w, (void*&) pStrName);
		delete pStrName;
	}

	m_ControlNameMap.RemoveAll();
}

const CGXStylesMap& CGXStylesMap::operator=(const CGXStylesMap& p)
{
	GX_CHECKIMP(GXGetAppData()->m_pParamCopyOperatorImp, "CGXStylesMap::operator=");

	if (GXGetAppData()->m_pParamCopyOperatorImp)
		GXGetAppData()->m_pParamCopyOperatorImp->CopyObject(this, p);

	return *this;
}

void CGXStylesMap::Serialize(CArchive& ar)
{
	GX_CHECKIMP(GXGetAppData()->m_pDocSerializeImp, "CGXStylesMap::Serialize");

	if (GXGetAppData()->m_pDocSerializeImp)
		GXGetAppData()->m_pDocSerializeImp->Serialize(this, ar);
}

#ifdef _DEBUG
void CGXStylesMap::AssertValid() const
{
	ASSERT(m_sSection.GetLength() > 0);
}
/*
void CGXStylesMap::Dump(CDumpContext& dc) const
{
//	dc << "\nm_sSection = " << m_sSection;
//	dc << "\n";
}
*/
#endif

// Profile

void CGXStylesMap::WriteProfile() const
{
	GX_CHECKIMP(GXGetAppData()->m_pGridProfileImp, "CGXStylesMap::WriteProfile");

	if (GXGetAppData()->m_pGridProfileImp)
		GXGetAppData()->m_pGridProfileImp->WriteProfile(this);
}

BOOL CGXStylesMap::ReadProfile()
{
	GX_CHECKIMP(GXGetAppData()->m_pGridProfileImp, "CGXStylesMap::ReadProfile");

	if (GXGetAppData()->m_pGridProfileImp)
		return GXGetAppData()->m_pGridProfileImp->ReadProfile(this);

	return FALSE;
}

/////////////////////////////////////////////////////////////////
// Controls

void CGXStylesMap::AddControl(WORD nID)
{
	CString* pStrName = 0;
	if (m_ControlNameMap.Lookup(nID, (void*&) pStrName))
		return;

	CString s;
	if (!s.LoadString(nID))
	{
		ASSERT(0);
		// ASSERTION-> nID must be a String-Resource ID. Is gxres.rc included? ->END
		AfxThrowResourceException();
	}

	m_ControlNameMap.SetAt(nID, new CString(s));
}

const CMapWordToPtr& CGXStylesMap::GetControlNameMap() const
{
	return m_ControlNameMap;
}

/////////////////////////////////////////////////////////////////
// User Attributes

CGXStylesMap::UserAttrInfo::UserAttrInfo(const UserAttrInfo& srcInfo)
{
	nID = srcInfo.nID;
	sName = srcInfo.sName;
	pStyle = NULL;
	if (srcInfo.pStyle)
		pStyle = srcInfo.pStyle->Clone();
}

void CGXStylesMap::AddUserAttribute(WORD nID, const CGXStyle* pStyle)
{
	UserAttrInfo* pInfo;

	// check, if already registered
	if (m_UserAttrInfoMap.Lookup(nID, (void*&) pInfo))
		return;

	pInfo = new UserAttrInfo;

	pInfo->nID = nID;
	if (!pInfo->sName.LoadString(nID))
	{
		ASSERT(0);
		// ASSERTION-> nID must be a String-Resource ID. Is gxres.rc included? ->END
		delete pInfo;
		AfxThrowResourceException();
	}

	pInfo->pStyle = (CGXStyle*) m_pStyleClass->CreateObject();

	if (pStyle)
		*pInfo->pStyle = *pStyle;

	m_UserAttrInfoMap.SetAt(nID, pInfo);
}

const CMapWordToPtr& CGXStylesMap::GetUserAttrInfoMap() const
{
	return m_UserAttrInfoMap;
}

/////////////////////////////////////////////////////////////////
// Base Styles

CGXStylesMap::BaseStyleInfo::BaseStyleInfo(const BaseStyleInfo& srcInfo)
{
	nID = srcInfo.nID;
	sName = srcInfo.sName;
	nResourceID = srcInfo.nResourceID;
	pStyle = NULL;
	bSystem = srcInfo.bSystem;
	if (srcInfo.pStyle)
		pStyle = srcInfo.pStyle->Clone();
}

WORD CGXStylesMap::GetBaseStyleId(LPCTSTR pszName) const
{
	// returns 0, if resource id cannot be found
	void* vptr = 0;
	m_mswNameId.Lookup(pszName, vptr);
	return (WORD) (LONG) vptr;
}

WORD CGXStylesMap::GetBaseStyleId(UINT nResourceID) const
{
	// returns 0, if resource id cannot be found
	WORD wStyleId = 0;
	m_mpwResourceId.Lookup((void*) (LONG) nResourceID, wStyleId);
	return wStyleId;
}

LPCTSTR CGXStylesMap::GetStyleName(WORD wStyleId, UINT& nResourceID) const
{
	BaseStyleInfo* pInfo;

	if (m_mwpIdInfo.Lookup(wStyleId, (void*&) pInfo))
	{
		// already existent, change style
		nResourceID = pInfo->nResourceID;
		return pInfo->sName;
	}

	ASSERT(0);
	// ASSERTION-> Style not existent! ->END

	nResourceID = 0;
	return NULL;
}

WORD CGXStylesMap::RegisterStyle(LPCTSTR szName, const CGXStyle& style, BOOL bSystem /* = FALSE*/)
{
	// RegisterStyle adds a style to the map of base styles
	// or replaces the style object for an existing base style.

	// bSystem = TRUE specifies that the style is not removable

	ASSERT(style.IsKindOf(RUNTIME_CLASS(CGXStyle)));
	ASSERT_VALID(&style);

	// Id
	void* vptr = 0;
	WORD wStyleId = 0;
	if (!m_mswNameId.Lookup(szName, vptr))
	{
		wStyleId = GetNewId();
		m_mswNameId.SetAt(szName, (void*) (LONG) wStyleId);
	}
	else
		wStyleId = (WORD) (LONG) vptr;

	// Store the style
	StoreStyle(wStyleId, style, szName, 0, bSystem);

	return wStyleId;
}

WORD CGXStylesMap::RegisterStyle(UINT nResourceID, const CGXStyle& style, BOOL bSystem /* = FALSE*/)
{
	// RegisterStyle adds a style to the map of base styles
	// or replaces the style object for an existing base style.

	// bSystem = TRUE specifies that the style is not removable

	ASSERT(style.IsKindOf(RUNTIME_CLASS(CGXStyle)));
	ASSERT_VALID(&style);


	CString s;
	if (!s.LoadString(nResourceID))
	{
		TRACE(_T("Warning: Could not find string resource for base style id %u\n"), nResourceID);
		s.Format(_T("UA%u"), nResourceID);
	}

	// Id
	WORD wStyleId = 0;
	if (!m_mpwResourceId.Lookup((void*) (LONG) nResourceID, wStyleId))
	{
		wStyleId = GetNewId();
		m_mpwResourceId.SetAt((void*) (LONG) nResourceID, wStyleId);
	}

	m_mswNameId.SetAt(s, (void*) (LONG) wStyleId);

	// Store the style
	StoreStyle(wStyleId, style, s, nResourceID, bSystem);

	return wStyleId;
}

void CGXStylesMap::StoreStyle(WORD wStyleId, const CGXStyle& style, LPCTSTR szName, UINT nResourceID, BOOL bSystem)
{
	// AddStyle adds a style to the map of base styles
	// or replaces the style object for an existing base style.

	// bSystem = TRUE specifies that the style is not removable

	ASSERT(style.IsKindOf(RUNTIME_CLASS(CGXStyle)));
	ASSERT_VALID(&style);

	BaseStyleInfo* pInfo;

	if (m_mwpIdInfo.Lookup(wStyleId, (void*&) pInfo))
	{
		// already existent, change style
		delete pInfo->pStyle;
		pInfo->pStyle = style.Clone();
	}
	else
	{
		// new base style
		pInfo = new BaseStyleInfo;
		pInfo->nID = wStyleId;
		pInfo->nResourceID = nResourceID;
		pInfo->sName = szName;
		pInfo->pStyle = style.Clone();
		pInfo->bSystem = bSystem;

		m_mwpIdInfo.SetAt(wStyleId, pInfo);
	}
}

WORD CGXStylesMap::GetNewId()
{
	return m_nLastId++;
}

// Styles

BOOL CGXStylesMap::LookupStyle(WORD wStyleId, CGXStyle*& pStyle) const
{
	BaseStyleInfo* pInfo;
	BOOL b = m_mwpIdInfo.Lookup(wStyleId, (void*&) pInfo);
	if (b)
	{
		pStyle = pInfo->pStyle;
		ASSERT_VALID(pStyle);
	}
	return b;
}

BOOL CGXStylesMap::IsSystemStyle(WORD wStyleId) const
{
	BaseStyleInfo* pInfo;

	if (m_mwpIdInfo.Lookup(wStyleId, (void*&) pInfo))
	{
		// already existent, change style
		return pInfo->bSystem;
	}

	ASSERT(0);
	// ASSERTION-> Style not existent! ->END

	return FALSE;
}

void CGXStylesMap::RemoveStyle(WORD wStyleId)
{
	ASSERT(!IsSystemStyle(wStyleId));
	// ASSERTION-> Removing system styles is not allowed! ->END

	BaseStyleInfo* pInfo;

	if (m_mwpIdInfo.Lookup(wStyleId, (void*&) pInfo))
	{
		m_mswNameId.RemoveKey(pInfo->sName);
		if (pInfo->nResourceID)
			m_mpwResourceId.RemoveKey((void*) (LONG) pInfo->nResourceID);

		delete pInfo;
		m_mwpIdInfo.RemoveKey(wStyleId);
	}
}

POSITION CGXStylesMap::StartStylesIteration() const
{
	return m_mswNameId.GetStartPosition();
}

void CGXStylesMap::GetNextStyle(POSITION& pos, WORD& wStyleId, CString& sName, UINT& nResourceID, const CGXStyle*& pStyle) const
{
	void* vptr;
	m_mswNameId.GetNextAssoc(pos, sName, vptr);

	wStyleId = (WORD) (LONG) vptr;

	BaseStyleInfo* pInfo = NULL;
	if (m_mwpIdInfo.Lookup(wStyleId, (void*&) pInfo))
	{
		pStyle = pInfo->pStyle;
		nResourceID = pInfo->nResourceID;
	}

	wStyleId = (WORD) (LONG) vptr;
}

void CGXStylesMap::RegisterDefaultControls()
{
	CGXAbstractControlFactory* pControlFactory = GXGetAppData()->m_pControlFactory;

	if (pControlFactory)
		pControlFactory->RegisterAllControls(this);
}

void CGXStylesMap::RegisterDefaultUserAttributes()
{
	CGXAbstractControlFactory* pControlFactory = GXGetAppData()->m_pControlFactory;

	if (pControlFactory)
		pControlFactory->RegisterAllUserAttributes(this);
}

CGXGridCore* CGXStylesMap::Grid() const
{
	return GXGetAppData()->GetFirstGrid((void*) this);
}


