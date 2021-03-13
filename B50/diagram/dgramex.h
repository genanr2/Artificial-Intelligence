//
//  Copyright (C) 1995-1996 by NEURON DATA Inc.
//  All Rights Reserved.
//
//  N O T I C E
//
//  THIS MATERIAL IS CONSIDERED A TRADE SECRET BY NEURON DATA.
//  UNAUTHORIZED ACCESS IS PROHIBITED.
//
//  This file is part of the published Open Interface examples.
//
//  @(#)dgramex.h	6.3 96/06/10
//

//
// This example demonstrates how to use the diagrammer widget of the Open 
// Interface extension library.
// When brought up, it lets the user pick one of three possible sub-examples:
//
//	1. Simple diagrammer
//		This subexample creates a diagrammer with its contents
//		defined in the file "gsimple.ddt". The user can use any of 
//		the predefined popups to act on the diagrammer (move a node, 
//		delete it, ...)
//		The menu bar actions let the user load a file into the
//		diagrammer, replacing or not the current contents, as well as 
//		save back the contents of the diagrammer into a file.
//
//	2. Icon diagrammer
//		This subexample creates a diagrammer with an icon in the 
//		background, and with custom drawing for the nodes.
//
//	3. Complex diagrammer
//		This subexample creates a diagrammer with a large number of 
//		random nodes and links. The node, link and diagrammer popups
//		are replaced by popups that let the end user trigger complex
//		actions on the dgram, like getting all the nodes that are
//		children of the current node.
//

// (( CodeGen: CodeHistory ))
// (( CodeGen: GeneratorVersion 4 ))

//--------------------------------------------------------------------------
//	Include files
//--------------------------------------------------------------------------

#ifndef _DGRAMEX_H_
#define _DGRAMEX_H_

// (( CodeGen: Includes
#include <dgrampub.h>
#include <iconpub.h>
#include <mbarpub.h>
#include <menupub.h>
#include <rclaspub.h>
#include <respub.h>
#include <rlibpub.h>
#include <tbutpub.h>
#include <winpub.h>
// )) CodeGen: Includes 16238

// (( CodeGen: CustomIncludes
// )) CodeGen: CustomIncludes 0

#include <pointpub.h>
#include <rectpub.h>
#include <rgnpub.h>
#include <filewpub.h>

// (( CodeGen: ClassInterfacePlaceHolder ))
// (( CodeGen: WinClassInterfacePlaceHolder ))

//--------------------------------------------------------------------------
//	"Simple Diagrammer" Window Interface
//--------------------------------------------------------------------------

// (( CodeGen: WindowSection WinSimple

class DGramExWinSimple : public NDWin {
protected:
	// (( CodeGen: WgtMembers
	NDDGramPtr	mDGram;
	// )) CodeGen: WgtMembers 1542

	NDDGramSAreaPtr mDGSArea;
public:
	DGramExWinSimple(CStr modName, CStr resName);

	// (( CodeGen: WgtNfyDeclarations
	WIN_NFYVOIDHANDLER(DGramExWinSimple, SelMenuBar)
	// )) CodeGen: WgtNfyDeclarations 4053

	WIN_NFYVOIDHANDLER(DGramExWinSimple, NodeClickSArea)
	WIN_NFYVOIDHANDLER(DGramExWinSimple, Init)

	static CStr GetDGramFileName(CStr, BoolEnum);
	static void LoadShow(void);
};
typedef class DGramExWinSimple C_FAR * DGramExWinSimplePtr;

// (( CodeGen: MenuSection
// (( CodeGen: MenuItemDefines
#define DGRAMEXWINSIMPLE_MITEMFILE	0x0001
#define   DGRAMEXWINSIMPLE_MITEMLOAD	0x0011
#define   DGRAMEXWINSIMPLE_MITEMINSERT	0x0012
#define   DGRAMEXWINSIMPLE_MITEMSAVE	0x0013
// )) CodeGen: MenuItemDefines 12178
// )) CodeGen: MenuSection

// (( CodeGen: MenuInterfacePlaceHolder ))
// )) CodeGen: WindowSection WinSimple

//--------------------------------------------------------------------------
//	"Diagrammer on Icon" Window Interface
//--------------------------------------------------------------------------

// (( CodeGen: WindowSection WinImage

class DGramExWinImage : public NDWin {
protected:
	// (( CodeGen: WgtMembers
	NDDGramPtr	mDGram;
	// )) CodeGen: WgtMembers 1542

	NDDGramSAreaPtr	mDGSArea;
public:
	DGramExWinImage(CStr modName, CStr resName);

	// (( CodeGen: WgtNfyDeclarations
	// )) CodeGen: WgtNfyDeclarations 0

	WIN_NFYVOIDHANDLER(DGramExWinImage, NodeDrawBoxSArea)
	WIN_NFYVOIDHANDLER(DGramExWinImage, NodeDrawLabelSArea)
	WIN_NFYVOIDHANDLER(DGramExWinImage, Init)

	void DrawNodeIcon(DGramNodePtr);
	static void LoadShow(void);
};
typedef class DGramExWinImage C_FAR * DGramExWinImagePtr;

// (( CodeGen: MenuInterfacePlaceHolder ))
// )) CodeGen: WindowSection WinImage

//--------------------------------------------------------------------------
//	"Complex Diagrammer" Window Interface
//--------------------------------------------------------------------------

// (( CodeGen: WindowSection WinComplex

class DGramExWinComplex : public NDWin {
private:
	Int		mNumNodes;
	Int		mNumLinks;
protected:
	// (( CodeGen: WgtMembers
	NDDGramPtr	mDGram;
	// )) CodeGen: WgtMembers 1542

	NDDGramSAreaPtr	mDGSArea;
public:
	DGramExWinComplex(CStr modName, CStr resName);

	// (( CodeGen: WgtNfyDeclarations
	// )) CodeGen: WgtNfyDeclarations 0

	WIN_NFYVOIDHANDLER(DGramExWinComplex, NodeMenuSelectSArea)
	WIN_NFYVOIDHANDLER(DGramExWinComplex, LinkMenuSelectSArea)
	WIN_NFYVOIDHANDLER(DGramExWinComplex, DiagrammerMenuSelectSArea)
	WIN_NFYVOIDHANDLER(DGramExWinComplex, Init)

	void HighlightNode(DGramNodePtr);
	void UnhighlightNode(DGramNodePtr);
	BoolEnum IsHighlightNode(DGramNodePtr);

	void ResetNodes(void);
	void ShowChildren(DGramNodePtr);
	void ShowParents(DGramNodePtr);
	void ShowNeighbors(DGramNodePtr);

	void AddNodesDGram(void);
	void AddLinksDGram(void);
	static void LoadShow(void);
};
typedef class DGramExWinComplex C_FAR * DGramExWinComplexPtr;

//
// Diagrammer SArea notification procedure. We customize this notification 
// procedure in order to be able to get the selections made by the end-
// user in the customized popups.
//
#define DGRAMEXMENUNODE_SHOWCHILDREN	0x0001
#define DGRAMEXMENUNODE_SHOWPARENTS	0x0002
#define DGRAMEXMENUNODE_SHOWNEIGHBORS	0x0003
#define DGRAMEXMENUNODE_DELETE		0x0004

#define DGRAMEXMENULINK_SHOWEXTREMES	0x0001
#define DGRAMEXMENULINK_DELETE		0x0002

#define DGRAMEXMENUDGRAM_SELECTNODE	0x0001
#define DGRAMEXMENUDGRAM_SELECTLINK	0x0002

// (( CodeGen: MenuInterfacePlaceHolder ))
// )) CodeGen: WindowSection WinComplex

//--------------------------------------------------------------------------
//	"Diagrammers" Window Interface
//--------------------------------------------------------------------------

// (( CodeGen: WindowSection WinSelect

class DGramExWinSelect : public NDWin {
protected:
	// (( CodeGen: WgtMembers
	NDPButPtr	mPButSimple;
	NDPButPtr	mPButImage;
	NDPButPtr	mPButComplex;
	NDPButPtr	mPButQuit;
	// )) CodeGen: WgtMembers 7776
public:
	DGramExWinSelect(CStr modName, CStr resName);

	// (( CodeGen: WgtNfyDeclarations
	WIN_NFYVOIDHANDLER(DGramExWinSelect, HitPButSimple)
	WIN_NFYVOIDHANDLER(DGramExWinSelect, HitPButImage)
	WIN_NFYVOIDHANDLER(DGramExWinSelect, HitPButComplex)
	WIN_NFYVOIDHANDLER(DGramExWinSelect, HitPButQuit)
	// )) CodeGen: WgtNfyDeclarations 17212

	static void LoadShow(void);
};
typedef class DGramExWinSelect C_FAR * DGramExWinSelectPtr;

// (( CodeGen: MenuInterfacePlaceHolder ))
// )) CodeGen: WindowSection WinSelect

// (( CodeGen: WindowInterfacePlaceHolder ))

#endif // _DGRAMEX_H_
