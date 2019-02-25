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
#include "monitor.h"    
#include "mnmldint.h"          

#include <math.h>

#ifdef _DEBUG
extern long global_debug_counter;
#endif



CMonMolNP::CMonMolNP(CInt* pOwningInt)
{
    m_pInt = pOwningInt;
}

CMonMolNP::~CMonMolNP()
{ 
	// should unlink itself tidily from list of near-neighbours
	// maintained by int and/or list of interior mols maintained
	// by monitor if it is within those lists.

	if (m_nFlags & MNML_INTERIOR)
	{
		// is inside monitor but leaving, so must remove itself from interior list
		Unlink();
	}

    // unlinking from standard CNP chain done in CNP::~CNP()
}

void CMonMolNP::Unlink()
{
    // removes from chain of interior NPs (not normal interaction-owned
    // chain of NPs)

	if (NULL != m_pNextInteriorNP)
	{
		// is not last in interior list
		m_pNextInteriorNP->m_pLastInteriorNP = m_pLastInteriorNP;
	}                                  
	else
	{
		// is last in interior list, so make one before new last (if exists)
		m_pStats->m_pLastMonMolNP = m_pLastInteriorNP;
		if (NULL != m_pLastInteriorNP)
		{
            // bug here, m_pLastInteriorNP non-NULL but already freed occasionally
			m_pLastInteriorNP->m_pNextInteriorNP = NULL;
		}
	}
	
	if (NULL != m_pLastInteriorNP)
	{
		// is not first in interior list
		m_pLastInteriorNP->m_pNextInteriorNP = m_pNextInteriorNP;
	}                                  
	else
	{
		// is first in interior list, so make next new first (if exists)
		m_pStats->m_pFirstMonMolNP = m_pNextInteriorNP;        
		if (NULL != m_pNextInteriorNP)
		{
			m_pNextInteriorNP->m_pLastInteriorNP = NULL;
		}
	}        

}

CMonitorMolDefaultInt::CMonitorMolDefaultInt()
{
	m_nBaseType1 = MIN(CI_MONITOR, CI_MOL);
	m_nBaseType2 = MAX(CI_MONITOR, CI_MOL);
    
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
	m_sName = "mon_mol_default";
	m_sDescription = "Molecules are measured and controlled by monitor"
	                 " as specified by the monitor properties";

	m_nIsHard = TRUE;
	m_nIsSoft = FALSE;

#ifdef _DEBUG
	m_lNumNP = 0;
#endif
}      	                                                                

CMonitorMolDefaultInt::~CMonitorMolDefaultInt()
{
    // This interaction has to tell any CSObjMonitor objects that it
    // references that it is about to die, because they may own some
    // CMonMolNPs in their stats lists that are not in the usual linked
    // list of NPs belonging to this interaction. They must be marked
    // has having no parent, else they will try and access this
    // interaction when they are deleted later, causing an exception.

    // Note: if this is just an instance of the interaction in the
    // gallery of examples owned by the sim, then the object types
    // will be NULL and we give up.
    // If it is an instance created purely for editing purposes
    // by the interactions dialogue, then it will own no CSObj
    // instances and nothing bad will happen.


#if (CI_MONITOR < CI_MOL)
	CObjTypeMonitor* pMonitorType = (CObjTypeMonitor*) m_pType1;
	CObjTypeMol* pMolType = (CObjTypeMol*) m_pType2;
#else
	CObjTypeMonitor* pMonitorType = (CObjTypeMonitor*) m_pType2;
	CObjTypeMol* pMolType = (CObjTypeMol*) m_pType1;
#endif                                                    
                                                    
    if ((NULL == pMonitorType) || (NULL == pMolType))
    {
        // this interaction isn't referencing any object types, probably
        // just instantiated as example or for editing purposes etc
        return;
    }

	CSObjMonitor* pMonitor = (CSObjMonitor*) pMonitorType->m_pFirstSObj;
    
    // get index into m_molStatsList for this CSObjMonitor
    int nStatIndex = pMonitorType->FindStatIndex(pMolType);

    if (SR_NO_SUCH_TYPE == nStatIndex)
    {
        // mol type maybe created after last build of stats list
        return;
    }

	// loop over all monitors of this type		
	while (NULL != pMonitor)
	{   
		// get pointer to mol stats entry for this mol type in mon:
		pMonitor->NoteIntDying(nStatIndex);

        pMonitor = (CSObjMonitor*) pMonitor->m_pNextSObj;
    }

}

CInt* CMonitorMolDefaultInt::Clone()
{
	CInt* pInt = new CMonitorMolDefaultInt;
	pInt->m_pSim = m_pSim;
	return pInt;

}                         
                 
void CMonitorMolDefaultInt::BuildNeighList()
{                              
	// We are required to rebuild the list of pairs of objects that may interact
	// soon.  
	
	// We recycle old neighbour pairs as much as possible to avoid the overhead
	// of creating new ones. NB Not sure yet if the list of neighbour pairs needs
	// to be singly or doubly linked.
	// Snip the list of existing neighbour pair objects off and glue it onto
	// the spare list for reuse:    
	
	// For this monitor-molecule interacion, really have two lists to
	// prepare; one belonging to us to keep tabs of molecules near monitor
	// edge that may arrive or leave, and another belonging to the monitor
	// listing those actually within its interior. Both must be prepared here.     
	
	// Debugged list creation and cursory look at recycling 14/8/97
    
    
	CMonMolNP* pNP; 
	
#ifdef _DEBUG
	global_debug_counter++;
#endif
	
	pNP = (CMonMolNP*) m_pFirstSpareNP;     // remember beginning of old spare list

    if (NULL != m_pFirstNP)
	{
		m_pFirstSpareNP = m_pFirstNP; // put existing NP list in place of spares
		m_pLastNP->m_pNextNP = pNP;   // tack old spare list on end
	}
	m_pFirstNP = NULL;  // NP list is now empty
	m_pLastNP = NULL;            
	
	
	CMonMolNP* pLastNP = NULL;
	CMonMolNP* pLastInteriorNP;      
	
	
	// For this heterogeneous interaction, need to go through all combinations of
	// pairs of the type of monitor and molecule it belongs to checking for those
	// lying within the safety distance.
	
#if (CI_MONITOR < CI_MOL)
	CObjTypeMonitor* pMonitorType = (CObjTypeMonitor*) m_pType1;
	CObjTypeMol* pMolType = (CObjTypeMol*) m_pType2;
#else
	CObjTypeMonitor* pMonitorType = (CObjTypeMonitor*) m_pType2;
	CObjTypeMol* pMolType = (CObjTypeMol*) m_pType1;
#endif                                                    

	CSObjMol* pMol;
	CSObjMonitor* pMonitor = (CSObjMonitor*) pMonitorType->m_pFirstSObj;
    
    // get index into m_molStatsList for this CSObjMonitor
    int nStatIndex = pMonitorType->FindStatIndex(pMolType);
	
	// loop over all possible pairs of mols and monitors of this type		
	while (NULL != pMonitor)
	{   
		// get pointer to mol stats entry for this mol type in mon:
		
		CMolTypeStats* pStats = (CMolTypeStats*) pMonitor->m_molStatsList[nStatIndex];  

		pStats->m_lNumMolsInside = 0; // we will be counting them here
		
		// recycle NPs in this mol type stats list in this monitor
		// that were there to track mon-interior mols; though some
		// may already be back in spares list if also belonged to int.
		// Traverse list of NPs connected to stats list, and any that
		// have NULL pointers for normal NP links will have been missed
		// out of recycling effort as they did not belong to neighbour
		// list but only interior list.
        // At the same time, reset pointers so that interior chain
        // reduced to a heap of separate "links". Otherwise if try and
        // delete an NP later that is not part of interior list, will try
        // and fiddle with outdated link to old neighbour in interior list.
		
		CMonMolNP* pStatsNP = pStats->m_pFirstMonMolNP;
		

		while (NULL != pStatsNP)
		{
            CMonMolNP* pJustDone;

            // mark no longer in interior list, as all will be recycled and don't
            // want it to think still in interior list if subsequently deleted from
            // spare list
            pStatsNP->m_nFlags &= ~MNML_INTERIOR;

			if (!(pStatsNP->m_nFlags & MNML_PROXIMAL))
			{
				// This NP was only in interior list, needs recycling now,
				// would have already been recycled if in neighbour list
				

				ASSERT(NULL == pStatsNP->m_pNextNP);
				ASSERT(NULL == pStatsNP->m_pLastNP);
				
				pStatsNP->m_pNextNP = m_pFirstSpareNP;
				m_pFirstSpareNP = pStatsNP;
			}
			
            pStatsNP->m_pLastInteriorNP = NULL; // break off chain
            pJustDone = pStatsNP;
			pStatsNP = pStatsNP->m_pNextInteriorNP;
            pJustDone->m_pNextInteriorNP = NULL; // break off chain
		}

		pLastInteriorNP = NULL; // needed to make new linked list of interior mols
		pStats->m_pFirstMonMolNP = NULL;
		pStats->m_pLastMonMolNP = NULL;
				
		// loop over mols of this type
		             
		pMol = (CSObjMol*) pMolType->m_pFirstSObj;   
		while (NULL != pMol)
		{
		                      
#ifdef _DEBUG
			global_debug_counter++;
#endif  
            
            int nClose;     // whether mol near to edge of monitor (and which if so)
            int nContained; // whether mol actually within monitor 
            
            // get local copies of frequently accessed quantities
            
            double xl = pMonitor->m_dXLeft;
            double xr = pMonitor->m_dXRight;
            double yt = pMonitor->m_dYTop;
            double yb = pMonitor->m_dYBottom;
            
            double x = pMol->m_dX;
            double y = pMol->m_dY; 
            
            double s = m_pSim->m_dSafetyDist;
            
            nContained = ((x > xl) && (x < xr) && (y < yt) && (y > yb));
                          
            // to be close to an edge, must be near top or bottom
            // and somewhere between left and right (plus margins)
            // or near left or right and somewhere between top and bottom;
            // want to remember which edges close to, so that need only
            // check those edges when running. Try and determine which
            // edges close to (if any) in minimum no. of comparisons:
            
            int nLeftOfRightOuterMargin  = (x < xr + s);
            int nRightOfRightInnerMargin = (x > xr - s);
            int nRightOfLeftOuterMargin  = (x > xl - s);
            int nLeftOfLeftInnerMargin   = (x < xl + s); 
            int nBelowTopOuterMargin     = (y < yt + s);
            int nAboveTopInnerMargin     = (y > yt - s);
            int nAboveBottomOuterMargin  = (y > yb - s);
            int nBelowBottomInnerMargin  = (y < yb + s);
            
            nClose = 0;
            
            if (nRightOfLeftOuterMargin && nLeftOfRightOuterMargin)
            {
            	if (nBelowTopOuterMargin && nAboveTopInnerMargin)
            	{
            		nClose |= MNML_TOP;
            	}
            	
            	if (nBelowBottomInnerMargin && nAboveBottomOuterMargin)
            	{
            		nClose |= MNML_BOTTOM;
            	}
            }  
            
            if (nAboveBottomOuterMargin && nBelowTopOuterMargin)
            {
            	if (nRightOfLeftOuterMargin && nLeftOfLeftInnerMargin)
            	{
            		nClose |= MNML_LEFT;
            	}
            	
            	if (nLeftOfRightOuterMargin && nRightOfRightInnerMargin)
            	{
            		nClose |= MNML_RIGHT;
            	}
            } 
            
           
			if (nClose || nContained)
			{
				// This mol is either inside the monitor or close to one
				// of its edges, so we need to set up a neighbour pair.    
				
				if (NULL != m_pFirstSpareNP)     
				{
					// reuse an old neighbour pair object
					pNP = (CMonMolNP*) m_pFirstSpareNP;
					m_pFirstSpareNP = pNP->m_pNextNP;
					
					ASSERT(NR_NOT_IN_TREE == pNP->m_nParentRelationship);
				}
				else
				{
					// have to make a new one
					pNP = new CMonMolNP((CInt*) this);
					pNP->m_handle1.m_pNP = pNP;
					pNP->m_handle2.m_pNP = pNP;
				}   

				pNP->m_pEarlierNode = NULL;
				pNP->m_pLaterNode = NULL;   
				pNP->m_pParentNode = NULL;
				pNP->m_nParentRelationship = NR_NOT_IN_TREE;
				
				pNP->m_pNextNP = NULL;
				pNP->m_pLastNP = NULL;
				pNP->m_pNextInteriorNP = NULL;
				pNP->m_pLastInteriorNP = NULL;
				pNP->m_nProximityHint = nClose;
				pNP->m_nFlags = 0;
				pNP->m_pStats = pStats;		
				
#if (CI_MONITOR < CI_MOL)
				pNP->m_pSObj1 = pMonitor;
				pNP->m_pSObj2 = pMol;
#else				
				pNP->m_pSObj2 = pMonitor;
				pNP->m_pSObj1 = pMol;
#endif
            }

            
			// depending whether within monitor, close to an edge,
			// or both, need to put that NP into the usual CInt list
			// of near neighbours and/or the CSObjMonitor's list of
			// mols of this type it contains
			
			if (nClose)   
			{             
		   		// add this NP to linked list attached to interaction
				
#if (CI_MONITOR < CI_MOL)
				pNP->m_handle2.m_pNextHandle = pMol->m_pFirstHandle;
				pMol->m_pFirstHandle = &(pNP->m_handle2);
#else				
				pNP->m_handle1.m_pNextHandle = pMol->m_pFirstHandle;
				pMol->m_pFirstHandle = &(pNP->m_handle1);	
#endif
            
            	// link on to list belonging to int of near-neighbours
            	
            	if (pLastNP != NULL)
				{
					pLastNP->m_pNextNP = pNP;
					pNP->m_pLastNP = pLastNP;
				}
				else
				{
					m_pFirstNP = pNP;  
					pNP->m_pLastNP = NULL;
				}

				pLastNP = pNP;    
				
				pNP->m_nFlags |= MNML_PROXIMAL;      
            }
            
            if (nContained)
            {
            	// Add this NP to the linked list belonging to the
            	// CMolTypeStats object for this mol type belonging
            	// to the CSObjMonitor    
            	
            	if (pLastInteriorNP != NULL)
				{
					pLastInteriorNP->m_pNextInteriorNP = pNP;
					pNP->m_pLastInteriorNP = pLastInteriorNP;
				}
				else
				{
					pStats->m_pFirstMonMolNP = pNP;  
					pNP->m_pLastInteriorNP = NULL;
				}

				pLastInteriorNP = pNP;  
				
				pNP->m_nFlags |= MNML_INTERIOR;       
				
				pStats->m_lNumMolsInside++;
            }

            
			pMol = (CSObjMol*) pMol->m_pNextSObj;
		} // loop over molecules                     
		
		// terminate linked list belonging to stats entry of this monitor
		// for this mol type of interior mols
		
		pStats->m_pLastMonMolNP = pLastInteriorNP;
		if (NULL != pLastInteriorNP)
		{ 
			pLastInteriorNP->m_pNextInteriorNP = NULL;     
		}
		
		pStats->m_dNumMolsInside = (double) pStats->m_lNumMolsInside;
		
		pMonitor = (CSObjMonitor*) pMonitor->m_pNextSObj;
	} // loop over monitors 
	                          
	                          
	// terminate linked list belonging to int of near-neighbours
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

void CMonitorMolDefaultInt::CalcAllHardEvents()
{
#ifdef _DEBUG
	global_debug_counter++;
#endif

    
	// Go through all neighbour pairs and calculate when each 
	// pair of objects will collide, if at all, within this timestep.

	CMonMolNP* pNP = (CMonMolNP*) m_pFirstNP; 
		
	while (NULL != pNP)
	{ 
		CalcHardEvent(pNP);		     						

		pNP = (CMonMolNP*) pNP->m_pNextNP;
	}

}

void CMonitorMolDefaultInt::CalcHardEvent(CNP* pNP)
{            

#ifdef _DEBUG
	global_debug_counter++;
#endif
	

#if (CI_MONITOR < CI_MOL)
	CObjTypeMonitor* pMonitorType = (CObjTypeMonitor*) m_pType1;
	CObjTypeMol* pMolType = (CObjTypeMol*) m_pType2; 
	CSObjMonitor* pMonitor = (CSObjMonitor*) pNP->m_pSObj1;   
	CSObjMol* pMol = (CSObjMol*) pNP->m_pSObj2;
#else
	CObjTypeMonitor* pMonitorType = (CObjTypeMonitor*) m_pType2;
	CObjTypeMol* pMolType = (CObjTypeMol*) m_pType1;
	CSObjMonitor* pMonitor = (CSObjMonitor*) pNP->m_pSObj2;   
	CSObjMol* pMol = (CSObjMol*) pNP->m_pSObj1;
#endif                                                    
    
    // get local copies of useful quantities
    double x = pMol->m_dX;
    double y = pMol->m_dY;
    double vx = pMol->m_dVx;
    double vy = pMol->m_dVy;
    double xl = pMonitor->m_dXLeft;
    double xr = pMonitor->m_dXRight;
    double yt = pMonitor->m_dYTop;
    double yb = pMonitor->m_dYBottom;
    
	// Mol arrives or leaves monitor when its centre coincides with an
	// edge exactly. We test only for those monitor edges specified
	// by the hint data, though the molecule may be at risk of passing
	// through several monitor edges. 
	
	// First check for future overlap with top or bottom edges; will
	// never hit either if has no vertical speed component
	
	int nHitType = 0;  // doesn't hit anything
	double dHitTime = NEARLY_INFINITY; // unless we find anything sooner
	
	if ((vy > NEARLY_ZERO) || (vy < -NEARLY_ZERO))
	{
		if (pNP->m_nProximityHint & MNML_TOP)
		{             
			// get time at which passes through line defining top of monitor
			double dTTop = (yt - y) / vy;    
			
			// see if earlier than earliest hit expected so far, if
			// not don't bother checking whether within line segment
			if ((dTTop > 0) && (dTTop < dHitTime))
			{
				// will pass through top line earlier than earliest
				// collision found so far, so check if with length of
				// monitor line segment
			
				double xnew = x + vx * dTTop;
			
				if ((xnew <= xr) && (xnew >= xl))
				{                                                     
					// will hit this edge, and earlier than the previous
					// earliest collision, so note it
					dHitTime = dTTop;
					nHitType = MNML_TOP;
				}
			}
		}
			
		if (pNP->m_nProximityHint & MNML_BOTTOM)
		{
			double dTBottom = (yb - y) / vy;    
			
			if ((dTBottom > 0) && (dTBottom < dHitTime))
			{
				double xnew = x + vx * dTBottom;
			
				if ((xnew <= xr) && (xnew >= xl))
				{                                                     
					dHitTime = dTBottom;
					nHitType = MNML_BOTTOM;
				} 
			}
		}
	}

	// similarly for left and right walls of monitor
	if ((vx > NEARLY_ZERO) || (vx < -NEARLY_ZERO))
	{		
		if (pNP->m_nProximityHint & MNML_LEFT)
		{
			double dTLeft = (xl - x) / vx;    
			
			if ((dTLeft > 0) && (dTLeft < dHitTime))
			{
				double ynew = y + vy * dTLeft;
			
				if ((ynew <= yt) && (ynew >= yb))
				{                                                     
					dHitTime = dTLeft;
					nHitType = MNML_LEFT;
				} 
			}
		}
		
		if (pNP->m_nProximityHint & MNML_RIGHT)
		{
			double dTRight = (xr - x) / vx;    
			
			if ((dTRight > 0) && (dTRight < dHitTime))
			{
				double ynew = y + vy * dTRight;
			
				if ((ynew <= yt) && (ynew >= yb))
				{                                                     
					dHitTime = dTRight;
					nHitType = MNML_RIGHT;
				} 
			}
		}
	}    
	
	if (nHitType)
	{		
		if (pMol->m_nTouched)
		{
			dHitTime += pMol->m_dTimeDoneAlready;
		}
				
		if ((dHitTime > m_pSim->m_dTimeSoFar) && (dHitTime <= m_pSim->m_dDeltaTime))
		{
			// due to hit monitor boundary in this timestep
			pNP->m_dTimeDue = dHitTime; 
			pNP->m_nEventHint = nHitType;
				  
			m_pSim->InsertNode(pNP);
				 
			ASSERT(NR_NOT_IN_TREE != pNP->m_nParentRelationship);
		}
	}

}

void CMonitorMolDefaultInt::ExecHardEvent(CNP* pNP)
{   
	// Execute the event indicated by the neighbour pair object and
	// recalc all affected events 
	
	// For the monitor-mol interaction, just involves updating records
	// of whether molecule in monitor or not, and sorting out
	// linked lists accordingly.  

#if (CI_MONITOR < CI_MOL)
	CObjTypeMonitor* pMonitorType = (CObjTypeMonitor*) m_pType1;
	CObjTypeMol* pMolType = (CObjTypeMol*) m_pType2; 
	CSObjMonitor* pMonitor = (CSObjMonitor*) pNP->m_pSObj1;   
	CSObjMol* pMol = (CSObjMol*) pNP->m_pSObj2;
#else
	CObjTypeMonitor* pMonitorType = (CObjTypeMonitor*) m_pType2;
	CObjTypeMol* pMolType = (CObjTypeMol*) m_pType1;
	CSObjMonitor* pMonitor = (CSObjMonitor*) pNP->m_pSObj2;   
	CSObjMol* pMol = (CSObjMol*) pNP->m_pSObj1;
#endif                                                    

	CMonMolNP* pMonMolNP = (CMonMolNP*) pNP;  // cast to right type	for convenience     
	CMolTypeStats* pStats = pMonMolNP->m_pStats; // used a lot here, so save some dereferencing
	
	if (MCNTRL_DESTROY == pMonitorType->m_nControlType)
	{
		// mark this mol for destruction at end of timestep (otherwise complicated
		// because of other hard events pending, though could make more rigorous later
		// by doing deletion as soon as touches monitor)
		
		pMonitor->m_destroyList.AddTail(pMol);
	}
	else if (pMonMolNP->m_nFlags & MNML_INTERIOR)
	{
		// is inside monitor but leaving, so must remove itself from interior list

        pMonMolNP->Unlink();
		
		pStats->m_lNumMolsInside--;       
		
		if (pStats->m_lNumMolsInside < 0)
		{
			ASSERT(FALSE);
			pStats->m_lNumMolsInside = 0;
		}
		
		pMonMolNP->m_nFlags &= ~MNML_INTERIOR;  
		
		// increment appropriate counter of departing molecules
		switch (pMonMolNP->m_nEventHint)
		{ 
			case MNML_TOP:
				pStats->m_lCountExitsTop++;
				break;
				
			case MNML_BOTTOM:
				pStats->m_lCountExitsBottom++;
				break;
				
			case MNML_LEFT: 
				pStats->m_lCountExitsLeft++;
				break;
				
			case MNML_RIGHT:
				pStats->m_lCountExitsRight++;
				break;
				
			default:
				ASSERT(FALSE);
				break;
		}
	}
	else
	{                                                                                 
		// is outside monitor but entering, so add to linked list maintained by stats object 
		if (NULL == pStats->m_pLastMonMolNP)
		{
			// stats list was empty
			ASSERT(0 == pStats->m_lNumMolsInside);
			
			pStats->m_pFirstMonMolNP = pMonMolNP;
			pStats->m_pLastMonMolNP = pMonMolNP;
		}  
		else
		{
			// add to end of existing list
			ASSERT(pStats->m_lNumMolsInside > 0);
			
			CMonMolNP* pOldLast = pStats->m_pLastMonMolNP;
			pOldLast->m_pNextInteriorNP = pMonMolNP;
			pMonMolNP->m_pLastInteriorNP = pOldLast;  
			pMonMolNP->m_pNextInteriorNP = NULL;
			pStats->m_pLastMonMolNP = pMonMolNP;
		}
		
		pStats->m_lNumMolsInside++;       
		
		pMonMolNP->m_nFlags |= MNML_INTERIOR; 

		// decrement appropriate counter of departing molecules
		switch (pMonMolNP->m_nEventHint)
		{ 
			case MNML_TOP:
				pStats->m_lCountExitsTop--;
				break;
				
			case MNML_BOTTOM:
				pStats->m_lCountExitsBottom--;
				break;
				
			case MNML_LEFT: 
				pStats->m_lCountExitsLeft--;
				break;
				
			case MNML_RIGHT:
				pStats->m_lCountExitsRight--;
				break;
				
			default:
				ASSERT(FALSE);
				break;
		}
	}           
}


