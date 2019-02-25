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

// Much of this code copied from \msvc\mfc\src\trckrect.cpp

#include "stdafx.h"
#include "lennard.h"
#include "simobjs.h"
#include "sim.h"
#include "tracker.h" 
#include "simvw.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


// various GDI objects we need to draw
static HCURSOR NEAR rghCursors[10];
static HBRUSH NEAR hatchBrush;
static HBITMAP NEAR hatchBitmap;
static HPEN NEAR blackDottedPen;
static int NEAR nHandleSize;


CGridTracker::CGridTracker()
{   
	m_nHandleStyle = TRK_ALL_HANDLES; // do rectangle with all handles by default  
	
	m_nAllowResize = TRUE; 
	
	m_nLeadingMultiple = FALSE; 
	
	m_nLeader = FALSE;
	
	m_pObj = NULL;
	
	Construct();
	
	CRectTracker::CRectTracker();
}                                            

void CGridTracker::DrawTrackerRect(
	LPCRECT lpRect, CWnd* pWndClipTo, CDC* pDC, CWnd* pWnd)
{           
	// This is altered from the standard version to prevent zero-height 
	// tracking rectangles (of horizontal walls) being invisible
	
	// first, normalize the rectangle for drawing
	CRect rect;
	rect.CopyRect(lpRect);
	
	if (rect.top == rect.bottom)
	{
		rect.top += 1;
		rect.bottom -= 1;
	}
	
	rect.NormalizeRect();

	// convert to client coordinates
	if (pWndClipTo != NULL)
	{
		pWnd->ClientToScreen(&rect);
		pWndClipTo->ScreenToClient(&rect);
	}
	// and draw it
	pDC->DrawFocusRect(&rect);
}

void CGridTracker::AdjustRect(int nHandle, LPRECT lpRect)
{
	if (m_nLeader)
	{	
		// will alter rectangle to snap to sim grid if appropriate
		// before calling default; should do this before moving any
		// objs following this one, so they move in gridlike steps too
		// (though individuals not currently on grid will stay offset)
        
        if (m_pSim->m_nUseGrid)
        {
			CPoint point(lpRect->left, lpRect->top);
			((CSimView*) m_pWnd)->GridifyMousePoint(&point);
			lpRect->left = point.x;
			lpRect->top = point.y;
		
			point.x = lpRect->right;
			point.y = lpRect->bottom;
			((CSimView*) m_pWnd)->GridifyMousePoint(&point);
			lpRect->right = point.x;
			lpRect->bottom = point.y;
		}
		
		// Now tell CSObj so that if in multiple selection can
		// get other objs to move trackers too

		m_pObj->UpdateFollowingTrackers(lpRect->left - m_startRect.left,
		                                lpRect->top - m_startRect.top,
		                                lpRect->right - m_startRect.right,
		                                lpRect->bottom - m_startRect.bottom,
		                                m_pWnd);  
    }
    
	CRectTracker::AdjustRect(nHandle, lpRect);
}

void CGridTracker::Construct()
{
	// do one-time initialization if necessary
	static BOOL bInitialized;
	if (!bInitialized)
	{

		if (hatchBitmap == NULL)
		{
			// create the hatch pattern + bitmap
			WORD hatchPattern[8];
			WORD wPattern = 0x11;
			for (int i = 0; i < 4; i++)
			{
				hatchPattern[i] = wPattern;
				hatchPattern[i+4] = wPattern;
				wPattern <<= 1;
			}
			hatchBitmap = CreateBitmap(8, 8, 1, 1, &hatchPattern);
			if (hatchBitmap == NULL)
				AfxThrowMemoryException();
		}

		if (hatchBrush == NULL)
		{
			// create black hatched brush
			hatchBrush = CreatePatternBrush(hatchBitmap);
			if (hatchBrush == NULL)
				AfxThrowMemoryException();
		}

		if (blackDottedPen == NULL)
		{
			// create black dotted pen
			blackDottedPen = CreatePen(PS_DOT, 0, RGB(0, 0, 0));
			if (blackDottedPen == NULL)
				AfxThrowMemoryException();
		}

		// initialize the cursor array
		rghCursors[0] = theApp.LoadCursor(MAKEINTRESOURCE(AFX_IDC_TRACKNWSE));
		rghCursors[1] = theApp.LoadCursor(MAKEINTRESOURCE(AFX_IDC_TRACKNESW));
		rghCursors[2] = rghCursors[0];
		rghCursors[3] = rghCursors[1];
		rghCursors[4] = theApp.LoadCursor(MAKEINTRESOURCE(AFX_IDC_TRACKNS));
		rghCursors[5] = theApp.LoadCursor(MAKEINTRESOURCE(AFX_IDC_TRACKWE));
		rghCursors[6] = rghCursors[4];
		rghCursors[7] = rghCursors[5];
		rghCursors[8] = theApp.LoadCursor(MAKEINTRESOURCE(AFX_IDC_TRACK4WAY));
		rghCursors[9] = theApp.LoadCursor(MAKEINTRESOURCE(AFX_IDC_MOVE4WAY));

		// get default handle size from Windows profile setting
		static char BASED_CODE szWindows[] = "windows";
		static char BASED_CODE szInplaceBorderWidth[] = "oleinplaceborderwidth";
		nHandleSize = GetProfileInt(szWindows, szInplaceBorderWidth,
			5);

		bInitialized = TRUE;
	}

	m_nStyle = 0;
	m_nHandleSize = nHandleSize;
	m_sizeMin.cy = m_sizeMin.cx = nHandleSize*2;
}

UINT CGridTracker::GetHandleMask() const
{
	UINT mask;
	
	switch (m_nHandleStyle)
	{
		case TRK_LINE_ONLY:
			mask = 0x05;    // just 2 corner handles 
			break;
			
		case TRK_CORNERS_ONLY:
			mask = 0x0f;    // just 4 corners
			break;
			
		case TRK_ALL_HANDLES:
			mask = 0xff;   // four corners and middles of edges
			break;
			
		default:
			ASSERT(FALSE);
			mask = 0xff;
	}
	
	return mask;
}

BOOL CGridTracker::Track(int nMultiple, CWnd* pWnd, CPoint point, BOOL bAllowInvert)
{      
	// Overloaded version of MFC original
	
	m_nHaveResized = FALSE;
	
	// try hit testing on the handles
	int nHandle = HitTestHandles(point);
	if (nHandle < 0)
	{
		// didn't hit a handle, so just return FALSE
		return FALSE;
	}
    
    if (nMultiple)
    {                       
    	// Don't allow resizing for multiple selection
    	nHandle = hitMiddle;                          
    	
    	// Note for child functions we are leader
    	m_nLeadingMultiple = TRUE;
    	
    	m_startRect = m_rect;
    } 
    
    if (!m_nAllowResize)
    {                                  
    	// Don't allow resizing
    	nHandle = hitMiddle;                          
    }
    
    m_pWnd = pWnd; // so can tell others to draw in it if necessary
    m_nLeader = TRUE;  // the tracker being tracked is this one
    
	// otherwise, call helper function to do the tracking
	m_bAllowInvert = bAllowInvert;

	m_pObj->PrepareOtherTrackersToFollow();
	
	PrepareToFollow();  // remember own starting position
	
	BOOL ret = TrackHandle(nHandle, pWnd, point, NULL);
	
	if (nHandle == hitMiddle)
	{
		m_nHaveResized = FALSE;
	}
	else
	{    
		m_nHaveResized = m_startRect.Size() != m_rect.Size();
	}
	
	m_nLeadingMultiple = FALSE; // over now
	m_nLeader = FALSE;
	
	return ret;
}

BOOL CGridTracker::SetCursor(CWnd* pWnd, UINT nHitTest) const
{
	return SetCursor(pWnd, nHitTest, FALSE); // assume no multiple selection by default
}

BOOL CGridTracker::SetCursor(CWnd* pWnd, UINT nHitTest, int nMultiple) const
{
	// trackers should only be in client area
	if (nHitTest != HTCLIENT)
		return FALSE;

	// convert cursor position to client co-ordinates
	CPoint point;
	GetCursorPos(&point);
	pWnd->ScreenToClient(&point);

	// do hittest and normalize hit
	int nHandle = HitTestHandles(point);
	if (nHandle < 0)
		return FALSE;

	// need to normalize the hittest such that we get proper cursors
	nHandle = NormalizeHit(nHandle);
    
    if (!m_nAllowResize || nMultiple)
    {
    	 // Don't do arrowey cursors if not supposed to resize obj,
    	 // because of its type or because have multiple selection on screen
    	 
    	 nHandle = hitMiddle;
   	}                        
   	
	// handle special case of hitting area between handles
	//  (logically the same -- handled as a move -- but different cursor)
	if (nHandle == hitMiddle && !m_rect.PtInRect(point))
	{
		// only for trackers with hatchedBorder (ie. in-place resizing)
		if (m_nStyle & hatchedBorder)
			nHandle = (TrackerHit)9;
	}

	ASSERT(nHandle < sizeof(rghCursors)/sizeof(rghCursors[0]));
	::SetCursor(rghCursors[nHandle]);
	return TRUE;
}

int CGridTracker::HitTestHandles(CPoint point) const
{
	CRect rect;
	UINT mask = GetHandleMask();

	// see if hit anywhere inside the tracker
	GetTrueRect(&rect);
	if (!rect.PtInRect(point))
		return hitNothing;  // totally missed

	// see if we hit a handle
	for (int i = 0; i < 8; ++i)
	{
		if (mask & (1<<i))
		{
			GetHandleRect((TrackerHit)i, &rect);
			if (rect.PtInRect(point))
				return (TrackerHit)i;
		}
	}

	// last of all, check for non-hit outside of object, between resize handles
	if ((m_nStyle & hatchedBorder) == 0)
	{
		CRect rect = m_rect;
		rect.NormalizeRect();
		if ((m_nStyle & dottedLine|solidLine) != 0)
			rect.InflateRect(+1, +1);
		if (!rect.PtInRect(point))
			return hitNothing;  // must have been between resize handles
	}
	return hitMiddle;   // no handle hit, but hit object (or object border)
}

void CGridTracker::Draw(CDC* pDC) const
{                                
	// set initial DC state
	VERIFY(pDC->SaveDC() != 0);
	pDC->SetMapMode(MM_TEXT);   // because need comparison with mouse co-ords
	pDC->SetViewportOrg(0, 0);
	pDC->SetWindowOrg(0, 0);

	// get normalized rectangle unless line style, in which case always
	// want top left to be one end and bottom right the other
	CRect rect = m_rect;                                     
	
	if (TRK_LINE_ONLY != m_nHandleStyle)
	{ 
		rect.NormalizeRect();
	}

	// draw resize handles
	if ((m_nStyle & (resizeInside|resizeOutside)) != 0)
	{
		UINT mask = GetHandleMask();
		for (int i = 0; i < 8; ++i)
		{
			if (mask & (1<<i))
			{
				GetHandleRect((TrackerHit)i, &rect);
				pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(),
					BLACKNESS);
			}
		}
	}
    
    VERIFY(pDC->RestoreDC(-1));
}

BOOL CGridTracker::TrackHandle(int nHandle, CWnd* pWnd, CPoint point,
	                                CWnd* pWndClipTo)
{
	ASSERT(nHandle >= 0);
	ASSERT(nHandle <= 8);   // handle 8 is inside the rect

	// don't handle if capture already set
	if (::GetCapture() != NULL)
		return FALSE;

	// save original width & height in pixels
	int nWidth = m_rect.Width();
	int nHeight = m_rect.Height();

	// set capture to the window which received this message
	pWnd->SetCapture();
	ASSERT(pWnd == CWnd::GetCapture());
	CRect rectSave = m_rect;

	// find out what x/y coords we are supposed to modify
	int *px, *py;
	int xDiff, yDiff;
	GetModifyPointers(nHandle, &px, &py, &xDiff, &yDiff);
	xDiff = point.x - xDiff;
	yDiff = point.y - yDiff;

	// get DC for drawing
	CDC* pDrawDC;
	if (pWndClipTo != NULL)
	{
		// clip to arbitrary window by using adjusted Window DC
		pDrawDC = pWndClipTo->GetWindowDC();
		CRect rect;
		pWndClipTo->GetWindowRect(&rect);
		pWndClipTo->ScreenToClient(&rect);
		pDrawDC->SetViewportOrg(-rect.left, -rect.top);
		pWndClipTo->GetClientRect(&rect);
		pDrawDC->IntersectClipRect(0, 0, rect.right, rect.bottom);
	}
	else
	{
		// otherwise, just use normal DC
		pDrawDC = pWnd->GetDC();
	}
	ASSERT_VALID(pDrawDC);

	CRect rectOld;
	BOOL bMoved = FALSE;

	// get messages until capture lost or cancelled/accepted
	for (;;)
	{
		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));

		if (CWnd::GetCapture() != pWnd)
			break;

		switch (msg.message)
		{
		// handle movement/accept messages
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			rectOld = m_rect;
			// handle resize cases (and part of move)
			if (px != NULL)
				*px = (int)(short)LOWORD(msg.lParam) - xDiff;
			if (py != NULL)
				*py = (int)(short)HIWORD(msg.lParam) - yDiff;

			// handle move case
			if (nHandle == hitMiddle)
			{
				m_rect.right = m_rect.left + nWidth;
				m_rect.bottom = m_rect.top + nHeight;
			}
			// allow caller to adjust the rectangle if necessary
			AdjustRect(nHandle, &m_rect);

			// only redraw and callback if the rect actually changed!
			if (!rectOld.EqualRect(&m_rect))
			{
				if (bMoved)
					DrawTrackerRect(&rectOld, pWndClipTo, pDrawDC, pWnd);
				OnChangedRect(rectOld);
				if (msg.message != WM_LBUTTONUP)
					bMoved = TRUE;
			}
			if (WM_LBUTTONUP == msg.message)
			{
				if (bMoved && rectOld.EqualRect(&m_rect))
					DrawTrackerRect(&rectOld, pWndClipTo, pDrawDC, pWnd);
				goto ExitLoop;
			}
			if (!rectOld.EqualRect(&m_rect))
				DrawTrackerRect(&m_rect, pWndClipTo, pDrawDC, pWnd);
			break;

		// handle cancel messages
		case WM_KEYDOWN:
			if (msg.wParam != VK_ESCAPE)
				break;
		case WM_RBUTTONDOWN:
			if (bMoved)
				DrawTrackerRect(&m_rect, pWndClipTo, pDrawDC, pWnd);
			m_rect = rectSave;
			goto ExitLoop;

		// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}

ExitLoop:
	if (pWndClipTo != NULL)
		pWndClipTo->ReleaseDC(pDrawDC);
	else
		pWnd->ReleaseDC(pDrawDC);
	ReleaseCapture();

	// restore rect in case bMoved is still FALSE
	if (!bMoved)
		m_rect = rectSave;

	// return TRUE only if rect has changed
	return !rectSave.EqualRect(&m_rect);
}

void CGridTracker::PrepareToFollow()
{
	m_startRect = m_rect; // remember where began 
	m_nMoved = FALSE;              
}

void CGridTracker::UpdateTrackerAsFollowing(int nDeltaXL, int nDeltaYT, 
                                            int nDeltaXR, int nDeltaYB, CWnd* pWnd)
{
	// Called each time the mouse is moved when we are tagging
	// along in synch with the main selection as it is dragged around 
	
	CRect rectOld = m_rect;
	
	m_rect.left = m_startRect.left + nDeltaXL;
	m_rect.top = m_startRect.top + nDeltaYT;
	m_rect.right = m_startRect.right + nDeltaXR;
	m_rect.bottom = m_startRect.bottom + nDeltaYB;
    
    CDC* pDC = pWnd->GetDC();
	// allow caller to adjust the rectangle if necessary
	AdjustRect(CRectTracker::hitMiddle, &m_rect);

	// only redraw and callback if the rect actually changed!
	if (!rectOld.EqualRect(&m_rect))
	{
		if (m_nMoved)
			DrawTrackerRect(&rectOld, NULL, pDC, pWnd);
		OnChangedRect(rectOld);
		m_nMoved = TRUE;
	}

	if (!rectOld.EqualRect(&m_rect))
		DrawTrackerRect(&m_rect, NULL, pDC, pWnd);

}

