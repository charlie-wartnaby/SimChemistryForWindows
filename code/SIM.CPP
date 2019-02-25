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
#include "lennard.h"
#include "simobjs.h"
#include "sim.h"
#include "tracker.h"
#include "mol.h"
#include "wall.h"   
#include "piston.h" 
#include "textbox.h"
#include "monitor.h"  
#include "graphx.h"  
#include "slider.h"      
#include "baseint.h"  
#include "nullint.h"
#include "mmhrdint.h"
#include "mmljint.h"
#include "wmdefint.h"  
#include "mphrdint.h"
#include "MolMolReactiveInt.h"  
#include "mnmldint.h"
#include "lendoc.h" 
#include "griddlg.h"      
#include "ResolveMultipleSelnDlg.h"

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

double global_debug_sim_time;

//////////////////////////////////////////////////////////////////////                                 
// CSim functionality                        
//////////////////////////////////////////////////////////////////////                                 

// Command list
CMD_ENTRY CSim::m_celCSimCommands[SIM_NUM_COMMANDS] = 
{                                                   
	{"mol",           CA_STRING, CT_OBJECT,  CI_MOL,           LIM_MIN_CHARS_OBJTYPE, LIM_MAX_CHARS_OBJTYPE},
	{"wall",          CA_STRING, CT_OBJECT,  CI_WALL,          LIM_MIN_CHARS_OBJTYPE, LIM_MAX_CHARS_OBJTYPE},
	{"piston",        CA_STRING, CT_OBJECT,  CI_PISTON,        LIM_MIN_CHARS_OBJTYPE, LIM_MAX_CHARS_OBJTYPE}, 
	{"textbox",       CA_STRING, CT_OBJECT,  CI_TEXTBOX,       LIM_MIN_CHARS_OBJTYPE, LIM_MAX_CHARS_OBJTYPE},
	{"monitor",       CA_STRING, CT_OBJECT,  CI_MONITOR,       LIM_MIN_CHARS_OBJTYPE, LIM_MAX_CHARS_OBJTYPE},
	{"graph",         CA_STRING, CT_OBJECT,  CI_GRAPH,         LIM_MIN_CHARS_OBJTYPE, LIM_MAX_CHARS_OBJTYPE},
	{"slider",        CA_STRING, CT_OBJECT,  CI_SLIDER,        LIM_MIN_CHARS_OBJTYPE, LIM_MAX_CHARS_OBJTYPE},
	{"temperature",   CA_DOUBLE, CT_STATE,   CI_TEMPERATURE,   LIM_MIN_TEMP, LIM_MAX_TEMP},
	{"begin_history", CA_NONE,   CT_CONTROL, CI_BEGIN_HISTORY, 0, 0   },
	{"end_history",   CA_NONE,   CT_CONTROL, CI_END_HISTORY,   0, 0   },    
	{"begin_state",   CA_NONE,   CT_CONTROL, CI_BEGIN_STATE,   0, 0   },
	{"end_state",     CA_NONE,   CT_CONTROL, CI_END_STATE,     0, 0   },
	{"objtype",       CA_STRING, CT_CONTROL, CI_OBJTYPE,       LIM_MIN_CHARS_OBJTYPE_TYPE, LIM_MAX_CHARS_OBJTYPE_TYPE},
	{"fillmol",       CA_NONE,   CT_CONTROL, CI_FILLMOL,       0, 0   },
	{"run",           CA_NONE,   CT_CONTROL, CI_RUN,           0, 2e9 },
	{"stop",          CA_NONE,   CT_CONTROL, CI_STOP,          0, 0   },
	{"interaction",   CA_STRING, CT_CONTROL, CI_INTERACTION,   LIM_MIN_CHARS_INTERACTION, LIM_MAX_CHARS_INTERACTION},
	{"deltype",       CA_STRING, CT_CONTROL, CI_DELTYPE,       LIM_MIN_CHARS_OBJTYPE, LIM_MAX_CHARS_OBJTYPE },
	{"reset",         CA_NONE,   CT_CONTROL, CI_RESET,         0, 0   },
	{"delay",         CA_DOUBLE, CT_CONTROL, CI_DELAY,         0, 1000},
	{"gravity",       CA_DOUBLE, CT_STATE,   CI_GRAVITY,   -LIM_MAX_GRAVITY, LIM_MAX_GRAVITY},
	{"autorun",       CA_BYTE,   CT_STATE,   CI_AUTORUN,       0, 1, CE_DONT_RECORD_IN_HISTORY   },
	{"seln_reset",    CA_NONE,   CT_CONTROL, CI_SELN_RESET,    0, 0   },
	{"select",        CA_STRING, CT_CONTROL, CI_SELECT,        LIM_MIN_CHARS_OBJTYPE, LIM_MAX_CHARS_OBJTYPE},
	{"seln_delete",   CA_NONE,   CT_CONTROL, CI_SELN_DELETE,   0, 0   },
	{"pause",         CA_NONE,   CT_CONTROL, CI_PAUSE,         0, 0   },
	{"del_objs",      CA_STRING, CT_CONTROL, CI_DEL_OBJS,      LIM_MIN_CHARS_OBJTYPE, LIM_MAX_CHARS_OBJTYPE},
	{"del_all",       CA_NONE,   CT_CONTROL, CI_DEL_ALL,       0, 0   },
	{"id",            CA_LONG,   CT_CONTROL, CI_ID,            0, 2e9 },
	{"first_id",      CA_LONG,   CT_CONTROL, CI_FIRST_ID,      0, 2e9 },
	{"last_id",       CA_LONG,   CT_CONTROL, CI_LAST_ID,       0, 2e9 },
	{"mon_uid",       CA_LONG,   CT_CONTROL, CI_MON_UID,       1, 2000},
	{"change",        CA_NONE,   CT_CONTROL, CI_CHANGE,        0, 0   },
	{"name",          CA_STRING, CT_CONTROL, CI_NAME,          LIM_MIN_CHARS_OBJTYPE, LIM_MAX_CHARS_OBJTYPE}, 
	{"seln_move",     CA_NONE,   CT_CONTROL, CI_SELN_MOVE,     0, 0   }, 
	{"x",             CA_DOUBLE, CT_CONTROL, CI_X,         -1000,  1000 },
	{"y",             CA_DOUBLE, CT_CONTROL, CI_Y,         -1000,  1000 },
	{"grid_on",       CA_BYTE,   CT_STATE,   CI_GRID_ON,       0, 1   },
	{"gridsize",      CA_DOUBLE, CT_STATE,   CI_GRIDSIZE,   0.01, 1   },
	{"set",           CA_STRING, CT_CONTROL, CI_SET,           LIM_MIN_CHARS_NUMERIC_NAME, LIM_MAX_CHARS_NUMERIC_NAME},
	{"clear_graph",   CA_STRING, CT_CONTROL, CI_CLEAR_GRAPH,   LIM_MIN_CHARS_OBJTYPE, LIM_MAX_CHARS_OBJTYPE},
	{"simchemistry_ver", CA_DOUBLE, CT_STATE, CI_SIMCHEMISTRY_VER, 0, 100, CE_DONT_RECORD_IN_HISTORY},
    {"sim_size",      CA_NONE,   CT_CONTROL,  CI_SIM_SIZE,      0, 0 , CE_DONT_RECORD_IN_HISTORY  },
    {"left",          CA_DOUBLE, CT_CONTROL, CI_LEFT,          LIM_MIN_X, LIM_MAX_SIM_LEFT},
    {"bottom",        CA_DOUBLE, CT_CONTROL, CI_BOTTOM,        LIM_MIN_Y, LIM_MAX_SIM_BOTTOM},
    {"width",         CA_DOUBLE, CT_CONTROL, CI_WIDTH,         LIM_MIN_SIM_WIDTH, LIM_MAX_SIM_WIDTH},
    {"height",        CA_DOUBLE, CT_CONTROL, CI_HEIGHT,        LIM_MIN_SIM_HEIGHT, LIM_MAX_SIM_HEIGHT},
    {"deltatime",          CA_DOUBLE, CT_STATE, CI_DELTATIME,    LIM_MIN_DELTA_TIME, LIM_MAX_DELTA_TIME},
    {"use_auto_deltatime", CA_DOUBLE, CT_STATE, CI_USE_AUTO_DELTATIME, 0, 1},
    {"avg_time_const",  CA_DOUBLE,  CT_STATE,  CI_AVERAGING_TC,  LIM_MIN_AVERAGE_TC, LIM_MAX_AVERAGE_TC},
    {"set_rand_seed", CA_LONG,   CT_CONTROL, CI_SET_RAND_SEED, 0, 2147483647 },
	{"end",           CA_NONE,   CT_ENDLIST, CI_ENDLIST,       0, 0   }
};

// Command handlers   

int CSim::CmdDelObjs(CString& errString)
{
	// Deletes all objects of named type but leaves type intact.
	
	int ret;
	
	if (SM_SIM_STOPPED == m_nSimState)
	{
		CObjType* pType = GetObjTypeByName(m_sArg);
		
		if (NULL != pType)
		{          
			pType->DelObjs();        
			
			m_nSimReady = FALSE;  // rebuild neighbour lists
			
			ret = SR_OK;
		}
		else
		{                  
			errString += "unknown type for del_obs: ";
			errString += m_sArg;
			errString += "; ";
			
			ret = SR_ERROR;
		}
	}
	else
	{
		errString += "del_objs only allowed with sim stopped; ";
		
		ret = SR_ERROR;
	}
	
	return ret;
}            

int CSim::CmdSet(CString& remainingLine, CString& errString)
{         
	int ret;
	
	CString sNumValue;
	
	PopWord(remainingLine, sNumValue);
	
	double dValue = atof(sNumValue);
	
	CNumeric* pNumeric = FetchNumeric(m_sArg);
	
	if (NULL == pNumeric)
	{
		ret = SR_ERROR;
		errString += "named quantity does not exist; ";
	}
	else if (pNumeric->m_nType & NUM_USERWRITE)
	{
		// is writeable
		
		if (dValue < pNumeric->m_dMin)
		{
			ret = SR_WARN;
			*(pNumeric->m_pdValue) = pNumeric->m_dMin;
			errString += "warning: clipped value to allowed min; ";
		}
		else if (dValue > pNumeric->m_dMax)
		{
			ret = SR_WARN;
			*(pNumeric->m_pdValue) = pNumeric->m_dMax;
			errString += "warning: clipped value to allowed max; ";
		}                                                          
		else
		{
			ret = SR_OK;                      
			*(pNumeric->m_pdValue) = dValue;
		}       
		
		m_dSafetyDistLeft = 0;  // kludge to force neighbour rebuild in case
		                        // interaction params altered etc
	}
	else
	{
		// not writeable
		
		ret = SR_ERROR;
		errString += "this quantity not writeable ";
	}
	
	return ret;
}

int CSim::CmdClearGraph(CString& remainingLine, CString& errString)
{         
	int ret;
	
	CString sNumValue;
	
	PopWord(remainingLine, sNumValue);
	
	long lId = atol(sNumValue);
	
	CObjType* pType = GetObjTypeByName(m_sArg);
	
	if ((NULL == pType) || (CI_GRAPH != pType->m_nBaseType))
	{
		ret = SR_ERROR;
		errString += "no such graph type; ";
	}
	else
	{
		CSObjGraph* pGraph = (CSObjGraph*) pType->FetchSObj(lId);
		
		if (NULL == pGraph)
		{               
			ret = SR_ERROR;
			errString += "no graph with this id; ";
		}
		else
		{
			pGraph->ClearData();
			ret = SR_OK;
		}	
	}
	
	return ret;
}

int CSim::CmdSimSize(CString& remainingLine, CString& errString)
{
	int nCmdCode = CI_EMPTY;
	int ret = SR_OK;
	
	double dLeft = 0, dBottom = 0, dWidth = 0, dHeight = 0;
	int nLeftFound = FALSE, nBottomFound = FALSE, nWidthFound = FALSE, nHeightFound = FALSE;
	
	do
	{
		ret |= GrabCommand(remainingLine, errString, nCmdCode);
		
		switch (nCmdCode)  
		{
			case CI_LEFT:
				dLeft = m_dArg;  
				nLeftFound = TRUE;
				break;
				
			case CI_BOTTOM:
				dBottom = m_dArg;  
				nBottomFound = TRUE;
				break;

			case CI_WIDTH:
				dWidth = m_dArg;  
				nWidthFound = TRUE;
				break;

			case CI_HEIGHT:
				dHeight = m_dArg;  
				nHeightFound = TRUE;
				break;
				
			case CI_EMPTY:
				break;
				
			default:
				errString += "expected: sim_size left <nm> bottom <nm> width <nm> height <nm>; ";
				ret |= SR_ERROR;
				break;
		}
		
	}
	while ((SR_ERROR != ret) && (CI_EMPTY != nCmdCode));
    
    if (SR_ERROR != ret)
    {
		if (!nLeftFound)
		{
			errString += "warning: kept existing left co-ord; ";
			ret |= SR_WARN;
			dLeft = m_dScreenLeft_nm;
		}
		
		if (!nBottomFound)
		{
			errString += "warning: kept existing bottom co-ord; ";
			ret |= SR_WARN;
			dBottom = m_dScreenBottom_nm;
		}
		if (!nWidthFound)
		{
			errString += "warning: kept existing width; ";
			ret |= SR_WARN;
			dWidth = m_dScreenWidth_nm;
		}
		if (!nHeightFound)
		{
			errString += "warning: kept existing height; ";
			ret |= SR_WARN;
			dHeight = m_dScreenHeight_nm;
		}

        RedisplayWithNewScaling(dLeft, dBottom, dWidth, dHeight);
	}     
	
	return ret;

}

int CSim::CmdDelAll(CString& errString)
{
	CObjType* pType;

	int ret;
	
	if (SM_SIM_STOPPED == m_nSimState)
	{
		for (int i = 0; i <= m_typeArray.GetUpperBound(); i++)
		{
			pType = (CObjType*) m_typeArray[i];
	
			pType->DelObjs();
		}
	
		m_nSimReady = FALSE;  // rebuild neighbour lists
	
		ret = SR_OK;	
	}
	else
	{
		errString += "del_all only allowed with sim stopped; ";
		
		ret = SR_ERROR;
	}                        
	
	return ret;
}

int CSim::CmdPause()
{
	// Pauses script replay until user presses key;
	// should have appropriate obvious message on screen.
	// Can be done interactively by user, or as prerecorded
	// part of a script. 
	// See also KillPauseIfPaused()    
	
	switch (m_nReplayState)
	{
		case SM_REPLAY_PLAYING: 
			m_nReplayState = SM_REPLAY_PAUSED_PLAY;
			break;

		case SM_REPLAY_FFWDING:
			// do not actually do pause if ffwding, but rather
			// do short delay
			
			CmdDelay(); // will use fixed time in ffwd mode
			
			break;

		case SM_REPLAY_STOPPED:  
			m_nReplayState = SM_REPLAY_PAUSED_STOP;
			break;

		case SM_REPLAY_PAUSED_PLAY:
		case SM_REPLAY_PAUSED_STOP:
			ASSERT(FALSE); // not supposed to be able to do this
			break;
		
		default:
			ASSERT(FALSE);
			break;
	}
	
	return SR_OK;
}

int CSim::CmdSelnDelete()
{
	CSObj* pSObj;
	
	while (!m_selectedObjects.IsEmpty())
	{
		pSObj = (CSObj*) m_selectedObjects.RemoveTail();
		pSObj->m_nSelected = FALSE;
		
		delete pSObj;
	}                 
	
	return SR_OK;
}

int CSim::CmdSelnMove(CString& remainingLine, CString& errString)
{
	// First need to get x and y displacements in order to know how much
	// to shift selected objects by
	
	int nCmdCode = CI_EMPTY;
	int ret = SR_OK;
	
	double dX = 0, dY = 0; // displacements of objects to be shifted
	int nXFound = FALSE, nYFound = FALSE;
	
	do
	{
		ret |= GrabCommand(remainingLine, errString, nCmdCode);
		
		switch (nCmdCode)  
		{
			case CI_X:
				dX = m_dArg;  
				nXFound = TRUE;
				break;
				
			case CI_Y:
				dY = m_dArg;   
				nYFound = TRUE;
				break; 
				
			case CI_EMPTY:
				break;
				
			default:
				errString += "expected: seln_move x <displacement> y <displacement>; ";
				ret |= SR_ERROR;
				break;
		}
		
	}
	while ((SR_ERROR != ret) && (CI_EMPTY != nCmdCode));
    
    if (SR_ERROR != ret)
    {
		if (!nXFound)
		{
			errString += "warning: inserted zero as missing x-displacement; ";
			ret |= SR_WARN;
			dX = 0;
		}
		
		if (!nYFound)
		{
			errString += "warning: inserted zero as missing y-displacement; ";
			ret |= SR_WARN;
			dY = 0;
		}
				             
		CSObj* pObj = NULL;
		POSITION pos;
		
		pos = m_selectedObjects.GetHeadPosition();
				
		while (NULL != pos)
		{   
			pObj = (CSObj*) m_selectedObjects.GetAt(pos);
		
			pObj->HandleMove(dX, dY);
				
			m_selectedObjects.GetNext(pos);
		}
	}     
	
	return ret;
}

int CSim::CmdSelnReset()
{   
	CSObj* pSObj;
	
	while (!m_selectedObjects.IsEmpty())
	{
		pSObj = (CSObj*) m_selectedObjects.RemoveTail();
		pSObj->DeleteTracker();
		pSObj->m_nSelected = FALSE;
	}
	
	return SR_OK;
}

int CSim::CmdSelect(CString& remainingLine, CString& errString)
{    
	// m_sArg should have object type name, and remainingLine should have
	// individual obj id to select
	
	CObjType* pType = GetObjTypeByName(m_sArg);	
	
	if (NULL == pType)
	{                                      
		errString += "select for unknown obj type attempted; ";
		return SR_ERROR;
	}
	else
	{
		CString sId;
		PopWord(remainingLine, sId);
		long lId = atol(sId);
		
		CSObj* pSObj = pType->m_pFirstSObj;
		int found = FALSE;
		
		while ((NULL != pSObj) && !found)
		{       
			if (pSObj->m_lId == lId)
			{
				found = TRUE; 
				
				// toggle selection
				if (!pSObj->m_nSelected)
				{
					pSObj->m_nSelected = TRUE;    
					m_selectedObjects.AddTail(pSObj);
					if (SM_SIM_RUNNING != m_nSimState)
					{
						pSObj->CreateTracker();
					}
				}
				else
				{
					Unselect(pSObj);
				}
			}   
			
			pSObj = pSObj->m_pNextSObj;
		}    
		
		if (found)
		{
			return SR_OK;
		}
		else
		{
			errString += "selected object id not found; ";
			return SR_ERROR;
		}	
	}
}

void CSim::Unselect(CSObj* pSObj)
{   
	pSObj->m_nSelected = FALSE;
	pSObj->DeleteTracker();  // may not be in use anyway
	      
	if (m_selectedObjects.IsEmpty())  // shouldn't have been called if so
	{
		ASSERT(FALSE);
	}
	else
	{
		int found = FALSE; 
		POSITION posn = m_selectedObjects.GetHeadPosition();
		
		CSObj* pListSObj = (CSObj*) m_selectedObjects.GetAt(posn); 	
		
		if (pListSObj == pSObj)
		{
			found = TRUE;
			m_selectedObjects.RemoveHead();
		}
		else
		{
			POSITION lastpos;
			
			do
			{
				lastpos = posn;
				
				pListSObj = (CSObj*) m_selectedObjects.GetNext(posn);
	
				if (pListSObj == pSObj)   
				{
					found = TRUE;
					m_selectedObjects.RemoveAt(lastpos);
				}
			}  
			while (!found && (NULL != posn));
		}
					
		ASSERT(found);
	}
}
	              
int CSim::CmdDelay()
{
	// m_dArg will contain the number of seconds to remain
	// doing whatever we are doing before looking at the
	// next command.
	
	switch (m_nReplayState)
	{
		case  SM_REPLAY_PLAYING: 
			// actually start delay
			
			// ns = 1e9 * sec / unitless
			m_dDelayTimer = 1e9 * m_dArg / m_dTimeRatio ;   
			m_dDelayTotal = m_dDelayTimer; 
			m_nDoingDelay = TRUE;
			break;
						
		case SM_REPLAY_FFWDING:
			// do short delay instead

			// ns = 1e9 * sec / unitless
			m_dDelayTimer = 1e9 * FFWD_DELAY / m_dTimeRatio ;   
			m_dDelayTotal = m_dDelayTimer; 
			m_nDoingDelay = TRUE;
			break;
	    
	    default:
	    	// do nothing, eg if immediate command for
	    	// delay when not replaying, don't actually do it 
	    	break;
	}
	
	return SR_OK;
}
              
int CSim::CmdReset()
{
	// delete all objects of all types and restore default
	// values for system parameters
	
	// delete all simulation objects 
	while (m_typeArray.GetUpperBound() > -1)
	{          
		CObjType* pType = (CObjType*) m_typeArray[0];   
		pType->Delete();
		delete pType;
	}   
	
	// delete lists of interactions by category
	DeleteIntCatLists();    
	
	// delete lists of object types by category
	DeleteTypeCatLists();
    
    // set default system params
    Initialise();
    
	m_pDoc->UpdateAllViews(NULL);
        
    
    return SR_OK;
}

int CSim::CmdDelType(CString& remainingLine, CString& errString)
{
	// ensure lists of ints rebuilt after this before use
    m_nSimReady = FALSE;
	
	// string argument is name of object type to delete
	
	CObjType* pType = GetObjTypeByName(m_sArg); 
	
	if (NULL != pType)
	{   
		pType->Delete();
		
		delete pType;

		m_pDoc->UpdateAllViews(NULL);        
        
        int ret = CheckIntsNotReferencingDeadTypes();

        if (SR_OK != ret)
        {
            errString += "interactions modified because deleted type was previously used; ";

            if (SM_REPLAY_STOPPED == m_nReplayState ||
                SM_REPLAY_PAUSED_STOP == m_nReplayState)
            {
                AfxMessageBox("interactions modified because deleted type was previously used");
            }
        }

        return ret;
	}
	else
	{
		errString += "type to delete doesn't exist; ";
		return SR_ERROR;
	}
}

int CSim::CheckIntsNotReferencingDeadTypes()
{
    int ret = SR_OK;

    for (int i = 0; i <= m_intArray.GetUpperBound(); i++)
    {
        CInt* pInt = (CInt*) m_intArray[i];
        ret |= pInt->CheckNotReferencingDeadTypes();
    }

    return ret;
}

int CSim::CmdInt(CString& remainingLine, CString& errString)
{   
	int ret;
	
	// string argument is name of interaction type
	CInt* pInt = GetIntByName(m_sArg);                
	
	if (NULL != pInt)
	{   
		// valid name of interaction type 
		pInt = pInt->Clone();
		
		ret = pInt->AcceptCommandLine(remainingLine, errString); 
		
		if (SR_ERROR != ret)
		{
			// all OK, so put this int in   
			int row = GetObjTypeIndexByName(pInt->m_pType1->m_sName);
			int col = GetObjTypeIndexByName(pInt->m_pType2->m_sName); 
			ASSERT((SR_NO_SUCH_TYPE != row) && (SR_NO_SUCH_TYPE != col));
			int i = m_intArray.GetIndex(row, col);
			CInt* pOldInt = (CInt*) m_intArray[i];
			delete pOldInt;
			m_intArray[i] = pInt;
			
			pInt->ContributeNumerics();
		}
		else
		{
			delete pInt;
		}
	}
	else
	{
		ret = SR_ERROR;
		errString += "no such interaction: " + m_sArg;
	}
    
    // ensure rebuild of int lists before use           
    m_nSimReady = FALSE;
                         
	return ret;                                    
}

int CSim::CmdTemperature()
{                                     
	int ret;
	
	m_dTemperature = m_dArg;
	ret = SR_OK;      
	TRACE("System temperature set to: %f\n", m_dTemperature);
	return ret;
}                        

int CSim::CmdBeginHistory()
{
	m_nCmdMode = SCM_HISTORY;
	return SR_OK;
}

int CSim::CmdEndHistory()
{
	m_nCmdMode = SCM_ACTIVE;
	return SR_OK;
}

int CSim::CmdBeginState()
{
	m_nCmdMode = SCM_STATE;
	return SR_OK;
}           

int CSim::CmdRun()
{
	TRACE("Initiating simulation run through 'run' command\n");
	m_nSimState = SM_SIM_RUNNING; 
	m_nSimReady = FALSE;
	
	// Destroy tracking rectangles belonging to any selected objects
		
	CSObj* pObj = NULL;
	POSITION pos;
	
	pos = m_selectedObjects.GetHeadPosition();
	
	while (NULL != pos)
	{
		pObj = (CSObj*) m_selectedObjects.GetAt(pos);
		pObj->DeleteTracker();
		
		m_selectedObjects.GetNext(pos);
	}

	return SR_OK;
}

void CSim::InvalidateAllHardEvents()
{
	CNP* pNP = m_pRootNode;
	
	while (pNP != NULL)
	{
		DetachNode(pNP);
		pNP = m_pRootNode;
	}
	
	SynchronizeToNow();   // all get to same point in time
}

int CSim::CmdStop()
{
	TRACE("Stopping simulation\n");
	
	if (m_nDoingHardEvents)
	{
		// user has requested stop in middle of calculations,
		// must neatly clear up hard event tree   
		
		TRACE(" -- during hard events, clearing up mess\n");
		
		InvalidateAllHardEvents();

		m_nDoingHardEvents = FALSE;
	}
	
	m_nSimState = SM_SIM_STOPPED; 
	m_nSimReady = FALSE;
	
	// Get any selected objects to display their tracking rectangles
	// from now on until sim started again
	
	CSObj* pObj = NULL;
	POSITION pos;
	
	pos = m_selectedObjects.GetHeadPosition();
	
	while (NULL != pos)
	{
		pObj = (CSObj*) m_selectedObjects.GetAt(pos);
		pObj->CreateTracker();
		m_selectedObjects.GetNext(pos);
	}
	
	return SR_OK;
}

int CSim::CmdEndState()
{
	m_nCmdMode = SCM_ACTIVE;
	return SR_OK;
}                     

int CSim::CmdChange(CString& remainingLine, CString& errString)
{             
	// Will be followed by obj type or object to be modified, eg
	// following Properties dialogue

	int command; 
	CMD_ENTRY* pCmdEntry = NULL; 
		
	int ret = GrabCommand(remainingLine, errString, command, &pCmdEntry); 

	if (command <= MAX_OBJ_CI)
	{
		// it is an instance of an object, call handler with TRUE
		// to indicate modification not creation
		ret |= HandleObj(command, remainingLine, errString, TRUE);
	}
	else
	{
		// handle misc commands individually
		switch (command)
		{ 
			case CI_OBJTYPE:                                       
				// TRUE here indicates we insist type already exists
				ret |= HandleObjType(remainingLine, errString, TRUE);
				break;   
						
			default:
				ret |= SR_ERROR;   
				errString += "confused near: " + remainingLine;
				break;                            
				
		}   // end of switch (command)  
		
	} // end of if (command <= MAX_OBJ_CI) 
	
	return ret;		
} 


int CSim::CmdObjType(CString& remainingLine, CString& errString)
{ 
	return HandleObjType(remainingLine, errString, FALSE);
}

int CSim::HandleObjType(CString& remainingLine, CString& errString,
                                    int nChangeExisting)
{
	// if nChangeExisting is TRUE, we should just be modifying existing type
	// but we do it by replacing with new anyway, and have to handle
	// "objtype wall ...." to modify existing type for backwards compatibility
	// with v 0.1, in which didn't do "change objtype wall..."
	
	int nBaseType;
    CString sTypeName;
    
	// Give GrabCommand eg "wall foo" so it finds command CI_WALL
	// as type/command and foo as dummy argument
	
	CString typeString = m_sArg + " foo";
	CString dummyErrString;
	
	// get base type of object we are going to create
	int ret = GrabCommand(typeString, dummyErrString, nBaseType); 
	
	// Does this already exist? Get name
	if (FindGrabCommand(remainingLine, CI_NAME))
	{
		sTypeName = m_sArg;
	}	
	else
	{
		ret = SR_ERROR; // can't create or modify obj type with no name   
		errString += "no objtype name specified; ";
	}

	// Scan existing types to see if already exists
	
	CObjType* pType = NULL;
	
	int nExistingFound = FALSE;
	
    if (SR_ERROR != ret)
	{
		for (int i = 0; i <= m_typeArray.GetUpperBound(); i++)
		{
			pType = (CObjType*) m_typeArray[i];
			if ((pType->m_nBaseType == nBaseType)
		         && (pType->m_sName == sTypeName))
		    {
		    	// This type already exists, just want to modify it
		    	nExistingFound = TRUE;
				break;
			}       
		}
	}

	// If supposed to be modifying existing type, it is now an error
	// not to have found it. Otherwise make a new type

	if (nChangeExisting && !nExistingFound) ret = SR_ERROR;
	
	if (nExistingFound)
	{
		// we must after all be changing existing type, though syntax used
		// was old-style "objtype thing existing_type_name property..."
		
		nChangeExisting = TRUE;
	}
				
	if (SR_ERROR != ret && !nChangeExisting)
	{    
		// Make new type	
		                                             
		switch (nBaseType)
		{
			case CI_MOL:
				pType = new CObjTypeMol;
				break;

			case CI_WALL:
				pType = new CObjTypeWall;
				break;				
				
			case CI_PISTON:
				pType = new CObjTypePiston;
				break;
				
			case CI_TEXTBOX:
				pType = new CObjTypeTextbox;
				break;				
				
			case CI_MONITOR:
				pType = new CObjTypeMonitor;
				break;				
				
			case CI_GRAPH:
				pType = new CObjTypeGraph;
				break;				

			case CI_SLIDER:
				pType = new CObjTypeSlider;
				break;				
				
			default:
				ASSERT(FALSE);
				pType = NULL;
				ret = SR_ERROR;  
				errString += "unknown type (not one of mol, wall, monitor, textbox or piston);";
				break;
		}
		
		if (NULL != pType)
		{
			// insert the new type in the list, in order  
			
			pType->m_nBaseType = nBaseType;
			pType->m_pSim = this;

			int i;
			for (i = 0; i <= m_typeArray.GetUpperBound(); i++)
			{
			     CObjType* pExistingType = (CObjType*) m_typeArray[i];
			     if (pExistingType->m_nBaseType > pType->m_nBaseType) break;
			}
			m_typeArray.InsertAt(i, pType);
				
			// Make space for it in the array of interactions, and add
			// default interactions
				
			m_intArray.AddIndex(i);
				 
		}
	}
	
	// If all is well, should now have type to give rest of command line
	
	if (SR_ERROR != ret)
	{			
		ret |= pType->AcceptCommandLine(remainingLine, errString); 
		
				

		pType->Initialise();  // so can do any startup init with
									  // new properties
									  
		if (!nChangeExisting)
		{
			// now it is named, make it add any alterable or displayable
			// numeric quantities it owns to our list
						
			pType->ContributeNumerics();
						
			// Find highest CSObj id that this type will be given from future
			// commands in history up to the point it is destroyed, so that any
			// new CSObjs added to its list will be given unique ids.
						
			ScanHistoryForUsedIds(pType);
		}									  
	}
            
	return ret;
}                                                      

int CSim::CmdNewObj(int nBaseType, CString& remainingLine, CString& errString)
{                                          
	return HandleObj(nBaseType, remainingLine, errString, FALSE);
}

int CSim::HandleObj(int nBaseType, CString& remainingLine, CString& errString,
                            int nChangeExisting)                     
{                               
	CString typeName = m_sArg;  // will have been got by GrabCommand

	int index = 0;   
	CObjType* pType;
	CSObj* pObj = NULL;
	int ret = SR_OK;
	long lId = 0;
	
	// find type 
	for (index = 0; index <= m_typeArray.GetUpperBound(); index++)
	{
		pType = (CObjType*) m_typeArray[index];
		if ((pType->m_nBaseType == nBaseType) && (pType->m_sName == typeName))
		{
			// found type by name and sort of object
			
			if (!nChangeExisting)
			{
				// make new one
				switch (nBaseType)
				{
					case CI_MOL:
						pObj = new CSObjMol;
						break;      
						
					case CI_WALL:
						pObj = new CSObjWall;
						break;
						
					case CI_PISTON:
						pObj = new CSObjPiston;
						break;
						
					case CI_TEXTBOX:
						pObj = new CSObjTextbox;
						break;
	
					case CI_MONITOR:
						pObj = new CSObjMonitor;
						break;
						
					case CI_GRAPH:
						pObj = new CSObjGraph;
						break;

					case CI_SLIDER:
						pObj = new CSObjSlider;
						break;
	
					default:
						ASSERT(FALSE);
						ret |= SR_ERROR;     
						pObj = NULL;
						errString += "internal error: base type unknown; ";
						break;
				}
			}
			else
			{
				// find existing obj with this id
				
				if (FindGrabCommand(remainingLine, CI_ID))
				{
					pObj = pType->FetchSObj(m_lArg);
				}
				else
				{
					pObj = NULL;
				}
				
				if (NULL == pObj) ret = SR_ERROR;
			}

			if ((SR_ERROR != ret) && !nChangeExisting)
			{                   
				// initialise new obj data members
				
				pObj->m_pSim = this;   
				pObj->m_pType = pType;
				pObj->m_lId = 0;
            }
            
            if (SR_ERROR != ret)
            {
            	ret |= pObj->AcceptCommandLine(remainingLine, errString);
            }

			if ((SR_ERROR != ret) && !nChangeExisting)
			{               
				// rest of line initialised obj OK, so try adding it to list
				if (!pType->AddObj(pObj))
				{
					// failed!
					ret = SR_ERROR;
				}   
			}

			if ((SR_ERROR != ret) && !nChangeExisting)
			{               
				// add identity for obj if not previously specified and if not
				// handling an internally-generated mol creation command originating
				// from a real 'fillmol' command
				
				if (m_nFillmolInProgress || FindGrabCommand(m_currentCommand, CI_ID))
				{
					// command that created obj already specified id, leave it alone
					// (fillmol does its own thing, see ::CmdFillMol).
				}
				else
				{
					// add it ourselves
					m_currentCommand += " id ";
					sprintf(buff, "%ld", pObj->m_lId);
					m_currentCommand += buff;
				}        
				
				// Similarly do special unique monitor id for any monitors created
				
				if ((CI_MONITOR == pType->m_nBaseType) 
				         && !FindGrabCommand(m_currentCommand, CI_MON_UID))
				{
					// Just made a monitor with no id specified in command so add it
					m_currentCommand += " mon_uid ";
					sprintf(buff, "%ld", ((CSObjMonitor*) pObj)->m_lUniqueMonId);
					m_currentCommand += buff;
				}        
			}
			
			if ((SR_ERROR == ret) && !nChangeExisting)
			{                    
				// error in setting up obj, so bin it
				delete pObj;
			}
			
			break;
		}
		else if ((pType->m_nBaseType != nBaseType) && (pType->m_sName == typeName)) 
		{
			// write name but wrong sort of object!
		}
		else
		{
			// not right sort, keep going through list
		}
	}
	
	return ret;	
}

int CSim::CmdFillmol(CString& remainingLine, CString& errString)
{                                                     
	CString typeName;
	PopWord(remainingLine, typeName);
	int index = 0;   
	CObjType* pType;
	int ret = SR_OK; 
	int nTypeFound = FALSE;
	
	// find type 
	for (index = 0; index <= m_typeArray.GetUpperBound(); index++)
	{
		pType = (CObjType*) m_typeArray[index];
		if ((pType->m_nBaseType == CI_MOL) && (pType->m_sName == typeName))
		{
			// found type by name and sort of object 
			nTypeFound = TRUE;  
			
			// The fillmol command line may or may not have included directives
			// for the first and last mol ids we should make, which would have been 
			// recorded if we are replaying from history. If we are, and it is,
			// and the mol type has been changed to make it smaller in radius, it
			// is possible that we will have to assign mol ids higher than that
			// specified in the history, and update our directives accordingly.
			
			long lFirstId = 0;
			long lLastId = 0;
						
			if (FindGrabCommand(remainingLine, CI_FIRST_ID))
			{
				lFirstId = m_lArg;
				SnipCommand(remainingLine, CI_FIRST_ID); // don't pass on to mol commands
			}
			
			if (FindGrabCommand(remainingLine, CI_LAST_ID))
			{
				lLastId = m_lArg;
				SnipCommand(remainingLine, CI_LAST_ID);
			}
			
			if (!(lFirstId && lLastId))
			{
				// if one or both missing, make both zero and let mol type decide ids
				lFirstId = 0;
				lLastId = 0;
			}

			int nIdRangeOK = FALSE;
						
			m_nFillmolInProgress = TRUE;  // so sim knows not real 'mol bibble x...' commands
			ret |= ((CObjTypeMol*) pType)->DoCmdFillMol(remainingLine,
			                                  errString, lFirstId, lLastId, nIdRangeOK);
			m_nFillmolInProgress = FALSE;    
			
			if (!nIdRangeOK)
			{
				// had no specified id range for mols before now, or didn't fit it,
				// so respecify in current command
				
				SnipCommand(m_currentCommand, CI_FIRST_ID); // remove existing one if present				
				SnipCommand(m_currentCommand, CI_LAST_ID);  // remove existing one if present
				
				m_currentCommand += " first_id ";
				sprintf(buff, "%ld", lFirstId);
				m_currentCommand += buff;
				m_currentCommand += " last_id ";
				sprintf(buff, "%ld", lLastId);
				m_currentCommand += buff;
			}
			
			break;
		}
	} 
	
	if (!nTypeFound)
	{
		ret = SR_ERROR;
		errString += "no mol of that name found; ";
	}
	
	return ret;	
}

void CSim::OnSizeSimScreen(CRect rect)
{
    // Whenever the user rescales the simulation screen, want to keep displaying
    // the currently visible rectangle at its correct aspect ratio so that it
    // just fits. In general will lead to some wastage either at the sides or
    // top and bottom. The default visible area is a historical accident from
    // the first released version of the program but is maintained for the
    // time being.

    rect.NormalizeRect(); // note will put smaller number higher up

    int temp = rect.top;
    rect.top = rect.bottom;
    rect.bottom = temp;    // now have rectangle right way up in our coord system

    m_ScreenLogicalRect = rect;

    RecalcScalingParams();
}

void CSim::RedisplayWithNewScaling(double dLeft, double dBottom, double dWidth, double dHeight)
{
    m_dScreenLeft_nm = dLeft;
    m_dScreenBottom_nm = dBottom;
    m_dScreenWidth_nm = dWidth;
    m_dScreenHeight_nm = dHeight;

    RecalcScalingParams();

    m_pDoc->UpdateAllViews(NULL);
}

void CSim::RecalcScalingParams()
{
    double dTargetAspectRatio = m_dScreenHeight_nm / m_dScreenWidth_nm;

    TRACE("CSim::RecalcScalingParams() l=%g b=%g w=%g h=%g nm\n",
          m_dScreenLeft_nm, m_dScreenBottom_nm, m_dScreenWidth_nm, m_dScreenHeight_nm);
    TRACE("... while screen log rect.Width() = %d, .Height() = %d\n",
        m_ScreenLogicalRect.Width(), m_ScreenLogicalRect.Height());

    int nJustFitsX = FALSE;

    if (0 == m_ScreenLogicalRect.Width() || 0 == m_ScreenLogicalRect.Height())
    {
        // cannot compute aspect ratio, use default (means won't be displaying
        // anything anyway)
        TRACE("CSim::RecalcScalingParams() found zero-thickness height or width\n");
        m_dScaleFactor = TRAD_SCREEN_SF;
    }
    else if (((double) -m_ScreenLogicalRect.Height() / m_ScreenLogicalRect.Width()) > dTargetAspectRatio)
    {
        // is taller and thinner than we want, have to fit x-wise
        m_dScaleFactor = (double) m_ScreenLogicalRect.Width() / m_dScreenWidth_nm;
        nJustFitsX = TRUE;
        TRACE("x-fit\n");
    }
    else
    {
        // is just right, or wider than we want, fit y-wise
        m_dScaleFactor = (double) -m_ScreenLogicalRect.Height() / m_dScreenHeight_nm;
        TRACE("y-fit\n");
    }

    // Now set offsets such that chosen rectangle is centred in window
    double dCentreX_nm = m_dScreenLeft_nm + m_dScreenWidth_nm / 2;
    double dCentreY_nm = m_dScreenBottom_nm + m_dScreenHeight_nm / 2;

    // logical coords always zero at left and top, so work out offsets: 
    m_dXOffset = dCentreX_nm * m_dScaleFactor - m_ScreenLogicalRect.Width() / 2;
    m_dYOffset = dCentreY_nm * m_dScaleFactor - m_ScreenLogicalRect.Height() / 2;

    TRACE("New scale factor = %f logicals/nm\n", m_dScaleFactor);

    // If the aspect ratio of the available screen rectangle is pretty well 
    // right, don't bother with grey bars. But if not, add grey bars to the
    // left and right of the sim screen if that is taller and thinner than the
    // available rectangle, or above and below if it is shorter and wider than
    // the available rectangle.
    int nRequiredLogical;
    int nBarThickness;
    m_nNeedGreyBars = FALSE;

    if (nJustFitsX)
    {
        // see how much space needed in y-direction for sim
        nRequiredLogical = (int) (m_dScreenHeight_nm * m_dScaleFactor + 0.5);
        nBarThickness = (-m_ScreenLogicalRect.Height() - nRequiredLogical) / 2;

        if (nBarThickness >  MIN_GREY_BAR_WIDTH_PIXELS)
        {
            // grey bars wider than threshold so do put them in
            m_nNeedGreyBars = TRUE;
            m_GreyLogicalRect1.left = m_ScreenLogicalRect.left;
            m_GreyLogicalRect1.top = m_ScreenLogicalRect.top;
            m_GreyLogicalRect1.right = m_ScreenLogicalRect.right;
            m_GreyLogicalRect1.bottom = m_ScreenLogicalRect.top - nBarThickness;
            m_GreyLogicalRect2.left = m_ScreenLogicalRect.left;
            m_GreyLogicalRect2.top = m_ScreenLogicalRect.bottom + nBarThickness;
            m_GreyLogicalRect2.right = m_ScreenLogicalRect.right;
            m_GreyLogicalRect2.bottom = m_ScreenLogicalRect.bottom;
            m_SimLogicalRect.top = m_GreyLogicalRect1.bottom - 1;
            m_SimLogicalRect.bottom = m_GreyLogicalRect2.top + 1;
            m_SimLogicalRect.left = m_ScreenLogicalRect.left;
            m_SimLogicalRect.right = m_ScreenLogicalRect.right;
        }
    }
    else
    {
        // consider bars at the left and right in the same way
        nRequiredLogical = (int) (m_dScreenWidth_nm * m_dScaleFactor + 0.5);
        nBarThickness = (m_ScreenLogicalRect.Width() - nRequiredLogical) / 2;

        if (nBarThickness >  MIN_GREY_BAR_WIDTH_PIXELS)
        {
            // grey bars wider than threshold so do put them in
            m_nNeedGreyBars = TRUE;
            m_GreyLogicalRect1.left = m_ScreenLogicalRect.left;
            m_GreyLogicalRect1.top = m_ScreenLogicalRect.top;
            m_GreyLogicalRect1.right = m_ScreenLogicalRect.left + nBarThickness;
            m_GreyLogicalRect1.bottom = m_ScreenLogicalRect.bottom;
            m_GreyLogicalRect2.left = m_ScreenLogicalRect.right - nBarThickness;
            m_GreyLogicalRect2.top = m_ScreenLogicalRect.top;
            m_GreyLogicalRect2.right = m_ScreenLogicalRect.right;
            m_GreyLogicalRect2.bottom = m_ScreenLogicalRect.bottom;
            m_SimLogicalRect.left = m_GreyLogicalRect1.right + 1;
            m_SimLogicalRect.right = m_GreyLogicalRect2.left - 1;
            m_SimLogicalRect.top = m_ScreenLogicalRect.top;
            m_SimLogicalRect.bottom = m_ScreenLogicalRect.bottom;
        }
    }

    // make sure correct sim area defined if no bars
    if (!m_nNeedGreyBars)
    {
        m_SimLogicalRect = m_ScreenLogicalRect;
    }

    // loop over object types telling them of new scale factor
    for (int index = 0; index <= m_typeArray.GetUpperBound(); index++)
	{
		CObjType* pType = (CObjType*) m_typeArray[index];
	    pType->Initialise();
	}

}

void CSim::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{   
	// If pausing during script replay, let keypress break
	// pause.  
	
	// Pressing delete key seems to sometimes get caught as
	// accelerator belonging to CLenDoc, and sometimes not
	// (if mouse has been dragging in sim view), so catch
	// here too.
	
	if (!KillPauseIfPaused())  // break pause if paused
	{                                              
		// if not curtail delay if in script delay
		if (m_nDoingDelay)
		{
			m_dDelayTimer = 0;
		}   
	}
		
	if (VK_DELETE == nChar)
	{
		HandleNewCommand("seln_delete");
	}
}

int CSim::OnLButtonDown(CPoint point, int nFlags, CWnd* pWnd)
{
	// User has clicked left button down on sim view and
	// we have co-ords.
	//
	// Firstly, if there is a prior selection and the sim is not running
	// and drawing is not in progress, check in case existing obj has been
	// hit to do move or resize.
	
	// Otherwise find out what (if anything) they have newly selected.
	
	TRACE("CSim::OnLButtonDown\n");
	
	int nHit = FALSE;
	
	
	if ((SM_SIM_STOPPED == m_nSimState) && !((nFlags & MK_CONTROL) || (nFlags & MK_SHIFT)))
	{	
		// If any selection, find first hit on obj, if any, and do
		// tracking if hit; if shift pressed though, should just toggle
		// selection of any obj hit, so do not do tracking in that case	
        
        TRACE("...sim stopped so checking for tracker hit\n");
        
		CSObj* pObj = NULL;
		POSITION pos;

		// don't allow resize if multiple selection, so convey this		
		int nMultiple = m_selectedObjects.GetCount() > 1;
		
		pos = m_selectedObjects.GetHeadPosition();
		nHit = FALSE;
		
		while ((NULL != pos) && !nHit)
		{   
			TRACE("...doing hit test on a tracker...\n");
			
			pObj = (CSObj*) m_selectedObjects.GetAt(pos);
			nHit = pObj->TrackerHitTest(point);
			m_selectedObjects.GetNext(pos);
		} 
		
		if (nHit)
		{
			// Do tracking on this selected object, ie allow user
			// to move it, or resize if appropriate   
		
			TRACE("...hit tracker, so doing track!\n");
				
			pObj->Track(nMultiple, pWnd, point);
		}
	}
	
				
	if (!nHit)
	{
		return GetSelection(nFlags, FALSE); // don't want to restrict to single selection
	}  
	
	return nHit; // whether or not button will be up again without view knowing
}       

void CSim::PrepareTrackersToFollow(CSObj* pCallingObj)
{
	// Tell all selected objs apart from caller that they are to move their tracking
	// rectangles in synch with the obj being dragged with the mouse

	CSObj* pOtherObj = NULL;
	POSITION pos;

	pos = m_selectedObjects.GetHeadPosition();
		
	while (NULL != pos)
	{   
		pOtherObj = (CSObj*) m_selectedObjects.GetAt(pos);

		if (pOtherObj != pCallingObj)
		{
			pOtherObj->PrepareTrackerForFollowing();
		}
		
		m_selectedObjects.GetNext(pos);
	}
}

void CSim::UpdateFollowingTrackers(int nDeltaX, int nDeltaY, CSObj* pCallingObj,
                                         CWnd* pWnd)
{
	// Tell all selected objs apart from caller that they are to move
	// their tracking rectangles by the given displacement from where
	// they started, as they are being shunted about in a multiple selection

	CSObj* pOtherObj = NULL;
	POSITION pos;

	pos = m_selectedObjects.GetHeadPosition();
		
	while (NULL != pos)
	{   
		pOtherObj = (CSObj*) m_selectedObjects.GetAt(pos);

		if (pOtherObj != pCallingObj)
		{
			pOtherObj->UpdateTrackerAsFollowing(nDeltaX, nDeltaY, pWnd);
		}
		
		m_selectedObjects.GetNext(pos);
	}
}
	
void CSim::OnLButtonDblClk(int nFlags)
{   
	TRACE("CSim::OnLButtonDblClk\n");

	if (SM_SIM_STOPPED == m_nSimState)
	{
		// Clear existing selection, make new selection at double-clicked point,
		// and bring up properties box of whatever object is there (if any);
		// except if single selection already and is hit, don't want to clear
		// selection and start again
		
		GetSelection(nFlags, TRUE);  // want to restrict to single selection
			
		// Should now have one or zero objects selected
		
		int nSelected = m_selectedObjects.GetCount();
			
		if (nSelected > 0)
		{
			ASSERT(nSelected == 1); // doesn't really matter if only do props for first if
			                        // multiple selection though
				                        
			CSObj* pObj = (CSObj*) m_selectedObjects.GetHead();
				
			pObj->ObjectProperties();
		}	
	}
	else
	{
		// do nothing if sim running
	}
}  

int CSim::GetSelection(int nFlags, int nSingle)
{
	// If nothing was previously selected, then make new selection at current
	// point; if there was previously a multiple selection, then clear it and
	// make new selection; if there was previously a single selection and the
	// same object it hit again, do nothing.
	//
	// This is to achieve the expected behaviour when objects are clicked, 
	// double-clicked or shift-clicked while issuing only the minimum of commands
	// to the simulation.
                                                                   
                                                                   
	// Allow user to click close to a wall say and still select it:	
	double dLatitude = 2 / m_dScaleFactor;                         
	
	
	int nSelected = m_selectedObjects.GetCount();
	
	int nClearSelection = FALSE;
	int nHitTest = FALSE;
	
	if (1 == nSelected) 
	{
		// Have single selection already, don't clear selection if we have
		// hit that with double click

		CSObj* pSelected = (CSObj*) m_selectedObjects.GetHead();   

		nHitTest = pSelected->HitTest(m_dSimMouseX - dLatitude,
		                              m_dSimMouseY - dLatitude,
	                                  m_dSimMouseX + dLatitude,
	                                  m_dSimMouseY + dLatitude); 
	                                  
		nClearSelection = !nHitTest;	                                  
	}
	else if (nSelected > 0)    
	{
    	nClearSelection = TRUE;
	}
	
	if (nClearSelection && !(((nFlags & MK_CONTROL) || (nFlags & MK_SHIFT)) && !nSingle))
	{ 
		//  clear existing selection and make new one so long as not shift-clicking;
		//  if only want single selection, ignore shift or ctrl in any case
		
		HandleNewCommand("seln_reset"); // clear existing selection
	}
	
	if (!nHitTest)
	{			
		// loop over types to get select object(s) within a couple of
		// pixels of where the mouse was clicked
			
		return GetSelectedObjects(m_dSimMouseX - dLatitude, m_dSimMouseY - dLatitude,
		                   m_dSimMouseX + dLatitude, m_dSimMouseY + dLatitude,
		                   FALSE,   // do want to toggle selection of objs found
		                   nSingle  // whether want restricted to single selection
		                   );
	}
    else
    {
        return FALSE; // button won't be up again if this called due to lbuttondown
    }
}

int CSim::OnRButtonDown(int nFlags)
{   
	TRACE("CSim::OnRButtonDown\n");

	if (SM_SIM_STOPPED == m_nSimState)
	{
		// For consistency with Windows 95, if right-click on existing selection
		// (single or multiple), should bring up menu relevant to that selection;
		// but if elsewhere, should make new selection if over another object,
		// and bring up menu appropriate to that (or empty space if none).
	
		GetSelection(nFlags, TRUE);  // want to restrict to single selection
		
		// Should now have one or zero objects selected
	}
	else
	{
		// do no selection if sim running
	}                                                             
	
	return TRUE; // ask for menu to appear, done by view class
}                                  

void CSim::ObjectProperties()
{
	// Get first selected object, if any, and get it to do properties
	// dialogue relevant to itself
	
	int nSelected = m_selectedObjects.GetCount();
	
	if (nSelected > 0)
	{
		((CSObj*) m_selectedObjects.GetHead())->ObjectProperties();
	}
}

void CSim::OnFinishedDragging(int nFlags)
{
	// loop over types to get select object(s) within 
	// selection box drawn by user. Set flag indicating
	// adding to dragged selection TRUE to avoid toggling
	// already-selected objs dragged over   
	
	TRACE("CSim::OnFinishedDragging\n");
	
	// Note that if button up happened while tracking selection (ie
	// dragging or resizing something), then we won't have got called here;
	// the lbuttonup message will have been caught by the tracker, not the
	// view class.
	
	GetSelectedObjects(m_dSimDragStartX, m_dSimDragStartY,
	                   m_dSimMouseX, m_dSimMouseY, TRUE, FALSE);
}

int CSim::GetSelectedObjects(double dXLeft, double dYBottom, 
                double dXRight, double dYTop, int nDragged, int nSingle)
{                
	NormaliseRect(dXLeft, dYTop, dXRight, dYBottom);
		
	// loop over types to get selected child objects
	
	int sel = FALSE;
    int nPreviouslySelected = m_selectedObjects.GetCount();
	
	for (int index = 0; index <= m_typeArray.GetUpperBound() && !(nSingle && sel); index++)
	{
		CObjType* pType = (CObjType*) m_typeArray[index];
	    sel = pType->GetSelectedObjects(dXLeft, dYBottom, dXRight, dYTop, nDragged, nSingle);
	}

    if (!nDragged && 
        theApp.m_nAutoShowSelnResolutionDlg &&
        (m_selectedObjects.GetCount() - nPreviouslySelected > 1))
    {
        CResolveMultipleSelnDlg dlg;

        dlg.m_pSim = this;

        dlg.DoModal();

        return TRUE;

    }
    else
    {
        return FALSE; // haven't had button up unbeknown to caller
    }
}

// Functionality overridden from CSimObj                    
int CSim::GetStateParam(int command)
{         
	int ret = SR_OK;
	
	switch(command)
	{
        case CI_AVERAGING_TC:
            m_dArg = m_dAverageTimeConst;
            break;

		case CI_TEMPERATURE:
			m_dArg = m_dTemperature;
			break;
			
		case CI_GRAVITY:
			m_dArg = m_dGravitationalAcceleration;
			break; 
			
		case CI_AUTORUN:
			m_bArg = m_nIsAutoRun;
			break;    
			
		case CI_GRID_ON:
			m_bArg = (unsigned char) m_nUseGrid;
			break;
			
		case CI_GRIDSIZE:
			m_dArg = m_dGridSize;
			break;
			
		case CI_SIMCHEMISTRY_VER:
			m_dArg = m_dDocSimChemistryVer;
			break;

        case CI_DELTATIME:
            m_dArg = m_dDeltaTime;
            break;

        case CI_USE_AUTO_DELTATIME:
            m_dArg = m_dUseAutoDeltaTime;
            break;
			
		default:
			TRACE("Asked for param for unknown command in CSim\n");
			ASSERT(FALSE);
			ret = SR_ERROR;
			break;
	}             
	
	return ret;
}          

BOOL CSim::OnDrawCommand(int typeId)
{          
	if ((typeId < 0) || (typeId > m_typeArray.GetUpperBound())) 
	{
		return FALSE;
	}
	else
	{
		CObjType* pType = (CObjType*) m_typeArray[typeId];
		if (pType->DrawCommand())
		{
			// drawing initiated; stop sim running
			m_pDrawingType = pType; 
			if (SM_SIM_STOPPED != m_nSimState)
			{
				m_nSimState = SM_SIM_PAUSED; 
				m_nSimReady = FALSE;
			}
			m_nDrawingState = SM_AWAITING_CLICK1;
		}
		else
		{
			// user must have cancelled
		}
		
		return TRUE;  // to indicate Windows command handled
	}
}                   

BOOL CSim::OnPropertiesCommand(int typeId)
{          
	if ((typeId < 0) || (typeId > m_typeArray.GetUpperBound())) 
	{
		return FALSE;
	}
	else
	{
		CObjType* pType = (CObjType*) m_typeArray[typeId];
		pType->PropertiesCommand();
		return TRUE;  // to indicate Windows command handled
	}
}                   

int CSim::GetNumHistoryEntries()
{         
	return m_historyStrings.GetSize();
}

int CSim::AcceptCommandLine(CString& remainingLine, CString& errString)
{
    int foo;

    return AcceptCommandLine(remainingLine, errString, &foo);
}

int CSim::AcceptCommandLine(CString& remainingLine, CString& errString, int* nNoHistory)
{            
	CMD_ENTRY* pCmdEntry = NULL; 
	int command;
    *nNoHistory = FALSE; // set true if rare case that shouldn't be recorded

	int ret = GrabCommand(remainingLine, errString, command, &pCmdEntry); 

    while ((CI_EMPTY != command) && (SR_ERROR != ret))
    {
		if (SCM_HISTORY == m_nCmdMode)
		{
			// don't actually execute command except end_history
			if (CI_END_HISTORY == command)
			{
				 ret |= CmdEndHistory();   
				 ret |= GrabCommand(remainingLine, errString, command, &pCmdEntry); 
			}  
			else
			{
				command = CI_EMPTY;
			}
		}
		else
		{
			// actually execute appropriate command	 
			if (command <= MAX_OBJ_CI)
			{
				// it is an instance of an object, handle generically 
				ret |= CmdNewObj(command, remainingLine, errString);
			}
			else
			{
				// handle misc commands individually
				switch (command)
				{
                    case CI_SET_RAND_SEED:
                        SeedRandom(m_lArg);
                        break;

                    case CI_AVERAGING_TC:
                        m_dAverageTimeConst = m_dArg;
                        break;

					case CI_CHANGE:
						// will be followed by obj or obj type to alter
						ret |= CmdChange(remainingLine, errString);
					    m_nSimReady = FALSE;
						break;
						
					case CI_INTERACTION:
						ret |= CmdInt(remainingLine, errString);
					    m_nSimReady = FALSE;
						break;
						 
					case CI_RUN:
						ret |= CmdRun();
					    m_nSimReady = FALSE;
						break;
					
					case CI_STOP:
						ret |= CmdStop();
					    m_nSimReady = FALSE;
						break;
					
					case CI_TEMPERATURE:
						ret |= CmdTemperature();
						break;
					
					case CI_BEGIN_HISTORY: 
						ret |= CmdBeginHistory();
						break;
					
					case CI_END_HISTORY: 
						ret |= CmdEndHistory();
						break;
					
					case CI_BEGIN_STATE: 
						ret |= CmdBeginState();
						break;
					
					case CI_END_STATE: 
						ret |= CmdEndState();
						break;
						
					case CI_OBJTYPE:
						ret |= CmdObjType(remainingLine, errString);
					    m_nSimReady = FALSE;
						break;   
						
					case CI_FILLMOL:
						ret |= CmdFillmol(remainingLine, errString); 
					    m_nSimReady = FALSE;
						break;
									
					case CI_DELTYPE:
						ret |= CmdDelType(remainingLine, errString); 
					    m_nSimReady = FALSE;
						break;
									
					case CI_RESET:
						ret |= CmdReset(); 
					    m_nSimReady = FALSE;
						break;
					
					case CI_DELAY:
						ret |= CmdDelay(); 
						break;  
						
					case CI_GRAVITY:
						m_dGravitationalAcceleration = m_dArg;
						break;
					
					case CI_AUTORUN:
						m_nIsAutoRun = m_bArg;
						break;
			
					case CI_SELN_RESET: 
						ret |= CmdSelnReset();
						break;
						
					case CI_SELECT:
						ret |= CmdSelect(remainingLine, errString);
						break;
														
					case CI_SELN_DELETE: 
						ret |= CmdSelnDelete();
					    m_nSimReady = FALSE;
						break;
					
					case CI_PAUSE: 
						ret |= CmdPause();
						break;
						
					case CI_DEL_OBJS: 
						ret |= CmdDelObjs(errString);
					    m_nSimReady = FALSE;
						break;
						
					case CI_DEL_ALL: 
						ret |= CmdDelAll(errString);
					    m_nSimReady = FALSE;
						break;
						
					case CI_SELN_MOVE:
						ret |= CmdSelnMove(remainingLine, errString);
						break;
						
					case CI_GRID_ON:
						m_nUseGrid = (int) m_bArg;
						break;
						
					case CI_GRIDSIZE:
						m_dGridSize = m_dArg;
						break;
						
					case CI_SET:
						ret |= CmdSet(remainingLine, errString);
						break;

					case CI_CLEAR_GRAPH:
						ret |= CmdClearGraph(remainingLine, errString);
						break;             
						
					case CI_SIMCHEMISTRY_VER:
						if (m_dArg > SIMCHEMISTRY_VER_NUMERIC)
                        {
                            ret |= SR_WARN;
                            errString += "file created with SimChemistry version newer than this one (";
                            errString += (const char*) SIMCHEMISTRY_VER_STRING;
                            errString += "); time to upgrade! ";
                        }
                        // not sure if should adopt new ver, but here goes:
                        m_dDocSimChemistryVer = m_dArg;
						break;

                    case CI_SIM_SIZE:
						ret |= CmdSimSize(remainingLine, errString);
						break;        
                        
                    case CI_DELTATIME:
                        m_dDeltaTime = m_dArg;
                        break;

                    case CI_USE_AUTO_DELTATIME:
                        m_dUseAutoDeltaTime = (m_dArg > 0.5 ? 1 : 0);
                        break;
						
					default:
						ret |= SR_ERROR;   
						errString += "unknown command before: " + remainingLine;
						break;
				}   // end of switch (command) 
			} // end of if (command <= MAX_OBJ_CI) 
			
            if (SR_ERROR != ret && (pCmdEntry->nSpecialFlags & CE_DONT_RECORD_IN_HISTORY))
            {
                *nNoHistory = TRUE;
            }

			ret |= GrabCommand(remainingLine, errString, command, &pCmdEntry); 
			
		} // end of if (only doing history)             

	    
	} // end of while (command to process)

	// If this sim was not autorun, note doc is now modified
	if (!m_nWasAutoRun)
	{
		m_pDoc->SetModifiedFlag(TRUE);
	}
			  
	return ret;
}

CMD_ENTRY* CSim::GetCmdList()
{   
	return &(m_celCSimCommands[0]);
}

// Functions special to CSim
	
CSim::CSim()
{       
	m_dDocSimChemistryVer = SIMCHEMISTRY_VER_NUMERIC;
	
	m_dScaleFactor = TRAD_SCREEN_SF;  // logical co-ord = (real co-ord * sf) - offset
	m_dXOffset = 0 * m_dScaleFactor;
	m_dYOffset = 0 * m_dScaleFactor + 500;  
    m_dScreenWidth_nm = TRAD_SCREEN_WIDTH_NM; 
    m_dScreenHeight_nm = TRAD_SCREEN_HEIGHT_NM;
    m_dScreenLeft_nm = TRAD_SCREEN_LEFT_NM;
    m_dScreenBottom_nm = TRAD_SCREEN_BOTTOM_NM;
    m_nNeedGreyBars = FALSE;
    m_GreyLogicalRect1 = CRect(0, 0, 0, 0);
    m_GreyLogicalRect2 = CRect(0, 0, 0, 0);
    m_SimLogicalRect = m_ScreenLogicalRect;
    m_pFirstSpareLN = NULL;

    m_lRanSeed = 0;

	// a simulation may be destroyed by user actions whilst a sim is running,
	// whereupon all of its member variables disappear; flags held by the
	// app keep track of whether the CSim is still intact:
	theApp.m_simExistenceFlags.Add(TRUE);
	m_nAppExistenceFlagIndex = theApp.m_simExistenceFlags.GetUpperBound();
                    
	// table of solution to integral equation required by 
	// thermal wall desorption speed algorithm:
	m_pdVelVsPForUnitMOverT = new double[PSCALING];      
	BuildThermalWallTable();                   
	
	// set default values for system params etc that should
	// be reset at sim replay or reset command:
	Initialise();
	
	// set default values for quantities that should not be
	// reset at sim replay:
	m_nWasAutoRun = FALSE;
	m_nIsRecording = TRUE;
   	m_nCurrentCmdIndex = -1;     

	m_nFillmolInProgress = FALSE;
	m_lLastMonIdUsed = 0;
	
	// build up gallery of possible interactions
	CInt* pInt;  // required by REGISTER_INT macro 
	int id = 0;  // required by REGISTER_INT macro 
	REGISTER_INT(CNullInt);  
	REGISTER_INT(CWallMolDefaultInt);
	REGISTER_INT(CMolMolHardInt);
    REGISTER_INT(CMolMolReactiveInt);
	REGISTER_INT(CLennardJonesInt);
	REGISTER_INT(CMolPistonHardInt);
	REGISTER_INT(CMonitorMolDefaultInt);
	      
	// Kick off list of public numeric quantities with
	// own properties (eg temperature)
	ContributeNumerics();
	      
#ifdef _DEBUG
	m_lNumNodes = 0;
#endif
}

CSim::~CSim()
{
    TRACE("CSim::~CSim()\n");

	// Mark sim as deleted in an array held by the app object; sim
	// can read from this posthumously and know not to try and access
	// its member variables.        
	theApp.m_simExistenceFlags[m_nAppExistenceFlagIndex] = FALSE;

    TRACE("... deleting m_pdVelVsPForUnitMOverT\n");

	delete [] m_pdVelVsPForUnitMOverT;
	
	CHistoryEntry* pEntry;  
	CObjType* pType;
	int index;
	
    TRACE("... deleting history\n");

	// delete history
	while ((index = m_historyStrings.GetUpperBound()) > -1)
	{                                                      
		pEntry = (CHistoryEntry*) m_historyStrings[index];
		m_historyStrings.RemoveAt(index);
		delete pEntry;
	}
	
    TRACE("... deleting object types\n");

	// delete all simulation objects 
	while (m_typeArray.GetUpperBound() > -1)
	{          
		pType = (CObjType*) m_typeArray[0];   
		pType->Delete();
		delete pType;
	}   
	
	// delete gallery of example interaction objects

    TRACE("... deleting interaction gallery\n");

	CInt* pInt;
	while (!m_intGallery.IsEmpty())  
	{                               
		pInt = (CInt*) m_intGallery.RemoveTail();
		delete pInt;
	}  
	
	// delete triangular array of interactions
	// should be empty anyway as we have deleted all object types
	
    TRACE("... deleting any leftover interactions\n");

	while ((index = m_intArray.GetUpperBound()) > -1)
	{                                                      
		ASSERT(FALSE);  // should have already been empty
		pInt = (CInt*) m_intArray[index];
		m_intArray.RemoveAt(index);
			
		delete pInt;
	}
	
	// delete lists of interactions by category
    TRACE("... deleting int cat lists\n");
	DeleteIntCatLists();    
	
	// delete lists of object types by category
    TRACE("... deleting type cat lists\n");
	DeleteTypeCatLists();
	
	ASSERT(0 == m_lTotalNumObjs); // should all be gone
}                                           

void CSim::OnLoad()
{  
	// Scan for monitors with already allocated unique monitor ids, so
	// that we can avoid giving any newly-created monitors a duplicate id.
	// These ids are used to decide what molecule ids to give those mols
	// automatically generated by monitors.
	ScanHistoryForMonitorIds();
	
	if (m_nIsAutoRun)
	{	
		if(IDYES == AfxMessageBox("This simulation is intended to be run automatically"
		                         " from its start. Do this now?" , MB_YESNO))
		{        
			m_nWasAutoRun = TRUE;     
			m_nIsRecording = FALSE;
			ReplayScript();
		}
	} 
	
	m_pDoc->SetModifiedFlag(FALSE);
}

void CSim::Initialise()
{             
    m_nSynchronizeHappened = FALSE;
	m_dUseAutoDeltaTime = 1;
	m_lTotalNumObjs = 0;
	m_nIssuedTooManyObjsWarning = FALSE;
	m_nUseGrid = TRUE;
	m_nIsAutoRun = FALSE;
	m_nDoingHardEvents = FALSE;
	m_dSimTimeLastCmd = 0;                  
	m_lIdleScreenUpdateMs = 200;
	m_lNextLoopStartTicks = GetTickCount() - 1; 
	m_nDoingDelay = FALSE;
	m_dDelayTimer = 0;
	m_dTimeRatio = 1e12;
	m_pRootNode = NULL;  
	m_nSimReady = FALSE;
	m_nSimState = SM_SIM_STOPPED;
	m_nDrawingState = SM_NOT_DRAWING;
	m_nReplayState = SM_REPLAY_STOPPED;
	m_nCmdMode = SCM_ACTIVE;  
	m_dTemperature = 100; 
	m_dGravitationalAcceleration = 0;
	m_dGridSize = 0.1; 
	m_typeArray.SetSize(0);
	m_intArray.m_pSim = this;
	m_pFirstHardIntEl = NULL; 
	m_pFirstSoftIntEl = NULL;      
	m_pFirstNonNullIntEl = NULL;
	m_pFirstMobileTypeEl = NULL;
	m_pFirstMonitorTypeEl = NULL;
	m_pFirstGraphTypeEl = NULL;
	m_dDeltaTime = 1e-6; // ns
	m_dSimTime = 0; 
	m_dAverageTimeConst = 0.003; // ns
	global_debug_sim_time = 0;  
	m_dScriptTime = 0;
	m_lStepsBetweenUpdates = 100;  
	m_dSafetyDist = SIM_PREF_SAFETY_DIST;
	m_dInsuranceDist = 0.2;
	m_dSafetyDistLeft = 0;
}

inline CLN* CSim::GetLN()  // returns an LN for use                                         
{      
	CLN* pLN;    
	
	if (NULL != (pLN = m_pFirstSpareLN))
	{
		m_pFirstSpareLN = pLN->m_pNextLN;
		return pLN;
	}
	else
	{
		return new CLN;
	}
}

inline void CSim::RecycleLNs(CLN*& pFirstLN, CLN*& pLastLN) // accepts a chain of LNs for recycling
{
	CLN* pOldFirstSpareLN = m_pFirstSpareLN;
	m_pFirstSpareLN = pFirstLN;
	pLastLN->m_pNextLN = pOldFirstSpareLN;
	pFirstLN = NULL;
	pLastLN = NULL; 
}

void CSim::ContributeNumerics()
{   
	// initialise numerics with names etc
	m_numTemperature.m_pSim = this;
	m_numTemperature.m_sName = "sim:temperature";
	m_numTemperature.m_sUnits = "K";
	m_numTemperature.m_sDescription = "system temperature";
	m_numTemperature.m_pdValue = &m_dTemperature; 
	m_numTemperature.m_nType = NUM_USERWRITE | NUM_USERREAD;
	m_numTemperature.m_dMin = LIM_MIN_TEMP;
	m_numTemperature.m_dMax = LIM_MAX_TEMP;
	AcceptNewNumeric(&m_numTemperature);

	m_numGravitationalAcceleration.m_pSim = this;
	m_numGravitationalAcceleration.m_sName = "sim:gravity";
	m_numGravitationalAcceleration.m_sUnits = "m/s/ns";
	m_numGravitationalAcceleration.m_sDescription = "gravitational acceleration";
	m_numGravitationalAcceleration.m_pdValue = &m_dGravitationalAcceleration; 
	m_numGravitationalAcceleration.m_nType = NUM_USERWRITE | NUM_USERREAD;
	m_numGravitationalAcceleration.m_dMin = -LIM_MAX_GRAVITY;
	m_numGravitationalAcceleration.m_dMax = LIM_MAX_GRAVITY;
	AcceptNewNumeric(&m_numGravitationalAcceleration);
	
	m_numDeltaTime.m_pSim = this;
	m_numDeltaTime.m_sName = "sim:deltatime";
	m_numDeltaTime.m_sUnits = "ns";
	m_numDeltaTime.m_sDescription = "time interval between steps";
	m_numDeltaTime.m_pdValue = &m_dDeltaTime; 
	m_numDeltaTime.m_nType = NUM_USERWRITE | NUM_USERREAD;
	m_numDeltaTime.m_dMin = LIM_MIN_DELTA_TIME;
	m_numDeltaTime.m_dMax = LIM_MAX_DELTA_TIME;
	AcceptNewNumeric(&m_numDeltaTime);

	m_numUseAutoDeltaTime.m_pSim = this;
	m_numUseAutoDeltaTime.m_sName = "sim:use_auto_deltatime";
	m_numUseAutoDeltaTime.m_sUnits = "";
	m_numUseAutoDeltaTime.m_sDescription = "whether to use automatic delta time";
	m_numUseAutoDeltaTime.m_pdValue = &m_dUseAutoDeltaTime; 
	m_numUseAutoDeltaTime.m_nType = NUM_USERWRITE | NUM_USERREAD;
	m_numUseAutoDeltaTime.m_dMin = 0;
	m_numUseAutoDeltaTime.m_dMax = 1;
	AcceptNewNumeric(&m_numUseAutoDeltaTime);

	m_numSimTime.m_pSim = this;
	m_numSimTime.m_sName = "sim:time";
	m_numSimTime.m_sUnits = "ns";
	m_numSimTime.m_sDescription = "elapsed simulation time";
	m_numSimTime.m_pdValue = &m_dSimTime; 
	m_numSimTime.m_nType = NUM_USERREAD;
	AcceptNewNumeric(&m_numSimTime);

	m_numAverageTimeConst.m_pSim = this;
	m_numAverageTimeConst.m_sName = "sim:avg_time_const";
	m_numAverageTimeConst.m_sUnits = "ns";
	m_numAverageTimeConst.m_sDescription = "smoothing time constant";
	m_numAverageTimeConst.m_pdValue = &m_dAverageTimeConst; 
	m_numAverageTimeConst.m_nType = NUM_USERWRITE | NUM_USERREAD;
	m_numAverageTimeConst.m_dMin = LIM_MIN_AVERAGE_TC;
	m_numAverageTimeConst.m_dMax = LIM_MAX_AVERAGE_TC;
	AcceptNewNumeric(&m_numAverageTimeConst);
}          

void CSim::AcceptNewNumeric(CNumeric* pNumeric)
{
	// for others to call to add one to list
	
	ASSERT(pNumeric->m_pSim == this);
	
	int highlim = m_numList.GetUpperBound();
	
	if (highlim > 0)
	{ 
		// do binary search of existing list to add
		// new one in alphabetical order    
		// sorry about this dreadful bit of code
		
		int sample; 
		int lowlim = 0;
		
		CNumeric* pSample;
		
		pSample = (CNumeric*) m_numList[0];
		if (pNumeric->m_sName < pSample->m_sName)
		{
			// belongs first in list
		    m_numList.InsertAt(0, pNumeric);  
		    return;
		}
		
		pSample = (CNumeric*) m_numList[highlim];
		if (pNumeric->m_sName > pSample->m_sName)
		{
			// belongs last in list
		    m_numList.Add(pNumeric);  
		    return;
		}
		
		// else belongs somewhere in between (usually)
		while (highlim - lowlim > 1)
		{          
			sample = (lowlim + highlim) / 2;
			pSample = (CNumeric*) m_numList[sample];
			
			if (pNumeric->m_sName < pSample->m_sName)
			{
				highlim = sample;                  
			}
			else
			{
				lowlim = sample;
			}
		}		  
		
		// want to put it between lowlim and highlim
		
		m_numList.InsertAt(highlim, pNumeric);
	}
	else if (highlim == 0)
	{
		// there is only one other in list so far
		CNumeric* pExisting = (CNumeric*) m_numList[0];
		if (pNumeric->m_sName < pExisting->m_sName)
		{
			m_numList.InsertAt(0, pNumeric);
		}
		else
		{
			m_numList.InsertAt(1, pNumeric);
		}
	}   
	else
	{
		// this is first to go in list
	
		m_numList.Add(pNumeric);
	}
}

CNumeric* CSim::FetchNumeric(CString& string)
{
	// find numeric of given name, if it exists, else 
	// return NULL
	
	int highlim = m_numList.GetUpperBound();
	int lowlim = 0;
	int sample;
		
	CNumeric* pSample;
	
	if (-1 == highlim)
	{
		// list empty
		return NULL;
	}
	else
	{   
		do
		{
			sample = (lowlim + highlim) / 2;
			pSample = (CNumeric*) m_numList[sample];
				
			if (string == pSample->m_sName)
			{
				return pSample;
			}
			else if (string < pSample->m_sName)
			{   
				highlim = sample;
			}
			else
			{
				// string >> sample, but could be at highlim
				if ((highlim - lowlim) > 1)
				{
					lowlim = sample; 
				}
				else
				{
					// should be at highlim if matches
					pSample = (CNumeric*) m_numList[highlim];
					if (string == pSample->m_sName)
					{
						return pSample;
					}
			        else
			        {
			        	return NULL;
			        }
			    }
			}
		}
		while (lowlim != highlim);
		
		return NULL;
	}
}

void CSim::RemoveNumeric(CNumeric* pNumeric)
{
	// If an obj is deleted, each CNumeric obj it owns
	// will call this to have itself removed from the list
	
	int i;
	int found = FALSE;  
	CNumeric* pDebug;
	
	for (i = 0; i <= m_numList.GetUpperBound(); i++)
	{
		if ((pDebug = (CNumeric*) m_numList[i]) == pNumeric)
		{
			m_numList.RemoveAt(i);
			found = TRUE;
			break;
		}
	}
	
	ASSERT(found);
}

void CSim::PlayScript()
{                                 
	if (SM_REPLAY_PLAYING != m_nReplayState)
	{	
		m_nReplayState = SM_REPLAY_PLAYING;
	}
	else
	{
		m_nReplayState = SM_REPLAY_STOPPED;
	}
}

void CSim::FastFwdScript()
{                              
	if (SM_REPLAY_FFWDING != m_nReplayState)
	{
		m_nReplayState = SM_REPLAY_FFWDING;
		
		if (m_nDoingDelay)
		{
			// curtail delay in progress to short
			// delay, see also CmdDelay
		
			// ns = 1e9 * sec / unitless
			m_dDelayTimer = 1e9 * FFWD_DELAY / m_dTimeRatio ;   
			m_dDelayTotal = m_dDelayTimer; 
		}	
	}
	else
	{
		m_nReplayState = SM_REPLAY_PLAYING;
	}
}

void CSim::StopScript()
{                                       
	ASSERT(SM_REPLAY_STOPPED != m_nReplayState);   
	
	m_nReplayState = SM_REPLAY_STOPPED;
}      

int CSim::KillPauseIfPaused()
{
	if (SM_REPLAY_PAUSED_PLAY == m_nReplayState)       
	{
		m_nReplayState = SM_REPLAY_PLAYING;

        // move on to next command unless this was the last
        // anyway, in which case replay has finished
        if (m_nCurrentCmdIndex < m_historyStrings.GetUpperBound())
        {
            m_nCurrentCmdIndex++; 
        }
        else
        {
            m_nReplayState = SM_REPLAY_STOPPED;
        }
		return TRUE;
	}
	else if (SM_REPLAY_PAUSED_STOP == m_nReplayState)       
	{
		m_nReplayState = SM_REPLAY_STOPPED;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CSim::ReplayScript()
{
	// Destroy current state and replay history from start
	
	CmdReset();  
	
	m_nReplayState = SM_REPLAY_PLAYING;  
	m_nCurrentCmdIndex = -1;
		
	CHistoryEntry* pHE;
	int ret = ReadFromHistory(pHE, HT_CMD, HP_FIRST, m_nCurrentCmdIndex);
		
	if (SR_OK != ret)
	{
		// history empty or some problem
		m_nReplayState = SM_REPLAY_STOPPED;
	}
	else
	{
		// have found first command to be executed, sim loop should pick up
	}
}

void CSim::RegisterInt(CInt* pInt)
{          
	// Registers a kind of interaction in the list maintained
	// by CSim as a gallery of the kinds available; keeps an
	// example copy of each.
	
	m_intGallery.AddTail(pInt);
}

int CSim::Save(CStdioFile& file)
{                     
	CString outString; 
	int ret;             
	int index;

	// First write state info
	outString = "begin_state";
	ret = WriteCmdToDisk(file, outString);
	if (SR_OK != ret) return ret;
	
	outString = "";

	// Write CSim's own properties:
	WriteState(file);

    ret = WriteCmdToDisk(file, outString);
	if (SR_OK != ret) return ret;
	
	// Write object types and attached objects
	for (index = 0; index <= m_typeArray.GetUpperBound(); index++)
	{
		outString = "objtype ";
		
		CObjType* pType = (CObjType*) m_typeArray[index];
		
		// find name of base type
		
		CMD_ENTRY* celList = GetCmdList();
		int i = 0;
		while (CI_ENDLIST != celList[i].cmd_id)
		{         
			if (celList[i].cmd_id == pType->m_nBaseType)
			{  
				outString += celList[i].command;
				break;
			}       
			i++;
		}

		outString += " ";
		pType->WriteState(outString);
		ret = WriteCmdToDisk(file, outString);
		if (SR_OK != ret) return ret;         
		
		// Iterate through linked list to dump instances of all objects of this type
		CSObj* pSObj = pType->m_pFirstSObj;
		while (NULL != pSObj)
		{
			outString = celList[i].command;   // base type 
			outString += " \"";
			outString += pType->m_sName;      // specific type name
			outString += "\" ";
			pSObj->WriteState(outString);
			ret = WriteCmdToDisk(file, outString);
			if (SR_OK != ret) return ret;         
			pSObj = pSObj->m_pNextSObj;
		}
		   
	}       
	
	// Now dump out all interactions in use
	for (index = 0; index <= m_intArray.GetUpperBound(); index++)
	{
		CInt* pInt = (CInt*) m_intArray[index];
		
		outString = "interaction " + pInt->m_sName + " ";
		pInt->WriteState(outString);
		ret = WriteCmdToDisk(file, outString);
		if (SR_OK != ret) return ret;         
	}
	
	outString = "end_state";
	ret = WriteCmdToDisk(file, outString);
	if (SR_OK != ret) return ret;            
	
	// Then write history strings 
	outString = ""; // blank line
	ret = WriteCmdToDisk(file, outString);
	if (SR_OK != ret) return ret;
	outString = "begin_history";
	ret = WriteCmdToDisk(file, outString);
	if (SR_OK != ret) return ret;

	CHistoryEntry* pEntry;        
	int currentIndex = -1;  
	
	while (SR_NO_MORE != ReadFromHistory(pEntry, HT_CMD, HP_NEXT, currentIndex))
	{   
		ret = WriteCmdToDisk(file, pEntry->string); 
		if (SR_OK != ret) return ret;
	}		

	outString = ""; // blank line
	ret = WriteCmdToDisk(file, outString);
	if (SR_OK != ret) return ret;
	outString = "end_history";
	ret = WriteCmdToDisk(file, outString);
	if (SR_OK != ret) return ret;

    m_pDoc->SetModifiedFlag(FALSE);
    
	return ret;
}

int CSim::WriteState(CStdioFile& file)
{                                            
	// Go through the list of state commands, but do one per command
    // unlike a normal object or object type. Hence file parameter passed
    // to this function instead of normal string too. See also CSimObj::WriteState().
	
	CMD_ENTRY* celList = GetCmdList();       
	
	int ret = SR_OK;            
	CString string;
	int i =  0;  
    CString workingLine;
    
    // list normal state info first

	while (SR_OK == ret && CI_ENDLIST != celList[i].cmd_id)
	{         
		if (CT_STATE == celList[i].cmd_type)
		{  
			if (SR_OK == GetStateParam(celList[i].cmd_id))  // should be, unless command not handled by mistake
			{
				workingLine += celList[i].command;
				workingLine += " ";
				switch (celList[i].arg_type)
				{
					case CA_DOUBLE:
						sprintf(buff, "%g ", m_dArg);
						workingLine += buff;
						break;
					
					case CA_NONE:
						break;        
						
					case CA_STRING:
						workingLine += "\"" + m_sArg + "\" ";
						break;    
						
					case CA_BYTE:
						sprintf(buff, "%d ", m_bArg);
						workingLine += buff;
						break;
						
					case CA_INT:
						sprintf(buff, "%d ", m_nArg);
						workingLine += buff;
						break;
						
					case CA_LONG:
						sprintf(buff, "%ld ", m_lArg);
						workingLine += buff;
						break;
						
					default:
						ASSERT(FALSE);
						break;
				}

                ret = WriteCmdToDisk(file, workingLine);
                workingLine = "";
			}    
			else
			{
				// problem with a command
				ret = SR_ERROR;
			}
		
		}       
		i++;
	}           
	
    if (SR_OK == ret)
    {
        // now do special commands
        sprintf(buff, "sim_size left %g bottom %g width %g height %g", 
                      m_dScreenLeft_nm,
                      m_dScreenBottom_nm,
                      m_dScreenWidth_nm,
                      m_dScreenHeight_nm);
        
        workingLine = buff;
        ret = WriteCmdToDisk(file, workingLine);
    }
    
    return ret;
}


void CSim::AddToHistory(const char* string, int type)
{   
	static bool nSimWasRunningLastTime = FALSE;
	
	CHistoryEntry* pEntry = new CHistoryEntry;
	pEntry->string = string;
	pEntry->type = type; 
		
	// If we're not replaying a script, just tag the new history item
	// on the end. If we are however, insert it at current point in time,
	// amending delay time if we are part way through a delay. Being paused
    // is a special case too, in that case put this command before the pause.
	
	CHistoryEntry* pDelayHE;  
	CHistoryEntry* pNewDelayHE;
		
	if (m_nDoingDelay)
	{   			
		int ret = ReadFromHistory(pDelayHE, HT_CMD, HP_THIS, m_nCurrentCmdIndex);  
				
		ASSERT (SR_NO_MORE != ret);
	            
        // change existing delay command to actual delay so far
	            
		double elapsedSecs = (m_dDelayTotal - m_dDelayTimer) / 1e9 * m_dTimeRatio;
				
		sprintf(buff, "delay %g", elapsedSecs);
		pDelayHE->string = buff;
				
		// add another delay command for remaining delay not yet done
		double remainSecs = m_dDelayTimer / 1e9 * m_dTimeRatio;
		sprintf(buff, "delay %g", remainSecs);  
		pNewDelayHE = new CHistoryEntry;
		pNewDelayHE->string = buff;
		pNewDelayHE->type = HT_CMD;    
		m_nCurrentCmdIndex++;
		m_historyStrings.InsertAt(m_nCurrentCmdIndex, (void*) pNewDelayHE);    
		
		// change timers so now in new delay
		m_dDelayTimer = remainSecs * 1e9 / m_dTimeRatio;
		m_dDelayTotal = m_dDelayTimer;
				
		// insert new command between delays              
				
		m_historyStrings.InsertAt(m_nCurrentCmdIndex, (void*) pEntry);
				
        // and get current command to be doing the second half of the delay
	 	        
        m_nCurrentCmdIndex++;
	}
	else if (SM_REPLAY_PAUSED_PLAY == m_nReplayState)
    {
        // If script is paused, often because want user to read text just
        // before clearing screen. If want to add anything at that point,
        // would probably want it to appear before the pause not afterwards,
        // so insert before the pause not after.
        
        // Same could apply if script was paused and then user stopped script
        // and/or sim to insert something. However, don't try and trap this
        // here, because otherwise there is no way of putting in new commands
        // immediately after the pause, which they might also want to do.

        // So if they want to do more before the pause, they must draw objects
        // etc before breaking the pause, but if they want new things to happen
        // just after the pause, then they must stop the script first.

        m_historyStrings.InsertAt(m_nCurrentCmdIndex, (void*) pEntry);

        m_nCurrentCmdIndex++; // so pointing at pause command again
    }
    else
	{
		// general case; insert after current command            

		// put in delay since last command if sim running
			
		if (nSimWasRunningLastTime)
		{
			double delay = (m_dSimTime - m_dSimTimeLastCmd) / 1e9 * m_dTimeRatio;
			m_dSimTimeLastCmd = m_dSimTime;
		       
			sprintf(buff, "delay %g", delay);  
	
			pNewDelayHE = new CHistoryEntry;
			pNewDelayHE->string = buff;
			pNewDelayHE->type = HT_CMD;    
	
			m_historyStrings.InsertAt(++m_nCurrentCmdIndex, pNewDelayHE);
	    }
			
		m_historyStrings.InsertAt(++m_nCurrentCmdIndex, (void*) pEntry);
	}      
	
	nSimWasRunningLastTime = (SM_SIM_RUNNING == m_nSimState) ||
	                               (SM_SIM_PAUSED == m_nSimState);
}

int CSim::ReadFromHistory(CHistoryEntry*& pEntry,
	                      int type, int index, int& currentIndex)
{                                     
	int ret;
	int found = FALSE;
	
	if (m_historyStrings.GetSize() < 1)
	{
		// empty
		ret = SR_NO_MORE;
	}
	else
	{
		switch (index)
		{           
			case HP_THIS:
				// get the one pointed to by currentIndex 
				pEntry = (CHistoryEntry*) m_historyStrings[currentIndex];
				
				if (pEntry->type & type)  
				{
					ret = SR_OK;
				}   
				else
				{
					ret = SR_NO_MORE;
				}
				
				break;
				
			case HP_NEXT:
				// search for next string of this type
				for (index = currentIndex + 1;
				        index <=m_historyStrings.GetUpperBound(); index++)
				{
					pEntry = (CHistoryEntry*) m_historyStrings[index];
					
					if (pEntry->type & type)
					{
						ret = SR_OK;
						currentIndex = index;
						found = TRUE;
						break;
					}
				}   
				
				if (!found)
				{
					ret = SR_NO_MORE;
				}
				
				break;
				
			case HP_PREVIOUS:
				// search for previous string of this type
				break;
	
			case HP_FIRST:
				// find first string of this type
				for (index = 0;
				        index <= m_historyStrings.GetUpperBound(); index++)
				{
					pEntry = (CHistoryEntry*) m_historyStrings[index];
					
					if (pEntry->type & type)
					{
						ret = SR_OK;
						currentIndex = index;
						found = TRUE;
						break;
					}
				}   
				
				if (!found)
				{
					ret = SR_NO_MORE;
				}
				
				break;           
				
			case HP_LAST: 
				// find last string of this type   
				break;
				
			default:
				// index supplied was what actually wanted, return that
				// (make sure type correct)  
				if (index < 0 || index > m_historyStrings.GetUpperBound())
				{
					TRACE("out-of-bounds history string requested\n");
					ret = SR_ERROR;
				}
				else  
				{
					pEntry = (CHistoryEntry*) m_historyStrings[index];
					
					if (pEntry->type & type)
					{
						ret = SR_OK;
					}
					else
					{                  
						TRACE("history string requested of wrong type\n");
						ret = SR_ERROR;                                   
					}
				}
				break;
				
		} // end of switch (type) 

	} // end of if (empty history) else
		
	return ret;
}	                      
 

int CSim::HandleNewCommand(const char* commandString) 
{
	return HandleNewCommand(commandString, CMD_IMMEDIATE);
}

int CSim::HandleNewCommand(const char* commandString, int type)
{            
	m_currentCommand = commandString;

#ifdef _DEBUG                                  
	// Visual C crashes if try and give TRACE line longer than 255 chars
	CString sShort, sCopy;           
	sCopy = commandString;
	if (sCopy.GetLength() < 100)
	{
		TRACE("Accepted new command: %s\n", sCopy);
	}
	else
	{
		TRACE("Accepted new (long) command:\n");
		int len;
		while ((len = sCopy.GetLength()) > 0)
		{
			sShort = sCopy.Left(100);
			if (len > 100)
			{
				sCopy = sCopy.Right(len - 100);
			}
			else
			{
				sCopy = "";
		    }
			TRACE(">>   %s\n", sShort);
		}
	}
#endif
	
	CString workingLine = m_currentCommand;
	
	int ret;
	CString errString("");
	int nOldCmdMode = m_nCmdMode;
    int nNoHistory;
        
	ret = AcceptCommandLine(workingLine, errString, &nNoHistory);  

	CString string;	
	switch (ret)
	{             
		case SR_OK:
			// we have successfully done something. add to 
			// history unless rebuilding state or have entered state mode 
			if ((m_nCmdMode != SCM_STATE) && (m_nCmdMode == nOldCmdMode)) 
			{    
				if ((CMD_IMMEDIATE == type) && m_nIsRecording && !nNoHistory)
				{
					AddToHistory(m_currentCommand, HT_CMD);
				}
				else
				{
					// don't add to history commands that are being replayed from it
				}
			}
			break;

		case SR_WARN:       
			// as for successful command but add warning string
            
            TRACE("command just caused warning\n");
            
			if ((m_nCmdMode != SCM_STATE) && (m_nCmdMode == nOldCmdMode)) 
			{
				if (CMD_IMMEDIATE == type && !nNoHistory)
				{
					if (m_nIsRecording) AddToHistory(m_currentCommand, HT_CMD);
				}
				else
				{
					// don't add to history commands that are being replayed from it
				}

				string = "warning: ";
				string += errString;
				AddToHistory(string, HT_INFO);
			}  
			else
			{
				// warning from state part of script, which doesn't correspond
				// to history displayed...
				string = "line in state section of script: " + m_currentCommand;
				AddToHistory(string, HT_INFO);
				string = "caused warning: " + errString;				
				AddToHistory(string, HT_INFO);
			}
			
			break;
						
		case SR_ERROR:                             
		
			TRACE("command just caused error\n");
			
			if ((m_nCmdMode != SCM_STATE) && (m_nCmdMode == nOldCmdMode)) 
			{
				string = "***error: ";
				string += m_currentCommand;
				AddToHistory(string, HT_INFO);
				AddToHistory(errString, HT_INFO);
			}
			else
			{
				string = "line in state section of script: " + m_currentCommand;
				AddToHistory(string, HT_INFO);
				string = "caused error: " + errString;				
				AddToHistory(string, HT_INFO);
			}
			break;
			
		default:
			// shouldn't get here
			ASSERT(FALSE);
			break;
	}

    if (m_nSimState != SM_SIM_RUNNING)
    {
    	// In case last command changed something that could muck up
    	// interactions, list of numeric quantities that should be
    	// offered by monitor, etc (if sim running, this will be done
    	// anyway if m_nSimReady is FALSE).
		PrepareMonitors();   // in case new mol types to consider now
		PrepareTextboxes();  // in case quantities ref'd no longer exist, or vice versa 
	}
	    
    m_pDoc->UpdateAllViews(NULL);
    
	return ret;
}

CPoint CSim::SimToLogical(double x, double y)
{
	CPoint point;             
	double logical_x, logical_y;                   

	logical_x = (x * m_dScaleFactor - m_dXOffset);
	logical_y = (y * m_dScaleFactor - m_dYOffset); 
	                       
	// clip such that if outside integer range, end up in corner                       
	if (logical_x < INT_MIN)
	{
		point.x = INT_MIN;
	}
	else if (logical_x > INT_MAX)
	{
		point.x = INT_MIN; 
	}
	else 
	{
		point.x = (int) logical_x;
	}

	if (logical_y < INT_MIN)
	{
		point.y = INT_MIN;
	}
	else if (logical_y > INT_MAX)
	{
		point.y = INT_MIN; 
	}
	else 
	{
		point.y = (int) logical_y;
	}
	
	return point;
}

void CSim::LogicalToSim(CPoint point, double* x, double* y)
{                         
	ASSERT (fabs(m_dScaleFactor) > 1e-20);

	*x = (point.x + m_dXOffset) / m_dScaleFactor;
	*y = (point.y + m_dYOffset) / m_dScaleFactor;
}                                        

void CSim::Gridify(double* x, double* y)
{
	double mult; 
	
	ASSERT (fabs(m_dGridSize) > 1e-20); 

	mult = floor((*x / m_dGridSize) + 0.5);
	*x = mult * m_dGridSize;
	
	mult = floor((*y / m_dGridSize) + 0.5);
	*y = mult * m_dGridSize;
}                                

int CSim::OnGridSettings()
{
	CGridDlg dlg;

	dlg.m_nUseGrid = m_nUseGrid;
	dlg.m_dGridSize = m_dGridSize;
	
	int changed = FALSE;
	
	if (dlg.DoModal())
	{
		if (dlg.m_nUseGrid && !m_nUseGrid)
		{
			HandleNewCommand("grid_on 1");
			changed = TRUE;
		}
		else if (!dlg.m_nUseGrid && m_nUseGrid)
		{
			HandleNewCommand("grid_on 0");
			changed = TRUE;
		}             
		
		if (fabs(dlg.m_dGridSize - m_dGridSize) > NEARLY_ZERO)
		{
			sprintf(buff, "gridsize %g", dlg.m_dGridSize);
			HandleNewCommand(buff);
			changed = TRUE;
		}
    }	            
	
	return changed;
}



double CSim::RandomDirection()
{                   
	double p = GetRandom();   

	return p * 2 * PI;
}            

double CSim::CosineDistributionAngle()
{      
	// Return a random angle between -pi/2 and pi/2 such that if this
	// is called many times, p(theta) = k(cos theta).
	// The integral of cos x between -pi/2 and pi/2 is 2, so divide by
	// 2 to normalise the integrated probability distribution 
	//
	//    I(p) = (sin x - sin (-pi/2)) / 2
	//
	// Inverting this to get the angle as a function of the linearly
	// distributed probability p,
	//
	//    x = asin (2p - 1)
	                                             
		                                             
	double p = GetRandom();
	
	return (asin (2.0 * p - 1.0));
}


void CSim::Round(double& dNumber, double dPrecision)
{
	// Rounds numbers so that they display nicely in edit boxes etc
	
	double dMult = floor(dNumber / dPrecision + 0.5);
	
	dNumber = dMult * dPrecision;
} 

void CSim::Round(double& dNumber, int nSigFigs)
{
	double dMagnitude;          
	
	ASSERT(nSigFigs >= 1);
	
	if (dNumber > NEARLY_ZERO)
	{
		dMagnitude = log10(dNumber);
		Round(dNumber, pow(10, floor(dMagnitude - nSigFigs + 1)));
	}
	else if (dNumber < -NEARLY_ZERO)
	{
		dMagnitude = log10(-dNumber);
		Round(dNumber, pow(10, floor(dMagnitude - nSigFigs + 1)));
	}                               
	else
	{
		// to small, leave it be
	}
}


double CSim::ThermalSpeed(double temperature, double mass)
{                
	// Want to return a speed picked such that if a large number
	// are chosen, will build up a Maxwellian distribution for the
	// given temperature.
	// See Atkins' Physical Chemistry, 2nd ed, p868-9; in 2D, looks
	// like get nice form for f(v)dv that is integrable, by luck:
	
	// f(vx) = (m/2 pi k T)^0.5 exp (-m vx^2 / 2kT)
	//
	// Area element in 2D is 2 pi v dv (cf 4 pi v^2 dv vol in 3D)
	//
	// so f(v) = (m / 2 pi k T) . 2 pi v exp(-m vx^2 / 2kT) dv
	//
	// Integrating between 0 and v,
	//
	// integ(f(v)) = 1 - exp(- m v^2 / 2kT)     
	//
	// The integrated speed distribution, rising from 0 to 1,
	// becomes our linear probability scale p. Inverting the
	// above to get v(p):
	//
	// v = sqrt(-2kT/m ln (1 - p))
	//
	// and as luck would have it, the random number generator 
	// never returns 1.0.
	//
	// Note: this doesn't return the right distribution for desorption
	// from walls, because (looking at it the other way round)
	// walls are hit more often by faster-moving molecules,
	// which make a disproportionate contribution. We end up
	// with too low an average temperature if this formula is
	// used, as fast molecules will more quickly hit another
	// wall or molecule and slow down, whereas slow molecules
	// will stay slow in the gas phase for some time. Have not
	// yet worked out correct algorithm for thermal distribution
	// from wall.   
	//
	// Have thought about it now. Considered case where molecules all
	// heading rightwards towards wall with speed vel
	//
	//   f(v) = (m/kT).v.exp(-mv^2/2kt).dv
	//
	// as above (NB could be dodgy, maybe should be just x-vel distr)
	//
	// Want to find speed distribution f'(v) of mols that hit wall.
	// Consider those that hit in time t where 0 < t < 1. Want number
	// of mols of vel v0 < v < (v0 + dv) as proportion of total.
	// All mols with this vel range will hit wall up to a distance
	// away from wall of v0; beyond that they will not hit at t < 1.
	// Assuming unit area, number in this vel range
	//
	//  = integ between v0 and v0+dv (f(v)dv)
	//  = (-exp(-m.(v0+dv)^2 / 2kT  +  exp(-m.v0^2 / 2kT) 
	//
	// Using Taylor approx get
	//
	//  = v0.(m/kT) exp (-mv0^2/2kT)
	//
	// and multiplying this by distance for which these mols will hit, v0,
	// end up with this total num of mols in this vel range that hit in t < 1:
	//
	//  = v0^2.(m/kT) exp (-mv0^2/2kT)
	//
	// Total number of mols of all vels that hit in this time got by double
	// integration; as move further away from the wall (dist x) need to be
	// going at speed above x to still hit wall. So for one slice thickness
	// dx at dist x away from wall, contributes
	//
	//  = integ between x and infinity((m/kT).v.exp(-mv^2/2kT).dv)
	//  = exp(-m.x^2/2kT)
	//
	// and have to integrate this from x=0 to x=infinity to get total;
	// this is similar to erf(x), see Atkins p 867.
	//
	//  total num mols hit in t < 1 = (pi.kT/2m)^0.5
	//
	// So proportion of total mols in our speed range v0 < v < (v0+dv) is
	//
	//   = (m/kT)^1.5 * (2/pi)^0.5 * v0^2 * exp (-mv0^2/2kT) * dv
	//
	// but sadly this requires numerical integration! (Have done so in Excel
	// to ensure integrates to unity over range 0 to infinity). Will cop out
	// for time being and used fixed speed for wall desorption (ie every mol
	// gets same speed) -- see ThermalWallSpeed below.
	
	double p = GetRandom();
	
	// convert amu to kg
	mass *= ONE_AMU;
	
	double v;   
	double v2 = (-2 * KB * temperature / mass) * log (1 - p);
	
	if (v2 >= 0) // guard against rounding errors, seemed to be a problem
	{
		v = sqrt(v2);
	}
	else
	{	
		v = 0;
	}
	
	
	// clip output
	
	if (v > LIM_MAX_MOL_SPEED)
	{
		return LIM_MAX_MOL_SPEED;
	}
	else
	{
		return v;
	}
}          

double CSim::ThermalWallSpeed(double temperature, double mass)
{
	// We get a random number in interval (0, 1) and look up the
	// corresponding value of v in m_pdVelVsPForUnitMOverT which is
	// initialised below in BuildThermalWallTable, interpolating between
	// the bracketing pair of precalculated values. The table is
	// calculated for mass/temperature = 1.0 (amu/K), so we then
	// have to scale that velocity by sqrt(mass/temperature) (NB
	// I haven't proved that works, though it appears to numerically.)

	double p = GetRandom();

	p *= PSCALING;  // converts to array index of m_pdVelVsPForUnitMOverT
	
	double proportion = p - floor(p);  // fractional part of array index
	
	// calc array indices corresponding to the bracketing values of p
	// and ensure lie within array bounds. Lower bound should always
	// be within array, though could be last element, upper one might
	// not be so clip if it is off end of array (consider if PSCALING = 10,
	// p-values corresponding to the 10 elements of the array would be
	// {0, 0.1, .... , 0.9} and the random number routine always returns
	// a value < 1.0).
	
	int index1 = (int) p;
	int index2;
	
	if (index1 >= PSCALING)
	{
		ASSERT(FALSE);
		index1 = PSCALING - 1;
		index2 = PSCALING - 1;
	}
	else
	{
		index2 = index1 + 1;
		
		if (index2 >= PSCALING)
		{
			index2 = PSCALING - 1;
		}                         
	}
	
	// interpolate between bracketting values of v
	
	double v = (1 - proportion) * (m_pdVelVsPForUnitMOverT[index1])
	           + proportion * (m_pdVelVsPForUnitMOverT[index2]);
	           
	// scale answer to supplied m/T ratio 
	
	v *= sqrt(temperature / mass);
	
	return v;
}

void CSim::BuildThermalWallTable()
{
	// Here we tabulate solutions v to the integral equation
	//
	//  integ from 0 to v ((m/kT)^1.5 * (2/pi)^0.5 * v0^2 * exp (-mv0^2/2kT) * dv) = p
	//
	// to get a series of values v(m/T, p) required for ThermalWallSpeed.
	// We use m/T = 1 here, and it turns out that the values of v obtained
	// just scale as sqrt(m/T) which we can do at run-time. I haven't
	// proved the scaling by sqrt(m/T), just observed it numerically.
	
    double dv = 1.0;   // should be amply small     
    
    double constfac = pow(((double) ONE_AMU / KB), 1.5) * sqrt ((double) 2 / PI);
    double expfac = (double) ONE_AMU / -KB / 2.0;
    
    double v = dv;  
    double v2 = v * v;
    
    double f_at_v = constfac * v2 * exp(expfac * v2);
    double f_at_v_minus_dv = 0;                                
    
    double integ_at_v = f_at_v / 2 * dv;  // trapezoidal integration
    double integ_at_v_minus_dv = 0;
    
    int n = 0;  // p[n] = n / PSCALING
    double p;
    
    while (n < PSCALING)
    {
    	p = (double) n / PSCALING;
    	
    	if (p > integ_at_v)
    	{
    		f_at_v_minus_dv = f_at_v;
    		integ_at_v_minus_dv = integ_at_v;
    		v += dv;
    		v2 = v * v;
    		f_at_v = constfac * v2 * exp(expfac * v2);
    		integ_at_v = integ_at_v_minus_dv + 0.5 * (f_at_v_minus_dv + f_at_v) * dv;
    	} 
    	else
    	{
    		// p = value of integral between (v-dv) and v, interpolate
    		double proportion = (p - integ_at_v_minus_dv) / (integ_at_v - integ_at_v_minus_dv);
    		double vvalue = proportion * v + (1 - proportion) * (v - dv);
    		m_pdVelVsPForUnitMOverT[n] = vvalue;
    		n++;
    	}
    }
}

int CSim::GetIntsByObjTypeNames(CString& sObjType1, CString& sObjType2,
	              CPtrArray* pIntArray)
{
	int ret;
	
	CObjType* pType1;
	CObjType* pType2; 
	CObjType* pSpareType;
	
	pType1 = GetObjTypeByName(sObjType1);
	pType2 = GetObjTypeByName(sObjType2);  
	
	pIntArray->RemoveAll();
	
	if ((NULL == pType1) || (NULL == pType2))
	{
		// object type not found
		ret = SR_ERROR;
	}
	else
	{ 
		if (pType1->m_nBaseType > pType2->m_nBaseType)
		{
			// base types must be in ascending order
			pSpareType = pType1;
			pType1 = pType2;
			pType2 = pSpareType;
		}

		// search for allowed interactions 
		POSITION posn;
		CInt* pInt; 
		
		ret = SR_OK;
		    
		posn = m_intGallery.GetHeadPosition();
		while (NULL != posn)
		{   
			pInt = (CInt*) m_intGallery.GetNext(posn);  

			// null interaction works for all pairs, otherwise must match:
			if (pInt->m_nIsNull ||  
			((pInt->m_nBaseType1 == pType1->m_nBaseType)
			    && (pInt->m_nBaseType2 == pType2->m_nBaseType)))
			{
				// types match, this type of int OK for this pair of obj types
				pIntArray->Add(pInt);
			}
		}

	}
	
	return ret;
}           

CInt* CSim::GetDefaultIntByIndex(int nType1, int nType2)
{
	// Choose the default interaction for the specified combination of
	// types, if it exists, or else the null interaction if there is none.
	
	CInt* pDefInt = NULL;
	CInt* pNullInt = NULL;

	int spare;
	
	if (nType1 > nType2)
	{
		spare = nType1;
		nType1 = nType2;
		nType2 = spare;
	}
	
	POSITION posn;      
	CInt* pInt;           
	
	CObjType* pType1 = (CObjType*) m_typeArray[nType1];
	CObjType* pType2 = (CObjType*) m_typeArray[nType2];
	
		    
	posn = m_intGallery.GetHeadPosition();
	while (NULL != posn)
	{   
		pInt = (CInt*) m_intGallery.GetNext(posn);  

		// null interaction works for all pairs, otherwise must match:
		if (pInt->m_nIsNull)
		{
			pNullInt = pInt;
		}
		else if ((pInt->m_nBaseType1 == pType1->m_nBaseType)
		         && (pInt->m_nBaseType2 == pType2->m_nBaseType)
		         && pInt->m_nIsDefault)
		{
			// types match, this type of int OK for this pair of obj types 
			// and it is the default for this combination
			pDefInt = pInt;
		}
	}   
	
	// We should definitely have found the null int
	ASSERT(NULL != pNullInt);
	
	if (NULL != pDefInt)
	{
		return pDefInt;
	}
	else
	{
		return pNullInt;
	}

}

CInt* CSim::GetIntInUseByNames(CString& sObjType1, CString& sObjType2)
{
	int nType1 = GetObjTypeIndexByName(sObjType1);
	int nType2 = GetObjTypeIndexByName(sObjType2);

	if ((SR_NO_SUCH_TYPE != nType1) && (SR_NO_SUCH_TYPE != nType2))
	{
		return GetIntInUseByIndex(nType1, nType2);
	}
	else
	{
		return NULL;
	}
}	

CInt* CSim::GetIntByName(CString& sName)
{
	CInt* pInt;
	int found = FALSE;
	POSITION posn;
		    
	posn = m_intGallery.GetHeadPosition();
	while (NULL != posn)
	{   
		pInt = (CInt*) m_intGallery.GetNext(posn);  
		
		if (pInt->m_sName == sName)
		{
			found = TRUE;
			break;
		}

	}
	
	if (found)
	{
		return pInt;
	}
	else
	{
		return NULL;
	} 
}

CInt* CSim::GetIntInUseByIndex(int nType1, int nType2)
{                                
	int spare;

	if (nType1 > nType2)
	{
		spare = nType1;
		nType1 = nType2;
		nType2 = spare;
	}
	
	int i = m_intArray.GetIndex(nType1, nType2);
	return (CInt*) m_intArray[i];
}                         

void CSim::PrepareMonitors()
{
	// Monitors will have to set up their own lists of mol types,
	// initialise totals etc.       
	
	CObjType* pType;
	
	for (int i = 0; i <= m_typeArray.GetUpperBound(); i++)
	{
		pType = (CObjType*) m_typeArray[i];
		
		if (CI_MONITOR == pType->m_nBaseType)
		{
			((CObjTypeMonitor*) pType)->Prepare();
		}
	}
}

void CSim::PrepareTextboxes()
{
	// Once monitors sorted out, all numeric quantities should
	// be ready for referencing by textbox items.       
	
	CObjType* pType;
	
	for (int i = 0; i <= m_typeArray.GetUpperBound(); i++)
	{
		pType = (CObjType*) m_typeArray[i];
		
		if (CI_TEXTBOX == pType->m_nBaseType)
		{
			pType->Prepare();
		}
	}
}

void CSim::BuildIntCatLists()
{
	// We keep lists of all of the currently active interactions
	// that are soft (ie LJ) and hard, to quickly iterate through
	// when running the simulation. This avoids touching null interactions
	// or deciding which to do in what order during simulation.
	
	// First ensure the lists are empty
	DeleteIntCatLists();
	
	// Now iterate through triangular array of interactions
    CIntListEl* pCurrentHardEl = NULL;
    CIntListEl* pCurrentSoftEl = NULL; 
    CIntListEl* pCurrentNonNullEl = NULL; 
    
    CIntListEl* pNewEl;
    
    CInt* pInt;
    
#ifdef _DEBUG
	int numHards = 0;
	int numSofts = 0;
	int numNonNulls = 0;
#endif
    
	for (int i = 0; i <= m_intArray.GetUpperBound(); i++)
	{
		pInt = (CInt*) m_intArray[i];
		
		if (pInt->m_nIsHard)
		{   

#ifdef _DEBUG
			numHards++;
#endif

			pNewEl = new CIntListEl;
			pNewEl->m_pInt = pInt; 
			pNewEl->m_pNextEl = NULL;
			
			if (NULL != pCurrentHardEl)
			{                        
				// add to list
				pCurrentHardEl->m_pNextEl = pNewEl;
			}
			else
			{
				// first in list
				m_pFirstHardIntEl = pNewEl;
			}

			pCurrentHardEl = pNewEl;
		}	 

		if (pInt->m_nIsSoft)
		{

#ifdef _DEBUG
			numSofts++;
#endif
			pNewEl = new CIntListEl;
			pNewEl->m_pInt = pInt; 
			pNewEl->m_pNextEl = NULL;
			
			if (NULL != pCurrentSoftEl)
			{                        
				// add to list
				pCurrentSoftEl->m_pNextEl = pNewEl;
			}
			else
			{
				// first in list
				m_pFirstSoftIntEl = pNewEl;
			}

			pCurrentSoftEl = pNewEl;
		}

		if (!(pInt->m_nIsNull))
		{

#ifdef _DEBUG
			numNonNulls++;
#endif                 

			pNewEl = new CIntListEl;
			pNewEl->m_pInt = pInt; 
			pNewEl->m_pNextEl = NULL;
			
			if (NULL != pCurrentNonNullEl)
			{                        
				// add to list
				pCurrentNonNullEl->m_pNextEl = pNewEl;
			}
			else
			{
				// first in list
				m_pFirstNonNullIntEl = pNewEl;
			}

			pCurrentNonNullEl = pNewEl;
		}
	}	           

#ifdef _DEBUG	
	TRACE("Rebuilt IntCatList with %d hard, %d soft and %d non-null\n",
	        numHards, numSofts, numNonNulls);
#endif	        
}   

void CSim::DeleteIntCatLists()
{                 
	// Here we empty the linked lists of interactions kept by category
	// (hard, soft) for quick use by simulation
	
	CIntListEl* pLastIntListEl = m_pFirstSoftIntEl;
	m_pFirstSoftIntEl = NULL;
	CIntListEl* pNextIntListEl = NULL;
		
	if (NULL != pLastIntListEl)
	{
		pNextIntListEl = pLastIntListEl->m_pNextEl;
	}
			
	while (NULL != pNextIntListEl)
	{
		delete pLastIntListEl;
		pLastIntListEl = pNextIntListEl;
		pNextIntListEl = pNextIntListEl->m_pNextEl;
	}
		
	if (NULL != pLastIntListEl)
	{
		delete pLastIntListEl;
	}
    
    // Now repeat for the hard interactions
    
	pLastIntListEl = m_pFirstHardIntEl;
	m_pFirstHardIntEl = NULL;
	pNextIntListEl = NULL;
		
	if (NULL != pLastIntListEl)
	{
		pNextIntListEl = pLastIntListEl->m_pNextEl;
	}
			
	while (NULL != pNextIntListEl)
	{
		delete pLastIntListEl;
		pLastIntListEl = pNextIntListEl;
		pNextIntListEl = pNextIntListEl->m_pNextEl;
	}
		
	if (NULL != pLastIntListEl)
	{
		delete pLastIntListEl;
	}


    // And for the non-null interactions
    
	pLastIntListEl = m_pFirstNonNullIntEl;
	m_pFirstNonNullIntEl = NULL;
	pNextIntListEl = NULL;
		
	if (NULL != pLastIntListEl)
	{
		pNextIntListEl = pLastIntListEl->m_pNextEl;
	}
			
	while (NULL != pNextIntListEl)
	{
		delete pLastIntListEl;
		pLastIntListEl = pNextIntListEl;
		pNextIntListEl = pNextIntListEl->m_pNextEl;
	}
		
	if (NULL != pLastIntListEl)
	{
		delete pLastIntListEl;
	}

}

void CSim::BuildTypeCatLists()
{
	// We keep a list of all object types that are mobile, so that we
	// can iterate only through those types when required; similarly
	// a list of monitor types, as they are required every timestep
	// for update
	
	// First ensure the lists are empty
	DeleteTypeCatLists();
	
	// Now iterate through type array
    CTypeListEl* pCurrentMobileEl = NULL;  
    CTypeListEl* pCurrentMonEl = NULL;    
    CTypeListEl* pCurrentGraphEl = NULL;
    CTypeListEl* pNewEl;

#ifdef _DEBUG
	int nMobiles = 0;
#endif 
    
    CObjType* pType;
    
	for (int i = 0; i <= m_typeArray.GetUpperBound(); i++)
	{
		pType = (CObjType*) m_typeArray[i];
		
		if (pType->m_nIsMobile)
		{              

#ifdef _DEBUG
			nMobiles++; 
#endif 
 
			pNewEl = new CTypeListEl;
			pNewEl->m_pType = pType; 
			pNewEl->m_pNextEl = NULL;
			
			if (NULL != pCurrentMobileEl)
			{                        
				// add to list
				pCurrentMobileEl->m_pNextEl = pNewEl;
			}
			else
			{
				// first in list
				m_pFirstMobileTypeEl = pNewEl;
			}

			pCurrentMobileEl = pNewEl;
		}

		if (CI_MONITOR == pType->m_nBaseType)
		{              
			pNewEl = new CTypeListEl;
			pNewEl->m_pType = pType; 
			pNewEl->m_pNextEl = NULL;
			
			if (NULL != pCurrentMonEl)
			{                        
				// add to list
				pCurrentMonEl->m_pNextEl = pNewEl;
			}
			else
			{
				// first in list
				m_pFirstMonitorTypeEl = pNewEl;
			}

			pCurrentMonEl = pNewEl;
		}

		if (CI_GRAPH == pType->m_nBaseType)
		{              
			pNewEl = new CTypeListEl;
			pNewEl->m_pType = pType; 
			pNewEl->m_pNextEl = NULL;
			
			if (NULL != pCurrentGraphEl)
			{                        
				// add to list
				pCurrentGraphEl->m_pNextEl = pNewEl;
			}
			else
			{
				// first in list
				m_pFirstGraphTypeEl = pNewEl;
			}

			pCurrentGraphEl = pNewEl;
		}
			 
	}

#ifdef _DEBUG		
	TRACE("Rebuilt TypeCatList with %d mobile types\n", nMobiles);
#endif
}   

void CSim::DeleteTypeCatLists()
{                 
	// Here we empty the linked lists of object types stored
	// by category

	// first do list of mobile types 
	
	// NB This code looks a bit unnecessarily complicated, should
	// tidy it up one day
		
	CTypeListEl* pLastEl = m_pFirstMobileTypeEl;
	m_pFirstMobileTypeEl = NULL;
	CTypeListEl* pNextEl = NULL;
		
	if (NULL != pLastEl)
	{
		pNextEl = pLastEl->m_pNextEl;
	}
			
	while (NULL != pNextEl)
	{
		delete pLastEl;
		pLastEl = pNextEl;
		pNextEl = pNextEl->m_pNextEl;
	}
		
	if (NULL != pLastEl)
	{
		delete pLastEl;
	}
    
    
    // now list of monitor types
    
	pLastEl = m_pFirstMonitorTypeEl;
	m_pFirstMonitorTypeEl = NULL;
	pNextEl = NULL;
		
	if (NULL != pLastEl)
	{
		pNextEl = pLastEl->m_pNextEl;
	}
			
	while (NULL != pNextEl)
	{
		delete pLastEl;
		pLastEl = pNextEl;
		pNextEl = pNextEl->m_pNextEl;
	}
		
	if (NULL != pLastEl)
	{
		delete pLastEl;
	}

    // and list of graph types
    
	pLastEl = m_pFirstGraphTypeEl;
	m_pFirstGraphTypeEl = NULL;
	pNextEl = NULL;
		
	if (NULL != pLastEl)
	{
		pNextEl = pLastEl->m_pNextEl;
	}
			
	while (NULL != pNextEl)
	{
		delete pLastEl;
		pLastEl = pNextEl;
		pNextEl = pNextEl->m_pNextEl;
	}
		
	if (NULL != pLastEl)
	{
		delete pLastEl;
	}
    
}

CString CSim::GetBaseTypeName(int nBaseType)
{
	CMD_ENTRY* celList = GetCmdList(); 
	
	CString sReturn;
	int nFound = FALSE;
	int i = 0;
		
	while (!nFound && CI_ENDLIST != celList[i].cmd_id)
	{         
		if (celList[i].cmd_id == nBaseType)
		{  
			sReturn = celList[i].command;  
			nFound = TRUE;   
		}       
		i++;
	}

	return sReturn;
}

CObjType* CSim::GetObjTypeByName(CString& objTypeName)
{
	CObjType* pType; 
	int index;  
	int match = FALSE;

	for (index = 0; index <= m_typeArray.GetUpperBound(); index++)
	{
		pType = (CObjType*) m_typeArray[index]; 
		if (pType->m_sName == objTypeName)
		{
			match = TRUE;
			break;
		}
	}
	
	if (match)
	{
		return pType;
	}
	else
	{
		return NULL;
	}

}
               
void CSim::ScanHistoryForUsedIds(CObjType* pType)
{
	// Let's say we're replaying history and have just created a wall
	// type called 'wall1'. This function will scan through history from
	// the current point forward looking to see what id numbers have been
	// assigned to any CSObjWalls of this type, and the highest used will
	// be told to the new wall type so that it can give any new CSObjWalls
	// made during replay (with recording) unique ids. That way, any future
	// reference to CSObjWalls will get the right one (for deletion etc).
	
	// The commands we're interested in are:
	//  
	//     wall wall1 x1 4 x2.....  (makes new wall, so check for id)
	//
	//     deltype wall1            (end of our search, type deleted)
	//     reset                    (end of our search, type deleted) 
	//
	//     fillmol       (only in case of mol types)
	
	CHistoryEntry* pHE = NULL;
	int nHistoryIndex = m_nCurrentCmdIndex;
    int ret;  // to see when history exhausted 
    
    int nFinished = FALSE;
    
    do
    {
		ret = ReadFromHistory(pHE, HT_CMD, HP_NEXT, nHistoryIndex);
		
		if (SR_NO_MORE != ret)
		{
			// check for end of this obj type, and if not look for new ids
			
			if (MatchCommand(pHE->string, CI_RESET))
			{
				// all types destroyed so search finished
				
				nFinished = TRUE;
			}
			else if ((MatchCommand(pHE->string, CI_DELTYPE))
			                  && (m_sArg == pType->m_sName))	
			{   
				// this particular type destroyed so search finished
				
				nFinished = TRUE;
			}          
			else if ((MatchCommand(pHE->string, pType->m_nBaseType))
			              && (m_sArg == pType->m_sName))
			{
				// A new CSObj of this type is being created in this command,
				// so check value of id
				
				if (FindGrabCommand(pHE->string, CI_ID) && (m_lArg > pType->m_lLastIdUsed))
				{
					pType->m_lLastIdUsed = m_lArg;
				}
			}        
			else if ((CI_MOL == pType->m_nBaseType)
			          && MatchCommand(pHE->string, CI_FILLMOL)
			           && (m_sArg == pType->m_sName))
			{
				// Special case of fillmol command, check last id if specified
				
				if (FindGrabCommand(pHE->string, CI_LAST_ID) && (m_lArg > pType->m_lLastIdUsed))
				{
					pType->m_lLastIdUsed = m_lArg;
				}
			}	
		}
		else
		{
			// history finished
			nFinished = TRUE;
		}

	}
	while (!nFinished);
}

void CSim::ScanHistoryForMonitorIds()
{
	// Monitors are special; they can make molecules by themselves, and those
	// molecules will be given ids related to the unique monitor id so that even
	// if the monitor makes an indeterminate number of mols, those created manually
	// will get reproducible ids for selection.
	
	// The commands we're interested in are:
	//  
	//     monitor <any type> ..... mon_uid 8 (makes new monitor, so check for id)
	
	CHistoryEntry* pHE = NULL;
	int nHistoryIndex = -1;
    int ret;  // to see when history exhausted 
	
	ret = ReadFromHistory(pHE, HT_CMD, HP_FIRST, nHistoryIndex);  
	
	while (SR_NO_MORE != ret)
    {
		if (MatchCommand(pHE->string, CI_MONITOR))
		{                                         
			// found a monitor creation command so check for explicit id:
			
			FindGrabCommand(pHE->string, CI_MON_UID);
				
			if (m_lArg > m_lLastMonIdUsed) m_lLastMonIdUsed = m_lArg;
		}                                                         
		
		ret = ReadFromHistory(pHE, HT_CMD, HP_NEXT, nHistoryIndex);  
	}

}
               
void CSim::GetTypeArray(CPtrArray& array, int baseType)
{
	// fills array supplied with pointers to objects of specified type
	
	// we don't clear out the array, so this can be called twice with
	// different types to get a list of both

	CObjType* pType; 
	int index; 
	
	for (index = 0; index <= m_typeArray.GetUpperBound(); index++)
	{
		pType = (CObjType*) m_typeArray[index]; 
		if (pType->m_nBaseType == baseType)
		{
			array.Add(pType);
		}
	}

}

void CSim::MakeUniqueTypeName(CString& sName)
{   
	// For example, if given 'molecule', add digit such
	// that becomes unique name like 'molecule3'
	
	int i = 1;
	
	CString baseName(sName);
	CObjType* pExistingType = NULL;
	
	do
	{
		sprintf(buff, "%d", i);
		sName = baseName + buff;
		pExistingType = GetObjTypeByName(sName);
		i++;
	}
	while (NULL != pExistingType);
}

int CSim::GetObjTypeIndexByName(CString& objTypeName)
{
	int index;  
	int match = FALSE; 
	CObjType* pType;

	for (index = 0; index <= m_typeArray.GetUpperBound(); index++)
	{
		pType = (CObjType*) m_typeArray[index]; 
		if (pType->m_sName == objTypeName)
		{
			match = TRUE;
			break;
		}
	}
	
	if (match)
	{
		return index;
	}
	else
	{
		return SR_NO_SUCH_TYPE;
	}
}

int CSim::GetObjTypeIndexByPtr(CObjType* pObjType)
{
	int index;  
	int match = FALSE; 
	CObjType* pType;

	for (index = 0; index <= m_typeArray.GetUpperBound(); index++)
	{
		pType = (CObjType*) m_typeArray[index]; 
		if (pType == pObjType)
		{
			match = TRUE;
			break;
		}
	}
	
	if (match)
	{
		return index;
	}
	else
	{
		return SR_NO_SUCH_TYPE;
	}
}

void CSim::InsertNode(CNP* pNP)
{          
	// Used to insert a new neighbour pair object into the tree
	// of collision events in the place appropriate for its time
	
#ifdef _DEBUG
	m_lNumNodes++;
#endif

	CNP* pSearchNP = m_pRootNode;
	CNP* pOriginNP;
	
	ASSERT(NULL == pNP->m_pEarlierNode);
	ASSERT(NULL == pNP->m_pLaterNode); 
	ASSERT(NULL == pNP->m_pParentNode);
	ASSERT(NR_NOT_IN_TREE == pNP->m_nParentRelationship);
	
	if (NULL == pSearchNP)
	{ 
		// this node becomes the root node
		m_pRootNode = pNP;  
		pNP->m_pParentNode = NULL;     
		pNP->m_nParentRelationship = NR_ROOT;
		return;
	}
	
	int earlier;
	
	do
	{ 
		pOriginNP = pSearchNP;
		if (pNP->m_dTimeDue < pOriginNP->m_dTimeDue)
		{
			pSearchNP = pSearchNP->m_pEarlierNode; 
			earlier = TRUE;
		}
		else
		{
			pSearchNP = pSearchNP->m_pLaterNode; 
			earlier = FALSE;
		}  
		
	}   
	while (NULL != pSearchNP);
	
	if (earlier)
	{
		pOriginNP->m_pEarlierNode = pNP;
		pNP->m_nParentRelationship = NR_EARLIER;
	}
	else
	{
		pOriginNP->m_pLaterNode = pNP;
		pNP->m_nParentRelationship = NR_LATER;
	}                              
	
	pNP->m_pParentNode = pOriginNP; 
	ASSERT(NR_NOT_IN_TREE != pNP->m_nParentRelationship);
}

void CSim::DetachNode(CNP* pNP)
{                
	// Remove the specified node from the hard event tree
    
#ifdef _DEBUG
	m_lNumNodes--;
#endif

	if (NULL == pNP->m_pEarlierNode)
	{
		if (NULL == pNP->m_pLaterNode)
		{
			// has no children
			switch (pNP->m_nParentRelationship)
			{
				case NR_LATER:
					pNP->m_pParentNode->m_pLaterNode = NULL;
					break;
					
				case NR_EARLIER:
					pNP->m_pParentNode->m_pEarlierNode = NULL;
					break;
					
				case NR_ROOT:
					m_pRootNode = NULL;
					break;
					
				default:
					ASSERT(FALSE);
					break;
			}
		}
		else
		{
			// only has a later child, which can replace it 
			switch (pNP->m_nParentRelationship)
			{
				case NR_LATER:
					pNP->m_pParentNode->m_pLaterNode = pNP->m_pLaterNode;
					pNP->m_pLaterNode->m_pParentNode = pNP->m_pParentNode;
					break;
					
				case NR_EARLIER:
					pNP->m_pParentNode->m_pEarlierNode = pNP->m_pLaterNode;
					pNP->m_pLaterNode->m_nParentRelationship = NR_EARLIER;  
					pNP->m_pLaterNode->m_pParentNode = pNP->m_pParentNode;
					break;
					
				case NR_ROOT:
					m_pRootNode = pNP->m_pLaterNode;
					m_pRootNode->m_nParentRelationship = NR_ROOT; 
					m_pRootNode->m_pParentNode = NULL;
					break;
					
				default:
					ASSERT(FALSE);
					break;
			}    
		}
	}
	else
	{
		// does have an earlier node
		if (NULL == pNP->m_pLaterNode)
		{
			// has only an earlier node, which can replace it 
			switch (pNP->m_nParentRelationship)
			{
				case NR_LATER:
					pNP->m_pParentNode->m_pLaterNode = pNP->m_pEarlierNode;
					pNP->m_pEarlierNode->m_nParentRelationship = NR_LATER;    
					pNP->m_pEarlierNode->m_pParentNode = pNP->m_pParentNode;
					break;
					
				case NR_EARLIER:
					pNP->m_pParentNode->m_pEarlierNode = pNP->m_pEarlierNode;
					pNP->m_pEarlierNode->m_pParentNode = pNP->m_pParentNode;
					break;
					
				case NR_ROOT:
					m_pRootNode = pNP->m_pEarlierNode;
					m_pRootNode->m_nParentRelationship = NR_ROOT; 
					m_pRootNode->m_pParentNode = NULL;
					break;
					
				default:
					ASSERT(FALSE);
					break;
			}
		}
		else
		{
			// Has both children; replace it with earlier, and reinsert later
			// on latest free node of earlier's children           
			switch (pNP->m_nParentRelationship)
			{
				case NR_LATER:
					pNP->m_pParentNode->m_pLaterNode = pNP->m_pEarlierNode;  
					pNP->m_pEarlierNode->m_nParentRelationship = NR_LATER; 
					pNP->m_pEarlierNode->m_pParentNode = pNP->m_pParentNode;
					break;
					
				case NR_EARLIER:
					pNP->m_pParentNode->m_pEarlierNode = pNP->m_pEarlierNode;
					pNP->m_pEarlierNode->m_pParentNode = pNP->m_pParentNode;
					break;
					
				case NR_ROOT:
					m_pRootNode = pNP->m_pEarlierNode;
					m_pRootNode->m_nParentRelationship = NR_ROOT; 
					m_pRootNode->m_pParentNode = NULL;
					break;
					
				default:
					ASSERT(FALSE);
					break;
			} 
			
			CNP* pSearchNP = pNP->m_pEarlierNode;
			CNP* pOriginNP;
			
			do
			{                  
				pOriginNP = pSearchNP;
				pSearchNP = pOriginNP->m_pLaterNode;
			}
			while (NULL != pSearchNP);  
			
			pOriginNP->m_pLaterNode = pNP->m_pLaterNode;
			pNP->m_pLaterNode->m_pParentNode = pOriginNP;
		}
	}	    
	
	pNP->m_nParentRelationship = NR_NOT_IN_TREE;  
	pNP->m_pParentNode = NULL;
	pNP->m_pEarlierNode = NULL;
	pNP->m_pLaterNode = NULL;
}

void CSim::ExecHardEvents()
{               
	DWORD tStart = GetTickCount(); 
	int nWaitCursorSet = FALSE;
	
	// Here we work through all hard events in the tree from earliest
	// to latest, actually doing the moves. If a mobile object is moved,
	// any later events it is involved in will have to be recalculated.

	// Get local pointer to flage held by app that indicates whether this CSim has
	// in fact been destroyed, as windows messages processed during long calculations
	// may destroy us and if so we will get a GPF if we then try to access our own
	// member variables, although we don't lose program control!    
    volatile BYTE* pExistenceFlag = &(theApp.m_simExistenceFlags[m_nAppExistenceFlagIndex]);

	m_nDoingHardEvents = TRUE;
	
	CNP* pNP; 
	CNP* pSearchNP;  
	int nEvents = 0;
	
	while (NULL != m_pRootNode)
	{
		// Get earliest node
		pSearchNP = m_pRootNode;
		
		do
		{                  
			pNP = pSearchNP;
			pSearchNP = pNP->m_pEarlierNode;
		}
		while (NULL != pSearchNP);
		
		ASSERT(NR_NOT_IN_TREE != pNP->m_nParentRelationship);

		DetachNode(pNP);
		
		// update time we have got to within this timestep
		m_dTimeSoFar = pNP->m_dTimeDue;
		
		// actually do move or other action, and recalc events as reqd
		pNP->m_pInt->ExecHardEvent(pNP);   
		
		nEvents++;
		
		if (nEvents > 10)
		{               
			// May end up doing an awful lot of hard collisions if we
			// have a close-packed block of mols, say; if so, yield some
			// time so user doesn't think we have hung.
			// See "Inside Visual C++" pp 202-5.
			
			MSG Message;
			
			if (::PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&Message);
				::DispatchMessage(&Message);  
			}           
            
            if (!(*pExistenceFlag))
            {
            	// This CSim now destroyed! 
            	break;
            } 
            
            // set wait cursor if taking a long time
            if ((GetTickCount() - tStart > 1000) && !nWaitCursorSet)
            {
            	theApp.BeginWaitCursor();     
            	nWaitCursorSet = TRUE;
            }
            
			nEvents = 0;
		}
	}

	m_nDoingHardEvents = FALSE;	
    
    if (nWaitCursorSet)
    {
    	theApp.EndWaitCursor();          
    }
    
	return;   // no hard events left this timestep
}

BOOL CSim::OnIdle()
{   
	static DWORD lasttime = 0;
	static DWORD screentime;  // ticks at which to redraw if sim not running

	// Get local pointer to flag held by app that indicates whether this CSim has
	// in fact been destroyed, as windows messages processed during long calculations
	// may destroy us and if so we will get a GPF if we then try to access our own
	// member variables, although we don't lose program control!    
    volatile BYTE* pExistenceFlag = &(theApp.m_simExistenceFlags[m_nAppExistenceFlagIndex]);
		
	DWORD timenow;
	
	timenow = GetTickCount(); // in ms 
	
	if (0 == lasttime)
	{
		screentime = timenow;
		lasttime = timenow;
		return TRUE;
	}
		            
	m_dTimeThisLoop = 0;
		    
	if (SM_SIM_RUNNING == m_nSimState)
	{
		// just return if we have looped round faster than we
		// need to to match the timescale we want   
		if (timenow < m_lNextLoopStartTicks)
		{
			return TRUE;
		}


		// Run a bit of simulation 
		
		// Do initialisation of simulation engine if reqd
		
		if (!m_nSimReady)
		{  
			PrepareSim();
		}
		   
		   
		ChooseDeltaTime();
		
    	for (long i = 1; ((GetTickCount() < timenow + 500)
    	                 && (i <= m_lStepsBetweenUpdates)); i++)
    	{
			SingleStep();
			
			if (!(*pExistenceFlag))
			{
				// CSim has been destroyed!
				return FALSE;
			}   
		}      
			
		m_lNextLoopStartTicks = timenow 
		         + (DWORD) (m_dTimeThisLoop * m_dTimeRatio / 1e6);
		         
		m_dSimTime += m_dTimeThisLoop;
        global_debug_sim_time = m_dSimTime;

		m_pDoc->UpdateAllViews(NULL);

	} 
	else
	{   
		if ((SM_SIM_PAUSED != m_nSimState) &&
		    ((SM_REPLAY_PLAYING == m_nReplayState) || 
		     (SM_REPLAY_FFWDING == m_nReplayState)))
		{
			// sim time stopped, but script time still rolls
				
			//        ns  +=  (ms - ms) * 1e6
			m_dTimeThisLoop = (timenow - lasttime) * 1e6 / m_dTimeRatio; 
			if (timenow >= screentime)
			{
				m_pDoc->UpdateAllViews(NULL);
					
				screentime = timenow + m_lIdleScreenUpdateMs;
			}	
		}
	}    

	if (((SM_REPLAY_PLAYING == m_nReplayState)
	    || (SM_REPLAY_FFWDING == m_nReplayState))
	     && (SM_SIM_PAUSED != m_nSimState))
	{          
		// if in a delay loop, just count that down; otherwise,
		// execute commands from history; we stop time only if part-way
		// through drawing something, when sim 'paused'
		
		m_dScriptTime += m_dTimeThisLoop;

		CHistoryEntry* pHE;
		int ret;
		
		if (m_nDoingDelay)
		{
			m_dDelayTimer -= m_dTimeThisLoop;           
			if (m_dDelayTimer < 0) m_dDelayTimer = 0;
			
			m_nDoingDelay = (m_dDelayTimer > NEARLY_ZERO);
			
			if (!m_nDoingDelay)
			{
				// finished delay so fetch next command
				ret = ReadFromHistory(pHE, HT_CMD, HP_NEXT, m_nCurrentCmdIndex);
			}
		}
		else
		{                                    
			ret = ReadFromHistory(pHE, HT_CMD, HP_THIS, m_nCurrentCmdIndex);
			
			while (!m_nDoingDelay && (SR_NO_MORE != ret) 
			        && ((SM_REPLAY_PLAYING == m_nReplayState) 
			         || (SM_REPLAY_FFWDING == m_nReplayState)))
			{      
				if (SR_ERROR == HandleNewCommand(pHE->string, CMD_FROM_SCRIPT))
				{
					AfxMessageBox("An error occurred while replaying a stored"
					              " command; see command console for details."
					              " Replay will continue.");
				}
				
				// Command may have been tinkered with for our own good, in that 'id 8'
				// or similar may have been added. Copy it back in case this is true:
				pHE->string = m_currentCommand;
				
				
				if (!m_nDoingDelay 
                    && (SM_REPLAY_PAUSED_PLAY != m_nReplayState)
                    && (SM_REPLAY_PAUSED_STOP != m_nReplayState))
				{
					// finished last command so try and step onto next
	            	ret = ReadFromHistory(pHE, HT_CMD, HP_NEXT, m_nCurrentCmdIndex);
	            }
			}
			
			if (SR_NO_MORE == ret)
			{
				// finished script so get into same state as if had been interactive
				m_nReplayState = SM_REPLAY_STOPPED;  
				m_dSimTimeLastCmd = m_dSimTime;
			}
		}
				
		if (SR_NO_MORE == ret)
		{
			m_nReplayState = SM_REPLAY_STOPPED;
		}
	}
	
	
	lasttime = timenow;
	
	return TRUE; // always want to get called again
}               

void CSim::PrepareSim()
{
	// Simulation may have been interrupted by drawing and the like,
	// so assume everything invalid and rebuild the context.
	
	TRACE("CSim::PrepareSim\n");
	
	m_dSafetyDistLeft = -1e-6;  // force neighbour list rebuild
	
	BuildTypeCatLists();  // make lists of mobile objects etc  
	BuildIntCatLists();  // make lists of soft and hard ints 
	PrepareMonitors();   // get them to set selves up
	PrepareTextboxes();  // now monitors ready to be referred to 
	
	m_nSimReady = TRUE; 
}           

void CSim::ChooseDeltaTime()
{
	if (m_dUseAutoDeltaTime >= 0.5)
	{	        
		m_dUseAutoDeltaTime = 1;  // so snaps to 1 if altered using slider
		
		if (NULL == m_pFirstSoftIntEl)
		{
			// no soft ints, go in bigger steps 
			m_dDeltaTime = 1e-5;         
		}
		else
		{
			// soft ints, go in little steps to conserve energy    
			m_dDeltaTime = 1e-5; // max will ever use for soft interactions
			
			CIntListEl* pIntEl = m_pFirstSoftIntEl; 
			
			CLennardJonesInt* pInt;
			double dMinMass;
			double dOtherMass;
			double dCalcDT;
			
			while (NULL != pIntEl)
			{
				pInt = (CLennardJonesInt*) pIntEl->m_pInt;
				
				// Only have LJ soft ints for time being, but in case of future additions:
				ASSERT(pInt->m_sName == "lennard_jones_int");

				dMinMass = ((CObjTypeMol*) pInt->m_pType1)->m_dMass;
				dOtherMass = ((CObjTypeMol*) pInt->m_pType2)->m_dMass;
				
				if (dOtherMass < dMinMass)
				{
					dMinMass = dOtherMass;
				}
				
				dCalcDT = 1.5e-5 * 
				          (pInt->m_dEqmSepn / 2) *
				          sqrt(dMinMass) *
				          exp(-pInt->m_dWellDepth / 30);
				
				Round(dCalcDT, 1e-9);
				          
				if (dCalcDT < m_dDeltaTime)
				{
					m_dDeltaTime = dCalcDT;
				}

				pIntEl = pIntEl->m_pNextEl;                                         
			}
		}   
	}
	else
	{                                                   
		m_dUseAutoDeltaTime = 0;  // so snaps to 0 if altered using slider
		
		// retain user setting of delta time
	}   

	// try and cover same amount of simulated time between screen updates
	// whatever (but if taking too long, will update screen more often anyway):	
	m_lStepsBetweenUpdates = (long) ceil(5e-5 / m_dDeltaTime);
                                     
	
	// The insurance distance is the furthest together that two objects
	// may approach in the next timestep, and the safety distance is the
	// distance between objs we use in deciding whether or not to include
	// them as neighbour pairs.
	                                    
	//    nm      =      ns        *     m/s    [as nm/ns == m/s]	
	m_dInsuranceDist = m_dDeltaTime * 2 * LIM_MAX_MOL_SPEED;
	m_dSafetyDist = m_dInsuranceDist;
	
	if (m_dSafetyDist < SIM_PREF_SAFETY_DIST)
	{
		m_dSafetyDist = SIM_PREF_SAFETY_DIST;
	}
}

void CSim::SingleStep()
{
#ifdef _DEBUG
	global_debug_counter++;
#endif

	// Get local pointer to flag held by app that indicates whether this CSim has
	// in fact been destroyed, as windows messages processed during long calculations
	// may destroy us and if so we will get a GPF if we then try to access our own
	// member variables, although we don't lose program control!    
    volatile BYTE* pExistenceFlag = &(theApp.m_simExistenceFlags[m_nAppExistenceFlagIndex]);
    
    
	CTypeListEl* pTypeEl;  
	CIntListEl* pIntEl;
	CSObj* pSObj;
	
	ASSERT(0 == m_lNumNodes);
	

	// Find fastest-moving object and zero all accelerations 
	m_dMaxVSquared = 0;                 
	m_dTimeSoFar = 0;
	double dMaxVSquared;
	pTypeEl = m_pFirstMobileTypeEl;
	while (NULL != pTypeEl)
	{
		dMaxVSquared = pTypeEl->m_pType->PrepStep(); 
		if (dMaxVSquared > m_dMaxVSquared)
		{
			m_dMaxVSquared = dMaxVSquared;
		}
		
		pTypeEl = pTypeEl->m_pNextEl;
	}

			                                                                           
	// Rebuild neighbour lists if may have moved far enough to invalidate them 
	m_dSafetyDistLeft -= sqrt(m_dMaxVSquared) * m_dDeltaTime * 2;
	if (m_dSafetyDistLeft <= m_dInsuranceDist)
	{                                
		// first disconnect chains of handles attached to specific objects
		pTypeEl = m_pFirstMobileTypeEl;
		while (NULL != pTypeEl)
		{
			pSObj = pTypeEl->m_pType->m_pFirstSObj;
			while (NULL != pSObj)
			{
				pSObj->m_pFirstHandle = NULL;
				pSObj = pSObj->m_pNextSObj;
			}
						
			pTypeEl = pTypeEl->m_pNextEl;
		}

        // then rebuild the neighbour list
		pIntEl = m_pFirstNonNullIntEl;
		while (NULL != pIntEl)
		{
			pIntEl->m_pInt->BuildNeighList();
			
			pIntEl = pIntEl->m_pNextEl;
		}                                      
		
		m_dSafetyDistLeft = m_dSafetyDist;
	}


	// Apply soft interaction forces to accelerate objects
	pIntEl = m_pFirstSoftIntEl;
	while (NULL != pIntEl)
	{   
		pIntEl->m_pInt->CalcForces(); 
		                 
		pIntEl = pIntEl->m_pNextEl;
	}      
    
    // apply accelerations from these forces and add gravity (if any)
    m_nGravityOn = (fabs(m_dGravitationalAcceleration) > NEARLY_ZERO);                 
    
    //        m/s/timestep        =   m/s2/ns * ns * (s/ns)
    m_dGravitationalYVelIncrement = -m_dGravitationalAcceleration * m_dDeltaTime;

    
	pTypeEl = m_pFirstMobileTypeEl;
	while (NULL != pTypeEl)
	{
		pTypeEl->m_pType->ApplyAccels();
		pTypeEl = pTypeEl->m_pNextEl;
	}

		
	// Do hard interactions, moving objects that far; first go through
	// getting earliest event for every object that might suffer a hard
	// interaction:


	ASSERT(0 == m_lNumNodes); 
    
	pIntEl = m_pFirstHardIntEl;
	while (NULL != pIntEl)
	{   
		pIntEl->m_pInt->CalcAllHardEvents(); 
		                 
		pIntEl = pIntEl->m_pNextEl;
	}      
	
		
	// Now do all the hard events in sequence, recalculating all affected
	// events as necessary. If hard event happens which invalidates all events
    // after that, will synchronise to time of that event and set
    // m_nSynchroniseHappened. Otherwise all events within the timestep will
    // be dealt with but there will be a little bit of time between the
    // last event actioned and the end of the timestep, for which all objects
    // must be moved.

    m_nSynchronizeHappened = FALSE;	

    ExecHardEvents(); 
    
    if (!(*pExistenceFlag))
    {
    	// This CSim has been destroyed!
    	return;
    }          

   	ASSERT(0 == m_lNumNodes);
	ASSERT(NULL == m_pRootNode); // should be none left

	// Do remaining moves if sim still running and synch didn't happen during
    // hard event processing (sim may have been stopped)

	if (SM_SIM_STOPPED != m_nSimState && !m_nSynchronizeHappened)
	{	
		m_dTimeSoFar = m_dDeltaTime;
		
		pTypeEl = m_pFirstMobileTypeEl;
		while (NULL != pTypeEl)
		{
			pTypeEl->m_pType->MoveSObjs();
			pTypeEl = pTypeEl->m_pNextEl;
		}
		ASSERT(0 == m_lNumNodes);
	    
	    m_dTimeThisLoop += m_dDeltaTime; 
	}          
                                                    
    // Update monitored quantities ready for display. Don't do this if synch happened,
    // because monitors may be referring to molecules that have disappeared through
    // reactions.
    
    pTypeEl = m_pFirstMonitorTypeEl;
    while (NULL != pTypeEl && !m_nSynchronizeHappened)
    {    
    	((CObjTypeMonitor*) pTypeEl->m_pType)->UpdateQuantities();
    	pTypeEl = pTypeEl->m_pNextEl;
    }  
    
    // Let graphs update their data now monitors up to date
    pTypeEl = m_pFirstGraphTypeEl;
    while (NULL != pTypeEl)
    {    
    	((CObjTypeGraph*) pTypeEl->m_pType)->UpdateQuantities();
    	pTypeEl = pTypeEl->m_pNextEl;
    }                                        

}

void CSim::SynchronizeToNow()
{
	// If stopped sim part way through hard-event calcs, some mols
	// and pistons will be further ahead in time than others. Make
	// all objs catch up to current time.
	// Might be useful one day if do neighbour rebuilds just as
	// required too.       
	
	CTypeListEl* pTypeEl;  
	
	pTypeEl = m_pFirstMobileTypeEl;
	while (NULL != pTypeEl)
	{
		pTypeEl->m_pType->MoveSObjs();
		pTypeEl = pTypeEl->m_pNextEl;
	}
	ASSERT(0 == m_lNumNodes);
    
    m_dTimeThisLoop += m_dTimeSoFar; 

    m_nSynchronizeHappened = TRUE;
}

CTriPtrArray::CTriPtrArray()
{
	m_nNumEdges = 0;
	SetSize(0);
}  

int CTriPtrArray::GetIndex(int row, int col)
{
	return GetIndex(row, col, m_nNumEdges);
}

int CTriPtrArray::GetIndex(int row, int col, int edgeLength)
{                 
	// Want to calc index (in linear array) corresponding to
	// element at column x, row y. Can think about this geometrically
	// by drawing array as little squares and calculating area of
	// elements before this one as sum of trapezium above it, plus
	// bits the other side of diagonal, plus els on this row already
	// gone.
	
	return (2 * edgeLength - row - 1) * row / 2 + col;
}

void CTriPtrArray::AddIndex(int newIndex)
{
	// This function inserts a new row and column (in general)
	// into the array and shifts the existing members accordingly.  
	// And puts default interactions in the newly created spaces.
	//     
	// For example, if the edge length is currently 5, so that
	// we can keep pointers to the 15 possible interactions between
	// all types, and we wish to insert a new type at number 3, then
	// the existing entries should move as follows: (new ints == *,
	// new indices in brackets):
	//
	//      0     1      2       3      4
	// ---+------------------------------
	//  0 | 0     1      2       3      4
	//  1 |       5      6       7      8
	//  2 |              9      10     11
	//  3 |                     12     13
	//  4 |                            14
    //
	//      0     1      2       3      4      5
	// ---+-------------------------------------
	//  0 | 0     1      2       *(3)   3(4)   4(5)
	//  1 |       5(6)   6(7)    *(8)   7(9)   8(10)
	//  2 |              9(11)   *(12) 10(13) 11(14)
	//  3 |                      *(15)  *(16)  *(17)
	//  4 |                            12(18) 13(19)
	//  5 |                                   14(20)       
	
	// Inflate edge length by 1, keeping track of what it was
	int nOldNumEdges = m_nNumEdges++;     
	ASSERT(newIndex >= 0);
	ASSERT(newIndex <= m_nNumEdges); 
	
	// Inflate the array to match new edge length
	SetSize(((m_nNumEdges + 1) * m_nNumEdges) / 2); 
	
	// Now move the existing elements as shown in the example above.
	// This need not be frightfully fast as it doesn't happen as part
	// of the simulation execution.
	
	int i;
	int row, col;
	CInt* pInt;
	
	// Work backwards to avoid overwriting elements yet to be moved
	
	for (row = m_nNumEdges - 1; row >= 0; row--)
	{
		for (col = m_nNumEdges - 1; col >= row; col--)
		{   
        	if ((row > newIndex) && (col > newIndex))
        	{
				// Triangular block at the end, displaced one to the
				// right and one down
				i = GetIndex(row - 1, col - 1, nOldNumEdges);
				pInt = (CInt*) GetAt(i);
				i = GetIndex(row, col, m_nNumEdges);
				SetAt(i, pInt);
			}
			else if ((col > newIndex) && (row < newIndex))   
			{
				// To the right of the new column, displaced one
				// to the right
				i = GetIndex(row, col - 1, nOldNumEdges);
				pInt = (CInt*) GetAt(i);
				i = GetIndex(row, col, m_nNumEdges);
				SetAt(i, pInt);
			}
			else if ((col < newIndex) && (row > 0))
			{
				// To left of new column, stays put in triangle 
				// but moves in our linear array
				i = GetIndex(row, col, nOldNumEdges);
				pInt = (CInt*) GetAt(i);
				i = GetIndex(row, col, m_nNumEdges);
				SetAt(i, pInt);
            }                                   
            else
            {
            	// No move required
            }
            
		}
	}
				
	// Fill the new row and column with default interactions
	
	col = newIndex;	
	for (row = 0; row <= newIndex; row++)	
	{                                       
		pInt = m_pSim->GetDefaultIntByIndex(row, col);
		pInt = pInt->Clone();
		pInt->m_pType1 = (CObjType*) m_pSim->m_typeArray[row];
		pInt->m_pType2 = (CObjType*) m_pSim->m_typeArray[col];
		i = GetIndex(row, col, m_nNumEdges);
		SetAt(i, pInt);
	}
	
	row = newIndex;
	for (col = newIndex + 1; col < m_nNumEdges; col++)
	{
		pInt = m_pSim->GetDefaultIntByIndex(row, col);
		pInt = pInt->Clone();
		pInt->m_pType1 = (CObjType*) m_pSim->m_typeArray[row];
		pInt->m_pType2 = (CObjType*) m_pSim->m_typeArray[col];
		i = GetIndex(row, col, m_nNumEdges);
		SetAt(i, pInt);
	}
	
}

void CTriPtrArray::RemoveIndex(int newIndex)
{
	// This is just the reverse of the above operation (AddIndex)

	int nOldNumEdges = m_nNumEdges--;     
	ASSERT(newIndex >= 0);
	ASSERT(newIndex <= m_nNumEdges); 
	
	// Delete the objects pointed to by the disappearing index, if any
	CInt* pInt;
	int i;   
	int row, col;
	
	col = newIndex;	
	for (row = 0; row <= newIndex; row++)	
	{
		i = GetIndex(row, col, nOldNumEdges);
		pInt = (CInt*) GetAt(i);
		delete pInt;
		SetAt(i, NULL);
	}
	
	row = newIndex;
	for (col = newIndex + 1; col < nOldNumEdges; col++)
	{
		i = GetIndex(row, col, nOldNumEdges);
		pInt = (CInt*) GetAt(i);
		delete pInt;
		SetAt(i, NULL);
	}
	
	// Now move the existing elements as shown in the example above.
	// Work forwards to avoid overwriting elements yet to be moved

	for (row = 0; row < nOldNumEdges; row++)
	{
		for (col = row; col < nOldNumEdges; col++)
		{   
        	if ((row > newIndex) && (col > newIndex))
        	{
				// Triangular block at the end, displaced one to the
				// left and one up
				i = GetIndex(row, col, nOldNumEdges);
				pInt = (CInt*) GetAt(i);
				i = GetIndex(row - 1, col - 1, m_nNumEdges);
				SetAt(i, pInt);
			}
			else if ((col > newIndex) && (row < newIndex))   
			{
				// To the right of the dying column, displaced one
				// to the left
				i = GetIndex(row, col, nOldNumEdges);
				pInt = (CInt*) GetAt(i);
				i = GetIndex(row, col - 1, m_nNumEdges);
				SetAt(i, pInt);
			}
			else if ((col < newIndex) && (row > 0))
			{
				// To left of dying column, stays put in triangle 
				// but moves in our linear array
				i = GetIndex(row, col, nOldNumEdges);
				pInt = (CInt*) GetAt(i);
				i = GetIndex(row, col, m_nNumEdges);
				SetAt(i, pInt);
            }                                   
            else
            {
            	// No move required
            }
            
		}
	}

		
	// Deflate the array to match new edge length
	SetSize(((m_nNumEdges + 1) * m_nNumEdges) / 2); 
}

void CSim::RemoveHotSpotRef(CHotSpot* pHotSpot)
{   
	int nFound = FALSE;
	
	for (int i = 0; i <= m_hotSpots.GetUpperBound(); i++)
	{
		if (pHotSpot == (CHotSpot*) m_hotSpots[i])
		{
			m_hotSpots.RemoveAt(i);
			nFound = TRUE;
			break;
		}
	}                     
	
	ASSERT(nFound);
}

void CSim::AddHotSpotRef(CHotSpot* pHotSpot)
{   
	int nFound = FALSE;
	
	for (int i = 0; i <= m_hotSpots.GetUpperBound(); i++)
	{
		if (pHotSpot == (CHotSpot*) m_hotSpots[i])
		{
			nFound = TRUE;
			break;
		}
	}                     
	
	if (!nFound)
	{
		// not already there, so add to list
		m_hotSpots.Add(pHotSpot);
	}
}
