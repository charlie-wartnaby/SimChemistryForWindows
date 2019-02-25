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
#include "mmhrdint.h" 
#include "lennard.h"

#include <math.h>

#ifdef _DEBUG
extern long global_debug_counter;
#endif


CMolMolHardInt::CMolMolHardInt()
{
	m_nBaseType1 = CI_MOL;
	m_nBaseType2 = CI_MOL;

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

	m_sName = "mol_mol_hard_int";
	m_sDescription = "Molecules bounce off each other like billiard balls"
	                 " so as to conserve kinetic energy and momentum, taking"
	                 " account of their individual masses and radii.";

	m_nIsHard = TRUE;
	m_nIsSoft = FALSE;
}
	
CInt* CMolMolHardInt::Clone()
{
	CInt* pInt = new CMolMolHardInt;
	pInt->m_pSim = m_pSim;
	return pInt;
}


void CMolMolHardInt::BuildNeighList()
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
	

	m_dSafetySqdIncRadii = m_pSim->m_dSafetyDist 
	                        + pMolType1->m_dHSRadius + pMolType2->m_dHSRadius;
	m_dSafetySqdIncRadii *= m_dSafetySqdIncRadii; 
	
	// squared sum of radii not needed here but will be in CalcHardEvent
	m_dRadiusSumSqd = pMolType1->m_dHSRadius + pMolType2->m_dHSRadius;
	m_dRadiusSumSqd *= m_dRadiusSumSqd;
	
	
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
					pNP = new CNP((CInt*) this);
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

		   		// add this NP to linked list attached to each molecule
				pNP->m_handle1.m_pNextHandle = pMol1->m_pFirstHandle;
				pNP->m_handle2.m_pNextHandle = pMol2->m_pFirstHandle;
				
				pMol1->m_pFirstHandle = &(pNP->m_handle1);
				pMol2->m_pFirstHandle = &(pNP->m_handle2);	


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

void CMolMolHardInt::CalcAllHardEvents()
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

void CMolMolHardInt::CalcHardEvent(CNP* pNP)
{   

#ifdef _DEBUG
	global_debug_counter++;
#endif
	
	CObjTypeMol* pMolType1 = (CObjTypeMol*) m_pType1;
	CObjTypeMol* pMolType2 = (CObjTypeMol*) m_pType2;

	CSObjMol* pMol1 = (CSObjMol*) pNP->m_pSObj1;
	CSObjMol* pMol2 = (CSObjMol*) pNP->m_pSObj2;


	// Mols hit wall when their separation equals the sum of their radii. 

	
	// Get co-ords of mols at same starting point in time
		
	ASSERT(pMol1->m_fsValid.bf.x);
	ASSERT(pMol1->m_fsValid.bf.y);
	ASSERT(pMol1->m_fsValid.bf.vx);
	ASSERT(pMol1->m_fsValid.bf.vy);
	ASSERT(pMol2->m_fsValid.bf.x);
	ASSERT(pMol2->m_fsValid.bf.y);
	ASSERT(pMol2->m_fsValid.bf.vx);
	ASSERT(pMol2->m_fsValid.bf.vy);
    
    double x1, y1, x2, y2; 
    
    double t0;  // time we place mols at, later of their individual times
	double dt;  // time difference between mols at the moment
	    
    if (pMol1->m_nTouched)
    {
    	if (pMol2->m_nTouched)
    	{
    		// Both mols have already moved this timestep. Make
    		// the one that is further behind catch up the one
    		// that is ahead.       
    		dt = pMol1->m_dTimeDoneAlready - pMol2->m_dTimeDoneAlready;
    		
    		if (dt > 0)
    		{
    			// first mol ahead, move second       
    			x1 = pMol1->m_dX;
    			y1 = pMol1->m_dY;
    			x2 = pMol2->m_dX + pMol2->m_dVx * dt;
    			y2 = pMol2->m_dY + pMol2->m_dVy * dt;
    			t0 = pMol1->m_dTimeDoneAlready;     
    		}
    		else
    		{
    			// second mol ahead (or dead heat), move first along a bit
		    	x1 = pMol1->m_dX - pMol1->m_dVx * dt;
		    	y1 = pMol1->m_dY - pMol1->m_dVy * dt;
		    	x2 = pMol2->m_dX;
    			y2 = pMol2->m_dY; 
    			t0 = pMol2->m_dTimeDoneAlready;
		    }   
		}
		else
		{
			// first has moved but second hasn't
			dt = pMol1->m_dTimeDoneAlready;
			x1 = pMol1->m_dX;
    		y1 = pMol1->m_dY;
			x2 = pMol2->m_dX + pMol2->m_dVx * dt;
    		y2 = pMol2->m_dY + pMol2->m_dVy * dt;
    		t0 = pMol1->m_dTimeDoneAlready;
    	}
    }
    else
    {
    	// first hasn't yet moved this timestep
		if (pMol2->m_nTouched)
		{
			// but second has
			dt = pMol2->m_dTimeDoneAlready;
			x1 = pMol1->m_dX + pMol1->m_dVx * dt;
    		y1 = pMol1->m_dY + pMol1->m_dVy * dt; 
    		x2 = pMol2->m_dX;
    		y2 = pMol2->m_dY;  
    		t0 = pMol2->m_dTimeDoneAlready;
    	}                          
    	else
    	{
    		// neither has yet moved
    		t0 = 0; 
    		x1 = pMol1->m_dX;
    		y1 = pMol1->m_dY; 
    		x2 = pMol2->m_dX;
    		y2 = pMol2->m_dY;
    	}
    }
    
	    
	// Now work out relative separation and velocity
	
	double dx = x2 - x1;
	double dy = y2 - y1; 
	double dvx = pMol2->m_dVx - pMol1->m_dVx;
	double dvy = pMol2->m_dVy - pMol1->m_dVy; 


	// Now work out when might hit each other.		
	// Sadly the distance between the mols is
	// quadratic in time, so we have to do a square root. (I think.) 
	// Hit when separation between mol centres 
	// equals sum of mol radii.
	// r^2 = (dx + dvx.t)^2 + (dy + dvy.t)^2
	// r^2 = dx^2 + dvx^2.t^2 + 2.dx.dvx.t + dy^2 + dvy^2.t^2 + 2.dy.dvy.t
	// (dvx^2 + dvy^2)t^2 + (2.dx.dvx + 2.dy.dvy)t + (dx^2 + dy^2 - r^2) = 0
	//
	// -- then apply standard quadratic formula for ax^2 + bx + c = 0 
    //
    // It was a problem that if a molecule was 'rattling' against others at
    // low temperature, under the pressure of a piston, it could end up
    // overlapping another molecule in error, as collision times became
    // closer and closer together in time and eventually numerical errors
    // (I presume) led to missing a collision.
    // To be more robust, if two molecules are already overlapping, then
    // we set a collision radius a little smaller than their actual separation,
    // and let them collide at this smaller radius if they have converging
    // trajectories. But we leave a bit of space, so that we don't end up
    // with an infinite number of collisions if molecules are squashed up
    // against each other.

    double actual_separation_radius_sum_sqd = dx * dx + dy * dy;
    double collision_radius_sum_sqd_to_use;
    if (actual_separation_radius_sum_sqd <= m_dRadiusSumSqd)
    {
        // The molecules overlap already, so use smaller collision radius than we should
        // fudge factor from trial and error, 0.99 too small, 0.999 gives reasonable
        // compromise between resilience to overlapping molecules getting squashed
        // together, and excessively slow simulation if molecules are trapped in
        // an overlapping configuration.
        collision_radius_sum_sqd_to_use = actual_separation_radius_sum_sqd * 0.999;
    }
    else
    {
        collision_radius_sum_sqd_to_use = m_dRadiusSumSqd;
    }

	double a, b, c;
	double dvx2 = dvx * dvx;
	double dvy2 = dvy * dvy;  
	double root;
	
	a = dvx2 + dvy2; 
	b = 2 * (dx * dvx + dy * dvy);
	c = dx * dx + dy * dy - collision_radius_sum_sqd_to_use;
	root = b * b - 4 * a * c; 
	
	double t;
			
	if (root >= 0 && a > NEARLY_ZERO)
	{
		// solution(s) exist
		root = sqrt(root);
				
		// We only check the earlier solution. It could be that this is -ve
		// and the other +ve, in which case it looks as if we should take the
		// latter; but if so, the mols must already overlapping (with diverging
        // trajectories), in which case we want to let them go anyway.
				
		t = (-b - root) / 2 / a;  
		
		// allow for time already spent
		t += t0;

		if ((t > m_pSim->m_dTimeSoFar) && (t <= m_pSim->m_dDeltaTime))
		{
			// due to collide in this timestep
			pNP->m_dTimeDue = t; 
				  
			m_pSim->InsertNode(pNP);
				 
			ASSERT(NR_NOT_IN_TREE != pNP->m_nParentRelationship);
		}

	}
}

void CMolMolHardInt::ExecHardEvent(CNP* pNP)
{
	// Execute the event indicated by the neighbour pair object and
	// recalc all affected events     


#ifdef _DEBUG
	global_debug_counter++;
#endif  

	CObjTypeMol* pMolType1 = (CObjTypeMol*) m_pType1;
	CObjTypeMol* pMolType2 = (CObjTypeMol*) m_pType2;

	CSObjMol* pMol1 = (CSObjMol*) pNP->m_pSObj1;
	CSObjMol* pMol2 = (CSObjMol*) pNP->m_pSObj2;

    TRACE("CMolMolHardInt::ExecHardEvent() st=%.20g tsf=%.20g ids: %ld, %ld\n", m_pSim->m_dSimTime, m_pSim->m_dTimeSoFar, pMol1->m_lId, pMol2->m_lId);

    MoveToTouching(pMolType1, pMol1, pMolType2, pMol2, pNP);

	DoCollisionAndCleanUp(pMolType1, pMol1, pMolType2, pMol2, pNP);
}

void CMolMolHardInt::DoCollisionAndCleanUp(CObjTypeMol* pMolType1, CSObjMol* pMol1,
                                           CObjTypeMol* pMolType2, CSObjMol* pMol2,
                                           CNP* pNP)
{
	// Have got mols to virtually touching position. Now work
	// out their new trajectories.
	
	// This copied from word; v1, v2, r, rh all vectors:
	// 
	// The two molecules exchange some impulse p, such that
	// -m1dv1 = m2dv2 = p (use this finally to get new vels v1’
	// and v2’) while   
	//
	// m1v1^2 + m2v2^2 = m1v1’^2 + m2v2’^2.
	
	// If the impulse vector p is at some angle theta to the x-axis (which is known), then
	// v1x’ = v1x - (p cos theta) / m1
	// v1y’ = v1y - (p sin theta) / m1
	// v2x’ = v2x + (p cos theta) / m2
	// v2y’ = v2y + (p sin theta) / m2
	
	// Subsituting into the energy eqn,
	// m1v1^2 + m2v2^2 = m1[{(-p cos theta)/m1 + v1x}^2 + {(-p sin theta)/m1 + v1y}^2] + m2[{(p cos theta)/m2 + v2x}^2 + {(p sin theta)/m2 + v2y}^2]
	
	// Multiplying out the brackets and using cos2 theta + sin2 theta = 1,
	// and dividing through by p,
	// 0 = p/m1 + p/m2 + 2(v2.rh) - 2(v1.rh)  (rh = r / r, ie r hat)
	
	// p = 2m1m2(v1.rh - v2.rh)/(m1 + m2)
	// and so we can get the new vels.
	
	double p; // impulse imparted on mol 2 by mol 1, amu.m/s
	double m1 = pMolType1->m_dMass;
	double m2 = pMolType2->m_dMass;       
	
	// r is the vector from 1 to 2:
	double rx = pMol2->m_dX - pMol1->m_dX;
	double ry = pMol2->m_dY - pMol1->m_dY;	
	double rLength = sqrt(rx * rx + ry * ry);
	
	double nrx, nry;  // normalised version of r
	
	double dvx, dvy;  // vel of 2 rel to 1
	
	if ((rLength > NEARLY_ZERO) // mols must not be coincident
	    && (m1 > NEARLY_ZERO)  //  or massless
	    && (m2 > NEARLY_ZERO)) 
	{
		nrx = rx / rLength;
		nry = ry / rLength;
		
		dvx = pMol1->m_dVx - pMol2->m_dVx;
		dvy = pMol1->m_dVy - pMol2->m_dVy;
		
		p = 2 * m1 * m2 * (dvx * nrx + dvy * nry) / (m1 + m2);
		
		double dv1 = -p / m1;
		double dv2 = p / m2;  
		
		pMol1->m_dVx += dv1 * nrx;
		pMol1->m_dVy += dv1 * nry;
		pMol2->m_dVx += dv2 * nrx;
		pMol2->m_dVy += dv2 * nry;
        
        // add impulsive contribution required for pressure calc of hard disks
		double dImpulseContrib = p * (nrx * rx + nry * ry) / 2;
		pMol1->m_dImpulseSum += dImpulseContrib;
		pMol2->m_dImpulseSum += dImpulseContrib;
		
#ifdef _DEBUG
		pMol1->m_fsValid.bf.cosines = 0;
		pMol1->m_fsValid.bf.direction = 0; 
		pMol1->m_fsValid.bf.speed = 0;
		pMol2->m_fsValid.bf.cosines = 0;
		pMol2->m_fsValid.bf.direction = 0; 
		pMol2->m_fsValid.bf.speed = 0;
#endif

		pMol1->m_dSpeed = sqrt(pMol1->m_dVx * pMol1->m_dVx
		                     + pMol1->m_dVy * pMol1->m_dVy);
		                     
		pMol2->m_dSpeed = sqrt(pMol2->m_dVx * pMol2->m_dVx
		                     + pMol2->m_dVy * pMol2->m_dVy);

		if (pMol1->m_dSpeed > NEARLY_ZERO)
		{
			pMol1->m_dCx = pMol1->m_dVx / pMol1->m_dSpeed;
			pMol1->m_dCy = pMol1->m_dVy / pMol1->m_dSpeed;
		}
		else
		{
			// molecule has coincidentally stopped, give it an
			// arbitrary direction
			pMol1->m_dCx = 1;
			pMol1->m_dCy = 0;
		}

		if (pMol2->m_dSpeed > NEARLY_ZERO)
		{
			pMol2->m_dCx = pMol2->m_dVx / pMol2->m_dSpeed;
			pMol2->m_dCy = pMol2->m_dVy / pMol2->m_dSpeed;
		}
		else
		{
			// molecule has coincidentally stopped, give it an
			// arbitrary direction
			pMol2->m_dCx = 1;
			pMol2->m_dCy = 0;
		}

#ifdef _DEBUG
		pMol1->m_fsValid.bf.cosines = 1;
		pMol1->m_fsValid.bf.speed = 1;
		pMol2->m_fsValid.bf.cosines = 1;
		pMol2->m_fsValid.bf.speed = 1;
#endif

		pMol1->ClipSpeed();
		pMol2->ClipSpeed();
	}
	else
	{
		// mols at same co-ords, or of no mass; would have given
		// arithmetic exceptions. let them continue
	}

	
	
	// Now recalc all other events involving these mols  
	// Don't bother with this particular NP, as it shouldn't
	// happen again unless some other event happens first (in
	// which case it will cause this one to be recalculated).
	
	CListHandle* pAffectedHandle = pMol1->m_pFirstHandle; 
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
    
    // repeat for molecule 2
	pAffectedHandle = pMol2->m_pFirstHandle; 
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

void CMolMolHardInt::MoveToTouching(CObjTypeMol* pMolType1,
                                    CSObjMol* pMol1,
                                    CObjTypeMol* pMolType2,
                                    CSObjMol* pMol2,
                                    CNP* pNP)
{
	// Do move here; first move mols together.
	
	double dt1, dt2;
    
    if (pMol1->m_nTouched)
    {
		dt1 = pNP->m_dTimeDue - pMol1->m_dTimeDoneAlready;
	}
	else
	{
		dt1 = pNP->m_dTimeDue;
	}
	
    if (pMol2->m_nTouched)
    {
		dt2 = pNP->m_dTimeDue - pMol2->m_dTimeDoneAlready;
	}
	else
	{
		dt2 = pNP->m_dTimeDue;
	}
	
	ASSERT(pMol1->m_fsValid.bf.x);
	ASSERT(pMol1->m_fsValid.bf.y);
	ASSERT(pMol1->m_fsValid.bf.vx);
	ASSERT(pMol1->m_fsValid.bf.vy);
	ASSERT(pMol2->m_fsValid.bf.x);
	ASSERT(pMol2->m_fsValid.bf.y);
	ASSERT(pMol2->m_fsValid.bf.vx);
	ASSERT(pMol2->m_fsValid.bf.vy);
    
    
    // Now actually move them  
    pMol1->m_dX += pMol1->m_dVx * dt1;
	pMol1->m_dY += pMol1->m_dVy * dt1;    
	pMol1->m_dTimeDoneAlready = pNP->m_dTimeDue;
	pMol1->m_nTouched = TRUE;
    pMol2->m_dX += pMol2->m_dVx * dt2;
	pMol2->m_dY += pMol2->m_dVy * dt2;    
	pMol2->m_dTimeDoneAlready = pNP->m_dTimeDue;
	pMol2->m_nTouched = TRUE;
	
}
