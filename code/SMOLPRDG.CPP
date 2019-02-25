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

// smolprdg.cpp : implementation file
//

#include "stdafx.h"
#include "lennard.h"  
#include "simobjs.h"
#include "sim.h"  
#include "mol.h"
#include "smolprdg.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSMolPropsDlg dialog


CSMolPropsDlg::CSMolPropsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSMolPropsDlg::IDD, pParent)
{          
	m_pSim = NULL;
	
	//{{AFX_DATA_INIT(CSMolPropsDlg)
	m_dDirection = 0;
	m_dSpeed = 0;
	m_dVx = 0;
	m_dVy = 0;
	m_dX = 0;
	m_dY = 0;
	m_dSpecificTemperature = 100;
	m_nUseSysTemp = SMOLPROP_USE_SYS_TEMP;
	//}}AFX_DATA_INIT
}

void CSMolPropsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSMolPropsDlg)
	DDX_Text(pDX, IDC_EDIT_DIRECTION, m_dDirection);
	DDV_MinMaxDouble(pDX, m_dDirection, -360., 360.);
	DDX_Text(pDX, IDC_EDIT_SPEED, m_dSpeed);
	DDV_MinMaxDouble(pDX, m_dSpeed, 0., LIM_MAX_MOL_SPEED);
	DDX_Text(pDX, IDC_EDIT_VX, m_dVx);
	DDV_MinMaxDouble(pDX, m_dVx, -LIM_MAX_MOL_SPEED, LIM_MAX_MOL_SPEED);
	DDX_Text(pDX, IDC_EDIT_VY, m_dVy);
	DDV_MinMaxDouble(pDX, m_dVy, -LIM_MAX_MOL_SPEED, LIM_MAX_MOL_SPEED);
	DDX_Text(pDX, IDC_EDIT_X, m_dX);
	DDV_MinMaxDouble(pDX, m_dX, LIM_MIN_X, LIM_MAX_X);
	DDX_Text(pDX, IDC_EDIT_Y, m_dY);
	DDV_MinMaxDouble(pDX, m_dY, LIM_MIN_Y, LIM_MAX_Y);
	DDX_Text(pDX, IDC_THIS_TEMP_EDIT, m_dSpecificTemperature);
	DDV_MinMaxDouble(pDX, m_dSpecificTemperature, 0, LIM_MAX_TEMP);
	DDX_Radio(pDX, IDC_RADIO_SYS_TEMP, m_nUseSysTemp);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSMolPropsDlg, CDialog)
	//{{AFX_MSG_MAP(CSMolPropsDlg)
	ON_BN_CLICKED(IDC_NEW_RANDOM_DIRN_BTN, OnNewRandomDirnBtn)
	ON_BN_CLICKED(IDC_NEW_THERM_SPEED_BTN, OnNewThermSpeedBtn)
	ON_BN_CLICKED(IDC_RADIO_SYS_TEMP, OnRadioSysTemp)
	ON_BN_CLICKED(IDC_RADIO_THIS_TEMP, OnRadioThisTemp)
	ON_BN_CLICKED(IDC_TYPE_PROPS_BTN, OnTypePropsBtn)
	ON_EN_KILLFOCUS(IDC_EDIT_DIRECTION, OnKillfocusEditDirection)
	ON_EN_KILLFOCUS(IDC_EDIT_SPEED, OnKillfocusEditSpeed)
	ON_EN_KILLFOCUS(IDC_EDIT_VX, OnKillfocusEditVx)
	ON_EN_KILLFOCUS(IDC_EDIT_VY, OnKillfocusEditVy)
	ON_EN_KILLFOCUS(IDC_THIS_TEMP_EDIT, OnKillfocusThisTempEdit)
	ON_BN_CLICKED(ID_OKBTN, OnOkbtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CSMolPropsDlg::OnInitDialog()
{                       
	// Round all of the numbers we have got, which would otherwise
	// be shown to some enormous precision in the edit boxes

	RoundNumbers();

	CDialog::OnInitDialog();

	// Disable edit box for temperature unless user chooses 
	// associated radio button:	
	(GetDlgItem(IDC_THIS_TEMP_EDIT))->EnableWindow(FALSE);
	
	// Display mol type name
	(GetDlgItem(IDC_MOL_TYPE_TEXT))->SetWindowText("Molecule Type:  " + m_pMolType->m_sName);
	
	// Display curren sys temp  
	sprintf(buff, "Current System Temperature (%g K)", m_pSim->m_dTemperature);
	(GetDlgItem(IDC_RADIO_SYS_TEMP))->SetWindowText(buff);	                   
	
	// Warn of max vel produced by thermal button
	sprintf(buff, "(max result %g m/s)", (double) LIM_MAX_MOL_SPEED);
	(GetDlgItem(IDC_MAX_VEL_TEXT))->SetWindowText(buff);
	
	return TRUE; // Windows will then highlight first edit box
}

void CSMolPropsDlg::RoundNumbers()
{   
	CSim::Round(m_dDirection, 0.1);
	CSim::Round(m_dSpeed, 0.01);
	CSim::Round(m_dVx, 0.01);
	CSim::Round(m_dVy, 0.01);
	CSim::Round(m_dX, 0.001);
	CSim::Round(m_dY, 0.001);
}

/////////////////////////////////////////////////////////////////////////////
// CSMolPropsDlg message handlers

void CSMolPropsDlg::OnNewRandomDirnBtn()
{
	// Get random direction for molecule
	
	double dDirnRadians = m_pSim->RandomDirection();
    
    m_dDirection = dDirnRadians * 180 / PI;
    
    if (m_dDirection > 180)
    {
    	m_dDirection = m_dDirection - 360;
    }
	// Direction changed, so keep speed constant but alter velocity
	// vector
	
	m_dVx = m_dSpeed * cos(dDirnRadians);
	m_dVy = m_dSpeed * sin(dDirnRadians);
	
	RoundNumbers();

	UpdateData(FALSE); // displays new values
	
}

void CSMolPropsDlg::OnNewThermSpeedBtn()
{
	if (SMOLPROP_USE_SYS_TEMP == m_nUseSysTemp)
	{
		// Radio button set to use system temperature
		
		m_dSpeed = m_pSim->ThermalSpeed(m_pSim->m_dTemperature, m_pMolType->m_dMass);
	}
	else
	{   
		// Radio button set to use user-specified temperature
		
		m_dSpeed = m_pSim->ThermalSpeed(m_dSpecificTemperature, m_pMolType->m_dMass);
	}
    
    // Clip speed to same limit user is allowed to enter
    
    if (m_dSpeed > LIM_MAX_MOL_SPEED) m_dSpeed = LIM_MAX_MOL_SPEED;
    
	// Speed changed, but keep direction constant
	
	m_dVx = m_dSpeed * cos(m_dDirection / 180 * PI);
	m_dVy = m_dSpeed * sin(m_dDirection / 180 * PI);
	
	RoundNumbers();

	UpdateData(FALSE); // displays new values
}

void CSMolPropsDlg::OnRadioSysTemp()
{
	// Has chosen to use system temperature, so grey out edit
	// box for specific temperature
	
	(GetDlgItem(IDC_THIS_TEMP_EDIT))->EnableWindow(FALSE);
}

void CSMolPropsDlg::OnRadioThisTemp()
{
	// Enable edit box for specific temperature
	(GetDlgItem(IDC_THIS_TEMP_EDIT))->EnableWindow(TRUE);
}

void CSMolPropsDlg::OnTypePropsBtn()
{
	m_pMolType->PropertiesCommand();
}

void CSMolPropsDlg::OnKillfocusEditDirection()
{
	UpdateData(TRUE);  // does data exchange
	
	// Direction changed, so keep speed constant but alter velocity
	// vector
	
	m_dVx = m_dSpeed * cos(m_dDirection / 180 * PI);
	m_dVy = m_dSpeed * sin(m_dDirection / 180 * PI);
	
	RoundNumbers();

	UpdateData(FALSE); // displays new values
}

void CSMolPropsDlg::OnKillfocusEditSpeed()
{
	UpdateData(TRUE);  // does data exchange
	
	// Speed changed, but keep direction constant
	
	m_dVx = m_dSpeed * cos(m_dDirection / 180 * PI);
	m_dVy = m_dSpeed * sin(m_dDirection / 180 * PI);
	
	RoundNumbers();

	UpdateData(FALSE); // displays new values
}

void CSMolPropsDlg::OnKillfocusEditVx()
{
	UpdateData(TRUE);  // does data exchange
	
	// Vx changed, keep Vy const and update speed and direction
	
	m_dSpeed = sqrt(m_dVx * m_dVx + m_dVy * m_dVy);
	
	m_dDirection = atan2(m_dVy, m_dVx) * 180 / PI;
	
	RoundNumbers();

	UpdateData(FALSE); // displays new values
}

void CSMolPropsDlg::OnKillfocusEditVy()
{
	UpdateData(TRUE);  // does data exchange
	
	// Vy changed, keep Vx const and update speed and direction
	
	m_dSpeed = sqrt(m_dVx * m_dVx + m_dVy * m_dVy);
	
	m_dDirection = atan2(m_dVy, m_dVx) * 180 / PI;
	
	RoundNumbers();

	UpdateData(FALSE); // displays new values
}

void CSMolPropsDlg::OnKillfocusThisTempEdit()
{
	UpdateData(TRUE); // displays new values
}

void CSMolPropsDlg::OnOkbtn()
{
	// Have a separate handler for this to prevent return dismissing
	// the dialogue normally
	
	CDialog::OnOK();
}      

void CSMolPropsDlg::OnOK()
{
	// doesn't close dialogue if return pressed

	// update as if user moved to another field and back	
	PrevDlgCtrl();
	NextDlgCtrl();
}

