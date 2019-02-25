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
#include "baseint.h" // for CNP 
#include "mnmldint.h" // to know about CMonMolNP
#include "monitor.h" 
#include "mol.h"     
#include "lennard.h"
#include "mncsetdg.h"    
#include "monprpdg.h"       
#include <math.h>  



#ifdef _DEBUG
extern long global_debug_counter;
#endif


//////////////////////////////////////////////////////////////////////                                 
// CObjTypeMonitor functionality                        
//////////////////////////////////////////////////////////////////////                                 

// constructor

// Command list
CMD_ENTRY CObjTypeMonitor::m_celCObjTypeMonitorCommands[MON_OBJTYPE_CMDS] = 
{                                        
	{"name",          CA_STRING, CT_STATE,   CI_NAME,        LIM_MIN_CHARS_OBJTYPE, LIM_MAX_CHARS_OBJTYPE   }, 
	{"destructor",    CA_NONE,   CT_CONTROL, CI_DESTRUCTOR,  0, 0   },
	{"fixer",         CA_NONE,   CT_CONTROL, CI_FIXER,       0, 0   },
	{"heater",        CA_NONE,   CT_CONTROL, CI_HEATER,      0, 0   }, 
	{"hatch",         CA_INT,    CT_STATE,   CI_HATCH,       0, 1   }, 
	{"label",         CA_INT,    CT_STATE,   CI_LABEL,       0, 1   }, 
	{"linethick",     CA_INT,    CT_STATE,   CI_LINETHICK,   LIM_MIN_MONITOR_THICK, LIM_MAX_MONITOR_THICK  },
	{"red",           CA_BYTE,   CT_STATE,   CI_RED,         0, 255 },
	{"green",         CA_BYTE,   CT_STATE,   CI_GREEN,       0, 255 },
	{"blue",          CA_BYTE,   CT_STATE,   CI_BLUE,        0, 255 },
	{"end",           CA_NONE,   CT_ENDLIST, CI_ENDLIST,     0, 0   }
};

// Command handlers

// Functionality overridden from generic CObjType 


int CObjTypeMonitor::GetStateParam(int command)
{
	int ret = SR_OK;
	
	switch(command)
	{               
		case CI_NAME:
			m_sArg = m_sName;
			break;

		case CI_RED:
			m_bArg = (unsigned char) (m_cColour & 0x000000ff); 
			break;
			
		case CI_GREEN:
			m_bArg = (unsigned char) ((m_cColour >> 8) & 0x000000ff); 
			break;
			
		case CI_BLUE:
			m_bArg = (unsigned char) ((m_cColour >> 16) & 0x000000ff); 
			break;                                         
		
		case CI_HATCH:
			m_nArg = m_nHatch;
			break;
			
		case CI_LABEL:
			m_nArg = m_nLabel;
			break;
			
		case CI_LINETHICK:
			m_nArg = m_nLineThick;
			break;
				
		default:
			TRACE("Asked for param for unknown command in CObjTypeMonitor\n");
			ret = SR_ERROR;
			break;
	}             
	
	return ret;
}

CMD_ENTRY* CObjTypeMonitor::GetCmdList()
{
	return &(m_celCObjTypeMonitorCommands[0]);
}      

int CObjTypeMonitor::AcceptCommandLine(CString& remainingLine, CString& errString)
{
	int command;
	int ret;
    
    // initially make it do nothing so only does things specified;
    // then if properties edited later, things not later specified
    // will be removed.
	m_nControlType = MCNTRL_NONE;
    
    ret = GrabCommand(remainingLine, errString, command);            
    while ((CI_EMPTY != command) && (SR_ERROR != ret))
    {
		switch (command)
		{   
			case CI_NAME:
				m_sName = m_sArg; 
				break;      
			
			case CI_DESTRUCTOR:
				if (MCNTRL_NONE != m_nControlType) 
				{
					ret |= SR_WARN;
					errString += "switching control type to destructor; ";
				}
				m_nControlType = MCNTRL_DESTROY;
				break;
				
			case CI_FIXER:
				if (MCNTRL_NONE != m_nControlType) 
				{
					ret |= SR_WARN;
					errString += "switching control type to fixer; ";
				}
				m_nControlType = MCNTRL_FIX_DENS_TEMP;
				break;
				
			case CI_HEATER:
				if (MCNTRL_NONE != m_nControlType) 
				{
					ret |= SR_WARN;
					errString += "switching control type to heater; ";
				}
				m_nControlType = MCNTRL_HEATER;
				break; 
				
			case CI_LINETHICK:
				m_nLineThick = m_nArg;
				break;      
				
			case CI_HATCH:
				m_nHatch = m_nArg;
				break;
				
			case CI_LABEL:
				m_nLabel = m_nArg;
				break;

			case CI_RED:	
				m_cColour &= 0x00ffff00;
				m_cColour |= m_bArg;
				break;
				
			case CI_GREEN:
				m_cColour &= 0x00ff00ff;
				m_cColour |= (COLORREF) m_bArg << 8;
				break;

			case CI_BLUE:
				m_cColour &= 0x0000ffff;
				m_cColour |= (COLORREF) m_bArg << 16;
				break;
							
				
			default:
				ret |= SR_ERROR;
				errString += "confused near: " + remainingLine;
				break;
		}
		
        ret |= GrabCommand(remainingLine, errString, command);
    
	}              
	
	if (SR_ERROR != ret)
	{
		// prepare list of mol types etc; done anyway when sim
		// run, but CSObjMonitors of this type may be made before then
		
		Prepare();
	}
	  
	return ret;
}

int CObjTypeMonitor::WriteState(CString& workingLine)
{
	// need to override the default in order to list types
	// of control
	                         
	switch (m_nControlType)
	{  
		case MCNTRL_NONE:
			break;
			
		case MCNTRL_DESTROY:
			workingLine += "destructor ";
			break;

		case MCNTRL_FIX_DENS_TEMP:
			workingLine += "fixer ";
			break;

		case MCNTRL_HEATER:
			workingLine += "heater ";
			break;
	}            
	
	// Now call the base version to handle the simple parameters
	return CSimObj::WriteState(workingLine);
}

CObjTypeMonitor::CObjTypeMonitor()
{                      
	// initialise variables to reasonable state
	
	m_nControlType = MCNTRL_NONE;        
	m_cColour = RGB(255, 0, 0);
	m_nLabel = TRUE;
	m_nHatch = FALSE;
	m_nLineThick = 2;     
	
	m_nHasInts = TRUE;
	m_nIsMobile = FALSE; 
	
	for (int i = 0; i < MON_OUTLINE_BITMAP_SIZE; i++)
	{
		m_ppOutlineBrush[i] = NULL;
	}
	
	m_pLabelFont = NULL;
	
	m_cColour = RGB(255, 0, 0);
	m_nLineThick = 2;
	m_nHatch = FALSE;
	m_nLabel = TRUE;  
	
	// TODO check constructor rules, originally had: CObjType::CObjType();
}                       

CObjTypeMonitor::~CObjTypeMonitor()
{
	for (int i = 0; i < MON_OUTLINE_BITMAP_SIZE; i++)
	{                       
		delete m_ppOutlineBrush[i];
	}
	
	delete m_pLabelFont;
}                       

void CObjTypeMonitor::DrawAllSObjs(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2)
{       
	if (NULL == m_ppOutlineBrush[0])
	{                             
		for (int i = 0; i < MON_OUTLINE_BITMAP_SIZE; i++)
		{
			m_ppOutlineBrush[i] = CreateOutlineBrush(pDC, m_cColour, i);
		}
	}
	
	if (NULL == m_pLabelFont)
	{  
		m_pLabelFont = CreateLabelFont();
	}
	          
	CSObjMonitor* pMonitor;
	pMonitor = (CSObjMonitor*) m_pFirstSObj;
	
	CPoint ptTopLeft, ptBottomRight;                                
	
	CSize labelCornerOffset; // from top-left of monitor in logicals
	
	while (NULL != pMonitor)
	{                                              
		ptTopLeft = m_pSim->SimToLogical(pMonitor->m_dXLeft, pMonitor->m_dYTop); 
		ptBottomRight = m_pSim->SimToLogical(pMonitor->m_dXRight, pMonitor->m_dYBottom);
		
		sprintf(buff, "%s:%ld", (const char*) m_sName, pMonitor->m_lId); 
		
		labelCornerOffset = DrawMonitor(pDC,
		            ptTopLeft.x,
		            ptTopLeft.y, 
		            ptBottomRight.x, 
		            ptBottomRight.y,
		            m_nLineThick,
		            m_nHatch,
		            m_nLabel,
		            buff,
		            m_ppOutlineBrush[pMonitor->m_nOutline],
		            m_pLabelFont,
		            m_cColour);
		            
		pMonitor->m_dTextCornerXOffset = labelCornerOffset.cx / m_pSim->m_dScaleFactor;
		pMonitor->m_dTextCornerYOffset = labelCornerOffset.cy / m_pSim->m_dScaleFactor;
		pMonitor->m_nDrawn = TRUE;

		// Make border shimmy if this is a fixer type monitor, or a heater
		// when that is switched on:
		
		if ((MCNTRL_FIX_DENS_TEMP == m_nControlType)
		    || (MCNTRL_HEATER == m_nControlType && (pMonitor->m_dHeaterOn >= 0.5)))
		{
			pMonitor->m_nOutline++;
			if (pMonitor->m_nOutline >= MON_OUTLINE_BITMAP_SIZE) pMonitor->m_nOutline = 0; 
		}
		            
		pMonitor = (CSObjMonitor*) pMonitor->m_pNextSObj;
	}                                          
	
}

void CObjTypeMonitor::UserDrawing(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2)
{                                                         
	// This is where we draw cursors, monitor in progress of being drawn etc.
    CPoint currentPt;
    
    CPen wallPen(PS_SOLID, 2, 
                 (COLORREF) RGB(255, 0, 0));
                                
    CPen* pOldPen;
    
	switch (m_pSim->m_nDrawingState)
	{                     
		case SM_AWAITING_CLICK1:     
		case SM_BUTDN_NOT_MOVED1:      
			// draw cursor at nearest gridpoint 
			currentPt = DrawCrosshair(pDC); 
			break;
			
		case SM_DRAGGING:
		case SM_AWAITING_CLICK2:
			currentPt = DrawCrosshair(pDC);    
			pOldPen = pDC->SelectObject(&wallPen);
			pDC->MoveTo(m_pSim->m_ptLogDragStart);
			pDC->LineTo(m_pSim->m_ptLogDragStart.x, currentPt.y); 
			pDC->LineTo(currentPt);
			pDC->LineTo(currentPt.x, m_pSim->m_ptLogDragStart.y);
			pDC->LineTo(m_pSim->m_ptLogDragStart);
			pDC->SelectObject(pOldPen);
			break;

		case SM_NOT_DRAWING:
		default:
			// unknown state
			ASSERT(FALSE);
			break;
	}

}

void CObjTypeMonitor::OnLButtonUp()
{
	if ((SM_AWAITING_CLICK2 == m_pSim->m_nDrawingState)
	    ||(SM_DRAGGING == m_pSim->m_nDrawingState))
	{
		double xl = m_pSim->m_dSimDragStartX;
		double yt = m_pSim->m_dSimDragStartY;
		double xr = m_pSim->m_dSimMouseX;
		double yb = m_pSim->m_dSimMouseY;
			
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
			
			
		// now invoke dialogue for density/pressure fixing, heater
		// power etc 
			
		CMonitorCtrlSetDlg dlg;
		dlg.m_pMonitorType = this;
		dlg.m_pSim = m_pSim;  
			
		if (IDOK == dlg.DoModal())
		{			
			CString cmd;

			dlg.m_pMonitor->m_dXLeft = xl;
			dlg.m_pMonitor->m_dXRight = xr;
			dlg.m_pMonitor->m_dYTop = yt;
			dlg.m_pMonitor->m_dYBottom = yb;
						
			cmd = "monitor \"";
			cmd += m_sName;
			cmd += "\" ";
			dlg.m_pMonitor->WriteState(cmd);
				
			// need to get rid of phoney id and mon_uid for this monitor
			// as these are added uniquely when the real one is made; getting
			// CSObjMonitor to do this as it knows about relevant commands
				
			dlg.m_pMonitor->SnipCommand(cmd, CI_ID);
			dlg.m_pMonitor->SnipCommand(cmd, CI_MON_UID);
				
			if (SR_OK != m_pSim->HandleNewCommand(cmd))
			{
				AfxMessageBox("Errors or warnings occurred -- see console for details");
			}
		}			
		else
		{
			// user cancelled
		}

		// reset drawing state machine whether making monitor or not
		m_pSim->m_nDrawingState = SM_NOT_DRAWING;
		if (SM_SIM_PAUSED == m_pSim->m_nSimState) 
		{
			m_pSim->m_nSimState = SM_SIM_RUNNING;
		}           
	}    
	else
	{
		// not a special state we're interested in
		
		ChangeDrawingStateOnLButtonUp();
	}
}

void CObjTypeMonitor::PropertiesCommand()
{
	CEditMonitorPropsDlg dlg; 
	
	dlg.m_sName = m_sName;
	dlg.m_nControlType = m_nControlType;
	dlg.m_cColour = m_cColour;
	dlg.m_nHatch = m_nHatch;
	dlg.m_nLabel = m_nLabel;
	dlg.m_nLineThick = m_nLineThick;
	
	if (IDOK == dlg.DoModal())
	{
		CString cmd("change objtype monitor name \"");
		cmd += dlg.m_sName + "\" ";
		
		switch (dlg.m_nControlType)
		{
			case MCNTRL_NONE: 
				break;
				
			case MCNTRL_DESTROY:
				cmd += "destructor";
				break;
				
			case MCNTRL_FIX_DENS_TEMP:
				cmd += "fixer";
				break;
				
			case MCNTRL_HEATER: 
				cmd += "heater";
				break;
				
			default:
				ASSERT(FALSE);
				break;
		}
		
		cmd += " hatch";
		
		if (dlg.m_nHatch)
		{
			cmd += " 1";
		}
		else
		{
			cmd += " 0";
		}
		

		cmd += " label";
		
		if (dlg.m_nLabel)
		{
			cmd += " 1";
		}
		else
		{
			cmd += " 0";
		} 
		
		
		sprintf(buff, " linethick %d", dlg.m_nLineThick);
		
		cmd += buff;
		
		sprintf(buff, " red %d green %d blue %d",
		                (int) (dlg.m_cColour & 0x000000ff),
		                (int) ((dlg.m_cColour & 0x0000ff00) >> 8),
		                (int) ((dlg.m_cColour & 0x00ff0000) >> 16));
		                                    
		cmd += buff;		                                    
		                                    
		
		if (SR_OK != m_pSim->HandleNewCommand(cmd))
		{
			AfxMessageBox("Warnings or errors occurred -- see Console");
		}
	}
}

void CObjTypeMonitor::ContributeNumerics()
{   
	// no type ones to contribute yet
}                         

void CObjTypeMonitor::Initialise()
{                                 
	// re-form brush and font used in case properties changes since sim
	// last ran. Needs CDC to do brush, so done in draw routine; just
	// invalidate any existing one here

	for (int i = 0; i < MON_OUTLINE_BITMAP_SIZE; i++)
	{	
		delete m_ppOutlineBrush[i];
		m_ppOutlineBrush[i] = NULL;
	}
	
	delete m_pLabelFont;
	m_pLabelFont = NULL;
}
          
void CObjTypeMonitor::Prepare()
{
	// sim will call this when simulation is about to run, giving
	// monitor a chance to get up-to-date list of mol types, initialise
	// totals, etc   
	// Also done when monitor type created or props changed       

	// Must handle sim having added or deleted mol types since last time
		
	int nThisIndex = m_pSim->GetObjTypeIndexByPtr(this);
	
	ASSERT(SR_NO_SUCH_TYPE != nThisIndex);
	
	// Scan all types, looking for mol types we have interactions with,
	// and add those to list
	
	CObjType* pType;
	CInt* pInt;   
	int nMolTypes = 0;

	CPtrArray newList;

    int i;
	for (i = 0; i <= m_pSim->m_typeArray.GetUpperBound(); i++)
	{
		pType = (CObjType*) m_pSim->m_typeArray[i];
		
		if (CI_MOL == pType->m_nBaseType)
		{                                
			// is a mol type, so get interaction between this and it
			
			pInt = m_pSim->GetIntInUseByIndex(nThisIndex, i);
			
			if (!(pInt->m_nIsNull)) // if there is interaction
			{
				newList.Add(pType);   
				nMolTypes++;
			}
		}
	}   
	
	// now have m_molTypesRefd, the old list of mol types, and newList
	// as they should now be. Need to delete types no longer present and
	// add new ones, and reflect these changes in the arrays of CMolTypeStats
	// in each monitor of this type.
    
    CSObjMonitor* pMonitor;
	CMolTypeStats* pStats;
		
	// make existing lists big enough for new list of mol types if not already
	
	if (m_molTypesRefd.GetSize() < nMolTypes)	
	{
		m_molTypesRefd.SetSize(nMolTypes);

		pMonitor = (CSObjMonitor*) m_pFirstSObj;  
		while (NULL != pMonitor)
		{                                                    
			pMonitor->m_molStatsList.SetSize(nMolTypes);
			pMonitor = (CSObjMonitor*) pMonitor->m_pNextSObj;
		}
	}
	
	void* pNewType;

	for (int nNewType = 0; nNewType <= nMolTypes - 1; nNewType++)
	{
		pNewType = newList[nNewType];
		
		if (m_molTypesRefd[nNewType] == pNewType)
		{
			// old and new elements the same, no action required
		}
		else
		{
			// see if type was already referred to but further down list
			
			int found = FALSE;
			
			for (int i = nNewType + 1; i <= m_molTypesRefd.GetUpperBound(); i++)
			{
				if (m_molTypesRefd[i] == pNewType)
				{
					// yes, this type further down list, so shuffle it down
					
					m_molTypesRefd.RemoveAt(i);
					m_molTypesRefd.InsertAt(nNewType, pNewType);
					pMonitor = (CSObjMonitor*) m_pFirstSObj;  
					while (NULL != pMonitor)
					{  
						pStats = (CMolTypeStats*) pMonitor->m_molStatsList[i];
						pMonitor->m_molStatsList.RemoveAt(i);
						pMonitor->m_molStatsList.InsertAt(nNewType, pStats);
						pMonitor = (CSObjMonitor*) pMonitor->m_pNextSObj;
					}
					found = TRUE;
					break;
				}
			} 
			
			if (!found)
			{
				// Must be a mol type introduced since last time, so insert it
				m_molTypesRefd.InsertAt(nNewType, pNewType);
				pMonitor = (CSObjMonitor*) m_pFirstSObj;  
				while (NULL != pMonitor)
				{   
					pStats = new CMolTypeStats;              
					pStats->m_pMolType = (CObjTypeMol*) pNewType;
					pStats->Initialise();
					pStats->ContributeNumerics(m_pSim, this, pMonitor->m_lId);
					pMonitor->m_molStatsList.InsertAt(nNewType, pStats);
					pMonitor = (CSObjMonitor*) pMonitor->m_pNextSObj;
				}
			}
		}
	}
	
	// finally trim off any mol types that are no longer referenced
	int surplus = m_molTypesRefd.GetSize() - nMolTypes;

	for (i = 1; i <= surplus; i++)
	{   
		int upper = m_molTypesRefd.GetUpperBound();
		m_molTypesRefd.RemoveAt(upper);

		pMonitor = (CSObjMonitor*) m_pFirstSObj;  
		while (NULL != pMonitor)
		{                                                    
			pStats = (CMolTypeStats*) pMonitor->m_molStatsList[upper];
			pMonitor->m_molStatsList.RemoveAt(upper);
			delete pStats;
			pMonitor = (CSObjMonitor*) pMonitor->m_pNextSObj;
		}
	}
	
	                  
	m_nMolTypesRefd = nMolTypes;

	pMonitor = (CSObjMonitor*) m_pFirstSObj;  
	while (NULL != pMonitor)
	{                                                    
		pMonitor->m_nMolTypes = m_nMolTypesRefd;
		pMonitor = (CSObjMonitor*) pMonitor->m_pNextSObj;
	}
	                                               
	                                               
	// Clear totals in stats arrays of each monitor.
	
	pMonitor = (CSObjMonitor*) m_pFirstSObj;  
	
	while (NULL != pMonitor)
	{                                                    
		pMonitor->InitialiseStatsArray(m_nMolTypesRefd);   
		pMonitor = (CSObjMonitor*) pMonitor->m_pNextSObj;
	}
}        

void CObjTypeMonitor::UpdateQuantities()
{
	// called at end of each timestep
	// can do any grand totals of all monitors of this type
	// here, otherwise delegate to individual monitors
	
	CSObjMonitor* pMonitor = (CSObjMonitor*) m_pFirstSObj;
	
	while (NULL != pMonitor)
	{                                                    
		pMonitor->UpdateQuantities();
		pMonitor = (CSObjMonitor*) pMonitor->m_pNextSObj;
	}
}

int CObjTypeMonitor::FindStatIndex(CString& sMolType)
{
	CObjTypeMol* pMolType = (CObjTypeMol*) m_pSim->GetObjTypeByName(sMolType);
	
	if (NULL != pMolType)
	{
		return FindStatIndex(pMolType);
	}                                  
	else
	{
		return SR_NO_SUCH_TYPE;
	}
}
	
int CObjTypeMonitor::FindStatIndex(CObjTypeMol* pMolType)
{
	// find mol type in array of referenced types and return index;
	// same array indices used in each CSObjMonitor for its array
	// of stats lists kept for each mol type
	
	int i;
	
	for (i = 0; i <= m_molTypesRefd.GetUpperBound(); i++)
	{ 
		if ((CObjTypeMol*) (m_molTypesRefd[i]) == pMolType)
		{
			return i;
		}
	}     
	
	ASSERT(FALSE);         
	
	return SR_NO_SUCH_TYPE;
}                                        

CBrush* CObjTypeMonitor::CreateOutlineBrush(CDC* pDC, COLORREF cColour, int nOffset)
{                        
	// When you make a patterned brush from a bitmap, always get a monochrome
	// one, so the supplied colour is in fact ignored here; use black instead,
	// else light colours end up with no bits set at all.
	
	CBitmap* pBitmap = new CBitmap;
	CDC dc;
	CBrush* pBrush = new CBrush;
	
	dc.CreateCompatibleDC(pDC);

    dc.SetMapMode(MM_TEXT); // so correctly deal with bits

	// Want our bitmap to be MON_OUTLINE_BITMAP_SIZE pixels across (max 8
	// or CBrush made from it won't work as expected -- see Help for CBrush)
	
	pBitmap->CreateCompatibleBitmap(&dc, MON_OUTLINE_BITMAP_SIZE, MON_OUTLINE_BITMAP_SIZE);

	// Save device context as will be selecting new bitmap, changing map mode etc
	int nSavedDCContext = dc.SaveDC();
	ASSERT(nSavedDCContext);	
    
    
	// select newly made bitmap into device context for drawing on
	dc.SelectObject(pBitmap);             
	
	// white background
	dc.SelectStockObject(WHITE_BRUSH);  
	dc.PatBlt(0, 0, MON_OUTLINE_BITMAP_SIZE, MON_OUTLINE_BITMAP_SIZE, PATCOPY);
	
	// now fill in diagonal pattern of bits such that this tile contains
	// portions of two stripes, half white and half foreground colour, with
	// first foreground bit in top row being at nOffset.

	if (nOffset < 0)
	{
		nOffset = 0;
		ASSERT(FALSE);
	}
	else if (nOffset > MON_OUTLINE_BITMAP_SIZE - 1)	
	{
		nOffset = MON_OUTLINE_BITMAP_SIZE - 1;
		ASSERT(FALSE);
	}
	
	for (int y = 0; y < MON_OUTLINE_BITMAP_SIZE; y++)
	{
		for (int x = 0; x < MON_OUTLINE_BITMAP_SIZE; x++)
		{                                             
			nOffset--;
			
			if (nOffset < 0) nOffset += MON_OUTLINE_BITMAP_SIZE;
			
			if (nOffset >= MON_OUTLINE_BITMAP_SIZE / 2)
			{
				dc.SetPixel(x, y, (COLORREF) 0); /* colour ignored, always get B&W anyway */
			}
		}    
		
		nOffset--;  // to stagger rows by one pixel
	}

	
	dc.RestoreDC(nSavedDCContext);		
	
	pBrush->CreatePatternBrush(pBitmap);
	                            
	delete pBitmap;
		                            
	return pBrush;	
}

CFont* CObjTypeMonitor::CreateLabelFont()
{                  
	CFont* pFont = new CFont;
	pFont->CreateFont(-10, // int nHeight,
	                           0, // int nWidth,
	                           0, // int nEscapement, 
                               0, // int nOrientation, 
                               FW_NORMAL, // int nWeight, 
                               FALSE, // BYTE bItalic, 
                               FALSE, // BYTE bUnderline, 
                               0, // BYTE cStrikeOut,
                               ANSI_CHARSET, // BYTE nCharSet, 
                               OUT_DEFAULT_PRECIS, // BYTE nOutPrecision, 
                               CLIP_DEFAULT_PRECIS, // BYTE nClipPrecision, 
                               DEFAULT_QUALITY, // BYTE nQuality, 
                               DEFAULT_PITCH, // BYTE nPitchAndFamily,  
                               "Arial"); // LPCSTR lpszFacename
	
	return pFont;
}
	
CSize CObjTypeMonitor::DrawMonitor(CDC* pDC,
	                              int nXLeft, // logical co-ords
	                              int nYTop,
	                              int nXRight,
	                              int nYBottom, 
	                              int nThickness,
	                              int nHatch,
	                              int nLabel,
	                              const char* sLabel,
	                              CBrush* pOutlineBrush,
	                              CFont* pLabelFont,
	                              COLORREF cColour)
{                  
	pDC->SetROP2(R2_MASKPEN);  // so mols show through hatching etc
	
	CRgn rgn;
	CRect rect(nXLeft, nYTop, nXRight, nYBottom);
	

	// I don't think top and bottom need be inverted like this, but
	// it gets it wrong otherwise (filling in whole rect):	
	rgn.CreateRectRgn(rect.left, rect.bottom, rect.right, rect.top);
    
    // Making a patterned brush always gets you a monochrome one,
    // which applies the CDC text colour when used. So temporarily
    // change the text colour to be the colour we want the brush to be:
	COLORREF oldColour = pDC->GetTextColor();
	pDC->SetTextColor(cColour);
	pDC->FrameRgn(&rgn, pOutlineBrush, nThickness, nThickness);   
	pDC->SetTextColor(oldColour);
	
	if (nHatch)
	{   
		// again, have treated y-axis differently here to get it to work:
		rect.InflateRect(-nThickness, nThickness);                       
		
		CRgn innerRgn;
		innerRgn.CreateRectRgn(rect.left, rect.bottom, rect.right, rect.top);
		CBrush hatchBrush(HS_DIAGCROSS, cColour);
		
		CBrush* pOldBrush = pDC->SelectObject(&hatchBrush);
		
		pDC->FillRgn(&innerRgn, &hatchBrush);
		pDC->SelectObject(pOldBrush);
	}

	CSize labelCornerOffset;  // from top-left of monitor to bot right of label text

#define TEXT_RIGHT_OFFSET 4
#define TEXT_DOWN_OFFSET  4
	
	if (nLabel)
	{                               
		CFont* pOldFont = pDC->SelectObject(pLabelFont);
		pDC->TextOut(nXLeft + TEXT_RIGHT_OFFSET, nYTop - TEXT_DOWN_OFFSET , sLabel);
		labelCornerOffset = pDC->GetTextExtent(sLabel, strlen(sLabel));
		labelCornerOffset.cx += TEXT_RIGHT_OFFSET;
		labelCornerOffset.cy += TEXT_DOWN_OFFSET;
		pDC->SelectObject(pOldFont);
	}                                            
	else
	{
		labelCornerOffset.cx = 0;
		labelCornerOffset.cy = 0;
	}
		
	return labelCornerOffset;
}                

///////////////////////////////////////////////////////////////////////////////////
//                CMolTypeStats
///////////////////////////////////////////////////////////////////////////////////
CMolTypeStats::CMolTypeStats()
{    
	m_nTemperatureFixed = FALSE;
	m_dFixedTemperature = 100;
	m_nDensityFixed = FALSE;
	m_dFixedDensity = 10;	
	m_pFirstMonMolNP = NULL;
	m_pLastMonMolNP = NULL;
	m_lLastMolIdUsed = 0;

	Initialise();
}

void CMolTypeStats::Initialise()
{
	m_lNumMolsInside = 0;    
	m_dNumMolsInside = 0; 
	m_lCountExitsTop = 0;
	m_lCountExitsBottom = 0;
	m_lCountExitsLeft = 0;
	m_lCountExitsRight = 0;  
	m_dCountExitsTop = 0;
	m_dCountExitsBottom = 0;
	m_dCountExitsLeft = 0;
	m_dCountExitsRight = 0;  
	m_dTemperature = 0;   
	m_dTemperatureS = 0;
	m_dAvgKE = 0;       
	m_dAvgKES = 0;
	m_dAvgPE = 0;        
	m_dAvgPES = 0; 
	m_dTotalEnergy = 0; 
	m_dTotalEnergyS = 0; 
	m_dPartialPressure = 0;
	m_dPartialPressureS = 0;
	m_dDensity = 0;
	m_dDensityS = 0; 
	m_dMolsToCreate = 0;
}

CMolTypeStats::~CMolTypeStats()
{                          
	// delete list of attached CMonMolNP objects; destruction of
	// int will remove those attached to it, so here need only
	// destroy those in interior that are not still in mol-monitor
	// neighbour list maintained by int

	CMonMolNP* pCurrentNP = m_pFirstMonMolNP;
	CMonMolNP* pNextNP;
		
	while (NULL != pCurrentNP)
	{    
		pNextNP = pCurrentNP->m_pNextInteriorNP;
		
		if (!(pCurrentNP->m_nFlags & MNML_PROXIMAL))
		{
			// This NP was only in interior list

            // Set parent interaction for this NP to NULL so that it doesn't
            // try and unlink itself from ordinary interaction chain. That
            // would cause problems because it will think it is at the start and
            // end of the chain (because next and prev pointers NULL), and so
            // will try and set interaction list empty when may not be.
            pCurrentNP->m_pInt = NULL;

			delete pCurrentNP;
		}
			
		pCurrentNP = pNextNP;
	}
}

void CMolTypeStats::NoteIntDying()
{
    CMonMolNP* pNP = m_pFirstMonMolNP;

    while (NULL != pNP)
    {
        pNP->m_pInt = NULL;
        pNP = pNP->m_pNextInteriorNP;
    }

}

          
///////////////////////////////////////////////////////////////////////////////////
//           CSObjMonitor
///////////////////////////////////////////////////////////////////////////////////

CMD_ENTRY CSObjMonitor::m_celCSObjMonitorCommands[MAX_SMON_COMMANDS_PER_OBJ] =
{                                                      
	{"id",        CA_LONG,   CT_STATE,   CI_ID,         0,      2000000000},
	{"mon_uid",   CA_LONG,   CT_STATE,   CI_MON_UID,    1,  2000},
	{"xleft",     CA_DOUBLE, CT_STATE,   CI_XLEFT,   LIM_MIN_X,  LIM_MAX_X},
	{"xright",    CA_DOUBLE, CT_STATE,   CI_XRIGHT,  LIM_MIN_X,  LIM_MAX_X},
	{"ytop",      CA_DOUBLE, CT_STATE,   CI_YTOP,    LIM_MIN_Y,  LIM_MAX_Y},
	{"ybottom",   CA_DOUBLE, CT_STATE,   CI_YBOTTOM, LIM_MIN_Y,  LIM_MAX_Y},    
	{"moltype",   CA_STRING, CT_CONTROL, CI_MOLTYPE,    LIM_MIN_CHARS_OBJTYPE,    LIM_MAX_CHARS_OBJTYPE },
	{"fixt",      CA_DOUBLE, CT_CONTROL, CI_FIXT,       LIM_MIN_TEMP,  LIM_MAX_TEMP},
	{"nofixt",    CA_DOUBLE, CT_CONTROL, CI_NOFIXT,     LIM_MIN_TEMP,  LIM_MAX_TEMP},
	{"fixdens",   CA_DOUBLE, CT_CONTROL, CI_FIXDENS,    LIM_MIN_CTRL_DENSITY,  LIM_MAX_CTRL_DENSITY },
	{"nofixdens", CA_DOUBLE, CT_CONTROL, CI_NOFIXDENS,  LIM_MIN_CTRL_DENSITY,  LIM_MAX_CTRL_DENSITY },
	{"power_on",  CA_DOUBLE, CT_CONTROL, CI_POWER_ON,  LIM_MIN_MON_POWER, LIM_MAX_MON_POWER},
	{"power_off", CA_DOUBLE, CT_CONTROL, CI_POWER_OFF, LIM_MIN_MON_POWER, LIM_MAX_MON_POWER},
	{"end",       CA_NONE,   CT_ENDLIST, CI_ENDLIST,     0,    0}
};

CMD_ENTRY* CSObjMonitor::GetCmdList() 
{
	return &(m_celCSObjMonitorCommands[0]);
}

CSObjMonitor::CSObjMonitor()
{      
	m_dTotalTemperature = 0; 
	m_dTotalTemperatureS = 0;
	m_dAvgKE = 0;
	m_dAvgKES = 0;
	m_dAvgPE = 0;
	m_dAvgPES = 0;
	m_dTotalEnergy = 0;
	m_dTotalEnergyS = 0;
	m_dTotalPressure = 0;
	m_dTotalPressureS = 0;
	m_dDensity = 0;
	m_dDensityS = 0;
	m_nDrawn = FALSE;
	m_dTextCornerXOffset = 0;
	m_dTextCornerYOffset = 0;
	m_nOutline = 0; 
	m_nMolStatsBuilt = FALSE; 
	m_lTotalNumMols = 0;
	m_dTotalNumMols = 0;
	m_nMolTypes = 0; 
	m_nFirstPass = TRUE;
	m_lUniqueMonId = 0;
	m_dHeaterOn = 0;
	m_dHeaterPower = 10;
	m_dEnergyToGive = 0;
}

int CSObjMonitor::GetStateParam(int command)
{
	int ret = SR_OK;
	
	switch(command)
	{
		case CI_ID:
			m_lArg = m_lId;
			break;    
			
		case CI_MON_UID:
			m_lArg = m_lUniqueMonId;
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

		default:
			TRACE("Asked for param for unknown command in CSObjMonitor\n");
			ret = SR_ERROR;
			break;
	}             
	
	return ret;
}

int CSObjMonitor::AcceptCommandLine(CString& remainingLine, CString& errString)
{
	int command;
	int ret = SR_OK; 
	
	int nMolType = SR_NO_SUCH_TYPE;     // none yet selected
    CMolTypeStats* pStats = NULL;    // for currently selected mol type 
    
    
    // If coming through here to change properties, we may already
    // have mol stats list; if not, make it:
    
    if (!m_nMolStatsBuilt)
    {
    	BuildStatsArray(((CObjTypeMonitor*) m_pType)->m_nMolTypesRefd);
    	m_nMolStatsBuilt = TRUE;
    }
    
    
    ret = GrabCommand(remainingLine, errString, command);            
    while ((CI_EMPTY != command) && (SR_ERROR != ret))
    {
		switch (command)
		{   
			case CI_ID:
				m_lId = m_lArg;
				break;

			case CI_MON_UID:
				m_lUniqueMonId = m_lArg;
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
			
			case CI_POWER_ON:
				m_dHeaterOn = 1;
				m_dHeaterPower = m_dArg;
				break;
				
			case CI_POWER_OFF:
				m_dHeaterOn = 0;
				m_dHeaterPower = m_dArg;
				break;
			
			case CI_MOLTYPE:
				// get mol type which subsequent fixer temps and densities
				// will refer to        
				nMolType = ((CObjTypeMonitor*) m_pType)->FindStatIndex(m_sArg);
				
				if (SR_NO_SUCH_TYPE == nMolType)
				{
					ret |= SR_WARN;
					errString += "monitor doesn't use mol type: ";
					errString += m_sArg + "; ";
				}                  
				else
				{
					pStats = (CMolTypeStats*) m_molStatsList[nMolType];
				}
					
				break;      
				
			case CI_FIXT:
				if (SR_NO_SUCH_TYPE != nMolType)
				{
					pStats->m_nTemperatureFixed = TRUE;
					pStats->m_dFixedTemperature = m_dArg;
				}
				else
				{
					ret |= SR_WARN;
					errString += "no mol type specified for fixt; ";
				}            
				break;

			case CI_NOFIXT:
				if (SR_NO_SUCH_TYPE != nMolType)
				{
					pStats->m_nTemperatureFixed = FALSE;
					pStats->m_dFixedTemperature = m_dArg;
				}
				else
				{
					ret |= SR_WARN;
					errString += "no mol type specified for nofixt; ";
				}            
				break;

			case CI_FIXDENS:
				if (SR_NO_SUCH_TYPE != nMolType)
				{
					pStats->m_nDensityFixed = TRUE;
					pStats->m_dFixedDensity = m_dArg;
				}
				else
				{
					ret |= SR_WARN;
					errString += "no mol type specified for fixd; ";
				}            
				break;

			case CI_NOFIXDENS:
				if (SR_NO_SUCH_TYPE != nMolType)
				{
					pStats->m_nDensityFixed = FALSE;
					pStats->m_dFixedDensity = m_dArg;
				}
				else
				{
					ret |= SR_WARN;
					errString += "no mol type specified for nofixd; ";
				}            
				break;
							
			default:
				errString += "confused near: " + remainingLine;
				ret |= SR_ERROR;
				break;
		}            
		
		ret |= GrabCommand(remainingLine, errString, command);            

	}
	
	
	// ensure co-ords expected way round
	
	double dSwap;
	
	if (m_dXLeft > m_dXRight)
	{
		dSwap = m_dXLeft;
		m_dXLeft = m_dXRight;
		m_dXRight = dSwap;
		errString += "swapped left and right co-ords; ";
		ret |= SR_WARN;
	}
	
	if (m_dYBottom > m_dYTop)
	{
		dSwap = m_dYBottom;
		m_dYBottom = m_dYTop;
		m_dYTop = dSwap;
		errString += "swapped top and bottom co-ords; ";
		ret |= SR_WARN;
	}           
	
	// calculate monitor area in nm2
	
	m_dArea = (m_dXRight - m_dXLeft) * (m_dYTop - m_dYBottom);
	
	if (m_dArea < NEARLY_ZERO)
	{
		ret |= SR_ERROR;
		errString += "illegal attempt to create monitor with zero area; ";
	}
	
	// Make unique monitor id if none specified thus far
	
	if (0 == m_lUniqueMonId)
	{
		m_lUniqueMonId = ++(m_pSim->m_lLastMonIdUsed);
	}
	
	// This unique id will be used to assign ids to any molecules
	// created by this monitor
	
	InitialiseMolIds();
	
	return ret;
}             

int CSObjMonitor::WriteState(CString& workingLine)
{	
	// List the molecule type specific info for each one referenced:
	
	CMolTypeStats* pStats;                          
	CObjTypeMol*   pMolType;
	
	for (int i = 0; i <	((CObjTypeMonitor*) m_pType)->m_nMolTypesRefd; i++)
	{
		pStats = (CMolTypeStats*) m_molStatsList[i];
		pMolType = pStats->m_pMolType;
		workingLine += "moltype \"" + pMolType->m_sName;
		workingLine += "\"";
		
		if (pStats->m_nTemperatureFixed)
		{
			workingLine += " fixt ";
		}
		else
		{
			workingLine += " nofixt ";
		}
		
		sprintf(buff, "%g", pStats->m_dFixedTemperature);
		workingLine += buff;

		if (pStats->m_nDensityFixed)
		{
			workingLine += " fixdens ";
		}
		else
		{
			workingLine += " nofixdens ";
		}
		
		sprintf(buff, "%g", pStats->m_dFixedDensity);
		workingLine += buff;
        
        workingLine += " ";
	}   
	
	if (MCNTRL_HEATER == ((CObjTypeMonitor*) m_pType)->m_nControlType)
	{
		if (m_dHeaterOn >= 0.5)
		{
			sprintf(buff, "power_on %g ", m_dHeaterPower);
		}
		else
		{
			sprintf(buff, "power_off %g ", m_dHeaterPower);
		}                           
		
		workingLine += buff;
	}
	
	// Now call the base version to handle the simple parameters
	return CSimObj::WriteState(workingLine);
}

void CSObjMonitor::ContributeNumerics()	
{                       
	sprintf(buff, ":%ld:", m_lId);
	CString sRootName = m_pType->m_sName + buff;  // eg "my_mon:3:"

	m_numHeaterPower.m_pSim = m_pSim;
    m_numHeaterPower.m_sName = sRootName + "power_value";
    m_numHeaterPower.m_sUnits = "kJ/mol/ns";
    m_numHeaterPower.m_sDescription = "heating power (when on) per mol monitors";
    m_numHeaterPower.m_pdValue = &m_dHeaterPower;
	m_numHeaterPower.m_nType = NUM_USERREAD | NUM_USERWRITE; 
	m_numHeaterPower.m_dMin = LIM_MIN_MON_POWER;	
	m_numHeaterPower.m_dMax = LIM_MAX_MON_POWER;
	m_pSim->AcceptNewNumeric(&m_numHeaterPower);	                                                            
            
	m_numHeaterOn.m_pSim = m_pSim;
    m_numHeaterOn.m_sName = sRootName + "power_on";
    m_numHeaterOn.m_sUnits = "";
    m_numHeaterOn.m_sDescription = "whether heating switched on";
    m_numHeaterOn.m_pdValue = &m_dHeaterOn;
	m_numHeaterOn.m_nType = NUM_USERREAD | NUM_USERWRITE; 
	m_numHeaterOn.m_dMin = 0;	
	m_numHeaterOn.m_dMax = 1;
	m_pSim->AcceptNewNumeric(&m_numHeaterOn);	                                                            
            
	m_numTotalTemperature.m_pSim = m_pSim;
    m_numTotalTemperature.m_sName = sRootName + "temperature";
    m_numTotalTemperature.m_sUnits = "K";
    m_numTotalTemperature.m_sDescription = "average temperature all types ref'd";
    m_numTotalTemperature.m_pdValue = &m_dTotalTemperature;
	m_numTotalTemperature.m_nType = NUM_USERREAD;
	m_pSim->AcceptNewNumeric(&m_numTotalTemperature);	                                                            

	m_numTotalTemperatureS.m_pSim = m_pSim;
    m_numTotalTemperatureS.m_sName = sRootName + "temperature_s";
    m_numTotalTemperatureS.m_sUnits = "K";
    m_numTotalTemperatureS.m_sDescription = "smoothed average temperature all types ref'd";
    m_numTotalTemperatureS.m_pdValue = &m_dTotalTemperatureS;
	m_numTotalTemperatureS.m_nType = NUM_USERREAD;
	m_pSim->AcceptNewNumeric(&m_numTotalTemperatureS); 

	m_numTotalNumMols.m_pSim = m_pSim;
    m_numTotalNumMols.m_sName = sRootName + "number";
    m_numTotalNumMols.m_sUnits = "";
    m_numTotalNumMols.m_sDescription = "number of molecules";
    m_numTotalNumMols.m_pdValue = &m_dTotalNumMols;
	m_numTotalNumMols.m_nType = NUM_USERREAD;
	m_pSim->AcceptNewNumeric(&m_numTotalNumMols);	                                                            

	m_numAvgKE.m_pSim = m_pSim;
    m_numAvgKE.m_sName = sRootName + "avg_ke";
    m_numAvgKE.m_sUnits = "J/mol";
    m_numAvgKE.m_sDescription = "average kinetic energy all types ref'd";
    m_numAvgKE.m_pdValue = &m_dAvgKE;
	m_numAvgKE.m_nType = NUM_USERREAD;
	m_pSim->AcceptNewNumeric(&m_numAvgKE);	                                                            

	m_numAvgKES.m_pSim = m_pSim;
    m_numAvgKES.m_sName = sRootName + "avg_ke_s";
    m_numAvgKES.m_sUnits = "J/mol";
    m_numAvgKES.m_sDescription = "average kinetic energy all types ref'd";
    m_numAvgKES.m_pdValue = &m_dAvgKES;
	m_numAvgKES.m_nType = NUM_USERREAD;
	m_pSim->AcceptNewNumeric(&m_numAvgKES);	                                                            

	m_numAvgPE.m_pSim = m_pSim;
    m_numAvgPE.m_sName = sRootName + "avg_pe";
    m_numAvgPE.m_sUnits = "J/mol";
    m_numAvgPE.m_sDescription = "average potential energy all types ref'd";
    m_numAvgPE.m_pdValue = &m_dAvgPE;
	m_numAvgPE.m_nType = NUM_USERREAD;
	m_pSim->AcceptNewNumeric(&m_numAvgPE);	                                                            

	m_numAvgPES.m_pSim = m_pSim;
    m_numAvgPES.m_sName = sRootName + "avg_pe_s";
    m_numAvgPES.m_sUnits = "J/mol";
    m_numAvgPES.m_sDescription = "smoothed average potential energy all types ref'd";
    m_numAvgPES.m_pdValue = &m_dAvgPES;
	m_numAvgPES.m_nType = NUM_USERREAD;
	m_pSim->AcceptNewNumeric(&m_numAvgPES); 

	m_numTotalEnergy.m_pSim = m_pSim;
    m_numTotalEnergy.m_sName = sRootName + "total_energy";
    m_numTotalEnergy.m_sUnits = "J/mol";
    m_numTotalEnergy.m_sDescription = "average total energy all types ref'd";
    m_numTotalEnergy.m_pdValue = &m_dTotalEnergy;
	m_numTotalEnergy.m_nType = NUM_USERREAD;
	m_pSim->AcceptNewNumeric(&m_numTotalEnergy);	                                                            

	m_numTotalEnergyS.m_pSim = m_pSim;
    m_numTotalEnergyS.m_sName = sRootName + "total_energy_s";
    m_numTotalEnergyS.m_sUnits = "J/mol";
    m_numTotalEnergyS.m_sDescription = "smoothed average total energy all types ref'd";
    m_numTotalEnergyS.m_pdValue = &m_dTotalEnergyS;
	m_numTotalEnergyS.m_nType = NUM_USERREAD;
	m_pSim->AcceptNewNumeric(&m_numTotalEnergyS); 

	m_numTotalPressure.m_pSim = m_pSim;
    m_numTotalPressure.m_sName = sRootName + "total_pressure";
    m_numTotalPressure.m_sUnits = "N/m";
    m_numTotalPressure.m_sDescription = "total pressure";
    m_numTotalPressure.m_pdValue = &m_dTotalPressure;
	m_numTotalPressure.m_nType = NUM_USERREAD;
	m_pSim->AcceptNewNumeric(&m_numTotalPressure);

	m_numTotalPressureS.m_pSim = m_pSim;
    m_numTotalPressureS.m_sName = sRootName + "total_pressure_s";
    m_numTotalPressureS.m_sUnits = "N/m";
    m_numTotalPressureS.m_sDescription = "smoothed total pressure";
    m_numTotalPressureS.m_pdValue = &m_dTotalPressureS;
	m_numTotalPressureS.m_nType = NUM_USERREAD;
	m_pSim->AcceptNewNumeric(&m_numTotalPressureS);

	m_numDensity.m_pSim = m_pSim;  
	m_numDensity.m_sName = sRootName + "num_density";
	m_numDensity.m_sUnits = "/nm2";
	m_numDensity.m_sDescription = "number density all types ref'd";
	m_numDensity.m_pdValue = &(m_dDensity); 
	m_numDensity.m_nType = NUM_USERREAD;            
	m_pSim->AcceptNewNumeric(&(m_numDensity));

	m_numDensityS.m_pSim = m_pSim;  
	m_numDensityS.m_sName = sRootName + "num_density_s";
	m_numDensityS.m_sUnits = "/nm2";
	m_numDensityS.m_sDescription = "smoothed number density all types ref'd";
	m_numDensityS.m_pdValue = &(m_dDensityS); 
	m_numDensityS.m_nType = NUM_USERREAD;            
	m_pSim->AcceptNewNumeric(&(m_numDensityS));
	
	// now add all those from stats array as it exists so far
	
	for (int i = 0; i <= m_molStatsList.GetUpperBound(); i++)
	{
		CMolTypeStats* pStats = (CMolTypeStats*) m_molStatsList[i];
		
		pStats->ContributeNumerics(m_pSim, m_pType, m_lId);
	}
}

CSObjMonitor::~CSObjMonitor()
{
	// delete all of stats objects
	int i;
	CMolTypeStats* pStats;
	
	for (i = 0; i <= m_molStatsList.GetUpperBound(); i++)
	{
		pStats = (CMolTypeStats*) m_molStatsList[i];
		
		delete pStats;
	}                 
	
	m_molStatsList.RemoveAll();
}

void CSObjMonitor::InitialiseStatsArray(int nMolTypes)
{ 
	// Initialise totals etc before sim run
    
    CMolTypeStats* pStats;
    
	for (int i = 0; i < nMolTypes; i++)
	{
		pStats = (CMolTypeStats*) m_molStatsList[i];
        
        pStats->Initialise();
	}                 
	
	m_nFirstPass = TRUE;
}

void CMolTypeStats::ContributeNumerics(CSim* pSim, CObjType* pType, long lId)
{                                        
	sprintf(buff, ":%ld:", lId);
	CString sRootName = pType->m_sName + buff;  // eg "my_mon:3:"

	m_numFixedDensity.m_pSim = pSim;  
	m_numFixedDensity.m_sName = sRootName + m_pMolType->m_sName + ":fixdens";
	m_numFixedDensity.m_sUnits = "/nm2";
	m_numFixedDensity.m_sDescription = "mol density to fix";
	m_numFixedDensity.m_pdValue = &(m_dFixedDensity); 
	m_numFixedDensity.m_nType = NUM_USERREAD | NUM_USERWRITE; 
	m_numFixedDensity.m_dMin = LIM_MIN_CTRL_DENSITY;
	m_numFixedDensity.m_dMax = LIM_MAX_CTRL_DENSITY;	
    pSim->AcceptNewNumeric(&(m_numFixedDensity));

	m_numFixedTemperature.m_pSim = pSim;  
	m_numFixedTemperature.m_sName = sRootName + m_pMolType->m_sName + ":fixt";
	m_numFixedTemperature.m_sUnits = "K";
	m_numFixedTemperature.m_sDescription = "mol temperature to fix";
	m_numFixedTemperature.m_pdValue = &(m_dFixedTemperature); 
	m_numFixedTemperature.m_nType = NUM_USERREAD | NUM_USERWRITE; 
	m_numFixedTemperature.m_dMin = LIM_MIN_TEMP;
	m_numFixedTemperature.m_dMax = LIM_MAX_TEMP;	
    pSim->AcceptNewNumeric(&(m_numFixedTemperature));

	m_numNumMolsInside.m_pSim = pSim;  
	m_numNumMolsInside.m_sName = sRootName + m_pMolType->m_sName + ":number";
	m_numNumMolsInside.m_sUnits = "";
	m_numNumMolsInside.m_sDescription = "number mols this type";
	m_numNumMolsInside.m_pdValue = &(m_dNumMolsInside); 
	m_numNumMolsInside.m_nType = NUM_USERREAD;
    pSim->AcceptNewNumeric(&(m_numNumMolsInside));
        
	m_numCountExitsTop.m_pSim = pSim;  
	m_numCountExitsTop.m_sName = sRootName + m_pMolType->m_sName + ":exits_top";
	m_numCountExitsTop.m_sUnits = "";
	m_numCountExitsTop.m_sDescription = "net number out top";
	m_numCountExitsTop.m_pdValue = &(m_dCountExitsTop); 
	m_numCountExitsTop.m_nType = NUM_USERREAD;
    pSim->AcceptNewNumeric(&(m_numCountExitsTop));
        
	m_numCountExitsBottom.m_pSim = pSim;  
	m_numCountExitsBottom.m_sName = sRootName +  m_pMolType->m_sName + ":exits_bottom";
	m_numCountExitsBottom.m_sUnits = "";
	m_numCountExitsBottom.m_sDescription = "net number out bottom";
	m_numCountExitsBottom.m_pdValue = &(m_dCountExitsBottom); 
	m_numCountExitsBottom.m_nType = NUM_USERREAD;
    pSim->AcceptNewNumeric(&(m_numCountExitsBottom));
        
	m_numCountExitsLeft.m_pSim = pSim;  
	m_numCountExitsLeft.m_sName = sRootName + m_pMolType->m_sName + ":exits_left";
	m_numCountExitsLeft.m_sUnits = "";
	m_numCountExitsLeft.m_sDescription = "net number out left";
	m_numCountExitsLeft.m_pdValue = &(m_dCountExitsLeft); 
	m_numCountExitsLeft.m_nType = NUM_USERREAD;
    pSim->AcceptNewNumeric(&(m_numCountExitsLeft));
        
	m_numCountExitsRight.m_pSim = pSim;  
	m_numCountExitsRight.m_sName = sRootName + m_pMolType->m_sName + ":exits_right";
	m_numCountExitsRight.m_sUnits = "";
	m_numCountExitsRight.m_sDescription = "net number out right";
	m_numCountExitsRight.m_pdValue = &(m_dCountExitsRight); 
	m_numCountExitsRight.m_nType = NUM_USERREAD;
    pSim->AcceptNewNumeric(&(m_numCountExitsRight));
        
	m_numTemperature.m_pSim = pSim;  
	m_numTemperature.m_sName = sRootName + m_pMolType->m_sName + ":temperature";
	m_numTemperature.m_sUnits = "K";
	m_numTemperature.m_sDescription = "temperature this type";
	m_numTemperature.m_pdValue = &(m_dTemperature); 
	m_numTemperature.m_nType = NUM_USERREAD;
    pSim->AcceptNewNumeric(&(m_numTemperature));
        
	m_numTemperatureS.m_pSim = pSim;  
	m_numTemperatureS.m_sName = sRootName + m_pMolType->m_sName + ":temperature_s";
	m_numTemperatureS.m_sUnits = "K";
	m_numTemperatureS.m_sDescription = "smoothed temperature this type";
	m_numTemperatureS.m_pdValue = &(m_dTemperatureS); 
	m_numTemperatureS.m_nType = NUM_USERREAD;
    pSim->AcceptNewNumeric(&(m_numTemperatureS));
        
	m_numAvgKE.m_pSim = pSim;  
	m_numAvgKE.m_sName = sRootName + m_pMolType->m_sName + ":avg_ke";
	m_numAvgKE.m_sUnits = "J/mol";
	m_numAvgKE.m_sDescription = "avg kinetic energy this type";
	m_numAvgKE.m_pdValue = &(m_dAvgKE); 
	m_numAvgKE.m_nType = NUM_USERREAD;            
	pSim->AcceptNewNumeric(&(m_numAvgKE));

	m_numAvgKES.m_pSim = pSim;  
	m_numAvgKES.m_sName = sRootName + m_pMolType->m_sName + ":avg_ke_s";
	m_numAvgKES.m_sUnits = "J/mol";
	m_numAvgKES.m_sDescription = "smoothed avg kinetic energy this type";
	m_numAvgKES.m_pdValue = &(m_dAvgKES); 
	m_numAvgKES.m_nType = NUM_USERREAD;            
	pSim->AcceptNewNumeric(&(m_numAvgKES));

	m_numAvgPE.m_pSim = pSim;  
	m_numAvgPE.m_sName = sRootName + m_pMolType->m_sName + ":avg_pe";
	m_numAvgPE.m_sUnits = "J/mol";
	m_numAvgPE.m_sDescription = "avg potential energy this type";
	m_numAvgPE.m_pdValue = &(m_dAvgPE); 
	m_numAvgPE.m_nType = NUM_USERREAD;            
	pSim->AcceptNewNumeric(&(m_numAvgPE));

	m_numAvgPES.m_pSim = pSim;  
	m_numAvgPES.m_sName = sRootName + m_pMolType->m_sName + ":avg_pe_s";
	m_numAvgPES.m_sUnits = "J/mol";
	m_numAvgPES.m_sDescription = "smoothed avg potential energy this type";
	m_numAvgPES.m_pdValue = &(m_dAvgPES); 
	m_numAvgPES.m_nType = NUM_USERREAD;            
	pSim->AcceptNewNumeric(&(m_numAvgPES));

	m_numTotalEnergy.m_pSim = pSim;  
	m_numTotalEnergy.m_sName = sRootName + m_pMolType->m_sName + ":total_energy";
	m_numTotalEnergy.m_sUnits = "J/mol";
	m_numTotalEnergy.m_sDescription = "avg total energy this type";
	m_numTotalEnergy.m_pdValue = &(m_dTotalEnergy); 
	m_numTotalEnergy.m_nType = NUM_USERREAD;            
	pSim->AcceptNewNumeric(&(m_numTotalEnergy));

	m_numTotalEnergyS.m_pSim = pSim;  
	m_numTotalEnergyS.m_sName = sRootName + m_pMolType->m_sName + ":total_energy_s";
	m_numTotalEnergyS.m_sUnits = "J/mol";
	m_numTotalEnergyS.m_sDescription = "smoothed avg total energy this type";
	m_numTotalEnergyS.m_pdValue = &(m_dTotalEnergyS); 
	m_numTotalEnergyS.m_nType = NUM_USERREAD;            
	pSim->AcceptNewNumeric(&(m_numTotalEnergyS));

	m_numPartialPressure.m_pSim = pSim;  
	m_numPartialPressure.m_sName = sRootName + m_pMolType->m_sName + ":partial_p";
	m_numPartialPressure.m_sUnits = "N/m";
	m_numPartialPressure.m_sDescription = "partial pressure";
	m_numPartialPressure.m_pdValue = &(m_dPartialPressure); 
	m_numPartialPressure.m_nType = NUM_USERREAD;            
	pSim->AcceptNewNumeric(&(m_numPartialPressure));

	m_numPartialPressureS.m_pSim = pSim;  
	m_numPartialPressureS.m_sName = sRootName + m_pMolType->m_sName + ":partial_p_s";
	m_numPartialPressureS.m_sUnits = "N/m";
	m_numPartialPressureS.m_sDescription = "smoothed partial pressure";
	m_numPartialPressureS.m_pdValue = &(m_dPartialPressureS); 
	m_numPartialPressureS.m_nType = NUM_USERREAD;            
	pSim->AcceptNewNumeric(&(m_numPartialPressureS));

	m_numDensity.m_pSim = pSim;  
	m_numDensity.m_sName = sRootName + m_pMolType->m_sName + ":num_density";
	m_numDensity.m_sUnits = "/nm2";
	m_numDensity.m_sDescription = "number density this type";
	m_numDensity.m_pdValue = &(m_dDensity); 
	m_numDensity.m_nType = NUM_USERREAD;            
	pSim->AcceptNewNumeric(&(m_numDensity));

	m_numDensityS.m_pSim = pSim;  
	m_numDensityS.m_sName = sRootName + m_pMolType->m_sName + ":num_density_s";
	m_numDensityS.m_sUnits = "/nm2";
	m_numDensityS.m_sDescription = "smoothed number density this type";
	m_numDensityS.m_pdValue = &(m_dDensityS); 
	m_numDensityS.m_nType = NUM_USERREAD;            
	pSim->AcceptNewNumeric(&(m_numDensityS));

}

void CSObjMonitor::BuildStatsArray(int nMolTypes)
{
	// Set up blank array of mol type
	
	// Delete old ones first (though should be empty)
	
	int i;
	CMolTypeStats* pStats;
	
	for (i = 0; i <= m_molStatsList.GetUpperBound(); i++)
	{
		pStats = (CMolTypeStats*) m_molStatsList[i];
		
		delete pStats;                               
		
		ASSERT(FALSE); // should have been empty
	}                 
	
	m_molStatsList.RemoveAll();
	
	CObjTypeMol* pMolType;
	                                                            
	for (i = 1; i <= nMolTypes; i++)
	{
		pStats = new CMolTypeStats;
		
		m_molStatsList.Add(pStats);  

		pMolType = (CObjTypeMol*) ((CObjTypeMonitor*) m_pType)->m_molTypesRefd[i - 1];
		
		pStats->m_pMolType = pMolType;
		pStats->Initialise();  // zero totals etc
	}                              
	
	m_nMolTypes = nMolTypes;  // to avoid having to GetUpperBound when running  
	
	ASSERT(m_molStatsList.GetUpperBound() == nMolTypes - 1);
}

void CSObjMonitor::InitialiseMolIds()
{    
	// Done later than other CMolTypeStats initialisation because need unique
	// id of monitor to do it
	                                
	CMolTypeStats* pStats;
	
	for (int i = 0; i < m_nMolTypes; i++)
	{
		pStats = (CMolTypeStats*) m_molStatsList[i];
        
		pStats->m_lLastMolIdUsed = MON_FIRST_MON_FIRST_MOL_ID +
		                               MON_MOL_IDS_PER_MON * (m_lUniqueMonId - 1);
	}
}		                                

void CSObjMonitor::NoteIntDying(int nStatIndex)
{
    if (m_molStatsList.GetUpperBound() > nStatIndex)
    {
        // probably called when stats list not built
        return;
    }

    CMolTypeStats* pStats = (CMolTypeStats*) m_molStatsList[nStatIndex];

    pStats->NoteIntDying();
}

void CSObjMonitor::UpdateQuantities()
{
	// called by parent monitor at end of each timestep
	
	CMolTypeStats* pStats; 
	CObjTypeMol* pMolType;
	CSObjMol* pMol; 
	double dVSquaredSum;      
	CMonMolNP* pNP;  
	double dPESum;  // for one type  
	double dVirialSum; // for one type
	double dImpulseSum;

	double dSFactor = m_pSim->m_dDeltaTime / m_pSim->m_dAverageTimeConst; // smoothing
	
	m_lTotalNumMols = 0;       
	m_dTotalTemperature = 0;
	m_dAvgPE = 0;
	m_dTotalPressure = 0;          
	m_dDensity = 0;
	
	ASSERT(m_nMolTypes == ((CObjTypeMonitor*) m_pType)->m_nMolTypesRefd);
	int nFixer = (MCNTRL_FIX_DENS_TEMP == ((CObjTypeMonitor*) m_pType)->m_nControlType);
	
	for (int i = 0; i < m_nMolTypes; i++)
	{
		pStats = (CMolTypeStats*) m_molStatsList[i];
		pMolType = (CObjTypeMol*) ((CObjTypeMonitor*) m_pType)->m_molTypesRefd[i];
		
		m_lTotalNumMols += pStats->m_lNumMolsInside;
		
		// maybe this should be method of stats, see how big gets
		
		pStats->m_dNumMolsInside = (double) pStats->m_lNumMolsInside;   
		pStats->m_dDensity = pStats->m_dNumMolsInside / m_dArea;
		
		
	    // experimental molecule insertion to fix density
	    if (nFixer && pStats->m_nDensityFixed)
	    {          
	    	//      molec/nm2    =     molec/nm2           -       molec/nm2
	    	double dDensityError = pStats->m_dFixedDensity - pStats->m_dDensity; 

			pStats->m_dMolsToCreate = dDensityError * m_dArea;
					                               
			while (pStats->m_dMolsToCreate >= 0.8)
			{ 
		    	// add to centre of this monitor with typical temperature    
		    	InsertNewMol(pMolType, m_dTotalTemperatureS);   
		    	                    
		    	pStats->m_dDensity += 1 / m_dArea; 
	    	    
	    	    pStats->m_dMolsToCreate--;
	    	    
	    		m_pSim->m_dSafetyDistLeft = 0; // kludge to force neighbour rebuild                     
	    	}

			while (pStats->m_dMolsToCreate <= -0.8)
			{
				// destroy molecule(s)  
				CMonMolNP* pNP = pStats->m_pFirstMonMolNP;
				
				if (NULL != pNP)
				{  

#if (CI_MONITOR < CI_MOL)
					CSObjMol* pMol = (CSObjMol*) pNP->m_pSObj2;
#else
					CSObjMol* pMol = (CSObjMol*) pNP->m_pSObj1;
#endif                                                    

					delete pMol;   
					delete pNP;

			    	pStats->m_dDensity -= 1 / m_dArea; 
	
		    		m_pSim->m_dSafetyDistLeft = 0; // kludge to force neighbour rebuild                     

					pStats->m_dMolsToCreate++;
				}   
				else
				{
					break;   // can't destroy any if none there
				}
			}             
	    }           
	    else
	    {
	    	// Density not fixed for this mol type
	    }
	    	    
		pStats->m_dCountExitsTop = (double) pStats->m_lCountExitsTop;
		pStats->m_dCountExitsBottom = (double) pStats->m_lCountExitsBottom;
		pStats->m_dCountExitsLeft = (double) pStats->m_lCountExitsLeft;
		pStats->m_dCountExitsRight = (double) pStats->m_lCountExitsRight;
		
		
		// Not sure whether all mols should always calc their own
		// vel squared anyway or not; trying it this way to start
		// with, working it out only for monitored mols

		// Loop over mols to get velocity, pressure, temperature
		
		dVSquaredSum = 0;             
		dVirialSum = 0;   
		dImpulseSum = 0;           
		dPESum = 0;
		pNP = pStats->m_pFirstMonMolNP;
		
		while (NULL != pNP)
		{   

#if CI_MOL < CI_MONITOR
			pMol = (CSObjMol*) pNP->m_pSObj1;
#else
			pMol = (CSObjMol*) pNP->m_pSObj2;
#endif
            
            pMol->m_dVSquared = pMol->m_dVx * pMol->m_dVx + pMol->m_dVy * pMol->m_dVy;
            dVSquaredSum += pMol->m_dVSquared;
            
            // Add potential energy due to interactions, formation energy and gravity
            // (latter two done here rather than for each molecule, so that only get
            // overhead of working them out if molecule actually monitored). All in J/mol
            dPESum += pMol->m_dPE 
                   + pMolType->m_dFormationEnergy * 1000 // convert from kJ/mol
                   //         amu/N    * kg/amu  * N/mol    * ns/s * m/nm * nm       * m / s / ns 
                   + pMolType->m_dMass * ONE_AMU * AVAGADRO * pMol->m_dY * m_pSim->m_dGravitationalAcceleration;

            dVirialSum += pMol->m_dVirial;
            
            dImpulseSum += pMol->m_dImpulseSum;
            
			pNP = pNP->m_pNextInteriorNP;
		}     
		
		if (pStats->m_lNumMolsInside > 0)
		{
			// Atkins 2nd ed p869, rms speed 'c' of mols = (3kT/m)^(1/2)
			// so T = m/3k <c^2>  in 3D, or m/2k <c^2> in 2D, i.e. 0.5kT
			// avg energy in each degree of freedom, totalling kin energy of mol
			                        
			// K (* num)     =           m2 / s2      *    amu         * kg/amu    / (J/K)   
			double dTContrib = dVSquaredSum * pMolType->m_dMass * ONE_AMU / 2.0 / KB;
			m_dTotalTemperature += dTContrib; 
			pStats->m_dTemperature = dTContrib / pStats->m_dNumMolsInside;    
			
			// If we should be fixing molecular temperature, do so now;
			// the problem of heating or cooling molecules is similar, but
			// requires a knowledge of the total number of mols contained
			
		    if (nFixer && pStats->m_nTemperatureFixed)
		    {
		    	pStats->SetTemperature(pStats->m_dFixedTemperature);
			}                                                        
			
			// Art of Molecular Dynamics Simulation p 20 has formula for extracting
			// pressure from virial sum (references J.-P. Hansen and I.R. McDonald (!),
			// "Theory of Simple Liquids", Academic Press, London, 2nd ed 1986
			//
			//    pV = NT + (1/d) SUM(Rij.Fij)
			//
			//  where N = Avagadro, d = dimensionality, Rij = intermol vector, Fij force 
			// but surely if no forces, pV = NkT (N=num moles, k = Boltz). For hard
            // spheres need to add term to take
			// account of collisions, else get underestimate; see Art MD Sim p 296 for
            // pressure algorithm; also get agreement
			// with piston-applied pressure for large LJ-interacting mols at high
			// density, so that works too (4/10/97).
			
			//       N/m               = (J/K   K (* num)        +   J/mol (*num)) /   (nm2 * 1e-18)			
			pStats->m_dPartialPressure = (KB  * dTContrib + dVirialSum / 2 / AVAGADRO
			                                  + dImpulseSum * ONE_AMU / 2.0 / m_pSim->m_dDeltaTime ) / (m_dArea * 1e-18);
			pStats->m_dAvgPE = dPESum / pStats->m_dNumMolsInside;
			m_dAvgPE += dPESum;
		}  
		else
		{
		 	pStats->m_dTemperature = 0;                       
		 	pStats->m_dAvgPE = 0; 
		 	pStats->m_dPartialPressure = 0;
		}  
		
		pStats->m_dAvgKE = pStats->m_dTemperature * AVAGADRO * KB;
		
		pStats->m_dTotalEnergy = pStats->m_dAvgKE + pStats->m_dAvgPE;
		
		if (m_nFirstPass)
		{
			pStats->m_dPartialPressureS = pStats->m_dPartialPressure;
			pStats->m_dTotalEnergyS = pStats->m_dTotalEnergyS;
			pStats->m_dAvgPES = pStats->m_dAvgPE;
			pStats->m_dAvgKES = pStats->m_dAvgKE;     
			pStats->m_dTemperatureS = pStats->m_dTemperature;
			pStats->m_dDensityS = pStats->m_dDensity;
		}
		else
		{
			pStats->m_dPartialPressureS += dSFactor * (pStats->m_dPartialPressure 
			                                            - pStats->m_dPartialPressureS);
			pStats->m_dTotalEnergyS += dSFactor * (pStats->m_dTotalEnergy 
			                                            - pStats->m_dTotalEnergyS);
			pStats->m_dAvgPES += dSFactor * (pStats->m_dAvgPE 
			                                            - pStats->m_dAvgPES);
			pStats->m_dAvgKES += dSFactor * (pStats->m_dAvgKE 
			                                            - pStats->m_dAvgKES);
			pStats->m_dTemperatureS += dSFactor * (pStats->m_dTemperature 
			                                            - pStats->m_dTemperatureS);
			pStats->m_dDensityS += dSFactor * (pStats->m_dDensity - pStats->m_dDensityS);
	    }
	    
		m_dTotalPressure += pStats->m_dPartialPressure;
		m_dDensity += pStats->m_dDensity;
	
	}  // end of loop over mol types  
    
    
	m_dTotalNumMols = (double) m_lTotalNumMols;

	if (m_lTotalNumMols > 0)                           
	{
		m_dTotalTemperature /= m_dTotalNumMols;
		m_dAvgPE /= m_dTotalNumMols;	                          
	}
	else
	{
		m_dTotalTemperature = 0;         
		m_dAvgPE = 0;
	}
	
	
	if (m_dHeaterOn >= 0.5 && MCNTRL_HEATER == ((CObjTypeMonitor*) m_pType)->m_nControlType)
	{                 
		m_dHeaterOn = 1;
		DoHeating();
	}                   
	else
	{
		m_dHeaterOn = 0;
	}
	
	

	m_dAvgKE = m_dTotalTemperature * AVAGADRO * KB;
	m_dTotalEnergy = m_dAvgKE + m_dAvgPE;

	if (m_nFirstPass)
	{	
		m_dTotalTemperatureS = m_dTotalTemperature;   
		m_dAvgPES = m_dAvgPE;
		m_dAvgKES = m_dAvgKE;       
		m_dTotalEnergyS = m_dTotalEnergy;
		m_dTotalPressureS = m_dTotalPressure;
		m_dDensityS = m_dDensity;
	}
	else
	{
		m_dTotalTemperatureS += dSFactor * (m_dTotalTemperature - m_dTotalTemperatureS);
		m_dAvgPES            += dSFactor * (m_dAvgPE - m_dAvgPES);
		m_dAvgKES            += dSFactor * (m_dAvgKE - m_dAvgKES);  
		m_dTotalEnergyS      += dSFactor * (m_dTotalEnergy - m_dTotalEnergyS);
		m_dTotalPressureS    += dSFactor * (m_dTotalPressure - m_dTotalPressureS);
		m_dDensityS          += dSFactor * (m_dDensity - m_dDensityS);
	}
	

	// if destructor, do destructions now
	if (((CObjTypeMonitor*) m_pType)->m_nControlType == MCNTRL_DESTROY)
	{
		while (!m_destroyList.IsEmpty())
		{
			CSObjMol* pMol = (CSObjMol*) m_destroyList.RemoveHead();
				
			delete pMol;      
				
			m_pSim->m_dSafetyDistLeft = 0; // kludge to force neighbour rebuild
		}
	}
    
    m_nFirstPass = FALSE;  // no need to test old value                      
}
            
void CSObjMonitor::DoHeating()
{
	// J/mol[heaters] +=       ns           * kJ/mol/ns      * (J/kJ)
	m_dEnergyToGive += m_pSim->m_dDeltaTime * m_dHeaterPower * 1000;
	
	// Don't allow a huge overdraft of energy to build up (ie if
	// we're switched on but have no mols to give it to)
	
	if (m_dEnergyToGive > MON_MAX_ENERGY_TO_GIVE)
	{                 
		m_dEnergyToGive = MON_MAX_ENERGY_TO_GIVE;
	}
	else if (m_dEnergyToGive < -MON_MAX_ENERGY_TO_GIVE)
	{                                            
		m_dEnergyToGive = -MON_MAX_ENERGY_TO_GIVE;
	}
	
	
	if (m_lTotalNumMols > 0)
	{
		// There are mols to heat (or cool), so do so, sharing the energy
		// out between them; but don't take average T outside of its normal
		// limits

		//       Kelvin   =   J/mol         / (n)         /    J/mol/K-1		
		double dTempToAdd = m_dEnergyToGive / AVAGADRO / m_dTotalNumMols / KB;

		if (dTempToAdd > 0)
		{
			// want to heat mols
					
			if (m_dTotalTemperature + dTempToAdd > LIM_MAX_TEMP)
			{
				dTempToAdd = LIM_MAX_TEMP - m_dTotalTemperature;
			}
			
			if (dTempToAdd < 0) dTempToAdd = 0;  // already too hot, do nothing
			
		}
		else if (dTempToAdd < 0)
		{
			// want to cool mols
					
			if (m_dTotalTemperature + dTempToAdd < LIM_MIN_TEMP)
			{
				dTempToAdd = LIM_MIN_TEMP - m_dTotalTemperature;
			}
			
			if (dTempToAdd > 0) dTempToAdd = 0;  // already too cold, do nothing
			
		}
		
		if (dTempToAdd != 0)
		{
			// Do the heating...                                        
			
			double dOldT;
			CMolTypeStats* pStats;
			
			for (int i = 0; i < m_nMolTypes; i++)
			{
				pStats = (CMolTypeStats*) m_molStatsList[i];
			
				dOldT = pStats->m_dTemperature;
					
				pStats->SetTemperature(dOldT + dTempToAdd);	
				
				m_dEnergyToGive -= (pStats->m_dTemperature - dOldT) 
				                     * KB * AVAGADRO * (double) pStats->m_lNumMolsInside;
			}
		}   
		
		m_dTotalTemperature += dTempToAdd;
	}
}

void CMolTypeStats::SetTemperature (double dNewTemperature)
{
	CSObjMol* pMol; 
	CMonMolNP* pNP;  
    
	if (m_dTemperature > 1)
	{                              
		// rescale vels to get right T though keeping distribution
		    		
		double dVScale = sqrt(dNewTemperature / m_dTemperature );
		    	    
	    pNP = m_pFirstMonMolNP;
		    	    
		while (NULL != pNP)
		{   

#if CI_MOL < CI_MONITOR
			pMol = (CSObjMol*) pNP->m_pSObj1;
#else
			pMol = (CSObjMol*) pNP->m_pSObj2;
#endif

			ASSERT(pMol->m_fsValid.bf.vx);
			ASSERT(pMol->m_fsValid.bf.vy);
						            
            pMol->m_dVx *= dVScale; 
            pMol->m_dVy *= dVScale;

			pNP = pNP->m_pNextInteriorNP;
		}
	}
	else
	{
		// Give all mols of this type same vel     
		    	    
	    double dSpeed = sqrt(2.0 * KB * dNewTemperature 
	                          / m_pMolType->m_dMass / ONE_AMU);
		    	                          
	    pNP = m_pFirstMonMolNP;
		    	    
		while (NULL != pNP)
		{   

#if CI_MOL < CI_MONITOR
			pMol = (CSObjMol*) pNP->m_pSObj1;
#else
			pMol = (CSObjMol*) pNP->m_pSObj2;
#endif
                        
            ASSERT(pMol->m_fsValid.bf.cosines);
                        
            pMol->m_dVx = dSpeed * pMol->m_dCx; 
            pMol->m_dVy = dSpeed * pMol->m_dCy;
            pMol->m_dSpeed = dSpeed;

#ifdef _DEBUG
			pMol->m_fsValid.bf.speed = 1;
			pMol->m_fsValid.bf.vx = 1;
			pMol->m_fsValid.bf.vy = 1;
#endif
						
			pNP = pNP->m_pNextInteriorNP;
		}
	}
				
	m_dTemperature = dNewTemperature;		    	
}
            
void CSObjMonitor::InsertNewMol(CObjTypeMol* pMolType, double dTemperature)
{                  
	// This must work out how to insert a new mol into the monitor
	// such that (if possible) it doesn't overlap any existing ones.
	
	// In the fullness of time should ensure new mol doesn't overlap
	// with pistons or walls either, or approach within LJ eqm dist
	// of other mols with which it shares LJ interactions, but for
	// now will just check proximity to other mols that this monitor
	// has interactions with in its interior list.
	

	// First get position and size of rectangle in which we will 
	// make trial insertions, if poss so that mols do not overlap
	// edges of monitor
		
	double dWidth = m_dXRight - m_dXLeft;
	double dHeight = m_dYTop - m_dYBottom;
	double dXLeft, dYBottom;  // of rectangle we shall insert in within monitor
	double dRadius = pMolType->m_dHSRadius + pMolType->m_dPackingStandoff;
	
	dWidth -= 2 * dRadius;                 
	
	if (dWidth >= 0)
	{
		dXLeft = m_dXLeft + dRadius;
	}
	else
	{
		dWidth = 0;
		dXLeft = (m_dXLeft + m_dXRight) / 2;
	}
	
	dHeight -= 2 * dRadius;
	
	if (dHeight >= 0)
	{
		dYBottom = m_dYBottom + dRadius;
	}
	else
	{
		dHeight = 0;
		dYBottom = (m_dYBottom + m_dYTop) / 2;
	}
	
	

	int nAttempts = 0;
	int nGoodFound = FALSE;
	
	double dX, dY;                        // trial insertion co-ords
	double dBestX, dBestY;                // if give up, these were least worst
	double dBestSepn2 = -NEARLY_INFINITY; // best nearest mol-mol sepn found (diff of sqs)
	CObjTypeMol* pOtherMolType;
	CSObjMol* pMol;              
	CMolTypeStats* pStats;
	
	do
	{
		dX = dXLeft + dWidth * m_pSim->GetRandom();
		dY = dYBottom + dHeight * m_pSim->GetRandom();
		
		nGoodFound = TRUE;  // have found no overlaps to start with
		double dWorstSpare2 = 0;
		
		for (int i = 0; i < m_nMolTypes; i++)
		{          
			pStats = (CMolTypeStats*) m_molStatsList[i];

			pOtherMolType = (CObjTypeMol*) ((CObjTypeMonitor*) m_pType)->m_molTypesRefd[i];
            CMonMolNP* pNP = pStats->m_pFirstMonMolNP;
		    
		    double dReqdSep2 = dRadius + pOtherMolType->m_dHSRadius;
		    dReqdSep2 *= dReqdSep2;
		    
			while (NULL != pNP)
			{   

#if CI_MOL < CI_MONITOR
				pMol = (CSObjMol*) pNP->m_pSObj1;
#else
				pMol = (CSObjMol*) pNP->m_pSObj2;
#endif
            
             	double dXSep = dX - pMol->m_dX;
             	double dYSep = dY - pMol->m_dY;
                
                double dSpare2 = dXSep * dXSep + dYSep * dYSep - dReqdSep2;
                if (dSpare2 <= 0)
            	{
            		// If mol inserted here will overlap this other mol
            		nGoodFound = FALSE; 
            		
            		if (dSpare2 < dWorstSpare2) dWorstSpare2 = dSpare2;
                }     
                
      			pNP = pNP->m_pNextInteriorNP;
            }  // loop over mols   
            
        }   // loop over mol types
        
        if (dWorstSpare2 > dBestSepn2)
        {  
        	// Least worst found so far, though may still overlap
        	
        	dBestSepn2 = dWorstSpare2;
        	dBestX = dX;
        	dBestY = dY;
        }
        
		nAttempts++;
	}
	while ((!nGoodFound) && (nAttempts < MAX_INSERTION_ATTEMPTS));
    
    // Either found a non-overlapping place to put mol, or gave
    // up guessing but have least-worst place. Now create mol.
    
    if (dTemperature <= 0)
    {
    	// avg T in monitor is zero, give new mol sys T instead
    	dTemperature = m_pSim->m_dTemperature;
    }

	// Now want to give mol an id in the block range allocated to
	// this monitor
    
    int nStatIndex = ((CObjTypeMonitor*) m_pType)->FindStatIndex(pMolType);
	pStats = (CMolTypeStats*) m_molStatsList[nStatIndex];
	
	long lId = ++(pStats->m_lLastMolIdUsed);
	    
    // Create new mol at co-ords found, at desired temperature, and with
    // next id in our allocation for this mol type
    pMolType->CreateNewMol(dBestX, dBestY, dTemperature, lId);
}

int CSObjMonitor::HitTest(double dXLeft, double dYBottom,
                               double dXRight, double dYTop)
{
	// Want selection only if edge clicked, not middle; this requires
	// that at least one of the edges of the selection rectangle intersects
	// one of the monitor edges. But now the monitors have finite thickness,
    // so have to consider the inner and outer limits of their edge walls.
	// And two special cases: monitor may be entirely encompassed by selection
	// rectangle, in which case want to select it (but not vice-versa), or
    // selection rectangle may lie between inner and outer layers of monitor
    // edge.
    // Finally if monitor has label, want to select if that is clicked on,
    // and if it is hatched do want to select if interior clicked.

	
	ASSERT(m_nDrawn);  // just curious, think should never get here otherwise
	
	CObjTypeMonitor* pType = (CObjTypeMonitor*) m_pType;
	double dLineThickness = pType->m_nLineThick 
	                                  / m_pSim->m_dScaleFactor;
	                                  
    double dInnerLeft   = m_dXLeft   + dLineThickness;
    double dInnerRight  = m_dXRight  - dLineThickness;
    double dInnerTop    = m_dYTop    - dLineThickness;
    double dInnerBottom = m_dYBottom + dLineThickness;

    double dOuterLeft   = m_dXLeft;
    double dOuterRight  = m_dXRight;
    double dOuterTop    = m_dYTop;
    double dOuterBottom = m_dYBottom;
    
    if (pType->m_nLabel && RectsOverlap(dXLeft, dYBottom, dXRight, dYTop, m_dXLeft,
        m_dYTop - m_dTextCornerYOffset, m_dXLeft + m_dTextCornerXOffset, m_dYTop))
    {
    	// This type of monitor is displaying labels and the selection
    	// rectangle intersects the rectangle between the top-left of
    	// the actual monitor region and the bottom-right of the label text
    	
    	return TRUE;
    }
    else if (!pType->m_nHatch && dXLeft > dInnerLeft && dXRight < dInnerRight
                              && dYTop < dInnerTop && dYBottom > dInnerBottom)
	{
		// This type of monitor is not hatched and the selection rectangle
		// is within the interior defined by the line, so not selected
		
		return FALSE;
	}                                
    else if (RectsOverlap(dXLeft, dYBottom, dXRight, dYTop,
                     dInnerLeft, dInnerBottom, dInnerRight, dInnerTop))
    {
    	// selection rectangle intersects inner extent
    	
    	return TRUE;
    }
    else if (RectsOverlap(dXLeft, dYBottom, dXRight, dYTop,
                     dOuterLeft, dOuterBottom, dOuterRight, dOuterTop))
    {
    	// selection rectangle intersects outer extent
    	
    	return TRUE;
    }                                          
    else
    {
    	return FALSE;
    }
}

void CSObjMonitor::DrawTracker(CDC* pDC)
{            
	// Need to supply tracker coordinates in device co-ords for comparison
	// to mouse pointer coords
    
    // get logical co-ords
	CPoint pointTopLeft = m_pSim->SimToLogical(m_dXLeft, m_dYTop);  
	CPoint pointBottomRight = m_pSim->SimToLogical(m_dXRight, m_dYBottom);  
	
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

void CSObjMonitor::HandleMove(double dDeltaX, double dDeltaY)
{
	m_dXLeft += dDeltaX;
	m_dXRight += dDeltaX;
	m_dYTop += dDeltaY;
	m_dYBottom += dDeltaY;
}

void CSObjMonitor::OnMoveOrResize(double dNewXLeft, double dNewYTop,
                                  double dNewXRight, double dNewYBottom,
	                              double dOldXLeft, double dOldYTop,
	                              int nResized)
{  
	CString cmd("change monitor \"");
	cmd += m_pType->m_sName; 
	cmd += "\" ";
	
	m_dXLeft = dNewXLeft;
	m_dXRight = dNewXRight;
	m_dYTop = dNewYTop;
	m_dYBottom = dNewYBottom; 

    WriteState(cmd); 
	    
    m_pSim->HandleNewCommand(cmd);	
}
	                              
void CSObjMonitor::ObjectProperties()
{                     
	CMonitorCtrlSetDlg dlg;
	
	dlg.m_pMonitorType = (CObjTypeMonitor*) m_pType;
	dlg.m_pSim = m_pSim;  
	dlg.m_nClone = TRUE;
	dlg.m_pCloneMonitor = this;	
			
	if (IDOK == dlg.DoModal())
	{			
		CString cmd;

		cmd = "change monitor \"";
		cmd += m_pType->m_sName;
		cmd += "\" ";
		dlg.m_pMonitor->WriteState(cmd);
				
		// need to get rid of phoney id and mon_uid for this monitor
		// as these are added uniquely when the real one is made; getting
		// CSObjMonitor to do this as it knows about relevant commands
				
		m_pSim->HandleNewCommand(cmd); 
	}			
	else
	{
		// user cancelled
	}

}
	                             
            