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
#include "simvw.h" 
#include "wall.h"
#include "piston.h"
#include "lennard.h"
#include "pstrstdg.h"
#include "pisprpdg.h" 
#include "spisprdg.h"

#include <math.h>



#ifdef _DEBUG
extern long global_debug_counter;
#endif
/////////////////////////////////////////////////////////////////////////////
//////// Pistons
/////////////////////////////////////////////////////////////////////////////

// Command list
CMD_ENTRY CObjTypePiston::m_celCObjTypePistonCommands[OBJTYPE_PISTON_CMDS] = 
{                                                   
	{"name",           CA_STRING, CT_STATE,   CI_NAME,           LIM_MIN_CHARS_OBJTYPE, LIM_MAX_CHARS_OBJTYPE },
	{"walltype",       CA_STRING, CT_STATE,   CI_WALLTYPE,       LIM_MIN_CHARS_OBJTYPE, LIM_MAX_CHARS_OBJTYPE },
	{"damp_const",     CA_DOUBLE, CT_STATE,   CI_DAMP_CONST,     0, LIM_MAX_PISTON_DAMP},
	{"horizontal",     CA_NONE,   CT_CONTROL, CI_HORIZONTAL,     0, 0   },
	{"vertical",       CA_NONE,   CT_CONTROL, CI_VERTICAL,       0, 0   },
	{"const_press",    CA_DOUBLE, CT_CONTROL, CI_CONST_PRESS,    -LIM_MAX_PISTON_PRESSURE, LIM_MAX_PISTON_PRESSURE}, // size unknown
	{"spring_press",   CA_DOUBLE, CT_CONTROL, CI_SPRING_PRESS,   0, LIM_MAX_PISTON_FCONST}, // size unknown 
	{"mass",           CA_DOUBLE, CT_STATE,   CI_MASS,           LIM_MIN_PISTON_MASS, LIM_MAX_PISTON_MASS },
	{"end",            CA_NONE,   CT_ENDLIST, CI_ENDLIST,        0, 0   }
};

// Command handlers   

int CObjTypePiston::CmdWallType(CString& remainingLine, CString& errString)
{                                                                         
	CPtrArray wallTypes;
	
	m_pSim->GetTypeArray(wallTypes, CI_WALL);
	
	for (int i = 0; i <= wallTypes.GetUpperBound(); i++)
	{
		CObjTypeWall* pWallType = (CObjTypeWall*) wallTypes[i];
		if (pWallType->m_sName == m_sArg)
		{
			m_pWallType = pWallType; 
			m_fsValid.bf.walltype = 1;
			break;
		}   
		
		m_fsValid.bf.walltype = 0;
	}  
	
	if (!m_fsValid.bf.walltype)
	{
		errString += "no such wall type: " + m_sArg + "; ";
		return SR_ERROR;
	}
	else
	{
		return SR_OK;
	}
}

CObjTypePiston::CObjTypePiston()
{   
	m_nHasInts = TRUE;
	m_nIsMobile = TRUE;
	m_nDirectionType = PT_VERTICAL;
	m_dFixedPressure = 0.1;    // N/m
	m_dForceConst = 0;       // N/m/nm
	m_dDampingConst = 500;     // /ns
	m_dMass = 100;             // amu
	m_nPressureType = PT_FIXED;
	m_pWallType = NULL;    
	m_fsValid.all = 0;  // indicate nothing initialised by user
}

void CObjTypePiston::ContributeNumerics()
{
	m_numFixedPressure.m_pSim = m_pSim;
	m_numFixedPressure.m_sName = m_sName + ":const_press";
	m_numFixedPressure.m_sUnits = "N/m";
	m_numFixedPressure.m_sDescription = "fixed pressure applied by piston (when used)";
	m_numFixedPressure.m_pdValue = &m_dFixedPressure; 
	m_numFixedPressure.m_nType = NUM_USERWRITE | NUM_USERREAD;
	m_numFixedPressure.m_dMin = -LIM_MAX_PISTON_PRESSURE;
	m_numFixedPressure.m_dMax = LIM_MAX_PISTON_PRESSURE;
	m_pSim->AcceptNewNumeric(&m_numFixedPressure);

	m_numForceConst.m_pSim = m_pSim;
	m_numForceConst.m_sName = m_sName + ":spring_press";
	m_numForceConst.m_sUnits = "N/m/nm";
	m_numForceConst.m_sDescription = "spring force constant (when used)";
	m_numForceConst.m_pdValue = &m_dForceConst; 
	m_numForceConst.m_nType = NUM_USERWRITE | NUM_USERREAD;
	m_numForceConst.m_dMin = 0;
	m_numForceConst.m_dMax = LIM_MAX_PISTON_FCONST;
	m_pSim->AcceptNewNumeric(&m_numForceConst);

	m_numDampingConst.m_pSim = m_pSim;
	m_numDampingConst.m_sName = m_sName + ":damp_const";
	m_numDampingConst.m_sUnits = "/ns";
	m_numDampingConst.m_sDescription = "damping constant (1/time const)";
	m_numDampingConst.m_pdValue = &m_dDampingConst; 
	m_numDampingConst.m_nType = NUM_USERWRITE | NUM_USERREAD;
	m_numDampingConst.m_dMin = 0;
	m_numDampingConst.m_dMax = LIM_MAX_PISTON_DAMP;
	m_pSim->AcceptNewNumeric(&m_numDampingConst);

	m_numMass.m_pSim = m_pSim;
	m_numMass.m_sName = m_sName + ":mass";
	m_numMass.m_sUnits = "amu";
	m_numMass.m_sDescription = "piston mass";
	m_numMass.m_pdValue = &m_dMass; 
	m_numMass.m_nType = NUM_USERWRITE | NUM_USERREAD;
	m_numMass.m_dMin = LIM_MIN_PISTON_MASS;
	m_numMass.m_dMax = LIM_MAX_PISTON_MASS;
	m_pSim->AcceptNewNumeric(&m_numMass);
}

int CObjTypePiston::WriteState(CString& workingLine)
{
	// need to override the default in order to state either fixed or spring
	// pressure applied, and to specify the wall type
	                         
	workingLine += "walltype \"";
	workingLine += m_pWallType->m_sName;
	workingLine += "\" ";
		
	if (PT_FIXED == m_nPressureType)
	{
		workingLine += "const_press ";
		sprintf(buff, "%g ", m_dFixedPressure);
		workingLine += buff;
	}
	else
	{
		workingLine += "spring_press ";
		sprintf(buff, "%g ", m_dForceConst);
		workingLine += buff;
	}       
	
	if (PT_VERTICAL == m_nDirectionType)
	{
		workingLine += "vertical ";
	}
	else
	{
		workingLine += "horizontal ";
	}
	
	// Now call the base version to handle the simple parameters
	return CSimObj::WriteState(workingLine);
}

int CObjTypePiston::GetStateParam(int command)
{                     
	int ret = SR_OK;
	
	switch(command)
	{
		case CI_NAME:
			m_sArg = m_sName;
			break;      
			
		case CI_WALLTYPE:
			m_sArg = m_pWallType->m_sName;
			break;    
			
		case CI_DAMP_CONST:
			m_dArg = m_dDampingConst;
			break;           
			
		case CI_MASS:
			m_dArg = m_dMass;
			break;
			
		default:
			TRACE("Asked for param for unknown command in CObjTypePiston\n");
			ret = SR_ERROR;
			break;
	}             
	
	return ret;

}

CMD_ENTRY* CObjTypePiston::GetCmdList()
{                           
	return &(m_celCObjTypePistonCommands[0]);
}
 
int CObjTypePiston::AcceptCommandLine(CString& remainingLine, CString& errString)
{
	int command;
	int ret;
    
    ret = GrabCommand(remainingLine, errString, command);            
    while ((CI_EMPTY != command) && (SR_ERROR != ret))
    {
		switch (command)
		{   
			case CI_HORIZONTAL:
				m_nDirectionType = PT_HORIZONTAL;
				m_fsValid.bf.direction = 1;
				break;
			
			case CI_VERTICAL:
				m_nDirectionType = PT_VERTICAL;
				m_fsValid.bf.direction = 1;
				break;
			
			case CI_NAME:
				m_sName = m_sArg; 
				m_fsValid.bf.name = 1;
				break;   
				
			case CI_WALLTYPE:   
				ret |= CmdWallType(remainingLine, errString);
				break;
			
			case CI_CONST_PRESS: 
				m_nPressureType = PT_FIXED;
				m_dFixedPressure = m_dArg;
				m_fsValid.bf.fixedpressure = 1;
				break;
				
			case CI_SPRING_PRESS:     
				m_nPressureType = PT_SPRING;
				m_dForceConst = m_dArg;
				m_fsValid.bf.forceconst = 1;
				break;                   

			case CI_DAMP_CONST:     
				m_dDampingConst = m_dArg;
				m_fsValid.bf.dampingconst = 1;
				break;                   
			
			case CI_MASS:
				m_dMass = m_dArg;
				m_fsValid.bf.mass = 1;    
				break;

			default:
				ret |= SR_ERROR;
				errString += "confused near: " + remainingLine;
				break;
		}
		
        ret |= GrabCommand(remainingLine, errString, command);
    
	}              
	
	
	// Now fill in any blanks left by user
	
	// If our properties have just been changed, type may have changed
	// from having horizontal freedom to vertical. Make sure all child
	// piston vels in direction not supposed to have freedom are zeroed.

	CSObjPiston* pPiston = (CSObjPiston*) m_pFirstSObj;

	while (NULL != pPiston)
    {
		if (PT_VERTICAL == m_nDirectionType)
		{
			pPiston->m_dVx = 0;
		}
		else
		{
			pPiston->m_dVy = 0;
		}
		
    	pPiston = (CSObjPiston*) pPiston->m_pNextSObj;
    }                           
	  
	return ret;
}

void CObjTypePiston::ApplyAccels()
{
	CSObjPiston* pPiston = (CSObjPiston*) m_pFirstSObj;

	// the force exerted by (on) the piston is expressed in N/m, so
	// will be measured by a lenth below (in nm) to get the force in N.
	//
	// acc (m/s/timestep) = press (N/m) * length (nm) * 1e-9 * dt (ns) * 1e-9
	//                      -------------------------------------------------
	//                                   mass (amu) * 1.661e-27
	
	// the pressure and length vary from piston to piston, but the time/mass
	// factor is constant:
	
	double dDeltaTimeOverMass = m_pSim->m_dDeltaTime * 1e-18 / (m_dMass * ONE_AMU);
	
	// and also add damping.
	
	
	while (NULL != pPiston)
    {
		if (PT_SPRING == m_nPressureType)
		{   
			ASSERT(pPiston->m_fsValid.bf.vx);
			ASSERT(pPiston->m_fsValid.bf.vy);
			ASSERT(pPiston->m_fsValid.bf.lengths);
			  
			// force goes with displacement from rest position
			if (PT_VERTICAL == m_nDirectionType)
			{
				pPiston->m_dVy -= m_dForceConst * (pPiston->m_dYTop - pPiston->m_dYTRest) 
				                                * pPiston->m_dLengthHorizontal
				                                * dDeltaTimeOverMass;  
			                                
				// I don't think pistons should be subject to gravity, as they
				// can be given their own force downwards anyway and it confuses
				// the pressure they apply				                                
				
				
				pPiston->m_dVy -= m_dDampingConst * pPiston->m_dVy * m_pSim->m_dDeltaTime;
			}
			else
			{
				pPiston->m_dVx -= m_dForceConst * (pPiston->m_dXLeft - pPiston->m_dXLRest) 
				                                * pPiston->m_dLengthVertical
				                                * dDeltaTimeOverMass;                     
				                                
				pPiston->m_dVx -= m_dDampingConst * pPiston->m_dVx * m_pSim->m_dDeltaTime;
			}

		}
		else
		{   
			if (PT_VERTICAL == m_nDirectionType)
			{
				pPiston->m_dVy -= m_dFixedPressure * pPiston->m_dLengthHorizontal
				                                * dDeltaTimeOverMass;
				                                
				pPiston->m_dVy -= m_dDampingConst * pPiston->m_dVy * m_pSim->m_dDeltaTime;				                                
			}
			else
			{
				pPiston->m_dVx -= m_dFixedPressure * pPiston->m_dLengthVertical
				                                * dDeltaTimeOverMass;                
				                                
				pPiston->m_dVx -= m_dDampingConst * pPiston->m_dVx * m_pSim->m_dDeltaTime;				                                
			}
		}
    	
    	pPiston->ClipSpeed();
    			
    	pPiston = (CSObjPiston*) pPiston->m_pNextSObj;
    }                           
}

void CObjTypePiston::MoveSObjs()
{                      
#ifdef _DEBUG
	global_debug_counter++;
#endif

	CSObjPiston* pPiston;  
	pPiston = (CSObjPiston*) m_pFirstSObj;       
	
	// time in ns, dist in nm, vels in m/s, so all OK
	double dt = m_pSim->m_dTimeSoFar;

	while (NULL != pPiston)
    {                 
		
    	if (pPiston->m_nTouched)
    	{
    		// has already moved a bit through hard interactions
    		dt -= pPiston->m_dTimeDoneAlready;
    		pPiston->m_dXLeft += pPiston->m_dVx * dt;
    		pPiston->m_dXRight += pPiston->m_dVx * dt;
    		pPiston->m_dYTop += pPiston->m_dVy * dt;
    		pPiston->m_dYBottom += pPiston->m_dVy * dt;
    	    dt = m_pSim->m_dTimeSoFar;
    	}  
    	else
    	{   
    		// hasn't yet moved this timestep
    		pPiston->m_dXLeft += pPiston->m_dVx * dt;
    		pPiston->m_dXRight += pPiston->m_dVx * dt;
    		pPiston->m_dYTop += pPiston->m_dVy * dt;
    		pPiston->m_dYBottom += pPiston->m_dVy * dt;
    	}
        
        pPiston->m_dXDisplacement = pPiston->m_dXLeft - pPiston->m_dXLRest;
        pPiston->m_dYDisplacement = pPiston->m_dYTop - pPiston->m_dYTRest;
        
   		pPiston = (CSObjPiston*) pPiston->m_pNextSObj;

    }
	
}     

double CObjTypePiston::PrepStep()
{
	// called prior to calc of forces, etc; should return fastest
	// vel squared
	
	CSObjPiston* pPiston = (CSObjPiston*) m_pFirstSObj;
	
	double dVSquared;
	double dMaxVSquared = 0;
	
	while (NULL != pPiston)
    {
    	dVSquared = pPiston->m_dVx * pPiston->m_dVx + pPiston->m_dVy * pPiston->m_dVy;

		if (dVSquared > dMaxVSquared)
		{
			dMaxVSquared = dVSquared;
		}
        
 
		
		// note this mol not touched by hard events
		pPiston->m_nTouched = FALSE;
		
    	pPiston = (CSObjPiston*) pPiston->m_pNextSObj;
    }                           
    
    return dMaxVSquared;
}

	
void CObjTypePiston::DrawAllSObjs(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2)
{
	CSObjPiston* pPiston;
	pPiston = (CSObjPiston*) m_pFirstSObj;
	
	CPoint ptTopLeft, ptBottomRight;
	
	COLORREF colour = RGB(m_pWallType->m_bLineRed,
	                      m_pWallType->m_bLineGreen, 
	                      m_pWallType->m_bLineBlue);
	
	CPen wallPen(PS_SOLID, m_pWallType->m_nLineThick, colour); 
	                            
	CPen* pOldPen = pDC->SelectObject(&wallPen);

	CPen restPosPen(PS_DOT, 1, (COLORREF) 0);
	
	CBrush hatchBrush(HS_BDIAGONAL, colour); 
	
	CBrush* pOldBrush = pDC->SelectObject(&hatchBrush);
	
	while (NULL != pPiston)
	{                                              
		ptTopLeft = m_pSim->SimToLogical(pPiston->m_dXLeft, pPiston->m_dYTop); 
		ptBottomRight = m_pSim->SimToLogical(pPiston->m_dXRight, pPiston->m_dYBottom);
		pDC->Rectangle(ptTopLeft.x, ptTopLeft.y,
		               ptBottomRight.x, ptBottomRight.y);
		
		// now do rest position               
		pDC->SelectObject(&restPosPen);
				
		ptTopLeft = m_pSim->SimToLogical(pPiston->m_dXLRest, pPiston->m_dYTRest); 
		ptBottomRight = m_pSim->SimToLogical(pPiston->m_dXLRest + pPiston->m_dLengthHorizontal,
		                                     pPiston->m_dYTRest - pPiston->m_dLengthVertical);
				
		pDC->MoveTo(ptTopLeft.x, ptTopLeft.y);
		pDC->LineTo(ptTopLeft.x, ptBottomRight.y);
		pDC->LineTo(ptBottomRight.x, ptBottomRight.y);
		pDC->LineTo(ptBottomRight.x, ptTopLeft.y);
		pDC->LineTo(ptTopLeft.x, ptTopLeft.y);
			               
	    pDC->SelectObject(&wallPen);         
	    
		               
		pPiston = (CSObjPiston*) pPiston->m_pNextSObj;
	}                                          
	
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}

void CObjTypePiston::UserDrawing(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2)
{                             
	// This is where we draw cursors, wall in progress of being drawn etc.
    CPoint currentPt;
    
    CPen wallPen(PS_SOLID, m_pWallType->m_nLineThick, 
                 (COLORREF) RGB(m_pWallType->m_bLineRed,
                                m_pWallType->m_bLineGreen,
                                m_pWallType->m_bLineBlue));
                                
    CPen* pOldPen;
    
	switch (m_pSim->m_nDrawingState)
	{                     
		case SM_AWAITING_CLICK1:     
			if (PD_PISTON == m_nDrawingPhase)
			{
				// draw cursor at nearest gridpoint 
				currentPt = DrawCrosshair(pDC); 
			}
			else
			{
				// indicate rest position and draw piston at current position
				pOldPen = pDC->SelectObject(&wallPen);
				pDC->MoveTo(m_ptDrawingPistonTopLeft);
				pDC->LineTo(m_ptDrawingPistonTopLeft.x, m_ptDrawingPistonBottomRight.y); 
				pDC->LineTo(m_ptDrawingPistonBottomRight);
				pDC->LineTo(m_ptDrawingPistonBottomRight.x, m_ptDrawingPistonTopLeft.y);
				pDC->LineTo(m_ptDrawingPistonTopLeft);
				pDC->SelectObject(pOldPen);  
				
				currentPt = DrawRestCursor(pDC);
			}
			break; 
			
		case SM_BUTDN_NOT_MOVED1:      
			currentPt = DrawCrosshair(pDC);    
			break;
			
		case SM_DRAGGING:
		case SM_AWAITING_CLICK2:
			// if only moved a little bit, may be accidental.
			// otherwise treat it as trying to draw a piston. 
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

CPoint CObjTypePiston::DrawRestCursor(CDC* pDC)
{                            
    CPoint currentPt;

	currentPt = m_pSim->m_ptLogMouse;
	 
	pDC->SelectStockObject(BLACK_PEN);

	CPoint oppCornerPt = currentPt + (m_ptDrawingPistonBottomRight
	                                 - m_ptDrawingPistonTopLeft);

	pDC->MoveTo(currentPt);
	pDC->LineTo(currentPt.x, oppCornerPt.y);
	pDC->LineTo(oppCornerPt);
	pDC->LineTo(oppCornerPt.x, currentPt.y);
	pDC->LineTo(currentPt);
		                                 	
	return currentPt;

}

void CObjTypePiston::OnLButtonUp()
{        
	if ((PD_PISTON == m_nDrawingPhase)
	    && ((SM_AWAITING_CLICK2 == m_pSim->m_nDrawingState)
	    ||(SM_DRAGGING == m_pSim->m_nDrawingState)))
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
			
		m_dDrawingPistonXLeft = xl;
		m_dDrawingPistonYTop = yt;
		m_dDrawingPistonXRight = xr;
		m_dDrawingPistonYBottom = yb;     
			
		// Note logical co-ords of piston and calc vector from centre
		// to corner:
		m_ptDrawingPistonTopLeft = m_pSim->SimToLogical(xl, yt);
		m_ptDrawingPistonBottomRight = m_pSim->SimToLogical(xr, yb);
			
		// now invoke dialogue to see if we should draw rest position
			
		CPistonRestPositionDlg dlg;
			
		if (IDOK == dlg.DoModal())
		{
			// does want to specify rest position
			m_nDrawingPhase = PD_REST_POSITION;          
			m_pSim->m_nDrawingState = SM_AWAITING_CLICK1;    
			TRACE("Now doing piston eqm posn\n");
		}
		else
		{
			// rest position to be same as current, finish drawing
			TRACE("Piston draw complete without eqm posn being specified\n");
			m_dDrawingRestLeft = m_dDrawingPistonXLeft;
			m_dDrawingRestTop = m_dDrawingPistonYTop;
			IssueNewPistonCmd();                           
		}	    
	}    
	else if (PD_REST_POSITION == m_nDrawingPhase)
	{  
	    // If cursor is in centre of piston rectangle, work out
	    // sim co-ords of top left corner
		m_dDrawingRestLeft = m_pSim->m_dSimMouseX;
		m_dDrawingRestTop = m_pSim->m_dSimMouseY;
		
		IssueNewPistonCmd();
	}
	else
	{
		// not a special state we're interested in
		
		ChangeDrawingStateOnLButtonUp();
	}
}

void CObjTypePiston::IssueNewPistonCmd()
{ 
	CString cmd;
	
	cmd = "piston \"";
	cmd += m_sName;
	cmd += "\" ";
	sprintf(buff, "xleft %g ytop %g xright %g ybottom %g xlrest %g ytrest %g",
	           m_dDrawingPistonXLeft, m_dDrawingPistonYTop,
	           m_dDrawingPistonXRight, m_dDrawingPistonYBottom,
	           m_dDrawingRestLeft, m_dDrawingRestTop);
	cmd += buff;
	
	m_pSim->HandleNewCommand(cmd); 

	// reset drawing state machine
	m_pSim->m_nDrawingState = SM_NOT_DRAWING;
	if (SM_SIM_PAUSED == m_pSim->m_nSimState) 
	{
		m_pSim->m_nSimState = SM_SIM_RUNNING;
	}           

}

int CObjTypePiston::DrawCommand()
{                        
	m_nDrawingPhase = PD_PISTON;
	return TRUE;
}
  
void CObjTypePiston::PropertiesCommand()
{                        
	CEditPistonPropsDlg pistonPropsDlg; 
	
	pistonPropsDlg.m_sName = m_sName;	
	pistonPropsDlg.m_pSim = m_pSim; 
	pistonPropsDlg.m_pWallType = m_pWallType;
	pistonPropsDlg.m_nDirectionType = m_nDirectionType;
	pistonPropsDlg.m_dFixedPressure = m_dFixedPressure;    // N/m
	pistonPropsDlg.m_dForceConst = m_dForceConst;       // N/m/nm
	pistonPropsDlg.m_dDampingConst = m_dDampingConst;
	pistonPropsDlg.m_dMass = m_dMass;             // amu
	pistonPropsDlg.m_nPressureType = m_nPressureType;
	
	if ((IDOK == pistonPropsDlg.DoModal()) && (NULL != pistonPropsDlg.m_pWallType))
	{
		// user meant it, buid up command to change piston
			   
		CString cmd("change objtype piston name \"");
		cmd += pistonPropsDlg.m_sName;
		cmd += "\" ";        
			
		cmd += "walltype \"";
		cmd += pistonPropsDlg.m_pWallType->m_sName;
		cmd += "\" ";

		if (PT_HORIZONTAL == pistonPropsDlg.m_nDirectionType)
		{
			cmd += "horizontal ";
		}
		else
		{
			cmd += "vertical ";
		}
			
		cmd += "mass ";
		sprintf(buff, "%g ", pistonPropsDlg.m_dMass);
		cmd += buff;
			
		if (PT_FIXED == pistonPropsDlg.m_nPressureType)
		{
			cmd += "const_press ";
			sprintf(buff, "%g", pistonPropsDlg.m_dFixedPressure);
		}
		else
		{
			cmd += "spring_press ";
			sprintf(buff, "%g", pistonPropsDlg.m_dForceConst);
		}    
		cmd += buff;                
		
		sprintf(buff, "%g", pistonPropsDlg.m_dDampingConst);
		cmd += " damp_const ";
		cmd += buff;
			
		if (SR_OK != m_pSim->HandleNewCommand(cmd))
		{
			AfxMessageBox("Warnings or errors occurred -- see Console");
		}
    }
}


////////////////////////////////////////////////////////////////////////
//////// CSObjPiston
////////////////////////////////////////////////////////////////////////////

CSObjPiston::CSObjPiston()
{
	m_pRestTracker = NULL;
	m_nHitRestTracker = FALSE;
	m_dXLeft = 0;
	m_dYTop = 1;
	m_dXRight = 1;
	m_dYBottom = 0; 
	m_dXLRest = 0;  // top-left equil co-ord
	m_dYTRest = 1;  // top-left equil co-ord 
	m_dXDisplacement = 0;
	m_dYDisplacement = 0;
	m_dVx = 0;
	m_dVy = 0;
	m_dCxHorizontal = 1;                                                                  
	m_dCyHorizontal = 0;
	m_dNxHorizontal = 0;
	m_dNyHorizontal = 1;
	m_dLengthHorizontal = 1;     
	m_dCxVertical = 0;                                                                  
	m_dCyVertical = 1;
	m_dNxVertical = 1;
	m_dNyVertical = 0;
	m_dLengthVertical = 1;
}               

CSObjPiston::~CSObjPiston()
{
	delete m_pRestTracker;
	
	// other one will be deleted by default CSObj destructor
}

CMD_ENTRY CSObjPiston::m_celCSObjPistonCommands[MAX_COMMANDS_PER_OBJ] =
{                         
	{"id",                CA_LONG,   CT_STATE,   CI_ID,                0,      2000000000},                  
	{"xleft",    CA_DOUBLE, CT_STATE,   CI_XLEFT,   LIM_MIN_X,  LIM_MAX_X },
	{"xright",   CA_DOUBLE, CT_STATE,   CI_XRIGHT,  LIM_MIN_X,  LIM_MAX_X },
	{"ytop",     CA_DOUBLE, CT_STATE,   CI_YTOP,    LIM_MIN_Y,  LIM_MAX_Y },
	{"ybottom",  CA_DOUBLE, CT_STATE,   CI_YBOTTOM, LIM_MIN_Y,  LIM_MAX_Y },
	{"xlrest",   CA_DOUBLE, CT_STATE,   CI_XLREST,  LIM_MIN_X,  LIM_MAX_X },
	{"ytrest",   CA_DOUBLE, CT_STATE,   CI_YTREST,  LIM_MIN_Y,  LIM_MAX_Y },
	{"vx",       CA_DOUBLE, CT_STATE,   CI_VX,      -LIM_MAX_PISTON_SPEED, LIM_MAX_PISTON_SPEED},
	{"vy",       CA_DOUBLE, CT_STATE,   CI_VY,      -LIM_MAX_PISTON_SPEED, LIM_MAX_PISTON_SPEED},
	{"end",      CA_NONE,   CT_ENDLIST, CI_ENDLIST, 0,     0   }
};

int CSObjPiston::GetStateParam(int command)
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

		case CI_XLREST:
			m_dArg = m_dXLRest;
			break;              

		case CI_YTREST:
			m_dArg = m_dYTRest;
			break;             
			
		case CI_VX:
			m_dArg = m_dVx;
			break;
			
		case CI_VY:
			m_dArg = m_dVy;
			break; 
			
		default:
			TRACE("Asked for param for unknown command in CSObjPiston\n");
			ret = SR_ERROR;
			break;
	}             
	
	return ret;

}

CMD_ENTRY* CSObjPiston::GetCmdList()
{                      
	return &(m_celCSObjPistonCommands[0]);
}

int CSObjPiston::AcceptCommandLine(CString& remainingLine, CString& errString)
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
				
			case CI_XLREST:     
				m_dXLRest = m_dArg;    
				m_fsValid.bf.xlrest = 1;
				break;
				
			case CI_YTREST:
				m_dYTRest = m_dArg;    
				m_fsValid.bf.ytrest = 1;
				break;       
				
			case CI_VX:
				m_dVx = m_dArg;
				m_fsValid.bf.vx = 1;
				break;

			case CI_VY:
				m_dVy = m_dArg;
				m_fsValid.bf.vy = 1;
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
		if (!m_fsValid.bf.xleft)
		{
			ret |= SR_WARN;        
			
			if (m_fsValid.bf.xright)
			{
				m_dXLeft = m_dXRight - PISTON_DEFAULT_SIZE;
			}
			else
			{
				m_dXLeft = 0;
			}
			m_fsValid.bf.xleft = 1;
		}

		if (!m_fsValid.bf.xright)
		{
			ret |= SR_WARN;        
			
			if (m_fsValid.bf.xleft)
			{
				m_dXRight = m_dXLeft + PISTON_DEFAULT_SIZE;
			}
			else
			{
				m_dXRight = 0;
			}
			m_fsValid.bf.xright = 1;
		}


		if (!m_fsValid.bf.ytop)
		{
			ret |= SR_WARN;        
			
			if (m_fsValid.bf.ybottom)
			{
				m_dYTop = m_dYBottom + PISTON_DEFAULT_SIZE;
			}
			else
			{
				m_dYTop = 0;
			}
			m_fsValid.bf.ytop = 1;
		}

		if (!m_fsValid.bf.ybottom)
		{
			ret |= SR_WARN;        
			
			if (m_fsValid.bf.ytop)
			{
				m_dYBottom = m_dYTop - PISTON_DEFAULT_SIZE;
			}
			else
			{
				m_dYBottom = 0;
			}
			m_fsValid.bf.ybottom = 1;
		}

		if (!m_fsValid.bf.xlrest)
		{
			ret |= SR_WARN;        
			
			if (m_fsValid.bf.xleft)
			{
				m_dXLRest = m_dXLeft;
			}
			else
			{
				m_dXLRest = 0;
			}
			m_fsValid.bf.xlrest = 1;
		}

		if (!m_fsValid.bf.ytrest)
		{
			ret |= SR_WARN;        
			
			if (m_fsValid.bf.ytop)
			{
				m_dYTRest = m_dYTop;
			}
			else
			{
				m_dYTRest = 0;
			}
			m_fsValid.bf.ytrest = 1;
		}   
		
		if (!m_fsValid.bf.vx)
		{
			m_dVx = 0;
			m_fsValid.bf.vx = 1;
		}
		
		if (!m_fsValid.bf.vy)
		{
			m_dVy = 0;
			m_fsValid.bf.vy = 1;
		}

		if (SR_WARN & ret)
		{
			errString += "supplied one or more missing co-ordinates; ";
		}
	                          
	    
	    // Now calculate other derived values that will be useful later
	                          
		m_dLengthHorizontal = m_dXRight - m_dXLeft;
		m_dLengthVertical = m_dYTop - m_dYBottom;   
		m_fsValid.bf.lengths = 1;
			
	}	
	  
	return ret;
}

void CSObjPiston::ClipSpeed()
{        
	ASSERT(m_fsValid.bf.vx);
	ASSERT(m_fsValid.bf.vy);
	
	if (PT_VERTICAL == ((CObjTypePiston*) m_pType)->m_nDirectionType)
	{              
		if (m_dVy > LIM_MAX_PISTON_SPEED)
		{
			m_dVy = LIM_MAX_PISTON_SPEED;
		}
		else if (m_dVy < -LIM_MAX_PISTON_SPEED)
		{
			m_dVy = -LIM_MAX_PISTON_SPEED;
		}
		
		m_dVx = 0;
	}
	else
	{
		if (m_dVx > LIM_MAX_PISTON_SPEED)
		{
			m_dVx = LIM_MAX_PISTON_SPEED;
		}
		else if (m_dVx < -LIM_MAX_PISTON_SPEED)
		{
			m_dVx = -LIM_MAX_PISTON_SPEED;
		}
		
		m_dVy = 0;
	}
}

void CSObjPiston::ContributeNumerics()
{
	m_numXLeft.m_pSim = m_pSim;            
	sprintf(buff, "%ld", m_lId);
	m_numXLeft.m_sName = m_pType->m_sName + ":" + buff + ":xleft";
	m_numXLeft.m_sUnits = "nm";
	m_numXLeft.m_sDescription = "current left edge x coordinate";
	m_numXLeft.m_pdValue = &m_dXLeft; 
	m_numXLeft.m_nType = NUM_USERREAD;
	m_pSim->AcceptNewNumeric(&m_numXLeft);

	m_numYTop.m_pSim = m_pSim;            
	sprintf(buff, "%ld", m_lId);
	m_numYTop.m_sName = m_pType->m_sName + ":" + buff + ":ytop";
	m_numYTop.m_sUnits = "nm";
	m_numYTop.m_sDescription = "current top edge y coordinate";
	m_numYTop.m_pdValue = &m_dYTop; 
	m_numYTop.m_nType = NUM_USERREAD;
	m_pSim->AcceptNewNumeric(&m_numYTop);

	m_numXRight.m_pSim = m_pSim;            
	sprintf(buff, "%ld", m_lId);
	m_numXRight.m_sName = m_pType->m_sName + ":" + buff + ":xright";
	m_numXRight.m_sUnits = "nm";
	m_numXRight.m_sDescription = "current right edge x coordinate";
	m_numXRight.m_pdValue = &m_dXRight; 
	m_numXRight.m_nType = NUM_USERREAD;
	m_pSim->AcceptNewNumeric(&m_numXRight);

	m_numYBottom.m_pSim = m_pSim;            
	sprintf(buff, "%ld", m_lId);
	m_numYBottom.m_sName = m_pType->m_sName + ":" + buff + ":ybottom";
	m_numYBottom.m_sUnits = "nm";
	m_numYBottom.m_sDescription = "current bottom edge y coordinate";
	m_numYBottom.m_pdValue = &m_dYBottom; 
	m_numYBottom.m_nType = NUM_USERREAD;
	m_pSim->AcceptNewNumeric(&m_numYBottom);

	m_numXDisplacement.m_pSim = m_pSim;            
	sprintf(buff, "%ld", m_lId);
	m_numXDisplacement.m_sName = m_pType->m_sName + ":" + buff + ":xdispl";
	m_numXDisplacement.m_sUnits = "nm";
	m_numXDisplacement.m_sDescription = "x displacement from rest position";
	m_numXDisplacement.m_pdValue = &m_dXDisplacement; 
	m_numXDisplacement.m_nType = NUM_USERREAD;
	m_pSim->AcceptNewNumeric(&m_numXDisplacement);

	m_numYDisplacement.m_pSim = m_pSim;            
	sprintf(buff, "%ld", m_lId);
	m_numYDisplacement.m_sName = m_pType->m_sName + ":" + buff + ":ydispl";
	m_numYDisplacement.m_sUnits = "nm";
	m_numYDisplacement.m_sDescription = "y displacement from rest position";
	m_numYDisplacement.m_pdValue = &m_dYDisplacement; 
	m_numYDisplacement.m_nType = NUM_USERREAD;
	m_pSim->AcceptNewNumeric(&m_numYDisplacement);

	m_numXLRest.m_pSim = m_pSim;            
	sprintf(buff, "%ld", m_lId);
	m_numXLRest.m_sName = m_pType->m_sName + ":" + buff + ":xlrest";
	m_numXLRest.m_sUnits = "nm";
	m_numXLRest.m_sDescription = "rest position left edge x coordinate";
	m_numXLRest.m_pdValue = &m_dXLRest; 
	m_numXLRest.m_nType = NUM_USERWRITE | NUM_USERREAD;
	m_numXLRest.m_dMin = LIM_MIN_X;
	m_numXLRest.m_dMax = LIM_MAX_X;
	m_pSim->AcceptNewNumeric(&m_numXLRest);

	m_numYTRest.m_pSim = m_pSim;            
	sprintf(buff, "%ld", m_lId);
	m_numYTRest.m_sName = m_pType->m_sName + ":" + buff + ":ytrest";
	m_numYTRest.m_sUnits = "nm";
	m_numYTRest.m_sDescription = "rest position top edge y coordinate";
	m_numYTRest.m_pdValue = &m_dYTRest; 
	m_numYTRest.m_nType = NUM_USERWRITE | NUM_USERREAD;
	m_numYTRest.m_dMin = LIM_MIN_Y;
	m_numYTRest.m_dMax = LIM_MAX_Y;
	m_pSim->AcceptNewNumeric(&m_numYTRest);
}

int CSObjPiston::HitTest(double dXLeft, double dYBottom,
                            double dXRight, double dYTop)
{
	int ret = RectsOverlap(dXLeft, dYBottom, dXRight, dYTop,
                        m_dXLeft, m_dYBottom, m_dXRight, m_dYTop);
    
    if (!ret)
    {
    	ret = RectsOverlap(dXLeft, dYBottom, dXRight, dYTop,
    	                   m_dXLRest, m_dYTRest - m_dLengthVertical,
    	                   m_dXLRest + m_dLengthHorizontal, m_dYTRest);
	}
	
	return ret; 
}

void CSObjPiston::CreateTracker()
{          
	// As well as standard tracking rectangle for piston itself, have
	// another one around its rest position so that can be moved too
	
	ASSERT(m_pTracker == NULL);
	ASSERT(m_pRestTracker == NULL);                    

	m_pTracker = new CGridTracker;
	m_pRestTracker = new CGridTracker;
	
	m_pTracker->m_pObj = this;
	m_pRestTracker->m_pObj = this;
	m_pTracker->m_pSim = m_pSim;
	m_pRestTracker->m_pSim = m_pSim;
}  

void CSObjPiston::DeleteTracker()
{
	delete m_pTracker;   
	delete m_pRestTracker;
	
	m_pTracker = NULL;
	m_pRestTracker = NULL;
}  

void CSObjPiston::DrawTracker(CDC* pDC)
{              
	// Need to supply tracker coordinates in device co-ords for comparison
	// to mouse pointer coords
    
    // get logical co-ords for piston itself
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
	
	
	// Now repeat for rest position rectangle

	pointTopLeft = m_pSim->SimToLogical(m_dXLRest, m_dYTRest);  
	pointBottomRight = m_pSim->SimToLogical(m_dXLRest + m_dLengthHorizontal,
	                                        m_dYTRest - m_dLengthVertical);  
	
	rect.left = pointTopLeft.x;
	rect.top = pointTopLeft.y;
	rect.right = pointBottomRight.x;
	rect.bottom = pointBottomRight.y;

	// now convert to device co-ords
	pDC->LPtoDP(&rect);
	rect.NormalizeRect();
	
	m_pRestTracker->m_rect = rect;
	
	m_pRestTracker->m_nStyle = CRectTracker::resizeOutside;
		
	m_pRestTracker->Draw(pDC);   
}

BOOL CSObjPiston::SetCursor(CWnd* pWnd, UINT nHitTest, int nMultiple) const
{
	// Called to see if should change cursor as moved over
	// tracking rectangle on selected obj. Usage copied from
	// CRectTracker, and by default pass it straight on to our
	// tracker. Except extra parameter so we know whether we
	// have multiple selection (which bars resizes).
	
	if ((NULL != m_pTracker) && (NULL != m_pRestTracker))
	{
		BOOL ret = m_pTracker->SetCursor(pWnd, nHitTest, nMultiple);
		
		if (!ret) ret = m_pRestTracker->SetCursor(pWnd, nHitTest, nMultiple);  
		
		return ret;
	}
	else
	{
		ASSERT(FALSE); // shouldn't have got called 
		
		return FALSE;
	}
}
	
int CSObjPiston::TrackerHitTest(CPoint point)  
{       
	// Called to see if mouse moved over or clicked on tracking rectangle(s)
	// belonging to this object; privately note which hit, if either 
	
	if ((NULL != m_pTracker) && (NULL != m_pRestTracker))
	{        
		m_nHitRestTracker = FALSE;
		
		if (CRectTracker::hitNothing != m_pTracker->HitTest(point))
		{
			return TRUE;
		}  
		else if (CRectTracker::hitNothing != m_pRestTracker->HitTest(point))
		{
			m_nHitRestTracker = TRUE;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		ASSERT(FALSE); // shouldn't have got called 
		
		return FALSE;
	}
}

int CSObjPiston::Track(int nMultiple, CWnd* pWnd, CPoint point)
{
	// If click on tracking rectangle of selected object, this function called
	// to allow user to drag it about (and resize if appropriate)  

	// Beware this code rather duplicates base version
		
	if ((NULL != m_pTracker) && (NULL != m_pRestTracker))
	{
		int nTrackerAltered;
		int nResized;
		
		if (m_nHitRestTracker)
		{
			nTrackerAltered = m_pRestTracker->Track(nMultiple, pWnd, point, TRUE /* allow invert */);
			nResized = m_pRestTracker->m_nHaveResized;
		}
		else
		{   
			nTrackerAltered = m_pTracker->Track(nMultiple, pWnd, point, TRUE /* allow invert */);
			nResized = m_pTracker->m_nHaveResized;
		}
		
		if (nTrackerAltered)
		{
			// Selection was moved; get co-ords of new rectangle and displacement
			// vector from original position,all in simulation co-ords:
			
			double x1, y1;  // old top left position
			CPoint point1(m_pTracker->m_startRect.left, m_pTracker->m_startRect.top); // where started
			double x2, y2;  // new top left position
			CPoint point2(m_pTracker->m_rect.left, m_pTracker->m_rect.top); // top left of where ended up
            double xr, yb;  // new bottom right position
            CPoint pointBR(m_pTracker->m_rect.right, m_pTracker->m_rect.bottom);
            ((CSimView*) pWnd)->SimifyMousePoint(point1, &x1, &y1);
            ((CSimView*) pWnd)->SimifyMousePoint(point2, &x2, &y2);
			((CSimView*) pWnd)->SimifyMousePoint(pointBR, &xr, &yb);
			
			if (nMultiple)
			{
				// Did do move, multiple selection, handle generically
				
				sprintf(buff, "seln_move x %g y %g", x2 - x1, y2 - y1);
				
				m_pSim->HandleNewCommand(buff);
			}
			else
			{
				// single selection, obj type specific behaviour 
				
				OnMoveOrResize(x2, y2, xr, yb, x1, y1, nResized);
			}
			
			return TRUE;
		}
		else
		{
			// no move or pressed Esc or right button to cancel tracking
			return FALSE;
		}
	}
	else
	{
		ASSERT(FALSE); // shouldn't have got called
		
		return FALSE;
	}	
}

void CSObjPiston::OnMoveOrResize(double dNewXLeft, double dNewYTop,
	                       double dNewXRight, double dNewYBottom,
	                       double dOldXLeft, double dOldYTop,
	                       int nResized)
{    
	// When single selection has been manipulated by mouse, end up
	// here to issue command to sim to alter it. Needs to be overridable
	// eg for piston to handle separate rest position and actual piston,
	// and for resizable things to issue 'change' commmand on themselves.
    
    double dDeltaX = dNewXLeft - dOldXLeft;
    double dDeltaY = dNewYTop - dOldYTop;
	    
    double dDeltaWidth = dNewXRight - dNewXLeft - m_dLengthHorizontal;
    double dDeltaHeight = dNewYTop - dNewYBottom - m_dLengthVertical;
	
	               
	// Find out whether user wanted to move both piston and rest position,
	// or just the one they dragged, if they didn't do a resize that is:

	int nResponse = IDYES;  // will do both rest pos and piston normally	
	
	if (!nResized)
	{       
		if (m_nHitRestTracker)
		{
			nResponse = AfxMessageBox("You moved the rest position rectangle of this piston;"
			                          " do you want to move the actual piston position along"
			                          " with it?",
			                          MB_YESNOCANCEL);
		}
		else
		{
			nResponse = AfxMessageBox("You moved the position of this piston;"
			                          " do you want to move its resting position along"
			                          " with it?",
			                          MB_YESNOCANCEL);
		}
	}
	
	if (IDCANCEL != nResponse)
	{
		CString cmd("change piston \"");
		cmd += m_pType->m_sName; 
		cmd += "\" ";
	    
	    if (!nResized)
	    {                    
	    	if (m_nHitRestTracker || (IDYES == nResponse))
			{
				m_dXLRest += dDeltaX;
				m_dYTRest += dDeltaY;
			}
			
			if (!m_nHitRestTracker || (IDYES == nResponse))
			{
				m_dXLeft += dDeltaX;
				m_dXRight += dDeltaX;
				m_dYTop += dDeltaY;
				m_dYBottom += dDeltaY;
			}
		}
		else
		{
			m_dXLeft += dDeltaX;
			m_dXRight = m_dXLeft + m_dLengthHorizontal + dDeltaWidth;
			m_dYTop += dDeltaY;
			m_dYBottom = m_dYTop - m_dLengthVertical - dDeltaHeight;
			m_dXLRest += dDeltaX;
			m_dYTRest += dDeltaY;
		}			 
	
	    WriteState(cmd); 
					
		m_pSim->HandleNewCommand(cmd);
	}
}
	                                  
void CSObjPiston::PrepareOtherTrackersToFollow()
{
	// when leading multiple selection drag 
	// We also need to prepare our own extra tracker, other
	// one from that hit
	
	if (m_nHitRestTracker)
	{
		m_pTracker->PrepareToFollow();
	}
	else                              
	{                                 
		m_pRestTracker->PrepareToFollow();
	}
	
	m_pSim->PrepareTrackersToFollow(this);
}

void CSObjPiston::UpdateFollowingTrackers(int nDeltaXL, int nDeltaYT,
	                                      int nDeltaXR, int nDeltaYB,
	                                      CWnd* pWnd)
{                       
	// As well as getting other objects to follow our tracker, 
	// which is being moved  by mouse, need to move its companion
	// tracker belonging to this piston. Also need to forcibly resize
	// companion if leading one has changed size

	if (m_nHitRestTracker)
	{
		m_pTracker->UpdateTrackerAsFollowing(nDeltaXL, nDeltaYT, nDeltaXR, nDeltaYB, pWnd);
	}
	else                              
	{                                    
		m_pRestTracker->UpdateTrackerAsFollowing(nDeltaXL, nDeltaYT, nDeltaXR, nDeltaYB, pWnd);
	}
	
	CSObj::UpdateFollowingTrackers(nDeltaXL, nDeltaYT, nDeltaXR, nDeltaYB, pWnd);
}

void CSObjPiston::UpdateTrackerAsFollowing(int nDeltaX, int nDeltaY, CWnd* pWnd)
{
	if ((NULL != m_pTracker) && (NULL != m_pRestTracker))
	{
		m_pTracker->UpdateTrackerAsFollowing(nDeltaX, nDeltaY, nDeltaX, nDeltaY, pWnd);
		m_pRestTracker->UpdateTrackerAsFollowing(nDeltaX, nDeltaY, nDeltaX, nDeltaY, pWnd);
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CSObjPiston::PrepareTrackerForFollowing()
{             
	if ((NULL != m_pTracker) && (NULL != m_pRestTracker))
	{
		m_pTracker->PrepareToFollow();
		m_pRestTracker->PrepareToFollow();
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CSObjPiston::HandleMove(double dDeltaX, double dDeltaY)
{
	m_dXLeft += dDeltaX;	
	m_dXRight += dDeltaX;
	m_dXLRest += dDeltaX;
	
	m_dYTop += dDeltaY;
	m_dYBottom += dDeltaY;   
	m_dYTRest += dDeltaY; 
}

void CSObjPiston::ObjectProperties() 
{
	CSPistonPropsDlg dlg;
	
	dlg.m_dXLeft = m_dXLeft;
	dlg.m_dYTop = m_dYTop;
	dlg.m_dXRight = m_dXRight;
	dlg.m_dYBottom = m_dYBottom; 
	dlg.m_dWidth = m_dLengthHorizontal;
	dlg.m_dHeight = m_dLengthVertical;
	dlg.m_dXLRest = m_dXLRest;
	dlg.m_dYTRest = m_dYTRest;
	dlg.m_dVx = m_dVx;
	dlg.m_dVy = m_dVy;
	
	dlg.m_pPistonType = (CObjTypePiston*) m_pType;
	dlg.m_pSim = m_pSim;
	
	if (IDOK == dlg.DoModal())
	{
		int nModified = FALSE;
		
		// if height or width were updated, corner coords would have been
		// to, so haven't checked those
		
		if (fabs(dlg.m_dXLeft - m_dXLeft) > 0.001) nModified = TRUE;
		if (fabs(dlg.m_dYTop - m_dYTop) > 0.001) nModified = TRUE;
		if (fabs(dlg.m_dXRight - m_dXRight) > 0.001) nModified = TRUE;
		if (fabs(dlg.m_dYBottom - m_dYBottom) > 0.001) nModified = TRUE;
		if (fabs(dlg.m_dXLRest - m_dXLRest) > 0.001) nModified = TRUE;
		if (fabs(dlg.m_dYTRest - m_dYTRest) > 0.001) nModified = TRUE;
		if (fabs(dlg.m_dVx - m_dVx) > 0.1) nModified = TRUE;
		if (fabs(dlg.m_dVy - m_dVy) > 0.1) nModified = TRUE;
		
				
		if (nModified)
		{ 
			m_dXLeft = dlg.m_dXLeft;
			m_dYTop = dlg.m_dYTop;
			m_dXRight = dlg.m_dXRight;
			m_dYBottom = dlg.m_dYBottom;
			m_dXLRest = dlg.m_dXLRest;
			m_dYTRest = dlg.m_dYTRest;
			m_dVx = dlg.m_dVx;
			m_dVy = dlg.m_dVy;
	
			CString cmd("change piston \"");
			cmd += m_pType->m_sName;
			cmd += "\" ";
			
			WriteState(cmd);
			
			m_pSim->HandleNewCommand(cmd);
		}
	}
}
