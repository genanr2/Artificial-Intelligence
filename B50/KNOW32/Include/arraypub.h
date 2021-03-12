//  arraypub.h
//  Elements Environment 2.0
//  Origin: arraypub.h
//  Copyright (C) 1985-1996 by Neuron Data Incorporated.
//  All Rights Reserved.
//------------------------------------------------------------------------
//	Overview
//------------------------------------------------------------------------
// This module defines the base implementation for all Open Interface
// collection classes.
// Collection classes can be:
//	- collection of possibly duplicate items
//		(bags)
//	- indexable collections of possibly duplicate items
//		(collections)
//	- collection of unduplicated items
//		(sets)
//	- sequences of items with insert and remove operations at
//	  any index in the sequence
//		(cover dequeues, queues, stacks)
// Collection classes can be:
//	- pointer-based collections:
//		only reference to objects are stored
//		copies are shallow copies
//		objects are allocated, deallocated by the application.
//	- value-based collections:
//		the actual value of the object is stored
//		copies are deep copies
//		copies are allocated, deallocated by the application.
// The implementation of the collection classes takes care of all internal
// allocation issues. When an element is inserted, Open Interface takes care
// of adjusting the size of the structures used to keep track of the items in
// the collection.
// In this version of Open Interface, the following generic collection
// classes are offered:
//	- ArPtr classes (see arptrpub.h):
//		- pointer-based collection, or integer-based collections
//		- duplicate or not items
//		- ordered or not items
//		- indexable
//		- insert/remove at any index
//	- ArRec classes (see arrecpub.h) :
//		- uniform-size value-based collections
//		- duplicate or not items
//		- ordered or not items
//		- indexable
//		- insert/remove at any index
//	- ArNum classes (see arnumpub.h) :
//		- uniform-size value-based collections for numeric values
//		- duplicate or not items
//		- ordered or not items
//		- indexable
//		- insert/remove at any index
// Note:
//	For compatibility reasons, this module also implements a
//	set of macros that allow to directly manipulate instances of
//	ARRAY.
//	The programmer should avoid those macros, and use the classes
//	implemented in arptrpub.h, arrecpub.h and arnumpub.h.
//------------------------------------------------------------------------
//	Includes
//------------------------------------------------------------------------
/*
#ifdef MAC
#pragma once
#endif
*/
#ifndef _ARRAYPUB_
#define _ARRAYPUB_
#define ARRAY_COMPAT	// for now 
#ifndef _PTRPUB_
#include <ptrpub.h>
#endif
#ifndef _STRPUB_
#include <strpub.h>
#endif
#define ARRAY_MODULE "Array"
//------------------------------------------------------------------------
//	Base collection definition
//------------------------------------------------------------------------
#ifndef _ARRAYIVAL_
#define _ARRAYIVAL_
typedef Int32 ArrayIVal;
typedef Int32 C_FAR*ArrayIValPtr;
typedef const Int32 C_FAR*ArrayIValCPtr;
typedef Int32 C_FAR*ArrayIValRef;
typedef const Int32 C_FAR*ArrayIValCRef;
typedef Int32 C_FAR*NDArrayIValPtr;
typedef const Int32 C_FAR*NDArrayIValCPtr;
typedef Int32 C_FAR&NDArrayIValRef;
typedef const Int32 C_FAR & NDArrayIValCRef;
#endif	// _ARRAYIVAL_ 
//	Type for the index of an element inside the array.
typedef struct NDArray C_FAR * ArrayPtr;
typedef const struct NDArray C_FAR * ArrayCPtr;
typedef struct NDArray C_FAR * ArrayRef;
typedef const struct NDArray C_FAR * ArrayCRef;
typedef struct NDArray C_FAR * NDArrayPtr;
typedef const struct NDArray C_FAR * NDArrayCPtr;
typedef struct NDArray C_FAR & NDArrayRef;
typedef const struct NDArray C_FAR & NDArrayCRef;
typedef struct NDArray ArrayRec;
typedef const struct NDArray ArrayCRec;
struct NDArray{ArrayIVal Used;ArrayIVal Alloc;ClientPtr C_HUGE*Data;};
//------------------------------------------------------------------------
//	Collection Implementation calls
//------------------------------------------------------------------------
C_TEMP_IMP_PROTO(EP, void, ARRAY_GenSetLen,	(ArrayPtr array, ArrayIVal i, PtrSizeVal eltSize))
C_TEMP_IMP_PROTO(EP, void, ARRAY_GenSetAlloc,	(ArrayPtr array, ArrayIVal i, PtrSizeVal eltSize))
C_TEMP_IMP_PROTO(EP, ArrayPtr, ARRAY_GenClone,	(ArrayCPtr src, PtrSizeVal eltSize))
C_TEMP_IMP_PROTO(EP, void, ARRAY_GenCopy,	(ArrayPtr dst, ArrayCPtr src, PtrSizeVal eltSize))
C_TEMP_IMP_INLIN(EP, ArrayIVal, ARRAY_GenGetI,(ArrayCPtr array, ArrayIVal i))
C_TEMP_IMP_PROTO(EP, ArrayIVal, ARRAY_GenAppendI,	(ArrayPtr array, PtrSizeVal eltSize))
C_TEMP_IMP_PROTO(EP, ArrayIVal, ARRAY_GenExtendI,	(ArrayPtr array, ArrayIVal i, PtrSizeVal eltSize))
C_TEMP_IMP_PROTO(EP, ArrayIVal, ARRAY_GenInsertI,	(ArrayPtr array, ArrayIVal i, PtrSizeVal eltSize))
C_TEMP_IMP_PROTO(EP, ArrayIVal, ARRAY_GenExtractI,	(ArrayPtr array, ArrayIVal i, PtrSizeVal eltSize))

C_TEMP_IMP_PROTO(EP, ArrayIVal, ARRAY_GenSortedInsertElt,	(ArrayPtr array, CmpProc cmp, ClientCPtr gelt, PtrSizeVal eltSize))
C_TEMP_IMP_PROTO(EP, ArrayIVal, ARRAY_GenSortedInsertAddr,	(ArrayPtr array, CmpProc cmp, ClientCPtr gelt, PtrSizeVal eltSize))
C_TEMP_IMP_PROTO(EP, ArrayIVal, ARRAY_GenSortedUniqInsertElt,	(ArrayPtr array, CmpProc cmp, ClientCPtr gelt, PtrSizeVal eltSize))
C_TEMP_IMP_PROTO(EP, ArrayIVal, ARRAY_GenSortedUniqInsertAddr,	(ArrayPtr array, CmpProc cmp, ClientCPtr gelt, PtrSizeVal eltSize))

C_TEMP_IMP_PROTO(EP, ArrayIVal, ARRAY_GenLookupElt,	(ArrayCPtr array, ClientCPtr gelt, PtrSizeVal eltSize))
C_TEMP_IMP_PROTO(EP, ArrayIVal, ARRAY_GenLookupAddr,	(ArrayCPtr array, ClientCPtr gelt, PtrSizeVal eltSize))
C_TEMP_IMP_PROTO(EP, ArrayIVal, ARRAY_GenFindElt,	(ArrayCPtr array, ClientCPtr gelt, PtrSizeVal eltSize))
C_TEMP_IMP_PROTO(EP, ArrayIVal, ARRAY_GenFindAddr,	(ArrayCPtr array, ClientCPtr gelt, PtrSizeVal eltSize))

C_TEMP_IMP_PROTO(EP, BoolEnum, ARRAY_GenSortedLookupElt,	(ArrayCPtr array, CmpProc cmp, ClientCPtr gelt, 	ArrayIValPtr i, PtrSizeVal eltSize))
C_TEMP_IMP_PROTO(EP, BoolEnum, ARRAY_GenSortedLookupAddr,	(ArrayCPtr array, CmpProc cmp, ClientCPtr gelt, 	ArrayIValPtr i, PtrSizeVal eltSize))
C_TEMP_IMP_PROTO(EP, ArrayIVal, ARRAY_GenSortedFindElt,	(ArrayCPtr array, CmpProc cmp, ClientCPtr gelt, PtrSizeVal eltSize))
C_TEMP_IMP_PROTO(EP, ArrayIVal, ARRAY_GenSortedFindAddr,	(ArrayCPtr array, CmpProc cmp, ClientCPtr gelt, PtrSizeVal eltSize))

C_TEMP_IMP_PROTO(EP, void, ARRAY_GenSort,	(ArrayPtr array, CmpProc cmp, PtrSizeVal eltSize))
C_TEMP_IMP_PROTO(EP, void, ARRAY_GenSortAddr,	(ArrayPtr array, CmpProc cmp, PtrSizeVal eltSize))
C_TEMP_IMP_PROTO(EP, BoolEnum, ARRAY_GenIsSorted,	(ArrayCPtr array, CmpProc cmp, PtrSizeVal eltSize))
C_TEMP_IMP_PROTO(EP, BoolEnum, ARRAY_GenIsSortedAddr,	(ArrayCPtr array, CmpProc cmp, PtrSizeVal eltSize))
C_TEMP_IMP_PROTO(EP, void, ARRAY_GenRemoveDupls,	(ArrayPtr array, PtrSizeVal eltSize))
C_TEMP_IMP_PROTO(EP, void, ARRAY_GenSortedRemoveDupls,	(ArrayPtr array, PtrSizeVal eltSize))

C_TEMP_IMP_PROTO(EP, void, ARRAY_GenClear,	(ArrayPtr array, PtrSizeVal totalSize))
C_TEMP_IMP_PROTO(EP, HugePtr, ARRAY_GenAllocData, (PtrHugeSizeVal totalSize))
C_TEMP_IMP_PROTO(EP, void, ARRAY_GenDeallocData, (HugePtr ptr))
//------------------------------------------------------------------------
//	Generic Implementation inlines
//------------------------------------------------------------------------
#ifdef C_INLINE
#	define ARRAY_GenGetI(array, i)	i
#endif
//------------------------------------------------------------------------
//	Generic structure
//------------------------------------------------------------------------
#define ARRAY_TEMPLATE_SUBREC(ARRAY_ELT)ArrayIVal		Used;		\
	ArrayIVal		Alloc;ARRAY_ELT C_HUGE*Data;		\

#define ARRAY_TEMPLATE_REC(ARRAY_ELT) 	\
	ARRAY_TEMPLATE_SUBREC(ARRAY_ELT)

#define ARRAY_REC(type) 	ARRAY_TEMPLATE_SUBREC(type)

#define ARRAY_DEFSTRUCT(atype, etype) 			\
	C_DEFSTRUCT(atype) { ARRAY_TEMPLATE_REC(etype) };
//------------------------------------------------------------------------
//	Compatibility Implementation Macros: New, Dispose, Init, End
//------------------------------------------------------------------------
extern "C" ArrayPtr C_DECLIMPORT ARRAY_GenAlloc (void);
extern "C" void C_DECLIMPORT ARRAY_GenDealloc (ArrayPtr array);
#define ARRAY_Alloc()		ARRAY_GenAlloc()
#define ARRAY_Dealloc(array)	ARRAY_GenDealloc((ArrayPtr)array);

#define ARRAY_Construct(array)						\
	ARRAY_GenClear((ArrayPtr)(array), sizeof(*array))

#define ARRAY_ConstructLen(array, len) {				\
	(array)->Alloc = len;						\
	(array)->Used = len;						\
	*((HugePtr *)&((array)->Data)) = ARRAY_GenAllocData((len) *	\
						sizeof(*(array)->Data));\
}
#define ARRAY_ConstructAlloc(array, alloc) {				\
	(array)->Alloc = alloc;						\
	(array)->Used = 0;						\
	*((HugePtr *)&((array)->Data)) = ARRAY_GenAllocData((alloc) * 	\
						sizeof(*(array)->Data));\
}
#define ARRAY_ConstructArray(array, array2) {				\
	ARRAY_Construct(array);						\
	ARRAY_GenCopy((ArrayPtr)(array), (ArrayPtr)(array2),	\
					sizeof(*(array)->Data));	\
}
#define ARRAY_Destruct(array) {						\
	if ((array)->Data != NULL) 					\
		ARRAY_GenDeallocData((HugePtr)(array)->Data);		\
	ARRAY_GenClear((ArrayPtr)(array), sizeof(*array));		\
}
//------------------------------------------------------------------------
//	Compatibility Implementation Macros: Clone, Copy, Reset
//------------------------------------------------------------------------
#define ARRAY_Clone(array)					\
	(ClientPtr)ARRAY_GenClone((ArrayPtr)(array),sizeof(*(array)->Data))
#define ARRAY_Copy(array1, array2)			\
	ARRAY_GenCopy((ArrayPtr)(array1), (ArrayPtr)(array2),sizeof(*(array1)->Data))

#define ARRAY_Reset(array)	{			\
	if ((array)->Data != NULL) 			\
		ARRAY_GenDeallocData((HugePtr)(array)->Data),\
	(array)->Alloc = 0,				\
	(array)->Used = 0,				\
	(array)->Data = NULL; 				\
}

//------------------------------------------------------------------------
//	Compatibility Implementation Macros: SetLen, SetAlloc
//------------------------------------------------------------------------

#define ARRAY_SetLen(array, len)	\
	ARRAY_GenSetLen((ArrayPtr)array, len, sizeof(*(array)->Data))

#define ARRAY_SetAlloc(array, len)	\
	ARRAY_GenSetAlloc((ArrayPtr)array, len, sizeof(*(array)->Data))

//------------------------------------------------------------------------
//	Compatibility Implementation Macros: Global Queries
//------------------------------------------------------------------------

#define ARRAY_GetLen(array)	((array)->Used)
#define ARRAY_IsEmpty(a)	(ARRAY_GetLen(a) == 0)

#define ARRAY_IsInRange(array, i)	\
		((i) >= 0 && (i) < ARRAY_GetLen(array))

//------------------------------------------------------------------------
//	Compatibility Implementation Macros: Accessing Elements
//------------------------------------------------------------------------

#define ARRAY_GetNthElt(array, i)	\
		(array)->Data[ARRAY_GenGetI((ArrayCPtr)(array), i)]

#define ARRAY_GetNthEltAddr(array, i)	\
		&(array)->Data[ARRAY_GenGetI((ArrayCPtr)(array), i)]

#define ARRAY_SetNthElt(array, i, elt)	\
		(array)->Data[ARRAY_GenGetI((ArrayCPtr)(array), i)] = (elt)

#define ARRAY_BoundedGetNthElt(array, i)	\
		(array)->Data[i]

#define ARRAY_BoundedGetNthEltAddr(array, i)	\
		&(array)->Data[i]

#define ARRAY_BoundedSetNthElt(array, i, elt)	\
		(array)->Data[i] = (elt)

#define ARRAY_UnboundedGetNthElt(array, i)				\
		(ARRAY_IsInRange(array, i) ? 				\
		 (array)->Data[ARRAY_GenGetI((ArrayCPtr)(array), i)] : 0)

#define ARRAY_UnboundedSetNthElt(array, i, elt)	\
	if ((elt) != 0) {			\
		ArrayIVal _i = ARRAY_GenExtendI((ArrayPtr)(array), i,	\
						sizeof(*(array)->Data)); \
		(array)->Data[_i] = (elt);	\
       }

//------------------------------------------------------------------------
//	Compatibility Implementation Macros: Finding Elements
//------------------------------------------------------------------------

#define ARRAY_ContainsElt(array, elt)	(ARRAY_LookupElt(array, elt) >= 0)

#define ARRAY_LookupElt(array, elt)				\
	ARRAY_GenLookupElt((ArrayCPtr)(array), (ClientCPtr)elt,\
		sizeof(*(array)->Data))

#define ARRAY_FindElt(array, elt)				\
	ARRAY_GenFindElt((ArrayCPtr)(array), (ClientCPtr)elt,\
		sizeof(*(array)->Data))

#define ARRAY_SortedFindElt(array, proc, key)			\
	ARRAY_GenSortedFindElt((ArrayCPtr)array, proc, (ClientCPtr)key,\
		sizeof(*(array)->Data))

#define ARRAY_SortedLookupElt(array, proc, key, iptr)		\
	ARRAY_GenSortedLookupElt((ArrayCPtr)array, proc, (ClientCPtr)key, \
		iptr, sizeof(*(array)->Data))

//------------------------------------------------------------------------
//	Compatibility Implementation Macros: Adding Elements
//------------------------------------------------------------------------

// careful, use _i intermediate variable in case Data is reallocated 
#define ARRAY_AppendElt(array, elt) {				\
	ArrayIVal _i = ARRAY_GenAppendI((ArrayPtr)(array),	\
		sizeof(*(array)->Data));			\
	(array)->Data[_i] = (elt);				\
}

#define ARRAY_UniqAppendElt(array, elt)		\
	{ if (!ARRAY_ContainsElt(array, elt)) 	\
		  ARRAY_AppendElt(array, elt); }

#define ARRAY_InsertNthElt(array, i, elt)	{ 		\
	ArrayIVal _i = ARRAY_GenInsertI((ArrayPtr)(array), i,\
		sizeof(*(array)->Data));			\
	(array)->Data[_i] = (elt);				\
}

#define ARRAY_SortedInsertElt(array, proc, elt)			\
	ARRAY_GenSortedInsertElt((ArrayPtr)(array), 		\
		proc, (ClientCPtr)elt, sizeof(*(array)->Data))

#define ARRAY_SortedUniqInsertElt(array, proc, elt)		\
	ARRAY_GenSortedUniqInsertElt((ArrayPtr)(array), 	\
		proc, (ClientCPtr)elt, sizeof(*(array)->Data))

//------------------------------------------------------------------------
//	Compatibility Implementation Macros: Removing Elements
//------------------------------------------------------------------------

#define ARRAY_RemoveNthElt(array, i) {	\
	ARRAY_SetNthElt(array, i, ARRAY_GetNthElt(array, (array)->Used - 1));\
	(array)->Used--;		\
}

#define ARRAY_RemoveElt(array, elt)	\
	ARRAY_RemoveNthElt(array, ARRAY_FindElt(array, elt))

#define ARRAY_ExtractNthElt(array, i)			\
	(void)ARRAY_GenExtractI((ArrayPtr)(array), i, \
				sizeof(*(array)->Data))

#define ARRAY_ExtractElt(array, elt)	\
	ARRAY_ExtractNthElt(array, ARRAY_FindElt(array, elt))

#define ARRAY_SortedExtractElt(array, proc, elt)			\
	ARRAY_GenExtractI((ArrayPtr)(array), 			\
				ARRAY_SortedFindElt(array, proc, elt),	\
				sizeof(*(array)->Data))

//------------------------------------------------------------------------
//	Compatibility Implementation Macros: Sorting
//------------------------------------------------------------------------

#define ARRAY_Sort(array, proc)		\
	ARRAY_GenSort((ArrayPtr)array, proc, sizeof(*(array)->Data))

#define ARRAY_IsSorted(array, proc)		\
	ARRAY_GenIsSorted((ArrayCPtr)array, proc, sizeof(*(array)->Data))

//------------------------------------------------------------------------
//	Compatibility Implementation Macros: Removing duplicates
//------------------------------------------------------------------------

#define ARRAY_RemoveDupls(array)			\
	ARRAY_GenRemoveDupls((ArrayPtr)(array), 	\
			sizeof(*(array)->Data))

#define ARRAY_SortedRemoveDupls(array)			\
	ARRAY_GenSortedRemoveDupls((ArrayPtr)(array),\
				sizeof(*(array)->Data))

#ifdef ARRAY_COMPAT
//------------------------------------------------------------------------
//	Compatibility API
//------------------------------------------------------------------------

extern "C" ArrayPtr C_DECLIMPORT ARRAY_NewSetLen (ArrayIVal arrayival);

//	Allocates a new array of n elements.
//	The elements are initially set to NULL and can be set with
//	This call assumes that the array is an array of pointers.
//	There is no corresponding call for arrays of records or scalars.
// 

#define ARRAY_AddElt		ARRAY_AppendElt
#define ARRAY_Contains		ARRAY_ContainsElt
#define ARRAY_GetElt		ARRAY_GetNthElt
#define ARRAY_RemoveIndex	ARRAY_RemoveNthElt
#define ARRAY_SetElt		ARRAY_SetNthElt
#define ARRAY_SortedAddIndex	ARRAY_InsertNthElt
#define ARRAY_SortedRemoveIndex	ARRAY_ExtractNthElt
#define ARRAY_SortedRemoveElt	ARRAY_ExtractElt
#define ARRAY_UniqAddElt	ARRAY_UniqAppendElt
#define ARRAY_NewSetSize	ARRAY_NewSetLen
//	Renamed
// 

#define ARRAY_SortedLookupKey(a,k,p,r) 	ARRAY_SortedLookupElt(a,p,k,r)
//	Same as ARRAY_SortedLookupElt(array, proc, key, result)
//	Warning: args 2 and 3 interverted.
// 

#define ARRAY_SortedFindKey(a,k,p) 	ARRAY_SortedLookupElt(a,p,k)
//	Same as ARRAY_SortedFindElt(array, proc, key)
//	Warning: args 2 and 3 interverted.
// 

#endif	// ARRAY_COMPAT 

//------------------------------------------------------------------------
//	Compatibility: Older Compatibility macros
//------------------------------------------------------------------------

#define ARRAY_Init(array)		ARRAY_Construct(array)
#define ARRAY_InitSetLen(array, len)	ARRAY_ConstructLen(array, len)
#define ARRAY_InitSetAlloc(array, alloc) ARRAY_ConstructAlloc(array, alloc)

#define ARRAY_End(array)		ARRAY_Destruct(array)

#define ARRAY_New()			ARRAY_Alloc()
#define ARRAY_Dispose(array)	{ 				\
					ARRAY_Destruct(array);	\
					ARRAY_Dealloc(array); 	\
				}

#define ARRAY_Dispose0(array)	{ 				\
					if (array != NULL) 	\
						ARRAY_Dispose(array); \
				}

#endif // _ARRAYPUB_ 
