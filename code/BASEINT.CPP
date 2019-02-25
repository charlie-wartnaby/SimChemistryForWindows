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
#include "lennard.h"  
#include "lendoc.h"         

#include <stdlib.h>                                   
#include <limits.h>     
#include <time.h>                       
#include <math.h>  


// limits.h doesn't seem to have these, in debug anyway:
#ifndef INT_MIN
#define INT_MIN (-32768)
#define INT_MAX 32767
#endif


#ifdef _DEBUG
extern long global_debug_counter;
#endif


CListHandle::CListHandle()
{
	m_pNextHandle = NULL;
	m_pNP = NULL;
}

CNP::CNP(CInt* pOwningInt)
{
	m_pSObj1 = NULL;
	m_pSObj2 = NULL;
	
	m_pLastNP = NULL;
	m_pNextNP = NULL; 
	
	m_handle1.m_pNP = this;
	m_handle2.m_pNP = this;
	
	m_dTimeDue = 0;
	
    m_nProximityHint = 0;
                           
	m_nEventHint = 0;
	m_pInt = NULL;  
	
	m_pParentNode = NULL;
	m_pEarlierNode = NULL;
	CNP* m_pLaterNode = NULL;  
	int m_nParentRelationship = NR_NOT_IN_TREE;

    m_pInt = pOwningInt;
}

CNP::CNP()
{
	m_pSObj1 = NULL;
	m_pSObj2 = NULL;
	
	m_pLastNP = NULL;
	m_pNextNP = NULL; 
	
	m_handle1.m_pNP = this;
	m_handle2.m_pNP = this;
	
	m_dTimeDue = 0;
	
    m_nProximityHint = 0;
                           
	m_nEventHint = 0;
	m_pInt = NULL;  
	
	m_pParentNode = NULL;
	m_pEarlierNode = NULL;
	CNP* m_pLaterNode = NULL;  
	int m_nParentRelationship = NR_NOT_IN_TREE;
}

CNP::~CNP()
{ 
	// should unlink itself tidily from list of near-neighbours
	// maintained by int when destroyed, to allow for molecules to
	// be destroyed without having to rebuild all lists from scratch
    // Complication: if it is derived type CMonMolNP being deleted, 
    // could be deleting instances owned by CSObjMonitors whose parent
    // interactions have already been destroyed. Must not in that
    // case try and access parent interaction. Hence test for
    // parent interaction pointer being non-NULL (will be set NULL
    // to warn is destroyed). In that case, no normal NP chain maintained
    // by NP any more, so don't try and unlink from it.

    if (NULL == m_pInt)
    {
        return;
    }

    // If we get here, parent interaction exists and we should unlink
    // neatly from its list of NPs.
	if (NULL != m_pNextNP)
	{
		// is not last in list
		m_pNextNP->m_pLastNP = m_pLastNP;
	}                                  
	else
	{
		// is last in list, so make one before new last (if exists)
        // Bug: m_pInt sometimes non-NULL but cannot be legally dereferenced,
        // when in ~CMonMolNP from ~CMolTypeStats
		m_pInt->m_pLastNP = m_pLastNP;
		if (NULL != m_pLastNP)
		{
			m_pLastNP->m_pNextNP = NULL;
		}
	}
	
	if (NULL != m_pLastNP)
	{
		// is not first in list
		m_pLastNP->m_pNextNP = m_pNextNP;
	}                                  
	else
	{
		// is first in list, so make next new first (if exists)
		m_pInt->m_pFirstNP = m_pNextNP;        
		if (NULL != m_pNextNP)
		{
			m_pNextNP->m_pLastNP = NULL;
		}
	}        
}

///////////////////////////////////////////////////////////////////////////
////// Interactions //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

///////////////
// Generic CInt

// Command list
CMD_ENTRY CInt::m_celCIntCommands[MAX_COMMANDS_PER_OBJ] = 
{                                        
	{"objtype1",           CA_STRING, CT_STATE,   CI_OBJTYPE1, LIM_MIN_CHARS_OBJTYPE, LIM_MAX_CHARS_OBJTYPE   },
	{"objtype2",           CA_STRING, CT_STATE,   CI_OBJTYPE2, LIM_MIN_CHARS_OBJTYPE, LIM_MAX_CHARS_OBJTYPE   },
	{"end",                CA_NONE,   CT_ENDLIST, CI_ENDLIST,  0, 0   }
};

CInt::~CInt()
{ 
	// remove all neighbour pairs
	
	CNP* pNP = m_pFirstNP;
	CNP* pLastNP;

    // first delete doubly-linked list of active CNPs:
    while (NULL != pNP)
	{
		pLastNP = pNP;
		pNP = pNP->m_pNextNP;
        
        // no need for it to unlink itself nicely; setting pointer back
        // to this interaction NULL will get NP not to bother doing
        // unlinking
        pLastNP->m_pInt = NULL; 
		delete pLastNP;
	}

	pNP = m_pFirstSpareNP;
	while (NULL != pNP)
	{
		pLastNP = pNP;
		pNP = pNP->m_pNextNP;

        pLastNP->m_pInt = NULL; // no need for it to unlink itself nicely

		delete pLastNP;
	}
	
}

void CInt::ContributeNumerics()
{
	// none by default
}

int CInt::CheckNotReferencingDeadTypes()
{
    // Interactions normally reference only those object types involved
    // directly in the interaction, so nothing to do. Reactive mol-mol
    // interaction is an exception.

    return SR_OK;
}

int CInt::AcceptCommandLine(CString& remainingLine, CString& errString)
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
		// get the types in correct order if same base types
		if (m_nBaseType1 == m_nBaseType2)
		{
			int type1 = m_pSim->GetObjTypeIndexByName(m_pType1->m_sName);
			int type2 = m_pSim->GetObjTypeIndexByName(m_pType2->m_sName);
			
			if (type2 < type1)
			{
				CObjType* pSpareType = m_pType1;
				m_pType1 = m_pType2;
				m_pType2 = pSpareType;
			}
		}
	}
	  
	return ret;
}

CMD_ENTRY* CInt::GetCmdList()
{       
	return &(m_celCIntCommands[0]);
}

int CInt::GetStateParam(int command)
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
			
		default:        
			TRACE("Asked for param for unknown command in CInt::GetStateParam\n");
			ret = SR_ERROR;
			break;
	}             
	
	return ret;
}

void CInt::BuildNeighList()
{
	// Shouldn't get called; only types that do anything should, and they
	// should override this.     
	ASSERT(FALSE);
}          

void CInt::CalcForces()
{
	// Shouldn't get called; only types that do anything should, and they
	// should override this.     
	ASSERT(FALSE);
}          

void CInt::CalcAllHardEvents()
{
	// Shouldn't get called; only types that do anything should, and they
	// should override this.     
	ASSERT(FALSE);
}          

void CInt::CalcHardEvent(CNP* pNP)
{
	// Shouldn't get called; only types that do anything should, and they
	// should override this.     
	ASSERT(FALSE);
}          

void CInt::ExecHardEvent(CNP* pNP)
{
	// Shouldn't get called; only types that do anything should, and they
	// should override this.     
	ASSERT(FALSE);
}  
        
int CInt::EditDetails()
{                     
	// Shouldn't get called; only types that do anything should, and they
	// should override this.     
	ASSERT(FALSE);  
	
	return SR_ERROR;
}



   