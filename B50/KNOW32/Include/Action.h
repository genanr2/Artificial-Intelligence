// Action.h: interface for the Action class.
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_ACTION_H__C504D8A6_D6CB_11D2_862A_00A024384FCF__INCLUDED_)
#define AFX_ACTION_H__C504D8A6_D6CB_11D2_862A_00A024384FCF__INCLUDED_
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
typedef CTypedPtrMap<CMapWordToOb,DWORD,Action*>CMapDWordToAction;
typedef CTypedPtrArray<CObArray, Action*>ActionsList; // ListOfRhsActions
typedef CTypedPtrArray<CObArray, CAssignAssignment*>AssignAssignmentList; // ListOfRhsActions
typedef CTypedPtrArray<CObArray, CResetAssignment*>CResetAssignmentList; // ListOfRhsActions

//CAssignAssignment*assignAssignment;

class AFX_EXT_CLASS Action : public NxpAtom  
{
public:
	Action();
	virtual~Action()
	{
//		assignAssignmentLst = new AssignAssignmentList;
//		resetAssignmentLst = new CResetAssignmentList;
	}
	Action(const Action&c)
	{
		secondStr=c.secondStr;
		aType=c.aType;
		thirdStr=c.thirdStr;
		atomType=c.atomType;
		assignAssignmentLst = new AssignAssignmentList;
		resetAssignmentLst = new CResetAssignmentList;
	}
	void operator=(const Action&c)
	{
		secondStr=c.secondStr;
		aType=c.aType;
		thirdStr=c.thirdStr;
		atomType=c.atomType;
		assignAssignmentLst = new AssignAssignmentList;
		resetAssignmentLst = new CResetAssignmentList;
	}
	void operator=(const Action*c)
	{
		secondStr=c->secondStr;
		aType=c->aType;
		thirdStr=c->thirdStr;
		atomType=c->atomType;
		assignAssignmentLst = new AssignAssignmentList;
		resetAssignmentLst = new CResetAssignmentList;
	}
	bool operator==(const Action&c)const{return this==&c;}
	bool operator==(const Action*c)const{return this==c;}
//	enum actionType { AssignmentTypeActs, DynamicType, InterfaceTypeActs };
	actionType aType;
	_NxAtomTypeEnum atomType;
	int actValue;
	CString*secondStr;
	CString*thirdStr;
	virtual void printOn(std::ostream&ofStream)const;
	union
	{
		//		LhsAssignments*assignments;
		CMapDWordToLhsAssignment*assignments;
		CMapDWordToInterfaceAction*interfaceActions;
		//		CMapDWordToDynObjectsAction*dynActions;
		CDynObjectsAction*dynActions;
		CMapDWordToInheritanceControl*inheritanceControls;
		CMapDWordToLhsTest*tests;
	}gen;

	CAssignAssignment*assignAssignment;
	AssignAssignmentList*assignAssignmentLst;
	CResetAssignmentList*resetAssignmentLst;

	void SetAssingments(RhsAssignments*rhsA);
	void SetDynObjectsActions(DynObjectsActions*rhsDA);
	void SetInterfaceActions(InterfaceActions*rhsDA);
	void SetInheritanceControls(InheritanceControls*rhsA);

//	InterfaceActions*rhsA = ac->gen.interfaceActions;


	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(Action)
};
#endif // !defined(AFX_ACTION_H__C504D8A6_D6CB_11D2_862A_00A024384FCF__INCLUDED_)
