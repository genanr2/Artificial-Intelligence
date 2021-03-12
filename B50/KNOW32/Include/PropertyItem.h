// PropertyItem.h: interface for the PropertyItem class.
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_PROPERTYITEM_H__E0112944_D604_11D2_862A_00A024384FCF__INCLUDED_)
#define AFX_PROPERTYITEM_H__E0112944_D604_11D2_862A_00A024384FCF__INCLUDED_
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
class AFX_EXT_CLASS PropertyItem;	
class AFX_EXT_CLASS PropertyItem:public NxpAtom  
{
public:
	PropertyItem()
	{
		propertyName=new string("");
		identifier=new string(""); //@PROPERTY
		format=new string(""); // @FORMAT
		atomType=StringType;
	}
	virtual~PropertyItem();
	PropertyItem(const PropertyItem&prop)
	{
		propertyName=prop.propertyName;identifier=prop.identifier;
		format=prop.format;atomType=prop.atomType;
	}
	string*propertyName;
	string*identifier; // @PROPERTY
	string*format;     // @FORMAT
	AtomValueType atomType;// @TYPE
	virtual char*nameOf()const{return"PropertyItem";}
	bool operator==(const PropertyItem&c)const{return this==&c;}
	bool operator==(const PropertyItem*c)const{return this==c;}
	void operator=(const PropertyItem&c)
	{
		propertyName=c.propertyName;identifier=c.identifier;
		format=c.format;atomType=c.atomType;
	}
	void operator=(const PropertyItem*c)
	{
		propertyName=c->propertyName;identifier=c->identifier;
		format=c->format;atomType=c->atomType;
	}
	virtual void printOn(std::ostream&) const;
	virtual void printOn(std::ostream&,int) const;
	virtual void read(std::istream&);
	LPSTR getAtomTypeString(AtomValueType type);
	__declspec(dllexport)friend std::ostream operator<<(std::ostream&,const PropertyItem*);
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(PropertyItem)
};
#endif // !defined(AFX_PROPERTYITEM_H__E0112944_D604_11D2_862A_00A024384FCF__INCLUDED_)
