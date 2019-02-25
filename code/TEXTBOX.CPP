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
#include "lennard.h"      
#include "simobjs.h"
#include "sim.h"      
#include "tracker.h"
#include "textbox.h" 
#include "edtxtdlg.h" 
#include "txtprpdg.h"     

#include <math.h>



#ifdef _DEBUG
extern long global_debug_counter;
#endif


//////////////////////////////////////////////////////////////////////                                 
// CObjTypeTextbox functionality                        
//////////////////////////////////////////////////////////////////////                                 

// constructor

CObjTypeTextbox::CObjTypeTextbox()
{         
	m_nHasInts = FALSE;
	m_nIsMobile = FALSE;   
	m_sFontName = "Arial";
	m_nFontSize = 12;         
}            

// Command list
CMD_ENTRY CObjTypeTextbox::m_celCObjTypeTextboxCommands[MAX_COMMANDS_PER_OBJ] = 
{                                        
	{"name",            CA_STRING, CT_STATE,   CI_NAME,              LIM_MIN_CHARS_OBJTYPE, LIM_MAX_CHARS_OBJTYPE },
	{"size",            CA_INT,    CT_STATE,   CI_SIZE,              0, 127 },
	{"font",            CA_STRING, CT_STATE,   CI_FONT,              0, LIM_MAX_CHARS_FONTNAME},
	{"end",             CA_NONE,   CT_ENDLIST, CI_ENDLIST,           0, 0   }
};

// Command handlers

// Functionality overridden from generic CObjType 

int CObjTypeTextbox::GetStateParam(int command)
{
	int ret = SR_OK;
	
	switch(command)
	{               
		case CI_NAME:
			m_sArg = m_sName;
			break;

		case CI_SIZE:
			m_nArg = m_nFontSize;
			break;  
			
		case CI_FONT:
			m_sArg = m_sFontName;
			break;
						
		default:
			TRACE("Asked for param for unknown command in CObjTypeTextbox\n");
			ret = SR_ERROR;
			break;
	}             
	
	return ret;

}

CMD_ENTRY* CObjTypeTextbox::GetCmdList()
{
	return &(m_celCObjTypeTextboxCommands[0]);
}      

int CObjTypeTextbox::AcceptCommandLine(CString& remainingLine, CString& errString)
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
			
			case CI_SIZE:
				m_nFontSize = m_nArg;
				break;
				
			case CI_FONT:
				m_sFontName = m_sArg;
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

void CObjTypeTextbox::DrawAllSObjs(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2)
{                               
	CSObjTextbox* pTextbox;
	pTextbox = (CSObjTextbox*) m_pFirstSObj;
	CPoint point;

	while (NULL != pTextbox)
	{                                              
		point = m_pSim->SimToLogical(pTextbox->m_dXLeft, pTextbox->m_dYTop); 
		
		DrawText(pDC, pTextbox->m_pFirstNumLink, point.x, point.y, pTextbox);
	    
		pTextbox = (CSObjTextbox*) pTextbox->m_pNextSObj;
	}
	  

}

void CObjTypeTextbox::UserDrawing(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2)
{   
	switch (m_pSim->m_nDrawingState)
	{                     

		case SM_AWAITING_CLICK1:
		case SM_BUTDN_NOT_MOVED1:      
		case SM_DRAGGING:
			// draw special cursor at current mouse position 
			DrawCrosshair(pDC);    
			break;
			
		default:
			// unknown state
			ASSERT(FALSE);
			break;
	}
}

void CObjTypeTextbox::OnLButtonUp()
{                       
	m_pSim->m_nDrawingState = SM_NOT_DRAWING;
	if (SM_SIM_PAUSED == m_pSim->m_nSimState) 
	{
		m_pSim->m_nSimState = SM_SIM_RUNNING;
	}           

	CString command("");   

	command = "textbox \"" + m_sName + "\" xleft ";
	sprintf(buff, "%g", m_pSim->m_dSimMouseX);
	command += buff;
	command += " ytop ";
	sprintf(buff, "%g", m_pSim->m_dSimMouseY);
	command += buff;
	EscapeBadChars(m_sUserTypedText); 
	command += " text \"" + m_sUserTypedText + "\"";
	
	m_pSim->HandleNewCommand(command);
}

void CObjTypeTextbox::ExpandBadChars(CString& string)
{
	// Starting with a string with escaped newlines like this:
	// "wibble$n$wobble", expand to something with newlines like
	// this: "wibble\nwobble". This is so that a multiline edit
	// control will display the text properly.

	CString oldString = string;
	
	string = "";
	
	int nPos;
	
	while (-1 != (nPos = oldString.Find("$n$")))
	{
		string += oldString.Left(nPos) + "\r\n";
		
		oldString = oldString.Right(oldString.GetLength() - nPos - 3);
	}    
	
	string += oldString;  // get tail end
}

void CObjTypeTextbox::EscapeBadChars(CString& string)
{
	CString oldString = string;
	
	string = "";
	
	int nPos;
	
	while (-1 != (nPos = oldString.FindOneOf("\n\r\"")))
	{
		char c = oldString[nPos];
		string += oldString.Left(nPos);
		oldString = oldString.Right(oldString.GetLength() - nPos - 1);
		
		switch (c)
		{   
			case '\r':
			
				string += "$n$";
				if ('\n' == oldString[0])
				{
					oldString = oldString.Right(oldString.GetLength() - 1);
				}
				break;
				
			case '\n':
			
				string += "$n$";
				break;
				
			case '\"':
			
				// for now, just bin these because input parsing routine
				// doesn't handle them in strings
				break;
				
			default:
				
				ASSERT(FALSE);
				break;
		}
	}    
	
	string += oldString;  // get tail end, free of funny chars
}

CPoint CObjTypeTextbox::DrawCrosshair(CDC* pDC)
{
	// helper function to draw positioning cursor;
	// overridden from base version which does simple
	// crosshair
	
    CPoint currentPt;
	currentPt = m_pSim->SimToLogical(m_pSim->m_dSimMouseX, m_pSim->m_dSimMouseY); 
	pDC->SelectStockObject(BLACK_PEN);
	currentPt.x -= DW_CROSSHAIR_RADIUS;
	pDC->MoveTo(currentPt.x, currentPt.y);
	pDC->LineTo(currentPt.x + 2 * DW_CROSSHAIR_RADIUS, currentPt.y); 
	currentPt.x += DW_CROSSHAIR_RADIUS;
	currentPt.y -= DW_CROSSHAIR_RADIUS;
	pDC->MoveTo(currentPt.x, currentPt.y);
	pDC->LineTo(currentPt.x, currentPt.y + 2 * DW_CROSSHAIR_RADIUS);
	currentPt.y += DW_CROSSHAIR_RADIUS;

	DrawTextFromString(pDC, m_sUserTypedText, currentPt.x, currentPt.y);
		
	return currentPt;
}

void CObjTypeTextbox::PropertiesCommand()
{         
	CTextboxPropsDlg dlg; 
	
	dlg.m_sName = m_sName; 
	dlg.m_sFontName = m_sFontName;
	dlg.m_nFontSize = m_nFontSize;
    dlg.m_dScaleFactor = m_pSim->m_dScaleFactor;
    
	if (IDOK == dlg.DoModal())
	{                     
		CString cmd("change objtype textbox name \"");
		cmd += dlg.m_sName;
		cmd += "\" font \"";
		cmd += dlg.m_sFontName;
		cmd += "\" size ";  
		sprintf(buff, "%d", dlg.m_nFontSize);
		cmd += buff;
		
		m_pSim->HandleNewCommand(cmd);
	}	
                                  
}

int CObjTypeTextbox::DrawCommand()
{                  
	CEditTextboxDlg dlg;
	
	dlg.m_sUserTypedText = "";
	dlg.m_pTextboxType = this;
	dlg.m_pSim = m_pSim;
	
	if (IDOK == dlg.DoModal())
	{            
		m_sUserTypedText = dlg.m_sUserTypedText;
		return TRUE; // do want to go into drawing state
	}
	else
	{
		return FALSE; // indicate user cancelled
	}
}

void CObjTypeTextbox::DrawTextFromString(CDC* pDC, CString& string, int xLeft, int yTop)
{      
	CNumLink* pFirstLink = ParseTextToLinks(string);
	
	DrawText(pDC, pFirstLink, xLeft, yTop, NULL); 
	
	if (NULL != pFirstLink)
	{
		CNumLink* pNextLink = pFirstLink->m_pNextLink;
		
		while (pNextLink != NULL)
		{
			delete pFirstLink;
			pFirstLink = pNextLink;
			pNextLink = pNextLink->m_pNextLink;
		}
		delete pFirstLink;	
	}
}   

void CObjTypeTextbox::Prepare()
{
	// get all child textboxes references to numeric quantities
	// initialised as sim about to run
	
	CSObjTextbox* pBox = (CSObjTextbox*) m_pFirstSObj;
	
	while (NULL != pBox)
	{                                            
		// delete existing chain of CNumLinks if any
		CNumLink* pFirstLink = pBox->m_pFirstNumLink;
		
    	if (NULL != pFirstLink)
		{
			CNumLink* pNextLink = pFirstLink->m_pNextLink;
				
			while (pNextLink != NULL)
			{
				delete pFirstLink;
				pFirstLink = pNextLink;
				pNextLink = pNextLink->m_pNextLink;
			}       
			
			delete pFirstLink;	
		}
		
		// and build new chain of them
		pBox->m_pFirstNumLink = ParseTextToLinks(pBox->m_sText);
		
	    pBox = (CSObjTextbox*) pBox->m_pNextSObj;
	}
}


CNumLink* CObjTypeTextbox::ParseTextToLinks(CString& string)
{                        
	if (string.IsEmpty())
	{
		return NULL;
	}
	else
	{
		
		CNumLink* pFirstLink = new CNumLink;
		CNumLink* pCurrentLink = pFirstLink;  

		enum {ADDING_TEXT, HIT_RETURN, BUILDING_LINKNAME} state;
		state = ADDING_TEXT;
		
		// Enter parsing loop. Attach text to a new CNumLink
		// object each time we encounter a delimiting $ or a return
		
		for (int i = 0; i < string.GetLength(); i++)
		{
			char c = string[i];
			
			switch (state)
			{
				case ADDING_TEXT:
					
					if (c == '$')
					{   
						if (pCurrentLink->m_sText.GetLength() > 0)
						{
							// start a new link object
							pCurrentLink->m_pNextLink = new CNumLink;
							pCurrentLink = pCurrentLink->m_pNextLink; 
						}
						pCurrentLink->m_nType = NL_NUMREF;
						state = BUILDING_LINKNAME;
					}
					else if (c == '\r')
					{    
						if (pCurrentLink->m_sText.GetLength() > 0)
						{
							// start a new link object
							pCurrentLink->m_pNextLink = new CNumLink;
							pCurrentLink = pCurrentLink->m_pNextLink; 
						}
						pCurrentLink->m_nType = NL_RETURNCHAR;
						state = HIT_RETURN;
					}    
					else
					{
						// any other char, just add to string
						pCurrentLink->m_sText += c;
					}
					
					break;      
				
				case HIT_RETURN:
					
					if (c == '\n')
					{
						// newline after return, ho hum    
					}
					else 
					{
						// well we've found another char, so make new link 
						pCurrentLink->m_pNextLink = new CNumLink;
						pCurrentLink = pCurrentLink->m_pNextLink;
						
						if (c == '$') 
						{
							pCurrentLink->m_nType = NL_NUMREF;
							state = BUILDING_LINKNAME;
						}
						else if ('\r' == c)
						{
							// another return char
							pCurrentLink->m_nType = NL_RETURNCHAR;
						}
						else
						{
							// general text
							pCurrentLink->m_sText += c;
							pCurrentLink->m_nType = NL_TEXT;
						}
					}     
					
					break;
				
				case BUILDING_LINKNAME:
					
					if (c == '$')
					{
						// finished linkname, built up in m_sLinkText 
						
						if (pCurrentLink->m_sLinkText == "n")
						{
							// escaped return char, not a numlink at all
							pCurrentLink->m_nType = NL_RETURNCHAR;
						}
						else
						{
							pCurrentLink->m_pNumeric = 
							       m_pSim->FetchNumeric(pCurrentLink->m_sLinkText);
							if (NULL != pCurrentLink->m_pNumeric) 
							{
								pCurrentLink->m_nPointerValid = TRUE; 
								
								// numeric should know about us in case of destruction
								pCurrentLink->m_pNumeric->m_refList.AddHead(pCurrentLink);
							}
							else
							{
								pCurrentLink->m_sText = "<not found>";
							}
						}
												
						pCurrentLink->m_pNextLink = new CNumLink;
						pCurrentLink = pCurrentLink->m_pNextLink;
						state = ADDING_TEXT;
					}
					else if (c == '\n')
					{
						// just ignore embedded returns in link text
					}
					else
					{                              
						// build up link name
						pCurrentLink->m_sLinkText += c; 
					}     
					
					break;
						
			} // end of switch
		} // end of parsing for loop
	
		return pFirstLink;				
	}	
}

int CObjTypeTextbox::CalcTargetFontSize(double dScaleFactor, int nFontSize)
{
    // static because gets called before have created any object type

    int nTargetFontSize = (int) (nFontSize * dScaleFactor / TRAD_SCREEN_SF);
    if (nTargetFontSize < 5)
    {
        nTargetFontSize = 5;
    }

    return nTargetFontSize;
}

void CObjTypeTextbox::DrawText(CDC* pDC, CNumLink* pFirstLink,
                                 int xLeft, int yTop, CSObjTextbox* pTextbox)
{
	CFont font;
	LOGFONT logfont;  
	int xCurrent = xLeft; 
    
    int xmax = xLeft;
    
	if (NULL != pTextbox)
	{	
		pTextbox->m_nXLeftLogical = xLeft;
		pTextbox->m_nYTopLogical = yTop;
		pTextbox->m_nMaxXLogical = xLeft;
		pTextbox->m_nMinYLogical = yTop;
	}
	
    int nTargetFontSize = CalcTargetFontSize(m_pSim->m_dScaleFactor, m_nFontSize);

    // note that this lot is duplicated in txtprpdg.cpp, which is a bit bad:

	logfont.lfHeight        = -nTargetFontSize; // height   
	logfont.lfWidth         = 0;   // width, use default
	logfont.lfEscapement    = 0;   // escapement           
	logfont.lfOrientation   = 0;   // orientation                
	logfont.lfWeight        = FW_NORMAL;
	logfont.lfItalic        = FALSE;   // italic                       
	logfont.lfUnderline     = FALSE;   // underline          
	logfont.lfStrikeOut     = 0;   // strikeout                    
	logfont.lfCharSet       = ANSI_CHARSET; 
	logfont.lfOutPrecision  = OUT_DEFAULT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality       = DEFAULT_QUALITY;    
	logfont.lfPitchAndFamily  = DEFAULT_PITCH;  
	strcpy(logfont.lfFaceName, m_sFontName);

	font.CreateFontIndirect(&logfont);
	
	pDC->SetBkMode(TRANSPARENT);
	                
	CFont* pOldFont = pDC->SelectObject(&font);

	while (NULL != pFirstLink) 
	{     
		switch (pFirstLink->m_nType)
		{
			case NL_NUMREF:
			
				if (pFirstLink->m_nPointerValid)
				{      
					sprintf(buff, "%g %s",
					   (double) *(pFirstLink->m_pNumeric->m_pdValue),
					    pFirstLink->m_pNumeric->m_sUnits);
					pFirstLink->m_sText = buff;
				}
				else
				{   
					pFirstLink->m_sText = "<not found>";
				}
				
				// deliberate fall-through to next case:
			case NL_TEXT: 
			
				pDC->TextOut(xCurrent + 1, yTop - 1, pFirstLink->m_sText);
				xCurrent += (pDC->GetTextExtent(pFirstLink->m_sText,
				                            pFirstLink->m_sText.GetLength())).cx; 
				
				if (xCurrent > xmax) xmax = xCurrent;                            
		        
		        break;
		                       
		    case NL_RETURNCHAR:   
		    	yTop -= (pDC->GetTextExtent("A", 1)).cy; 
		    	xCurrent = xLeft;
		    	break;   
		}
		
		pFirstLink = pFirstLink->m_pNextLink;
	}

	if (NULL != pTextbox)
	{	
		pTextbox->m_nMaxXLogical = xmax;
		pTextbox->m_nMinYLogical = yTop - (pDC->GetTextExtent("A", 1)).cy;
	}
	
	pDC->SelectObject(pOldFont);

#ifdef _DEBUG 
	if (NULL != pTextbox)
	{
		pTextbox->m_nDrawn = TRUE;
	}
#endif
}
///////////////////////////////////////////////////////////////////////////////////
//           CSObjTextbox
///////////////////////////////////////////////////////////////////////////////////

CMD_ENTRY CSObjTextbox::m_celCSObjTextboxCommands[MAX_COMMANDS_PER_OBJ] =
{                                        
	{"id",                CA_LONG,   CT_STATE,   CI_ID,                0,      2000000000},                  
	{"xleft",     CA_DOUBLE, CT_STATE,   CI_XLEFT,           LIM_MIN_X, LIM_MAX_X},
	{"ytop",      CA_DOUBLE, CT_STATE,   CI_YTOP,            LIM_MIN_Y, LIM_MAX_Y},
	{"text",      CA_STRING, CT_STATE,   CI_TEXT,            0,     LIM_MAX_CHARS_TEXTBOX},
	{"end",       CA_NONE,   CT_ENDLIST, CI_ENDLIST,         0,     0   }
};

CSObjTextbox::CSObjTextbox()
{
	m_pFirstNumLink = NULL;

#ifdef _DEBUG
	m_nDrawn = FALSE;
#endif
}

CSObjTextbox::~CSObjTextbox() 
{ 
	// delete linked list of numlinks
	
	DeleteLinks();
}

void CSObjTextbox::DeleteLinks()
{
	// delete linked list of numlinks
	
	CNumLink* pLink = m_pFirstNumLink;

	if (NULL != pLink)
	{
		CNumLink* pNextLink = pLink->m_pNextLink;
	
		while (NULL != pNextLink)
		{                       
			delete pLink;
			pLink = pNextLink;
			pNextLink = pLink->m_pNextLink;
		}       
	
		delete pLink;
	}
}

CMD_ENTRY* CSObjTextbox::GetCmdList() 
{
	return &(m_celCSObjTextboxCommands[0]);
}

int CSObjTextbox::GetStateParam(int command)
{
	int ret = SR_OK;
	
	switch(command)
	{
		case CI_XLEFT:
			m_dArg = m_dXLeft;
			break;

		case CI_YTOP:
			m_dArg = m_dYTop;
			break;              

		case CI_TEXT:
			m_sArg = m_sText;
			break;
			
		case CI_ID:
			m_lArg = m_lId;
			break;

		default:
			TRACE("Asked for param for unknown command in CSObjMol\n");
			ret = SR_ERROR;
			break;
	}             
	
	return ret;
}

int CSObjTextbox::AcceptCommandLine(CString& remainingLine, CString& errString)
{
	int command;
	int ret = SR_OK;
	
	m_fsValid.all = 0;   // mark nothing as initialised
	
	DeleteLinks();  // get rid of old version if having properties changed
    
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
			
			case CI_YTOP: 
				m_dYTop = m_dArg;
				m_fsValid.bf.ytop = 1;
				break;

			case CI_TEXT:
				m_sText = m_sArg;
				m_fsValid.bf.text = 1;
				break;
			
			default:
				errString += "confused near: " + remainingLine;
				ret |= SR_ERROR;
				break;
		}            
		
		ret |= GrabCommand(remainingLine, errString, command);            

	}
	
	if (SR_ERROR != ret)
	{
		// check that molecule initialised sufficiently
		if (!m_fsValid.bf.xleft)
		{
			// co-ords not init, no good
			ret |= SR_WARN;
			m_dXLeft = 0;      
			m_fsValid.bf.xleft = 1;
			errString += "supplied 0 as missing x co-ord; ";
		}

		if (!m_fsValid.bf.ytop)
		{
			// co-ords not init, no good
			ret |= SR_WARN; 
			m_dYTop = 0;  
			m_fsValid.bf.ytop = 1;
			errString += "supplied 0 as missing y co-ord; ";
		}
		
		if (!m_fsValid.bf.text)
		{
			ret |= SR_WARN; 
			m_sText = "wibble";  
			m_fsValid.bf.text = 1;
			errString += "supplied wibble as missing text; ";
		}    
		
		// build up links ready for display
		m_pFirstNumLink = ((CObjTypeTextbox*) m_pType)->ParseTextToLinks(m_sText);
	}
	
	return ret;
}             

int CSObjTextbox::HitTest(double dXLeft, double dYBottom, 
                                  double dXRight, double dYTop)
{
	ASSERT(m_nDrawn);
    	
	CPoint ptBotRight(m_nMaxXLogical, m_nMinYLogical);
    	
	double xr, yb;
    	
	m_pSim->LogicalToSim(ptBotRight, &xr, &yb);                 
        
    return RectsOverlap(dXLeft, dYBottom, dXRight, dYTop,
                             m_dXLeft, yb, xr, m_dYTop);
}

void CSObjTextbox::CreateTracker()
{
	CSObj::CreateTracker();

	m_pTracker->m_nHandleStyle = TRK_CORNERS_ONLY;
		
	m_pTracker->m_nAllowResize = FALSE; 
}  

void CSObjTextbox::DrawTracker(CDC* pDC)
{            
	// Need to supply tracker coordinates in device co-ords for comparison
	// to mouse pointer coords
    
    CRect rect;
	rect.left = m_nXLeftLogical;
	rect.top = m_nYTopLogical;
	rect.right = m_nMaxXLogical;
	rect.bottom = m_nMinYLogical;

	// now convert to device co-ords
	pDC->LPtoDP(&rect);
	rect.NormalizeRect();
	
	if (NULL != m_pTracker)	
	{
		m_pTracker->m_rect = rect;
		
		m_pTracker->m_nStyle = CRectTracker::resizeOutside;
			
		m_pTracker->Draw(pDC);
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CSObjTextbox::HandleMove(double dDeltaX, double dDeltaY)
{
	m_dXLeft += dDeltaX;
	m_dYTop += dDeltaY;
}

void CSObjTextbox::ObjectProperties()
{                     
	CEditTextboxDlg dlg;
	
	CString sText(m_sText);  // copy text to another string
	((CObjTypeTextbox*) m_pType)->ExpandBadChars(sText);   // expand any escaped newlines for display
	dlg.m_sUserTypedText = sText;
	dlg.m_pTextboxType = (CObjTypeTextbox*) m_pType;
	dlg.m_pSim = m_pSim;
	
	if (IDOK == dlg.DoModal())
	{            
		m_sText = dlg.m_sUserTypedText;
		((CObjTypeTextbox*) m_pType)->EscapeBadChars(m_sText);
		
		CString cmd("change textbox \"");
		cmd += m_pType->m_sName;
		cmd += "\" ";
		
		WriteState(cmd); 
	
		m_pSim->HandleNewCommand(cmd);
	}
}