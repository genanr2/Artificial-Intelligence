#pragma once
#ifndef __OBJBROWSER_H
#define __OBJBROWSER_H

#include <afxwin.h>

#include <StrStream>
//#endif
#include <sStream>
//#ifndef __OBJSTRM_H
//#include <classlib\objStrm.h>
//#endif
#if !defined( __KBASE_H )
#include "KBase.h"
#endif // __KBASE_H

class AFX_EXT_CLASS ObjBrowser :	public CView
{
protected:
	DECLARE_DYNCREATE(ObjBrowser)
public:
	ObjBrowser();
	virtual ~ObjBrowser();
protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view //}}AFX_VIRTUAL Implementation
//protected:
//	virtual ~ObjBrowser();
#ifdef _DEBUG
	virtual void AssertValid() const;
	//	virtual void Dump(CDumpContext& dc) const;
#endif
	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

class ObjNode :public ObjBrowser
{
public:
	ObjNode()
	{
		btreeNode = NULL;
		column = 0; row = 0; son = NULL; brother = NULL;
		isLeaf = TRUE;
	}
	ObjNode(CString* str)
	{
		btreeNode = NULL;
		column = 0; row = 0; son = NULL; brother = NULL;
		isLeaf = TRUE;
		strValue = str;
	}
	~ObjNode()
	{
		delete value; delete son; delete brother; delete btreeNode;
	}
	//		virtual classType isA() const {return ruleNode;}
	virtual char* nameOf() const { return "RuleNode"; }
	//		virtual int isEqual( const Object& ) const;
	ObjNode(ObjectItem* anObj, int col, int rowInt);
	//	RuleNode(RuleNode*anObj, int col, int rowInt);
	ObjNode(ObjectItem* anObj, int col, int rowInt, ObjNode* aNode);
	int retRow() { return row; }
	int getCol() { return column; }
	ObjectItem* retBtreeNode() { return btreeNode; }
	ObjNode* retBrother() { return brother; }
	ObjNode* retSon() { return son; }
	string& getValue() { return *value; }

		void setRow(int r) { row = r; }
	void setCol(int col) { column = col; }
	void setBrother(ObjNode* br) { brother = br; }
	void setSon(ObjNode* s) { son = s; }
	void setBtreeNode(ObjectItem* bTr) { btreeNode = bTr; }
	virtual void printOn(strstream& stream);

	union { ClassItem* cls; ObjectItem* obj; PropertyItem* prop; SlotItem* slot; }gen;
//	enum DescType { ObjectType = 1, ClassType = 2, PropType = 3, SlotType = 4 };
	DescType NodeType;


	BOOL isLeaf;
	string* value;
	CString* strValue;

	ObjNode* getLast() { return head; }
	ObjNode* getFirst() { return head; }
private:
	int column, row;
	ObjNode* son;
	ObjNode* brother;
	ObjectItem* btreeNode;
	int retColumn() { return column; }
	//		PObject retValue()            { return value; }
	string* retValue() { return value; }
	bool containsPoint(POINT point, RECT extent);
	void valueAndSon(ObjectItem* anObj, int col, int rowInt, ObjNode* aNode)
	{
		btreeNode = anObj;
		column = col; row = rowInt; son = aNode; brother = aNode;
	}
	ObjNode* head;
};

#endif