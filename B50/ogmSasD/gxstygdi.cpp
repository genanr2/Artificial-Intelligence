// Author: Stefan Hoenig
// gxstygdi.cpp : implementation of font, brush and pen objects
#include "stdafx.h"
#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif
// Headers
#ifndef _GXRESRC_H_
#include "gxresrc.h"
#endif
#ifndef _GXSTYLES_H_
#include "gxstyles.h"
#endif
#ifndef _GXSTATE_H_
#include "gxstate.h"
#endif
#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif
#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXSTYGDI")
#endif
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
IMPLEMENT_SERIAL(CGXFont, CObject, 0 /* schema number*/ )
#define new DEBUG_NEW
#define _countof(array) (sizeof(array)/sizeof(array[0]))
///////////////////////////////////////////////////////////////////
// struct GX_FONT_STATE
#ifndef _GXDLL
GX_FONT_STATE GXNEAR _gxFontState;
#endif
CGXFontInfo::CGXFontInfo()
{
	memset(this, 0, sizeof(CGXFontInfo));nFontIndex = 1023;nFaceLen = 0;
}
struct CGXFontInfoArray
{
	CGXFontInfo* m_paFontInfo[256];  // max. 32K fonts (FaceNames without charset)
	int m_nSize;
	int m_nCount;
	int m_nMaxInfoArray;
	BOOL m_bScript;
	TCHAR lfFaceName[LF_FACESIZE];
};
static int _gxAddFont(LPCTSTR szFaceName,BOOL bDefaultCharset,int nFaceLen,LPLOGFONT lplf,
	LPTEXTMETRIC,int nFontType,CGXFontInfoArray*array)
{
	if (array->m_nCount == array->m_nSize)
	{
		if (array->m_nMaxInfoArray == 256)
			// more than 32K fonts not supported
			return 0;
		array->m_paFontInfo[array->m_nMaxInfoArray] = new CGXFontInfo[256];
		array->m_nSize += 256;
		array->m_nMaxInfoArray++;
	}
	CGXFontInfo* pInfo = &array->m_paFontInfo[array->m_nMaxInfoArray-1][array->m_nCount++%256];
	_tcscpy(pInfo->lfFaceName, szFaceName);
	pInfo->nFaceLen = (BYTE) max(0, nFaceLen);
	pInfo->nFontType = nFontType & 0x07;
	pInfo->nCharSet = lplf->lfCharSet;
	pInfo->lfPitchAndFamily = lplf->lfPitchAndFamily;
	pInfo->nDefaultCharset = (unsigned) bDefaultCharset;
	pInfo->nInstalled = 1;
	return 1;
}
#if _MFC_VER >= 0x0400
static TCHAR s_elfScript[256][LF_FACESIZE];
#endif
int FAR PASCAL AFX_EXPORT _gxfsEnumAllFaces(LPLOGFONT lplf,LPTEXTMETRIC lptm,int nFontType,CGXFontInfoArray*array)
{
	// Add one font without (charset) - others will have (charset) appended
	if (_tcscmp(lplf->lfFaceName, array->lfFaceName) != 0)
	{
		_gxAddFont(lplf->lfFaceName, TRUE, 0, lplf, lptm, nFontType, array);
	}
#if _MFC_VER >= 0x0400
	// Support for different charsets - append (charset) to facename, e.g. Arial (Western)
	LPENUMLOGFONTEX eplf = (LPENUMLOGFONTEX) lplf;
	TCHAR szFaceName[LF_FACESIZE*2+4]; 
	int len = _tcslen(lplf->lfFaceName);
	_tcscpy(s_elfScript[lplf->lfCharSet], (LPTSTR) eplf->elfScript);
	wsprintf(szFaceName, _T("%s (%s)"), lplf->lfFaceName, eplf->elfScript);
	_gxAddFont(szFaceName, FALSE, len, lplf, lptm, nFontType, array);
#endif
	_tcscpy(array->lfFaceName, lplf->lfFaceName);
	return 1;
}
#ifndef _WINDU_SOURCE
extern "C" static int __cdecl __gxcompareinfo(const void* p1, const void* p2)
#else
extern "C" int __cdecl __gxcompareinfo(const void* p1, const void* p2)
#endif
{
	return _tcscmp(((CGXFontInfo*)p1)->lfFaceName, ((CGXFontInfo*)p2)->lfFaceName);
}
#ifndef _WINDU_SOURCE
extern "C" static int __cdecl __gxcomparenotinstalledinfo(const void* p1, const void* p2)
#else
extern "C" int __cdecl __gxcomparenotinstalledinfo(const void* p1, const void* p2)
#endif
{
	CGXFontInfo* pp1 = *((CGXFontInfo**) p1);
	CGXFontInfo* pp2 = *((CGXFontInfo**) p2);
	return _tcscmp(pp1->lfFaceName, pp2->lfFaceName);
}
/////////////////////////////////////////////////////////////////////////////
#if _MFC_VER < 0x0300
// Turn off optiomization for VC 1.5, see MSDN KB Q113428
#pragma optimize("", off)
#endif
GX_FONT_STATE::GX_FONT_STATE()
{
	// zero data
	m_nFontInfoCount = 0;
	m_aFontInfo = NULL;			
	// check out facenames
	HDC hdc = ::CreateIC (_T("DISPLAY"), NULL, NULL, NULL) ;
	if (!hdc)
	{
		ASSERT(0);
		// ASSERTION-> Could not create Display-DC ->END
	}
	else
	{
		BOOL bUseOldFontEnum = TRUE;
		CGXFontInfoArray array;
		array.m_nSize = 0;
		array.m_nCount = 0;
		array.m_nMaxInfoArray = 0;
		array.m_bScript = 0;
		_tcscpy(array.lfFaceName, _T(""));
#if _MFC_VER >= 0x400 && !defined(_UNIX_) && !defined(_MAC)
		// Note: If the following code breaks with Unix or Macintosh
		// please let us know. We will then add the necessary ifdef
		// EnumFontFamiliesEx does not work with Win32s
		DWORD dwVersion = ::GetVersion();
		BOOL bWin32s = (dwVersion & 0x80000000) != 0;
		BOOL bWin4 = (BYTE)dwVersion >= 4;
		BOOL bWin31 = bWin32s && !bWin4; // Windows 95 reports Win32s
		if (!bWin31)
		{
			array.m_bScript = 1;
			LOGFONT lf;
			memset(&lf, 0, sizeof(LOGFONT));
			lf.lfCharSet =  DEFAULT_CHARSET;
			_tcscpy(lf.lfFaceName, _T("")); 
			lf.lfPitchAndFamily = 0;
			EnumFontFamiliesEx(hdc,&lf,(FONTENUMPROC) &_gxfsEnumAllFaces,	
				(LPARAM)(LPVOID)&array,0// reserved; must be zero
				);
		}
		// Call old EnumFontFamilies if function was not succesfull
		bUseOldFontEnum = array.m_nCount == 0;
#endif
		if (bUseOldFontEnum)
		{
			EnumFontFamilies (hdc,NULL,(FONTENUMPROC)&_gxfsEnumAllFaces,(LPARAM)(LPVOID) &array);}
		// Allocate font info array
		m_aFontInfo = new CGXFontInfo[array.m_nCount];
		m_nFontInfoCount = array.m_nCount;
		// copy elements from temporary structure
		CGXFontInfo* p = m_aFontInfo;
		for (int i = 0; i < array.m_nMaxInfoArray; i++)
		{
			int nSize = min(256, array.m_nCount);
			memcpy(p, array.m_paFontInfo[i], nSize*sizeof(CGXFontInfo));
			array.m_nCount -= 256;
			p += 256;
			delete array.m_paFontInfo[i];
		}
		// sort face names alphabetically so that we can later use bsearch
		qsort(m_aFontInfo, (size_t) m_nFontInfoCount, sizeof(CGXFontInfo), __gxcompareinfo);
		// make sure arial is the first font in cache
		LookupFontInfo(CGXFont::szArial);
		m_nLogPixelsY = ::GetDeviceCaps (hdc, LOGPIXELSY);
		m_nLogPixelsX = ::GetDeviceCaps (hdc, LOGPIXELSX);
		DeleteDC (hdc) ;
	}
	m_mapFontData.InitHashTable(1001);
}

GX_FONT_STATE::~GX_FONT_STATE()
{
	delete[] m_aFontInfo;

	for (int i = 0; i < m_apNotInstalledFontInfo.GetSize(); i++)
	{
		CGXFontInfo* pInfo=(CGXFontInfo*)m_apNotInstalledFontInfo[i];delete pInfo;
	}
}
#if _MFC_VER < 0x0300
#pragma optimize("", on)
#endif
#if _MFC_VER < 0x0400
// CPtrArray has no GetData() member
class CGXPtrArray: public CPtrArray
{                                         
public:void** GetData() { return m_pData; }
};
#else
#define CGXPtrArray CPtrArray
#endif	
unsigned GX_FONT_STATE::LookupFontInfo(LPCTSTR szFaceName, BYTE lfdefPitchAndFamily, BYTE ndefCharSet) 
{
	if (_tcslen(szFaceName) == 0)return 1023;
	// search installed fonts
	CGXFontInfo* pInfo = NULL;
	if (m_nFontInfoCount > 0)
		pInfo=(CGXFontInfo*) bsearch(szFaceName,m_aFontInfo, (size_t)m_nFontInfoCount, 
			sizeof(CGXFontInfo),__gxcompareinfo);
	// search not installed fonts
	if (pInfo == NULL)
	{
		CGXFontInfo tmpinfo;
		_tcscpy(tmpinfo.lfFaceName, szFaceName);
		CGXFontInfo* ptmpInfo = &tmpinfo;
		CGXPtrArray* apNotInstalledFontInfo = (CGXPtrArray*) &m_apNotInstalledFontInfo;
		CGXFontInfo** ppInfo = NULL;
		if (apNotInstalledFontInfo->GetSize() > 0)
			ppInfo = (CGXFontInfo**) 
			bsearch(&ptmpInfo,apNotInstalledFontInfo->GetData(), 
				(size_t) apNotInstalledFontInfo->GetSize(),sizeof(CGXFontInfo*), 
				__gxcomparenotinstalledinfo);
		if (ppInfo)pInfo = *ppInfo;
	}
	if (pInfo == NULL && m_apUsedFonts.GetSize() >= 1023)
		return 1023; // not found and too much fonts
	// not found -> add to not installed fonts 
	if (pInfo == NULL)
	{
		// allocate font and add it
		pInfo = new CGXFontInfo;
		// using default family add charset - the grid will try to display
		// a font with similar characteristic (e.g. symbol or japanese etc).
		_tcscpy(pInfo->lfFaceName, szFaceName);
		pInfo->lfPitchAndFamily = lfdefPitchAndFamily;
		pInfo->nCharSet = ndefCharSet;
		m_apNotInstalledFontInfo.SetAtGrow(m_apNotInstalledFontInfo.GetSize(), pInfo);
		CGXPtrArray* apNotInstalledFontInfo = (CGXPtrArray*) &m_apNotInstalledFontInfo;
		qsort(apNotInstalledFontInfo->GetData(), 
				(size_t) apNotInstalledFontInfo->GetSize(), 
				sizeof(CGXFontInfo*), 
				__gxcomparenotinstalledinfo);
	}
	// index
	if (pInfo->nFontIndex == 1023 && m_apUsedFonts.GetSize() < 1023)
	{
		pInfo->nFontIndex = m_apUsedFonts.GetSize();
		m_apUsedFonts.SetAtGrow(pInfo->nFontIndex, pInfo);
		// TRACE("%s - %d\n", pInfo->lfFaceName, pInfo->nFontIndex);
	}
	return pInfo->nFontIndex;  // not found, not included (see CGXFont)
}
CGXFontInfo* GX_FONT_STATE::GetFontInfo(unsigned nFont) const
{
	if (nFont >= (unsigned) m_apUsedFonts.GetSize())return NULL;
	return (CGXFontInfo*) m_apUsedFonts[nFont];
}
int GX_FONT_STATE::GetFontType(unsigned nFont) const
{
	if (nFont >= (unsigned) m_apUsedFonts.GetSize())return 0;
	return GetFontInfo(nFont)->nFontType;
}
BYTE GX_FONT_STATE::GetFontCharset(unsigned nFont) const
{
	if (nFont >= (unsigned) m_apUsedFonts.GetSize())return 0;
	return GetFontInfo(nFont)->nCharSet;
}
// constants
const DWORD ExcludeAll = 0;
const DWORD IncludeAll = 0xffff;
///////////////////////////////////////////////////////////////////
// CGXFont
unsigned AFXAPI CGXFont::Size_lf2ui(double lfSize)
{
	unsigned nSize = (unsigned) abs((int) (lfSize*4));
	return min(nSize, 1023);
}
double AFXAPI CGXFont::Size_ui2lf(unsigned nSize)
{
	double lfSize = nSize / 4.0;return lfSize;
}
/* Orientation in 15 degree steps, 511 - not included */
unsigned AFXAPI CGXFont::Orientation_lf2ui(LONG lfOrientation)
{
	unsigned nOrientation;
	while (lfOrientation < 0)lfOrientation += 3600;
	nOrientation = (unsigned) ((lfOrientation)/10);
	// ASSERT(nOrientation <= 24);
	return nOrientation;
}
LONG AFXAPI CGXFont::Orientation_ui2lf(unsigned nOrientation)
{
	// ASSERT(nOrientation <= 24);
	LONG lfOrientation;
	lfOrientation = (LONG) (nOrientation*10);
	return lfOrientation;
}
/* WEIGHT, 15 - not included
#define FW_DONTCARE         0
#define FW_THIN             100
#define FW_EXTRALIGHT       200
#define FW_LIGHT            300
#define FW_NORMAL           400
#define FW_MEDIUM           500
#define FW_SEMIBOLD         600
#define FW_BOLD             700
#define FW_EXTRABOLD        800
#define FW_HEAVY            900
*/
unsigned AFXAPI CGXFont::Weight_lf2ui(LONG lfWeight)
{
	unsigned nWeight;
	nWeight = (unsigned) (lfWeight / 100);ASSERT(nWeight <= 10);return nWeight;
}
LONG AFXAPI CGXFont::Weight_ui2lf(unsigned nWeight)
{
	LONG lfWeight;
	ASSERT(nWeight <= 10);
	lfWeight = (LONG) (nWeight*100);
	return lfWeight;
}
/* Facename - see array in GX_FONT_STATE, 0 - not included */
unsigned AFXAPI CGXFont::FaceName_str2ui(LPCTSTR lfFaceName)
{
	unsigned nFaceName;
	nFaceName = GXGetFontState()->LookupFontInfo(lfFaceName);
	return nFaceName;
}
LPCTSTR AFXAPI CGXFont::FaceName_ui2str(unsigned nFaceName)
{
	ASSERT(nFaceName < 1023);
	CGXFontInfo* pInfo = GXGetFontState()->GetFontInfo(nFaceName);
	ASSERT(pInfo);
	return pInfo->lfFaceName;
}
// constants
LPCTSTR CGXFont::szArial = _T("Arial Cyr");
CGXFont::CGXFont(const LOGFONT &lf)
{
#if _MFC_VER >= 0x400
	if (lf.lfCharSet != DEFAULT_CHARSET && lf.lfCharSet != ANSI_CHARSET)
	{
		TCHAR szFaceName[LF_FACESIZE*2+4]; 
		wsprintf(szFaceName, _T("%s (%s)"), lf.lfFaceName, s_elfScript[lf.lfCharSet]);
		fontbits.nFaceName = FaceName_str2ui(szFaceName);
	}
	else fontbits.nFaceName = FaceName_str2ui(lf.lfFaceName);
#else
	fontbits.nFaceName = FaceName_str2ui(lf.lfFaceName);
#endif
	fontbits.nSize = lf.lfHeight != 0 ? Size_lf2ui(MulDiv(abs(lf.lfHeight), 72*4, GXGetFontState()->m_nLogPixelsY)/4.0) : GX_DEFAULT_SIZE;
	fontbits.nWeight = Weight_lf2ui(lf.lfWeight);
	fontbits.nOrientation = Orientation_lf2ui(lf.lfOrientation);
	SetStrikeOut(lf.lfStrikeOut);
	SetUnderline(lf.lfUnderline);
	SetItalic(lf.lfItalic);
}
CGXFont::CGXFont(const CGXFont& font)
{
	m_b8Value = font.m_b8Value;
	if (fontbits.fontdata)
#if _MFC_VER >= 0x0300	
		InterlockedIncrement(&GXGetFontState()->m_mapFontData[GetFontKey()].nRefs);
#else
		GXGetFontState()->m_mapFontData[GetFontKey()].nRefs++;
#endif
}
void CGXFont::Release(GXBYTE8 key)
{
	CGXFontData* pData = GetData(key);
	if (fontbits.fontdata)
#if _MFC_VER >= 0x0300	
		InterlockedDecrement(&pData->nRefs);
#else
		pData->nRefs++;
#endif
	if (pData->nRefs == 0)
		GXGetFontState()->m_mapFontData.RemoveKey(key);
	fontbits.fontdata = 0;
}
CGXFontData* CGXFont::GetData(GXBYTE8 key) const
{
	CGXFontData* pData = &GXGetFontState()->m_mapFontData[key];
	// Init LOGFONT
	if (!fontbits.fontdata)
	{
		// Attach to FontData
#if _MFC_VER >= 0x0300	
		InterlockedIncrement(&pData->nRefs);
#else
		pData->nRefs++;
#endif
		// cast to non const
		CGXFont* pFont = (CGXFont*) this;
		pFont->fontbits.fontdata = 1;
		// Init LOGFONT
		if (!pData->bInit)
		{
			InitLogFont(pData->lf);
			pData->bInit = 1;
		}return pData;
	}
	else return pData;
	return pData;
}
CGXFont::~CGXFont(){Release();}
void CGXFont::InitLogFont(LOGFONT& lf) const
{
	memset(&lf, 0, sizeof(LOGFONT));
	if (GetIncludeItalic())lf.lfItalic = (BYTE) GetItalic();
	if (GetIncludeStrikeOut())lf.lfStrikeOut = (BYTE) GetStrikeOut();
	if (GetIncludeUnderline())lf.lfUnderline = (BYTE) GetUnderline();
	if (GetIncludeSize())
	{
		lf.lfHeight = -MulDiv((int) (Size_ui2lf(fontbits.nSize)*4), GXGetFontState()->m_nLogPixelsY, 72*4);
		// TRACE("InitLogFont: %d\n", lf.lfHeight);
	}
	if (GetIncludeWeight())lf.lfWeight = (int) GetWeight();
	if (GetIncludeOrientation())lf.lfEscapement = lf.lfOrientation = (int) GetOrientation();
	if (GetIncludeFaceName())
	{
		unsigned nFont = GetFaceNameId();
		CGXFontInfo* pInfo = GXGetFontState()->GetFontInfo(nFont);
		if (pInfo->nFaceLen == 0)_tcscpy(lf.lfFaceName, pInfo->lfFaceName);
		else
		{
			_tcsncpy(lf.lfFaceName, pInfo->lfFaceName, pInfo->nFaceLen);
			lf.lfFaceName[pInfo->nFaceLen] = 0;
		}
		lf.lfPitchAndFamily = (BYTE) (pInfo->lfPitchAndFamily | (lf.lfOrientation > 0 ? 0x04 : 0));
		lf.lfCharSet = pInfo->nCharSet;
	}
}
// LogFont
const LOGFONT& CGXFont::GetLogFontRef() const
{ 
	// TRACE("GetLogFontRef() %d - %d\n", GetSize(), GetData()->lf.lfHeight);
	return GetData()->lf; 
}
LOGFONT CGXFont::GetLogFont(int nZoom) const
{   
	LOGFONT lf = GetData()->lf;
	lf.lfHeight = lf.lfHeight*nZoom/100;
	return lf;
}
// LogFont
CGXFont& CGXFont::SetLogFont(const LOGFONT& lf){ *this = lf;return *this;}
CGXFont::operator LOGFONT() const{return GetLogFontRef();};
CGXFont::operator const LOGFONT*() const{return &GetLogFontRef();};
void CGXFont::Init()
{
	Release();
	memset(&m_b8Value, 0, sizeof(m_b8Value));
	fontbits.nOrientation = GX_DEFAULT_ORIENTATION;
	fontbits.nWeight = GX_DEFAULT_WEIGHT;
	fontbits.nFaceName = GX_DEFAULT_FACENAME;
}
const CGXFont& CGXFont::operator=(const CGXFont& font)
{
	// euqual font
	if (GetFontKey() == font.GetFontKey())return *this;
	Release();
	m_b8Value = font.m_b8Value;
	if (fontbits.fontdata)
#if _MFC_VER >= 0x0300	
		InterlockedIncrement(&GetData(GetFontKey())->nRefs);
#else
		GetData(GetFontKey())->nRefs++;
#endif
	return *this;
}
const CGXFont& CGXFont::operator=(const LOGFONT& lf)
{
	Release();
#if _MFC_VER >= 0x400
	if (lf.lfCharSet != DEFAULT_CHARSET && lf.lfCharSet != ANSI_CHARSET)
	{
		TCHAR szFaceName[LF_FACESIZE*2+4]; 
		wsprintf(szFaceName, _T("%s (%s)"), lf.lfFaceName, s_elfScript[lf.lfCharSet]);
		fontbits.nFaceName = FaceName_str2ui(szFaceName);
	}
	else
		fontbits.nFaceName = FaceName_str2ui(lf.lfFaceName);
#else
		fontbits.nFaceName = FaceName_str2ui(lf.lfFaceName);
#endif
	fontbits.nSize = lf.lfHeight != 0 ? Size_lf2ui(MulDiv(abs(lf.lfHeight), 72*4, GXGetFontState()->m_nLogPixelsY)/4.0) : GX_DEFAULT_SIZE;
	fontbits.nWeight = Weight_lf2ui(lf.lfWeight);
	fontbits.nOrientation = Orientation_lf2ui(lf.lfOrientation);
	SetStrikeOut(lf.lfStrikeOut);
	SetUnderline(lf.lfUnderline);
	SetItalic(lf.lfItalic);
	return *this;
}
CGXFont& CGXFont::SetDefault()
{
	Release(); 
	// use "Arial" as default font
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	_tcscpy(lf.lfFaceName, szArial);
#if _MFC_VER >= 0x0400
	if (GetSystemMetrics(SM_DBCSENABLED))
	{
		// Win95/4.0 only supports DEFAULT_GUI_FONT.  Using this
		// font makes it more easy to port the app to
		// international markets
		// Load default system font
		CFont font;
		font.CreateStockObject(DEFAULT_GUI_FONT);
		if (font.GetSafeHandle() == 0)font.CreateStockObject(SYSTEM_FONT);
		// Retrieve logical font information
		if (font.GetSafeHandle())font.GetObject(sizeof(lf), &lf);
	}
#endif
	lf.lfHeight = -11;
	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = 0;
	lf.lfUnderline = 0;
	*this = lf;
	return *this;
}
CGXFont& CGXFont::ChangeFont(const CGXFont& font, GXModifyType mt)
{
	GXBYTE8 key = GetFontKey();
	ASSERT(mt == gxOverride || mt == gxApplyNew || mt == gxExclude || mt == gxCopy);
	if (mt == gxExclude)
	{
		if (font.GetIncludeWeight())SetIncludeWeight(FALSE);
		if (font.GetIncludeItalic())SetIncludeItalic(FALSE);
		if (font.GetIncludeUnderline())SetIncludeUnderline(FALSE);
		if (font.GetIncludeStrikeOut())SetIncludeStrikeOut(FALSE);
		if (font.GetIncludeSize())SetIncludeSize(FALSE);
		if (font.GetIncludeOrientation())SetIncludeOrientation(FALSE);
		if (font.GetIncludeFaceName())SetIncludeFaceName(FALSE);
	}
	else if (mt == gxCopy){*this = font;}
	else // if (mt == gxOverride || mt == gxApplyNew)
	{
		BOOL b = (mt == gxOverride || mt == gxCopy);
		if ((b || !GetIncludeWeight()) && font.GetIncludeWeight())
			SetWeight(font.GetWeight());
		if ((b || !GetIncludeItalic()) && font.GetIncludeItalic())
			SetItalic(font.GetItalic());
		if ((b || !GetIncludeUnderline()) && font.GetIncludeUnderline())
			SetUnderline(font.GetUnderline());
		if ((b || !GetIncludeStrikeOut()) && font.GetIncludeStrikeOut())
			SetStrikeOut(font.GetStrikeOut());
		if ((b || !GetIncludeSize()) && font.GetIncludeSize())
			SetSize(font.GetSize());
		if ((b || !GetIncludeOrientation()) && font.GetIncludeOrientation())
			SetOrientation(font.GetOrientation());
		if ((b || !GetIncludeFaceName()) && font.GetIncludeFaceName())
			SetFaceNameId(font.GetFaceNameId());
	}
	if (fontbits.fontdata && key != GetFontKey())Release(key);
	return *this;
}
BOOL CGXFont::IsSubSet(const CGXFont& font) const
{
	BOOL b = TRUE;
	if(b&&font.GetIncludeWeight())b &= GetIncludeWeight()&&GetWeight()== font.GetWeight();
	if(b&&font.GetIncludeItalic())b &= GetIncludeItalic()&&GetItalic()==font.GetItalic();
	if(b&&font.GetIncludeUnderline())b&=GetIncludeUnderline()&&GetUnderline()==font.GetUnderline();
	if(b&&font.GetIncludeStrikeOut())b&=GetIncludeStrikeOut()&&GetStrikeOut()==font.GetStrikeOut();
	if(b&&font.GetIncludeSize())b&=GetIncludeSize()&&GetSize()==font.GetSize();
	if(b&&font.GetIncludeOrientation())b&=GetIncludeOrientation()&&GetOrientation()==font.GetOrientation();
	if(b&&font.GetIncludeFaceName())b&=GetIncludeFaceName()&&GetFaceNameId()==font.GetFaceNameId();
	return b;
}
CArchive& AFXAPI operator <<(CArchive& ar, const GXBYTE8& b)
{
	// I am sure that Serialize does not modify data
	BYTE* p = (BYTE*) &b;
	for(int i=0;i<8;i++)ar<<*p++;return ar;
}
CArchive& AFXAPI operator >>(CArchive& ar, GXBYTE8& b)
{
	BYTE* p = (BYTE*) &b;
	for(int i=0;i<8; i++)ar>>*p++;
	return ar;
}
void CGXFont::Serialize(CArchive &ar)
{
	GX_CHECKIMP(GXGetAppData()->m_pStyleSerializeImp, "CGXFont::Serialize");
	if (GXGetAppData()->m_pStyleSerializeImp)
		GXGetAppData()->m_pStyleSerializeImp->Serialize(this, ar);
}
void CGXFont::WriteProfile(LPCTSTR pszSection, LPCTSTR pszEntry) const
{
	GX_CHECKIMP(GXGetAppData()->m_pGridProfileImp, "CGXFont::WriteProfile");
	if (GXGetAppData()->m_pGridProfileImp)
		GXGetAppData()->m_pGridProfileImp->WriteProfile(this, pszSection, pszEntry);
}
BOOL CGXFont::ReadProfile(LPCTSTR pszSection, LPCTSTR pszEntry)
{
	GX_CHECKIMP(GXGetAppData()->m_pGridProfileImp, "CGXFont::ReadProfile");
	if (GXGetAppData()->m_pGridProfileImp)
		return GXGetAppData()->m_pGridProfileImp->ReadProfile(this, pszSection, pszEntry);
	return FALSE;
}
CArchive& AFXAPI operator <<(CArchive& ar, const CGXFont& font)
{
	// I am sure that Serialize does not modify data
	((CGXFont*) &font)->Serialize(ar);
	return ar;
}
CArchive& AFXAPI operator >>(CArchive& ar, CGXFont& font)
{
	font.Serialize(ar);return ar;
}
///////////////////////////////////////////////////////////////////
// CGXBrush
extern int _gxnPatternCount;
#define GXNOPATTERN 63
LPCTSTR CGXBrush::szBrushFormat = _T("%lu %ld %d %d %d %d %d %d %d");
CGXBrush::CGXBrush()
{ 
	lbStyle = 0; 
	lbColor1 = 0; 
	lbColor2 = 0; 
	lbHatch = 0; 
	lbPattern = GXNOPATTERN; 
	lbBkColor1 = 0; 
	lbBkColor2 = (DWORD) RGB(255,255,255); 
}

CGXBrush::CGXBrush(const LOGBRUSH& logbrush)
{ 
	lbStyle = logbrush.lbStyle; 
	lbColor1 = GXCOLOR1(logbrush.lbColor); 
	lbColor2 = GXCOLOR2(logbrush.lbColor); 
	lbHatch = logbrush.lbHatch; 
	lbPattern = GXNOPATTERN; 
	lbBkColor1 = 0; 
	lbBkColor2 = (DWORD) RGB(255,255,255); 
}

CGXBrush::CGXBrush(const CGXBrush& brush)
{ 
	memcpy(this, &brush, sizeof(CGXBrush)); 
}

CGXBrush::CGXBrush(UINT style, COLORREF color, LONG hatch)
{ 
	lbStyle = style; 
	lbColor1 = GXCOLOR1(color); 
	lbColor2 = GXCOLOR2(color); 
	lbHatch = (int) hatch; 
	lbPattern = GXNOPATTERN; 
	lbBkColor1 = 0; 
	lbBkColor2 = (DWORD) RGB(255,255,255); 
}

CGXBrush::operator tagLOGBRUSH() const
{
	static LOGBRUSH lb;

	lb.lbStyle = lbStyle;
	lb.lbHatch = lbHatch;
	if (lbStyle == BS_PATTERN)
	{
		if (lbPattern >= 0 && lbPattern < (unsigned) _gxnPatternCount)
			lb.lbHatch = (int) GXGetPatternBitmap(lbPattern)->GetSafeHandle();
		else
			lb.lbStyle = BS_NULL;
	}
	lb.lbColor = GXGETCOLOR2(lbColor1, lbColor2);

	return lb;
}

CGXBrush::operator const tagLOGBRUSH*() const
{
	static LOGBRUSH lb;

	lb.lbStyle = lbStyle;
	lb.lbHatch = lbHatch;
	if (lbStyle == BS_PATTERN)
	{
		if (lbPattern >= 0 && lbPattern < (unsigned) _gxnPatternCount)
			lb.lbHatch = (int) GXGetPatternBitmap(lbPattern)->GetSafeHandle();
		else
			lb.lbStyle = BS_NULL;
	}
	lb.lbColor = GXGETCOLOR2(lbColor1, lbColor2);

	return &lb;
}

CArchive& AFXAPI operator <<(CArchive& ar, const CGXBrush& lb)
{
	GX_CHECKIMP(GXGetAppData()->m_pStyleSerializeImp, "operator <<(CArchive& ar, const CGXBrush& lb)");

	if (GXGetAppData()->m_pStyleSerializeImp)
		GXGetAppData()->m_pStyleSerializeImp->Serialize((CGXBrush&) lb, ar);
	
	return ar;
}

CArchive& AFXAPI operator >>(CArchive& ar, CGXBrush& lb)
{
	GX_CHECKIMP(GXGetAppData()->m_pStyleSerializeImp, "operator >>(CArchive& ar, CGXBrush& lb)");

	if (GXGetAppData()->m_pStyleSerializeImp)
		GXGetAppData()->m_pStyleSerializeImp->Serialize(lb, ar);
	
	return ar;
}

///////////////////////////////////////////////////////////////////
// CGXPen

LPCTSTR CGXPen::szPenFormat = _T("%lu %ld %d %d %d %d");

// lopnWidth.y is unused in Windows/MFC
// I use it as Included-Flag
CGXPen& CGXPen::SetInclude(BOOL b)
{ 
	if (!b) 
		lopnStyle = GX_PS_DEFAULT; 
	else if (lopnStyle == GX_PS_DEFAULT) 
		lopnStyle = PS_SOLID; 
	return *this; 
}

// #define PS_DASH             1       /* -------  */
// #define PS_DOT              2       /* .......  */
// #define PS_DASHDOT          3       /* _._._._  */
// #define PS_DASHDOTDOT       4       /* _.._.._  */

static const int GXNEAR anHorzPattern[5] = { 0, 33, 34, 35, 36 };
static const int GXNEAR anVertPattern[5] = { 0, 37, 38, 39, 40 };

BOOL CGXPen::CreateCompatibleBrush(CGXBrush& br, const CGXBrush& brInterior, BOOL bHorzOrVert, BOOL bBlackWhite) const
{
	const CGXPen& pen = *this;


	if (pen.lopnStyle == PS_NULL)
	{
		br = brInterior;
		return TRUE;
	}
	else if (pen.lopnStyle >= PS_DASH && pen.lopnStyle <= PS_DASHDOTDOT)
	{
		if (bHorzOrVert)
			// Horizontal
			br.SetPattern(anHorzPattern[pen.lopnStyle]);
		else
			// Vertical
			br.SetPattern(anVertPattern[pen.lopnStyle]);

		if (bBlackWhite)
		{
			br.SetBkColor(RGB(255,255,255));
			br.SetColor(RGB(0,0,0));
		}
		else
		{
			br.SetColor(pen.GetColor());
			if (brInterior.GetStyle() == BS_SOLID)
				br.SetBkColor(brInterior.GetColor());
			else
				br.SetBkColor(brInterior.GetBkColor());
		}

		return TRUE;
	}

	return FALSE;
}

CArchive& AFXAPI operator <<(CArchive& ar, const CGXPen& lp)
{
	GX_CHECKIMP(GXGetAppData()->m_pStyleSerializeImp, "operator <<(CArchive& ar, const CGXPen& lp)");

	if (GXGetAppData()->m_pStyleSerializeImp)
		GXGetAppData()->m_pStyleSerializeImp->Serialize((CGXPen&) lp, ar);
	
	return ar;
}

CArchive& AFXAPI operator >>(CArchive& ar, CGXPen& lp)
{
	GX_CHECKIMP(GXGetAppData()->m_pStyleSerializeImp, "operator >>(CArchive& ar, CGXPen& lp)");

	if (GXGetAppData()->m_pStyleSerializeImp)
		GXGetAppData()->m_pStyleSerializeImp->Serialize(lp, ar);
	
	return ar;
}

CGXPen::operator tagLOGPEN() const
{
	static LOGPEN lp;

	lp.lopnStyle = lopnStyle;
	lp.lopnWidth.x = lopnWidth;
	lp.lopnWidth.y = 0;
	lp.lopnColor = GXGETCOLOR2(lopnColor1, lopnColor2);

	return lp;
}

CGXPen::operator tagLOGPEN*() const
{
	static LOGPEN lp;

	lp.lopnStyle = lopnStyle;
	lp.lopnWidth.x = lopnWidth;
	lp.lopnWidth.y = 0;
	lp.lopnColor = GXGETCOLOR2(lopnColor1, lopnColor2);

	return &lp;
}

