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
#include "sim.h"      
#include "tracker.h"
#include "mol.h"
#include "lennard.h"
#include "drwmoldg.h"
#include "molprpdg.h"   
#include "smolprdg.h"

#include <math.h>



#ifdef _DEBUG
extern long global_debug_counter;
#endif

//////////////////////////////////////////////////////////////////////                                 
// CObjTypeMol functionality                        
//////////////////////////////////////////////////////////////////////                                 

// constructor

CObjTypeMol::CObjTypeMol()
{           
    m_lNumFillMols = 0;
	m_nHasInts = TRUE;
	m_dPackingStandoff = 1e-3;
	m_nIsMobile = TRUE;
    m_dFormationEnergy = 0;
	m_dLineThick = 0.2; 
	m_dMass = 20;
	m_dHSRadius = 0.07;
	m_bLineRed = 255;
	m_bLineGreen = 0;
	m_bLineBlue = 255;
	m_bFillRed = 255;
	m_bFillGreen = 0;
	m_bFillBlue = 0;  
	m_nMolScreenRadius = 5; 
	m_nMolScreenDiameter = 10;
	m_nScreenLineThick = 1;
	m_nUseGridWhenGridOn = FALSE;
    m_lNextIDForReactionProduct = MOL_FIRST_REACTION_PRODUCT_ID;
	m_nSpeedType = DM_SPECIFIC_THERMAL;
	m_nDirectionType = DM_DIRECTION_FIXED;
	m_dDirectionAngle = 0;
	m_dFixedSpeedValue = 0;
	m_dThermalTemperatureValue = 100;	
}            


// Command list
CMD_ENTRY CObjTypeMol::m_celCObjTypeMolCommands[MAX_COMMANDS_MOL] = 
{                                        
	{"name",      CA_STRING, CT_STATE,   CI_NAME,      LIM_MIN_CHARS_OBJTYPE, LIM_MAX_CHARS_OBJTYPE},
	{"radius",    CA_DOUBLE, CT_STATE,   CI_HSRADIUS,  LIM_MIN_MOL_RADIUS,  LIM_MAX_MOL_RADIUS},
	{"mass",      CA_DOUBLE, CT_STATE,   CI_MASS,      LIM_MIN_MOL_MASS,  LIM_MAX_MOL_MASS},     
	{"form_energy", CA_DOUBLE, CT_STATE, CI_FORM_ENERGY, LIM_MIN_FORMATION_ENERGY,  LIM_MAX_FORMATION_ENERGY},     
	{"fillred",   CA_BYTE,   CT_STATE,   CI_FILLRED,   0,     255},
	{"fillblue",  CA_BYTE,   CT_STATE,   CI_FILLBLUE,  0,     255},
	{"fillgreen", CA_BYTE,   CT_STATE,   CI_FILLGREEN, 0,     255},
	{"linered",   CA_BYTE,   CT_STATE,   CI_LINERED,   0,     255},
	{"lineblue",  CA_BYTE,   CT_STATE,   CI_LINEBLUE,  0,     255},
	{"linegreen", CA_BYTE,   CT_STATE,   CI_LINEGREEN, 0,     255},
	{"linethick", CA_DOUBLE, CT_STATE,   CI_LINETHICK, 0,     255},   
	{"left",      CA_DOUBLE, CT_CONTROL, CI_LEFT,      -10,   10 },
	{"right",     CA_DOUBLE, CT_CONTROL, CI_RIGHT,     -10,   10 },
	{"bottom",    CA_DOUBLE, CT_CONTROL, CI_BOTTOM,    -10,   10 },
	{"top",       CA_DOUBLE, CT_CONTROL, CI_TOP,       -10,   10 },
	{"end",       CA_NONE,   CT_ENDLIST, CI_ENDLIST,   0,     0}
};

// Command handlers                            
int CObjTypeMol::CmdMass()
{
	m_dMass = m_dArg;   
	return SR_OK;
}

int CObjTypeMol::CmdHSRadius()
{
	m_dHSRadius = m_dArg;
	return SR_OK;
}           

int CObjTypeMol::CmdLineRed()
{
	m_bLineRed = m_bArg;
	return SR_OK;
}

int CObjTypeMol::CmdLineGreen()
{
	m_bLineGreen = m_bArg;
	return SR_OK;
}

int CObjTypeMol::CmdLineBlue()
{
	m_bLineBlue = m_bArg;
	return SR_OK;
}

int CObjTypeMol::CmdFillRed()
{
	m_bFillRed = m_bArg;
	return SR_OK;
}

int CObjTypeMol::CmdFillGreen()
{
	m_bFillGreen = m_bArg;
	return SR_OK;
}

int CObjTypeMol::CmdFillBlue()
{
	m_bFillBlue = m_bArg;
	return SR_OK;
}           

int CObjTypeMol::CmdLineThick()
{
	if (m_dArg < 0.0)
	{
		m_dLineThick = 0.0;
	}
	else if (m_dArg > 1.0)
	{               
		m_dLineThick = 1.0;
	}
	else
	{   
		m_dLineThick = m_dArg;
	}      
	return SR_OK;
}

// Functionality overridden from CSimObj                    
void CObjTypeMol::MoveSObjs()
{                      
#ifdef _DEBUG
	global_debug_counter++;
#endif

	CSObjMol* pMol;  
	pMol = (CSObjMol*) m_pFirstSObj;       
	
	// time in ns, dist in nm, vels in m/s, so all OK
	double dt = m_pSim->m_dTimeSoFar;

	while (NULL != pMol)
    {                 
    	if (pMol->m_nTouched)
    	{
    		// has already moved a bit through hard interactions
    		dt -= pMol->m_dTimeDoneAlready;
    		pMol->m_dX += pMol->m_dVx * dt;
    		pMol->m_dY += pMol->m_dVy * dt; 
    	    dt = m_pSim->m_dTimeSoFar;
    	}  
    	else
    	{   
    		// hasn't yet moved this timestep
	    	pMol->m_dX += pMol->m_dVx * dt;
	    	pMol->m_dY += pMol->m_dVy * dt; 
    	}


   		pMol = (CSObjMol*) pMol->m_pNextSObj;

    }
	
}     

void CObjTypeMol::ContributeNumerics()
{   
	// initialise numerics with names etc
	m_numMass.m_pSim = m_pSim;
	m_numMass.m_sName = m_sName + ":mass";
	m_numMass.m_sUnits = "amu";
	m_numMass.m_sDescription = "molecular mass for this type";
	m_numMass.m_pdValue = &m_dMass; 
	m_numMass.m_nType = NUM_USERWRITE | NUM_USERREAD;
	m_numMass.m_dMin = LIM_MIN_MOL_MASS;
	m_numMass.m_dMax = LIM_MAX_MOL_MASS;
	
	m_numRadius.m_pSim = m_pSim;
	m_numRadius.m_sName = m_sName + ":radius";
	m_numRadius.m_sUnits = "nm";
	m_numRadius.m_sDescription = "hard-sphere radius for this type";
	m_numRadius.m_pdValue = &m_dHSRadius; 
	m_numRadius.m_nType = NUM_USERREAD;

   	m_numFormationEnergy.m_pSim = m_pSim;
	m_numFormationEnergy.m_sName = m_sName + ":form_energy";
	m_numFormationEnergy.m_sUnits = "kJ/mol";
	m_numFormationEnergy.m_sDescription = "inherent potential energy";
	m_numFormationEnergy.m_pdValue = &m_dFormationEnergy; 
	m_numFormationEnergy.m_nType = NUM_USERWRITE | NUM_USERREAD;
	m_numFormationEnergy.m_dMin = LIM_MIN_FORMATION_ENERGY;
	m_numFormationEnergy.m_dMax = LIM_MAX_FORMATION_ENERGY;

	// make public 
	m_pSim->AcceptNewNumeric(&m_numMass);
	m_pSim->AcceptNewNumeric(&m_numRadius);
    m_pSim->AcceptNewNumeric(&m_numFormationEnergy);
}          


void CObjTypeMol::ApplyAccels()
{
	CSObjMol* pMol = (CSObjMol*) m_pFirstSObj;

	// accels are in J/mol/nm, time is in ns, mass in amu, vels in m/s (or nm/ns)
		
	// haven't sussed units yet, guessing wildly about 1e-18 (m2 to nm2)
	double dDeltaTimeOverMass = m_pSim->m_dDeltaTime / (AVAGADRO * m_dMass * ONE_AMU);
	
	while (NULL != pMol)
    {
		ASSERT(pMol->m_fsValid.bf.vx);
		ASSERT(pMol->m_fsValid.bf.vy);
		
        pMol->m_dVx += pMol->m_dFx * dDeltaTimeOverMass;
		pMol->m_dVy += pMol->m_dFy * dDeltaTimeOverMass;
		
		if (m_pSim->m_nGravityOn)
		{
			pMol->m_dVy += m_pSim->m_dGravitationalYVelIncrement; 
		}

		pMol->m_dSpeed = sqrt(pMol->m_dVx * pMol->m_dVx + pMol->m_dVy * pMol->m_dVy);		
	
    	if (pMol->m_dSpeed > NEARLY_ZERO)
    	{
    		pMol->m_dCx = pMol->m_dVx / pMol->m_dSpeed;
    		pMol->m_dCy = pMol->m_dVy / pMol->m_dSpeed;
    	}
    	else
    	{
    		// mol pretty well stopped, direction arbitrary
    		pMol->m_dCx = 1.0;
    		pMol->m_dCy = 0;
    	}                 
    	
    	pMol->ClipSpeed();
    	
#ifdef _DEBUG
		pMol->m_fsValid.bf.speed = 1;
		pMol->m_fsValid.bf.cosines = 1;
		pMol->m_fsValid.bf.vx = 1;
		pMol->m_fsValid.bf.vy = 1;
#endif		
    			
    	pMol = (CSObjMol*) pMol->m_pNextSObj;
    }                           
}

double CObjTypeMol::PrepStep()
{
	// called prior to calc of forces, etc; should return fastest
	// vel squared
	
	CSObjMol* pMol = (CSObjMol*) m_pFirstSObj;
	
	double dVSquared;
	double dMaxVSquared = 0;
	
	while (NULL != pMol)
    {
    	dVSquared = pMol->m_dVx * pMol->m_dVx + pMol->m_dVy * pMol->m_dVy;

		if (dVSquared > dMaxVSquared)
		{
			dMaxVSquared = dVSquared;
		}
        
        // Haven't yet decided whether accels or forces will be summed,
        // so for now zero both:
        pMol->m_dFx = 0;
		pMol->m_dFy = 0;
		pMol->m_dAx = 0;
		pMol->m_dAy = 0;
		
		pMol->m_dPE = 0;   
		pMol->m_dVirial = 0;
		pMol->m_dImpulseSum = 0;
		
		// note this mol not touched by hard events
		pMol->m_nTouched = FALSE;
		
    	pMol = (CSObjMol*) pMol->m_pNextSObj;
    }                           
    
    return dMaxVSquared;
}

int CObjTypeMol::GetStateParam(int command)
{         
	int ret = SR_OK;
	
	switch(command)
	{
		case CI_MASS:
			m_dArg = m_dMass;
			break;

		case CI_HSRADIUS:
			m_dArg = m_dHSRadius;
			break;    
            
        case CI_FORM_ENERGY:
            m_dArg = m_dFormationEnergy;
            break;
			
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
			
		case CI_FILLRED:
			m_bArg = m_bFillRed;
			break;
			
		case CI_FILLGREEN:
			m_bArg = m_bFillGreen;
			break;

		case CI_FILLBLUE:
			m_bArg = m_bFillBlue;
			break;    
			
		case CI_LINETHICK:
			m_dArg = m_dLineThick;
			break;
			
		default:
			TRACE("Asked for param for unknown command in CObjTypeMol\n");
			ret = SR_ERROR;
			break;
	}             
	
	return ret;
}          

CMD_ENTRY* CObjTypeMol::GetCmdList()
{
	return &(m_celCObjTypeMolCommands[0]);
}
 
int CObjTypeMol::AcceptCommandLine(CString& remainingLine, CString& errString)
{
	int command;
	int ret;
    
    ret = GrabCommand(remainingLine, errString, command);            
    while ((CI_EMPTY != command) && (SR_ERROR != ret))
    {
		switch (command)
		{   
			case CI_HSRADIUS:
				ret |= CmdHSRadius();
				break;
			
			case CI_MASS: 
				ret |= CmdMass();
				break;

            case CI_FORM_ENERGY:
                m_dFormationEnergy = m_dArg;
                ret |= SR_OK;
                break;
			
			case CI_NAME:
				m_sName = m_sArg; 
				ret |= SR_OK;
				break;   
				
			case CI_LINERED:
				ret |= CmdLineRed();
				break;
				
			case CI_LINEGREEN:
				ret |= CmdLineGreen();
				break;
				
			case CI_LINEBLUE:
				ret |= CmdLineBlue();
				break;

			case CI_FILLRED:
				ret |= CmdFillRed();
				break;
				
			case CI_FILLGREEN:
				ret |= CmdFillGreen();
				break;
				
			case CI_FILLBLUE:
				ret |= CmdFillBlue();
				break;
			
			case CI_LINETHICK:
				ret |= CmdLineThick();
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

void CObjTypeMol::Initialise()
{
	double radius = (m_dHSRadius * m_pSim->m_dScaleFactor);
	if (radius <= MAX_SCREEN_RADIUS)
	{      
		m_nMolScreenRadius = (int) radius;
	}
	else
	{    
		m_nMolScreenRadius = MAX_SCREEN_RADIUS;
	}
	
	m_nMolScreenDiameter = m_nMolScreenRadius * 2;
    m_nScreenLineThick = int (m_dLineThick * m_nMolScreenRadius);

}

CSObjMol* CObjTypeMol::CreateNewMol(double dX, double dY, double dTemperature, long lId)
{
  	// This used my monitors to add mols to simulation on the fly 
    double dSpeed = m_pSim->ThermalSpeed(dTemperature, m_dMass);
    double dDirection = m_pSim->RandomDirection();
    return CreateNewMol(dX, dY, dSpeed, dDirection, lId);
}

CSObjMol* CObjTypeMol::CreateNewMol(double dX, double dY, double dSpeed, double dDirection)
{
    // used for reactive collision products
    long lId = m_lNextIDForReactionProduct++;
    return CreateNewMol(dX, dY, dSpeed, dDirection, lId);
}

CSObjMol* CObjTypeMol::CreateNewMol(double dX, double dY,  double dSpeed, double dDirection, long lId)
{                            
	
	CSObjMol* pMol = new CSObjMol;
	
	pMol->m_pSim = m_pSim;   
	pMol->m_pType = this;
	pMol->m_lId = lId;   

	pMol->m_fsValid.all = 0;
		
	pMol->m_dX = dX;
	pMol->m_dY = dY;
	pMol->m_fsValid.bf.x = 1;
	pMol->m_fsValid.bf.y = 1;
	
	pMol->m_dDirection = dDirection;
	pMol->m_fsValid.bf.direction = 1; 
	
	pMol->m_dCx = cos(pMol->m_dDirection);
	pMol->m_dCy = sin(pMol->m_dDirection);
	pMol->m_fsValid.bf.cosines = 1;
	
	pMol->m_dSpeed = dSpeed;
	pMol->m_fsValid.bf.speed = 1;

	pMol->m_dVx = pMol->m_dSpeed * pMol->m_dCx;
	pMol->m_dVy = pMol->m_dSpeed * pMol->m_dCy;
	pMol->m_fsValid.bf.vx = 1; 
	pMol->m_fsValid.bf.vy = 1; 
    
    
    pMol->ClipSpeed();

    if (!AddObj(pMol))
    {
    	// failed!
    	delete pMol;
        return NULL;
    }
    else
    {
        return pMol;
    }
}

int CObjTypeMol::DoCmdFillMol(CString& remainingLine, CString& errString,
	                  long& lFirstId, long& lLastId, int& nIdRangeOK)
{
	// need to gather co-ords of fill box and pass rest on to normal mol init
	double left, right, top, bottom;
	int gotLeft = 0, gotRight = 0, gotTop = 0, gotBottom = 0;
    
    int command;
    int ret = GrabCommand(remainingLine, errString, command);            
    while ((CI_EMPTY != command) && (SR_ERROR != ret)
           && (gotLeft + gotRight + gotTop + gotBottom < 4))
    {
    	switch (command)
    	{
    		case CI_LEFT:
    			left = m_dArg;
    			gotLeft = 1;
    			break;
    			
    		case CI_RIGHT:
    			right = m_dArg;
    			gotRight = 1;
    			break;
    			
    		case CI_BOTTOM:  
    			bottom = m_dArg;
    			gotBottom = 1;
    			break;
    			
    		case CI_TOP:
    			top = m_dArg;
    			gotTop = 1;
    			break;
    			
    		default:
    			ret = SR_ERROR;
    			errString += "failed to get box coords for fill rectangle; ";
    			break;
    	}    
    	
    	if (gotLeft + gotRight + gotTop + gotBottom < 4)
    	{
    		ret |= GrabCommand(remainingLine, errString, command);
    	}
    } 
    
	if (SR_ERROR != ret)
	{
		// have coords for box, so get list of mols that fit into it
		CPtrList fillCoordList;
		GetMolFillCoords(&fillCoordList, left, bottom,
		                  right, top, m_dHSRadius);
		
		// decide what ids to give mols: fit in specified range if poss

		int nMolsToCreate = fillCoordList.GetCount();
		
		if ((lFirstId && lLastId) && (lFirstId + nMolsToCreate - 1 <= lLastId))
		{
			// range specified and will fit in specified range, leave alone 
			nIdRangeOK = TRUE;
		}
		else
		{
			// make new range
			lFirstId = ++m_lLastIdUsed;
			lLastId = lFirstId + nMolsToCreate - 1;
			m_lLastIdUsed = lLastId;
			
			nIdRangeOK = FALSE;
		}

		CSimPoint* pSimPoint;                  
        CString molCmd;
        CString nullString;
        
        theApp.BeginWaitCursor();
        
        // We issue a command to CSim for each mol we want to create
        // rather than doing it ourselves just so that the other options
        // (eg speed and direction options) are handled exactly as for
        // single molecules:
        
        long lId = lFirstId;
        
		while (!fillCoordList.IsEmpty() && (SR_ERROR != ret))
		{                                                       
			molCmd = "mol \"" + m_sName + "\" ";
			pSimPoint = (CSimPoint*) fillCoordList.RemoveHead();
			sprintf(buff, "id %ld x %g y %g ", lId++, pSimPoint->m_dX, pSimPoint->m_dY);
			molCmd += buff;
			molCmd += remainingLine;
			nullString = "";
			ret |= m_pSim->AcceptCommandLine(molCmd, nullString);	 
			delete pSimPoint;
		} 
		
		// If we didn't finish the list, may have undeleted CSimPoints
		while (!fillCoordList.IsEmpty())
		{
			pSimPoint = (CSimPoint*) fillCoordList.RemoveHead();
			delete pSimPoint;
		}
		
		errString += nullString; // get only one copy if any errors
		
		remainingLine = "";      // used it all up
		
		theApp.EndWaitCursor();
	}
	
	return ret; 
}
                        
int CObjTypeMol::DrawCommand()
{                 
	CDrawMolsDlg dlg;  
	
	dlg.m_nSpeedType = DM_SYSTEM_THERMAL;
	dlg.m_nDirectionType = DM_DIRECTION_RANDOM;
	dlg.m_dDirectionAngle = 0; 
	dlg.m_dFixedSpeedValue = 0;
	dlg.m_dThermalTemperatureValue = 100;
	
	if (IDOK == dlg.DoModal())
	{
		// do drawing with conditions specified in dlg box 
		
		m_nSpeedType = dlg.m_nSpeedType;
		m_nDirectionType = dlg.m_nDirectionType; 
		m_dDirectionAngle = dlg.m_dDirectionAngle;
		m_dFixedSpeedValue = dlg.m_dFixedSpeedValue;
		m_dThermalTemperatureValue = dlg.m_dThermalTemperatureValue; 

		return TRUE;
	}
	else
	{
		// user cancelled, do no drawing
		return FALSE;
	}	
}                  

void CObjTypeMol::PropertiesCommand()
{           
	CEditMolPropsDlg dlg;
	dlg.m_cFillColour = RGB(m_bFillRed, m_bFillGreen, m_bFillBlue); 
	dlg.m_cLineColour = RGB(m_bLineRed, m_bLineGreen, m_bLineBlue); 
	dlg.m_dLineThick = m_dLineThick;
	dlg.m_dRadius = m_dHSRadius;
	dlg.m_dMass = m_dMass;
    dlg.m_dFormationEnergy = m_dFormationEnergy;
	dlg.m_sName = m_sName; 
	dlg.m_pSim = m_pSim;
	
	if (IDOK == dlg.DoModal())
	{
		// build up command to change molecule type properties
		
		CString cmd;
		
		// user may clone mol type by changing name
		
		if (m_sName == dlg.m_sName)
		{
			cmd = "change objtype mol name \"";
		}
		else
		{
			cmd = "objtype mol name \"";
		}
		
		cmd += dlg.m_sName;
		cmd += "\" mass ";
		sprintf(buff, "%g", dlg.m_dMass);
		cmd += buff;
		cmd += " radius ";
		sprintf(buff, "%g", dlg.m_dRadius);    
		cmd += buff; 
		cmd += " form_energy ";
		sprintf(buff, "%g", dlg.m_dFormationEnergy);    
		cmd += buff; 
		cmd += " linethick ";
		sprintf(buff, "%g", dlg.m_dLineThick);    
		cmd += buff; 
		cmd += " fillred ";
		sprintf(buff, "%d", (int) (dlg.m_cFillColour & 0x000000ff));
		cmd += buff;
		cmd += " fillgreen ";      
		sprintf(buff, "%d", (int) ((dlg.m_cFillColour & 0x0000ff00) >> 8));
		cmd += buff;
		cmd += " fillblue ";      
		sprintf(buff, "%d", (int) ((dlg.m_cFillColour & 0x00ff0000) >> 16));
		cmd += buff;
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
		// user will have pressed cancel
	}

}                                   

void CObjTypeMol::DrawAllSObjs(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2)
{
	// will set up DC for drawing mols of this type,
	// and draw them all, here
	
	// pSpareDC1 ends up as coloured molecule surrounded by white, 
	// which is then ANDed with the rest of the picture so far. Had
	// intended to first AND with a black circle surrounded by white,
	// to mask out whatever might overlap molecule, and then OR in
	// a molecule surrounded by black, but simpler (and faster) scheme
	// works OK.

	// First paint DC1 entirely white
		
	pSpareDC1->SelectStockObject(WHITE_BRUSH);    

	// this bitmap is 1 pixel wider and taller than I think it need be;
	// otherwise sometimes get unexpected black lines to right or below
	// molecules on NT box.
	pSpareDC1->PatBlt(0, 0, m_nMolScreenDiameter + 2, -(m_nMolScreenDiameter + 2), PATCOPY);

    
    CPen molLinePen(PS_SOLID, m_nScreenLineThick,
	                    RGB(m_bLineRed, m_bLineGreen, m_bLineBlue));
	CPen* pOldPen = pSpareDC1->SelectObject(&molLinePen);
    
	COLORREF colInterior = RGB(m_bFillRed, m_bFillGreen, m_bFillBlue);
	
	if (RGB(255, 255, 255) == colInterior)
	{   
		// Debug version asserts if use pure white brush, finds handle
		// already there when does LookUpPermanent or something. Ho, hum.
		colInterior = RGB(255, 255, 254);  
	}
	
	CBrush molInteriorBrush(colInterior);
	
	CBrush* pOldBrush = pSpareDC1->SelectObject(&molInteriorBrush);

	pSpareDC1->Ellipse(0 + m_nScreenLineThick / 2,
	                   0 - m_nScreenLineThick / 2,
	                   m_nMolScreenDiameter - m_nScreenLineThick / 2,
	                   m_nScreenLineThick / 2 - m_nMolScreenDiameter);    
	

	CSObjMol* pMol;
	pMol = (CSObjMol*) m_pFirstSObj;
	
	CRect rect;
	CPoint point;

	while (NULL != pMol)
	{                                              
		point = m_pSim->SimToLogical(pMol->m_dX, pMol->m_dY); 
		point.x -= m_nMolScreenRadius;
		point.y += m_nMolScreenRadius;
		pDC->BitBlt(point.x, point.y, m_nMolScreenDiameter + 1, -(m_nMolScreenDiameter + 1),
		            pSpareDC1, 0, 0, SRCAND);  

		pMol = (CSObjMol*) pMol->m_pNextSObj;
	}
	  
	pSpareDC1->SelectObject(pOldBrush);
	pSpareDC1->SelectObject(pOldPen);
}   

void CObjTypeMol::UserDrawing(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2)
{
	CPoint point;

    CPen molLinePen(PS_SOLID, m_nScreenLineThick,                   
	                    RGB(m_bLineRed, m_bLineGreen, m_bLineBlue));  
	                    
	COLORREF colInterior = RGB(m_bFillRed, m_bFillGreen, m_bFillBlue);
	
	if (RGB(255, 255, 255) == colInterior)
	{   
		// Debug version asserts if use pure white brush, finds handle
		// already there when does LookUpPermanent or something. Ho, hum.
		colInterior = RGB(255, 255, 254);  
	}
	
	CBrush molInteriorBrush(colInterior);
   
	if (SM_DRAGGING == m_pSim->m_nDrawingState)
	{
		// Draw rectangle to indicate dragged area, even if pointer has
		// gone outside of client rectangle 
		CPoint originPt = m_pSim->m_ptLogDragStart;
		CPoint currentPt = m_pSim->m_ptLogMouse;
		
		CRect rect(originPt.x, originPt.y, currentPt.x, currentPt.y);
		
		CPtrList fillCoordList;
		GetMolFillCoords(&fillCoordList, m_pSim->m_dSimDragStartX, m_pSim->m_dSimDragStartY,
		                  m_pSim->m_dSimMouseX, m_pSim->m_dSimMouseY, m_dHSRadius);
		
		CSimPoint* pSimPoint;                  

		pDC->SelectObject(&molLinePen);
		pDC->SelectObject(&molInteriorBrush);	
        
        m_lNumFillMols = 0;
		while (!fillCoordList.IsEmpty())
		{   
			pSimPoint = (CSimPoint*) fillCoordList.RemoveHead();
			point = m_pSim->SimToLogical(pSimPoint->m_dX, pSimPoint->m_dY); 
			point.x -= m_nMolScreenRadius;
			point.y += m_nMolScreenRadius;
			pDC->Ellipse(point.x + m_nScreenLineThick / 2,
			             point.y - m_nScreenLineThick / 2,
			             point.x + m_nMolScreenDiameter - m_nScreenLineThick / 2,
			             point.y - m_nMolScreenDiameter + m_nScreenLineThick / 2);
			delete pSimPoint;
			m_lNumFillMols++;
		}                    
		
		
		int rop2 = pDC->GetROP2();
		pDC->SelectStockObject(BLACK_PEN);
		pDC->SelectStockObject(WHITE_BRUSH);
		pDC->SetROP2(R2_MASKPEN); 
		pDC->Rectangle(&rect);    
		pDC->SetROP2(rop2);
	}
	else
	{
		// Not dragging
		// Draw a mol wherever the mouse is pointing, unless have gone
		// out of client rectangle
		if (m_pSim->m_bInClientRect)
		{
			CPen* pOldPen = pDC->SelectObject(&molLinePen);
			CBrush* pOldBrush = pDC->SelectObject(&molInteriorBrush);	
			
			point = m_pSim->SimToLogical(m_pSim->m_dSimMouseX, m_pSim->m_dSimMouseY); 
			point.x -= m_nMolScreenRadius;
			point.y += m_nMolScreenRadius;
			pDC->Ellipse(point.x + m_nScreenLineThick / 2,
			             point.y - m_nScreenLineThick / 2,
			             point.x + m_nMolScreenDiameter - m_nScreenLineThick / 2,
			             point.y - m_nMolScreenDiameter + m_nScreenLineThick / 2); 
			             
			pDC->SelectObject(pOldPen);
			pDC->SelectObject(pOldBrush);
		} 
	}
}

void CObjTypeMol::GetMolFillCoords(CPtrList* ptrList, double left, double bottom,
	                         double right, double top, double radius)
{
	// given the bounding rectangle supplied, this function populates
	// the pointer list supplied with the coords of the mols reqd to
	// fill the rectangle with hexagonal close packing    
	double temp;
	
	// Normalise rectangle so that top left is just that.
	if (left > right)
	{
		temp = left;
		left = right;
		right = temp;
	}
	
	if (bottom > top)
	{
		temp = bottom;
		bottom = top;
		top = temp;
	}
	
	double height = top - bottom;
	double width = right - left;
	
	// keep mols slightly apart to avoid ambiguities in
	// hard interactions:
	radius += m_dPackingStandoff;
	
	CSimPoint* pSP;
	
	if ((width < 2 * radius) || (height < 2 * radius))
	{
		// Rectangle is as yet too small to accommodate a molecule.
		// Just place one in centre anyway in case user has dragged
		// a couple of pixels by accident instead of clicking on
		// the spot.
		pSP = new CSimPoint;
		pSP->m_dX = left + width / 2;
		pSP->m_dY = bottom + height / 2;
		ptrList->AddTail(pSP);
	}
	else
	{
		// Room for at least one molecule, so fill up rectangle
		// with close-packed mols starting with one fitting just
		// in bottom-left. The bottom row (say row1) has as many
		// mols as possible as they start right at the left of the
		// rectangle; the next row (row2) will be offset to the 
		// right by (radius * cos(60 deg)), and may contain one fewer
		// molecules.
		
		int row1mols = (int) floor(width / (2 * radius));
		double row2offset = 2 * radius * COS60;
		int row2mols = (int) floor((width - 2 * radius * COS60) 
		                               / (2 * radius));
		int rows = 1 + (int) floor((height - 2 * radius) 
		                            / (2 * radius * SIN60));
		
		double centreHeight;
		
		for (int row = 1; row <= rows; row++)
		{	
			centreHeight = bottom + radius + 2 * radius * SIN60 * (row - 1);
			
			if (row & 1) // rows 1, 3 etc
			{
				for (int col = 1; col <= row1mols; col++)
				{
					pSP = new CSimPoint;
					pSP->m_dY = centreHeight;
					pSP->m_dX = left + radius + (col - 1) * 2 * radius;
					ptrList->AddTail(pSP);
				}
			}
			else  
			{
				for (int col = 1; col <= row2mols; col++)
				{
					pSP = new CSimPoint;
					pSP->m_dY = centreHeight;
					pSP->m_dX = left + radius + (col - 1) * 2 * radius + row2offset;
					ptrList->AddTail(pSP);
				}
			}
		}
	}
}
	                           
void CObjTypeMol::OnLButtonUp()
{                       
	BOOL wasDragging = (SM_DRAGGING == m_pSim->m_nDrawingState);
	
	CObjType::OnLButtonUp();

	// We are only interested in one whole button event for drawing
	// molecules, so put state machine back to button one:
	if (SM_AWAITING_CLICK2 == m_pSim->m_nDrawingState)  
	{ 
		m_pSim->m_nDrawingState = SM_AWAITING_CLICK1;
		TRACE("Drawing reset to waiting for click 1 by ObjTypeMol\n");
	}                            
	
	CString command("");   

	int proceed = TRUE;  // may stop drawing if too many mols
	
	// Now create molecule(s) as specified by user drawing
	if (wasDragging)
	{   
		// User dragged a region, either a large region on purpose
		// or a small one by accident if the mouse moved between button
		// down and button up. Creation code will just do single molecule
		// in centre of region if it is less than a molecular size, so
		// doesn't matter.
		TRACE("Creating molecule-filled region\n");
		
		if (m_lNumFillMols > LIM_MAX_FILLMOLS)
		{   
			sprintf(buff, "This will create %ld close-packed molecules, "
			              "which will take some time to separate in simulation. "
			              "Are you sure you want to proceed?", m_lNumFillMols);
			              
			proceed = (IDOK == AfxMessageBox(buff, MB_OKCANCEL));
		}
		 
		if (proceed)
		{
			command = "fillmol \"" + m_sName + "\" ";
			sprintf(buff, "left %g top %g right %g bottom %g ",
			        m_pSim->m_dSimDragStartX, m_pSim->m_dSimDragStartY,
			                  m_pSim->m_dSimMouseX, m_pSim->m_dSimMouseY);
			command += buff;
		}
	}
	else
	{
		// User clicked on one spot without dragging; place single molecule
		// there.
		TRACE("Creating single molecule\n");
		command = "mol \"" + m_sName;
		command += "\" x ";
		sprintf(buff, "%g", m_pSim->m_dSimMouseX);
		command += buff;
		command += " y ";
		sprintf(buff, "%g", m_pSim->m_dSimMouseY);
		command += buff;
	}
	
	// the rest is common to a block or single mols: 
	if (proceed)
	{
		switch (m_nSpeedType)
		{       
			case 0:
				// thermal distrib at system temp
				command += " speed_sys_thermal";
				break;
					
			case 1:
				// thermal distrib at specified temp
				command += " speed_thermal ";
				sprintf(buff, "%g", m_dThermalTemperatureValue);
				command += buff;
				break;
					
			case 2:
				// fixed speed
				command += " speed_fixed ";
				sprintf(buff, "%g", m_dFixedSpeedValue);
				command += buff;
				break; 
					
			default:
				ASSERT(FALSE);
				break;				
		} 
			
		if (0 == m_nDirectionType)
		{
			// use random direction
			command += " direction_random";
		}
		else
		{
			// use specified direction
			command += " direction_fixed ";
			sprintf(buff, "%g", m_dDirectionAngle);
			command += buff;
		}
		
		m_pSim->HandleNewCommand(command);
	}
	else
	{
		// user cancelled when warned about large number of mols
	}
}       


/////////////////////////////////////////////////////////////////////////
// CSObjMol functionality
/////////////////////////////////////////////////////////////////////////
   
CSObjMol::CSObjMol()
{
    m_dX = 0;
	m_dY = 0;
	m_dVx = 0;
	m_dVy = 0; 
	m_dVSquared = 0;
	m_dFx = 0;
	m_dFy = 0;
	m_dAx = 0;
	m_dAy = 0;
	m_dPE = 0;
	m_dVirial = 0;
	m_dImpulseSum = 0;
	m_dDirection = 0;
	m_dCx = 1;
	m_dCy = 0;
	m_dSpeed = 0;
	m_fsValid.all = 0;   
}


// Command list
CMD_ENTRY CSObjMol::m_celCSObjMolCommands[MAX_COMMANDS_MOL] = 
{                      
	{"id",                CA_LONG,   CT_STATE,   CI_ID,                0,      2000000000},                  
	{"x",                 CA_DOUBLE, CT_STATE,   CI_X,                 LIM_MIN_X,  LIM_MAX_X},
	{"y",                 CA_DOUBLE, CT_STATE,   CI_Y,                 LIM_MIN_Y,  LIM_MAX_Y},
	{"vx",                CA_DOUBLE, CT_STATE,   CI_VX,                -LIM_MAX_MOL_SPEED, LIM_MAX_MOL_SPEED},
	{"vy",                CA_DOUBLE, CT_STATE,   CI_VY,                -LIM_MAX_MOL_SPEED, LIM_MAX_MOL_SPEED}, 
	{"speed_fixed",       CA_DOUBLE, CT_CONTROL, CI_SPEED_FIXED,       0,      LIM_MAX_MOL_SPEED},
	{"speed_thermal",     CA_DOUBLE, CT_CONTROL, CI_SPEED_THERMAL,     0,      LIM_MAX_TEMP },
	{"speed_sys_thermal", CA_NONE,   CT_CONTROL, CI_SPEED_SYS_THERMAL, 0,      0    }, 
	{"direction_fixed",   CA_DOUBLE, CT_CONTROL, CI_DIRECTION_FIXED,   -180,   360  },
	{"direction_random",  CA_NONE,   CT_CONTROL, CI_DIRECTION_RANDOM,  0,      0    },
	{"end",               CA_NONE,   CT_ENDLIST, CI_ENDLIST,           0,      0    }
};

// Command handlers                            
int CSObjMol::CmdX()
{
	m_dX = m_dArg; 
	m_fsValid.bf.x = 1;
	return SR_OK;
}

int CSObjMol::CmdY()
{
	m_dY = m_dArg;
	m_fsValid.bf.y = 1;
	return SR_OK;
}

int CSObjMol::CmdVx()
{
	m_dVx = m_dArg;
	m_fsValid.bf.vx = 1;
	return SR_OK;
}

int CSObjMol::CmdVy()
{
	m_dVy = m_dArg;
	m_fsValid.bf.vy = 1;
	return SR_OK;
}

int CSObjMol::CmdSpeedFixed()
{
	m_dSpeed = m_dArg;
	m_fsValid.bf.speed = 1;
	return SR_OK;
}

int CSObjMol::CmdSpeedThermal()
{
	m_dSpeed = m_pSim->ThermalSpeed(m_dArg, ((CObjTypeMol*) m_pType)->m_dMass);
	m_fsValid.bf.speed = 1;
	return SR_OK;
}

int CSObjMol::CmdSpeedSysThermal()
{
	m_dSpeed = m_pSim->ThermalSpeed(m_pSim->m_dTemperature, ((CObjTypeMol*) m_pType)->m_dMass);
	m_fsValid.bf.speed = 1;
	return SR_OK;
}

int CSObjMol::CmdDirectionFixed()
{
	m_dDirection = m_dArg / 180 * PI;
	m_fsValid.bf.direction = 1;
	return SR_OK;
}

int CSObjMol::CmdDirectionRandom()
{
	m_dDirection = m_pSim->RandomDirection();
	m_fsValid.bf.direction = 1;
	return SR_OK;
}

// Overridden from CSimObj
int CSObjMol::AcceptCommandLine(CString& remainingLine, CString& errString)
{
	int command;
	int ret = SR_OK;
	
	m_fsValid.all = 0;   // mark nothing as initialised
    
    ret = GrabCommand(remainingLine, errString, command);            
    while ((CI_EMPTY != command) && (SR_ERROR != ret))
    {
		switch (command)
		{   
			case CI_X:
				ret |= CmdX();
				break;
			
			case CI_Y: 
				ret |= CmdY();
				break;

			case CI_VX:
				ret |= CmdVx();
				break;
			
			case CI_VY: 
				ret |= CmdVy();
				break; 
				
			case CI_SPEED_FIXED:     
				ret |= CmdSpeedFixed();
				break;
				
			case CI_SPEED_THERMAL:
				ret |= CmdSpeedThermal();
				break;
				
			case CI_SPEED_SYS_THERMAL:
				ret |= CmdSpeedSysThermal();
				break;
				
			case CI_DIRECTION_FIXED:
				ret |= CmdDirectionFixed();
				break;
				
			case CI_DIRECTION_RANDOM:
				ret |= CmdDirectionRandom();
				break;
				
			case CI_ID:
				m_lId = m_lArg;
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
		if (!m_fsValid.bf.x)
		{
			// co-ords not init, no good
			ret |= SR_WARN;
			m_dX = 0;      
			m_fsValid.bf.x = 1;
			errString += "supplied 0 as missing x co-ord; ";
		}

		if (!m_fsValid.bf.y)
		{
			// co-ords not init, no good
			ret |= SR_WARN; 
			m_dY = 0;  
			m_fsValid.bf.y = 1;
			errString += "supplied 0 as missing y co-ord; ";
		}
		
		if (!m_fsValid.bf.vx)
		{
			// see if can generate x vel from dirn and speed
			if (m_fsValid.bf.direction && m_fsValid.bf.speed) 
			{
				m_dVx = m_dSpeed * cos(m_dDirection);
				m_fsValid.bf.vx = 1;
			}
			else
			{
				ret |= SR_WARN;
				m_dVx = 0;
				m_fsValid.bf.vx = 1;   
				errString += "inserted 0 as missing x-vel; ";
			}
		}

		if (!m_fsValid.bf.vy)
		{
			// see if can generate x vel from dirn and speed
			if (m_fsValid.bf.direction && m_fsValid.bf.speed) 
			{
				m_dVy = m_dSpeed * sin(m_dDirection);
				m_fsValid.bf.vy = 1;
			}
			else
			{
				ret |= SR_WARN;
				errString += "inserted 0 as missing y-vel; ";
				m_dVy = 0;
				m_fsValid.bf.vy = 1;   
			}
		}        
		
		// calculate cosines and speed from velocities
		m_dSpeed = sqrt(m_dVx * m_dVx + m_dVy * m_dVy); 
		m_fsValid.bf.speed = 1;
		
		if (m_dSpeed > NEARLY_ZERO)
		{
			m_dCx = m_dVx / m_dSpeed;
			m_dCy = m_dVy / m_dSpeed;
		}
		else
		{
			// define an arbitrary direction if stationary
			m_dCx = 1;
			m_dCy = 0;
		}       
		m_fsValid.bf.cosines = 1;

	}              
	
	return ret;
}                        

void CSObjMol::ClipSpeed()
{ 
	ASSERT(m_fsValid.bf.speed = 1);
	ASSERT(m_fsValid.bf.cosines = 1);
	
	if (m_dSpeed > LIM_MAX_MOL_SPEED)
	{
		m_dSpeed = LIM_MAX_MOL_SPEED;
		m_dVx = m_dSpeed * m_dCx;
		m_dVy = m_dSpeed * m_dCy;			
		TRACE("Clipped mol speed to limit of %f\n", (double) LIM_MAX_MOL_SPEED);
	}
}

CMD_ENTRY* CSObjMol::GetCmdList()
{   
	return &(m_celCSObjMolCommands[0]);
}

int CSObjMol::GetStateParam(int command)
{         
	int ret = SR_OK;
	
	switch(command)
	{                                                          
		case CI_ID:
			m_lArg = m_lId;
			break;
			
		case CI_X:
			m_dArg = m_dX;
			break;

		case CI_Y:
			m_dArg = m_dY;
			break;              

		case CI_VX:
			m_dArg = m_dVx;
			break;

		case CI_VY:
			m_dArg = m_dVy;
			break;              
			
		default:
			TRACE("Asked for param for unknown command in CSObjMol\n");
			ret = SR_ERROR;
			break;
	}             
 	
	return ret;
}          
	                                           
int CSObjMol::HitTest(double dXLeft, double dYBottom,
                       double dXRight, double dYTop)
{                                                          
	double dRadius = ((CObjTypeMol*) m_pType)->m_dHSRadius;
	
	return RectCircleOverlap(dXLeft, dYBottom, dXRight, dYTop,
	                          m_dX, m_dY, dRadius);
}

void CSObjMol::ObjectProperties()
{                 
	CSMolPropsDlg dlg; 
	
	ASSERT(m_fsValid.bf.x);
	ASSERT(m_fsValid.bf.y);	
	ASSERT(m_fsValid.bf.vx);
	ASSERT(m_fsValid.bf.vy);
	ASSERT(m_fsValid.bf.speed);
	ASSERT(m_fsValid.bf.cosines);
	
	m_dDirection = atan2(m_dCy, m_dCx);

#ifdef _DEBUG
	m_fsValid.bf.direction = 1;
#endif
    
    dlg.m_dDirection = m_dDirection * 180 / PI;
	dlg.m_dSpeed = m_dSpeed;
	dlg.m_dVx = m_dVx;
	dlg.m_dVy = m_dVy;
	dlg.m_dX = m_dX;
	dlg.m_dY = m_dY;
	dlg.m_dSpecificTemperature = 100;	
	
	dlg.m_pSim = m_pSim;
	dlg.m_pMolType = (CObjTypeMol*) m_pType;
	
	if (dlg.DoModal())
	{
		// Issue command to sim to change our properties
		
		CString cmd("change mol \"");
		
		cmd += m_pType->m_sName;
		cmd += "\" ";
		
		m_dX = dlg.m_dX;
		m_dY = dlg.m_dY;
		m_dVx = dlg.m_dVx;
		m_dVy = dlg.m_dVy;
		
	    WriteState(cmd); 
	    
	    m_pSim->HandleNewCommand(cmd);
	}

}                         

void CSObjMol::CreateTracker()
{
	CSObj::CreateTracker();

	m_pTracker->m_nHandleStyle = TRK_CORNERS_ONLY;
		
	m_pTracker->m_nAllowResize = FALSE; 
}  

void CSObjMol::DrawTracker(CDC* pDC)
{            
	// Need to supply tracker coordinates in device co-ords for comparison
	// to mouse pointer coords
    
    // get centre of mol in logical co-ords
	CPoint point = m_pSim->SimToLogical(m_dX, m_dY);  
	
	int radius = ((CObjTypeMol*) m_pType)->m_nMolScreenRadius;
	int diameter = ((CObjTypeMol*) m_pType)->m_nMolScreenDiameter;
	
    // get rectangle containing mol in logicals
    CRect rect;
	rect.left = point.x - radius;
	rect.top = point.y + radius;
	rect.right = rect.left + diameter;
	rect.bottom = rect.top - diameter;

	// now convert to device co-ords
	pDC->LPtoDP(&rect);
	rect.NormalizeRect();
	
	m_pTracker->m_rect = rect;
	
	m_pTracker->m_nStyle = CRectTracker::resizeOutside;
		
	m_pTracker->Draw(pDC);
}

void CSObjMol::HandleMove(double dDeltaX, double dDeltaY)
{
	m_dX += dDeltaX;
	m_dY += dDeltaY;
}
