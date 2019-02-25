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
#include "wall.h"    
#include "wmdefint.h"          

#include <math.h>

#ifdef _DEBUG
extern long global_debug_counter;
#endif


CWallMolDefaultInt::CWallMolDefaultInt()
{
	m_nBaseType1 = MIN(CI_MOL, CI_WALL);
	m_nBaseType2 = MAX(CI_MOL, CI_WALL);
    
	if (CI_MOL == m_nBaseType1)
	{             
		m_nType1Mobile = TRUE;
		m_nType2Mobile = FALSE;
	}                          
	else
	{             
		m_nType1Mobile = FALSE;
		m_nType2Mobile = TRUE;
	}                          

	m_nIsDefault = TRUE;
	m_nIsNull = FALSE;      
	m_nHasDetails = FALSE;
	m_pFirstNP = NULL;
	m_pLastNP = NULL;
	m_pFirstSpareNP = NULL;
	m_pType1 = NULL;
	m_pType2 = NULL;
	m_sName = "wall_mol_default";
	m_sDescription = "Molecules bounce off walls like billiard balls"
	                 " with a change in velocity specified by the wall properties";

	m_nIsHard = TRUE;
	m_nIsSoft = FALSE;

#ifdef _DEBUG
	m_lNumNP = 0;
#endif
}      	                                                                

CInt* CWallMolDefaultInt::Clone()
{
	CInt* pInt = new CWallMolDefaultInt;
	pInt->m_pSim = m_pSim;
	return pInt;

}                         
                 
void CWallMolDefaultInt::BuildNeighList()
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
	
#ifdef _DEBUG
	// count spares to ensure correct number still there
	long count = 0;
	pNP = m_pFirstSpareNP;
	while (pNP != NULL)
	{
		count++;
		pNP = pNP->m_pNextNP;
	}
	ASSERT(count == m_lNumNP);
#endif
	
	CNP* pLastNP = NULL;        
	
	
	// For this heterogeneous interaction, need to go through all combinations of
	// pairs of the type of wall and molecule it belongs to checking for those
	// lying within the safety distance.
	
#if (CI_MOL < CI_WALL)
	CObjTypeMol* pMolType = (CObjTypeMol*) m_pType1;
	CObjTypeWall* pWallType = (CObjTypeWall*) m_pType2;
#else
	CObjTypeMol* pMolType = (CObjTypeMol*) m_pType2;
	CObjTypeWall* pWallType = (CObjTypeWall*) m_pType1;
#endif                                                    

	CSObjMol* pMol = (CSObjMol*) pMolType->m_pFirstSObj;
	CSObjWall* pWall;
    
	m_dSafetySqdIncRadius = (m_pSim->m_dSafetyDist / 2) + pMolType->m_dHSRadius;
	m_dSafetySqdIncRadius *= m_dSafetySqdIncRadius; 
	
	m_dMolRadiusSqd = pMolType->m_dHSRadius * pMolType->m_dHSRadius; // needed later on

	
	// loop over all possible pairs of mols and walls of this type		
	while (NULL != pMol)
	{                
		pWall = (CSObjWall*) pWallType->m_pFirstSObj;   
		while (NULL != pWall)
		{
		                      
#ifdef _DEBUG
			global_debug_counter++;
#endif  

			// calc closest approach between edge of mol and wall, and
			// create a neighbour pair entry if that distance is less than
			// the safety distance:
			
			// If some point on the wall Q = m(rx,ry) + (x1,y1), where the
			// wall starts at (x1,y1) and the vector (rx,ry) is normalised
			// and points along the wall, and the centre of the molecule
			// P = (x1,y1) + m(rx,ry) + l(nx,ny) where (nx,ny) is the normalised
			// vector normal to the wall, then we can solve for l and m. Q is
			// the point on the line of the wall closest to the molecule.
			// If 0 <= m <= wall length, then that is it; otherwise, the closest
			// point on the wall will be its beginning or end.

            // If mol is at (dx, dy) relative to start of wall (wx, wy) then
            // 
            //              dx = m.rx + l.nx             (1)
            //              dy = m.ry + l.ny             (2)
            // (rx/ry).dy - dx = l.ny(rx/ry) - l.nx      (2)*(rx/ry) - (1)
            //               l = (rx.dy - dx.ry) / (ny.rx - nx.ry) (solving for l)
            //
            // and ny.rx - nx.ry = -1 always, because (imagine a wall inclined
            // at some angle of less than pi/2 from the origin at an angle theta,
            // with its normal vector pointing up and to the left)
            //  
            //       rx = cos theta,  ry = sin theta
            //       nx = -sin theta, ny = cos theta
            //  so   ny.rx - nx.ry = cos^2 theta + sin^2 theta
            //                     = 1
            //
            // In fact historically my wall normals seem to point the other way, so we
            // get the negative of what we'd expect from that.
			
			double dx = pMol->m_dX - pWall->m_dX1;
			double dy = pMol->m_dY - pWall->m_dY1;
			                                                                         
			double l = dx * pWall->m_dCy - dy * pWall->m_dCx;  
			double m = dy * pWall->m_dNx - dx * pWall->m_dNy; 
			
			double d;
			double d2;
			 
			int nClose;
			
			if (m < 0)
			{
				// do distance to beginning of wall
				d2 = dx * dx + dy * dy;
				nClose = (d2 <= m_dSafetySqdIncRadius);
			}
			else if (m > pWall->m_dLength)
			{                          
				// do distance to end of wall
				dx = pMol->m_dX - pWall->m_dX2;
				dy = pMol->m_dY - pWall->m_dY2;
				d2 = dx * dx + dy * dy;
				nClose = (d2 <= m_dSafetySqdIncRadius);
			}
			else
			{
				// m already within wall, abs(l) is distance to wall
				// from centre of mol
				if (l < 0)
				{
				 	d = -l  - pMolType->m_dHSRadius;
				}                             
				else
				{
					d = l - pMolType->m_dHSRadius;
				}

				nClose = (d <= m_pSim->m_dSafetyDist); 
			}

			if (nClose)
			{
				// The separation between the edge of the molecule and
				// the wall is sufficiently small that they might hit,
				// so add this pair of objects to list. (They might already
				// be overlapping, in which case d will now be -ve.)    
				
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
				pNP->m_pSObj2 = pWall;
#else				
				pNP->m_pSObj2 = pMol;
				pNP->m_pSObj1 = pWall;
#endif
                
		   		// add this NP to linked list attached to molecule
				
#if (CI_MOL < CI_WALL)
				pNP->m_handle1.m_pNextHandle = pMol->m_pFirstHandle;
				pMol->m_pFirstHandle = &(pNP->m_handle1);
#else				
				pNP->m_handle2.m_pNextHandle = pMol->m_pFirstHandle;
				pMol->m_pFirstHandle = &(pNP->m_handle2);	
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
					
			
			pWall = (CSObjWall*) pWall->m_pNextSObj;
		} // loop over walls                          
		
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

void CWallMolDefaultInt::CalcAllHardEvents()
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

void CWallMolDefaultInt::CalcHardEvent(CNP* pNP)
{            

#ifdef _DEBUG
	global_debug_counter++;
#endif
	
	CSObjWall* pWall;
	CSObjMol* pMol;     

#if (CI_MOL < CI_WALL)
	CObjTypeMol* pMolType = (CObjTypeMol*) m_pType1;
	CObjTypeWall* pWallType = (CObjTypeWall*) m_pType2;
#else
	CObjTypeMol* pMolType = (CObjTypeMol*) m_pType2;
	CObjTypeWall* pWallType = (CObjTypeWall*) m_pType1;
#endif                                                    

#if (CI_MOL < CI_WALL)
	pMol = (CSObjMol*) pNP->m_pSObj1;
	pWall = (CSObjWall*) pNP->m_pSObj2;
#else
	pMol = (CSObjMol*) pNP->m_pSObj2;
	pWall = (CSObjWall*) pNP->m_pSObj1;
#endif                                 

#ifdef _DEBUG
	if (pMol->m_nTouched)
	{
		ASSERT(pMol->m_dTimeDoneAlready <= m_pSim->m_dTimeSoFar);
	}
#endif

    
	// Mol hits wall when the distance between it and the wall is equal
	// to the molecular radius. It may hit the middle of the wall, the
	// end of the wall or it may be going parallel to it. 
	
	// t will be the time of the first bit of wall the mol will hit
	double t = NEARLY_INFINITY;
	
	// Get co-ords of mol rel to each end of wall	
	ASSERT(pMol->m_fsValid.bf.x);
	ASSERT(pMol->m_fsValid.bf.y);
	ASSERT(pWall->m_fsValid.bf.x1);	
	ASSERT(pWall->m_fsValid.bf.y1);	
	ASSERT(pWall->m_fsValid.bf.x2);	
	ASSERT(pWall->m_fsValid.bf.y2);	
    
    // Following line prevents GPF when pWall->m_dX1 used after 
    // that (I've absolutely no idea why) (this was with MSVC1.5)
	volatile double x1 = pWall->m_dX1;

    double dx1 = pMol->m_dX - x1;
	double dy1 = pMol->m_dY - pWall->m_dY1; 
	double dx2 = pMol->m_dX - pWall->m_dX2;
	double dy2 = pMol->m_dY - pWall->m_dY2; 



	double dHitDist = 0;
				
	// calc dist from wall we are at present (see above); sign of this
	// shows which side we are on.
	
	// Get direction cosines for wall, which we need a few times
	ASSERT(pWall->m_fsValid.bf.cosines);	
	double rx = pWall->m_dCx;
	double ry = pWall->m_dCy;

    ASSERT(pMol->m_fsValid.bf.vx);
	ASSERT(pMol->m_fsValid.bf.vy);    
    double vx = pMol->m_dVx;
    double vy = pMol->m_dVy; 

	ASSERT(pWall->m_fsValid.bf.normal);        
    double nx = pWall->m_dNx;
    double ny = pWall->m_dNy;

    
	// l is dist between mols current position and
	// closest point on line wall is part of		                                                                         
	double l = dx1 * ry - dy1 * rx;
	
	// m will be dist along wall at which mol is specific distance from line,
    // or dist along wall at which mol starts, to begin with:
	double m = dy1 * nx - dx1 * ny;
		
	int nEnd = WT_NO_HIT;  // default is no collision


    // squared radius of mol
    double r2 = pMolType->m_dHSRadius * pMolType->m_dHSRadius;
	

    // First consider if the molecule is already overlapping the wall. If
    // so and it is going towards the wall, make it collide as if it were a
    // smaller molecule centred at the same point just hitting the wall. This
    // means that any mols that accidentally transgress walls a bit (e.g.
    // if generated by reactions that produce products bigger than the reactants,
    // or clumsy user placement) will still be nicely contained.

    double v_away_from_wall;

    if (m >= 0 && m <= pWall->m_dLength && 
        l <= pMolType->m_dHSRadius && -l <= pMolType->m_dHSRadius)
    {
        // molecule is overlapping some middle part of the wall; do
        // collision if its centre has yet to hit wall, otherwise let
        // it go. Dot product of molecule velocity vector with wall normal
        // gives component of vel towards wall normal; if l is +ve, then
        // +ve vel component means moving away from wall
        v_away_from_wall = vx * nx + vy * ny;
        
        if (l < 0)
        {
            v_away_from_wall = -v_away_from_wall;
        }

        if (v_away_from_wall < 0)
        {
            // Closing on wall so do collision. Do not do it absolutely immediately,
            // because that can lead to infinite collisions if a molecule is trapped
            // overlapping two parallel walls and moving normal to them.
            t = m_pSim->m_dDeltaTime * TIMESTEP_FRACTION_FOR_OVERLAP_COLLN;
            nEnd = WT_HITS_MIDDLE;
        }
    }
    else if (dx1 * dx1 + dy1 * dy1 < r2)
    {
        // overlapping end 1 of wall. 'Velocity' away from it here is
        // unscaled dot product, but sign will still be correct.
        v_away_from_wall = dx1 * vx + dy1 * vy;
        if (v_away_from_wall < 0)
        {
            t = m_pSim->m_dDeltaTime * TIMESTEP_FRACTION_FOR_OVERLAP_COLLN;
            nEnd = WT_END1;
        }
    }
    else if (dx2 * dx2 + dy2 * dy2 < r2)
    {
        // similarly for other end of wall
        v_away_from_wall = dx2 * vx + dy2 * vy;
        if (v_away_from_wall < 0)
        {
            t = m_pSim->m_dDeltaTime * TIMESTEP_FRACTION_FOR_OVERLAP_COLLN;
            nEnd = WT_END2;
        }
    }
    else
    {

        // Now work out ordinary collisions in the future when mol
	    if (l > 0)
	    {
		    // on the side of the wall that the normal points
		    dHitDist = pMolType->m_dHSRadius;
	    }
	    else if (l < 0)
	    {
		    // other side 
		    dHitDist = -pMolType->m_dHSRadius;

	    }
	    else
	    {
		    // exactly on line of wall (very rare, but caught below) 
	    }  
        
        // We now know the distance from the wall we are looking for in
        // order to hit it. If our mol is at (x0 + vx.t, y0 + vy.t) and
        // it ends up a distance dHitDist from the wall, we can solve for
        // the distance from the wall origin, m, and find t:
       
        double denom = rx * vy - ry * vx;

	    if ((denom < NEARLY_ZERO) && (denom > -NEARLY_ZERO))
	    {
		    // the mol is going pretty well parallel to the wall, but
		    // may still hit end of it
	    }
	    else
	    {
		    m = (vy * dx1 - vx * dy1 - dHitDist * (nx * vy - ny * vx)) / denom;
			    
		    // Now if 0 <= m <= wall length, mol is on target to hit wall on the side, 
		    // although it may hit an end sooner.
		    
		    ASSERT(pWall->m_fsValid.bf.length);
			    
		    if ((m >= 0) && (m <= pWall->m_dLength))
		    {
			    // nice normal collision due, work out when it is (or was):
			    t = (dHitDist * (rx * ny - ry * nx) + dx1 * ry - dy1 * rx ) / denom;
			    nEnd = WT_HITS_MIDDLE;
		    }                  
	    }  

	    // Now work out when might hit wall ends.		
	    // Sadly the distance between the mol and the wall ends is
	    // quadratic in time, so we have to do a square root. (I think.) 
	    // Hits end of wall when separation between mol centre and wall
	    // end equals mol radius.
	    // r^2 = (x0 + vx.t - xw)^2 + (y0 + vy.t - yw)^2
	    // r^2 = (dx + vx.t)^2 + (dy + vy.t)^2
	    // r^2 = dx^2 + vx^2.t^2 + 2.dx.vx.t + dy^2 + vy^2.t^2 + 2.dy.vy.t
	    // (vx^2 + vy^2)t^2 + (2.dx.vx + 2.dy.vy)t + (dx^2 + dy^2 - r^2) = 0
	    //
	    // -- then apply standard quadratic formula for ax^2 + bx + c = 0 
	                          
	                          
	    // do quick check to avoid looking at distant wall ends
	    // NB ought to use ProximityHint, checked when build neigh list,
	    // for this

	    int nMightHitEnd1 = (dx1 * dx1 + dy1 * dy1 < m_dSafetySqdIncRadius);
	    int nMightHitEnd2 = (dx2 * dx2 + dy2 * dy2 < m_dSafetySqdIncRadius);

	    double a, b, c;
	    double vx2;
	    double vy2;  
	    double root;
	    
	    double dTEnd;
	    
	    if (nMightHitEnd1 || nMightHitEnd2)
	    {		             
		    vx2 = vx * vx;
		    vy2 = vy * vy;
		    a = vx2 + vy2;  // same for both wall ends	     
	    
		    if (a > NEARLY_ZERO)
		    {		
			    if (nMightHitEnd1)   
			    {
				    // may hit first end of wall
				    b = 2 * (dx1 * vx + dy1 * vy);
				    c = dx1 * dx1 + dy1 * dy1 - m_dMolRadiusSqd;
				    root = b * b - 4 * a * c;
					    
				    if (root >= 0)
				    {
					    // solution(s) exist
					    root = sqrt(root);
						    
					    // We only check the earlier solution. It could be that this is -ve
					    // and the other +ve, in which case it looks as if we should take the
					    // latter; but if so, the mol must already be moving away from the end
					    // of the wall and be overlapping it, in which case we want to let it
					    // go anyway.
						    
					    dTEnd = (-b - root) / 2 / a;
					    
					    if (dTEnd < t)
					    {
						    t = dTEnd;
						    nEnd = WT_END1;
					    }
				    }
			    }  
				    
			    if (nMightHitEnd2)
			    {
				    // may hit second end of wall
				    b = 2 * (dx2 * vx + dy2 * vy);
				    c = dx2 * dx2 + dy2 * dy2 - m_dMolRadiusSqd;
				    root = b * b - 4 * a * c;
					    
				    if (root >= 0)
				    {
					    // solution(s) exist
					    root = sqrt(root);
						    
					    // We only check the earlier solution again.
						    
					    dTEnd = (-b - root) / 2 / a;
					    
					    if (dTEnd < t)
					    {
						    t = dTEnd;
						    nEnd = WT_END2;
					    }
				    }
			    }
		    }   // if (a > 0)
	    }  // if (might hit an end)  
    }
    
	// We now take the earliest time for the same reason we went
	// for the earlier root; if trajectory would hit both ends, or the
	// middle of the wall and an end, but first hit is
	// in the past, must be overlapping wall and want to leave it.
    
    
	if (WT_NO_HIT != nEnd)
	{		
		if (pMol->m_nTouched)
		{
			t += pMol->m_dTimeDoneAlready;
			ASSERT(pMol->m_dTimeDoneAlready <= m_pSim->m_dTimeSoFar);
		}
		
		
				
		if ((t > m_pSim->m_dTimeSoFar) && (t <= m_pSim->m_dDeltaTime))
		{
			// due to hit wall in this timestep
			pNP->m_dTimeDue = t; 
			pNP->m_nEventHint = nEnd;
				  
			m_pSim->InsertNode(pNP);
				 
			ASSERT(NR_NOT_IN_TREE != pNP->m_nParentRelationship);
		}
	}

}

void CWallMolDefaultInt::ExecHardEvent(CNP* pNP)
{   
	// Execute the event indicated by the neighbour pair object and
	// recalc all affected events     
	
	CSObjWall* pWall;
	CSObjMol* pMol;   
	
#ifdef _DEBUG
	global_debug_counter++;
#endif  

#if (CI_MOL < CI_WALL)
	CObjTypeMol* pMolType = (CObjTypeMol*) m_pType1;
	CObjTypeWall* pWallType = (CObjTypeWall*) m_pType2;
#else
	CObjTypeMol* pMolType = (CObjTypeMol*) m_pType2;
	CObjTypeWall* pWallType = (CObjTypeWall*) m_pType1;
#endif                                                    

#if (CI_MOL < CI_WALL)
	pMol = (CSObjMol*) pNP->m_pSObj1;
	pWall = (CSObjWall*) pNP->m_pSObj2;
#else
	pMol = (CSObjMol*) pNP->m_pSObj2;
	pWall = (CSObjWall*) pNP->m_pSObj1;
#endif

    TRACE("CWallMolDefaultInt::ExecHardEvent() st=%.20g tsf=%.20g ids: m %ld, w %ld\n", m_pSim->m_dSimTime, m_pSim->m_dTimeSoFar, pMol->m_lId, pWall->m_lId);

	// Do move here; first move mol up against the wall
	
	ASSERT(pMol->m_fsValid.bf.x);
	ASSERT(pMol->m_fsValid.bf.y);   
	ASSERT(pMol->m_fsValid.bf.vx);
	ASSERT(pMol->m_fsValid.bf.vy);   

	double dt;
	if (pMol->m_nTouched)
	{  
		dt = pNP->m_dTimeDue - pMol->m_dTimeDoneAlready;
		ASSERT(pMol->m_dTimeDoneAlready <= m_pSim->m_dTimeSoFar);
	}
	else
	{
		dt = pNP->m_dTimeDue;
	}         
	
	
	pMol->m_dX += pMol->m_dVx * dt;
	pMol->m_dY += pMol->m_dVy * dt;
	pMol->m_dTimeDoneAlready = pNP->m_dTimeDue;
	pMol->m_nTouched = TRUE;
    
    ASSERT(pMol->m_dTimeDoneAlready <= m_pSim->m_dTimeSoFar);
    

	// what happens to mol speed and direction now depends on wall properties,
	// and whether we hit the end or the middle of the wall

	ASSERT(pWall->m_fsValid.bf.normal);               
	ASSERT(pWall->m_fsValid.bf.ndirn);               

	double nx, ny;    // normal to wall
	
	// If the molecule is going to hit not a normal interior bit of wall but
	// instead one of its ends, want it to behave as if the end was terminated
	// with an infinitesimal semicircle; choose direction and normal
	// appropriately; and in either case, want to consistently have normal
	// pointing away from wall as far as molecule is concerned.
	
	double length; 
	double dotprod;
	    
    switch (pNP->m_nEventHint)
    {
		case WT_HITS_MIDDLE:	 
			nx = pWall->m_dNx;
			ny = pWall->m_dNy;
			ASSERT(pMol->m_fsValid.bf.cosines);
			dotprod = nx * pMol->m_dCx + ny * pMol->m_dCy;
			if (dotprod > 0)
			{
				// normal was towards wall not away from mols perspective
				nx = -nx;
				ny = -ny;
			} 
        	break;
        	
        case WT_END1:
        	// normal is vector from first end of wall to mol
        	nx = pMol->m_dX - pWall->m_dX1;
        	ny = pMol->m_dY - pWall->m_dY1; 
        	length = sqrt(nx * nx + ny * ny);
        	if (length > NEARLY_ZERO)
        	{
        		nx /= length;
        		ny /= length;
        	}
        	else
        	{         
        		// if get mol centre on wall end (shouldn't happen)
        		nx = 1.0;
        		ny = 0.0;
        	}
        	break;
        	
        case WT_END2:
        	// normal is vector from second end of wall to mol
        	nx = pMol->m_dX - pWall->m_dX2;
        	ny = pMol->m_dY - pWall->m_dY2; 
        	length = sqrt(nx * nx + ny * ny);
        	if (length > NEARLY_ZERO)
        	{
        		nx /= length;
        		ny /= length;
        	}
        	else
        	{
        		nx = 1.0;
        		ny = 0.0;
        	}
        	break;    
        	
        default:
        	ASSERT(FALSE);
    }
        
    
    // Now calculate the new direction cosines for the mol:
    
    double cx, cy;
    
	if (WT_ELASTIC == pWallType->m_nMolIntType)
	{                                
		// ensure everything initialised as we expect
		ASSERT(pMol->m_fsValid.bf.cosines);               

	    cx = (ny * ny - nx * nx) * pMol->m_dCx - 2 * nx * ny * pMol->m_dCy;
	    cy = -2 * nx * ny * pMol->m_dCx + (nx * nx - ny * ny) * pMol->m_dCy;
	}
	else
	{ 
		// cosine distribution   
		// using trig functions here, but don't hit walls that often
		
		// get random angle from wall normal fitting cosine distribution
		double dExitAngle = m_pSim->CosineDistributionAngle();

		// get direction cosines for this angle
		double newcx = cos(dExitAngle);
		double newcy = sin(dExitAngle);  
		
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
		
		cx =  nx * newcx + ny * newcy;
		cy = -ny * newcx + nx * newcy;
    }

	// direction should be in same sense as normal to wall
	dotprod = cx * nx + cy * ny;
	if (dotprod < 0)
	{
		pMol->m_dCx = -cx;
		pMol->m_dCy = -cy;
	}
	else
	{		
		pMol->m_dCx = cx;
		pMol->m_dCy = cy;
	}
		
#ifdef _DEBUG   
	pMol->m_fsValid.bf.cosines = 1;
	pMol->m_fsValid.bf.direction = 0;
	pMol->m_fsValid.bf.vx = 0;
    pMol->m_fsValid.bf.vy = 0;
#endif


	// Now deal with new speed for molecule 
	
	if (WT_ELASTIC != pWallType->m_nMolIntType)
	{
		// must thermalise speed
		if (WT_USE_SPECIFIC_TEMP == pWallType->m_nTemperatureType)
		{
			pMol->m_dSpeed = m_pSim->ThermalWallSpeed(pWallType->m_dTemperature,
			                                          pMolType->m_dMass);
		}
		else
	    {
			pMol->m_dSpeed = m_pSim->ThermalWallSpeed(m_pSim->m_dTemperature,
			                                          pMolType->m_dMass);
		}
		
#ifdef _DEBUG
		pMol->m_fsValid.bf.speed = 1;
#endif    

	}
	
	// calc vel vectors from new speed and direction cosines

	ASSERT(pMol->m_fsValid.bf.speed);
	ASSERT(pMol->m_fsValid.bf.cosines);	      
	
	// remember old vel so can get change and hence impulsive
	// contribution to pressure
	double dOldVx = pMol->m_dVx;
	double dOldVy = pMol->m_dVy;
	
	pMol->m_dVx = pMol->m_dCx * pMol->m_dSpeed;
	pMol->m_dVy = pMol->m_dCy * pMol->m_dSpeed;   
	
	dOldVx -= pMol->m_dVx;  // now change in vel
	dOldVy -= pMol->m_dVy;
	
	pMol->m_dImpulseSum += pMolType->m_dMass * pMolType->m_dHSRadius *
	                         sqrt(dOldVx * dOldVx + dOldVy * dOldVy);

#ifdef _DEBUG 
    pMol->m_fsValid.bf.vx = 1;
    pMol->m_fsValid.bf.vy = 1;
	pMol->m_fsValid.bf.direction = 0;  
#endif
	
	
	
	// Now recalc all other events involving this mol  
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
}


