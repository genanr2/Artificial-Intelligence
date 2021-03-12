// RuleItem.h: interface for the RuleItem class.
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_RULEITEM_H__C504D8A8_D6CB_11D2_862A_00A024384FCF__INCLUDED_)
#define AFX_RULEITEM_H__C504D8A8_D6CB_11D2_862A_00A024384FCF__INCLUDED_
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
typedef CTypedPtrMap<CMapStringToOb,CString,RuleItem*>CMapStringToRuleItem;
class AFX_EXT_CLASS RuleItem:public NxpAtom  
{
public:
	RuleItem();
	virtual~RuleItem(); //{delete conditions;delete actions;}//delete hypo; }
	BOOL operator==(const RuleItem&r)const{return this==&r;}
	bool operator==(const RuleItem*r)const{return this==r;}

	virtual char*nameOf()const{return"RuleItem";}
	virtual int isEqual(RuleItem&t)const{return this==&t;}
	virtual void printOn(std::ostream&ofStream)const;
//	virtual void printOn(stringstream&ofStream)const;
	virtual void read(std::istream &is);//ru
	void setConditions(yListOfConditions*cond);
	void setActions(yListOfRhsActions*actLis);
	void setActions2(ListOfActions*actLis);
	
	void printRule(std::ostream&os) const;

	RuleItem(const RuleItem&cl)
	{
		infCat=cl.infCat;
		conditions=cl.conditions;
		actions=cl.actions;
		why=new string(*cl.why);
		comments=new string(*cl.comments);
		ruleName=new string(*cl.ruleName);
		hypo=cl.hypo;
		MapOfActions=cl.MapOfActions;
		MapOfActions2 = cl.MapOfActions2;
		MapOfConditions=cl.MapOfConditions;
	}
	void operator=(const RuleItem&cl)  //assignment operator
	{
		infCat=cl.infCat;
		conditions=cl.conditions;
		actions=cl.actions;
		why=new string(*cl.why);
		comments=new string(*cl.comments);
		ruleName=new string(*cl.ruleName);
		hypo=cl.hypo;
		MapOfActions=cl.MapOfActions;
		MapOfActions2 = cl.MapOfActions2;
		MapOfConditions=cl.MapOfConditions;
	}
	void operator=(const RuleItem*r)
	{
	 infCat=r->infCat;
	 conditions=r->conditions;
	 actions=r->actions;
	 why=new string(*r->why);
	 comments=new string(*r->comments);
	 ruleName=new string(*r->ruleName);
	 hypo=r->hypo;
	 MapOfActions=r->MapOfActions;
	 MapOfActions2 = r->MapOfActions2;
	 MapOfConditions=r->MapOfConditions;
	}
	int infCat;        // @INFCAT
	SlotItem*infAtom;  // @INFATOM
	string*comments;// @COMMENTS
	string*why;     // @WHY
	string*ruleName;
	SlotItem*hypo;     // @HYPO
	ListOfConditions*conditions; // @LHS
	ListOfRhsActions*actions;    // @RHS

	CMapDWordToCondition*MapOfConditions;
	CMapDWordToAction*MapOfActions;
	CMapDWordToAction*MapOfActions2;
	//	CMapDWordToAction
	ActionsList*actList;
	__declspec(dllexport)friend std::ostream&operator<<(std::ostream&,const RuleItem*);
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(RuleItem)
};
#endif // !defined(AFX_RULEITEM_H__C504D8A8_D6CB_11D2_862A_00A024384FCF__INCLUDED_)
