#include "stdafx.h"
#include "b50.h"
#include "b50Set.h"
#include "b50Doc.h"
#include "CntrItem.h"
#include "b50View.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
IMPLEMENT_DYNCREATE(CB50View,CDaoRecordView)//CView)
BEGIN_MESSAGE_MAP(CB50View,CDaoRecordView)
	//{{AFX_MSG_MAP(CB50View)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_COMMAND(ID_OLE_INSERT_NEW, OnInsertObject)
	ON_COMMAND(ID_CANCEL_EDIT_CNTR, OnCancelEditCntr)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CDaoRecordView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CDaoRecordView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CDaoRecordView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()
CB50View::CB50View():CDaoRecordView(CB50View::IDD)
{
	//{{AFX_DATA_INIT(CB50View)
		// NOTE: the ClassWizard will add member initialization here
	m_pSet=NULL;
	//}}AFX_DATA_INIT
	m_pSelection = NULL;
	// TODO: add construction code here
}
CB50View::~CB50View(){}
void CB50View::DoDataExchange(CDataExchange*pDX)
{
	CDaoRecordView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CB50View)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}
BOOL CB50View::PreCreateWindow(CREATESTRUCT&cs){return CDaoRecordView::PreCreateWindow(cs);}
void CB50View::OnInitialUpdate()
{
	m_pSet=&GetDocument()->m_b50Set;
	CDaoRecordView::OnInitialUpdate();
	// TODO: remove this code when final selection model code is written
	m_pSelection=NULL;    // initialize selection
}
BOOL CB50View::OnPreparePrinting(CPrintInfo*pInfo){return DoPreparePrinting(pInfo);}
void CB50View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/){}
void CB50View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/){}
void CB50View::OnDestroy()
{
	// Deactivate the item on destruction; this is important
	// when a splitter view is being used.
   CDaoRecordView::OnDestroy();
   COleClientItem*pActiveItem=GetDocument()->GetInPlaceActiveItem(this);
   if(pActiveItem!=NULL&&pActiveItem->GetActiveView()==this)
   {
      pActiveItem->Deactivate();ASSERT(GetDocument()->GetInPlaceActiveItem(this)==NULL);
   }
}
/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands
BOOL CB50View::IsSelected(const CObject* pDocItem) const
{
	// The implementation below is adequate if your selection consists of
	//  only CB50CntrItem objects.  To handle different selection
	//  mechanisms, the implementation here should be replaced.
	// TODO: implement this function that tests for a selected OLE client item
	return pDocItem==m_pSelection;
}
void CB50View::OnInsertObject()
{
	// Invoke the standard Insert Object dialog box to obtain information
	//  for new CB50CntrItem object.
	COleInsertDialog dlg;
	if(dlg.DoModal()!=IDOK)return;
	BeginWaitCursor();
	CB50CntrItem*pItem=NULL;
	TRY
	{
		// Create new item connected to this document.
		CB50Doc*pDoc=GetDocument();
		ASSERT_VALID(pDoc);
		pItem=new CB50CntrItem(pDoc);
		ASSERT_VALID(pItem);
		// Initialize the item from the dialog data.
		if(!dlg.CreateItem(pItem))AfxThrowMemoryException();  // any exception will do
		ASSERT_VALID(pItem);
		// If item created from class list (not from file) then launch
		//  the server to edit the item.
		if(dlg.GetSelectionType() == COleInsertDialog::createNewItem)pItem->DoVerb(OLEIVERB_SHOW, this);
		ASSERT_VALID(pItem);
		// As an arbitrary user interface design, this sets the selection
		//  to the last item inserted.
		// TODO: reimplement selection as appropriate for your application
		m_pSelection = pItem;   // set selection to last inserted item
		pDoc->UpdateAllViews(NULL);
	}
	CATCH(CException,e)
	{
		if(pItem!=NULL){ASSERT_VALID(pItem);pItem->Delete();}AfxMessageBox(IDP_FAILED_TO_CREATE);
	}
	END_CATCH
	EndWaitCursor();
}
// The following command handler provides the standard keyboard
//  user interface to cancel an in-place editing session.  Here,
//  the container (not the server) causes the deactivation.
void CB50View::OnCancelEditCntr()
{
	// Close any in-place active item on this view.
	COleClientItem*pActiveItem=GetDocument()->GetInPlaceActiveItem(this);
	if(pActiveItem!=NULL){pActiveItem->Close();}
	ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
}
// Special handling of OnSetFocus and OnSize are required for a container
//  when an object is being edited in-place.
void CB50View::OnSetFocus(CWnd* pOldWnd)
{
	COleClientItem*pActiveItem=GetDocument()->GetInPlaceActiveItem(this);
	if(pActiveItem!=NULL&&pActiveItem->GetItemState()==COleClientItem::activeUIState)
	{
		// need to set focus to this item if it is in the same view
		CWnd*pWnd=pActiveItem->GetInPlaceWindow();
		if(pWnd!=NULL){pWnd->SetFocus();return;}
	}CDaoRecordView::OnSetFocus(pOldWnd);
}
void CB50View::OnSize(UINT nType, int cx, int cy)
{
	CDaoRecordView::OnSize(nType,cx,cy);
	COleClientItem*pActiveItem=GetDocument()->GetInPlaceActiveItem(this);
	if(pActiveItem!=NULL)pActiveItem->SetItemRects();
}
#ifdef _DEBUG
void CB50View::AssertValid()const{CDaoRecordView::AssertValid();}
/*
void CB50View::Dump(CDumpContext&dc)const
{
//	CDaoRecordView::Dump(dc);
}
*/
CB50Doc* CB50View::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CB50Doc)));return (CB50Doc*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CB50View database support
CDaoRecordset*CB50View::OnGetRecordset(){return m_pSet;}
/*
void CB50View::OnDraw(CDC* pDC) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	
}
*/

void CB50View::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDaoRecordView::OnLButtonDown(nFlags, point);
}


void CB50View::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDaoRecordView::OnLButtonUp(nFlags, point);
}


void CB50View::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDaoRecordView::OnRButtonDown(nFlags, point);
}


void CB50View::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDaoRecordView::OnRButtonUp(nFlags, point);
}
