// Author: Stefan Hoenig
// gxstydlg.cpp : implementation of CGXStyleSheet and CGXStylesDialog classes
#include "stdafx.h"
#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif
// Headers
#ifndef _GXRESRC_H_
#include "gxresrc.h"
#endif
#ifndef _GXEXT_H_
#include "gxext.h"
#endif
#ifndef _GXWND_H_
#include "gxwnd.h"
#endif
#ifndef _GXSTYLE_H_
#include "gxstyles.h"
#endif
#ifndef _GXSTYDLG_H_
#include "gxstydlg.h"
#endif

#ifndef _GXCTRL_H_
#include "gxctrl.h"
#endif

#ifndef _GXMSG_H_
#include "gxmsg.h"
#endif


#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXSTYDLG")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CGXStyleSheet, CGXPropertySheet)
IMPLEMENT_DYNAMIC(CGXStylesDialog, CDialog)
IMPLEMENT_DYNCREATE(CGXFontPage, CGXPropertyPage)
IMPLEMENT_DYNCREATE(CGXFormatPage, CGXPropertyPage)
IMPLEMENT_DYNCREATE(CGXColorPage, CGXPropertyPage)
IMPLEMENT_DYNCREATE(CGXBorderPage, CGXPropertyPage)
IMPLEMENT_DYNCREATE(CGXAlignmentPage, CGXPropertyPage)
IMPLEMENT_DYNCREATE(CGXMergeCellPage, CGXPropertyPage)
IMPLEMENT_DYNCREATE(CGXFloatCellPage, CGXPropertyPage)
IMPLEMENT_DYNCREATE(CGXControlPage, CGXPropertyPage)
IMPLEMENT_DYNCREATE(CGXUserAttributPage, CGXPropertyPage)
IMPLEMENT_DYNCREATE(CGXBasedOnPage, CGXPropertyPage)
static const LOGPEN BASED_CODE penArray[8] = {
		{ PS_SOLID, 1, 0 },{ PS_SOLID, 2, 0 },{ PS_SOLID, 3, 0 },
		{ PS_SOLID, 4, 0 },{ PS_DASH, 1, 0 },{ PS_DOT, 1, 0 },
		{ PS_DASHDOT, 1,0},{ PS_NULL, 1, 0 }
};
static const TCHAR BASED_CODE szAutomatic[] = _T("(none)");
static const TCHAR BASED_CODE szStandard[] = _T("Standard");
#define new DEBUG_NEW
/////////////////////////////////////////////////////////////////////////////
// CGXStylesDialog dialog
CGXStylesDialog::CGXStylesDialog(CWnd* pParent /*=NULL*/):CDialog(CGXStylesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGXStylesDialog)
	m_bSaveDefault = FALSE;
	m_sStyle = _T("");
	//}}AFX_DATA_INIT
	m_pParam = NULL;
}
void CGXStylesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGXStylesDialog)
	DDX_Control(pDX, IDC_GXCOMBOSTYLENAMES, m_cbStyles);
	DDX_Control(pDX, IDC_GXBTNREMOVESTYLE, m_btnRemoveStyle);
	DDX_Control(pDX, IDC_GXBTNCHANGESTYLE, m_btnChangeStyle);
	DDX_Control(pDX, IDC_GXBTNADDSTYLE, m_btnAddStyle);
	DDX_Check(pDX, IDC_GXCHECKSAVESTYLESDEF, m_bSaveDefault);
	DDX_CBString(pDX, IDC_GXCOMBOSTYLENAMES, m_sStyle);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate){}
	else
	{
		// initialize dialog controls
		// Styles Combobox
		CString s = m_sStyle;
		m_cbStyles.ResetContent();
		for (POSITION pos = m_StylesMap.StartStylesIteration(); pos;)
		{
			WORD wStyleId;
			UINT nResourceID;
			CString sName;
			const CGXStyle* pStyle;
			m_StylesMap.GetNextStyle(pos, wStyleId, sName, nResourceID, pStyle);
			m_cbStyles.AddString(sName);
		}
		if (!s.IsEmpty())m_cbStyles.SelectString(-1, s);
	}
	// Buttons
	int index = m_cbStyles.FindStringExact(-1, m_sStyle);
	m_btnAddStyle.EnableWindow(!m_sStyle.IsEmpty() && index == CB_ERR);
	m_btnChangeStyle.EnableWindow(!m_sStyle.IsEmpty() && index != CB_ERR);

	m_btnRemoveStyle.EnableWindow(!m_sStyle.IsEmpty()
		&& index != CB_ERR && !m_StylesMap.IsSystemStyle(m_StylesMap.GetBaseStyleId(m_sStyle)));
}
BEGIN_MESSAGE_MAP(CGXStylesDialog, CDialog)
	//{{AFX_MSG_MAP(CGXStylesDialog)
	ON_BN_CLICKED(IDC_GXBTNADDSTYLE, OnBtnAddStyle)
	ON_BN_CLICKED(IDC_GXBTNCHANGESTYLE, OnBtnChangeStyle)
	ON_BN_CLICKED(IDC_GXBTNLOADDEFSTYLES, OnBtnLoadDefStyles)
	ON_BN_CLICKED(IDC_GXBTNREMOVESTYLE, OnBtnRemoveStyle)
	ON_CBN_SELCHANGE(IDC_GXCOMBOSTYLENAMES, OnSelchangeStyleNames)
	ON_CBN_EDITCHANGE(IDC_GXCOMBOSTYLENAMES, OnEditchangeStyleName)
	ON_CBN_DBLCLK(IDC_GXCOMBOSTYLENAMES, OnDblclkStyleNames)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGXStylesDialog message handlers

void CGXStylesDialog::OnBtnAddStyle()
{
	UpdateData(TRUE);

	int index = m_cbStyles.FindStringExact(-1, m_sStyle);
	ASSERT(!m_sStyle.IsEmpty() && index == CB_ERR);

	CGXStyle* style;

	VERIFY(m_StylesMap.LookupStyle(m_StylesMap.m_wStyleStandard, style));

	// Create and copy a new style of same class
	CGXStyle* pDefaultStyle = style->Clone();

	// the programmer can create a dialog dialog derived from CGXStyleSheet
	// by overriding CreateStyleSheet
	CGXStyleSheet* pSheet = CreateStyleSheet();

	pSheet->CreatePages();

	// Create a temporary style object with same class
	CRuntimeClass* pStyleClass = pDefaultStyle->GetRuntimeClass();
	CGXStyle* pFillStyle = (CGXStyle*) pStyleClass->CreateObject();

	pSheet->SetStyle(*pFillStyle);

	// Fill up pDefaultStyle
	pDefaultStyle->ChangeStyle((*pFillStyle).SetDefault(), gxApplyNew);

	pSheet->SetDefaultStyle(*pDefaultStyle);

	if (pSheet->DoModal() == IDOK)
	{
		m_StylesMap.RegisterStyle(m_sStyle, pSheet->GetStyle());
		UpdateData(FALSE);
	}

	delete pSheet;
	delete pFillStyle;
	delete pDefaultStyle;
}

CGXStyleSheet* CGXStylesDialog::CreateStyleSheet()
{
	return new CGXStyleSheet(m_sStyle, m_pParam, &m_StylesMap);
}

void CGXStylesDialog::SetStylesMap(CGXStylesMap& stylesmap)
{ 
	m_StylesMap = stylesmap; 
}

const CGXStylesMap& CGXStylesDialog::GetStylesMap()
{ 
	return m_StylesMap; 
}

void CGXStylesDialog::SetParam(CGXGridParam* pParam)
{
	m_pParam = pParam;
}

void CGXStylesDialog::OnBtnChangeStyle()
{
	UpdateData(TRUE);

	int index = m_cbStyles.FindStringExact(-1, m_sStyle);
	ASSERT(!m_sStyle.IsEmpty() && index != CB_ERR);

	CGXStyleSheet* pSheet = CreateStyleSheet();

	pSheet->CreatePages();

	CGXStyle* style;
	WORD wStyleId = m_StylesMap.GetBaseStyleId(m_sStyle);
	if (!m_StylesMap.LookupStyle(wStyleId, style))
	{
		ASSERT(0);
		return;
	}

	pSheet->SetStyle(*style);

	// Compute style with default values (used in font and color preview)

	// Create a temporary style object with same class
	CRuntimeClass* pStyleClass = style->GetRuntimeClass();
	CGXStyle* pDefaultStyle = (CGXStyle*) pStyleClass->CreateObject();
	pDefaultStyle->SetDefault();

	while (style->GetIncludeBaseStyle())
	{
		if (!m_StylesMap.LookupStyle(style->GetBaseStyle(), style))
			break;
		pDefaultStyle->ChangeStyle(*style, gxApplyNew);
	}

	if (m_StylesMap.LookupStyle(m_StylesMap.m_wStyleStandard, style))
		pDefaultStyle->ChangeStyle(*style, gxApplyNew);

	pSheet->SetDefaultStyle(*pDefaultStyle);

	// Dialog

	if (pSheet->DoModal() == IDOK)
	{
		if (wStyleId == m_StylesMap.m_wStyleStandard)
		{
			// include unchecked attributs for Standard Style
			CGXStyle* pStyle = (CGXStyle*) pStyleClass->CreateObject();
			pStyle->SetDefault();
			pStyle->ChangeStyle(pSheet->GetStyle(), gxOverride);

			m_StylesMap.RegisterStyle(m_sStyle, *pStyle);
			delete pStyle;
		}
		else
			m_StylesMap.RegisterStyle(m_sStyle, pSheet->GetStyle());
		UpdateData(FALSE);
	}

	delete pDefaultStyle;
	delete pSheet;
}

void CGXStylesDialog::OnBtnLoadDefStyles()
{
	m_StylesMap.ReadProfile();
	UpdateData(FALSE);
}

void CGXStylesDialog::OnBtnRemoveStyle()
{
	UpdateData(TRUE);

	int index = m_cbStyles.FindStringExact(-1, m_sStyle);
	ASSERT(!m_sStyle.IsEmpty() && index != CB_ERR
		&& !m_StylesMap.IsSystemStyle(m_StylesMap.GetBaseStyleId(m_sStyle)));

	TCHAR s[50];
	wsprintf(s, _T("Do you want to remove '%s'?"), (LPCTSTR) m_sStyle);
	if (AfxMessageBox(s, MB_OKCANCEL) == IDOK)
	{
		m_StylesMap.RemoveStyle(m_StylesMap.GetBaseStyleId(m_sStyle));
		UpdateData(FALSE);
	}
}

void CGXStylesDialog::OnSelchangeStyleNames()
{
	CString s;
	int index = m_cbStyles.GetCurSel();
	
	if (index != CB_ERR)
	{
		m_cbStyles.GetLBText(index, s);
		m_cbStyles.SetWindowText(s);
	}

	UpdateData(TRUE);
}

void CGXStylesDialog::OnEditchangeStyleName()
{
	UpdateData(TRUE);
}

void CGXStylesDialog::OnDblclkStyleNames()
{
	OnSelchangeStyleNames();
	OnBtnChangeStyle();
}

void CGXStylesDialog::OnOK()
{
	UpdateData(TRUE);

	if (m_bSaveDefault)
		m_StylesMap.WriteProfile();

	CDialog::OnOK();
}


/////////////////////////////////////////////////////////////////////////////
// CGXStyleSheet


#if _MFC_VER < 0x0300 && defined(USE_TABDLG)

#define STYLESHEET(nID, pWnd, iPage) CGXPropertySheet(nID, pWnd, iPage, (UINT) IDD_GXSTYLESHEET)
#define ADDPAGE(page, title) if (page) { AddPage(page, title); }

#else

#define STYLESHEET(nID, pWnd, iPage) CGXPropertySheet(nID, pWnd, iPage)
#define ADDPAGE(page, title) if (page) { AddPage(page); }

#endif

CGXStyleSheet::CGXStyleSheet(UINT nIDCaption, CGXStylesMap& stylesMap, CWnd* pParentWnd, UINT iSelectPage)
	: STYLESHEET(nIDCaption, pParentWnd, iSelectPage)
{
	m_pParam = NULL;
	m_pStylesMap = &stylesMap;
	Init();
}

CGXStyleSheet::CGXStyleSheet(LPCTSTR pszCaption, CGXStylesMap& stylesMap, CWnd* pParentWnd, UINT iSelectPage)
	: STYLESHEET(pszCaption, pParentWnd, iSelectPage)
{
	m_pParam = NULL;
	m_pStylesMap = &stylesMap;
	Init();
}

CGXStyleSheet::CGXStyleSheet(UINT nIDCaption, CGXGridParam* pParam, CGXStylesMap* pStylesMap, CWnd* pParentWnd, UINT iSelectPage)
	: STYLESHEET(nIDCaption, pParentWnd, iSelectPage)
{
	m_pParam = pParam;
	if (pStylesMap)
		m_pStylesMap = pStylesMap;
	else
		m_pStylesMap = pParam->GetStylesMap();
	Init();
}

CGXStyleSheet::CGXStyleSheet(LPCTSTR pszCaption, CGXGridParam* pParam, CGXStylesMap* pStylesMap, CWnd* pParentWnd, UINT iSelectPage)
	: STYLESHEET(pszCaption, pParentWnd, iSelectPage)
{
	m_pParam = pParam;
	if (pStylesMap)
		m_pStylesMap = pStylesMap;
	else
		m_pStylesMap = pParam->GetStylesMap();
	Init();
}

void CGXStyleSheet::Init()
{
	m_pFontPage = NULL;
	m_pFormatPage = NULL;
	m_pColorPage = NULL;
	m_pBorderPage = NULL;
	m_pAlignmentPage = NULL;
	m_pMergeCellPage = NULL;
	m_pFloatCellPage = NULL;
	m_pControlPage = NULL;
	m_pUserAttributPage = NULL;
	m_pBasedOnPage = NULL;
	m_pStyle = (CGXStyle*) m_pStylesMap->m_pStyleClass->CreateObject();
	m_pDefaultStyle = (CGXStyle*) m_pStylesMap->m_pStyleClass->CreateObject();
}

void CGXStyleSheet::CreatePages()
{
	// this method is overridable!

	// if you want to display only certain pages or if you want to
	// add your own pages, you should override this method.

	m_pFontPage = CreateFontPage();
	m_pColorPage = CreateColorPage();
	m_pBorderPage = CreateBorderPage();
	m_pAlignmentPage = CreateAlignmentPage();
	m_pControlPage = CreateControlPage();
	m_pFormatPage = CreateFormatPage();

	if (m_pParam && m_pParam->m_wFloatCellsMode != gxnFloatDisable)
		m_pFloatCellPage = CreateFloatCellPage();

	if (m_pParam && m_pParam->m_wMergeCellsMode != gxnMergeDisable)
		m_pMergeCellPage = CreateMergeCellPage();

	m_pUserAttributPage = NULL;
	if (m_pStylesMap->GetUserAttrInfoMap().GetCount() > 0)
		m_pUserAttributPage = CreateUserAttributPage();

	m_pBasedOnPage = CreateBasedOnPage();

	ADDPAGE(m_pFontPage, _T("Font"));
	ADDPAGE(m_pColorPage, _T("Color"));
	ADDPAGE(m_pBorderPage, _T("Border"));
	ADDPAGE(m_pAlignmentPage, _T("Align"));
	ADDPAGE(m_pFormatPage, _T("Format"));
	ADDPAGE(m_pControlPage, _T("Control"));
	ADDPAGE(m_pMergeCellPage, _T("Merge Cells"));
	ADDPAGE(m_pFloatCellPage, _T("Floating Cells"));
	ADDPAGE(m_pUserAttributPage, _T("User"));
	ADDPAGE(m_pBasedOnPage, _T("Based On"));
}

CGXFontPage* CGXStyleSheet::CreateFontPage()
{
	return new CGXFontPage;
}

CGXColorPage* CGXStyleSheet::CreateColorPage()
{
	return new CGXColorPage;
}

CGXBorderPage* CGXStyleSheet::CreateBorderPage()
{
	return new CGXBorderPage;
}

CGXAlignmentPage* CGXStyleSheet::CreateAlignmentPage()
{
	return new CGXAlignmentPage;
}

CGXFloatCellPage* CGXStyleSheet::CreateFloatCellPage()
{
	return new CGXFloatCellPage;
}

CGXMergeCellPage* CGXStyleSheet::CreateMergeCellPage()
{
	return new CGXMergeCellPage;
}

CGXControlPage* CGXStyleSheet::CreateControlPage()
{
	return new CGXControlPage;
}

CGXUserAttributPage* CGXStyleSheet::CreateUserAttributPage()
{
	return new CGXUserAttributPage;
}

CGXBasedOnPage* CGXStyleSheet::CreateBasedOnPage()
{
	return new CGXBasedOnPage;
}

CGXFormatPage* CGXStyleSheet::CreateFormatPage()
{
	return new CGXFormatPage;
}

CGXStyleSheet::~CGXStyleSheet()
{
	delete m_pFontPage;
	delete m_pFormatPage;
	delete m_pColorPage;
	delete m_pBorderPage;
	delete m_pAlignmentPage;
	delete m_pMergeCellPage;
	delete m_pFloatCellPage;
	delete m_pControlPage;
	delete m_pUserAttributPage;
	delete m_pBasedOnPage;

	delete m_pStyle;
	delete m_pDefaultStyle;
}

BEGIN_MESSAGE_MAP(CGXStyleSheet, CGXPropertySheet)
	//{{AFX_MSG_MAP(CGXStyleSheet)
	ON_COMMAND(IDOK, OnOK)
	ON_WM_NCACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CGXStyleSheet::OnNcActivate(BOOL bActive)
{
	if (GXDiscardNcActivate())
		return TRUE;

	return CGXPropertySheet::OnNcActivate(bActive);
}

/////////////////////////////////////////////////////////////////////////////
// CGXStyleSheet message handlers

const CGXStyle& CGXStyleSheet::GetStyle() const
	{ return *m_pStyle; }

const CGXStyle& CGXStyleSheet::GetDefaultStyle() const
	{ return *m_pDefaultStyle; }

const CGXStylesMap& CGXStyleSheet::GetStylesMap() const
	{ return *m_pStylesMap; }

void CGXStyleSheet::SetStyle(const CGXStyle& style)
{
	delete m_pStyle;
	m_pStyle = style.Clone();
}

void CGXStyleSheet::UpdateData(BOOL bSaveAndValidate)
{
	if(bSaveAndValidate)
	{
		// check out pages
		// Block: Font
		if(m_pFontPage)
		{
			const CGXFont&font=m_pFontPage->GetFont();
			if(!font.IsEmptyFont())m_pStyle->SetFont(font);
			else m_pStyle->SetIncludeFont(FALSE);
			if(m_pFontPage->GetIncludeTextColor())m_pStyle->SetTextColor(m_pFontPage->GetTextColor());
			else m_pStyle->SetIncludeTextColor(FALSE);
		}
		// Block: Format
		if (m_pFormatPage)
		{
			if(m_pFormatPage->m_nFormat!=-1)m_pStyle->SetFormat(m_pFormatPage->m_nFormat);
			else m_pStyle->SetIncludeFormat(FALSE);
			if(!m_pFormatPage->m_strPlaces.IsEmpty())m_pStyle->SetPlaces(_ttoi(m_pFormatPage->m_strPlaces));
			else m_pStyle->SetIncludePlaces(FALSE);
		}
		// Block: Colors
		if(m_pColorPage)
		{
			if(m_pColorPage->GetIncludeInterior())
			{
				CGXBrush brush=m_pColorPage->GetInterior();
				if(brush.GetStyle()==BS_SOLID)
					brush.SetBkColor(brush.GetColor());
				else if(brush.GetStyle()==BS_NULL)
					brush.SetStyle(BS_SOLID).SetColor(brush.GetBkColor());
				m_pStyle->SetInterior(brush);
			}
			else 
				m_pStyle->SetIncludeInterior(FALSE);
			if(m_pColorPage->Get3dFrame()>=0&&m_pColorPage->Get3dFrame()<=2)
			{
				GXDraw3dFrameType aa=(GXDraw3dFrameType)m_pColorPage->Get3dFrame();
				m_pStyle->SetDraw3dFrame((GXDraw3dFrameType)m_pColorPage->Get3dFrame());
			}
		}
		// Block: Borders
		if(m_pBorderPage)
		{
			BOOL b=FALSE;
			for(int i=0;i<4;i++)b|=m_pBorderPage->GetIncludeBorders(i);
			if(b)
			{
				// First, initialize the pen array in m_pStyle
				m_pStyle->SetBorders(gxBorderLeft, CGXPen());
				// ... then initialize all borders
				for (int i = 0; i < 4; i++)
					if(m_pBorderPage->GetIncludeBorders(i))m_pStyle->SetBorders((GXBorderType)i,m_pBorderPage->GetBorders(i));
					else m_pStyle->SetIncludeBorders((GXBorderType)i,FALSE);
			}else m_pStyle->SetIncludeBorders(FALSE);
		}
		// Block: Alignment
		if (m_pAlignmentPage)
		{
			CGXAlignmentPage& pg = *m_pAlignmentPage;
			if(pg.m_nAllowEnter<2)m_pStyle->SetAllowEnter(pg.m_nAllowEnter);
			else m_pStyle->SetIncludeAllowEnter(FALSE);
			if(pg.m_nWrapText<2)m_pStyle->SetWrapText(pg.m_nWrapText);
			else m_pStyle->SetIncludeWrapText(FALSE);
			if(pg.m_nAutoSize < 2)m_pStyle->SetAutoSize(pg.m_nAutoSize);
			else m_pStyle->SetIncludeAutoSize(FALSE);
			if(pg.m_nHorzAlign==0)m_pStyle->SetIncludeHorizontalAlignment(FALSE);
			else
			{
				switch (pg.m_nHorzAlign)
				{
					case 1:m_pStyle->SetHorizontalAlignment(DT_LEFT);break;
					case 2:m_pStyle->SetHorizontalAlignment(DT_CENTER);break;
					case 3:m_pStyle->SetHorizontalAlignment(DT_RIGHT);break;
				}
			}
			if(pg.m_nVertAlign==0)m_pStyle->SetIncludeVerticalAlignment(FALSE);
			else
			{
				switch (pg.m_nVertAlign)
				{
					case 1:m_pStyle->SetVerticalAlignment(DT_TOP);break;
					case 2:m_pStyle->SetVerticalAlignment(DT_VCENTER);break;
					case 3:m_pStyle->SetVerticalAlignment(DT_BOTTOM);break;
				}
			}
		}
		// Block: Float Cells
		if (m_pFloatCellPage)
		{
			CGXFloatCellPage&pg=*m_pFloatCellPage;
			if(pg.m_nFloatCell<2)m_pStyle->SetFloatCell(pg.m_nFloatCell);
			else m_pStyle->SetIncludeFloatCell(FALSE);
			if(pg.m_nFloodCell<2)m_pStyle->SetFloodCell(pg.m_nFloodCell);
			else m_pStyle->SetIncludeFloodCell(FALSE);
		}
		// Block: Merge Cells
		if(m_pMergeCellPage)
		{
			CGXMergeCellPage&pg=*m_pMergeCellPage;
			if(pg.m_nMergeCell==0)m_pStyle->SetIncludeMergeCell(FALSE);
			else
			{
				switch (pg.m_nMergeCell)
				{
				case 1: m_pStyle->SetMergeCell(GX_MERGE_DISABLED); break;
				case 2: m_pStyle->SetMergeCell(GX_MERGE_HORIZONTAL|GX_MERGE_COMPVALUE); break;
				case 3: m_pStyle->SetMergeCell(GX_MERGE_VERTICAL|GX_MERGE_COMPVALUE); break;
				case 4: m_pStyle->SetMergeCell(GX_MERGE_HORIZONTAL); break;
				case 5: m_pStyle->SetMergeCell(GX_MERGE_VERTICAL); break;
				}
			}
		}
		// Block: Control
		if (m_pControlPage)
		{
			CGXControlPage& pg = *m_pControlPage;
			if (pg.m_nReadOnly==2)m_pStyle->SetIncludeReadOnly(FALSE);
			else m_pStyle->SetReadOnly(pg.m_nReadOnly);
			if(pg.m_nTriState==2)m_pStyle->SetIncludeTriState(FALSE);
			else m_pStyle->SetTriState(pg.m_nTriState);
			if(pg.m_nEnabled==2)m_pStyle->SetIncludeEnabled(FALSE);
			else m_pStyle->SetEnabled(pg.m_nEnabled);
			if (pg.m_sMaxLength.GetLength() == 0 || _ttoi(pg.m_sMaxLength) <= 0)
				m_pStyle->SetIncludeMaxLength(FALSE);
			else m_pStyle->SetMaxLength((WORD)_ttoi(pg.m_sMaxLength));
			if(pg.m_nControl==WORD_MAX)m_pStyle->SetIncludeControl(FALSE);
			else m_pStyle->SetControl(pg.m_nControl);
			if (pg.m_sChoices.GetLength() > 0)
			{
				CString s = pg.m_sChoices;
				GXReplaceString(s, _T("\\t"), _T("\t"));
				GXReplaceString(s, _T("\\r"), _T("\r"));
				GXReplaceString(s, _T("\\n"), _T("\n"));
				m_pStyle->SetChoiceList(s);
			}
			else m_pStyle->SetIncludeChoiceList(FALSE);
			if(pg.m_nValue==1){m_pStyle->SetValue(pg.m_sValue);}
			else m_pStyle->SetIncludeValue(FALSE);
		}
		// Block: User Attributes
		if (m_pUserAttributPage)
		{
			CGXUserAttributPage& pg = *m_pUserAttributPage;
			for (int i = 0; i < pg.m_arrayID.GetSize(); i++)
			{
				if (!pg.m_arrayInclude[i])m_pStyle->SetIncludeUserAttribute(pg.m_arrayID[i], FALSE);
				else m_pStyle->SetUserAttribute(pg.m_arrayID[i], pg.m_arrayContent[i]);
			}
		}
		// Block: BasedOn
		if (m_pBasedOnPage)
		{
			CGXBasedOnPage& pg = *m_pBasedOnPage;

			if (pg.m_sBasedOn.GetLength() > 0)
				m_pStyle->SetBaseStyle(m_pStylesMap->GetBaseStyleId(pg.m_sBasedOn));
			else
				m_pStyle->SetIncludeBaseStyle(FALSE);
		}
	}
	else
	{
		// Initialize pages
		// Block: Font
		if (m_pFontPage)
		{
			if (m_pStyle->GetIncludeFont())m_pFontPage->SetFont(&m_pStyle->GetFontRef());
			else m_pFontPage->SetFont(NULL);
			if (m_pStyle->GetIncludeTextColor())m_pFontPage->SetTextColor(m_pStyle->GetTextColor());
			else m_pFontPage->SetIncludeTextColor(FALSE);
		}
		// Block: Format
		if (m_pFormatPage)
		{
			if (m_pStyle->GetIncludeFormat())m_pFormatPage->m_nFormat = m_pStyle->GetFormat();
			else m_pFormatPage->m_nFormat = -1;
			if (m_pStyle->GetIncludePlaces())m_pFormatPage->m_strPlaces.Format(_T("%d"), m_pStyle->GetPlaces());
			else m_pFormatPage->m_strPlaces.Empty();
		}
		// Block: Colors
		if (m_pColorPage)
		{
			if (m_pStyle->GetIncludeInterior())m_pColorPage->SetInterior(m_pStyle->GetInteriorRef());
			else m_pColorPage->SetIncludeInterior(FALSE);
			if (m_pStyle->GetIncludeDraw3dFrame())m_pColorPage->Set3dFrame(m_pStyle->GetDraw3dFrame());
			else m_pColorPage->Set3dFrame(3);
		}
		// Block: Borders
		if (m_pBorderPage)
		{
			for (int i = 0; i < 4; i++)
			{
				if (m_pStyle->GetIncludeBorders((GXBorderType) i))
					m_pBorderPage->SetBorders(i, m_pStyle->GetBorders((GXBorderType) i));
				else m_pBorderPage->SetIncludeBorders(i, FALSE);
			}
		}
		// Block: Alignment
		if (m_pAlignmentPage)
		{
			CGXAlignmentPage&pg=*m_pAlignmentPage;
			if(m_pStyle->GetIncludeAllowEnter())pg.m_nAllowEnter=m_pStyle->GetAllowEnter();
			else pg.m_nAllowEnter=2;
			if(m_pStyle->GetIncludeWrapText())pg.m_nWrapText=m_pStyle->GetWrapText();
			else pg.m_nWrapText=2;
			if (m_pStyle->GetIncludeAutoSize())pg.m_nAutoSize = m_pStyle->GetAutoSize();
			else pg.m_nAutoSize=2;
			if (!m_pStyle->GetIncludeHorizontalAlignment())pg.m_nHorzAlign = 0;
			else
			{
				switch (m_pStyle->GetHorizontalAlignment())
				{
					case DT_LEFT:   pg.m_nHorzAlign = 1; break;
					case DT_CENTER: pg.m_nHorzAlign = 2; break;
					case DT_RIGHT:  pg.m_nHorzAlign = 3; break;
				}
			}
			if (!m_pStyle->GetIncludeVerticalAlignment())pg.m_nVertAlign = 0;
			else
			{
				switch (m_pStyle->GetVerticalAlignment())
				{
					case DT_TOP:    pg.m_nVertAlign = 1; break;
					case DT_VCENTER: pg.m_nVertAlign = 2; break;
					case DT_BOTTOM: pg.m_nVertAlign = 3; break;
				}
			}
		}
		// Block: Float Cells
		if (m_pFloatCellPage)
		{
			CGXFloatCellPage& pg = *m_pFloatCellPage;
			if (m_pStyle->GetIncludeFloatCell())pg.m_nFloatCell = m_pStyle->GetFloatCell();
			else pg.m_nFloatCell=2;
			if (m_pStyle->GetIncludeFloodCell())pg.m_nFloodCell = m_pStyle->GetFloodCell();
			else pg.m_nFloodCell = 2;
		}
		// Block: Merge Cells
		if (m_pMergeCellPage)
		{
			CGXMergeCellPage& pg = *m_pMergeCellPage;
			if (!m_pStyle->GetIncludeMergeCell())pg.m_nMergeCell = 0;
			else
			{
				switch (m_pStyle->GetMergeCell())
				{
					case GX_MERGE_DISABLED:   pg.m_nMergeCell = 1; break;
					case GX_MERGE_HORIZONTAL|GX_MERGE_COMPVALUE: pg.m_nMergeCell = 2; break;
					case GX_MERGE_VERTICAL|GX_MERGE_COMPVALUE:  pg.m_nMergeCell = 3; break;
					case GX_MERGE_HORIZONTAL: pg.m_nMergeCell = 4; break;
					case GX_MERGE_VERTICAL:  pg.m_nMergeCell = 5; break;
				}
			}
		}
		// Block: Control
		if (m_pControlPage)
		{
			CGXControlPage& pg = *m_pControlPage;
			pg.m_pControlNameMap = &m_pStylesMap->GetControlNameMap();
			if (m_pStyle->GetIncludeReadOnly())pg.m_nReadOnly = m_pStyle->GetReadOnly();
			else pg.m_nReadOnly = 2;
			if (m_pStyle->GetIncludeEnabled())pg.m_nEnabled = m_pStyle->GetEnabled();
			else pg.m_nEnabled = 2;
			if (m_pStyle->GetIncludeTriState())pg.m_nTriState = m_pStyle->GetTriState();
			else pg.m_nTriState = 2;
			if (m_pStyle->GetIncludeMaxLength() && m_pStyle->GetMaxLength() > 0)
			{
				TCHAR s[10];
				wsprintf(s, _T("%d"), m_pStyle->GetMaxLength());
				pg.m_sMaxLength = s;
			}
			else pg.m_sMaxLength.Empty();
			if (m_pStyle->GetIncludeControl())pg.m_nControl = m_pStyle->GetControl();
			else pg.m_nControl = WORD_MAX;
			if (m_pStyle->GetIncludeChoiceList())pg.m_sChoices = m_pStyle->GetChoiceListRef();
			else pg.m_sChoices.Empty();
			GXReplaceString(pg.m_sChoices, _T("\t"), _T("\\t"));
			GXReplaceString(pg.m_sChoices, _T("\r"), _T("\\r"));
			GXReplaceString(pg.m_sChoices, _T("\n"), _T("\\n"));
			if (m_pStyle->GetIncludeValue())
			{
				pg.m_sValue = m_pStyle->GetValueRef();
				pg.m_nValue = 1;
			}
			else pg.m_nValue = 2;
		}
		// Block: User Attributes
		if(m_pUserAttributPage)
		{
			CGXUserAttributPage& pg = *m_pUserAttributPage;
			pg.m_pUserAttrInfoMap = &m_pStylesMap->GetUserAttrInfoMap();
			int i = 0;
			for (POSITION pos = pg.m_pUserAttrInfoMap->GetStartPosition(); pos;)
			{
				CGXStylesMap::UserAttrInfo* pInfo;
				WORD w;
				pg.m_pUserAttrInfoMap->GetNextAssoc(pos, w, (void*&) pInfo);
				pg.m_arrayContent.SetAtGrow(i, m_pStyle->GetUserAttribute(w));
				pg.m_arrayInclude.SetAtGrow(i, (BYTE) m_pStyle->GetIncludeUserAttribute(w));
				pg.m_arrayID.SetAtGrow(i++, w);
			}
		}
		// Block: BasedOn
		if (m_pBasedOnPage)
		{
			CGXBasedOnPage& pg = *m_pBasedOnPage;
			pg.m_BasedOnStringList.RemoveAll();
			for (POSITION pos = m_pStylesMap->StartStylesIteration(); pos;)
			{
				const CGXStyle* pStyle;
				CString name;
				WORD wStyleId;
				UINT nResourceID;
				m_pStylesMap->GetNextStyle(pos, wStyleId, name, nResourceID, pStyle);
				pg.m_BasedOnStringList.AddHead(name);
			}
			if(m_pStyle->GetIncludeBaseStyle())pg.m_sBasedOn=m_pStylesMap->GetStyleName(m_pStyle->GetBaseStyle());
			else pg.m_sBasedOn.Empty();
		}
	}
}

void CGXStyleSheet::SetDefaultStyle(const CGXStyle& style)
{
	delete m_pDefaultStyle;
	m_pDefaultStyle = style.Clone();

	if (m_pFontPage)
	{
		if (style.GetIncludeFont())
			m_pFontPage->SetParentFont(&style.GetFontRef());
		else
			m_pFontPage->SetParentFont(NULL);
	}
}


void CGXStyleSheet::OnOK()
{
	ASSERT_VALID(this);

#if _MFC_VER < 0x0300 && defined(USE_TABDLG)
	for (int i = 0; i < m_nDialogs; i++)
		((CDialog*) m_DialogPtrArray[i])->UpdateData(TRUE);
	UpdateData(TRUE);

	CGXPropertySheet::OnOK();
#else
	if (GetActivePage()->OnKillActive())
	{
		UpdateData(TRUE);

		GetActivePage()->OnOK();
		if (!m_bModeless)
			EndDialog(IDOK);
	}
#endif
}

int CGXStyleSheet::DoModal()
{
	UpdateData(FALSE);

	return CGXPropertySheet::DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CGXFontSample control

CGXFontSample::CGXFontSample()
{
	m_rgbTextColor = GetSysColor(COLOR_WINDOWTEXT);
	m_pFont = NULL;
}

#if _MFC_VER < 0x0300
WNDPROC* CGXFontSample::GetSuperWndProcAddr()
{
	// Note: it is no longer necessary to override GetSuperWndProcAddr
	//  for each control class with a different WNDCLASS.
	//  This implementation now uses instance data, such that the previous
	//  WNDPROC can be anything.

	static WNDPROC NEAR pfnSuper;
	return &pfnSuper;
}
#endif

BEGIN_MESSAGE_MAP(CGXFontSample, CWnd)
	//{{AFX_MSG_MAP(CGXFontSample)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGXFontSample message handlers

void CGXFontSample::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CString text;
	GetWindowText(text);

	CRect rect;
	GetClientRect(rect);

	CFont* pOldFont = NULL;

	if (m_pFont)
		pOldFont = dc.SelectObject(m_pFont);

	GXPatB(&dc, rect, GetSysColor(COLOR_WINDOW));

	dc.SetBkColor(GetSysColor(COLOR_WINDOW));
	dc.SetTextColor(m_rgbTextColor);
	dc.SetBkMode(OPAQUE);

	if (m_nOrientation)
		// draw vertical text
		GXDaFTools()->GXDrawRotatedText(&dc, text, text.GetLength(), rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER, m_nOrientation);
	else
		// regular text
		GXDrawTextLikeMultiLineEdit(&dc, text, text.GetLength(), rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	_GXDrawSunkenBorders(&dc, rect, TRUE, TRUE);

	if (pOldFont)
		dc.SelectObject(pOldFont);
}

/////////////////////////////////////////////////////////////////////////////
// CGXTextColorComboBox control

CGXTextColorComboBox::CGXTextColorComboBox()
{
}

CGXTextColorComboBox::~CGXTextColorComboBox()
{
}


BEGIN_MESSAGE_MAP(CGXTextColorComboBox, CComboBox)
	//{{AFX_MSG_MAP(CGXTextColorComboBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGXTextColorComboBox message handlers

void CGXTextColorComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rc = lpDrawItemStruct->rcItem;
	COLORREF rgbColor = lpDrawItemStruct->itemData;
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	if (lpDrawItemStruct->itemID == 0)
	{
		pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
		pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		pDC->SetBkMode(OPAQUE);
		GXDrawTextLikeMultiLineEdit(pDC, _T("Default"), 7, rc, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	}
	else
		GXPatB(pDC, rc, rgbColor);

	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		CBrush br;
		br.CreateStockObject(BLACK_BRUSH);
		pDC->FrameRect(CRect(rc.left, rc.top, rc.right, rc.bottom), &br);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGXFontSample

void CGXFontSample::SetTextColor(COLORREF rgbColor)
{ 
	m_rgbTextColor = rgbColor; 
}

COLORREF CGXFontSample::GetTextColor() const
{ 
	return m_rgbTextColor; 
}

void CGXFontSample::SetSampleFont(CFont* pFont, int nOrientation)
{ 
	m_pFont = pFont; 
	m_nOrientation = nOrientation;
}

CFont* CGXFontSample::GetSampleFont()
{ 
	return m_pFont; 
}

/////////////////////////////////////////////////////////////////////////////
// CGXFontFaceNameBox

CGXFontFaceNameBox::CGXFontFaceNameBox()
{
	m_bmTTFont.LoadBitmap(GX_IDB_TTFONT);
	InitSysColors();
}

CGXFontFaceNameBox::~CGXFontFaceNameBox()
{
	m_bmTTFont.DeleteObject();
}

void CGXFontFaceNameBox::InitSysColors()
{
	m_rgbWindowText  = ::GetSysColor(COLOR_WINDOWTEXT);
	m_rgbWindow      = ::GetSysColor(COLOR_WINDOW);
	m_rgbHilight     = ::GetSysColor(COLOR_HIGHLIGHT);
	m_rgbHilightText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
}


BEGIN_MESSAGE_MAP(CGXFontFaceNameBox, CComboBox)
	//{{AFX_MSG_MAP(CGXFontFaceNameBox)
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGXFontFaceNameBox message handlers

void CGXFontFaceNameBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rc = lpDrawItemStruct->rcItem;
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	COLORREF rgbWindow = m_rgbWindow;
	COLORREF rgbWindowText = m_rgbWindowText;

	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		rgbWindow = m_rgbHilight;
		rgbWindowText = m_rgbHilightText;
	}

	// Font name and type
	CString strFaceName;
	GetLBText(lpDrawItemStruct->itemID, strFaceName);
	int nFontType = (int) GXGetFontState()->m_aFontInfo[lpDrawItemStruct->itemData].nFontType;

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(rgbWindow);
	pDC->SetTextColor(rgbWindowText);

	CRect temRect = rc;
	temRect.left += 16;

	GXPatB(pDC, rc, rgbWindow);

	// Draw bitmap for TrueType fonts
	if (nFontType == TRUETYPE_FONTTYPE)
	{
		GXDrawTransparentBitmap(pDC, // The destination DC.
			&m_bmTTFont, // The bitmap to be drawn.
			rc.left,        // X coordinate.
			rc.top + 2,        // Y coordinate.
			RGB(255, 255, 255)); // Make white pixels transparent
	}
	else
	{
		NULL;
	}

	pDC->DrawText(strFaceName, strFaceName.GetLength(), temRect, DT_SINGLELINE|DT_VCENTER);
}

void CGXFontFaceNameBox::OnSysColorChange()
{
	CComboBox::OnSysColorChange();

	InitSysColors();

}

/////////////////////////////////////////////////////////////////////////////
// CGXFontPage property page

CGXFontPage::CGXFontPage(const CGXFont* pFont /*=NULL*/) : CGXPropertyPage(CGXFontPage::IDD)
{
	//{{AFX_DATA_INIT(CGXFontPage)
	//}}AFX_DATA_INIT

	if (pFont)
		m_font = *pFont;

	m_bUpdate = FALSE;
}

CGXFontPage::~CGXFontPage()
{
}

void CGXFontPage::SetParentFont(const CGXFont* pFontParent, LPCTSTR szParentName)
{
	m_fontParent.SetDefault();

	if (pFontParent)
		m_fontParent.ChangeFont(*pFontParent, gxOverride);

	if (szParentName)
		m_sParentName = szParentName;
	else
		m_sParentName = szStandard;
}

const CGXFont& CGXFontPage::GetFont() const
{
	return m_font;
}

void CGXFontPage::SetFont(const CGXFont* pFont)
{
	if (pFont)
		m_font = *pFont;
}

void CGXFontPage::SetTextColor(COLORREF rgbColor)
{ 
	m_rgbText = rgbColor; m_bIncludeText = TRUE; 
}

COLORREF CGXFontPage::GetTextColor()
{
	return m_bIncludeText?m_rgbText:0; 
}

void CGXFontPage::SetIncludeTextColor(BOOL b)
{ 
	m_bIncludeText = b; 
}

BOOL CGXFontPage::GetIncludeTextColor()
{ 
	return m_bIncludeText; 
}

void CGXFontPage::DoDataExchange(CDataExchange* pDX)
{
	CGXPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGXFontPage)
	DDX_Control(pDX, IDC_GXCHECKSTRIKEOUT, m_btnStrikeout);
	DDX_Control(pDX, IDC_GXCHECKUNDERLINE, m_btnUnderline);
	DDX_Control(pDX, IDC_GXCBSIZE, m_cbSize);
	DDX_Control(pDX, IDC_GXCBOUTLINE, m_cbOutline);
	DDX_Control(pDX, IDC_GXCBFACENAME, m_cbFaceName);
	//}}AFX_DATA_MAP
	if (GetDlgItem(IDC_GXCBORIENTATION) != NULL)
		DDX_Control(pDX, IDC_GXCBORIENTATION, m_cbOrientation);

	if (pDX->m_bSaveAndValidate)
	{
		CString s;  // CComboBox
		int index;

		// Face Name
		m_cbFaceName.GetWindowText(s);
		if (m_cbFaceName.FindStringExact(-1, s) == CB_ERR)
			m_font.SetIncludeFaceName(FALSE);
		else
			m_font.SetFaceName(s);

		// Outline
		m_cbOutline.GetWindowText(s);
		index = m_cbOutline.FindStringExact(-1, s);
		switch (index)
		{
		case 0: m_font.SetBold(FALSE).SetItalic(FALSE); break;
		case 1: m_font.SetIncludeWeight(FALSE).SetItalic(TRUE); break;
		case 2: m_font.SetBold(TRUE).SetIncludeItalic(FALSE); break;
		case 3: m_font.SetBold(TRUE).SetItalic(TRUE); break;
		case 4: m_font.SetBold(FALSE).SetIncludeItalic(FALSE); break;
		case 5: m_font.SetIncludeWeight(FALSE).SetItalic(FALSE); break;
		case CB_ERR: m_font.SetIncludeWeight(FALSE).SetIncludeItalic(FALSE); break;
		default:
			ASSERT(0);
			// ASSERTION-> Unknown return value from Outline-ComboBox ->END
		}

		// Size
		m_cbSize.GetWindowText(s);
		if (_gxttof(s) > 0)
			m_font.SetSize(_gxttof(s));
		else
			m_font.SetIncludeSize(FALSE);

		// Orientation
		if (m_cbOrientation.GetSafeHwnd())
		{
			m_cbOrientation.GetWindowText(s);
			if (s.GetLength() > 0)
				m_font.SetOrientation(_ttoi(s)*10);
			else
				m_font.SetIncludeOrientation(FALSE);
		}

		// Strikeout
		switch (m_btnStrikeout.GetCheck())
		{
		case 0: m_font.SetStrikeOut(FALSE); break;
		case 1: m_font.SetStrikeOut(TRUE); break;
		case 2: m_font.SetIncludeStrikeOut(FALSE); break;
		}

		// Underline
		switch (m_btnUnderline.GetCheck())
		{
		case 0: m_font.SetUnderline(FALSE); break;
		case 1: m_font.SetUnderline(TRUE); break;
		case 2: m_font.SetIncludeUnderline(FALSE); break;
		}

		// Text Color
		int n = m_cbTextColor.GetCurSel();
		if (n > 0)
			SetTextColor(GXGetVGAColor(n-1));
		else
			SetIncludeTextColor(FALSE);
	}
	else
	{
		CString s;  // CComboBox
		int index = -1;

		// Face Name
		if (m_font.GetIncludeFaceName())
			m_cbFaceName.SelectString(-1, m_font.GetFaceName());

		// Outline
		// 0: Normal
		// 1: Italic
		// 2: Bold
		// 3: Bold Italic
		// 4: Not Bold
		// 5: Not Italic

		if (!m_font.GetIncludeWeight() && !m_font.GetIncludeItalic())
			index = -1;
		else if (m_font.GetIncludeWeight() && !m_font.GetBold() && m_font.GetIncludeItalic() && !m_font.GetItalic())
			index = 0;
		else if (m_font.GetIncludeWeight() && m_font.GetBold() && m_font.GetIncludeItalic() && m_font.GetItalic())
			index = 3;
		else if (m_font.GetIncludeItalic() && m_font.GetItalic())
			index = 1;
		else if (m_font.GetIncludeWeight() && m_font.GetBold())
			index = 2;
		else if (m_font.GetIncludeItalic() && !m_font.GetItalic())
			index = 5;
		else if (m_font.GetIncludeWeight() && !m_font.GetBold())
			index = 4;
		else
			ASSERT(0);
			// ASSERTION-> index for Outline-ComboBox could not be determinded ->END

		m_cbOutline.SetCurSel(index);

		// Size
		if (m_font.GetIncludeSize() && m_cbSize.GetCount() > 0)
		{
			TCHAR s[16];
			_stprintf(s, _T("%g"), m_font.GetSize());
			int n = m_cbSize.FindStringExact(-1, s);
			if (n == CB_ERR)
			{
				TRACE(_T("Size %s could not be selected\n"), s);
				m_cbSize.SetWindowText(s);
			}
			else
				m_cbSize.SetCurSel(n);

		}

		// Orientation
		if (m_cbOrientation.GetSafeHwnd() && m_font.GetIncludeOrientation() && m_cbOrientation.GetCount() > 0)
		{
			TCHAR s[16];
			_stprintf(s, _T("%d"), m_font.GetOrientation()/10);
			int n = m_cbOrientation.FindStringExact(-1, s);
			if (n == CB_ERR)
			{
				TRACE(_T("Orientation %s could not be selected\n"), s);
				m_cbOrientation.SetWindowText(s);
			}
			else
				m_cbOrientation.SetCurSel(n);

		}

		// Strikeout
		if (!m_font.GetIncludeStrikeOut())
			m_btnStrikeout.SetCheck(2);
		else if (m_font.GetStrikeOut())
			m_btnStrikeout.SetCheck(1);
		else
			m_btnStrikeout.SetCheck(0);

		// Underline
		if (!m_font.GetIncludeUnderline())
			m_btnUnderline.SetCheck(2);
		else if (m_font.GetUnderline())
			m_btnUnderline.SetCheck(1);
		else
			m_btnUnderline.SetCheck(0);

		// Text Color
		if (GetIncludeTextColor())
			m_cbTextColor.SetCurSel(GXFindColor(m_rgbText)+1);
		else
			m_cbTextColor.SetCurSel(0);
	}

	// Sample Font
	CGXFont sample(m_font);
	sample.ChangeFont(m_fontParent, gxApplyNew);

	if (fontSample.GetSafeHandle() != NULL)
		fontSample.DeleteObject();

	fontSample.CreateFontIndirect(&sample.GetLogFontRef());

	m_wndSample.SetSampleFont(&fontSample, sample.GetIncludeOrientation()?(int) sample.GetOrientation():0);
	m_wndSample.SetTextColor(GetTextColor());
	m_wndSample.Invalidate(FALSE);
}


BEGIN_MESSAGE_MAP(CGXFontPage, CGXPropertyPage)
	//{{AFX_MSG_MAP(CGXFontPage)
	ON_CBN_SELCHANGE(IDC_GXCBFACENAME, OnSelchangeFaceName)
	ON_CBN_SELCHANGE(IDC_GXCBOUTLINE, OnSelchangeOutline)
	ON_CBN_SELCHANGE(IDC_GXCBSIZE, OnSelchangeSize)
	ON_CBN_KILLFOCUS(IDC_GXCBSIZE, OnKillfocusSize)
	ON_CBN_SELCHANGE(IDC_GXCBORIENTATION, OnSelchangeOrientation)
	ON_CBN_KILLFOCUS(IDC_GXCBORIENTATION, OnKillfocusOrientation)
	ON_CBN_KILLFOCUS(IDC_GXCBOUTLINE, OnKillfocusOutline)
	ON_CBN_KILLFOCUS(IDC_GXCBFACENAME, OnKillfocusFacename)
	ON_BN_CLICKED(IDC_GXCHECKUNDERLINE, OnUnderlineClicked)
	ON_BN_CLICKED(IDC_GXCHECKSTRIKEOUT, OnStrikeoutClicked)
	ON_CBN_CLOSEUP(IDC_GXCBTEXTCOLOR, OnCloseupGxcbtextcolor)
	ON_CBN_EDITCHANGE(IDC_GXCBTEXTCOLOR, OnEditchangeGxcbtextcolor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGXFontPage message handlers


BOOL CGXFontPage::OnInitDialog()
{
	m_cbTextColor.SubclassDlgItem(IDC_GXCBTEXTCOLOR, this);
	m_wndSample.SubclassDlgItem(IDC_GXTEXTSAMPLE, this);
	m_cbFaceName.SubclassDlgItem(IDC_GXCBFACENAME, this);

	CGXPropertyPage::OnInitDialog();

	// Empty face name list
	m_cbFaceName.ResetContent();

	// Load face name list from GX_FONT_STATE
	for (int i = 0; i < GXGetFontState()->m_nFontInfoCount; i++)
	{
		if (GXGetAppData()->m_bDisplayFontCharset || GXGetFontState()->m_aFontInfo[i].nDefaultCharset)
			m_cbFaceName.SetItemData(m_cbFaceName.AddString(GXGetFontState()->m_aFontInfo[i].lfFaceName), (DWORD) i);
	}

	// Block: Size box
	{
		CString sSize;
		sSize.LoadString(GX_IDS_FONTSIZE);
		int n = sSize.Find(_T('\n'));
		while (!sSize.IsEmpty() && n != -1)
		{
			CString sVal;
			n = sSize.Find(_T('\n'));
			if (n > 0)
				sVal = sSize.Left(n);
			else
				sVal = sSize;

			if (!sVal.IsEmpty())
				m_cbSize.AddString(sVal);
			sSize = sSize.Mid(n+1);
		}
	}

	// Block: Orientation box
	{
		CString sOrientation;
		sOrientation.LoadString(GX_IDS_FONTORIENTATION);
		int n = sOrientation.Find(_T('\n'));
		while (m_cbOrientation.GetSafeHwnd() && !sOrientation.IsEmpty() && n != -1)
		{
			CString sVal;
			n = sOrientation.Find(_T('\n'));
			if (n > 0)
				sVal = sOrientation.Left(n);
			else
				sVal = sOrientation;

			if (!sVal.IsEmpty())
				m_cbOrientation .AddString(sVal);
			sOrientation = sOrientation.Mid(n+1);
		}
	}

	// Block: Outline box
	{
		CString sOutline;
		sOutline.LoadString(GX_IDS_FONTOUTLINE);
		int n = sOutline.Find(_T('\n'));
		while (!sOutline.IsEmpty() && n != -1)
		{
			CString sVal;
			n = sOutline.Find(_T('\n'));
			if (n > 0)
				sVal = sOutline.Left(n);
			else
				sVal = sOutline;

			if (!sVal.IsEmpty())
				m_cbOutline.AddString(sVal);
			sOutline = sOutline.Mid(n+1);
		}
	}

	// Block: Color Listbox
	{
		m_cbTextColor.AddString((LPCTSTR) RGB(255,255,255));
		for (int n = 0; n < 20; n++)
			m_cbTextColor.AddString((LPCTSTR) GXGetVGAColor(n));

		m_cbTextColor.SetCurSel(0);
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CGXFontPage::OnSelchangeFaceName()
{
	int index = m_cbFaceName.GetCurSel();
	if (index != CB_ERR)
	{
		CString s;
		m_cbFaceName.GetLBText(index, s);
		m_cbFaceName.SetWindowText(s);
	}

	UpdateData(TRUE);
}

void CGXFontPage::OnSelchangeOutline()
{
	int index = m_cbOutline.GetCurSel();
	if (index != CB_ERR)
	{
		CString s;
		m_cbOutline.GetLBText(index, s);
		m_cbOutline.SetWindowText(s);
	}

	UpdateData(TRUE);
}

void CGXFontPage::OnSelchangeSize()
{
	int index = m_cbSize.GetCurSel();
	if (index != CB_ERR)
	{
		CString s;
		m_cbSize.GetLBText(index, s);
		m_cbSize.SetWindowText(s);
	}

	UpdateData(TRUE);
}

void CGXFontPage::OnKillfocusSize()
{
	UpdateData(TRUE);
}

void CGXFontPage::OnSelchangeOrientation()
{
	if (!m_cbOrientation.GetSafeHwnd())
		return;

	int index = m_cbOrientation.GetCurSel();
	if (index != CB_ERR)
	{
		CString s;
		m_cbOrientation.GetLBText(index, s);
		m_cbOrientation.SetWindowText(s);
	}

	UpdateData(TRUE);
}

void CGXFontPage::OnKillfocusOrientation()
{
	UpdateData(TRUE);
}

void CGXFontPage::OnKillfocusOutline()
{
	UpdateData(TRUE);
}

void CGXFontPage::OnKillfocusFacename()
{
	// UpdateData(TRUE);
}

void CGXFontPage::OnUnderlineClicked()
{
	UpdateData(TRUE);
}

void CGXFontPage::OnStrikeoutClicked()
{
	UpdateData(TRUE);
}

void CGXFontPage::OnCloseupGxcbtextcolor()
{
	UpdateData(TRUE);
}


void CGXFontPage::OnEditchangeGxcbtextcolor()
{
	UpdateData(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CGXColorPage property page

// CGXColorButtonBox control

IMPLEMENT_REGISTER(CGXColorButtonBox, GX_GLOBALCLASS, 0, 0, 0);

CGXColorButtonBox::CGXColorButtonBox()
{
	m_nCurSel = -1;
	m_bDefaultButton = TRUE;
}

CGXColorButtonBox::~CGXColorButtonBox()
{
}


#if _MFC_VER < 0x0300
WNDPROC* CGXColorButtonBox::GetSuperWndProcAddr()
{
	// Note: it is no longer necessary to override GetSuperWndProcAddr
	//  for each control class with a different WNDCLASS.
	//  This implementation now uses instance data, such that the previous
	//  WNDPROC can be anything.

	static WNDPROC NEAR pfnSuper;
	return &pfnSuper;
}
#endif

BEGIN_MESSAGE_MAP(CGXColorButtonBox, CWnd)
	//{{AFX_MSG_MAP(CGXColorButtonBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CGXColorButtonBox::GetCurSel() const
{
	return m_nCurSel;
}

void CGXColorButtonBox::SetCurSel(int nIndex)
{
	m_nCurSel = nIndex;
}

/////////////////////////////////////////////////////////////////////////////
// CGXColorButtonBox message handlers

void CGXColorButtonBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);

	int nIndex;

	if (HitTest(point, nIndex) && nIndex != m_nCurSel)
	{
		InvalidateRect(GetButtonRect(m_nCurSel), FALSE);
		m_nCurSel = nIndex;
		InvalidateRect(GetButtonRect(m_nCurSel), FALSE);
		UpdateWindow();

		GetParent()->SendMessage(WM_GX_COLORCHANGED, GetDlgCtrlID(), 0);
	}
}

void CGXColorButtonBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	for (int i = (m_bDefaultButton ? -1 : 0); i < 20; i++)
		DrawColorButton(&dc, i);
}

CRect CGXColorButtonBox::GetButtonRect(int nIndex) const
{
	CRect rect;
	GetClientRect(rect);

	int dx = rect.Width()/5;
	int dy = rect.Height()/ (m_bDefaultButton ? 5 : 4);
	int ex = dx/15;
	int ey = dy/15;

	if (nIndex == -1)
	{
		// Automatic Button
		rect.SetRect(rect.left+ex,
			rect.top+ey,
			rect.left+dx*5-ex,
			rect.top+dy-ey);
	}
	else if (nIndex >= 0 && nIndex < 20)
	{
		// Color Buttons
		int row = nIndex / 5 + (m_bDefaultButton ? 1 : 0);
		int col = nIndex % 5;
		int x = rect.left+col*dx;
		int y = rect.top+row*dy;

		rect.SetRect(x+ex, y+ey, x+dx-ex, y+dy-ey);
	}
	else
		ASSERT(0);

	return rect;
}

BOOL CGXColorButtonBox::HitTest(const CPoint& pt, int& retVal) const
{
	for (retVal = (m_bDefaultButton ? -1 : 0); retVal < 20; retVal++)
	{
		if (GetButtonRect(retVal).PtInRect(pt))
			return TRUE;
	}

	return FALSE;
}

void CGXColorButtonBox::DrawColorButton(CDC* pDC, int nIndex) const
{
	BOOL bDown = (nIndex == m_nCurSel);

	CRect rc = GetButtonRect(nIndex);

	COLORREF rgbColor = (nIndex >= 0)
		? GXGetVGAColor(nIndex)
		: RGB(255,255,255);

	GXPatB(pDC, rc, rgbColor);

	GXDrawEdge(pDC, rc,
		bDown ? BDR_SUNKENINNER|BDR_SUNKENOUTER
		: BDR_RAISEDINNER|BDR_RAISEDOUTER);

	if (nIndex == -1)
	{
		if (nIndex == m_nCurSel)
		{
			rc.top++;
			rc.left++;
		}

		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(0,0,0));
		pDC->DrawText(szAutomatic, _tcsclen(szAutomatic),
			&rc,
			DT_CENTER | DT_VCENTER);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGXColorSample control

IMPLEMENT_REGISTER(CGXColorSample, GX_GLOBALCLASS, 0, 0, 0);

void CGXColorSample::SetInterior(const CGXBrush& logbrush)
{ 
	m_brInterior = logbrush; 
}

const CGXBrush& CGXColorSample::GetInteriorRef() const
{ 
	return m_brInterior; 
}

int CGXColorSample::Get3dFrame() const
{ 
	return m_n3dFrame; 
}

void CGXColorSample::Set3dFrame(int n)
{ 
	m_n3dFrame = n; 
}

CGXColorSample::CGXColorSample()
{
	m_brInterior.SetStyle(BS_SOLID).SetColor(COLOR_WINDOW);
	m_n3dFrame = 3; // none
}


#if _MFC_VER < 0x0300
WNDPROC* CGXColorSample::GetSuperWndProcAddr()
{
	// Note: it is no longer necessary to override GetSuperWndProcAddr
	//  for each control class with a different WNDCLASS.
	//  This implementation now uses instance data, such that the previous
	//  WNDPROC can be anything.

	static WNDPROC NEAR pfnSuper;
	return &pfnSuper;
}
#endif

BEGIN_MESSAGE_MAP(CGXColorSample, CWnd)
	//{{AFX_MSG_MAP(CGXColorSample)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGXColorSample message handlers

void CGXColorSample::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CString text;
	GetWindowText(text);


	CRect rect;
	GetClientRect(rect);

	if (m_brInterior.GetStyle() == BS_NULL)
		GXPatB(&dc, rect, m_brInterior.GetBkColor());
	else
	{
		CBrush br;

		dc.SetTextColor(m_brInterior.GetColor());
		dc.SetBkColor(m_brInterior.GetBkColor());
		dc.SetBkMode(OPAQUE);            
		LOGBRUSH lb = m_brInterior;
		br.CreateBrushIndirect(&lb);
		dc.FillRect(rect, &br);
	}

	const CRect& r = rect;

	COLORREF rgb3dDkShadow = RGB(0, 0, 0);
	COLORREF rgb3dHilight = GXGetSysData()->GetSysColor(COLOR_BTNHIGHLIGHT);

	switch (m_n3dFrame)
	{
	case gxFrameRaised:
		GXDraw3dFrame(&dc, r.left, r.top, r.right-1, r.bottom-1, 1,
			  rgb3dHilight, rgb3dDkShadow);
		break;

	case gxFrameInset:
		GXDraw3dFrame(&dc, r.left, r.top, r.right-1, r.bottom-1, 1,
			  rgb3dDkShadow, rgb3dHilight);
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGXPatternComboBox control

CGXPatternComboBox::CGXPatternComboBox()
{
	m_rgbBack = RGB(255,255,255);
	m_rgbText = RGB(0,0,0);
}

CGXPatternComboBox::~CGXPatternComboBox()
{
}

COLORREF CGXPatternComboBox::GetTextColor() const
{
	return m_rgbText;
}

void CGXPatternComboBox::SetTextColor(COLORREF rgbColor)
{
	m_rgbText = rgbColor;
}

COLORREF CGXPatternComboBox::GetBackColor() const
{
	return m_rgbBack;
}

void CGXPatternComboBox::SetBackColor(COLORREF rgbColor)
{
	m_rgbBack = rgbColor;
}


BEGIN_MESSAGE_MAP(CGXPatternComboBox, CComboBox)
	//{{AFX_MSG_MAP(CGXPatternComboBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGXPatternComboBox message handlers

void CGXPatternComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rc = lpDrawItemStruct->rcItem;
	// COLORREF rgbColor = lpDrawItemStruct->itemData;
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	GXPatB(pDC, rc, ::GetSysColor(COLOR_WINDOW));
	switch (lpDrawItemStruct->itemID)
	{
	case 0:
		// not include Brush
		pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
		pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		pDC->SetBkMode(OPAQUE);
		pDC->DrawText(_T("*******"), 7, rc, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		break;
	case 1:
		// background color
		GXPatB(pDC, rc, GetBackColor());
		break;
	case 2:
		// solid brush
		GXPatB(pDC, rc, GetTextColor());
		break;
	default:
		// Pattern
		pDC->SetBkColor(GetBackColor());
		pDC->SetTextColor(GetTextColor());
		pDC->SetBkMode(OPAQUE);
		CGXBrush brush;
		if (lpDrawItemStruct->itemID < 3+GXNPENPATTERN1)
			brush.SetPattern(lpDrawItemStruct->itemID-3).SetColor(GetTextColor()).SetBkColor(GetBackColor());
		else
			brush.SetStyle(BS_HATCHED).SetHatch(lpDrawItemStruct->itemID-3-GXNPENPATTERN1).SetColor(GetTextColor()).SetBkColor(GetBackColor());
		LOGBRUSH lb = brush;
		CBrush br;
		br.CreateBrushIndirect(&lb);
		pDC->FillRect(CRect(rc.left+2, rc.top+2, rc.right-2, rc.bottom-2), &br);
	}

	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		const COLORREF rgbBlack = RGB(0,0,0);
		GXPatB(pDC, rc.left, rc.top, 2, rc.Height(), rgbBlack);
		GXPatB(pDC, rc.left, rc.top, rc.Width(), 2, rgbBlack);
		GXPatB(pDC, rc.right-2, rc.top, 2, rc.Height(), rgbBlack);
		GXPatB(pDC, rc.left, rc.bottom-2, rc.Width(), 2, rgbBlack);
		/*CBrush br;
		br.CreateStockObject(BLACK_BRUSH);
		pDC->FrameRect(CRect(rc.left, rc.top, rc.right, rc.bottom), &br);*/
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGXColorPage property page

CGXColorPage::CGXColorPage() : CGXPropertyPage(CGXColorPage::IDD)
{
	//{{AFX_DATA_INIT(CGXColorPage)
	m_n3dFrame = 3;
	//}}AFX_DATA_INIT

	m_logbrushParent
		.SetStyle(BS_NULL)
		.SetColor(GetSysColor(COLOR_WINDOWTEXT))
		.SetBkColor(GetSysColor(COLOR_WINDOW));
	m_logbrushInterior = CGXBrush(BS_NULL, RGB(0,0,0), RGB(255,255,255));
	SetIncludeInterior(FALSE);
}

CGXColorPage::~CGXColorPage()
{
}


#define GX_NHATCH 6


void CGXColorPage::DoDataExchange(CDataExchange* pDX)
{
	CGXPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGXColorPage)
	DDX_Radio(pDX, IDC_GXBTN3DRADIO1, m_n3dFrame);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		if (TextColorWnd().GetCurSel() >= 0)
			m_logbrushInterior.SetColor(GXGetVGAColor(TextColorWnd().GetCurSel()));
		if (BackColorWnd().GetCurSel() >= 0)
			m_logbrushInterior.SetBkColor(GXGetVGAColor(BackColorWnd().GetCurSel()));

		if (m_cbPatterns.GetCurSel() <= 0)
			SetIncludeInterior(FALSE);
		else if (m_cbPatterns.GetCurSel() == 1)
			m_logbrushInterior.SetStyle(BS_NULL), SetIncludeInterior(TRUE);
		else if (m_cbPatterns.GetCurSel() == 2)
			m_logbrushInterior.SetStyle(BS_SOLID), SetIncludeInterior(TRUE);
		else if (m_cbPatterns.GetCurSel() < 3+GXNPENPATTERN1)
			m_logbrushInterior.SetPattern(m_cbPatterns.GetCurSel()-3), SetIncludeInterior(TRUE);
		else
			m_logbrushInterior.SetStyle(BS_HATCHED).SetHatch(m_cbPatterns.GetCurSel()-3-GXNPENPATTERN1), SetIncludeInterior(TRUE);
	}
	else
	{
		if (m_brSample.GetSafeHandle())
			m_brSample.DeleteObject();
		LOGBRUSH lb = m_logbrushInterior;
		m_brSample.CreateBrushIndirect(&lb);

		TextColorWnd().SetCurSel(GXFindColor(m_logbrushInterior.GetColor()));
		BackColorWnd().SetCurSel(GXFindColor(m_logbrushInterior.GetBkColor()));

		if (!GetIncludeInterior())
			m_cbPatterns.SetCurSel(0);
		else if (m_logbrushInterior.GetStyle() == BS_NULL)
			m_cbPatterns.SetCurSel(1);
		else if (m_logbrushInterior.GetStyle() == BS_SOLID)
			m_cbPatterns.SetCurSel(2);
		else if (m_logbrushInterior.GetStyle() == BS_PATTERN)
			m_cbPatterns.SetCurSel(m_logbrushInterior.GetPattern()+3);
		else if (m_logbrushInterior.GetStyle() == BS_HATCHED)
			m_cbPatterns.SetCurSel(min((int) m_logbrushInterior.GetHatch(), GX_NHATCH)+3+GXNPENPATTERN1);
		else // Unsupported style
			m_cbPatterns.SetCurSel(0);

		m_cbPatterns.SetBackColor(m_logbrushInterior.GetBkColor());
		m_cbPatterns.SetTextColor(m_logbrushInterior.GetColor());

		ColorSampleWnd().SetInterior(
			GetIncludeInterior() ? GetInterior() : m_logbrushParent);

		ColorSampleWnd().Set3dFrame(m_n3dFrame);

		// Draw sample
		ColorSampleWnd().Invalidate();
	}
}

// 3d Frame
int CGXColorPage::Get3dFrame() const
{
	return m_n3dFrame; 
}

void CGXColorPage::Set3dFrame(int n)
{ 
	m_n3dFrame = n; 
}

// dialog controls
CGXColorButtonBox& CGXColorPage::BackColorWnd()
{ 
	return m_wndBackColor; 
}

CGXColorButtonBox& CGXColorPage::TextColorWnd()
{ 
	return m_wndTextColor; 
}

CGXColorSample& CGXColorPage::ColorSampleWnd()
	{ return m_wndColorSample; }


BEGIN_MESSAGE_MAP(CGXColorPage, CGXPropertyPage)
	//{{AFX_MSG_MAP(CGXColorPage)
	ON_MESSAGE(WM_GX_COLORCHANGED, OnChangedColor)
	ON_BN_CLICKED(IDC_GXBTN3DRADIO1, OnGxbtn3dradioClicked)
	ON_BN_CLICKED(IDC_GXBTN3DRADIO2, OnGxbtn3dradioClicked)
	ON_BN_CLICKED(IDC_GXBTN3DRADIO3, OnGxbtn3dradioClicked)
	ON_BN_CLICKED(IDC_GXBTN3DRADIO4, OnGxbtn3dradioClicked)
	ON_CBN_SELCHANGE(IDC_GXCOMBOPATTERN, OnSelchangeGxcombopattern)
	ON_CBN_CLOSEUP(IDC_GXCOMBOPATTERN, OnSelchangeGxcombopattern)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CGXColorPage::GetIncludeInterior() const
{
	return m_bIncludeInterior;
}

void CGXColorPage::SetIncludeInterior(BOOL b)
{
	m_bIncludeInterior = b;
}

const CGXBrush& CGXColorPage::GetInterior() const
{
	return m_logbrushInterior;
}

void CGXColorPage::SetInterior(const CGXBrush& brush)
{
	m_logbrushInterior = brush;
	SetIncludeInterior(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
// CGXColorPage message handlers
BOOL CGXColorPage::OnInitDialog()
{
	m_cbPatterns.SubclassDlgItem(IDC_GXCOMBOPATTERN, this);
	m_wndTextColor.SubclassDlgItem(IDC_GXUSER1, this);
	m_wndBackColor.SubclassDlgItem(IDC_GXUSER2, this);
	m_wndColorSample.SubclassDlgItem(IDC_GXUSER3, this);
	for(int i=0;i<3+GXNPENPATTERN1+GX_NHATCH; i++)m_cbPatterns.AddString(_T(""));
	CGXPropertyPage::OnInitDialog();
	TextColorWnd().m_bDefaultButton = FALSE;
	BackColorWnd().m_bDefaultButton = FALSE;
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
LRESULT CGXColorPage::OnChangedColor(WPARAM wParam, LPARAM /*lParam*/)
{
	if(m_cbPatterns.GetCurSel()<=0)
	{
		// if user pressed a color and no pattern is specified, select the NULL or SOLID brush.
		if(wParam==IDC_GXUSER2)
			m_cbPatterns.SetCurSel(1);// back color hit
		else if(wParam==IDC_GXUSER1)
			m_cbPatterns.SetCurSel(2); // text color
	}
	UpdateData(TRUE);
	ColorSampleWnd().SetInterior(GetIncludeInterior()?GetInterior():m_logbrushParent);
	ColorSampleWnd().Set3dFrame(m_n3dFrame);
	m_cbPatterns.SetBackColor(m_logbrushInterior.GetBkColor());
	m_cbPatterns.SetTextColor(m_logbrushInterior.GetColor());
	// Draw sample
	ColorSampleWnd().Invalidate();m_cbPatterns.Invalidate();return 0;
}
void CGXColorPage::OnSelchangeGxcombopattern(){OnChangedColor(0,0);}
void CGXColorPage::OnGxbtn3dradioClicked(){OnChangedColor(0,0);}
/////////////////////////////////////////////////////////////////////////////
// CGXBorderPage property page
// CGXBorderTypeButton control
CGXBorderTypeButton::CGXBorderTypeButton(){m_bActive=FALSE;m_bCheckBox=FALSE;m_bInclude=FALSE;}
void CGXBorderTypeButton::SetLogPen(const LOGPEN& logpen)
{
	if(m_pen.GetSafeHandle())m_pen.DeleteObject();
	m_logpen = logpen;
	CGXBrush brush;
	brush.SetColor(logpen.lopnColor);
#if _MFC_VER >= 0x0300 && !defined(_MAC) && !defined(_UNIX_)
	if (!GXGetAppData()->bWin31)
		m_pen.CreatePen(logpen.lopnStyle | PS_GEOMETRIC | PS_ENDCAP_FLAT, logpen.lopnWidth.x, brush, 0);
	else
#endif
	m_pen.CreatePenIndirect((LOGPEN*) &logpen);
	ASSERT(m_pen.GetSafeHandle());
}
const LOGPEN&CGXBorderTypeButton::GetLogPen(){ASSERT(m_bInclude);return m_logpen;}
void CGXBorderTypeButton::EnableCheckBox(BOOL b){m_bCheckBox = b;}
void CGXBorderTypeButton::SetActive(BOOL b){m_bActive = b;}
void CGXBorderTypeButton::SetInclude(BOOL b){m_bInclude = b;}
void CGXBorderTypeButton::SetColor(COLORREF rgbColor){m_logpen.lopnColor=rgbColor;SetLogPen(m_logpen);}
int CGXBorderTypeButton::GetInclude(){return m_bInclude;}
CGXBorderTypeButton::~CGXBorderTypeButton(){if(m_pen.GetSafeHandle())m_pen.DeleteObject();}
BEGIN_MESSAGE_MAP(CGXBorderTypeButton, CButton)
	//{{AFX_MSG_MAP(CGXBorderTypeButton)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CGXBorderTypeButton message handlers
void CGXBorderTypeButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (m_pen.GetSafeHandle() == NULL)
	{
		TRACE0("CGXBorderTypeButton::DrawItem called without valid pen\n");
//      ASSERT(0);
		return;
	}
	CRect rc = lpDrawItemStruct->rcItem;
	CRect rc1 = rc;
	rc1.top++;
	rc1.left++;
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
#ifdef _UNIX_
	// GetState is not supported by Wind/U
	BOOL bDown = GetCheck() || m_bActive;
#else
	BOOL bDown = GetState() & 0x0004 || m_bActive;
#endif

	GXPatB(pDC, rc1, GXGetSysData()->GetSysColor(COLOR_WINDOW));

	if (m_bCheckBox || m_bActive)
		GXDrawEdge(pDC, rc1, BDR_SUNKENINNER|BDR_SUNKENOUTER);
	else
		GXDrawEdge(pDC, rc1, BDR_RAISEDINNER|BDR_RAISEDOUTER);
	
	CBrush br;

#ifdef _UNIX_
	// GetState is not supported by Wind/U
	if (GetCheck() || (lpDrawItemStruct->itemState & ODS_FOCUS) && !m_bActive)
#else
	if (GetState() & 0x0004 || GetState() & 0x0008 && !m_bActive)
#endif
		br.CreateStockObject(DKGRAY_BRUSH);
	else if (m_bActive)
		br.CreateStockObject(BLACK_BRUSH);
	else
		br.CreateSolidBrush(GXGetSysData()->GetSysColor(COLOR_BTNFACE));

	pDC->FrameRect(rc, &br);

	if (m_bCheckBox)
	{
		if (!GetInclude())
		{
			CBrush br2;
			rc.top++;
			rc.left++;
			br2.CreateStockObject(GRAY_BRUSH);
			pDC->FillRect(CRect(rc.left+3, rc.top+3, rc.right-3, rc.bottom-3), &br2);
			return;
		}
	}
	else if(bDown){rc.top++;rc.left++;}
	CPoint pt1(rc.left+5, rc.top+rc.Height()/2);
	CPoint pt2(rc.right-5, rc.top+rc.Height()/2);
	CPen* pOldPen = pDC->SelectObject(&m_pen);
	if (pOldPen == NULL)
	{
		TRACE0("CGXBorderTypeButton::DrawItem failed to select pen into dc\n");
		ASSERT(0);return;
	}
	pDC->MoveTo(pt1);
	pDC->LineTo(pt2);
	pDC->SelectObject(pOldPen);
}
/////////////////////////////////////////////////////////////////////////////
// CGXColorComboBox control
CGXColorComboBox::CGXColorComboBox(){}
CGXColorComboBox::~CGXColorComboBox(){}
BEGIN_MESSAGE_MAP(CGXColorComboBox, CComboBox)
	//{{AFX_MSG_MAP(CGXColorComboBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CGXColorComboBox message handlers
void CGXColorComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rc = lpDrawItemStruct->rcItem;
	COLORREF rgbColor = lpDrawItemStruct->itemData;
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	GXPatB(pDC, rc, rgbColor);
	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		CBrush br;
		br.CreateStockObject(BLACK_BRUSH);
		pDC->FrameRect(CRect(rc.left, rc.top, rc.right, rc.bottom), &br);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CGXBorderPage property page
CGXBorderPage::CGXBorderPage() : CGXPropertyPage(CGXBorderPage::IDD)
{
	//{{AFX_DATA_INIT(CGXBorderPage)
	//}}AFX_DATA_INIT
	m_nCurType = 0;
	m_bRange = FALSE;
	m_nCurBorder = 0;
	for(UINT i=0;i<8;i++){m_btnType[i].SetLogPen(penArray[i]);}
	m_btnType[m_nCurType].SetActive(TRUE);
	for (UINT i = 0; i < 5; i++)
	{
		m_btnBorder[i].SetLogPen(penArray[0]);
		m_btnBorder[i].EnableCheckBox(TRUE);
		m_btnBorder[i].SetInclude(FALSE);
	}
}

CGXBorderPage::~CGXBorderPage()
{
}

void CGXBorderPage::DoDataExchange(CDataExchange* pDX)
{
	CGXPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGXBorderPage)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGXBorderPage, CGXPropertyPage)
	//{{AFX_MSG_MAP(CGXBorderPage)
	ON_CBN_SELENDOK(IDC_GXCOMBOCOLOR, OnSelendokCombocolor)
	ON_CBN_CLOSEUP(IDC_GXCOMBOCOLOR, OnCloseupCombocolor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGXBorderPage message handlers

BOOL CGXBorderPage::OnInitDialog()
{
	CGXPropertyPage::OnInitDialog();

	for (UINT i = 0; i < 8; i++)
		VERIFY(m_btnType[i].SubclassDlgItem(IDC_GXBTNTYPE1+i, this));

	for (UINT i = 0; i < 5; i++)
		VERIFY(m_btnBorder[i].SubclassDlgItem(IDC_GXBTNBORDERALL+i, this));

	VERIFY(m_lboxColors.SubclassDlgItem(IDC_GXCOMBOCOLOR, this));

	for (int n = 0; n < 20; n++)
		m_lboxColors.AddString((LPCTSTR) GXGetVGAColor(n));

	m_lboxColors.SetCurSel(0);

	if (m_bRange)
	{
		GetDlgItem(IDC_GXBTNBORDERALL)->EnableWindow(TRUE);
		GetDlgItem(IDC_GXBTNBORDERALL)->SetFocus();
		GetDlgItem(IDC_GXTEXTALL)->EnableWindow(TRUE);
		m_nCurBorder = 0;
	}
	else
	{
		GetDlgItem(IDC_GXBTNBORDERALL+1)->SetFocus();
		m_nCurBorder = 1;
	}

	return FALSE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CGXBorderPage::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (wParam >= IDC_GXBTNTYPE1 && wParam <= IDC_GXBTNTYPE8)
	{
		m_btnType[m_nCurType].SetActive(FALSE);
		m_btnType[m_nCurType].Invalidate(FALSE);

		m_nCurType = wParam-IDC_GXBTNTYPE1;

		m_btnType[m_nCurType].SetActive(TRUE);
		m_btnType[m_nCurType].Invalidate(FALSE);

		// Update current border
		m_btnBorder[m_nCurBorder].SetInclude(TRUE);
		m_btnBorder[m_nCurBorder].SetLogPen(penArray[m_nCurType]);

		int n = m_lboxColors.GetCurSel();
		m_btnBorder[m_nCurBorder].SetColor(GXGetVGAColor(n));
		m_btnBorder[m_nCurBorder].Invalidate(FALSE);
	}
	else if (wParam >= IDC_GXBTNBORDERALL && wParam <= IDC_GXBTNBORDERBOTTOM)
	{
		m_nCurBorder = wParam-IDC_GXBTNBORDERALL;
		BOOL b = m_btnBorder[m_nCurBorder].GetInclude();
		m_btnBorder[m_nCurBorder].SetInclude(!b);
		m_btnBorder[m_nCurBorder].SetLogPen(penArray[m_nCurType]);

		int n = m_lboxColors.GetCurSel();
		m_btnBorder[m_nCurBorder].SetColor(GXGetVGAColor(n));
		m_btnBorder[m_nCurBorder].Invalidate(FALSE);
	}

	return CGXPropertyPage::OnCommand(wParam, lParam);
}

void CGXBorderPage::OnSelendokCombocolor()
{
	int n = m_lboxColors.GetCurSel();
	for (int i = 0; i < 8; i++)
	{
		m_btnType[i].SetColor(GXGetVGAColor(n));
		m_btnType[i].Invalidate(FALSE);
	}

	// Update current border
	if (!m_btnBorder[m_nCurBorder].GetInclude())
	{
		m_btnBorder[m_nCurBorder].SetInclude(TRUE);
		m_btnBorder[m_nCurBorder].SetLogPen(penArray[m_nCurType]);
	}

	m_btnBorder[m_nCurBorder].SetColor(GXGetVGAColor(n));
	m_btnBorder[m_nCurBorder].Invalidate(FALSE);
}

void CGXBorderPage::OnCloseupCombocolor()
{
}

void CGXBorderPage::SetIncludeBorders(int nBorder, BOOL b)
{
	ASSERT(nBorder >= -1 && nBorder <= 4);

	m_btnBorder[nBorder+1].SetInclude(b);
}

void CGXBorderPage::SetBorders(int nBorder, const LOGPEN& pen)
{
	ASSERT(nBorder >= -1 && nBorder <= 4);

	m_btnBorder[nBorder+1].SetInclude(TRUE);
	m_btnBorder[nBorder+1].SetLogPen(pen);
}

BOOL CGXBorderPage::GetIncludeBorders(int nBorder)
{
	ASSERT(nBorder >= -1 && nBorder <= 4);

	return m_btnBorder[nBorder+1].GetInclude();
}

const LOGPEN& CGXBorderPage::GetBorders(int nBorder)
{
	ASSERT(nBorder >= -1 && nBorder <= 4);

	return m_btnBorder[nBorder+1].GetLogPen();
}

void CGXBorderPage::EnableRangeButton(BOOL bVisible)
{
	m_bRange = bVisible;
}

BOOL CGXBorderPage::PreTranslateMessage(MSG* pMsg)
{
	if (IsDialogMessage(pMsg))
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			CWnd* pWndFocus = GetFocus();
			int nId = pWndFocus->GetDlgCtrlID();

			if (nId >= IDC_GXBTNBORDERALL && nId <= IDC_GXBTNBORDERBOTTOM)
				m_nCurBorder = nId-IDC_GXBTNBORDERALL;
		}

		return TRUE;
	}
	else
		return CWnd::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CGXAlignmentPage property page


CGXAlignmentPage::CGXAlignmentPage() : CGXPropertyPage(CGXAlignmentPage::IDD)
{
	//{{AFX_DATA_INIT(CGXAlignmentPage)
	m_nAllowEnter = FALSE;
	m_nWrapText = FALSE;
	m_nHorzAlign = -1;
	m_nVertAlign = -1;
	m_nAutoSize = FALSE;
	//}}AFX_DATA_INIT
}

CGXAlignmentPage::~CGXAlignmentPage()
{
}

void CGXAlignmentPage::DoDataExchange(CDataExchange* pDX)
{
	CGXPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGXAlignmentPage)
	DDX_Check(pDX, IDC_GXCHECKENTER, m_nAllowEnter);
	DDX_Check(pDX, IDC_GXCHECKWRAP, m_nWrapText);
	DDX_Radio(pDX, IDC_GXRADIOSTANDARD, m_nHorzAlign);
	DDX_Radio(pDX, IDC_GXRADIOVSTANDARD, m_nVertAlign);
	DDX_Check(pDX, IDC_GXCHECKAUTOSIZE, m_nAutoSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGXAlignmentPage, CGXPropertyPage)
	//{{AFX_MSG_MAP(CGXAlignmentPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGXAlignmentPage message handlers



/////////////////////////////////////////////////////////////////////////////
// CGXControlPage property page

CGXControlPage::CGXControlPage()
	: CGXPropertyPage(CGXControlPage::IDD)
{
	//{{AFX_DATA_INIT(CGXControlPage)
	m_nReadOnly = FALSE;
	m_sChoices = _T("");
	m_nValue = FALSE;
	m_sValue = _T("");
	m_sMaxLength = _T("");
	m_nTriState = FALSE;
	m_nEnabled = FALSE;
	//}}AFX_DATA_INIT
	// m_nVertScrollbar = FALSE;
	m_nControl = WORD_MAX;

	m_pControlNameMap = NULL;
}

CGXControlPage::~CGXControlPage()
{
}

void CGXControlPage::DoDataExchange(CDataExchange* pDX)
{
	CGXPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGXControlPage)
	DDX_Control(pDX, IDC_GXCOMBOCONTROLS, m_cbControlName);
	DDX_Control(pDX, IDC_GXEDITVALUE, m_editValue);
	DDX_Control(pDX, IDC_GXCHECKVALUE, m_checkValue);
	DDX_Check(pDX, IDC_GXCHECKREADONLY, m_nReadOnly);
	DDX_Text(pDX, IDC_GXEDITLIST, m_sChoices);
	DDX_Check(pDX, IDC_GXCHECKVALUE, m_nValue);
	DDX_Text(pDX, IDC_GXEDITVALUE, m_sValue);
	DDX_Text(pDX, IDC_GXEDITMAXLENGTH, m_sMaxLength);
	DDV_MaxChars(pDX, m_sMaxLength, 3);
	DDX_Check(pDX, IDC_GXCHECKTRISTATE, m_nTriState);
	DDX_Check(pDX, IDC_GXCHECKENABLED, m_nEnabled);
	//}}AFX_DATA_MAP
	// DDX_Check(pDX, IDC_GXCHECKVERTSCROLLB, m_nVertScrollbar);

	if (pDX->m_bSaveAndValidate)
	{
		m_nControl = WORD_MAX;  // WORD_MAX indicates that no control is specified

		int index = m_cbControlName.GetCurSel();
		if (index != CB_ERR)
		{
			WORD w;
			CString* pStrName;
			CString sCurSel;

			m_cbControlName.GetLBText(index, sCurSel);

			for (POSITION pos = m_pControlNameMap->GetStartPosition(); pos; )
			{
				m_pControlNameMap->GetNextAssoc(pos, w, (void*&) pStrName);
				if (*pStrName == sCurSel)
				{
					m_nControl = w;
					break;
				}
			}
		}
	}
}


BEGIN_MESSAGE_MAP(CGXControlPage, CGXPropertyPage)
	//{{AFX_MSG_MAP(CGXControlPage)
	ON_BN_CLICKED(IDC_GXCHECKVALUE, OnGxcheckValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGXControlPage message handlers

BOOL CGXControlPage::OnInitDialog()
{
	ASSERT(m_pControlNameMap);
	// ASSERTION-> m_pControlMap is not initialized! ->END

	CGXPropertyPage::OnInitDialog();

	OnGxcheckValue();

	// Block: Control box
	{
		m_cbControlName.ResetContent();

		CString sCurSel;
		CString* pStrName;
		WORD w;
		for (POSITION pos = m_pControlNameMap->GetStartPosition(); pos; )
		{
			m_pControlNameMap->GetNextAssoc(pos, w, (void*&) pStrName);
			m_cbControlName.AddString(*pStrName);
			if (w == m_nControl)
				sCurSel = *pStrName;
		}
		if (!sCurSel.IsEmpty())
			m_cbControlName.SelectString(-1, sCurSel);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CGXControlPage::OnGxcheckValue()
{
	if (m_checkValue.GetCheck() == 0)
		m_checkValue.SetCheck(1);
	m_editValue.EnableWindow(m_checkValue.GetCheck() == 1);
}

/////////////////////////////////////////////////////////////////////////////
// CGXUserAttributPage

#define Grid() ((CGXGridWnd*) GetDlgItem(IDC_GXUSRATTR_GRIDWND))
#define Page() ((CGXUserAttributPage*) GetParent())

// CGXUserAttributPageGrid control

IMPLEMENT_REGISTER(CGXUserAttributPageGrid, GX_GLOBALCLASS, 0, 0, 0)

BOOL CGXUserAttributPageGrid::GetStyleRowCol(ROWCOL nRow, ROWCOL nCol, CGXStyle& style, GXModifyType mt, int nType)
{
	BOOL bRet = CGXGridCore::GetStyleRowCol(nRow, nCol, style, mt, nType);

	if (mt == gxRemove || nType == -1 || nCol == 0) // not supported
		return bRet;

	if (nRow == 0)
	{
		// Column headers
		if (nCol == 1)
			style.SetValue(m_sNameTitle);
		else if (nCol == 2)
			style.SetValue(_T(""));
		else if (nCol == 3)
			style.SetValue(m_sValueTitle);
	}
	else
	{
		CGXStylesMap::UserAttrInfo* pInfo;
		Page()->m_pUserAttrInfoMap->Lookup(Page()->m_arrayID[(int) nRow-1], (void*&) pInfo);
		if (nCol == 1)
		{
			style
				.SetValue(pInfo->sName)
				.SetControl(GX_IDS_CTRL_STATIC);
		}
		else if (nCol == 2)
		{
			style
				.SetValue((WORD) Page()->m_arrayInclude[(int) nRow-1])
				.SetHorizontalAlignment(DT_CENTER)
				.SetVerticalAlignment(DT_VCENTER)
				.SetInterior(GXSYSCOLOR(COLOR_BTNFACE));
		}
		else if (nCol == 3)
		{
			if (pInfo->pStyle)
				style.ChangeStyle(*pInfo->pStyle, gxOverride);
			if (!Page()->m_arrayInclude[(int) nRow-1])
				style.SetTextColor(GXSYSCOLOR(COLOR_GRAYTEXT));
			style.SetValue(Page()->m_arrayContent[(int) nRow-1]);
		}
	}

	// unreferenced:
	mt;

	return TRUE;
}

BOOL CGXUserAttributPageGrid::StoreStyleRowCol(ROWCOL nRow, ROWCOL nCol, const CGXStyle* pStyle, GXModifyType mt, int nType)
{
	BOOL bRet = CGXGridCore::StoreStyleRowCol(nRow, nCol, pStyle, mt, nType);

	if (mt == gxRemove || nType == -1 || nCol < 2 || nRow == 0 || pStyle == NULL) // not supported
		return bRet;

	if (nCol == 2)
		Page()->m_arrayInclude[(int) nRow-1] = (BYTE) pStyle->GetShortValue();
	else if (nCol == 3)
		Page()->m_arrayContent[(int) nRow-1] = pStyle->GetValue();

	return TRUE;
}

void CGXUserAttributPageGrid::OnInitialUpdate()
{
	CRect rect;
	GetClientRect(rect);

	SetParam(new CGXGridParam);
	GetParam()->SetProperties(new CGXProperties);

	CGXStylesMap* pStyMap = new CGXStylesMap;
	pStyMap->CreateStandardStyles();
	GetParam()->SetStylesMap(pStyMap);

	CGXGridWnd::OnInitialUpdate();

	CGXStyle* pStandardStyle;
	pStyMap->LookupStyle(pStyMap->m_wStyleStandard, pStandardStyle);
	pStandardStyle->SetInterior(GXSYSCOLOR(COLOR_WINDOW))
		.SetAutoSize(TRUE)
		.SetVerticalAlignment(DT_VCENTER)
		.SetBorders(gxBorderAll, CGXPen(PS_SOLID, 3, GXSYSCOLOR(COLOR_BTNFACE)));

	CGXStyle* pColHeaderStyle;
	pStyMap->LookupStyle(pStyMap->m_wStyleColHeader, pColHeaderStyle);
	pColHeaderStyle->SetEnabled(FALSE);

	GetParam()->EnableUndo(FALSE);

	SetRowCount(Page()->m_arrayID.GetSize());
	SetFrozenCols(1,1);
	SetColCount(3);

	SetScrollBarMode(SB_VERT, gxnAutomatic | gxnEnhanced);
	SetScrollBarMode(SB_HORZ, gxnDisabled);

	SetColWidth(0,0,0);

	GetParam()->EnableTrackColWidth(FALSE);
	GetParam()->EnableTrackRowHeight(
		GX_TRACK_INDIVIDUAL | GX_TRACK_NOTHEADER);

	GetParam()->SetActivateCellFlags(GX_CAFOCUS_SETCURRENT);

	GetParam()->GetProperties()->SetDisplayVertLines(FALSE);
	GetParam()->GetProperties()->SetDisplayHorzLines(FALSE);

	GetParam()->EnableSelection(FALSE);

	GetParam()->GetProperties()->SetMarkColHeader(FALSE);

	m_sNameTitle.LoadString(GX_IDS_USERATTRNAME);
	m_sValueTitle.LoadString(GX_IDS_USERATTRTITLE);

	SetStyleRange(CGXRange().SetCols(2,2), CGXStyle()
		.SetControl(GX_IDS_CTRL_CHECKBOX3D));
	SetStyleRange(CGXRange().SetCols(3,3), CGXStyle()
		.SetDraw3dFrame(gxFrameInset));

	ResizeColWidthsToFit(CGXRange(1,1,GetRowCount(),1));
	if (GetColWidth(1) > rect.Width()/2-20)
		SetColWidth(1,1,rect.Width()/2-20);
	SetColWidth(2,2,25);
	SetColWidth(3,3,rect.Width()-GetColWidth(1) - GetColWidth(2) - GetSystemMetrics(SM_CXVSCROLL));
	ResizeRowHeightsToFit(CGXRange(1,1,GetRowCount(),3));

	if (GetRowCount() > 0)
		SetCurrentCell(1, 3);

	GetParam()->EnableUndo(TRUE);

	UpdateScrollbars();
}

BOOL CGXUserAttributPageGrid::CanSelectCurrentCell(BOOL bSelect, ROWCOL dwSelectRow, ROWCOL dwSelectCol, ROWCOL dwOldRow, ROWCOL dwOldCol)
{
	if (dwSelectCol == 1)
		return FALSE;

	return CGXGridWnd::CanSelectCurrentCell(bSelect,
		dwSelectRow, dwSelectCol, dwOldRow, dwOldCol);
}

void CGXUserAttributPageGrid::OnModifyCell(ROWCOL nRow, ROWCOL nCol)
{
	if (nCol == 3 && Page()->m_arrayInclude[(int) nRow-1] == 0)
	{
		Page()->m_arrayInclude[(int) nRow-1] = 1;
		RedrawRowCol(CGXRange(nRow, 2, nRow, 3));
	}
}

void CGXUserAttributPageGrid::OnClickedButtonRowCol(ROWCOL nRow, ROWCOL nCol)
{
	if (nCol == 3 && Page()->m_arrayInclude[(int) nRow-1] == 0)
	{
		Page()->m_arrayInclude[(int) nRow-1] = 1;
		RedrawRowCol(CGXRange(nRow, 2, nRow, 3));
	}
	else if (nCol == 2)
		RedrawRowCol(CGXRange(nRow, 3, nRow, 3));
}

/////////////////////////////////////////////////////////////////////////////
// CGXUserAttributPage property page

CGXUserAttributPage::CGXUserAttributPage()
	: CGXPropertyPage(CGXUserAttributPage::IDD)
{
	//{{AFX_DATA_INIT(CGXUserAttributPage)
	//}}AFX_DATA_INIT
	// m_nVertScrollbar = FALSE;
}

CGXUserAttributPage::~CGXUserAttributPage()
{
}

void CGXUserAttributPage::DoDataExchange(CDataExchange* pDX)
{
	CGXPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGXUserAttributPage)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGXUserAttributPage, CGXPropertyPage)
	//{{AFX_MSG_MAP(CGXUserAttributPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGXUserAttributPage message handlers

BOOL CGXUserAttributPage::OnInitDialog()
{
	m_userGrid.SubclassDlgItem(IDC_GXUSRATTR_GRIDWND, this);

	CGXPropertyPage::OnInitDialog();

	Grid()->Initialize();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CGXBasedOnPage property page

CGXBasedOnPage::CGXBasedOnPage() : CGXPropertyPage(CGXBasedOnPage::IDD)
{
	//{{AFX_DATA_INIT(CGXBasedOnPage)
	//}}AFX_DATA_INIT
}

CGXBasedOnPage::~CGXBasedOnPage()
{
}

void CGXBasedOnPage::DoDataExchange(CDataExchange* pDX)
{
	CGXPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGXBasedOnPage)
	DDX_Control(pDX, IDC_GXCOMBOBASEDON, m_cbBasedOn);
	DDX_Text(pDX, IDC_GXCOMBOBASEDON, m_sBasedOn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGXBasedOnPage, CGXPropertyPage)
	//{{AFX_MSG_MAP(CGXBasedOnPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGXBasedOnPage message handlers

BOOL CGXBasedOnPage::OnInitDialog()
{
	CGXPropertyPage::OnInitDialog();

	m_cbBasedOn.ResetContent();

	for (POSITION pos = m_BasedOnStringList.GetHeadPosition(); pos; )
	{
		CString name = m_BasedOnStringList.GetNext(pos);
		m_cbBasedOn.InsertString(-1, name);
	}

	int index;
	if ((index = m_cbBasedOn.FindStringExact(-1, m_sBasedOn)) != CB_ERR)
		m_cbBasedOn.SetCurSel(index);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CGXFloatCellPage property page

CGXFloatCellPage::CGXFloatCellPage() : CGXPropertyPage(CGXFloatCellPage::IDD)
{
	//{{AFX_DATA_INIT(CGXFloatCellPage)
	m_nFloatCell = FALSE;
	m_nFloodCell = FALSE;
	//}}AFX_DATA_INIT
}

CGXFloatCellPage::~CGXFloatCellPage()
{
}

void CGXFloatCellPage::DoDataExchange(CDataExchange* pDX)
{
	CGXPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGXFloatCellPage)
	DDX_Check(pDX, IDC_GXCHECKFLOATCELL, m_nFloatCell);
	DDX_Check(pDX, IDC_GXCHECKFLOODCELL, m_nFloodCell);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGXFloatCellPage, CGXPropertyPage)
	//{{AFX_MSG_MAP(CGXFloatCellPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGXMergeCellPage property page


CGXMergeCellPage::CGXMergeCellPage() : CGXPropertyPage(CGXMergeCellPage::IDD)
{
	//{{AFX_DATA_INIT(CGXMergeCellPage)
	m_nMergeCell = -1;
	//}}AFX_DATA_INIT
}

CGXMergeCellPage::~CGXMergeCellPage()
{
}

void CGXMergeCellPage::DoDataExchange(CDataExchange* pDX)
{
	CGXPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGXMergeCellPage)
	DDX_Radio(pDX, IDC_GXRADIOMERGE, m_nMergeCell);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGXMergeCellPage, CGXPropertyPage)
	//{{AFX_MSG_MAP(CGXMergeCellPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CGXFormatPage dialog


CGXFormatPage::CGXFormatPage() : CGXPropertyPage(CGXFormatPage::IDD)
{
	//{{AFX_DATA_INIT(CGXFormatPage)
	//}}AFX_DATA_INIT
	m_nFormat = -1;
}

CGXFormatPage::~CGXFormatPage()
{
}

void CGXFormatPage::DoDataExchange(CDataExchange* pDX)
{
	CGXPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGXFormatPage)
	DDX_Control(pDX, IDC_GXCOMBO_FORMAT, m_cbFormat);
	DDX_Text(pDX, IDC_GXEDIT_PLACES, m_strPlaces);
	//}}AFX_DATA_MAP
	DDV_MinMaxUInt(pDX, _ttoi(m_strPlaces), 0, 23);

	if (pDX->m_bSaveAndValidate)
	{
		// Face Name
		CString s;
		m_cbFormat.GetWindowText(s);
		m_nFormat = m_cbFormat.FindStringExact(-1, s);
	}
	else
	{
		m_cbFormat.SetCurSel(m_nFormat);
	}
}
BEGIN_MESSAGE_MAP(CGXFormatPage, CGXPropertyPage)
	//{{AFX_MSG_MAP(CGXFormatPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CGXFormatPage message handlers
BOOL CGXFormatPage::OnInitDialog()
{
	CGXPropertyPage::OnInitDialog();
	// TODO: Add extra initialization here
	CString sFormatList;
	sFormatList.LoadString(GX_IDS_CHOICE_FORMAT);
	CString s;
	while (GXGetNextLine(sFormatList, s) >= 0)m_cbFormat.AddString(s);
	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}
