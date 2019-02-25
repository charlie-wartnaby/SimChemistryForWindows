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

// simvw.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSimView view

// m_nFlashState states
#define FS_NOT_FLASHING  1
#define FS_FLASH_HI      2
#define FS_FLASH_LO      3

class CSimView : public CView
{
	DECLARE_DYNCREATE(CSimView)
protected:
	CSimView();			// protected constructor used by dynamic creation

// Attributes
public:
    CDC* m_pMemDC;    // used to prepare screen before display
    CBitmap* m_pBitmap;       
    BOOL m_bInitialised;        
    
    int m_nLeftButtonDown;  // if user is doing selection      
    int m_nDragging;                  
    
    CDC* m_pSpareDC1;  // used for molecule or other object masks  
    CBitmap* m_pSpareBitmap1;
    CDC* m_pSpareDC2;        
    CBitmap* m_pSpareBitmap2; 

	CHotSpot* m_pTrackingHotSpot; // eg if slider knob being moved
	int m_nHotSpotChangedCursor;  // if different cursor last time
	                                
	BOOL m_bWasInClientRect;
	
	int m_nFlashState;  
	UINT m_nFlashTimerID;
	
	void SimifyMousePoint(CPoint point, double* simX, double* simY);    
	void GridifyMousePoint(CPoint* pPoint);
    void DoBitBlt(CRect updateRect, CPaintDC* dc);
    void DoWhite1(CRect updateRect);
    void DoWhite2(CRect updateRect);
    void DoGreyBars(CSim* pSim);
	
// Operations
public:

// Implementation
protected:
	virtual ~CSimView();
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	virtual	void OnDraw(CDC* pDC);		// overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);	

	// Printing support
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// Generated message map functions
protected:
	//{{AFX_MSG(CSimView)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
                                                 
#define MAX_BITMASK_SIZE 1024                                                 
/////////////////////////////////////////////////////////////////////////////
