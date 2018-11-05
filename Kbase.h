#if !defined( __KBASE_H )
#define __KBASE_H
#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif
#ifndef _STRING_
#include "string"
#endif
#include "..\KbParse\calc.h"
#include ".\include\NxpAtom.h"
#include ".\include\PropertyItem.h"
#include ".\include\ClassItem.h"
#include ".\include\ObjectItem.h"
#include ".\include\Condition.h"
#include ".\include\Action.h"
#include ".\include\SlotItem.h"
#include ".\include\GlobalsItem.h"
#include ".\include\RuleItem.h"
using namespace std;
// /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"Debug/b50.pch" /Yu"stdafx.h" /Fo"Debug/" /Fd"Debug/" /FD /c 
class AFX_EXT_CLASS KnownValue:public CObject
{
	public:
		KnownValue() 
		{ 
			type = (KnownValueType)0;
			value = NULL;
		}
		KnownValue(string&val)
		{
			value=new string(val);
			type = stringType;
		}
		KnownValue(char*val)
		{
			if(val)value=new string(val);
			else value=new string;
			type = stringType;
		}
		KnownValue(yKnownValue*val)
		{
//			knownValue.str=new string(val->value.str);
			type=val->type;

			switch((int)val->type)
			{
				case digit:	knownValue.digit=val->value.digit;break;
				case stringType:knownValue.str=new string(val->value.str);break;
				case known1:knownValue.known=val->value.known1;break;
				case unknown:knownValue.unknown=val->value.unknown1;break;
				case notknown:knownValue.notknown=val->value.notknown1;break;
				case trueFalse1:knownValue.trueFalse=val->value.trueFalse1;break;
				default:break;
			}
		}
		void printOn(std::ostream&os);
		KnownValue(const KnownValue&c);
		void operator=(const KnownValue&c);
		void operator=(const KnownValue*c);
		void SetValue(string&str)
		{
			value=new string(str);
			type = stringType;
		}
		~KnownValue(void){delete value;}
		KnownValueType type;
		union 
		{
			double digit;
			string*str;
			int known;
			int unknown;
			int notknown;
			int trueFalse;
		}knownValue;
		string*value;
//    int privateValue;
};
typedef CTypedPtrArray<CObArray,CCondition*>ConditionsList;//ListOfConditions
//typedef CTypedPtrArray<CObArray,Action*>ActionsList; // ListOfRhsActions
typedef CTypedPtrArray<CObArray,CStringList*>TArrList;
class AFX_EXT_CLASS ListOfSources:public TArrList//, public TStreamableBase
{
	public:
	  ListOfSources(int u,int l=0,int aD=0):TArrList(){SetSize(u,aD);}
//	  ListOfSources():TArrList(){SetSize(0,1);}
	 ~ListOfSources(){}
	private:
//	 const char *streamableName() const { return "ListOfSources"; }
		__declspec(dllexport)friend std::ostream&operator<<(std::ostream&,const ListOfSources*);
};
class AFX_EXT_CLASS IfChangeActions:public TArrList
{
	public:
		IfChangeActions(int u,int l=0,int aD=0):TArrList(){SetSize(u,aD);}
		~IfChangeActions(){}
	private:
//		const char * streamableName() const { return "IfChangeActions"; }
		__declspec(dllexport)friend std::ostream&operator<<(std::ostream&,const IfChangeActions*);
};
class AFX_EXT_CLASS VersionIdentification
{
	public:
		char*versNumber; // @VERSION
		VersionIdentification(){versNumber="011";}
};
class AFX_EXT_CLASS Comments
{
public:
	Comments(){type=comments;comment=new string;}
	~Comments(){delete comment;}
	KnowledgeBaseUnitType type;
	string*comment;  // @COMMENTS
};
typedef CArray<SlotItem,SlotItem&>TSlotList;
class AFX_EXT_CLASS SlotItemList:public TSlotList 
{
	public:
	 SlotItemList(int u,int l=0,int d=0):TSlotList(){SetSize(u,d);}
	 SlotItemList():TSlotList(){SetSize(0,1);}
	 ~SlotItemList(){}
	private:
//	 const char * streamableName() const { return "SlotItemList"; }
	__declspec(dllexport)friend std::ostream&operator<<(std::ostream&,const SlotItemList*);
	__declspec(dllexport)friend std::ostream&operator<<(std::ostream&,const SlotItemList&);
};
typedef CArray<PropertyItem,PropertyItem&>TPropList;
typedef CTypedPtrMap<CMapStringToOb,CString,PropertyItem*>CMapStringToPropertyItem;
class AFX_EXT_CLASS PropertiesList:public TPropList //, TStreamableBase
{
	public:
		PropertiesList(int u,int l=0,int d=0):TPropList(){SetSize(u,d);}
		~PropertiesList(){}
//		BOOL operator == ( const PropertiesList& c ) const //comparison operator
//			{ return this == &c; }
		virtual void*read(std::istream&);
	private:
//		const char *streamableName() const { return "PropertiesList"; }
		__declspec(dllexport)friend std::ostream&operator<<(std::ostream far&,const PropertiesList far*);
};
/////////////////////////////////////////////////////////////////////////////////////////////
typedef CArray<ClassItem,ClassItem&>TClassItList;
class AFX_EXT_CLASS ClassItemList:public TClassItList
{
	public:ClassItemList(int u,int l=0,int d=0):TClassItList(){SetSize(u,d);}
		~ClassItemList(){}
	private:
//		const char *streamableName() const { return "ClassItemList"; }
		__declspec(dllexport) friend std::ostream&operator<<(std::ostream&,const ClassItemList*);
//		friend std::ostream&operator<<(std::ostream&,const ClassItemList*);
};
typedef CArray<ObjectItem,ObjectItem&>TObjList;
class AFX_EXT_CLASS ObjectItemList:public TObjList //, public TStreamableBase
{
	public:
	 ObjectItemList(int u,int l=0,int d=0):TObjList(){SetSize(u,d);}
	 ~ObjectItemList(){}
	private:
//	const char *streamableName() const { return "ObjectItemList"; }
		__declspec(dllexport)friend std::ostream&operator<<(std::ostream&,const ObjectItemList*);
};
//////////////////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS ListOfConditions:public TArrList//, public TStreamableBase
{
public:
	ListOfConditions(int u,int l=0,int d=0):TArrList(){SetSize(u,d);}
	~ListOfConditions(){}
//private:
//	const char * streamableName() const { return "ListOfConditions"; }
//	friend ostream& operator << (ostream&,const ListOfConditions*);
};
class AFX_EXT_CLASS ListOfRhsActions:public TArrList//, public TStreamableBase
{
public:
	ListOfRhsActions(int u,int l=0,int d=0):TArrList(){SetSize(u,d);}
	~ListOfRhsActions(){}
//private:
//	const char * streamableName() const { return "ListOfRhsActions"; }
// friend ostream& operator << (ostream&,const ListOfRhsActions*);
};
//typedef CArray<RuleItem,RuleItem*>TRuleList;
typedef CArray<RuleItem,RuleItem&>TRuleList;
class AFX_EXT_CLASS RuleItemList:public TRuleList//, public TStreamableBase
{
	public:
	 RuleItemList(int u,int l=0,int d=0):TRuleList(){SetSize(u,d);}
	 ~RuleItemList(){}
	 BOOL operator==(const RuleItemList&c)const{return this==&c;}
	 BOOL operator==(const RuleItemList*c)const{return this==c;}
	private:
//	 const char * streamableName() const { return "RuleItemList"; }
	 __declspec(dllexport)friend std::ostream&operator<<(std::ostream&,const RuleItemList*);
};
class AFX_EXT_CLASS KnowledgeBase:public CObject
{
public:
	KnowledgeBase()
	{
		fileName=NULL;
		props = NULL;
		classes = NULL;
		objects = NULL;
		slots = NULL;
		rules = NULL;
	}
	~KnowledgeBase(){}
	VersionIdentification versionIdentification;
	string*fileName;
	void*read(std::istream&is);
	void*read(FILE*is);
	void*read(LPCTSTR is);
	PropertyDescription*props;
	ClassDescription*classes;
	ObjectDescription*objects;
	SlotDescription*slots;
	RuleDescription*rules;
	GlobalsItem*globals;
	void traceProperties();
	void traceClasses();
	void traceObjects();
	void traceSlots();
	void traceRules();
};
class AFX_EXT_CLASS Descriptions:public CObject//public KnowledgeBase
{
public:
	Descriptions(){}~Descriptions(){}
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(Descriptions)
};
class AFX_EXT_CLASS RuleDescription:public Descriptions//public KnowledgeBase
{
public:
	RuleDescription()
	{
		type=rule_description;identifier=new string;ruleItems=new RuleItemList(0,0,1);
		rules=new CMapStringToRuleItem;
	}
	~RuleDescription(){delete ruleItems;}
	KnowledgeBaseUnitType type;
	string*identifier;       // @RULE
	RuleItemList*ruleItems;
	CMapStringToRuleItem*rules;
	virtual char*nameOf()const{return"RuleDescription";}
	virtual void printOn(std::ostream&)const{}
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(RuleDescription)
};
//////////////////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS ObjectDescription:public Descriptions
{
public:
	ObjectDescription()
	{
		type=object_description;identifier=new string();
		objectItemList=new ObjectItemList(0,0,1);
		objectItems=new CMapStringToObjectItem;
	}
	~ObjectDescription(){delete objectItemList;}
	CMapStringToObjectItem*objectItems;
	KnowledgeBaseUnitType type;
	string*identifier; // @OBJECT
	ObjectItemList*objectItemList;
	virtual char*nameOf()const{return"ObjectDescription";}
	virtual void printOn(std::ostream&)const{}
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(ObjectDescription)
};
/////////////////////////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS ClassDescription:public Descriptions
{
public:
	ClassDescription()
	{
		type=class_description;
		identifier=new string();
		items=new ClassItemList(0,0,1);
		classes=new CMapStringToClassItem;
	}
	~ClassDescription(){delete items;}
	string*identifier; //@CLASS
	ClassItemList*items;
	CMapStringToClassItem*classes;
	KnowledgeBaseUnitType type;
	virtual char*nameOf()const{return"ClassDescription";}
	virtual void printOn(std::ostream&)const{}
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(ClassDescription)
};
class AFX_EXT_CLASS PropertyDescription:public Descriptions
{
public:
	PropertyDescription()
	{
		type=property_description;
		items=new PropertiesList(0,0,1);
		propItems=new CMapStringToPropertyItem;
	}
	~PropertyDescription(){delete items;}
	KnowledgeBaseUnitType type;
	PropertiesList*items;
	CMapStringToPropertyItem*propItems;
	virtual char*nameOf()const{return"PropertyDescription";}
	virtual void printOn(std::ostream&)const{}
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(PropertyDescription)
};
class AFX_EXT_CLASS SlotDescription:public Descriptions
{
public:
	SlotDescription()
	{
	 type=slot_description;identifier=new string();
	 slots=new CMapStringToSlotItem;
//	 slotItems=new SlotItemList(0,0,1);
	}
	~SlotDescription(){/*delete slotItems;*/}
	KnowledgeBaseUnitType type;
	string*identifier;       // @SLOT
	SlotItemList slotItems;//(0,0,1);
	CMapStringToSlotItem*slots;
	virtual char*nameOf()const{return"SlotDescription";}
	virtual void printOn(std::ostream&)const{}
	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
//	virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(SlotDescription)
};
/////////////////////////////////////////////////////////////////////////////////////////////////
template<class T,int i>class TestClass 
{
	public:char buffer[i];
   T testFunc(T*p1);
};
template <class T,int i>
T TestClass<T,i>::testFunc(T*p1){return*(p1++)};
#endif
