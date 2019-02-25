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

// This file is called "graphx.h" instead of "graph.h" because MSVC 1.5
// mysteriously fails to add the latter to its dependencies in the makefile,
// even though it did #include it. Maybe because there is a 'graph.h' standard
// include file.


// See simobjs.h for generic CObjType details

#define GRAPH_TITLE_PROPN       (0.07)  
#define GRAPH_LABEL_PROPN       (0.05)
#define GRAPH_SCALE_PROPN       (0.05)
#define GRAPH_TICK_LENGTH_PROPN (0.03)
#define GRAPH_EDGE_PLOT_INDENT_PROPN (0.04)
#define GRAPH_EDGE_LABEL_INDENT_PROPN (0.02)   
#define GRAPH_LABEL_PLOT_GAP_PROPN (0.03)  
#define GRAPH_LABEL_SCALE_GAP_PROPN (0.02)     
#define GRAPH_SCALE_TICK_GAP_PROPN (0.008)    

#define GRAPH_MIN_POINT_SIZE    4    

#define GRAPH_MAX_POINTS      1000 // max points allowed to exist on one graph
#define GRAPH_NUM_BUFFER_POINTS 20 // chunk size to remove if getting too big

#define GRAPH_BUTTON_WIDTH      23  // pixels
#define GRAPH_BUTTON_HEIGHT     21        

#define HS_CLEAR     1
#define HS_SAVE      2       

// The following graph symbol styles must agree with
// the order of the corresponding choices in 
// CDrawGraphDlg.

#define GS_CIRCLE    0
#define GS_CRISS     1
#define GS_CROSS     2
#define GS_SQUARE    3
#define GS_TRIANGLE  4

#define MAX_SGRAPH_COMMANDS_PER_OBJ  17

class CObjTypeGraph : public CObjType
{                    
public:       
	CObjTypeGraph(); 

	// Compulsory overrides
	virtual CMD_ENTRY* GetCmdList(); 
	virtual int AcceptCommandLine(CString& remainingLine, CString& errString); 
	virtual int GetStateParam(int command);
	virtual void DrawAllSObjs(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2);   
	virtual void PropertiesCommand();          

	// Optional overrides	
	virtual void OnLButtonUp();
    virtual void Initialise();

	
	static CMD_ENTRY m_celCObjTypeGraphCommands[MAX_COMMANDS_PER_OBJ];

	// When drawing a graph, must remember names of quantities
	// user wishes to plot:	
	CString m_sDrawingGraphXNumeric;
	CString m_sDrawingGraphYNumeric;

	// called at end of each timestep so that we can add data:	
	void UpdateQuantities(); 
};

class CAxis
{                      
public:
	// For each data value used as a graph axis, eg temperature
	CSim* m_pSim;
	
	CNumLink m_numLink;
	double m_dScaleMin;
	double m_dScaleMax;
	
	void ConnectNumeric();   // gets pointers to CNumerics we are plotting
};               

class CGraphPoint
{                
public:
	// For each recorded data point
	
	double m_dRealX;
	double m_dRealY; 
};

class CSObjGraph : public CSObj
{   
public:   
	CSObjGraph();    
	virtual ~CSObjGraph();             
	
	static void DrawPoint(CDC* pDC, int nX, int nY, int nStyle, 
                                       int nSize, int nFilled, COLORREF cColour);
	             
	// need to keep track of what valid and what not:
	union 
	{
		unsigned all;        // all the flags in one go
		struct               // bf for bit flag
		{
		    unsigned xdata:    1; 
		    unsigned ydata:    1;   
   		    unsigned xleft:    1; 
		    unsigned ytop:     1;
		    unsigned xright:   1;
		    unsigned ybottom:  1;   
		    unsigned title:    1;   
			unsigned xlabel:   1;       
            unsigned ylabel:   1; 
            unsigned style:    1;
            unsigned filled:   1;
            unsigned size:     1;
            unsigned red:      1;
            unsigned blue:     1;
            unsigned green:    1;      
		} bf; 
	}
	m_fsValid;       
	
	int CmdStyle(CString& errString);
	
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

	void SaveData(); // saves graphed data to ASCII file

	void ClearData(); // removes all data stored so far and marks bitmap unready
				                           
	static CMD_ENTRY m_celCSObjGraphCommands[MAX_SGRAPH_COMMANDS_PER_OBJ]; 
	
	// for now will have simple XY graph only;
	// pointers to recent data:
	CAxis m_axisX;
	CAxis m_axisY;

	// points being averaged in:
	double m_dXSum;
	double m_dYSum;
	int m_nSamples;   
	double m_dSamplingTimeInterval;
	double m_dSampleStartTime; 

	// called at end of each timestep so that we can add data:	
	void UpdateQuantities(); 
	
	// stored data series
	CPtrArray m_storedData;
	int m_nLastPointPlotted;
	int m_nNoPointsDrawnSinceClear;
	void FindExtremes();
	double m_dSmallestX;
	double m_dLargestX;
	double m_dSmallestY;
	double m_dLargestY;

	// will make round-numbered scale and return labels in CStringArray:	
	CStringArray* FormNiceScale(double& dLowLimit,     // will return value of bottom of new scale
	                            double& dHighLimit,    // will return value of top of new scale
	                            int& nMostChars,       // will return max number of chars amongst returned labels
	                            double dSmallestDatum, // lowest real data to accommodate
	                            double dLargestDatum,  // and highest
	                            int nHeightOneNumeral, // pixels
	                            int nWidthOneNumeral,  // pixels, avg value
	                            int nIsXAxis,          // ie labels side by side not stacked vertically
	                            int nScaleLength);     // pixels, from bottom of scale to end of last scale label
	                  
	// where located on simulation screen
	double m_dXLeft;
	double m_dYTop;
	double m_dXRight;
	double m_dYBottom;   
	
	// the stored bitmap with the graph so far, to which new points can
	// be added without redrawing everything             
    CBitmap* m_pBitmap;                        
    int m_nBitmapReady;     
	void UpdateBitmap(CDC* pSpareDC1, CDC* pSpareDC2); // draws any extra points to get up to date  
	void RedrawBitmap(CDC* pSpareDC1, CDC* pSpareDC2); // redraws from scratch
	CPoint m_ptTopLeft;  // screen co-ords
	CPoint m_ptBottomRight;  
	int m_nScreenWidth;
	int m_nScreenHeight;

	// label properties
	int m_nHasTitle;
	CString m_sTitle;
	int m_nHasXLabel;
	CString m_sXLabel;
	int m_nHasYLabel;
	CString m_sYLabel;

	// point style properties
	int m_nStyle;
	COLORREF m_cPointColour;
	int m_nFilled;
	int m_nSize;
	    
    // screen co-ords of plotting area of graph
	int m_nPlotXLeft;
	int m_nPlotYTop;
	int m_nPlotXRight;
	int m_nPlotYBottom;
	
	// value limits of plotting area of graph
	double m_dMaxPlotX;
	double m_dMinPlotX;
	double m_dMaxPlotY;
	double m_dMinPlotY;   
	
	// hot spots for clear and save
	CHotSpot m_hsClear;
	CHotSpot m_hsSave;
};  
