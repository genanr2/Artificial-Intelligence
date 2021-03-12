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

// gxprp.cpp : implementation of the CGXProperties class
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

#ifndef _GXRESRC_H_
#include "gxresrc.h"
#endif

#ifndef _GXEXT_H_
#include "gxext.h"
#endif


#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXPRP")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_SERIAL(CGXProperties, CObject, 0 /* schema number*/ )

#define new DEBUG_NEW

///////////////////////////////////////////////////////////////////
// CGXProperties

// constants
const TCHAR chDelim = _T('.');

// profile entries
LPCTSTR CGXProperties::szMarkRowHeader      = _T("MarkRowHeader");
LPCTSTR CGXProperties::szMarkColHeader      = _T("MarkColumnHeader");
LPCTSTR CGXProperties::szDisplay3DButtons   = _T("3dHeader");
LPCTSTR CGXProperties::szDisplayVertLines   = _T("VerticalLines");
LPCTSTR CGXProperties::szDisplayHorzLines   = _T("HorizontalLines");
LPCTSTR CGXProperties::szDisplayRowHeader   = _T("DisplayRowHeader");
LPCTSTR CGXProperties::szDisplayColHeader   = _T("DisplayColumnHeader");

LPCTSTR CGXProperties::szDefaultSection     = _T("Grid");

LPCTSTR CGXProperties::szPrintVertLines     = _T("Print Vertical Lines");
LPCTSTR CGXProperties::szPrintHorzLines     = _T("Print Horizontal Lines");
LPCTSTR CGXProperties::szBlackWhite         = _T("Print Only Black&White");
LPCTSTR CGXProperties::szRowHeaders         = _T("Print Row Headers");
LPCTSTR CGXProperties::szColHeaders         = _T("Print Column Headers");
LPCTSTR CGXProperties::szCenterVertical     = _T("Center Vertical");
LPCTSTR CGXProperties::szCenterHorizontal   = _T("Center Horizontal");
LPCTSTR CGXProperties::szPageOrder          = _T("Page Order");
LPCTSTR CGXProperties::szMargins            = _T("Margins");
LPCTSTR CGXProperties::szPrintFrame         = _T("Print Frame");

// Header&Footer
LPCTSTR CGXProperties::szFirstPage          = _T("Start Page Numbering");
LPCTSTR CGXProperties::szDistances          = _T("Distances");

CGXProperties::CGXProperties()
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
	// called you can call m_pProp = new CGXProperties. In your
	// class destructor don't forget to delete m_pProp.
	// ->END

	sInvertNormal.LoadString(GX_IDS_INVERTNORMAL);
	sInvertThick.LoadString(GX_IDS_INVERTTHICK);
	sInvertDrawBorder.LoadString(GX_IDS_INVERTDRAWBORDER);
	sInvertThickBorder.LoadString(GX_IDS_INVERTTHICKBORDER);
	sInvertNoBorder.LoadString(GX_IDS_INVERTNOBORDER);

	// Default settings
	m_bDisplayVertLines      = TRUE;
	m_bDisplayHorzLines      = TRUE;
	m_b3dButtons             = TRUE;
	m_bMarkRowHeader         = TRUE;
	m_bMarkColHeader         = TRUE;
	m_bPrinting              = FALSE;
	m_sSection               = szDefaultSection;
	m_nZoom                  = 100;

	// Colors
	AddColorProperty(GX_IDS_COLOR_GRIDLINES, GXSYSCOLOR(COLOR_GRAYTEXT));
	AddColorProperty(GX_IDS_COLOR_FIXEDLINES, GXSYSCOLOR(COLOR_ACTIVECAPTION));
	AddColorProperty(GX_IDS_COLOR_TRACKINGLINE, RGB(255, 0, 0));
	AddColorProperty(GX_IDS_COLOR_DRAGGINGLINE, RGB(255, 0, 0));
	AddColorProperty(GX_IDS_COLOR_BACKGROUND, GXSYSCOLOR(COLOR_BTNFACE));

	ASSERT(m_ColorArray.GetSize() >= 5);
	// ASSERTION-> initializing properties failed ->END

	// Print settings
	m_bPrintVertLines         = TRUE;
	m_bPrintHorzLines         = TRUE;
	m_bBlackWhite             = TRUE;
	m_bRowHeaders             = TRUE;
	m_bColHeaders             = TRUE;
	m_bPrintFrame             = TRUE;

	m_bCenterVertical         = FALSE;
	m_bCenterHorizontal       = TRUE;
	m_nPageOrder              = 0;

	m_nLeftMargin             = INCHtoDP(1);
	m_nRightMargin            = INCHtoDP(1);
	m_nTopMargin              = INCHtoDP(1.5);
	m_nBottomMargin           = INCHtoDP(1.5);

	// Header&Footer
	m_nDistTop                = INCHtoDP(0.4);
	m_nDistBottom             = INCHtoDP(0.4);
	m_nFirstPage              = -1;

	AddToken(_T("$F"));
	AddToken(_T("$A"));
	AddToken(_T("$P"));
	AddToken(_T("$N"));
	AddToken(_T("$D"));
	AddToken(_T("$R"));

	// Header
	m_mapDataHeader.StoreColCount(3);
	m_mapDataHeader.StoreRowCount(NROWS);
	m_mapDataHeader.StoreStyleRowCol(1, 2, CGXStyle()
		.SetValue("$R")
		.SetFont(CGXFont()
			.SetBold(TRUE)
			.SetSize(16)),
		gxCopy
		);

	m_mapDataFooter.StoreColCount(3);
	m_mapDataFooter.StoreRowCount(NROWS);
	m_mapDataFooter.StoreStyleRowCol(1, 1, CGXStyle().SetValue(_T("$D")), gxCopy);
	m_mapDataFooter.StoreStyleRowCol(1, 3, CGXStyle().SetValue(_T("$P/$N")), gxCopy);

	ASSERT_VALID(this);
}

void CGXProperties::AddDefaultUserProperties()
{
	AddUserProperty(GX_IDS_OUTLINECURRENTCELL, CGXStyle()
			.SetControl(GX_IDS_CTRL_CBS_DROPDOWNLIST)
			.SetChoiceList(sInvertNormal + _T("\n")
			+ sInvertThick + _T("\n")
			+ sInvertDrawBorder + _T("\n")
			+ sInvertThickBorder + _T("\n")
			+ sInvertNoBorder )
			.SetValue(sInvertNormal)
		);
}

CGXProperties::CGXProperties(const CGXProperties& p)
{
	m_pAppData = GXGetAppData();

	*this = p;
}

CGXProperties::~CGXProperties()
{
	Free();
}

void CGXProperties::Free()
{
	m_ColorNameArray.RemoveAll();
	m_ColorArray.RemoveAll();

	// user properties
	if (m_UserPropertyInfoMap.GetCount() > 0)
	{
		WORD nID;
		UserPropertyInfo* pInfo;
		POSITION pos = m_UserPropertyInfoMap.GetStartPosition();
		while (pos)
		{
			m_UserPropertyInfoMap.GetNextAssoc(pos, nID, (void*&) pInfo);
			delete pInfo;
		}
	}

	m_UserPropertyInfoMap.RemoveAll();
}

const CGXProperties& CGXProperties::operator=(const CGXProperties& p)
{
	GX_CHECKIMP(m_pAppData->m_pParamCopyOperatorImp, "CGXProperties::operator=");

	if (m_pAppData->m_pParamCopyOperatorImp)
		m_pAppData->m_pParamCopyOperatorImp->CopyObject(this, p);

	return *this;
}

void CGXProperties::Serialize(CArchive& ar)
{
	GX_CHECKIMP(m_pAppData->m_pDocSerializeImp, "CGXProperties::Serialize");

	if (m_pAppData->m_pDocSerializeImp)
		m_pAppData->m_pDocSerializeImp->Serialize(this, ar);
}

#ifdef _DEBUG
void CGXProperties::AssertValid() const
{
	ASSERT(m_ColorNameArray.GetSize() == m_ColorArray.GetSize());
	ASSERT(m_sSection.GetLength() > 0);
}
/*
void CGXProperties::Dump(CDumpContext& dc) const
{

	dc << "\nm_sSection = " << m_sSection;
	dc << "\nm_bPrinting = " << m_bPrinting;
	// Colors
	dc << "\nm_ColorArray\n" << m_ColorArray;
	dc << "\nm_ColorNameArray\n" << m_ColorNameArray;
	dc << "\n";

}
*/
#endif

// Profile
void CGXProperties::WriteProfile() const
{
	GX_CHECKIMP(m_pAppData->m_pGridProfileImp, "CGXProperties::WriteProfile");

	if (m_pAppData->m_pGridProfileImp)
		m_pAppData->m_pGridProfileImp->WriteProfile(this);
}

BOOL CGXProperties::ReadProfile()
{
	GX_CHECKIMP(m_pAppData->m_pGridProfileImp, "CGXProperties::ReadProfile");

	if (m_pAppData->m_pGridProfileImp)
		return m_pAppData->m_pGridProfileImp->ReadProfile(this);

	return FALSE;
}

// Colors
int CGXProperties::AddColorProperty(LPCTSTR pszName, COLORREF color)
{
	ASSERT(pszName != NULL);
	// ASSERTION-> null pointer in argument ->END

	if (pszName)
	{
		for (int i = 0; i < m_ColorNameArray.GetSize(); i++)
		{
			if (m_ColorNameArray.GetAt(i).Compare(pszName) == 0)
				return i;
		}
	}

	const int n = m_ColorNameArray.GetSize();

	m_ColorNameArray.SetAtGrow(n, pszName);
	m_ColorArray.SetAtGrow(n, color);

	ASSERT_VALID(this);

	return n;
}

int CGXProperties::AddColorProperty(UINT nID, COLORREF color)
{
	CString s;
	s.LoadString(nID);

	if (s.IsEmpty())
	{
		TRACE(_T("Warning: String resource for color property id %u not found\n."), nID);
		s.Format(_T("COLOR%u"), nID);
	}

	return AddColorProperty(s, color);
}


COLORREF CGXProperties::GetColor(int nIndex) const
{
	ASSERT(nIndex < m_ColorArray.GetSize());
	// ASSERTION-> index is out of range
	// Did you call AddColorProerty? ->END

	if (nIndex < m_ColorArray.GetSize())
		return m_ColorArray[nIndex];
	return 0;
}

void CGXProperties::SetColor(int nIndex, COLORREF rgbColor)
{
	ASSERT(nIndex < m_ColorArray.GetSize());
	// ASSERTION-> index is out of range
	// Did you call AddColorProerty? ->END

	m_ColorArray[nIndex] = rgbColor;
}

// User Properties
void CGXProperties::AddUserProperty(WORD nID, const CGXStyle& style)
{
	UserPropertyInfo* pInfo;
	if (!m_UserPropertyInfoMap.Lookup(nID, (void*&) pInfo))
	{
		pInfo = new UserPropertyInfo;
		pInfo->nID = nID;
		if (!pInfo->sName.LoadString(nID))
		{
			TRACE(_T("Warning: String resource for user property id %u not found\n."), nID);
			pInfo->sName.Format(_T("USERPROP%u"), nID);
		}
	}
	else
		delete pInfo->pStyle;

	// Duplicate the style object
	pInfo->pStyle = style.Clone();

	m_UserPropertyInfoMap.SetAt(nID, pInfo);
}

void CGXProperties::SetUserProperty(WORD nID, const CGXStyle& style)
{
	UserPropertyInfo* pInfo;

	if (!m_UserPropertyInfoMap.Lookup(nID, (void*&) pInfo))
	{
		ASSERT(0);
		// ASSERTION-> Did you register this property? ->END

		AfxThrowResourceException();
	}

	// copy the style object
	pInfo->pStyle->ChangeStyle(style, gxOverride);
}

const CMapWordToPtr& CGXProperties::GetUserPropertyInfoMap() const
{
	return m_UserPropertyInfoMap;
}

const CGXStyle& CGXProperties::GetUserProperty(WORD nID)
{
	UserPropertyInfo* pInfo;

	if (!m_UserPropertyInfoMap.Lookup(nID, (void*&) pInfo))
	{
		return m_styleEmpty;
		// ASSERT(0);
		// ASSERTION-> Did you register this property? ->END

		// AfxThrowResourceException();
	}
	return *pInfo->pStyle;
}

CGXProperties::UserPropertyInfo::UserPropertyInfo(const UserPropertyInfo& srcInfo)
{
	nID = srcInfo.nID;
	sName = srcInfo.sName;
	pStyle = NULL;
	if (srcInfo.pStyle)
	{
		// Duplicate the style object
		pStyle = srcInfo.pStyle->Clone();
	}
}

// ============ Header&Footer Printing =======================

void CGXProperties::OnPrintHeaderAndFooter(CDC* pDC, CPrintInfo* pInfo, CDocument* pDoc, CGXGridParam* pParam)
{
	CGXGridCore* pGrid = Grid();

	if (pGrid == NULL)
		return;

	GX_CHECKIMP(pGrid->m_pPrintImp, "CGXProperties::OnPrintHeaderAndFooter");

	if (pGrid->m_pPrintImp)
		pGrid->m_pPrintImp->OnPrintHeaderAndFooter(this, pDC, pInfo, pDoc, pParam);
}


int CGXProperties::CalculateHeight(CDC* pDC, CGXData &data, CGXStylesMap* pStylesMap)
{
	CGXGridCore* pGrid = Grid();

	if (pGrid && pGrid->m_pPrintImp)
		return pGrid->m_pPrintImp->CalculateHeight(this, pDC, data, pStylesMap);

	return 0;
}

void CGXProperties::PrintHeaderFooter(CDC* pDC, CPrintInfo* pInfo, CRect& rect, CGXData& data, CGXStylesMap* pStylesMap)
{
	CGXGridCore* pGrid = Grid();

	if (pGrid == NULL)
		return;

	GX_CHECKIMP(pGrid->m_pPrintImp, "CGXProperties::PrintHeaderFooter");

	if (pGrid->m_pPrintImp)
		pGrid->m_pPrintImp->PrintHeaderFooter(this, pDC, pInfo, rect, data, pStylesMap);
}

CString CGXProperties::ParseText(LPCTSTR sz)
{
	CGXGridCore* pGrid = Grid();

	if (pGrid && pGrid->m_pPrintImp)
		return pGrid->m_pPrintImp->ParseText(this, sz);

	return _T("");
}

int CGXProperties::AddToken(LPCTSTR sz)
{
	int n = m_sTokenArray.GetSize();
	m_sTokenArray.SetAtGrow(n, sz);
	return n;
}

CString CGXProperties::SubstTokenText(int nToken, CString& sRest)
{
	CGXGridCore* pGrid = Grid();

	if (pGrid && pGrid->m_pPrintImp)
		return pGrid->m_pPrintImp->SubstTokenText(this, nToken, sRest);

	return _T("");
}

CString CGXProperties::GetTokenArgs(CString& sRest)
{
	CGXGridCore* pGrid = Grid();

	if (pGrid && pGrid->m_pPrintImp)
		return pGrid->m_pPrintImp->GetTokenArgs(this, sRest);

	return _T("");
}


CGXGridCore* CGXProperties::Grid() const
{
	return m_pAppData->GetFirstGrid((void*) this);
}

