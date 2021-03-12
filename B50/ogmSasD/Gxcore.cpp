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

// gxcore.cpp : implementation of the CGXGridCore class
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

#ifndef _GXEXT_H_
#include "gxext.h"
#endif

#ifndef _GXCORE_H_
#include "gxcore.h"
#endif

#ifndef _GXCTRL_H_
#include "gxctrl.h"
#endif

#ifndef _GXDLL_H_
#include "gxdll.h"
#endif

#ifndef _GXMSG_H_
#include "gxmsg.h"
#endif

#ifndef _AFXPRIV_H_
#include "afxpriv.h"
#endif

#ifndef _GXPLUGIN_H_
#include "gxplugin.h"
#endif

#ifndef _GXABSTR_H_
#include "gxabstr.h"
#endif

#include "gxpriv.h"


#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXCORE")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef TICKCOUNT
#define _TC_(f) f
#else
#define _TC_(f)
#endif

// Control CWnd IDs

#define new DEBUG_NEW

#ifdef TICKCOUNT
DWORD tickIndFont = 0, tickStdFont = 0, tickDrawItem = 0, tickDraw = 0, tickLookup = 0;
DWORD tickLookupFont = 0, tickLoadFont = 0, tickCompStd = 0, tickComposeStyle = 0;
DWORD tick1 = 0, tick2 = 0, tick3 = 0, tick4 = 0, tick5 = 0, tick6 = 0, tick7 = 0;
DWORD tick8 = 0, tick9 = 0, tick10 = 0;
#endif

#ifndef _GXDLL
GX_CURSOR_STATE GXNEAR _gxCursorState;
#endif

/////////////////////////////////////////////////////////////////////////////
// GX_CURSOR_STATE

GX_CURSOR_STATE::GX_CURSOR_STATE()
{ 
	m_hcurLast = m_hcurDestroy = 0; m_idcPrimaryLast = 0; 
	m_bNotFound = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CGXGridCore

// this file contains following methods from CGXGridCore:

// - Initialization, Client Area and Parameter Object
// - Cell Formatting
// - Cell coordinates, Inserting, Moving and Removing cells, frozen cells
// - Methods for Windows messages


/////////////////////////////////////////////////////////////////////////////
// CGXGridCore construction/destruction

CGXGridCore::CGXGridCore(CWnd* pGridWnd, CDocument* pGridDoc, BOOL bIsViewContext)
{
	m_pAppData = GXGetAppData();

	ASSERT(m_pAppData->m_bInitDone);
	// ASSERTION-> This constructor is called before the GXInit()
	// method has been called. This can be problematic because
	// at this time no factory object is available.
	//
	// Make sure you didn't simply forget to call GXInit() from
	// your application's InitInstance method. 
	//
	// Another reason for this problem can be that the object is 
	// constructed at the same time that your application object 
	// is instantiated, for example if you embedded this class 
	// as member variable in the application class. 
	// 
	// To resolve the problem we recommend you instantiate the
	// object on the heap. Change your class declaration and
	// declare a pointer to this class. After GXInit() has been
	// called you can call m_pGrid = new CGXGridCore. In your
	// class destructor don't forget to delete m_pGrid.
	// ->END

	// pointer to window object
	m_pGridWnd = pGridWnd;
	m_bIsViewContext = bIsViewContext;
	m_pGridDoc = pGridDoc;

	// support for splitter windows
	m_pSplitterWnd = NULL;
	m_nSplitRow = 0;
	m_nSplitCol = 0;
	m_pWndLastFocus = NULL;

	// Default control
	m_pDefaultControl = NULL;
	m_nLastControlHit = 0;
	m_pLastControlHit = NULL;

	// parameter object
	m_pParam          = NULL;
	m_bOwnParam       = FALSE;

	// Topleft cell
	m_nTopRow = 1;
	m_nLeftCol = 1;

	// Current selection
	m_SelRectId = 0;

	// Grid window rectangle
	m_bRectDisplay = FALSE;
	m_nZoom = 100;

	// limit cell size so that it is not larger than
	// the grid area.
	m_bLimitRowHeight = TRUE;
	m_bLimitColWidth = TRUE;

	// active cell
	m_nEditRow = 0;
	m_nEditCol = 0;

	m_bActiveEditCell = FALSE;
	m_bActiveFrame = FALSE;
	m_bCacheCurrentCell = TRUE;     // set this FALSE if you want to outline the current cell with special formattings
	m_bRefreshOnSetCurrentCell = FALSE;
		// If your cells appearence is depending on whether
		// it is a current cell or not, you should set
		// m_bRefreshOnSetCurrentCell = TRUE. Otherwise
		// a smarter redrawing of the cell will be used.
	m_pCurrentCellControl = NULL;
	m_bDropDownCell = FALSE;

	// DBCS characters
	m_bDoubleByteChar = FALSE;

	// previous current cell coordinates (used by OnInitCurrentCell())
	m_nLastInitRow = GX_INVALID;
	m_nLastInitCol = GX_INVALID;

	m_dwButtonDownRow = 1;
	m_dwButtonDownCol = 1;
	m_dwLastMoveRow = 1;
	m_dwLastMoveRow = 1;

	// System Metrics
	m_dyBorder   = GetSystemMetrics(SM_CYBORDER);
	m_dxBorder   = GetSystemMetrics(SM_CXBORDER);
	m_dyFrame    = GetSystemMetrics(SM_CYFRAME);
	m_dxFrame    = GetSystemMetrics(SM_CXFRAME);

	// update, hint
	m_bHintsEnabled = FALSE;
	m_nViewID = 0;

	// colors, pen styles
	m_psSelDragLine  = PS_DASH;

	m_idcCursor = 0;

	// Display row/column header
	m_bDisplayHeaderRow = TRUE;
	m_bDisplayHeaderCol = TRUE;

	m_bPrintInProgress = FALSE;
	m_nPrintTopRow = 1;
	m_nPrintLeftCol = 1;
	m_nPrintHandleRecordCount = gxCountPrint;
	m_bPrintCurSelOnly = FALSE;

	m_bOnlyValueNeeded = FALSE;
	m_bNoValueNeeded = FALSE;
	m_bDisableMouseMoveOver = TRUE;
	m_nHitState = 0;
	m_bEditBtnHit = FALSE;
	m_bLockDrawSelectionFrame = FALSE;

	m_bVertScrollBar = TRUE;
	m_bHorzScrollBar = TRUE;
	m_nLastSBRowCount = 0;
	m_nLastSBColCount = 0;
	m_bDoScrollNoUpdateWindow = FALSE;

	// Draw using a memory dc (less flickering but slower)
	m_nDrawingTechnique = gxDrawDirectToDC;

	// Formula Engine support
	m_pDefaultDataClass = RUNTIME_CLASS(CGXData);
	m_bRefreshOnlyUpdatedCells = FALSE;
	m_bLockAddRecalcRange = FALSE;
	m_nExpressionValueType = m_pAppData->m_nExpressionValueType;	// formula engine will set this to GX_VALUE_EXPRESSION

	// Private members
	m_hcurArrow  = ::LoadCursor(NULL, IDC_ARROW);

	m_bAutoScroll    = FALSE;
	m_bAutoScrollStart = FALSE;
	m_bAutoScrollStop = FALSE;
	m_bSpeedKey = FALSE;
	m_bAccelArrowKey = FALSE;
	m_bAccelScrollbars = FALSE;
	m_nAutoScrollDirection = -1;
	m_bUpdateScrollbar = FALSE;
	m_bLockedScrollbars = FALSE;
	m_nVScrollSetting = 0;
	m_nHScrollSetting = 0;

	m_bInitDone = FALSE;
	m_bTracking = FALSE;
	m_bTrackMove = FALSE;
	m_bCellHit = FALSE;
	m_bSelDrag = FALSE;
	m_pWndDrag = NULL;

	m_nVScrollFactor = 2;
	m_nHScrollFactor = 2;

	m_nLastVisibleCol =
	m_nLastVisibleRow = GX_INVALID;

	m_cxOld = m_cyOld = 0;
	m_bLockResize = FALSE;
	m_bZoomed = FALSE;

	m_bValidatingCell = FALSE;
	m_bLockActivateGrid = FALSE;
	m_bLockCurrentCell = FALSE;
	m_bWarningTextDisplayed = FALSE;
	m_bRefreshControl = FALSE;
	m_bIgnoreFocus = FALSE;
	m_bDrawCoveredCell = FALSE;
	m_bForceDrawBackground = FALSE;
	m_nNestedDraw = 0;
	m_pDrawStruct = NULL;
	m_pOldDrawStruct = NULL;
	m_bInvertRect = FALSE;
	m_bForceOldDrawing = FALSE;
	m_bLockEvalFloatCells = FALSE;
	m_bLockEvalMergeCells = FALSE;

	// cache for styles created by LookupStyleRowCol
	m_nAltStylesRows = 92;
	m_nAltStylesCols = 64;
	m_nAltStylesCount = m_nAltStylesRows*m_nAltStylesCols;
	m_nAltStylesColOffs = 16;
	m_nAltStylesRowOffs = 16;

	m_papAltStyles = NULL;

	m_mapStyles.InitHashTable(257);     // prime number 

	// Grid Device Context
	m_pGridDC = NULL;
	m_hOldFont = 0;

	// avoid freeing cells next time FreeObjectCache() is called
	m_nLockRow = m_nLockCol = 0;
	m_nLockRow2 = m_nLockCol2 = 0;
	m_bLockGDIObjects = FALSE;
	m_bLockStyleEdit = FALSE;

	// buffered style - objects
	m_pStyleValue = NULL;
	m_pStyleGetValue = NULL;
	m_pStyleClass = NULL;
	m_pStyleEdit = NULL;

	// Standard styles
	m_pStyleStandard =
	m_pStyleRowHeader =
	m_pStyleColHeader = NULL;

	// Drag and Drop
#if _MFC_VER >= 0x0400 && !defined(_AFX_NO_OLE_SUPPORT)
	m_pOleDataSourceProxy = NULL;
	m_nDndRowExt = 0;
	m_nDndColExt = 0;

	m_nRowsCopied = 0;
	m_nColsCopied = 0;

	m_nDndLastRow = 0;
	m_nDndLastCol = 0;
	m_nDndDropeffect = 0;
	m_bDndGlobalTimer = FALSE;
	m_dwDndDropTargetFlags = GX_DNDEGDESCROLL | GX_DNDAUTOSCROLL | GX_DNDTEXT | GX_DNDSTYLES;
	m_bDndLastScrollCode2 = FALSE;
	m_nDndLastScrollCode = 0;
	m_pRegDropTarget = NULL;
#endif
	m_bHitTestSelEdge = FALSE;
	m_nHitTestFrame = 8;
	m_nDndFlags = GX_DNDDISABLED;
	m_pCurrentCellControl = NULL;
	// old settings
	// m_nClipboardFlags = GX_DNDCOLHEADER|GX_DNDROWHEADER
	//  |GX_DNDDIFFRANGEDLG|GX_DNDTEXT|GX_DNDSTYLES;

	// new settings: no col header, no row header, no "Diff Range" dialog
	m_nClipboardFlags = GX_DNDTEXT|GX_DNDSTYLES|GX_DNDCOMPOSE;

	// Tooltips
#if _MFC_VER >= 0x0400
	m_bTtnIsEnabled = FALSE;
	m_pTooltipProxy = NULL;
#endif

	m_pOldStyle = NULL;

	// Undo, Redo 
	m_pUndoImp = NULL;
	m_bBlockCmd = FALSE;
	m_bCellUndone = FALSE;

	m_nLastRow = 0;
	m_nLastCol = 0;
	m_sTransText.Empty();
	m_sEditRedo.Empty();
	m_sEditUndo.Empty();

	memset(&begin_component_ptr, 0, (&end_component_ptr-&begin_component_ptr)*sizeof(void*));

	CGXAbstractControlFactory* pControlFactory = m_pAppData->m_pControlFactory;
	if (pControlFactory && !m_bInitDone)
		pControlFactory->InitializeGridComponents(this);
}

CGXGridCore::~CGXGridCore()
{
#ifdef _CCHIT
	{
		TRACE0("Cells composed more than once:\n");
		LONG l;
		DWORD dw;
		POSITION pos = m_mapCellHit.GetStartPosition();
		while (pos)
		{
			m_mapCellHit.GetNextAssoc(pos, dw, l);
			if (l > 1)
				TRACE3("(%u,%u): %ld\n", GXDWordToRow(dw), GXDWordToCol(dw), l);
		}
	}
#endif

	ResetGrid();

#if _MFC_VER >= 0x0400
	// OLE data source
	delete m_pOleDataSourceProxy;
	m_pOleDataSourceProxy = NULL;

	// Tooltips
	delete m_pTooltipProxy;
	m_pTooltipProxy = NULL;
#endif

	delete m_pStyleValue;
	delete m_pStyleGetValue;
	delete m_pStyleEdit;

	// buffered style - objects
	m_pStyleValue = NULL;
	m_pStyleGetValue = NULL;
	m_pStyleClass = NULL;
	m_pStyleEdit = NULL;

	// Grid Local Data
	GXDeleteAllObjects(m_mapGridLocalData);

	{
		POSITION pos = m_autodeleteptrlist.GetHeadPosition();
		while (pos)
			delete m_autodeleteptrlist.GetNext(pos);
		m_autodeleteptrlist.RemoveAll();
		// memset(&begin_component_ptr, 0, (&end_component_ptr-&begin_component_ptr)*sizeof(void*));
	}

	// Plugins
	GXDeleteAllObjects(m_pluginList);

	// Style buffer
	int n = m_StyleBuffer.GetCount();

	while (n-- > 0)
	{
		CGXStyle* pStyle = (CGXStyle*) m_StyleBuffer.RemoveHead();
		delete pStyle;
	}

	if (m_papAltStyles)
	{
		for (unsigned w = 0; w < m_nAltStylesCount; w++)
		{
			CGXStyle* pStyle = (CGXStyle*) m_papAltStyles[w];
			delete pStyle;
		}

		delete m_papAltStyles;
		m_papAltStyles = NULL;
	}

	{
		for (POSITION pos = m_mapStyles.GetStartPosition(); pos; )
		{
			CGXStyle* pStyle;
			GXNDX ndx;
			m_mapStyles.GetNextAssoc(pos, ndx, (CObject*&) pStyle);
			delete pStyle;
		}

		m_mapStyles.RemoveAll();
	}

	delete m_pGridDC;

	m_pAppData->DetachGrid(this);
}

void CGXGridCore::ResetGrid()
{
	// destroy controls
	void* vptr;
	CGXControl* pControl;

	for (POSITION pos = m_ControlMap.GetStartPosition(); pos; )
	{
		m_ControlMap.GetNextAssoc(pos, vptr, (void*&) pControl);
		delete pControl;
	}

	m_ControlMap.RemoveAll();

	// cached objects
	FreeObjectCache();

	// Parameter Object
	if (m_bOwnParam)
		delete m_pParam;
	m_pParam = NULL;

	m_nLastControlHit = 0;
	m_pLastControlHit = NULL;
	m_pCurrentCellControl = NULL;
	if (m_pStyleValue) m_pStyleValue->Free();
	if (m_pStyleGetValue) m_pStyleGetValue->Free();
	if (m_pStyleEdit) m_pStyleEdit->Free();
}

// Initialization of all associated objects

void CGXGridCore::OnGridInitialUpdate()
{
	WNDCLASS wcls;
	if (::GetClassInfo(AfxGetInstanceHandle(), _T("GXWND"), &wcls) == 0)
	{
		ASSERT(0);
		// ASSERTION-> Did you call GXInit() in the InitInstance method
		// of your application object? ->END
		AfxThrowNotSupportedException();
	}

	m_pAppData->m_bDiscardNcActivate = FALSE;

	POSITION pos = m_pluginList.GetHeadPosition();
	while (pos)
	{
		CGXPluginComponent* pComp = (CGXPluginComponent*) m_pluginList.GetNext(pos);
		pComp->PlugIn(m_pGridWnd);
	}

#if _MFC_VER >= 0x0400
	if (m_pTooltipProxy)
		m_pTooltipProxy->InitToolTip();
#endif

	// buffered style - objects
	if (m_pStyleValue == NULL)
		m_pStyleValue = CreateStyle();

	if (m_pStyleGetValue == NULL)
		m_pStyleGetValue = CreateStyle();

	m_pStyleClass = m_pStyleValue->GetRuntimeClass();

	if (m_pStyleEdit == NULL)
		m_pStyleEdit = CreateStyle();

	// check parameter object
	CGXGridParam* pParam = GetParam();

	if (pParam == NULL)
		SetParam(pParam = new CGXGridParam);

	m_pAppData->AttachGridToObject(pParam, this);

	FreeObjectCache();

	// check properties object
	if (pParam->GetProperties() == NULL)
	{
		// create a new one
		CGXProperties* pProperties = new CGXProperties;

		// Initialize it
		pProperties->AddDefaultUserProperties();
		
		if (m_pAppData->m_pGridProfileImp)
			pProperties->ReadProfile();

		// and connect it with the parameter object
		pParam->SetProperties(pProperties);

		// the parameter class will destroy the object
	}

	m_pAppData->AttachGridToObject(pParam->GetProperties(), this);

	// check styles
	if (pParam->GetStylesMap() == NULL)
	{
		CGXStylesMap* stylesmap = new CGXStylesMap(m_pStyleClass);
		stylesmap->CreateStandardStyles();
		if (m_pAppData->m_pGridProfileImp)
			stylesmap->ReadProfile();
		pParam->SetStylesMap(stylesmap);
	}

	m_pAppData->AttachGridToObject(pParam->GetStylesMap(), this);

	m_pStyleStandard =
	m_pStyleRowHeader =
	m_pStyleColHeader = NULL;

	// Standard Styles
	CGXStylesMap* pStyMap = pParam->m_pStylesMap;

	VERIFY(pStyMap->LookupStyle(pStyMap->m_wStyleStandard, m_pStyleStandard));
	// ASSERTION-> standard styles not registered.
	// Did you call CGXStylesMap::CreateStandardStyles()? ->END

	VERIFY(pStyMap->LookupStyle(pStyMap->m_wStyleRowHeader, m_pStyleRowHeader));
	// ASSERTION-> standard styles not registered.
	// Did you call CGXStylesMap::CreateStandardStyles()? ->END

	VERIFY(pStyMap->LookupStyle(pStyMap->m_wStyleColHeader, m_pStyleColHeader));
	// ASSERTION-> standard styles not registered.
	// Did you call CGXStylesMap::CreateStandardStyles()? ->END

	ASSERT(m_pStyleStandard && m_pStyleColHeader && m_pStyleRowHeader);

	ASSERT_VALID(m_pStyleStandard);
	ASSERT_VALID(m_pStyleColHeader);
	ASSERT_VALID(m_pStyleRowHeader);

	// Register default control
	CGXAbstractControlFactory* pControlFactory = m_pAppData->m_pControlFactory;
	if (pControlFactory)
	{
		WORD nDefaultId = m_pStyleStandard->GetControl();
		CGXControl* pControl = pControlFactory->CreateControl(nDefaultId, this);
		if (pControl)
			RegisterControl(nDefaultId, pControl, TRUE /*  set default control */, FALSE);
	}

	// check data
#if _MFC_VER >= 0x0300
	ASSERT(m_pDefaultDataClass->IsDerivedFrom(RUNTIME_CLASS(CGXData)));
	// ASSERTION-> m_pDefaultDataClass must be derived from CCGXData ->END
#endif

	if (pParam->GetData() == NULL)
	{
		pParam->SetData((CGXData*) m_pDefaultDataClass->CreateObject());
		// m_pDefaultDataClass is CGXData by default 
		// or CGXFormulaSheet if EnableFormulaEngine has been called

		// Give CGXData a chance to adjust default parameter settings
		// (e.g. Formula Engine prefers direct copy/paste as default)
		pParam->GetData()->InitParam(pParam);
	}

	m_pAppData->AttachGridToObject(pParam->GetData(), this);

	pParam->m_bIsInit = TRUE;   // mark parameter object as initialized

	m_nZoom = pParam->m_pProperties->GetLastZoom();

	// calculate normal-text-font height and width
	GetDefaultFontSize(m_dxWidth, m_dyHeight);

	// mark as initialized
	m_bInitDone = TRUE;

	// set topleft cell
	m_nTopRow = max(GetFrozenRows()+1, m_nTopRow);
	m_nLeftCol = max(GetFrozenCols()+1, m_nLeftCol);

	// CGXTabWnd support
	if (m_pAppData->m_pTabWndInt)
		// Load sheet name from tabwindow if sheet-name is empty
		// or update tab beam if sheet-name is not empty
		m_pAppData->m_pTabWndInt->UpdateTabName(m_pGridWnd, pParam->m_sSheetName, !pParam->m_sSheetName.IsEmpty());

	// Sync current cell with existing views
	if (GetParam()->IsSyncCurrentCell()
		&& (GetColCount() >= GetParam()->m_nLastSyncEditCol)
		&& (GetRowCount() >= GetParam()->m_nLastSyncEditRow)
		)
	{
		ROWCOL nRow = GetParam()->m_nLastSyncEditRow;
		ROWCOL nCol = GetParam()->m_nLastSyncEditCol;

		StoreCurrentCell(TRUE, nRow, nCol);

		// update the record status beam
		UpdateRecordInfoWnd();
	}

	m_rgLastSelectionFrame.m_bValid = FALSE;
	if (!GetParam()->GetRangeList()->IsEmpty())
		m_rgLastSelectionFrame = *GetParam()->GetRangeList()->GetTail();

	m_pCurrentCellControl = NULL;

	UpdateFontMetrics();    // Actualizes the internally stored font metrics.
	UpdateScrollbars();     // Updates the scrollbars
}

// parameter object

void CGXGridCore::SetParam(CGXGridParam* pParam, BOOL bMustDelete)
{
	if (pParam == NULL)
		pParam = new CGXGridParam;

	if (pParam == NULL)
		::AfxThrowMemoryException();

	ASSERT(pParam->IsKindOf(RUNTIME_CLASS(CGXGridParam)));
	ASSERT_VALID(pParam);

	if (m_pParam && m_bOwnParam)
		delete m_pParam;

	// Connect the parameter object
	m_pParam = pParam;
	m_bOwnParam = bMustDelete;
}

int CGXGridCore::GetViewID()
{
	return m_nViewID;
}

void CGXGridCore::EnableHints(BOOL b)
{ 
	m_bHintsEnabled = b;

#ifdef _DEBUG
	if (b)
		GX_CHECKIMP(m_pUpdateHintImp, "CGXGridCore::EnableHints");
#endif
}

// Client Area

void CGXGridCore::SetGridRect(BOOL bSet, LPRECT pRect /* = NULL*/)
{
	ASSERT(bSet == FALSE || pRect != NULL);
	// ASSERTION-> missing rect ->END

	m_bRectDisplay = bSet;
	if (pRect)
		m_rectDisplay = *pRect;
}

// Client Area depending on Printing or Display
CRect CGXGridCore::GetGridRect()
{
	CRect rect;

	if (m_bPrintInProgress)
		rect = m_rectPrint;
	else if (m_bRectDisplay)
		rect = m_rectDisplay;
	else
		m_pGridWnd->GetClientRect(&rect);

	if (rect.right < rect.left || rect.bottom < rect.top)
		rect.SetRect(0,0,0,0);

	return rect;
}

void CGXGridCore::SetPrinting(BOOL b, BOOL bClientToRect /* = FALSE */)
{
	if (b)
	{
		// Reset edit-mode for current cell
		ROWCOL nRow, nCol;
		if (GetCurrentCell(nRow, nCol))
		{
			CGXControl* pControl = GetControl(nRow, nCol);
			//if (pControl->GetModify())
			//  pControl->Store();
			if (pControl->IsActive())
				TransferCurrentCell();
			pControl->SetActive(FALSE);
		}
	}

	m_bPrintInProgress = b;

	if (bClientToRect)
	{
		// Initialize print attributes (see preview-window in print-settings-dialog)
		GetClientRect(m_rectPrint);
		GetDefaultFontSize(m_dxPrintWidth, m_dyPrintHeight);
		m_nPrintLeftCol = GetFrozenCols()+1;
		m_nPrintTopRow  = GetFrozenRows()+1;
	}

	// Initialize properties
	m_pParam->m_pProperties->SetPrinting(b);
}

CWnd* CGXGridCore::GetSharedScrollbarParentWnd() const
{
	CPtrList* pList = &m_pAppData->m_pSharedScrollBarRuntimeClassList;
	POSITION pos = pList->GetHeadPosition();
	CWnd* pWnd = NULL;
	while (pos && pWnd == NULL)
	{
		CRuntimeClass* pRuntimeClass = (CRuntimeClass*) pList->GetNext(pos);
		pWnd = GXGetParentWnd(m_pGridWnd, pRuntimeClass, TRUE);
	}

	return pWnd;
}

// Document
void CGXGridCore::SetModifiedFlag(BOOL bModified)
{
	// Setting the document dirty makes only sense, when the view is
	// linked to a document. If the view is linked to a document,
	// EnableHints(TRUE) should have been called.
	CDocument* pDoc = GetGridDoc();

	if (m_bHintsEnabled && pDoc != NULL)
		pDoc->SetModifiedFlag(bModified);
}


/////////////////////////////////////////////////////////////////////////////
// CGXGridCore diagnostics

#ifdef _DEBUG
void CGXGridCore::AssertValid() const
{
}
/*
void CGXGridCore::Dump(CDumpContext& dc) const
{
/*
// Aktuelle Sicht
	AFX_DUMP1(dc, "\nm_nEditRow         = ", m_nEditRow);
	AFX_DUMP1(dc, "\nm_nEditCol         = ", m_nEditCol);
	AFX_DUMP1(dc, "\nm_nTopRow          = ", m_nTopRow);
	AFX_DUMP1(dc, "\nm_nLeftCol         = ", m_nLeftCol);
	AFX_DUMP1(dc, "\nm_nVScrollFactor   = ", m_nVScrollFactor);
	AFX_DUMP1(dc, "\nm_nHScrollFactor   = ", m_nHScrollFactor);

	AFX_DUMP1(dc, "\nm_bActiveFrame     = ", m_bActiveFrame);
	AFX_DUMP1(dc, "\nm_bActiveEditCell  = ", m_bActiveEditCell);

// Font
	AFX_DUMP1(dc, "\nm_dyHeight         = ", m_dyHeight);
	AFX_DUMP1(dc, "\nm_dxWidth          = ", m_dxWidth);

// interne Statusvariablen
	AFX_DUMP1(dc, "\nm_bInitDone        = ", m_bInitDone);
	AFX_DUMP1(dc, "\nm_bDisplayHeaderRow = ", m_bDisplayHeaderRow);
	AFX_DUMP1(dc, "\nm_bDisplayHeaderCol = ", m_bDisplayHeaderCol);

// Drucken oder Seitenansicht
	AFX_DUMP1(dc, "\nm_bPrintInProgress = ", m_bPrintInProgress);
	AFX_DUMP0(dc, "\nm_rectPrint        = ");
	dc << m_rectPrint;
	AFX_DUMP1(dc, "\nm_nPrintTopRow     = ", m_nPrintTopRow);
	AFX_DUMP1(dc, "\nm_nPrintLeftCol    = ", m_nPrintLeftCol);
	AFX_DUMP1(dc, "\nm_dyPrintHeight    = ", m_dyPrintHeight);
	AFX_DUMP1(dc, "\nm_dxPrintWidth     = ", m_dxPrintWidth);

	AFX_DUMP0(dc, "\nm_awPageFirstRow   = ");
	dc << m_awPageFirstRow;
	AFX_DUMP0(dc, "\nm_awPageFirstCol   = ");
	dc << m_awPageFirstCol;
	AFX_DUMP1(dc, "\nm_nCurrentPageRowIndex = ", m_nCurrentPageRowIndex);
	AFX_DUMP1(dc, "\nm_nCurrentPageColIndex = ", m_nCurrentPageColIndex);

// Paramter
	AFX_DUMP1(dc, "\nm_bOwnParam = ", m_bOwnParam);
	AFX_DUMP0(dc, "\nm_pParam = ");
	dc << m_pParam;
}
*/
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// internal mode

BOOL CGXGridCore::IsTracking(int* pnTrackingMode /*= NULL*/, ROWCOL* pnTracking /*= NULL*/)
{
	if (pnTrackingMode)
		*pnTrackingMode = m_nTrackingMode;
	if (pnTracking)
		*pnTracking = (m_nTrackingMode & GX_TRACKHEIGHT)
			? m_nRowTracking
			: m_nColTracking;
	return m_bTracking;
}

BOOL CGXGridCore::IsDragSelection(int* pnSelDragMode /*= NULL*/)
{
	if (pnSelDragMode)
		*pnSelDragMode = m_nSelDragMode;
	return m_bSelDrag;
}

BOOL CGXGridCore::IsSelectingCells()
{
	return m_nHitState > 0;
}

void CGXGridCore::SetReadOnly(BOOL bReadOnly)
{
	BOOL bOldReadOnly = IsReadOnly();

	if (StoreReadOnly(bReadOnly))
		UpdateReadOnly(bOldReadOnly, TRUE);
}

// ReadOnly

BOOL CGXGridCore::IsReadOnly()
{
	return m_pParam && m_pParam->m_bReadOnly;
}

BOOL CGXGridCore::StoreReadOnly(BOOL bReadOnly)
{
	ASSERT(m_pParam);
	m_pParam->m_bReadOnly = bReadOnly;
	return TRUE;
}

void CGXGridCore::UpdateReadOnly(BOOL bOldReadOnly, BOOL bCreateHint)
{
	ROWCOL nRow, nCol;
	CGXControl *pControl;
	if (GetCurrentCell(nRow, nCol) && (pControl = GetControl(nRow, nCol))->IsActive())
	{
		pControl->Reset();
		pControl->Init(nRow, nCol);
	}

	if (m_pUpdateHintImp && bCreateHint && m_bHintsEnabled)
		m_pUpdateHintImp->HintUpdateReadOnly(this, bOldReadOnly);
}

void CGXGridCore::AutoDeletePtr(void* p)
{
	m_autodeleteptrlist.AddHead(p);
}

