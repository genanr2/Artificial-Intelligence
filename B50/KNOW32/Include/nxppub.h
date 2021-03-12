//  nxppub.h
//  Elements Environment 2.0
//  Origin: nxppub.h
//  Copyright (C) 1985-1996 by Neuron Data Incorporated.
//  All Rights Reserved.
#ifndef _NXPPUB_
#define _NXPPUB_

#ifndef	_STRPUB_
#	include "strpub.h"
#endif
/*
#ifndef	_ARRAYPUB_
#	include "arraypub.h"
#endif
*/

typedef	struct	_AtomRec*AtomId;
typedef	AtomId far*AtomIdPtr;
typedef	struct	_KBRec*KBId;
typedef	KBId  far*KBIdPtr;
/*
// because of possible nd.h collision ... 
#ifndef _NXPIPROC_
	typedef	int(far*NxpIProc)();
#	define _NXPIPROC_
#endif
typedef	NxpIProc far*NxpIProcPtr;
typedef	void(far*NXDBNameProc)L((ArrayPtr));
typedef	NXDBNameProc far*NXDBNameProcPtr;
*/
//------------------------------------------------------------------------
//	Error Codes
//------------------------------------------------------------------------
#define	NXP_ERR_NOERR				0
#define	NXP_ERR_INVARG1			1
#define	NXP_ERR_INVARG2			2
#define	NXP_ERR_INVARG3			3
#define	NXP_ERR_INVARG4			4
#define	NXP_ERR_INVARG5			5
#define	NXP_ERR_INVARG6			6
#define	NXP_ERR_NOTFOUND		7
#define	NXP_ERR_INVATOM			8
#define	NXP_ERR_UNKNOWN			9
#define	NXP_ERR_NOTKNOWN		10
#define	NXP_ERR_INVSTATE		11
#define	NXP_ERR_MATHERROR		12
#define	NXP_ERR_NOTAVAIL		13
#define	NXP_ERR_COMPILEPB		14
#define	NXP_ERR_PROTPB			15
#define NXP_ERR_FILEOPEN		16
#define NXP_ERR_FILEEOF			17
#define NXP_ERR_FILEREAD		18
#define NXP_ERR_FILEWRITE		19
#define NXP_ERR_FILESEEK		20
#define NXP_ERR_SYNCERROR		21
#define NXP_ERR_FORMATERROR	22
#define NXP_ERR_NOMEMORY		23
#define NXP_ERR_ABORT				24
#define NXP_ERR_SYNTAX			25
#define NXP_ERR_INTERNAL		26
#define	NXP_ERR_NOSERVER		27
#define	NXP_ERR_VOLINVAL		28
#define	NXP_ERR_SAVEDISABLED			29
#define	NXP_ERR_VALIDATEERROR			30
#define	NXP_ERR_VALIDATEMISSING		31
#define	NXP_ERR_VALIDATEUSER			32
#define NXP_ERR_SUBSYSTEMNOTFOUND	33
#define	NXP_ERR_INVARG7						34
//------------------------------------------------------------------------
//	Errors related to compiling with the nxpedit API.
//------------------------------------------------------------------------
#define NXP_ERR_DEPENDENCIES	35
#define NXP_ERR_NULLEDPTR			36
#define NXP_ERR_NULLDATA			37
#define NXP_ERR_DATANSYNC			38
#define NXP_ERR_INVALIDID			39
#define NXP_ERR_INVALIDVSTR		40
#define NXP_ERR_MISSINGREQD		41
#define NXP_ERR_NOATOMTYPE		42

#define	NXP_CPLERR_NOERR				0x0000
#define NXP_CPLERR_MASK					0x00ff
#define	NXP_CPLERR_NOMEM				0x0100
#define	NXP_CPLERR_CANCEL				0x0200
#define	NXP_CPLERR_INVSYNTAXLOW	0x0400
#define NXP_CPLERR_INVSYNTAXUP	0x0800
#define	NXP_CPLERR_NEVER				0x1000

#define	NXP_CPLERR_TYPEERROR					0x0001
#define NXP_CPLERR_MODIFYDEFAULTNPROP	0x0002
#define NXP_CPLERR_DELETEDEFAULTNPROP	0x0003
#define NXP_CPLERR_PROPHASNONAME			0x0004
#define NXP_CPLERR_PROPEXISTS					0x0005
#define NXP_CPLERR_ERRPROPISSPECIAL		0x0006
#define NXP_CPLERR_ERRPROPISPRIVATE		0x0007
#define NXP_CPLERR_MODIFYSYSMETHOD		0x0008
#define NXP_CPLERR_DELETESYSMETHOD		0x0009
#define	NXP_CPLERR_SLOTHASNONAME			0x000a
#define	NXP_CPLERR_ERRINVMETANAME			0x000b
#define	NXP_CPLERR_ERRMETAEXISTS			0x000c
#define	NXP_CPLERR_INVNAME						0x000d
#define NXP_CPLERR_BADRULE		0x000e
#define	NXP_CPLERR_DEFINVTYPE		0x000f
#define NXP_CPLERR_GLOBALS		0x0010
#define	NXP_CPLERR_INVATOMNATURE	0x0011
#define	NXP_CPLERR_CLASSEXISTSASOBJ	0x0012
#define	NXP_CPLERR_OBJEXISTSASCLASS	0x0013
#define	NXP_CPLERR_BADMETHOD		0x0014
#define	NXP_CPLERR_BADMETHODATTACHATOM	0x0015
#define	NXP_CPLERR_ERRINVHYP		0x0016
#define	NXP_CPLERR_ERRINVNUMBATOM	0x0017
#define	NXP_CPLERR_ERRINVINFNUM		0x0018
#define	NXP_CPLERR_WARMISSINGVBAR	0x0019
#define	NXP_CPLERR_ERRMISSINGDQUOTE 	0x001a
#define	NXP_CPLERR_ERRMISSINGPMEND	0x001b
#define	NXP_CPLERR_ERRPMONVALUE		0x001c
#define	NXP_CPLERR_ERRNOMEM		0x001d
#define	NXP_CPLERR_ERRINVSYNTAXUP	0x001e
#define	NXP_CPLERR_ERRNEVER		0x001f
#define	NXP_CPLERR_ERRINVCNTX		0x0020
#define	NXP_CPLERR_ERRCNTXLSTEMPTY 	0x0021
#define	NXP_CPLERR_ERRCNTXEXISTS	0x0022
#define NXP_CPLERR_ERRPARSE		0x0023
#define NXP_CPLERR_ERRINVFUNC		0x0024
#define	NXP_CPLERR_RENCLOBJEXISTS	0x0025
#define	NXP_CPLERR_RENPROPEXISTS	0x0026
#define	NXP_CPLERR_RENAYSCLOBJ		0x0027
#define	NXP_CPLERR_RENAYSPROP		0x0028
#define	NXP_CPLERR_RENINVNAME		0x0029
#define	NXP_CPLERR_ERRXPCTCLASS		0x002a
#define	NXP_CPLERR_ERRXPCTOBJECT	0x002b
#define	NXP_CPLERR_ERRNONAME		0x002c
#define	NXP_CPLERR_ERRATOMEXISTS	0x002d
#define NXP_CPLERR_ERRFCTTYPECLASH	0x002e
#define NXP_CPLERR_ERRARGMISSING	0x002f
#define NXP_CPLERR_ERRARGTOOMANY	0x0030
#define NXP_CPLERR_ERRINVINTER		0x0031
#define NXP_CPLERR_ERRINVALLPMATCH	0x0032
#define NXP_CPLERR_ERRFCTLISTMATCH	0x0033
#define NXP_CPLERR_ERRDBLISTPB		0x0034
#define NXP_CPLERR_ERRDBSEQADD		0x0035
#define NXP_CPLERR_ERRDBSEQWRITE	0x0036
#define NXP_CPLERR_ERRDBSLOTSPROPS	0x0037
#define	NXP_CPLERR_ERRINVRULENAME	0x0038
#define	NXP_CPLERR_ERRNOLHS		0x0039
#define	NXP_CPLERR_ERRNONUNIQRULE	0x003a
#define	NXP_CPLERR_ERRNULLRULENAME	0x003b
#define	NXP_CPLERR_ERRNOTYPE		0x003c
#define	NXP_CPLERR_ERRNONATURE		0x003d
#define	NXP_CPLERR_ERRNOLOCALARGNAME	0x003e
#define	NXP_CPLERR_ERRDBNOTYPE		0x003f
#define NXP_CPLERR_ERRINVBOOLVALUE	0x0040
#define	NXP_CPLERR_ERRNOTEXISTS		0x0041
#define NXP_CPLERR_ERRNOPROPTYPE	0x0042
#define	NXP_CPLERR_BADVALIDFCN		0x0043
#define	NXP_CPLERR_ERRORPARSE		0x0044
#define	NXP_CPLERR_KBASEUNIT		0x0045
#define	NXP_CPLERR_GLOBIT		0x0046
#define	NXP_CPLERR_FORMAT		0x0047
#define	NXP_CPLERR_PROPDESC		0x0048
#define	NXP_CPLERR_PROPTYPE		0x0049
#define	NXP_CPLERR_KW_TYPE		0x004a
#define	NXP_CPLERR_CLASS		0x004b
#define	NXP_CPLERR_CLASSIT		0x004c
#define	NXP_CPLERR_OBJECT		0x004d
#define	NXP_CPLERR_OBJIT		0x004e
#define	NXP_CPLERR_METAIT		0x004f
#define	NXP_CPLERR_METAITEM		0x0050
#define	NXP_CPLERR_RULEIT		0x0051
#define	NXP_CPLERR_RULEITEM		0x0052
#define	NXP_CPLERR_LHS			0x0053
#define	NXP_CPLERR_RHS			0x0054
#define	NXP_CPLERR_OS			0x0055
#define	NXP_CPLERR_IC			0x0056
#define	NXP_CPLERR_LPAR			0x0057
#define	NXP_CPLERR_GENSLOTARG1		0x0058
#define	NXP_CPLERR_EMPTYARG2		0x0059
#define	NXP_CPLERR_ISSTRLSTCARG2	0x005a
#define	NXP_CPLERR_GENCLOBJARG1		0x005b
#define	NXP_CPLERR_GENCLOBJLSTCARG2	0x005c
#define	NXP_CPLERR_EXPRARG1		0x005d
#define	NXP_CPLERR_GENSLOTARG2		0x005e
#define	NXP_CPLERR_QSTRHDLARG1		0x005f
#define	NXP_CPLERR_SHOWARG2		0x0060
#define	NXP_CPLERR_EXECUTEARG2		0x0061
#define	NXP_CPLERR_RETRIEVEARG2		0x0062
#define	NXP_CPLERR_BOOLEXPRARG1		0x0063
#define	NXP_CPLERR_DYNOBJARG2		0x0064
#define	NXP_CPLERR_STRATARG1		0x0065
#define	NXP_CPLERR_XLOADARG2		0x0066
#define	NXP_CPLERR_DEFAULTARG		0x0067
#define	NXP_CPLERR_TRUEFALSEARG		0x0068
#define	NXP_CPLERR_KNOWNVALARG		0x0069
#define	NXP_CPLERR_RECTLSTCARG		0x006a
#define	NXP_CPLERR_IDENTARG		0x006b
#define	NXP_CPLERR_GENATOMLSTCARG	0x006c
#define	NXP_CPLERR_QSTRHDLARG		0x006d
#define	NXP_CPLERR_GENCLOBJLSTCARG	0x006e
#define	NXP_CPLERR_QSTRLSTCARG		0x006f
#define	NXP_CPLERR_PROPLSTCARG		0x0070
#define	NXP_CPLERR_GENSLOTLSTCARG	0x0071
#define	NXP_CPLERR_SLOTARG		0x0072
#define	NXP_CPLERR_SLOTLSTCARG		0x0073
#define	NXP_CPLERR_STRATENUMARG		0x0074
#define	NXP_CPLERR_MSGTOARG		0x0075
#define	NXP_CPLERR_METHODITEM		0x0076
#define	NXP_CPLERR_STRATENUM		0x0077
#define	NXP_CPLERR_MSGARGARG		0x0078
#define	NXP_CPLERR_NOTLOCALARG		0x0079
#define	NXP_CPLERR_CANNOTCOMPARE	0x007a
#define	NXP_CPLERR_CANNOTMIXPMS		0x007b
#define	NXP_CPLERR_LOCALARGSYNTAXERROR	0x007c
#define NXP_CPLERR_YYERROR		0x007d
#define NXP_CPLERR_LEXOVERFLOW		0x007e
#define	NXP_CPLERR_VERSION		0x007f

//------------------------------------------------------------------------
//	Reference Codes for NXPEDIT Dependencies
//------------------------------------------------------------------------

#define	NXP_CPL_USEDWITHPM		0x1000
#define	NXP_CPL_USEDASHYPO		0x0100
#define	NXP_CPL_USEDASDATA		0x0200
#define	NXP_CPL_USEDASINFATOM		0x0300
#define	NXP_CPL_USEDASINHATOM		0x0400
#define	NXP_CPL_USEDASCONTEXT		0x0500
#define	NXP_CPL_USEDINMEMBEROS		0x0600
#define	NXP_CPL_USEDINOWNOS		0x0700
#define	NXP_CPL_USEDINMEMBERICA		0x0800
#define	NXP_CPL_USEDINOWNICA		0x0900
#define	NXP_CPL_USEDHASMETASLOT		0x0A00
#define NXP_CPL_USEDINMETHODXHS      	0x0B00
#define NXP_CPL_USEDINVALIDATION	0x0C00
#define NXP_CPL_USEDINMETHODTO       	0x0D00
#define NXP_CPL_USEDINSLOT       	0x0E00

//------------------------------------------------------------------------
//	Descriptors describing the data type of an argument
//------------------------------------------------------------------------

#define	NXP_DESC_UNKNOWN		0
#define	NXP_DESC_NOTKNOWN		1
#define	NXP_DESC_INT			2
#define	NXP_DESC_FLOAT			3
#define	NXP_DESC_DOUBLE			4
#define	NXP_DESC_STR			5
#define	NXP_DESC_ATOM			6
#define NXP_DESC_VALUE			7
#define NXP_DESC_LONG			8
#define NXP_DESC_DATE			9
#define NXP_DESC_TIME			10

/*
#if MCH_OS == MCH_OSCMS
#define NXP_DESC_MEMPTR 		11
#define NXP_DESC_DATABASE 		12
#endif
*/
//------------------------------------------------------------------------
//	NXP_GetAtomInfo Information codes
//------------------------------------------------------------------------
#define	NXP_AINFO_PUBLIC		0x0000
#define	NXP_AINFO_PRIVATE		0x0100
#define	NXP_AINFO_CURSTRAT		0x0200
#define	NXP_AINFO_MLHS			0x0400
// "0" For backward compatibility 
#define	NXP_AINFO_MRHS			0x0000
#define	NXP_AINFO_MEHS			0x0800

#define NXP_AINFO_MASK			0x00ff
#define	NXP_AINFO_NAME			0x0000
#define	NXP_AINFO_TYPE			0x0001
#define	NXP_AINFO_VALUETYPE		0x0002
#define	NXP_AINFO_VALUE			0x0003
#define	NXP_AINFO_NEXT			0x0004
#define	NXP_AINFO_PREV			0x0005
#define	NXP_AINFO_PARENTOBJECT		0x0006
#define	NXP_AINFO_CHILDOBJECT		0x0007
#define	NXP_AINFO_PARENTCLASS		0x0008
#define	NXP_AINFO_CHILDCLASS		0x0009
#define	NXP_AINFO_SLOT			0x000A
#define	NXP_AINFO_LINKED		0x000B
#define	NXP_AINFO_CHOICE		0x000C
#define	NXP_AINFO_PARENT		0x000D
#define	NXP_AINFO_SUGGEST		0x000E
#define	NXP_AINFO_CURRENT		0x000F
#define	NXP_AINFO_HYPO			0x0010
#define	NXP_AINFO_LHS			0x0011
#define	NXP_AINFO_RHS			0x0012
#define	NXP_AINFO_CACTIONS		0x0013
#define	NXP_AINFO_SOURCES		0x0014
#define	NXP_AINFO_PROP			0x0015
#define NXP_AINFO_HASMETA		0x0016
#define NXP_AINFO_INFCAT		0x0017
#define	NXP_AINFO_INHCAT		0x0018
#define	NXP_AINFO_INHDEFAULT		0x0019
#define	NXP_AINFO_INHUP			0x001a
#define	NXP_AINFO_INHDOWN		0x001b
#define	NXP_AINFO_INHVALDEFAULT		0x001c
#define	NXP_AINFO_INHVALUP		0x001d
#define	NXP_AINFO_INHVALDOWN		0x001e
#define	NXP_AINFO_DEFAULTFIRST		0x001f
#define	NXP_AINFO_PARENTFIRST		0x0020
#define	NXP_AINFO_BREADTHFIRST		0x0021
#define	NXP_AINFO_PROMPTLINE		0x0022
#define	NXP_AINFO_DEFVAL		0x0023
#define	NXP_AINFO_INHOBJUP		0x0024
#define	NXP_AINFO_INHOBJDOWN		0x0025
#define	NXP_AINFO_INHCLASSUP		0x0026
#define	NXP_AINFO_INHCLASSDOWN		0x0027
#define	NXP_AINFO_PWTRUE		0x0028
#define	NXP_AINFO_PWFALSE		0x0029
#define	NXP_AINFO_PWNOTKNOWN		0x002a
#define	NXP_AINFO_EXHBWRD		0x002b
#define	NXP_AINFO_PFACTIONS		0x002c
#define NXP_AINFO_SUGLIST		0x002d
#define NXP_AINFO_VOLLIST		0x002e
#define NXP_AINFO_INFATOM		0x002f
#define NXP_AINFO_INHATOM		0x0030
#define NXP_AINFO_CONTEXT		0x0031
#define NXP_AINFO_KBID			0x0032
#define NXP_AINFO_SOURCESON		0x0033
#define NXP_AINFO_CACTIONSON		0x0034
#define NXP_AINFO_COMMENTS		0x0035
#define NXP_AINFO_FORMAT		0x0036
#define NXP_AINFO_WHY			0x0037
#define NXP_AINFO_MOTSTATE		0x0038
#define NXP_AINFO_PTGATES		0x0039
#define NXP_AINFO_CLIENTDATA		0x003a
#define	NXP_AINFO_VERSION		0x003b
#define	NXP_AINFO_SELF			0x003c
#define	NXP_AINFO_PROCEXECUTE		0x003d
#define	NXP_AINFO_FOCUSPRIO		0x003e
#define	NXP_AINFO_AGDVBREAK		0x003f
#define	NXP_AINFO_INFBREAK		0x0040
#define	NXP_AINFO_BWRDLINKS		0x0041
#define	NXP_AINFO_FWRDLINKS		0x0042
#define	NXP_AINFO_KBNAME		0x0043
#define	NXP_AINFO_CURRENTKB		0x0044
#define	NXP_AINFO_VALUELENGTH		0x0045
#define NXP_AINFO_SOURCESCONTINUE	0x0046
#define NXP_AINFO_CACTIONSUNKNOWN	0x0047
#define NXP_AINFO_VALIDUSER_OFF		0x0048
#define NXP_AINFO_VALIDUSER_ACCEPT	0x0049
#define NXP_AINFO_VALIDUSER_REJECT	0x004a
#define NXP_AINFO_VALIDUSER_ASK		0x004b
#define NXP_AINFO_VALIDENGINE_OFF	0x004c
#define NXP_AINFO_VALIDENGINE_ACCEPT	0x004d
#define NXP_AINFO_VALIDENGINE_REJECT	0x004e
#define NXP_AINFO_VALIDENGINE_ASK	0x004f
#define NXP_AINFO_VALIDUSER_ON		0x0050
#define NXP_AINFO_VALIDENGINE_ON	0x0051
#define	NXP_AINFO_EHS			0x0052

// not yet available 
#define NXP_AINFO_ALLOWQUESTIONS	0x0053

// not yet available 
#define NXP_AINFO_HYPOFORWARD		0x0054

#define	NXP_AINFO_PFELSEACTIONS		0x0055
#define	NXP_AINFO_PFMETHODACTIONS	0x0056
#define	NXP_AINFO_PFMETHODELSEACTIONS	0x0057
#define	NXP_AINFO_VALIDFUNC		0x0058
#define	NXP_AINFO_VALIDEXEC		0x0059
#define	NXP_AINFO_VALIDHELP		0x005a
#define	NXP_AINFO_QUESTWIN		0x005b
#define	NXP_AINFO_METHODS		0x005c
#define NXP_AINFO_PROPPRIVATE		0x005d
#define NXP_AINFO_PROPPUBLIC		0x005e
#define NXP_AINFO_KBSTATE               0x005f
#define NXP_AINFO_XREF                  0x0060
#define NXP_AINFO_XREFEND               0x0061
#define NXP_AINFO_PRIVATEINITVAL	0x0062
#define NXP_AINFO_PUBLICINITVAL		0x0063
#define	NXP_AINFO_ATTACHEDTO		0x0064
#define	NXP_AINFO_ARGLIST		0x0065
#define NXP_AINFO_METHODFLAGS		0x0066

/*
#if MCH_OS == MCH_OSCMS
//------------------------------------------------------------------------
//       KB State Flags NXP_GetAtomInfo / NXP_AINFO_KBSTATE
//------------------------------------------------------------------------
#define NXP_KBSTATE_DISABLEWEAK         0x0002
#define NXP_KBSTATE_DISABLESTRONG       0x0004
#define NXP_KBSTATE_ENABLED             0x0008
#define NXP_KBSTATE_MODIFIED            0x0010
#define NXP_KBSTATE_WILLSAVE            0x0020
#endif // CMS 
*/
//------------------------------------------------------------------------
//	Atom types - NXP_GetAtomInfo / NXP_AINFO_TYPE
//------------------------------------------------------------------------
#define	NXP_ATYPE_DATA			0x0100
#define	NXP_ATYPE_HYPO			0x0200
#define	NXP_ATYPE_PERM			0x0000
#define	NXP_ATYPE_TEMP			0x1000

#define	NXP_ATYPE_MASK			0x000F
#define	NXP_ATYPE_NONE			0x0000
#define	NXP_ATYPE_CLASS			0x0001
#define	NXP_ATYPE_OBJECT		0x0002
#define	NXP_ATYPE_PROP			0x0003
#define	NXP_ATYPE_SLOT			0x0004
#define	NXP_ATYPE_RULE			0x0005
#define	NXP_ATYPE_LHS			0x0006
#define	NXP_ATYPE_RHS			0x0007
#define	NXP_ATYPE_CACTIONS		0x0008
#define	NXP_ATYPE_SOURCES		0x0009
#define	NXP_ATYPE_KB			0x000A
#define	NXP_ATYPE_EHS			0x000B
#define	NXP_ATYPE_METHOD		0x000C
#define NXP_ATYPE_CONTEXT		0x000D

//------------------------------------------------------------------------
//	Special codes for status of Boolean Atoms
//	(NXP_GetAtomInfo / NXP_AINFO_VALUE)
//------------------------------------------------------------------------

#define	NXP_BOOL_UNKNOWN		-2
#define	NXP_BOOL_NOTKNOWN		-1
#define	NXP_BOOL_FALSE			0
#define	NXP_BOOL_TRUE			1

//------------------------------------------------------------------------
//	Data type codes - NXP_GetAtomInfo / NXP_AINFO_VALUETYPE
//------------------------------------------------------------------------

#define	NXP_VTYPE_BOOL			0
// keep this one for compatibility 
#define	NXP_VTYPE_NUMB			1
#define	NXP_VTYPE_DOUBLE		1
#define	NXP_VTYPE_STR			2
#define	NXP_VTYPE_SPECIAL		3
#define	NXP_VTYPE_DATE			4
#define	NXP_VTYPE_LONG			5
#define	NXP_VTYPE_TIME			6

//------------------------------------------------------------------------
//	Codes for NXP_GetAtomInfo / NXP_AINFO_LINKED
//------------------------------------------------------------------------

#define	NXP_LINK_NOLINK			0
#define	NXP_LINK_TEMPLINK		1
#define	NXP_LINK_PERMLINK		2
#define	NXP_LINK_TEMPUNLINK		3

//------------------------------------------------------------------------
//	Inference engine state - NXP_GetAtomInfo / NXP_AINFO_MOTSTATE
//------------------------------------------------------------------------
/*
#if MCH_OS == MCH_OSCMS
#define NXP_STATE_NOTINIT		0
#endif
*/
#define NXP_STATE_DONE			1
#define NXP_STATE_RUNNING		2
#define NXP_STATE_STOPPED		3
#define NXP_STATE_QUESTION		4
//------------------------------------------------------------------------
//	Suggest priorities - NXP_Suggest
//------------------------------------------------------------------------
#define	NXP_SPRIO_UNSUG			-1
#define	NXP_SPRIO_FORCE			0
#define	NXP_SPRIO_SUG			1
#define	NXP_SPRIO_HYPISL		2
#define	NXP_SPRIO_DATAISL		3
#define	NXP_SPRIO_CNTX			4

//------------------------------------------------------------------------
//	Volunteer strategies - NXP_Volunteer
//------------------------------------------------------------------------
#define	NXP_VSTRAT_SETMASK	0x0F00
#define NXP_VSTRAT_QUEUE		0x0100
#define	NXP_VSTRAT_SET			0x0200
#define	NXP_VSTRAT_SETQUEUE	(NXP_VSTRAT_QUEUE|NXP_VSTRAT_SET)
#define NXP_VSTRAT_NOCHECK  0x0201

#define	NXP_VSTRAT_FWRDMASK	0x000F
#define	NXP_VSTRAT_NOFWRD		0x0000
#define	NXP_VSTRAT_VOLFWRD	0x0001
#define	NXP_VSTRAT_RHSFWRD	0x0002
#define	NXP_VSTRAT_CURFWRD	0x0003
#define	NXP_VSTRAT_QFWRD		0x0004
#define	NXP_VSTRAT_RESET		0x0005

//------------------------------------------------------------------------
//	Forward Action effect settings
//------------------------------------------------------------------------
#define	NXP_FSTRAT_OFF		0x000
#define NXP_FSTRAT_ON			0x001
#define	NXP_FSTRAT_GLOBAL	0x002
//------------------------------------------------------------------------
//	Knowledge base levels - NXP_UnloadKB
//------------------------------------------------------------------------
#define	NXP_XLOAD_ENABLE				0x0000
#define	NXP_XLOAD_DISABLEWEAK		0x0001
#define	NXP_XLOAD_DISABLESTRONG	0x0002
#define	NXP_XLOAD_DELETE				0x0003
#define	NXP_XLOAD_WIPEOUT				0x0004
//------------------------------------------------------------------------
//	Control codes - NXP_Control
//------------------------------------------------------------------------

#define	NXP_CTRL_MASK			0x00FF

// test for possible nd.h collisions ... 

#ifndef NXP_CTRL_INIT
#	define	NXP_CTRL_INIT		0
#endif
#define	NXP_CTRL_KNOWCESS		1
#define	NXP_CTRL_STOPSESSION		2
#define	NXP_CTRL_CONTINUE		3
#define	NXP_CTRL_RESTART		4
#define	NXP_CTRL_CLEARKB		5
#ifndef NXP_CTRL_EXIT
#	define	NXP_CTRL_EXIT		6
#endif
#define	NXP_CTRL_SETSTOP		7
#define	NXP_CTRL_SAVESTRAT		8

/*
#if MCH_OS == MCH_OSCMS
#define  NXP_CTRL_TOGGLETRAN		9
#endif
*/
#define	NXP_CTRL_ATTOP			0x0000
#define	NXP_CTRL_ATBOTTOM		0x0100
#define	NXP_CTRL_AFTERCURRENT		0x0200
//------------------------------------------------------------------------
//	Handler codes - NXP_SetHandler
//------------------------------------------------------------------------
#define NXP_PROC_EXECUTE		0
// possible collision with nd.h 
#ifndef NXP_PROC_POLLING
#	define NXP_PROC_POLLING		1
#endif

#define NXP_PROC_QUESTION		2
#define NXP_PROC_ALERT			3
#define NXP_PROC_APROPOS		4
#define NXP_PROC_NOTIFY			5
#define NXP_PROC_GETSTATUS	6
#define NXP_PROC_SETDATA		7
#define NXP_PROC_GETDATA		8
#define	NXP_PROC_FORMINPUT	9
#define	NXP_PROC_CANCEL			10
#define	NXP_PROC_ENCRYPT		11
#define	NXP_PROC_DECRYPT		12
#define	NXP_PROC_PASSWORD		13
#define	NXP_PROC_MEMEXIT		14
#define	NXP_PROC_ENDOFSESSION		15
#define	NXP_PROC_VOLVALIDATE		16
#define	NXP_PROC_QUIT						17
#define	NXP_PROC_VALIDATE				18

//------------------------------------------------------------------------
//	Database Handlers
//------------------------------------------------------------------------
#define NXP_PROC_DBCONNECT		100
#define NXP_PROC_DBCLOSE			101
#define NXP_PROC_DBEXECUTE		102
#define NXP_PROC_DBBEGINREAD	103
#define NXP_PROC_DBGETREC			104
#define NXP_PROC_DBGETFIELD		105
#define NXP_PROC_DBBEGINUPDATE		106
#define NXP_PROC_DBPUTREC		107
#define NXP_PROC_DBBEGININSERT		108
#define NXP_PROC_DBADDREC		109
#define NXP_PROC_DBENDQUERY		110

//------------------------------------------------------------------------
//	Handler Types
//------------------------------------------------------------------------
// 0x0000 to 0x00FF is reserved for ND's internal usage 
#define	NXP_HDLTYPE_DEFAULT		0x0000
#define	NXP_HDLTYPE_SETHANDLER		0x0001
#define	NXP_HDLTYPE_EXECUTELIB		0x0002
#define	NXP_HDLTYPE_CLIENTSERVER	0x0003
#define	NXP_HDLTYPE_DATABASE		0x0004
#define	NXP_HDLTYPE_DEVELOPMENT		0x0005
#define	NXP_HDLTYPE_NXPFORMS		0x0006
#define	NXP_HDLTYPE_VMSSETHANDLER	0x0007
#define	NXP_HDLTYPE_VMSSETHANDLER2	0x0008
#define	NXP_HDLTYPE_NXPW		0x0009

#define	NXP_HDLTYPE_USER		0x0100

/*
#if MCH_OS == MCH_OSCMS
//------------------------------------------------------------------------
//      Handler Languages - NXP_Handler                                
//------------------------------------------------------------------------
#define NXP_LANG_SASC      1
#define NXP_LANG_PL1       2
#define NXP_LANG_COBOL     3
#define NXP_LANG_REXX      4
#define NXP_LANG_NXP       5       // internal Use ONLY               
#define NXP_LANG_FORTRAN   6
#define NXP_LANG_ASM       7
#define NXP_LANG_IBMC      9

//------------------------------------------------------------------------
//      Handler flags - NXP_SetHandler                                 
//------------------------------------------------------------------------
#define NXP_FLAG_DEFAULT   0x01    // internal Use Only                
#define NXP_FLAG_LOADED    0x02    // Handler must be loaded by name   
#endif // CMS 
*/
//------------------------------------------------------------------------
//	NXP_SetAtomInfo - information codes
//------------------------------------------------------------------------

#define	NXP_SAINFO_CURRENTKB		0x0001
#define	NXP_SAINFO_PERMLINK		0x0002
#define	NXP_SAINFO_MERGEKB		0x0003
#define	NXP_SAINFO_INKB			0x0004
#define	NXP_SAINFO_PERMLINKKB		0x0005
#define	NXP_SAINFO_AGDVBREAK		0x0006
#define	NXP_SAINFO_INFBREAK		0x0007
#define	NXP_SAINFO_DISABLESAVEKB	0x0008
#define	NXP_SAINFO_VERSION		0x0009

//------------------------------------------------------------------------
//	Alert types and return codes - NXP_SetHandler / NXP_PROC_ALERT
//------------------------------------------------------------------------

#define	NXP_ALRT_OK			1
#define	NXP_ALRT_OKCANCEL		2
#define	NXP_ALRT_YESNOCANCEL		3

#define	NXP_RET_OK			1
#define	NXP_RET_CANCEL			2
#define	NXP_RET_YES			6
#define	NXP_RET_NO			7

//------------------------------------------------------------------------
//	Notification codes - NXP_Notify and NXP_SetHandler / NXP_PROC_NOTIFY
//------------------------------------------------------------------------

#define	NXP_NF_DELETE			0x0001
#define	NXP_NF_CREATE			0x0002
#define	NXP_NF_MODIFY			0x0003
#define	NXP_NF_UPDATE			0x0004
#define	NXP_NF_FOCUS			0x0005
#define	NXP_NF_RESTART			0x0006
#define	NXP_NF_BEGINFILL		0x0007
#define	NXP_NF_ENDFILL			0x0008
#define	NXP_NF_REDRAW			0x0009
#define	NXP_NF_SHOW			0x000A
#define	NXP_NF_HIDE			0x000B
#define	NXP_NF_PRINT			0x000C
#define	NXP_NF_ENABLE			0x000D
#define	NXP_NF_DISABLE			0x000E
#define	NXP_NF_TOGGLE			0x000F
#define	NXP_NF_USEBOXES			0x0010
#define	NXP_NF_DIRTY			0x0011
#define	NXP_NF_BEGINREAD		0x0012
#define	NXP_NF_ENDREAD			0x0013
#define	NXP_NF_JRNLSHOW			0x0014
#define	NXP_NF_JRNLUPDATE		0x0015
#define	NXP_NF_LOADKB			0x0016
#define	NXP_NF_UNLOADKB			0x0017

//------------------------------------------------------------------------
//	GetStatus codes - NXP_GetStatus and NXP_SetHandler / NXP_PROC_GETSTATUS
//------------------------------------------------------------------------

#define	NXP_GS_MASK			0x080F
#define	NXP_GS_DIRTY			0x0001
#define	NXP_GS_ENABLED			0x000F

//------------------------------------------------------------------------
//	Window codes
//------------------------------------------------------------------------

#define	NXP_WIN_RULEDIT			0x0001
#define	NXP_WIN_OBJEDIT			0x0002
#define	NXP_WIN_CLASSEDIT		0x0003
#define	NXP_WIN_PROPEDIT		0x0004
#define	NXP_WIN_CNTXEDIT		0x0005
#define	NXP_WIN_METAEDIT		0x0006
#define NXP_WIN_TRAN			0x0010
#define NXP_WIN_BANNER			0x0011
#define	NXP_WIN_RULE			0x0012
#define	NXP_WIN_CONC			0x0013
#define	NXP_WIN_HYPO			0x0014
#define	NXP_WIN_DDE			0x0018
#define	NXP_WIN_QUESTION		0x0019
#define NXP_WIN_NATUREINFO		0x001D
#define NXP_WIN_TYPEINFO		0x001E
#define	NXP_WIN_RULEPROPS		0x0022
#define	NXP_WIN_STRATEGY		0x0023
#define	NXP_WIN_CASESTATUS		0x0025
#define	NXP_WIN_FULLREPORT		0x0026
#define NXP_WIN_SUGVOL			0x0027
#define	NXP_WIN_NEDWARN			0x0028
#define	NXP_WIN_DELNED			0x002C
#define	NXP_WIN_WHY			0x002D
#define	NXP_WIN_AGENDAM			0x002F
#define	NXP_WIN_XREFNET			0x0030
#define	NXP_WIN_XREFOVERVIEW		0x0031
#define	NXP_WIN_STRATEGYEDIT		0x0032
#define	NXP_WIN_METHODEDIT		0x0033
#define NXP_WIN_METHODNB		0x0034

//------------------------------------------------------------------------
//	Window Item codes
//------------------------------------------------------------------------

#define	NXP_ITEM_NONE			0x00000000
#define	NXP_ITEM_MASK			0x0000FF00
#define	NXP_ITEM_SUBCLASS		0x00000100
#define	NXP_ITEM_SUBOBJECT		0x00000200
#define	NXP_ITEM_NAME			0x00000300
#define	NXP_ITEM_PROPERTY		0x00000400
#define	NXP_ITEM_CACTIONS		0x00000500
#define	NXP_ITEM_SOURCES		0x00000600
#define	NXP_ITEM_CLASSES		0x00000700
#define	NXP_ITEM_OBJECTS		0x00000800
#define	NXP_ITEM_TITLE			0x00000900
#define	NXP_ITEM_CONTEXT		0x00000a00
#define	NXP_ITEM_HYPO			0x00000b00
#define	NXP_ITEM_LHS			0x00000c00
#define	NXP_ITEM_RHS			0x00000d00
#define	NXP_ITEM_TYPE			0x00000e00
#define	NXP_ITEM_INFCAT			0x00000f00
#define	NXP_ITEM_INHCAT			0x00001000
#define	NXP_ITEM_INHSLOTDEFAULT		0x00001100
#define	NXP_ITEM_INHSLOTUP		0x00001200
#define	NXP_ITEM_INHSLOTDOWN		0x00001300
#define	NXP_ITEM_INHVALDEFAULT		0x00001400
#define	NXP_ITEM_INHVALUP		0x00001500
#define	NXP_ITEM_INHVALDOWN		0x00001600
#define	NXP_ITEM_DEFAULTFIRST		0x00001700
#define	NXP_ITEM_PARENTFIRST		0x00001800
#define	NXP_ITEM_BREADTHFIRST		0x00001900
#define	NXP_ITEM_PROMPTLINE		0x00001a00
#define	NXP_ITEM_STORENAME		0x00001b00
#define	NXP_ITEM_STOREBUF		0x00001c00
#define	NXP_ITEM_CLASSFIRST		0x00001d00
#define	NXP_ITEM_DEPTHFIRST		0x00001e00
#define	NXP_ITEM_METASLOTS		0x00001f00
#define	NXP_ITEM_BOOLTYPE		0x00002000
#define	NXP_ITEM_DOUBLETYPE		0x00002100
#define	NXP_ITEM_STRINGTYPE		0x00002200
#define	NXP_ITEM_SPECIALTYPE		0x00002300
#define	NXP_ITEM_UNDEFTYPE		0x00002400
#define	NXP_ITEM_COMPONENTS		0x00002500
#define	NXP_ITEM_INFATOM		0x00002600
#define	NXP_ITEM_INHCLASSDOWN		0x00002700
#define	NXP_ITEM_INHCLASSUP		0x00002800
#define	NXP_ITEM_INHOBJDOWN		0x00002900
#define	NXP_ITEM_INHOBJUP		0x00002A00
#define	NXP_ITEM_PWTRUE			0x00002B00
#define	NXP_ITEM_PWFALSE		0x00002C00
#define	NXP_ITEM_PWNOTKNOWN		0x00002D00
#define	NXP_ITEM_EXHBWRD		0x00002E00
#define	NXP_ITEM_PFACTIONS		0x00002F00
#define	NXP_ITEM_BEGIN			0x00003000
#define	NXP_ITEM_END			0x00003100
#define	NXP_ITEM_QUERY			0x00003200
#define	NXP_ITEM_ARGS			0x00003300
#define	NXP_ITEM_DBTYPE			0x00003400
#define	NXP_ITEM_NEW			0x00003500
#define	NXP_ITEM_ADD			0x00003600
#define	NXP_ITEM_LINKTO			0x00003700
#define	NXP_ITEM_NOFWRD			0x00003800
#define	NXP_ITEM_CURFWRD		0x00003900
#define	NXP_ITEM_ALWAYSFWRD		0x00003A00
#define	NXP_ITEM_PROPS			0x00003B00
#define	NXP_ITEM_FIELDS			0x00003C00
#define	NXP_ITEM_IN			0x00003D00
#define	NXP_ITEM_UNKNOWN		0x00003E00
#define NXP_ITEM_COMPILED		0x00004000
#define NXP_ITEM_KBLIST			0x00004100
#define	NXP_ITEM_INHATOM		0x00004200
#define	NXP_ITEM_COMMENTS		0x00004300
#define	NXP_ITEM_CURSOR			0x00004400
#define	NXP_ITEM_SOURCESON		0x00004500
#define	NXP_ITEM_CACTIONSON		0x00004600
#define	NXP_ITEM_LEFT			0x00004700
#define	NXP_ITEM_TOP			0x00004800
#define	NXP_ITEM_WIDTH			0x00004900
#define	NXP_ITEM_HEIGHT			0x00004a00
#define	NXP_ITEM_KEEP			0x00004b00
#define	NXP_ITEM_WAIT			0x00004c00
#define	NXP_ITEM_ATOMID			0x00004d00
#define	NXP_ITEM_STRING			0x00004e00
#define	NXP_ITEM_USEDIN			0x00004F00
#define	NXP_ITEM_LONGTYPE		0x00005000
#define	NXP_ITEM_DATETYPE		0x00005100
#define	NXP_ITEM_FORMAT			0x00005200
#define	NXP_ITEM_PTGATES		0x00005300
#define	NXP_ITEM_WHY			0x00005400
#define	NXP_ITEM_TIMETYPE		0x00005500
#define	NXP_ITEM_HOW			0x00005600
#define	NXP_ITEM_SUGGEST		0x00005700
#define	NXP_ITEM_VOLUNTEER		0x00005800

//------------------------------------------------------------------------
//	Window item cell codes
//------------------------------------------------------------------------

#define	NXP_CELL_MASK			0xF0000000
#define	NXP_CELL_NONE			0x00000000
#define	NXP_CELL_COL1			0x10000000
#define	NXP_CELL_COL2			0x20000000
#define	NXP_CELL_COL3			0x30000000
#define	NXP_CELL_BOX			0x40000000
#define	NXP_CELL_NUMB			0x50000000
#define NXP_CELL_OPNUM			0x60000000
#define	NXP_CELL_COL4			0x70000000
#define	NXP_CELL_ATOM			0x80000000

//------------------------------------------------------------------------
//	Edit codes - NXP_Edit
//------------------------------------------------------------------------

#define	NXP_EDIT_MASK			0x00F0
#define	NXP_EDIT_NEW			0x0010
#define	NXP_EDIT_MODIFY			0x0020
#define	NXP_EDIT_COPY			0x0030
#define	NXP_EDIT_DELETE			0x0040
#define	NXP_EDIT_CHTYPE			0x0050
#define	NXP_EDIT_DEFAULT		0x0060
#define	NXP_EDIT_CURRENT		0x0070

//------------------------------------------------------------------------
//	Mode codes
//------------------------------------------------------------------------
/*
#if MCH_OS == MCH_OSCMS
#define NXP_MODE_TEXT			0x0000
#endif
*/
#define NXP_MODE_MASK			0x000F
#define	NXP_MODE_CHECK			0x0001
#define	NXP_MODE_PROMPT			0x0002
#define	NXP_MODE_COMPILED		0x0004
#define	NXP_MODE_COMMENTS		0x0008

//------------------------------------------------------------------------
//	Journaling Codes - NXP_Journal
//------------------------------------------------------------------------

#define	NXP_JRNL_QUESTION		0x1000
#define	NXP_JRNL_SUGGEST		0x2000
#define	NXP_JRNL_VALUE			0x4000
#define	NXP_JRNL_VOLUNTEER		0x8000

#define	NXP_JRNL_CODEMASK		0x000F
#define	NXP_JRNL_RECORDSTART		0x0000
#define	NXP_JRNL_RECORDSTOP		0x0001
#define	NXP_JRNL_RECORDEVENT		0x0002
#define	NXP_JRNL_PLAYSTART		0x0003
#define	NXP_JRNL_PLAYSTOP		0x0004
#define	NXP_JRNL_PLAYEVENT		0x0005
#define	NXP_JRNL_VALUESSAVE		0x0006
#define	NXP_JRNL_STATESAVE		0x0007
#define	NXP_JRNL_STATERESTORE		0x0008

#define	NXP_JRNL_PLAYNOSCAN		0x0100
#define	NXP_JRNL_PLAYSKIPSHOW		0x0200
#define	NXP_JRNL_PLAYSTEP		0x0400

/*
#if MCH_OS == MCH_OSCMS
#define NXP_JOURNALTYPE_RESET    0x00
#define NXP_JOURNALTYPE_MEMORY   0x01
#define NXP_JOURNALTYPE_DATABASE 0x02
#define NXP_JOURNALTYPE_FILE     0x03
#define NXP_JOURNALTYPE_GETLEN   0x04
#define NXP_JOURNALTYPE_DBOPTIONS 0x05
#endif
*/
//------------------------------------------------------------------------
//	Inheritance Codes - NXP_Inherit
//------------------------------------------------------------------------
#define	NXP_WALKN_DOWN			0
#define	NXP_WALKN_UP			1

#define	NXP_WALKN_CONT			0
#define	NXP_WALKN_END			1
#define	NXP_WALKN_STOP			2

//------------------------------------------------------------------------
//	NXPGFX codes - NXPGFX_Control
//------------------------------------------------------------------------

#if !defined(NXPGFX_CTRL_INIT)
#	define NXPGFX_CTRL_INIT			1
#	define NXPGFX_CTRL_START		2
#	define NXPGFX_CTRL_STARTKNOWCESS	3
#	define NXPGFX_CTRL_EXIT			4
#endif

//------------------------------------------------------------------------
//	Value record
//------------------------------------------------------------------------
/*
#if defined(ND_COMPAT_NXPAPI)

typedef struct NXP_ValueRec {
	int		Known;
	int		Notknown;
	int		Type;
	union {
		int		Bool;
		Double		Numb;
		Double		Double;
		Long		Long;
		// Date and Time not available yet 
	} NVal;
	char	far	*_Str;
} NXP_ValueRec, *NXP_ValuePtr;

#else
*/
typedef struct NXP_ValueRec {
	int		Known;
	int		Notknown;
	int		Type;
	union {
		int		NxBool;
		double		NxNumb;
		double		NxDouble;
		long		NxLong;
		// Date and Time not available yet 
	} NVal;
	char	far*NxStr;
} NXP_ValueRec,*NXP_ValuePtr;
//#endif

//------------------------------------------------------------------------
//NXP_DateRec used with Volunteer priority of NXP_VSTRAT_NOCHECK
//------------------------------------------------------------------------
typedef struct NXP_DateRec
{
	UINT Month;
	UINT Day;
	UINT Year;
	UINT Hour;
	UINT Min;
	UINT Sec;
} NXP_DateRec,far*NXP_DatePtr;
//------------------------------------------------------------------------
//	Point record - NXP_SetHandler / NXP_PROC_APROPOS
//------------------------------------------------------------------------
typedef struct NXP_PtRec {
	int		x;
	int		y;
} NXP_PtRec,far*NXP_PtPtr;
//------------------------------------------------------------------------
//	Macros for NXP_GetAtomInfo
//------------------------------------------------------------------------

//------------------------------------------------------------------------
//	NXP_GETXXXINFO macros - when optAtom and optint are useless
//------------------------------------------------------------------------

#define NXP_GETATOMINFO(atom, code, ptr)				\
	NXP_GetAtomInfo((AtomId)(atom), code, (AtomId)0, 0,		\
	NXP_DESC_ATOM, (Str)(ptr), 0)

#define NXP_GETINTINFO(atom, code, ptr)					\
	NXP_GetAtomInfo((AtomId)(atom), code, (AtomId)0, 0,		\
	NXP_DESC_INT, (Str)(ptr), 0)

#define NXP_GETLONGINFO(atom, code, ptr)				\
	NXP_GetAtomInfo((AtomId)(atom), code, (AtomId)0, 0,		\
	NXP_DESC_LONG, (Str)(ptr), 0)

#define NXP_GETDOUBLEINFO(atom, code, ptr)				\
	NXP_GetAtomInfo((AtomId)(atom), code, (AtomId)0, 0,		\
	NXP_DESC_DOUBLE, (Str)(ptr), 0)

#define NXP_GETSTRINFO(atom, code, ptr, len)				\
	NXP_GetAtomInfo((AtomId)(atom), code, (AtomId)0, 0,		\
	NXP_DESC_STR, (Str)(ptr), len)

//------------------------------------------------------------------------
//	NXP_GETXXXVAL macros - get values
//------------------------------------------------------------------------

#define NXP_GETINTVAL(atom, ptr)					\
	NXP_GETINTINFO(atom, NXP_AINFO_VALUE, ptr)

#define NXP_GETLONGVAL(atom, ptr)					\
	NXP_GETLONGINFO(atom, NXP_AINFO_VALUE, ptr)

#define NXP_GETDOUBLEVAL(atom, ptr) 					\
	NXP_GETDOUBLEINFO(atom, NXP_AINFO_VALUE, ptr)

#define NXP_GETSTRVAL(atom, ptr, len)					\
	NXP_GETSTRINFO(atom, NXP_AINFO_VALUE, ptr, len)

#define NXP_GETUNKNOWNVAL(atom, ptr)					\
	NXP_GetAtomInfo((AtomId)(atom), NXP_AINFO_VALUE, (AtomId)0, 0,	\
	NXP_DESC_UNKNOWN, (Str)(ptr), 0)

#define NXP_GETNOTKNOWNVAL(atom, ptr)					\
	NXP_GetAtomInfo((AtomId)(atom), NXP_AINFO_VALUE, (AtomId)0, 0,	\
	NXP_DESC_NOTKNOWN, (Str)(ptr), 0)

//------------------------------------------------------------------------
//	NXP_GETNAME - get atom name
//------------------------------------------------------------------------

#define NXP_GETNAME(atom, ptr, len)					\
	NXP_GETSTRINFO(atom, NXP_AINFO_NAME, ptr, len)

//------------------------------------------------------------------------
//	NXP_GETLIST macros - get lists
//------------------------------------------------------------------------

#define NXP_GETLISTLEN(atom, info, ptr)					\
	NXP_GetAtomInfo((AtomId)(atom), info, (AtomId)0, -1,		\
	NXP_DESC_INT, (Str)(ptr), 0)

#define NXP_GETLISTELT(atom, info, index, ptr)				\
	NXP_GetAtomInfo((AtomId)(atom), info, (AtomId)0, index,		\
	NXP_DESC_ATOM, (Str)(ptr), 0)

#define NXP_GETLISTELTSTR(atom, info, index, ptr, len)			\
	NXP_GetAtomInfo((AtomId)(atom),info,(AtomId)0,index,NXP_DESC_STR,(Str)(ptr),len)

#define NXP_GETLISTNEXT(atom, type, ptr)				\
	NXP_GetAtomInfo((AtomId)(atom),NXP_AINFO_NEXT,(AtomId)0,type,NXP_DESC_ATOM,(Str)(ptr),0)

#define NXP_GETLISTFIRST(type, ptr)	NXP_GETLISTNEXT(0, type, ptr)

//------------------------------------------------------------------------
//	Function declarations
//------------------------------------------------------------------------
//typedef	int(*NxpWalkProc)L((long,AtomId,AtomId));
typedef	int(*NxpWalkProc)L((unsigned long,unsigned long,unsigned long));

typedef	int(far*NxpAlertProc)L((int, CStr, int*));
typedef	int(far*NxpAlert2Proc)L((ULong,int,CStr,int*));

typedef	int(far*NxpAproposProc)L((Str, int, int, NXP_PtPtr, NXP_PtPtr));
typedef	int(far*NxpApropos2Proc)L((ULong, Str, int, int, NXP_PtPtr, NXP_PtPtr));

typedef	int(far*NxpCancelProc)L((int, int*));
typedef	int(far*NxpCancel2Proc)L((ULong, int, int*));

typedef	int(far*NxpEndOfSessionProc)L(());
typedef	int(far*NxpEndOfSession2Proc)L((ULong));

typedef	int(far*NxpExecuteProc)L((Str, int, AtomId far *));
typedef	int(far*NxpExecute2Proc)L((ULong, Str, int, AtomId far *));

typedef	int(far*NxpFormInputProc)  L((Str, int));
typedef	int(far*NxpFormInput2Proc) L((ULong, Str, int));

typedef	int(far*NxpGetDataProc)L((int,int,int,Str));
typedef	int(far*NxpGetData2Proc)L((ULong,int,int,int,Str));

typedef	int(far*NxpGetStatusProc)L((int,int,int*));
typedef	int(far*NxpGetStatus2Proc)L((ULong, int, int, int*));

typedef	int(far*NxpMemExitProc)L(());
typedef	int(far*NxpMemExit2Proc)L((ULong));

typedef	int(far*NxpNotifyProc)L((int,int,AtomId));
typedef	int(far*NxpNotify2Proc)L((ULong,int,int,AtomId));

typedef	int(far*NxpPasswordProc)  L((CStr, Str));
typedef	int(far*NxpPassword2Proc) L((ULong, CStr, Str));

typedef	int(far*NxpPollingProc)  L(());
typedef	int(far*NxpPolling2Proc) L((ULong));

typedef	int(far*NxpQuestionProc)  L((AtomId, CStr));
typedef	int(far*NxpQuestion2Proc) L((ULong, AtomId, CStr));

typedef	int(far*NxpQuitProc)  L((int*));
typedef	int(far*NxpQuit2Proc) L((ULong, int*));

typedef	int(far*NxpSetDataProc)  L((int, int, int, CStr));
typedef	int(far*NxpSetData2Proc) L((ULong, int, int, int, CStr));

typedef	int(far*NxpValidateProc)  L((int, AtomId, VoidPtr, int*));
typedef	int(far*NxpValidate2Proc) L((ULong, int, AtomId, VoidPtr, int*));

typedef	int(far*NxpVolValidateProc)  L((AtomId, int, Str, int, int*));
typedef	int(far*NxpVolValidate2Proc) L((ULong, AtomId, int, Str, int, int*));
#endif // _NXPPUB_ 
