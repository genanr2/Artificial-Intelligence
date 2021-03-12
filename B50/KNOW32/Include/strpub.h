//  strpub.h
//  Elements Environment 2.0
//  Origin: strpub.h 6.22
//  Copyright (C) 1985-1996 by Neuron Data Incorporated.
//  All Rights Reserved.
//------------------------------------------------------------------------
//	Overview
//------------------------------------------------------------------------
// The Str module implements our basic string data type and string manipulation
// routines. It is functionally very similar to the C RTL string package
// (strlen, strcat, ...) but offers the following advantages:
// - consistent naming.
// - better error handling.
// - support for multi-byte character sets. If you intend to produce versions
// of your software for asian countries and you want a portable library of
// routines supporting multi-byte character sets, you should use the STR
// module instead of the standard C RTL routines. Some manufacturers (i.e. HP)
// patch the C RTL so that it also works with multi-byte characters, others
// provide a separate set of routines (i.e. jstrlen, jstrcat) for multi-byte
// characters. There are several standards for the character codes and no
// real standard for the RTL support.
//------------------------------------------------------------------------
//	Includes
//------------------------------------------------------------------------
/*
#ifdef MAC
#pragma once
#endif
*/
#ifndef _STRPUB_
#define _STRPUB_
#define STR_COMPAT	// for now 
#ifndef _BASEPUB_
#include <basepub.h>
#endif
#ifndef _VAPUB_
#include <vapub.h>
#endif
#ifndef _CHARPUB_
#include <charpub.h>
#endif
#define STR_MODULE		"Str"
//------------------------------------------------------------------------
//	Str Class
//------------------------------------------------------------------------
//	The Str class is the base class for string operations.
//------------------------------------------------------------------------
//	Data Types for basic strings and characters
//------------------------------------------------------------------------
// Basic Strings
// -------------
// A basic string is a null terminated array of bytes.
// In the simple case of an ASCII or ISO LATIN1 string, each byte encodes a character.
// More complex coding schemes have been developed for Asian languages to
// support a larger set of characters. In such schemes a string might be a
// mixture of single byte and double byte characters.
// Open Interface can be configured for various character coding schemes.
// See charpub.h for a description of the environment variables which control
// how strings are encoded.
// The basic string types are:
// NatStr
//	zero terminated string, in "native" encoding, as defined by the
//	ND_CHARNATIVE environment variable.
// Str
//	zero terminated string, in "internal" code, as defined by the
//	combination of ND_CHARNATIVE and ND_CHARGLOBAL (see charpub.h	for details).
// UniStr
//	UNICODE string.
// Note: these types are actually defined in charpub.h.
// Strings vs Binary Data
// ----------------------
// The Str module deals with strings (text intended for humans), not with
// arbitrary binary data. So you should not use the Char or Str type when
// dealing with binary data, you should use the VoidPtr, Byte or BytePtr types.
// Indexing Strings
// ----------------
// The various string types are defined as "huge" pointers. This type
// qualifier is only relevant in segmented architectures such as DOS or OS/2.
// By considering all strings as "huge" we avoid many complications with
// strings which are larger than 32 KBytes.
// StrIVal
//	integer type to index strings. A 32 bit integer is necessary
//	to support "huge" strings.
//	This type is defined in charpub.h.
// Characters
// ----------
// The character types are described in detail in charpub.h
// Here is a summary of the main character types:
// NatChar	"native" string byte.
// NatCode	"native" character code (encodes multi-byte characters)
// Char		"internal" string byte.
// ChCode	"internal" character code (encodes multi-byte characters)
// UniCode	UNICODE character code.
// CharInfoVal	domain + level + lexical cat + case info + ascii-ness + ...
// Code types and code sets
// ------------------------
// The charpub.h header files gives detailed information about these topics.
// Basic Strings, Substrings and higher level String Objects
// ---------------------------------------------------------
// C represents strings as null terminated array of bytes.
// This representation is very practical for some operations such as
// comparisons and searches but also has some drawbacks:
//	- the lengths of strings must be recomputed instead of being cached
//	in string objects.
//	- substrings are difficult to manipulate because they are properly
//	terminated only if they are at the end of a string.
//	- it is difficult to design practical APIs for operations which
//	modify strings because the string pointer may need to be reallocated
//	(and it may be stack allocated to start with).
//
// The Str API actually consists of two APIs:
//
//	- a compatibility API, which is usually very close to the C RTL
//	API. In this part of the API, the naming conventions are not always
// 	very consistent and a number of calls are "unsafe" (calls which write
//	into string buffers and may overflow).
//
//	- a "new" API, with more consistent naming conventions and only
//	"safe" calls.
//
// You are encouraged to use the "new" API rather than the compatibility API.
// In this API, we have tried to follow the following principles:
//
//	- most calls are provided in two versions: an Str version
//	in which strings are passed as simple pointers and a SubStr version in
//	which the strings and the lengths are passed as separate arguments.
//	In such calls, the "substrings" are not necessarily terminated by a
//	null at the specified length. Also, in all the SubStr calls, a length
//	of -1 is interpreted as an unknown length, in which case the
//	terminating null will be used as an end of string indicator.
//	The SubStr calls also stop if a null is encountered before the length
//	which was specified.
//
//	- operations which write into string buffers receive a pointer to the
//	buffer and the size of the buffer. They never overflow the destination
//	buffer and always (except when specified otherwise) terminate their
//	output with a null byte.
//
//
// Higher level String Objects and APIs
// ------------------------------------
//
// Open Interface introduces the following high level "string objects":
//
//	- VStr is a general purpose and compact string object which handles
//	buffer reallocation automatically.
//
//	- SBuf is a larger string object which keeps track of a "gap" inside
//	the string, so that successive insertions can be performed
//	efficiently, even at the beginning of a string.
//	This object is designed to support complex string manipulations
//	(insertions, deletions, formatting) in an efficient way.
//
// The VStr object is a good "storage" object, the SBuf object a good "work"
// object. Only a limited set of operations are provided on the VStr object
// (append, format). The SBuf API is much more complete and also provides
// a simple API to temporarily attach an SBuf object to a VStr object or to
// a stack buffer and to detach it afterwards.
//------------------------------------------------------------------------
//	Cloning and disposing strings
//------------------------------------------------------------------------
// Note: we recommend that you use VStr objects rather than simple Str
// pointers for dynamically allocated strings. The VStr object encapsulates
// the length of the string and thus avoids useless length recomputations.
// static  Str NDStr::NewSet(CStr str1);
// static  Str NDStr::Clone(CStr str1);
//	Returns a new string, copy of `str1'.
//	STR_Clone is just an alias for STR_NewSet.
// static  Str NDStr::NewSetSub(CStr str1, StrIVal len);
//	returns a new string containing the `len' first bytes of `str1'.
// static  void NDStr::Dispose(Str str);
//	Dispose the string buffer.
// static  void NDStr::Dispose0(Str str);
//	Dispose a string buffer if not NULL.
//------------------------------------------------------------------------
//	Set and Append
//------------------------------------------------------------------------
// The following routines allow you to change the contents of a string
// or to append a string to an existing string.
// They take the address of a string as first argument so that they can
// reallocate the string if necessary.
// You are encouraged to use the VStr or SBuf modules when performing
// complex string manipulations. These calls should be reserved for
// simple cases only.
// 
// static  void NDStr::Set(StrPtr str, CStr cstr);
// static  void NDStr::SetSub(StrPtr str, CStr cstr, StrIVal strival);
// static  void NDStr::Append(StrPtr str, CStr cstr);
// static  void NDStr::AppendSub(StrPtr str, CStr cstr, StrIVal strival);
//------------------------------------------------------------------------
//	String length
//------------------------------------------------------------------------
#define STR_SizeOf(type)	((StrIVal)sizeof(type))
// static  StrIVal NDStr::GetLen(CStr str);
//	Return the length in bytes of the string (not including	terminating 0)
// static  StrIVal NDStr::GetTruncLen(CStr cstr, StrIVal size);
//	Returns the number of bytes of the string which may be copied into a
//	buffer of size `size'. If the length of the string is larger than
//	`size', the returned value is not necessarily `size' - 1 because the
//	truncation might occur on a multi-byte character.
//------------------------------------------------------------------------
//	Iterating through strings
//------------------------------------------------------------------------
// Iterating through strings requires some special care because strings
// may contain multi-byte characters.
// One way is to access the string byte by byte (Char by Char) and to
// get the length of characters by calling STR_GetLen.
//
// Another (safer) way is to use the following API calls which return
// character codes and advance the index in the string.
//
//
// To iterate forwards in a string, you should use STR_GetFwrd instead
// of reading byte by byte, except when you are only interested in ASCII
// characters and performance is critical (you can test the ASCII-ness first
// and call STR_GetLen only on non ASCII characters).
//
// To iterate backwards, you have no other choice than using STR_GetBwrd
// because iterating backwards is not a straightforward operation in general.
// For example, the SJIS code type allows ASCII letters as second byte of
// multi-byte characters, so the length of a character cannot be derived
// simply from the value of its last byte.
//

// static  ChCode NDStr::GetCode(CStr str);
// static  ChCode NDStr::GetFwrd(CStr str, StrIValPtr lenp);
// static  ChCode NDStr::GetBwrd(CStr str, StrIVal i, StrIValPtr lenp);
//	STR_GetCode gets the code at the beginning of the string.
//	STR_GetFwrd gets the code at the beginning of the string and sets
// 	`*lenp' to the length of the character.
//	STR_GetBwrd gets the character code before index `i' and sets `*lenp'
//	to its length.
//	STR_GetCode and STR_GetFwrd return 0 when they reach the end of the
//	string.
//	STR_GetBwrd returns 0 if `i' is 0.
//	STR_GetFwrd and STR_GetBwrd do not test whether `lenp' is NULL or not
//	and always set `*lenp'. We did not want to penalize such low level
//	routines by an extra test which would be useless in most cases.
//	If you are not interested in the length of the character, you should
//	call STR_GetCode rather than STR_GetFwrd.
//	Also, STR_GetFwrd sets `*lenp' to 1 when it encounters a NULL character
//	but STR_GetBwrd sets `*lenp' to 0 when `i' is 0. It would have
//	been more consistent to have `GetFwrd' set `*lenp' to 0 at the end
//	of the string, but this would have implied an additional test in
//	every call to STR_GetFwrd.
// 

// static  NatCode NDStr::CtGetCode(NatCStr nStr, CtCPtr ct);
// static  NatCode NDStr::CtGetFwrd(NatCStr nStr, CtCPtr ct, StrIValPtr lenp);
// static  NatCode NDStr::CtGetBwrd(NatCStr nStr, CtCPtr ct, StrIVal i, StrIValPtr lenp);
//	Similar to the previous calls but operate on strings encoded
//	with code type `ct'.
// 

//------------------------------------------------------------------------
//	Writing into string buffers
//------------------------------------------------------------------------

//
// The main problem when we are writing into string buffers is that
// what we write may be too big for the buffer and may cause an overflow.
// The C RTL routines such as strcpy, strcat or sprintf are unsafe because
// the caller cannot specify the size for which the buffer has been allocated
// and thus, in general, there is a risk of overflow.
//
// Most of the Str API implements operations which access strings in read-only
// mode but we also provide routines which write into string buffers, even
// if the preferred API for string manipulations is the SBuf API.
//
// The main routines are safe and return all the information necessary
// to find out if the operations resulted in a truncation or not.
// We also provide some routines which are unsafe or which do not indicate
// whether a truncation occured or not. These routines are provided mostly
// for compatibility purposes.
//
// The "safe" routines all use the same general API principle for the
// first two arguments and their return value. The principle can be
// illustrated taking the example of the STR_Put call.
//	 len = NdStr::Put(buf, size, str, endp)
// 	The convention used for the returned value may sound somewhat awkward
//	but is actually very practical when we have to concatenate various
//	values in a string. For example, we can write:
//		Char	buf[MAXSIZE];
//		Str	s	= buf;
//		StrIVal	size	= MAXSIZE;
//		StrIVal	len;
//		len = NdSTR::PutDecInt(s, size, i); 	s += len; size -= len;
//		len = NdStr::Put(s, size, ", ", NULL);	s += len; size -= len;
//		len = NdStr::PutDecInt(s, size, j);	s += len; size -= len;
// 	If an overflow occurs, the string will be properly truncated, size
// 	will become 0 and subsequent STR_Put calls will return immediately.
//	If you want to check the overflow condition, you can compare len and
// 	size after every STR_Put call (or compare size with 0 after the
//	increment/decrement operations). Then, you may reallocate the buffer
//	and retry the STR_Put operation.
//
// 	The `endp' argument is useful if you want to reallocate the buffer in
//	case of overflow and continue the Put operation with the remaining
//	string. If truncation is harmless, you do not need to worry about
//	reallocation and you can simply pass NULL as `endp'.
// 	In some calls, the `endp' argument is a little more complex
//	(for example
// 	in formatting routines, it describes a synchronization point between
//	the format and what has been written). In other calls, such as calls
//	which format numeric values, there is no need for an `endp' argument.

// static  StrIVal NDStr::Put(Str buf, StrIVal size, CStr str, StrIValPtr endp);
//	copies string `str' into `buf', truncating if `str' is too large
//	for `buf'.
//	`size' is the size for which `buf' has been allocated.
//	The routine always terminates `buf' with a null byte and never writes
//	more than `size' bytes into `buf' (including terminating null).
//	If the operation can be done without truncation, `len' will be the
//	number of characters which have been written to `buf', not including
//	the terminating null. In this case `len' is stricly less than `size'.
//	If the operation resulted in a truncation, `size' will be returned.
//	if `endp' is not NULL, `*endp' will be set to the number of characters
//	which have been copied (same as `len' when no truncation occured, less
//	than `len' otherwise).
// static  StrIVal NDStr::PutSub(Str buf, StrIVal size, CStr s, StrIVal slen, StrIValPtr endp);
//	"Puts" the `s', `slen' substring into `buf'.
// static  StrIVal NDStr::PutAscii(Str buf, StrIVal size, Char ch);
// static  StrIVal NDStr::PutCode(Str buf, StrIVal size, ChCode chcode);
//	Puts the ASCII character or the character code into buf.
//	In debugging version, PutAscii signals a failure if `ch' is not
//	an ASCII character.
// 	As all the other STR_Put calls, these last two calls terminate `buf'
//	with a null after `ch' or `chcode'. They are practical to append
// 	individual characters to a string but are not very efficient
// 	if we have to write many characters in a row
// 

// static  StrIVal NDStr::WriteAscii(Str str, StrIVal size, Char ch);
// static  StrIVal NDStr::WriteCode(Str str, StrIVal size, ChCode chcode);
//	Same as STR_Put calls but does not write a null after the character
//	(but writes a null and returns `size' in case character would
//	overflow). If you have to write many characters in a row, these
//	calls are more efficient than the previous STR_Put calls.
// 
// static  StrIVal NDStr::NatPutAscii(NatStr nStr, StrIVal size, NatChar nch);
// static  StrIVal NDStr::NatWriteAscii(NatStr nStr, StrIVal size, NatChar nch);
// static  StrIVal NDStr::NatPutCode(NatStr nStr, StrIVal size, NatCode ncode);
// static  StrIVal NDStr::NatWriteCode(NatStr nStr, StrIVal size, NatCode ncode);
//	Same as STR_Put and STR_WriteCode for native strings.
//	As we encode NatCode the same way as ChCode, we can share the
//	implementation.
// 
//------------------------------------------------------------------------
//	Basic String Comparisons
//------------------------------------------------------------------------
// static  CmpEnum NDStr::Cmp(CStr s1, CStr s2);
// static  CmpEnum NDStr::CmpSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2);
//	These routines compare strings by comparing bytes one by one.
//	They are very fast (usually implemented with C RTL strcmp and strncmp)
//	but sorting strings with these comparison routines will sort
//	lower case words after all upper case words, which is not very smart.
// 
// static  CmpEnum NDStr::ICmp(CStr s1, CStr s2);
// static  CmpEnum NDStr::ICmpSub(CStr s1, StrIVal l1, CStr c2, StrIVal l2l);
//	These routines compare strings by comparing bytes one by one but
//	ignoring case differences in the ASCII range.
//	In the ASCII range, characters are sorted based on the ASCII order,
//	even on an EBCDIC platform (so that '~' is not sorted between 'r' and
//	's').
//	These routines are fast too but do a poor job with extended ASCII
//	characters.
// 
// static  BoolEnum NDStr::Equals(CStr s1, CStr s2);
// static  BoolEnum NDStr::EqualsSub(CStr s1, StrIVal l1, CStr c2, StrIVal l2);
// static  BoolEnum NDStr::IEquals(CStr s1, CStr s2);
// static  BoolEnum NDStr::IEqualsSub(CStr s1, StrIVal l1, CStr c2, StrIVal l2);
//	Macros which return whether the strings are equal or not according
//	to the corresponding comparison criteria.
// 
//------------------------------------------------------------------------
//	Testing Matches
//------------------------------------------------------------------------
//
// The following routines or macros test whether a position in a string
// matches a character, string or pattern.
// Note: these routines replace avantageously strncmp and strnicmp to test
// that a string position matches a substring.

// static  BoolEnum NDStr::MatchesChar(CStr s1, ChCode chcode, StrIValPtr lenp);
// static  BoolEnum NDStr::Matches(CStr s1, CStr s2, StrIValPtr lenp);
// static  BoolEnum NDStr::MatchesPat(CStr s1, CStr s2, StrIValPtr lenp);
// static  BoolEnum NDStr::MatchesSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2, StrIValPtr lenp);
// static  BoolEnum NDStr::MatchesPatSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2, StrIValPtr lenp);
// static  BoolEnum NDStr::IMatches(CStr s1, CStr s2, StrIValPtr lenp);
// static  BoolEnum NDStr::IMatchesPat(CStr s1, CStr s2, StrIValPtr lenp);
// static  BoolEnum NDStr::IMatchesSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2, StrIValPtr lenp);
// static  BoolEnum NDStr::IMatchesPatSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2, StrIValPtr lenp);
//	These routines find if `s1' or the `s1', `l1' substring matches
// 	the specified character, string, substring or pattern.
//	The return value indicates if the match was successful.
//	The "patterns" are very simple patterns which only accept '?' to
//	indicate an optional character and '*' to indicate any substring.
//	More powerful "regular expressions" will be introduced in the future.
//	The basic calls compare exactly, the `I' calls ignore case differences
//	in the ASCII range only.
//	If `lenp' is not NULL, it will be set to the end of the match.
// 
//------------------------------------------------------------------------
//	Searching
//------------------------------------------------------------------------
// static  StrIVal NDStr::FindFirstChar(CStr s1, ChCode chcode);
// static  StrIVal NDStr::FindFirstCharSub(CStr s1, StrIVal l1, ChCode chcode);
// static  StrIVal NDStr::FindFirst(CStr s1, CStr s2);
// static  StrIVal NDStr::FindFirstSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2);
// static  StrIVal NDStr::IFindFirst(CStr s1, CStr s2);
// static  StrIVal NDStr::IFindFirstSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2);
// static  StrIVal NDStr::FindIFirst(CStr s1, CStr s2, BoolEnum caseI);
// static  StrIVal NDStr::FindIFirstSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2, BoolEnum caseI);
//	These routines search the character or substring in the specified
//	string (or substring).
//	They return the index at which the character or substring has been
//	found, -1 if the search failed.
//	The basic calls compare exactly, the STR_IFindFirst, .. calls ignore case
//	differences in the ASCII range only.
//	The STR_FindIFirst, ... calls ignore case in the ASCII range if `caseI' is
//	set to BOOL_TRUE.
//
// 	Note: for the simple calls, `endp' is not necessary because the length
//	 of the match is the length of the character or substring which is
//	searched.
// 
// static  StrIVal NDStr::FindLastChar(CStr s1, ChCode chcode);
// static  StrIVal NDStr::FindLastCharSub(CStr s1, StrIVal l1, ChCode chcode);
// static  StrIVal NDStr::FindLast(CStr s1, CStr s2);
// static  StrIVal NDStr::FindLastSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2);
// static  StrIVal NDStr::IFindLast(CStr s1, CStr s2);
// static  StrIVal NDStr::IFindLastSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2);
// static  StrIVal NDStr::FindILast(CStr s1, CStr s2, BoolEnum caseI);
// static  StrIVal NDStr::FindILastSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2, BoolEnum caseI);
// 	Similar to the STR_FindFirst calls, except that they return the index
//	of the last occurence of the specified character or substring.
//------------------------------------------------------------------------
//	Scanning of numeric values
//------------------------------------------------------------------------
// We provide variants of these calls for Int, Int32, Int16, UInt, UInt32 and
// UInt16. The naming is straightforward.
// The Int16, Int, UInt16 and UInt32 versions of the calls simply
// cast the result of the corresponding Int32 or UInt32 calls and do
// not signal over/underflows.
// static  Int32 NDStr::GetDecInt32(CStr s, StrIValPtr lenp);
// static  Int16 NDStr::GetDecInt16(CStr s, StrIValPtr lenp);
// static  Int NDStr::GetDecInt(CStr s, StrIValPtr lenp);
// static  UInt32 NDStr::GetDecUInt32(CStr s, StrIValPtr lenp);
// static  UInt16 NDStr::GetDecUInt16(CStr s, StrIValPtr lenp);
// static  UInt NDStr::GetDecUInt(CStr s, StrIValPtr lenp);
// static  Int32 NDStr::GetHexInt32(CStr s, StrIValPtr lenp);
// static  Int16 NDStr::GetHexInt16(CStr s, StrIValPtr lenp);
// static  Int NDStr::GetHexInt(CStr s, StrIValPtr lenp);
// static  UInt32 NDStr::GetHexUInt32(CStr s, StrIValPtr lenp);
// static  UInt16 NDStr::GetHexUInt16(CStr s, StrIValPtr lenp);
// static  UInt NDStr::GetHexUInt(CStr s, StrIValPtr lenp);
// static  Int32 NDStr::GetRadixInt32(CStr s, Int radix, StrIValPtr lenp);
// static  Int16 NDStr::GetRadixInt16(CStr s, Int radix, StrIValPtr lenp);
// static  Int NDStr::GetRadixInt(CStr s, Int i, StrIValPtr lenp);
// static  UInt32 NDStr::GetRadixUInt32(CStr s, Int radix, StrIValPtr lenp);
// static  UInt16 NDStr::GetRadixUInt16(CStr s, Int radix, StrIValPtr lenp);
// static  UInt NDStr::GetRadixUInt(CStr s, Int radix, StrIValPtr lenp);
// static  Double NDStr::GetDouble(CStr s, StrIValPtr lenp);

// static  Int32 NDStr::SubGetDecInt32(CStr s, StrIVal l, StrIValPtr lenp);
// static  Int16 NDStr::SubGetDecInt16(CStr s, StrIVal l, StrIValPtr lenp);
// static  Int NDStr::SubGetDecInt(CStr s, StrIVal l, StrIValPtr lenp);
// static  UInt32 NDStr::SubGetDecUInt32(CStr s, StrIVal l, StrIValPtr lenp);
// static  UInt16 NDStr::SubGetDecUInt16(CStr s, StrIVal l, StrIValPtr lenp);
// static  UInt NDStr::SubGetDecUInt(CStr s, StrIVal l, StrIValPtr lenp);
// static  Int32 NDStr::SubGetHexInt32(CStr s, StrIVal l, StrIValPtr lenp);
// static  Int16 NDStr::SubGetHexInt16(CStr s, StrIVal l, StrIValPtr lenp);
// static  Int NDStr::SubGetHexInt(CStr s, StrIVal l, StrIValPtr lenp);
// static  UInt32 NDStr::SubGetHexUInt32(CStr s, StrIVal l, StrIValPtr lenp);
// static  UInt16 NDStr::SubGetHexUInt16(CStr s, StrIVal l, StrIValPtr lenp);
// static  UInt NDStr::SubGetHexUInt(CStr s, StrIVal l, StrIValPtr lenp);
// static  Int32 NDStr::SubGetRadixInt32(CStr s, StrIVal l, Int radix, StrIValPtr lenp);
// static  Int16 NDStr::SubGetRadixInt16(CStr s, StrIVal l, Int radix, StrIValPtr lenp);
// static  Int NDStr::SubGetRadixInt(CStr s, StrIVal l, Int radix, StrIValPtr lenp);
// static  UInt32 NDStr::SubGetRadixUInt32(CStr s, StrIVal l, Int radix, StrIValPtr lenp);
// static  UInt16 NDStr::SubGetRadixUInt16(CStr s, StrIVal l, Int radix, StrIValPtr lenp);
// static  UInt NDStr::SubGetRadixUInt(CStr s, StrIVal l, Int radix, StrIValPtr lenp);
// static  Double NDStr::SubGetDouble(CStr s, StrIVal l, StrIValPtr lenp);
//	return the integer or real numeric value found at the beginning of `s'.
//	if `endp' is not NULL, `*endp' is set to the end of the numeric
//	substring.
//	The calls return 0 and set `*endp' to 0 if the string does not contain
//	a numeric value.
//	The `Dec' versions assume that the integer is expressed in decimal
//	notation, the `Hex' versions that it is expressed in hexadecimal
//	notation and the `Radix' versions allow you to specify the
//	radix.  `Radix' can be between 2 and 36 for Get methods.
//	Put methods only allow the value of `Radix' to be 8, 10 or 16.
//	The `Double' versions parse doubles.
//
//	Note:
//	the value returned in `*endp' allows the caller to find out if the
//	parsing was successful or not and also indicates where parsing
//	should resume.
//	Testing the success condition may be more complex than simply testing
//	that `*endp' is non zero. For example, we might want to check that the
//	character immediately following the numeric value is a space or that
//	the end of string has been reached. These low level routines do not
//	assume any special delimiter and stop as soon as they encounter a
//	character which cannot be part of the numeric format. Testing the
//	success of the operation is left to the caller.
// 
//------------------------------------------------------------------------
//	Formatting numeric values
//------------------------------------------------------------------------
//
// We provide variants of these calls for Int, Int32, Int16, UInt, UInt32 and
// UInt16. The naming is straightforward.
//
// static  StrIVal NDStr::PutDecInt32(Str buf, StrIVal size, Int32 i);
// static  StrIVal NDStr::PutDecInt16(Str buf, StrIVal size, Int16 i);
// static  StrIVal NDStr::PutDecInt(Str buf, StrIVal size, Int i);
// static  StrIVal NDStr::PutDecUInt32(Str buf, StrIVal size, UInt32 i);
// static  StrIVal NDStr::PutDecUInt16(Str buf, StrIVal size, UInt16 i);
// static  StrIVal NDStr::PutDecUInt(Str buf, StrIVal size, UInt i);
// static  StrIVal NDStr::PutHexInt32(Str buf, StrIVal size, Int32 i);
// static  StrIVal NDStr::PutHexInt16(Str buf, StrIVal size, Int16 i);
// static  StrIVal NDStr::PutHexInt(Str buf, StrIVal size, Int i);
// static  StrIVal NDStr::PutHexUInt32(Str buf, StrIVal size, UInt32 i);
// static  StrIVal NDStr::PutHexUInt16(Str buf, StrIVal size, UInt16 i);
// static  StrIVal NDStr::PutHexUInt(Str buf, StrIVal size, UInt i);
// static  StrIVal NDStr::PutRadixInt32(Str buf, StrIVal size, Int radix, Int32 i);
// static  StrIVal NDStr::PutRadixInt16(Str buf, StrIVal size, Int radix, Int16 i);
// static  StrIVal NDStr::PutRadixInt(Str buf, StrIVal size, Int radix, Int i);
// static  StrIVal NDStr::PutRadixUInt32(Str buf, StrIVal size, Int radix, UInt32 i);
// static  StrIVal NDStr::PutRadixUInt16(Str buf, StrIVal size, Int radix, UInt16 i);
// static  StrIVal NDStr::PutRadixUInt(Str buf, StrIVal size, Int radix, UInt i);
// static  StrIVal NDStr::PutDouble(Str buf, StrIVal size, Double d);
// 	These routines format the numeric value `i' or `d' into `buf'.
//	The `buf' and `size' arguments and the `len' returned are described
//	in detail in the STR_Put call above.
//	Put methods only allow the value of `Radix' to be 8, 10 or 16.
// 

//------------------------------------------------------------------------
//	Basic Conversions
//------------------------------------------------------------------------

// static  void NDStr::AsciiUpCase(Str s);
// static  void NDStr::AsciiUpCaseSub(Str s, StrIVal l);
// static  void NDStr::AsciiDownCase(Str s);
// static  void NDStr::AsciiDownCaseSub(Str s, StrIVal l);
//	Converts string or substring by converting only ASCII letters.
//	The string is converted in place.
// 

// static  StrIVal NDStr::PutAsciiUpper(Str buf, StrIVal len, CStr str, StrIValPtr lenp);
// static  StrIVal NDStr::PutAsciiLower(Str buf, StrIVal len, CStr str, StrIValPtr lenp);
// static  StrIVal NDStr::PutAsciiUpperSub(Str buf, StrIVal len, CStr str, StrIVal slen, StrIValPtr lenp);
// static  StrIVal NDStr::PutAsciiLowerSub(Str buf, StrIVal len, CStr str, StrIVal slen, StrIValPtr lenp);
//	Same as STR_Put but perform the indicated conversion
//	(ASCII only conversion or simple code conversion).
// 
//------------------------------------------------------------------------
//	Conversions between code types
//------------------------------------------------------------------------
typedef struct NDStrCvtCtx C_FAR*StrCvtCtxPtr;
typedef const struct NDStrCvtCtx C_FAR*StrCvtCtxCPtr;
typedef struct NDStrCvtCtx C_FAR*StrCvtCtxRef;
typedef const struct NDStrCvtCtx C_FAR*StrCvtCtxCRef;
typedef struct NDStrCvtCtx C_FAR*NDStrCvtCtxPtr;
typedef const struct NDStrCvtCtx C_FAR*NDStrCvtCtxCPtr;
typedef struct NDStrCvtCtx C_FAR&NDStrCvtCtxRef;
typedef const struct NDStrCvtCtx C_FAR&NDStrCvtCtxCRef;
typedef struct NDStrCvtCtx StrCvtCtxRec;
typedef const struct NDStrCvtCtx StrCvtCtxCRec;
struct NDStrCvtCtx {StrIVal	SrcPos;StrIVal DstPos;};
// static  StrIVal NDStr::FromCt(Str buf, StrIVal size, NatCStr ctstr, CtCPtr ct, StrCvtCtxPtr ctx);
// static  StrIVal NDStr::FromCtSub(Str buf, StrIVal size, NatCStr ctbuf, StrIVal ctslen, CtCPtr ct, StrCvtCtxPtr ctx);
// static  StrIVal NDStr::ToCt(NatStr ctbuf, StrIVal size, CStr str, CtCPtr ct, StrCvtCtxPtr ctx);
// static  StrIVal NDStr::ToCtSub(NatStr ctbuf, StrIVal size, CStr str, StrIVal slen, CtCPtr ct, StrCvtCtxPtr ctx);
//	converts `ctstr', a `ct' encoded string to `buf', an Str,
//	or `str', an Str to `ctbuf', a `ct' encoded buffer.
//	The conventions for `buf/ctbuf', `size' and `len' are the standard
//	`Put' conventions (see above).
//	If `ct' is NULL, the native code type is assumed.
//	`ctx' may be NULL or a pointer to an StrCvtCtx structure which will
//	be filled with the `buf' and `str' positions where the conversion
//	can be resumed after the destination buffer has been reallocated.
//
// Note: the STR_ToCt may stop converting if `str' contains characters
// which do not belong to the `ct' code set. In this case `ctx->FmtPos'
// will be less than the length of `str'. This will only happen if your
// application mixes strings encoded in various code sets.
// When this happens, you should inquire the code set of the offending
// character and resume conversion with a code type which covers this
// code set. An alternative is to ignore offending characters and
// eventually replace them with a "missing" character (i.e. ?).
// 
// static  StrIVal NDStr::FromUni(Str buf, StrIVal size, UniCStr unistr, CharCvtSet flags, StrCvtCtxPtr ctx);
// static  StrIVal NDStr::FromUniSub(Str buf, StrIVal size, UniCStr unistr, StrIVal unilen, CharCvtSet flags, StrCvtCtxPtr ctx);
// static  StrIVal NDStr::ToUni(UniStr unistr, StrIVal unisize, CStr str, CharCvtSet flags, StrCvtCtxPtr ctx);
// static  StrIVal NDStr::ToUniSub(UniStr unistr, StrIVal unisize, CStr str, StrIVal slen, CharCvtSet flags, StrCvtCtxPtr ctx);
//	routines to convert between internal string (Str) and UNICODE (UniStr).
//	The `unisize' and `unilen' are sizes and length in number of 16 bit
//	integers, not in number of bytes.
//	The `flags' allow to specify conversion flags to control UNICODE
//	specific conversion options such as precomposed vs decomposed form,
//	compatibility area mapping, ...
//	Only the UNICODE specific conversion flags are supported here.
//	Other types of conversions (i.e. case conversions) are only supported
//	on the Str type.
//------------------------------------------------------------------------
//	Loading from resources
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
typedef const Int32 C_FAR&NDArrayIValCRef;
#endif	// _ARRAYIVAL_ 
//	Forward definition: type for the index of an entry in
//	an array. See arptrpub.h, arrecpub.h, arnumpub.h.
// 
// static  CStr NDStr::ResLoad(CStr mod, CStr res, StrIValPtr lenp);
// static  CStr NDStr::ResFind(CStr mod, CStr res, StrIValPtr lenp);
//	return the string contained in the StrR resource `mod.res'.
//	If `lenp' is not NULL, it is set to the length of the returned
//	string.
//	If the resource does not exist, STR_ResLoad signals an error
//	and STR_ResFind returns NULL.
// 
// static  CStr NDStr::ResLoadNth(CStr mod, CStr res, ArrayIVal n, StrIValPtr lenp);
// static  CStr NDStr::ResFindNth(CStr mod, CStr res, ArrayIVal n, StrIValPtr lenp);
//	return the string number `n' in the StrL resource `mod.res'.
//	If `lenp' is not NULL, it is set to the length of of the returned	string.
//	If the resource does not exist, STR_ResLoadNth signals an error
//	and  STR_ResFindNth returns NULL.
//------------------------------------------------------------------------
//	Compatibility: Formatting and scanning
//------------------------------------------------------------------------
// These macros or routines are simply portable versions of the C RTL routines:
// static  void NDStr::Vsprintf(Str buf, CStr fmt, va_list va);
//	same as vsprintf but returns void.
// static  void NDStr::Vprintf(CStr fmt, va_list va);
//	If there is a `standard output' available same as vprintf
//	but returns void.
//
//	If there is no `standard output' available to the application
//	the output is written to: a log file, an alert window
//	on the screen, or to both a log file and a console window
//	if the application can create a console window.
//
//	An application which has no `standard output' and is running on
//	a platform which allows it to create a console window
//	will create a console window on the first call to STR_Vprintf
//	unless the console window is disabled
//	by setting the environment variable ND_CONSOLE=FALSE.
//------------------------------------------------------------------------
//	Enable/Disable use of console window by the string module
//------------------------------------------------------------------------
// static  void NDStr::SetConsoleWindowEnabled(BoolEnum enabled);
// If `enabled' == BOOL_TRUE enables use of a console window for
// platforms which need to create a console window to display strings
// printed via the string module.
// When the console window is enabled strings are written to both the
// console window and nd.dbg.
// By default the console window is enabled.  It can be disabled
// by default by setting the environment variable ND_CONSOLE=FALSE.
// If the application is a console application, i.e. it does not
// need to create a console window to display messages,
// STR_SetConsoleWindowEnabled will have no effect on the behaviour of
// printing by the string module.
// static  BoolEnum NDStr::GetConsoleWindowEnabled(void);
// Returns BOOL_TRUE if the use of a console window by the string module
// is enabled. Returns BOOL_FALSE if it is disabled.
//------------------------------------------------------------------------
//	Str class implementation
//------------------------------------------------------------------------
#if MCH_OS == MCH_OSWIN32
#include <wtypes.h>
#include <winbase.h>
#endif // MCH_OS == MCH_OSWIN32 
typedef class NDStr StrRec;
typedef const class NDStr StrCRec;
class C_DECLIMPORT NDStr  {
public:
	static  Str NewSet(CStr str1);
	static  Str Clone(CStr str1);
	static  Str NewSetSub(CStr str1, StrIVal len);
	static  void Dispose(Str str);
	static  void Dispose0(Str str);
	static  void Set(StrPtr str, CStr cstr);
	static  void SetSub(StrPtr str, CStr cstr, StrIVal strival);
	static  void Append(StrPtr str, CStr cstr);
	static  void AppendSub(StrPtr str, CStr cstr, StrIVal strival);
	static  StrIVal GetLen(CStr str);
	static  StrIVal GetTruncLen(CStr cstr, StrIVal size);
	static  ChCode GetCode(CStr str);
	static  ChCode GetFwrd(CStr str, StrIValPtr lenp);
	static  ChCode GetBwrd(CStr str, StrIVal i, StrIValPtr lenp);
	static  NatCode CtGetCode(NatCStr nStr, CtCPtr ct);
	static  NatCode CtGetFwrd(NatCStr nStr, CtCPtr ct, StrIValPtr lenp);
	static  NatCode CtGetBwrd(NatCStr nStr, CtCPtr ct, StrIVal i, StrIValPtr lenp);
	static  StrIVal Put(Str buf, StrIVal size, CStr str, StrIValPtr endp);
	static  StrIVal PutSub(Str buf, StrIVal size, CStr s, StrIVal slen, StrIValPtr endp);
	static  StrIVal PutAscii(Str buf, StrIVal size, Char ch);
	static  StrIVal PutCode(Str buf, StrIVal size, ChCode chcode);
	static  StrIVal WriteAscii(Str str, StrIVal size, Char ch);
	static  StrIVal WriteCode(Str str, StrIVal size, ChCode chcode);
	static  StrIVal NatPutAscii(NatStr nStr, StrIVal size, NatChar nch);
	static  StrIVal NatWriteAscii(NatStr nStr, StrIVal size, NatChar nch);
	static  StrIVal NatPutCode(NatStr nStr, StrIVal size, NatCode ncode);
	static  StrIVal NatWriteCode(NatStr nStr, StrIVal size, NatCode ncode);
	static  CmpEnum Cmp(CStr s1, CStr s2);
	static  CmpEnum CmpSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2);
	static  CmpEnum ICmp(CStr s1, CStr s2);
	static  CmpEnum ICmpSub(CStr s1, StrIVal l1, CStr c2, StrIVal l2l);
	static  BoolEnum Equals(CStr s1, CStr s2);
	static  BoolEnum EqualsSub(CStr s1, StrIVal l1, CStr c2, StrIVal l2);
	static  BoolEnum IEquals(CStr s1, CStr s2);
	static  BoolEnum IEqualsSub(CStr s1, StrIVal l1, CStr c2, StrIVal l2);
	static  BoolEnum MatchesChar(CStr s1, ChCode chcode, StrIValPtr lenp);
	static  BoolEnum Matches(CStr s1, CStr s2, StrIValPtr lenp);
	static  BoolEnum MatchesPat(CStr s1, CStr s2, StrIValPtr lenp);
	static  BoolEnum MatchesSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2, StrIValPtr lenp);
	static  BoolEnum MatchesPatSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2, StrIValPtr lenp);
	static  BoolEnum IMatches(CStr s1, CStr s2, StrIValPtr lenp);
	static  BoolEnum IMatchesPat(CStr s1, CStr s2, StrIValPtr lenp);
	static  BoolEnum IMatchesSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2, StrIValPtr lenp);
	static  BoolEnum IMatchesPatSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2, StrIValPtr lenp);
	static  StrIVal FindFirstChar(CStr s1, ChCode chcode);
	static  StrIVal FindFirstCharSub(CStr s1, StrIVal l1, ChCode chcode);
	static  StrIVal FindFirst(CStr s1, CStr s2);
	static  StrIVal FindFirstSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2);
	static  StrIVal IFindFirst(CStr s1, CStr s2);
	static  StrIVal IFindFirstSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2);
	static  StrIVal FindIFirst(CStr s1, CStr s2, BoolEnum caseI);
	static  StrIVal FindIFirstSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2, BoolEnum caseI);
	static  StrIVal FindLastChar(CStr s1, ChCode chcode);
	static  StrIVal FindLastCharSub(CStr s1, StrIVal l1, ChCode chcode);
	static  StrIVal FindLast(CStr s1, CStr s2);
	static  StrIVal FindLastSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2);
	static  StrIVal IFindLast(CStr s1, CStr s2);
	static  StrIVal IFindLastSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2);
	static  StrIVal FindILast(CStr s1, CStr s2, BoolEnum caseI);
	static  StrIVal FindILastSub(CStr s1, StrIVal l1, CStr s2, StrIVal l2, BoolEnum caseI);
	static  Int32 GetDecInt32(CStr s, StrIValPtr lenp);
	static  Int16 GetDecInt16(CStr s, StrIValPtr lenp);
	static  Int GetDecInt(CStr s, StrIValPtr lenp);
	static  UInt32 GetDecUInt32(CStr s, StrIValPtr lenp);
	static  UInt16 GetDecUInt16(CStr s, StrIValPtr lenp);
	static  UInt GetDecUInt(CStr s, StrIValPtr lenp);
	static  Int32 GetHexInt32(CStr s, StrIValPtr lenp);
	static  Int16 GetHexInt16(CStr s, StrIValPtr lenp);
	static  Int GetHexInt(CStr s, StrIValPtr lenp);
	static  UInt32 GetHexUInt32(CStr s, StrIValPtr lenp);
	static  UInt16 GetHexUInt16(CStr s, StrIValPtr lenp);
	static  UInt GetHexUInt(CStr s, StrIValPtr lenp);
	static  Int32 GetRadixInt32(CStr s, Int radix, StrIValPtr lenp);
	static  Int16 GetRadixInt16(CStr s, Int radix, StrIValPtr lenp);
	static  Int GetRadixInt(CStr s, Int i, StrIValPtr lenp);
	static  UInt32 GetRadixUInt32(CStr s, Int radix, StrIValPtr lenp);
	static  UInt16 GetRadixUInt16(CStr s, Int radix, StrIValPtr lenp);
	static  UInt GetRadixUInt(CStr s, Int radix, StrIValPtr lenp);
	static  Double GetDouble(CStr s, StrIValPtr lenp);
	static  Int32 SubGetDecInt32(CStr s, StrIVal l, StrIValPtr lenp);
	static  Int16 SubGetDecInt16(CStr s, StrIVal l, StrIValPtr lenp);
	static  Int SubGetDecInt(CStr s, StrIVal l, StrIValPtr lenp);
	static  UInt32 SubGetDecUInt32(CStr s, StrIVal l, StrIValPtr lenp);
	static  UInt16 SubGetDecUInt16(CStr s, StrIVal l, StrIValPtr lenp);
	static  UInt SubGetDecUInt(CStr s, StrIVal l, StrIValPtr lenp);
	static  Int32 SubGetHexInt32(CStr s, StrIVal l, StrIValPtr lenp);
	static  Int16 SubGetHexInt16(CStr s, StrIVal l, StrIValPtr lenp);
	static  Int SubGetHexInt(CStr s, StrIVal l, StrIValPtr lenp);
	static  UInt32 SubGetHexUInt32(CStr s, StrIVal l, StrIValPtr lenp);
	static  UInt16 SubGetHexUInt16(CStr s, StrIVal l, StrIValPtr lenp);
	static  UInt SubGetHexUInt(CStr s, StrIVal l, StrIValPtr lenp);
	static  Int32 SubGetRadixInt32(CStr s, StrIVal l, Int radix, StrIValPtr lenp);
	static  Int16 SubGetRadixInt16(CStr s, StrIVal l, Int radix, StrIValPtr lenp);
	static  Int SubGetRadixInt(CStr s, StrIVal l, Int radix, StrIValPtr lenp);
	static  UInt32 SubGetRadixUInt32(CStr s, StrIVal l, Int radix, StrIValPtr lenp);
	static  UInt16 SubGetRadixUInt16(CStr s, StrIVal l, Int radix, StrIValPtr lenp);
	static  UInt SubGetRadixUInt(CStr s, StrIVal l, Int radix, StrIValPtr lenp);
	static  Double SubGetDouble(CStr s, StrIVal l, StrIValPtr lenp);
	static  StrIVal PutDecInt32(Str buf, StrIVal size, Int32 i);
	static  StrIVal PutDecInt16(Str buf, StrIVal size, Int16 i);
	static  StrIVal PutDecInt(Str buf, StrIVal size, Int i);
	static  StrIVal PutDecUInt32(Str buf, StrIVal size, UInt32 i);
	static  StrIVal PutDecUInt16(Str buf, StrIVal size, UInt16 i);
	static  StrIVal PutDecUInt(Str buf, StrIVal size, UInt i);
	static  StrIVal PutHexInt32(Str buf, StrIVal size, Int32 i);
	static  StrIVal PutHexInt16(Str buf, StrIVal size, Int16 i);
	static  StrIVal PutHexInt(Str buf, StrIVal size, Int i);
	static  StrIVal PutHexUInt32(Str buf, StrIVal size, UInt32 i);
	static  StrIVal PutHexUInt16(Str buf, StrIVal size, UInt16 i);
	static  StrIVal PutHexUInt(Str buf, StrIVal size, UInt i);
	static  StrIVal PutRadixInt32(Str buf, StrIVal size, Int radix, Int32 i);
	static  StrIVal PutRadixInt16(Str buf, StrIVal size, Int radix, Int16 i);
	static  StrIVal PutRadixInt(Str buf, StrIVal size, Int radix, Int i);
	static  StrIVal PutRadixUInt32(Str buf, StrIVal size, Int radix, UInt32 i);
	static  StrIVal PutRadixUInt16(Str buf, StrIVal size, Int radix, UInt16 i);
	static  StrIVal PutRadixUInt(Str buf, StrIVal size, Int radix, UInt i);
	static  StrIVal PutDouble(Str buf, StrIVal size, Double d);
	static  void AsciiUpCase(Str s);
	static  void AsciiUpCaseSub(Str s, StrIVal l);
	static  void AsciiDownCase(Str s);
	static  void AsciiDownCaseSub(Str s, StrIVal l);
	static  StrIVal PutAsciiUpper(Str buf, StrIVal len, CStr str, StrIValPtr lenp);
	static  StrIVal PutAsciiLower(Str buf, StrIVal len, CStr str, StrIValPtr lenp);
	static  StrIVal PutAsciiUpperSub(Str buf, StrIVal len, CStr str, StrIVal slen, StrIValPtr lenp);
	static  StrIVal PutAsciiLowerSub(Str buf, StrIVal len, CStr str, StrIVal slen, StrIValPtr lenp);
	static  StrIVal FromCt(Str buf, StrIVal size, NatCStr ctstr, CtCPtr ct, StrCvtCtxPtr ctx);
	static  StrIVal FromCtSub(Str buf, StrIVal size, NatCStr ctbuf, StrIVal ctslen, CtCPtr ct, StrCvtCtxPtr ctx);
	static  StrIVal ToCt(NatStr ctbuf, StrIVal size, CStr str, CtCPtr ct, StrCvtCtxPtr ctx);
	static  StrIVal ToCtSub(NatStr ctbuf, StrIVal size, CStr str, StrIVal slen, CtCPtr ct, StrCvtCtxPtr ctx);
	static  StrIVal FromUni(Str buf, StrIVal size, UniCStr unistr, CharCvtSet flags, StrCvtCtxPtr ctx);
	static  StrIVal FromUniSub(Str buf, StrIVal size, UniCStr unistr, StrIVal unilen, CharCvtSet flags, StrCvtCtxPtr ctx);
	static  StrIVal ToUni(UniStr unistr, StrIVal unisize, CStr str, CharCvtSet flags, StrCvtCtxPtr ctx);
	static  StrIVal ToUniSub(UniStr unistr, StrIVal unisize, CStr str, StrIVal slen, CharCvtSet flags, StrCvtCtxPtr ctx);
	static  CStr ResLoad(CStr mod, CStr res, StrIValPtr lenp);
	static  CStr ResFind(CStr mod, CStr res, StrIValPtr lenp);
	static  CStr ResLoadNth(CStr mod, CStr res, ArrayIVal n, StrIValPtr lenp);
	static  CStr ResFindNth(CStr mod, CStr res, ArrayIVal n, StrIValPtr lenp);
	static  void Vsprintf(Str buf, CStr fmt, va_list va);
	static  void Vprintf(CStr fmt, va_list va);
	static  void SetConsoleWindowEnabled(BoolEnum enabled);
	static  BoolEnum GetConsoleWindowEnabled(void);

};

#define STR_REC

#endif // _STRPUB_ 
