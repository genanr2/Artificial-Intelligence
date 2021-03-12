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

// gxchbmbt.cpp : implementation of CGXBitmapButtonChild child class
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

#ifndef _GXCTRLI_H_
#include "gxctrli.h"
#endif

#ifdef _GXDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CGXBitmapButtonChild child class

// CGXBitmapButtonChild supports device independent bitmaps

CGXBitmapButtonChild::CGXBitmapButtonChild(CGXControl* pComposite)
	: CGXButton(pComposite)
{
	m_hBitmap = NULL;
	m_hBitmapSel = NULL;
	m_hBitmapFocus = NULL;
}

CGXBitmapButtonChild::~CGXBitmapButtonChild()
{
	// delete old bitmaps (if present)
	if (m_hBitmap)
		::GlobalFree(m_hBitmap);
	if (m_hBitmapSel)
		::GlobalFree(m_hBitmapSel);
	if (m_hBitmapFocus)
		::GlobalFree(m_hBitmapFocus);
}

// LoadBitmaps will load in one, two, three or all four bitmaps
// returns TRUE if all specified images are loaded
BOOL CGXBitmapButtonChild::LoadBitmaps(LPCTSTR lpszBitmapResource,
	LPCTSTR lpszBitmapResourceSel, LPCTSTR lpszBitmapResourceFocus)
{
	// delete old bitmaps (if present)
	if (m_hBitmap)
		::GlobalFree(m_hBitmap);
	if (m_hBitmapSel)
		::GlobalFree(m_hBitmapSel);
	if (m_hBitmapFocus)
		::GlobalFree(m_hBitmapFocus);

	m_hBitmap = NULL;
	m_hBitmapSel = NULL;
	m_hBitmapFocus = NULL;

	m_hBitmap = GXLoadResourceBitmap(lpszBitmapResource);
	if (!m_hBitmap)
	{
		TRACE0("Failed to load bitmap for normal image.\n");
		return FALSE;   // need this one image
	}
	BOOL bAllLoaded = TRUE;
	if (lpszBitmapResourceSel != NULL)
	{
		m_hBitmapSel = GXLoadResourceBitmap(lpszBitmapResourceSel);
		if (!m_hBitmapSel)
		{
			TRACE0("Failed to load bitmap for selected image.\n");
			bAllLoaded = FALSE;
		}
	}
	if (lpszBitmapResourceFocus != NULL)
	{
		m_hBitmapFocus = GXLoadResourceBitmap(lpszBitmapResourceFocus);
		if (!m_hBitmapFocus)
			bAllLoaded = FALSE;
	}

	m_bCanPress = (lpszBitmapResourceSel != NULL)
		|| (lpszBitmapResourceFocus != NULL);

	return bAllLoaded;
}

BOOL CGXBitmapButtonChild::LoadBitmaps(UINT nIDBitmapResource,
	UINT nIDBitmapResourceSel, UINT nIDBitmapResourceFocus)
{
	return LoadBitmaps(MAKEINTRESOURCE(nIDBitmapResource),
		MAKEINTRESOURCE(nIDBitmapResourceSel),
		MAKEINTRESOURCE(nIDBitmapResourceFocus));
}

// Draw the appropriate bitmap
void CGXBitmapButtonChild::Draw(CDC* pDC, BOOL bActive)
{
	// must have at least the first bitmap loaded before calling DrawItem
	ASSERT(m_hBitmap != NULL);     // required

	// use the main bitmap for up, the selected bitmap for down
	HANDLE hBitmap = m_hBitmap;

	UINT state = m_bPressed ? ODS_SELECTED : HasFocus() ? ODS_FOCUS : 0;
	if ((state & ODS_SELECTED) && m_hBitmapSel != NULL)
	{
		hBitmap = m_hBitmapSel;
	}
#ifndef _MAC
	else if ((state & ODS_FOCUS) && m_hBitmapFocus != NULL)
#else
  else if ((state & ODS_FOCUS) && m_hBitmapFocus != NULL  &&
		(GridWnd()->GetParent()->GetStyle() & DS_WINDOWSUI))
#endif
	{
		hBitmap = m_hBitmapFocus;   // third image for focused
	}

	CRect rect;
	rect.CopyRect(&m_rect);

	// Bitmap Info, width and height
	LPSTR lpDIB = (LPSTR) GlobalLock(hBitmap);

	BITMAPINFO* pBMI = (BITMAPINFO *) lpDIB;
	int nDibWidth = (int) min(rect.Width(), pBMI->bmiHeader.biWidth);
	int nDibHeight = (int) min(rect.Height(), pBMI->bmiHeader.biHeight);

	int sx = 0, sy = 0;

	if (pBMI->bmiHeader.biHeight > rect.Height())
		sy = (int) pBMI->bmiHeader.biHeight - rect.Height();

	// Look at color table size, and work out where image bits start
	DWORD dwColors;
	DWORD dwColorTableSize;
	dwColors = GXGetNumDIBColorEntries((LPBITMAPINFO)lpDIB);
	dwColorTableSize = dwColors * sizeof(RGBQUAD);
	LPSTR pBits = lpDIB + sizeof(BITMAPINFOHEADER) + dwColorTableSize;

	::StretchDIBits(pDC->GetSafeHdc(),
					rect.left,                // Destination x
					rect.top,                 // Destination y
					nDibWidth,                // Destination width
					nDibHeight,               // Destination height
					sx,                       // Source x
					sy,                       // Source y
					nDibWidth,                // Source width
					nDibHeight,               // Source height
					pBits,                    // Pointer to bits
					pBMI,                     // BITMAPINFO
					DIB_RGB_COLORS,           // Options
					SRCCOPY);                 // Raster operator code (ROP)

	GlobalUnlock(hBitmap);

	// unreferenced:
	bActive;
}

CSize CGXBitmapButtonChild::GetSize()
{
	CSize size = GetSize(m_hBitmap);
	CSize sizeSel = GetSize(m_hBitmapSel);
	CSize sizeFocus = GetSize(m_hBitmapFocus);

	return CSize(
		max(size.cx, max(sizeSel.cx, sizeFocus.cx)),
		max(size.cy, max(sizeSel.cy, sizeFocus.cy))
		);
}

CSize CGXBitmapButtonChild::GetSize(HANDLE hbm)
{
	CSize size(0,0);

	if (hbm)
	{
		CGXDIB dib;
		dib.Attach(hbm);
		size = dib.GetSize();
		dib.Detach();
	}

	return size;
}
