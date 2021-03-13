#if !defined(AFX_NXPVIEW_H__B2303643_C991_11D1_A093_00A024384FCF__INCLUDED_)
#define AFX_NXPVIEW_H__B2303643_C991_11D1_A093_00A024384FCF__INCLUDED_
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
class AFX_EXT_CLASS NxpView: public CView
{
protected:
	NxpView();           // protected constructor used by dynamic creation
	virtual LRESULT DefWindowProc(UINT message,WPARAM wParam,LPARAM lParam);
	DECLARE_DYNCREATE(NxpView)
// Attributes
public:
//	CPreferencesDlg* m_pPrefs;
	//	virtual BOOL PreTranslateMessage(MSG* pMsg);
//	virtual BOOL OnIdle(LONG lCount);
/*
  static int notify(  int, int, unsigned long); // nxpmain2.cpp
  static int getData( int, int, int, char * );  // nxpmain2.cpp
  static int setData( int, int, int, char * );  // nxpmain2.cpp
  static int alert(int Code,char *thePtr,int *ret); // nxpmain2.cpp
  static int updateValue( char *atomStr, int nAtoms, unsigned long *qAtoms ); // nxpmain2.cpp
//  static int question( unsigned long, char * );
//	  static int amountOfTime( char *, int, unsigned long *);
  static int getStatus(int winId,int Code,int *ret); // // nxpmain2.cpp
//  static 
	NxpView*mainWindow;
*/
// Operations
public:
//	DGramExWinSelectPtr win;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(NxpView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL
// Implementation
protected:
	virtual ~NxpView();
#ifdef _DEBUG
	virtual void AssertValid() const;
//	virtual void Dump(CDumpContext& dc) const;
#endif
/*
private:
	unsigned long currentAtomId;
	unsigned long currentKBase;
	enum LOADTYPE{task,fromFile};
	LOADTYPE loadType;
	CString currentAlert,currentShowDB,currentShow,promptLine,topic,hypoName;
	CString*baseFile;
//	knowBases = new BaseList();
	BaseList*knowBases;
	Strategies*strategies;
	Strategy*currentStrategy;
	Strategy*defaultStrategy;
//	TMDIChild* childMandela,*childJulia;
	NXP_Compiler*nxpCompiler;
	bool nexpert;
	bool restart, loaded, unloads, yesStation, noNetWare, noNetDefined, alreadyStopped;
	BOOL SaveKnowledgeBase(CString*savefile,int); // unloadType
	SaveProperties(std::ofstream&os,PropertyDescription&d);
	SaveClasses(std::ofstream&os,ClassDescription&d);
	SaveObjects(ofstream&os,ObjectDescription&d);
	SaveSlots(std::ostream&os,SlotDescription&d);
	SaveRules(std::ofstream&os,RuleDescription*d);
	CProgressCtrl	m_progress;
	CAnimateCtrl	m_avi;
*/
//	AtomViewFrame atomsView;
	// Generated message map functions
protected:
/*
	afx_msg void LoadKB();
	afx_msg void SaveKB();
*/
	CPreferencesDlg* m_pPrefs;
	void ResetPrefs();
	CShortcutManager m_mgrShortcuts;

	CTDCImportExportMgr m_mgrImportExport;
//	CTDLTasklistStorageMgr m_mgrStorage;
//	CToDoCtrlMgr m_mgrToDoCtrls;
	CTDLContentMgr m_mgrContent;
//	CWndPromptManager m_mgrPrompts;
//	CMenuIconMgr m_mgrMenuIcons;
	CUIExtensionMgr m_mgrUIExtensions;
	CUIThemeFile m_theme;
	CToDoCtrlMgr m_mgrToDoCtrls;

	CTDLFilterBar m_filterBar;

	CTabCtrlEx m_tabCtrl;


	void DoPreferences(int nInitPage);
	//{{AFX_MSG(NxpView)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg void AtomTreeView();
	afx_msg void WinSetup();
	afx_msg void SetStrategies();
	afx_msg void Diagram();
	afx_msg void Diagram2();

//	afx_msg void REdit();
//	afx_msg void ClEdit();
//	afx_msg void ObjEdit();
//	afx_msg void PropEdit();
//	afx_msg void MetaSlotEdit();
//	afx_msg void RuleGraph();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPreferences();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_NXPVIEW_H__B2303643_C991_11D1_A093_00A024384FCF__INCLUDED_)
