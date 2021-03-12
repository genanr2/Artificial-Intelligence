#include "_build.h"
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>          // MFC database classes
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxpriv.h>
#include <ctype.h>
#ifdef _GXDLL
	// needed for MFC 3.0
	#define GX_DATAEXT      AFX_DATA_EXPORT
	#define GX_DATADEF
#endif
// No need to automatically link with OG libs for this build
#define _GXNOAUTOLIB
#define _GXNOADOAUTOLIB
// Don't display message that automatic linking is turned off
#define _GXBUILD
/*
#if !defined(_GXDLL) && !defined(_GXEXT) && _MFC_VER >= 0x0400 && _MFC_VER <= 0x0410
// turn off global optimization for MFC 4.0/4.1
// to prevent Win32s problems with static release builds

#pragma optimize("g", off)
#endif
*/

#include <gxall.h>
/*
#if _GX_VER != 0x0611
#pragma message( "You have tried to compile with old Objective Grid header files." )
#pragma message( "Please make sure that you have correctly setup your include directory!" )
#error Wrong header files.
#endif
#if _MFC_VER < 0x0421
#pragma message( "Objective Grid MSDN Edition requires Visual C++ 5.0." )
#pragma message( "You need to upgrade to the professional version if you need support for VC 1.5 (16-bit)!" )
#error Wrong Visual C version.
#endif
*/
#pragma warning( disable : 4700 )  // use of unitialized data 

