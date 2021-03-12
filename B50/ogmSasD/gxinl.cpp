// Author: Stefan Hoenig
// gxinl.cpp : Expansion of inline methods
#include "stdafx.h"
/*
// compile this file only
// a) when inline expansions have not been included 
//    in header file (ndef _GX_ENABLE_INLINES)
// b) and inline functions shall not be left out (e.g. when 
//    creating .DEF files - ndef _GX_DISABLE_INLINES)
*/
#if !defined(_GX_ENABLE_INLINES) && !defined(_GX_DISABLE_INLINES)
#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif
// Headers
#ifndef _GXCORE_H_
#include "gxcore.h"
#endif
#ifndef _GXSTYLES_H_
#include "gxstyles.h"
#endif
#ifndef _GXEXT_H_
#include "gxext.h"
#endif
#ifndef _GXCOLL_H_
#include "gxcoll.h"
#endif
#ifndef _GXDELAYR_H_
#include "gxdelayr.h"
#endif
#ifndef _GXBROWSE_H_
#include "gxbrowse.h"
#endif
#ifndef _GXVW_H_
#include "gxvw.h"
#endif
#ifndef _GXWND_H_
#include "gxwnd.h"
#endif
#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif
#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXINL")
#endif
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
#define new DEBUG_NEW
#define GX_INLINE
#include "gxcore.inl"
#include "gxstyles.inl"
#include "gxext.inl"
#include "gxcoll.inl"
#include "gxdelayr.inl"
#include "gxbrowse.inl"
#include "gxvw.inl"
#include "gxwnd.inl"
#include "gxdtcal.inl"

#endif //!_GX_ENABLE_INLINES
