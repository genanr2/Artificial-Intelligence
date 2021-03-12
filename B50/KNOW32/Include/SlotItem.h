// SlotItem.h: interface for the SlotItem class.
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_SLOTITEM_H__C504D8A4_D6CB_11D2_862A_00A024384FCF__INCLUDED_)
#define AFX_SLOTITEM_H__C504D8A4_D6CB_11D2_862A_00A024384FCF__INCLUDED_
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
typedef CTypedPtrMap<CMapStringToOb,CString,SlotItem*>CMapStringToSlotItem;
class AFX_EXT_CLASS SlotItem:public NxpAtom  
{
public:
	SlotItem();
	virtual~SlotItem()
	{
//	 delete sources;delete privSources;delete cActions;
//	 delete privCActions;delete contexts;
//	 if(infAtom!=NULL)delete infAtom;
//	 if(inhAtom!=NULL)delete inhAtom;
//	 delete initVal;
//	 delete privInitVal;
	}
	SlotItem(const SlotItem&cl);
	void operator=(const SlotItem&cl);  //assignment operator
	void operator=(const SlotItem*cl);  //assignment operator
	int infCat;         // @INFCAT
	int inhCat;         // @INHCAT
//  int inhSlotDefault;     // @INHDEFAULT
//  int inhValDefault;     // @INHDEFAULT
	int hasMeta;        //
	SlotItem*infAtom;  // slot, @INFATOM
	SlotItem*inhAtom;  // slot, @INHATOM
	string* help22;      // @WHY
	string *why;      // @WHY
	string*prompt;   // @PROMPT
	string*comments; // @COMMENTS
	string*format;   // @FORMAT
	string*slotName; // @FORMAT
	string*help; // @FORMAT
	string* questWin; // @FORMAT
	
	
	PropertyItem*property;

	union { ClassItem*cls; ObjectItem*obj; }gen;
	enum SlotPatternType type;
	/*
	struct KBPARSE_API SlotPattern
	{
		char*name;
		char*property;
		int existDepth;
		int universeDepth;
		union { yClassName*cls; yObjectName*obj; }gen;
		enum SlotPatternType type;
	};
	*/
	
	int inhSlotUp;      // true_or_false, @INHSLOTUP
	int inhSlotDown;    // true_or_false, @INHSLOTDOWN
	int inhValUp;       // true_or_false, @INHVALUP
	int inhValDown;     // true_or_false, @INHVALDOWN
//  int inhDefFirst;     // true_or_false, @INHVALDOWN
	int inhBreadth;     // true_or_false, @INHBREADTH
	int inhParent;      // true_or_false, @INHPARENT
//  SlotList cnt;
	SlotItemList*contexts;       // @CONTEXTS
	KnownValue*initVal;           // @INITVAL
	KnownValue*privInitVal;       // @PRIVINITVAL

	ListOfSources*sources;       // @SOURCES
	ListOfSources*privSources;   // @PRIVSOURCES
	IfChangeActions*cActions;    // @CACTIONS
	IfChangeActions*privCActions;// @PRIVCACTIONS

	CMapDWordToCondition*MapOfSources;
	CMapDWordToCondition*MapOfPrivSources;
	CMapDWordToAction*MapCActions;
	CMapDWordToAction*MapPrivCActions;
	virtual char*nameOf()const{return"SlotItem";}
	BOOL operator==(const SlotItem&c)const{return this==&c;}
	virtual void printOn(std::ostream&os)const;
	virtual void read(std::istream&is);
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(SlotItem)
};
#endif // !defined(AFX_SLOTITEM_H__C504D8A4_D6CB_11D2_862A_00A024384FCF__INCLUDED_)
