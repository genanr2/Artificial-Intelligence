// ObjectItem.h: interface for the ObjectItem class.
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_OBJECTITEM_H__C504D8A3_D6CB_11D2_862A_00A024384FCF__INCLUDED_)
#define AFX_OBJECTITEM_H__C504D8A3_D6CB_11D2_862A_00A024384FCF__INCLUDED_
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
typedef CTypedPtrMap<CMapStringToOb,CString,ObjectItem*>CMapStringToObjectItem;


class AFX_EXT_CLASS ObjectItem:public NxpAtom  
{
public:
	ObjectItem();
	virtual~ObjectItem()
	{
//	 if(classes)    delete classes;  //
//	 if(properties) delete properties;  //
//	 if(subobjects) delete subobjects;
	}
	ObjectItem(const ObjectItem&cl)
	{
		classes=cl.classes;  //
		properties=cl.properties;  //
		privateProperties = cl.privateProperties;  //

		subobjects=cl.subobjects;  // @SUBOBJECTS
		objectName=new string((LPSTR)cl.objectName->c_str());
		slot=cl.slot;
		subobjectsItems=cl.subobjectsItems;
		classesItems=cl.classesItems;
		type=cl.type;
	}
	ObjectItemList*subobjects; // @SUBOBJECTS
	CMapStringToObjectItem*subobjectsItems;
	CMapStringToClassItem*classesItems;
//	TObjList *subobjects;
	PropertiesList*properties; // @PROPERTIES
	PrivatePropertiesList* privateProperties; // @PROPERTIES
	
	ClassItemList*classes;    // @CLASSES
	SlotItem*slot;  // @INFATOM
	string*objectName;
	int type;
	BOOL operator==(const ObjectItem&o)const{return this==&o;}
	bool operator==(const ObjectItem*o)const{return this==o;}
	void operator=(const ObjectItem&o)//assignment operator
	{
		classes=o.classes;  //
		properties=o.properties;  //
		privateProperties = o.privateProperties;  //

		subobjects=o.subobjects;  // @SUBOBJECTS
		objectName=o.objectName;
		slot=o.slot;
		subobjectsItems=o.subobjectsItems;
		classesItems=o.classesItems;
		type=o.type;
	}
	void operator=(const ObjectItem*o)  //assignment operator
	{
		classes=o->classes;  //
		properties=o->properties;  //
		privateProperties = o->privateProperties;  //

		subobjects=o->subobjects;  // @SUBOBJECTS
		objectName=o->objectName;
		slot=o->slot;
		subobjectsItems=o->subobjectsItems;
		classesItems=o->classesItems;type=o->type;
	}
	virtual char*nameOf()const{return"ObjectItem";}
	virtual void printOn(std::ostream&)const;
	virtual void read(std::istream&is);
	__declspec(dllexport)friend std::ostream&operator<<(std::ostream&,const ObjectItem*);
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(ObjectItem)
};
#endif // !defined(AFX_OBJECTITEM_H__C504D8A3_D6CB_11D2_862A_00A024384FCF__INCLUDED_)
