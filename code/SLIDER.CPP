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
#include "slider.h"
#include "lennard.h"   
#include "sslprpdg.h"   
#include "newsvdg.h"
#include "sltyprdg.h"




#ifdef _DEBUG
extern long global_debug_counter;
#endif

//////////////////////////////////////////////////////////////////////                                 
// CObjTypeSlider functionality                        
//////////////////////////////////////////////////////////////////////                                 

// constructor          
CObjTypeSlider::CObjTypeSlider()
{                   
	// initialise variables to reasonable state
	
	m_nHasInts = FALSE;
	m_nIsMobile = FALSE; 
	
	// TODO check constructor rules, originally had: CObjType::CObjType();
}

// Command list
CMD_ENTRY CObjTypeSlider::m_celCObjTypeSliderCommands[MAX_COMMANDS_PER_OBJ] = 
{                                        
	{"name",      CA_STRING, CT_STATE,   CI_NAME,      LIM_MIN_CHARS_OBJTYPE, LIM_MAX_CHARS_OBJTYPE},
	{"end",       CA_NONE,   CT_ENDLIST, CI_ENDLIST,   0,     0}
};

// Command handlers                            

// Overrides of CObjType

CMD_ENTRY* CObjTypeSlider::GetCmdList()         
{ 
	return &(m_celCObjTypeSliderCommands[0]);
}

int CObjTypeSlider::AcceptCommandLine(CString& remainingLine, CString& errString)
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

int CObjTypeSlider::GetStateParam(int command)
{                         
	int ret = SR_OK;
	
	switch(command)
	{
		case CI_NAME:
			m_sArg = m_sName;
			break;     
			
		default:
			TRACE("Asked for param for unknown command in CObjTypeSlider\n");
			ASSERT(FALSE);
			ret = SR_ERROR;
			break;
	}             
	
	return ret;
}

void CObjTypeSlider::DrawAllSObjs(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2)
{                        
	// Each Slider draws itself onto its own bitmap, to which
	// new points can be added without redrawing. Here we need
	// only ask each of our Slider child objects to get their
	// bitmaps up to date and then blat them onto the display.
	
	CSObjSlider* pSlider;
	pSlider = (CSObjSlider*) m_pFirstSObj;
    
    CBitmap* pOldBitmap = NULL;
    int nFirst = TRUE;
    
	while (NULL != pSlider)
	{   
		pSlider->UpdateBitmap(pSpareDC1);
		
		if (nFirst)
		{
			pOldBitmap = pSpareDC1->SelectObject(pSlider->m_pBitmap);
			nFirst = FALSE;
		}
		else
		{
			pSpareDC1->SelectObject(pSlider->m_pBitmap);
		}

        DoBlt(pDC, pSpareDC1, pSlider);
//		pDC->BitBlt(pSlider->m_ptTopLeft.x, pSlider->m_ptTopLeft.y,
//		            pSlider->m_nScreenWidth, -pSlider->m_nScreenHeight,
//		            pSpareDC1, 0, 0, SRCAND);  
		            
		pSlider = (CSObjSlider*) pSlider->m_pNextSObj;
	}                
	
	if (NULL != pOldBitmap)
	{    
		pSpareDC1->SelectObject(pOldBitmap);
	}

}   

void CObjTypeSlider::DoBlt(CDC* pDC, CDC*pSpareDC1, CSObjSlider* pSlider)
{
		pDC->BitBlt(pSlider->m_ptTopLeft.x, pSlider->m_ptTopLeft.y,
		            pSlider->m_nScreenWidth, -pSlider->m_nScreenHeight,
		            pSpareDC1, 0, 0, SRCAND);  
}

void CObjTypeSlider::PropertiesCommand()          
{                        
	AfxMessageBox("No type properties to edit for sliders");
}
    
void CObjTypeSlider::OnLButtonUp()
{        
	// If user has dragged substantially, assume they were trying
	// to draw a Slider; if only a little bit or not at all and it
	// was the first click, then let them click elsewhere to define
	// the other end of the Slider.

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
			
		CSSliderPropsDlg dlg;
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

		if (IDOK == dlg.DoModal())
		{   
			// go ahead and create Slider as specified
				
			CString cmd;
				
			cmd = "slider \"";
			cmd += m_sName;
			cmd += "\" ";
				
			cmd += "data \"" + dlg.m_sData; // quotes in case null string
			cmd += "\" ";
								
			if (dlg.m_nCheckTitle)
			{
				cmd += "title \"";
				cmd += dlg.m_sTitle;
				cmd += "\" ";
			}
				
			sprintf(buff, "xleft %g ytop %g xright %g ybottom %g ", xl, yt, xr, yb);
			cmd += buff;
				
			sprintf(buff, "scalemin %g scalemax %g", dlg.m_dScaleMin, dlg.m_dScaleMax);
			cmd += buff;
				
			m_pSim->HandleNewCommand(cmd); 
		}
		else
		{
			// user cancelled in dialogue, don't create Slider
		}
			
		// reset drawing state machine whether making Slider or not
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
    
void CObjTypeSlider::Initialise()
{
    // gets called if sim scaling factor changes etc.

    CSObjSlider* pSlider = (CSObjSlider*) m_pFirstSObj;
	
	while (NULL != pSlider)
	{                                                    
		pSlider->m_nBitmapReady = FALSE; // force redraw when next drawn
		pSlider = (CSObjSlider*) pSlider->m_pNextSObj;
	}

}
    
/////////////////////////////////////////////////////////////////////////
// CSObjSlider functionality
/////////////////////////////////////////////////////////////////////////

CSObjSlider::CSObjSlider()
{    
	// nonvirtual base CSObj constructor will have already been called
	
	m_pBitmap = NULL;
	m_pTitleFont = NULL;
	m_nTitleFontSize = 0;
	m_pNumTextFont = NULL;
	m_nNumTextFontSize = 0;
	m_nBitmapReady = FALSE;
	m_nValueChanged = TRUE;
	m_dValue = 0;
	m_dScaleMin = 0;
	m_dScaleMax = 1;
	m_hsNumText.m_pOwner = this;
	m_hsNumText.m_nId = HS_NUMTEXT;
	m_hsKnob.m_pOwner = this;      
	m_hsKnob.m_nId = HS_KNOB;  
	m_dTrackingValue = 0;
	m_nTrackingKnob = FALSE;
}

CSObjSlider::~CSObjSlider()
{    
	delete m_pBitmap;     
	delete m_pTitleFont;
	delete m_pNumTextFont;

	// base CSObj destructor will be called next automatically    
}
	                                    
// Command list
CMD_ENTRY CSObjSlider::m_celCSObjSliderCommands[MAX_COMMANDS_PER_OBJ] = 
{                      
	{"id",        CA_LONG,   CT_STATE,   CI_ID,      0,      2000000000},                  
	{"xleft",     CA_DOUBLE, CT_STATE,   CI_XLEFT,   LIM_MIN_X,  LIM_MAX_X },
	{"xright",    CA_DOUBLE, CT_STATE,   CI_XRIGHT,  LIM_MIN_X,  LIM_MAX_X },
	{"ytop",      CA_DOUBLE, CT_STATE,   CI_YTOP,    LIM_MIN_Y,  LIM_MAX_Y },
	{"ybottom",   CA_DOUBLE, CT_STATE,   CI_YBOTTOM, LIM_MIN_Y,  LIM_MAX_Y },    
	{"title",     CA_STRING, CT_STATE,   CI_TITLE,   0,     LIM_MAX_CHARS_SLIDER_TITLE},
	{"data",      CA_STRING, CT_STATE,   CI_DATA,    LIM_MIN_CHARS_NUMERIC_NAME,     LIM_MAX_CHARS_NUMERIC_NAME},	
	{"scalemin",  CA_DOUBLE, CT_STATE,   CI_SCALEMIN,-1e20, 1e20},
	{"scalemax",  CA_DOUBLE, CT_STATE,   CI_SCALEMAX,-1e20, 1e20},
	{"end",       CA_NONE,   CT_ENDLIST, CI_ENDLIST, 0,     0   }
};

CMD_ENTRY* CSObjSlider::GetCmdList()
{ 
	return &(m_celCSObjSliderCommands[0]);
}

int CSObjSlider::AcceptCommandLine(CString& remainingLine, CString& errString)
{
	int command;
	int ret;
    
    m_fsValid.all = 0;  // mark nothing as initialised
    m_nBitmapReady = FALSE;
    m_nValueChanged = TRUE;
    
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
			
			case CI_TITLE:
				m_sTitle = m_sArg;
				m_fsValid.bf.title = 1;
				break;
				
			case CI_DATA:
				m_numLink.m_sLinkText = m_sArg;
				m_fsValid.bf.data = 1;
				break;

			case CI_SCALEMIN: 
				m_dScaleMin = m_dArg;   
				m_fsValid.bf.scalemin = 1;
				break; 

			case CI_SCALEMAX: 
				m_dScaleMax = m_dArg;   
				m_fsValid.bf.scalemax = 1;
				break; 
							
			default:
				ret |= SR_ERROR;
				errString += "confused near: " + remainingLine;
				break;
		}
		
        ret |= GrabCommand(remainingLine, errString, command);
    
	}              
	                                                
	                                                
	// further initialisation now we are established
	
	
	if (m_fsValid.bf.title)
	{
		m_nHasTitle = (m_sTitle != "");
		
		if (m_sTitle.GetLength() > LIM_MAX_CHARS_SLIDER_TITLE)
		{
			errString += "title truncated; ";
			ret |= SR_WARN;
			m_sTitle = m_sTitle.Left(LIM_MAX_CHARS_SLIDER_TITLE);
		}
	}
	else
	{
		m_nHasTitle = FALSE;
		m_sTitle = "";
	}

	// register hot spots which will enable cursor changing and clicking
	// on slider knob and number text (re-registering doesn't matter)
	
	m_pSim->AddHotSpotRef(&m_hsNumText);
	m_pSim->AddHotSpotRef(&m_hsKnob);
	  

	return ret;
}

int CSObjSlider::GetStateParam(int command)
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
		
		case CI_TITLE:
			m_sArg = m_sTitle;
			break;  
			
		case CI_DATA:
			m_sArg = m_numLink.m_sLinkText;
			break;   
			
		case CI_SCALEMIN:
			m_dArg = m_dScaleMin;
			break;
			
		case CI_SCALEMAX:
			m_dArg = m_dScaleMax;
			break;
				
		default:
			TRACE("Asked for param for unknown command in CSObjSlider\n");
			ASSERT(FALSE);
			ret = SR_ERROR;
			break;
	}             
	
	return ret;
}

void CSObjSlider::UpdateBitmap(CDC* pSpareDC)
{   
	// see if slider position and text will have changed, and if so
	// redraw bitmap, otherwise don't bother
	
	
	// connect numeric if not already done so
	if (!m_numLink.m_nPointerValid)
	{
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
	
	if (m_numLink.m_nPointerValid)
	{
		double dNewSliderValue;
		
		if (m_nTrackingKnob && (m_hsKnob.m_rect != m_hsKnob.m_trackingRect))
		{
			// Clip tracking rect to allowed limits
			m_hsKnob.m_trackingRect.top = m_hsKnob.m_rect.top;
			m_hsKnob.m_trackingRect.bottom = m_hsKnob.m_rect.bottom; 
			
			if (m_hsKnob.m_trackingRect.left < m_nScaleMinDeviceKnobXLeft)
			{
				 m_hsKnob.m_trackingRect.left = m_nScaleMinDeviceKnobXLeft;
				 m_hsKnob.m_trackingRect.right = m_hsKnob.m_trackingRect.left +
				                             (m_hsKnob.m_rect.right - m_hsKnob.m_rect.left);
			}
			else if (m_hsKnob.m_trackingRect.left > m_nScaleMaxDeviceKnobXLeft)
			{
				 m_hsKnob.m_trackingRect.left = m_nScaleMaxDeviceKnobXLeft;
 				 m_hsKnob.m_trackingRect.right = m_hsKnob.m_trackingRect.left +
				                             (m_hsKnob.m_rect.right - m_hsKnob.m_rect.left);
			}
			
			if ((m_hsKnob.m_trackingRect.left == m_nScaleMinDeviceKnobXLeft)
			      || (m_nScaleMinDeviceKnobXLeft == m_nScaleMaxDeviceKnobXLeft))
			{
				dNewSliderValue = m_dScaleMin;
			}                                  
			else if (m_hsKnob.m_trackingRect.left == m_nScaleMaxDeviceKnobXLeft)
			{
				dNewSliderValue = m_dScaleMax;
			}                                 
			else
			{
				dNewSliderValue = m_dScaleMin + ((double) m_hsKnob.m_trackingRect.left - m_nScaleMinDeviceKnobXLeft)
				                                 / (m_nScaleMaxDeviceKnobXLeft - m_nScaleMinDeviceKnobXLeft)
				                                 * (m_dScaleMax - m_dScaleMin);
				
				CSim::Round(dNewSliderValue, (int) 4);
			}
			
			if (dNewSliderValue != m_dTrackingValue)
			{
				m_nValueChanged = TRUE;
				m_dTrackingValue = dNewSliderValue;
			}
		}
		else
		{
			dNewSliderValue = *(m_numLink.m_pNumeric->m_pdValue);
	
			if (m_dValue != dNewSliderValue)
			{
				m_nValueChanged = TRUE;
				m_dValue = dNewSliderValue;
			}
		}                                                         

		if (m_nValueChanged)
		{		
			sprintf(buff, "%g ", dNewSliderValue);
			m_numLink.m_sText = buff + m_numLink.m_pNumeric->m_sUnits;
		}
	}  
	else
	{
		if (m_numLink.m_sText != "<not found>")
		{
			m_numLink.m_sText = "<not found>";
			m_dValue = 0;
			m_nValueChanged = TRUE;
		}
	}
	
	if (!m_nBitmapReady || m_nValueChanged)
	{   
		RedrawBitmap(pSpareDC);                                   
	}   
	else
	{
		// skip redraw
	}     
	
}

void CSObjSlider::RedrawBitmap(CDC* pSpareDC)
{   
	// Save device context as will be selecting various pens etc
	int nSavedDCContext = pSpareDC->SaveDC();
	ASSERT(nSavedDCContext);
	

	// Various coordinates needed only while we are updating labels etc
		
	// visible edge round whole Slider
	int nBorderXLeft, nBorderYTop, nBorderXRight, nBorderYBottom;
		
	// First prepare blank bitmap of correct screen size
		
	// Get current screen size from sim units
				
	m_ptTopLeft = m_pSim->SimToLogical(m_dXLeft, m_dYTop); 
	m_ptBottomRight = m_pSim->SimToLogical(m_dXRight, m_dYBottom);
        
    m_nScreenWidth = m_ptBottomRight.x - m_ptTopLeft.x + 1;
    m_nScreenHeight = m_ptTopLeft.y - m_ptBottomRight.y + 1;
    
	if (!m_nBitmapReady)
	{
		// make new bitmap only if size has changed, as get Windows
		// doing disk stuff allocating the memory for new one
		
		delete m_pBitmap;   // destroy any existing version
			
		m_pBitmap = new CBitmap;
	        
		m_pBitmap->CreateCompatibleBitmap(pSpareDC, m_nScreenWidth, m_nScreenHeight);
	}
			
	// bitmap is now ready for use, select into DC to draw onto
			
	// Paint bitmap white, then add 1-pixel black border around it
		
	pSpareDC->SelectObject(m_pBitmap);
		
	pSpareDC->SelectStockObject(WHITE_BRUSH);  
	pSpareDC->PatBlt(0, 0, m_nScreenWidth + 1, -(m_nScreenHeight + 1), PATCOPY); 
		
        
	// Calculate and draw border round bitmap within which everything else lies
		
	// Indent in extra pixel or two at right and bottom, otherwise sometimes
	// includes in BitBlt and sometimes doesn't. Don't know why.
		
	nBorderXLeft = 0;
	nBorderYTop = 0;
	nBorderXRight = m_nScreenWidth - 2; // one width->max coord, one spare
	nBorderYBottom = -(m_nScreenHeight - 2); // similarly

	CPen penBorder(PS_SOLID, 1, RGB(0, 0, 0));
	pSpareDC->SelectObject(&penBorder);
		
	pSpareDC->MoveTo(nBorderXLeft, nBorderYTop);
	pSpareDC->LineTo(nBorderXRight, nBorderYTop);
	pSpareDC->LineTo(nBorderXRight, nBorderYBottom);
	pSpareDC->LineTo(nBorderXLeft, nBorderYBottom);
	pSpareDC->LineTo(nBorderXLeft, nBorderYTop);


	// now in a position to calculate absolute indents from edge
	int nBorderHeight = nBorderYTop - nBorderYBottom;
	int nBorderWidth = nBorderXRight - nBorderXLeft;
	
	int nTitleYTop, nTitleXLeft, nTitleXRight, nTitleHeight, nTitleYBottom, nTitleWidth;

	nTitleYTop = nBorderYTop - (int) (nBorderHeight * SLIDER_TOP_GAP_PROPORTION);
	nTitleXLeft = nBorderXLeft + (int) (nBorderWidth * SLIDER_TITLE_SIDE_GAP_PROPORTION);
	nTitleXRight = nBorderXRight - (int) (nBorderWidth * SLIDER_TITLE_SIDE_GAP_PROPORTION);
	nTitleWidth = nTitleXRight - nTitleXLeft;
	  
	if (m_nHasTitle)
	{
		nTitleHeight = (int) (nBorderHeight * SLIDER_TITLE_HEIGHT_PROPORTION);
	}
	else
	{
		nTitleHeight = 0;
	}
	
    // Draw title, if any     
    
    if (m_nHasTitle)
    {
	    if (m_nBitmapReady)
	    {
	    	// Use font as previously created to fit this space
	    	nTitleHeight = m_nTitleFontSize;
	    }
	    else
	    {
	    	// slider has changed size, make new title font
			
			int nTitleFits; 
			CSize sizeTitle;
				    	
	    	do
	    	{
		    	delete m_pTitleFont;
		    	
		    	m_pTitleFont = new CFont;
		    	
				m_pTitleFont->CreateFont(nTitleHeight, // int nHeight,
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
		                                 DEFAULT_PITCH, // BYTE nPitchAndFamily,  
		                                 "Arial"); // LPCSTR lpszFacename
		        
		        pSpareDC->SelectObject(m_pTitleFont);
		                                 
				sizeTitle = pSpareDC->GetTextExtent(m_sTitle, m_sTitle.GetLength());
				
				nTitleFits = (sizeTitle.cx <= nTitleWidth) || (sizeTitle.cx == 0);
				
				if (!nTitleFits)
				{   
					// reduce font size until just fits; don't go straight for expected
					// correct size, or might get a few points smaller than really
					// wanted. This seems to work OK!
					
					double dShrinkFactor = (double) nTitleWidth / sizeTitle.cx;
					dShrinkFactor = 1 - ((1 - dShrinkFactor) / 1.5);
					nTitleHeight = (int) (dShrinkFactor * nTitleHeight) - 1; // -1 ensures can't loop forever
				}
			}	                                
	        while (!nTitleFits && nTitleHeight > 3);
		
		}  // end of if (size changed)
		
		m_nTitleFontSize = nTitleHeight; // remember for next time
		        
        pSpareDC->SetTextAlign(TA_CENTER | TA_TOP);
        
        pSpareDC->SelectObject(m_pTitleFont);
        
        pSpareDC->TextOut((nTitleXLeft + nTitleXRight) / 2, nTitleYTop, m_sTitle);
    }
    else
    {                                   
    	// no title
    	
    	m_nTitleFontSize = nTitleHeight;
    }
	
	// Now we know how big the title was, pack in the slider control and 
	// number text
	
	nTitleYBottom = nTitleYTop - m_nTitleFontSize;
	
	int nNumTextXRight = nBorderXRight - (int) (nBorderWidth * SLIDER_NUMTEXT_RIGHT_GAP_PROPORTION);
	int nNumTextXLeft = nNumTextXRight - (int) (nBorderWidth * SLIDER_NUMTEXT_WIDTH_PROPORTION);
	int nNumTextWidth = nNumTextXRight - nNumTextXLeft;
	
	int nControlXLeft = nBorderXLeft + (int) (nBorderWidth * SLIDER_CONTROL_LEFT_GAP_PROPORTION);
    int nControlXRight = nNumTextXLeft - (int) (nBorderWidth * SLIDER_CONTROL_NUMTEXT_GAP_PROPORTION);
    int nControlWidth = nControlXRight - nControlXLeft;
    
	// Control and number text should be centred vertically
	
	int nBottom = nBorderYBottom + (int) (nBorderHeight * SLIDER_BOTTOM_GAP_PROPORTION);
	int nTop = nTitleYBottom - (int) (nBorderHeight * SLIDER_TITLE_BOTTOM_GAP_PROPORTION);
    int nHeight = nTop - nBottom; 
    int nControlIndent = (int) (nHeight * (1 - SLIDER_CONTROL_HEIGHT_PROPORTION) / 2);
    int nNumTextIndent = (int) (nHeight * (1 - SLIDER_NUMTEXT_HEIGHT_PROPORTION) / 2); 
    
    int nControlYBottom = nBottom + nControlIndent;
    int nControlYTop = nTop - nControlIndent;    
    int nControlHeight = nControlYTop - nControlYBottom;
    
    int nNumTextYBottom = nBottom + nNumTextIndent;
    int nNumTextYTop = nTop - nNumTextIndent;   
    int nNumTextHeight = nNumTextYTop - nNumTextYBottom;


    // Draw number text
    
    // If bitmap doesn't need complete update and have previously existing
    // font, try using that, but if text doesn't fit make it smaller

	int nNumTextFits = FALSE; 
	CSize sizeNumText;
    
    if (m_nBitmapReady && (m_pNumTextFont != NULL))
    {
        pSpareDC->SelectObject(m_pNumTextFont);
		                                 
		sizeNumText = pSpareDC->GetTextExtent(m_numLink.m_sText, m_numLink.m_sText.GetLength());
				
		nNumTextFits = (sizeNumText.cx <= nNumTextWidth);
	}
	
	if (!nNumTextFits) // or no previous font
	{			    	
    	do
    	{
	    	delete m_pNumTextFont;
		    	
	    	m_pNumTextFont = new CFont;
		    	
			m_pNumTextFont->CreateFont(nNumTextHeight, // int nHeight,
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
	                                 DEFAULT_PITCH, // BYTE nPitchAndFamily,  
	                                 "Arial"); // LPCSTR lpszFacename
		        
	        pSpareDC->SelectObject(m_pNumTextFont);
		                                 
			sizeNumText = pSpareDC->GetTextExtent(m_numLink.m_sText, m_numLink.m_sText.GetLength());
				
			nNumTextFits = (sizeNumText.cx <= nNumTextWidth) || (sizeNumText.cx == 0);
				
			if (!nNumTextFits)
			{
					// Do same sort of thing as for title, above
					
					double dShrinkFactor = (double) nNumTextWidth / sizeNumText.cx;
					dShrinkFactor = 1 - ((1 - dShrinkFactor) / 1.5);
					nNumTextHeight = (int) (dShrinkFactor * nNumTextHeight) - 1; // -1 ensures can't loop forever
			}
		}	                                
        while (!nNumTextFits && nNumTextHeight > 3);
		
	}  // end of if (needed new font)
		
		        
    pSpareDC->SetTextAlign(TA_LEFT | TA_TOP);
        
    pSpareDC->SelectObject(m_pNumTextFont);
    
    if (m_nTrackingKnob)
    {
    	pSpareDC->SetTextColor(RGB(255, 0, 0)); // red when changing through slide
    }                         
    else
    {                
    	pSpareDC->SetTextColor(RGB(0, 0, 255)); // blue like hyperlink
    }
        
    pSpareDC->TextOut(nNumTextXLeft, nNumTextYTop, m_numLink.m_sText);    
    
    // Set numtext hotspot coordinates, so that if mouse moved over
    // number, cursor will change
    
    m_hsNumText.m_rect.left = nNumTextXLeft;
    m_hsNumText.m_rect.right = nNumTextXRight;
    m_hsNumText.m_rect.top = nNumTextYTop;
    m_hsNumText.m_rect.bottom = nNumTextYBottom;
    
    // those coordinates are relative to top left of bitmap, so need
    // shifting by top-left coord of this control object overall to
    // get coordinates relative to top-left of simulation window
    
    CPoint topLeft = m_pSim->SimToLogical(m_dXLeft, m_dYTop);
    m_hsNumText.m_rect += topLeft;
    
    pSpareDC->LPtoDP(&(m_hsNumText.m_rect)); // needs device coords to compare
                                             // with mouse positions etc
    
    
    // Finally draw slider control itself 
    
    pSpareDC->SelectStockObject(BLACK_PEN);
    
    pSpareDC->MoveTo(nControlXLeft, nControlYBottom);
    pSpareDC->LineTo(nControlXLeft, nControlYTop);
    pSpareDC->MoveTo(nControlXRight, nControlYBottom);
    pSpareDC->LineTo(nControlXRight, nControlYTop); 
    
    int nControlMidHeight = (nControlYTop + nControlYBottom) / 2;
    
    pSpareDC->MoveTo(nControlXLeft, nControlMidHeight);
    pSpareDC->LineTo(nControlXRight, nControlMidHeight);

	int nControlSmallTickHeight = (int) ((double) nControlHeight * SLIDER_SMALL_TICK_PROPORTION);    

	int nSmallTickBottom = nControlMidHeight - nControlSmallTickHeight / 2;
	int nSmallTickTop = nSmallTickBottom + nControlSmallTickHeight;
	
    for (int i = 1; i <= 9; i++)
    {
    	int x = nControlXLeft + (int) ((double) nControlWidth * i / 10);
    	
    	pSpareDC->MoveTo(x, nSmallTickBottom);
    	pSpareDC->LineTo(x, nSmallTickTop);
    }                                   
    
    int nKnobWidth = (int) ((double) nControlWidth * SLIDER_KNOB_WIDTH_PROPORTION);
    int nKnobHeight = (int) ((double) nControlHeight * SLIDER_KNOB_HEIGHT_PROPORTION);  
    
    if (nKnobWidth < 2) nKnobWidth = 2;
    
    if (nKnobHeight < 3) nKnobHeight = 3;
    
    pSpareDC->SelectStockObject(LTGRAY_BRUSH);
    CPen penBlue(PS_SOLID, 2, RGB(0,0,255)); // blue because clickable   
    pSpareDC->SelectObject(&penBlue);
    
    double dProportion = (m_dValue - m_dScaleMin) / (m_dScaleMax - m_dScaleMin);
    
    if (dProportion < 0)
    {
    	dProportion = 0;
    }
    else if (dProportion > 1)
    {
    	dProportion = 1;
    }
    
    // Use hot spot rect first in logical co-ords to draw knob, then convert
    // to device coords for subsequent mouse point compare operations
    
    m_hsKnob.m_rect.left = (int) (dProportion * nControlWidth) + nControlXLeft - nKnobWidth / 2;
    m_hsKnob.m_rect.right = m_hsKnob.m_rect.left + nKnobWidth;
    m_hsKnob.m_rect.bottom = nControlMidHeight - nKnobHeight / 2;  // reversed
    m_hsKnob.m_rect.top = m_hsKnob.m_rect.bottom + nKnobHeight;
    
    pSpareDC->Rectangle(&(m_hsKnob.m_rect));

    m_hsKnob.m_rect += topLeft;
    
    pSpareDC->LPtoDP(&(m_hsKnob.m_rect)); // needs device coords to compare
                                             // with mouse positions etc

	// work out limits of knob movement in device coords    
    CPoint ptLim;                                       
    ptLim.y = 0;  // arbitrary

    ptLim.x = topLeft.x + nControlXLeft - nKnobWidth / 2;
    pSpareDC->LPtoDP(&ptLim);
    m_nScaleMinDeviceKnobXLeft = ptLim.x;

    ptLim.x = topLeft.x + nControlXRight - nKnobWidth / 2;
    pSpareDC->LPtoDP(&ptLim);
    m_nScaleMaxDeviceKnobXLeft = ptLim.x;

	if (m_nTrackingKnob)
	{
		// Highlight where user is moving knob to; make use of device
		// co-ords we already have
		
		CPoint ptTrackTopLeft(m_hsKnob.m_trackingRect.left, m_hsKnob.m_trackingRect.top);
		CPoint ptTrackBottomRight(m_hsKnob.m_trackingRect.right, m_hsKnob.m_trackingRect.bottom);
		
		pSpareDC->DPtoLP(&ptTrackTopLeft);		
		pSpareDC->DPtoLP(&ptTrackBottomRight);
		
		CRect rectTrack(ptTrackTopLeft.x, ptTrackTopLeft.y, 
		                ptTrackBottomRight.x, ptTrackBottomRight.y);
		
		rectTrack -= topLeft;
		
		CPen redPen(PS_SOLID, 2, RGB(255, 0, 0));
		CBrush redHatch(HS_DIAGCROSS, RGB(255, 0, 0));
		
		pSpareDC->SelectObject(&redPen);
		pSpareDC->SelectObject(&redHatch);
		
		pSpareDC->Rectangle(&rectTrack);
	}
	
	    
	pSpareDC->RestoreDC(nSavedDCContext);
				
	m_nBitmapReady = TRUE;
	m_nValueChanged = FALSE;
}
// Command handlers                            



int CSObjSlider::HitTest(double dXLeft, double dYBottom,
                          double dXRight, double dYTop)
{
	return RectsOverlap(dXLeft, dYBottom, dXRight, dYTop,
                         m_dXLeft, m_dYBottom, m_dXRight, m_dYTop);
}
                
void CSObjSlider::ObjectProperties()
{                   
	CSSliderPropsDlg dlg;
	dlg.m_pSim = m_pSim;
	
	dlg.m_nCheckTitle = m_nHasTitle;
	dlg.m_sTitle = m_sTitle;
	dlg.m_dScaleMin = m_dScaleMin;
	dlg.m_dScaleMax = m_dScaleMax;  
	dlg.m_nScaleLimsInit = TRUE;
	dlg.m_sData = m_numLink.m_sLinkText;
			
	if (IDOK == dlg.DoModal())
	{   
		// modify ourselves to new properties, and tell sim
		// to make us change to that new state
		if (dlg.m_nCheckTitle)
		{
			m_nHasTitle = TRUE;
			m_sTitle = dlg.m_sTitle;
		}
		else
		{
			m_nHasTitle = FALSE;
			m_sTitle = "";
		}
		
		m_dScaleMin = dlg.m_dScaleMin;
		m_dScaleMax = dlg.m_dScaleMax;
		
		// Remove existing NumLink/CNumeric link, if any, as may well
		// be referencing something else now
		
		if (m_numLink.m_nPointerValid)
		{
			m_numLink.m_pNumeric->RemoveRef(&m_numLink);
			m_numLink.m_nPointerValid = FALSE;
		}                                     
		
		m_numLink.m_sLinkText = dlg.m_sData;
		
				
		CString cmd;
				
		cmd = "change slider \"";
		cmd += m_pType->m_sName;
		cmd += "\" ";
		
		WriteState(cmd);		

		m_pSim->HandleNewCommand(cmd); 
	}
	else
	{
		// user cancelled in dialogue, don't change slider
	}

}


void CSObjSlider::DrawTracker(CDC* pDC)
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

void CSObjSlider::HandleMove(double dDeltaX, double dDeltaY)
{
	m_dXLeft += dDeltaX;
	m_dXRight += dDeltaX;
	m_dYTop += dDeltaY;
	m_dYBottom += dDeltaY;
}

void CSObjSlider::OnMoveOrResize(double dNewXLeft, double dNewYTop,
                                  double dNewXRight, double dNewYBottom,
	                              double dOldXLeft, double dOldYTop,
	                              int nResized)
{  
	CString cmd("change slider \"");
	cmd += m_pType->m_sName; 
	cmd += "\" ";
	
	m_dXLeft = dNewXLeft;
	m_dXRight = dNewXRight;
	m_dYTop = dNewYTop;
	m_dYBottom = dNewYBottom; 

    WriteState(cmd); 
	    
    m_pSim->HandleNewCommand(cmd);	
}
	                              
int CSObjSlider::HotSpotSetCursor(CHotSpot* pHotSpot, CWnd* pWnd, UINT nHitTest)
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
    		case HS_NUMTEXT:
    			hCursor = theApp.LoadCursor(AFX_ID_HAND);
    			break;
    			
    		case HS_KNOB:
    			hCursor = theApp.LoadCursor(IDC_SLIDE_LEFT_RIGHT);
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

int CSObjSlider::OnHotSpotLButtonDown(CHotSpot* pHotSpot, UINT nFlags, CPoint point)
{  
	// default will note initial coords etc in case tracking started:                         
	CSObj::OnHotSpotLButtonDown(pHotSpot, nFlags, point);            
	          
	int ret = HSLBD_ERROR;
	
	CNewSliderValueDlg dlg;
	
	dlg.m_dMinValue = m_dScaleMin;
	dlg.m_dMaxValue = m_dScaleMax;  
	dlg.m_dValue = m_dValue;              

	int nNumericValid;
		
	if (m_numLink.m_nPointerValid)
	{
		dlg.m_sUnits = m_numLink.m_pNumeric->m_sUnits;
		nNumericValid = TRUE;
	}
	else
	{
		dlg.m_sUnits = "<not found>";
		nNumericValid = FALSE;
	}
	
	CString cmd;
	
	switch (pHotSpot->m_nId)
	{ 
		case HS_NUMTEXT: 
			if (nNumericValid)
			{
				if (IDOK == dlg.DoModal())
				{
					cmd = "set \"" + m_numLink.m_pNumeric->m_sName;
					cmd += "\" ";
					sprintf(buff, "%g", dlg.m_dValue);
					cmd += buff;
					
					m_pSim->HandleNewCommand(cmd); 				
				}
			}
			ret = HSLBD_ONE_OFF_ACTION_DONE;
			break;
    			
		case HS_KNOB:
			m_dTrackingValue = m_dValue;
			m_nTrackingKnob = TRUE; 
			ret = HSLBD_TRACKING_STARTED;
			break;
    			
		default:
			ASSERT(FALSE);
			break;
	}
  	
	
	return ret;
} 

void CSObjSlider::OnHotSpotLButtonUp(CHotSpot* pHotSpot, UINT nFlags, CPoint point)
{ 
	m_nTrackingKnob = FALSE; 
	
	CString cmd;

	cmd = "set \"" + m_numLink.m_pNumeric->m_sName;
	cmd += "\" ";
	sprintf(buff, "%g", m_dTrackingValue);
	cmd += buff;               
	
	m_nBitmapReady = FALSE;
					
	m_pSim->HandleNewCommand(cmd); 				
}