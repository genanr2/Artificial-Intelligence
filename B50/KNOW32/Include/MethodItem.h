// ClassItem.h: interface for the ClassItem class.
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_CMethodItem_H__C504D8A2_D6CB_11D2_862A_00A024384FCF__INCLUDED_)
#define AFX_CMethodItem_H__C504D8A2_D6CB_11D2_862A_00A024384FCF__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#ifndef _INC_CTYPE
#include <CTYPE.H>
#endif
#ifndef _STRING_
#include "string"
#endif
using namespace std;
#include "NxpAtom.h"
class AFX_EXT_CLASS Atom_Id :public NxpAtom
{

};
typedef CTypedPtrMap<CMapStringToOb,CString, CMethodItem*>CMapStringToCMethodItem;
class AFX_EXT_CLASS CMethodItem :public NxpAtom
{
public:
	CMethodItem();
	virtual~CMethodItem()
	{
//		if(slot) delete slot;
	}
	CMethodItem(const CMethodItem&cl)
	{
//		slot=cl.slot;
		type=cl.type; // class.
		methodName = cl.methodName;
		methodName2 = cl.methodName2;
//		strcpy(methodName3, cl.methodName3);
		actions = cl.actions;
		MapOfActions = cl.MapOfActions;
		flagType = cl.flagType;
		methodType = cl.methodType;

	}
	void operator=(const CMethodItem& cm)  //assignment operator
	{
		type = cm.type;
		methodName = cm.methodName;
		methodName2 = cm.methodName2;
		actions = cm.actions;
		MapOfActions = cm.MapOfActions;
		flagType = cm.flagType;
		methodType = cm.methodType;

	}
	void operator=(const CMethodItem* cm)
	{
		type = cm->type;
		methodName = cm->methodName;
		methodName2 = cm->methodName2;
		actions = cm->actions;
		MapOfActions = cm->MapOfActions;
		flagType = cm->flagType;
		methodType = cm->methodType;
	}
//	SlotItem*slot;  // @INFATOM
	CString*methodName;
	string* methodName2;
	char* methodName3;
	Atom_Id atom_Id;
	int flags;

	ListOfRhsActions* actions;    // @RHS
	CMapDWordToAction* MapOfActions;

	int type;//0 - class, 1 - |class|.
	//int methodType;
	enum FlagType flagType;
	enum MethodType methodType;
	virtual char*nameOf()const{return"CMethodItem";}
	void setActions(yListOfRhsActions* actLis);
	BOOL operator==(const CMethodItem&c)const{return this==&c;}
//	NxpAtomType atomType;
	virtual void printOn(std::ostream&ofStream)const;
	virtual void printOnAsLine(std::ostream&ofStream)const;
	virtual void read(std::istream&is);
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CMethodItem)
};
#endif // !defined(AFX_CMethodItem_H__C504D8A2_D6CB_11D2_862A_00A024384FCF__INCLUDED_)
