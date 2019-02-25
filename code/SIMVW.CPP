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

// simvw.cpp : implementation file
//

#include "stdafx.h"    
#include "lennard.h"
#include "mainfrm.h"
#include "simobjs.h"    
#include "sim.h"       
#include "lendoc.h"
#include "tracker.h"   
#include "simvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimView

IMPLEMENT_DYNCREATE(CSimView, CView)

CSimView::CSimView()
{	
	m_pMemDC = new CDC; 
	m_pSpareDC1 = new CDC;
	m_pSpareDC2 = new CDC;
	m_pBitmap = new CBitmap; 
	m_pSpareBitmap1 = new CBitmap;
	m_pSpareBitmap2 = new CBitmap;
	m_pTrackingHotSpot = NULL;
	m_nHotSpotChangedCursor = FALSE;
	m_bInitialised = FALSE;
	m_nLeftButtonDown = FALSE;
	m_nDragging = FALSE;
	m_nFlashState = FS_NOT_FLASHING;
}

CSimView::~CSimView()
{
	CLenDoc* pDoc = (CLenDoc*) GetDocument();   
	pDoc->m_bSimShowing = FALSE;
	delete m_pMemDC;   
	delete m_pSpareDC1;
	delete m_pSpareDC2;
	delete m_pBitmap; 
	delete m_pSpareBitmap1;
	delete m_pSpareBitmap2;
}


BEGIN_MESSAGE_MAP(CSimView, CView)
	//{{AFX_MSG_MAP(CSimView)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CHAR()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSimView drawing

void CSimView::OnDraw(CDC* pDC)
{   
	int dcContext = pDC->SaveDC();

	pDC->SetBkColor(RGB(255, 255, 255)); 

	CLenDoc* pDoc = (CLenDoc*) GetDocument();
	CSim* pSim = pDoc->m_pSim;  

    CString sStatus;
    
    sprintf(buff, "sim: %f ns  delay: %f ns  ", pSim->m_dSimTime, pSim->m_dDelayTimer);

    CString string(buff);
    
    if (pSim->m_nDrawingState == SM_NOT_DRAWING)
    {   
	    if (SM_SIM_RUNNING == pSim->m_nSimState)
	    {
	    	string += "  Sim running!  ";
	    }
	    else
	    {
	    	string += "  Sim stopped  ";
	    }
	}
   
    sStatus = string;

   	if (pSim->m_nDrawingState != SM_NOT_DRAWING)
   	{
        sprintf(buff, "x: %g nm  y: %g nm  ",
                      pSim->m_dSimMouseX, pSim->m_dSimMouseY);

        sStatus += buff;
    }

    // Now handle flashing messages to tell user to press
    // Esc when finished drawing or to press a key to break pause,
    // if either of these are relevant          
    
    if ((pSim->m_nDrawingState != SM_NOT_DRAWING) 
        || (SM_REPLAY_PAUSED_PLAY == pSim->m_nReplayState)
        || (SM_REPLAY_PAUSED_STOP == pSim->m_nReplayState))
    {
    	
    	if (FS_NOT_FLASHING == m_nFlashState)
    	{   
    		// If setting this timer fails, message will not flash                           
    		m_nFlashTimerID = SetTimer(1, 500, NULL);  // 500 ms
			ASSERT(m_nFlashTimerID);
			    		
    		m_nFlashState = FS_FLASH_HI;
    	} 
    	
    	if (FS_FLASH_HI == m_nFlashState)
    	{    	
	    	if (pSim->m_nDrawingState != SM_NOT_DRAWING)
	    	{
	    		string = "Press Esc to stop drawing";
	    	}
	    	else
	    	{
	    		string = "Replay paused -- press a key";
	    	}
	    	
    	
            sStatus += string;
		}                                
		else
		{
			// off part of flashing cycle so draw nothing
		}   	
    }  
    else // not flashing or stop flashing
    { 
    	if (FS_NOT_FLASHING != m_nFlashState)
    	{      
    		if (m_nFlashTimerID) KillTimer(m_nFlashTimerID);
       		
    		m_nFlashState = FS_NOT_FLASHING;
    	}
    }

    CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();

    pMainFrame->SetStatusText(sStatus);

	CBitmap* pOldBitmap1 = (CBitmap*) m_pSpareDC1->SelectObject(m_pSpareBitmap1);
		
	// Iterate through list of object types
	CObjType* pType;    
	int lastTypeIndex = pSim->m_typeArray.GetUpperBound();
    
	for (int index = 0; index <= lastTypeIndex; index ++)
	{               
		pType = (CObjType*) pSim->m_typeArray[index];
		pType->DrawAllSObjs(pDC, m_pSpareDC1, m_pSpareDC2);
	}
	
	// If user is drawing something, get obj type responsible to do
	// whatever extra drawing it wants
	
	if (SM_NOT_DRAWING != pSim->m_nDrawingState)
	{
		pSim->m_pDrawingType->UserDrawing(pDC, m_pSpareDC1, m_pSpareDC2);
	}                                                           
	                           
	// User may be dragging to make selections, in which case show box
	
	if (m_nDragging)
	{
		CPoint ptCorner1 = pSim->m_ptLogDragStart;	
		CPoint ptCorner2 = pSim->m_ptLogMouse;
		
		pDC->MoveTo(ptCorner1);
		pDC->LineTo(ptCorner1.x, ptCorner2.y);
		pDC->LineTo(ptCorner2);
		pDC->LineTo(ptCorner2.x, ptCorner1.y);
		pDC->LineTo(ptCorner1);
	}     
	
	// Finally draw any active tracking rectangles on selected objects       
	
	if (SM_SIM_STOPPED == pSim->m_nSimState)
	{
		CSObj* pObj = NULL;
		POSITION pos;
		
		pos = pSim->m_selectedObjects.GetHeadPosition();
		
		while (NULL != pos)
		{
			pObj = (CSObj*) pSim->m_selectedObjects.GetAt(pos);
			pObj->DrawTracker(pDC);
			pSim->m_selectedObjects.GetNext(pos);
		}
		
	}
	                          
	m_pSpareDC1->SelectObject(pOldBitmap1);  
	
	pDC->RestoreDC(dcContext);
}

void CSimView::OnInitialUpdate()
{                   
	// The device context and bitmap manipulations are borrowed
	// from Inside Visual C++ and some of it may well be
	// unnecessary...
	
	CLenDoc* pDoc = (CLenDoc*) GetDocument();   
	pDoc->m_pSimWnd = (CMDIChildWnd*) GetParentFrame();
	pDoc->m_bSimShowing = TRUE; 
	m_bInitialised = TRUE;

    // Get device context for this window
	CClientDC dc(this);
	OnPrepareDC(&dc);

    // measure size of window and tell simulation about it:
    WINDOWPLACEMENT wpl;
    BOOL gotPlacement = GetWindowPlacement(&wpl);
    ASSERT(gotPlacement);
    TRACE("GetWindowPlacement says rect is: (%ld,%ld,%ld,%ld)\n", wpl.rcNormalPosition.left,
                                                                  wpl.rcNormalPosition.top,
                                                                  wpl.rcNormalPosition.right,
                                                                  wpl.rcNormalPosition.bottom);

    dc.DPtoLP(&wpl.rcNormalPosition);
    TRACE("After conversion it is: (%ld,%ld,%ld,%ld)\n", wpl.rcNormalPosition.left,
                                                                  wpl.rcNormalPosition.top,
                                                                  wpl.rcNormalPosition.right,
                                                                  wpl.rcNormalPosition.bottom);

    pDoc->m_pSim->OnSizeSimScreen(wpl.rcNormalPosition);


	if (NULL != m_pMemDC->GetSafeHdc())
	{
		m_pMemDC->DeleteDC();
		delete m_pBitmap;
		m_pBitmap = new CBitmap;
	}
	
	if (m_pMemDC->GetSafeHdc() == NULL)
	{                                 
		CRect rect;
		GetClientRect(&rect);
	                              
		m_pMemDC->CreateCompatibleDC(&dc);
		m_pBitmap->CreateCompatibleBitmap(&dc, rect.right, rect.bottom);
		m_pMemDC->SetMapMode(MM_LOENGLISH);
	}                              

	if (m_pSpareDC1->GetSafeHdc() == NULL)
	{                                 
		m_pSpareDC1->CreateCompatibleDC(&dc);
		m_pSpareBitmap1->CreateCompatibleBitmap(&dc, MAX_BITMASK_SIZE, MAX_BITMASK_SIZE);
		m_pSpareDC1->SetMapMode(MM_LOENGLISH);
	}                              

	if (m_pSpareDC2->GetSafeHdc() == NULL)
	{                                 
		CSize sizeTotal;
		CRect rect;
		GetClientRect(&rect);
		sizeTotal.cx = MAX_BITMASK_SIZE;
		sizeTotal.cy = MAX_BITMASK_SIZE;
	                              
		CRect rectMax(0, 0, sizeTotal.cx, -sizeTotal.cy);
		dc.LPtoDP(rectMax);
		m_pSpareDC2->CreateCompatibleDC(&dc);
		m_pSpareDC2->SetMapMode(MM_LOENGLISH);
	}                              

	CView::OnInitialUpdate();

}           
            
void CSimView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{                                                      
	CView::OnPrepareDC(pDC, pInfo);
	pDC->SetMapMode(MM_LOENGLISH);
}

void CSimView::SimifyMousePoint(CPoint point, double* simX, double* simY)
{
	CClientDC dc(this);
	OnPrepareDC (&dc);
	
	dc.SetMapMode(MM_LOENGLISH);
	
	dc.DPtoLP(&point);

	CLenDoc* pDoc = (CLenDoc*) GetDocument();

	pDoc->m_pSim->LogicalToSim(point, simX, simY);
}

void CSimView::GridifyMousePoint(CPoint* pPoint)
{
	CLenDoc* pDoc = (CLenDoc*) GetDocument();
	
	if (pDoc->m_pSim->m_dGridSize > NEARLY_ZERO)
	{
		// grid switched on
		double x, y;
			
		SimifyMousePoint(*pPoint, &x, &y);
		
		pDoc->m_pSim->Gridify(&x, &y);

		*pPoint = pDoc->m_pSim->SimToLogical(x, y);
		
		CClientDC dc(this);

		OnPrepareDC (&dc);
			
		dc.SetMapMode(MM_LOENGLISH);
			
		dc.LPtoDP(pPoint);
	}	
}
               
void CSimView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	Invalidate(FALSE);
}                              

/////////////////////////////////////////////////////////////////////////////
// CSimView printing

BOOL CSimView::OnPreparePrinting(CPrintInfo* pInfo)
{ 
	// default preparation
	BOOL success = DoPreparePrinting(pInfo);
	        
	if (success)
	{
		CLenDoc* pDoc = (CLenDoc*) GetDocument();
		CSim* pSim = pDoc->m_pSim;  
	    
	    if (SM_SIM_RUNNING == pSim->m_nSimState)
	    {
	    	pSim->m_nSimState = SM_SIM_PAUSED;
	    }
	}   
	
	return success;
}

void CSimView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSimView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	CLenDoc* pDoc = (CLenDoc*) GetDocument();
	CSim* pSim = pDoc->m_pSim;  
    
    if (SM_SIM_PAUSED == pSim->m_nSimState)
    {
    	pSim->m_nSimState = SM_SIM_RUNNING;
    }
}


/////////////////////////////////////////////////////////////////////////////
// CSimView message handlers
 

void CSimView::OnPaint()
{
    CRect updateRect;
    CPaintDC dc(this);
    OnPrepareDC(&dc);
    dc.GetClipBox(&updateRect);
    CBitmap* pOldBitmap = (CBitmap*) (m_pMemDC->SelectObject(m_pBitmap));
    
    m_pMemDC->SelectClipRgn(NULL);
    m_pMemDC->IntersectClipRect(&updateRect);

    DoWhite1(updateRect);
    DoWhite2(updateRect);
//    m_pMemDC->SelectStockObject(WHITE_BRUSH);
//    m_pMemDC->PatBlt(updateRect.left, updateRect.top,
//                     updateRect.Width(), updateRect.Height(), PATCOPY);
    
    CLenDoc* pDoc = (CLenDoc*) GetDocument();
    CSim* pSim = pDoc->m_pSim;

    if (pSim->m_nNeedGreyBars)
    {
        DoGreyBars(pSim);
 //       m_pMemDC->SelectStockObject(GRAY_BRUSH);
   //     m_pMemDC->PatBlt(pSim->m_GreyLogicalRect1.left, pSim->m_GreyLogicalRect1.top,
     //                pSim->m_GreyLogicalRect1.Width(), pSim->m_GreyLogicalRect1.Height(), PATCOPY);
     //   m_pMemDC->PatBlt(pSim->m_GreyLogicalRect2.left, pSim->m_GreyLogicalRect2.top,
       //              pSim->m_GreyLogicalRect2.Width(), pSim->m_GreyLogicalRect2.Height(), PATCOPY);
    }

    OnDraw(m_pMemDC);
    DoBitBlt(updateRect, &dc);
//        dc.BitBlt(updateRect.left, updateRect.top,
  //            updateRect.Width(), updateRect.Height(), m_pMemDC,
    //          updateRect.left, updateRect.top, SRCCOPY);  

    m_pMemDC->SelectObject(pOldBitmap);
}                                       

void CSimView::DoBitBlt(CRect updateRect, CPaintDC* dc)
{
    dc->BitBlt(updateRect.left, updateRect.top,
              updateRect.Width(), updateRect.Height(), m_pMemDC,
              updateRect.left, updateRect.top, SRCCOPY);  
}

void CSimView::DoWhite1(CRect updateRect)
{
    m_pMemDC->SelectStockObject(WHITE_BRUSH);
    m_pMemDC->PatBlt(updateRect.left, updateRect.top,
                     updateRect.Width(), updateRect.Height(), PATCOPY );
}

void CSimView::DoWhite2(CRect updateRect)
{
    m_pMemDC->SelectStockObject(WHITE_BRUSH);
    m_pMemDC->PatBlt(updateRect.left, updateRect.top,
                     updateRect.Width(), updateRect.Height(), WHITENESS );
}

void CSimView::DoGreyBars(CSim* pSim)
{
        m_pMemDC->SelectStockObject(GRAY_BRUSH);
        m_pMemDC->PatBlt(pSim->m_GreyLogicalRect1.left, pSim->m_GreyLogicalRect1.top,
                     pSim->m_GreyLogicalRect1.Width(), pSim->m_GreyLogicalRect1.Height(), PATCOPY);
        m_pMemDC->PatBlt(pSim->m_GreyLogicalRect2.left, pSim->m_GreyLogicalRect2.top,
                     pSim->m_GreyLogicalRect2.Width(), pSim->m_GreyLogicalRect2.Height(), PATCOPY);
}

void CSimView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CLenDoc* pDoc = (CLenDoc*) GetDocument();
	CSim* pSim = pDoc->m_pSim;
	
	CRect rect;
	GetClientRect(&rect);
	BOOL inClientRect = rect.PtInRect(point);
	pSim->m_bInClientRect = inClientRect;                                          
	
	SimifyMousePoint(point, &(pSim->m_dSimMouseX), &(pSim->m_dSimMouseY));
	pSim->m_ptLogMouse = pSim->SimToLogical(pSim->m_dSimMouseX, pSim->m_dSimMouseY);

	if ((SM_NOT_DRAWING != pSim->m_nDrawingState)
	     && pSim->m_pDrawingType->m_nUseGridWhenGridOn && pSim->m_nUseGrid)
	{
		pSim->Gridify(&pSim->m_dSimMouseX, &pSim->m_dSimMouseY);
		pSim->m_ptLogMouse = pSim->SimToLogical(pSim->m_dSimMouseX, pSim->m_dSimMouseY);
	}

	if ((SM_DRAGGING != pSim->m_nDrawingState) 
	        && (SM_AWAITING_CLICK2 != pSim->m_nDrawingState))
	{
		// Note start coords in case drawing an object, but don't do so
		// if now looking for second point (eg opposite corner of graph)
		pSim->m_dSimDragStartX = pSim->m_dSimMouseX;
		pSim->m_dSimDragStartY = pSim->m_dSimMouseY;    
		pSim->m_ptLogDragStart = pSim->m_ptLogMouse; 
	} 
		

	// First consider if user has clicked hotspot on slider or graph for example
	
	CHotSpot* pHotSpot;
	int nHit = FALSE;   
	int nHitType;
		
    for (int i = 0; !nHit && i <= pSim->m_hotSpots.GetUpperBound(); i++)
    {
    	pHotSpot = (CHotSpot*) pSim->m_hotSpots[i];
    	
    	if (pHotSpot->m_rect.PtInRect(point))
    	{
    		nHit = TRUE;
    		nHitType = pHotSpot->m_pOwner->OnHotSpotLButtonDown(pHotSpot, nFlags, point);
    	}
    }  
    
    
    if (nHit)
    {
    	switch (nHitType)
    	{
    		case HSLBD_ONE_OFF_ACTION_DONE:
    			// nothing more to do, has been done by target object
    			break;
    			
    		case HSLBD_TRACKING_STARTED:
    			// must direct mouse moves to this object until button up
    			m_pTrackingHotSpot = pHotSpot;
    			SetCapture();
    			break;
    			
    		case HSLBD_ERROR:
    		default:
    			ASSERT(FALSE);
    			nHit = FALSE; 
    			break;
    	}
    }
    
    if (!nHit)
    {
    	// Didn't hit a hot spot, so could be starting to draw something
    	// or selecting something
    	
		if (SM_NOT_DRAWING != pSim->m_nDrawingState)
		{
			pSim->m_pDrawingType->OnLButtonDown(); 
		}
		else
		{
			// could be selecting something but ignore if sim running
			if (SM_SIM_STOPPED == pSim->m_nSimState)
			{
				m_nLeftButtonDown = TRUE;   
				
				if (pSim->OnLButtonDown(point, nFlags, this))
				{
					// Returns true if did object tracking, which would have
					// been terminated by button up, with the tracker accepting Windows
					// messages instead of us
					
					m_nLeftButtonDown = FALSE;
				}
			}
		}
	}
		
	Invalidate(FALSE);  
}

void CSimView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CLenDoc* pDoc = (CLenDoc*) GetDocument();
	CSim* pSim = pDoc->m_pSim;
	
	if (m_pTrackingHotSpot != NULL)
	{
		m_pTrackingHotSpot->m_pOwner->OnHotSpotLButtonUp(m_pTrackingHotSpot, nFlags, point);
		
		m_pTrackingHotSpot = NULL;
	}
	
	ReleaseCapture();
	
	if ((SM_NOT_DRAWING != pSim->m_nDrawingState) || m_nDragging)
	{
		CRect rect;
		GetClientRect(&rect);
		BOOL inClientRect = rect.PtInRect(point);
		pSim->m_bInClientRect = inClientRect;
		SimifyMousePoint(point, &(pSim->m_dSimMouseX), &(pSim->m_dSimMouseY));
        pSim->m_ptLogMouse = pSim->SimToLogical(pSim->m_dSimMouseX, pSim->m_dSimMouseY);		
		
		if (SM_NOT_DRAWING != pSim->m_nDrawingState)
		{
			if (pSim->m_pDrawingType->m_nUseGridWhenGridOn && pSim->m_nUseGrid)
			{
				pSim->Gridify(&pSim->m_dSimMouseX, &pSim->m_dSimMouseY);
			}
            
            pSim->m_ptLogMouse = pSim->SimToLogical(pSim->m_dSimMouseX, pSim->m_dSimMouseY);

			pSim->m_pDrawingType->OnLButtonUp();
		}
		else
		{
			m_nDragging = FALSE;
			pSim->OnFinishedDragging(nFlags);
		}
		
		Invalidate(FALSE);  
	}
	

	m_nLeftButtonDown = FALSE;
}

void CSimView::OnMouseMove(UINT nFlags, CPoint point)
{
	CLenDoc* pDoc = (CLenDoc*) GetDocument();
	CSim* pSim = pDoc->m_pSim;
	
	if (NULL != m_pTrackingHotSpot)
	{
		// We are moving a hotspot, let it know   
		TRACE("Tracking a hotspot in CSimView::OnMouseMove, redrawing\n");
		
		m_pTrackingHotSpot->m_pOwner->OnHotSpotMouseMove(m_pTrackingHotSpot, nFlags, point);
		Invalidate(FALSE);  // redraw including moved hotspot
	}
	else if (m_nHotSpotChangedCursor)
	{
		// OnSetCursor doesn't get called if mouse captured, but we had
		// to capture mouse to stop the cursor going back to an arrow every
		// time this window is redrawn with the sim running. So call it ourselves,
		// giving opportunity to move back off hotspot and release capture.
		// Except not if we are tracking a moving hotspot.
		
		OnSetCursor(this, HTCLIENT, 0);
	}
	
	if ((SM_NOT_DRAWING != pSim->m_nDrawingState) || m_nLeftButtonDown)
	{   
		// The user is drawing something or selecting, so may have to set cursors
		// and the like as the mouse pointer moves about.
		CRect rect;
		GetClientRect(&rect);
		BOOL inClientRect = rect.PtInRect(point); 
		
		if (inClientRect && !m_bWasInClientRect)
		{
			SetCapture();
		}
		else if (!inClientRect && m_bWasInClientRect)
		{
			ReleaseCapture();
		}
		m_bWasInClientRect = inClientRect;
		pSim->m_bInClientRect = inClientRect;
		SimifyMousePoint(point, &(pSim->m_dSimMouseX), &(pSim->m_dSimMouseY));
		pSim->m_ptLogMouse = pSim->SimToLogical(pSim->m_dSimMouseX, pSim->m_dSimMouseY); 
		
		if (SM_NOT_DRAWING != pSim->m_nDrawingState)
		{
			if (pSim->m_pDrawingType->m_nUseGridWhenGridOn && pSim->m_nUseGrid)
			{
				pSim->Gridify(&pSim->m_dSimMouseX, &pSim->m_dSimMouseY);
			}
            
            pSim->m_ptLogMouse = pSim->SimToLogical(pSim->m_dSimMouseX, pSim->m_dSimMouseY);
			pSim->m_pDrawingType->OnMouseMove();
		}
		else
		{
			if (m_nLeftButtonDown) m_nDragging = TRUE;
		}
		
		Invalidate(FALSE);  
	}   
//	else
//	{   
//		if (m_bWasInClientRect)
//		{
//			m_bWasInClientRect = FALSE;
//			ReleaseCapture();
//		}
//	}
}

void CSimView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
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

void CSimView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (m_bInitialised)      // don't do it until OnInitialUpdate already been called
	{
		// need to resize the memory DC and bitmap used for quick
		// molecule drawing if window changes size
		OnInitialUpdate();
	}
}

void CSimView::OnTimer(UINT nIDEvent)
{
	switch (m_nFlashState)
	{
		case FS_NOT_FLASHING:
			// timer shouldn't still be running
			ASSERT(FALSE);
			break;
			
		case FS_FLASH_HI:
			m_nFlashState = FS_FLASH_LO;
			break;
			  
		case FS_FLASH_LO:
			m_nFlashState = FS_FLASH_HI;
			break;        
			
		default:
			ASSERT(FALSE);
			m_nFlashState = FS_FLASH_HI; // make msg visible by default
			break;
	}
	
	Invalidate(FALSE);
}

void CSimView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CLenDoc* pDoc = (CLenDoc*) GetDocument();
	CSim* pSim = pDoc->m_pSim;
	
	if (SM_NOT_DRAWING == pSim->m_nDrawingState)
	{
		SimifyMousePoint(point, &(pSim->m_dSimMouseX), &(pSim->m_dSimMouseY));
		
		pSim->OnLButtonDblClk(nFlags);
		
		Invalidate(FALSE);  
	}
}

void CSimView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CLenDoc* pDoc = (CLenDoc*) GetDocument();
	CSim* pSim = pDoc->m_pSim;
	
	if (SM_NOT_DRAWING == pSim->m_nDrawingState)
	{
		SimifyMousePoint(point, &(pSim->m_dSimMouseX), &(pSim->m_dSimMouseY));
		
		if (pSim->OnRButtonDown(nFlags))
		{
			// if returns TRUE, do floating Object menu
			
			CMenu floatMenu;
			floatMenu.LoadMenu(IDR_LENNTYPE);
 	
			ClientToScreen(&point);
		
			CMenu* pPopup = floatMenu.GetSubMenu(2); // Object submenu
			ASSERT(pPopup != NULL);
	
			// route commands through the main frame
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		                       point.x,
		                       point.y, 
		                       theApp.m_pMainWnd);
		}
		
		Invalidate(FALSE);  
	}
}

BOOL CSimView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{   
	CLenDoc* pDoc = (CLenDoc*) GetDocument();
	CSim* pSim = pDoc->m_pSim;   

	int nDoDefault;
	
	// Can set new cursor over hot spots (eg slider knobs) even
	// if sim running
	
		
	// Only want to think about changing cursors over tracking
	// rectangles if sim stopped and not drawing either
	
	CHotSpot* pHotSpot;
	int nHit = FALSE;
		
    for (int i = 0; !nHit && i <= pSim->m_hotSpots.GetUpperBound(); i++)
    {
    	pHotSpot = (CHotSpot*) pSim->m_hotSpots[i];
    	
    	nHit = pHotSpot->m_pOwner->HotSpotSetCursor(pHotSpot, this, nHitTest);
    } 
    
    if (nHit)
    {
    	if (!m_nHotSpotChangedCursor)
    	{
	    	SetCapture();
	    	m_nHotSpotChangedCursor = TRUE;
	    	TRACE("set capture over hotspot\n");
	    }
    }
    else 
    {
	    if (m_nHotSpotChangedCursor)
	    {
	    	// Have moved off hotspot, so release mouse capture
	    	ReleaseCapture();               
	    	TRACE("released hotspot capture\n");
	    }
	    
	    m_nHotSpotChangedCursor = FALSE;  
    }
    
	nDoDefault = !(SM_SIM_STOPPED == pSim->m_nSimState);
		
	if (!nHit && !nDoDefault)
	{
		// scan selected objs looking for hit test on tracking rectangles
		CSObj* pObj = NULL;
		POSITION pos;

		// don't allow resize if multiple selection, so convey this		
		int nMultiple = pSim->m_selectedObjects.GetCount() > 1;
		
		pos = pSim->m_selectedObjects.GetHeadPosition();
		
		while ((NULL != pos) && !nHit)
		{
			pObj = (CSObj*) pSim->m_selectedObjects.GetAt(pos);
			nHit = pObj->SetCursor(this, nHitTest, nMultiple);
			pSim->m_selectedObjects.GetNext(pos);
		}   
		
		nDoDefault = !nHit;    
	}
	
	if (nDoDefault)
	{
		// Sim running or drawing, or no hits on selected obj tracking rectangles
		return CView::OnSetCursor(pWnd, nHitTest, message);
	}
	else             
	{
		return TRUE; // we've dealt with it
	}
}
