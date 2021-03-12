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

// gxprndev.cpp : serializable printer settings
//

#include "stdafx.h"

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

// Headers

#ifndef _GXEXT_H_
#include "gxext.h"
#endif

#ifndef _AFXPRIV_H_
#include "afxpriv.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXPRNDEV")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CGXPrintDevice, CObject, 1)

#define new DEBUG_NEW

#if _MFC_VER_ <= 0x0250
#define DWPORT  UINT
#else
#define DWPORT  DWORD
#endif

////////////////////////////////////////////////////////////////////////////////
// CGXPrintDevice constructor/destructor

CGXPrintDevice::CGXPrintDevice()
{
	CGXAppData* pAppData = GXGetAppData();

	ASSERT(pAppData->m_bInitDone);
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
	// called you can call m_pPrnDev = new CGXPrintDevice. In your
	// class destructor don't forget to delete m_pPrnDev.
	// ->END

	m_hDevMode = 0;
	m_hDevNames = 0;
	m_hPrintDC = 0;

	m_pPrintDeviceImp = pAppData->m_pPrintDeviceImp;
}

CGXPrintDevice::~CGXPrintDevice()
{
	DestroyDeviceHandles();
}

CGXPrintDevice::CGXPrintDevice(const CGXPrintDevice& pd)
{
	m_hDevMode = 0;
	m_hDevNames = 0;
	CreateDeviceHandles(pd.m_hDevNames, pd.m_hDevMode);
}

CGXPrintDevice& CGXPrintDevice::operator=(const CGXPrintDevice& pd)
{
	if (&pd == this)
		return *this;

	CreateDeviceHandles(pd.m_hDevNames, pd.m_hDevMode);
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
// CGXPrintDevice serialization

void CGXPrintDevice::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

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
			TRACE0("Incompatible format while reading CGXPrintDevice" );
			TRACE2("in %s at line %d\n", __FILE__, __LINE__);
#endif
			AfxThrowArchiveException(CArchiveException::badSchema);
			return;
		}
	}

	LPDEVNAMES pDevNames = 0;
	DWORD cbSizeDevNames = 0;

	LPDEVMODE pDevMode = 0;
	DWORD cbSizeDevMode = 0;

	if (ar.IsStoring())
	{
		GetDeviceInfo(pDevNames, cbSizeDevNames, pDevMode, cbSizeDevMode);

		// DevNames structure
		ar << cbSizeDevNames;
		if (cbSizeDevNames > 0)
			ar.Write(pDevNames, (DWPORT) cbSizeDevNames);

		// DevMode structure
		ar << cbSizeDevMode;
		if (cbSizeDevMode > 0)
			ar.Write(pDevMode, (DWPORT) cbSizeDevMode);

		delete pDevNames;
		delete pDevMode;
	}
	else
	{
		// DevNames structure
		ar >> cbSizeDevNames;
		if (cbSizeDevNames > 0)
		{
			pDevNames = (LPDEVNAMES) new char [cbSizeDevNames];
			ar.Read(pDevNames, (DWPORT) cbSizeDevNames);
		}

		// DevMode structure
		ar >> cbSizeDevMode;
		if (cbSizeDevMode > 0)
		{
			pDevMode = (LPDEVMODE) new char [cbSizeDevMode];
			ar.Read(pDevMode, (DWPORT) cbSizeDevMode);
		}

		// Handles
		CreateDeviceHandles((LPDEVNAMES) pDevNames,
			cbSizeDevNames,
			(LPDEVMODE) pDevMode,
			cbSizeDevMode);

		delete pDevNames;
		delete pDevMode;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGXPrintDevice diagnostics

#ifdef _DEBUG
void CGXPrintDevice::AssertValid() const
{
	CObject::AssertValid();

	ASSERT(!(m_hDevNames == 0 && m_hDevMode != 0));
}
/*
void CGXPrintDevice::Dump(CDumpContext& dc) const
{

	CObject::Dump(dc);
	dc << "\nm_hDevMode = " << (UINT)m_hDevMode;
	dc << "\nm_hDevNames = " << (UINT)m_hDevNames;
	dc << "\nm_hPrintDC = " << (UINT)m_hPrintDC;

}
*/
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGXPrintDevice operations

void CGXPrintDevice::CreateDeviceHandles(
	LPDEVNAMES pDevNames,
	DWORD cbSizeDevNames,
	LPDEVMODE pDevMode,
	DWORD cbSizeDevMode)
{
	if (m_pPrintDeviceImp)
		m_pPrintDeviceImp->CreateDeviceHandles(
			this,
			pDevNames,
			cbSizeDevNames,
			pDevMode,
			cbSizeDevMode);
}

void CGXPrintDevice::CreateDeviceHandles(
	HGLOBAL hDN,
	HGLOBAL hDM)
{
	if (m_pPrintDeviceImp)
		m_pPrintDeviceImp->CreateDeviceHandles(this, hDN, hDM);
}

void CGXPrintDevice::DestroyDeviceHandles()
{
	if (m_pPrintDeviceImp)
		m_pPrintDeviceImp->DestroyDeviceHandles(this);
}

void CGXPrintDevice::GetDeviceInfo(
	HGLOBAL hDN,
	HGLOBAL hDM,
	LPDEVNAMES& pDevNames,
	DWORD &cbSizeDevNames,
	LPDEVMODE& pDevMode,
	DWORD& cbSizeDevMode)
{
	if (m_pPrintDeviceImp)
		m_pPrintDeviceImp->GetDeviceInfo(this, hDN, hDM, pDevNames, cbSizeDevNames, pDevMode, cbSizeDevMode);
}

void CGXPrintDevice::GetDeviceInfo(
	LPDEVNAMES& pDevNames,
	DWORD &cbSizeDevNames,
	LPDEVMODE& pDevMode,
	DWORD& cbSizeDevMode)
{
	GetDeviceInfo(m_hDevNames, m_hDevMode, pDevNames, cbSizeDevNames, pDevMode, cbSizeDevMode);
}

void CGXPrintDevice::GetDefaults()
{
	if (m_pPrintDeviceImp)
		m_pPrintDeviceImp->GetDefaults(this);
}

void CGXPrintDevice::NeedDeviceHandles()
{
	if (m_hDevNames == 0)
		GetDefaults();
}

HDC CGXPrintDevice::GetPrintDC()
{
	GX_CHECKIMP(m_pPrintDeviceImp, "CGXPrintDevice::GetPrintDC");

	if (m_pPrintDeviceImp)
		return m_pPrintDeviceImp->GetPrintDC(this);

	return 0;
}

void CGXPrintDevice::ResetPrintDC()
{
	GX_CHECKIMP(m_pPrintDeviceImp, "CGXPrintDevice::ResetPrintDC");

	if (m_pPrintDeviceImp)
		m_pPrintDeviceImp->ResetPrintDC(this);

	m_hPrintDC = 0;
}

void CGXPrintDevice::CopyDeviceHandles(HGLOBAL& hDN, HGLOBAL& hDM)
{
	if (m_pPrintDeviceImp)
		m_pPrintDeviceImp->CopyDeviceHandles(this, hDN, hDM);
}

BOOL CGXPrintDevice::CompareDeviceHandles(
	LPDEVNAMES pDevNames,
	DWORD cbSizeDevNames,
	LPDEVMODE pDevMode,
	DWORD cbSizeDevMode)
{
	if (m_pPrintDeviceImp)
		return m_pPrintDeviceImp->CompareDeviceHandles(this, 
			pDevNames,
			cbSizeDevNames,
			pDevMode,
			cbSizeDevMode);

	return FALSE;
}

BOOL CGXPrintDevice::CompareDeviceHandles(HGLOBAL hDN, HGLOBAL hDM)
{
	if (m_pPrintDeviceImp)
		return m_pPrintDeviceImp->CompareDeviceHandles(this, hDN, hDM);

	return FALSE;
}
