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

// lendoc.cpp : implementation of the CLenDoc class
//

#include "stdafx.h"
#include "simobjs.h"
#include "sim.h"

#include "mol.h"
#include "wall.h"
#include "piston.h"  
#include "textbox.h" 
#include "monitor.h"  

#include "lennard.h"

#include "lendoc.h"
#include "molprpdg.h"
#include "walprpdg.h" 
#include "edintsdg.h"    
#include "pisprpdg.h"
#include "ndwalldg.h"
#include "txtprpdg.h" 
#include "deltypdg.h"
#include "monprpdg.h"       
#include "sysprpdg.h"
#include "scpdlydg.h"                         
#include "daloftdg.h"
#include "grprpsdg.h"         
#include "sltyprdg.h"
#include "SimSizeDlg.h"
#include "ResolveMultipleSelnDlg.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLenDoc

IMPLEMENT_DYNCREATE(CLenDoc, CDocument)

BEGIN_MESSAGE_MAP(CLenDoc, CDocument)
	//{{AFX_MSG_MAP(CLenDoc)
	ON_COMMAND(ID_OBJECT_NEWTYPEOF_MOLECULE, OnObjectNewtypeofMolecule)
	ON_COMMAND(ID_OBJECT_NEWTYPEOF_WALL, OnObjectNewtypeofWall)
	ON_COMMAND(ID_OBJECT_INTERACTIONS, OnObjectInteractions)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_INTERACTIONS, OnUpdateObjectInteractions)
	ON_COMMAND(ID_SIMULATION_RUN, OnSimulationRun)
	ON_UPDATE_COMMAND_UI(ID_SIMULATION_RUN, OnUpdateSimulationRun)
	ON_COMMAND(ID_OBJECT_NEWTYPEOF_PISTON, OnObjectNewtypeofPiston)
	ON_COMMAND(ID_SIMULATION_REPLAY_SCRIPT, OnSimulationReplayScript)
	ON_UPDATE_COMMAND_UI(ID_SIMULATION_REPLAY_SCRIPT, OnUpdateSimulationReplayScript)
	ON_COMMAND(ID_OBJECT_DELETE, OnObjectDelete)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_DELETE, OnUpdateObjectDelete)
	ON_COMMAND(ID_SIMULATION_RESET, OnSimulationReset)
	ON_COMMAND(ID_OBJECT_NEWTYPEOF_TEXTBOX, OnObjectNewtypeofTextbox)
	ON_COMMAND(ID_OBJECT_NEWTYPEOF_MONITOR, OnObjectNewtypeofMonitor)
	ON_COMMAND(ID_SIMULATION_SYSTEMPROPERTIES, OnSimulationSystemproperties)
	ON_UPDATE_COMMAND_UI(ID_SIMULATION_RESET, OnUpdateSimulationReset)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_NEWTYPEOF_MOLECULE, OnUpdateObjectNewtypeofMolecule)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_NEWTYPEOF_MONITOR, OnUpdateObjectNewtypeofMonitor)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_NEWTYPEOF_PISTON, OnUpdateObjectNewtypeofPiston)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_NEWTYPEOF_TEXTBOX, OnUpdateObjectNewtypeofTextbox)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_NEWTYPEOF_WALL, OnUpdateObjectNewtypeofWall)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_TYPEPROPERTIES, OnUpdateObjectTypeproperties)
	ON_COMMAND(ID_OBJECT_DELETESELECTED, OnObjectDeleteselected)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_DELETESELECTED, OnUpdateObjectDeleteselected)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	ON_COMMAND(ID_SIMULATION_INSERT, OnSimulationInsert)
	ON_UPDATE_COMMAND_UI(ID_SIMULATION_INSERT, OnUpdateSimulationInsert)
	ON_COMMAND(ID_SIMULATION_PAUSE_SCRIPT, OnSimulationPauseScript)
	ON_UPDATE_COMMAND_UI(ID_SIMULATION_PAUSE_SCRIPT, OnUpdateSimulationPauseScript)
	ON_COMMAND(ID_SIMULATION_PLAY_SCRIPT, OnSimulationPlayScript)
	ON_UPDATE_COMMAND_UI(ID_SIMULATION_PLAY_SCRIPT, OnUpdateSimulationPlayScript)
	ON_COMMAND(ID_SIMULATION_STOP_SCRIPT, OnSimulationStopScript)
	ON_UPDATE_COMMAND_UI(ID_SIMULATION_STOP_SCRIPT, OnUpdateSimulationStopScript)
	ON_COMMAND(ID_SIMULATION_FFWD, OnSimulationFfwd)
	ON_UPDATE_COMMAND_UI(ID_SIMULATION_FFWD, OnUpdateSimulationFfwd)
	ON_COMMAND(ID_SIMULATION_RECORD, OnSimulationRecord)
	ON_UPDATE_COMMAND_UI(ID_SIMULATION_RECORD, OnUpdateSimulationRecord)
	ON_COMMAND(ID_OBJECT_DELETEALLOFTYPE, OnObjectDeletealloftype)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_DELETEALLOFTYPE, OnUpdateObjectDeletealloftype)
	ON_COMMAND(ID_OBJECT_DELETE_ALL, OnObjectDeleteAll)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_DELETE_ALL, OnUpdateObjectDeleteAll)
	ON_COMMAND(ID_OBJECT_NEWTYPEOF_GRAPH, OnObjectNewtypeofGraph)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_NEWTYPEOF_GRAPH, OnUpdateObjectNewtypeofGraph)
	ON_COMMAND(ID_OBJECT_PROPERTIES, OnObjectProperties)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_PROPERTIES, OnUpdateObjectProperties)
	ON_COMMAND(ID_OBJECT_GRIDSETTINGS, OnObjectGridsettings)
	ON_COMMAND(ID_OBJECT_NEWTYPEOF_SLIDER, OnObjectNewtypeofSlider)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_NEWTYPEOF_SLIDER, OnUpdateObjectNewtypeofSlider)
	ON_COMMAND(ID_SIMULATION_STARTBOTH, OnSimulationStartboth)
	ON_UPDATE_COMMAND_UI(ID_SIMULATION_STARTBOTH, OnUpdateSimulationStartboth)
	ON_COMMAND(ID_SIMULATION_STOPBOTH, OnSimulationStopboth)
	ON_UPDATE_COMMAND_UI(ID_SIMULATION_STOPBOTH, OnUpdateSimulationStopboth)
	ON_COMMAND(ID_SIMULATION_SIZE_SCALING, OnSimulationSizeScaling)
	ON_COMMAND(ID_OBJECT_RESOLVEMULTIPLESELECTION, OnObjectResolvemultipleselection)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_RESOLVEMULTIPLESELECTION, OnUpdateObjectResolvemultipleselection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLenDoc construction/destruction

CLenDoc::CLenDoc()
{
	m_pSim = new CSim;  
	m_pSim->m_pDoc = this;  
	m_bConsShowing = FALSE;
	m_bSimShowing = FALSE;
}

CLenDoc::~CLenDoc()
{   
	delete m_pSim; 
}

BOOL CLenDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

BOOL CLenDoc::OnOpenDocument(const char* pszPathName)
{        
	// Tried using CFile.read here, and catching the end-of-file
	// exception, but in fact that exception is never thrown;
	// it just keeps returning the last byte in the file.
	CStdioFile file;
	
	TRACE("OnOpenDocument: %s\n", pszPathName);
	
	if (file.Open(pszPathName, CFile::modeRead))
	{  
		CString cmd;
		CString accum;
		int nLastSemicolonPosn = 0;
		int nAccumLen; 
		int nErrStatus = SR_OK;
		
		enum {WAITING_NONWHITE, IN_NONWHITE, IN_QUOTED, POST_QUOTED} state = WAITING_NONWHITE;
				
		theApp.BeginWaitCursor();
		           
		while (file.ReadString(buff, GLOBAL_BUFF_SIZE) != NULL)
		{                    
			// Read in another string
			accum += buff;   
			nAccumLen = accum.GetLength();
			
			for (int i = 0; i < nAccumLen; i++)
			{
				char c = accum[i];
				
				switch (state)
				{
					case WAITING_NONWHITE:
						
						if ((' ' == c) || ('\t' == c) || ('\n' == c))
						{
							// more whitespace, ignore	
						}
						else if (';' == c)
						{
							nErrStatus |= m_pSim->HandleNewCommand(cmd);
							cmd = "";
							state = WAITING_NONWHITE; 
							nLastSemicolonPosn = i;
						}
						else 
						{
							cmd += c;
							
							if ('\"' == c)  
							{
								state = IN_QUOTED;
							}
							else
							{
								state = IN_NONWHITE;
							}
						}
						
						break;
						
					case IN_QUOTED:
					
						if ('\n' == c)
						{
							cmd += ' ';  // return -> space
						}
						else if ('\"' == c)
						{
							state = POST_QUOTED;
						}
						else
						{
							cmd += c;
						}
						
						break;
					
					case POST_QUOTED:
					
						if ((' ' == c) || ('\t' == c) || ('\n' == c))
						{
							// ignore whitespace
						}
						else if ('\"' == c)
						{
							// concatenate with existing string
							state = IN_QUOTED;
						}
						else
						{
							cmd += "\" ";
							
							if (';' == c)
							{
								nErrStatus |= m_pSim->HandleNewCommand(cmd);
								cmd = "";
								state = WAITING_NONWHITE; 
								nLastSemicolonPosn = i;
                            }
                            else
                            {
								cmd += c;
								state = IN_NONWHITE;
							}
						}
						
						break;
								
					case IN_NONWHITE:
					
						if ((' ' == c) || ('\t' == c) || ('\n' == c))
						{
							cmd += ' ';
							state = WAITING_NONWHITE;
						}
						else if (';' == c)
						{
							nErrStatus |= m_pSim->HandleNewCommand(cmd);
							cmd = "";
							state = WAITING_NONWHITE; 
							nLastSemicolonPosn = i;
						}
						else
						{
							cmd += c;
						}
						
						break;	
				}                       
				
			}  // end of for (i = accum)    
			
			accum = "";
			 
		} // end of while (readstring != NULL)
		
		file.Close();  
					    

		theApp.EndWaitCursor();

        
        switch (nErrStatus)
        {
        	case SR_OK:
        		break;
        		
        	case SR_WARN:
        		AfxMessageBox("At least one warning occurred from loading"
        		              " this script. See command console for details.");
        		break;
        		
        	case SR_ERROR:
        		AfxMessageBox("At least one error occurred from loading"
        		              " this script. See command console for details.");
        		break;
        		
        	default:
        		ASSERT(FALSE);
        		break;
        }	


        m_pSim->OnLoad(); // prompts to run demos etc
        
		return TRUE;
	} 
	else 
	{
		AfxMessageBox(CString("Unable to open ") + pszPathName);
		return FALSE;
	}
}

BOOL CLenDoc::OnSaveDocument(const char* pszPathName)
{                     
	CStdioFile file;
	
	TRACE("OnSaveDocument: %s\n", pszPathName);
	
	if (file.Open(pszPathName, CFile::modeWrite | CFile::modeCreate))
	{                                            
		TRACE("File opened OK\n"); 
		int ret = m_pSim->Save(file);
		file.Close();
		if (SR_OK == ret)
		{
			return TRUE;
		}
		else
		{
			AfxMessageBox("Error in file save");
			return FALSE;
		}
	}
	else
	{                
		TRACE("Error opening file!\n");
		AfxMessageBox("Error trying to open file to save to");
		return FALSE;                  
	}

}
/////////////////////////////////////////////////////////////////////////////
// CLenDoc diagnostics

#ifdef _DEBUG
void CLenDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLenDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLenDoc commands

void CLenDoc::OnObjectNewtypeofMolecule()
{
	CEditMolPropsDlg dlg;
	dlg.m_cFillColour = RGB(255,0,255);  // purple
	dlg.m_cLineColour = RGB(0,0,255);    // blue
	dlg.m_dLineThick = 0.2;
	dlg.m_dRadius = 0.07;
	dlg.m_dMass = 20;
    dlg.m_dFormationEnergy = 0;
	dlg.m_sName = "molecule"; 
	m_pSim->MakeUniqueTypeName(dlg.m_sName);
	dlg.m_pSim = m_pSim;
	
	if (IDOK == dlg.DoModal())
	{
		// build up command to create new molecule type
		CString cmd("objtype mol name \"");
		
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

void CLenDoc::OnObjectNewtypeofWall()
{
	CEditWallPropsDlg dlg;

	dlg.m_nLineThick = 2;
	dlg.m_dSpecialTemperature = 100;
	dlg.m_sName = "wall"; 
	m_pSim->MakeUniqueTypeName(dlg.m_sName);
	dlg.m_nTemperatureType = WT_USE_SYS_TEMP;
	dlg.m_nMolIntType = WT_THERMAL;
	dlg.m_cLineColour = RGB(0,0,255); // blue
	
	if (IDOK == dlg.DoModal())
	{ 
		// user has specified new type of wall, so issue command
		CString cmd("objtype wall name \"");
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

void CLenDoc::OnObjectInteractions()
{
	CEditIntsDlg dlg;
	dlg.m_pSim = m_pSim;
	
	dlg.DoModal();
	
}

void CLenDoc::OnUpdateObjectInteractions(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SM_SIM_STOPPED == m_pSim->m_nSimState);
}

void CLenDoc::OnSimulationRun()
{
	CString cmd;
	
	if (SM_SIM_STOPPED == m_pSim->m_nSimState)
	{
		cmd = "run";
	}
	else if (SM_SIM_RUNNING == m_pSim->m_nSimState)
	{
		cmd = "stop";
	}
	else
	{
		// shouldn't happen, should be disabled if paused
		ASSERT(FALSE);
		cmd = "";
	}
	
	if ((cmd != "") && (SR_OK != m_pSim->HandleNewCommand(cmd)))
	{
		AfxMessageBox("Warnings or errors occurred -- see Console");
	}
}

void CLenDoc::OnUpdateSimulationRun(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SM_SIM_PAUSED != m_pSim->m_nSimState);
	
	pCmdUI->SetCheck(SM_SIM_RUNNING == m_pSim->m_nSimState);
}

void CLenDoc::OnSimulationStop()
{
	CString cmd("stop");
	
	if (SR_OK != m_pSim->HandleNewCommand(cmd))
	{
		AfxMessageBox("Warnings or errors occurred -- see Console");
	}
}

void CLenDoc::OnObjectNewtypeofPiston()
{ 
	CPtrArray wallTypeList;
	int wallsExist;
	
	m_pSim->GetTypeArray(wallTypeList, CI_WALL);
	
	if (wallTypeList.GetUpperBound() < 0)
	{   
		// No types of wall defined at present, user must do one first
		// to make piston out of
		
		CNeedWallsForPistonsDlg needWallsDlg;
		
		if (IDOK == needWallsDlg.DoModal())  
		{
			// user opts to make new type of wall first
			OnObjectNewtypeofWall();  
			
			// check if they did make a type 
			m_pSim->GetTypeArray(wallTypeList, CI_WALL);
			wallsExist = (wallTypeList.GetUpperBound() >= 0);
		}
		else
		{
			wallsExist = FALSE;
		}
	}
	else
	{
		wallsExist = TRUE;
	}   
	
	if (wallsExist)
	{
		
		CEditPistonPropsDlg pistonPropsDlg;     
		pistonPropsDlg.m_sName = "piston";
		m_pSim->MakeUniqueTypeName(pistonPropsDlg.m_sName); 
		
		pistonPropsDlg.m_pSim = m_pSim;
		
		if ((IDOK == pistonPropsDlg.DoModal()) && (NULL != pistonPropsDlg.m_pWallType))
		{
			// user meant it, buid up command to make piston
			   
			CString cmd("objtype piston name \"");
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
}

void CLenDoc::OnSimulationReplayScript()
{
	m_pSim->ReplayScript();
}

void CLenDoc::OnUpdateSimulationReplayScript(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CLenDoc::OnObjectDelete()
{
	CDelTypesDlg dlg;
	
	if (SM_SIM_STOPPED == m_pSim->m_nSimState)
	{
		dlg.m_pSim = m_pSim;
	
		dlg.DoModal(); 
	}
	else
	{
		// menu item shouldn't have been available anyway
		ASSERT(FALSE);
	}
}

void CLenDoc::OnUpdateObjectDelete(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SM_SIM_STOPPED == m_pSim->m_nSimState);
}

void CLenDoc::OnSimulationReset()
{
	int msgboxret = AfxMessageBox("All object types and all objects"
	                              " will be permanently destroyed, and system"
	                              " parameters (eg temperature) will return to"
	                              " default values. Do you really want to do"
	                              " this?", MB_YESNO);
	                              
	if (IDYES == msgboxret)
	{
		m_pSim->HandleNewCommand("reset");
	}	
}

void CLenDoc::OnObjectNewtypeofTextbox()
{
	CTextboxPropsDlg dlg; 
	
	dlg.m_sName = "textbox";
	m_pSim->MakeUniqueTypeName(dlg.m_sName); 
	dlg.m_sFontName = "Arial";
	dlg.m_nFontSize = 16;
    dlg.m_dScaleFactor = m_pSim->m_dScaleFactor;
	
	if (IDOK == dlg.DoModal())
	{                     
		CString cmd("objtype textbox name \"");
		cmd += dlg.m_sName;
		cmd += "\" font \"";
		cmd += dlg.m_sFontName;
		cmd += "\" size ";  
		sprintf(buff, "%d", dlg.m_nFontSize);
		cmd += buff;
		
		m_pSim->HandleNewCommand(cmd);
	}	
}

void CLenDoc::OnObjectNewtypeofMonitor()
{
	CEditMonitorPropsDlg dlg; 
	
	dlg.m_sName = "monitor";
	m_pSim->MakeUniqueTypeName(dlg.m_sName);
	
	if (IDOK == dlg.DoModal())
	{
		CString cmd("objtype monitor name \"");
		cmd += dlg.m_sName + "\" ";
		
		switch (dlg.m_nControlType)
		{
			case MCNTRL_NONE: 
				break;
				
			case MCNTRL_DESTROY:
				cmd += "destructor";
				break;
				
			case MCNTRL_FIX_DENS_TEMP:
				cmd += "fixer";
				break;
				
			case MCNTRL_HEATER: 
				cmd += "heater";
				break;
				
			default:
				ASSERT(FALSE);
				break;
		}

		cmd += " hatch";
		
		if (dlg.m_nHatch)
		{
			cmd += " 1";
		}
		else
		{
			cmd += " 0";
		}
		

		cmd += " label";
		
		if (dlg.m_nLabel)
		{
			cmd += " 1";
		}
		else
		{
			cmd += " 0";
		} 
		
		
		sprintf(buff, " linethick %d", dlg.m_nLineThick);
		
		cmd += buff;
		
		sprintf(buff, " red %d green %d blue %d",
		                (int) (dlg.m_cColour & 0x000000ff),
		                (int) ((dlg.m_cColour & 0x0000ff00) >> 8),
		                (int) ((dlg.m_cColour & 0x00ff0000) >> 16));
		                                    
		cmd += buff;		                                    
		
		m_pSim->HandleNewCommand(cmd);
	}
}

void CLenDoc::OnSimulationSystemproperties()
{
	CEditSysPropsDlg dlg;

    int nWasUsingAutoTimestep = (m_pSim->m_dUseAutoDeltaTime > 0.5);
	
	dlg.m_dGravitationalAcceleration = m_pSim->m_dGravitationalAcceleration;
	dlg.m_dSystemTemperature = m_pSim->m_dTemperature;
	dlg.m_nAutoRun = m_pSim->m_nIsAutoRun;
    dlg.m_nUseAutoTimestep = nWasUsingAutoTimestep;
    dlg.m_dTimestep = m_pSim->m_dDeltaTime;
    dlg.m_dAverageTimeConst = m_pSim->m_dAverageTimeConst;
    dlg.m_lRandSeed = -1;
	
	if (IDOK == dlg.DoModal())
	{
        if (dlg.m_nUseAutoTimestep && !nWasUsingAutoTimestep)
        {
            m_pSim->HandleNewCommand("use_auto_deltatime 1");
        }
        else if (!dlg.m_nUseAutoTimestep && nWasUsingAutoTimestep)
        {
            m_pSim->HandleNewCommand("use_auto_deltatime 0");
        }
        else
        {
            // auto timestep status unchanged
        }

        if (fabs(dlg.m_dTimestep - m_pSim->m_dDeltaTime) > NEARLY_ZERO)
        {
            sprintf(buff, "deltatime %g", dlg.m_dTimestep);
            m_pSim->HandleNewCommand(buff);
        }

        if (fabs(dlg.m_dAverageTimeConst - m_pSim->m_dAverageTimeConst) > NEARLY_ZERO)
        {
            sprintf(buff, "avg_time_const %g", dlg.m_dAverageTimeConst);
            m_pSim->HandleNewCommand(buff);
        }

		if (dlg.m_dSystemTemperature != m_pSim->m_dTemperature)
		{
			sprintf(buff, "temperature %g", dlg.m_dSystemTemperature);
			
			m_pSim->HandleNewCommand(buff);
		}

		if (dlg.m_dGravitationalAcceleration != m_pSim->m_dGravitationalAcceleration)
		{
			sprintf(buff, "gravity %g", dlg.m_dGravitationalAcceleration);
			
			m_pSim->HandleNewCommand(buff);
		}                                 
		
		if (dlg.m_nAutoRun != m_pSim->m_nIsAutoRun)
		{
			sprintf(buff, "autorun %d", dlg.m_nAutoRun);
		
			m_pSim->HandleNewCommand(buff);
		}

        if (dlg.m_lRandSeed >= 0)
        {
            sprintf(buff, "set_rand_seed %ld", dlg.m_lRandSeed);
            m_pSim->HandleNewCommand(buff);	
        }
    }
}

void CLenDoc::OnUpdateSimulationReset(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SM_SIM_STOPPED == m_pSim->m_nSimState);
}

void CLenDoc::OnUpdateObjectNewtypeofMolecule(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SM_SIM_STOPPED == m_pSim->m_nSimState);
}

void CLenDoc::OnUpdateObjectNewtypeofMonitor(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SM_SIM_STOPPED == m_pSim->m_nSimState);
}

void CLenDoc::OnUpdateObjectNewtypeofPiston(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SM_SIM_STOPPED == m_pSim->m_nSimState);
}

void CLenDoc::OnUpdateObjectNewtypeofTextbox(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SM_SIM_STOPPED == m_pSim->m_nSimState);
}

void CLenDoc::OnUpdateObjectNewtypeofWall(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SM_SIM_STOPPED == m_pSim->m_nSimState);
}

void CLenDoc::OnUpdateObjectTypeproperties(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SM_SIM_STOPPED == m_pSim->m_nSimState);
}

void CLenDoc::OnObjectDeleteselected()
{
	m_pSim->HandleNewCommand("seln_delete");
}

void CLenDoc::OnUpdateObjectDeleteselected(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SM_SIM_STOPPED == m_pSim->m_nSimState
	               && !m_pSim->m_selectedObjects.IsEmpty());
}

void CLenDoc::OnUpdateFilePrint(CCmdUI* pCmdUI)
{
	// pCmdUI->Enable(SM_SIM_STOPPED == m_pSim->m_nSimState);
}

void CLenDoc::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	// pCmdUI->Enable(SM_SIM_STOPPED == m_pSim->m_nSimState);
}

void CLenDoc::OnSimulationInsert()
{
	// Allow user to introduce delay into script
	
	static double old_value = -1;
	
	CScriptDelayDlg dlg;
	
	if (old_value > 0)
	{
		dlg.m_dDelaySec = old_value;
	}
	
	if (dlg.DoModal())
	{
		old_value = dlg.m_dDelaySec;
			
		sprintf(buff, "delay %g", dlg.m_dDelaySec);
		
		m_pSim->HandleNewCommand(buff);
	}
	
}

void CLenDoc::OnUpdateSimulationInsert(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CLenDoc::OnSimulationPauseScript()
{   
	if (!m_pSim->KillPauseIfPaused())
	{   
		// wasn't paused already so pause now
		m_pSim->HandleNewCommand("pause");
	}
}

void CLenDoc::OnUpdateSimulationPauseScript(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(((SM_REPLAY_PAUSED_PLAY == m_pSim->m_nReplayState)
	                || (SM_REPLAY_PAUSED_STOP == m_pSim->m_nReplayState))); 
}

void CLenDoc::OnSimulationPlayScript()
{
	m_pSim->PlayScript();
}

void CLenDoc::OnUpdateSimulationPlayScript(CCmdUI* pCmdUI)
{                                                        
	pCmdUI->SetCheck(((SM_REPLAY_PLAYING == m_pSim->m_nReplayState)
	                || (SM_REPLAY_PAUSED_PLAY == m_pSim->m_nReplayState)
	                || (SM_REPLAY_FFWDING == m_pSim->m_nReplayState)));
}

void CLenDoc::OnSimulationStopScript()
{
	m_pSim->StopScript();
}

void CLenDoc::OnUpdateSimulationStopScript(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SM_REPLAY_STOPPED != m_pSim->m_nReplayState); 
	              
	pCmdUI->SetCheck(SM_REPLAY_PAUSED_STOP == m_pSim->m_nReplayState);
}

void CLenDoc::OnSimulationFfwd()
{
	m_pSim->FastFwdScript();
}

void CLenDoc::OnUpdateSimulationFfwd(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(SM_REPLAY_FFWDING == m_pSim->m_nReplayState);
}

void CLenDoc::OnSimulationRecord()
{                                    
	// toggle recording state 
	
	if (m_pSim->m_nIsRecording)
	{                         
		m_pSim->m_nIsRecording = FALSE;
	}
	else
	{   
		m_pSim->m_nIsRecording = TRUE;
	}
}

void CLenDoc::OnUpdateSimulationRecord(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_pSim->m_nIsRecording);
}

void CLenDoc::OnObjectDeletealloftype()
{
	CDelAllOfTypeDlg dlg;
	
	if (SM_SIM_STOPPED == m_pSim->m_nSimState)
	{
		dlg.m_pSim = m_pSim;
	
		dlg.DoModal(); 
	}
	else
	{
		// menu item shouldn't have been available anyway
		ASSERT(FALSE);
	}
}

void CLenDoc::OnUpdateObjectDeletealloftype(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SM_SIM_STOPPED == m_pSim->m_nSimState);
}

void CLenDoc::OnObjectDeleteAll()
{
	m_pSim->HandleNewCommand("del_all");
}

void CLenDoc::OnUpdateObjectDeleteAll(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SM_SIM_STOPPED == m_pSim->m_nSimState);
}

void CLenDoc::OnObjectNewtypeofGraph()
{
	CEditGraphTypePropsDlg dlg;
	
	dlg.m_sName = "graph"; 
	m_pSim->MakeUniqueTypeName(dlg.m_sName);
	
	if (IDOK == dlg.DoModal())
	{
		// build up command to create new graph type
		CString cmd("objtype graph name \"");
		
		sprintf(buff, "%s", dlg.m_sName);
		cmd += buff;
		
		cmd += "\"";
		
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

void CLenDoc::OnUpdateObjectNewtypeofGraph(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SM_SIM_STOPPED == m_pSim->m_nSimState);
}

void CLenDoc::OnObjectProperties()
{
	int nSelected = m_pSim->m_selectedObjects.GetCount();
	
	if ((1 == nSelected) && (SM_SIM_STOPPED == m_pSim->m_nSimState))
	{
		m_pSim->ObjectProperties();
	}
}

void CLenDoc::OnUpdateObjectProperties(CCmdUI* pCmdUI)
{
	int nSelected = m_pSim->m_selectedObjects.GetCount();
	
	pCmdUI->Enable((1 == nSelected) && (SM_SIM_STOPPED == m_pSim->m_nSimState));
}

void CLenDoc::OnObjectGridsettings()
{            
	m_pSim->OnGridSettings();
}

void CLenDoc::OnObjectNewtypeofSlider()
{
	CSliderTypePropsDlg dlg;
	
	dlg.m_sName = "slider"; 
	m_pSim->MakeUniqueTypeName(dlg.m_sName);
	
	if (IDOK == dlg.DoModal())
	{
		// build up command to create new slider type
		CString cmd("objtype slider name \"");
		
		sprintf(buff, "%s", dlg.m_sName);
		cmd += buff;
		
		cmd += "\"";
		
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

void CLenDoc::OnUpdateObjectNewtypeofSlider(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(SM_SIM_STOPPED == m_pSim->m_nSimState);
}

void CLenDoc::OnSimulationStartboth()
{   
	OnSimulationPlayScript();                     
	OnSimulationRun();
}

void CLenDoc::OnUpdateSimulationStartboth(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(((SM_REPLAY_STOPPED == m_pSim->m_nReplayState) ||
	               (SM_REPLAY_PAUSED_PLAY == m_pSim->m_nReplayState) || 
	               (SM_REPLAY_FFWDING == m_pSim->m_nReplayState) ||
	               (SM_REPLAY_PAUSED_STOP == m_pSim->m_nReplayState))
	               && (SM_SIM_STOPPED == m_pSim->m_nSimState));
	
}

void CLenDoc::OnSimulationStopboth()
{
	OnSimulationStopScript();
	OnSimulationStop();
	
}

void CLenDoc::OnUpdateSimulationStopboth(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(((SM_REPLAY_PLAYING == m_pSim->m_nReplayState) ||
	                (SM_REPLAY_FFWDING == m_pSim->m_nReplayState) ||
	                (SM_REPLAY_PAUSED_PLAY == m_pSim->m_nReplayState) ||
	                (SM_REPLAY_PAUSED_STOP == m_pSim->m_nReplayState))
 	               && (SM_SIM_RUNNING == m_pSim->m_nSimState));
}
                                
void CLenDoc::OnSimulationSizeScaling() 
{
	CSimSizeDlg dlg;
	
    dlg.m_dBottom = m_pSim->m_dScreenBottom_nm;
	dlg.m_dHeight = m_pSim->m_dScreenHeight_nm;
	dlg.m_dLeft = m_pSim->m_dScreenLeft_nm;
	dlg.m_dWidth = m_pSim->m_dScreenWidth_nm;
    dlg.m_pSim = m_pSim;

    dlg.DoModal();
}

void CLenDoc::OnObjectResolvemultipleselection() 
{
	CResolveMultipleSelnDlg dlg;

    dlg.m_pSim = m_pSim;

    dlg.DoModal();
}

void CLenDoc::OnUpdateObjectResolvemultipleselection(CCmdUI* pCmdUI) 
{
	int nSelected = m_pSim->m_selectedObjects.GetCount();
	
	pCmdUI->Enable((nSelected > 1) && (SM_SIM_STOPPED == m_pSim->m_nSimState));
}
