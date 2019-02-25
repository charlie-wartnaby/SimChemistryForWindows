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
#include "wall.h"      
#include "piston.h"
#include "lennard.h"
#include "walprpdg.h"
#include "swallpdg.h"

#include <math.h>



#ifdef _DEBUG
extern long global_debug_counter;
#endif


//////////////////////////////////////////////////////////////////////                                 
// CObjTypeWall functionality                        
//////////////////////////////////////////////////////////////////////                                 

// constructor

CObjTypeWall::CObjTypeWall()
{              
	m_nHasInts = TRUE;    
	m_nIsMobile = FALSE;
	m_dTemperature = 100;
	m_nLineThick = 2;
	m_nTemperatureType = WT_USE_SYS_TEMP; 
	m_nMolIntType = WT_THERMAL;  
	m_bLineRed = 0;
	m_bLineGreen = 0;
	m_bLineBlue = 255;
}            


// Command list
CMD_ENTRY CObjTypeWall::m_celCObjTypeWallCommands[MAX_COMMANDS_PER_OBJ] = 
{                                        
	{"name",               CA_STRING, CT_STATE,   CI_NAME,              LIM_MIN_CHARS_OBJTYPE, LIM_MAX_CHARS_OBJTYPE},
	{"linered",            CA_BYTE,   CT_STATE,   CI_LINERED,           0, 255 },
	{"lineblue",           CA_BYTE,   CT_STATE,   CI_LINEBLUE,          0, 255 },
	{"linegreen",          CA_BYTE,   CT_STATE,   CI_LINEGREEN,         0, 255 },
	{"linethick",          CA_INT,    CT_STATE,   CI_LINETHICK,         LIM_MIN_WALL_THICK, LIM_MAX_WALL_THICK  },   
	{"system_thermal",     CA_NONE,   CT_CONTROL, CI_SYSTEM_THERMAL,    0, 0   },
	{"thermal",            CA_NONE,   CT_CONTROL, CI_THERMAL,           0, 0   },  
	{"elastic",            CA_NONE,   CT_CONTROL, CI_ELASTIC,           0, 0   },
	{"temperature",        CA_DOUBLE, CT_CONTROL, CI_TEMPERATURE,       LIM_MIN_TEMP, LIM_MAX_TEMP},
	{"end",                CA_NONE,   CT_ENDLIST, CI_ENDLIST,           0, 0   }
};

// Command handlers

// Functionality overridden from generic CObjType 
int CObjTypeWall::WriteState(CString& workingLine)
{
	// Here we do special handling for defining the temperature
	// of the wall and what happens when molecules hit it. The
	// line thickness and colour can be handled by the base class
	// version CSimObj::WriteState automatically.  
	
	if (WT_USE_SYS_TEMP == m_nTemperatureType)
	{
		workingLine += "system_thermal ";
	}
	else
	{
		workingLine += "temperature ";
		sprintf(buff, "%g ", m_dTemperature);
		workingLine += buff;
	}
	
	if (WT_THERMAL == m_nMolIntType) 
	{
		workingLine += "thermal ";
	}
	else
	{            
		workingLine += "elastic ";
	}

	
	// Now call the base version to handle the simple parameters
	return CSimObj::WriteState(workingLine);
}                                 


int CObjTypeWall::GetStateParam(int command)
{
	int ret = SR_OK;
	
	switch(command)
	{               
		case CI_NAME:
			m_sArg = m_sName;
			break;
			
		case CI_LINERED:
			m_bArg = m_bLineRed;
			break;
			
		case CI_LINEGREEN:
			m_bArg = m_bLineGreen;
			break;

		case CI_LINEBLUE:
			m_bArg = m_bLineBlue;
			break;
			
		case CI_LINETHICK:
			m_nArg = m_nLineThick;
			break;
			
		default:
			TRACE("Asked for param for unknown command in CObjTypeWall\n");
			ASSERT(FALSE);
			ret = SR_ERROR;
			break;
	}             
	
	return ret;
}

CMD_ENTRY* CObjTypeWall::GetCmdList()
{
	return &(m_celCObjTypeWallCommands[0]);
}      

void CObjTypeWall::ContributeNumerics()
{                     
	m_numTemperature.m_pSim = m_pSim;
	m_numTemperature.m_sName = m_sName + ":special_t";
	m_numTemperature.m_sUnits = "K";
	m_numTemperature.m_sDescription = "temperature (if thermal and not sys)";
	m_numTemperature.m_pdValue = &m_dTemperature; 
	m_numTemperature.m_nType = NUM_USERWRITE | NUM_USERREAD;
	m_numTemperature.m_dMin = LIM_MIN_TEMP;
	m_numTemperature.m_dMax = LIM_MAX_TEMP;
	m_pSim->AcceptNewNumeric(&m_numTemperature);
}

int CObjTypeWall::AcceptCommandLine(CString& remainingLine, CString& errString)
{
	int command;
	int ret;
    
    ret = GrabCommand(remainingLine, errString, command);            
    while ((CI_EMPTY != command) && (SR_ERROR != ret))
    {
		switch (command)
		{   
			case CI_TEMPERATURE:
				m_dTemperature = m_dArg;
				m_nTemperatureType = WT_USE_SPECIFIC_TEMP;
				break;
			
			case CI_NAME:
				m_sName = m_sArg; 
				break;   
				
			case CI_LINERED:
				m_bLineRed = m_bArg;
				break;
				
			case CI_LINEGREEN:
				m_bLineGreen = m_bArg;
				break;
				
			case CI_LINEBLUE:
				m_bLineBlue = m_bArg;
				break;
			
			case CI_LINETHICK:
				m_nLineThick = m_nArg;
				break;	
				
			case CI_THERMAL:
				m_nMolIntType = WT_THERMAL;
				break;
				
			case CI_ELASTIC:     
				m_nMolIntType = WT_ELASTIC;
				break;

			case CI_SYSTEM_THERMAL:
				m_nTemperatureType = WT_USE_SYS_TEMP;
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

void CObjTypeWall::Delete()
{
	// have to delete any objects of type piston that
	// reference this wall type as well as deleting self
	
	// each time we delete a piston type, the other types will
	// shuffle down in the list, so if we delete type 3 (say), need
	// to check it again in case type 4 was another piston referencing
	// this wall type that becomes number 3 when the first one is deleted
	
	for (int i = 0; i <= m_pSim->m_typeArray.GetUpperBound(); i++)
	{   
		int deleted;

		do
		{                       
			CObjType* pType = (CObjType*) m_pSim->m_typeArray[i];  
			deleted = FALSE;
				
			if (CI_PISTON == pType->m_nBaseType)
			{
				CObjTypePiston* pPistonType = (CObjTypePiston*) pType;
				
				if (pPistonType->m_pWallType == this)
				{
					pPistonType->Delete();
					delete pPistonType;
					deleted = TRUE;
				}    
			}
		}   
		while (deleted && (i <= m_pSim->m_typeArray.GetUpperBound()));
		
	}
	
	// call base class to remove CSObjWall objects, interactions etc
	
	CObjType::Delete(); 
}

void CObjTypeWall::DrawAllSObjs(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2)
{ 
	// draw all walls of this type
	                        
	CSObjWall* pWall;
	pWall = (CSObjWall*) m_pFirstSObj;
	
	CPoint point1, point2;
	
	CPen wallPen(PS_SOLID, m_nLineThick, 
	             (COLORREF) RGB(m_bLineRed, m_bLineGreen, m_bLineBlue));
	CPen* pOldPen = pDC->SelectObject(&wallPen);

	while (NULL != pWall)
	{                                              
		point1 = m_pSim->SimToLogical(pWall->m_dX1, pWall->m_dY1); 
		point2 = m_pSim->SimToLogical(pWall->m_dX2, pWall->m_dY2);
		pDC->MoveTo(point1);
		pDC->LineTo(point2);
		
		pWall = (CSObjWall*) pWall->m_pNextSObj;
	}                                          
	
	pDC->SelectObject(pOldPen);
}

void CObjTypeWall::UserDrawing(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2)
{                             
	// This is where we draw cursors, wall in progress of being drawn etc.
    CPoint currentPt;
    
    CPen wallPen(PS_SOLID, m_nLineThick, 
                 (COLORREF) RGB(m_bLineRed, m_bLineGreen, m_bLineBlue));
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
			pDC->LineTo(currentPt); 
			pDC->SelectObject(pOldPen);
 
			break;

		case SM_NOT_DRAWING:
		default:
			// unknown state
			ASSERT(FALSE);
			break;
	}
}

void CObjTypeWall::OnLButtonUp()
{        
	if ((SM_AWAITING_CLICK2 == m_pSim->m_nDrawingState)
	    ||(SM_DRAGGING == m_pSim->m_nDrawingState))
	{
		m_pSim->m_nDrawingState = SM_AWAITING_CLICK1;

		CString cmd = "wall \"";
		cmd += m_sName;
		sprintf(buff, "\" x1 %g y1 %g x2 %g y2 %g", m_pSim->m_dSimDragStartX,
													m_pSim->m_dSimDragStartY,
													m_pSim->m_dSimMouseX,
													m_pSim->m_dSimMouseY);

		cmd += buff;
			
        m_pSim->HandleNewCommand(cmd);
	}    
    else
	{	
		// not a special state we're interested in
		
		ChangeDrawingStateOnLButtonUp();
	}
}

void CObjTypeWall::PropertiesCommand()
{                                           
	CEditWallPropsDlg dlg;

	dlg.m_nLineThick = m_nLineThick;
	dlg.m_dSpecialTemperature = m_dTemperature;
	dlg.m_sName = m_sName;
	dlg.m_nTemperatureType = m_nTemperatureType;
	dlg.m_nMolIntType = m_nMolIntType; 
	dlg.m_cLineColour = RGB(m_bLineRed, m_bLineGreen, m_bLineBlue); 
		
	if (IDOK == dlg.DoModal())
	{ 
		// user has specified new type of wall, so issue command

		CString cmd("change objtype wall name \"");
		cmd += dlg.m_sName;
		cmd += "\" linethick ";
		sprintf(buff, "%d", dlg.m_nLineThick);
		cmd += buff;
		
		if (WT_USE_SYS_TEMP == dlg.m_nTemperatureType)
		{
			cmd += " system_thermal";
		}
		else
		{
			cmd += " temperature";
			sprintf(buff, " %g", dlg.m_dSpecialTemperature);
			cmd += buff;
		}
		
		if (WT_THERMAL == dlg.m_nMolIntType)
		{
			cmd += " thermal";
		}
		else
		{
			cmd += " elastic";
		}
		
		cmd += " linered ";      
		sprintf(buff, "%d", (int) (dlg.m_cLineColour & 0x000000ff));
		cmd += buff;
		cmd += " linegreen ";      
		sprintf(buff, "%d", (int) ((dlg.m_cLineColour & 0x0000ff00) >> 8));
		cmd += buff;
		cmd += " lineblue ";      
		sprintf(buff, "%d", (int) ((dlg.m_cLineColour & 0x00ff0000) >> 16));
		cmd += buff;                        

		if (SR_OK != m_pSim->HandleNewCommand(cmd))
		{
			AfxMessageBox("Warnings or errors occurred -- see Console");
		}
	}
	else
	{
		// user must have cancelled
	}

}

///////////////////////////////////////////////////////////////////////////////////
//           CSObjWall
///////////////////////////////////////////////////////////////////////////////////

CMD_ENTRY CSObjWall::m_celCSObjWallCommands[MAX_COMMANDS_PER_OBJ] =
{                                        
	{"id",                CA_LONG,   CT_STATE,   CI_ID,                0,      2000000000},                  
	{"x1",        CA_DOUBLE, CT_STATE,   CI_X1,            LIM_MIN_X, LIM_MAX_X},
	{"y1",        CA_DOUBLE, CT_STATE,   CI_Y1,            LIM_MIN_X, LIM_MAX_X},
	{"x2",        CA_DOUBLE, CT_STATE,   CI_X2,            LIM_MIN_Y, LIM_MAX_Y},
	{"y2",        CA_DOUBLE, CT_STATE,   CI_Y2,            LIM_MIN_Y, LIM_MAX_Y}, 
	{"end",       CA_NONE,   CT_ENDLIST, CI_ENDLIST,           0,    0}
};

CMD_ENTRY* CSObjWall::GetCmdList() 
{
	return &(m_celCSObjWallCommands[0]);
}

int CSObjWall::GetStateParam(int command)
{
	int ret = SR_OK;
	
	switch(command)
	{
		case CI_X1:
			m_dArg = m_dX1;
			break;

		case CI_Y1:
			m_dArg = m_dY1;
			break;              

		case CI_X2:
			m_dArg = m_dX2;
			break;

		case CI_Y2:
			m_dArg = m_dY2;
			break;              

		case CI_ID:
			m_lArg = m_lId;
			break;
			
		default:
			TRACE("Asked for param for unknown command in CSObjWall\n");
			ASSERT(FALSE);
			ret = SR_ERROR;
			break;
	}             
	
	return ret;
}

int CSObjWall::AcceptCommandLine(CString& remainingLine, CString& errString)
{
	int command;
	int ret = SR_OK;
	
	m_fsValid.all = 0;   // mark nothing as initialised
    
    ret = GrabCommand(remainingLine, errString, command);            
    while ((CI_EMPTY != command) && (SR_ERROR != ret))
    {
		switch (command)
		{   
			case CI_ID:
				m_lId = m_lArg;
				break;

			case CI_X1:
				m_dX1 = m_dArg;     
				m_fsValid.bf.x1 = 1;
				break;
			
			case CI_X2:
				m_dX2 = m_dArg;     
				m_fsValid.bf.x2 = 1;
				break;
				
			case CI_Y1:
				m_dY1 = m_dArg;
				m_fsValid.bf.y1 = 1;
				break;
			
			case CI_Y2:
				m_dY2 = m_dArg;      
				m_fsValid.bf.y2 = 1;
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
		// check that wall initialised sufficiently
		if (!m_fsValid.bf.x1)
		{
			// co-ords not init, no good
			ret |= SR_WARN;
			m_dX1 = 0;      
			m_fsValid.bf.x1 = 1;
			errString += "supplied 0 as missing x1 co-ord; ";
		}

		if (!m_fsValid.bf.x2)
		{
			// co-ords not init, no good
			ret |= SR_WARN;
			m_dX2 = 0;      
			m_fsValid.bf.x2 = 1;
			errString += "supplied 0 as missing x2 co-ord; ";
		}                                         
		
		if (!m_fsValid.bf.y1)
		{
			// co-ords not init, no good
			ret |= SR_WARN;
			m_dY1 = 0;      
			m_fsValid.bf.y1 = 1;
			errString += "supplied 0 as missing y1 co-ord; ";
		}                                        
		
		if (!m_fsValid.bf.y2)
		{
			// co-ords not init, no good
			ret |= SR_WARN;
			m_dY2 = 0;      
			m_fsValid.bf.y2 = 1;
			errString += "supplied 0 as missing y2 co-ord; ";
		}   
		
		// Calculate direction cosines and the like now that we have
		// the end co-ordinates. This saves calculating them later when
		// needed (for example, in determining if a molecule will hit the
		// wall).
		
		double dx = m_dX2 - m_dX1;
		double dy = m_dY2 - m_dY1;
		
		m_dLength = sqrt(dx * dx + dy * dy);
		m_fsValid.bf.length = 1;
		
		if (m_dLength > NEARLY_ZERO)
		{
			m_dCx = dx / m_dLength;
			m_dCy = dy / m_dLength;
		}
		else
		{
			m_dCx = 1.0;
			m_dCy = 1.0;
		}
		
		m_fsValid.bf.cosines = 1;      
		
		m_dNx = m_dCy;
		m_dNy = - m_dCx;  
		m_fsValid.bf.normal = 1;      
		
		m_dDirection = asin(m_dCy);
		m_dNormDirection = asin(m_dNy);
		m_fsValid.bf.ndirn = 1;      

	}              
	
	  
	return ret;
}             

int CSObjWall::HitTest(double dXLeft, double dYBottom, 
                        double dXRight, double dYTop)
{
    // A wall has thickness, so its body consists of a (generally
    // inclined) rectangle centred on its ideal line but widened
    // to the line thickness, and with semi-circular ends capping
    // that rectangle.

	double dHalfLineThickness = ((CObjTypeWall*) m_pType)->m_nLineThick 
	                              / m_pSim->m_dScaleFactor / 2;    
	                              
    if (RectCircleOverlap(dXLeft, dYBottom, dXRight, dYTop,
                             m_dX1, m_dY1, dHalfLineThickness))
    {
    	// selection rectangle overlaps end 1 circle
    	
    	return TRUE;
    }
    else if (RectCircleOverlap(dXLeft, dYBottom, dXRight, dYTop,
                             m_dX2, m_dY2, dHalfLineThickness))
    {
    	// selection rectangle overlaps end 2 circle
    	
    	return TRUE;
    }
    else
    {
    	// Now if this wall is selected, it will either be because
    	// the edges of the selection rectangle intersect the lines
    	// defining the boundary of the inclined rectangle that makes
    	// up the body of the wall, or because the selection rectangle
    	// is entirely contained by that rectangle
    	
    	
    	// Construct set of 4 points defining inclined rectangular
    	// part of finite-thickness wall, end up with this kind of pattern:
    	//
    	//          1---------------------3 (thickness rectangle)  ^
    	//          |                     |                        |  normal
    	//          *=====================*  (actual wall)
    	//          |                     |
    	//          2---------------------4
    	
    	ASSERT(m_fsValid.bf.normal);
    	
    	double x1 = m_dX1 + m_dNx * dHalfLineThickness;
    	double y1 = m_dY1 + m_dNy * dHalfLineThickness;
    	double x2 = m_dX1 - m_dNx * dHalfLineThickness;
    	double y2 = m_dY1 - m_dNy * dHalfLineThickness; 
    	double x3 = m_dX2 + m_dNx * dHalfLineThickness;
    	double y3 = m_dY2 + m_dNy * dHalfLineThickness;
    	double x4 = m_dX2 - m_dNx * dHalfLineThickness;
    	double y4 = m_dY2 - m_dNy * dHalfLineThickness;
    	
    	// Whoa! Sixteen permutations to check all four lines of selection
    	// rectangle against all four of wall rectangle. Has to be a better
    	// way, but will do it for time being:
    	
    	if (LinesIntersect(dXLeft, dYBottom, dXRight, dYBottom,
    	                   x1, y1, x2, y2))
    	{
    		return TRUE;
    	}
    	else if (LinesIntersect(dXLeft, dYBottom, dXRight, dYBottom,
    	                       x1, y1, x3, y3))
    	{
    		return TRUE;
    	} 
    	else if (LinesIntersect(dXLeft, dYBottom, dXRight, dYBottom,
    	                       x4, y4, x3, y3))
    	{
    		return TRUE;
    	} 
    	else if (LinesIntersect(dXLeft, dYBottom, dXRight, dYBottom,
    	                       x2, y2, x4, y4))
    	{
    		return TRUE;
    	}
    	else if (LinesIntersect(dXLeft, dYTop, dXRight, dYTop,
    	                        x1, y1, x2, y2))
    	{
    		return TRUE;
    	}
    	else if (LinesIntersect(dXLeft, dYTop, dXRight, dYTop,
    	                       x1, y1, x3, y3))
    	{
    		return TRUE;
    	} 
    	else if (LinesIntersect(dXLeft, dYTop, dXRight, dYTop,
    	                       x4, y4, x3, y3))
    	{
    		return TRUE;
    	} 
    	else if (LinesIntersect(dXLeft, dYTop, dXRight, dYTop,
    	                       x2, y2, x4, y4))
    	{
    		return TRUE;
    	}
    	else if (LinesIntersect(dXLeft, dYBottom, dXLeft, dYTop,
    	                   x1, y1, x2, y2))
    	{
    		return TRUE;
    	}
    	else if (LinesIntersect(dXLeft, dYBottom, dXLeft, dYTop,
    	                       x1, y1, x3, y3))
    	{
    		return TRUE;
    	} 
    	else if (LinesIntersect(dXLeft, dYBottom, dXLeft, dYTop,
    	                       x4, y4, x3, y3))
    	{
    		return TRUE;
    	} 
    	else if (LinesIntersect(dXLeft, dYBottom, dXLeft, dYTop,
    	                       x2, y2, x4, y4))
    	{
    		return TRUE;
    	}
    	else  if (LinesIntersect(dXRight, dYBottom, dXRight, dYTop,
    	                         x1, y1, x2, y2))
    	{
    		return TRUE;
    	}
    	else if (LinesIntersect(dXRight, dYBottom, dXRight, dYTop,
    	                       x1, y1, x3, y3))
    	{
    		return TRUE;
    	} 
    	else if (LinesIntersect(dXRight, dYBottom, dXRight, dYTop,
    	                       x4, y4, x3, y3))
    	{
    		return TRUE;
    	} 
    	else if (LinesIntersect(dXRight, dYBottom, dXRight, dYTop,
    	                       x2, y2, x4, y4))
    	{
    		return TRUE;
    	}   
    	else
    	{
    		// Finally consider selection rectangle may be entirely
    		// contained by width of wall. Need only check one corner
    		// of selection rectangle as otherwise would have already
    		// detected an edge crossing the wall thickness boundary.
    		
    		return PointInInclinedRect(x1, y1, x2, y2, x3, y3, dXLeft, dYTop);
    	}
    }
        
}
	
void CSObjWall::CreateTracker()
{
	CSObj::CreateTracker();

	m_pTracker->m_nHandleStyle = TRK_LINE_ONLY;
		
	m_pTracker->m_nAllowResize = TRUE; 
	
	m_pTracker->m_sizeMin.cx = 0;
	m_pTracker->m_sizeMin.cy = 0;
}  

void CSObjWall::DrawTracker(CDC* pDC)
{            
	// Need to supply tracker coordinates in device co-ords for comparison
	// to mouse pointer coords
    
    // get logical co-ords
	CPoint point1 = m_pSim->SimToLogical(m_dX1, m_dY1);  
	CPoint point2 = m_pSim->SimToLogical(m_dX2, m_dY2);  
	
    // get rectangle  in logicals
    CRect rect;
	rect.left = point1.x;
	rect.top = point1.y;
	rect.right = point2.x;
	rect.bottom = point2.y;

	// now convert to device co-ords
	pDC->LPtoDP(&rect);
	
	m_pTracker->m_rect = rect;
	
	m_pTracker->m_nStyle = CRectTracker::resizeOutside;
		
	m_pTracker->Draw(pDC);
}

void CSObjWall::HandleMove(double dDeltaX, double dDeltaY)
{
	m_dX1 += dDeltaX;
	m_dX2 += dDeltaX;
	m_dY1 += dDeltaY;
	m_dY2 += dDeltaY;
}

void CSObjWall::OnMoveOrResize(double dNewXLeft, double dNewYTop,
	                           double dNewXRight, double dNewYBottom,
	                           double dOldXLeft, double dOldYTop,
	                           int nResized)
{  
	CString cmd("change wall \"");
	cmd += m_pType->m_sName; 
	cmd += "\" ";
	
	m_dX1 = dNewXLeft;
	m_dX2 = dNewXRight;
	m_dY1 = dNewYTop;
	m_dY2 = dNewYBottom; 

    WriteState(cmd); 
	    
    m_pSim->HandleNewCommand(cmd);
}

void CSObjWall::ObjectProperties() 
{
	CSWallPropsDlg dlg;
	
	dlg.m_dX1 = m_dX1;
	dlg.m_dY1 = m_dY1;
	dlg.m_dX2 = m_dX2;
	dlg.m_dY2 = m_dY2;
	dlg.m_pWallType = (CObjTypeWall*) m_pType;
	dlg.m_pSim = m_pSim;
	
	if (IDOK == dlg.DoModal())
	{
		int nModified = FALSE;
		
		if (fabs(dlg.m_dX1 - m_dX1) > NEARLY_ZERO) nModified = TRUE;
		if (fabs(dlg.m_dY1 - m_dY1) > NEARLY_ZERO) nModified = TRUE;
		if (fabs(dlg.m_dX2 - m_dX2) > NEARLY_ZERO) nModified = TRUE;
		if (fabs(dlg.m_dY2 - m_dY2) > NEARLY_ZERO) nModified = TRUE;
		
		if (nModified)
		{
		  	m_dX1 = dlg.m_dX1;
			m_dY1 = dlg.m_dY1;
			m_dX2 = dlg.m_dX2;
			m_dY2 = dlg.m_dY2;
			
			CString cmd("change wall \"");
			cmd += m_pType->m_sName;
			cmd += "\" ";
			
			WriteState(cmd);
			
			m_pSim->HandleNewCommand(cmd);
		}
	}
}
