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

#include "stdafx.h"
#include "simobjs.h"      
#include "tracker.h"
#include "sim.h"      
#include "graphx.h"
#include "lennard.h"  
#include "lendoc.h"
#include "drwgphdg.h"
#include <math.h>   




#ifdef _DEBUG
extern long global_debug_counter;
#endif

//////////////////////////////////////////////////////////////////////                                 
// CObjTypeGraph functionality                        
//////////////////////////////////////////////////////////////////////                                 

// constructor          
CObjTypeGraph::CObjTypeGraph()
{                   
	// initialise variables to reasonable state
	
	m_nHasInts = FALSE;
	m_nIsMobile = FALSE; 
}

// Command list
CMD_ENTRY CObjTypeGraph::m_celCObjTypeGraphCommands[MAX_COMMANDS_PER_OBJ] = 
{                                        
	{"name",      CA_STRING, CT_STATE,   CI_NAME,      LIM_MIN_CHARS_OBJTYPE,     LIM_MAX_CHARS_OBJTYPE},
	{"end",       CA_NONE,   CT_ENDLIST, CI_ENDLIST,   0,     0}
};

// Command handlers                            

// Overrides of CObjType

CMD_ENTRY* CObjTypeGraph::GetCmdList()         
{ 
	return &(m_celCObjTypeGraphCommands[0]);
}

int CObjTypeGraph::AcceptCommandLine(CString& remainingLine, CString& errString)
{
	int command;
	int ret;
    
    ret = GrabCommand(remainingLine, errString, command);            
    while ((CI_EMPTY != command) && (SR_ERROR != ret))
    {
		switch (command)
		{   
			case CI_NAME:
				m_sName = m_sArg; 
				break;   

			default:
				ret |= SR_ERROR;
				errString += "confused near: " + remainingLine;
				break;
		}
		
        ret |= GrabCommand(remainingLine, errString, command);
    
	}              
	
	  
	return ret;
}

int CObjTypeGraph::GetStateParam(int command)
{                         
	int ret = SR_OK;
	
	switch(command)
	{
		case CI_NAME:
			m_sArg = m_sName;
			break;     
			
		default:
			TRACE("Asked for param for unknown command in CObjTypeGraph\n");
			ASSERT(FALSE);
			ret = SR_ERROR;
			break;
	}             
	
	return ret;
}

void CObjTypeGraph::DrawAllSObjs(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2)
{                        
	// Each graph draws itself onto its own bitmap, to which
	// new points can be added without redrawing. Here we need
	// only ask each of our graph child objects to get their
	// bitmaps up to date and then blat them onto the display.
	
	CSObjGraph* pGraph;
	pGraph = (CSObjGraph*) m_pFirstSObj;
    
    CBitmap* pOldBitmap = NULL;
	int nFirst = TRUE;
	    
	while (NULL != pGraph)
	{   
		pGraph->UpdateBitmap(pSpareDC1, pSpareDC2);

		if (nFirst)
		{
			pOldBitmap = pSpareDC1->SelectObject(pGraph->m_pBitmap);
			nFirst = FALSE;
		}
		else
		{
			pSpareDC1->SelectObject(pGraph->m_pBitmap);
		}		

		pDC->BitBlt(pGraph->m_ptTopLeft.x, pGraph->m_ptTopLeft.y,
		            pGraph->m_nScreenWidth, -pGraph->m_nScreenHeight,
		            pSpareDC1, 0, 0, SRCAND);  
		            
		pGraph = (CSObjGraph*) pGraph->m_pNextSObj;
	}                
	
	if (NULL != pOldBitmap)
	{    
		pSpareDC1->SelectObject(pOldBitmap);
	}

}   

void CObjTypeGraph::PropertiesCommand()          
{                         
	AfxMessageBox("No type properties to edit for graphs");
}
    
void CObjTypeGraph::OnLButtonUp()
{        
	// If user has dragged substantially, assume they were trying
	// to draw a graph; if only a little bit or not at all and it
	// was the first click, then let them click elsewhere to define
	// the other end of the graph.

	// get rough idea how far dragged in logicals
    
    int callbase = TRUE;
    
	if ((SM_AWAITING_CLICK2 == m_pSim->m_nDrawingState)
	    ||(SM_DRAGGING == m_pSim->m_nDrawingState))
	{
		double xl, xr, yt, yb;
		xl = m_pSim->m_dSimDragStartX;
		yt = m_pSim->m_dSimDragStartY;
		xr = m_pSim->m_dSimMouseX;
		yb = m_pSim->m_dSimMouseY;
			
		CDrawGraphDlg dlg;
		dlg.m_pSim = m_pSim;

		double spare;
		if (xl > xr)
		{
			spare = xr;
			xr = xl;
			xl = spare;
		}
				
		if (yb > yt)
		{
			spare = yt;
			yt = yb;
			yb = spare;
		} 

		dlg.m_dXLeft = xl;
		dlg.m_dXRight = xr;
		dlg.m_dYBottom = yb;
		dlg.m_dYTop = yt;	
			
		if (IDOK == dlg.DoModal())
		{   
			// go ahead and create graph as specified
				
			CString cmd;
				
			cmd = "graph \"";
			cmd += m_sName;
			cmd += "\" ";
				
			cmd += "xdata \"";
			cmd += dlg.m_sX;
			cmd += "\" ";
			cmd += "ydata \"";
			cmd += dlg.m_sY;
			cmd += "\" ";
				
			sprintf(buff, "xleft %f ", dlg.m_dXLeft);
			cmd += buff;
			sprintf(buff, "ytop %f ", dlg.m_dYTop);
			cmd += buff;
			sprintf(buff, "xright %f ", dlg.m_dXRight);
			cmd += buff;
			sprintf(buff, "ybottom %f ", dlg.m_dYBottom);
			cmd += buff; 
				
			if (dlg.m_nCheckTitle)
			{
				cmd += "title \"";
				cmd += dlg.m_sGraphTitle;
				cmd += "\" ";
			}
				
			if (dlg.m_nCheckXLabel)
			{
				cmd += "xlabel \"";
				cmd += dlg.m_sXLabel;
				cmd += "\" ";
			}
	
			if (dlg.m_nCheckYLabel)
			{
				cmd += "ylabel \"";
				cmd += dlg.m_sYLabel;
				cmd += "\" ";
			}
			
			if (dlg.m_nFilled)
			{
				cmd += "filled 1 ";
			}
			else
			{
				cmd += "filled 0 ";
			}
			
			sprintf(buff, "size %d ", dlg.m_nSize);
			cmd += buff;
			
			switch (dlg.m_nStyle)
			{
				case GS_CIRCLE:
					cmd += "style circle ";
					break;
					
				case GS_CRISS:
					cmd += "style criss ";
					break;
					
				case GS_CROSS:
					cmd += "style cross ";
					break;
					
				case GS_SQUARE:
					cmd += "style square ";
					break;
					
				case GS_TRIANGLE:
					cmd += "style triangle ";
					break;

				default:
					cmd += "style criss ";
					ASSERT(FALSE);
					break;					
			}
			
			sprintf(buff, "red %d ", (int) (dlg.m_cPointColour & 0x000000ff));
			cmd += buff;
			sprintf(buff, "green %d ", (int) ((dlg.m_cPointColour & 0x0000ff00) >> 8));
			cmd += buff;
			sprintf(buff, "blue %d ", (int) ((dlg.m_cPointColour & 0x00ff0000) >> 16));
			cmd += buff;                        
			
			m_pSim->HandleNewCommand(cmd); 
		}
		else
		{
			// user cancelled in dialogue, don't create graph
		}
			
		// reset drawing state machine whether making graph or not
		m_pSim->m_nDrawingState = SM_NOT_DRAWING;
		if (SM_SIM_PAUSED == m_pSim->m_nSimState) 
		{
			m_pSim->m_nSimState = SM_SIM_RUNNING;
		}           
  
	}    
	else
	{
		ChangeDrawingStateOnLButtonUp(); // do default
	}
}

void CObjTypeGraph::UpdateQuantities()
{
	// called at end of each timestep; delegate call to
	// each graph of this type we own to let them add data
	
	CSObjGraph* pGraph = (CSObjGraph*) m_pFirstSObj;
	
	while (NULL != pGraph)
	{                                                    
		pGraph->UpdateQuantities();
		pGraph = (CSObjGraph*) pGraph->m_pNextSObj;
	}
}

void CObjTypeGraph::Initialise()
{
    // gets called if sim scaling factor changes etc.

    CSObjGraph* pGraph = (CSObjGraph*) m_pFirstSObj;
	
	while (NULL != pGraph)
	{                                                    
		pGraph->m_nBitmapReady = FALSE; // force redraw when next drawn
		pGraph = (CSObjGraph*) pGraph->m_pNextSObj;
	}

}
    
/////////////////////////////////////////////////////////////////////////
// CSObjGraph functionality
/////////////////////////////////////////////////////////////////////////

CSObjGraph::CSObjGraph()
{    
	// nonvirtual base CSObj constructor will have already been called
	
	m_nNoPointsDrawnSinceClear = TRUE;
	m_pBitmap = NULL;       
	m_nBitmapReady = FALSE;
	m_dXSum = 0;
	m_dYSum = 0;
	m_nSamples = 0;	   
	m_dSamplingTimeInterval = 0.001;
	m_dSampleStartTime = 0;
	m_nLastPointPlotted = -1;
	m_hsSave.m_pOwner = this;
	m_hsSave.m_nId = HS_SAVE;
	m_hsClear.m_pOwner = this;
	m_hsClear.m_nId = HS_CLEAR; 
	m_nHasTitle = FALSE;
	m_nHasXLabel = FALSE;
	m_nHasYLabel = FALSE;
	m_nStyle = GS_CRISS;
	m_cPointColour = 0;
	m_nFilled = FALSE;
	m_nSize = 10;
}

CSObjGraph::~CSObjGraph()
{    
	delete m_pBitmap;   
	
	CGraphPoint* pGP;
	
	for (int i = 0; i <= m_storedData.GetUpperBound(); i++)
	{
		pGP = (CGraphPoint*) m_storedData[i];
		
		delete pGP;
	}
	
	// base CSObj destructor will be called next automatically    
}
	                                    
// Command list
CMD_ENTRY CSObjGraph::m_celCSObjGraphCommands[MAX_SGRAPH_COMMANDS_PER_OBJ] = 
{                      
	{"id",        CA_LONG,   CT_STATE,   CI_ID,      0,      2000000000},                  
	{"xleft",     CA_DOUBLE, CT_STATE,   CI_XLEFT,   LIM_MIN_X,  LIM_MAX_X},
	{"xright",    CA_DOUBLE, CT_STATE,   CI_XRIGHT,  LIM_MIN_X,  LIM_MAX_X},
	{"ytop",      CA_DOUBLE, CT_STATE,   CI_YTOP,    LIM_MIN_Y,  LIM_MAX_Y},
	{"ybottom",   CA_DOUBLE, CT_STATE,   CI_YBOTTOM, LIM_MIN_Y,  LIM_MAX_Y},    
	{"xdata",     CA_STRING, CT_STATE,   CI_XDATA,   LIM_MIN_CHARS_NUMERIC_NAME,     LIM_MAX_CHARS_NUMERIC_NAME},
	{"ydata",     CA_STRING, CT_STATE,   CI_YDATA,   LIM_MIN_CHARS_NUMERIC_NAME,     LIM_MAX_CHARS_NUMERIC_NAME},	
	{"title",     CA_STRING, CT_STATE,   CI_TITLE,   0,     LIM_MAX_GR_TITLE_CHARS   },	
	{"xlabel",    CA_STRING, CT_STATE,   CI_XLABEL,  0,     LIM_MAX_GR_LABEL_CHARS   },	
	{"ylabel",    CA_STRING, CT_STATE,   CI_YLABEL,  0,     LIM_MAX_GR_LABEL_CHARS   },
	{"style",     CA_STRING, CT_STATE,   CI_STYLE,   LIM_MIN_GR_STYLE_NAME, LIM_MAX_GR_STYLE_NAME},
	{"filled",    CA_INT,    CT_STATE,   CI_FILLED,  0,     1   },
	{"size",      CA_INT,    CT_STATE,   CI_SIZE,    LIM_MIN_GR_SYMB_SIZE, LIM_MAX_GR_SYMB_SIZE},
	{"red",       CA_BYTE,   CT_STATE,   CI_RED,     0,     255 },
	{"green",     CA_BYTE,   CT_STATE,   CI_GREEN,   0,     255 },
	{"blue",      CA_BYTE,   CT_STATE,   CI_BLUE,    0,     255 },
	{"end",       CA_NONE,   CT_ENDLIST, CI_ENDLIST, 0,     0   }
};

CMD_ENTRY* CSObjGraph::GetCmdList()
{ 
	return &(m_celCSObjGraphCommands[0]);
}

int CSObjGraph::AcceptCommandLine(CString& remainingLine, CString& errString)
{
	int command;
	int ret;
    
    m_fsValid.all = 0;  // mark nothing as initialised
    m_nBitmapReady = FALSE;
    
    ret = GrabCommand(remainingLine, errString, command);            
    while ((CI_EMPTY != command) && (SR_ERROR != ret))
    {
		switch (command)
		{   
			case CI_ID:
				m_lId = m_lArg; 
				break;   

			case CI_XLEFT:
				m_dXLeft = m_dArg; 
				m_fsValid.bf.xleft = 1;
				break;
			
			case CI_XRIGHT: 
				m_dXRight = m_dArg;
				m_fsValid.bf.xright = 1;
				break;

			case CI_YTOP:
				m_dYTop = m_dArg;      
				m_fsValid.bf.ytop = 1;
				break;
			
			case CI_YBOTTOM: 
				m_dYBottom = m_dArg;   
				m_fsValid.bf.ybottom = 1;
				break; 
			
			case CI_XDATA:
				m_axisX.m_numLink.m_sLinkText = m_sArg;
				m_fsValid.bf.xdata = 1;
				break;            

			case CI_YDATA:
				m_axisY.m_numLink.m_sLinkText = m_sArg;
				m_fsValid.bf.ydata = 1;
				break;            
			
			case CI_TITLE:
				m_sTitle = m_sArg;
				m_fsValid.bf.title = 1;
				break;

			case CI_XLABEL:
				m_sXLabel = m_sArg;
				m_fsValid.bf.xlabel = 1;
				break;

			case CI_YLABEL:
				m_sYLabel = m_sArg;
				m_fsValid.bf.ylabel = 1;
				break;
				
			case CI_STYLE:
				ret |= CmdStyle(errString);
				m_fsValid.bf.style = 1; // cmd handler will insert default if err
				break;
				
			case CI_FILLED:
				m_nFilled = m_nArg;
				m_fsValid.bf.filled = 1;
				break;
				
			case CI_SIZE:
				m_nSize = m_nArg;
				m_fsValid.bf.size = 1;
				break;
				
			case CI_RED:	
				m_cPointColour &= 0x00ffff00;
				m_cPointColour |= m_bArg;
				m_fsValid.bf.red = 1;
				break;
				
			case CI_GREEN:
				m_cPointColour &= 0x00ff00ff;
				m_cPointColour |= (COLORREF) m_bArg << 8;
				m_fsValid.bf.green = 1;
				break;

			case CI_BLUE:
				m_cPointColour &= 0x0000ffff;
				m_cPointColour |= (COLORREF) m_bArg << 16;
				m_fsValid.bf.blue = 1;
				break;
							
			default:
				ret |= SR_ERROR;
				errString += "confused near: " + remainingLine;
				break;
		}
		
        ret |= GrabCommand(remainingLine, errString, command);
    
	}              
	                                                
	                                                
	// further initialisation now we are established
	
	m_axisX.m_pSim = m_pSim;
	m_axisY.m_pSim = m_pSim;    
	
	if (m_fsValid.bf.title)
	{
		m_nHasTitle = (m_sTitle != "");
	}
	else
	{
		m_nHasTitle = FALSE;
		m_sTitle = "";
	}

	if (m_fsValid.bf.xlabel)
	{
		m_nHasXLabel = (m_sXLabel != "");
	}
	else
	{
		m_nHasXLabel = FALSE;
		m_sXLabel = "";
	}
	  
	if (m_fsValid.bf.ylabel)
	{
		m_nHasYLabel = (m_sYLabel != "");
	}
	else
	{
		m_nHasYLabel = FALSE;
		m_sYLabel = "";
	}
    
    if (SR_ERROR != ret)
    {
    	// Fill in any missing data
		if (!m_fsValid.bf.xleft)
		{
			ret |= SR_WARN;
			m_dXLeft = 0;
			errString += "warning: inserted 0 as missing xleft; "; 
			m_fsValid.bf.xleft = 1;
		}

		if (!m_fsValid.bf.xright)
		{
			ret |= SR_WARN;
			m_dXRight = 2;
			errString += "warning: inserted 2 as missing xright; ";
			m_fsValid.bf.xright = 1;
		}

		if (!m_fsValid.bf.ytop)
		{
			ret |= SR_WARN;
			m_dYTop = 2;
			errString += "warning: inserted 2 as missing ytop; ";  
			m_fsValid.bf.ytop = 1;
		}

		if (!m_fsValid.bf.ybottom)
		{
			ret |= SR_WARN;
			m_dYBottom = 0;
			errString += "warning: inserted 0 as missing ybottom; "; 
			m_fsValid.bf.ybottom = 1;
		}    
		
		// Don't insist on style parameters
		if (!m_fsValid.bf.style)
		{
			m_nStyle = GS_CRISS;
			m_fsValid.bf.style = 1;
		}

		if (!m_fsValid.bf.filled)
		{
			m_nFilled = FALSE;
			m_fsValid.bf.filled = 1;
		}

		if (!m_fsValid.bf.size)
		{
			m_nSize = 10;
			m_fsValid.bf.size = 1;
		}

		if (!m_fsValid.bf.red)
		{
			m_cPointColour &= ~0x000000ff; // if miss all colours, will get black
			m_fsValid.bf.red = 1;
		}
		
		if (!m_fsValid.bf.green)
		{
			m_cPointColour &= ~0x0000ff00; // if miss all colours, will get black
			m_fsValid.bf.green = 1;
		}

		if (!m_fsValid.bf.blue)
		{
			m_cPointColour &= ~0x00ff0000; // if miss all colours, will get black
			m_fsValid.bf.blue = 1;
		}
	}
	// register hot spots which will enable cursor changing and clicking
	// on buttons to save data and clear graph (re-registering doesn't matter)
	
	m_pSim->AddHotSpotRef(&m_hsSave);
	m_pSim->AddHotSpotRef(&m_hsClear);

	return ret;
}

int CSObjGraph::CmdStyle(CString& errString)
{
	int ret = SR_WARN;
	
	if (m_sArg == "circle")
	{
		ret = SR_OK;
		m_nStyle = GS_CIRCLE;
	}
	else if (m_sArg == "criss")
	{
		ret = SR_OK;
		m_nStyle = GS_CRISS;
	}
	else if (m_sArg == "cross")
	{
		ret = SR_OK;
		m_nStyle = GS_CROSS;
	}
	else if (m_sArg == "square")
	{
		ret = SR_OK;
		m_nStyle = GS_SQUARE;
	}
	else if (m_sArg == "triangle")
	{
		ret = SR_OK;
		m_nStyle = GS_TRIANGLE;
	}                           
	else
	{
		ret = SR_WARN;
		errString += "style unknown, using criss; ";
		m_nStyle = GS_CRISS;
	}
	
	return ret;
}

int CSObjGraph::GetStateParam(int command)
{                         
	int ret = SR_OK;
	
	switch(command)
	{
		case CI_ID:
			m_lArg = m_lId;
			break;                                

		case CI_XLEFT:
			m_dArg = m_dXLeft;
			break;

		case CI_XRIGHT:
			m_dArg = m_dXRight;
			break;              

		case CI_YTOP:
			m_dArg = m_dYTop;
			break;

		case CI_YBOTTOM:
			m_dArg = m_dYBottom;
			break;

		case CI_XDATA:
			m_sArg = m_axisX.m_numLink.m_sLinkText;
			break;
						
		case CI_YDATA:
			m_sArg = m_axisY.m_numLink.m_sLinkText;
			break;  
		
		case CI_TITLE:
			m_sArg = m_sTitle;
			break;
				
		case CI_XLABEL:
			m_sArg = m_sXLabel;
			break;
						
		case CI_YLABEL:
			m_sArg = m_sYLabel;
			break;
			
		case CI_FILLED:
			m_nArg = m_nFilled;
			break;

		case CI_SIZE:
			m_nArg = m_nSize;
			break;          
			
		case CI_RED:
			m_bArg = (unsigned char) (m_cPointColour & 0x000000ff); 
			break;
			
		case CI_GREEN:
			m_bArg = (unsigned char) ((m_cPointColour >> 8) & 0x000000ff); 
			break;
			
		case CI_BLUE:
			m_bArg = (unsigned char) ((m_cPointColour >> 16) & 0x000000ff); 
			break;                                         
			
		case CI_STYLE:
			switch (m_nStyle)
			{
				case GS_CIRCLE: m_sArg = "circle"; break;
				case GS_CRISS: m_sArg = "criss"; break;
				case GS_CROSS: m_sArg = "cross"; break;
				case GS_SQUARE: m_sArg = "square"; break;
				case GS_TRIANGLE: m_sArg = "triangle"; break;
				default: ASSERT(FALSE); m_sArg = "criss"; break;
			}
			break;
				
						
		default:
			TRACE("Asked for param for unknown command in CSObjGraph\n");
			ASSERT(FALSE);
			ret = SR_ERROR;
			break;
	}             
	
	return ret;
}

void CSObjGraph::DrawPoint(CDC* pDC, int nX, int nY, int nStyle, 
                                       int nSize, int nFilled, COLORREF cColour)
{       
	CPen pen(PS_SOLID, 1, cColour);
				
	CPen* pOldPen = pDC->SelectObject(&pen); 
	
	CBrush brush;
	
	if (nFilled)
	{
		brush.CreateSolidBrush(cColour);
	}
	else
	{
		brush.CreateSolidBrush(RGB(255, 255, 255));
	}
	
	CBrush* pOldBrush = pDC->SelectObject(&brush);
	
	CPoint trianglePoints[3];
	
	switch (nStyle)
	{
		case GS_CIRCLE:
			pDC->Ellipse(nX - nSize / 2, nY + nSize / 2, nX + nSize / 2, nY - nSize / 2);
			break;

		case GS_CRISS:
			pDC->MoveTo(nX - nSize / 2, nY + nSize / 2);
			pDC->LineTo(nX + nSize / 2 + 1, nY - nSize / 2 - 1);
			pDC->MoveTo(nX - nSize / 2, nY - nSize / 2);
			pDC->LineTo(nX + nSize / 2 + 1, nY + nSize / 2 + 1);
			break;
	
		case GS_CROSS:
			pDC->MoveTo(nX, nY - nSize / 2);
			pDC->LineTo(nX, nY + nSize / 2 + 1);
			pDC->MoveTo(nX - nSize / 2, nY);
			pDC->LineTo(nX + nSize / 2 + 1, nY);
			break;
			
		case GS_SQUARE:
			pDC->Rectangle(nX - nSize / 2, nY + nSize / 2, nX + nSize / 2, nY - nSize / 2);
			break;
			
		case GS_TRIANGLE:
			trianglePoints[0] = CPoint((int) (nX - (double) COS30 * nSize / 2), (int) (nY - (double) SIN30 * nSize / 2));
			trianglePoints[1] = CPoint((int) (nX + (double) COS30 * nSize / 2), (int) (nY - (double) SIN30 * nSize / 2));
			trianglePoints[2] = CPoint(nX                                     , nY + nSize / 2);
			pDC->Polygon(trianglePoints, 3);
			break;
			
		default:
			ASSERT(FALSE);
			pDC->MoveTo(nX - nSize / 2, nY + nSize / 2);
			pDC->LineTo(nX + nSize / 2, nY - nSize / 2);
			pDC->MoveTo(nX - nSize / 2, nY - nSize / 2);
			pDC->LineTo(nX + nSize / 2, nY + nSize / 2);
			break;
	}
	
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
}

void CSObjGraph::UpdateBitmap(CDC* pSpareDC1, CDC* pSpareDC2)
{
	CBitmap* pOldBitmap;
	                          
	if (!m_nBitmapReady)
	{                  
		RedrawBitmap(pSpareDC1, pSpareDC2);
	}        
	
	// see if any new points to draw, if so do so	
	int nLast = m_storedData.GetUpperBound();
	
	if (m_nLastPointPlotted < nLast)
	{   
		// we have one or more data points to add to bitmap stored  
		
		pOldBitmap = pSpareDC1->SelectObject(m_pBitmap);


        
        CGraphPoint* pGP;
        
		for (int i = m_nLastPointPlotted + 1; i <= nLast; i++)
		{
			pGP = (CGraphPoint*) m_storedData[i];
			
			double px = (pGP->m_dRealX - m_dMinPlotX) / (m_dMaxPlotX - m_dMinPlotX);
			double py = (pGP->m_dRealY - m_dMinPlotY) / (m_dMaxPlotY - m_dMinPlotY);
			
			if (px >= 0 && px <= 1 && py >= 0 && py <= 1 && !m_nNoPointsDrawnSinceClear)
			{
				// point fits on graph as already drawn
				
				int x = (int) (px * (m_nPlotXRight - m_nPlotXLeft)) + m_nPlotXLeft;
				int y = (int) (py * (m_nPlotYTop - m_nPlotYBottom)) + m_nPlotYBottom;			

				DrawPoint(pSpareDC1, x, y, m_nStyle, m_nSize, m_nFilled, m_cPointColour);
			}
			else
			{
				// new point out of range, have to redraw everything
				RedrawBitmap(pSpareDC1, pSpareDC2);      
				m_nLastPointPlotted = -1;
				i = -1;                    
				m_nNoPointsDrawnSinceClear = FALSE;  // forced redraw after very first
				                                     // point in case fitted in default
				                                     // range     
				
				// DC context supposed to be saved, but have to reselect
				// GDI objs or doesn't work:
				pSpareDC1->SelectObject(m_pBitmap); 
			}
		}
		
		m_nLastPointPlotted = nLast;
	}
}

void CSObjGraph::RedrawBitmap(CDC* pSpareDC1, CDC* pSpareDC2)
{   
	// Save device context as will be selecting various pens etc
	int nSavedDCContext = pSpareDC1->SaveDC();
	ASSERT(nSavedDCContext);
	
	
	// Various coordinates needed only while we are updating labels etc
		
	// visible edge round whole graph
	int nBorderXLeft, nBorderYTop, nBorderXRight, nBorderYBottom;
		
	int nBottomTitleSpace; // where top limit of Y scale labels should begin
	int nRightYLabelSpace; // where left limit of X label and X scale labels begin
	int nTopXLabelSpace;  // where bottom of X scale labels begin
		
	int nXEdgeLabelIndent, nYEdgeLabelIndent; // gap between border and title or axis label
	int nXEdgePlotIndent, nYEdgePlotIndent; // gap between border and graph or scale if no label
	int nLabelPlotGap;     // between title and plot
	int nXLabelScaleGap, nYLabelScaleGap;  // between axis label and scale labels
	int nXScaleTickGap, nYScaleTickGap;   // between scale labels and tick marks that protrude from graph
		
	int nLabelPointSize; // same for both x and y axis labels
		
	int nScalePointSize; // same for both x and y scale labels
		
	int nTickLength; // same for both x and y axes     
		
	int ySize;  // used as temporary to see if x or y smaller several times
		
		
	// First prepare blank bitmap of correct screen size
		
	delete m_pBitmap;   // destroy any existing version
		
	m_pBitmap = new CBitmap;
		
	// Get current screen size from sim units
				
	m_ptTopLeft = m_pSim->SimToLogical(m_dXLeft, m_dYTop); 
	m_ptBottomRight = m_pSim->SimToLogical(m_dXRight, m_dYBottom);
        
    m_nScreenWidth = m_ptBottomRight.x - m_ptTopLeft.x + 1;
    m_nScreenHeight = m_ptTopLeft.y - m_ptBottomRight.y + 1;
        
	m_pBitmap->CreateCompatibleBitmap(pSpareDC1, m_nScreenWidth, m_nScreenHeight);
		
	// bitmap is now ready for use, select into DC to draw onto
		
		
	// Get X and Y quantities
	m_axisX.ConnectNumeric();
	m_axisY.ConnectNumeric();
		
			
	// Paint bitmap white, then add 1-pixel black border around it
		
	pSpareDC1->SelectObject(m_pBitmap);
		
	pSpareDC1->SelectStockObject(WHITE_BRUSH);  
	pSpareDC1->PatBlt(0, 0, m_nScreenWidth + 1, -(m_nScreenHeight + 1), PATCOPY); 
		
        
    pSpareDC1->SetROP2(R2_MASKPEN); // so that writing and lines all superimpose
        
        
	// Calculate and draw border round bitmap within which everything else lies
		
	// Indent in extra pixel or two at right and bottom, otherwise sometimes
	// includes in BitBlt and sometimes doesn't. Don't know why.
		
	nBorderXLeft = 0;
	nBorderYTop = 0;
	nBorderXRight = m_nScreenWidth - 2; // one width->max coord, one spare
	nBorderYBottom = -(m_nScreenHeight - 2); // similarly

	CPen penBorder(PS_SOLID, 1, RGB(0, 0, 0));
	pSpareDC1->SelectObject(&penBorder);
		
	pSpareDC1->MoveTo(nBorderXLeft, nBorderYTop);
	pSpareDC1->LineTo(nBorderXRight, nBorderYTop);
	pSpareDC1->LineTo(nBorderXRight, nBorderYBottom);
	pSpareDC1->LineTo(nBorderXLeft, nBorderYBottom);
	pSpareDC1->LineTo(nBorderXLeft, nBorderYTop);


	// now in a position to calculate absolute indents from edge
		
	nXEdgePlotIndent = (int) ((nBorderXRight - nBorderXLeft) * GRAPH_EDGE_PLOT_INDENT_PROPN);
    nYEdgePlotIndent = (int) ((nBorderYTop - nBorderYBottom) * GRAPH_EDGE_PLOT_INDENT_PROPN);

	// edge-label used for title, x-axis and y-axis labels
	nXEdgeLabelIndent = (int) ((nBorderXRight - nBorderXLeft) * GRAPH_EDGE_LABEL_INDENT_PROPN);
	nYEdgeLabelIndent = (int) ((nBorderYTop - nBorderYBottom) * GRAPH_EDGE_LABEL_INDENT_PROPN);
        
    // label-plot gap, for main title 
    nLabelPlotGap = (int) ((nBorderYTop - nBorderYBottom) * GRAPH_LABEL_PLOT_GAP_PROPN);
        
    // gaps between X and Y axis labels and X and Y scale labels
    nXLabelScaleGap = (int) ((nBorderXRight - nBorderXLeft) * GRAPH_LABEL_SCALE_GAP_PROPN);
	nYLabelScaleGap = (int) ((nBorderYTop - nBorderYBottom) * GRAPH_LABEL_SCALE_GAP_PROPN);
        
    // gaps between scale labels and extremes of tick marks
    nXScaleTickGap = (int) ((nBorderXRight - nBorderXLeft) * GRAPH_SCALE_TICK_GAP_PROPN);
	nYScaleTickGap = (int) ((nBorderYTop - nBorderYBottom) * GRAPH_SCALE_TICK_GAP_PROPN);
        
    // length of tick marks
    nTickLength = (int) ((nBorderXRight - nBorderXLeft) * GRAPH_TICK_LENGTH_PROPN);
	ySize = (int) ((nBorderYTop - nBorderYBottom) * GRAPH_TICK_LENGTH_PROPN);
        
    if (ySize < nTickLength) nLabelPointSize = ySize; 
        
    // Calc point size for X and Y labels, both the same, smallest wins
	nLabelPointSize = (int) ((nBorderXRight - nBorderXLeft) * GRAPH_LABEL_PROPN); 
	ySize = (int) ((nBorderYTop - nBorderYBottom) * GRAPH_LABEL_PROPN);
		
	if (ySize < nLabelPointSize) nLabelPointSize = ySize;
			
	if (nLabelPointSize < GRAPH_MIN_POINT_SIZE) nLabelPointSize = GRAPH_MIN_POINT_SIZE;
			
	// Point size for scale labels (eg 0  100  200...)		
	nScalePointSize = (int) ((nBorderXRight - nBorderXLeft) * GRAPH_SCALE_PROPN); 
	ySize = (int) ((nBorderYTop - nBorderYBottom) * GRAPH_SCALE_PROPN);
		
	if (ySize < nScalePointSize) nScalePointSize = ySize;
			
	if (nScalePointSize < GRAPH_MIN_POINT_SIZE) nScalePointSize = GRAPH_MIN_POINT_SIZE;
		
		
	// Add main title if present
		
	if (m_nHasTitle)
	{		
		int nTitlePointSize = (int) ((nBorderYTop - nBorderYBottom) * GRAPH_TITLE_PROPN); 
			
		if (nTitlePointSize < GRAPH_MIN_POINT_SIZE) nTitlePointSize = GRAPH_MIN_POINT_SIZE;
			
		CFont fontTitle;
			
		fontTitle.CreateFont(-nTitlePointSize, // int nHeight,
	                         0, // int nWidth,
	                         0, // int nEscapement, 
                             0, // int nOrientation, 
                             FW_BOLD, // int nWeight, 
                             FALSE, // BYTE bItalic, 
                             FALSE, // BYTE bUnderline, 
                             0, // BYTE cStrikeOut,
                             ANSI_CHARSET, // BYTE nCharSet, 
                             OUT_TT_PRECIS, // BYTE nOutPrecision, 
                             CLIP_DEFAULT_PRECIS, // BYTE nClipPrecision, 
                             DEFAULT_QUALITY, // BYTE nQuality, 
                             DEFAULT_PITCH | TMPF_TRUETYPE, // BYTE nPitchAndFamily,  
                             "Arial"); // LPCSTR lpszFacename
			
		pSpareDC1->SelectObject(&fontTitle);
			
		// centre title within border 
	    pSpareDC1->SetTextAlign(TA_CENTER | TA_TOP);
	        
		pSpareDC1->TextOut((nBorderXLeft + nBorderXRight) / 2,
		                   nBorderYTop - nYEdgeLabelIndent,
		                   m_sTitle);
			
		CSize sizeTitle = pSpareDC1->GetTextExtent(m_sTitle, m_sTitle.GetLength());
			
		nBottomTitleSpace = nBorderYTop - nYEdgeLabelIndent - sizeTitle.cy - nLabelPlotGap;
	}
	else
	{
		// if no title, leave only room for buttons
		if (nYEdgePlotIndent > GRAPH_BUTTON_HEIGHT)
		{
			nBottomTitleSpace = nBorderYTop - nYEdgePlotIndent;
		}
		else
		{
			nBottomTitleSpace = nBorderYTop - GRAPH_BUTTON_HEIGHT;
		}
	}   	
		
		
		
	// Next do Y-axis label, which X-label space will begin from (ie wins bottom left corner)
		
	if (m_nHasYLabel)
	{
		CFont fontLabel;
			
		fontLabel.CreateFont(-nLabelPointSize, // int nHeight,
	                         0, // int nWidth,
	                         270 * 10, // int nEscapement, want 90 degrees from horizontal
                             0, // int nOrientation,
                             FW_BOLD, // int nWeight, 
                             FALSE, // BYTE bItalic, 
                             FALSE, // BYTE bUnderline, 
                             0, // BYTE cStrikeOut,
                             ANSI_CHARSET, // BYTE nCharSet, 
                             OUT_TT_PRECIS, // BYTE nOutPrecision, 
                             CLIP_DEFAULT_PRECIS, // BYTE nClipPrecision, 
                             DEFAULT_QUALITY, // BYTE nQuality, 
                             DEFAULT_PITCH | TMPF_TRUETYPE, // BYTE nPitchAndFamily,  
                             "Arial"); // LPCSTR lpszFacename
			
		pSpareDC1->SelectObject(&fontLabel);
			
		// align centred vertical, from left edge horizontal 
	    pSpareDC1->SetTextAlign(TA_LEFT | TA_TOP);
                                                                                         
		// note this gets rectangular size as if right way up, ie x is bigger than y                                                                                         
		CSize sizeLabel = pSpareDC1->GetTextExtent(m_sYLabel, m_sYLabel.GetLength());

		// similarly TextOut means left of text in orientation of text, not on screen
		pSpareDC1->TextOut(nBorderXLeft + nXEdgeLabelIndent,
		                   nBorderYBottom + (nBorderYTop - nBorderYBottom - sizeLabel.cx) / 2,
		                   m_sYLabel);
			
	    nRightYLabelSpace = nBorderXLeft + nXEdgeLabelIndent 
	                            + sizeLabel.cy + nXLabelScaleGap;
	}
	else
	{
		// no Y label
		nRightYLabelSpace = nXEdgePlotIndent;
	}


	// Next do X-axis label, the space having been defined by Y label
		
	if (m_nHasXLabel)
	{
		CFont fontLabel;
			
		fontLabel.CreateFont(-nLabelPointSize, // int nHeight,
	                         0, // int nWidth,
	                         0, // int nEscapement, 
                             0, // int nOrientation,
                             FW_BOLD, // int nWeight, 
                             FALSE, // BYTE bItalic, 
                             FALSE, // BYTE bUnderline, 
                             0, // BYTE cStrikeOut,
                             ANSI_CHARSET, // BYTE nCharSet, 
                             OUT_TT_PRECIS, // BYTE nOutPrecision, 
                             CLIP_DEFAULT_PRECIS, // BYTE nClipPrecision, 
                             DEFAULT_QUALITY, // BYTE nQuality, 
                             DEFAULT_PITCH | TMPF_TRUETYPE , // BYTE nPitchAndFamily,  
                             "Arial"); // LPCSTR lpszFacename
			
		pSpareDC1->SelectObject(&fontLabel);
			
		// align centred 
	    pSpareDC1->SetTextAlign(TA_CENTER | TA_BOTTOM);
                                                                                         
		CSize sizeLabel = pSpareDC1->GetTextExtent(m_sXLabel, m_sXLabel.GetLength());

		pSpareDC1->TextOut((nRightYLabelSpace + nBorderXRight - nXEdgeLabelIndent) / 2,
		                  nBorderYBottom + nYEdgeLabelIndent,
		                  m_sXLabel); 
			
		nTopXLabelSpace = nBorderYBottom + nYEdgeLabelIndent 
		                      + sizeLabel.cy + nYLabelScaleGap;
	}
	else
	{
		// no X label   
			
		nTopXLabelSpace =  nBorderYBottom + nYEdgePlotIndent;
	}


	// Now calc 'nice' scales with about the right number of breakpoints
	// to fit the data we have stored so far (assuming we have any)
		
	// The horizontal extent of the y scale labels may scrunch the graph up to
	// some extent in width, but on the other hand the x labels are of known
	// height; so work out y scale first, and then x
        
    FindExtremes();         
        
    // will need to know size of font to get spacing of labels right...

	CFont fontScale;
			
	fontScale.CreateFont(-nScalePointSize, // int nHeight,
                         0, // int nWidth,
                         0, // int nEscapement, 
                         0, // int nOrientation,
                         FW_NORMAL, // int nWeight, 
                         FALSE, // BYTE bItalic, 
                         FALSE, // BYTE bUnderline, 
                         0, // BYTE cStrikeOut,
                         ANSI_CHARSET, // BYTE nCharSet, 
                         OUT_TT_PRECIS, // BYTE nOutPrecision, 
                         CLIP_DEFAULT_PRECIS, // BYTE nClipPrecision, 
                         DEFAULT_QUALITY, // BYTE nQuality, 
                         DEFAULT_PITCH | TMPF_TRUETYPE, // BYTE nPitchAndFamily,  
                         "Arial"); // LPCSTR lpszFacename
			
	pSpareDC1->SelectObject(&fontScale);
			
	// get size in pixels of a typical number in this font
	// (a bit approx I know for proportional fonts)                                                                                        
	CSize sizeExample = pSpareDC1->GetTextExtent("0123456789", 10); 
		
	// Can now work out where top and bottom of plot should be!
	m_nPlotYBottom = nTopXLabelSpace + sizeExample.cy + nYScaleTickGap + nTickLength;

    // Knowing that can calc nice spacing for y scale
        
    int nMostChars;  // will get max number of chars in any label
        
    CStringArray* pYLabels = FormNiceScale(m_dMinPlotY,         // double& dLowLimit,
                                           m_dMaxPlotY,         // double& dHighLimit,   
                                           nMostChars,          // int& nMostChars,
                                           m_dSmallestY,        // double dSmallestDatum,
                                           m_dLargestY,         // double dLargestDatum,
                                           sizeExample.cy,      // int nHeightOneNumeral,
                                           sizeExample.cx / 10, // int nWidthOneNumeral,
                                           FALSE,               // int nIsXAxis,
                                           nBottomTitleSpace - m_nPlotYBottom); // int nScaleLength);
        
        
    // Now also know position of y axis that leaves room for axis labels
    // (but RHS depends on length of last label on x axis)
        
    m_nPlotXLeft = nRightYLabelSpace + nMostChars * sizeExample.cx / 10 
                    + nXScaleTickGap + nTickLength;

	m_nPlotYTop = nBottomTitleSpace - sizeExample.cy / 2; // as top y scale label centred on it

    // Now we know how much width we have left for the x-axis scale labels
        
    CStringArray* pXLabels = FormNiceScale(m_dMinPlotX,         // double& dLowLimit,
                                           m_dMaxPlotX,         // double& dHighLimit,   
                                           nMostChars,          // int& nMostChars,
                                           m_dSmallestX,        // double dSmallestDatum,
                                           m_dLargestX,         // double dLargestDatum,
                                           sizeExample.cy,      // int nHeightOneNumeral,
                                           sizeExample.cx / 10, // int nWidthOneNumeral,
                                           TRUE,                // int nIsXAxis,
                                           nBorderXRight - nXEdgePlotIndent - m_nPlotXLeft); // int nScaleLength);


	int nXLabels = pXLabels->GetSize();  

	// indent edge of plot such that rightmost scale label can be centred on it        
 	m_nPlotXRight = nBorderXRight - nXEdgePlotIndent 
 	                 - pXLabels->GetAt(nXLabels - 1).GetLength() * sizeExample.cx / 20;
        
	// right align text for y-axis labels 
    pSpareDC1->SetTextAlign(TA_RIGHT | TA_BOTTOM);
		
	int nYLabels = pYLabels->GetSize();  

	CPen tickPen(PS_SOLID, 1, RGB(0, 0, 0));
		
	pSpareDC1->SelectObject(&tickPen); 		      
    
    int i;
    for (i = 0; i < nYLabels; i++)
    {   
    	int nYTick = (int) ((double) i / (nYLabels - 1) * (m_nPlotYTop - m_nPlotYBottom)) + m_nPlotYBottom;

    	pSpareDC1->TextOut(m_nPlotXLeft - nTickLength - nXScaleTickGap,
    	                  nYTick - sizeExample.cy / 2,
    	                  pYLabels->GetAt(i));
        	                  
    	pSpareDC1->MoveTo(m_nPlotXLeft - nTickLength - 1, nYTick);
    	pSpareDC1->LineTo(m_nPlotXRight, nYTick);   
    }
        
    delete pYLabels;
        
	// centre align text for x-axis labels 
    pSpareDC1->SetTextAlign(TA_CENTER | TA_BOTTOM);

    for (i = 0; i < nXLabels; i++)
    {   
    	int nXTick = (int) ((double) i / (nXLabels - 1) * (m_nPlotXRight - m_nPlotXLeft)) + m_nPlotXLeft;

    	pSpareDC1->TextOut(nXTick,
    	                  nTopXLabelSpace,
    	                  pXLabels->GetAt(i));
        	                  
    	pSpareDC1->MoveTo(nXTick, m_nPlotYTop);
    	pSpareDC1->LineTo(nXTick, m_nPlotYBottom - nTickLength);   
    }
        
    delete pXLabels;       
    
    
    // Add bitmap button hotspots to clear graph data or save it to disk
    
    CBitmap bmpSave;
    bmpSave.LoadBitmap(IDB_FILE_SAVE);
    CBitmap* pOldBitmap = pSpareDC2->SelectObject(&bmpSave);

	// Get dimensions of a button in logicals    
    CPoint ptBRButton(GRAPH_BUTTON_WIDTH, GRAPH_BUTTON_HEIGHT);
    pSpareDC1->DPtoLP(&ptBRButton);

    m_hsSave.m_rect.right = nBorderXRight - 2;
    m_hsSave.m_rect.left = m_hsSave.m_rect.right - ptBRButton.x;
    m_hsSave.m_rect.top = nBorderYTop - 2;
    m_hsSave.m_rect.bottom = m_hsSave.m_rect.top + ptBRButton.y;
    
    pSpareDC1->BitBlt(m_hsSave.m_rect.left, m_hsSave.m_rect.top,
                      ptBRButton.x, ptBRButton.y,
                      pSpareDC2,
                      0, 0,
                      SRCCOPY);
    
	// And the same for the clear graph button                      
    
    CBitmap bmpClear;
    bmpClear.LoadBitmap(IDB_CLR_GRAPH);
    pSpareDC2->SelectObject(&bmpClear);

    m_hsClear.m_rect.right = m_hsSave.m_rect.left - 1;
    m_hsClear.m_rect.left = m_hsClear.m_rect.right - ptBRButton.x;
    m_hsClear.m_rect.top = nBorderYTop - 2;
    m_hsClear.m_rect.bottom = m_hsClear.m_rect.top + ptBRButton.y;
    
    pSpareDC1->BitBlt(m_hsClear.m_rect.left, m_hsClear.m_rect.top,
                      ptBRButton.x, ptBRButton.y,
                      pSpareDC2,
                      0, 0,
                      SRCCOPY);         
    
	pSpareDC2->SelectObject(pOldBitmap);
    
    // Now convert co-ords of these buttons to device coords for
    // comparison with mouse position data later:
                      
    m_hsSave.m_rect += m_ptTopLeft; 
    m_hsClear.m_rect += m_ptTopLeft;
    
    pSpareDC1->LPtoDP(&m_hsSave.m_rect);                      
    pSpareDC1->LPtoDP(&m_hsClear.m_rect);
    
    
    
	// The graph is now ready to add points to as we go along without having to
	// redraw the labels or axes etc

	pSpareDC1->RestoreDC(nSavedDCContext);
				
	m_nBitmapReady = TRUE;                                    
	m_nLastPointPlotted = -1; // none now plotted
}

CStringArray* CSObjGraph::FormNiceScale
(
	double& dLowLimit,     // will return value of bottom of new scale
	double& dHighLimit,    // will return value of top of new scale
	int& nMostChars,       // will return max number of chars amongst returned labels
	double dSmallestDatum, // lowest real data to accommodate
	double dLargestDatum,  // and highest
	int nHeightOneNumeral, // pixels
	int nWidthOneNumeral,  // pixels, avg value
	int nIsXAxis,          // ie labels side by side not stacked vertically
	int nScaleLength     // pixels
)
{
	CStringArray* pReturnStrings = new CStringArray;
	
	const int nDivider[] = {1, 2, 5, 10, 20, 50, 100, 200};


	// Find what power of 10 will contain the range required
	int nLog; 
	
	// can label min and max (as round numbers) for range not spanning
	// zero, but need to include zero for range that does span it
	int nMinPoints;
	
	// include zero in range if nearby
	if (dSmallestDatum > NEARLY_ZERO && dLargestDatum > NEARLY_ZERO)
	{
		// zero not straddled and both positive
		
		if (dSmallestDatum / dLargestDatum < 0.4) dSmallestDatum = 0;
		
		nLog = (int) ceil(log10(dLargestDatum));
		
		nMinPoints = 2;   
	}
	else if (dSmallestDatum < -NEARLY_ZERO && dLargestDatum < -NEARLY_ZERO)
	{
		// zero not straddled and both negative
		
		if (dSmallestDatum / dLargestDatum < 0.4) dLargestDatum = 0;
		
		nLog = (int) ceil(log10(fabs(dSmallestDatum)));
		
		nMinPoints = 2;
	}                                                     
	else
	{
		// one or both of them already zero, or zero straddled
		
		if ((fabs(dSmallestDatum) < NEARLY_ZERO) && (fabs(dLargestDatum) < NEARLY_ZERO))
		{
			// invent proper range as both zero or thereabouts
			
			dSmallestDatum = -NEARLY_ZERO;
			dLargestDatum = NEARLY_ZERO;           
			
			nLog = (int) ceil(log10(dLargestDatum));   
			
			nMinPoints = 3;
		}   
		else if (fabs(dSmallestDatum) < NEARLY_ZERO)
		{
			// get magnitude from upper lim as lower one near-zero
			nLog = (int) ceil(log10(fabs(dLargestDatum)));
			
			nMinPoints = 2;
		}
		else if (fabs(dLargestDatum) < NEARLY_ZERO)
		{
			// get magnitude from lower lim as upper one near-zero
			nLog = (int) ceil(log10(fabs(dSmallestDatum)));
			
			nMinPoints = 2;
		}      
		else
		{
			// zero straddled by finite limits
			int nLowLog = (int) ceil(log10(fabs(dSmallestDatum)));
			int nHighLog = (int) ceil(log10(fabs(dLargestDatum)));   

			if (nLowLog > nHighLog)
			{
				nLog = nLowLog;
			}
			else
			{
				nLog = nHighLog;
			}            
			
			nMinPoints = 3;
        }
	}

	
	double dBigPower = pow(10, nLog);
	
	// Starting from the max number of possible tick increments,
	// try fewer and fewer until the labels fit nicely
	
	int nDivIndex = sizeof(nDivider) / sizeof(int) - 1;
	
	double dIncrement; // value interval between ticks                  
	int nPoints; // number of tick breakpoints
	int nFits; // whether text labels will not overlap
	int nUpperMult;  // multiplier of dIncrement giving biggest scale value
	int nLowerMult;
	
	do
	{
		dIncrement = dBigPower / nDivider[nDivIndex];
		
		nUpperMult = (int) ceil(dLargestDatum / dIncrement);
		nLowerMult = (int) floor(dSmallestDatum / dIncrement);
		
		if (nLowerMult == nUpperMult)
		{
			// Unlucky! Range of one number that hits a breakpoint too.
			
			nLowerMult--;
			nUpperMult++;
		}
		
		nPoints = nUpperMult - nLowerMult + 1;
	    
	    if (nPoints <= 21)
	    {
			pReturnStrings->RemoveAll(); 
			
			nMostChars = 0;
			int nCharLen;
			
			for (int i = nLowerMult; i <= nUpperMult; i++)
			{         
				sprintf(buff, "%g", (double) i * dIncrement);
				pReturnStrings->Add(buff);
				nCharLen = strlen(buff);
				
				if (nCharLen > nMostChars) nMostChars = nCharLen;
			}
			
			if (nIsXAxis)
			{
				// Would two labels of max length, put next to each
				// other on scale, still be reasonably separated?
				
				nFits = (double) nScaleLength / (nPoints - 1) 
				                  / (nMostChars * nWidthOneNumeral) > 1.5;
			}	                                                           
			else
			{
				// Y-axis case: would labels at this separation stacked
				// vertically be reasonably separated?
				
				nFits = (double) nScaleLength / (nPoints - 1)
				                  / nHeightOneNumeral > 1.5;
			}
		}
		else
		{
			// too many points even if writing does fit
			
			nFits = FALSE;
		}
			
		nDivIndex--;
	}
	while (!nFits && (nPoints > nMinPoints) && (nDivIndex >= 0));
	
	dLowLimit = (double) nLowerMult * dIncrement;	
	dHighLimit = (double) nUpperMult * dIncrement;
	
	return pReturnStrings;
}

void CSObjGraph::FindExtremes()
{       
    int nLast = m_storedData.GetUpperBound();
    
    if (nLast >= 0)
    {
    	// there is some stored data so find minima and maxima

		m_dSmallestX = NEARLY_INFINITY;
		m_dLargestX = -NEARLY_INFINITY;
		m_dSmallestY = NEARLY_INFINITY;
		m_dLargestY = -NEARLY_INFINITY;

	    CGraphPoint* pGP;
    	    
		for (int i = 0; i <= nLast; i++)
		{		
			pGP = (CGraphPoint*) m_storedData[i];
				
			if (pGP->m_dRealX < m_dSmallestX)
			{
				m_dSmallestX = pGP->m_dRealX;
			}
			
			if (pGP->m_dRealX > m_dLargestX)
			{
				m_dLargestX = pGP->m_dRealX;
			}
	
			if (pGP->m_dRealY < m_dSmallestY)
			{
				m_dSmallestY = pGP->m_dRealY;
			}
			
			if (pGP->m_dRealY > m_dLargestY)
			{
				m_dLargestY = pGP->m_dRealY;
			}
		}
	}
	else
	{
		// no stored data, put in default values
		
		m_dSmallestX = 0;
		m_dLargestX = 1;
		m_dSmallestY = 0;
		m_dLargestY = 1;
	}
}

void CAxis::ConnectNumeric()
{
	if (m_numLink.m_nPointerValid)
	{
		// should already be connected
		ASSERT(m_numLink.m_pNumeric != NULL);  
		
		// link may have changed, so disconnect
		m_numLink.m_pNumeric->RemoveRef(&m_numLink);
		m_numLink.m_pNumeric = NULL;
		m_numLink.m_nPointerValid = FALSE;
	}

	// try and connect to CNumeric by name
	m_numLink.m_pNumeric = m_pSim->FetchNumeric(m_numLink.m_sLinkText);  
		
	if (NULL != m_numLink.m_pNumeric)
	{
		// was found OK
		m_numLink.m_nPointerValid = TRUE;   
			
		// numeric should know about us in case of destruction
		m_numLink.m_pNumeric->m_refList.AddHead(&m_numLink);
	}
}

// Command handlers                            


// CSObjGraph specific functionality
void CSObjGraph::UpdateQuantities()
{
	// Called at end of each timestep to allow us to
	// accumulate data
	
	if (m_pSim->m_dSimTime >= m_dSampleStartTime + m_dSamplingTimeInterval)
	{
		// make a new point from the data averaged in so far (if any)
		
		CGraphPoint* pGP;

		// Delete oldest existing points, if necessary, to avoid getting
		// infinitely large number of points
		
		if (m_storedData.GetSize() >= GRAPH_MAX_POINTS)
		{                                             
			for (int i = 0; i < GRAPH_NUM_BUFFER_POINTS; i++)
			{
				pGP = (CGraphPoint*) m_storedData[i];
				delete pGP;
			}
			
			m_storedData.RemoveAt(0, GRAPH_NUM_BUFFER_POINTS);	
			
			m_nBitmapReady = FALSE;
		}
		
		
		// Then add new point		
		if (m_nSamples > 0)
		{		
			pGP = new CGraphPoint;
			
			pGP->m_dRealX = m_dXSum / (double) m_nSamples;
			pGP->m_dRealY = m_dYSum / (double) m_nSamples;
	
			m_storedData.Add(pGP);
		}	
		

		m_dXSum = 0;
		m_dYSum = 0;
		m_nSamples = 0;
		
		m_dSampleStartTime += m_dSamplingTimeInterval;
		
		if (m_pSim->m_dSimTime - m_dSampleStartTime > m_dSamplingTimeInterval)
		{
			// sampling seems to have got rather behind-hand (might just
			// be because graph drawn when sim underway)
			
			m_dSampleStartTime = floor(m_pSim->m_dSimTime / m_dSamplingTimeInterval)
			                         * m_dSamplingTimeInterval;
		}
	}        
	
	if ((m_axisX.m_numLink.m_nPointerValid) && (m_axisY.m_numLink.m_nPointerValid))
	{
		m_dXSum += *(m_axisX.m_numLink.m_pNumeric->m_pdValue);
		m_dYSum += *(m_axisY.m_numLink.m_pNumeric->m_pdValue);
		
		m_nSamples++;		
	}
}

int CSObjGraph::HitTest(double dXLeft, double dYBottom,
                          double dXRight, double dYTop)
{
	return RectsOverlap(dXLeft, dYBottom, dXRight, dYTop,
                         m_dXLeft, m_dYBottom, m_dXRight, m_dYTop);
}
                
void CSObjGraph::ObjectProperties()
{                   
	CDrawGraphDlg dlg;
	dlg.m_pSim = m_pSim;

	dlg.m_dXLeft = m_dXLeft;
	dlg.m_dXRight = m_dXRight;
	dlg.m_dYBottom = m_dYBottom;
	dlg.m_dYTop = m_dYTop;
	
	dlg.m_sX = m_axisX.m_numLink.m_sLinkText;
	dlg.m_sY = m_axisY.m_numLink.m_sLinkText;
	
	dlg.m_nCheckTitle = m_nHasTitle;
	dlg.m_sGraphTitle = m_sTitle;
	dlg.m_nCheckXLabel = m_nHasXLabel;
	dlg.m_sXLabel = m_sXLabel;	
	dlg.m_nCheckYLabel = m_nHasYLabel;
	dlg.m_sYLabel = m_sYLabel;
	
	dlg.m_nStyle = m_nStyle;
	dlg.m_cPointColour = m_cPointColour;
	dlg.m_nSize = m_nSize;
	dlg.m_nFilled = m_nFilled;
			
	if (IDOK == dlg.DoModal())
	{   
		// modify ourselves to new properties, and tell sim
		// to make us change to that new state
		
		m_dXLeft = dlg.m_dXLeft;
		m_dXRight = dlg.m_dXRight;
		m_dYBottom = dlg.m_dYBottom;
		m_dYTop = dlg.m_dYTop;
		
		m_axisX.m_numLink.m_sLinkText = dlg.m_sX;
		m_axisY.m_numLink.m_sLinkText = dlg.m_sY;
		
		if (dlg.m_nCheckTitle)
		{
			m_nHasTitle = TRUE;
			m_sTitle = dlg.m_sGraphTitle;
		}
		else
		{
			m_nHasTitle = FALSE;
			m_sTitle = "";
		}

		if (dlg.m_nCheckXLabel)
		{
			m_nHasXLabel = TRUE;
			m_sXLabel = dlg.m_sXLabel;
		}
		else
		{
			m_nHasXLabel = FALSE;
			m_sXLabel = "";
		}

		if (dlg.m_nCheckYLabel)
		{
			m_nHasYLabel = TRUE;
			m_sYLabel = dlg.m_sYLabel;
		}
		else
		{
			m_nHasYLabel = FALSE;
			m_sYLabel = "";
		}

		m_nFilled = dlg.m_nFilled;
		m_nSize = dlg.m_nSize;
		m_nStyle = dlg.m_nStyle;
		m_cPointColour = dlg.m_cPointColour;
				
		CString cmd;
				
		cmd = "change graph \"";
		cmd += m_pType->m_sName;
		cmd += "\" ";
		
		WriteState(cmd);		

		m_pSim->HandleNewCommand(cmd); 
	}
	else
	{
		// user cancelled in dialogue, don't create graph
	}

}


void CSObjGraph::DrawTracker(CDC* pDC)
{            
	// Need to supply tracker coordinates in device co-ords for comparison
	// to mouse pointer coords
    
    // get logical co-ords
	CPoint pointTopLeft = m_pSim->SimToLogical(m_dXLeft, m_dYTop);  
	CPoint pointBottomRight = m_pSim->SimToLogical(m_dXRight, m_dYBottom);  
	
    // get rectangle in logicals
    CRect rect;
	rect.left = pointTopLeft.x;
	rect.top = pointTopLeft.y;
	rect.right = pointBottomRight.x;
	rect.bottom = pointBottomRight.y;

	// now convert to device co-ords
	pDC->LPtoDP(&rect);
	rect.NormalizeRect();
	
	m_pTracker->m_rect = rect;
	
	m_pTracker->m_nStyle = CRectTracker::resizeOutside;
		
	m_pTracker->Draw(pDC);
}

void CSObjGraph::HandleMove(double dDeltaX, double dDeltaY)
{
	m_dXLeft += dDeltaX;
	m_dXRight += dDeltaX;
	m_dYTop += dDeltaY;
	m_dYBottom += dDeltaY;
}

void CSObjGraph::OnMoveOrResize(double dNewXLeft, double dNewYTop,
                                  double dNewXRight, double dNewYBottom,
	                              double dOldXLeft, double dOldYTop,
	                              int nResized)
{  
	CString cmd("change graph \"");
	cmd += m_pType->m_sName; 
	cmd += "\" ";
	
	m_dXLeft = dNewXLeft;
	m_dXRight = dNewXRight;
	m_dYTop = dNewYTop;
	m_dYBottom = dNewYBottom; 

    WriteState(cmd); 
	    
    m_pSim->HandleNewCommand(cmd);	
}
	                              
int CSObjGraph::HotSpotSetCursor(CHotSpot* pHotSpot, CWnd* pWnd, UINT nHitTest)
{
	// Gist of this copied from CRectTracker (see tracker.cpp)
	
	// trackers should only be in client area
	if (nHitTest != HTCLIENT)
		return FALSE;

	// convert cursor position to client co-ordinates
	CPoint point;
	GetCursorPos(&point);
	pWnd->ScreenToClient(&point);
    
    // Is point over this hotspot?
    if (pHotSpot->m_rect.PtInRect(point))
    {
    	// If so set new cursor
    	
    	HCURSOR hCursor;
    	
    	switch (pHotSpot->m_nId)
    	{ 
    		case HS_SAVE:
    		case HS_CLEAR:
    			hCursor = theApp.LoadCursor(AFX_ID_HAND);
    			break;

    		default:
    			ASSERT(FALSE);
    			hCursor = NULL;
    			break;
    	}
    	
    	if (NULL != hCursor)
    	{
    		::SetCursor(hCursor);
    		return TRUE;
    	}
    	else
    	{
    		ASSERT(FALSE);
    		return FALSE;
    	}
    }
    else
    {
    	return FALSE;
    }

}

int CSObjGraph::OnHotSpotLButtonDown(CHotSpot* pHotSpot, UINT nFlags, CPoint point)
{
	int ret;
	CString cmd;
	
	switch (pHotSpot->m_nId)
	{ 
		case HS_SAVE: 
			SaveData();
			ret = HSLBD_ONE_OFF_ACTION_DONE;
			break;
    			
		case HS_CLEAR:    
			cmd = "clear_graph \"";
			cmd += m_pType->m_sName;
			cmd += "\" ";
			sprintf(buff, "%ld", m_lId);
			cmd += buff;
			m_pSim->HandleNewCommand(cmd);
			ret = HSLBD_TRACKING_STARTED;
			break;
    			
		default:
			ASSERT(FALSE); 
			ret = HSLBD_ERROR;
			break;
	}                         
	
	return ret;
} 
                                        
void CSObjGraph::OnHotSpotLButtonUp(CHotSpot* pHotSpot, UINT nFlags, CPoint point)
{
}                      

void CSObjGraph::SaveData()
{
	if (0 == m_storedData.GetSize())
	{
		AfxMessageBox("Sorry, no data to save as yet");
	}
	else
	{
		CFileDialog dlg(FALSE, // BOOL bOpenFileDialog,
		                "txt", // LPCSTR lpszDefExt = NULL,
		                "data.txt", // LPCSTR lpszFileName = NULL,
		                OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, // DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		                "Text Files (*.txt) | *.txt | All Files | *.* ||"); // LPCSTR lpszFilter = NULL, CWnd* pParentWnd = NULL; 
		
		dlg.m_ofn.lpstrTitle = "Save graphed data as tab-delimited text";
		
		if (IDOK == dlg.DoModal())
		{
			CString sFileName = dlg.GetPathName();
	
			CStdioFile file;
			
			if (file.Open(sFileName, CFile::modeWrite | CFile::modeCreate))
			{                                            
				TRACE("File opened OK\n"); 

				CString sXUnits, sYUnits;
				
				if (m_axisX.m_numLink.m_nPointerValid)
				{
					sXUnits = m_axisX.m_numLink.m_pNumeric->m_sUnits;
				}
				else
				{
					sXUnits = "units unavailable";
				}
				
				if (m_axisY.m_numLink.m_nPointerValid)
				{
					sYUnits = m_axisY.m_numLink.m_pNumeric->m_sUnits;
				}
				else
				{
					sYUnits = "units unavailable";
				} 
				
				CString outString;
				
				outString = "Data saved from SimChemistry for Windows document "
				             + m_pSim->m_pDoc->GetTitle();
				             
				outString += " at ";				             
				             
				CTime time(CTime::GetCurrentTime());
				
				// Note that the FormatGmt function of CTime returns wrong results,
				// at least for %H and %d, so I'm not using it deliberately. Apart
				// from the month name.
				
				sprintf(buff, "%2.2d:%2.2d on %2.2d", time.GetHour(),
				                                      time.GetMinute(),
				                                      time.GetDay());
				
				outString += buff; 
				
				outString += time.FormatGmt("-%b-");
				
				sprintf(buff, "%d", time.GetYear());
				
				outString += buff; 
				
				file.WriteString(outString);

				file.WriteString("\n");
				
				outString = m_axisX.m_numLink.m_sLinkText + '\t' + m_axisY.m_numLink.m_sLinkText + '\n'; 
				
				file.WriteString(outString);
				
				outString = m_sXLabel + '\t' + m_sYLabel + '\n';
				
				file.WriteString(outString);
				
				outString = sXUnits + '\t' + sYUnits + '\n';
				
				file.WriteString(outString);
				
				file.WriteString("\n");
				

				CGraphPoint* pGP;
	
				for (int i = 0; i <= m_storedData.GetUpperBound(); i++)
				{
					pGP = (CGraphPoint*) m_storedData[i];
					
					sprintf(buff, "%g\t%g\n", pGP->m_dRealX, pGP->m_dRealY);
					
					file.WriteString(buff);
				}

				file.Close();
			}
			else
			{                
				TRACE("Error opening file!\n");
				AfxMessageBox("Couldn't save to file -- may be in use by another program, "
				              "or disk full or read only.");
			}
	
		}                   
		
	} // end of if (no data)
}

void CSObjGraph::ClearData()
{  
	CGraphPoint* pGP;
	
	int i;
	
	while ((i = m_storedData.GetUpperBound()) >= 0)
	{
		pGP = (CGraphPoint*) m_storedData[i];
		m_storedData.RemoveAt(i);
		
		delete pGP;
	}
    
    m_nSamples = 0;
    m_dXSum = 0;
    m_dYSum = 0;               
    m_nLastPointPlotted = -1;
    m_nNoPointsDrawnSinceClear = TRUE;
    
    m_nBitmapReady = FALSE;
}
