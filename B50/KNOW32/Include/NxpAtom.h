// NxpAtom.h: interface for the NxpAtom class.
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_NXPATOM_H__E0112941_D604_11D2_862A_00A024384FCF__INCLUDED_)
#define AFX_NXPATOM_H__E0112941_D604_11D2_862A_00A024384FCF__INCLUDED_
#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "NxpPub.h"
class AFX_EXT_CLASS KnownValue;
class AFX_EXT_CLASS IfChangeActions;
class AFX_EXT_CLASS ListOfSources;
class AFX_EXT_CLASS ClassItem;
class AFX_EXT_CLASS ClassItemList;	class AFX_EXT_CLASS ClassDescription;
class AFX_EXT_CLASS ObjectItem;			
class AFX_EXT_CLASS ObjectItemList;	class AFX_EXT_CLASS ObjectDescription;
class AFX_EXT_CLASS SlotItem;				
class AFX_EXT_CLASS SlotItemList;		class AFX_EXT_CLASS SlotDescription;
class AFX_EXT_CLASS PropertiesList;	
class AFX_EXT_CLASS PrivatePropertiesList;
class AFX_EXT_CLASS PropertyDescription;

class AFX_EXT_CLASS RuleItem;				
class AFX_EXT_CLASS RuleItemList;		class AFX_EXT_CLASS RuleDescription;

class AFX_EXT_CLASS CMethodItem;
class AFX_EXT_CLASS MethodItemList;		class AFX_EXT_CLASS MethodDescription;

class AFX_EXT_CLASS GlobalsItem; 
class AFX_EXT_CLASS ListOfConditions;
class AFX_EXT_CLASS ListOfRhsActions;
class AFX_EXT_CLASS KnowledgeBase;

class AFX_EXT_CLASS CCondition;
class AFX_EXT_CLASS Action;
class AFX_EXT_CLASS CLhsAssignment;
class AFX_EXT_CLASS CInterfaceAction;
class AFX_EXT_CLASS CDynObjectsAction;
class AFX_EXT_CLASS CInheritanceControl;
class AFX_EXT_CLASS CLhsTest;
class AFX_EXT_CLASS CNameAssignment;
class AFX_EXT_CLASS CResetAssignment;
class AFX_EXT_CLASS CAssignAssignment;
class AFX_EXT_CLASS CExpression;
class AFX_EXT_CLASS CGenericSlot;
class AFX_EXT_CLASS CInterpretedSlot;
class AFX_EXT_CLASS CSlotPattern;

class AFX_EXT_CLASS CShowActions;
class AFX_EXT_CLASS CExecuteActions;
class AFX_EXT_CLASS CRetrieveActions;
class AFX_EXT_CLASS CWriteActions;
class AFX_EXT_CLASS CLoadKbActions;
class AFX_EXT_CLASS CUnloadKbActions;
class AFX_EXT_CLASS CAskQuestion;
class AFX_EXT_CLASS CShowKeyWord;
class AFX_EXT_CLASS CListOfExecuteKeyWords;
class AFX_EXT_CLASS CRetrieveKeyWord;
class AFX_EXT_CLASS CGenSlotsObjectsClasses;
class AFX_EXT_CLASS CGenericObject;
class AFX_EXT_CLASS CGenericObject;
class AFX_EXT_CLASS CGenericClass;
class AFX_EXT_CLASS CGenericObjectsOrClasses;
class AFX_EXT_CLASS CStrategyKeyWord;
class AFX_EXT_CLASS CComparisons;
class AFX_EXT_CLASS CMemberTest;
class AFX_EXT_CLASS CBooleanTest;
class AFX_EXT_CLASS CEqualityListConstants;
class AFX_EXT_CLASS CClassName;
class AFX_EXT_CLASS CObjectName;
class AFX_EXT_CLASS CInterpretedObject;
class AFX_EXT_CLASS CObjectPattern;
class AFX_EXT_CLASS CExistObjPattern;
class AFX_EXT_CLASS CUniverseObjectPattern;

//enum AtomValueType{BoolType=1,IntegerType=2,FloatType=3,StringType=4,DateType=5,TimeType=6,SpecialType=7};
enum KnowledgeBaseUnitType
{comments=1,property_description=2,class_description=3,object_description=4,
	slot_description=5,rule_description=6,globals_description=7
};
// Тестовые операторы в левых и правых частях правил и т.д.
enum BooleanType{Yes=1,No=2};
//enum StatusType{Is=3,IsNot=4};
enum EqualityType{Equal=5,NotEqual=6};
//enum MemberType{Member=7,NotMember=8};
// Операторы назначений.
enum AssignmentTypeCond{Name=9,CondReset=10};
// Интерфейсные операторы.
enum InterfaceTypeCond{ShowCond=11,ExecuteCond=12,RetrieveCond=13,AskQ=14};
enum StrategyType{StrategyValue=15};

enum conditionType{BooleanType,EqualityType,StatusType,
		AssignmentTypeCond,MemberType,InterfaceTypeCond,DynActions,
		InheritanceControlsCond,TestCondition};

// Операторы назначений.
//enum AssignmentTypeActs{Do=1,Let=2,ActsReset=3,Assign=4};
// Операции над динамическими объектами.
//enum DynamicType{CreateDynamicObject=4,DeleteDynamicObject=5};
// Интерфейсные операторы.
//enum InterfaceTypeActs{ShowActs=6,ExecuteActs=7,RetrieveActs=8,Write=9,LoadKB=10,UnloadKB=11,AskQuestionType=12};
enum actionType{AssignmentTypeActs,DynamicType,InterfaceTypeActs};

// Управление наследованием и выводом.
//enum ControlType{StrategyControl,ControlOfInheritance,Backward,Iterrupt,RunTimeValue};
/*
typedef class NxpAtom far*NxAtomPtr;
typedef const class NxpAtom far*NxAtomCPtr;
typedef class NxpAtom far*NxAtomRef;
typedef const class NxpAtom far*NxAtomCRef;

typedef class NxpAtom far*NxpAtomPtr;
typedef const class NxpAtom far*NxpAtomCPtr;
typedef class NxpAtom far&NDNxpAtomRef;
typedef const class NxpAtom far&NDNxpAtomCRef;
typedef NxpAtomPtr far*NxpAtomPtrPtr;
*/
typedef class NDNxAtom far*NxAtomPtr;
typedef const class NDNxAtom far*NxAtomCPtr;
typedef class NDNxAtom far*NxAtomRef;
typedef const class NDNxAtom far*NxAtomCRef;
typedef class NDNxAtom far*NDNxAtomPtr;
typedef const class NDNxAtom far*NDNxAtomCPtr;
typedef class NDNxAtom far&NDNxAtomRef;
typedef const class NDNxAtom far&NDNxAtomCRef;
typedef NxAtomPtr far*NxAtomPtrPtr;
//------------------------------------------------------------------------
//	Static methods ...
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//	Error return codes
//------------------------------------------------------------------------
typedef int NxAtomErrEnum;
typedef int far*NxAtomErrEnumPtr;
typedef const int far*NxAtomErrEnumCPtr;
typedef int far*NxAtomErrEnumRef;
typedef const int far*NxAtomErrEnumCRef;
typedef int far*NDNxAtomErrEnumPtr;
typedef const int far*NDNxAtomErrEnumCPtr;
typedef int far&NDNxAtomErrEnumRef;
typedef const int far&NDNxAtomErrEnumCRef;
enum _NxAtomErrEnum{
	NxpAtom_ERR_FIRST,
	NxpAtom_ERR_FIRST_TAG = NxpAtom_ERR_FIRST - 1,

	NxpAtom_ERR_NOERR		= NXP_ERR_NOERR,		// Ошибка: ошибки нет.
	NxpAtom_ERR_INVARG1	= NXP_ERR_INVARG1,	// Ошибка: неверный первый аргумент вызова.
	NxpAtom_ERR_INVARG2	= NXP_ERR_INVARG2,	// Ошибка: неверный второй аргумент вызова.
	NxpAtom_ERR_INVARG3	= NXP_ERR_INVARG3,	// Ошибка: неверный третий аргумент вызова.
	NxpAtom_ERR_INVARG4	= NXP_ERR_INVARG4,	// Ошибка: неверный четвёртый аргумент вызова.
	NxpAtom_ERR_INVARG5	= NXP_ERR_INVARG5,	// Ошибка: неверный пятый аргумент вызова.
	NxpAtom_ERR_INVARG6	= NXP_ERR_INVARG6,	// Ошибка: неверный шестой аргумент вызова.
	NxpAtom_ERR_NOTFOUND= NXP_ERR_NOTFOUND,	// Ошибка: No atom of the right type was found 
	NxpAtom_ERR_INVATOM	= NXP_ERR_INVATOM,	// Ошибка: Some atoms saved in the file are invalid 
	NxpAtom_ERR_UNKNOWN	= NXP_ERR_UNKNOWN,	// Ошибка: значение атома - UNKNOWN.
	NxpAtom_ERR_NOTKNOWN= NXP_ERR_NOTKNOWN,	// Ошибка: значение атома - NOTKNOWN.
	NxpAtom_ERR_INVSTATE= NXP_ERR_INVSTATE,	// Ошибка: аргумент в плохом ссостоянии. 
	NxpAtom_ERR_MATHERROR=NXP_ERR_MATHERROR,// Ошибка: ошибка плавающей точки.
	NxpAtom_ERR_NOTAVAIL= NXP_ERR_NOTAVAIL,	// Ошибка: сведения недоступны.
	NxpAtom_ERR_COMPILEPB=NXP_ERR_COMPILEPB,// Ошибка: не завершена компиляция нового атома
																					// и неправильное сообщение об ошибке.
//		 
	NxpAtom_ERR_PROTPB	= NXP_ERR_PROTPB,		// Ошибка: ошибка защиты программы.
	NxpAtom_ERR_FILEOPEN= NXP_ERR_FILEOPEN,	// Ошибка: файл не может быть открыт.
	NxpAtom_ERR_FILEEOF	= NXP_ERR_FILEEOF,	// Ошибка: неожиданный конец файла.
	NxpAtom_ERR_FILEREAD= NXP_ERR_FILEREAD,	// Ошибка: ошибка чтения файла.
	NxpAtom_ERR_FILEWRITE=NXP_ERR_FILEWRITE,// Ошибка: ошибка записи файла.
	NxpAtom_ERR_FILESEEK= NXP_ERR_FILESEEK,	// Ошибка: ошибка поиска по файлу.
	NxpAtom_ERR_SYNCERROR=NXP_ERR_SYNCERROR,// Ошибка: разборщик потерял синхронизацию.
																						// Могло нарушиться содержимое файла.
//		 
	NxpAtom_ERR_FORMATERROR=NXP_ERR_FORMATERROR,// Ошибка: неверный заголовок файла.
	NxpAtom_ERR_NOMEMORY		= NXP_ERR_NOMEMORY,	// Ошибка: ошибка размещения памяти.
	NxpAtom_ERR_ABORT		= NXP_ERR_ABORT,				// Ошибка: компиляция прервана пользователем либо
																							// незаконченное описание и нет возможности спросить 
																							// пользователя.
//		 
	NxpAtom_ERR_SYNTAX		= NXP_ERR_SYNTAX,			// Ошибка: синтасическая ошибка в файле.
	NxpAtom_ERR_INTERNAL		= NXP_ERR_INTERNAL,	// Ошибка: ошибка внутреннего состояния.
	NxpAtom_ERR_VOLINVAL		= NXP_ERR_VOLINVAL,	// Ошибка: указатель VolunteerValidate возвратил FALSE.
	NxpAtom_ERR_SAVEDISABLED= NXP_ERR_SAVEDISABLED,	// Ошибка: сохранение БЗ было запрещено.
	NxpAtom_ERR_VALIDATEERROR=NXP_ERR_VALIDATEERROR,// Ошибка: ошибка допустимости данных. Данные недопустимы.
//		 
	NxpAtom_ERR_VALIDATEMISSING	=	NXP_ERR_VALIDATEMISSING,	// Ошибка: допустимость данных. Отсутствие данных.
	NxpAtom_ERR_VALIDATEUSER		= NXP_ERR_VALIDATEUSER,			// Ошибка: допустимость данных. The user rejected | data 
	NxpAtom_ERR_INVARG7		= NXP_ERR_INVARG7,			// Ошибка: неправильный седьмой аргумент вызова.

	NxpAtom_ERR_LAST_TAG,
	NxpAtom_ERR_LAST = NxpAtom_ERR_LAST_TAG - 1
};

//------------------------------------------------------------------------
//	Найти атом по имени/типу
//------------------------------------------------------------------------
typedef int NxAtomTypeEnum;
typedef int far*NxAtomTypeEnumPtr;
typedef const int far*NxAtomTypeEnumCPtr;
typedef int far*NxAtomTypeEnumRef;
typedef const int far*NxAtomTypeEnumCRef;
typedef int far*NDNxAtomTypeEnumPtr;
typedef const int far*NDNxAtomTypeEnumCPtr;
typedef int far&NDNxAtomTypeEnumRef;
typedef const int far&NDNxAtomTypeEnumCRef;
enum _NxAtomTypeEnum {
	NxpAtom_TYPE_FIRST,
	NxpAtom_TYPE_FIRST_TAG = NxpAtom_TYPE_FIRST - 1,

	NxpAtom_ATYPE_DATA	=NXP_ATYPE_DATA,	// Тип атома: используемый как данные слот.
	NxpAtom_ATYPE_HYPO	=NXP_ATYPE_HYPO,	// Тип атома: используемый как предположение слот.
	NxpAtom_ATYPE_PERM	=NXP_ATYPE_PERM,	// Тип атома: постоянный объект.
	NxpAtom_ATYPE_TEMP	=NXP_ATYPE_TEMP,	// Тип атома: временный объект.
	NxpAtom_ATYPE_MASK	=NXP_ATYPE_MASK,	// Тип атома: маска атома.
	NxpAtom_ATYPE_NONE	=NXP_ATYPE_NONE,	// Тип атома: неопределённый.
	NxpAtom_ATYPE_CLASS	=NXP_ATYPE_CLASS,	// Тип атома: класс.
	NxpAtom_ATYPE_OBJECT=NXP_ATYPE_OBJECT,	// Тип атома: объект (входной тип).
	NxpAtom_ATYPE_PERMOBJECT=NXP_ATYPE_OBJECT|NXP_ATYPE_PERM,
						// Тип атома: постоянный объект (return/output type only)
//		 
	NxpAtom_ATYPE_TEMPOBJECT=NXP_ATYPE_OBJECT|NXP_ATYPE_TEMP,
						// Тип атома: временный объект (return/output type only)
//		 
	NxpAtom_ATYPE_PROP	= NXP_ATYPE_PROP,	// Тип атома: свойство.
	NxpAtom_ATYPE_SLOT	= NXP_ATYPE_SLOT,	// Тип атома: слот (входной тип).
	NxpAtom_ATYPE_DATASLOT=NXP_ATYPE_SLOT|NXP_ATYPE_DATA,
															// Тип атома: слот данных (return/output type only)
	NxpAtom_ATYPE_HYPOSLOT=NXP_ATYPE_SLOT|NXP_ATYPE_HYPO,
														// Тип атома: слот предположения (return/output type only) 
	NxpAtom_ATYPE_RULE=NXP_ATYPE_RULE,// Тип атома: правило.
	NxpAtom_ATYPE_LHS=NXP_ATYPE_LHS,	// Тип атома: условие правила либо метод (в левой части).
//		 
	NxpAtom_ATYPE_RHS=NXP_ATYPE_RHS,// Тип атома: действие правила "DO" либо метод (в правиой части).
//		 
	NxpAtom_ATYPE_CACTIONS=NXP_ATYPE_CACTIONS,// Тип атома: метод If Change.
	NxpAtom_ATYPE_SOURCES=NXP_ATYPE_SOURCES,	// Тип атома: метод Order of Sources.
	NxpAtom_ATYPE_KB=NXP_ATYPE_KB,						// Тип атома: база знаний.
	NxpAtom_ATYPE_EHS=NXP_ATYPE_EHS,	// Тип атома:  действие ELSE правила или метода.
																		// (в части Else).
//		 
	NxpAtom_ATYPE_METHOD=NXP_ATYPE_METHOD,	// Тип атома: метод.
	NxpAtom_ATYPE_CONTEXT=NXP_ATYPE_CONTEXT,// Тип атома: контекстная связь.
	NxpAtom_TYPE_LAST_TAG,
	NxpAtom_TYPE_LAST=NxpAtom_TYPE_LAST_TAG-1
};//NxpAtomType;
// static  NxAtomPtr NDNxAtom::Find(CStr name, NxAtomTypeEnum code);
//	Returns an NxAtomPtr given its `name'.  `code' indicates the atom type
//	to help speed up the search, but can be NXATOM_ATYPE_NONE if the atom
//	type is unknown (see the NXATOM_TYPE_xxx constants for more types).
//	If no atom is, found, NULL is returned.
//------------------------------------------------------------------------
//	Advanced SetInfo API (use is discouraged)
//------------------------------------------------------------------------
// Use of this API is discouraged.  All the various enumerated options
// should be available via other means.
typedef int NxAtomSAInfoEnum;
typedef int far*NxAtomSAInfoEnumPtr;
typedef const int far*NxAtomSAInfoEnumCPtr;
typedef int far*NxAtomSAInfoEnumRef;
typedef const int far*NxAtomSAInfoEnumCRef;
typedef int far*NDNxAtomSAInfoEnumPtr;
typedef const int far*NDNxAtomSAInfoEnumCPtr;
typedef int far&NDNxAtomSAInfoEnumRef;
typedef const int far&NDNxAtomSAInfoEnumCRef;
enum _NxpAtomSAInfoEnum {
	NxpAtom_SAINFO_FIRST,
	NxpAtom_SAINFO_FIRST_TAG= NxpAtom_SAINFO_FIRST - 1,
	NxpAtom_SAINFO_CURRENTKB=NXP_SAINFO_CURRENTKB,	// установить текущую или изначальную базу знаний.
	NxpAtom_SAINFO_PERMLINK	=NXP_SAINFO_PERMLINK,		// изменить связь атома на постоянную.
	NxpAtom_SAINFO_MERGEKB	=NXP_SAINFO_MERGEKB,		// слить две базы знаний в одну.
	NxpAtom_SAINFO_INKB			=NXP_SAINFO_INKB,				// установить ту базу знаний, к которой относится атом.
	NxpAtom_SAINFO_PERMLINKKB=NXP_SAINFO_PERMLINKKB,// сделать все связи в базе знаний постоянными.
	NxpAtom_SAINFO_AGDVBREAK= NXP_SAINFO_AGDVBREAK,	// set/unset agenda breakpoints on hypotheses 
	NxpAtom_SAINFO_INFBREAK	= NXP_SAINFO_INFBREAK,	// set/unset inference breakpoints on atoms 
	NxpAtom_SAINFO_DISABLESAVEKB=NXP_SAINFO_DISABLESAVEKB,
		// запретить сохранение базы знаний из API.
	NxpAtom_SAINFO_LAST_TAG,
	NxpAtom_SAINFO_LAST = NxpAtom_SAINFO_LAST_TAG - 1
};
// static  Int32 NDNxAtom::SetInfo(NxAtomPtr atom1, NxAtomSAInfoEnum code, NxAtomPtr atom2, Int32 optInt);
//	Sets various types of information about `atom'.  See the API reference
//	manual for much more extensive information on allowed codes, etc.
//	Returns an integer status.
//------------------------------------------------------------------------
//	Advanced GetInfo API (use is discouraged)
//------------------------------------------------------------------------
// Use of this API is discouraged.  All the various enumerated options
// should be available via other means.
typedef int NxAtomGAInfoEnum;
typedef int far*NxAtomGAInfoEnumPtr;
typedef const int far*NxAtomGAInfoEnumCPtr;
typedef int far*NxAtomGAInfoEnumRef;
typedef const int far*NxAtomGAInfoEnumCRef;
typedef int far*NDNxAtomGAInfoEnumPtr;
typedef const int far*NDNxAtomGAInfoEnumCPtr;
typedef int far&NDNxAtomGAInfoEnumRef;
typedef const int far&NDNxAtomGAInfoEnumCRef;
enum _NxAtomGAInfoEnum{
	NxpAtom_GAINFO_FIRST,
	NxpAtom_GAINFO_FIRST_TAG=NxpAtom_GAINFO_FIRST-1,
	NxpAtom_GAINFO_PUBLIC=NXP_AINFO_PUBLIC,				// mask/flag for public information 
	NxpAtom_GAINFO_PRIVATE	= NXP_AINFO_PRIVATE,	// mask/flag for private information 
	NxpAtom_GAINFO_CURSTRAT	= NXP_AINFO_CURSTRAT,	// mask/flag for current strategy information 
	NxpAtom_GAINFO_MLHS		= NXP_AINFO_MLHS,				// mask for Left-Hand-Side information 
	NxpAtom_GAINFO_MRHS		= NXP_AINFO_MRHS,				// mask for Right-Hand-Side information 
	NxpAtom_GAINFO_MEHS		= NXP_AINFO_MEHS,				// mask for Else-Hand-Side information 
	NxpAtom_GAINFO_MASK		= NXP_AINFO_MASK,
		// mask 
	NxpAtom_GAINFO_NAME		= NXP_AINFO_NAME,					// запросить имя атома.
	NxpAtom_GAINFO_TYPE		= NXP_AINFO_TYPE,					// запросить вид атома (не тип данных).
	NxpAtom_GAINFO_VALUETYPE	= NXP_AINFO_VALUETYPE,// запросить вид данных атома.
	NxpAtom_GAINFO_VALUE		= NXP_AINFO_VALUE,			// запрос значения атома.
	NxpAtom_GAINFO_NEXT		= NXP_AINFO_NEXT,					// запрос сведений о следующем атоме.
	NxpAtom_GAINFO_PREV		= NXP_AINFO_PREV,					// запрос свдений о предыдущем атоме.
	NxpAtom_GAINFO_PARENTOBJECT	= NXP_AINFO_PARENTOBJECT,
									// запрос сведений о родителе (родителях) объекта..
//		 
	NxpAtom_GAINFO_CHILDOBJECT	= NXP_AINFO_CHILDOBJECT,// request information about the child object(s) of an
																											// object or class.
//		 
	NxpAtom_GAINFO_PARENTCLASS	= NXP_AINFO_PARENTCLASS,
		// request information about the parent class(es) of an
		// object or class
//		 
	NxpAtom_GAINFO_CHILDCLASS	= NXP_AINFO_CHILDCLASS,
		// request information about the child class(es) of a class 
	NxpAtom_GAINFO_SLOT		= NXP_AINFO_SLOT,
		// request information about the properties of an object or
		// class
//		 
	NxpAtom_GAINFO_LINKED		= NXP_AINFO_LINKED,
		// request information about the type of link between a class
		// or an object and another class or object
//		 
	NxpAtom_GAINFO_CHOICE		= NXP_AINFO_CHOICE,
		// request the choice of values (as displayed in the session
		// control window) for a given slot
//		 
	NxpAtom_GAINFO_PARENT		= NXP_AINFO_PARENT,		// request information about the parent of an atom 
	NxpAtom_GAINFO_SUGGEST		= NXP_AINFO_SUGGEST,// request whether a hypothesis is suggested or not 
	NxpAtom_GAINFO_CURRENT		= NXP_AINFO_CURRENT,// request information about the current atoms in the
		// inference engine
//		 
	NxpAtom_GAINFO_HYPO		= NXP_AINFO_HYPO,				// request the hypothesis of a rule 
	NxpAtom_GAINFO_LHS		= NXP_AINFO_LHS,				// request information about the conditions of a rule or
		// method
//		 
	NxpAtom_GAINFO_RHS		= NXP_AINFO_RHS,				// request information about the DO actions (Right-Hand-Side)
																								// of a rule or method
//		 
	NxpAtom_GAINFO_CACTIONS		= NXP_AINFO_CACTIONS,
							// request the text of the If Change method conditions or actions
//		 
	NxpAtom_GAINFO_SOURCES		= NXP_AINFO_SOURCES,
							// request the text of the Order of Sources methods attached to an atom
//		 
	NxpAtom_GAINFO_PROP		= NXP_AINFO_PROP,				// request the property of a specified slot 
	NxpAtom_GAINFO_HASMETA		= NXP_AINFO_HASMETA,// request whether or not a slot has meta-information
																								// defined for it
//		 
	NxpAtom_GAINFO_INFCAT		= NXP_AINFO_INFCAT,		// request the inference priority number attached to an atom 
	NxpAtom_GAINFO_INHCAT		= NXP_AINFO_INHCAT,		// request the inheritance priority number attached to an atom
//		 
	NxpAtom_GAINFO_INHDEFAULT	= NXP_AINFO_INHDEFAULT,
		// request whether or not the slot inheritability of the atom
		// follows the default (global strategy)
//		 
	NxpAtom_GAINFO_INHUP		= NXP_AINFO_INHUP,			// request whether or not the slot is upward inheritable 
	NxpAtom_GAINFO_INHDOWN		= NXP_AINFO_INHDOWN,	// request whether or not the slot is downward inheritable 
	NxpAtom_GAINFO_INHVALDEFAULT=NXP_AINFO_INHVALDEFAULT,
		// request whether or not the inheritability of the value
		// of the atom follows the default (global strategy)
//		 
	NxpAtom_GAINFO_INHVALUP		= NXP_AINFO_INHVALUP,
		// request whether or not the value of the atom is upward
		// inheritable
//		 
	NxpAtom_GAINFO_INHVALDOWN	= NXP_AINFO_INHVALDOWN,
		// request whether or not the value of the atom is downward
		// inheritable
//		 
	NxpAtom_GAINFO_DEFAULTFIRST	= NXP_AINFO_DEFAULTFIRST,
		// request whether or not the inheritance strategy for the
		// atom follows the default (global strategy)
//		 
	NxpAtom_GAINFO_PARENTFIRST	= NXP_AINFO_PARENTFIRST,
		// request whether the inheritance search for the atom should
		// begin by searching the parent objects of the atom or
		// the classes to which the atom belongs
//		 
	NxpAtom_GAINFO_BREADTHFIRST	= NXP_AINFO_BREADTHFIRST,
		// request whether the inheritance search for the atom is
		// done in a breadth first or depth first manner
//		 
	NxpAtom_GAINFO_PROMPTLINE	= NXP_AINFO_PROMPTLINE,	// request the prompt line information attached to the atom 
	NxpAtom_GAINFO_DEFVAL		= NXP_AINFO_DEFVAL,
		// request the initvalue for the slot (if any) as a string 
	NxpAtom_GAINFO_INHOBJUP		= NXP_AINFO_INHOBJUP,
		// request whether or not object slots are inheritable
		// upwards
//		 
	NxpAtom_GAINFO_INHOBJDOWN	= NXP_AINFO_INHOBJDOWN,
		// request whether or not object slots are inheritable downwards
//		 
	NxpAtom_GAINFO_INHCLASSUP	= NXP_AINFO_INHCLASSUP,
		// request whether or not class slots are inheritable upwards
//		 
	NxpAtom_GAINFO_INHCLASSDOWN	= NXP_AINFO_INHCLASSDOWN,
		// request whether or not class slots are inheritable downwards
//		 
	NxpAtom_GAINFO_PWTRUE		= NXP_AINFO_PWTRUE,
		// request whether or not the context propagation is enabled on TRUE hypotheses
//		 
	NxpAtom_GAINFO_PWFALSE		= NXP_AINFO_PWFALSE,
		// request whether or not the context propagation is enabled on FALSE hypotheses
//		 
	NxpAtom_GAINFO_PWNOTKNOWN	= NXP_AINFO_PWNOTKNOWN,
		// request whether or not the context propagation is enabled on NOTKNOWN hypotheses
//		 
	NxpAtom_GAINFO_EXHBWRD		= NXP_AINFO_EXHBWRD,
		// request whether or not exhaustive backward chaining is enabled
//		 
	NxpAtom_GAINFO_PFACTIONS		= NXP_AINFO_PFACTIONS,
		// request whether or not the assignments done in the RHS of
		// rules or in methods are forwarded
//		 
	NxpAtom_GAINFO_SUGLIST		= NXP_AINFO_SUGLIST,
		// request the list of hypotheses kept in the suggest selection
//		 
	NxpAtom_GAINFO_VOLLIST		= NXP_AINFO_VOLLIST,
		// request the list of slots kept in the volunteer selection 
	NxpAtom_GAINFO_INFATOM		= NXP_AINFO_INFATOM,
		// request the inference priority atom attached to the atom 
	NxpAtom_GAINFO_INHATOM		= NXP_AINFO_INHATOM,
		// request the inheritance priority atom attached to the atom
//		 
	NxpAtom_GAINFO_CONTEXT		= NXP_AINFO_CONTEXT,
		// request the hypotheses that are in the context of a given hypothesis
//		 
	NxpAtom_GAINFO_KBID		= NXP_AINFO_KBID,
		// request the knowledge base to which the atom belongs 
	NxpAtom_GAINFO_SOURCESON		= NXP_AINFO_SOURCESON,
		// request whether or not Order of Sources are enabled 
	NxpAtom_GAINFO_CACTIONSON	= NXP_AINFO_CACTIONSON,
		// request whether of not If Change methods are enabled 
	NxpAtom_GAINFO_COMMENTS		= NXP_AINFO_COMMENTS,
		// request the comments attached to the atom 
	NxpAtom_GAINFO_FORMAT		= NXP_AINFO_FORMAT,
		// request the format information attached to the atom 
	NxpAtom_GAINFO_WHY		= NXP_AINFO_WHY,
		// request the Why information attach to the atom 
	NxpAtom_GAINFO_MOTSTATE		= NXP_AINFO_MOTSTATE,
		// request information about the current state of the inference engine
//		 
	NxpAtom_GAINFO_PTGATES		= NXP_AINFO_PTGATES,
		// request whether or not forward chaining through gate is enabled
//		 
	NxpAtom_GAINFO_CLIENTDATA	= NXP_AINFO_CLIENTDATA,
		// request the client or user information attached to an atom
//		 
	NxpAtom_GAINFO_VERSION		= NXP_AINFO_VERSION,
		// request the names and version number of the software components included in the package used
//		 
	NxpAtom_GAINFO_SELF		= NXP_AINFO_SELF,
		// request the name or atom of the current SELF atom 
	NxpAtom_GAINFO_PROCEXECUTE	= NXP_AINFO_PROCEXECUTE,
		// request the number of execute routines installed or the
		// name of the execute handler
//		 
	NxpAtom_GAINFO_FOCUSPRIO		= NXP_AINFO_FOCUSPRIO,
		// request the priority of the hypotheses on the agenda 
	NxpAtom_GAINFO_AGDVBREAK		= NXP_AINFO_AGDVBREAK,
		// request whether a specific hypothesis has an agenda break point set for it
//		 
	NxpAtom_GAINFO_INFBREAK		= NXP_AINFO_INFBREAK,
		// request whether a specific rule, condition, method, slot,
		// object, class, or property has an inference breakpoint set on it
//		 
	NxpAtom_GAINFO_BWRDLINKS		= NXP_AINFO_BWRDLINKS,
		// request the backward links from a hypothesis to its rules 
	NxpAtom_GAINFO_FWRDLINKS		= NXP_AINFO_FWRDLINKS,
		// request the forward links from a slot to the conditions 
	NxpAtom_GAINFO_KBNAME		= NXP_AINFO_KBNAME,
		// returns the name of a knowledge base, given its atom 
	NxpAtom_GAINFO_CURRENTKB		= NXP_AINFO_CURRENTKB,
		// request the current knowledge base containing the atom 
	NxpAtom_GAINFO_VALUELENGTH	= NXP_AINFO_VALUELENGTH,
		// request the length of a string slot value 
	NxpAtom_GAINFO_SOURCESCONTINUE	= NXP_AINFO_SOURCESCONTINUE,
		// request whether or not Order of Sources methods will be
		// fully executed even after a value is determined
//		 
	NxpAtom_GAINFO_CACTIONSUNKNOWN	= NXP_AINFO_CACTIONSUNKNOWN,
		// request whether or not If Change methods will also be
		// execute when the slot if set to UNKNOWN
//		 
	NxpAtom_GAINFO_VALIDUSER_OFF	= NXP_AINFO_VALIDUSER_OFF,
		// request whether or not the validation of values entered
		// by the end user is disabled
//		 
	NxpAtom_GAINFO_VALIDUSER_ACCEPT	= NXP_AINFO_VALIDUSER_ACCEPT,
		// request whether or not the validation of values entered by
		// the end user is enabled and the value accepted
		// automatically if the validation expression is incomplete
//		 
	NxpAtom_GAINFO_VALIDUSER_REJECT	= NXP_AINFO_VALIDUSER_REJECT,
		// request whether or not the validation of values entered by
		// the end user is enabled and the value rejected
		// automatically if the validation expression is incomplete
//		 
	NxpAtom_GAINFO_VALIDENGINE_OFF	= NXP_AINFO_VALIDENGINE_OFF,
		// request whether or not the validation of values set by the
		// engine is enabled
//		 
	NxpAtom_GAINFO_VALIDENGINE_ACCEPT= NXP_AINFO_VALIDENGINE_ACCEPT,
		// request whether or not the validation of values set by
		// the engine is enabled and the value accepted
		// automatically if the validation expression is incomplete
//		 
	NxpAtom_GAINFO_VALIDENGINE_REJECT= NXP_AINFO_VALIDENGINE_REJECT,
		// request whether or not the validation of values set by
		// the engine is enabled and the value rejected
		// automatically if the validation expression is incomplete
//		 
	NxpAtom_GAINFO_VALIDUSER_ON	= NXP_AINFO_VALIDUSER_ON,
		// request whether or not the validation of values entered
		// by the end user is enabled
//		 
	NxpAtom_GAINFO_VALIDENGINE_ON	= NXP_AINFO_VALIDENGINE_ON,
		// request whether or not the validation of values set by
		// the engine is enabled
//		 
	NxpAtom_GAINFO_EHS		= NXP_AINFO_EHS,
		// request information about the Else actions (Right-Hand-Side)
		// of a rule or method
//		 
	NxpAtom_GAINFO_PFELSEACTIONS	= NXP_AINFO_PFELSEACTIONS,
		// request the value to which the forward Else actions
		// strategy is set
//		 
	NxpAtom_GAINFO_PFMETHODACTIONS	= NXP_AINFO_PFMETHODACTIONS,
		// request the value to which the forward LHS/RHS actions
		// from methods strategy is set
//		 
	NxpAtom_GAINFO_PFMETHODELSEACTIONS= NXP_AINFO_PFMETHODELSEACTIONS,
		// request the value to which the forward Else actions
		// from methods strategy is set
//		 
	NxpAtom_GAINFO_VALIDFUNC		= NXP_AINFO_VALIDFUNC,
		// request the validation expression string attached to the
		// atom
//		 
	NxpAtom_GAINFO_VALIDEXEC		= NXP_AINFO_VALIDEXEC,
		// request the validation external routine name attached to
		// the atom
//		 
	NxpAtom_GAINFO_VALIDHELP		= NXP_AINFO_VALIDHELP,
		// request the validation ошибка string attached to the atom 
	NxpAtom_GAINFO_QUESTWIN		= NXP_AINFO_QUESTWIN,
		// request the question window name attached to the atom
		// (this may be used by the user to trigger different question
		// windows)
//		 
	NxpAtom_GAINFO_METHODS		= NXP_AINFO_METHODS,
		// request the list of methods attached to the atom 
	NxpAtom_GAINFO_PROPPRIVATE	= NXP_AINFO_PROPPRIVATE,
		// request whether or not a slot is private 
	NxpAtom_GAINFO_PROPPUBLIC	= NXP_AINFO_PROPPUBLIC,
		// request whether or not a slot is public 

	NxpAtom_GAINFO_LAST_TAG,
	NxpAtom_GAINFO_LAST = NxpAtom_GAINFO_LAST_TAG - 1

};
enum _NxAtomDescEnum {
	NxpAtom_DESC_FIRST,
	NxpAtom_DESC_FIRST_TAG = NxpAtom_DESC_FIRST - 1,

	NxpAtom_DESC_UNKNOWN	= NXP_DESC_UNKNOWN,		// Вид - Unknown 
	NxpAtom_DESC_NOTKNOWN	= NXP_DESC_NOTKNOWN,	// Вид - Notknown 
	NxpAtom_DESC_INT		= NXP_DESC_INT,					// Вид - Integer 
	NxpAtom_DESC_FLOAT	= NXP_DESC_FLOAT,				// Вид - Float 
	NxpAtom_DESC_DOUBLE	= NXP_DESC_DOUBLE,			// Вид - Double 
	NxpAtom_DESC_STR		= NXP_DESC_STR,					// Вид - String 
	NxpAtom_DESC_ATOM	= NXP_DESC_ATOM,					// Вид - Atom 
	NxpAtom_DESC_VALUE	= NXP_DESC_VALUE,				// Вид - Value 
	NxpAtom_DESC_LONG	= NXP_DESC_LONG,					// Вид - Long 
	NxpAtom_DESC_DATE	= NXP_DESC_DATE,					// Вид - Data 
	NxpAtom_DESC_TIME	= NXP_DESC_TIME,					// Вид - Time 

	NxpAtom_DESC_LAST_TAG,
	NxpAtom_DESC_LAST = NxpAtom_DESC_LAST_TAG - 1
};
typedef class NxpAtom NxAtomRec;
typedef const class NxpAtom NxAtomCRec;
class AFX_EXT_CLASS NxpAtom:public CObject  
{
public:
	NxpAtom();
	virtual~NxpAtom();
	static NxAtomPtr Find(CStr name,NxAtomTypeEnum code);
	static int SetInfo(NxAtomPtr atom1,NxAtomSAInfoEnum code,NxAtomPtr atom2,int optInt);
	static int GetIntInfo(NxAtomPtr atom1,NxAtomGAInfoEnum code,NxAtomPtr atom2,int optInt);
	static Long GetLongInfo(NxAtomPtr atom1,NxAtomGAInfoEnum code,NxAtomPtr atom2,int optInt);
	static Double GetDoubleInfo(NxAtomPtr atom1,NxAtomGAInfoEnum code,NxAtomPtr atom2,int optInt);
	static Str GetStrInfo(NxAtomPtr atom1,NxAtomGAInfoEnum code,NxAtomPtr atom2,int optInt,int len);
	static NxAtomPtr GetAtomInfo(NxAtomPtr atom1,NxAtomGAInfoEnum code,NxAtomPtr atom2,int optInt);
	NxAtomTypeEnum GetType(void);
	Str GetName(void);
	Long GetClientData(void);
	void SetClientData(Long data);
//	static void far*operator new(size_t size);
//	static void far*operator new(size_t size,void far*ptr);
//	static void operator delete(void far *ptr);

	virtual void Serialize(CArchive&ar);
#ifdef _DEBUG    
  void AssertValid()const;
	//virtual void Dump(CDumpContext&dc)const;
#endif
protected:
	DECLARE_SERIAL(NxpAtom)
};
#endif // !defined(AFX_NXPATOM_H__E0112941_D604_11D2_862A_00A024384FCF__INCLUDED_)
