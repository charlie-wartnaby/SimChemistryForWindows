/////////////////////////////////////////////////////////////////////////////
//                      SimChemistry for Windows
// www.simchemistry.co.uk    / www.wartnaby.org
// chem @ simchemistry.co.uk / charlie @ wartnaby.org
//
// Copyright 2016 Charlie Wartnaby
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
/////////////////////////////////////////////////////////////////////////////

// consvw.cpp : implementation of the CConsoleView class
//

#include "stdafx.h"
#include "simobjs.h"
#include "sim.h"
#include "lennard.h"

#include "lendoc.h"
#include "consvw.h"                                

#include <limits.h>
// limits.h doesn't seem to have these, in debug anyway:
#ifndef INT_MIN
#define INT_MIN (-32768)
#define INT_MAX 32767
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConsoleView

IMPLEMENT_DYNCREATE(CConsoleView, CScrollView)

BEGIN_MESSAGE_MAP(CConsoleView, CScrollView)
	//{{AFX_MSG_MAP(CConsoleView)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConsoleView construction/destruction

CConsoleView::CConsoleView()
{
	// TODO: add construction code here
}

CConsoleView::~CConsoleView()
{
	CLenDoc* pDoc = GetDocument();   
	pDoc->m_bConsShowing = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CConsoleView drawing

void CConsoleView::OnDraw(CDC* pDC)
{
	CLenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);   
	CSim* pSim = pDoc->m_pSim;


	
	CFont font;
	
	font.CreateFont(12, // int nHeight,
	                        0, // int nWidth,
	                        0, // int nEscapement, 
                            0, // int nOrientation, 
                            FW_DONTCARE, // int nWeight, 
                            FALSE, // BYTE bItalic, 
                            FALSE, // BYTE bUnderline, 
                            0, // BYTE cStrikeOut,
                            ANSI_CHARSET, // BYTE nCharSet, 
                            OUT_DEFAULT_PRECIS, // BYTE nOutPrecision, 
                            CLIP_DEFAULT_PRECIS, // BYTE nClipPrecision, 
                            DEFAULT_QUALITY, // BYTE nQuality, 
                            DEFAULT_PITCH, // BYTE nPitchAndFamily,  
                            "MS Sans Serif"); // LPCSTR lpszFacename
	
	CFont* pOldFont = pDC->SelectObject(&font);      
	                                                              
	// get height of font using string with descenders and big letters	                                                              
	CSize fontSize = pDC->GetTextExtent("AgjyM", sizeof("AgjyM"));    
	
	int nRowHeight = fontSize.cy; // could adjust spacing here

	// set current scroll sizes to accommodate all of history
	CSize sizeTotal;
	sizeTotal.cx = 5000;         
	int numHistEntries = pSim->GetNumHistoryEntries();
	long sizeReqd = (long) numHistEntries * nRowHeight;
	if (sizeReqd > INT_MAX)
	{
		sizeTotal.cy = INT_MAX;
	}                      
	else
	{
		sizeTotal.cy = (int) sizeReqd;
	}
	
	
	SetScrollSizes(MM_TEXT, sizeTotal, 
	     CSize(100, nRowHeight * 20), CSize(10, nRowHeight));

	// Temporary simple drawing of command history
	int y = 0; 
	CHistoryEntry* pEntry;        
	int currentIndex = -1;  
	
	while (SR_NO_MORE != pSim->ReadFromHistory(pEntry, HT_ALL, HP_NEXT, currentIndex))
	{   
		if (HT_CMD == pEntry->type)
		{
			pDC->SetTextColor(RGB(0,0,0));
		}
		else
		{
		     pDC->SetTextColor(RGB(255,0,0));
		}
		
		
		if (currentIndex == pSim->m_nCurrentCmdIndex)
		{                                   
			// highlight currently executing command
			pDC->SetTextColor(RGB(0,255,0));
			pDC->TextOut(20, y, pEntry->string);  
		}
		else
		{
			pDC->TextOut(0, y, pEntry->string);  
		}
				
		y += nRowHeight;
	}		                                   
	
	pDC->SelectObject(pOldFont);
	
}

void CConsoleView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	CLenDoc* pDoc = (CLenDoc*) GetDocument();   
	pDoc->m_pConsWnd = (CMDIChildWnd*) GetParentFrame();
	pDoc->m_bConsShowing = TRUE;                        
}

/////////////////////////////////////////////////////////////////////////////
// CConsoleView printing

BOOL CConsoleView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CConsoleView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CConsoleView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CConsoleView diagnostics

#ifdef _DEBUG
void CConsoleView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CConsoleView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CLenDoc* CConsoleView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLenDoc)));
	return (CLenDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CConsoleView message handlers
void CConsoleView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView); 
	if (bActivate)
	{ 
  		CLenDoc* pDoc = GetDocument();
		CString docTitle = pDoc->GetTitle();
		docTitle += ":Console";

		CFrameWnd* pFrame = GetParentFrame();	
		ASSERT_VALID(pFrame);
		pFrame->SetWindowText(docTitle);  
	}

}

void CConsoleView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Copied from CSimView in case user starts drawing with
	// this window highlighted
	
	// If ESC pressed, stop doing drawing if we are currently

	CLenDoc* pDoc = (CLenDoc*) GetDocument();
	CSim* pSim = pDoc->m_pSim;

	if ((27 == nChar) && (SM_NOT_DRAWING != pSim->m_nDrawingState))
	{                          
		// may become necessary to tell drawing object to stop,
		// but for now just say we've stopped
		pSim->m_nDrawingState = SM_NOT_DRAWING;
		if (SM_SIM_PAUSED == pSim->m_nSimState) 
		{
			pSim->m_nSimState = SM_SIM_RUNNING;
		}                                    
		TRACE("Drawing stopped by pressing Esc\n");   
		Invalidate(FALSE);
	}
	else
	{                    
		// sim may break delay if replaying from script
		
		pSim->OnChar(nChar, nRepCnt, nFlags);
	}
}
