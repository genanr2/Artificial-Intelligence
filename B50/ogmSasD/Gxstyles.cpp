// This is a part of the Objective Grid C++ Library.
// Copyright (C) 1995-1998 Stingray Software, Inc.
// All rights reserved.
//
// This source code is only intended as a supplement to
// the Objective Grid Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding
// the Objective Grid product.
//
// Author: Stefan Hoenig
//

// gxstyles.cpp : implementation of the CGXStyle and supporting classes
//

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

#if _MFC_VER >= 0x0300
#ifndef _WINNLS_
#include "winnls.h"
#endif
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXSTYLES")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CGXStyle, CObject, 0 /* schema number*/ )

#define new DEBUG_NEW

// Colors
const TCHAR BASED_CODE szColorFormat[] = _T("%d %d %d");

const DWORD ExcludeAll = 0;
const DWORD IncludeAll = 0xffff;

///////////////////////////////////////////////////////////////////
// CGXStyle

DWORD CGXStyle::STYLEBITS::GetPortable() const
{
	if (NoPortableIsEmptyBits())
	{
		return 0;
	}

	return
		  0x00000001 * horzalign
		| 0x00000002 * vertalign
		| 0x00000004 * readonly
		| 0x00000008 * control
		| 0x00000010 * interior
		| 0x00000020 * textcolor
		| 0x00000040 * wraptext
		| 0x00000080 * allowenter
		| 0x00000100 * borders
		| 0x00000200 * font
		| 0x00000400 * value
		| 0x00000800 * choicelist
		| 0x00001000 * vertscroll
		| 0x00002000 * maxlength
		| 0x00004000 * basestyle
		| 0x00008000 * enabled
		| 0x00010000 * tristate
		| 0x00020000 * draw3dframe
		| 0x00040000 * userattr
		| 0x00080000 * autosize
		| 0x00100000 * itemdataptr
		| 0x00200000 * floatcell
		| 0x00400000 * floodcell
		| 0x00800000 * mergecell
		| 0x01000000 * format
		| 0x02000000 * places
	;
}

#if !defined(_DEBUG) && _MFC_VER < 0x0400
#pragma optimize("", off)
#endif

void CGXStyle::STYLEBITS::SetPortable(DWORD dw)
{
	if (dw == 0)
    {
		SetNoPortableBits(dw);
		return;
	}

	horzalign = (dw & 0x0001) > 0;
	vertalign = (dw & 0x0002) > 0;
	readonly = (dw & 0x0004) > 0;
	control = (dw & 0x0008) > 0;
	interior = (dw& 0x0010) > 0;
	textcolor = (dw & 0x0020) > 0;
	wraptext = (dw & 0x0040) > 0;
	allowenter = (dw & 0x0080) > 0;
	borders = (dw & 0x0100) > 0;
	font = (dw & 0x0200) > 0;
	value = (dw & 0x0400) > 0;
	choicelist = (dw & 0x0800) > 0;
	vertscroll = (dw & 0x1000) > 0;
	maxlength = (dw & 0x2000) > 0;
	basestyle = (dw & 0x4000) > 0;
	enabled = (dw & 0x8000) > 0;
	tristate = (dw & 0x10000) > 0;
	draw3dframe = (dw & 0x20000) > 0;
	userattr = (dw & 0x40000) > 0;
	autosize = (dw & 0x80000) > 0;
	itemdataptr = (dw & 0x100000) > 0;
	floatcell = (dw & 0x200000) > 0;
	floodcell = (dw & 0x400000) > 0;
	mergecell = (dw & 0x800000) > 0;
	format = (dw & 0x1000000) > 0;
	places = (dw & 0x2000000) > 0;
}

#if !defined(_DEBUG) && _MFC_VER < 0x0400
#pragma optimize("", on)
#endif

// Profile entries
LPCTSTR CGXStyle::szHorizontalAlignment = _T("HorizontalAlignment");
LPCTSTR CGXStyle::szVerticalAlignment = _T("VerticalAlignment");
LPCTSTR CGXStyle::szReadOnly = _T("ReadOnly");
LPCTSTR CGXStyle::szControl = _T("ControlId");
LPCTSTR CGXStyle::szInterior = _T("Interior");
LPCTSTR CGXStyle::szTextColor = _T("TextColor");
LPCTSTR CGXStyle::szWrapText = _T("WrapText");
LPCTSTR CGXStyle::szAllowEnter = _T("AllowEnter");
LPCTSTR CGXStyle::szFont = _T("Font");
LPCTSTR CGXStyle::szBorders = _T("Borders");
LPCTSTR CGXStyle::szBorder[] = { _T("Top"), _T("Left"), _T("Bottom"), _T("Right") };
LPCTSTR CGXStyle::szValue = _T("Value");
LPCTSTR CGXStyle::szMaxLength = _T("MaxLength");
LPCTSTR CGXStyle::szVertScroll = _T("VerticalScrollbar");
LPCTSTR CGXStyle::szChoiceList = _T("ChoicesList");
LPCTSTR CGXStyle::szEnabled = _T("Enabled");
LPCTSTR CGXStyle::szTriState = _T("TriState");
LPCTSTR CGXStyle::szDraw3dFrame = _T("Draw3dFrame");
LPCTSTR CGXStyle::szAutoSize = _T("AutoSize");
LPCTSTR CGXStyle::szBaseStyle = _T("BaseStyle");
LPCTSTR CGXStyle::szFloatCell = _T("FloatCell");
LPCTSTR CGXStyle::szFloodCell = _T("FloodCell");
LPCTSTR CGXStyle::szMergeCell = _T("MergeCell");
LPCTSTR CGXStyle::szFormat = _T("Format");
LPCTSTR CGXStyle::szPlaces = _T("Places");

const int CGXStyle::nBorders = 4;

CGXStyle::CGXStyle(const CGXStyle& p)
{
	m_pFont = NULL;
	m_pPenArray = NULL;
	m_pmapUserAttributes = NULL;
	m_pItemDataPtr = p.m_pItemDataPtr;

	stylebits.SetNoPortableBits(p.stylebits.GetNoPortableBits());
	memcpy(&m_lbInterior, &p.m_lbInterior, sizeof(m_lbInterior));

	m_nMaxLength = p.m_nMaxLength;
	m_nControl = p.m_nControl;
	memcpy(&boolbits, &p.boolbits, sizeof(boolbits));

	// Value
	if (p.GetIncludeValue())
	{
		SetValue(p.GetValueRef());
		SetValueType(p.GetValueType());
	}

	// Format
	if (p.GetIncludeFormat())
		SetFormat(p.GetFormat());
	
	// Places
	if (p.GetIncludePlaces())
		SetPlaces(p.GetPlaces());
	
	// Base Style
	if (p.GetIncludeBaseStyle())
		SetBaseStyle(p.GetBaseStyle());

	// Value Array
	if (p.GetIncludeChoiceList())
		SetChoiceList(p.GetChoiceListRef());

	// UserAttributs
	SetUserAttributes(p.GetUserAttributeMap());

	// Borders
	if (p.GetIncludeBorders())
		SetBorders(p.m_pPenArray);

	// Font
	if (p.GetIncludeFont())
		SetFont(*p.m_pFont, gxCopy);
}

CGXStyle* CGXStyle::Clone() const
{
	return new CGXStyle(*this);
}

void CGXStyle::ConstructStyle()
{
	stylebits.SetPortable(ExcludeAll);
	memset(&m_lbInterior, 0, sizeof(m_lbInterior));

	m_pFont = NULL;
	m_pPenArray = NULL;
	m_pmapUserAttributes = NULL;
	m_pItemDataPtr = NULL;
}

CGXStyle& CGXStyle::SetDefault()
{
	Free();

	SetHorizontalAlignment(DT_LEFT);
	SetVerticalAlignment(DT_TOP);
	SetReadOnly(FALSE);
	SetControl(GX_IDS_CTRL_EDIT);
	SetInterior(CGXBrush().SetColor(GXSYSCOLOR(COLOR_WINDOW)));
	SetBorders(NULL);
	SetVertScrollBar(FALSE);
	SetMaxLength(0);
	SetDraw3dFrame(gxFrameNormal);

	SetTextColor(GXSYSCOLOR(COLOR_WINDOWTEXT));
	SetWrapText(TRUE);
	SetAutoSize(FALSE);
	SetAllowEnter(FALSE);
	SetEnabled(TRUE);
	SetTriState(FALSE);

	SetFont(CGXFont().SetDefault(), gxCopy);

	SetValue(_T(""));
	SetChoiceList(_T(""));
	SetItemDataPtr(NULL);

	SetFloatCell(TRUE);
	SetFloodCell(TRUE);
	SetMergeCell(GX_MERGE_DISABLED);
	
	SetFormat(GX_FMT_DEFAULT);
	SetPlaces(13);

	ASSERT_VALID(this);

	return *this;
}

CGXStyle::~CGXStyle()
{
	Free();
}

void CGXStyle::Free()
{
	delete[] m_pPenArray;
	delete m_pFont;
	m_pFont = 0;
	m_pPenArray = 0;
	m_pItemDataPtr = NULL;
	m_sValue.Empty();
	m_sChoiceList.Empty();

	if (stylebits.NoPortableIsEmptyBits())
		return;

	stylebits.SetNoPortableBits(0);

	// UserAttributs
	delete m_pmapUserAttributes;
	m_pmapUserAttributes = NULL;
}

void CGXStyle::Serialize(CArchive& ar)
{
	Serialize(ar, NULL);
}

void CGXStyle::Serialize(CArchive& ar, const CGXStylesMap* pStylesMap)
{
	GX_CHECKIMP(GXGetAppData()->m_pStyleSerializeImp, "CGXStyle::Serialize");

	if (GXGetAppData()->m_pStyleSerializeImp)
		GXGetAppData()->m_pStyleSerializeImp->Serialize(this, ar, pStylesMap);
}

void CGXStyle::WriteProfile(LPCTSTR pszSection, const CGXStylesMap* pStylesMap) const
{
	GX_CHECKIMP(GXGetAppData()->m_pGridProfileImp, "CGXStyle::WriteProfile");

	if (GXGetAppData()->m_pGridProfileImp)
		GXGetAppData()->m_pGridProfileImp->WriteProfile(this, pszSection, pStylesMap);
}

BOOL CGXStyle::ReadProfile(LPCTSTR pszSection, const CGXStylesMap* pStylesMap)
{
	GX_CHECKIMP(GXGetAppData()->m_pGridProfileImp, "CGXStyle::ReadProfile");

	if (GXGetAppData()->m_pGridProfileImp)
		return GXGetAppData()->m_pGridProfileImp->ReadProfile(this, pszSection, pStylesMap);

	return FALSE;
}

const CGXStyle& CGXStyle::operator=(const CGXStyle& p)
{
	if (&p == this)
		return *this;

	if (!p.stylebits.GetNoPortableBits())
		Free();
	else
	{
		stylebits.SetNoPortableBits(p.stylebits.GetNoPortableBits());
		memcpy(&m_lbInterior, &p.m_lbInterior, sizeof(m_lbInterior));

		m_nMaxLength = p.m_nMaxLength;
		m_nControl = p.m_nControl;
		m_pItemDataPtr = p.m_pItemDataPtr;
		memcpy(&boolbits, &p.boolbits, sizeof(boolbits));

		// Value
		if (p.GetIncludeValue())
		{
			SetValue(p.GetValueRef());
			SetValueType(p.GetValueType());
		}

		// Format
		if (p.GetIncludeFormat())
			SetFormat(p.GetFormat());
		
		// Places
		if (p.GetIncludePlaces())
			SetPlaces(p.GetPlaces());
		
		// Base Style
		if (p.GetIncludeBaseStyle())
			SetBaseStyle(p.GetBaseStyle());

		// Value Array
		if (p.GetIncludeChoiceList())
			SetChoiceList(p.GetChoiceListRef());

		// UserAttributs
		SetUserAttributes(p.GetUserAttributeMap());

		// Borders
		if (p.GetIncludeBorders())
			SetBorders(p.m_pPenArray);
		else
		{
			delete[] m_pPenArray;
			m_pPenArray = NULL;
		}

		// Font
		if (p.GetIncludeFont())
			SetFont(p.GetFontRef(), gxCopy);
		else
		{
			delete m_pFont;
			m_pFont = NULL;
		}
	}

	return *this;
}

void CGXStyle::ChangeStyle(const CGXStyle& p, GXModifyType mt)
{
	ASSERT(p.IsKindOf(RUNTIME_CLASS(CGXStyle)));
	ASSERT_VALID(&p);
	ASSERT(mt == gxOverride || mt == gxApplyNew || mt == gxExclude || mt == gxCopy);
		ASSERT_VALID(this);

	if (mt == gxExclude)
	{
		if (!p.stylebits.GetNoPortableBits())
			return;

		// HorizontalAlignment
		if (p.GetIncludeHorizontalAlignment())
			SetIncludeHorizontalAlignment(FALSE);

		// VerticalAlignment
		if (p.GetIncludeVerticalAlignment())
			SetIncludeVerticalAlignment(FALSE);

		// ReadOnly
		if (p.GetIncludeReadOnly())
			SetIncludeReadOnly(FALSE);

		// FloatCell
		if (p.GetIncludeFloatCell())
			SetIncludeFloatCell(FALSE);

		// FloodCell
		if (p.GetIncludeFloodCell())
			SetIncludeFloodCell(FALSE);

		// MergeCell
		if (p.GetIncludeMergeCell())
			SetIncludeMergeCell(FALSE);

		// Draw3dFrame
		if (p.GetIncludeDraw3dFrame())
			SetIncludeDraw3dFrame(FALSE);

		// MaxLength
		if (p.GetIncludeMaxLength())
			SetIncludeMaxLength(FALSE);

		// Vertical Scrollbar
		if (p.GetIncludeVertScrollBar())
			SetIncludeVertScrollBar(FALSE);

		// Textcolor
		if (p.GetIncludeTextColor())
			SetIncludeTextColor(FALSE);

		// WrapText
		if (p.GetIncludeWrapText())
			SetIncludeWrapText(FALSE);

		// AutoSize
		if (p.GetIncludeAutoSize())
			SetIncludeAutoSize(FALSE);

		// AllowEnter
		if (p.GetIncludeAllowEnter())
			SetIncludeAllowEnter(FALSE);

		// Enabled
		if (p.GetIncludeEnabled())
			SetIncludeEnabled(FALSE);

		// TriState
		if (p.GetIncludeTriState())
			SetIncludeTriState(FALSE);

		// Control
		if (p.GetIncludeControl())
			SetIncludeControl(FALSE);

		// Interior
		if (p.GetIncludeInterior())
			SetIncludeInterior(FALSE);

		ASSERT_VALID(this);
		// Borders
		if (GetIncludeBorders() && p.GetIncludeBorders())
		{
			for (int i = 0; i < nBorders; i++)
			{
				GXBorderType t = (GXBorderType) i;
				if (p.GetIncludeBorders(t))
					SetIncludeBorders(t, FALSE);
			}
		}
		ASSERT_VALID(this);

		// Font
		if (GetIncludeFont() && p.GetIncludeFont())
			m_pFont->ChangeFont(p.GetFontRef(), mt);

		// Value
		if (p.GetIncludeValue())
			SetIncludeValue(FALSE);

		// Base Style
		if (p.GetIncludeBaseStyle())
			SetIncludeBaseStyle(FALSE);

		// Value Array
		if (p.GetIncludeChoiceList())
			SetIncludeChoiceList(FALSE);

		// UserAttributs
		if (GetUserAttributeMap() && p.GetUserAttributeMap() && p.GetUserAttributeMap()->GetSize() > 0)
			m_pmapUserAttributes->ChangeMap(p.GetUserAttributeMap(), mt);

		// ItemDataPtr
		if (p.GetIncludeItemDataPtr())
			SetIncludeItemDataPtr(FALSE);

		// Format
		if (p.GetIncludeFormat())
			SetIncludeFormat(FALSE);

		// Places
		if (p.GetIncludePlaces())
			SetIncludePlaces(FALSE);
	}
	else // if (mt == gxOverride || mt = gxCopy || mt == gxApplyNew)
	{
		ASSERT_VALID(this);
		BOOL b = (mt == gxOverride || mt == gxCopy);

		if (mt == gxCopy || !stylebits.GetNoPortableBits())
		{
			*this = p;
			return;
		}
		else if (!p.stylebits.GetNoPortableBits())
			return;

		// HorizontalAlignment
		if ((b || !GetIncludeHorizontalAlignment()) && p.GetIncludeHorizontalAlignment())
			SetHorizontalAlignment(p.GetHorizontalAlignment());

		// VerticalAlignment
		if ((b || !GetIncludeVerticalAlignment()) && p.GetIncludeVerticalAlignment())
			SetVerticalAlignment(p.GetVerticalAlignment());

		// ReadOnly
		if ((b || !GetIncludeReadOnly()) && p.GetIncludeReadOnly())
			SetReadOnly(p.GetReadOnly());

		// FloatCell
		if ((b || !GetIncludeFloatCell()) && p.GetIncludeFloatCell())
			SetFloatCell(p.GetFloatCell());

		// FloodCell
		if ((b || !GetIncludeFloodCell()) && p.GetIncludeFloodCell())
			SetFloodCell(p.GetFloodCell());

		// MergeCell
		if ((b || !GetIncludeMergeCell()) && p.GetIncludeMergeCell())
			SetMergeCell(p.GetMergeCell());

		// Draw3dFrame
		if ((b || !GetIncludeDraw3dFrame()) && p.GetIncludeDraw3dFrame())
			SetDraw3dFrame(p.GetDraw3dFrame());

		// MaxLength
		if ((b || !GetIncludeMaxLength()) && p.GetIncludeMaxLength())
			SetMaxLength(p.GetMaxLength());

		// Vertical Scrollbar
		if ((b || !GetIncludeVertScrollBar()) && p.GetIncludeVertScrollBar())
			SetVertScrollBar(p.GetVertScrollBar());

		// Textcolor
		if ((b || !GetIncludeTextColor()) && p.GetIncludeTextColor())
			SetTextColor(p.GetTextColorNA());

		// WrapText
		if ((b || !GetIncludeWrapText()) && p.GetIncludeWrapText())
			SetWrapText(p.GetWrapText());

		// AutoSize
		if ((b || !GetIncludeAutoSize()) && p.GetIncludeAutoSize())
			SetAutoSize(p.GetAutoSize());

		// AllowEnter
		if ((b || !GetIncludeAllowEnter()) && p.GetIncludeAllowEnter())
			SetAllowEnter(p.GetAllowEnter());

		// Enabled
		if ((b || !GetIncludeEnabled()) && p.GetIncludeEnabled())
			SetEnabled(p.GetEnabled());

		// TriState
		if ((b || !GetIncludeTriState()) && p.GetIncludeTriState())
			SetTriState(p.GetTriState());

		// Control
		if ((b || !GetIncludeControl()) && p.GetIncludeControl())
			SetControl(p.GetControl());

		ASSERT_VALID(this);
		// Interior
		if ((b || !GetIncludeInterior()) && p.GetIncludeInterior())
			SetInterior(p.GetInteriorRef());

		ASSERT_VALID(this);
		// Borders
		if (p.GetIncludeBorders())
		{
			for (int i = 0; i < nBorders; i++)
			{
				GXBorderType t = (GXBorderType) i;
				if ((b || !GetIncludeBorders(t)) && p.GetIncludeBorders(t))
					SetBorders(t, p.GetBordersRef(t));
			}
		}

		ASSERT_VALID(this);
		// Font
		if (p.GetIncludeFont())
		{
			if (!GetIncludeFont())
			{
	            if (m_pFont != 0) 
					delete m_pFont;
				m_pFont = new CGXFont(p.GetFontRef());
				SetIncludeFont(TRUE);
			}
			else
				m_pFont->ChangeFont(p.GetFontRef(), mt);
		}

		// extern DWORD tick10;
		// DWORD ti = GetTickCount();

		// Value
		if ((b || !GetIncludeValue()) && p.GetIncludeValue())
		{
			SetValue(p.GetValueRef());
			SetValueType(p.GetValueType());
		}

		// Format
		if ((b || !GetIncludeFormat()) && p.GetIncludeFormat())
			SetFormat(p.GetFormat());
	
		// Places
		if ((b || !GetIncludePlaces()) && p.GetIncludePlaces())
			SetPlaces(p.GetPlaces());
	
		// tick10 += GetTickCount()-ti;

		// Base Style
		if ((b || !GetIncludeBaseStyle()) && p.GetIncludeBaseStyle())
			SetBaseStyle(p.GetBaseStyle());

		// Value Array
		if ((b || !GetIncludeChoiceList()) && p.GetIncludeChoiceList())
			SetChoiceList(p.GetChoiceListRef());

		// UserAttributs
		if (p.GetUserAttributeMap() && !p.GetUserAttributeMap()->IsEmpty())
		{
			if (m_pmapUserAttributes == NULL)
				m_pmapUserAttributes = new CGXUserAttributeMap;

			m_pmapUserAttributes->ChangeMap(p.GetUserAttributeMap(), mt);
			stylebits.userattr = 1;
		}

		// ItemDataPtr
		if ((b || !GetIncludeItemDataPtr()) && p.GetIncludeItemDataPtr())
			SetItemDataPtr(p.GetItemDataPtr());
	}

	ASSERT_VALID(this);
}

BOOL CGXStyle::IsSubSet(const CGXStyle& p) const
{
	GX_CHECKIMP(GXGetAppData()->m_pStyleCompareSubsetImp, "CGXStyle::IsSubSet");

	if (GXGetAppData()->m_pStyleCompareSubsetImp)
		return GXGetAppData()->m_pStyleCompareSubsetImp->IsSubset(this, p);

	return FALSE;
}

void CGXStyle::LoadBaseStyle(const CGXStylesMap& stylesmap)
{
	CGXStyle* pBaseStyle = this;

	while (pBaseStyle->GetIncludeBaseStyle())
	{
		// check out pointer to new basestyle
		if (!stylesmap.LookupStyle(pBaseStyle->GetBaseStyle(), pBaseStyle))
			break;

		// apply base style to this style
		ChangeStyle(*pBaseStyle, gxApplyNew);
	}
}

#ifdef _DEBUG

void CGXStyle::AssertValid() const
{
	CObject::AssertValid();
}
/*
void CGXStyle::Dump(CDumpContext& dc) const
{
	ASSERT_VALID(this);
	CObject::Dump(dc);
	if (GetIncludeHorizontalAlignment())
		dc << "\nHorizontalAlignment = " << (UINT)GetHorizontalAlignment();
	if (GetIncludeVerticalAlignment())
		dc << "\nVerticalAlignment =  = " << (UINT)GetVerticalAlignment();
	if (GetIncludeReadOnly())
		dc << "\nReadOnly = " << (UINT)GetReadOnly();
	if (GetIncludeFloatCell())
		dc << "\nFloatCell = " << (UINT)GetFloatCell();
	if (GetIncludeFloodCell())
		dc << "\nFloodCell = " << (UINT)GetFloodCell();
	if (GetIncludeMergeCell())
		dc << "\nMergeCell = " << (UINT)GetMergeCell();
	if (GetIncludeDraw3dFrame())
		dc << "\nDraw3dFrame = " << (UINT)GetDraw3dFrame();
	if (GetIncludeTextColor())
		dc << "\nTextColor = " << (UINT)GetTextColorNA();
	if (GetIncludeWrapText())
		dc << "\nWrapText = " << (UINT)GetWrapText();
	if (GetIncludeAutoSize())
		dc << "\nAutoSize = " << (UINT)GetAutoSize();
	if (GetIncludeAllowEnter())
		dc << "\nAllowEnter = " << (UINT)GetAllowEnter();
	if (GetIncludeEnabled())
		dc << "\nEnabled = " << (UINT)GetEnabled();
	if (GetIncludeTriState())
		dc << "\nTriState = " << (UINT)GetTriState();
	if (GetIncludeControl())
		dc << "\nControl = " << (UINT)GetControl();
	if (GetIncludeMaxLength())
		dc << "\nMaxLength = " << (UINT)GetMaxLength();
	if (GetIncludeVertScrollBar())
		dc << "\nVertScrollBar = " << (UINT)GetVertScrollBar();
	if (GetIncludeInterior())
		dc << "\nInterior = " << GetInteriorRef().GetDescription();
	if (GetIncludeBorders())
	{
		for (int i = 0; i < nBorders; i++)
			if (GetIncludeBorders((GXBorderType) i))
				dc << "\nBorders[bt" << szBorder[i]
					<< "] = " << GetBordersRef((GXBorderType) i).GetDescription();
	}
	if (GetIncludeBaseStyle())dc << "\nBaseStyle = " << GetBaseStyle();
	if (GetIncludeFont())dc << "\nFont = " << GetFontRef().GetDescription();
	if (GetIncludeValue())dc << "\nValue = " << GetValue();
	if (GetIncludeChoiceList())dc << "\nChoiceList = " << GetChoiceListRef();

	// UserAttributs
	//if (m_pmapUserAttributes){} 
//	dc << "\n";
}
*/
#endif // _DEBUG


CGXStyle& CGXStyle::SetUserAttributes(const CGXUserAttributeMap* p)
{
	if (p && !p->IsEmpty())
	{
		if (m_pmapUserAttributes == NULL)
			m_pmapUserAttributes = new CGXUserAttributeMap;

		m_pmapUserAttributes->ChangeMap(p, gxCopy);
		stylebits.userattr = 1;
	}
	else 
	{
		delete m_pmapUserAttributes;
		m_pmapUserAttributes = NULL;
		stylebits.userattr = 0;
	}
	return *this;
}

CGXStyle& CGXStyle::SetBorders(const CGXPen* pPenArray)
{
	if (pPenArray)
	{
		if (!m_pPenArray)
			m_pPenArray = new CGXPen[nBorders];
		for (int i = 0; i < nBorders; i++)
			m_pPenArray[i] = pPenArray[i];
		SetIncludeBorders(TRUE);
	}
	else
	{
		delete[] m_pPenArray;
		m_pPenArray = NULL;
	}
	SetIncludeBorders(pPenArray != NULL);

	ASSERT_VALID(this);

	return *this;
}

CGXStyle& CGXStyle::SetBorders(GXBorderType border, const CGXPen& pen)
{
	if (!m_pPenArray)
		m_pPenArray = new CGXPen[nBorders];

	if (border == gxBorderAll)
	{
		for (int i = 0; i < 4; i++)
			m_pPenArray[i] = pen;
	}
	else
		m_pPenArray[border] = pen;

	SetIncludeBorders(TRUE);
	return *this;
}

CGXStyle& CGXStyle::SetIncludeBorders(GXBorderType border, BOOL b)
{
	if (!m_pPenArray)
		m_pPenArray = new CGXPen[nBorders];

	if (border == gxBorderAll)
	{
		for (int i = 0; i < 4; i++)
			m_pPenArray[i].SetInclude(b);
		SetIncludeBorders(b);
	}
	else
	{
		m_pPenArray[border].SetInclude(b);

		BOOL bAll = FALSE;
		for (int i = 0; i < 4; i++)
			bAll |= m_pPenArray[i].GetInclude();
		SetIncludeBorders(bAll);
	}

	return *this;
}

CGXStyle& CGXStyle::SetFont(const CGXFont& font, GXModifyType mt)
{
	if (mt == gxExclude)
	{
		// Font
		if (GetIncludeFont())
			m_pFont->ChangeFont(font, mt);
	}
	else
	{
		if (m_pFont)
			m_pFont->ChangeFont(font, mt);
		else
			m_pFont = new CGXFont(font);
		SetIncludeFont(TRUE);
	}

	ASSERT_VALID(this);

	return *this;
}

// User attributs

static const CGXAbstractUserAttribute gxEmptyUserAttribute;

BOOL CGXStyle::GetIncludeUserAttribute(WORD nID) const
{
	if (m_pmapUserAttributes)
	{
		return m_pmapUserAttributes->FindAttribute(nID) != -1;
	}
	return FALSE;
}

const CGXAbstractUserAttribute& CGXStyle::GetUserAttribute(WORD nID) const
{
	const CGXAbstractUserAttribute* pValue = NULL;

	if (m_pmapUserAttributes)
		pValue = m_pmapUserAttributes->GetAttribute(nID);

	if (pValue)
		return *pValue;
	else
		return gxEmptyUserAttribute;
}

CGXStyle& CGXStyle::SetUserAttribute(WORD nID, const CGXAbstractUserAttribute& value)
{
	if (!m_pmapUserAttributes)
		m_pmapUserAttributes = new CGXUserAttributeMap;

	m_pmapUserAttributes->SetAttribute(nID, value);
	stylebits.userattr = 1;

	return *this;
}

CGXStyle& CGXStyle::SetUserAttributePtr(WORD nID, CGXAbstractUserAttribute* pValue)
{
	if (!m_pmapUserAttributes)
		m_pmapUserAttributes = new CGXUserAttributeMap;

	m_pmapUserAttributes->SetAttributePtr(nID, pValue);
	stylebits.userattr = 1;

	return *this;
}

