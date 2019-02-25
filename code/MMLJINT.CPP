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
#include "lennard.h"
#include "baseint.h"
#include "mol.h"    
#include "mmljint.h"          
#include "editljdg.h"

#include <math.h>

#ifdef _DEBUG
extern long global_debug_counter;
#endif

CMD_ENTRY CLennardJonesInt::m_celCLennardJonesIntCommands[MAX_COMMANDS_PER_OBJ] = 
{                                        
	{"objtype1",        CA_STRING, CT_STATE,   CI_OBJTYPE1,          LIM_MIN_CHARS_OBJTYPE,  LIM_MAX_CHARS_OBJTYPE},
	{"objtype2",        CA_STRING, CT_STATE,   CI_OBJTYPE2,          LIM_MIN_CHARS_OBJTYPE,  LIM_MAX_CHARS_OBJTYPE},
	{"welldepth",       CA_DOUBLE, CT_STATE,   CI_WELLDEPTH,         LIM_MIN_WELL_DEPTH,    LIM_MAX_WELL_DEPTH},
	{"eqm_sepn",        CA_DOUBLE, CT_STATE,   CI_EQM_SEPN,          LIM_MIN_LJ_EQM_SEPN, LIM_MAX_LJ_EQM_SEPN   },
	{"cutoff",          CA_DOUBLE, CT_STATE,   CI_CUTOFF,            LIM_MIN_LJ_CUTOFF,  LIM_MAX_LJ_CUTOFF  }, 
	{"max_pe",          CA_DOUBLE, CT_STATE,   CI_MAX_PE,            LIM_MIN_CUT_PE,    LIM_MAX_CUT_PE},
	{"end",             CA_NONE,   CT_ENDLIST, CI_ENDLIST,           0,    0   }
};
   

CLennardJonesInt::CLennardJonesInt()
{
	m_nBaseType1 = CI_MOL;
	m_nBaseType2 = CI_MOL;

	m_nType1Mobile = TRUE;
	m_nType2Mobile = TRUE;

	m_nIsDefault = FALSE;
	m_nIsNull = FALSE;      
	m_nHasDetails = TRUE;
	m_pFirstNP = NULL;
	m_pLastNP = NULL;
	m_pFirstSpareNP = NULL;
	m_pType1 = NULL;
	m_pType2 = NULL;

	m_sName = "lennard_jones_int";
	m_sDescription = "Molecules attract or repel each other according to how"
	                 " far apart from one another they are; depending on the"
                     " temperature and pressure, they will tend to clump together"
                     " to form solids or liquids.";	              

	m_nIsHard = FALSE;
	m_nIsSoft = TRUE;
	
	m_dCutoffDist = .5;
	m_dEqmSepn = 0.1;
	m_dMaxPE = 500;
	m_dWellDepth = 1;
	
	// Initialise working quantities to arbitrary in-range values:
	m_dSafetySqdIncRadii = 10; // nm2
	m_dCloseLimit = 0.1;       // nm
	m_dCloseLimitSqd = 0.01;   // nm2
	m_dCutoffSqd = 1;          // nm2
	m_dSigma = 0.1;            // nm
	m_dSigma6 = 1e-6;          // nm2
	m_dEta = -1000;            // J/mol 
	m_dFMax = -20000;          // J/mol/nm	

	m_nEditedYet = FALSE;  
}
	
CInt* CLennardJonesInt::Clone()
{
	CInt* pInt = new CLennardJonesInt;
	pInt->m_pSim = m_pSim;
	return pInt;
}

void CLennardJonesInt::ContributeNumerics()
{                 
	m_numCutoffDist.m_pSim = m_pSim;
	m_numCutoffDist.m_sName = "interaction:" + m_pType1->m_sName + ":"
	                            + m_pType2->m_sName + ":cutoff";
	m_numCutoffDist.m_sUnits = "nm";
	m_numCutoffDist.m_sDescription = "cutoff distance";
	m_numCutoffDist.m_pdValue = &m_dCutoffDist; 
	m_numCutoffDist.m_nType = NUM_USERWRITE | NUM_USERREAD;
	m_numCutoffDist.m_dMin = LIM_MIN_LJ_CUTOFF;
	m_numCutoffDist.m_dMax = LIM_MAX_LJ_CUTOFF;
	m_pSim->AcceptNewNumeric(&m_numCutoffDist);

	m_numEqmSepn.m_pSim = m_pSim;
	m_numEqmSepn.m_sName = "interaction:" + m_pType1->m_sName + ":"
	                            + m_pType2->m_sName + ":eqm_sepn";
	m_numEqmSepn.m_sUnits = "nm";
	m_numEqmSepn.m_sDescription = "equilibrium pair separation";
	m_numEqmSepn.m_pdValue = &m_dEqmSepn; 
	m_numEqmSepn.m_nType = NUM_USERWRITE | NUM_USERREAD;
	m_numEqmSepn.m_dMin = LIM_MIN_LJ_CUTOFF;
	m_numEqmSepn.m_dMax = LIM_MAX_LJ_CUTOFF;
	m_pSim->AcceptNewNumeric(&m_numEqmSepn);

	m_numMaxPE.m_pSim = m_pSim;
	m_numMaxPE.m_sName = "interaction:" + m_pType1->m_sName + ":"
	                            + m_pType2->m_sName + ":max_pe";
	m_numMaxPE.m_sUnits = "kJ/mol";
	m_numMaxPE.m_sDescription = "max pair potl energy";
	m_numMaxPE.m_pdValue = &m_dMaxPE; 
	m_numMaxPE.m_nType = NUM_USERWRITE | NUM_USERREAD;
	m_numMaxPE.m_dMin = LIM_MIN_LJ_CUTOFF;
	m_numMaxPE.m_dMax = LIM_MAX_LJ_CUTOFF;
	m_pSim->AcceptNewNumeric(&m_numMaxPE);

	m_numWellDepth.m_pSim = m_pSim;
	m_numWellDepth.m_sName = "interaction:" + m_pType1->m_sName + ":"
	                            + m_pType2->m_sName + ":welldepth";
	m_numWellDepth.m_sUnits = "kJ/mol";
	m_numWellDepth.m_sDescription = "potential energy well depth";
	m_numWellDepth.m_pdValue = &m_dWellDepth; 
	m_numWellDepth.m_nType = NUM_USERWRITE | NUM_USERREAD;
	m_numWellDepth.m_dMin = LIM_MIN_LJ_CUTOFF;
	m_numWellDepth.m_dMax = LIM_MAX_LJ_CUTOFF;
	m_pSim->AcceptNewNumeric(&m_numWellDepth);
}

int CLennardJonesInt::EditDetails()
{                     
	CEditLJDetailsDlg dlg;
	int ret;

	dlg.m_dCutoffDist = m_dCutoffDist;
	dlg.m_dMaxPE = m_dMaxPE;
	dlg.m_dWellDepth = m_dWellDepth;
	
	if (m_nEditedYet)
	{
		dlg.m_dEqmSepn = m_dEqmSepn;
	}	
	else
	{
		// initialise to use sum of mol radii as default eqm sepn
		
        dlg.m_dEqmSepn = ((CObjTypeMol*) m_pType1)->m_dHSRadius +
	                     ((CObjTypeMol*) m_pType2)->m_dHSRadius; 
	}
	
	ret = dlg.DoModal(); 
	
	if (IDOK == ret)
	{
		m_dCutoffDist = dlg.m_dCutoffDist;
		m_dEqmSepn = dlg.m_dEqmSepn;
		m_dMaxPE = dlg.m_dMaxPE;
		m_dWellDepth = dlg.m_dWellDepth;  
		m_nEditedYet = TRUE;

        CString cmd = "interaction " + m_sName + " ";    
		WriteState(cmd);
		m_pSim->HandleNewCommand(cmd);
	}
	
	return ret;
}

void CLennardJonesInt::BuildNeighList()
{ 
	CNP* pNP; 

	// This process can take a while, especially the very first time round for
	// a big block of molecules when all the CNP objects have to be created rather
	// than reused (for a specific example in a debug build, took about 11 sec to
	// create about 30000 CNP objs first time round, only about 400 ms to reuse
	// a similar number at the next rebuild). So if seems to be taking some time,
	// will set the cursor to the waiting version:
	DWORD tStart = GetTickCount();
	long lConsiderTimeCounter = 0; 
	int nWaitCursorSet = FALSE;                   

	
#ifdef _DEBUG
	global_debug_counter++;
#endif
	
	pNP = m_pFirstSpareNP;     // remember beginning of old spare list    
	if (NULL != m_pFirstNP)
	{
		m_pFirstSpareNP = m_pFirstNP; // put existing NP list in place of spares
		m_pLastNP->m_pNextNP = pNP;   // tack old spare list on end
	}
	m_pFirstNP = NULL;  // NP list is now empty
	m_pLastNP = NULL;            
	
	CNP* pLastNP = NULL;        
	
	// For this interaction, need to go through all combinations of
	// pairs of molecules checking for those lying within the safety distance.
	// We avoid checking pairs twice in the case where this interaction
	// is a self-interaction of one type.
	
	CObjTypeMol* pMolType1 = (CObjTypeMol*) m_pType1;
	CObjTypeMol* pMolType2 = (CObjTypeMol*) m_pType2;

	int nIsSelfInt = (m_pType1 == m_pType2);
	

	m_dSafetySqdIncRadii = m_pSim->m_dSafetyDist + m_dCutoffDist;
	m_dSafetySqdIncRadii *= m_dSafetySqdIncRadii; 
	
	// calculate some other quantities once here that we shall need many
	// times in force calculations
	m_dCutoffSqd = m_dCutoffDist * m_dCutoffDist;
	m_dSigma = m_dEqmSepn / TWO_TO_THE_ONE_SIXTH;  // Atkins 2nd ed p785
	m_dSigma6 = m_dSigma * m_dSigma * m_dSigma; // cubed
	m_dSigma6 = m_dSigma6 * m_dSigma6;          // 6th power
	
	double dWellDepthkJ = m_dWellDepth;
	if (fabs(dWellDepthkJ) < NEARLY_ZERO)
	{
		dWellDepthkJ = 0.00001;
	}               

	m_dEta = dWellDepthkJ * 1000;
	
		
	// We shall not allow the potential energy in a pair interaction to be
	// higher than m_dMaxPE, yet the LJ potential becomes infinite at a
	// separation r=0. We divert from the LJ curve, taking a straight line
	// up to m_dMaxPE at r=0 instead. We must calculate what the gradient
	// of that line is (the max force) and where it meets the LJ curve,
	// matching for value (potential V) and gradient (force F).
	//
	//      V(LJ) = 4 eta [(sigma/r)^12 - (sigma/r)^6]
	//
	//      V(straight line) = Vmax + Fmax.r
	// 
	// Equating these potentials (Vmax == m_dMaxPE, Fmax == m_dFMax),
	//
	//      Vmax + Fmax.r = 4 eta [(sigma/r)^12 - (sigma/r)^6]   (1)
	//
	// Now considering the gradients, 
	//
	//      F(LJ) = (-4 eta / r) [12(sigma/r)^12 - 6(sigma/r)^6]
	//
	//      F(straight line) = Fmax  (-ve)
	//
	// Equating these,
	//
	//      Fmax = (-4 eta / r) [12(sigma/r)^12 - 6(sigma/r)^6]
	//
	// Substituting this into (1),  
	//
	//     Vmax - r (4 eta / r) [12(sigma/r)^12 - 6(sigma/r)^6] =
	//                           4 eta [(sigma/r)^12 - (sigma/r)^6]
	//
	// Replacing (sigma/r)^6 with x and cancelling,
	//
	//     Vmax/4.eta - (12 x^2 - 6 x) = x^2 - x
	//
	//     -13 x^2  + 7 x  + Vmax/4eta = 0
	//
	// Which is in the standard quadratic form with a=-13, b=7, c=Vmax/4eta.
	// We therefore have x and hence the separation r at which both the
	// potential and gradient are matched, and we can calculate the
	// force Fmax there that we shall apply within that radius.

	
	double x = (-7 - sqrt(49 + 13 * m_dMaxPE / dWellDepthkJ)) / -26;
	double sigma_over_rmin = pow(x, ONE_SIXTH);
	m_dCloseLimit = m_dSigma / sigma_over_rmin;
	m_dCloseLimitSqd = m_dCloseLimit * m_dCloseLimit; 	      

	// J/mol/nm = J/mol    /   nm         *   (unitless)
	m_dFMax = -4 * m_dEta / m_dCloseLimit * (12 * x * x - 6 * x);
	            
	
	
	// loop over all possible pairs of mols of the two types		
	CSObjMol* pMol1 = (CSObjMol*) pMolType1->m_pFirstSObj;
	CSObjMol* pMol2;
	
	while (NULL != pMol1)
	{                             
		if (nIsSelfInt)
		{   
			// doing self interaction, if this is mol (i) then
			// start from mol (i+1)
			pMol2 = (CSObjMol*) pMol1->m_pNextSObj;
		}
		else
		{    
			// doing heterogeneous interaction, cover all pairs
			pMol2 = (CSObjMol*) pMolType2->m_pFirstSObj;
		}    
		
		while (NULL != pMol2)
		{                                
			lConsiderTimeCounter++;
       		
       		double dx = pMol1->m_dX - pMol2->m_dX;
			double dy = pMol1->m_dY - pMol2->m_dY;
			
			if (dx * dx + dy * dy <= m_dSafetySqdIncRadii)
			{
				// this pair within safety distance of each other, add
				// to list of neigbour pairs
				if (NULL != m_pFirstSpareNP)     
				{
					// reuse an old neighbour pair object
					pNP = m_pFirstSpareNP;
					m_pFirstSpareNP = pNP->m_pNextNP;
					ASSERT(NR_NOT_IN_TREE == pNP->m_nParentRelationship);
				}
				else
				{
					// have to make a new one
					pNP = new CNP(this);
					pNP->m_handle1.m_pNP = pNP;
					pNP->m_handle2.m_pNP = pNP;
				}   

				pNP->m_pEarlierNode = NULL;
				pNP->m_pLaterNode = NULL;   
				pNP->m_pParentNode = NULL;
				pNP->m_nParentRelationship = NR_NOT_IN_TREE;

                pNP->m_pNextNP = NULL;
                pNP->m_pLastNP = NULL;

				pNP->m_pSObj1 = pMol1;
				pNP->m_pSObj2 = pMol2;

				// As this interaction is soft, mol movements will not be
				// recalcd in the event of a hard collision, so don't add
				// NP to linked list of handles attached to mol. 
				// But when monitors introduced, may have to attach handle
				// so that a destroyed mol can remove its CNPs.
				
				if (pLastNP != NULL)
				{
					pLastNP->m_pNextNP = pNP;
				}
				else
				{
					m_pFirstNP = pNP;
				}

				pLastNP = pNP;
			}  // end of if (close to each other)
			
			pMol2 = (CSObjMol*) pMol2->m_pNextSObj;
		}  // loop over mols of type 2


		if ((lConsiderTimeCounter > 1000) && !nWaitCursorSet)
		{
			if (GetTickCount() - tStart > 1000)
			{
				theApp.BeginWaitCursor();
				nWaitCursorSet = TRUE;
			}
			
			lConsiderTimeCounter = 0;
		}	
		
		pMol1 = (CSObjMol*) pMol1->m_pNextSObj;
	}  // loop over mols of type 1
	
	// terminate linked list
	if (NULL != pLastNP)
	{
		pLastNP->m_pNextNP = NULL; 
		m_pLastNP = pLastNP;
	}
	else
	{
		m_pLastNP = NULL;
	}

    if (nWaitCursorSet)
    {
    	theApp.EndWaitCursor();
    }
}

int CLennardJonesInt::AcceptCommandLine(CString& remainingLine, CString& errString)
{
	int command;
	int ret;                                   
	CObjType* pType;
    
    ret = GrabCommand(remainingLine, errString, command);            
    while ((CI_EMPTY != command) && (SR_ERROR != ret))
    {
		switch (command)
		{     
			case CI_OBJTYPE1:
			case CI_OBJTYPE2:

				// check given name against current objects and set object
				// pointer as reqd  
				pType = m_pSim->GetObjTypeByName(m_sArg);
				
				if (NULL != pType)
				{
					if ((m_nIsNull || (pType->m_nBaseType == m_nBaseType1)) && (NULL == m_pType1))
					{
						// OK for our type 1  
						m_pType1 = pType;
					}
					else if ((m_nIsNull || (pType->m_nBaseType == m_nBaseType2)) && (NULL == m_pType2))
					{
						// OK for our type 2 
						m_pType2 = pType;
					}
					else
					{
						// we can't accept an object of that type   
						ret |= SR_ERROR;   
						errString += "can't use this type for this interaction:  "
						               + m_sArg;
					}
				}
				else
				{
					// no such type
					ret |= SR_ERROR;
					errString += "no such object type: " + m_sArg;
				}
				
				break;
				      
			case CI_CUTOFF:
				m_dCutoffDist = m_dArg;
				break;
				
			case CI_EQM_SEPN:
				m_dEqmSepn = m_dArg;
				break;
				
			case CI_MAX_PE:
				m_dMaxPE = m_dArg;
				break;
				
			case CI_WELLDEPTH:
				m_dWellDepth = m_dArg;
				break;
				
			default:
				ret |= SR_ERROR;
				errString += "confused near: " + remainingLine;
				break;
		}
		
        ret |= GrabCommand(remainingLine, errString, command);
    
	}              
	
	if ((NULL == m_pType1) || (NULL == m_pType2))
	{
		ret |= SR_ERROR;
		errString += "need both types specified for interaction";
	}    
	else
	{
		// get the types in correct order
		int type1 = m_pSim->GetObjTypeIndexByName(m_pType1->m_sName);
		int type2 = m_pSim->GetObjTypeIndexByName(m_pType2->m_sName);
			
		if (type2 < type1)
		{
			CObjType* pSpareType = m_pType1;
			m_pType1 = m_pType2;
			m_pType2 = pSpareType;
		}
	}
	  
	return ret;
}

CMD_ENTRY* CLennardJonesInt::GetCmdList()
{
	return &(m_celCLennardJonesIntCommands[0]);
}

int CLennardJonesInt::GetStateParam(int command)
{
	int ret = SR_OK;
	
	switch(command)
	{               
		case CI_OBJTYPE1:
			m_sArg = m_pType1->m_sName;
			break;
			
		case CI_OBJTYPE2:
			m_sArg = m_pType2->m_sName;
			break;    
			
		case CI_WELLDEPTH:
			m_dArg = m_dWellDepth;
			break;
			
		case CI_EQM_SEPN: 
			m_dArg = m_dEqmSepn;
			break;
			
		case CI_CUTOFF:
			m_dArg = m_dCutoffDist;
			break;
			
		case CI_MAX_PE:
			m_dArg = m_dMaxPE;
			break;
			
		default:        
			TRACE("Asked for param for unknown command in CInt::GetStateParam\n");
			ret = SR_ERROR;
			break;
	}             
	
	return ret;
}                    

void CLennardJonesInt::CalcForces()
{          
	CObjTypeMol* pType1 = (CObjTypeMol*) m_pType1;
	CObjTypeMol* pType2 = (CObjTypeMol*) m_pType2;
	
	CSObjMol* pMol1;
	CSObjMol* pMol2;
	
	
	// Go through all neighbour pairs and calculate the forces exerted
	// on each molecule.
	                      
	CNP* pNP = m_pFirstNP;
	double dt_over_2 = m_pSim->m_dDeltaTime / 2; // for leapfrogging integration
			
	while (NULL != pNP)
	{ 
		pMol1 = (CSObjMol*) pNP->m_pSObj1;		     						
		pMol2 = (CSObjMol*) pNP->m_pSObj2;		     						

        /* 
         * Calculate acceleration increment for each molecule
         * suffering an interaction using Lennard-Jones potential
         *
         * V = 4 eta [(sigma/r)^12 - (sigma/r)^6]
         *
         * Fx = -dV/dx
         *
         * Fr = -dV/dr          r for radial, dr = radial sepn dist
         *    =  -4 eta [-12 sigma^12 / r^13 + 6 sigma^6 / r^7]
         *
         * Fx = Fr cos theta 
         *
         * cos theta = x / r 
         *
         * Fx = (x / r) Fr
         *    = -4 eta  x (1/r^2)[-12 sigma^12 / r^12 + 6 sigma^6 / r^6] 
         *    = eta . x (1/r^2)[48 (sigma/r)^12 - 24 (sigma/r) ^6]
         *
         * similarly for Fy
         */
         
		// Should do leapfrog integration, with velocities calculated
		// half a timestep out of phase with position and acceleration
		// (see Feynman Lectures on Physics vol 1); however, the algebra
		// is identical extrapolating the velocity and position using the
		// acceleration calculated at the current position, so this is no
		// less stable numerically; it is just that the small difference in
		// initial conditions (approximating v0 = v(0 + dt/2)) is ignored.
		// Did spreadsheet with simple extrapolation like this, leapfrog
		// integration, or third regime in which accelerations calculated
		// at extrapolated x (t + dt/2); latter was disaster, didn't conserve
		// energy, mols just clump together and stop. This scheme and leapfrog
		// go divergent at same dt, which goes first depends on luck (ie first
		// to get big accel?).
         
        double dx = pMol2->m_dX - pMol1->m_dX;  // nm
                     
        double dy = pMol2->m_dY - pMol1->m_dY;  // nm
        
        // get intermol dist in nm^2
        double sepn_2 = dx * dx + dy * dy;
        
        double fx, fy;  // force components    
        double pe_over_2; // potential energy
        double virial_over_2; // pressure contribution, explained below
        
		if (sepn_2 < m_dCutoffSqd)
        {   
	        if (sepn_2 >= m_dCloseLimitSqd)
	        {   
	        	// Mols are within interaction range but not so close as to divert
	        	// from the proper LJ interaction:
	        	
     		    double sepn_6 = sepn_2 * sepn_2 * sepn_2; // nm6
		        
		        double sigma_by_r_6 = m_dSigma6 / sepn_6; // unitless
		        
		        double sigma_by_r_12 = sigma_by_r_6 * sigma_by_r_6; // unitless
		        
		        // J/mol/(nm2)
		        double f_over_r = -m_dEta / sepn_2 * (48 * sigma_by_r_12 - 24 * sigma_by_r_6);
		        
		        // J/mol/nm
		        fx = f_over_r * dx;
		        fy = f_over_r * dy;
		        
		        // Pressure and PE figures only really required for monitored
				// mols, though doing for all here:
					
				// Add virial contributions for pressure measurement
				// See Art of Molecular Dynamics Simulation p20 for formulae			
					
				//        J/mol      =   J/mol/(nm2)   * (nm2)
				virial_over_2 = -f_over_r * sepn_2 / 2;

					
				// Divvy out potential energy between the two molecules
					
				pe_over_2 =  2 * m_dEta * (sigma_by_r_12 - sigma_by_r_6);       
					
			}            
			else
			{
				// Mols are sufficiently close together that we use straight-line
				// PE vs r terminating at m_dMaxPE at r=0, with constant force
				
				double cx, cy;  // cosines of separation vector
				            
	            double r;  // needed unfortunately
	
				if (sepn_2 > NEARLY_ZERO)
				{
					r = sqrt(sepn_2);
	
					cx = dx / r;
					cy = dy / r;
				}
				else
				{
					r = 0;
					
					cx = SQRT_2_OVER_2; // arbitrary direction, not likely to get stuck
					cy = SQRT_2_OVER_2; // against horizontal or vertical walls
				}
				
				
				// force is fixed at max	        	
	     		fx = m_dFMax * cx;
		        fy = m_dFMax * cy;
			    
				// PE goes down linearly from max at r=0 with gradient m_dFMax
				                                              
				// J/mol  =     (unitless)                * kJ/mol   * 1000
				pe_over_2 = (m_dMaxPE * 1000 + m_dFMax * r) / 2;
			    
				virial_over_2 = -m_dFMax * r / 2;			                                                                  
			}

			pMol1->m_dFx += fx;
			pMol1->m_dFy += fy;
			pMol2->m_dFx -= fx;
			pMol2->m_dFy -= fy;	
						                                                       
			pMol1->m_dVirial += virial_over_2;
			pMol2->m_dVirial += virial_over_2;

			pMol1->m_dPE += pe_over_2;
			pMol2->m_dPE += pe_over_2;
        }   
        else
        {
        	// mols are outside of cutoff distance, forces etc are zero
        }

		pNP = pNP->m_pNextNP;
	}

}
	                

