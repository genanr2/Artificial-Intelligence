// GlobalsItem.h: interface for the GlobalsItem class.
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_GLOBALSITEM_H__C504D8A7_D6CB_11D2_862A_00A024384FCF__INCLUDED_)
#define AFX_GLOBALSITEM_H__C504D8A7_D6CB_11D2_862A_00A024384FCF__INCLUDED_
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
class AFX_EXT_CLASS GlobalsItem:public NxpAtom  
{
public:
	GlobalsItem();
	virtual~GlobalsItem();//{delete sugList;delete volList;}
	int PwTrue,PwFalse,PwNotknown,ExhBwrd,PfActions,PtGates;
	int SourceOn,CActionOn,InhClassUp,InhClassDown,InhObjUp,InhObjDown;
	int InhValUp,InhValDown,ParentFirst,BreadthFirst;
	SlotItemList*sugList;
	SlotItemList*volList;
	CMapStringToSlotItem*sugItems;
	CMapStringToSlotItem*volItems;
	virtual char*nameOf()const{return"GlobalsItem";}
	virtual void printOn(std::ostream&ofStream) const;
	virtual void read(std::istream&is);
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(GlobalsItem)
};
inline std::istream&operator>>(std::istream&is,GlobalsItem*gi){gi->read(is);return is;}
inline std::istream&operator>>(std::istream&is,GlobalsItem&gi){gi.read(is);return is;}
#endif // !defined(AFX_GLOBALSITEM_H__C504D8A7_D6CB_11D2_862A_00A024384FCF__INCLUDED_)
