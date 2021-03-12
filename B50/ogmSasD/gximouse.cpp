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

// gximouse.cpp : implementation of the CGXIntelliMousePlugin class
//                (adds support for Mousewheel panning/dragging)

#include "stdafx.h"
#include "gxresrc.h"

#ifdef _GXDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

#if defined(_GX) && _MFC_VER < 0x0300

#include /**/ "gxdll.h"
#include /**/ "gxstate.h"

#endif

#include "gximouse.h"

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// Timer id for scrolling 

#ifndef GX_IMOUSETIMER 
#define GX_IMOUSETIMER 991
#endif

// determine number of elements in an array (not bytes)
#define _gxcountof(array) (sizeof(array)/sizeof(array[0]))

const int nOffs = 15;

#define DD_DEFLOCKMOUSETICKS 400  // ClickLock
#define DD_DEFSCROLLSPEED 600   // slowest scroll speed
#define DD_DEFSCROLLACCEL 2   // Interval will be (600/(nPixels/2))
#define DD_DEFSCROLLBYPIX 40	// if more than 40 pixels start scrolling more than one line at a time

// ClickLock locks down the mouse button after a single click. 
// Just click and hold down mouse button for a momemnt
// and your click is locked. With ClickLock you can scroll
// the grid very easy. Click again to release ClickLock. To adjust
// the amount of time you need to hold down the mouse wheel
// button before it locks change the above DD_DEFLOCKMOUSETICKS value.
 

GX_PROCESS_LOCAL(GX_INTELLI_STATE, _gxIntelliState)

GX_INTELLI_STATE::GX_INTELLI_STATE()
{
	nLockMouseTicks = DD_DEFLOCKMOUSETICKS;
	nScrollSpeed = DD_DEFSCROLLSPEED;
	nScrollAccel = DD_DEFSCROLLACCEL;
	nScrollByPix = DD_DEFSCROLLBYPIX;
	m_hcurLast = 0;
	m_hcurDestroy = 0; 
	m_idcPrimaryLast = 0; 
	m_bNotFound = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CGXMouseWheelOriginWnd

IMPLEMENT_DYNAMIC(CGXMouseWheelOriginWnd, CWnd)

CGXMouseWheelOriginWnd::CGXMouseWheelOriginWnd()
{
}

BOOL CGXMouseWheelOriginWnd::CreateWnd(CWnd* pParent, const CRect& rect, UINT nID)
{
	if (!CreateEx(
			0, NULL, NULL, WS_CHILD|WS_VISIBLE,
			rect.left, rect.top, rect.Width(), rect.Height(),
			pParent->GetSafeHwnd(), NULL))
	{
		TRACE0("Failed to create window in CGXMouseWheelOriginWnd::CreateWnd\n");
		ASSERT(0);

		return FALSE;
	}

	if (m_bmOrigin.GetSafeHandle() == 0 && !m_bmOrigin.LoadBitmap(nID))
	{
		TRACE1("Failed to load bitmap (id = %d) in CGXMouseWheelOriginWnd::CreateWnd\n", nID);
		TRACE0("Please check if the specified resource is available!\n");
		TRACE0("If you linked static to Objective Grid - Is gxres.rc included?\n");

		return FALSE;
	}

	return TRUE;
};

BEGIN_MESSAGE_MAP(CGXMouseWheelOriginWnd, CWnd)
	//{{AFX_MSG_MAP(CGXMouseWheelOriginWnd)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// DrawTransparentBitmap
//
// This function lets you draw transparent bitmaps. The parameter
// cTransparentColor specifies the color for transparent pixels.
// All pixels in the bitmap which should be transparent should be
// marked with this color.
//
// The function was copied and adapated from knowledge base article Q79212
// Title: Drawing Transparent Bitmaps

void AFXAPI CGXMouseWheelOriginWnd::DrawTransparentBitmap(CDC* pDC, CBitmap* pBitmap, int xStart,
	int yStart, COLORREF cTransparentColor)
{
#ifndef _GX
   CBitmap    bmAndBack, bmAndObject, bmAndMem, bmSave;
   CDC        dcMem, dcBack, dcObject, dcTemp, dcSave;

   dcTemp.CreateCompatibleDC(pDC);
   dcTemp.SelectObject(pBitmap);   // Select the bitmap

   BITMAP     bm;
   pBitmap->GetObject(sizeof(BITMAP), (LPSTR)&bm);

   CPoint     ptSize;
   ptSize.x = bm.bmWidth;            // Get width of bitmap
   ptSize.y = bm.bmHeight;           // Get height of bitmap
   dcTemp.DPtoLP(&ptSize, 1);      // Convert from device
									 // to logical points

   // Create some DCs to hold temporary data.
   dcBack.CreateCompatibleDC(pDC);
   dcObject.CreateCompatibleDC(pDC);
   dcMem.CreateCompatibleDC(pDC);
   dcSave.CreateCompatibleDC(pDC);

   // Create a bitmap for each DC. DCs are required for a number of
   // GDI functions.

   // Monochrome DC
   bmAndBack.CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

   // Monochrome DC
   bmAndObject.CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

   bmAndMem.CreateCompatibleBitmap(pDC, ptSize.x, ptSize.y);
   bmSave.CreateCompatibleBitmap(pDC, ptSize.x, ptSize.y);

   // Each DC must select a bitmap object to store pixel data.
   CBitmap* pbmBackOld   = dcBack.SelectObject(&bmAndBack);
   CBitmap* pbmObjectOld = dcObject.SelectObject(&bmAndObject);
   CBitmap* pbmMemOld    = dcMem.SelectObject(&bmAndMem);
   CBitmap* pbmSaveOld   = dcSave.SelectObject(&bmSave);

   // Set proper mapping mode.
   dcTemp.SetMapMode(pDC->GetMapMode());

   // Save the bitmap sent here, because it will be overwritten.
   dcSave.BitBlt(0, 0, ptSize.x, ptSize.y, &dcTemp, 0, 0, SRCCOPY);

   // Set the background color of the source DC to the color.
   // contained in the parts of the bitmap that should be transparent
   COLORREF cColor = dcTemp.SetBkColor(cTransparentColor);

   // Create the object mask for the bitmap by performing a BitBlt
   // from the source bitmap to a monochrome bitmap.
   dcObject.BitBlt(0, 0, ptSize.x, ptSize.y, &dcTemp, 0, 0,
		  SRCCOPY);

   // Set the background color of the source DC back to the original
   // color.
   dcTemp.SetBkColor(cColor);

   // Create the inverse of the object mask.
   dcBack.BitBlt(0, 0, ptSize.x, ptSize.y, &dcObject, 0, 0,
		  NOTSRCCOPY);

   // Copy the background of the main DC to the destination.
   dcMem.BitBlt(0, 0, ptSize.x, ptSize.y, pDC, xStart, yStart,
		  SRCCOPY);

   // Mask out the places where the bitmap will be placed.
   dcMem.BitBlt(0, 0, ptSize.x, ptSize.y, &dcObject, 0, 0, SRCAND);

   // Mask out the transparent colored pixels on the bitmap.
   dcTemp.BitBlt(0, 0, ptSize.x, ptSize.y, &dcBack, 0, 0, SRCAND);

   // XOR the bitmap with the background on the destination DC.
   dcMem.BitBlt(0, 0, ptSize.x, ptSize.y, &dcTemp, 0, 0, SRCPAINT);

   // Copy the destination to the screen.
   pDC->BitBlt(xStart, yStart, ptSize.x, ptSize.y, &dcMem, 0, 0,
		  SRCCOPY);

   // Place the original bitmap back into the bitmap sent here.
   dcTemp.BitBlt(0, 0, ptSize.x, ptSize.y, &dcSave, 0, 0, SRCCOPY);

   // Reset the memory bitmaps.
   dcBack.SelectObject(pbmBackOld);
   dcObject.SelectObject(pbmObjectOld);
   dcMem.SelectObject(pbmMemOld);
   dcSave.SelectObject(pbmSaveOld);

   // Memory DCs and Bitmap objects will be deleted automatically

#else
   // Use OG built-in method

   GXDrawTransparentBitmap(pDC, pBitmap, xStart,
		yStart, cTransparentColor);

#endif
}

void CGXMouseWheelOriginWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rect;
	GetClientRect(rect);

	// choose bitmap for the header
	CBitmap* pBitmap = &m_bmOrigin;
	CDC* pDC = &dc;

	if (pBitmap && pBitmap->GetSafeHandle())
	{
		// Bitmap size
		BITMAP     bm;
		pBitmap->GetObject(sizeof(BITMAP), (LPSTR)&bm);

		CPoint     ptSize;
		ptSize.x = bm.bmWidth;            // Get width of bitmap
		ptSize.y = bm.bmHeight;           // Get height of bitmap
		pDC->DPtoLP(&ptSize, 1);      // Convert from device to logical points

		// Draw bitmap
		if (rect.Width() >= ptSize.x && rect.Height() >= ptSize.x)
		{
			// must have at least the first bitmap loaded before calling DrawItem
			ASSERT(pBitmap->m_hObject != NULL);     // required

			int x = rect.left + max(1, (rect.Width()-ptSize.x)/2);
			int y = rect.top + max(1, (rect.Height()-ptSize.y)/2);

			DrawTransparentBitmap(pDC, // The destination DC.
				pBitmap, // The bitmap to be drawn.
				x,        // X coordinate.
				y,        // Y coordinate.
				RGB(255,0,0)); // The color for transparent
								   // pixels (white grey).

		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGXIntelliMousePlugin

IMPLEMENT_DYNAMIC(CGXIntelliMousePlugin, CGXMouseWheelPlugin)

BEGIN_MESSAGE_MAP(CGXIntelliMousePlugin, CGXMouseWheelPlugin)
	//{{AFX_MSG_MAP(CGXIntelliMousePlugin)
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
	// Standard printing commands
#if _MFC_VER >= 0x0421
	ON_WM_MOUSEWHEEL()
#endif
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGXIntelliMousePlugin construction/destruction

CGXIntelliMousePlugin::CGXIntelliMousePlugin()
{
	m_bMouseWheelDrag = FALSE;
	m_idcCursor = 0;
}

CGXIntelliMousePlugin::~CGXIntelliMousePlugin()
{
}

/////////////////////////////////////////////////////////////////////////////
// CGXIntelliMousePlugin diagnostics

#ifdef _DEBUG
void CGXIntelliMousePlugin::AssertValid() const
{
	CGXMouseWheelPlugin::AssertValid();
}
/*
void CGXIntelliMousePlugin::Dump(CDumpContext& dc) const
{
//	CGXMouseWheelPlugin::Dump(dc);
}
*/
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGXIntelliMousePlugin message handlers

#if _MFC_VER >= 0x0421

BOOL CGXIntelliMousePlugin::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	// cancel any existing autoscroll behavior
	if (m_bMouseWheelDrag)
		KillMouseWheel(point);	

	return CGXMouseWheelPlugin::OnMouseWheel(nFlags, zDelta, point);
}

#endif

void CGXIntelliMousePlugin::OnMButtonDown(UINT nFlags, CPoint point) 
{
	BOOL bCtl = GetKeyState(VK_CONTROL) & 0x8000;

	if (m_bMouseWheelDrag)
		KillMouseWheel(point);	
	
	else if (!bCtl && nFlags == MK_MBUTTON)
	{
		SetCapture();

		GetCursorPos(&point);
		point.x -= nOffs;
		point.y -= nOffs;
		SetCursorPos(point.x, point.y);
		ScreenToClient(&point);
		
		m_bMouseWheelDrag = TRUE;
		m_ptMouseWheelOrg = point;
		m_nMouseWheelTimer = SetTimer(GX_IMOUSETIMER, 10, NULL);
		m_dwMouseWheelTick = GetTickCount();
		m_dwMouseWheelLastTick = 0;
		m_nMouseWheelNextTicks = 0;
		m_nMouseWheelLastDirection = -1;

		m_bMouseWheelCanVertScroll = TRUE;
		m_bMouseWheelCanHorzScroll = TRUE;

		int nIdBmp;
		if (m_bMouseWheelCanVertScroll && m_bMouseWheelCanHorzScroll)
			nIdBmp = GX_IDB_ORIGIN_ALL;
		else if (m_bMouseWheelCanVertScroll)
			nIdBmp = GX_IDB_ORIGIN_VERT;
		else 
			nIdBmp = GX_IDB_ORIGIN_HORZ;

		// Save background
		CRect rect(m_ptMouseWheelOrg.x, m_ptMouseWheelOrg.y, m_ptMouseWheelOrg.x+30, m_ptMouseWheelOrg.y+30);
		SaveBackground(rect);

		m_pWndOrigin = new CGXMouseWheelOriginWnd();
		m_pWndOrigin->CreateWnd(this, rect, nIdBmp);

		m_bExitMesssage = TRUE;
	}
	else
		CGXMouseWheelPlugin::OnMButtonDown(nFlags, point);
}

void CGXIntelliMousePlugin::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bMouseWheelDrag)
	{
		m_bExitMesssage = TRUE;
		ShowImouseCursor(m_idcCursor);
	}

	CGXMouseWheelPlugin::OnMouseMove(nFlags, point);
}


BOOL CGXIntelliMousePlugin::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_bMouseWheelDrag)
	{
		m_bExitMesssage = TRUE;
		return TRUE;    // we will handle it in the mouse move
	}

	return CGXMouseWheelPlugin::OnSetCursor(pWnd, nHitTest, message);
}

void CGXIntelliMousePlugin::OnMButtonUp(UINT nFlags, CPoint point) 
{
	if (m_dwMouseWheelTick == 0 || GetTickCount()-m_dwMouseWheelTick < (DWORD) _gxIntelliState->nLockMouseTicks)
		KillMouseWheel(point);

	CGXMouseWheelPlugin::OnMButtonUp(nFlags, point);
}

void CGXIntelliMousePlugin::OnLButtonDown(UINT nFlags, CPoint point) 
{
	KillMouseWheel(point);	
	
	CGXMouseWheelPlugin::OnLButtonDown(nFlags, point);
}

void CGXIntelliMousePlugin::OnRButtonDown(UINT nFlags, CPoint point) 
{
	KillMouseWheel(point);	

	CGXMouseWheelPlugin::OnRButtonDown(nFlags, point);
}

void CGXIntelliMousePlugin::OnCancelMode() 
{
	KillMouseWheel(CPoint(0,0));	

	CGXMouseWheelPlugin::OnCancelMode();
}

void CGXIntelliMousePlugin::KillMouseWheel(CPoint point) 
{
	if (m_bMouseWheelDrag)
	{
		// Hide origin window - old window contents will be restored 
		if (m_pWndOrigin)
		{
			m_pWndOrigin->SetWindowPos(NULL, 0,0,0,0, SWP_NOREDRAW);
			RestoreBackground();
			m_pWndOrigin->DestroyWindow();
			delete m_pWndOrigin;
		}

		m_bMouseWheelDrag = FALSE;
		ReleaseCapture();
		KillTimer(m_nMouseWheelTimer);
		SetImouseCursor(NULL);

		GetCursorPos(&point);
		point.x += nOffs;
		point.y += nOffs;
		SetCursorPos(point.x, point.y);
		ScreenToClient(&point);
	}
}

enum Direction
{
	SCROLL_UP               = 1,
	SCROLL_LEFT             = 2,
	SCROLL_DOWN             = 3,
	SCROLL_RIGHT            = 4,
};

void CGXIntelliMousePlugin::SaveBackground(const CRect& rect)
{
	CClientDC dc(this);

	m_rcBackground = rect;

	m_dcBackground.CreateCompatibleDC(&dc);
	m_bmBackground.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	m_hbmBackgroundOld   = (HBITMAP) m_dcBackground.SelectObject(&m_bmBackground)->GetSafeHandle();
	
	// Save the existing background so that we can later hide the window again
	m_dcBackground.BitBlt(0, 0, rect.Width(), rect.Height(), &dc, rect.left, rect.top, SRCCOPY);
}

void CGXIntelliMousePlugin::RestoreBackground()
{
	if (m_dcBackground.m_hDC)
	{
		CClientDC dc(this);

		CRect& rect = m_rcBackground;

		dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &m_dcBackground, 0, 0,
		  SRCCOPY);

		m_dcBackground.SelectObject(CBitmap::FromHandle(m_hbmBackgroundOld));
		m_bmBackground.DeleteObject();

		m_dcBackground.DeleteDC();
	}
}

void CGXIntelliMousePlugin::HideOriginWnd()
{
	// Hide origin window - old window contents will be restored 
	m_pWndOrigin->SetWindowPos(NULL, 0,0,0,0, SWP_NOREDRAW);
	RestoreBackground();
}

void CGXIntelliMousePlugin::ShowOriginWnd()
{
	CRect rect(m_ptMouseWheelOrg.x, m_ptMouseWheelOrg.y, m_ptMouseWheelOrg.x+30, m_ptMouseWheelOrg.y+30);

	// In case the origin window is drawn over an invalid area
	// we have to make sure this area is updated first. Otherwise
	// the saved background for double buffering is wrong.
	CRect rcUpdate;
	if (GetUpdateRect(&rcUpdate)
		&& rcUpdate.IntersectRect(rcUpdate, rect))
		UpdateWindow();
	
	SaveBackground(rect);
	m_pWndOrigin->SetWindowPos(NULL, rect.left,rect.top,rect.Width(),rect.Height(), SWP_NOREDRAW);
	m_pWndOrigin->Invalidate(FALSE);
	m_pWndOrigin->ShowWindow(SW_SHOW);
	m_pWndOrigin->UpdateWindow();
}

void CGXIntelliMousePlugin::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == GX_IMOUSETIMER && m_bMouseWheelDrag)
	{
		CPoint ptCur;
		GetCursorPos(&ptCur);
		ScreenToClient(&ptCur);
		
		CPoint pt = ptCur - m_ptMouseWheelOrg;
		
		int direction;
		int nScroll;
		int nAccel;
		
		if (abs(pt.x) > abs(pt.y) && m_bMouseWheelCanHorzScroll || !m_bMouseWheelCanVertScroll)
		{
			direction = pt.x > 0 ? SCROLL_RIGHT : SCROLL_LEFT;
			nScroll = abs(pt.x) < 8 ? 0 : abs(pt.x) < _gxIntelliState->nScrollByPix ? 1 : (abs(pt.x)-_gxIntelliState->nScrollByPix)/_gxIntelliState->nScrollAccel+1;
			nAccel = max(1, (abs(pt.x)-10)/_gxIntelliState->nScrollAccel);
		}
		else
		{
			direction = pt.y > 0 ? SCROLL_DOWN : SCROLL_UP;
			nScroll = abs(pt.y) < 8 ? 0 : abs(pt.y) < _gxIntelliState->nScrollByPix ? 1 : (abs(pt.y)-_gxIntelliState->nScrollByPix)/_gxIntelliState->nScrollAccel+1;
			nAccel = max(1, (abs(pt.y)-10)/_gxIntelliState->nScrollAccel);
		}
		
		DWORD dwTick = GetTickCount();
		
		if (nScroll == 0 && abs(pt.x) < 8 && abs(pt.y) < 8)
			direction = -1;
		else
			m_dwMouseWheelTick = 0;
		
		int nCursor;
		UINT nSBCode = 0;
		BOOL bHorz = 0;
		switch (direction)
		{
		case SCROLL_LEFT: nCursor = GX_IDC_IML; bHorz = TRUE; nSBCode = SB_LINEUP; break;
		case SCROLL_RIGHT: nCursor = GX_IDC_IMR; bHorz = TRUE; nSBCode = SB_LINEDOWN; break;
		case SCROLL_UP: nCursor = GX_IDC_IMU; bHorz = FALSE; nSBCode = SB_LINEUP; break;
		case SCROLL_DOWN: nCursor = GX_IDC_IMD; bHorz = FALSE; nSBCode = SB_LINEDOWN; break;
		default: nCursor = GX_IDC_IMA; break;
		}
		
		SetImouseCursor(nCursor);
		m_nMouseWheelLastDirection = direction;
		
		// scroll if necessary
		if (nScroll > 0 && (direction != m_nMouseWheelLastDirection
			|| dwTick - m_dwMouseWheelLastTick > (DWORD) m_nMouseWheelNextTicks))
		{
			m_dwMouseWheelLastTick = dwTick;
			m_nMouseWheelNextTicks = _gxIntelliState->nScrollSpeed/nAccel;
			
			HideOriginWnd();
			
			// Scroll
			if (bHorz)
				OnIMouseHScroll(nSBCode, nScroll);
			else
				OnIMouseVScroll(nSBCode, nScroll);

			ShowOriginWnd();
		}
	}
	else
		CGXMouseWheelPlugin::OnTimer(nIDEvent);
}

void CGXIntelliMousePlugin::SetImouseCursor(UINT idcCursor)
{
	m_idcCursor = idcCursor;
	ShowImouseCursor(m_idcCursor);
}

BOOL CGXIntelliMousePlugin::ShowImouseCursor(UINT idcPrimary)
{                                     
#if _MFC_VER >= 0x0400
	LPCTSTR idcSecondary = IDC_SIZEALL;
#else
	LPCTSTR idcSecondary = IDC_SIZE;
#endif

	if (idcPrimary != 0)
	{
		HCURSOR hcurToDestroy = NULL;
		if (idcPrimary != _gxIntelliState->m_idcPrimaryLast)
		{
			HINSTANCE hInst = AfxFindResourceHandle(
				MAKEINTRESOURCE(idcPrimary), RT_GROUP_CURSOR);

			// load in another cursor
			hcurToDestroy = _gxIntelliState->m_hcurDestroy;

			// Note: If this LoadCursor call fails, it is likely that
			// gxres.rc file is not included in your resource file.
			// To correct the situation, include gxres.rc into your
			// resource script.
			//
			// This should be done using the Resource.Set Includes... command.
			// and including gxresrc.h and gxres.rc

			if ((_gxIntelliState->m_hcurDestroy = _gxIntelliState->m_hcurLast =
			   ::LoadCursor(hInst, MAKEINTRESOURCE(idcPrimary))) == NULL)
			{
				// will not look as good
				if (!_gxIntelliState->m_bNotFound)
				{
					// Display warning message only first time
					TRACE1("Warning: Could not find default cursor (id = %u) - using system provided alternative.\n", idcPrimary);
					_gxIntelliState->m_bNotFound = TRUE;
				}

				ASSERT(_gxIntelliState->m_hcurDestroy == NULL);    // will not get destroyed
				_gxIntelliState->m_hcurLast = ::LoadCursor(NULL, idcSecondary);
				ASSERT(_gxIntelliState->m_hcurLast != NULL);
			}
			_gxIntelliState->m_idcPrimaryLast = idcPrimary;
		}
		ASSERT(_gxIntelliState->m_hcurLast != NULL);
		if (_gxIntelliState->m_hcurLast)
		{
			SetCursor(_gxIntelliState->m_hcurLast);
			ASSERT(_gxIntelliState->m_hcurLast != hcurToDestroy);
			if (hcurToDestroy != NULL)
				DestroyCursor(hcurToDestroy); // destroy after being set
		}

		return TRUE;
	}

	return FALSE;
}

