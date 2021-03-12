// ClassItem.h: interface for the ClassItem class.
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_CLASSITEM_H__C504D8A2_D6CB_11D2_862A_00A024384FCF__INCLUDED_)
#define AFX_CLASSITEM_H__C504D8A2_D6CB_11D2_862A_00A024384FCF__INCLUDED_
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
typedef CTypedPtrMap<CMapStringToOb,CString,ClassItem*>CMapStringToClassItem;
class AFX_EXT_CLASS ClassItem:public NxpAtom  
{
public:
	ClassItem();
	virtual~ClassItem()
	{
//		if(subclasses)delete subclasses;
//		if(properties)delete properties;
//		if(className) delete className;
//     if(slot) delete slot;
	}
	ClassItem(const ClassItem&cl)
	{
		subclasses=cl.subclasses;
		properties=cl.properties;
		privateProperties = cl.privateProperties;
		className =new string((LPSTR)cl.className->c_str());
		slot=cl.slot;
		subclassesItems=cl.subclassesItems;
		type=cl.type; // class.
	}
	ClassItemList*subclasses;   // @SUBCLASSES
	CMapStringToClassItem*subclassesItems;
//  SubclassesList *superclasses; // @SUBCLASSES
//  SubobjectsList *subobjects; // @SUBOBJECTS
	PropertiesList*properties; // @PROPERTIES
	PrivatePropertiesList* privateProperties; // @PROPERTIES

	SlotItem*slot;  // @INFATOM
	string*className;
	int type;//0 - class, 1 - |class|.
	virtual char*nameOf()const{return"ClassItem";}
	BOOL operator==(const ClassItem&c)const{return this==&c;}
	void operator=(const ClassItem&cl)//assignment operator
	{
		className=cl.className;subclasses=cl.subclasses;properties=cl.properties;
		privateProperties = cl.privateProperties;

		subclassesItems=cl.subclassesItems;
		type=cl.type;
	}
	void operator=(const ClassItem*cl)  //assignment operator
	{
		className=cl->className;subclasses=cl->subclasses;properties=cl->properties;
		privateProperties = cl->privateProperties;

		subclassesItems=cl->subclassesItems;type=cl->type;
	}
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
	DECLARE_SERIAL(ClassItem)
};
#endif // !defined(AFX_CLASSITEM_H__C504D8A2_D6CB_11D2_862A_00A024384FCF__INCLUDED_)
