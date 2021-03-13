#define _AFXDLL

#include <afx.h>         // MFC core and standard components

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC OLE automation classes
#endif // _AFX_NO_OLE_SUPPORT
#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT
#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
//#include "..\inc\mrcext.h"
#include <afxpriv.h>		// for WM_INITIALUPDATE
#include <afxcontrolbars.h>
#include <afxsplitterwndex.h>
#include <afxwin.h>
#include <afxwin.h>
#include <afxwin.h>
#include <afxcmn.h>
#include <afxcontrolbars.h>
#include "mylib.h"

static AFX_EXTENSION_MODULE MyLibDLL = { NULL,NULL };
MyLib::MyLib()
{
}

void MyLib::test()
{
    qDebug() << "Hello from our dll";

}
extern "C" void WINAPI InitMyLibDLL() { new CDynLinkLibrary(MyLibDLL); }
