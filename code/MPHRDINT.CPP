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
#include "baseint.h"
#include "mol.h"     
#include "wall.h"
#include "piston.h"  
#include "mphrdint.h"          

#include <math.h>

#ifdef _DEBUG
extern long global_debug_counter;
#endif


CMolPistonHardInt::CMolPistonHardInt()
{                   
	m_nBaseType1 = MIN(CI_MOL, CI_PISTON);
	m_nBaseType2 = MAX(CI_MOL, CI_PISTON);

	m_nType1Mobile = TRUE;
	m_nType2Mobile = TRUE;

	m_nIsDefault = TRUE;
	m_nIsNull = FALSE;      
	m_nHasDetails = FALSE;
	m_pFirstNP = NULL;
	m_pLastNP = NULL;
	m_pFirstSpareNP = NULL;
	m_pType1 = NULL;
	m_pType2 = NULL;
	m_sName = "piston_mol_hard_int";
	m_sDescription = "Molecules bounce off pistons like billiard balls"
	                 " with a change in velocity specified by the pison wall properties"
	                 " and the pistons recoil along their allowed direction of"
	                 " movement to preserve linear momentum in that direction";

	m_nIsHard = TRUE;
	m_nIsSoft = FALSE;
}
	
CInt* CMolPistonHardInt::Clone()
{
	CInt* pInt = new CMolPistonHardInt;
	pInt->m_pSim = m_pSim;
	return pInt;
}

void CMolPistonHardInt::BuildNeighList()
{
	// We are required to rebuild the list of pairs of objects that may interact
	// soon.  
	
	// We recycle old neighbour pairs as much as possible to avoid the overhead
	// of creating new ones. NB Not sure yet if the list of neighbour pairs needs
	// to be singly or doubly linked.
	// Snip the list of existing neighbour pair objects off and glue it onto
	// the spare list for reuse:

	CNP* pNP; 
	
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
	
	
	// For this heterogeneous interaction, need to go through all combinations of
	// pairs of the type of piston and molecule it belongs to checking for those
	// lying within the safety distance.
	
#if (CI_MOL < CI_PISTON)
	CObjTypeMol* pMolType = (CObjTypeMol*) m_pType1;
	CObjTypePiston* pPistonType = (CObjTypePiston*) m_pType2;
#else
	CObjTypeMol* pMolType = (CObjTypeMol*) m_pType2;
	CObjTypePiston* pPistonType = (CObjTypePiston*) m_pType1;
#endif                                                    

	CSObjMol* pMol = (CSObjMol*) pMolType->m_pFirstSObj;
	CSObjPiston* pPiston;
    
	
	m_dMolRadiusIncSafety = pMolType->m_dHSRadius + m_pSim->m_dSafetyDist;
	
	// loop over all possible pairs of mols and pistons of this type		
	while (NULL != pMol)
	{                
		pPiston = (CSObjPiston*) pPistonType->m_pFirstSObj;   
		while (NULL != pPiston)
		{
		                      
#ifdef _DEBUG
			global_debug_counter++;
#endif  
            
            // Need only check if centre of molecule lies within a rectangle
            // about the piston inflated by the safety distance plus the molecular
            // radius. This is a bit conservative round the corners, but is very
            // quick and easy to calculate.
            
            if ((pMol->m_dX >= pPiston->m_dXLeft - m_dMolRadiusIncSafety)
               && (pMol->m_dX <= pPiston->m_dXRight + m_dMolRadiusIncSafety)
               && (pMol->m_dY >= pPiston->m_dYBottom - m_dMolRadiusIncSafety)
               && (pMol->m_dY <= pPiston->m_dYTop + m_dMolRadiusIncSafety))
			{
				// The separation between the edge of the molecule and
				// the piston is sufficiently small that they might hit,
				// so add this pair of objects to list. (They might already
				// be overlapping.)    
				
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

#ifdef _DEBUG					
					m_lNumNP++;
#endif
				}   

				pNP->m_pEarlierNode = NULL;
				pNP->m_pLaterNode = NULL;   
				pNP->m_pParentNode = NULL;
				pNP->m_nParentRelationship = NR_NOT_IN_TREE;

                pNP->m_pNextNP = NULL;
                pNP->m_pLastNP = NULL;

#if (CI_MOL < CI_WALL)
				pNP->m_pSObj1 = pMol;
				pNP->m_pSObj2 = pPiston;
#else				
				pNP->m_pSObj2 = pMol;
				pNP->m_pSObj1 = pPiston;
#endif
                
		   		// add this NP to linked list attached to molecule and piston
		   		
#if (CI_MOL < CI_PISTON)
				pNP->m_handle1.m_pNextHandle = pMol->m_pFirstHandle;
				pNP->m_handle2.m_pNextHandle = pPiston->m_pFirstHandle;
				pMol->m_pFirstHandle = &(pNP->m_handle1);
				pPiston->m_pFirstHandle = &(pNP->m_handle2);
#else				
				pNP->m_handle2.m_pNextHandle = pMol->m_pFirstHandle;
				pNP->m_handle1.m_pNextHandle = pPiston->m_pFirstHandle;
				pMol->m_pFirstHandle = &(pNP->m_handle2);
				pPiston->m_pFirstHandle = &(pNP->m_handle1);
#endif


				if (pLastNP != NULL)
				{
					pLastNP->m_pNextNP = pNP;
				}
				else
				{
					m_pFirstNP = pNP;
				}

				pLastNP = pNP;
				
			}
					
			
			pPiston = (CSObjPiston*) pPiston->m_pNextSObj;
		} // loop over pistons                          
		
		pMol = (CSObjMol*) pMol->m_pNextSObj;
	} // loop over molecules 
	                          
	                          
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
}

void CMolPistonHardInt::CalcAllHardEvents()
{
#ifdef _DEBUG
	global_debug_counter++;
#endif

    
	// Go through all neighbour pairs and calculate when each 
	// pair of objects will collide, if at all, within this timestep.
	                          
	CNP* pNP = m_pFirstNP; 
		
	while (NULL != pNP)
	{ 
		CalcHardEvent(pNP);		     						

		pNP = pNP->m_pNextNP;
	}
}

void CMolPistonHardInt::CalcHardEvent(CNP* pNP)
{

#ifdef _DEBUG
	global_debug_counter++;
#endif
	
	CSObjPiston* pPiston;
	CSObjMol* pMol;     

#if (CI_MOL < CI_PISTON)
	CObjTypeMol* pMolType = (CObjTypeMol*) m_pType1;
	CObjTypePiston* pPistonType = (CObjTypePiston*) m_pType2;
#else
	CObjTypeMol* pMolType = (CObjTypeMol*) m_pType2;
	CObjTypePiston* pPistonType = (CObjTypePiston*) m_pType1;
#endif                                                    

#if (CI_MOL < CI_WALL)
	pMol = (CSObjMol*) pNP->m_pSObj1;
	pPiston = (CSObjPiston*) pNP->m_pSObj2;
#else
	pMol = (CSObjMol*) pNP->m_pSObj2;
	pPiston = (CSObjPiston*) pNP->m_pSObj1;
#endif


	// Mol hits piston when the distance between them is equal
	// to the molecular radius. The mol may hit any of the faces of
	// the piston or any of the corners.
	
	// t will be the time of the first bit of piston the mol will hit
	double t = NEARLY_INFINITY;       
	int nHint = PT_NO_HIT;
	
	ASSERT(pMol->m_fsValid.bf.x);
	ASSERT(pMol->m_fsValid.bf.y);
	ASSERT(pMol->m_fsValid.bf.vx);
	ASSERT(pMol->m_fsValid.bf.vy);   
	
    double t0;  // time we place mol and piston at, later of their individual times
	double dt;  // time difference between them at the moment
	
	double x, y;   // corrected mol coordinates
	double xl, xr, yt, yb;  // corrected piston coordinates
	double dx, dy;        // amount piston might need to move to catch up
	    
    if (pMol->m_nTouched)
    {
    	if (pPiston->m_nTouched)
    	{
    		// Both have already moved this timestep. Make
    		// the one that is further behind catch up the one
    		// that is ahead.       
    		dt = pMol->m_dTimeDoneAlready - pPiston->m_dTimeDoneAlready;
    		
    		if (dt > 0)
    		{
    			// mol ahead, move piston       
    			x = pMol->m_dX;
    			y = pMol->m_dY;
    			dx = pPiston->m_dVx * dt;
    			dy = pPiston->m_dVy * dt;
    			xl = pPiston->m_dXLeft + dx;
    			xr = pPiston->m_dXRight + dx;
    			yt = pPiston->m_dYTop + dy;
    			yb = pPiston->m_dYBottom + dy;
    			
    			t0 = pMol->m_dTimeDoneAlready;     
    		}
    		else
    		{
    			// piston ahead (or dead heat), move mol along to match
		    	x = pMol->m_dX - pMol->m_dVx * dt;
		    	y = pMol->m_dY - pMol->m_dVy * dt;
    			xl = pPiston->m_dXLeft;
    			xr = pPiston->m_dXRight;
    			yt = pPiston->m_dYTop;
    			yb = pPiston->m_dYBottom;

    			t0 = pPiston->m_dTimeDoneAlready;
		    }   
		}
		else
		{
			// mol has moved but piston hasn't
			x = pMol->m_dX;
			y = pMol->m_dY;
			dt = pMol->m_dTimeDoneAlready;
			dx = pPiston->m_dVx * dt;
			dy = pPiston->m_dVy * dt;
			xl = pPiston->m_dXLeft + dx;
			xr = pPiston->m_dXRight + dx;
			yt = pPiston->m_dYTop + dy;
			yb = pPiston->m_dYBottom + dy;
    			
			t0 = pMol->m_dTimeDoneAlready;     
    	}
    }
    else
    {
    	// mol hasn't yet moved this timestep
		if (pPiston->m_nTouched)
		{
			// but piston has
			dt = pPiston->m_dTimeDoneAlready;
	    	x = pMol->m_dX + pMol->m_dVx * dt;
	    	y = pMol->m_dY + pMol->m_dVy * dt;
			xl = pPiston->m_dXLeft;
			xr = pPiston->m_dXRight;
			yt = pPiston->m_dYTop;
			yb = pPiston->m_dYBottom;

			t0 = pPiston->m_dTimeDoneAlready;
    	}                          
    	else
    	{
    		// neither has yet moved
    		t0 = 0; 
    		x = pMol->m_dX;
    		y = pMol->m_dY; 
			xl = pPiston->m_dXLeft;
			xr = pPiston->m_dXRight;
			yt = pPiston->m_dYTop;
			yb = pPiston->m_dYBottom;
    	}
    }

	// Get vel of mol rel to piston
    	
	double dvx = pMol->m_dVx - pPiston->m_dVx;
	double dvy = pMol->m_dVy - pPiston->m_dVy;
    	
	// and rel coordinates
    	
	double dxl = x - xl;
	double dxr = x - xr;
	double dyt = y - yt;
	double dyb = y - yb;
	
	double molRadius = pMolType->m_dHSRadius;
    double molRadiusSqd = molRadius * molRadius;

    
    // First check to see if molecule is overlapping piston already. If its centre is
    // already inside piston, let it drift on so that it escapes. But if not, then do
    // a collision as if it were a smaller molecule just touching the piston. This is
    // so that if molecules are created overlapping the piston edges (due to reactive
    // interactions that make bigger molecules than the reactants, or because of
    // careless user placement) the molecules will still be nicely contained.
    // Have to check all of the faces and corners. If no overlaps, then go on to
    // calculate whether molecule will collide with piston.

    if ((dxl >= 0) && (dxr <= 0))
    {
        // molecule is potentially overlapping top or bottom edge of piston already

        if ((dyb >= -molRadius) && (dyt <= molRadius))
        {
            if (dyb < 0)
            {
                // but its centre is still outside of piston below centre, so do collision
                // if moving upwards rel to piston
                if (dvy > 0)
                {
                    // Make collision occur in a little while; otherwise, if trapped between
                    // two pistons, would collide an infinite number of times (if set t=0)
                    nHint = PT_HITS_BOTTOM_FACE;
                    t = m_pSim->m_dDeltaTime * TIMESTEP_FRACTION_FOR_OVERLAP_COLLN;
                }
            }
            else if (dyt > 0)
            {
                // centre of mol above top face of piston, so do collision if moving down
                // towards piston
                if (dvy < 0)
                {
                    nHint = PT_HITS_TOP_FACE;
                    t = m_pSim->m_dDeltaTime * TIMESTEP_FRACTION_FOR_OVERLAP_COLLN;
                }
            }
            else
            {
                // centre of mol already overlapping piston, let it drift through
                nHint = PT_OVERLAPS;
            }
        }
    }
    else if (dyb >= 0 && dyt <= 0)
    {
        // molecule is potentially overlapping left or right face of piston already, all
        // similar to above cases

        if ((dxl >= -molRadius) && (dxr <= molRadius))
        {
            if (dxl < 0)
            {
                if (dvx > 0)
                {
                    nHint = PT_HITS_LEFT_FACE;
                    t = m_pSim->m_dDeltaTime * TIMESTEP_FRACTION_FOR_OVERLAP_COLLN;
                }
            }
            else if (dxr > 0)
            {
                // centre of mol above top face of piston, so do collision if moving down
                // towards piston
                if (dvx < 0)
                {
                    nHint = PT_HITS_RIGHT_FACE;
                    t = m_pSim->m_dDeltaTime * TIMESTEP_FRACTION_FOR_OVERLAP_COLLN;
                }
            }
            else
            {
                // centre of mol already overlapping piston, let it drift through
                nHint = PT_OVERLAPS;
            }
        }
    }
    else if (dxl > -molRadius && dxl < 0 && dyt < molRadius && dyt > 0 && dvx > 0 && dvy < 0)
    {
        // mol is overlapping top-left corner, centre is still outside piston and vel vector
        // is towards piston corner
        nHint = PT_HITS_TOP_LEFT_CORNER;
        t = m_pSim->m_dDeltaTime * TIMESTEP_FRACTION_FOR_OVERLAP_COLLN;
    }
    else if (dxl > -molRadius && dxl < 0 && dyb > -molRadius && dyb < 0 && dvx > 0 && dvy > 0)
    {
        // similarly for bottom left corner
        nHint = PT_HITS_BOTTOM_LEFT_CORNER;
        t = m_pSim->m_dDeltaTime * TIMESTEP_FRACTION_FOR_OVERLAP_COLLN;
    }
    else if (dxr < molRadius && dxr > 0 && dyt < molRadius && dyt > 0 && dvx < 0 && dvy < 0)
    {
        // similarly for top right corner
        nHint = PT_HITS_TOP_RIGHT_CORNER;
        t = m_pSim->m_dDeltaTime * TIMESTEP_FRACTION_FOR_OVERLAP_COLLN;
    }
    else if (dxr < molRadius && dxr > 0 && dyb > -molRadius && dyb < 0 && dvx < 0 && dvy > 0)
    {
        // similarly for bottom right corner
        nHint = PT_HITS_BOTTOM_RIGHT_CORNER;
        t = m_pSim->m_dDeltaTime * TIMESTEP_FRACTION_FOR_OVERLAP_COLLN;
    }
    else
    {
        // piston and molecule are not overlapping
    }

    if (PT_NO_HIT == nHint)
    {    
    	// Calculate when (if at all) the mol will overlap a piston face, will do corners later
  	    	
    	if ((dvx > NEARLY_ZERO) || (dvx < -NEARLY_ZERO))
    	{
    		// calc when would hit left or right faces
    		
    		// calc when would hit left face if it were infinite in extent;
    		// this happens when x-vector from left face of wall, dxl = -molRadius
    		double tLeft = -(molRadius + dxl) / dvx;
    		
    		// see if will hit finite length of face at that time; centre of
    		// mol will be lower than piston top and above piston bottom
    		// at that time if so:
    		if ((dyt + tLeft * dvy <= 0) && (dyb + tLeft * dvy >= 0))
    		{
    			// it will hit face
    			t = tLeft;
    			nHint = PT_HITS_LEFT_FACE;
    		}
    		
    		// Now do similarly for the other three faces to find out which 
    		// it will hit first:
    		
    		double tRight = (molRadius - dxr) / dvx;
    		if ((dyt + tRight * dvy <= 0) && (dyb + tRight * dvy >= 0))
    		{
    			if (tRight < t)
    			{
	    			t = tRight;
	    			nHint = PT_HITS_RIGHT_FACE;
    			}
    		}

		}
		
    	if ((dvy > NEARLY_ZERO) || (dvy < -NEARLY_ZERO))
    	{
    		double tTop = (molRadius - dyt) / dvy;
    		if ((dxr + tTop * dvx <= 0) && (dxl + tTop * dvx >= 0))
    		{
    			if (tTop < t)
    			{
	    			t = tTop;
	    			nHint = PT_HITS_TOP_FACE;
    			}
    		}

    		double tBottom = -(molRadius + dyb) / dvy;
    		if ((dxr + tBottom * dvx <= 0) && (dxl + tBottom * dvx >= 0))
    		{
    			if (tBottom < t)
    			{
	    			t = tBottom;
	    			nHint = PT_HITS_BOTTOM_FACE;
    			}
    		}    		
    	}

    
    
        // Now check about hitting any one of the four corners, very similar
        // to hitting another molecule except the latter here has zero radius.

	    // Sadly the distance between the mol and a corner is
	    // quadratic in time, so we have to do a square root. (I think.) 
	    // Hit when separation between centres 
	    // equals mol radius.     
	    // dx = x(mol) - x(piston) etc
	    // r^2 = (dx + dvx.t)^2 + (dy + dvy.t)^2
	    // r^2 = dx^2 + dvx^2.t^2 + 2.dx.dvx.t + dy^2 + dvy^2.t^2 + 2.dy.dvy.t
	    // (dvx^2 + dvy^2)t^2 + (2.dx.dvx + 2.dy.dvy)t + (dx^2 + dy^2 - r^2) = 0
	    //
	    // -- then apply standard quadratic formula for ax^2 + bx + c = 0 

	    double a, b, c;    // the three quadratic equation coeffs
	    double dvx2 = dvx * dvx;
	    double dvy2 = dvy * dvy;  
	    double root;          // square root contents of quadratic eqn, ie (b^2 - 4ac)
	    

	    double tCorner;    // time it would hit the corner in question
	    
	    a = dvx2 + dvy2;  // same in all cases

	    // give up now if mol and piston moving with same vel or already found overlapping
	    if ((a > NEARLY_ZERO) && (PT_OVERLAPS != nHint))
	    {                                               
		    // Mol and piston not moving with same vel, so check in turn to see
		    // if mol may hit any of four corners
		    
		    
		    // start with top left corner

		    dx = dxl;
		    dy = dyt;
		    
		    b = 2 * (dx * dvx + dy * dvy);
		    c = dx * dx + dy * dy - molRadiusSqd;
		    root = b * b - 4 * a * c; 
				    
		    if (root >= 0)
		    {
			    // solution(s) exist
			    root = sqrt(root);
					    
			    // We only check the earlier solution. It could be that this is -ve
			    // and the other +ve, in which case it looks as if we should take the
			    // latter; but if so, the mol must already be overlapping the corner,
			    // in which case we want to let it go anyway.
					    
			    tCorner = (-b - root) / 2 / a;  
                if (tCorner < t)
			    {
    			    t = tCorner;
    			    nHint = PT_HITS_TOP_LEFT_CORNER;
			    }

		    }
                                
                                
		    // top right corner next
		    
		    dx = dxr;
		    
		    b = 2 * (dx * dvx + dy * dvy);
		    c = dx * dx + dy * dy - molRadiusSqd;
		    root = b * b - 4 * a * c; 
				    
		    if (root >= 0)
		    {
			    root = sqrt(root);
					    
			    tCorner = (-b - root) / 2 / a;  
                if (tCorner < t)
			    {
    			    t = tCorner;
    			    nHint = PT_HITS_TOP_RIGHT_CORNER;
			    }

		    }
                                
		    // bottom right corner next
		    
		    dy = dyb;
		    
		    b = 2 * (dx * dvx + dy * dvy);
		    c = dx * dx + dy * dy - molRadiusSqd;
		    root = b * b - 4 * a * c; 
				    
		    if (root >= 0)
		    {
			    root = sqrt(root);
					    
			    tCorner = (-b - root) / 2 / a;  
                if (tCorner < t)
			    {
    			    t = tCorner;
    			    nHint = PT_HITS_BOTTOM_RIGHT_CORNER;
			    }

		    }
                                

		    // bottom left corner last
		    
		    dx = dxl;
		    
		    b = 2 * (dx * dvx + dy * dvy);
		    c = dx * dx + dy * dy - molRadiusSqd;
		    root = b * b - 4 * a * c; 
				    
		    if (root >= 0)
		    {
			    root = sqrt(root);
					    
			    tCorner = (-b - root) / 2 / a;  
                if (tCorner < t)
			    {
    			    t = tCorner;
    			    nHint = PT_HITS_BOTTOM_LEFT_CORNER;
			    }

		    }

	    }  // end of if (a > NEARLY_ZERO)
	
    }  // end of if (PT_NO_HIT == nHint, i.e. checking for normal collisions
    
	if (PT_NO_HIT != nHint)
	{		   
		// correct calculated time for any time already done by mol or piston
		t += t0;
				
		if ((t > m_pSim->m_dTimeSoFar) && (t <= m_pSim->m_dDeltaTime))
		{
			// due to hit collide in this timestep
			pNP->m_dTimeDue = t; 
			pNP->m_nEventHint = nHint;
				  
			m_pSim->InsertNode(pNP);
				 
			ASSERT(NR_NOT_IN_TREE != pNP->m_nParentRelationship);
		}
	}
}

void CMolPistonHardInt::ExecHardEvent(CNP* pNP)
{                     
	// Execute the event indicated by the neighbour pair object and
	// recalc all affected events     
	
	CSObjPiston* pPiston;
	CSObjMol* pMol;   
	
#ifdef _DEBUG
	global_debug_counter++;
#endif  

#if (CI_MOL < CI_PISTON)
	CObjTypeMol* pMolType = (CObjTypeMol*) m_pType1;
	CObjTypePiston* pPistonType = (CObjTypePiston*) m_pType2;
#else
	CObjTypeMol* pMolType = (CObjTypeMol*) m_pType2;
	CObjTypePiston* pPistonType = (CObjTypePiston*) m_pType1;
#endif        

	CObjTypeWall* pWallType = pPistonType->m_pWallType;                                            

#if (CI_MOL < CI_PISTON)
	pMol = (CSObjMol*) pNP->m_pSObj1;
	pPiston = (CSObjPiston*) pNP->m_pSObj2;
#else
	pMol = (CSObjMol*) pNP->m_pSObj2;
	pPiston = (CSObjPiston*) pNP->m_pSObj1;
#endif


    TRACE("CMolPistonHardInt::ExecHardEvent() st=%.20g tsf=%.20g ids: m %ld, p %ld\n", m_pSim->m_dSimTime, m_pSim->m_dTimeSoFar, pMol->m_lId, pPiston->m_lId);

	// Do move here; first move mol and piston to collision positions
	
	
	double dt1, dt2;
    
    if (pMol->m_nTouched)
    {
		dt1 = pNP->m_dTimeDue - pMol->m_dTimeDoneAlready;
	}
	else
	{
		dt1 = pNP->m_dTimeDue;
	}
	
    if (pPiston->m_nTouched)
    {
		dt2 = pNP->m_dTimeDue - pPiston->m_dTimeDoneAlready;
	}
	else
	{
		dt2 = pNP->m_dTimeDue;
	}
	

	ASSERT(pMol->m_fsValid.bf.x);
	ASSERT(pMol->m_fsValid.bf.y);
	ASSERT(pMol->m_fsValid.bf.vx);
	ASSERT(pMol->m_fsValid.bf.vy);
    
    
    // Now actually move them  

	double dx, dy;   // amount piston needs to move
	
    pMol->m_dX += pMol->m_dVx * dt1;
	pMol->m_dY += pMol->m_dVy * dt1;    
	pMol->m_dTimeDoneAlready = pNP->m_dTimeDue;
	pMol->m_nTouched = TRUE; 
	
	dx = pPiston->m_dVx * dt2;
	dy = pPiston->m_dVy * dt2;
    pPiston->m_dXLeft += dx;
    pPiston->m_dXRight += dx;
	pPiston->m_dYTop += dy;    
	pPiston->m_dYBottom += dy;    
    pPiston->m_dXDisplacement = pPiston->m_dXLeft - pPiston->m_dXLRest;
    pPiston->m_dYDisplacement = pPiston->m_dYTop - pPiston->m_dYTRest;	
	pPiston->m_dTimeDoneAlready = pNP->m_dTimeDue;
	pPiston->m_nTouched = TRUE;
	

	// Have got mol and piston to virtually touching position. Now work
	// out their new trajectories.

	// what happens to mol speed and direction now depends on wall properties,
	// and whether we hit the end or the middle of the wall

	double nx, ny;    // normal to piston face or corner
	
	// If the molecule is going to hit not a piston face but 
	// instead one of its corners, want it to behave as if the corner were
	// an infinitesimal quarter-circle; choose direction and normal
	// appropriately; and in either case, want to consistently have normal
	// pointing away from wall as far as molecule is concerned.
	
	double length; 
	    
    switch (pNP->m_nEventHint)
    {
		case PT_HITS_LEFT_FACE:	 
			nx = -1;
			ny = 0;
        	break;
        	
		case PT_HITS_RIGHT_FACE:	 
			nx = 1;
			ny = 0;
        	break;
        	
		case PT_HITS_BOTTOM_FACE:	 
			nx = 0;
			ny = -1;
        	break;
        	
		case PT_HITS_TOP_FACE:	 
			nx = 0;
			ny = 1;
        	break;
        	
        case PT_HITS_TOP_LEFT_CORNER:
        	// normal is vector from corner to mol
        	nx = pMol->m_dX - pPiston->m_dXLeft;
        	ny = pMol->m_dY - pPiston->m_dYTop; 
        	length = sqrt(nx * nx + ny * ny);
        	if (length > NEARLY_ZERO)
        	{
        		nx /= length;
        		ny /= length;
        	}
        	else
        	{         
        		// if get mol centre on corner (shouldn't happen) go for 45 degrees
        		nx = -SQRT_2_OVER_2;
        		ny = SQRT_2_OVER_2;
        	}
        	break;
        	
        case PT_HITS_TOP_RIGHT_CORNER:
        	// normal is vector from corner to mol
        	nx = pMol->m_dX - pPiston->m_dXRight;
        	ny = pMol->m_dY - pPiston->m_dYTop; 
        	length = sqrt(nx * nx + ny * ny);
        	if (length > NEARLY_ZERO)
        	{
        		nx /= length;
        		ny /= length;
        	}
        	else
        	{         
        		// if get mol centre on corner (shouldn't happen) go for 45 degrees
        		nx = SQRT_2_OVER_2;
        		ny = SQRT_2_OVER_2;
        	}
        	break;

        case PT_HITS_BOTTOM_RIGHT_CORNER:
        	// normal is vector from corner to mol
        	nx = pMol->m_dX - pPiston->m_dXRight;
        	ny = pMol->m_dY - pPiston->m_dYBottom; 
        	length = sqrt(nx * nx + ny * ny);
        	if (length > NEARLY_ZERO)
        	{
        		nx /= length;
        		ny /= length;
        	}
        	else
        	{         
        		// if get mol centre on corner (shouldn't happen) go for 45 degrees
        		nx = SQRT_2_OVER_2;
        		ny = -SQRT_2_OVER_2;
        	}
        	break;

        case PT_HITS_BOTTOM_LEFT_CORNER:
        	// normal is vector from corner to mol
        	nx = pMol->m_dX - pPiston->m_dXLeft;
        	ny = pMol->m_dY - pPiston->m_dYBottom; 
        	length = sqrt(nx * nx + ny * ny);
        	if (length > NEARLY_ZERO)
        	{
        		nx /= length;
        		ny /= length;
        	}
        	else
        	{         
        		// if get mol centre on corner (shouldn't happen) go for 45 degrees
        		nx = -SQRT_2_OVER_2;
        		ny = -SQRT_2_OVER_2;
        	}
        	break;
        	
        default:
        	ASSERT(FALSE);
    }
        
    
    // Now calculate the new trajectories:
	
	if (WT_ELASTIC == pWallType->m_nMolIntType)
    {
    	// elastic collision between wall and mol; KE conserved,
    	// momentum only if mol hits face pushing piston in direction
    	// of motion
    	
    	// Let us suppose that the molecule hits the corner of the piston
    	// so that the angle between its centre and the corner is theta.
    	// If the piston is constrained to move only in a vertical direction,
    	// its x-vel vpx = 0 always. The mol and piston have masses mm and mp.
    	// They exchange some impulse p such that
    	//
    	//     vpy' = vpy + p.sin(theta)/mp    
    	//     vmy' = vmy - p.sin(theta)/mp
    	//     vmx' = vmx - p.cos(theta)/mp
    	//
    	// Equating the total kinetic energy before and after the collision,
    	// and solving for p, we get
    	//
    	//      2(vmx.cos(theta) + vmy.sin(theta) - vpy.sin(theta)
    	// p =  --------------------------------------------------
    	//                 (1/mm + sin2(theta)/mp)
    	//
    	// We already have sin(theta) and cos(theta), the direction cosines
    	// for the wall-molecule direction (ny and nx). (May have chosen them
    	// with opposite sign, but never mind.) Consider next the
    	// special cases where the molecule hits the left face of the piston
    	// or the bottom face. If hits the left, theta = 0, sin(theta)=0,
    	// cos(theta) = 1 and
    	// 
    	// p = 2.vmx * mm,   i.e. double the present x-momentum of the molecule,
    	//                   so the molecule is reversed and the piston is unmoved.
    	//
    	// If the molecule hits the bottom face such that theta = pi/2, then
    	//
    	// p = 2(vmy - vpy) / (1/mm + 1/mp)
    	//
    	// which hopefully conserves momentum too. As these special cases save
    	// us a bit of maths and we already know whether they hold, they are
    	// treated separately.

		double p;         
		    	
		switch (pNP->m_nEventHint)
		{
			case PT_HITS_LEFT_FACE:	 
			case PT_HITS_RIGHT_FACE:
			
				if (PT_VERTICAL == pPistonType->m_nDirectionType)	 
				{ 
					pMol->m_dVx = -pMol->m_dVx;
					pMol->m_dCx = -pMol->m_dCx;
				}
				else
				{             
					p = 2 * (pMol->m_dVx - pPiston->m_dVx) 
					     / ((1.0 / pMolType->m_dMass) + (1.0 / pPistonType->m_dMass));
					
					pMol->m_dVx -= p / pMolType->m_dMass;
					pPiston->m_dVx += p / pPistonType->m_dMass;
					pMol->m_dSpeed = sqrt(pMol->m_dVx * pMol->m_dVx + pMol->m_dVy * pMol->m_dVy);    
					if (pMol->m_dSpeed > NEARLY_ZERO)
					{
						pMol->m_dCx = pMol->m_dVx / pMol->m_dSpeed;
						pMol->m_dCy = pMol->m_dVy / pMol->m_dSpeed;
					}
					else
					{
						pMol->m_dCx = 1.0;
						pMol->m_dCy = 1.0;
					}
				}
		    	break;
		        	
			case PT_HITS_BOTTOM_FACE:	 
			case PT_HITS_TOP_FACE:	 

				if (PT_VERTICAL == pPistonType->m_nDirectionType)	 
				{                 
					p = 2 * (pMol->m_dVy - pPiston->m_dVy) 
					     / ((1.0 / pMolType->m_dMass) + (1.0 / pPistonType->m_dMass));
					
					pMol->m_dVy -= p / pMolType->m_dMass;
					pPiston->m_dVy += p / pPistonType->m_dMass;
					pMol->m_dSpeed = sqrt(pMol->m_dVx * pMol->m_dVx + pMol->m_dVy * pMol->m_dVy);    
					if (pMol->m_dSpeed > NEARLY_ZERO)
					{
						pMol->m_dCx = pMol->m_dVx / pMol->m_dSpeed;
						pMol->m_dCy = pMol->m_dVy / pMol->m_dSpeed;
					}
					else
					{
						pMol->m_dCx = 1.0;
						pMol->m_dCy = 1.0;
					}	
				}
				else
				{
					pMol->m_dVy = -pMol->m_dVy;
					pMol->m_dCy = -pMol->m_dCy;
				}
		    	break;
		        	
		    case PT_HITS_TOP_LEFT_CORNER:
		    case PT_HITS_TOP_RIGHT_CORNER:
		    case PT_HITS_BOTTOM_RIGHT_CORNER:
		    case PT_HITS_BOTTOM_LEFT_CORNER:
				
				double p;

				if (PT_VERTICAL == pPistonType->m_nDirectionType)	 
				{                 
					p  = 2 * (pMol->m_dVx * nx + ny * (pMol->m_dVy - pPiston->m_dVy))
					     / (1 / pMolType->m_dMass + ny * ny / pPistonType->m_dMass);  
					     
					pPiston->m_dVy += p * ny / pPistonType->m_dMass;
				}
				else
				{                 
					p  = 2 * (nx * (pMol->m_dVx - pPiston->m_dVx) + ny * pMol->m_dVy)
					     / (1 / pMolType->m_dMass + nx * nx / pPistonType->m_dMass);

					pPiston->m_dVy += p * nx / pPistonType->m_dMass;
				}            
				
				pMol->m_dVx -= nx * p / pMolType->m_dMass;
				pMol->m_dVy -= ny * p / pMolType->m_dMass;
				break;
		        	
		    default:
		    	ASSERT(FALSE);
		}    
		
		pMol->m_dSpeed = sqrt(pMol->m_dVx * pMol->m_dVx + pMol->m_dVy * pMol->m_dVy);    
		if (pMol->m_dSpeed > NEARLY_ZERO)
		{
			pMol->m_dCx = pMol->m_dVx / pMol->m_dSpeed;
			pMol->m_dCy = pMol->m_dVy / pMol->m_dSpeed;
		}
		else
		{
			pMol->m_dCx = 1.0;
			pMol->m_dCy = 0.0;
		}	
                                
	}
    else 
	{
		// With these wall properties, the interaction is thermal;
		// the molecule sticks to the piston, thermally accommodates
		// to it, and leaves.
		
		// First accommodate mol to piston; combine momenta in piston's
		// allowed direction of movement, and lose it in the normal
		// direction.
		
		double dTotMtm; 
		double dCombinedSpeed;   

		double mm = pMolType->m_dMass;
		double mp = pPistonType->m_dMass;

		double dMassSum = mp + mm;
		
		if (PT_HORIZONTAL == pPistonType->m_nDirectionType)
		{
			dTotMtm = pPiston->m_dVx * mp + pMol->m_dVx * mm;   
			
			if (dMassSum > NEARLY_ZERO)
			{          
				dCombinedSpeed = dTotMtm / dMassSum;
			}
			else
			{
				// should never be allowed to get zero masses, but just in case
				dCombinedSpeed = 0;
			}
			
			pMol->m_dVx = dCombinedSpeed; 
			pMol->m_dVy = 0;
			pPiston->m_dVx = dCombinedSpeed;
		}
		else
		{
			dTotMtm = pPiston->m_dVy * mp + pMol->m_dVy * mm;   
			
			if (dMassSum > NEARLY_ZERO)
			{          
				dCombinedSpeed = dTotMtm / dMassSum;
			}
			else
			{
				// should never be allowed to get zero masses, but just in case
				dCombinedSpeed = 0;
			}
			
			pMol->m_dVy = dCombinedSpeed;
			pMol->m_dVx = 0;
			pPiston->m_dVy = dCombinedSpeed;
		}     
		
		// Now do desorption of molecule from piston surface; in general, piston
		// will simultaneously recoil. In the limit of a very heavy piston, the 
		// recoil will be negligible and the molecule will depart with the full
		// expected velocity relative to the piston. In the limit of a very light
		// piston, the molecule will not move at all in that direction but the
		// piston will recoil instead.
		//
		// For example, say the molecule is stuck to the bottom face
		// of the piston, and if the piston were infinitely heavy it
		// would be ejected with a speed ve in a direction theta from
		// the normal to the piston. The impulse p acts in an absolute
		// direction phi =  - (pi/2 + theta) in this case. (In general,
		// phi = normal dirn - theta). Before desorption, both mol
		// and piston are travelling upwards with a speed vi. To conserve
		// momentum in the vertical direction,
		//
		//  vp'  = vi - p.sin(phi)/mp
		//  vmy' = vi + p.sin(phi)/mm
		//  vmx' =  0 + p.cos(phi)/mm
		//  
		// Now we can solve for p conserving energy, with a potential
		// energy initially present of mm.ve^2/2 for desorbing the molecule:
		//
		//  mm.ve^2/2 + (mm + mp).vi^2/2 = mm.vm'^2/2 + mp.vp'^2/2
		//
		// substituting the expressions for vp' etc we end up with
		//
		//           {              mm.ve^2                   }
		//  p = sqrt {----------------------------------------}
		//           { cos^2(phi)/mm + sin^2(phi)(1/mp + 1/mm)}  
		//
		// and then using the expressions for vp' we get the new velocities.

		double theta = m_pSim->CosineDistributionAngle();

		// get direction cosines for this angle
		double cosTheta = cos(theta);
		double sinTheta = sin(theta);  
		
		// apply rotation matrix to subtract angle of normal so that
		// we get this new angle relative to the normal rather than
		// relative to zero (i.e. east)
		//
		// to rotate by x, premultiply by (cos x   -sin x)
		//                                (sin x    cox x)
		//
		// by minus x, cos (-x) = cos (x), sin (-x) = - sin (x) so
		// use:
		//           ( cos x  sin x)
		//           (-sin x  cos x)   
		
		double cosPhi =  nx * cosTheta + ny * sinTheta;
		double sinPhi = -ny * cosTheta + nx * sinTheta;

		// direction should be in same sense as normal to wall
		double dotprod = cosPhi * nx + sinPhi * ny;
		if (dotprod < 0)
		{
			cosPhi = -cosPhi;
			sinPhi = -sinPhi;
		}      
		
		double cos2Phi = cosPhi * cosPhi;
		double sin2Phi = sinPhi * sinPhi;
		
		double dNewSpeed;  // ve in above maths
		
		if (WT_USE_SPECIFIC_TEMP == pWallType->m_nTemperatureType)
		{
			dNewSpeed = m_pSim->ThermalWallSpeed(pWallType->m_dTemperature, mm);
		}
		else
	    {
			dNewSpeed = m_pSim->ThermalWallSpeed(m_pSim->m_dTemperature, mm);
		}    
		
		double root = (mm * dNewSpeed * dNewSpeed)
		              / (cos2Phi / mm + sin2Phi * (1.0 / mp + 1.0 / mm));
		              
		double p;
		
		if (root >= 0)
		{
			p = sqrt(root);
		}
		else
		{
			// shouldn't happen, but just in case
			p = 0;                        
			ASSERT(FALSE);
		}
          
        
        // now apply calculated impulse to work out new vels
        
        pMol->m_dVx += p * cosPhi / mm;
        pMol->m_dVy += p * sinPhi / mm;
          
		if (PT_HORIZONTAL == pPistonType->m_nDirectionType)
		{
        	pPiston->m_dVx -= p * cosPhi / mp;
        }
        else
        {
        	pPiston->m_dVy -= p * sinPhi / mp;
        }
		
		pMol->m_dSpeed = sqrt(pMol->m_dVx * pMol->m_dVx + pMol->m_dVy * pMol->m_dVy);    
		if (pMol->m_dSpeed > NEARLY_ZERO)
		{
			pMol->m_dCx = pMol->m_dVx / pMol->m_dSpeed;
			pMol->m_dCy = pMol->m_dVy / pMol->m_dSpeed;
		}
		else
		{
			pMol->m_dCx = 1.0;
			pMol->m_dCy = 0.0;  
		}	
	}                 


	// don't allow wild speeds as a result of big mass mismatches	
	pMol->ClipSpeed();                                           
	pPiston->ClipSpeed();	
	
		
	// Now recalc all other events involving this mol and piston 
	// Don't bother with this particular NP, as it shouldn't
	// happen again unless some other event happens first (in
	// which case it will cause this one to be recalculated).
	
	CListHandle* pAffectedHandle = pMol->m_pFirstHandle; 
	CNP* pAffectedNP;
	
	ASSERT(NULL != pAffectedHandle); // should at least have this one attached
	
	while (NULL != pAffectedHandle)
	{   
		if (pNP != (pAffectedNP = pAffectedHandle->m_pNP))
		{
			// this NP isn't the one we've just done the move for            
			if (NR_NOT_IN_TREE != pAffectedNP->m_nParentRelationship)
			{
				m_pSim->DetachNode(pAffectedNP);  
				ASSERT(NR_NOT_IN_TREE == pAffectedNP->m_nParentRelationship);
			}   

			pAffectedNP->m_pInt->CalcHardEvent(pAffectedNP);
		}
				
		pAffectedHandle = pAffectedHandle->m_pNextHandle;
	}	
    
    // repeat for piston
	pAffectedHandle = pPiston->m_pFirstHandle; 
	pAffectedNP;
	
	ASSERT(NULL != pAffectedHandle); // should at least have this one attached
	
	while (NULL != pAffectedHandle)
	{   
		if (pNP != (pAffectedNP = pAffectedHandle->m_pNP))
		{
			// this NP isn't the one we've just done the move for            
			if (NR_NOT_IN_TREE != pAffectedNP->m_nParentRelationship)
			{
				m_pSim->DetachNode(pAffectedNP);  
				ASSERT(NR_NOT_IN_TREE == pAffectedNP->m_nParentRelationship);
			}   

			pAffectedNP->m_pInt->CalcHardEvent(pAffectedNP);
		}
				
		pAffectedHandle = pAffectedHandle->m_pNextHandle;
	}	

}


