// Condition.h: interface for the CCondition class.
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_CONDITION_H__C504D8A5_D6CB_11D2_862A_00A024384FCF__INCLUDED_)
#define AFX_CONDITION_H__C504D8A5_D6CB_11D2_862A_00A024384FCF__INCLUDED_
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
//#include "..\KbParse\Defs.h"
#include "NxpAtom.h"
typedef CTypedPtrMap<CMapWordToOb,DWORD,CCondition*>CMapDWordToCondition;
typedef CTypedPtrMap<CMapWordToOb,DWORD,CInterfaceAction*>CMapDWordToInterfaceAction;
typedef CTypedPtrMap<CMapWordToOb,DWORD,CLhsAssignment*>CMapDWordToLhsAssignment;
typedef CTypedPtrMap<CMapWordToOb,DWORD,CDynObjectsAction*>CMapDWordToDynObjectsAction;
typedef CTypedPtrMap<CMapWordToOb,DWORD,CInheritanceControl*>CMapDWordToInheritanceControl;
typedef CTypedPtrMap<CMapWordToOb,DWORD,CLhsTest*>CMapDWordToLhsTest;
typedef CTypedPtrMap<CMapWordToOb,DWORD,CRetrieveKeyWord*>CMapDWordToRetrieveKeyWord;
typedef CTypedPtrMap<CMapWordToOb,DWORD,CShowKeyWord*>CMapDWordToShowKeyWord;
typedef CTypedPtrMap<CMapWordToOb,DWORD,CGenericSlot*>CMapDWordToGenericSlot;
//typedef CTypedPtrMap<CMapWordToOb,DWORD,CStrategyKeyWord*>CListOfStrategyKeyWords;
typedef CList<CStrategyKeyWord,CStrategyKeyWord&>CListOfStrategyKeyWords;

typedef CList<string,string&>CNameList;
typedef CList<string,string&>CListOfQuotedString;
typedef CList<KnownValue*,KnownValue*>CConstants;
typedef CList<CGenericObjectsOrClasses,CGenericObjectsOrClasses&>CListOfGenericObjectsOrClasseses;
typedef CList<CExpression,CExpression&>CListOfExpression;

class AFX_EXT_CLASS CCondition:public NxpAtom  
{
public:
	CCondition();
	virtual~CCondition(){}
	CCondition(const CCondition&c);
	void operator=(const CCondition&c);
	void operator=(const CCondition*c);
//BooleanType,EqualityType,StatusType,AssignmentTypeCond,MemberType,InterfaceTypeCond,
//DynActions,InheritanceControlsCond,TestCondition
	conditionType cType;
	int condValue;
	CString*secondStr;
	CString*thirdStr;
	NxpAtom *obj;
	virtual void printOn(std::ostream&ofStream)const;
	virtual void printOnAssignments(std::ostream&os, CMapDWordToLhsAssignment * ass) const;
	virtual void printOnInterAct(std::ostream&os, CMapDWordToInterfaceAction * inter) const;
	virtual void printOnDynActions(std::ostream&os, CDynObjectsAction * dynAct) const;
	virtual void printOnInheritanceControls(std::ostream&os, CMapDWordToInheritanceControl * inheritances) const;
	virtual void printOnTestCondition(std::ostream&os, CMapDWordToLhsTest * tst) const;

//	virtual void printOn(stringstream&ofStream)const;
	union
	{
//		LhsAssignments*assignments;
		CMapDWordToLhsAssignment*assignments;/////////////////////////////
		CMapDWordToInterfaceAction*interfaceActions;/////////////////////////////
//		CMapDWordToDynObjectsAction*dynActions;
		CDynObjectsAction*dynActions;/////////////////////////////////////////////
//		CMapDWordToDynObjectsAction*dynActions;/////////////////////////////////////////////
		CMapDWordToInheritanceControl*inheritanceControls;///////////////////////////////////////
		CMapDWordToLhsTest*tests;//////////////////////////////////////
	}gen;
//assignments=1,interfaceActions=2,dynActions=3,inheritanceControls=4,testCondition=5
	TypeOfCondition type;
	void setLhsAssignments(LhsAssignments*ass);/////////////////////////////
	void setInterfaceActions(InterfaceActions*act);
	void setDynActions(DynObjectsActions*act);
	void setInheritanceControls(InheritanceControls*cont);
	void setTestConditions(LhsTests*tests);
	_NxAtomTypeEnum atomType;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CCondition)
};
class AFX_EXT_CLASS CLhsAssignment:public NxpAtom  
{
public:
	CLhsAssignment();virtual~CLhsAssignment(){}
	CLhsAssignment(LhsAssignment*ass);
	CLhsAssignment(const CLhsAssignment&c);
	void operator=(const CLhsAssignment&c);
	void operator=(const CLhsAssignment*c);
	virtual void printOn(std::ostream&ofStream)const;
	//Do=1,Let=2,ActsReset=3,Assign=4
//	enum AssignmentTypeActs { Do = 1, Let = 2, ActsReset = 3, Assign = 4 };
	union
	{
		CNameAssignment*name;
		CResetAssignment*reset;
		CAssignAssignment*assign;
	}lhs;
	enum AssignmentTypeActs type;
	char*getTypeString() const;
	void getFirstPart(std::stringstream&os);
	void getSecondPart(std::stringstream&os);


//	void setNameAssignments(NameAssignment*ass);
//	void setAssignAssignmets(AssignAssignment*ass);
//	void setResetAssignment(ResetAssignment*ass);
	_NxAtomTypeEnum atomType;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CLhsAssignment)
};


class AFX_EXT_CLASS CInterfaceAction:public NxpAtom  
{
public:
	CInterfaceAction();virtual~CInterfaceAction(){}
	CInterfaceAction(InterfaceAction*act);
	CInterfaceAction(const CInterfaceAction&c);
	void operator=(const CInterfaceAction&c);
	void operator=(const CInterfaceAction*c);
	virtual void printOn(std::ostream&ofStream)const;
	void printAskQuestion(CAskQuestion* aa, std::ostream& os);
	void printGenericSlot(CGenericSlot* slot, std::ostream& os);
	void printSlotPattern(CSlotPattern* aa, std::ostream& os);

	_NxAtomTypeEnum atomType;
//ShowActs=6,ExecuteActs=7,RetrieveActs=8,Write=9,LoadKB=10,UnloadKB=11,AskQuestionType=12
	enum InterfaceTypeActs type;
	union
	{
		CShowActions*showAct;			CExecuteActions*execAct;
		CRetrieveActions*retrAct;	CWriteActions*writeAct;
		CLoadKbActions*loadAct;		CUnloadKbActions*unloadAct;	CAskQuestion*askAct;
	}gen;
	virtual void Serialize(CArchive&ar);
	char*getTypeString() const;
	void getFirstPart(std::ostream&os) const;
	void getSecondPart(std::ostream&os) const;

#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CInterfaceAction)
};



class AFX_EXT_CLASS CShowActions:public NxpAtom
{
public:
	CShowActions();virtual~CShowActions(){}
	CShowActions(ShowActions*act);
	CShowActions(const CShowActions&c){str=c.str;keys=c.keys;}
	void operator=(const CShowActions&c){str=c.str;keys=c.keys;}
	void operator=(const CShowActions*c){str=c->str;keys=c->keys;}
	_NxAtomTypeEnum atomType;
	string*str;
	CMapDWordToShowKeyWord*keys;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CShowActions)
};
class AFX_EXT_CLASS CShowKeyWord:public NxpAtom
{
public:
	CShowKeyWord();virtual~CShowKeyWord(){}
	CShowKeyWord(ShowKeyWord*key);
	CShowKeyWord(const CShowKeyWord&c);
	void operator=(const CShowKeyWord&c);
	void operator=(const CShowKeyWord*c);
	_NxAtomTypeEnum atomType;
	union
	{
		TrueFalseType keep;
		TrueFalseType wait;
		TrueFalseType rect;
	}gen;
	ShowWordsType type;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CShowKeyWord)
};
class AFX_EXT_CLASS CExecuteActions:public NxpAtom
{
public:
	CExecuteActions();virtual~CExecuteActions(){}
	CExecuteActions(ExecuteActions*act);
	CExecuteActions(const CExecuteActions&c){str=c.str;keys=c.keys;}
	void operator=(const CExecuteActions&c){str=c.str;keys=c.keys;}
	void operator=(const CExecuteActions*c){str=c->str;keys=c->keys;}
	_NxAtomTypeEnum atomType;
	string*str;
	CListOfExecuteKeyWords*keys;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CExecuteActions)
};
class AFX_EXT_CLASS CListOfExecuteKeyWords:public NxpAtom
{
public:
	CListOfExecuteKeyWords();virtual~CListOfExecuteKeyWords(){}
	CListOfExecuteKeyWords(ListOfExecuteKeyWords*keys);
	CListOfExecuteKeyWords(const CListOfExecuteKeyWords&c)
	{
		exe=c.exe;wait=c.wait;frm=c.frm;str=c.str;atomId=c.atomId;
	}
	void operator=(const CListOfExecuteKeyWords&c)
	{
		exe=c.exe;wait=c.wait;frm=c.frm;str=c.str;atomId=c.atomId;
	}
	void operator=(const CListOfExecuteKeyWords*c)
	{
		exe=c->exe;wait=c->wait;frm=c->frm;str=c->str;atomId=c->atomId;
	}
	_NxAtomTypeEnum atomType;
	virtual void Serialize(CArchive&ar);
	TrueFalseType exe;
	TrueFalseType wait;
	TrueFalseType frm;
	string*str;
	CGenSlotsObjectsClasses*atomId;
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CListOfExecuteKeyWords)
};
class AFX_EXT_CLASS CGenSlotsObjectsClasses:public NxpAtom
{
public:
	CGenSlotsObjectsClasses();virtual~CGenSlotsObjectsClasses(){}
	CGenSlotsObjectsClasses(GenSlotsObjectsClasses*atomId);
	CGenSlotsObjectsClasses(const CGenSlotsObjectsClasses&c);
	void operator=(const CGenSlotsObjectsClasses&c);
	void operator=(const CGenSlotsObjectsClasses*c);
	_NxAtomTypeEnum atomType;
	DescType type;
	union{CGenericSlot*slot;CGenericObject*obj;CGenericClass*cls;}gen;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CGenSlotsObjectsClasses)
};
class AFX_EXT_CLASS CGenericObject:public NxpAtom
{
public:
	CGenericObject();virtual~CGenericObject(){}
	CGenericObject(GenericObject*obj);
	CGenericObject(const CGenericObject&c);
	void operator=(const CGenericObject&c);
	void operator=(const CGenericObject*c);
	virtual void printOn(std::ostream&ofStream)const;
	void getFirstPart(std::ostream&os) const;

	_NxAtomTypeEnum atomType;
//MainType=1,InterpretedType=2,PatternType=3,SelfType=4
	string*name;
	union
	{
		CObjectName*objName;
		CInterpretedObject*interpretedObject;
		CObjectPattern*objectPattern;
	}gen;
	int depth;
	GenericType type;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CGenericObject)
};
class AFX_EXT_CLASS CInterpretedObject:public NxpAtom
{
public:
	CInterpretedObject();virtual~CInterpretedObject(){}
	CInterpretedObject(InterpretedObject*obj);
	CInterpretedObject(const CInterpretedObject&c);
	void operator=(const CInterpretedObject&c);
	void operator=(const CInterpretedObject*c);
	_NxAtomTypeEnum atomType;
	string*objName;
	enum InterpretedType type; //
	int depth;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CInterpretedObject)
};
class AFX_EXT_CLASS CObjectPattern:public NxpAtom
{
public:
	CObjectPattern();virtual~CObjectPattern(){}
	CObjectPattern(ObjectPattern*obj);
	CObjectPattern(const CObjectPattern&c);
	void operator=(const CObjectPattern&c);
	void operator=(const CObjectPattern*c);
	_NxAtomTypeEnum atomType;
	CExistObjPattern*exist;
	CUniverseObjectPattern*universe;
//	ObjectName*objName;
	int type; // 0 - <existObjPattern>, 1 - {universeObjectPattern}.
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CObjectPattern)
};
class AFX_EXT_CLASS CExistObjPattern:public NxpAtom
{
public:
	CExistObjPattern();virtual~CExistObjPattern(){}
	CExistObjPattern(ExistObjPattern*c);
	CExistObjPattern(const CExistObjPattern&c);
	void operator=(const CExistObjPattern&c);
	void operator=(const CExistObjPattern*c);
	_NxAtomTypeEnum atomType;
	string*objName;
	int type; // 0 - <existObjPattern>, 1 - {universeObjectPattern}.
	int depth;
	int verType;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CExistObjPattern)
};
class AFX_EXT_CLASS CUniverseObjectPattern:public NxpAtom
{
public:
	CUniverseObjectPattern();virtual~CUniverseObjectPattern(){}
	CUniverseObjectPattern(UniverseObjectPattern*c);
	CUniverseObjectPattern(const CUniverseObjectPattern&c);
	void operator=(const CUniverseObjectPattern&c);
	void operator=(const CUniverseObjectPattern*c);
	_NxAtomTypeEnum atomType;
	string*objName;
	int type; // 0 - <existObjPattern>, 1 - {universeObjectPattern}.
	int depth;
	int verType;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CUniverseObjectPattern)
};
class AFX_EXT_CLASS CGenericClass:public NxpAtom
{
public:
	CGenericClass();virtual~CGenericClass(){}
	CGenericClass(GenericClass*c);
	CGenericClass(const CGenericClass&c);
	void operator=(const CGenericClass&c);
	void operator=(const CGenericClass*c);
	_NxAtomTypeEnum atomType;
	string*name;
	int depth;
	GenericType type;
	virtual void printOn(std::ostream&ofStream)const;
	virtual void getFirstPart(std::ostream&os) const;

	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CGenericClass)
};
class AFX_EXT_CLASS CRetrieveActions:public NxpAtom
{
public:
	CRetrieveActions();virtual~CRetrieveActions(){}
	CRetrieveActions(RetrieveActions*act);
	CRetrieveActions(const CRetrieveActions&c);
	void operator=(const CRetrieveActions&c);
	void operator=(const CRetrieveActions*c);
	_NxAtomTypeEnum atomType;
	string*str;
	CMapDWordToRetrieveKeyWord*keys;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CRetrieveActions)
};
class AFX_EXT_CLASS CWriteActions:public NxpAtom
{
public:
	CWriteActions();virtual~CWriteActions(){}
	CWriteActions(WriteActions*act);
	CWriteActions(const CWriteActions&c);
	void operator=(const CWriteActions&c);
	void operator=(const CWriteActions*c);
	_NxAtomTypeEnum atomType;
	string*str;
	CMapDWordToRetrieveKeyWord*keys;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CWriteActions)
};
class AFX_EXT_CLASS CRetrieveKeyWord:public NxpAtom
{
public:
	CRetrieveKeyWord();virtual~CRetrieveKeyWord(){}
	CRetrieveKeyWord(RetrieveKeyWord*key);
	CRetrieveKeyWord(const CRetrieveKeyWord&c);
	void operator=(const CRetrieveKeyWord&c);
	void operator=(const CRetrieveKeyWord*c);
	_NxAtomTypeEnum atomType;
	union
	{
		string*typeOfDatabase;
		FillType fill;					//fillType';'
		TrueFalseType unknown;	//	trueFalse
		TrueFalseType fwrd;		//	trueFalse
		string*begin;
		string*end;
		string*name;
		CGenericObjectsOrClasses*create;
		CNameList*props;//retrievePropList';'
		CListOfQuotedString*fields;
		string*query;
		CMapDWordToGenericSlot*args;	// listOfGenericSlots';'
		CNameList* mSlots;		//slotLst';'
		CGenSlotsObjectsClasses*atoms;
		string*cursor;//'='slot';'
	}gen;
	TypeOfRetrKeyword type;
	void printOn(std::ostream&os);
	void getFirstPart(std::ostream&os);

	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CRetrieveKeyWord)
};
class AFX_EXT_CLASS CLoadKbActions:public NxpAtom
{
public:
	CLoadKbActions();virtual~CLoadKbActions(){}
	CLoadKbActions(LoadKbActions*c);
	CLoadKbActions(const CLoadKbActions&c);
	void operator=(const CLoadKbActions&c);
	void operator=(const CLoadKbActions*c);
	_NxAtomTypeEnum atomType;
	string*str;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CLoadKbActions)
};
class AFX_EXT_CLASS CUnloadKbActions:public NxpAtom
{
public:
	CUnloadKbActions();virtual~CUnloadKbActions(){}
	CUnloadKbActions(UnloadKbActions*c);
	CUnloadKbActions(const CUnloadKbActions&c);
	void operator=(const CUnloadKbActions&c);
	void operator=(const CUnloadKbActions*c);
	_NxAtomTypeEnum atomType;
	string*str;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CUnloadKbActions)
};
class AFX_EXT_CLASS CAskQuestion:public NxpAtom
{
public:
	CAskQuestion();virtual~CAskQuestion(){}
	CAskQuestion(AskQuestion*ask);
	CAskQuestion(const CAskQuestion&c);
	void operator=(const CAskQuestion&c);
	void operator=(const CAskQuestion*c);
	void printOn(std::ostream& os);


	_NxAtomTypeEnum atomType;
	CGenericSlot*slot;
	KnownValue*value;

	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CAskQuestion)
};
class AFX_EXT_CLASS CDynObjectsAction:public NxpAtom  
{
public:
	CDynObjectsAction();virtual~CDynObjectsAction(){}
	CDynObjectsAction(DynObjectsActions*act);
	CDynObjectsAction(const CDynObjectsAction&c);
	void operator=(const CDynObjectsAction&c);
	void operator=(const CDynObjectsAction*c);
	_NxAtomTypeEnum atomType;
	CGenericObject*genericObject;
	CGenericObjectsOrClasses*genericObjectsOrClasses;
	virtual void printOn(std::ostream&ofStream)const;
//CreateDynamicObject=4,DeleteDynamicObject=5
	enum DynamicType type;
	virtual void Serialize(CArchive&ar);
	char*getTypeString() const;
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CDynObjectsAction)
};
class AFX_EXT_CLASS CGenericObjectsOrClasses:public NxpAtom  
{
public:
	CGenericObjectsOrClasses();virtual~CGenericObjectsOrClasses(){}
	CGenericObjectsOrClasses(GenericObjectsOrClasses*gen);
	CGenericObjectsOrClasses(const CGenericObjectsOrClasses&c);
	void operator=(const CGenericObjectsOrClasses&c);
	void operator=(const CGenericObjectsOrClasses*c);
	virtual void printOn(std::ostream&ofStream)const;
	void getFirstPart(std::ostream&os) const;

	_NxAtomTypeEnum atomType;
	CGenericObject*genericObject;
	CGenericClass*genericClass;
	DescType type;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CGenericObjectsOrClasses)
};
class AFX_EXT_CLASS CInheritanceControl:public NxpAtom  
{
public:
	CInheritanceControl();virtual~CInheritanceControl(){}
	CInheritanceControl(InheritanceControl*c);
	CInheritanceControl(const CInheritanceControl&c);
	void operator=(const CInheritanceControl&c);
	void operator=(const CInheritanceControl*c);
	virtual void printOn(std::ostream&ofStream)const;
	union
	{
		CListOfStrategyKeyWords*listOfStrategyKeyWords;
		KnownValue*knownValue;
		int trFalse;
	}gen;
	enum ControlType type;
	_NxAtomTypeEnum atomType;
	char*getTypeString() const;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CInheritanceControl)
};
class AFX_EXT_CLASS CStrategyKeyWord:public NxpAtom  
{
public:
	CStrategyKeyWord();virtual~CStrategyKeyWord(){}
	CStrategyKeyWord(ListOfStrategyKeyWords*keys);
	CStrategyKeyWord(const CStrategyKeyWord&c);
	void operator=(const CStrategyKeyWord&c);
	void operator=(const CStrategyKeyWord*c);
	_NxAtomTypeEnum atomType;
	union
	{
		int	inhValUp;int inhValDown;int inhObjUp;int inhObjDown;int inhClassUp;
		int inhClassDown;int inhBreadth;int inhParent;int inhPwTrue;int inhPwFalse;
		int pwNotKnown;int exhBwrd;int ptGates;int pfActions;int sourceOn;int cActionsOn;
	}gen;
	StrategyKeyWords type;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CStrategyKeyWord)
};
class AFX_EXT_CLASS CLhsTest:public NxpAtom  
{
public:
	CLhsTest();virtual~CLhsTest(){}
	CLhsTest(LhsTest*bb);
	CLhsTest(const CLhsTest&c);
	void operator=(const CLhsTest&c);
	void operator=(const CLhsTest*c);
	_NxAtomTypeEnum atomType;
	virtual void Serialize(CArchive&ar);
	virtual void printOn(std::ostream&ofStream)const;
	char*getTypeString() const;
//	CString*getFirstPart() const;
//	char*getFirstPart() const;
//	char* getFirstPart(CString *) const;
//	char* getFirstPart(char*) const;
	char* getFirstPart(std::stringstream&os);// ostream&os);
	void getSecondPart(std::stringstream&os);

	//BoolTestType=1,CompareType=2,EqualListType=3,StatusType=4,EqualSlotsType=5,MemberType=6
	union
	{
		CComparisons*comparisons;
		CMemberTest*memberTest;
		CBooleanTest*booleanTest;
		CEqualityListConstants*equalityListConstants;
	}gen;
	LhsTestType type;
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CLhsTest)
};
class AFX_EXT_CLASS CComparisons:public NxpAtom
{
public:
	CComparisons();virtual~CComparisons(){}
	CComparisons(Comparisons*cc);
	CComparisons(const CComparisons&c);
	void operator=(const CComparisons&c);
	void operator=(const CComparisons*c);
	_NxAtomTypeEnum atomType;
	CExpression*expression;
	comparisonType type;//lessThen=1,moreThen=2,lessOrEqual=3,moreOrEqual=4
	KnownValue*value;
	char* getTypeString();
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CComparisons)
};
class AFX_EXT_CLASS CMemberTest:public NxpAtom
{
public:
	CMemberTest();virtual~CMemberTest(){}
	CMemberTest(MemberTest*test);
	CMemberTest(const MemberTest&c);
//	CMemberTest(const CMemberTest&c);

	void operator=(const MemberTest&c);
	//	void operator=(const CMemberTest&c);
	void operator=(const MemberTest*c);
//	void operator=(CMemberTest*c);
	_NxAtomTypeEnum atomType;
	CGenericObject*obj;
	CGenericClass*cls;
	CListOfGenericObjectsOrClasseses*list;
	enum MemberType type;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CMemberTest)
};
class AFX_EXT_CLASS CBooleanTest:public NxpAtom
{
public:
	CBooleanTest();virtual~CBooleanTest(){}
	CBooleanTest(BooleanTest*test);
	CBooleanTest(const CBooleanTest&c);
	void operator=(const CBooleanTest&c);
	void operator=(const CBooleanTest*c);
	_NxAtomTypeEnum atomType;
	CGenericSlot*slot;
	BooleanTestType type;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CBooleanTest)
};
class AFX_EXT_CLASS CEqualityListConstants:public NxpAtom
{
public:
	CEqualityListConstants();virtual~CEqualityListConstants(){}
	CEqualityListConstants(EqualityListConstants*eq);
	CEqualityListConstants(const CEqualityListConstants&c);
	void operator=(const CEqualityListConstants&c);
	void operator=(const CEqualityListConstants*c);
	virtual void printOn(std::ostream&ofStream)const;
	_NxAtomTypeEnum atomType;
	CExpression*expression;
	KnownValue*constanta;
	CConstants*constants;
	EqualityListType type;//equalType=1,listEqualType=2;
	void getFirstPart(std::ostream &os) const;
	void GetSecondPart(std::ostream&os) const;


	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CEqualityListConstants)
};

class AFX_EXT_CLASS CNameAssignment:public NxpAtom  
{
public:
	CNameAssignment();virtual~CNameAssignment(){}
	CNameAssignment(NameAssignment*ass);
	CNameAssignment(const CNameAssignment&c);
	void operator=(const CNameAssignment&c);
	void operator=(const CNameAssignment*c);
	CListOfExpression*expressions;
	CGenericSlot*genericSlot;
	_NxAtomTypeEnum atomType;
//	void printOn(std::ostream&os);
	virtual void printOn(std::ostream&ofStream)const;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	void setExpression(Expression*exp);
	DECLARE_SERIAL(CNameAssignment)
};
class AFX_EXT_CLASS CResetAssignment:public NxpAtom  
{
public:
	CResetAssignment();virtual~CResetAssignment(){}
	CResetAssignment(ResetAssignment*ass);
	CResetAssignment(const CResetAssignment&c);
	void operator=(const CResetAssignment&c);
	void operator=(const CResetAssignment*c);
	virtual void printOn(std::ostream&ofStream)const;
	void getFirstPart(std::ostream&os) const;
	void getSecondPart(std::ostream&os) const;

	_NxAtomTypeEnum atomType;
	CGenericSlot*genericSlot;

	void SetAssignment(ResetAssignment*resetAssignment);
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CResetAssignment)
};
class AFX_EXT_CLASS CAssignAssignment:public NxpAtom  
{
public:
	CAssignAssignment();virtual~CAssignAssignment(){}
	CAssignAssignment(AssignAssignment*ass);
	CAssignAssignment(const CAssignAssignment&c);
	void operator=(const CAssignAssignment&c);
	void operator=(const CAssignAssignment*c);
	_NxAtomTypeEnum atomType;
//	CExpression*expression;
	CListOfExpression*expressions;
//	void SetExpressions(CAssignAssignment*assignAssignment);
	void SetAssignment(AssignAssignment*assignAssignment);
	virtual void printOn(std::ostream&ofStream)const;
	void getFirstPart(std::ostream&os) const;
	void getSecondPart(std::ostream&os) const;

	CString*expr;
	CGenericSlot*slot;
	KnownValue*val;
	int type;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	void setExpression(Expression*exp);
	DECLARE_SERIAL(CAssignAssignment)
};

class AFX_EXT_CLASS CExpression:public NxpAtom  
{
public:
	CExpression();virtual~CExpression(){}
	CExpression(Expression*exp);
	CExpression(const CExpression&c);
	void operator=(const CExpression&c);
	void operator=(const CExpression*c);
	virtual void printOn(std::ostream&ofStream)const;
//	CString*getFirstPart() const;
	char*getFirstPart(std::ostream&os) const;
//	void getFirstPart(CString *) const;

	union
	{
		string*expr;
		CGenericSlot*slot;
		KnownValue*constanta;
	}gen;
	int type; // 0 - expr 1 - slot 2 - constanta
//	Expression*next;
	ArithSign sign; //exprPlus=1,exprMinus=2,exprMult=3,exprDiv=4
//	void setExpression(char*expr);
//	void setGenericSlot(GenericSlot*slot);
//	void setKnownValue(yKnownValue*value);
	_NxAtomTypeEnum atomType;
//	CObject*
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CExpression)
};
class AFX_EXT_CLASS CGenericSlot:public NxpAtom  
{
public:
	CGenericSlot();virtual~CGenericSlot(){}
	CGenericSlot(GenericSlot*ss);
	CGenericSlot(const CGenericSlot&c);
	void operator=(const CGenericSlot&c);
	void operator=(const CGenericSlot*c);
	string*name;
	int depth;
	void printOn(std::ostream&os);
	void getFirstPart(std::ostream&os);
	union
	{
		CInterpretedSlot*interpretedSlot;
		CSlotPattern*slotPattern;
	}gen;
	void SetAssignment(GenericSlot*slot);
	GenericType type;
	_NxAtomTypeEnum atomType;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CGenericSlot)
};
class AFX_EXT_CLASS CInterpretedSlot:public NxpAtom  
{
public:
	CInterpretedSlot();virtual~CInterpretedSlot(){}
	CInterpretedSlot(InterpretedSlot*slot);
	CInterpretedSlot(const CInterpretedSlot&c);
	void operator=(const CInterpretedSlot&c);
	void operator=(const CInterpretedSlot*c);
	_NxAtomTypeEnum atomType;
	string*root;
	string*slot;
	int lSlashDepth;
	int apostr;
	enum InterpretedType type;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CInterpretedSlot)
};
class AFX_EXT_CLASS CSlotPattern:public NxpAtom
{
public:
	CSlotPattern();virtual~CSlotPattern(){}
	CSlotPattern(SlotPattern*slot);
	CSlotPattern(const CSlotPattern&c);
	void operator=(const CSlotPattern&c);
	void operator=(const CSlotPattern*c);
	_NxAtomTypeEnum atomType;
	string*name;
	string*property;
	int existDepth;
	int universeDepth;
	void printOn(std::ostream&os);
	void getFirstPart(std::ostream&os);

	union{CClassName*cls;CObjectName*obj;}gen;
	enum SlotPatternType type;
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CSlotPattern)
};
class AFX_EXT_CLASS CClassName:public NxpAtom
{
public:
	CClassName();virtual~CClassName(){}
	CClassName(yClassName*cls);
	CClassName(const CClassName&c);
	void operator=(const CClassName&c);
	void operator=(const CClassName*c);
	_NxAtomTypeEnum atomType;
	string*name;
	int type;/*0 - class, 1 - |class|.*/
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CClassName)
};
class AFX_EXT_CLASS CObjectName:public NxpAtom
{
public:
	CObjectName();virtual~CObjectName(){}
	CObjectName(yObjectName*obj);
	CObjectName(const CObjectName&c);
	void operator=(const CObjectName&c);
	void operator=(const CObjectName*c);
	_NxAtomTypeEnum atomType;
	string*name;
	int type; // 0 - object, 1 - |object|.
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(CObjectName)
};

#endif // !defined(AFX_CONDITION_H__C504D8A5_D6CB_11D2_862A_00A024384FCF__INCLUDED_)
