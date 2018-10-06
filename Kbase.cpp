#include "..\b54mdll\stdafx.h"
#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include "afxcoll.h"
#include <stdio.h>
#ifndef _INC_CTYPE
#include <CTYPE.H>
#endif
#include <iostream>
#ifndef _INC_STRSTREAM
//#include <StrStrea.h>
#include <StrStream>
#endif
#include <sStream>
//#include <iostream.h>
//#include <fstream.h>
//#include <iostream.h>
#ifndef _STRING_
#include <string>
#endif
#if !defined( __ClassList_H )
#include "Lists.h"
#endif // __ClassList_H
#include <limits>
using namespace std;
#if !defined( __KBASE_H )
#include "KBase.h"
#endif
//#include "..\KbParse\kbParse.h"
//extern "C" int KBPARSE_API main3(char*argv);
extern  "C" KBPARSE_API KnowBase*knBase;
//extern  "C" KnowBase*knBase;
extern "C" int main3(char*argv);
//extern  "C" KnowBase*knBase;
//extern  "C" void printRule(yRuleDescription*rule);
//extern  "C" void  printRule(yRuleDescription*rule);


//#include "..\KbParse\calc.h"

//using namespace std;
//#include "..\KbParse\\Defs.h"

//#include "..\KbParse\calc.h"
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
extern __declspec(dllexport)std::istream&operator>>(std::istream&is,PropertyItem&pi);
extern __declspec(dllexport)std::istream&operator>>(std::istream&is,ClassItem&ci);
extern __declspec(dllexport)std::istream&operator>>(std::istream&is,ObjectItem&ci);
extern __declspec(dllexport)std::istream&operator>>(std::istream&i,SlotItem&c);
extern __declspec(dllexport)std::istream&operator>>(std::istream&i,RuleItem&c);
//extern KBPARSE_API void printRule(yRuleDescription*rule);

__declspec(dllexport) std::istream&operator>>(std::istream&i,RuleItem&c);
__declspec(dllexport) std::ostream&operator<<(std::ostream&os,const PropertiesList*Items )
{
	for(int i=0;i<Items->GetSize();i++)((PropertyItem&)Items->GetAt(i)).printOn(os);
	os<<"\n";return os;
}
//std::ostream&operator<<(std::ostream&os,const ClassItemList*cl)
__declspec(dllexport) std::ostream&operator<<(std::ostream&os,const ClassItemList*cl)
{
//	for(int i=0;i<cl->GetSize();i++){ClassItem&clas=cl->GetAt(i);clas.printOn(os);os<<")\n";}
	for(int i=0;i<cl->GetSize();i++){ClassItem clas(cl->GetAt(i));clas.printOn(os);os<<")\n";}
	return os; // << (PTStreamableBase)cl;
}
///////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(Descriptions,CObject,0)
void Descriptions::Serialize(CArchive&ar)
{
//	WORD w;
	if(ar.IsStoring()){}//w=(WORD)m_int;ar<<w;ar<<m_float;ar<<m_str;}
	else{}//ar>>w;m_int= w;ar>>m_float;ar>>m_str;}
}
#ifdef _DEBUG
void Descriptions::AssertValid()const
{
	CObject::AssertValid();
}
/*
void Descriptions::Dump(CDumpContext&dc)const
{
//	CObject::Dump(dc);
//	afxDump<<"Описания\t"<<'\n';
}
*/
#endif
///////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(ObjectDescription,CObject,0)
void ObjectDescription::Serialize(CArchive&ar)
{
//	WORD w;
	if(ar.IsStoring()){}//w=(WORD)m_int;ar<<w;ar<<m_float;ar<<m_str;}
	else{}//ar>>w;m_int= w;ar>>m_float;ar>>m_str;}
}
#ifdef _DEBUG
void ObjectDescription::AssertValid()const
{
	CObject::AssertValid();
	ASSERT(objectItems!=NULL);
//ASSERT(m_years<105;
}
/*
void ObjectDescription::Dump(CDumpContext&dc)const
{
//	CObject::Dump(dc);
//	afxDump<<"Описание объектов.\t"<<'\n';
}
*/
#endif
///////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(ClassDescription,CObject,0)
void ClassDescription::Serialize(CArchive&ar)
{
//	WORD w;
	if(ar.IsStoring()){}//w=(WORD)m_int;ar<<w;ar<<m_float;ar<<m_str;}
	else{}//ar>>w;m_int= w;ar>>m_float;ar>>m_str;}
}
#ifdef _DEBUG
void ClassDescription::AssertValid()const
{
	CObject::AssertValid();
	ASSERT(classes!=NULL);     
}
/*
void ClassDescription::Dump(CDumpContext&dc)const
{
//	CObject::Dump(dc);
//	afxDump<<"Описание классов.\t"<<'\n';
}
*/
#endif
///////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(PropertyDescription,CObject,0)
void PropertyDescription::Serialize(CArchive&ar)
{
//	WORD w;
	if(ar.IsStoring()){}//w=(WORD)m_int;ar<<w;ar<<m_float;ar<<m_str;}
	else{}//ar>>w;m_int= w;ar>>m_float;ar>>m_str;}
}
#ifdef _DEBUG
void PropertyDescription::AssertValid()const
{
	CObject::AssertValid();
	ASSERT(propItems!=NULL);     
//ASSERT(m_years<105;
}
/*
void PropertyDescription::Dump(CDumpContext&dc)const
{
//	CObject::Dump(dc);
//	afxDump<<"Описание свойств.\t"<<'\n';
}
*/
#endif
///////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(SlotDescription,CObject,0)
void SlotDescription::Serialize(CArchive&ar)
{
//	WORD w;
	if(ar.IsStoring()){}//w=(WORD)m_int;ar<<w;ar<<m_float;ar<<m_str;}
	else{}//ar>>w;m_int= w;ar>>m_float;ar>>m_str;}
}
#ifdef _DEBUG
void SlotDescription::AssertValid()const
{
	CObject::AssertValid();
	ASSERT(slots!=NULL);     
//ASSERT(m_years<105;
}
/*
void SlotDescription::Dump(CDumpContext&dc)const
{
//	CObject::Dump(dc);
  // now do the stuff for our specific class
//dc << "last name: " << m_lastName << "\n"
//<< "first name: " << m_firstName << "\n";
//	dc<<"Условие\t"<<'\n';
//	afxDump<<"Описание слотов.\t"<<'\n';
}
*/
#endif
///////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(RuleDescription,CObject,0)
void RuleDescription::Serialize(CArchive&ar)
{
//	WORD w;
	if(ar.IsStoring()){}//w=(WORD)m_int;ar<<w;ar<<m_float;ar<<m_str;}
	else{}//ar>>w;m_int= w;ar>>m_float;ar>>m_str;}
}
#ifdef _DEBUG
void RuleDescription::AssertValid()const
{
	CObject::AssertValid();
	ASSERT(rules!=NULL);     
}
/*
void RuleDescription::Dump(CDumpContext&dc)const
{
//	CObject::Dump(dc);
//	afxDump<<"Описание правил.\t"<<'\n';
}
*/
#endif
///////////////////////////////////////////////////////////////////////////////////////////////
__declspec(dllexport) std::ostream&operator<<(std::ostream&os,const ObjectItemList*cl)
{
//	for(int i=0;i<cl->GetSize();i++){ObjectItem&obj=cl->GetAt(i);obj.printOn(os);os<<")\n";}return os;
	for(int i=0;i<cl->GetSize();i++){ObjectItem obj(cl->GetAt(i));obj.printOn(os);os<<")\n";}return os;
}
///////////////////////////////////////////////////////////////////////////////////////////////
__declspec(dllexport) std::istream&operator>>(std::istream&is,SlotItemList*){return is;}
//std::ostream&operator<<(std::ostream&os,const SlotItemList&cl){return os<<cl;}
__declspec(dllexport)std::ostream&operator<<(std::ostream&os,const SlotItemList*cl)
{
//	for(int i=0;i<cl->GetSize();i++){SlotItem&slt=cl->GetAt(i);slt.printOn(os);os<<")\n";}return os;
	for(int i=0;i<cl->GetSize();i++){SlotItem slt(cl->GetAt(i));slt.printOn(os);os<<")\n";}return os;
}
__declspec(dllexport)std::ostream&operator<<(std::ostream&os,const SlotItemList&cl)
{
//	for(int i=0;i<cl.GetSize();i++){SlotItem&slt=cl.GetAt(i);slt.printOn(os);os<<")\n";}return os;
	for(int i=0;i<cl.GetSize();i++){SlotItem slt(cl.GetAt(i));slt.printOn(os);os<<")\n";}return os;
}
///////////////////////////////////////////////////////////////////////////////////////////////
AtomValueType getAtomType(LPSTR str)
{
	AtomValueType type;
	if(strcmp(str,"Boolean")==0)type=BoolType;else
	if(strcmp(str,"Float")==0)type=FloatType;else
	if(strcmp(str,"Integer")==0)type=IntegerType;else
	if(strcmp(str,"String")==0)type=StringType;else
	if(strcmp(str,"Date")==0)type=DateType;else
	if(strcmp(str,"Time")==0)type=TimeType;else
	if(strcmp(str,"Special")==0)type=SpecialType;
	return type;
}
bool Enable=true;
string*trimSlash(string*str1){size_t size1=str1->length()-1;str1->resize(size1);return str1;}
string*removeSlashesStr(string*str)
{
	string str1;
	int iPos;
	while((iPos=str->find_first_of('\\'))!=str->npos)
	{
		str1+=str->substr(0,iPos);
		*str=str->substr(iPos+2);
	}
	if(!str->empty())str1+=*str;
	*str=str1;
	return str;
/*
	string*str2=new string;
	char*str3=new char[str->length()+1];
	is.unsetf(ios::skipws);
	is<<((char*)str->c_str());
	is<<str->c_str();
	ws(is);
	while(is.good())
	{
  	str1.assign("");ws(is);is.getline(str3,str->length()+1,'\\');str1=str3;
		if(!str1.empty()&&str1.at(str1.length()-1)=='\\')trimSlash(&str1);
		*str2+=str1;//->append(str1);
		ws(is);
  }return str2;
*/
}
char*trimBlanks(char*str)
{
	char*str1=new char[strlen(str)+1];
	strstream is;
	strstream is2;

	is.unsetf(ios::skipws);
	is2.unsetf(ios::skipws);
	//	is.setf(ios::skipws);
	is<<str;
	ws(is);
	is.getline(str1,strlen(str)+1);
	is.seekp(0);
	strrev(str1);
	is2 << str1;
	is2.seekg(0);
	ws(is2);
	is2.getline(str1,strlen(str1)+1);
	strcpy(str,strrev(str1));
	delete str1;
	return str;
}
CStringList*arrayFromIs(std::istream&is,CStringList*arr)
{
	string str;
	getline(is,str,'(');
	is>>ws;
	getline(is,str,'(');
	trimBlanks((LPSTR)str.c_str());
	arr->AddTail(*(new CString(str.c_str())));
	is>>ws;
	str="";
	char c1;
	char c[2];
  try{while((c1=is.peek())!='('&&c1!=')'){is.get(c,2);str+=c;}}
	catch(char*ss){MessageBox(NULL,ss,NULL,MB_OK);}
//	catch(xmsg&x){MessageBox(NULL,x.why().c_str(),NULL,MB_OK);}
	is.ignore();
	is>>ws;
	is.clear();
	trimBlanks((LPSTR)str.c_str());
	if(!str.empty()&&str.at(str.length()-1)=='\\')trimSlash(&str);
	arr->AddTail(*(new CString(str.c_str())));
	is>>ws;
  try
	{
  	if(is.peek()=='(')
		{
    	is.ignore();
			getline(is,str,')');
		 	trimBlanks((LPSTR)str.c_str());
		 	if(str.find_first_of('\\')!=-1)
			{
				string*str22=removeSlashesStr(&str);
				arr->AddTail(*(new CString(str22->c_str())));
				TRACE0((char*)str.c_str());TRACE0("\n");
			}
		 	else arr->AddTail(*(new CString(str.c_str())));
		 	is>>ws;
			getline(is,str,')');
    }else{getline(is,str,')');arr->AddTail(*(new CString()));}
//		arr->AddAt(new string,2);}
  }
	catch(char*str){MessageBox(NULL,str,NULL,MB_OK);}
	is>>ws;
	return arr;
}
void*KnowledgeBase::read(FILE*is)
{
	try
  {
		props=new PropertyDescription();ASSERT_VALID(props);
		objects=new ObjectDescription();ASSERT_VALID(objects);
		classes=new ClassDescription();ASSERT_VALID(classes);
		slots=new SlotDescription();ASSERT_VALID(slots);
		rules=new RuleDescription();ASSERT_VALID(rules);
		globals=new GlobalsItem();ASSERT_VALID(globals);
  }catch(char*ss){MessageBox(NULL,ss,NULL,MB_OK);}
	string str;
//	is->
//	is.setf(ios::skipws);
	return this;
}
void*KnowledgeBase::read(LPCTSTR is)
{
	try
  {
		fileName=new string(is);
		props=new PropertyDescription();ASSERT_VALID(props);
		objects=new ObjectDescription();ASSERT_VALID(objects);
		classes=new ClassDescription();ASSERT_VALID(classes);
		slots=new SlotDescription();ASSERT_VALID(slots);
		rules=new RuleDescription();ASSERT_VALID(rules);
		globals=new GlobalsItem();ASSERT_VALID(globals);
  }catch(char*ss){MessageBox(NULL,ss,NULL,MB_OK);}
	string str;
	main3((char*)is);
//	KnowBase*knBase222;
	if(knBase!=NULL)
	{
		PropertyItems*aa=knBase->propDesc->next;
		if (aa != NULL)
		{
			yPropertyItem*item = aa->item;
			if (aa->item != NULL)
			{
				while (aa != NULL)
				{
					PropertyItem&prop = *(new PropertyItem());
					ASSERT_VALID(&prop);
					if (strcmp(item->propertyName, "") != 0)prop.propertyName->append(aa->item->propertyName);
					if (strcmp(item->format1, "") != 0)prop.format->append(aa->item->format1);
					if (strcmp(item->identifier, "") != 0)prop.identifier->append(aa->item->identifier);
					prop.atomType = aa->item->atomType;
					props->items->Add(prop);
					(*props->propItems)[prop.propertyName->c_str()] = &prop;
					aa = aa->next;
				}
			}
		}
		ClassItems*bb=knBase->classDesc->next;
		if (bb != NULL)
		{
			yClassItem*clItem = bb->item;
			if (clItem != NULL)
			{
				while (bb != NULL)
				{
					clItem = bb->item;
					ClassItem&clas = *(new ClassItem());
					ASSERT_VALID(&clas);
					if (clItem->className != NULL)
					{
						clas.className = new string(clItem->className->name);
						clas.type = clItem->className->type;
					}
					if (bb->item->classItemList != NULL)
					{
						yClassItemList*items = clItem->classItemList;
						SubclassList*subClassList = items->subClassList;
						if (subClassList != NULL)
						{
							NameList*nameList = subClassList->list;
							while (nameList != NULL)
							{
								ClassItem&item = *(new ClassItem());
								item.className = new string(nameList->name);
								clas.subclasses->Add(item); nameList = nameList->nextName;
							}
						}
						PublicPropList*pProps = items->publicPropList;
						while (pProps != NULL)
						{
							if (pProps->prop->name != NULL)//PublicProperty*prop;
							{
								PropertyItem&item = *(new PropertyItem());
								item.propertyName = new string(pProps->prop->name);
								item.atomType = pProps->prop->atomType;
								clas.properties->Add(item);
							}
							pProps = pProps->nextPropName;
						}
					}
					classes->items->Add(clas);
					(*classes->classes)[clas.className->c_str()] = &clas;
					bb = bb->next;
				}
			}
		}

		ObjectItems*cc=knBase->objectDesc->next;
		if(cc != NULL)
		{
			yObjectItem*oItem = cc->item;
			if (oItem != NULL)
			{
				while (cc != NULL)
				{
					oItem = cc->item;
					ObjectItem&objItem = *(new ObjectItem());
					ASSERT_VALID(&objItem);
					if (oItem->objectName != NULL)
				{
					objItem.objectName = new string(oItem->objectName->name);
					objItem.type = oItem->objectName->type;
				}
					yObjectItemList*list = oItem->objectItemList;
					if (list != NULL)
				{
					NameList*classList = list->classList;
					while (classList != NULL)
					{
						if (classList->name != NULL)
						{
							ClassItem&item = *(new ClassItem());
							item.className = new string(classList->name);
							objItem.classes->Add(item);
						}
						classList = classList->nextName;
					}
					PublicPropList*publicPropList = list->publicPropList;
					while (publicPropList != NULL)
					{
						PublicProperty*prop = publicPropList->prop;
						if (prop->name != NULL)
						{
							PropertyItem&item = *(new PropertyItem());
							item.propertyName = new string(prop->name);
							item.atomType = prop->atomType;
							objItem.properties->Add(item);
						}
						publicPropList = publicPropList->nextPropName;
					}
					NameList*subObjects = list->subObjects;
					while (subObjects != NULL)
					{
						if (subObjects->name != NULL)
						{
							ObjectItem&item = *(new ObjectItem());
							item.objectName = new string(subObjects->name);
							objItem.subobjects->Add(item);
						}
						subObjects = subObjects->nextName;
					}
				}
					objects->objectItemList->Add(objItem);
					(*objects->objectItems)[objItem.objectName->c_str()] = &objItem;
					cc = cc->next;
				}
			}
		}
		MethodItems*dd=knBase->methodDesc->next;
//		MethodItem*mItem=dd->item;
		if(dd && dd->item!=NULL)
		{
			MethodItem*mItem = dd->item;
			/*
			while(dd!=NULL)
			{
				yAtomId*name=dd->item->atomId;
				if(name!=NULL)AfxMessageBox(name->atomName);
				dd=dd->next;
			}*/
		}
		MetaSlots*ff=knBase->metaSlots->next;
		if( ff != NULL)
		{
			SlotDesc*sItem = ff->item;
			if (sItem != NULL)
		{
			while (ff != NULL)
			{
				sItem = ff->item;
				if (sItem->slotName1 != NULL)
				{
					SlotItem&item = *(new SlotItem());
					ASSERT_VALID(&item);
					ySlotItem*ysItem = sItem->slot;
					//				if(ff->item->slotName1!=NULL)AfxMessageBox(ff->item->slotName1);
					item.slotName = new string(sItem->slotName1);

					item.infCat = ysItem->infCat1;
					item.inhCat = ysItem->inhCat1;
					if (ysItem->infAtom1 != NULL)
					{
						item.infAtom = new SlotItem;
						item.infAtom->slotName = new string(ysItem->infAtom1);
					}
					if (ysItem->inhAtom1 != NULL)
					{
						item.inhAtom = new SlotItem;
						item.inhAtom->slotName = new string(ysItem->inhAtom1);
					}
					if (ysItem->prompt1 != NULL)item.prompt = new string(ysItem->prompt1);
					if (ysItem->comments1 != NULL)item.comments = new string(ysItem->comments1);
					if (ysItem->why1 != NULL)item.why = new string(ysItem->why1);
					if (ysItem->format1 != NULL)item.format = new string(ysItem->format1);

					item.inhSlotUp = ysItem->inhSlotUp1;
					item.inhSlotDown = ysItem->inhSlotDown1;
					item.inhValUp = ysItem->inhValUp1;
					item.inhValDown = ysItem->inhValDown1;
					item.inhBreadth = ysItem->inhBreadth1;
					item.inhParent = ysItem->inhParent1;
					NameList*contexts = ysItem->contexts;
					while (contexts != NULL)
					{
						SlotItem&sl = *(new SlotItem());
						sl.slotName = new string(contexts->name);
						item.contexts->Add(sl);
						contexts = contexts->nextName;
					}
					//				KnownValue*knownValue;
					//				yKnownValue*initVal1;
					//				yKnownValue*privInitVal1;
					slots->slotItems.Add(item);
					(*slots->slots)[item.slotName->c_str()] = &item;
				}
				ff = ff->next;
			}
		}
		}
		yRuleDescriptions*ee=knBase->ruleDesc->next;
		if(ee && ee->item !=NULL)
		{
			int cn = 0;
			yRuleDescription*rDesc = ee->item;
			strstream os;
//			os.rdbuf;
			char buf[1024];
//			os
//			stdout->_Placeholder
//			setbuf(stdout, buf);
//			printRule(rDesc);
//			cout << rDesc;
			while(ee!=NULL)
			{
				rDesc=ee->item;
				RuleItem&item=*(new RuleItem());
				ASSERT_VALID(&item);
				if(rDesc->identifier!=NULL)
				{
					item.ruleName=new string(rDesc->identifier);
				}
				yRuleItemList*items=rDesc->ruleItems;
				while(items!=NULL)
				{	//infCatType=1,infAtomType=2,commentsType=3,whyType=4,ruleNameType=5,hypoType=6,conditionsType=7,actionsType=8
					yRuleItem*rItem=items->item;
					switch((int)rItem->type)
					{
						case infCatType:
							break;
						case infAtomType:
							break;
						case commentsType:
							break;
						case whyType:
							break;
						case ruleNameType:
							break;
						case hypoType:if(rItem->gen.hypo!=NULL)
							item.hypo->slotName=new string(rItem->gen.hypo->name);break;
						case conditionsType:
							if(rItem->gen.conditions!=NULL)
							{
								yListOfConditions*condList=rItem->gen.conditions;
								item.setConditions(condList);
								cn++;
							}
							break;
						case actionsType:
							if(rItem->gen.actions!=NULL)
							{
/*
								struct KBPARSE_API yListOfRhsActions
								{
									RhsAssignments*assignments;
									InterfaceActions*interfaceActions;
									DynObjectsActions*dynActions;
									InheritanceControls*inheritanceControls;
								};
*/
								int abc = 1;
								yListOfRhsActions*actList = rItem->gen.actions;
								item.setActions(actList);
//								item.

//								printActions(item->gen.actions);
							}
							break;
						case actionsType2:
							if (rItem->gen.actions2 != NULL)
							{
								int abc = 1;
								ListOfActions*actList2 = rItem->gen.actions2;
								item.setActions2(actList2);
							}
							break;
						default:break;
					}
					items=items->next;
				}
//				rules->ruleItems->Add(item);
//typedef CArray<RuleItem,RuleItem&>TRuleList;
//				class AFX_EXT_CLASS RuleItemList :public TRuleList//, public TStreamableBase
//				{
				rules->ruleItems->Add(item);
//				rules->ruleItems.
				CString *str = new CString(item.ruleName->c_str());
//				(*rules->rules)[item.ruleName->c_str()] = &item;
				(*rules->rules)[*str]=&item;
				ee=ee->next;
			}
		}
	}
	traceProperties();
	traceClasses();
	traceObjects();
	traceSlots();
//	traceRules();
	return this;
}
void*KnowledgeBase::read(std::istream&is)
{
	try
  {
		props=new PropertyDescription();ASSERT_VALID(props);
		objects=new ObjectDescription();ASSERT_VALID(objects);
		classes=new ClassDescription();ASSERT_VALID(classes);
		slots=new SlotDescription();ASSERT_VALID(slots);
		rules=new RuleDescription();ASSERT_VALID(rules);
		globals=new GlobalsItem();ASSERT_VALID(globals);
  }catch(char*ss){MessageBox(NULL,ss,NULL,MB_OK);}
	string str;
	is.setf(ios::skipws);
	try
  {
		while(is.good())
		{
			getline(is,str,'@');
			getline(is,str,'=');
			is>>ws;
//		if(str=="VERSION"){str.read_to_delim(is,')');}else
			if(str=="PROPERTY")
			{
				PropertyItem&prop=*(new PropertyItem());
				CString*pName;
//				pName = &(prop.propertyName);// ->c_str();
				char st1[128];
//				string*
				strcpy(st1, prop.propertyName->c_str());
				ASSERT_VALID(&prop);
				is>>prop;
				strcpy(st1, prop.propertyName->c_str());
				//				is >> *(prop.propertyName->c_str);
				props->items->Add(prop);
				(*props->propItems)[prop.propertyName->c_str()]=&prop;
				
			}else
			if(str=="CLASS")
			{
				ClassItem&clas=*(new ClassItem());
				ASSERT_VALID(&clas);
				is>>clas;
				classes->items->Add(clas);
				(*classes->classes)[clas.className->c_str()]=&clas;
			}else
			if(str=="OBJECT")
			{
				ObjectItem&item=*(new ObjectItem());
				ASSERT_VALID(&item);
				is>>item;
				objects->objectItemList->Add(item);
				(*objects->objectItems)[item.objectName->c_str()]=&item;
			}else
			if(str=="SLOT")
			{
				SlotItem&item=*(new SlotItem());
				ASSERT_VALID(&item);
				is>>item;
				slots->slotItems.Add(item);
				(*slots->slots)[item.slotName->c_str()]=&item;
			}else
			if(str=="RULE")
			{
				RuleItem&item=*(new RuleItem());
				ASSERT_VALID(&item);
				is>>item;
				rules->ruleItems->Add(item);
				(*rules->rules)[item.ruleName->c_str()]=&item;
			}else
			if(str=="GLOBALS"){if(str=="GLOBALS")is>>globals;}
		}
  }catch(char*ss){MessageBox(NULL,ss,"Программа чтения базы знаний.",MB_OK);}
	traceProperties();
	traceClasses();
	traceObjects();
	traceSlots();
	traceRules();
	return this;
}
void KnowledgeBase::traceProperties()
{
	PropertyItem*prop2;
	CString strKey;
	CMapStringToPropertyItem*props2=props->propItems;
	POSITION pos=props2->GetStartPosition();
	TRACE0("Свойства\t\t\t\tСвойства\t\t\t\tСвойства\t\t\t\tСвойства\t\t\t\tСвойства\n");
	while(pos!=NULL)
	{
		props2->GetNextAssoc(pos,strKey,prop2);
		TRACE0("\t");
		TRACE0(prop2->propertyName->c_str());
		TRACE0("\n");
	}
}
void KnowledgeBase::traceClasses()
{
	CString strKey;
	ClassItem*clas2;
	CMapStringToClassItem*cls2=classes->classes;
	POSITION pos=cls2->GetStartPosition();
	TRACE0("Классы\t\t\t\tКлассы\t\t\t\tКлассы\t\t\t\tКлассы\t\t\t\tКлассы\n");
	while(pos!=NULL)
	{
		cls2->GetNextAssoc(pos,strKey,clas2);
		TRACE0("\t");
		TRACE0(clas2->className->c_str());
		TRACE0("\n");
	}
}
void KnowledgeBase::traceObjects()
{
	CString strKey;
	ObjectItem*item2;
	CMapStringToObjectItem*obj2=objects->objectItems;
	POSITION pos=obj2->GetStartPosition();
	TRACE0("Объекты\t\t\t\tОбъекты\t\t\t\tОбъекты\t\t\t\tОбъекты\t\t\t\tОбъекты\n");
	while(pos!=NULL)
	{
		obj2->GetNextAssoc(pos,strKey,item2);
		TRACE0("\t");
		TRACE0(item2->objectName->c_str());
		TRACE0("\n");
	}
}
void KnowledgeBase::traceSlots()
{
	CString strKey;
	SlotItem*slotItem2;
	CMapStringToSlotItem*slotItems=slots->slots;
	POSITION pos=slotItems->GetStartPosition();
	TRACE0("Слоты\t\t\t\tСлоты\t\t\t\tСлоты\t\t\t\tСлоты\t\t\t\tСлоты\n");
	while(pos!=NULL)
	{
		slotItems->GetNextAssoc(pos,strKey,slotItem2);
		TRACE0("\t");
		TRACE0(slotItem2->slotName->c_str());// .slotName->c_str()
		TRACE0("\n");
	}
}
void KnowledgeBase::traceRules()
{
	CString strKey;
	RuleItem*rItem;
	CMapStringToRuleItem*rItems=rules->rules;
	POSITION pos=rItems->GetStartPosition();
	DWORD strKeyCond, strKeyCond2;
	TRACE0("Правила\t\t\t\tПравила\t\t\t\tПравила\t\t\t\tПравила\t\t\t\tПравила\n");
	ostrstream stStream;
//	RuleDescription*d
	stStream << rules->ruleItems;
	while(pos!=NULL)
	{
		rItems->GetNextAssoc(pos,strKey,rItem);
		TRACE0("\t");
		TRACE0(rItem->ruleName->c_str());// .slotName->c_str()
		TRACE0("\t\t");
		TRACE0(rItem->hypo->slotName->c_str());// .slotName->c_str()
		TRACE0("\n");
		CMapDWordToCondition*condMap =rItem->MapOfConditions;
		CCondition*cond;
		CLhsTest*lhsTest;
/*
		CMapStringToClassItem*cls2 = classes->classes;
		POSITION pos = cls2->GetStartPosition();
		TRACE0("Классы\t\t\t\tКлассы\t\t\t\tКлассы\t\t\t\tКлассы\t\t\t\tКлассы\n");
		while (pos != NULL)
		{
			cls2->GetNextAssoc(pos, strKey, clas2);
*/
		POSITION pos2 = condMap->GetStartPosition();
//		condMap.
		CMapDWordToLhsTest*tst;
		int asas;
		POSITION pos3;
		while (pos2 != NULL)
		{
//			typedef CTypedPtrMap<CMapWordToOb, DWORD, CLhsTest*>CMapDWordToLhsTest;
			condMap->GetNextAssoc(pos2, strKeyCond, cond);
			TypeOfCondition type;
//			enum TypeOfCondition {assignments = 1, interfaceActions = 2, dynActions = 3,inheritanceControls = 4, testCondition = 5};
			CBooleanTest*cbt;
			CMemberTest*cmt;
			CListOfGenericObjectsOrClasseses*ListOfGenericObjects;
			switch (cond->type)
			{
				case assignments:
				asas = 1;
				break;
				case interfaceActions:
				asas = 1;
				break;
				case dynActions:
				asas = 1;
				break;
				case inheritanceControls:
				asas = 1;
				break;
				case testCondition:
					tst=cond->gen.tests;
					pos3=tst->GetStartPosition();
					while (pos3 != NULL)
					{
						tst->GetNextAssoc(pos3, strKeyCond2, lhsTest);
//	union{CComparisons*comparisons;CMemberTest*memberTest;CBooleanTest*booleanTest;CEqualityListConstants*equalityListConstants;}gen;
// enum LhsTestType{BoolTestType=1,CompareType=2,EqualListType=3,LhsStatusType=4, EqualSlotsType = 5, LhsMemberType = 6					};
						switch (lhsTest->type)
						{
							case BoolTestType:
/*
								cbt=lhsTest->gen.booleanTest;
//								cbt->slot->
								if (cbt->type == YesType)TRACE("Yes\t\t");
								if (cbt->type == NoType)TRACE("No\t\t");
								if (cbt->slot!=NULL)
								{
									TRACE(cbt->slot->name->c_str());
									TRACE("\t\t\n");
								}
*/
								asas = 1;
								break;
							case CompareType: 
								break;
							case EqualListType: 
								break;
							case LhsStatusType: 
								break;
							case EqualSlotsType: 
								break;
							case LhsMemberType: 
//								enum MemberType { Member = 1, NotMember = 2 };
								cmt = lhsTest->gen.memberTest;
								if (cmt->type == Member)TRACE("Member\t\t");
								if (cmt->type == NotMember)TRACE("NotMember\t\t");
								ListOfGenericObjects = cmt->list;
								if (cmt->obj != NULL)
								{
									if (cmt->obj->type == PatternType)
									{
										TRACE(cmt->obj->gen.objectPattern->exist->objName->c_str());
										TRACE("\n");
									}
								}
								//								cmt->obj->gen.objName;
//									TRACE(cmt->obj->name->c_str());
								if (cmt->cls != NULL)
								{
//									TRACE(cmt->cls->name->c_str());
								}
								//								TRACE(cmt->list.
								asas = 1;

								break;

						}
					}
//					tst->
					asas = 1;
//				tst->

				break;
			}
			int bbb = 1;
		}

/*
		while ((cond = condMap->GetNextAssoc) != NULL)
		{
			int aa = 1;
		}
*/
	}
}

void*PropertiesList::read(std::istream&is)
{
	string str;
	while(is.good())
	{
		getline(is,str,'@');getline(is,str,')');PropertyItem&item=*(new PropertyItem());is>>item;Add(item);
	}return this;
}

//__declspec(dllexport)std::istream&operator>>(std::istream&i,RuleItem&c){c.read(i);return i;}
__declspec(dllexport)std::ostream&operator<<(std::ostream&os,const RuleItemList*cl)
{
	for(int i=0;i<cl->GetSize();i++)
	{
//		RuleItem*rul=&cl->GetAt(i);rul->printOn(os);os<<")\n";
		RuleItem*rul=(RuleItem*)&(cl->GetAt(i));
		rul->printOn(os);os<<")\n";
	}
	return os; // << (PTStreamableBase )cl;
}
/*
ListOfConditions::~ListOfConditions()
{
//  this->forEach((iterFuncType)delete); //,(void *)NULL); //iterFuncType
//  RArrayIterator Iterator = *(new ArrayIterator(this));//initIterator();
	PArray array;
	RContainerIterator Iterator = initIterator();
	while( Iterator != 0 )
	{
//      RArray aValue = RArray (iter1.current()); // aValue  - ¬ ббЁў б ®¤­®©
//    RString str = (RString)((RArray)Iterator.current())[0];
		array = &(RArray)(Iterator.current());
		array->detach(0);
		array->detach(1);
		array->detach(2);
		//&(Iterator.current());
		Iterator++;
	}
}
ListOfRhsActions::~ListOfRhsActions()
{
//  RArrayIterator Iterator = *(new ArrayIterator(this));//initIterator();
	PArray array;
	RContainerIterator Iterator = initIterator();
	while( Iterator != 0 )
	{  //delete &(Iterator++);
//    delete &(Iterator.current());
		array = &(RArray)(Iterator.current());
		array->detach(0);
		array->detach(1);
		array->detach(2);
		//&(Iterator.current());
		Iterator++;
	}
//  if(rightParts != NULL) delete rightParts;
}
TStreamableBase * ListOfRhsActions::build(){return new ListOfRhsActions(streamableInit);}
TStreamableBase * RuleItemList::build(){return new RuleItemList(streamableInit);}
TStreamableClass RegRuleItemList("RuleItemList",RuleItemList::build,__DELTA(RuleItemList));
*/
KnownValue::KnownValue(const KnownValue&val)
{
	type=val.type;
	switch((int)val.type)
	{
		case digit:	knownValue.digit=val.knownValue.digit;break;
		case stringType:knownValue.str=val.knownValue.str;break;
		case known1:knownValue.known=val.knownValue.known;break;
		case unknown:knownValue.unknown=val.knownValue.unknown;break;
		case notknown:knownValue.notknown=val.knownValue.notknown;break;
		case trueFalse1:knownValue.trueFalse=val.knownValue.trueFalse;break;
		default:break;
	}
}
void KnownValue::operator=(const KnownValue&val)
{
	type=val.type;
	switch((int)val.type)
	{
		case digit:	knownValue.digit=val.knownValue.digit;break;
		case stringType:knownValue.str=val.knownValue.str;break;
		case known1:knownValue.known=val.knownValue.known;break;
		case unknown:knownValue.unknown=val.knownValue.unknown;break;
		case notknown:knownValue.notknown=val.knownValue.notknown;break;
		case trueFalse1:knownValue.trueFalse=val.knownValue.trueFalse;break;
		default:break;
	}
}
void KnownValue::operator=(const KnownValue*val)
{
	type=val->type;
	switch((int)val->type)
	{
		case digit:	knownValue.digit=val->knownValue.digit;break;
		case stringType:knownValue.str=val->knownValue.str;break;
		case known1:knownValue.known=val->knownValue.known;break;
		case unknown:knownValue.unknown=val->knownValue.unknown;break;
		case notknown:knownValue.notknown=val->knownValue.notknown;break;
		case trueFalse1:knownValue.trueFalse=val->knownValue.trueFalse;break;
		default:break;
	}
}
//void KnownValue::print(KnownValue*val)
void KnownValue::printOn(std::ostream&os)
{
	//digit=1,string=2,known1=3,unknown=4,notknown=5,trueFalse1=6
	switch ((int)type)
	{
		case digit:
			printf("%.10g", knownValue.digit);
			TRACE("%.10g", knownValue.digit);
			os<< ("%.10g", knownValue.digit);
			break;
		case stringType:
			printf("\"%s\"", knownValue.str->c_str());
			TRACE("\"%s\"", knownValue.str->c_str());
			os << ("\"%s\"", knownValue.str->c_str());
			break;
		case known1:
			if (knownValue.known == 1)
			{
				printf(knownStr);
				TRACE(knownStr);
				os << (knownStr);
			}
			break;
		case unknown:
			if (knownValue.unknown == 1)
			{
				printf(unknownStr);
				TRACE(unknownStr);
				os<< (unknownStr);
			}
			break;
		case notknown:	
			if (knownValue.notknown == 1)
			{
				printf(notknownStr); 
				TRACE(notknownStr);
				os<< (notknownStr);
			}
			break;
		case trueFalse1:	
			if (knownValue.trueFalse == 1)
			{
				printf(trueStr);
				TRACE(trueStr);
				os<< (trueStr);
			}
			else
			{
				printf(falseStr);
				TRACE(falseStr);
				os<< (falseStr);
			}
			break;
		default:
			break;
	}
}

