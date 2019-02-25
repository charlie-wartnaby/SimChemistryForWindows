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

// mncsetdg.cpp : implementation file
//

#include "stdafx.h"
#include "lennard.h"  
#include "simobjs.h"
#include "sim.h"       
#include "mol.h"
#include "monitor.h"
#include "mncsetdg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMonitorCtrlSetDlg dialog


CMonitorCtrlSetDlg::CMonitorCtrlSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMonitorCtrlSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMonitorCtrlSetDlg)
	m_bHeaterActive = FALSE;
	m_sCtrlSummary = "";
	m_dDensitySelected = 0;
	m_dHeaterPower = 10;
	m_dTemperatureSelected = 100;
	m_nTemperatureFixedCheck = FALSE;
	m_nDensityFixedCheck = FALSE;
	//}}AFX_DATA_INIT
	
	m_nClone = FALSE; 
	m_pCloneMonitor = NULL; 
	m_pMonitor = NULL;
}                                                                     

CMonitorCtrlSetDlg::~CMonitorCtrlSetDlg()
{
	delete m_pMonitor;
}

BOOL CMonitorCtrlSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();             

	m_pMonitor = new CSObjMonitor;   
	
	m_pMonitor->m_pType = m_pMonitorType;
	m_pMonitor->m_pSim = m_pSim;   
	m_pMonitor->ContributeNumerics();  // so that will destruct neatly
	m_pMonitor->m_lId = 0;
	
	m_pMonitor->BuildStatsArray(m_pMonitorType->m_nMolTypesRefd);
    m_pMonitor->m_nMolStatsBuilt = TRUE;
    
    if (m_nClone)
    {
    	// copy existing monitor attributes to our pet one  
    	CString cmd;     
    	CString dummyErrString;
    	m_pCloneMonitor->WriteState(cmd);
    	m_pMonitor->AcceptCommandLine(cmd, dummyErrString);
    }

	// Set up tab stops in list box. The co-ordinates are shown in
	// App Studio for the headings above the list-box when they
	// are selected.
	
#define LISTBOX_MARGIN   8  // pixels from edge of dlg box
#define TEMPERATURE     98
#define TFIXED         159
#define DENSITY        191
#define DFIXED         248 
#define INDENT          10
	
	DWORD lDlgBaseUnits = ::GetDialogBaseUnits();     
	WORD  wDlgWidthUnits =  LOWORD(GetDialogBaseUnits());
	m_molListTabstops[0] = ((TEMPERATURE - LISTBOX_MARGIN + INDENT) * wDlgWidthUnits) / 8;
	m_molListTabstops[1] = ((TFIXED      - LISTBOX_MARGIN + INDENT) * wDlgWidthUnits) / 8;
	m_molListTabstops[2] = ((DENSITY     - LISTBOX_MARGIN + INDENT) * wDlgWidthUnits) / 8;
	m_molListTabstops[3] = ((DFIXED      - LISTBOX_MARGIN + INDENT) * wDlgWidthUnits) / 8;
    
    CListBox* pListBox = (CListBox*) GetDlgItem(IDC_FIXER_LIST);       
    BOOL ret = pListBox->SetTabStops(sizeof(m_molListTabstops)/sizeof(m_molListTabstops[0]),
                                     m_molListTabstops);
    ASSERT(ret);
    
	UpdateMolList();

	int fred = pListBox->SetCurSel(0);
	
	m_nOldMolSeln = pListBox->GetCurSel();
	
	UpdateEditBoxes();
		
	EnableControls();    

	return (TRUE);  // get windows to set focus to something
}

void CMonitorCtrlSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMonitorCtrlSetDlg)
	DDX_Check(pDX, IDC_CHECK_POWER_ON, m_bHeaterActive);
	DDX_Text(pDX, IDC_CONTROL_SUMMARY, m_sCtrlSummary);
	DDX_Text(pDX, IDC_EDIT_DENSITY, m_dDensitySelected);
	DDV_MinMaxDouble(pDX, m_dDensitySelected, LIM_MIN_CTRL_DENSITY, LIM_MAX_CTRL_DENSITY);
	DDX_Text(pDX, IDC_EDIT_POWER, m_dHeaterPower);
	DDV_MinMaxDouble(pDX, m_dHeaterPower, LIM_MIN_MON_POWER, LIM_MAX_MON_POWER);
	DDX_Text(pDX, IDC_EDIT_TEMPERATURE, m_dTemperatureSelected);
	DDV_MinMaxDouble(pDX, m_dTemperatureSelected, LIM_MIN_TEMP, LIM_MAX_TEMP);
	DDX_Check(pDX, IDC_CHECK_TEMPERATURE, m_nTemperatureFixedCheck);
	DDX_Check(pDX, IDC_CHECK_DENSITY, m_nDensityFixedCheck);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMonitorCtrlSetDlg, CDialog)
	//{{AFX_MSG_MAP(CMonitorCtrlSetDlg)
	ON_LBN_SELCHANGE(IDC_FIXER_LIST, OnSelchangeFixerList)
	ON_BN_CLICKED(IDC_CHECK_DENSITY, OnCheckDensity)
	ON_BN_CLICKED(IDC_CHECK_TEMPERATURE, OnCheckTemperature)
	ON_EN_KILLFOCUS(IDC_EDIT_DENSITY, OnKillfocusEditDensity)
	ON_EN_KILLFOCUS(IDC_EDIT_TEMPERATURE, OnKillfocusEditTemperature)
	ON_BN_CLICKED(ID_TYPE_PROPS_BTN, OnTypePropsBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMonitorCtrlSetDlg::UpdateEditBoxes()
{
    CListBox* pListBox = (CListBox*) GetDlgItem(IDC_FIXER_LIST);       
    CEdit* pTemperatureEdit = (CEdit*) GetDlgItem(IDC_EDIT_TEMPERATURE);
    CEdit* pDensityEdit = (CEdit*) GetDlgItem(IDC_EDIT_DENSITY);        
    CButton* pTemperatureFixed = (CButton*) GetDlgItem(IDC_CHECK_TEMPERATURE);
    CButton* pDensityFixed = (CButton*) GetDlgItem(IDC_CHECK_DENSITY);
    
    m_bHeaterActive = (m_pMonitor->m_dHeaterOn >= 0.5);
    m_dHeaterPower = m_pMonitor->m_dHeaterPower;
    
	int sel = pListBox->GetCurSel();     
	
	if (LB_ERR == sel)
	{                       
		// no current selection
        m_dTemperatureSelected = 100;
		m_dDensitySelected = 0;
		m_nTemperatureFixedCheck = FALSE;
		m_nDensityFixedCheck = FALSE;
	}
	else
	{   
		TRACE("In UpdateEditBoxes, current seln = %d\n", sel);
		
		CMolTypeStats* pStats = (CMolTypeStats*) m_pMonitor->m_molStatsList[sel];
		
		TRACE("... so copying T = %g and dens = %g\n", pStats->m_dFixedTemperature,
		                                   pStats->m_dFixedDensity);
		                                   
        m_dTemperatureSelected = pStats->m_dFixedTemperature;
        
		m_dDensitySelected = pStats->m_dFixedDensity;
		
		m_nTemperatureFixedCheck = pStats->m_nTemperatureFixed;
		
		m_nDensityFixedCheck = pStats->m_nDensityFixed;
	}  
	
	UpdateData(FALSE); // copy variable values to controls
}

void CMonitorCtrlSetDlg::EnableControls()
{
	// Depending on current monitor properties, enable or disable
	// controls on dialogue.
    
	switch (m_pMonitorType->m_nControlType)
	{                                   
		case MCNTRL_NONE:  
			m_sCtrlSummary = "This type of monitor is not currently set to control molecules, "
			                 "but just measure them, so these settings are disabled.";
			
		    GetDlgItem(IDC_FIXER_BOX)->EnableWindow(FALSE);         // box around fixer settings part of dialogue
		    GetDlgItem(IDC_FIXER_HEADING_MOLTYPE)->EnableWindow(FALSE); // title string above listbox
		    GetDlgItem(IDC_FIXER_HEADING_FIXEDT)->EnableWindow(FALSE);
		    GetDlgItem(IDC_FIXER_HEADING_TFIXED)->EnableWindow(FALSE);
		    GetDlgItem(IDC_FIXER_HEADING_FIXEDD)->EnableWindow(FALSE);
		    GetDlgItem(IDC_FIXER_HEADING_DFIXED)->EnableWindow(FALSE);		    		    		    		    
		    GetDlgItem(IDC_FIXER_LIST)->EnableWindow(FALSE);        // list of mol types, set densities and temps     
		    GetDlgItem(IDC_FOR_SELECTION)->EnableWindow(FALSE);     // text                     
		    GetDlgItem(IDC_EDIT_TEMPERATURE)->EnableWindow(FALSE);  // edit box for fixed temperature
		    GetDlgItem(IDC_CHECK_TEMPERATURE)->EnableWindow(FALSE); // tick box
		    GetDlgItem(IDC_EDIT_DENSITY)->EnableWindow(FALSE);      // edit box for fixed density    
		    GetDlgItem(IDC_CHECK_DENSITY)->EnableWindow(FALSE);     // tick box    
		    
		    GetDlgItem(IDC_HEATER_BOX)->EnableWindow(FALSE);        // box around heater settings  
		    GetDlgItem(IDC_POWER_TEXT)->EnableWindow(FALSE);        // label              
		    GetDlgItem(IDC_EDIT_POWER)->EnableWindow(FALSE);        // edit box for heater power 
		    GetDlgItem(IDC_CHECK_POWER_ON)->EnableWindow(FALSE);    // heater on?  
		    break;
			 
		case MCNTRL_DESTROY: 
			m_sCtrlSummary = "This type of monitor is currently set to just destroy any molecules "
			                 "that touch its boundary, so the settings below are disabled.";
			
		    GetDlgItem(IDC_FIXER_BOX)->EnableWindow(FALSE);         // box around fixer settings part of dialogue
		    GetDlgItem(IDC_FIXER_HEADING_MOLTYPE)->EnableWindow(FALSE); // title string above listbox
		    GetDlgItem(IDC_FIXER_HEADING_FIXEDT)->EnableWindow(FALSE);
		    GetDlgItem(IDC_FIXER_HEADING_TFIXED)->EnableWindow(FALSE);
		    GetDlgItem(IDC_FIXER_HEADING_FIXEDD)->EnableWindow(FALSE);
		    GetDlgItem(IDC_FIXER_HEADING_DFIXED)->EnableWindow(FALSE);		    		    		    		    
		    GetDlgItem(IDC_FIXER_LIST)->EnableWindow(FALSE);        // list of mol types, set densities and temps     
		    GetDlgItem(IDC_FOR_SELECTION)->EnableWindow(FALSE);     // text                     
		    GetDlgItem(IDC_EDIT_TEMPERATURE)->EnableWindow(FALSE);  // edit box for fixed temperature
		    GetDlgItem(IDC_CHECK_TEMPERATURE)->EnableWindow(FALSE); // tick box
		    GetDlgItem(IDC_EDIT_DENSITY)->EnableWindow(FALSE);      // edit box for fixed density    
		    GetDlgItem(IDC_CHECK_DENSITY)->EnableWindow(FALSE);     // tick box    
		    
		    GetDlgItem(IDC_HEATER_BOX)->EnableWindow(FALSE);        // box around heater settings  
		    GetDlgItem(IDC_POWER_TEXT)->EnableWindow(FALSE);        // label              
		    GetDlgItem(IDC_EDIT_POWER)->EnableWindow(FALSE);        // edit box for heater power 
		    GetDlgItem(IDC_CHECK_POWER_ON)->EnableWindow(FALSE);    // heater on?  
		    break;
		    
		case MCNTRL_FIX_DENS_TEMP:      
			m_sCtrlSummary = "This monitor is currently set to control molecular densities and "
			                 "temperatures as specified by the settings below.";
			
		    GetDlgItem(IDC_FIXER_BOX)->EnableWindow(TRUE);         // box around fixer settings part of dialogue
		    GetDlgItem(IDC_FIXER_HEADING_MOLTYPE)->EnableWindow(TRUE); // title string above listbox
		    GetDlgItem(IDC_FIXER_HEADING_FIXEDT)->EnableWindow(TRUE);
		    GetDlgItem(IDC_FIXER_HEADING_TFIXED)->EnableWindow(TRUE);
		    GetDlgItem(IDC_FIXER_HEADING_FIXEDD)->EnableWindow(TRUE);
		    GetDlgItem(IDC_FIXER_HEADING_DFIXED)->EnableWindow(TRUE);		    		    		    		    
		    GetDlgItem(IDC_FIXER_LIST)->EnableWindow(TRUE);        // list of mol types, set densities and temps     
		    GetDlgItem(IDC_FOR_SELECTION)->EnableWindow(TRUE);     // text                     
		    GetDlgItem(IDC_EDIT_TEMPERATURE)->EnableWindow(TRUE);  // edit box for fixed temperature
		    GetDlgItem(IDC_CHECK_TEMPERATURE)->EnableWindow(TRUE); // tick box
		    GetDlgItem(IDC_EDIT_DENSITY)->EnableWindow(TRUE);      // edit box for fixed density    
		    GetDlgItem(IDC_CHECK_DENSITY)->EnableWindow(TRUE);     // tick box    
		    
		    GetDlgItem(IDC_HEATER_BOX)->EnableWindow(FALSE);        // box around heater settings  
		    GetDlgItem(IDC_POWER_TEXT)->EnableWindow(FALSE);        // label              
		    GetDlgItem(IDC_EDIT_POWER)->EnableWindow(FALSE);        // edit box for heater power 
		    GetDlgItem(IDC_CHECK_POWER_ON)->EnableWindow(FALSE);    // heater on?  
		    break;
		    
		case MCNTRL_HEATER: 
			m_sCtrlSummary = "This type of monitor is currently set to act as a heater/cooler "
			                 "using the settings below, but does no control of density or temperature.";
			
		    GetDlgItem(IDC_FIXER_BOX)->EnableWindow(FALSE);         // box around fixer settings part of dialogue
		    GetDlgItem(IDC_FIXER_HEADING_MOLTYPE)->EnableWindow(FALSE); // title string above listbox
		    GetDlgItem(IDC_FIXER_HEADING_FIXEDT)->EnableWindow(FALSE);
		    GetDlgItem(IDC_FIXER_HEADING_TFIXED)->EnableWindow(FALSE);
		    GetDlgItem(IDC_FIXER_HEADING_FIXEDD)->EnableWindow(FALSE);
		    GetDlgItem(IDC_FIXER_HEADING_DFIXED)->EnableWindow(FALSE);		    		    		    		    
		    GetDlgItem(IDC_FIXER_LIST)->EnableWindow(FALSE);        // list of mol types, set densities and temps     
		    GetDlgItem(IDC_FOR_SELECTION)->EnableWindow(FALSE);     // text                     
		    GetDlgItem(IDC_EDIT_TEMPERATURE)->EnableWindow(FALSE);  // edit box for fixed temperature
		    GetDlgItem(IDC_CHECK_TEMPERATURE)->EnableWindow(FALSE); // tick box
		    GetDlgItem(IDC_EDIT_DENSITY)->EnableWindow(FALSE);      // edit box for fixed density    
		    GetDlgItem(IDC_CHECK_DENSITY)->EnableWindow(FALSE);     // tick box    
		    
		    GetDlgItem(IDC_HEATER_BOX)->EnableWindow(TRUE);        // box around heater settings  
		    GetDlgItem(IDC_POWER_TEXT)->EnableWindow(TRUE);        // label              
		    GetDlgItem(IDC_EDIT_POWER)->EnableWindow(TRUE);        // edit box for heater power 
		    GetDlgItem(IDC_CHECK_POWER_ON)->EnableWindow(TRUE);    // heater on?  
		    break;
		    
		default:  
			ASSERT(FALSE);  
			break;
	}                           
	
	UpdateData(FALSE); // so that string copied to display
}

void CMonitorCtrlSetDlg::UpdateMolList()
{
	// draws list box with mol types and set densitites etc
	
	CListBox* pListBox = (CListBox*) GetDlgItem(IDC_FIXER_LIST);       
	
	pListBox->ResetContent();      
	
	CMolTypeStats* pStats;
	
	CString sFormatted;
	
    int nStatsList;
    
	for (nStatsList = 0; nStatsList <= m_pMonitor->m_molStatsList.GetUpperBound(); nStatsList++)
	{                     
		pStats = (CMolTypeStats*) m_pMonitor->m_molStatsList[nStatsList];
		
		sFormatted = pStats->m_pMolType->m_sName; 
		
		sFormatted += "\t";
				
		sprintf(buff, "%g", pStats->m_dFixedTemperature);
		sFormatted += buff;

		sFormatted += "\t";
        
        if (pStats->m_nTemperatureFixed)
        {
        	sFormatted += "y";
        }
        else
        {
        	sFormatted += "n";
        }

		sFormatted += "\t";

		sprintf(buff, "%g", pStats->m_dFixedDensity);
		sFormatted += buff;

		sFormatted += "\t";
        		
		if (pStats->m_nDensityFixed)
		{   
        	sFormatted += "y";
        }
        else
        {
        	sFormatted += "n";
        }
		
		pListBox->AddString(sFormatted);
	}
}

void CMonitorCtrlSetDlg::ApplyEdits()
{                                  
    UpdateData(TRUE);   // copy user settings to variables
    
    m_pMonitor->m_dHeaterPower = m_dHeaterPower;
    
    if (m_bHeaterActive)
    {
    	m_pMonitor->m_dHeaterOn = 1;
    }
    else                            
    {
    	m_pMonitor->m_dHeaterOn = 0;
    }
    
    CEdit* pTemperatureEdit = (CEdit*) GetDlgItem(IDC_EDIT_TEMPERATURE);
    CEdit* pDensityEdit = (CEdit*) GetDlgItem(IDC_EDIT_DENSITY);        
    CButton* pTemperatureFixed = (CButton*) GetDlgItem(IDC_CHECK_TEMPERATURE);
    CButton* pDensityFixed = (CButton*) GetDlgItem(IDC_CHECK_DENSITY);
    
	if (LB_ERR == m_nOldMolSeln)
	{
		// no current selection
		
		TRACE("In apply edits, no current selection\n");
	}
	else
	{                                                   
		TRACE("In apply edits, selection no. = %d\n", m_nOldMolSeln);
		
		CMolTypeStats* pStats = (CMolTypeStats*) m_pMonitor->m_molStatsList[m_nOldMolSeln];
		
		TRACE("Copying to stats T = %g, dens = %g\n",
		                m_dTemperatureSelected, m_dDensitySelected);
		                
		pStats->m_dFixedTemperature = m_dTemperatureSelected;
		pStats->m_dFixedDensity = m_dDensitySelected;
		pStats->m_nTemperatureFixed = m_nTemperatureFixedCheck;
		pStats->m_nDensityFixed = m_nDensityFixedCheck;
	}
    
    UpdateMolList();
}
/////////////////////////////////////////////////////////////////////////////
// CMonitorCtrlSetDlg message handlers

void CMonitorCtrlSetDlg::OnSelchangeFixerList()
{
    CListBox* pListBox = (CListBox*) GetDlgItem(IDC_FIXER_LIST);       

	int nNewSeln = pListBox->GetCurSel();
	
	ApplyEdits();   // to last mol type selected, wipes selection

    pListBox->SetCurSel(nNewSeln); // restore selection
	m_nOldMolSeln = nNewSeln; // remember new seln    
	
	UpdateEditBoxes();  // with new seln
}

void CMonitorCtrlSetDlg::OnOK()
{
	OnSelchangeFixerList();   
	
	CDialog::OnOK();
}

void CMonitorCtrlSetDlg::OnCheckDensity()
{
	OnSelchangeFixerList();
}

void CMonitorCtrlSetDlg::OnCheckTemperature()
{
	OnSelchangeFixerList();
}

void CMonitorCtrlSetDlg::OnKillfocusEditDensity()
{
	OnSelchangeFixerList();
}

void CMonitorCtrlSetDlg::OnKillfocusEditTemperature()
{
	OnSelchangeFixerList();
}

void CMonitorCtrlSetDlg::OnTypePropsBtn()
{
	m_pMonitorType->PropertiesCommand();
	
	EnableControls();  // may now be fixer instead of heater etc
}

