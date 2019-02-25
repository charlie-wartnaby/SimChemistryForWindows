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

// Overriden version of CRectTracker to do tracking of
// selection rectangles (for moving and resizing).

#define TRK_LINE_ONLY      1
#define TRK_CORNERS_ONLY   2
#define TRK_ALL_HANDLES    3

class CSObj;

class CGridTracker : public CRectTracker
{   
public:   
	CGridTracker();
	
	// do this to allow snap to grid
	virtual void AdjustRect(int nHandle, LPRECT lpRect);   

	// override this to avoid horizontal walls disappearing:	
	void DrawTrackerRect(LPCRECT lpRect, CWnd* pWndClipTo, CDC* pDC, CWnd* pWnd);

	UINT GetHandleMask() const;
	BOOL SetCursor(CWnd* pWnd, UINT nHitTest) const;
	BOOL SetCursor(CWnd* pWnd, UINT nHitTest, int nMultiple) const; // own overload
	int HitTestHandles(CPoint point) const;
	void Draw(CDC* pDC) const; // default uses MM_TEXT
	void Construct();      
	BOOL Track(int nMultiple, CWnd* pWnd, CPoint point, BOOL bAllowInvert = FALSE);	// own overload
	BOOL CGridTracker::TrackHandle(int nHandle, CWnd* pWnd, CPoint point,
	                                           CWnd* pWndClipTo);
	
	// Special properties 
	
	int m_nHandleStyle; // for walls, only want handles at ends
	
	int m_nAllowResize; // not for mols, allowed for graphs etc  
	
	int m_nHaveResized; // whether did when tracking occurred
	
	int m_nLeader;  // whether is the tracker being moved, multiple seln or not
	
	CSObj* m_pObj;      // so can alert obj to tracking moves, so
	                    // that others in multiple seln can move too  
	                    
	int m_nLeadingMultiple; // whether being pushed around by mouse with
	                        // multiple selection shadowing this one  
	                        
	CRect m_startRect;   // where began if shadowing other tracker during
	                     // multiple selection   
	int m_nMoved;        // since started shadowing
	CWnd* m_pWnd;        // for leader to remember
	                     
	void PrepareToFollow();  // called when will be shadowing another tracker,
	                         // gives chance to remember where started off
	                         
	void UpdateTrackerAsFollowing(int nDeltaXL, int nDeltaYT,
	                              int nDeltaXR, int nDeltaYB,
	                              CWnd* pWnd); // when shadowing   
	                              
	CSim* m_pSim;	                                                	                        	                    
}; 