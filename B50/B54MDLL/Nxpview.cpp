// NxpView.cpp : implementation file
//#if !defined(AFX_STDAFX_H__350A277E_BFEC_11D1_A093_00A024384FCF__INCLUDED_)
//#include "stdafx.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

//#include <afxwin.h>         // MFC core and standard components
#include "afxcontrolbars2.h"
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
#include "..\inc\mrcext.h"
#include <afxpriv.h>		// for WM_INITIALUPDATE
//#include <afxcontrolbars.h>
#include <afxsplitterwndex.h>

//#endif
//#ifndef __AFXTEMPL_H__
//#include <afxtempl.h>
//#endif
/*
#ifndef _STRING_
#include "string"
#endif
#ifndef _FSTREAM_
#include <fstream>
#endif
#endif
using namespace std;
#if !defined(__BaseList_H)
#include ".\know32\Bases.h"
#endif
//using namespace std;
#if !defined( __NXP_COMPILE_H )
#include ".\know32\Nxp_Comp.h"
#endif
#if !defined( __STRATEGY_H )
#include ".\know32\Strategy.h"
#endif  // __STRATEGY_H
//#include "resource.h"

//#include ".\b54mdll\traceapi.h"
#define _EXEWRITE
//#ifndef __WRITEKBASE_H
#include ".\know32\WriteKB.h"
//#endif
//#endif
*/


#include "..\Prefrences\PreferencesDlg.h"
#include "..\Shared\Preferences.h"
#include "..\Shared\ShortcutManager.h"

//#include ".\Prefrences\PreferencesDlg.h"
#include "..\shared\localizer.h"



#include <nd.h>
#include "dgramex.h"
//#include "dgvwex.h"
#include "dgvwex.h"
#include "..\EeScripts\app2\app2.h"

#if !defined( __KBASE_H )
#include "..\know32\kBase.h"
#endif
#include "..\b54mdll\resource.h"
#ifndef __NetSetup_H
#include "..\know32\netsetup.h"
#endif

//#include ".\know32\MyRuleBar.h"
#include "..\know32\AtomViewFrame.h"
//#include ".\know32\ruleviewframe.h"
//#include ".\know32\ruleviewdoc.h"
//#include ".\know32\RuleView.h"



#include "..\Prefrences\ToDoListWnd.h"
#include "..\Prefrences\TDLContentMgr.h"
#include "..\Prefrences\TDCImportExportMgr.h"
#include "..\Shared\UIExtensionMgr.h"
#include "..\Shared\UIThemeFile.h"
#include "..\Prefrences\ToDoCtrlMgr.h"
#include "..\Shared\tabctrlex.h"
#include "..\Prefrences\TDLFilterBar.h"


#include "NxpView.h"
#include "..\know32\strategy_box.h"
#include <KBaseDoc.h>


//Preferences\ToDoListWnd.h"
//ToDoListWnd.h"


//#include "SizeViewBar.h"
//#include ".\inc\mrcext.h"
//G_hInst = GetModule()->GetInstance();
//#if !defined(__BaseList_H)
//#include "Bases.h"
//#endif  // __BaseList_H
/*
#define IBMAT
#define LINT_ARGS
#include "..\know32\nxpdef.h"
*/
#undef _DEBUG
#define ERR_LIB NewDgram2
//#define MCH_WIN  MCH_WINPM
#define MCH_OS MCH_OSWIN32
#undef APP_CONSOLE
//#include <nd.h>

//#ifndef _DGRAMEX_H_
//#include "..\diagram\dgramex.h"
//#endif
//#include <nd.h>

//#include "..\dgVw\dgvwex.h"
//#include "..\DgRam\dgramex.h"

//ERR_EXTERN

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern	CMainFrame*m_pMainWnd;
//extern CB50App theApp;
//	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))return FALSE;
//	m_pMainWnd=pMainFrame;
//extern std::ostream&operator<<(std::ostream&,const ClassItemList*);

#define update_kbase WM_USER+1
HWND G_hWnd;
//KnowledgeBase*knowBase=NULL;// = new KnowledgeBase;
IMPLEMENT_DYNCREATE(NxpView, CView)
NxpView::NxpView():CView(),
m_pPrefs(NULL),
m_tabCtrl(TCE_POSTDRAW | TCE_MBUTTONCLOSE | TCE_DRAGDROP | TCE_CLOSEBUTTON | TCE_BOLDSELTEXT),
m_mgrToDoCtrls(m_tabCtrl)
{
	G_hWnd=m_hWnd;

//CToDoListWnd::CToDoListWnd() : CFrameWnd(),m_bVisible(-1),

/*
	mainWindow=this;
	if(!NXP_Control(NXP_CTRL_INIT))
	{
	 AfxMessageBox("Нет возможности запустить нексперт"); //,"Ошибка Нэксперта",MB_OK);
	 nexpert = false;
//	 loaded = alreadyStopped = TRUE;
	}
//	baseFile=new CString();
	baseFile=NULL;
	if(knowBase!=NULL)delete knowBase;
	knowBase=new KnowledgeBase;
	knowBases=new BaseList();
	nxpCompiler=NULL;
	strategies=new Strategies();
	currentStrategy=new Strategy();
	defaultStrategy=new Strategy();
	strategies->defStrat=defaultStrategy;
	strategies->curStrat=currentStrategy;
	nxpCompiler=new NXP_Compiler();
	CRect rc(7,7,189,25); // ACS_TRANSPARENT
//	m_avi.Create(WS_CHILD|WS_VISIBLE|ACS_CENTER,rc,this,99);
//		{
//			m_avi1->Open(IDR_AVI1);			 // open the avi resource. CAnimateCtrl
//			m_avi1->Play(0,-1,-1);			 // play avi resource. CProgressCtrl	m_progress;
//		}
*/
}
LRESULT NxpView::DefWindowProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
//	if(NXP_ProcessHandlerMessage(m_hWnd,(int)Msg,wParam,lParam)==FALSE)
		return CView::DefWindowProc(Msg,wParam,lParam);
//	else return 1;//TMDIClient::
}
NxpView::~NxpView() // DefWindowProc
{
	delete m_pPrefs;
//	delete &knowBases;//BaseList;
}
void NxpView::OnDraw(CDC*pDC)
{
	CDocument*pDoc=GetDocument();
}
//#ifdef _DEBUG
void NxpView::AssertValid()const{CView::AssertValid();}
/*
void NxpView::Dump(CDumpContext&dc)const
{
//	CView::Dump(dc);
}
*/
//#endif //_DEBUG
/*
int NxpView::alert(int,char*thePtr,int*ret)
{
//	mainWindow->showAlert(string(thePtr));
	*(int*)ret=NXP_RET_OK;return(TRUE);
}
int NxpView::getStatus(int winId,int Code,int*ret)
{
	if(winId!=NXP_WIN_TRAN||Code!=NXP_GS_ENABLED)return FALSE;
	*(int*)ret=FALSE;return(TRUE);
}
//#pragma argsused
int NxpView::setData(int winId,int ctrlId,int index,char*thePtr )
{
	if(thePtr == NULL) return TRUE;//curOper->showLoading( (LPSTR)" " );
	string str("NXP_WIN_TRAN ");
	if(winId==NXP_WIN_TRAN){str+=thePtr;return (TRUE);}
  if(winId == NXP_WIN_HYPO)
		{if(strcmp(thePtr,"") != 0){str="Текущая гипотеза ";str+=thePtr;}return (TRUE);}
  if(winId==NXP_WIN_RULE)
		{if(strcmp(thePtr,"")!=0){str="Текущее правило ";str+=thePtr;}return(TRUE);}
  if(winId==NXP_WIN_CONC)
		{if(strcmp(thePtr,"")!=0){str="Выводы ";str+=thePtr;}return (TRUE);}
	if((ctrlId&NXP_CELL_MASK)==NXP_CELL_COL3){str=thePtr;}
  else
  {
	 char far*ptr;ptr=strchr(thePtr,' ');
	 if((strncmp(thePtr,"Retrieving",ptr-thePtr)) == 0)
	 {
		CString load;
		if(!load.LoadString(IDS_LoadDB))AfxMessageBox("Error Loading String: IDS_FILENOTFOUND");
		str=load;
	 }
	 else if((strncmp(thePtr,"Создание",ptr-thePtr))==0)
	 {
		CString save;
		if(!save.LoadString(IDS_SaveDB)){AfxMessageBox("Error Loading String: IDS_FILENOTFOUND");}
		str=save;
	 }
	 else if((strncmp(thePtr,"Выгрузка",ptr-thePtr))==0){str="Выгрузка";}
	 else
		{//str=SetUpData.szLoads;str+=mainWindow->baseFile;mainWindow->showLoading(str);
		}
  }return TRUE;
}
//#pragma argsused
int NxpView::notify(int winId,int ctrlId,unsigned long atomId){return TRUE;}
//#pragma argsused
int NxpView::getData(int winId,int ctrlId,int index,char *thePtr )
{
/
  switch((int)winId)
  {
	 case NXP_WIN_CLASSEDIT:mainWindow->nxpClassEdit(winId,ctrlId,index,thePtr);break;
	 case NXP_WIN_OBJEDIT:mainWindow->nxpObjectEdit(winId,ctrlId,index,thePtr);break;
	 case NXP_WIN_PROPEDIT:mainWindow->nxpPropEdit(winId,ctrlId,index,thePtr);break;
	 case NXP_WIN_CNTXEDIT:mainWindow->nxpContextEdit(winId,ctrlId,index,thePtr);break;
	 case NXP_WIN_METAEDIT:mainWindow->nxpMetaEdit(winId,ctrlId,index,thePtr);break;
	 case NXP_WIN_RULEDIT:mainWindow->nxpRuleEdit(winId,ctrlId,index,thePtr);break;
	 default: return FALSE;
  }
/
	return TRUE;
}
void NxpView::LoadKB()
{
//  char tmr[11];
	G_hWnd=m_hWnd;
//	if(nexpert)
//	{
//LoadString(GetApplication()->hInstance, IDS_TIMER, tmr, 11 );
/
		NXP_SetHandler(NXP_PROC_ALERT,(NxpIProc)(WNDPROC)alert,"alert");
//  NXP_SetHandler(NXP_PROC_QUESTION,(NxpIProc)(WNDPROC)question,    (LPSTR)NULL);
//  NXP_SetHandler(NXP_PROC_EXECUTE,(NxpIProc)(WNDPROC)amountOfTime,(LPSTR)tmr);
		NXP_SetHandler(NXP_PROC_GETSTATUS,(NxpIProc)(WNDPROC)getStatus,(LPSTR)NULL);
		NXP_SetHandler(NXP_PROC_SETDATA,(NxpIProc)(WNDPROC)setData,(LPSTR)"setData");
		NXP_SetHandler(NXP_PROC_GETDATA,(NxpIProc)(WNDPROC)getData,(LPSTR)"getData");
		NXP_SetHandler(NXP_PROC_NOTIFY,(NxpIProc)(WNDPROC)notify,(LPSTR)"notify");
/
//	}
	KBId kbId;
/
	whatLoad();if(loadType==task)topicBox();
	else if(loadType==fromFile)    // OFN_OVERWRITEPROMPT|
	{
/
	 char BASED_CODE szFilter[]="Файлы баз знаний (*.*kb)|*.kb;*.tkb;*.ckb|Все файлы (*.*)|*.*|";
	 CFileDialog fd(true,"*.kb","bank1.kb",OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST,szFilter);
//	 fd.m_ofn.nFilterIndex=1;
	 fd.m_ofn.lpstrInitialDir=(LPSTR)"c:\\gene\\know\\banks";
	 if(fd.DoModal()==IDOK)baseFile=new CString(fd.GetPathName());else return; // CString
//  if( unloads == TRUE ) unloadsBox();
//	if(!baseFile.is_null() && !loaded)
//	{
//	 KBId kbId;
//	 NXP_LoadKB((LPSTR)baseFile.c_str(),(KBId *)&kbId); //, (KBId *)NULL);
//	 loaded = TRUE;
//	}
	bool alreadyLoaded=false;
	BaseName*bName=knowBases->bases;
	while(bName!=NULL) // Если база знаний есть в списке баз. CString
	{
//		if(strcmp(bName->baseName,(LPCTSTR)*baseFile)==0){alreadyLoaded=true;break;}
		if(*bName->baseName==*baseFile)
		{
			alreadyLoaded=TRUE;break;
		}
		bName=bName->nextBase;
	}
	if(!alreadyLoaded&&!baseFile->IsEmpty())
	{
//		CAnimateCtrl*m_avi1=new CAnimateCtrl;
//		CRect rc(7,7,189,25); // ACS_TRANSPARENT
//		if(m_avi1->Create(WS_CHILD|WS_VISIBLE|ACS_CENTER,rc,this,99))
//		{
//			m_avi.Open(IDR_AVI1);			 // open the avi resource. CAnimateCtrl
//			m_avi.Play(0,-1,-1);			 // play avi resource. CProgressCtrl	m_progress;
//		}
		ifstream*ifs;try{ifs=new ifstream((LPCTSTR)*baseFile);}
		catch(CFileException*ex)
		{
//			m_avi.Stop();			 // open the avi resource. CAnimateCtrl
//			m_avi.Close();
			if(ex->m_cause==CFileException::fileNotFound)
			{
				TRACE("Файл не найден\n" );ex->Delete();AfxMessageBox("Не открывается файл.");return;
			}
		}
    try{knowBase->read(*ifs);}
		catch(CException*ex)
		{
//			m_avi.Stop();			 // open the avi resource. CAnimateCtrl
//			m_avi.Close();
//			m_avi1->Close();
//			m_avi.Delete();
			TCHAR szCause[255];ex->GetErrorMessage(szCause,255);ex->Delete();AfxMessageBox(szCause);return;
		}
//		KBId kbId;
		nexpert=true;
		CWnd*pParent=GetParent();
		if(pParent)pParent->SendMessage(update_kbase,(LPARAM)knowBase,(WPARAM)update_kbase);
    try
    {
			if(nexpert)
			{
				if(nxpCompiler)
				{
					if(nxpCompiler->compileKBase()){currentKBase=NULL;loaded=true;}
//			else
//			{
//					NXP_LoadKB((LPSTR)baseFile.c_str(),(KBId *)&kbId); //, (KBId *)NULL);
//					int ret=NXP_Error();
//					if(ret!=NXP_ERR_NOERR)showError(ret);
//				else
//					{
					currentKBase=kbId;
					knowBases->addBase(baseFile,currentKBase);
	//			if(knowBase!=NULL)delete knowBase;knowBase=new KnowledgeBase;
//					showTopic((LPSTR)"");topicItem = IDS_BALLS;
//					char tem[70];strcpy(tem,"( ");strcat(tem,(LPSTR)topic.c_str());
//					strcat(tem," )");showTopic(tem);
					loaded = TRUE;
//Certain certFactor(GetApplication()->hInstance,HWindow);
//			certFactor = new Certain();
//			if(strcmp(baseFile,"nata.kb")==0)    nataDialog(knowBases->bases);
//			if(strcmp(baseFile,"hello1.tkb")==0) operDialog(knowBases->bases);
//			if(strcmp(baseFile,"regine.tkb")==0) regNetDialog(knowBases->bases);
//			if(strcmp(baseFile,"fault.tkb")==0)  interfacDialog(knowBases->bases);
				}
			}
		}
    catch(CException*ex)
		{
//			m_avi.Stop();			 // open the avi resource. CAnimateCtrl
//			m_avi.Open(NULL);			 // open the avi resource. CAnimateCtrl
//			m_avi.Close();
//			m_avi1->Close();
			TCHAR szCause[255];ex->GetErrorMessage(szCause,255);ex->Delete();AfxMessageBox(szCause);return;
		}
//			m_avi.Stop();			 // open the avi resource. CAnimateCtrl
//			m_avi.Open(NULL);			 // open the avi resource. CAnimateCtrl
	//		m_avi.Close();
//		m_avi1->Close();
//		delete m_avi1;
	}
//  if((strcmp(baseFile,"") == 0) nataDialog();
//  kBaseDDE->fillKnowledgeBaseWithRules();
//  kBaseDDE->fillKnowledgeBaseWithClasses();
//  kBaseDDE->fillKnowledgeBaseWithObjects();
//  kBaseDDE->fillKnowledgeBaseWithProperties();
//  kBaseDDE->fillKnowledgeBaseWithSlots();
//  kBaseDDE->fillKnowledgeBaseWithGlobals();
}
void NxpView::SaveKB()
{
	BaseName*baseInfo=new BaseName;
	int ret;
	WriteKb writeTo((CView*)this,baseInfo,knowBases);//this);//->m_hWnd);//((CView*)this);
	ret=writeTo.DoModal();
//	writeTo.DoModal();
	if(ret==-1)AfxMessageBox("Не создаётся окно сохранения базы.");
	else if(ret==IDOK)
	{
		if(!baseInfo->secondName->IsEmpty())
		{
//			ASSERT(baseInfo->secondName->IsEmpty());
			SaveKnowledgeBase(baseInfo->secondName,baseInfo->unloadType);
		}
	}

	struct TracerData data;
	data.bEnabled = afxTraceEnabled;
	data.flags = afxTraceFlags;

	TRACE0("Calling Tracer DLL\n");
	if(PromptTraceFlags(this->m_hWnd,&data))
	{
		TRACE0("Changing trace flags\n");afxTraceEnabled = data.bEnabled;
		afxTraceFlags = data.flags;TRACE0("Changed trace flags\n");
	}
	WriteKb*m_pListOut=new WriteKb;
	// all objects are allocated on the application's heap, even CListOutputFrame which is defined in another DLL
	CRect rectOriginal(250,0,600,300);
	// hard-coded initial position
	if(!m_pListOut->Create(_T("List Output"),WS_OVERLAPPEDWINDOW|WS_CHILD|WS_VISIBLE,rectOriginal,(CMDIFrameWnd*)theApp.m_pMainWnd))
	{
		AfxMessageBox(_T("Failed to create WriteKb Window"));
		m_pListOut = NULL;  // just in case (Create will delete the C++ object)
		return;
	}
	m_pListOut->SetBackpointer(&m_pListOut);
	delete baseInfo;
}
BOOL NxpView::SaveKnowledgeBase(CString*savefile,int) // unloadType
{
	ofstream*ofs;//(savefile);
//	ifstream*ifs;
	try{ofs=new ofstream((LPCTSTR)*savefile);}
	catch(CFileException*ex)
	{
		if(ex->m_cause==CFileException::fileNotFound)
		{
			TRACE("Файл не найден\n" );ex->Delete();AfxMessageBox("Не открывается файл.");return FALSE;
		}
	}
	(*ofs)<<"(@VERSION= 011)\n";
	SaveProperties(*ofs,knowBase->props);
	SaveClasses(*ofs,knowBase->classes);
	SaveObjects(*ofs,knowBase->objects);
	SaveSlots(*ofs,knowBase->slots);
	SaveRules(*ofs,knowBase->rules);
	knowBase->globals->printOn(*ofs);
	return 1;
}
BOOL NxpView::SaveObjects(ofstream&os,ObjectDescription&d)
{
	os<<d.objectItemList;return TRUE;
}
BOOL NxpView::SaveClasses(std::ofstream&os,ClassDescription&d)
{
	os<<d.items;return TRUE;
}
BOOL NxpView::SaveProperties(std::ofstream&os,PropertyDescription&d){os<<d.items;return TRUE;}
BOOL NxpView::SaveSlots(std::ostream&os,SlotDescription&d)
{
//	os<<d.slotItems;return TRUE;
}
BOOL NxpView::SaveRules(std::ofstream&os,RuleDescription*d)
{
	os<<d->ruleItems;return TRUE;
}
/*
BOOL NxpView::PreTranslateMessage(MSG* pMsg)
{
	// special filter for DLL
//	if(CWinApp::PreTranslateMessage(pMsg))return TRUE;
	if(CView::PreTranslateMessage(pMsg))return TRUE;
	return FilterDllMsg(pMsg);
}
*/
/*
BOOL NxpView::OnIdle(LONG lCount)
{
	if(CWinApp::OnIdle(lCount))return TRUE;

//	if(CView::OnIdle(lCount))return TRUE;
//	if(CWinApp::OnIdle(lCount))return TRUE;
	ProcessDllIdle();
	return FALSE;   // no more for me to do
}
*/

//	EV_COMMAND(exmenu_load,   LoadKB),
void NxpView::WinSetup()
{
	if(NetSetup(this).DoModal()==IDOK)
	{ }
}
void NxpView::AtomTreeView()
{
//	POSITION curTemplatePos=theApp.GetFirstDocTemplatePosition();
	CWinApp*theApp=AfxGetApp();
	POSITION curTemplatePos=theApp->GetFirstDocTemplatePosition();
	while(curTemplatePos!=NULL)
	{
		CDocTemplate*curTemplate=theApp->GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str,CDocTemplate::docName);
		if(str==_T("AtomsView")){curTemplate->OpenDocumentFile(NULL);return;}
	}
	AfxMessageBox(IDS_NONxpTEMPLATE);
}

//void NxpView::REdit()
//{
/*
	TMDIChild* child = new TMDIChild(*this,"Редактор правил",new RuleEdit(loaded));
	TLayoutMetrics lm;
	lm.X.Absolute(lmLeft,10);	lm.Y.Absolute(lmTop,10);
	lm.Width.PercentOf(lmParent,lmRight,80);lm.Height.PercentOf(lmParent,lmBottom,80);
	SetChildLayoutMetrics(*child,lm);
//  child->SetIcon(0, 0);
	child->Create();
*/
//	RuleViewFrame ruleEdit; // m_hWnd
//	ruleEdit.Create(NULL,"Редактор правил",WS_CHILD|WS_VISIBLE|WS_OVERLAPPEDWINDOW,CRect(0,0,100,100),this);//, CCreateContext* pContext = NULL );
//	CMultiDocTemplate*ruleView;
//	ruleView=new CMultiDocTemplate(IDR_RULE_VIEW,RUNTIME_CLASS(RuleViewDoc),
//		RUNTIME_CLASS(RuleViewFrame),RUNTIME_CLASS(RuleView));
//	ruleView->OpenDocumentFile(NULL);
//}
//void NxpView::ClEdit()
//{
/*
	TMDIChild*child=new TMDIChild(*this,"Редактор классов",new ClassEdit(loaded));
	TLayoutMetrics lm;
	lm.X.Absolute(lmLeft,15);	lm.Y.Absolute(lmTop,15);
	lm.Width.PercentOf(lmParent,lmRight,80);lm.Height.PercentOf(lmParent,lmBottom,80);
	SetChildLayoutMetrics(*child,lm);
//  child->SetIcon(0, 0);
	child->Create();
*/
//}
//void NxpView::ObjEdit()
//{
/*
	TMDIChild* child=new TMDIChild(*this,"Редактор объектов",new ObjectEdit(loaded));
	TLayoutMetrics lm;
	lm.X.Absolute(lmLeft,20);lm.Y.Absolute(lmTop,20);
	lm.Width.PercentOf(lmParent,lmRight,80);lm.Height.PercentOf(lmParent,lmBottom,80);
	SetChildLayoutMetrics(*child,lm);
	child->Create();
*/
//}
//void NxpView::PropEdit()
//{
/*
	TMDIChild* child = new TMDIChild(*this,"Редактор свойств",new PropertyEdit(loaded));
	TLayoutMetrics lm;
	lm.X.Absolute(lmLeft,12);lm.Y.Absolute(lmTop,12);
	lm.Width.PercentOf(lmParent,lmRight,80);lm.Height.PercentOf(lmParent,lmBottom,80);
	SetChildLayoutMetrics(*child,lm);
//  child->SetIcon(0, 0);
	child->Create();
*/
//}
//void NxpView::MetaSlotEdit()
//{
/*
	TMDIChild* child = new TMDIChild(*this,"Редактор метаслотов",new MetaEdit(loaded));
	TLayoutMetrics lm;
	lm.X.Absolute(lmLeft,10);	lm.Y.Absolute(lmTop,10);
	lm.Width.PercentOf(lmParent,lmRight,80);lm.Height.PercentOf(lmParent,lmBottom,80);
	SetChildLayoutMetrics(*child,lm);
//  child->SetIcon(0, 0);
	child->Create();
*/
//}
//void NxpView::RuleGraph()
//{
//	if(knowBase != NULL)
//	{
/*
		TMDIChild* child = new TMDIChild(*this,"Сеть объектов",new RuleNetWindow(knowBase));
		CLayoutMetrics lm;
		lm.X.Set(lmLeft,lmRightOf,lmParent,lmLeft,10);
		lm.Y.Set(lmTop,lmBelow,lmParent,lmTop,10);
		lm.Width.Set(lmRight,  lmLeftOf,lmParent,lmRight, 40);
		lm.Height.Set(lmBottom,lmAbove, lmParent,lmBottom,40);
		SetChildLayoutMetrics(*child,lm);
//  child->SetIcon(0, 0);
		child->Create();
*/
/*
		RuleNetWindow *ruleNetDialog = new RuleNetWindow(this,"Сеть объектов",knowBase);
		ruleNetDialog->Attr.Style |= WS_POPUPWINDOW|WS_CHILD|WS_VISIBLE|WS_BORDER|
																		WS_DLGFRAME|WS_SIZEBOX;
//  ruleDialog->Attr.Style = FONT 8, "Courier"
//  classScrollDialog->Attr.Style |= WS_VSCROLL | WS_HSCROLL;
		ruleNetDialog->Attr.X = 20;
		ruleNetDialog->Attr.Y = 50;
		ruleNetDialog->Attr.W = 550;
		ruleNetDialog->Attr.H = 350;
		GetApplication()->MakeWindow(ruleNetDialog);
*/
//	}
//	else MessageBox("База знаний пуста.","Ошибка");
//}


BEGIN_MESSAGE_MAP(NxpView,CView)
	//{{AFX_MSG_MAP(NxpView)
//	ON_COMMAND(ID_BLACK, OnColor)
//	ON_COMMAND(ID_BLUE, OnColor)
//	ON_COMMAND(ID_GREEN, OnColor)
//	ON_COMMAND(ID_RED, OnColor)
	ON_COMMAND(rmenu_treeVew,AtomTreeView)
	ON_COMMAND(idd_diagram_view,Diagram)
	ON_COMMAND(idd_diagram_view2,Diagram2)

	ON_COMMAND(winmenu_set,WinSetup)
	ON_COMMAND(exmenu_strat,SetStrategies)
//	ON_COMMAND(exmenu_save,SaveKB)
//	ON_COMMAND(exmenu_load,LoadKB)

//	ON_COMMAND(rmenu_rule,REdit)
//	ON_COMMAND(rmenu_class,ClEdit)
//	ON_COMMAND(rmenu_object,ObjEdit)
//	ON_COMMAND(rmenu_props,PropEdit)
//	ON_COMMAND(rmenu_slots,MetaSlotEdit)
//	ON_COMMAND(netmenu_rule,RuleGraph)
		// NOTE - the ClassWizard will add and remove mapping macros here.
//	ON_UPDATE_COMMAND_UI(ID_VIEW_TREEBAR,OnUpdateControlBarMenu)
//	ON_COMMAND_EX(ID_VIEW_TREEBAR, OnBarCheck)
	//}}AFX_MSG_MAP
//	ON_COMMAND(ID_PREFERENCES, &NxpView::OnPreferences)
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_RBUTTONDOWN()
ON_WM_RBUTTONUP()
ON_WM_MOVE()
ON_WM_WINDOWPOSCHANGED()
ON_WM_CREATE()
END_MESSAGE_MAP()

void NxpView::OnUpdate(CView*pSender,LPARAM knowBase,CObject*pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	CWnd*pParent=GetParent();
	if(pParent)pParent->SendMessage(update_kbase,(LPARAM)knowBase,(WPARAM)update_kbase);
}
void NxpView::SetStrategies()  // exmenu_strat
{
	Strategies*strat=new Strategies;
	StrategyBox stratDlg((CView*)this,strat);//strategies);
	stratDlg.DoModal();
}
/*
//#include ".\diagram\dgramex.h"
//--------------------------------------------------------------------------
//	"Simple Diagrammer" Window Implementation
//--------------------------------------------------------------------------
CStr	DGramExWinSimple::GetDGramFileName L2(CStr, caption, BoolEnum, newFile)
{
	NDFileWPtr filew = new NDFileW;
	filew->Init();
	filew->ResetFilters();
	filew->SetNthFilter(0, "*.ddt", "Файла диаграмм(*.ddt)",FMGR_MACTYPENONE);
	if (newFile) {filew->SetFileExistence(FILEW_FILEEXISTFALSE);} 
	else {filew->SetFileExistence(FILEW_FILEEXISTTRUE);}
	filew->SetPrompt(caption);
	CStr fileName = filew->ProcessGetResult();
	filew->End();
	delete filew;
	return fileName;
}
void 	DGramExWinSimple::SelMenuBar(void)
{
	NDMBarPtr mbar = GetMenuBar();
	UInt16 id;
	CStr fileName;
	switch (id = mbar->ChosenGetId()) 
	{
	case DGRAMEXWINSIMPLE_MITEMLOAD:
		fileName = GetDGramFileName("Загрузить файл:", BOOL_FALSE);// pick a file
		if(fileName){mDGram->ClearAll();mDGram->ReadDGram(fileName);
		// insert the file
		}break;
	case DGRAMEXWINSIMPLE_MITEMINSERT:
		// "Insert from file ..." Item
		fileName=GetDGramFileName("Вставить файл:", BOOL_FALSE);
		if(fileName) 
		{
			static NDPoint32 loc = NDPoint32(300,300) ;
			// insert the file
			mDGram->InsertDGram(&loc, fileName);
		}break;
	case DGRAMEXWINSIMPLE_MITEMSAVE:
		// pick a file name
		fileName = GetDGramFileName("Сохранить в файл:", BOOL_TRUE);
		if(fileName){mDGram->SaveAs(fileName);}break;
	default:DBG_ERROR;break;
	}
}
void	DGramExWinSimple::NodeClickSArea(void){mDGSArea->NDDGramSArea::NfyNodeClick();}
void	DGramExWinSimple::Init(void)
{
	// Initialize the window 
	NDWin::NfyInit();
	// Add the call-back for the DGramSArea component
	mDGSArea = mDGram->GetDGramSArea();
	WIN_SETNFYHANDLER(DGramExWinSimple, mDGSArea, DGRAMSAREA_NFYNODECLICK, NodeClickSArea);
}
///////////////////////////////////////////////////////////////////////////
DGramExWinSimple::DGramExWinSimple(CStr modName,CStr resName):NDWin(modName,resName)
{
	mDGram=(NDDGramPtr)GetNamedWgt("DGram");
	WIN_SETNFYHANDLER(DGramExWinSimple,this,WIN_NFYSELMENUBAR,SelMenuBar);
	// The DGramSArea component of the diagrammer is only created after
	// initialization. We need to differ the installation of the
	// corresponding call-back after initialization of the window.
	mDGSArea=NULL;
	WIN_SETNFYHANDLER(DGramExWinSimple,this,WIN_NFYINIT,Init);
}
void	DGramExWinSimple::LoadShow(void)
{
	DGramExWinSimplePtr win;
	Point16Ptr	point;
	ERR_TRACEIN;
	win=new DGramExWinSimple("DGramEx","WinSimple");
	win->Init();
	win->Show();
	point=(Point16Ptr)win->mDGram->GetExt();
	win->mwidth=point->GetX();
	win->mheight=point->GetY();
	win->mvargr->RegisterView((ResPtr)win->mDGram);
	ERR_TRACEOUT;
}
*/
void NxpView::Diagram()
{
	CString pszPathName;
	CString fileName;
	CFileDialog	dlg(TRUE,_T("dat"),NULL,OFN_HIDEREADONLY,_T("Файлы диаграмм (DGramEx.dat)|*.dat||"));//,this);
	if(IDOK==dlg.DoModal())
	{
		pszPathName=dlg.GetPathName();// get full path to database file
		fileName=dlg.GetFileName();
		
		if(fileName=="dgramex.dat")
		{
//			(void)NDRLib::LoadLibFile("DGramEx" , "dgramex.dat");
			(void)NDRLib::LoadLibFile("DGramEx",pszPathName);//"NewDgRam2.dat");
//			DGramExWinSelect::LoadShow();
			DGramExWinSelectPtr win;
			win=new DGramExWinSelect("DGramEx","WinSelect");
			win->Init();win->Show();
			ND::Run();
			ND::Exit();
		}
		else if(fileName=="NewDgRam2.dat")
		{
			(void)NDRLib::LoadLibFile("DGramEx",pszPathName);//"NewDgRam2.dat");
			DGramExWinSimple::LoadShow();
			ND::Run();
			ND::Exit();
		}
		else if(fileName=="dgvwex.dat")
		{
			(void)NDRLib::LoadLibFile("dgvwex", pszPathName);
			// (( CodeGen: WindowLoading
			DgvwexGrWin::LoadShow();
			ND::Run();
			ND::Exit();
		}
		else if (fileName == "app2.dat")
		{
			(void)NDRLib::LoadLibFile("app2", pszPathName);
			App2Win1::LoadShow();
			ND::Run();
			ND::Exit();
		}
		
	}
}
void NxpView::Diagram2()
{

	CString pszPathName;
	CFileDialog	dlg(TRUE,_T("dat"),NULL,OFN_HIDEREADONLY,_T("Файлы диаграмм (dgvwex.dat)|*.dat||"));//,this);
	if(IDOK==dlg.DoModal())
	{

		pszPathName=dlg.GetPathName();// get full path to database file
		(void)NDRLib::LoadLibFile("dgvwex",pszPathName);
	// -- Remove 'CodeGen' directives to modify the following block of window loading.
	// (( CodeGen: WindowLoading
		DgvwexGrWin::LoadShow();
	// )) CodeGen: WindowLoading 2037
		ND::Run();
		ND::Exit();
		
	}
}
void C_FAR APP_InitFirst L0()
{
//	S_HandShake();
}									
void C_FAR APP_InitLast L0(){} 
void C_FAR APP_Exit L0(){}
/*
void 	DGramExWinSimple::HitPanel3Panel1ChangeValue(void)
{
	CStr question="Enter New Value.";
	char answer[255] = "\0";
	ERR_TRACEIN;
/*
	// -- Insert your code here...
	mnode1 = mvargr->GetNodeCursor();
	if(mvargr->IsNodeValid(mnode1))mnode=mnode1;
	else mnode->GoIndexed(mvargr->GetNumNodes()-1);
	if (mvargr->IsNodeValid(mnode )) 
	{
		if(NDAskW::AskQuestion(question, answer))
		{
			mvalue.SetStr((CStr)answer);
			mvargr->SetNodeValue(mnode, &mvalue);
		}
	}
//
	ERR_TRACEOUT;
}
// )) CodeGen: WgtNfyHandler HitPanel3Panel1ChangeValue
// (( CodeGen: WgtNfyHandler HitPanel3Panel1NodeXY
void 	DGramExWinSimple::HitPanel3Panel1NodeXY(void)
{
}
// )) CodeGen: WgtNfyHandler HitPanel3Panel1NodeXY
// (( CodeGen: WgtNfyHandler HitPanel3Panel1AddNode
void 	DGramExWinSimple::HitPanel3Panel1AddNode(void)
{
}
// )) CodeGen: WgtNfyHandler HitPanel3Panel1AddNode

// (( CodeGen: WgtNfyHandler HitPanel3Panel1RemoveNode
void 	DGramExWinSimple::HitPanel3Panel1RemoveNode(void)
{
}
// )) CodeGen: WgtNfyHandler HitPanel3Panel1RemoveNode

// (( CodeGen: WgtNfyHandler HitPanel3Panel2AddLink
void 	DGramExWinSimple::HitPanel3Panel2AddLink(void)
{
}
// )) CodeGen: WgtNfyHandler HitPanel3Panel2AddLink

// (( CodeGen: WgtNfyHandler HitPanel3Panel2RemoveLink
void 	DGramExWinSimple::HitPanel3Panel2RemoveLink(void)
{
}
// )) CodeGen: WgtNfyHandler HitPanel3Panel2RemoveLink

// (( CodeGen: WgtNfyHandler HitPanel3Panel5Register
void 	DGramExWinSimple::HitPanel3Panel5Register(void)
{
	static Int turn=0;
	DsPtr	ds=(DsPtr)mvargr;
	ERR_TRACEIN;
/*
	// -- Insert your code here...
	switch(turn) 
	{
		case 0:
			ds->UnregisterView((ResPtr)mDGram);
			mRegister->SetLabel("Register");
			turn++;break;
		case 1:
			ds->RegisterView((ResPtr)mDGram);
			mRegister->SetLabel("Unregister");
			turn=0;
	};
//
	ERR_TRACEOUT;
}
// )) CodeGen: WgtNfyHandler HitPanel3Panel5Register
// (( CodeGen: WgtNfyHandler HitPanel3Panel5AutoSize
void 	DGramExWinSimple::HitPanel3Panel5AutoSize(void)
{
}
// )) CodeGen: WgtNfyHandler HitPanel3Panel5AutoSize

// (( CodeGen: WgtNfyHandler HitPanel3Panel5ReadOnly
void 	DGramExWinSimple::HitPanel3Panel5ReadOnly(void)
{
}
// )) CodeGen: WgtNfyHandler HitPanel3Panel5ReadOnly
// (( CodeGen: WgtNfyHandler HitPanel3Panel6Salesman
void 	DGramExWinSimple::HitPanel3Panel6Salesman(void)
{
}
// )) CodeGen: WgtNfyHandler HitPanel3Panel6Salesman
// (( CodeGen: WgtNfyHandler EltSelectedPanel4WgtOption
void 	DGramExWinSimple::EltSelectedPanel4WgtOption(CBoxEltSelectedNfyCPtr info)
{
}
// )) CodeGen: WgtNfyHandler EltSelectedPanel4WgtOption
// (( CodeGen: WgtNfyHandler EltSelectedPanel4SetOption
void 	DGramExWinSimple::EltSelectedPanel4SetOption(CBoxEltSelectedNfyCPtr info)
{
}
*/


void NxpView::OnPreferences()
{
	// TODO: Add your command handler code here
	MessageBox("NxpView::OnPreferences()", "NxpView::OnPreferences()");
	int a = 1;
	a = 2;
	DoPreferences(2);
}

void NxpView::ResetPrefs()
{
	delete m_pPrefs;
	m_pPrefs = new CPreferencesDlg(&m_mgrShortcuts,&m_mgrContent,&m_mgrImportExport,&m_mgrUIExtensions);
	// update
	m_pPrefs->SetUITheme(m_theme);
	m_mgrToDoCtrls.SetPrefs(m_pPrefs);
	// Update Filter bar colours
	CDWordArray aPriorityColors;
	m_pPrefs->GetPriorityColors(aPriorityColors);
	m_filterBar.SetPriorityColors(aPriorityColors);
	CStringArray aDefTaskViews;
	m_pPrefs->GetDefaultTaskViews(aDefTaskViews);
	CFilteredToDoCtrl::SetDefaultTaskViews(aDefTaskViews);
}
void NxpView::DoPreferences(int nInitPage)
{
	// take a copy of current userPrefs to check changes against
	const CPreferencesDlg oldPrefs;
//	CToDoListWnd todoList;
//	CToDoListWnd* pTDL = new CToDoListWnd;

//	oldPrefs
	// kill timers for the duration
//	KillTimers();
	
	// restore translation of dynamic menu items shortcut prefs
//	EnableDynamicMenuTranslation(TRUE);
//	ASSERT(m_pPrefs);
//	UINT nRet = m_pPrefs->DoModal(nInitPage);

/*
	// re-disable dynamic menu translation
	EnableDynamicMenuTranslation(FALSE);
	// updates userPrefs
	RedrawWindow();
	ResetPrefs();
	const CPreferencesDlg& newPrefs = Prefs();
	if (nRet == IDOK)
	{
		// language changes may require restart so do that first
		if (UpdateLanguageTranslationAndCheckForRestart(oldPrefs))
		{
			DoExit(TRUE);
			return;
		}

		SetUITheme(newPrefs.GetUITheme());

		// mark all todoctrls as needing refreshing
		m_mgrToDoCtrls.SetAllNeedPreferenceUpdate(TRUE);

		// delete fonts if they appear to have changed
		// and recreate in UpdateToDoCtrlPrefs
		CString sFaceName;
		int nFontSize;

		if (!newPrefs.GetTreeFont(sFaceName, nFontSize) || !GraphicsMisc::SameFont(m_fontTree, sFaceName, nFontSize))
			GraphicsMisc::VerifyDeleteObject(m_fontTree);

		if (!newPrefs.GetCommentsFont(sFaceName, nFontSize) || !GraphicsMisc::SameFont(m_fontComments, sFaceName, nFontSize))
			GraphicsMisc::VerifyDeleteObject(m_fontComments);

		// topmost
#ifndef _DEBUG
		BOOL bTopMost = (newPrefs.GetAlwaysOnTop() && !IsZoomed());
		SetWindowPos(bTopMost ? &wndTopMost : &wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
#endif

		// tray icon
		m_trayIcon.ShowTrayIcon(newPrefs.GetUseSysTray());

		// support for .tdl extension
		EnableTDLExtension(newPrefs.GetEnableTDLExtension(), FALSE);

		// support for tdl:// protocol
		EnableTDLProtocol(newPrefs.GetEnableTDLProtocol(), FALSE);

		// default task attributes
		newPrefs.GetDefaultTaskAttributes(m_tdiDefault);

		// source control
		BOOL bAutoCheckOut = newPrefs.GetAutoCheckOut();
		BOOL bWantCheckOut = newPrefs.GetKeepTryingToCheckout();

		// update all open files to ensure they're in the right state
		int nCtrl = GetTDCCount();

		while (nCtrl--)
		{
			if (m_mgrToDoCtrls.IsSourceControlled(nCtrl))
			{
				if (bWantCheckOut && !m_mgrToDoCtrls.IsCheckedOut(nCtrl))
					m_mgrToDoCtrls.CheckOut(nCtrl);

				GetToDoCtrl(nCtrl).SetStyle(TDCS_CHECKOUTONLOAD, bAutoCheckOut);
			}
		}

		// menu icons
		UINT nPrevID = MapNewTaskPos(oldPrefs.GetNewTaskPos(), FALSE);
		m_mgrMenuIcons.ChangeImageID(nPrevID, GetNewTaskCmdID());

		nPrevID = MapNewTaskPos(oldPrefs.GetNewSubtaskPos(), TRUE);
		m_mgrMenuIcons.ChangeImageID(nPrevID, GetNewSubtaskCmdID());

		// reload menu 
		LoadMenubar();

		// tab bar
		if (newPrefs.GetShowTasklistTabCloseButton())
			m_tabCtrl.ModifyFlags(0, TCE_CLOSEBUTTON);
		else
			m_tabCtrl.ModifyFlags(TCE_CLOSEBUTTON, 0);

		BOOL bResizeDlg = (oldPrefs.GetAutoHideTabbar() != newPrefs.GetAutoHideTabbar());

		if (oldPrefs.GetStackTabbarItems() != newPrefs.GetStackTabbarItems())
		{
			BOOL bStackTabbar = newPrefs.GetStackTabbarItems();

			bResizeDlg = TRUE;
			m_tabCtrl.ModifyStyle(bStackTabbar ? 0 : TCS_MULTILINE, bStackTabbar ? TCS_MULTILINE : 0);
		}
		else
		{
			m_tabCtrl.Invalidate(); // handle priority colour changes
		}

		// visible filter controls
		if (m_bShowFilterBar)
			bResizeDlg = TRUE;

		BOOL bEnableMultiSel = newPrefs.GetMultiSelFilters();
		BOOL bPrevMultiSel = oldPrefs.GetMultiSelFilters();

		if (bPrevMultiSel != bEnableMultiSel)
		{
			m_filterBar.EnableMultiSelection(bEnableMultiSel);

			OnViewRefreshfilter();
		}

		m_filterBar.ShowDefaultFilters(newPrefs.GetShowDefaultFilters());

		// title filter option
		PUIP_MATCHTITLE nTitleOption = newPrefs.GetTitleFilterOption();
		PUIP_MATCHTITLE nPrevTitleOption = oldPrefs.GetTitleFilterOption();

		if (nPrevTitleOption != nTitleOption)
			OnViewRefreshfilter();

		// inherited parent task attributes for new tasks
		CTDCAttributeMap mapParentAttrib;
		BOOL bUpdateAttrib;

		newPrefs.GetParentAttribsUsed(mapParentAttrib, bUpdateAttrib);
		CFilteredToDoCtrl::SetInheritedParentAttributes(mapParentAttrib, bUpdateAttrib);

		// hotkey
		UpdateGlobalHotkey();

		// time periods
		CTimeHelper::SetHoursInWorkday(newPrefs.GetHoursInWorkday());
		CTimeHelper::SetWorkdaysInWeek(newPrefs.GetWorkdaysInWeek());
		CDateHelper::SetWeekendDays(newPrefs.GetWeekendDays());

		RefreshTabOrder();
		RefreshPauseTimeTracking();
		UpdateCaption();

		// colours
		if (m_findDlg.GetSafeHwnd())
			m_findDlg.RefreshUserPreferences();

		// active tasklist userPrefs
		UpdateActiveToDoCtrlPreferences();
		UpdateTimeTrackerPreferences();

		// then refresh filter bar for any new default cats, statuses, etc
		m_filterBar.RefreshFilterControls(GetToDoCtrl());

		if (bResizeDlg)
			Resize();

		// Stickies Support
		CString sStickiesPath;

		if (newPrefs.GetUseStickies(sStickiesPath))
			VERIFY(m_reminders.UseStickies(TRUE, sStickiesPath));
		else
			m_reminders.UseStickies(FALSE);

		// Content controls
		m_mgrContent.LoadPreferences(CPreferences(), _T("ContentControls"), TRUE);

		// Recently modified period
		CFilteredToDoCtrl::SetRecentlyModifiedPeriod(newPrefs.GetRecentlyModifiedPeriod());

		// don't ask me for the full details on this but it seems as
		// though the CSysImageList class is waiting to process a 
		// message before we can switch image sizes so we put it
		// right at the end after everything is done.
		Misc::ProcessMsgLoop();
		AppendTools2Toolbar(TRUE);
	}

	// finally set or terminate the various status check timers
	RestoreTimers();
*/
}



void NxpView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CRect rc1;
	CRect rc2;
	GetWindowRect(&rc2);//GetClientRect(&rc);
	int rcRight = rc2.right;
	int rcLeft = rc2.left;
	int rcTop = rc2.top;
	int rcBottom = rc2.bottom;
	CString str1;
	char right[20]; char left[20]; char top[20]; char bottom[20];
	itoa(rcLeft, left, 10);	itoa(rcRight, right, 10);	itoa(rcTop, top, 10);	itoa(rcBottom, bottom, 10);
	//	itoa()
	//	itoa(rcWLeft, str2, 10);
//	CString*str4 = new CString("RuleScrollWindow2 left  ");
	CString* str4 = new CString(" left  ");	CString* str5 = new CString(" right  ");	CString* str6 = new CString(" top  ");	CString* str7 = new CString(" bottom  ");
	*str4 += left; 	*str4 += "\n";	*str5 += right; *str5 += "\n";	*str6 += top;  *str6 += "\n";	*str7 += bottom;

	CWnd* cwn = GetOwner();
	CFrameWnd* frm = cwn->GetParentFrame();
	//	frm->GetMessageString()
	//	frm->SetMessageText(*str4 + *str6 + *str5 + *str7);

	//	MessageBox(*str4+*str6+*str5+*str7, "RuleScrollWindow2");

	CString* str44 = new CString(" left  ");	CString* str55 = new CString(" right  ");	CString* str66 = new CString(" top  ");	CString* str77 = new CString(" bottom  ");
	char right2[20] = "a";
	char left2[20];
	char top2[20];
	char bottom2[20];
	itoa(point.x, left2, 10);	//itoa(rcRight, right, 10);	
	itoa(point.y, top2, 10);	//itoa(rcBottom, bottom, 10);

	*str44 += left2; 	*str66 += top2;

	CRect rc3;
	frm->GetWindowRect(&rc3);
	CString* str442 = new CString(" x  ");	CString* str552 = new CString(" w  ");	CString* str662 = new CString(" y  ");	CString* str772 = new CString(" h  ");
	char left22[20];
	char top22[20];
	char right22[20];
	char bottom22[20];

	itoa(rc3.left, left22, 10);	//itoa(rcRight, right, 10);	
	itoa(rc3.top, top22, 10);	//itoa(rcBottom, bottom, 10);
	itoa(rc3.right, right22, 10);	//itoa(rcRight, right, 10);	
	itoa(rc3.bottom, bottom22, 10);	//itoa(rcBottom, bottom, 10);

	*str442 += left22; 	*str662 += top22;
	*str552 += right22; 	*str772 += bottom22;

	CRect rc4;
	GetClientRect(&rc4);
	char left222[20];
	char top222[20];
	char right222[20];
	char bottom222[20];

	itoa(rc4.left, left222, 10);
	itoa(rc4.top, top222, 10);
	itoa(rc4.right, right222, 10);
	itoa(rc4.bottom, bottom222, 10);

	CString* StrLeft222 = new CString(" X  ");	CString* StrTop222 = new CString(" Y  ");	CString* StrRight222  = new CString(" W  ");	CString* StrBottom222 = new CString(" H  ");
	*StrLeft222 += left222; 	*StrTop222 += top222;
	*StrRight222 += right222; 	*StrBottom222 += bottom222;


	frm->SetMessageText("NxpView::OnLButtonDown: mouse " + *str44 + *str66 + "  |||||||||||||||| frm->GetWindowRect(&rc3); " + *str442 + *str662 + *str552 + *str772+"  GetClientRect(&rc4); " + *StrLeft222 + *StrTop222 + *StrRight222 + *StrBottom222);

	rc1.left = point.x;
	rc1.top = point.y;

	KBaseDoc*kbDoc=(KBaseDoc*)GetDocument();
	KnowledgeBase* knowBase = kbDoc->knowBase;
	if (knowBase != NULL)
	{
		string* kbName = knowBase->fileName;
		if (kbName!= NULL)
		{
			frm->SetWindowTextA(kbName->c_str());
		}

	}


	CView::OnLButtonDown(nFlags, point);
}

void NxpView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CView::OnLButtonUp(nFlags, point);
}

void NxpView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CView::OnRButtonDown(nFlags, point);
}

void NxpView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CView::OnRButtonUp(nFlags, point);
}

void NxpView::OnMove(int x, int y)
{
	CString* str44 = new CString(" left  ");	CString* str55 = new CString(" right  ");	CString* str66 = new CString(" top  ");	CString* str77 = new CString(" bottom  ");

	char left2[20];
	char right2[20];

	char top2[20];
	char bottom2[20];
	itoa(x, left2, 10);	//itoa(rcRight, right, 10);	
	itoa(y, top2, 10);	//itoa(rcBottom, bottom, 10);

	CRect rr;
	GetWindowRect(&rr);


	int right22;
	int bottom22;

	right22 = x + rr.right;
	bottom22 = x + rr.bottom;


	itoa(right22, right2, 10);
	itoa(bottom22, bottom2, 10);

	*str44 += left2; 	*str66 += top2;
	*str55 += right2; 	*str77 += bottom2;

	CWnd* cwn = GetOwner();
	CFrameWnd* frm = cwn->GetParentFrame();
	CString coordStr;

	frm->SetMessageText("NxpView::OnMove Координаты окна " +*str44 + *str66 + *str55 + *str77);

	CView::OnMove(x, y);
	// TODO: Add your message handler code here
}


void NxpView::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CView::OnWindowPosChanged(lpwndpos);
	CString* str44 = new CString(" left  ");	CString* str55 = new CString(" width  ");	CString* str66 = new CString(" top  ");	CString* str77 = new CString(" height  ");

	char left[20];
	char right[20];
	char top[20];
	char bottom[20];

	itoa(lpwndpos->x, left, 10);
	itoa(lpwndpos->y, top, 10);

	itoa(lpwndpos->cx, right, 10);
	itoa(lpwndpos->cy, bottom, 10);

	*str44 += left; 	*str66 += top;
	*str55 += right; 	*str77 += bottom;

	CWnd* cwn = GetOwner();
	CFrameWnd* frm = cwn->GetParentFrame();
	CString coordStr;

	frm->SetMessageText("NxpView::OnWindowPosChanged WINDOWPOS " + *str44 + *str66 + *str55 + *str77);

	CRect rc;
	GetWindowRect(&rc);

	// TODO: Add your message handler code here
}


int NxpView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
//	lpCreateStruct->cx = lpCreateStruct->cx / 2;
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}


BOOL NxpView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
//	cs.cx = cs.cx / 2;
	return CView::PreCreateWindow(cs);
}
