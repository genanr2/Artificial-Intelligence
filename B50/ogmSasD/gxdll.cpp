// Author: Stefan Hoenig
// gxdll.cpp: extension dll initialization
#include "stdafx.h"
#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif
// Headers
#ifdef _GXDLL
#ifndef _GXDLL_H_
#include "gxdll.h"
#endif
#ifndef _GXSTATE_H_
#include "gxstate.h"
#endif
#ifndef _GX_PROC_H__
#include "gxproc.h"
#endif
#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif
#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXDLL")
#endif
#if _MFC_VER >= 0x0400 && _MFC_VER <= 0x0410
// turn off global optimization for this file 
// to prevent Win32s problems in release build
#pragma optimize("g", off)
#endif
#if _MFC_VER < 0x0400 // Win16
class CGXDynLinkLibrary: public CDynLinkLibrary
{
	DECLARE_DYNAMIC(CGXDynLinkLibrary)
public:
	CGXDynLinkLibrary(AFX_EXTENSION_MODULE& state);

private:
	GX_APP_STATE m_gxAppState;
	friend GX_APP_STATE* AFXAPI GXGetAppState();
};
#endif// Win16
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
#if _MFC_VER <= 0x0200
	#ifdef _DEBUG
		LPCTSTR szLibName = _T("GX6116D.DLL");
	#else
		LPCTSTR szLibName = _T("GX6116R.DLL");
	#endif
#else _
	#ifdef _UNICODE
		#ifdef _DEBUG
			LPCTSTR szLibName = _T("ogmsasud.DLL");
		#else
			LPCTSTR szLibName = _T("ogmsasu.DLL");
		#endif
	#else
		#ifdef _DEBUG
			LPCTSTR szLibName = _T("ogmsasd.DLL");
		#else
			LPCTSTR szLibName = _T("ogmsas.DLL");
		#endif
	#endif
#endif //16-bits
/////////////////////////////////////////////////////////////////////////////
// Initialization of MFC Extension DLL
#include "afxdllx.h"    // standard MFC Extension DLL routines
static AFX_EXTENSION_MODULE NEAR gxExtensionDLL = { NULL, NULL };
static CDynLinkLibrary* s_pDLL;
//HINSTANCE AFXAPI GXGetResourceHandle()
HINSTANCE AFXAPI GXGetResourceHandle()
//HINSTANCE __declspec(dllexport)GXGetResourceHandle()
{
#if _MFC_VER >= 0x0300
	ASSERT(gxExtensionDLL.hResource != NULL);
	return gxExtensionDLL.hResource;
#else
	return gxExtensionDLL.hModule;
#endif
}
#if _MFC_VER >= 0x0300 // Win32
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE1("%s Initializing!\n", szLibName);
		// Extension DLL one-time initialization
		AfxInitExtensionModule(gxExtensionDLL, hInstance);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE1("%s Terminating!\n", szLibName);
		AfxTermExtensionModule(gxExtensionDLL, FALSE);
		s_pDLL = NULL;
	}
	// unused_
	lpReserved;
	return 1;   // ok
}
//extern "C" void  GXInitDll()
//extern "C" APIENTRY void WINAPI GXInitDll()
//void __declspec(dllexport)GXInitDll()
extern "C" extern void WINAPI GXInitDll()
{
	// Insert this DLL into the resource chain
#if _MFC_VER < 0x0400
	s_pDLL = new CGXDynLinkLibrary(gxExtensionDLL);
#else
	s_pDLL = new CDynLinkLibrary(gxExtensionDLL);
#endif
}
#else // Win16
/*
extern "C" int CALLBACK LibMain(HINSTANCE hInstance, WORD, WORD, LPSTR)
{
	TRACE1("%s Initializing!\n", szLibName);
	// Extension DLL one-time initialization
	AfxInitExtensionModule(gxExtensionDLL, hInstance);
	return 1;
}
extern "C" extern void WINAPI GXInitDll()
{
	// Insert this DLL into the resource chain
	s_pDLL = new CGXDynLinkLibrary(gxExtensionDLL);
}
*/
#endif // Win16
/////////////////////////////////////////////////////////////////////////////
// GX_APP_STATE: Global for Data for Instance
GX_APP_STATE::GX_APP_STATE()
{
	m_pAppData = new CGXAppData;m_pLangDll = NULL;
}
GX_APP_STATE::~GX_APP_STATE()
{
	delete m_pAppData;
#if _MFC_VER >= 0x0400
	delete m_pLangDll; // Language Dll
#else
	// Process local data for VC 1,5
	GXDeleteAllObjects(m_mapProcessLocalData);
#endif
}
#if _MFC_VER >= 0x0420 // Win32
// special class for automatic cleanup AFX_EXT_CLASS
class  CGXAutoCleanUpProcessLocal: public CGXProcessLocal<GX_APP_STATE> 
{
public:
	CGXAutoCleanUpProcessLocal();
	~CGXAutoCleanUpProcessLocal();
// grid specific clean up function
	virtual void AutoTerminate(AFX_MODULE_STATE* pModuleState, GX_APP_STATE* pState);
	virtual void ForceTerminate(GX_APP_STATE* pState = NULL);
};
CGXAutoCleanUpProcessLocal::CGXAutoCleanUpProcessLocal(){}
CGXAutoCleanUpProcessLocal::~CGXAutoCleanUpProcessLocal()
{
		GX_APP_STATE* pData = NULL;
		AFX_MODULE_STATE* pState = NULL;
		//ASSERT(0);
		for( POSITION pos = m_ObjectMap.GetStartPosition(); pos != NULL; )    
		{
			m_ObjectMap.GetNextAssoc( pos, pState, pData);
#ifdef _DEBUG
//			afxDump << "~CGXProcessLocal called\n";
//			afxDump << pState << " : Data value" << pData << "\n";
			// if you get this TRACE message then something is wrong with the state
			// of the object map that CGXAutoCleanUpProcessLocal holds. The module state
			// needs to be non zero and valid at clean up time. For Ole control modules that
			// are unloaded by COM please use ForceTerminate with the correct module state.
			if(!pState)
				TRACE0("Invalid module state in CGXAutoCleanUpProcessLocal object map\n");
#endif  //_DEBUG
			//clean up and delete the object
			if(pData && pState)
			{
				// ensures that the correct module state is set
				AutoTerminate(pState, pData);
				delete pData;
			}
		}
		m_ObjectMap.RemoveAll();
		m_pObject = NULL;
}
// grid specific clean up function
void CGXAutoCleanUpProcessLocal::AutoTerminate(AFX_MODULE_STATE* pModuleState, GX_APP_STATE* pState)
{
	// it is important that this module state is valid. This is no problem with 
	// applications, extension dlls and regular dll but is to be carefully considered
	// with Ole controls
	AFX_MANAGE_STATE(pModuleState);
	ForceTerminate(pState);
}
// force terminate assumes that the module state has been correctly set. Call this function
// when Ole Control modules are terminated. In general this function is to be called when
// something that has an unique module state is terminated
void CGXAutoCleanUpProcessLocal::ForceTerminate(GX_APP_STATE* pState)
{
	if(pState == NULL)pState = GetData();
	if(pState == NULL)
	{
		TRACE0("Call to CGXAutoCleanUpProcessLocal::ForceTerminate fails\n");
		TRACE0("You can ignore this warning if you are simply calling GXTerminate\n");
		TRACE0("You no longer need the call to GXTerminate except with OLE controls\n");
		// Did you set the correct module state with AFX_MANAGE_STATE before calling
		// ForceTerminate? See also "Shared application data problem" below.
		return;
	}
	CGXAppData* pData = pState->m_pAppData;
	if(pData == NULL)
	{
		TRACE0("Call to CGXAutoCleanUpProcessLocal::ForceTerminate fails\n");
		TRACE0("You can ignore this warning if you are simply calling GXTerminate\n");
		TRACE0("You no longer need the call to GXTerminate except with OLE controls\n");
		// Did you set the correct module state with AFX_MANAGE_STATE before calling
		// ForceTerminate? See also "Shared application data problem" below.
		return;
	}
	GXTerminate(pData);
}
/*
Shared application data problem:
--------------------------------
The details in this section do not affect normal applications. They only affect more
complex applications that link to the grid including ActiveX controls and extension/regular dlls.
Scenario with shared data and problems with earlier versions of OG:
-------------------------------------------------------------------
Here we illustrate a typical scenario where this problem would occur.
There are two Active-X controls that use the grid. Both call GXInit and GXTerminate.
In VB(for example) what happens is this
Control1 loads up and calls GXInit. 
Control2 loads up and calls GXInit overwriting the appstate.
We explicitly unload one control. Now the issue is that on NT VB does
not really unload the control but holds on to it and that is the reason
we don't usually have the crash on NT (But if we follow the same
procedure with the control test container it unloads the control and
calls GXTerminate.) Since the app data is shared this causes the other
control to crash (This usually happens when we try to drag a column
because of window classes being un registered).
Solution implemented in 6.1:
------------------------------
The app state that we had earlier was shared betwen different modules and was
declared as process local. So though we get a copy of the appstate
for every unique process we share the same with different modules in the
same process. To get around this the app state was mapped to
the module state of each module. We do this by deriving from
CProcessLocalObject and implementing our own process local data.
 
CGXProcessLocal maintains a map betwen the module state and the
appstate. It always looks up the appstate in this map and returns the
value that corresponds to the module state. The public interface remains
the same. The grid code also remains the same except for the macro
change from PROCESS_LOCAL to GXPROCESS_LOCAL to use CGXProcessLocal.
inline TYPE* GetData()
	{
		//TRACE0("CGXProcessLocal invoked\n");
		TYPE* pData = NULL;
		if(m_ObjectMap.Lookup(AfxGetModuleState(), pData))
		{
			//TRACE1("CGXProcessLocal::Module State %p\n", AfxGetModuleState());
			//TRACE1("CGXProcessLocal::Returning old object %p\n", pData);
			return pData;
		}
		pData = (TYPE*)GetData(&CreateObject);
		ASSERT(pData != NULL);
		m_ObjectMap.SetAt(AfxGetModuleState(), pData);
		//TRACE1("CGXProcessLocal::Module State %p\n", AfxGetModuleState());
		//TRACE1("CGXProcessLocal::Creating new object %p\n", pData);
		return pData;
	}
The cast operators always call GetData:
 inline operator TYPE*(){ return GetData(); }
 inline TYPE* operator->(){ return GetData(); }
We also made some changes to accomodate cases where the application and one or more regular
or extension dlls (these have the same module state) that it uses depend on the OG extension dll.
With these changes we can avoid the need for regular applications,extension dlls and regular dlls 
to call GXTerminate. Only Active-X controls will need to call GXTerminate.
Clean up in the case of default applications is done with a call from the desctructor for 
CGXAutoCleanUpProcessLocal. This class derives from CGXProcessLocal and is specific to GX_APP_STATE.
It passes the module state and the appstate to AutoTerminate which then sets the correct module state and calls ForceTerminate.
ForceTerminate does whatever GXTerminate does (in fact it calls through).
CGXAutoCleanUpProcessLocal::~CGXAutoCleanUpProcessLocal()
{
		GX_APP_STATE* pData = NULL;
		AFX_MODULE_STATE* pState = NULL;
		for( POSITION pos = m_ObjectMap.GetStartPosition(); pos != NULL; )    
		{
			m_ObjectMap.GetNextAssoc( pos, pState, pData);
#ifdef _DEBUG
			afxDump << "~CGXProcessLocal called\n";
			afxDump << pState << " : Data value" << pData << "\n";
			// if you get this TRACE message then something is wrong with the state
			// of the object map that CGXAutoCleanUpProcessLocal holds. The module state
			// needs to be non zero and valid at clean up time. For Ole control modules that
			// are unloaded by COM please use ForceTerminate with the correct module state.
			if(!pState)
				TRACE0("Invalid module state in CGXAutoCleanUpProcessLocal object map\n");
#endif  //_DEBUG

			//clean up and delete the object
			if(pData && pState)
			{
				// ensures that the correct module state is set
				AutoTerminate(pState, pData);
				delete pData;
			}
		}
		m_ObjectMap.RemoveAll();
		m_pObject = NULL;
}
// grid specific clean up function
void CGXAutoCleanUpProcessLocal::AutoTerminate(AFX_MODULE_STATE* pModuleState, GX_APP_STATE* pState)
{
	// it is important that this module state is valid. This is no problem with 
	// applications, extension dlls and regular dll but is to be carefully considered
	// with Ole controls
	AFX_MANAGE_STATE(pModuleState);
	ForceTerminate(pState);
}
// force terminate assumes that the module state has been correctly set. Call this function
// when Ole Control modules are terminated. In general this function is to be called when
// something that has an unique module state is terminated
void CGXAutoCleanUpProcessLocal::ForceTerminate(GX_APP_STATE* pState)
{
	if(pState == NULL)pState = GetData();
	if(pState == NULL)
	{
		TRACE0("Call to CGXAutoCleanUpProcessLocal::ForceTerminate fails\n");
		TRACE0("You can ignore this warning if you are simply calling GXTerminate\n");
		TRACE0("You no longer need the call to GXTerminate except with OLE controls\n");
		// Did you set the correct module state with AFX_MANAGE_STATE before calling
		// ForceTerminate?
		return;
	}
	CGXAppData* pData = pState->m_pAppData;
	if(pData == NULL)
	{
		TRACE0("Call to CGXAutoCleanUpProcessLocal::ForceTerminate fails\n");
		// Did you set the correct module state with AFX_MANAGE_STATE before calling
		// ForceTerminate?
		TRACE0("You can ignore this warning if you are simply calling GXTerminate\n");
		TRACE0("You no longer need the call to GXTerminate except with OLE controls\n");
		return;
	}GXTerminate(pData);
}
If you have an application that calls GXTerminate...
------------------------------------------------------
You can
a) Safely remove this call
b) Leave it as it is. In this case you will see the following TRACE message
		TRACE0("Call to CGXAutoCleanUpProcessLocal::ForceTerminate fails\n");
		TRACE0("You can ignore this warning if you are simply calling GXTerminate\n");
		TRACE0("You no longer need the call to GXTerminate except with OLE controls\n");
You can ignore this message.
If you have an application that links with multiple extension dlls that use OG or if the application
-----------------------------------------------------------------------------------------------------
itself uses OG in addition to the DLL.
--------------------------------------
Remove calls to GXTerminate as this will only cause problems.
*/
// used for automatic clean up
AFX_DATADEF CGXAutoCleanUpProcessLocal _gxAppState;
extern "C" extern void WINAPI GXForceTerminate(){_gxAppState.ForceTerminate();}
GX_APP_STATE* AFXAPI GXGetAppState(){return _gxAppState;}
#elif _MFC_VER >= 0x0400 // Win32
PROCESS_LOCAL(GX_APP_STATE, _gxAppState)
GX_APP_STATE* AFXAPI GXGetAppState(){return _gxAppState;}
#else // 16 - Bit
GX_APP_STATE* AFXAPI GXGetAppState()
{
	// Find CDynLinkLibrary for this library
	CDynLinkLibrary* pDll;
	for(pDll=_AfxGetAppData()->pFirstDLL;pDll&&pDll->m_hModule!=gxExtensionDLL.hModule;
		pDll = pDll->m_pNextDLL)NULL;
	return &((CGXDynLinkLibrary*) pDll)->m_gxAppState;
}
#endif // 16 - Bit
/////////////////////////////////////////////////////////////////////////////
// CGXDynLinkLibrary application specific data
#if _MFC_VER < 0x0400 // Win16
IMPLEMENT_DYNAMIC(CGXDynLinkLibrary, CDynLinkLibrary);
CGXDynLinkLibrary::CGXDynLinkLibrary(AFX_EXTENSION_MODULE&state):CDynLinkLibrary(state){}
#endif
/////////////////////////////////////////////////////////////////////////////
// Support for OG language Dll's
#if _MFC_VER >= 0x0400 // Win32
CGXLangDll::CGXLangDll(){m_hLangDll=m_hOldLang=m_hOldRes=NULL;}
BOOL CGXLangDll::Load(LPCTSTR pszLang, LPCTSTR pszPath)
{
	BOOL bOK = FALSE;
	TCHAR szLangDLL[_MAX_PATH+14];  // max_path + \ + 8.3 name + 0-term
	if(m_hLangDll)Free();// free previous loaded language
	if (s_pDLL && pszLang)          // do not load if Grid dll not loaded
	{
		if (pszPath)
		{
			_tcsncpy(szLangDLL, pszPath, sizeof(szLangDLL)-14);
			szLangDLL[sizeof(szLangDLL)-14] = 0;
			if (szLangDLL[_tcsclen(szLangDLL)-1]!='\\')_tcscat(szLangDLL, _T("\\"));
		}
		else szLangDLL[0]=0;
		_tcscat(szLangDLL, _T("GX61"));
		_tcscat(szLangDLL, pszLang);
		_tcscat(szLangDLL, _T(".DLL"));  // example : GX61NLD.DLL
		HINSTANCE hLangDLL = AfxLoadLibrary(szLangDLL);
		if (hLangDLL)               // loaded successfully
		{
			m_hOldLang = gxExtensionDLL.hResource;  // remember old hResource
			m_hOldRes  = s_pDLL->m_hResource;
			m_hLangDll =                // need handle to free dll
			gxExtensionDLL.hResource =  // modify return-value for GXGetResourceHandle()
			s_pDLL->m_hResource =       // modify return-value for AfxFindResourceHandle()
			hLangDLL;
			GXGetAppData()->LoadResources(); // reload resources
			bOK=TRUE;
		}
	}return bOK;
}
BOOL CGXLangDll::Free()
{
	BOOL bOK = FALSE;
	if (m_hLangDll && AfxFreeLibrary(m_hLangDll)) // free language dll
	{
		if (s_pDLL) // restore previous values if Grid dll still loaded
		{
			gxExtensionDLL.hResource = m_hOldLang;
			s_pDLL->m_hResource = m_hOldRes;
			GXGetAppData()->LoadResources(); // reload resources
		}
		m_hLangDll = m_hOldLang = m_hOldRes = NULL;	bOK = TRUE;
	}return bOK;
}
CGXLangDll::~CGXLangDll(){Free();}
#endif // _MFC_VER >= 0x0400
#endif // _GXDLL
