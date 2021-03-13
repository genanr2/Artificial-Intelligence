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
//  @(#)dgramex.cxx	6.10 96/12/11
//

//
//  This example demonstrates how to use the diagrammer widget of the Open 
//  Interface extension library.
//  See header file <dgramex.h> for a complete description.
//

// (( CodeGen: CodeHistory ))
// (( CodeGen: GeneratorVersion 4 ))

#define ERR_LIB DGRAMEX

#include "dgramex.h"
#include <stdlib.h>

ERR_EXTERN
ERR_INMODULE("DGramEx")

//--------------------------------------------------------------------------
//	Utility
//--------------------------------------------------------------------------

#define S_STRDEFSIZE	256

static void C_NEAR S_Sprintf(Str buf, CStr fmt, ...)
{
	va_list args;
	
	va_start(args, fmt);
	NDStr::Vsprintf(buf, fmt, args);
	va_end(args);
}

// (( CodeGen: ClassImplementationPlaceHolder ))
// (( CodeGen: WinClassImplementationPlaceHolder ))

//--------------------------------------------------------------------------
//	"Simple Diagrammer" Window Implementation
//--------------------------------------------------------------------------

// (( CodeGen: WindowSection WinSimple

//
// Picking a file name for the diagrammer.
//
CStr	DGramExWinSimple::GetDGramFileName L2(CStr, caption, BoolEnum, newFile)
{
	NDFileWPtr filew = new NDFileW;
	filew->Init();
	filew->ResetFilters();
	filew->SetNthFilter(0, "*.ddt", "Diagrammer files (*.ddt)", 
			    FMGR_MACTYPENONE);
	if (newFile) {
		filew->SetFileExistence(FILEW_FILEEXISTFALSE);
	} else {
		filew->SetFileExistence(FILEW_FILEEXISTTRUE);
	}
	filew->SetPrompt(caption);
	CStr fileName = filew->ProcessGetResult();

	filew->End();
	delete filew;

	return fileName;
}

// (( CodeGen: MenuSection
void 	DGramExWinSimple::SelMenuBar(void)
{
	NDMBarPtr mbar = GetMenuBar();
	UInt16 id;
	CStr fileName;

	switch (id = mbar->ChosenGetId()) {
	// (( CodeGen: MenuSwitch
	case DGRAMEXWINSIMPLE_MITEMLOAD:
		// pick a file
		fileName = GetDGramFileName("Load file:", BOOL_FALSE);
		if (fileName) {
			// clear the dgram
			mDGram->ClearAll();
			// insert the file
			mDGram->ReadDGram(fileName);
		}
		break;
	case DGRAMEXWINSIMPLE_MITEMINSERT:
		// "Insert from file ..." Item
		fileName = GetDGramFileName("Insert file:", BOOL_FALSE);
		if (fileName) {
			static NDPoint32 loc = NDPoint32(300,300) ;
			// insert the file
			mDGram->InsertDGram(&loc, fileName);
		}
		break;
	case DGRAMEXWINSIMPLE_MITEMSAVE:
		// pick a file name
		fileName = GetDGramFileName("Save into file:", BOOL_TRUE);
		if (fileName) {
			// save into file
			mDGram->SaveAs(fileName);
		}
		break;
	// )) CodeGen: MenuSwitch
	default:
		DBG_ERROR;
		break;
	}
}
// )) CodeGen: MenuSection
// (( CodeGen: MenuImplementationPlaceHolder ))

void	DGramExWinSimple::NodeClickSArea(void)
{
	mDGSArea->NDDGramSArea::NfyNodeClick();
}

void	DGramExWinSimple::Init(void)
{
	// Initialize the window 
	NDWin::NfyInit();

	// Add the call-back for the DGramSArea component
	mDGSArea = mDGram->GetDGramSArea();
	WIN_SETNFYHANDLER(DGramExWinSimple, mDGSArea, DGRAMSAREA_NFYNODECLICK, NodeClickSArea);
}

// (( CodeGen: WgtNfyPlaceHolder ))

DGramExWinSimple::DGramExWinSimple(CStr modName, CStr resName) : NDWin(modName, resName)
{
	// (( CodeGen: WgtInitializations
	mDGram = (NDDGramPtr)GetNamedWgt("DGram");
	WIN_SETNFYHANDLER(DGramExWinSimple, this, WIN_NFYSELMENUBAR, SelMenuBar);
	// )) CodeGen: WgtInitializations 9261

	//
	// The DGramSArea component of the diagrammer is only created after
	// initialization. We need to differ the installation of the
	// corresponding call-back after initialization of the window.
	//
	mDGSArea = NULL;
	WIN_SETNFYHANDLER(DGramExWinSimple, this, WIN_NFYINIT, Init);
}

void	DGramExWinSimple::LoadShow(void)
{
	DGramExWinSimplePtr win;

	win = new DGramExWinSimple("DGramEx", "WinSimple");

	win->Init();
	win->Show();
}

// )) CodeGen: WindowSection WinSimple

//--------------------------------------------------------------------------
//	"Diagrammer on Icon" Window Implementation
//--------------------------------------------------------------------------

// (( CodeGen: WindowSection WinImage
// (( CodeGen: MenuImplementationPlaceHolder ))

//
// Custom drawing in a diagrammer node
//
void	DGramExWinImage::NodeDrawBoxSArea(void)
{
	// Default drawing
	mDGSArea->NDDGramSArea::NfyNodeDrawBox();       

	// Get the diagrammer rectangle
	DGramNodePtr node = mDGram->NodeGetNfy();
	NDRect32 nodeRect;
	mDGram->NodeQueryRect(node, &nodeRect, BOOL_TRUE);
	NDRect16 rect, clip;
	mDGSArea->Rect32ToRects(&nodeRect, &rect, &clip);

	// Get the icon for the node
	Char buf[S_STRDEFSIZE];
	mDGram->NodeQueryLabel(node, buf);
	NDIconPtr icon;
	if (NDStr::CmpSub((Str)buf, 9, "Telescope", 9) == CMP_EQUAL) {
		icon = (NDIconPtr)NDRes::Load("DGramEx", "IconTelescope");
	} else {        
		icon = (NDIconPtr)NDRes::Load("DGramEx", "IconRadioTelescope");
	}
	NDPoint16 iconExts;
	icon->QuerySize(&iconExts);
	NDWgtRect iconRect;
	iconRect.Ext.x = iconExts.x;
	iconRect.Ext.y = iconExts.y;
	iconRect.Ori.x = rect.Ori.x + (rect.Ext.x - iconRect.Ext.x)/2;
	iconRect.Ori.y = rect.Ori.y + (rect.Ext.y - iconRect.Ext.y)/2;

	// Set the dgramic context
	DGramNodeDrawDataRec ddata;
	mDGram->NodeQueryDData(node, &ddata, DGRAM_DRAWMODESTANDARD);
	mDGSArea->DrawSetPen(NDPen::Solid());
	mDGSArea->DrawSetFont(NDFont::Normal());
	mDGSArea->DrawSetPattern(NDPatt::Empty());
	mDGSArea->DrawSetColors(NULL, ddata.BgColor);

	// Draw the icon
	mDGSArea->DrawIcon(&iconRect, icon);
}

void	DGramExWinImage::NodeDrawLabelSArea(void)
{
	//
	// Do not draw the label!
	//
}

void	DGramExWinImage::Init(void)
{
	// Initialize the window 
	NDWin::NfyInit();

	// Add the call-back for the DGramSArea component
	mDGSArea = mDGram->GetDGramSArea();
	WIN_SETNFYHANDLER(DGramExWinImage, mDGSArea, DGRAMSAREA_NFYNODEDRAWBOX, NodeDrawBoxSArea);
	WIN_SETNFYHANDLER(DGramExWinImage, mDGSArea, DGRAMSAREA_NFYNODEDRAWLABEL, NodeDrawLabelSArea);
}

// (( CodeGen: WgtNfyPlaceHolder ))

DGramExWinImage::DGramExWinImage(CStr modName, CStr resName) : NDWin(modName, resName)
{
	// (( CodeGen: WgtInitializations
	mDGram = (NDDGramPtr)GetNamedWgt("DGram");
	// )) CodeGen: WgtInitializations 3371

	//
	// The DGramSArea component of the diagrammer is only created after
	// initialization. We need to differ the installation of the
	// corresponding call-back after initialization of the window.
	//
	mDGSArea = NULL;
	WIN_SETNFYHANDLER(DGramExWinImage, this, WIN_NFYINIT, Init);
}

void 	DGramExWinImage::LoadShow(void)
{
	DGramExWinImagePtr win = new DGramExWinImage("DGramEx", "WinImage");

	win->Init();

	// fill diagrammer with icon
	IconPtr icon = (IconPtr)NDRes::LoadInit(S_ModuleName, "Worldmap");
	win->mDGram->SetIcon(icon);

	win->Show();
}

// )) CodeGen: WindowSection WinImage

//--------------------------------------------------------------------------
//	"Complex Diagrammer" Window Implementation
//--------------------------------------------------------------------------

// (( CodeGen: WindowSection WinComplex
// (( CodeGen: MenuImplementationPlaceHolder ))

//
// Highlight nodes. We use the dual standard/focused drawing modes for
// nodes in order to keep track of highlighted nodes.
//
void	DGramExWinComplex::HighlightNode(DGramNodePtr node)
{
	mDGram->NodeSetDrawMode(node, DGRAM_DRAWMODEFOCUSED);
	mDGram->Inval(BOOL_FALSE);
}

void	DGramExWinComplex::UnhighlightNode(DGramNodePtr node)
{
	mDGram->NodeSetDrawMode(node, DGRAM_DRAWMODESTANDARD);
	mDGram->Inval(BOOL_FALSE);
}

BoolEnum DGramExWinComplex::IsHighlightNode(DGramNodePtr node)
{
	return (mDGram->NodeGetDrawMode(node) == DGRAM_DRAWMODEFOCUSED);
}

//
// Reset all nodes
//
void	DGramExWinComplex::ResetNodes(void)
{
	mNumNodes = mDGram->GetNumNodes();	
	for (Int iNode = 0; iNode < mNumNodes; iNode++) {
		DGramNodePtr node = mDGram->GetNthNode(iNode);
		UnhighlightNode(node);
	}
}

//
// Highlight all nodes corresponding to a given pattern:
//	- all nodes that are transitively children of a given node,
//	- all nodes that are transitively parent of a given node,
//	- all nodes that are transitively neighbors of a given node,
//
void	DGramExWinComplex::ShowChildren(DGramNodePtr node)
{
	HighlightNode(node);
	int nNodes;
	nNodes = mDGram->NodeGetNumChildren(node);	
	for (Int iNode = 0; iNode < nNodes; iNode++) {
		DGramNodePtr cNode = mDGram->NodeGetNthChild(node, iNode);
		if (!IsHighlightNode(cNode)) {
			ShowChildren(cNode);
		}
	}
	mNumNodes = nNodes;
}

void	DGramExWinComplex::ShowParents(DGramNodePtr node)
{
	HighlightNode(node);
	int nNodes;
	nNodes = mDGram->NodeGetNumParents(node);	
	for (Int iNode = 0; iNode < nNodes; iNode++) {
		DGramNodePtr cNode = mDGram->NodeGetNthParent(node, iNode);
		if (!IsHighlightNode(cNode)) {
			 ShowParents(cNode);
		}
	}
	mNumNodes = nNodes;
}


void	DGramExWinComplex::ShowNeighbors(DGramNodePtr node)
{
	HighlightNode(node);
	int nNodes;
	nNodes = mDGram->NodeGetNumNeighbors(node);	
	for (Int iNode = 0; iNode < nNodes; iNode++) {
		DGramNodePtr cNode = mDGram->NodeGetNthNeighbor(node, iNode);
		if (!IsHighlightNode(cNode)) {
			 ShowNeighbors(cNode);
		}
	}
	mNumNodes = nNodes;
}

void	DGramExWinComplex::NodeMenuSelectSArea(void)
{
	DGramNodePtr node = mDGram->NodeGetNfy();

	switch (mDGram->MenuSelectGetId()) {
	case DGRAMEXMENUNODE_SHOWCHILDREN:
		ResetNodes();
		ShowChildren(node);
		break;
	case DGRAMEXMENUNODE_SHOWPARENTS:
		ResetNodes();
		ShowParents(node);
		break;
	case DGRAMEXMENUNODE_SHOWNEIGHBORS:
		ResetNodes();
		ShowNeighbors(node);
		break;
	case DGRAMEXMENUNODE_DELETE:
		mDGram->NodeDelete(node, BOOL_TRUE);
		break;
	default:
		break;	
	}
}

void	DGramExWinComplex::LinkMenuSelectSArea(void)
{
	DGramLinkPtr link = mDGram->LinkGetNfy();
	DGramNodePtr node1, node2;

	switch (mDGram->MenuSelectGetId()) {
	case DGRAMEXMENULINK_SHOWEXTREMES:
		// Show the two extremes for the link
		ResetNodes();
		node1 = mDGram->LinkGetNodeFrom(link);
		node2 = mDGram->LinkGetNodeTo(link);
		HighlightNode(node1);
		HighlightNode(node2);
		break;
	case DGRAMEXMENULINK_DELETE:
		node1 = mDGram->LinkGetNodeFrom(link);
		node2 = mDGram->LinkGetNodeTo(link);
		mDGram->LinkDelete(node1, node2, BOOL_TRUE);
		break;
	default:
		break;
	}
}

void DGramExWinComplex::DiagrammerMenuSelectSArea(void)
{
	DGramNodePtr node, node1, node2;
	DGramLinkPtr link;

	switch(mDGram->MenuSelectGetId()) {
	case DGRAMEXMENUDGRAM_SELECTNODE:
		node = mDGram->ChooseNode();
		if (node != NULL) {
			ResetNodes();
			HighlightNode(node);
			mDGram->NodeShow(node);
		}
		break;
	case DGRAMEXMENUDGRAM_SELECTLINK:
		link = mDGram->ChooseLink();
		if (link != NULL) {
			ResetNodes();
			node1 = mDGram->LinkGetNodeFrom(link);
			node2 = mDGram->LinkGetNodeTo(link);
			HighlightNode(node1);
			HighlightNode(node2);
			mDGram->LinkShow(link);
		}
		break;
	default:
		break;
	}
}

void	DGramExWinComplex::Init(void)
{
	// Initialize the window 
	NDWin::NfyInit();

	// Add the call-back for the DGramSArea component
	mDGSArea = mDGram->GetDGramSArea();
	WIN_SETNFYHANDLER(DGramExWinComplex, mDGSArea, DGRAMSAREA_NFYNODEMENUSELECT, NodeMenuSelectSArea);
	WIN_SETNFYHANDLER(DGramExWinComplex, mDGSArea, DGRAMSAREA_NFYLINKMENUSELECT, LinkMenuSelectSArea);
	WIN_SETNFYHANDLER(DGramExWinComplex, mDGSArea, DGRAMSAREA_NFYDIAGRAMMERMENUSELECT, DiagrammerMenuSelectSArea);
}

// (( CodeGen: WgtNfyPlaceHolder ))

DGramExWinComplex::DGramExWinComplex(CStr modName, CStr resName) : NDWin(modName, resName)
{
	mNumNodes = 40;
	mNumLinks = 80;

	// (( CodeGen: WgtInitializations
	mDGram = (NDDGramPtr)GetNamedWgt("DGram");
	// )) CodeGen: WgtInitializations 3371

	//
	// The DGramSArea component of the diagrammer is only created after
	// initialization. We need to differ the installation of the
	// corresponding call-back after initialization of the window.
	//
	mDGSArea = NULL;
	WIN_SETNFYHANDLER(DGramExWinComplex, this, WIN_NFYINIT, Init);
}

void	DGramExWinComplex::AddNodesDGram(void)
{
	// Work box extension
	NDPoint32 wbExt;
	wbExt.x = 1500;
	wbExt.y = 500;
	mDGram->SetWorkArea(&wbExt);
	
	// Nodes
	NDRect16 gRect;
	gRect.Ori.x = gRect.Ori.y = 0;
	gRect.Ext.x = (Int16)wbExt.x;
	gRect.Ext.y = (Int16)wbExt.y;
	NDRegionPtr rgn = new NDRgn(&gRect);
	for (Int i = 0; i < mNumNodes; i++) {
		// Add node
		NDRect32 nodeRect;
		NDRect16 nRect16;
		Int attempts = 0;
		do {
			nodeRect.Ori.x = rand() % gRect.Ext.x;
			nodeRect.Ori.y = rand() % gRect.Ext.y;
			nodeRect.Ext.x = 20 + rand() % 20;
			nodeRect.Ext.y = 20 + rand() % 20;
			
			nRect16.Ori.x = (Int16)nodeRect.Ori.x;
			nRect16.Ori.y = (Int16)nodeRect.Ori.y;
			nRect16.Ext.x = (Int16)nodeRect.Ext.x;
			nRect16.Ext.y = (Int16)nodeRect.Ext.y;
			
		} while (rgn->RectPos((Rect16CPtr) &nRect16) != RGN_POSINSIDE 
			 && attempts++ <= 100);
		rgn->RectSubtract(&nRect16);
		
		DGramNodePtr node = mDGram->NodeAdd(&nodeRect, BOOL_FALSE);
		// Set label
		Char label[S_STRDEFSIZE];
		S_Sprintf(label, (CStr)"(%d)", i);
		mDGram->NodeSetLabel(node, (CStr)label);
		// Select random shape
		DGramNodeShapeEnum nShape = DGRAM_NODESHAPERECT +
				(rand() % (DGRAM_NODESHAPETRIANGLE -
					   DGRAM_NODESHAPERECT) + 1);
		DGramNodeDrawDataRec nDData;
		mDGram->NodeQueryDData(node, &nDData, 
				       DGRAM_DRAWMODESTANDARD);
		nDData.BgColor = NDColor::Yellow();
		nDData.Shape = nShape;
		mDGram->NodeSetDData(node, &nDData,
				     DGRAM_DRAWMODESTANDARD);
		mDGram->NodeQueryDData(node, &nDData, 
				       DGRAM_DRAWMODEFOCUSED);
		nDData.BgColor = NDColor::Red();
		nDData.Shape = nShape;
		mDGram->NodeSetDData(node, &nDData,
				     DGRAM_DRAWMODEFOCUSED);
	}
	delete(rgn);
}

void	DGramExWinComplex::AddLinksDGram(void)
{
	for (Int i = 0; i < mNumLinks; i++) {
		DGramLinkTypeEnum linkType = DGRAM_LINKDIRECTED +
				(rand() % (DGRAM_LINKUNDIRECTED -
					   DGRAM_LINKDIRECTED + 1));
		Int32 nId1 = (Int32)(rand() % mNumNodes) + 1;
		DGramNodePtr node1 = mDGram->NodeGetById(nId1);
		DBG_CHECK(node1 != NULL);
		DGramNodePtr node2;
		do {
			Int32 nId2 = (Int32)(rand() % mNumNodes) + 1;
			node2 = mDGram->NodeGetById(nId2);
			DBG_CHECK(node2 != NULL);
		} while (node2 == node1);

		DGramLinkPtr link = mDGram->LinkAdd(node1, node2, linkType, BOOL_FALSE);
	}
}

void 	DGramExWinComplex::LoadShow(void)
{
	DGramExWinComplexPtr win;

	win = new DGramExWinComplex("DGramEx", "WinComplex");

	win->Init();
  
	// Change the various popups
  	NDMnuPtr menuNode = new NDMnu("DGramEx", "MenuNode");
	win->mDGram->SetNodeMenu(menuNode);
 	NDMnuPtr menuLink = new NDMnu("DGramEx", "MenuLink");
	win->mDGram->SetLinkMenu(menuLink);
  	NDMnuPtr menuDiagrammer = new NDMnu("DGramEx", "MenuDiagrammer");
	win->mDGram->SetDrawMenu(menuDiagrammer);

	// Set Handscrolling mode
	win->mDGram->SetHandScroll(BOOL_TRUE);

	// Fill the DGram with randomly generated Nodes and links
	win->AddNodesDGram();
	win->AddLinksDGram();

	win->Show();
}

// )) CodeGen: WindowSection WinComplex

//--------------------------------------------------------------------------
//	"Diagrammers" Window Implementation
//--------------------------------------------------------------------------

// (( CodeGen: WindowSection WinSelect
// (( CodeGen: MenuImplementationPlaceHolder ))

// (( CodeGen: WgtNfyHandler HitPButSimple
void 	DGramExWinSelect::HitPButSimple(void)
{
	DGramExWinSimple::LoadShow();
}
// )) CodeGen: WgtNfyHandler HitPButSimple

// (( CodeGen: WgtNfyHandler HitPButImage
void 	DGramExWinSelect::HitPButImage(void)
{
	DGramExWinImage::LoadShow();
}
// )) CodeGen: WgtNfyHandler HitPButImage

// (( CodeGen: WgtNfyHandler HitPButComplex
void 	DGramExWinSelect::HitPButComplex(void)
{
	DGramExWinComplex::LoadShow();
}
// )) CodeGen: WgtNfyHandler HitPButComplex

// (( CodeGen: WgtNfyHandler HitPButQuit
void 	DGramExWinSelect::HitPButQuit(void)
{
	NDEvent::MainExit();
}
// )) CodeGen: WgtNfyHandler HitPButQuit

// (( CodeGen: WgtNfyPlaceHolder ))

DGramExWinSelect::DGramExWinSelect(CStr modName, CStr resName) : NDWin(modName, resName)
{
	// (( CodeGen: WgtInitializations
	mPButSimple = (NDPButPtr)GetNamedWgt("PButSimple");
	mPButImage = (NDPButPtr)GetNamedWgt("PButImage");
	mPButComplex = (NDPButPtr)GetNamedWgt("PButComplex");
	mPButQuit = (NDPButPtr)GetNamedWgt("PButQuit");
	WIN_SETNFYHANDLER(DGramExWinSelect, mPButSimple, TBUT_NFYHIT, HitPButSimple);
	WIN_SETNFYHANDLER(DGramExWinSelect, mPButImage, TBUT_NFYHIT, HitPButImage);
	WIN_SETNFYHANDLER(DGramExWinSelect, mPButComplex, TBUT_NFYHIT, HitPButComplex);
	WIN_SETNFYHANDLER(DGramExWinSelect, mPButQuit, TBUT_NFYHIT, HitPButQuit);
	// )) CodeGen: WgtInitializations 42260
}

void 	DGramExWinSelect::LoadShow(void)
{
	DGramExWinSelectPtr win;

	win = new DGramExWinSelect("DGramEx", "WinSelect");

	win->Init();
	win->Show();
}

// )) CodeGen: WindowSection WinSelect

// (( CodeGen: WindowImplementationPlaceHolder ))

//--------------------------------------------------------------------------
//	Main
//--------------------------------------------------------------------------

// (( CodeGen: MainSection

//#include <nd.h>

ERR_DECLARE

int 	main(int argc, char** argv)
{
	ERR_MAININIT;
	ERR_MODULEUSE;

	ND::Init(argc, argv);

	// (( CodeGen: ClassRegistration
	// )) CodeGen: ClassRegistration 0

	(void)NDRLib::LoadLibFile("DGramEx", "dgramex.dat");

	DGramExWinSelect::LoadShow();

	ND::Run();
	ND::Exit();

	return EXIT_OK;
}
// )) CodeGen: MainSection
// (( CodeGen: MainPlaceHolder ))
