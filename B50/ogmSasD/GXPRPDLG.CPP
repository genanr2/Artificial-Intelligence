// Author: Stefan Hoenig
// gxprpdlg.cpp : implementation of the property dialogs
#include "stdafx.h"
#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif
// Headers
#ifndef _GXVIEW_H_
#include "gxview.h"
#endif
#ifndef _GXWND_H_
#include "gxwnd.h"
#endif
#ifndef _GXEXT_H_
#include "gxext.h"
#endif

#ifndef _GXCTRLI_H_
#include "gxctrli.h"
#endif

#ifndef _GXPRPD_H_
#include "gxprpdlg.h"
#endif

#ifndef _GXTWND_H_
#include "gxtwnd.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXPRPDLG")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static const TCHAR BASED_CODE szAutomatic[] = _T("(none)");

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CGXGridLineColorBox

CGXGridLineColorBox::CGXGridLineColorBox()
{
	m_nCurSel = 0;
}

CGXGridLineColorBox::~CGXGridLineColorBox()
{
}

#if _MFC_VER < 0x0300
WNDPROC* CGXGridLineColorBox::GetSuperWndProcAddr()
{
	// Note: it is no longer necessary to override GetSuperWndProcAddr
	//  for each control class with a different WNDCLASS.
	//  This implementation now uses instance data, such that the previous
	//  WNDPROC can be anything.

	static WNDPROC NEAR pfnSuper;
	return &pfnSuper;
}
#endif

BEGIN_MESSAGE_MAP(CGXGridLineColorBox, CWnd)
	//{{AFX_MSG_MAP(CGXGridLineColorBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CGXGridLineColorBox::GetCurSel() const
{
	return m_nCurSel;
}

void CGXGridLineColorBox::SetCurSel(int nIndex)
{
	m_nCurSel = nIndex;
}

/////////////////////////////////////////////////////////////////////////////
// CGXGridLineColorBox message handlers

void CGXGridLineColorBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);

	int nIndex;

	if (HitTest(point, nIndex) && nIndex != m_nCurSel)
	{
		InvalidateRect(GetButtonRect(m_nCurSel), FALSE);
		m_nCurSel = nIndex;
		InvalidateRect(GetButtonRect(m_nCurSel), FALSE);
		UpdateWindow();

		GetParent()->SendMessage(WM_GX_COLORCHANGED, 0, 0);
	}
}

void CGXGridLineColorBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	for (int i = 0; i < 20; i++)
		DrawColorButton(&dc, i);
}

CRect CGXGridLineColorBox::GetButtonRect(int nIndex) const
{
	CRect rect;
	GetClientRect(rect);

	int dx = rect.Width()/4;
	int dy = rect.Height()/5;
	int ex = dx/15;
	int ey = dy/15;

	if (nIndex == -1)
	{
		// Automatic Button
		rect.SetRect(0,0,0,0);
			/*rect.left+ex,
			rect.top+ey,
			rect.left+dx*5-ex,
			rect.top+dy-ey);*/
	}
	else if (nIndex >= 0 && nIndex < 20)
	{
		// Color Buttons
		int row = nIndex / 4;// + 1;
		int col = nIndex % 4;
		int x = rect.left+col*dx;
		int y = rect.top+row*dy;

		rect.SetRect(x+ex, y+ey, x+dx-ex, y+dy-ey);
	}
	else
		ASSERT(0);

	return rect;
}

BOOL CGXGridLineColorBox::HitTest(const CPoint& pt, int& retVal) const
{
	for (retVal = 0; retVal < 20; retVal++)
	{
		if (GetButtonRect(retVal).PtInRect(pt))
			return TRUE;
	}

	return FALSE;
}

void CGXGridLineColorBox::DrawColorButton(CDC* pDC, int nIndex) const
{
	BOOL bDown = (nIndex == m_nCurSel);

	CRect rc = GetButtonRect(nIndex);

	COLORREF rgbColor = (nIndex >= 0)
		? GXGetVGAColor(nIndex)
		: RGB(255,255,255);

	GXDrawButton(pDC, rc.left+1, rc.top+1, rc.Width()-2, rc.Height()-2,
		bDown, rgbColor);

	CBrush br;
	if (bDown)
		br.CreateStockObject(BLACK_BRUSH);
	else
		br.CreateStockObject(LTGRAY_BRUSH);

	// br.CreateStockObject(BLACK_BRUSH);

	pDC->FrameRect(rc, &br);

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
// CGXDisplayPropertiesDialog dialog

#define CtlListColors()     (*(CListBox*)GetDlgItem(GX_IDC_LISTCOLORS))

CGXDisplayPropertiesDialog::CGXDisplayPropertiesDialog(CGXProperties* pSettings, CGXStylesMap* pStylesMap, BOOL* pbSaveDefault, BOOL bIsChild, UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
{
	//{{AFX_DATA_INIT(CGXDisplayPropertiesDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pSettings = pSettings;
	m_pStylesMap = pStylesMap;
	m_pbSaveDefault = pbSaveDefault;

	m_bChild = bIsChild;
	m_bCurrentDialog = FALSE;

	if (bIsChild)
		Create(nIDTemplate, pParent);

	// m_bHideDialog = FALSE;
}


void CGXDisplayPropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGXDisplayPropertiesDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	if (!pDX->m_bSaveAndValidate || m_bCurrentDialog)
		DDX_Check(pDX, GX_IDC_CHECK_SAVEDEFAULT, *m_pbSaveDefault);

	if (m_pSettings)
	{
		DDX_Check(pDX, GX_IDC_CHECK_3DBTNS, m_pSettings->m_b3dButtons);
		DDX_Check(pDX, GX_IDC_CHECK_HORZLINES, m_pSettings->m_bDisplayHorzLines);
		DDX_Check(pDX, GX_IDC_CHECK_MARKFIXEDCOL, m_pSettings->m_bMarkColHeader);
		DDX_Check(pDX, GX_IDC_CHECK_MARKFIXEDROW, m_pSettings->m_bMarkRowHeader);
		DDX_Check(pDX, GX_IDC_CHECK_VERTLINES, m_pSettings->m_bDisplayVertLines);
	}
}


void CGXDisplayPropertiesDialog::UpdateSettings(BOOL bSave)
{
	if (bSave)
	{
	}
	else
	{
	}
}


BEGIN_MESSAGE_MAP(CGXDisplayPropertiesDialog, CDialog)
	//{{AFX_MSG_MAP(CGXDisplayPropertiesDialog)
	ON_BN_CLICKED(GX_IDC_BTN_PRINTER, OnClickedBtnPrinter)
	ON_BN_CLICKED(GX_IDC_CHECK_3DBTNS, OnCheckedCtl)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_GX_COLORCHANGED, OnChangedColor)
	ON_LBN_SELCHANGE(GX_IDC_LISTCOLORS, OnSelchangeIdcListcolors)
	ON_BN_CLICKED(GX_IDC_CHECK_HORZLINES, OnCheckedCtl)
	ON_BN_CLICKED(GX_IDC_CHECK_MARKFIXEDCOL, OnCheckedCtl)
	ON_BN_CLICKED(GX_IDC_CHECK_MARKFIXEDROW, OnCheckedCtl)
	ON_BN_CLICKED(GX_IDC_CHECK_VERTLINES, OnCheckedCtl)
	ON_WM_NCACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGXDisplayPropertiesDialog message handlers

BOOL CGXDisplayPropertiesDialog::OnInitDialog()
{
	m_wndColorBox.SubclassDlgItem(GX_IDC_COLORBOX, this);
	m_wndUserGrid.SubclassDlgItem(GX_IDC_USERPROPERTIES, this);
	m_wndPreviewGrid.SubclassDlgItem(GX_IDC_PREVIEW_USERCTL, this);

	CDialog::OnInitDialog();

	m_wndPreviewGrid.SetParam(new CGXGridParam);
	m_wndPreviewGrid.GetParam()->SetProperties(m_pSettings, FALSE);
	m_wndPreviewGrid.GetParam()->SetStylesMap(m_pStylesMap, FALSE);
	m_wndPreviewGrid.Initialize();

	m_wndPreviewGrid.SetRowCount(10);
	m_wndPreviewGrid.SetColCount(10);
	m_wndPreviewGrid.SetFrozenCols(1);
	m_wndPreviewGrid.SetStyleRange(CGXRange(1, 1), CGXStyle().SetValue(_T("Hello,")), gxCopy);
	m_wndPreviewGrid.SetStyleRange(CGXRange(2, 1), CGXStyle()
		.SetValue(_T("world!"))
		.SetHorizontalAlignment(DT_RIGHT), gxCopy);
	m_wndPreviewGrid.SetCurrentCell(1,1);

	UpdateSettings(FALSE);

	for (int i = 0; i < m_pSettings->m_ColorNameArray.GetSize(); i++)
		CtlListColors().AddString(m_pSettings->m_ColorNameArray.GetAt(i));

	CtlListColors().SetCurSel(0);

	{
		int i = 0;
		for (POSITION pos = m_pSettings->GetUserPropertyInfoMap().GetStartPosition(); pos;)
		{
			CGXProperties::UserPropertyInfo* pInfo;
			WORD w;

			m_pSettings->GetUserPropertyInfoMap().GetNextAssoc(pos, w, (void*&) pInfo);
			m_arrayID.SetAtGrow(i++, w);
		}
	}

	if (m_arrayID.GetSize() > 0)
		m_wndUserGrid.Initialize();
	else
	{
		m_wndUserGrid.EnableWindow(FALSE);
		m_wndUserGrid.ShowWindow(SW_HIDE);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// =========== Checkboxes ===================

void CGXDisplayPropertiesDialog::OnCheckedCtl()
{
	UpdateData(TRUE);       // store controls contents to settings
	m_wndPreviewGrid.Redraw();
}


// =========== Store Settings, End Dialog ================

void CGXDisplayPropertiesDialog::OnOK()
{
	UpdateData(TRUE);
	UpdateSettings(TRUE);

	if (m_bChild)
		GetParent()->PostMessage(WM_COMMAND, IDOK);
	else
		CDialog::OnOK();
}

void CGXDisplayPropertiesDialog::OnCancel()
{
	if (m_bChild)
		GetParent()->PostMessage(WM_COMMAND, IDCANCEL);
	else
		CDialog::OnCancel();
}

void CGXDisplayPropertiesDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	// m_bHideDialog = !bShow;
	CDialog::OnShowWindow(bShow, nStatus);

	if (bShow)
		UpdateData(FALSE);
	else
		UpdateData(TRUE);

	m_bCurrentDialog = bShow;
}

void CGXDisplayPropertiesDialog::OnClickedBtnPrinter()
{
	CPrintDialog pd(TRUE);
	CView* pView = GXGetActiveView();
	if (pView && pView->IsKindOf(RUNTIME_CLASS(CGXView)))
		((CGXView*) pView)->DoPrintDialog(&pd);
}

LRESULT CGXDisplayPropertiesDialog::OnChangedColor(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	int nVGAIndex, nCurSel;

	// Textcolor
	if ((nVGAIndex = m_wndColorBox.GetCurSel()) == -1)
		return 0;

	if ((nCurSel = CtlListColors().GetCurSel()) == LB_ERR)
		return 0;

	m_pSettings->m_ColorArray[nCurSel] = GXGetVGAColor(nVGAIndex);

	m_wndPreviewGrid.Redraw();

	return 0;
}


void CGXDisplayPropertiesDialog::OnSelchangeIdcListcolors()
{
	int nVGAIndex, nCurSel;

	if ((nCurSel = CtlListColors().GetCurSel()) == LB_ERR)
		return;

	nVGAIndex = GXFindColor(m_pSettings->m_ColorArray[nCurSel]);

	m_wndColorBox.SetCurSel(nVGAIndex);
	m_wndColorBox.Invalidate();
}

BOOL CGXDisplayPropertiesDialog::OnNcActivate(BOOL bActive)
{
	if (GXDiscardNcActivate())
		return TRUE;

	return CDialog::OnNcActivate(bActive);
}

#define GetDialog() ((CGXDisplayPropertiesDialog*) GetParent())
#define Properties() (GetDialog()->m_pSettings)

BOOL CGXUserPropertyGrid::GetStyleRowCol(ROWCOL nRow, ROWCOL nCol, CGXStyle& style, GXModifyType mt, int nType)
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
			style.SetValue(m_sValueTitle);
	}
	else
	{
		CGXProperties::UserPropertyInfo* pInfo;
		Properties()->GetUserPropertyInfoMap().Lookup(GetDialog()->m_arrayID[(int) nRow-1], (void*&) pInfo);
		if (nCol == 1)
		{
			style
				.SetValue(pInfo->sName)
				.SetControl(GX_IDS_CTRL_STATIC);
		}
		else if (nCol == 2)
		{
			if (pInfo->pStyle)
				style.ChangeStyle(*pInfo->pStyle, gxOverride);
		}
	}

	// unreferenced:
	mt;

	return TRUE;
}

BOOL CGXUserPropertyGrid::StoreStyleRowCol(ROWCOL nRow, ROWCOL nCol, const CGXStyle* pStyle, GXModifyType mt, int nType)
{
	BOOL bRet = CGXGridCore::StoreStyleRowCol(nRow, nCol, pStyle, mt, nType);

	if (mt == gxRemove || nType == -1 || nCol != 2 || nRow == 0 || pStyle == NULL) // not supported
		return bRet;

	CGXProperties::UserPropertyInfo* pInfo;
	Properties()->GetUserPropertyInfoMap().Lookup(GetDialog()->m_arrayID[(int) nRow-1], (void*&) pInfo);
	pInfo->pStyle->ChangeStyle(*pStyle, mt);

	return TRUE;
}

void CGXUserPropertyGrid::OnInitialUpdate()
{
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

	SetRowCount(Properties()->GetUserPropertyInfoMap().GetCount());
	SetFrozenCols(1,1);
	SetColCount(2);

	SetScrollBarMode(SB_VERT, gxnAutomatic | gxnEnhanced);
	SetScrollBarMode(SB_HORZ, gxnDisabled);

	CRect rect;
	GetClientRect(rect);

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
		.SetDraw3dFrame(gxFrameInset));

	if (GetRowCount() > 0)
		ResizeColWidthsToFit(CGXRange(1, 1,GetRowCount(),1));
	else
		SetColWidth(1,1,rect.Width()/3);

	if (GetColWidth(1) > rect.Width()/2)
		SetColWidth(1,1,rect.Width()/2);

	SetColWidth(2,2,rect.Width()-GetColWidth(1) - GetSystemMetrics(SM_CXVSCROLL));

	if (GetRowCount() > 0)
	{
		ResizeRowHeightsToFit(CGXRange(1, 1,GetRowCount(),2));
		SetCurrentCell(1, 2);
	}

	GetParam()->EnableUndo(TRUE);

	UpdateScrollbars();
}

BOOL CGXUserPropertyGrid::CanSelectCurrentCell(BOOL bSelect, ROWCOL dwSelectRow, ROWCOL dwSelectCol, ROWCOL dwOldRow, ROWCOL dwOldCol)
{
	if (dwSelectCol == 1)
		return FALSE;

	return CGXGridWnd::CanSelectCurrentCell(bSelect,
		dwSelectRow, dwSelectCol, dwOldRow, dwOldCol);
}


/////////////////////////////////////////////////////////////////////////////
// CGXPrintPropertiesDialog dialog

CGXPrintPropertiesDialog::CGXPrintPropertiesDialog(CGXProperties* pSettings,
		CGXStylesMap* pStylesMap,
		BOOL* pbSaveDefault, CWnd* pParent )
	: CDialog(CGXPrintPropertiesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGXPrintPropertiesDialog)
	m_sMarginBottom = _T("");
	m_sMarginLeft = _T("");
	m_sMarginRight = _T("");
	m_sMarginTop = _T("");
	m_bCenterHorizontal = FALSE;
	m_bCenterVertikal = FALSE;
	m_bColHeaders = FALSE;
	m_bHorzLines = FALSE;
	m_bRowHeaders = FALSE;
	m_bVertLines = FALSE;
	m_bBlackWhite = FALSE;
	m_bPrintFrame = FALSE;
	m_nPageOrder = 0;
	//}}AFX_DATA_INIT

	m_pSettings = pSettings;
	m_pStylesMap = pStylesMap;
	m_pbSaveDefault = pbSaveDefault;
	m_bCurrentDialog = FALSE;
	m_bUseInch = TRUE;

	// Create(IDD, pParent);
}

void CGXPrintPropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGXPrintPropertiesDialog)
	DDX_Text(pDX, GX_IDC_PMARGIN_BOTTOM, m_sMarginBottom);
	DDX_Text(pDX, GX_IDC_PMARGIN_LEFT, m_sMarginLeft);
	DDX_Text(pDX, GX_IDC_PMARGIN_RIGHT, m_sMarginRight);
	DDX_Text(pDX, GX_IDC_PMARGIN_TOP, m_sMarginTop);
	DDX_Check(pDX, GX_IDC_PCENTER_HORIZONTAL, m_bCenterHorizontal);
	DDX_Check(pDX, GX_IDC_PCENTER_VERTIKAL, m_bCenterVertikal);
	DDX_Check(pDX, GX_IDC_PCHECK_COLLABELS, m_bColHeaders);
	DDX_Check(pDX, GX_IDC_PCHECK_HORZLINES, m_bHorzLines);
	DDX_Check(pDX, GX_IDC_PCHECK_ROWLABELS, m_bRowHeaders);
	DDX_Check(pDX, GX_IDC_PCHECK_VERTLINES, m_bVertLines);
	DDX_Check(pDX, GX_IDC_PCHECK_BLACKWHITE, m_bBlackWhite);
	DDX_Check(pDX, GX_IDC_PCHECK_FRAME, m_bPrintFrame);
	DDX_Radio(pDX, GX_IDC_PRADIO_FIRSTROWS, m_nPageOrder);
	//}}AFX_DATA_MAP
	if (!pDX->m_bSaveAndValidate || m_bCurrentDialog)
		DDX_Check(pDX, GX_IDC_PCHECK_SAVEDEFAULT, *m_pbSaveDefault);
}

BEGIN_MESSAGE_MAP(CGXPrintPropertiesDialog, CDialog)
	//{{AFX_MSG_MAP(CGXPrintPropertiesDialog)
	ON_BN_CLICKED(GX_IDC_PBTN_PRINTER, OnBtnPrinter)
	ON_BN_CLICKED(GX_IDC_PCHECK_BLACKWHITE, OnCheckedCtl)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(GX_IDC_PCHECK_COLLABELS, OnCheckedCtl)
	ON_BN_CLICKED(GX_IDC_PCHECK_HORZLINES, OnCheckedCtl)
	ON_BN_CLICKED(GX_IDC_PCHECK_ROWLABELS, OnCheckedCtl)
	ON_BN_CLICKED(GX_IDC_PCHECK_VERTLINES, OnCheckedCtl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// ================== Update settings ======================

void CGXPrintPropertiesDialog::UpdateSettings(BOOL bSave)
{
	if (bSave)
	{
		LPCTSTR szcm = _T("cm");
		int nTop, nLeft, nBottom, nRight;

		m_sMarginTop.MakeLower();
		m_sMarginLeft.MakeLower();
		m_sMarginBottom.MakeLower();
		m_sMarginRight.MakeLower();

		nTop = m_sMarginTop.Find(szcm) != -1 ? CMtoDP(_gxttof(m_sMarginTop)) : INCHtoDP(_gxttof(m_sMarginTop));
		nLeft = m_sMarginLeft.Find(szcm) != -1 ? CMtoDP(_gxttof(m_sMarginLeft)) : INCHtoDP(_gxttof(m_sMarginLeft));
		nBottom = m_sMarginBottom.Find(szcm) != -1 ? CMtoDP(_gxttof(m_sMarginBottom)) : INCHtoDP(_gxttof(m_sMarginBottom));
		nRight = m_sMarginRight.Find(szcm) != -1 ? CMtoDP(_gxttof(m_sMarginRight)) : INCHtoDP(_gxttof(m_sMarginRight));

		m_pSettings->SetMargins(nTop, nLeft, nBottom, nRight);

		m_pSettings->SetPrintHorzLines(m_bHorzLines);
		m_pSettings->SetPrintVertLines(m_bVertLines);
		m_pSettings->SetCenterVertical(m_bCenterVertikal);
		m_pSettings->SetCenterHorizontal(m_bCenterHorizontal);
		m_pSettings->SetPrintRowHeaders(m_bRowHeaders);
		m_pSettings->SetPrintColHeaders(m_bColHeaders);
		m_pSettings->SetBlackWhite(m_bBlackWhite);
		m_pSettings->SetPageOrder(m_nPageOrder);
		m_pSettings->SetPrintFrame(m_bPrintFrame);
	}
	else
	{
		m_bCenterVertikal   = m_pSettings->GetCenterVertical();
		m_bCenterHorizontal = m_pSettings->GetCenterHorizontal();
		m_bHorzLines        = m_pSettings->GetPrintHorzLines();
		m_bVertLines        = m_pSettings->GetPrintVertLines();
		m_bRowHeaders        = m_pSettings->GetPrintRowHeaders();
		m_bColHeaders        = m_pSettings->GetPrintColHeaders();
		m_bBlackWhite       = m_pSettings->GetPrintBlackWhite();
		m_nPageOrder        = m_pSettings->GetPageOrder();
		m_bPrintFrame       = m_pSettings->GetPrintFrame();

		m_sMarginTop.Empty();
		m_sMarginLeft.Empty();
		m_sMarginRight.Empty();
		m_sMarginBottom.Empty();

		int nTop, nLeft, nBottom, nRight;

		if (m_bUseInch)
		{
			LPCTSTR szInch = _T("%.2f in");

			m_pSettings->GetMargins(nTop, nLeft, nBottom, nRight);

			_stprintf(m_sMarginTop.GetBufferSetLength(99), szInch, DPtoINCH(nTop));
			m_sMarginTop.ReleaseBuffer();
			_stprintf(m_sMarginLeft.GetBufferSetLength(99), szInch, DPtoINCH(nLeft));
			m_sMarginLeft.ReleaseBuffer();
			_stprintf(m_sMarginBottom.GetBufferSetLength(99), szInch, DPtoINCH(nBottom));
			m_sMarginBottom.ReleaseBuffer();
			_stprintf(m_sMarginRight.GetBufferSetLength(99), szInch, DPtoINCH(nRight));
			m_sMarginRight.ReleaseBuffer();
		}
		else
		{
			LPCTSTR szCM = _T("%.2f cm");

			m_pSettings->GetMargins(nTop, nLeft, nBottom, nRight);

			_stprintf(m_sMarginTop.GetBufferSetLength(99), szCM, DPtoCM(nTop));
			m_sMarginTop.ReleaseBuffer();
			_stprintf(m_sMarginLeft.GetBufferSetLength(99), szCM, DPtoCM(nLeft));
			m_sMarginLeft.ReleaseBuffer();
			_stprintf(m_sMarginBottom.GetBufferSetLength(99), szCM, DPtoCM(nBottom));
			m_sMarginBottom.ReleaseBuffer();
			_stprintf(m_sMarginRight.GetBufferSetLength(99), szCM, DPtoCM(nRight));
			m_sMarginRight.ReleaseBuffer();
		}
	}
}

void CGXPrintPropertiesDialog::UpdatePreview()
{
	m_wndPreview.Redraw();
}

// =========== message handlers =============================

BOOL CGXPrintPropertiesDialog::OnInitDialog()
{
	VERIFY(m_wndPreview.SubclassDlgItem(GX_IDC_PPREVIEW_USERCTL, this));

	m_wndPreview.SetParam(new CGXGridParam);
	m_wndPreview.GetParam()->SetProperties(m_pSettings, FALSE);
	m_wndPreview.GetParam()->SetStylesMap(m_pStylesMap, FALSE);

	m_wndPreview.Initialize();

	m_wndPreview.SetRowCount(10);
	m_wndPreview.SetColCount(10);
	m_wndPreview.SetFrozenCols(1);
	m_wndPreview.SetValueRange(CGXRange(1, 1), _T("Hello,"), gxCopy);
	m_wndPreview.SetStyleRange(CGXRange(2, 1), CGXStyle()
		.SetValue(_T("world!"))
		.SetHorizontalAlignment(DT_RIGHT), gxCopy);

	UpdateSettings(FALSE);

	CDialog::OnInitDialog();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGXPrintPropertiesDialog::OnBtnPrinter()
{
	CPrintDialog pd(TRUE);
	CView* pView = GXGetActiveView();
	if (pView && pView->IsKindOf(RUNTIME_CLASS(CGXView)))
		((CGXView*) pView)->DoPrintDialog(&pd);
}

void CGXPrintPropertiesDialog::OnCheckedCtl()
{
	UpdateData(TRUE);
	UpdateSettings(TRUE);
	UpdatePreview();
}

void CGXPrintPropertiesDialog::OnOK()
{
	UpdateData(TRUE);
	UpdateSettings(TRUE);

	// GetParent()->PostMessage(WM_COMMAND, IDOK);
	CDialog::OnOK();
}

void CGXPrintPropertiesDialog::OnDestroy()
{

	CDialog::OnDestroy();
}

void CGXPrintPropertiesDialog::OnCancel()
{
	// GetParent()->PostMessage(WM_COMMAND, IDCANCEL);
	CDialog::OnCancel();
}

void CGXPrintPropertiesDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if (bShow)
		UpdateData(FALSE);
	else
		UpdateData(TRUE);

	m_bCurrentDialog = bShow;
}


/////////////////////////////////////////////////////////////////////////////
// CGXPrintPropertiesWnd

CGXPrintPropertiesWnd::CGXPrintPropertiesWnd()
{
}

CGXPrintPropertiesWnd::~CGXPrintPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CGXPrintPropertiesWnd, CGXGridWnd)
	//{{AFX_MSG_MAP(CGXPrintPropertiesWnd)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGXPrintPropertiesWnd message handlers


void CGXPrintPropertiesWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	SetPrinting(TRUE, TRUE);

	DrawUsingMemDC(&dc);

	SetPrinting(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CGXHeaderFooterDialog dialog

CGXHeaderFooterDialog::CGXHeaderFooterDialog(
	CGXProperties* pSettings,
	BOOL* pbSaveDefault,
	CWnd* pParent)
	: CDialog(CGXHeaderFooterDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGXHeaderFooterDialog)
	m_sDistFootNote = _T("");
	m_sDistHeader = _T("");
	m_sPageFirst = _T("");
	//}}AFX_DATA_INIT

	m_pSettings = pSettings;
	m_pbSaveDefault = pbSaveDefault;
	m_wndActive = 0;
	m_bCurrentDialog = FALSE;
	m_bUseInch = TRUE;

	// Create(IDD, pParent);

}

void CGXHeaderFooterDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGXHeaderFooterDialog)
	DDX_Text(pDX, GX_IDC_DIST_FOOTNOTE, m_sDistFootNote);
	DDX_Text(pDX, GX_IDC_DIST_HEADLINE, m_sDistHeader);
	DDX_Text(pDX, GX_IDC_PAGE_FIRST, m_sPageFirst);
	//}}AFX_DATA_MAP
	if (!pDX->m_bSaveAndValidate || m_bCurrentDialog)
		DDX_Check(pDX, GX_IDC_PCHECK_SAVEDEFAULT, *m_pbSaveDefault);

	DDV_GXTabWnd(pDX, &GetTabWnd());
}

CGXTabWnd& CGXHeaderFooterDialog::GetTabWnd()
{ 
	return *(CGXTabWnd*)GetDlgItem(GX_IDC_HFTABWND); 
}

void CGXHeaderFooterDialog::UpdateSettings(BOOL bSave)
{
	LPCTSTR szauto = _T("auto");

	if (bSave)
	{
		LPCTSTR szcm = _T("cm");
		int nTop, nBottom;
		int nStart;

		m_sDistFootNote.MakeLower();
		m_sDistHeader.MakeLower();
		m_sPageFirst.MakeLower();

		nTop = m_sDistHeader.Find(szcm) != -1 ? CMtoDP(_gxttof(m_sDistHeader)) : INCHtoDP(_gxttof(m_sDistHeader));
		nBottom = m_sDistFootNote.Find(szcm) != -1 ? CMtoDP(_gxttof(m_sDistFootNote)) : INCHtoDP(_gxttof(m_sDistFootNote));

		m_pSettings->SetDistances(nTop, nBottom);

		nStart = m_sPageFirst.Find(szauto) != -1 ? -1 : _ttoi(m_sPageFirst);
		m_pSettings->SetFirstPage(nStart);
	}
	else
	{
		m_sDistFootNote.Empty();
		m_sDistHeader.Empty();
		m_sPageFirst.Empty();

		if (m_bUseInch)
		{
			int nTop, nBottom;
			LPCTSTR szInch = _T("%.2f in");

			m_pSettings->GetDistances(nTop, nBottom);

			_stprintf(m_sDistHeader.GetBufferSetLength(99), szInch, DPtoINCH(nTop));
			m_sDistHeader.ReleaseBuffer();
			_stprintf(m_sDistFootNote.GetBufferSetLength(99), szInch, DPtoINCH(nBottom));
			m_sDistFootNote.ReleaseBuffer();
			if (m_pSettings->GetFirstPage() == -1)
				m_sPageFirst = szauto;
			else
			{
				wsprintf(m_sPageFirst.GetBufferSetLength(99), _T("%d"), (int) m_pSettings->GetFirstPage());
				m_sPageFirst.ReleaseBuffer();
			}
		}
		else
		{
			int nTop, nBottom;
			LPCTSTR szCM = _T("%.2f cm");

			m_pSettings->GetDistances(nTop, nBottom);

			_stprintf(m_sDistHeader.GetBufferSetLength(99), szCM, DPtoCM(nTop));
			m_sDistHeader.ReleaseBuffer();
			_stprintf(m_sDistFootNote.GetBufferSetLength(99), szCM, DPtoCM(nBottom));
			m_sDistFootNote.ReleaseBuffer();
			if (m_pSettings->GetFirstPage() == -1)
				m_sPageFirst = szauto;
			else
			{
				wsprintf(m_sPageFirst.GetBufferSetLength(99), _T("%d"), (int) m_pSettings->GetFirstPage());
				m_sPageFirst.ReleaseBuffer();
			}
		}
	}
}


BEGIN_MESSAGE_MAP(CGXHeaderFooterDialog, CDialog)
	//{{AFX_MSG_MAP(CGXHeaderFooterDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(GX_IDC_PBTN_FONT, OnClickedBtnFont)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(GX_IDC_PBTN_PRINTER, OnClickedBtnPrinter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGXHeaderFooterDialog message handlers

HBRUSH CGXHeaderFooterDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL CGXHeaderFooterDialog::OnInitDialog()
{
	UpdateSettings(FALSE);

	CDialog::OnInitDialog();

	GetTabWnd().ShowScrollBar(SB_BOTH, FALSE);

	m_gridHeader.Create(WS_BORDER | WS_VSCROLL, CRect(0,0,1,1), &GetTabWnd(), GetTabWnd().GetNextID());
	m_gridFooter.Create(WS_BORDER | WS_VSCROLL, CRect(0,0,1,1), &GetTabWnd(), GetTabWnd().GetNextID());

	// properties: display 3d-buttons as headers
	properties.SetDisplay3dButtons(TRUE);
	properties.SetMarkRowHeader(FALSE);
	properties.SetMarkColHeader(FALSE);

	// connect objects to parameter object
	parHeader.SetProperties(&properties, FALSE);
	parFooter.SetProperties(&properties, FALSE);

	stylesmap.CreateStandardStyles();
	parHeader.SetStylesMap(&stylesmap, FALSE);
	parFooter.SetStylesMap(&stylesmap, FALSE);

	parHeader.SetData(&m_pSettings->GetDataHeader(), FALSE);
	parFooter.SetData(&m_pSettings->GetDataFooter(), FALSE);

	// connect paramter object to grid
	m_gridHeader.SetParam(&parHeader, FALSE);
	m_gridFooter.SetParam(&parFooter, FALSE);

	// Initialize Grids
	m_gridHeader.Initialize();
	m_gridFooter.Initialize();

	CRect rectTabWnd;
	GetTabWnd().GetClientRect(rectTabWnd);

	// Tab window settings
	CString s;
	s.LoadString(GX_IDS_TEXT_HEADER);
	GetTabWnd().AttachWnd(&m_gridHeader, s);
	s.LoadString(GX_IDS_TEXT_FOOTER);
	GetTabWnd().AttachWnd(&m_gridFooter, s);
	GetTabWnd().GetBeam().DisplayScrollBtns(FALSE);
	GetTabWnd().GetBeam().m_rgbBeam = GXGetSysData()->GetSysColor(COLOR_BTNFACE);   // dialog color
	GetTabWnd().GetBeam().GetTab(0).nWidth = rectTabWnd.Width() / 4;
	GetTabWnd().GetBeam().GetTab(1).nWidth = rectTabWnd.Width() / 4;

	m_gridHeader.SetCurrentCell(1, 2, GX_INVALIDATE);
	m_gridFooter.SetCurrentCell(1, 2, GX_INVALIDATE);

	CGXGridWnd* pWnd = (CGXGridWnd*) GetTabWnd().m_pActivePane;
	if (pWnd)
		GetDlgItem(GX_IDC_PBTN_FONT)->EnableWindow(pWnd->IsCurrentCell());

	// Scrollbars
	m_gridHeader.SetScrollBarMode(SB_HORZ, gxnDisabled);
	m_gridHeader.SetScrollBarMode(SB_VERT, gxnEnabled | gxnEnhanced);
	m_gridFooter.SetScrollBarMode(SB_HORZ, gxnDisabled);
	m_gridFooter.SetScrollBarMode(SB_VERT, gxnEnabled | gxnEnhanced);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGXHeaderFooterDialog::OnClickedBtnFont()
{
	CGXGridWnd* pWnd = (CGXGridWnd*) GetTabWnd().m_pActivePane;
	DoFontDialog(pWnd);
}

void CGXHeaderFooterDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (bShow)
	{
		UpdateData(FALSE);
	}
	else
		UpdateData(TRUE);

	m_bCurrentDialog = bShow;

	CDialog::OnShowWindow(bShow, nStatus);
}

void CGXHeaderFooterDialog::OnClickedBtnPrinter()
{
	CPrintDialog pd(TRUE);
	CView* pView = GXGetActiveView();
	if (pView && pView->IsKindOf(RUNTIME_CLASS(CGXView)))
		((CGXView*) pView)->DoPrintDialog(&pd);
}

void CGXHeaderFooterDialog::OnOK()
{
	UpdateData(TRUE);
	UpdateSettings(TRUE);

	// GetParent()->PostMessage(WM_COMMAND, IDOK);
	CDialog::OnOK();
}

void CGXHeaderFooterDialog::OnCancel()
{
	// GetParent()->PostMessage(WM_COMMAND, IDCANCEL);
	CDialog::OnCancel();
}

void CGXHeaderFooterDialog::DoFontDialog(CGXGridWnd* pGrid)
{
	// if there are no selections, add area with Edit Cell
	CGXRangeList selList;
	if (!pGrid->CopyRangeList(selList, TRUE))
		return;

	ROWCOL nRow = selList.GetHead()->top;
	ROWCOL nCol = selList.GetHead()->left;

	const CGXStyle oldstyle = pGrid->LookupStyleRowCol(nRow, nCol);
	LOGFONT lf = oldstyle.GetFontRef();

	CFontDialog dlg(&lf);

	if (dlg.DoModal() != IDOK)
		return;

	// Selected Cells
	POSITION pos = selList.GetHeadPosition();
	while (pos)
	{
		CGXRange rect = *selList.GetNext(pos);
		rect.ExpandRange(1, 1, pGrid->GetRowCount(), pGrid->GetColCount());

		BOOL bLock = pGrid->LockUpdate();
		pGrid->SetStyleRange(rect, CGXStyle().SetFont(CGXFont(lf)), gxOverride);
		pGrid->ResizeRowHeightsToFit(CGXRange().SetRows(rect.top, rect.bottom));
		pGrid->LockUpdate(bLock);
		pGrid->Redraw();
	}
}


BEGIN_MESSAGE_MAP(CGXHeaderFooterGrid, CGXGridWnd)
	//{{AFX_MSG_MAP(CGXHeaderFooterGrid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CGXHeaderFooterGrid::OnInitCurrentCell(ROWCOL nRow, ROWCOL nCol)
{
	CWnd* pWnd = GetParent()->GetParent()->GetDlgItem(GX_IDC_PBTN_FONT);

	if (pWnd)
		pWnd->EnableWindow(TRUE);

	CGXGridWnd::OnInitCurrentCell(nRow, nCol);
}

BOOL CGXHeaderFooterGrid::OnActivateGrid(BOOL bActivate)
{
	CWnd* pWnd = GetParent()->GetParent()->GetDlgItem(GX_IDC_PBTN_FONT);

	if (pWnd)
		pWnd->EnableWindow(IsCurrentCell());

	return CGXGridWnd::OnActivateGrid(bActivate);
}

void CGXHeaderFooterGrid::OnInitialUpdate()
{
	CGXGridWnd::OnInitialUpdate();

	CGXGridParam* pParam = GetParam();

	pParam->EnableTrackColWidth(FALSE);
	pParam->EnableMoveCols(FALSE);
	pParam->EnableChangeTab(FALSE);
	pParam->EnableTrackRowHeight(GX_TRACK_INDIVIDUAL | GX_TRACK_EXTHITTEST | GX_TRACK_NOTHEADER | GX_TRACK_SMOOTHCELLS);
	pParam->SetActivateCellFlags(GX_CAFOCUS_SETCURRENT);

	// Header
	int nHeight = Height_LPtoDP(GX_NYHEIGHT) * 2;
									// GX_NYHEIGHT is logical height of a line
	pParam->EnableUndo(FALSE);
	SetColCount(3);
	SetRowCount(NROWS);
	SetStyleRange(CGXRange(0, 1, 0, 3), CGXStyle().SetVerticalAlignment(DT_VCENTER), gxOverride);

	CString str;
	str.LoadString(GX_IDS_LEFTALIGN);
	SetValueRange(CGXRange(0, 1), str);

	str.LoadString(GX_IDS_CENTER);
	SetValueRange(CGXRange(0, 2), str);

	str.LoadString(GX_IDS_RIGHTALIGN);
	SetValueRange(CGXRange(0, 3), str);

	SetStyleRange(CGXRange(1, 1, NROWS, 1), CGXStyle().SetHorizontalAlignment(DT_LEFT), gxOverride);
	SetStyleRange(CGXRange(1, 2, NROWS, 2), CGXStyle().SetHorizontalAlignment(DT_CENTER), gxOverride);
	SetStyleRange(CGXRange(1, 3, NROWS, 3), CGXStyle().SetHorizontalAlignment(DT_RIGHT), gxOverride);

	CRect rectTabWnd;
	GetParent()->GetClientRect(rectTabWnd);

	int nx = GetSystemMetrics(SM_CXVSCROLL);
	nx = (rectTabWnd.Width() - nx) / 3;
	nx--;

	SetColWidth(0, 0, 0);
	SetColWidth(1, 1, nx);
	SetColWidth(2, 2, nx);
	SetColWidth(3, 3, nx);
	SetValueRange(CGXRange(1, 0, NROWS, 0), _T(""));
	SetDefaultRowHeight(nHeight);

	GetParam()->SetNewGridLineMode(TRUE);
		// use dotted grid lines
	GetParam()->SetGridLineStyle(PS_DOT);
		// and don't draw grid lines between header cells
	ColHeaderStyle().SetBorders(gxBorderAll, CGXPen().SetStyle(PS_NULL));
	RowHeaderStyle().SetBorders(gxBorderAll, CGXPen().SetStyle(PS_NULL));
	StandardStyle().SetAutoSize(TRUE);

	ResizeRowHeightsToFit(CGXRange().SetRows(1, NROWS));

	pParam->EnableUndo(TRUE);
}

