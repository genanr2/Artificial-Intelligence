#include "..\b54mdll\stdafx.h"
#include "..\b54mdll\b54mdll.h"
#ifndef __OBJBROWSER_H
#include "ObjBrowser.h"
#endif

IMPLEMENT_DYNCREATE(ObjBrowser, CView)
ObjBrowser::ObjBrowser(){}
ObjBrowser::~ObjBrowser(){}
BEGIN_MESSAGE_MAP(ObjBrowser, CView)
	//{{AFX_MSG_MAP(SlotView)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// SlotView drawing
void ObjBrowser::OnDraw(CDC* pDC) { CDocument* pDoc = GetDocument(); }
/////////////////////////////////////////////////////////////////////////////
// SlotView diagnostics
#ifdef _DEBUG
void ObjBrowser::AssertValid()const { CView::AssertValid(); }
#endif //_DEBUG
void ObjBrowser::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rc;
	CRect rc2;
	GetWindowRect(&rc2);
	GetClientRect(&rc);
	int rcLeft = rc.right;
	int rcWLeft = rc2.left;
	CString str1;
	char str2[20];;
	char str3[20];;

	itoa(rcLeft, str2, 10);
	itoa(rcWLeft, str3, 10);
	//	itoa()
//	itoa(rcWLeft, str2, 10);
	CString* str4 = new CString("ObjBrowser right  ");
	CString* str5 = new CString("ObjBrowser left  ");
	*str4 += str2;
	*str5 += str3;
	MessageBox(*str4, *str5);
	CView::OnLButtonDown(nFlags, point);
}

bool ObjNode::containsPoint(POINT point, RECT extent)
{
	if (column < point.x < extent.right)if (row < point.y < extent.bottom)return true;
	return false;
}
void ObjNode::printOn(strstream&)
{
	//	btreeNode->printOn(stream);
}
//_export RuleNode::RuleNode(RuleItem*anObj,int col,int rowInt,RuleNode*aNode)
ObjNode::ObjNode(ObjectItem* anObj, int col, int rowInt, ObjNode* aNode)
{
	btreeNode = anObj;
	column = col; row = rowInt; son = aNode; brother = aNode;
	isLeaf = TRUE;
}
ObjNode::ObjNode(ObjectItem* anObj, int col, int rowInt)
{
	//	btreeNode = anObj;
	char outBuf[81];
	strstream* ostr = new strstream();
	ObjNode* a;
	btreeNode = anObj;

//	value = new string(anObj->hypo->slotName->c_str());
	value = new string(anObj->objectName->c_str());

	column = col; row = rowInt;
	brother = NULL; son = NULL;
	isLeaf = TRUE;
}
