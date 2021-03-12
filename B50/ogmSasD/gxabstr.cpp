// Author: Stefan Hoenig
// gxabstr.cpp : Function implementations for abstract base classes
//,_MBCS,OGMSASD_EXPORTS AFXEXT
#include "stdafx.h"
#include <stdarg.h>
#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif
// Headers
#ifndef _GXABSTR_H_
#include "gxabstr.h"
#endif
#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXABSTR")
#endif
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
#define new DEBUG_NEW
#if _MFC_VER >= 0x0400 && !defined(_AFX_NO_OLE_SUPPORT)
//////////////////////////////////////////////////////////////////////////////
// CGXNoOleDataSourceProxy
CGXNoOleDataSourceProxy::~CGXNoOleDataSourceProxy(){}
//////////////////////////////////////////////////////////////////////////////
// CGXNoOleDataObjectProxy
CGXNoOleDataObjectProxy::~CGXNoOleDataObjectProxy(){}
#endif
//////////////////////////////////////////////////////////////////////////////
// CGXNoToolTipCtrlProxy
CGXNoToolTipCtrlProxy::~CGXNoToolTipCtrlProxy(){}
//////////////////////////////////////////////////////////////////////////////
// CGXAbstractCommandFactory
CGXAbstractCommandFactory::~CGXAbstractCommandFactory(){}
//////////////////////////////////////////////////////////////////////////////
// Helper for warning about calls unimplemented functions
#ifdef _DEBUG
void AFXAPI GXCheckImp(void* pImp, LPCTSTR pszFunc)
{
	void* vptr;
	if (pImp == NULL && !GXGetAppData()->m_mapNoImpWarnings.Lookup(pszFunc, vptr))
	{
		GXGetAppData()->m_mapNoImpWarnings.SetAt(pszFunc, (void*) pszFunc);
		TRACE(_T("Warning: No concrete implementation for %s.\n"), pszFunc);

		ASSERT(!GXGetAppData()->m_mapAssertNoImp.Lookup(pszFunc, vptr));
		// ASSERTION-> GXSetAssertNoImp caused ASSERT ->END
	}
}
void AFXAPI GXSetAssertNoImp(LPCTSTR pszFunc)
{
	GXGetAppData()->m_mapAssertNoImp.SetAt(pszFunc, (void*) pszFunc);
}
void AFX_CDECL GXTraceOnce(LPCTSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);

	int nBuf;
	TCHAR szBuffer[512];

	nBuf = _vsntprintf(szBuffer, 512, lpszFormat, args);

	// was there an error? was the expanded string too long?
	ASSERT(nBuf >= 0);

	void* vptr;
	if (!GXGetAppData()->m_mapTraceOnce.Lookup(szBuffer, vptr))
	{
		GXGetAppData()->m_mapNoImpWarnings.SetAt(szBuffer, (void*) lpszFormat);
		TRACE(szBuffer);
	}

	va_end(args);
}
#endif //_DEBUG

