









#include "stdafx.h"




//#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
//#include <afxwin.h>         // MFC core and standard components
//#include <afxext.h>         // MFC extensions
//#include <afxole.h>         // MFC OLE classes
//#include <afxodlgs.h>       // MFC OLE dialog classes
//#include <afxdisp.h>        // MFC OLE automation classes
//#include <afxdao.h>			// MFC DAO database classes
//#ifndef _AFX_NO_AFXCMN_SUPPORT
//#include <afxcmn.h>			// MFC support for Windows Common Controls
//#endif // _AFX_NO_AFXCMN_SUPPORT
//#include ".\inc\mrcext.h"
//#include <afxpriv.h>		// for WM_INITIALUPDATE








#include ".\B54MDLL\afxcontrolbars2.h"
#include ".\inc\mrcext.h"
#include <gxall.h>          // Objective Grid classes
//#include <nd.h>
#if !defined( __KBASE_H )
#include ".\know32\kBase.h"
#endif
//#include ".\MyLib3\mylib3.h"

#include ".\DataGrid\DaoView.h"
#include ".\DataGrid\daoFrm.h"
#include ".\DataGrid\GridMDIChild.h"
#include ".\DataGrid\GridDataView.h"
//#include ".\DataGrid\DaovwDoc.h"		// Document
//#include ".\DataGrid\TreeView.h"		// Left hand side view
//#include ".\know32\MyRuleBar.h"
//#include ".\QtClassLibrary1\QtClassLibrary1\qtclasslibrary1_global.h"
#include "b50.h"
//#define __B54m_H
//#include ".\b54mDll\b54mdll.h"
extern "C" AFX_EXT_API void WINAPI Initb54mdllDLL();
extern "C" AFX_EXT_API void WINAPI InitDataGridDLL();
extern "C" AFX_EXT_API void WINAPI InitMyClassL();
//extern "C" AFX_EXT_API void WINAPI MyLibDLL();
//extern "C" void WINAPI Initb54mdllDLL() { new CDynLinkLibrary(b54mdllDLL); }

#include "MainFrm.h"
#include "ChildFrm.h"
#include "b50Set.h"
#include "b50Doc.h"
#include "b50View.h"
#include "HelloFrm.h"
#include "HelloDoc.h"
#include "HelloVw.h"
#include "KohFrm.h"
#include "KohDoc.h"
#include "KohVw.h"
#include ".\know32\AtomViewFrame.h"
#include ".\know32\AtomViewDoc.h"
#include ".\know32\AtomTreeView.h"
//#include <gxall.h>          // Objective Grid classes
#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif
#ifndef _STRING_
#include "string"
#endif
#if !defined( __KBASE_H )
#include ".\know32\kBase.h"
#endif
#if !defined( __STRATEGY_H )
#include ".\know32\Strategy.h"
#endif  // __STRATEGY_H
#if !defined( __NXP_COMPILE_H )
#include ".\know32\Nxp_Comp.h"
#endif
#if !defined(__BaseList_H)
#include ".\know32\Bases.h"
#endif
#include ".\DataGrid\DataGridView.h"
#include ".\DataGrid\DataGridDoc.h"
#include ".\DataGrid\DataGridChild.h"
#include "locale.h"

#define ERR_LIB B50
//#define MCH_WIN  MCH_WINPM
#define MCH_OS MCH_OSWIN32
#undef APP_CONSOLE
#include <nd.h>
#include <WinPub.h>



#include <dgrampub.h>
#include <iconpub.h>
#include <mbarpub.h>
#include <menupub.h>
#include <rclaspub.h>
#include <respub.h>
#include <rlibpub.h>
#include <tbutpub.h>
#include <winpub.h>
// )) CodeGen: Includes 16238
// (( CodeGen: CustomIncludes
// )) CodeGen: CustomIncludes 0
#include <pointpub.h>
#include <rectpub.h>
#include <rgnpub.h>
#include <filewpub.h>


//#include ".\Prefrences\PreferencesDlg.h"



//#include ".\DllImport\PrinceDll.h"




using namespace std;
//using namespace q
#include "NxpChild.h"
//#include "NxpDoc.h"
#include ".\know32\kbasedoc.h"
#include ".\b54mdll\NxpView.h"
#include "SizeViewBar.h"
//#include ".\b54mdll\traceapi.h"
#include ".\b54mdll\resource.h"
#include ".\DataGrid\resource.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

/* комментарий от 28.01.2019

#ifdef PRINCEDLL_EXPORTS
#define PRINCEDLL_API __declspec(dllexport)
#else
#define PRINCEDLL_API __declspec(dllimport)
#endif


# if defined(QTCLASSLIBRARY1_LIB)
#  define QTCLASSLIBRARY1_EXPORT __declspec(dllexport)//Q_DECL_EXPORT
# else
#  define QTCLASSLIBRARY1_EXPORT __declspec(dllimport) //Q_DECL_IMPORT
# endif

*/















//#else
//# define QTCLASSLIBRARY1_EXPORT
//#endif
//#include ".\QtClassLibrary1\QtClassLibrary1\QtClassLibrary1.h"



/* комментарий от 28.01.2019
extern "C" PRINCEDLL_API HRESULT APIENTRY InitLibrary();
*/
//extern "C" AFX_EXT_API HRESULT WINAPI InitLibrary();

//extern "C" AFX_EXT_API void WINAPI Initb54mdllDLL();




//extern PRINCEDLL_API HRESULT APIENTRY InitLibrary();
//extern "C" AFX_EXT_API void WINAPI InitLibrary();
//PRINCEDLL_API HRESULT APIENTRY InitLibrary()


/* комментарий от 28.01.2019

extern "C" PRINCEDLL_API HRESULT APIENTRY SetFaceTracking(long boolValue);
extern "C" PRINCEDLL_API HRESULT APIENTRY GetFaceTracking(long *boolValue);
*/

/* комментарий от 28.01.2019
QTCLASSLIBRARY1_EXPORT QtClassLibrary1;// ::QtClassLibrary1()
*/



//#include "dgramex.h"
//ERR_EXTERN
//#include "resource.h"       // main symbols istream
//extern class RUNTIME_CLASS NxpChild;
//extern class RUNTIME_CLASS NxpChild;
//extern AFX_EXT_CLASS NxpChild;
BEGIN_MESSAGE_MAP(CB50App, CWinApp)
	//{{AFX_MSG_MAP(CB50App)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_DAOVIEW,OnDaoView)
	ON_COMMAND(ID_FILE_HELLONEW,OnHelloView)
	ON_COMMAND(IDM_LEARNING,OnKohonen)
	ON_COMMAND(IDM_Nexpert,OnNxpDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW,OnNxpDoc)//OnOleDoc)//CWinApp::OnFileNew)
	ON_COMMAND(idd_ole_dao_vew,OnOleDoc)
	
	ON_COMMAND(ID_FILE_OPEN,CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()
CB50App::CB50App()
{
//	m_bShowSystemObjects=FALSE;m_bShowWarnings=TRUE;m_bOpenODBC=FALSE;m_nMaxRecords=100;
}
CB50App::~CB50App(){
//	ND::Exit();
}
CB50App theApp;
// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.
// {350A2776-BFEC-11D1-A093-00A024384FCF}
static const CLSID clsid ={0x350a2776,0xbfec,0x11d1,{0xa0,0x93,0x0,0xa0,0x24,0x38,0x4f,0xcf}};
void C_FAR APP_InitFirst L0()
{
//	S_HandShake();
}									
void C_FAR APP_InitLast L0(){} 
void C_FAR APP_Exit L0(){}
BOOL CB50App::InitInstance()
{
	int msgboxID;// = MessageBox(NULL, "Запускать оболочку Neuron Data?", "", MB_ICONWARNING | MB_YESNO);// MB_CANCELTRYCONTINUE);
	msgboxID = IDNO;
	// Initialize OLE libraries
	if(!AfxOleInit()){AfxMessageBox(IDP_OLE_INIT_FAILED);return FALSE;}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Expert System"));
	LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	InitDataGridDLL();
	Initb54mdllDLL();
//	InitMyClassL();


	GXInit();       // initializes library and resources
	GXSetNewGridLineMode(TRUE);	// use smarter grid lines (and dotted) 
	_tsetlocale(LC_ALL,"rus.1251");
	int argc=0;
	char**argv=NULL;
	bool cc = false;
	//int msgboxID = MessageBox(NULL, "Запускать оболочку Neuron Data?", "", MB_ICONWARNING | MB_YESNO);// MB_CANCELTRYCONTINUE);
	if(msgboxID== IDYES)
		ND::Init(argc,argv);

	/* комментарий от 28.01.2019
		InitLibrary();
*/

//	QtClassLibrary1 *qt1 = new QtClassLibrary1();
//	QtClassLibrary1 qt1();// = new QtClassLibrary1();
//	qt1.
//	NDWin::

//	va_list args;
//	CStr fmt;
//	Str buf;
//	va_start(args, fmt);
//	NDStr::Vsprintf(buf, fmt, args);
//	NDWin
//	NDWin::NfyInit();
//	NDWin::NfyInit();
//	ND::Run();
	CMultiDocTemplate*pNexpertTemplate;
	pNexpertTemplate=new CMultiDocTemplate(NExpert_menu,RUNTIME_CLASS(KBaseDoc),RUNTIME_CLASS(NxpChild),RUNTIME_CLASS(NxpView));
//	pNexpertTemplate=new CSizeBarDocTemplate(NExpert_menu,RUNTIME_CLASS(NxpDoc),RUNTIME_CLASS(NxpChild),RUNTIME_CLASS(NxpView));
//	pKohonenTemplate->SetContainerInfo(IDR_B54MTYPE_CNTR_IP);
	AddDocTemplate(pNexpertTemplate);

	CMultiDocTemplate*pDocTemplate;
	pDocTemplate=new CMultiDocTemplate(IDR_B50TYPE,RUNTIME_CLASS(CB50Doc),RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CB50View));
	pDocTemplate->SetContainerInfo(IDR_B50TYPE_CNTR_IP);
	AddDocTemplate(pDocTemplate);

	CMultiDocTemplate*pDaoTemplate;
	pDaoTemplate=new CMultiDocTemplate(IDR_DAOVIEW,RUNTIME_CLASS(CGridDoc),RUNTIME_CLASS(CDaoFrame),RUNTIME_CLASS(CDaoTreeView));
	AddDocTemplate(pDaoTemplate); //namespace

	CMultiDocTemplate*pDocTemplate1;
	pDocTemplate1=new CMultiDocTemplate(IDR_HELLOTYPE,RUNTIME_CLASS(CHelloDoc),RUNTIME_CLASS(CHelloFrame),RUNTIME_CLASS(CHelloView));
	AddDocTemplate(pDocTemplate1);

	CMultiDocTemplate*pKohonenTemplate;
	pKohonenTemplate=new CMultiDocTemplate(IDM_KOHONEN_CHILD,RUNTIME_CLASS(CKohonenDoc),RUNTIME_CLASS(CKohonenFrame),RUNTIME_CLASS(CKohonenView));
//	pKohonenTemplate->SetContainerInfo(IDR_B54MTYPE_CNTR_IP);
	AddDocTemplate(pKohonenTemplate);

	CMultiDocTemplate*atomsView;
	atomsView=new CMultiDocTemplate(IDR_ATOMS_VIEW,RUNTIME_CLASS(AtomViewDoc),RUNTIME_CLASS(AtomViewFrame),RUNTIME_CLASS(AtomTreeView));
	AddDocTemplate(atomsView); //namespace

	CMultiDocTemplate*GridTemplate;
	GridTemplate=new CMultiDocTemplate(Grid_Data_Res,RUNTIME_CLASS(DataGridDoc),RUNTIME_CLASS(DataGridChild),RUNTIME_CLASS(DataGridView));
	AddDocTemplate(GridTemplate);
	CMultiDocTemplate*gridDoc=new CMultiDocTemplate(IDR_GRID_DATA,RUNTIME_CLASS(CGridDoc),//CGridSampleDoc),
			RUNTIME_CLASS(GridMDIChild),/*CScrltabsMDIChildWnd),*/RUNTIME_CLASS(GridDataView));
//ruleView=new CMultiDocTemplate(IDR_RULE_VIEW,RUNTIME_CLASS(KBaseDoc),RUNTIME_CLASS(RuleViewFrame),RUNTIME_CLASS(RuleView));
	AddDocTemplate(gridDoc);
//	m_pSplitterTemplate=new CMultiDocTemplate(Grid_Data_Res_Split,RUNTIME_CLASS(DataGridDoc),
//			RUNTIME_CLASS(CSplitterMDIChildWnd),RUNTIME_CLASS(DataGridView));
	// Connect the COleTemplateServer to the document template.
	//  The COleTemplateServer creates new documents on behalf
	//  of requesting OLE containers by using information
	//  specified in the document template.
	m_server.ConnectTemplate(clsid,pDocTemplate,FALSE);
	// Register all OLE server factories as running.  This enables the
	//  OLE libraries to create objects from other applications.
	COleTemplateServer::RegisterAll();
		// Note: MDI applications register all server objects without regard
		//  to the /Embedding or /Automation on the command line.
	// create main MDI Frame window
	CMainFrame*pMainFrame=new CMainFrame;
	if(!pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		TRACE0("Не могу загрузить основное окно. Выход из программы");return FALSE;
	}
//	if(!pMainFrame->LoadFrame(IDR_B50TYPE))return FALSE;
//	if(!pMainFrame->LoadFrame(NExpert_menu))return FALSE;
	m_pMainWnd=pMainFrame;
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Check to see if launched as OLE server
	if(cmdInfo.m_bRunEmbedded||cmdInfo.m_bRunAutomated)
	{
		//Application was run with /Embedding or /Automation. Don't show the main window in this case.
		return TRUE;
	}
	// When a server application is launched stand-alone, it is a good idea
	//  to update the system registry in case it has been damaged.
	m_server.UpdateRegistry(OAT_DISPATCH_OBJECT);
	COleObjectFactory::UpdateRegistryAll();
	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))return FALSE;
	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	return TRUE;
}
void CB50App::OnDaoView()
{
	POSITION curTemplatePos=GetFirstDocTemplatePosition();
	while(curTemplatePos!=NULL)
	{
		CDocTemplate*curTemplate=GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str,CDocTemplate::docName);
		if(str==_T("DaoView")){curTemplate->OpenDocumentFile(NULL);return;}
	}
	AfxMessageBox(IDS_NODAOVIEWTEMPLATE);
}
void CB50App::OnNxpDoc()
{
	POSITION curTemplatePos=GetFirstDocTemplatePosition();
	while(curTemplatePos!=NULL)
	{
		CDocTemplate*curTemplate=GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str,CDocTemplate::docName);
		if(str==_T("Экспертная оболочка"))
		{
			curTemplate->OpenDocumentFile(NULL);
			int a = 1;
			return;
		}
	}
	AfxMessageBox(IDS_NONxpTEMPLATE);
}

void CB50App::OnDataGridsDoc()
{
	POSITION curTemplatePos=GetFirstDocTemplatePosition();
	while(curTemplatePos!=NULL)
	{
		CDocTemplate*curTemplate=GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str,CDocTemplate::docName);
		if(str==_T("Решётки данных")){curTemplate->OpenDocumentFile(NULL);return;}
	}
	AfxMessageBox(IDS_NONxpTEMPLATE);
}

//	ON_COMMAND(idd_data_grids,OnDataGridsDoc)

void CB50App::OnOleDoc()
{
	POSITION curTemplatePos=GetFirstDocTemplatePosition();
	while(curTemplatePos!=NULL)
	{
		CDocTemplate*curTemplate=GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str,CDocTemplate::docName);
		if(str==_T("B50")){curTemplate->OpenDocumentFile(NULL);return;}
	}
	AfxMessageBox(IDS_NOOLETEMPLATE);
}
void CB50App::OnHelloView()
{
	POSITION curTemplatePos=GetFirstDocTemplatePosition();
	while(curTemplatePos!=NULL)
	{
		CDocTemplate*curTemplate=GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str,CDocTemplate::docName);
		if(str==_T("Hello"))
		{
			curTemplate->OpenDocumentFile(NULL);return;
		}
	}AfxMessageBox(IDS_NOBOUNCETEMPLATE);
}
void CB50App::OnKohonen()
{
	POSITION curTemplatePos=GetFirstDocTemplatePosition();
	while(curTemplatePos!=NULL)
	{
		CDocTemplate*curTemplate=GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str,CDocTemplate::docName);

//		IDM_KOHONEN_CHILD       "\nKohonen         \nKohonen  \n\n\nB50.Document\nДокумент сети Кохонена"
//								"\nРедактор свойств\nPropView \n\n\nB50.Document\nДокумент PropView"
//		IDR_PROP_VIEW           "\nРедактор свойств\nPropView\n\n\nB50.Document\nДокумент PropView"

		TRACE(str);
		TRACE("\n");
		if(str==_T("Kohonen"))
		{
			curTemplate->OpenDocumentFile(NULL);
			return;
		}
	}
	AfxMessageBox(IDS_NOKOHONENTEMPLATE);
}

BOOL CB50App::PreTranslateMessage(MSG* pMsg)
{
	// special filter for DLL
//	if(CWinApp::PreTranslateMessage(pMsg))return TRUE;
	if(CWinApp::PreTranslateMessage(pMsg))return TRUE;
	return false;
// 06.11.2016	return FilterDllMsg(pMsg);
}


BOOL CB50App::OnIdle(LONG lCount)
{
//	if(CWinApp::OnIdle(lCount))return TRUE;

	if(CWinApp::OnIdle(lCount))return TRUE;
// 06.11.2016	ProcessDllIdle();
	return FALSE;   // no more for me to do
}

/*
void DisplayDaoException(CDaoException*e)
{
	CString strMsg;
	if(e->m_pErrorInfo!=NULL)
	{
		strMsg.Format(_T("%s   (%d)\n\n")_T("Would you like to see help?"),
			(LPCTSTR)e->m_pErrorInfo->m_strDescription,e->m_pErrorInfo->m_lErrorCode);
		if(AfxMessageBox(strMsg,MB_YESNO)==IDYES)
		{
			WinHelp(GetDesktopWindow(),e->m_pErrorInfo->m_strHelpFile,HELP_CONTEXT,
					e->m_pErrorInfo->m_lHelpContext);
		}
	}
	else
	{
		strMsg.Format(_T("ERROR:CDaoException\n\n")_T("SCODE_CODE		=%d\n")	
			_T("SCODE_FACILITY	=%d\n")_T("SCODE_SEVERITY	=%d\n")_T("ResultFromScode	=%d\n"),
			SCODE_CODE(e->m_scode),SCODE_FACILITY	(e->m_scode),
			SCODE_SEVERITY	(e->m_scode),ResultFromScode (e->m_scode));
		AfxMessageBox(strMsg);
	}
}

*/




class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
CAboutDlg::CAboutDlg():CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
void CB50App::OnAppAbout(){CAboutDlg aboutDlg;aboutDlg.DoModal();}
int CB50App::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	GXTerminate();
	return CWinApp::ExitInstance();
}

// StgCreateDocfile WriteClassStg
/* 06.11.2016
UINT CCosmoDoc::Load(BOOL fChangeFile, LPTSTR pszFile)    
{    
	HRESULT hr;
	LPSTORAGE pIStorage;
	if(NULL==pszFile)
	{//For new untitled document, just rename ourselves.        
		Rename(NULL);m_lVer=VERSIONCURRENT;return DOCERR_NONE;
	}
	pIStorage=NULL;
	if(NOERROR!=StgIsStorageFile(pszFile))
	{
		hr=StgCreateDocfile(pszFile,STGM_TRANSACTED|STGM_READWRITE|STGM_CONVERT|STGM_SHARE_EXCLUSIVE,
			0,&pIStorage);
		if(FAILED(hr))
		{//If denied write access, try to load the old way.
			if(STG_E_ACCESSDENIED==GetScode(hr))m_lVer=m_pPL->ReadFromFile(pszFile);
			else return DOCERR_COULDNOTOPEN;
		}
	}
	else
	{
		hr=StgOpenStorage(pszFile,NULL,STGM_DIRECT|STGM_READ|STGM_SHARE_EXCLUSIVE,NULL,0,&pIStorage);
		if(FAILED(hr))return DOCERR_COULDNOTOPEN;
	}
	if(NULL!=pIStorage)
	{
		m_lVer=m_pPL->ReadFromStorage(pIStorage);pIStorage->Release();
	}
	if(POLYLINE_E_READFAILURE==m_lVer)return DOCERR_READFAILURE;
	if(POLYLINE_E_UNSUPPORTEDVERSION==m_lVer)return DOCERR_UNSUPPORTEDVERSION;
	if(fChangeFile)Rename(pszFile);
	//Importing a file makes things dirty.
	FDirtySet(!fChangeFile);
	return DOCERR_NONE;    
}
*/
//The binary interchange file format (BIFF) is the file format in which 
//Microsoft Excel workbooks are saved on disk. Microsoft Excel versions 5.0 and 
//later use compound files; this is the OLE 2 implementation of the 
//Structured Storage Model standard. For more information on this technology, see the 
//OLE 2 Programmer's Reference, Volume One, and Inside OLE 2, 
//both published by Microsoft Press and available from your local bookstore.
