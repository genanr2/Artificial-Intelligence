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

// gxparam.cpp : implementation of the CGXGridParam class
//

#include "stdafx.h"

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

// Headers

#ifndef _GXCORE_H_
#include "gxcore.h"
#endif

#ifndef _GXEXT_H_
#include "gxext.h"
#endif

#ifndef _GXSTYLES_H_
#include "gxstyles.h"
#endif

#ifndef _GXDELAYR_H_
#include "gxdelayr.h"
#endif

#include "gxpriv.h"

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef GX_USE_SEGMENTS
#pragma code_seg("GX_SEG_GXPARAM")
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_SERIAL(CGXGridParam, CObject, 1);

#define new DEBUG_NEW

//


/////////////////////////////////////////////////////////////////////////////
// CGXGridParam

CGXGridParam::CGXGridParam()
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
	// called you can call m_pParam = new CGXGridParam. In your
	// class destructor don't forget to delete m_pParam.
	// ->END

	// Property-Object
	m_bOwnProperties  = FALSE;
	m_pProperties = NULL;

	// Data-Object
	m_bOwnData = FALSE;
	m_pData = NULL;

	// Styles-Map
	m_bOwnStylesMap = FALSE;
	m_pStylesMap = NULL;

	// PrintDevice-Object
	m_bOwnPD = FALSE;
	m_pPD = NULL;

	m_sSheetName.Empty();

	m_bReadOnly = FALSE;
	m_bThumbTrack = TRUE;
	m_bNeedChangeTab = TRUE;

	m_nFrozenRows = 0;
	m_nFrozenCols = 0;

	// Set factor for row height and column width
	// GX_NXYFACTOR is the logical unit for the character height and width
	m_RowHeights.InitHashTable(1021);
	m_ColWidths.InitHashTable(257);
	m_nDefaultHeight = GX_NYHEIGHT*12/10;  // 1.2
	m_nDefaultWidth  = GX_NXAVGWIDTH*10;     // 10
	m_RowHeights[0]  = GX_NYHEIGHT*14/10;  // 1.4
	m_ColWidths[0]   = GX_NXAVGWIDTH*6;      // 6

	m_wAllowSelection = 0xff;
	m_bAllowSelDragCols =
	m_bAllowSelDragRows = TRUE;
	m_wAllowTrackRowHeight =
	m_wAllowTrackColWidth = GX_TRACK_DEFAULT;

	m_nSpecialMode = 0;

	m_nTrans = 0;
	m_bUndoEnabled = TRUE;

	m_nDefaultComboBoxHeight = 100;
	m_bEatFirstMouseClick = FALSE;
	m_bTransparentBackground = FALSE;
	m_bSmartResize = TRUE;
	m_bScrollLikeExcel = TRUE;

	m_nRemoveRowsFlags =
	m_nRemoveColsFlags = GX_RMUNDO_BASESTYLES | GX_RMUNDO_CELLSTYLES;

	m_nMinTrackRowHeight = m_nMinTrackColWidth = 0;

	m_wActivateCell = GX_CAFOCUS_CLICKINTEXT;


	m_nDrawRowOrder = GX_DRAWORDER_ROWS;

	m_nHideCurrentCell = GX_HIDE_INACTIVEFRAME;

	m_nHeaderRows =
	m_nHeaderCols = 0;

	m_bUpdateLocked = FALSE;

	m_bSyncCurrentCell = FALSE;
	m_nLastSyncEditRow = 1;
	m_nLastSyncEditCol = 1;

	m_bLockReadOnly = TRUE;

	m_bNumberedRowHeaders = TRUE;
	m_bNumberedColHeaders = TRUE;

	m_nUndoFreezeFromCol = 0;
	m_nUndoFreezeToCol = 0;
	m_nUndoFreezeDestCol = 0;
	m_nUndoFreezeFromCol = 0;
	m_nUndoFreezeToCol = 0;
	m_nUndoFreezeDestCol = 0;

	m_bDrawWYSIWYG = FALSE;

	m_nEnterKeyAction = GX_RIGHT;
	m_dwUndoLimit = 512;

	// Excel like selection frame and current cell
	m_bExcelLikeCurrentCell = FALSE;
	m_bExcelLikeSelectionFrame = FALSE;
	m_bSortRowsOnDblClk = FALSE;
	m_bSortColsOnDblClk = FALSE;

	m_nLastSortedRow = GX_INVALID;	// remember last sorted row
	m_nLastSortedCol = GX_INVALID;;

	m_bSortAscending = FALSE;	// remember if last sort was ascending or descending
	
	// New grid line mode (will be loaded from CGXAppData)
	m_bNewGridLineMode = m_pAppData->m_bEnableNewGridLineMode;
	m_nGridLineStyle = (WORD) m_pAppData->m_nDefaultGridLineStyle;

	// Drag&Drop temporary arttributes
	m_bDndSource = FALSE;
	m_nDndStartRow = 0;
	m_nDndStartCol = 0;
	m_pDndSelList = NULL;
	m_nDndRowOffset = 0;
	m_nDndColOffset = 0;

	// Copy paste support without clipboard 
	m_bDirectCopyPaste = FALSE;
	m_bDirectCutPaste = FALSE;
	m_bDirectDragDrop = FALSE;
	m_bCut = FALSE;
	
	// Don't display expressions in current cell - always call GetControlText
	m_nDisplayExpression = 0;

	// Float Cells
	m_wFloatCellsMode = gxnFloatDisable;
	m_pPrintFloatCells = NULL;
	m_pDispFloatCells = NULL;
	m_pDelayedDispFloatCells = NULL;
	m_pDelayedPrintFloatCells = NULL;

	// Merge Cells
	m_wMergeCellsMode = gxnMergeDisable;
	m_pDelayedMergeCells = NULL;
	m_pHorzMergeCells = NULL;
	m_pVertMergeCells = NULL;

	// Tabbed comboboxes
	m_nSerial = 0;

	m_pCoveredCells = NULL;
	InitCoveredCells();

	m_bIsInit = FALSE;
}

void CGXGridParam::InitCoveredCells()
{
	// Covered Cells
	if (m_pCoveredCells)
		return;

	if (m_pAppData->m_pCoveredCellPoolImp)
		m_pCoveredCells = m_pAppData->m_pCoveredCellPoolImp->CreateCoveredCellPool();
	else
		m_pCoveredCells = NULL;

}

CGXGridParam::~CGXGridParam()
{
	// Undo, Redo List
	POSITION pos;
	for (pos = m_UndoList.GetHeadPosition(); pos;)
		delete m_UndoList.GetNext(pos);

	for (pos = m_RedoList.GetHeadPosition(); pos; )
		delete m_RedoList.GetNext(pos);

	m_UndoList.RemoveAll();
	m_RedoList.RemoveAll();

	// Covered Cells
	delete (CObject*) m_pCoveredCells;

	// Merge Cells
	delete m_pDelayedMergeCells;
	delete m_pHorzMergeCells;
	delete m_pVertMergeCells;

	// Float Cells
	delete m_pDelayedDispFloatCells;
	delete m_pDelayedPrintFloatCells;
	delete m_pDispFloatCells;
	delete m_pPrintFloatCells;

	// Tabbed comboboxes
	for (int i = 0; i < m_ArrayOfMapValueToChoice.GetSize(); i++)
	{
		CMapStringToString* p = (CMapStringToString*) m_ArrayOfMapValueToChoice[i];
		delete p;
	}

	for (int i = 0; i < m_ArrayOfMapTextToChoice.GetSize(); i++)
	{
		CMapStringToString* p = (CMapStringToString*) m_ArrayOfMapTextToChoice[i];
		delete p;
	}

	// Styles Map, properties, Data
	if (m_bOwnStylesMap)
		delete m_pStylesMap;

	if (m_bOwnProperties)
		delete m_pProperties;

	if (m_bOwnData)
		delete m_pData;

	if (m_bOwnPD)
		delete m_pPD;
	m_pPD = 0;

	m_sSheetName.Empty();
	

	GXDeleteAllObjects(m_mapParamLocalData);
}

void CGXGridParam::UpdateCoveredCellsList()
{
	if (m_pAppData->m_pCoveredCellPoolImp)
		((CGXAbstractCoveredCellPoolImp*) m_pAppData->m_pCoveredCellPoolImp)
			->InitFromRangeList((CGXCoveredCellPool*) m_pCoveredCells, m_CoveredCellsList);
}

/////////////////////////////////////////////////////////////////////////////
// CGXGridParam serialization

void CGXGridParam::Serialize(CArchive& ar)
{
	GX_CHECKIMP(m_pAppData->m_pDocSerializeImp, "CGXGridParam::Serialize");

	if (m_pAppData->m_pDocSerializeImp)
		m_pAppData->m_pDocSerializeImp->Serialize(this, ar);
}

CGXGridParam& CGXGridParam::operator=(const CGXGridParam& p)
{
	GX_CHECKIMP(m_pAppData->m_pParamCopyOperatorImp, "CGXGridParam::operator=");

	if (m_pAppData->m_pParamCopyOperatorImp)
		m_pAppData->m_pParamCopyOperatorImp->CopyObject(this, p);

	return *this;
}

// Properties

void CGXGridParam::SetProperties(CGXProperties* pProperties /*=NULL*/, BOOL bMustDelete /*=TRUE*/)
{
	ONASSERT(this, return);
	// ASSERTION-> Did you forget to call SetParam(new CGXGridParam)? ->END

	ASSERT_VALID(this);

	ASSERT(pProperties);
	// ASSERTION-> NULL pointer is not allowed
	// Did you allocate the object with 'new CGXStylesMap'? ->END

	ASSERT_VALID(pProperties);
	ASSERT(pProperties->IsKindOf(RUNTIME_CLASS(CGXProperties)));

	if (m_pProperties && m_bOwnProperties)
	{
		TRACE0("Warning: The CGXProperties object that was attached to the parameter object will be destroyed\n");
		TRACE0("and replaced with a new CGXProperties object. You should check your initialization\n");
		TRACE0("routine if this is really wanted behavior.\n");
		delete m_pProperties;
	}

	m_pProperties = pProperties;
	m_bOwnProperties = bMustDelete;
}

// Data

void CGXGridParam::SetData(CGXData* pData, BOOL bMustDelete)
{
	ONASSERT(this, return);
	// ASSERTION-> Did you forget to call SetParam(new CGXGridParam)? ->END

	ASSERT_VALID(this);

	if (pData)
	{
		ASSERT_VALID(pData);
		ASSERT(pData->IsKindOf(RUNTIME_CLASS(CGXData)));
	}

	if (m_pData && m_bOwnData)
	{
		TRACE0("Warning: The CGXData object that was attached to the parameter object will be destroyed\n");
		TRACE0("and replaced with a new CGXData object. You should check your initialization\n");
		TRACE0("routine if this is really wanted behavior.\n");
		delete m_pData;
	}

	m_pData = pData;
	m_bOwnData = bMustDelete;
}

// Styles

void CGXGridParam::SetStylesMap(CGXStylesMap* pStylesMap, BOOL bMustDelete)
{
	ONASSERT(this, return);
	// ASSERTION-> Did you forget to call SetParam(new CGXGridParam)? ->END

	ASSERT_VALID(this);

	ASSERT(pStylesMap);
	// ASSERTION-> NULL pointer is not allowed
	// Did you allocate the object with 'new CGXStylesMap'? ->END

	ASSERT_VALID(pStylesMap);
	ASSERT(pStylesMap->IsKindOf(RUNTIME_CLASS(CGXStylesMap)));

	if (m_pStylesMap && m_bOwnStylesMap)
	{
		TRACE0("Warning: The CGXStylesMap object that was attached to the parameter object will be destroyed\n");
		TRACE0("and replaced with a new CGXStylesMap object. You should check your initialization\n");
		TRACE0("routine if this is really wanted behavior.\n");
		delete m_pStylesMap;
	}

	m_pStylesMap = pStylesMap;
	m_bOwnStylesMap = bMustDelete;
}


// Printer Settings Object

void CGXGridParam::SetPrintDevice(CGXPrintDevice* pPD, BOOL bMustDelete)
{
	ONASSERT(this, return);
	// ASSERTION-> Did you forget to call SetParam(new CGXGridParam)? ->END

	ASSERT_VALID(this);

	if (pPD)
	{
		ASSERT_VALID(pPD);
		ASSERT(pPD->IsKindOf(RUNTIME_CLASS(CGXPrintDevice)));
	}

	if (m_pPD && m_bOwnPD)
	{
		TRACE0("Warning: The CGXPrintDevice object that was attached to the parameter object will be destroyed\n");
		TRACE0("and replaced with a new CGXPrintDevice object. You should check your initialization\n");
		TRACE0("routine if this is really wanted behavior.\n");
		delete m_pPD;
	}

	m_pPD = pPD;
	m_bOwnPD = bMustDelete;
}

CObList& CGXGridParam::GetUndoList()
{
	ASSERT(this);
	return m_UndoList;
}

CObList& CGXGridParam::GetRedoList()
{
	ASSERT(this);
	return m_RedoList;
}

void CGXGridParam::EmptyUndoList()
{
	ASSERT(this);
	POSITION pos;

	for (pos = m_UndoList.GetHeadPosition(); pos;)
		delete m_UndoList.GetNext(pos);

	m_UndoList.RemoveAll();

	m_nTrans = 0;
}

void CGXGridParam::EmptyRedoList()
{
	ASSERT(this);
	POSITION pos;

	for (pos = m_RedoList.GetHeadPosition(); pos;)
		delete m_RedoList.GetNext(pos);

	m_RedoList.RemoveAll();
}

// User Actions

void CGXGridParam::EnableSelection(UINT wFlags)
	{ ASSERT(this); m_wAllowSelection = (WORD) wFlags; }

void CGXGridParam::EnableMoveCols(BOOL b)
	{ ASSERT(this); m_bAllowSelDragCols = b; }

void CGXGridParam::EnableMoveRows(BOOL b)
	{ ASSERT(this); m_bAllowSelDragRows = b; }

void CGXGridParam::EnableTrackRowHeight(UINT wFlags)
	{ ASSERT(this); m_wAllowTrackRowHeight = (WORD) wFlags; }

void CGXGridParam::EnableTrackColWidth(UINT wFlags)
	{ ASSERT(this); m_wAllowTrackColWidth = (WORD) wFlags;}

void CGXGridParam::EnableThumbTrack(BOOL b)
	{ ASSERT(this); m_bThumbTrack = b;}

void CGXGridParam::EnableAllFeatures(BOOL b)
{
	ASSERT(this);
	if (b)
		m_wAllowSelection = GX_SELFULL;

	m_bAllowSelDragCols =
	m_bAllowSelDragRows = b;

	if (b)
		m_wAllowTrackRowHeight =
		m_wAllowTrackColWidth = GX_TRACK_DEFAULT;
	else
		m_wAllowTrackRowHeight =
		m_wAllowTrackColWidth = 0;
}

void CGXGridParam::SetSpecialMode(WORD wFlags)
	{ ASSERT(this); m_nSpecialMode = wFlags; }

BOOL CGXGridParam::IsEnableSelection(WORD wFlags)
{
	ASSERT(this); return (m_wAllowSelection & wFlags) > 0;
}

BOOL CGXGridParam::IsEnableMoveCols()
	{ ASSERT(this); return m_bAllowSelDragCols; }

BOOL CGXGridParam::IsEnableMoveRows()
	{ ASSERT(this); return m_bAllowSelDragRows; }

BOOL CGXGridParam::IsEnableTrackRowHeight(WORD wFlags)
	{ ASSERT(this); return (m_wAllowTrackRowHeight & wFlags) > 0; }

BOOL CGXGridParam::IsEnableTrackColWidth(WORD wFlags)
	{ ASSERT(this); return (m_wAllowTrackColWidth & wFlags) > 0; }

BOOL CGXGridParam::IsSpecialMode(WORD wFlags)
	{ ASSERT(this); return (m_nSpecialMode & wFlags) > 0; }

BOOL CGXGridParam::IsEnableThumbTrack()
	{ ASSERT(this); return m_bThumbTrack; }

void CGXGridParam::EnableUndo(BOOL bEnable)
	{ ASSERT(this); m_bUndoEnabled = bEnable;}

BOOL CGXGridParam::IsEnableUndo()
	{ ASSERT(this); return m_bUndoEnabled; }

void CGXGridParam::EnableChangeTab(BOOL b)
	{ ASSERT(this); m_bNeedChangeTab = b; }

BOOL CGXGridParam::IsEnableChangeTab()
	{ ASSERT(this); return m_bNeedChangeTab; }

#ifdef _DEBUG
void CGXGridParam::AssertValid() const
{
	CObject::AssertValid();

	if (m_bOwnProperties)
		m_pProperties->AssertValid();

	if (m_bOwnData)
		m_pData->AssertValid();

	if (m_bOwnPD)
		m_pPD->AssertValid();

	m_SelRects.AssertValid();
	m_UndoList.AssertValid();
	m_RedoList.AssertValid();
}
/*
void CGXGridParam::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
	AFX_DUMP1(dc, "\nm_nFrozenRows           = ", (UINT)m_nFrozenRows);
	AFX_DUMP1(dc, "\nm_nFrozenCols           = ", (UINT)m_nFrozenCols);
	AFX_DUMP1(dc, "\nm_bReadOnly             = ", (UINT)m_bReadOnly);
	AFX_DUMP1(dc, "\nm_bThumbTrack           = ", (UINT)m_bThumbTrack);
	AFX_DUMP1(dc, "\nm_wAllowSelection       = ", (UINT)m_wAllowSelection);
	AFX_DUMP1(dc, "\nm_bAllowSelDragCols     = ", (UINT)m_bAllowSelDragCols);
	AFX_DUMP1(dc, "\nm_bAllowSelDragRows     = ", (UINT)m_bAllowSelDragRows);
	AFX_DUMP1(dc, "\nm_wAllowTrackRowHeight  = ", (UINT)m_wAllowTrackRowHeight);
	AFX_DUMP1(dc, "\nm_wAllowTrackColWidth   = ", (UINT)m_wAllowTrackColWidth);

	AFX_DUMP1(dc, "\nm_bOwnProperties        = ", (UINT)m_bOwnProperties);
	AFX_DUMP1(dc, "\nm_bOwnData              = ", (UINT)m_bOwnData);
	AFX_DUMP1(dc, "\nm_bOwnPD                = ", (UINT)m_bOwnPD);

	if (m_bOwnProperties)dc << m_pProperties;
	if (m_bOwnData)dc << m_pData;
	if (m_bOwnPD)dc << m_pPD;
	AFX_DUMP0(dc, "\nm_SelRects: ");
	m_SelRects.Dump(dc);
	AFX_DUMP0(dc, "\nm_UndoList: ");
	m_UndoList.Dump(dc);
	AFX_DUMP0(dc, "\nm_RedoList: ");
	m_RedoList.Dump(dc);

}
*/
#endif

CGXGridCore* CGXGridParam::Grid() const
{
	return m_pAppData->GetFirstGrid((void*) this);
}

