#include "stdafx.h"
#include "b50.h"
#include "b50Set.h"
#include "b50Doc.h"
#include "CntrItem.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
IMPLEMENT_DYNCREATE(CB50Doc,COleDocument)
BEGIN_MESSAGE_MAP(CB50Doc,COleDocument)
	//{{AFX_MSG_MAP(CB50Doc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Enable default OLE container implementation
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE,COleDocument::OnUpdatePasteMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK,COleDocument::OnUpdatePasteLinkMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT,COleDocument::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_OLE_EDIT_CONVERT,COleDocument::OnEditConvert)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS,COleDocument::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS,COleDocument::OnEditLinks)
	ON_UPDATE_COMMAND_UI(ID_OLE_VERB_FIRST,COleDocument::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_FILE_SEND_MAIL,OnFileSendMail)
	ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL,OnUpdateFileSendMail)
END_MESSAGE_MAP()
BEGIN_DISPATCH_MAP(CB50Doc, COleDocument)
	//{{AFX_DISPATCH_MAP(CB50Doc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//      DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()
// Note: we add support for IID_IB50 to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.
// {350A2778-BFEC-11D1-A093-00A024384FCF}
static const IID IID_IB50={0x350a2778,0xbfec,0x11d1,{0xa0,0x93,0x0,0xa0,0x24,0x38,0x4f,0xcf}};
BEGIN_INTERFACE_MAP(CB50Doc, COleDocument)
	INTERFACE_PART(CB50Doc,IID_IB50, Dispatch)
END_INTERFACE_MAP()
CB50Doc::CB50Doc()
{
	// Use OLE compound files
	EnableCompoundFile();
	// TODO: add one-time construction code here
	EnableAutomation();
	AfxOleLockApp();
}
CB50Doc::~CB50Doc(){AfxOleUnlockApp();}
BOOL CB50Doc::OnNewDocument()
{
	if(!COleDocument::OnNewDocument())return FALSE;
	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	return TRUE;
}
void CB50Doc::Serialize(CArchive&ar)
{
	if(ar.IsStoring()){}else{}
	// Calling the base class COleDocument enables serialization
	//  of the container document's COleClientItem objects.
	COleDocument::Serialize(ar);
}
#ifdef _DEBUG
void CB50Doc::AssertValid()const{COleDocument::AssertValid();}
/*
void CB50Doc::Dump(CDumpContext&dc)const
{
//	COleDocument::Dump(dc);
}
*/
#endif //_DEBUG
