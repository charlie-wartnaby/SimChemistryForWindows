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

// These constants define the layout of a slider; once all the gaps
// are accounted for, the actual text and drawing should fit the 
// spaces left.

#define SLIDER_TOP_GAP_PROPORTION             (0.05)
#define SLIDER_BOTTOM_GAP_PROPORTION          (0.05)
#define SLIDER_TITLE_SIDE_GAP_PROPORTION      (0.1)  
#define SLIDER_TITLE_HEIGHT_PROPORTION        (0.4) 
#define SLIDER_TITLE_BOTTOM_GAP_PROPORTION    (0.05)
#define SLIDER_CONTROL_LEFT_GAP_PROPORTION    (0.05)
#define SLIDER_NUMTEXT_RIGHT_GAP_PROPORTION   (0.03)
#define SLIDER_NUMTEXT_WIDTH_PROPORTION       (0.3)   
#define SLIDER_NUMTEXT_HEIGHT_PROPORTION      (0.8) // of space under title gap and above bottom gap
#define SLIDER_CONTROL_HEIGHT_PROPORTION      (1.0) // of space under title gap and above bottom gap
#define SLIDER_CONTROL_NUMTEXT_GAP_PROPORTION (0.05)
#define SLIDER_SMALL_TICK_PROPORTION          (0.75) // of height allocated to slidey bit
#define SLIDER_KNOB_WIDTH_PROPORTION          (0.1) // of scale width
#define SLIDER_KNOB_HEIGHT_PROPORTION         (1.0) // of height allocated to slidey bit

#define HS_NUMTEXT     1
#define HS_KNOB        2

class CSObjSlider;

class CObjTypeSlider : public CObjType
{                    
public:       
	CObjTypeSlider(); 

	// Compulsory overrides
	virtual CMD_ENTRY* GetCmdList(); 
	virtual int AcceptCommandLine(CString& remainingLine, CString& errString); 
	virtual int GetStateParam(int command);
	virtual void DrawAllSObjs(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2);   
	virtual void PropertiesCommand();          

	// Optional overrides	
	virtual void OnLButtonUp();
    virtual void Initialise();

    void DoBlt(CDC* pDC,  CDC* pSpareDC1, CSObjSlider* pSlider);

	static CMD_ENTRY m_celCObjTypeSliderCommands[MAX_COMMANDS_PER_OBJ];
};

class CSObjSlider : public CSObj
{   
public:   
	CSObjSlider();    
	virtual ~CSObjSlider();
	             
	// need to keep track of what valid and what not:
	union 
	{
		unsigned all;        // all the flags in one go
		struct               // bf for bit flag
		{
   		    unsigned xleft:    1; 
		    unsigned ytop:     1;
		    unsigned xright:   1;
		    unsigned ybottom:  1;   
		    unsigned title:    1;   
		    unsigned data:     1;  
		    unsigned scalemin: 1;
		    unsigned scalemax: 1;
		} bf; 
	}
	m_fsValid;     
	
	virtual int GetStateParam(int command);
	virtual CMD_ENTRY* GetCmdList(); 
	virtual int AcceptCommandLine(CString& remainingLine, CString& errString);

	virtual int HitTest(double dXLeft, double dYBottom, 
	                           double dXRight, double dYTop);

	virtual void DrawTracker(CDC* pDC);  
	virtual void HandleMove(double dDeltaX, double dDeltaY);
	virtual void OnMoveOrResize(double dNewXLeft, double dNewYTop,
	                            double dNewXRight, double dNewYBottom,
	                            double dOldXLeft, double dOldYTop,
	                            int nResized);
	                            
	virtual void ObjectProperties();      
	
	virtual int HotSpotSetCursor(CHotSpot* pHotSpot, CWnd* pWnd, UINT nHitTest); 
	virtual int OnHotSpotLButtonDown(CHotSpot* pHotSpot, UINT nFlags, CPoint point);
	virtual void OnHotSpotLButtonUp(CHotSpot* pHotSpot, UINT nFlags, CPoint point);		                           
	static CMD_ENTRY m_celCSObjSliderCommands[MAX_COMMANDS_PER_OBJ]; 
	
	// where located on simulation screen
	double m_dXLeft;
	double m_dYTop;
	double m_dXRight;
	double m_dYBottom;   
	
	// the stored bitmap with the graph so far, to which new points can
	// be added without redrawing everything             
    CBitmap* m_pBitmap;       
    CFont* m_pTitleFont; 
    int m_nTitleFontSize;
    CFont* m_pNumTextFont;
    int m_nNumTextFontSize;
                     
    int m_nBitmapReady;  // set to FALSE if complete redraw required
    int m_nValueChanged;  // set to TRUE if slider value changed, partial redraw   
	void UpdateBitmap(CDC* pSpareDC); // draws any extra points to get up to date
	void RedrawBitmap(CDC* pSpareDC); // redraws from scratch

	CPoint m_ptTopLeft;  // screen co-ords
	CPoint m_ptBottomRight;  
	int m_nScreenWidth;
	int m_nScreenHeight;

	// label properties
	int m_nHasTitle;
	CString m_sTitle;    
	
	double m_dValue;  // numeric value of quantity controlled at present,
	                  // or value slid to during transition to new value 
	                  
	CNumLink m_numLink; // used to get at value being tracked
	
	double m_dScaleMin;  // end-stop values
	double m_dScaleMax;
	
	CHotSpot m_hsKnob;     // which user can drag to change value
	CHotSpot m_hsNumText;  // which user can click on to change value  
	
	double m_dTrackingValue; // when sliding knob
	int m_nTrackingKnob;     // whether doing this   
	int m_nScaleMinDeviceKnobXLeft;
	int m_nScaleMaxDeviceKnobXLeft;
};  
