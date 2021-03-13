// MRCEXT.CPP
// $Date:   27 Jun 1997 12:13:36  $
// $Revision:   1.1  $
// $Author:   SJR  $
// mrcext.cpp : Defines the initialization routines for the DLL.
#include "mrcstafx.h"
#include <afxdllx.h>
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[]=__FILE__;
#endif
static AFX_EXTENSION_MODULE mrcextDLL={NULL,NULL};
extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if(dwReason==DLL_PROCESS_ATTACH)
  {
//		TRACE0("MRCEXT.DLL Initializing! - (C)1995 Micro Focus Ltd\n");
		// Extension DLL one-time initialization
		AfxInitExtensionModule(mrcextDLL, hInstance);
		// Insert this DLL into the resource chain
		new CDynLinkLibrary(mrcextDLL);
  }
  else if(dwReason==DLL_PROCESS_DETACH)
  {
//		TRACE0("MRCEXT.DLL Terminating!\n");
		AfxTermExtensionModule(mrcextDLL);
	}return 1;   // ok
}
static CMapPtrToPtr ObjExtDataMap;
void*AllocObjExtData(CObject*pObj,int cBytes)
{
	void*pVoid;
	ASSERT(ObjExtDataMap.Lookup(pObj,pVoid)==FALSE);
	pVoid=new BYTE[cBytes];
	ObjExtDataMap.SetAt(pObj,pVoid);return pVoid;
}
void*GetObjExtDataPtr(CObject*pObj)
{
	void*pVoid;VERIFY(ObjExtDataMap.Lookup(pObj,pVoid));return pVoid;	
}
void DeleteObjExtData(CObject*pObj)
{
	void*pVoid;VERIFY(ObjExtDataMap.Lookup(pObj, pVoid));delete[] pVoid;
}
