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

// drwmoldg.cpp : implementation file
//

#include "stdafx.h"   
#include "simobjs.h"
#include "mol.h"
#include "lennard.h"
#include "drwmoldg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawMolsDlg dialog


CDrawMolsDlg::CDrawMolsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDrawMolsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDrawMolsDlg)
	m_dDirectionAngle = 0;
	m_nDirectionType = -1;
	m_dFixedSpeedValue = 0;
	m_nSpeedType = -1;
	m_dThermalTemperatureValue = 100;
	//}}AFX_DATA_INIT
}

void CDrawMolsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDrawMolsDlg)
	DDX_Text(pDX, IDC_DIRECTION_ANGLE, m_dDirectionAngle);
	DDV_MinMaxDouble(pDX, m_dDirectionAngle, -180., 360.);
	DDX_Radio(pDX, IDC_DIRECTION_RANDOM, m_nDirectionType);
	DDX_Text(pDX, IDC_FIXED_SPEED_VALUE, m_dFixedSpeedValue);
	DDV_MinMaxDouble(pDX, m_dFixedSpeedValue, 0., 1000.);
	DDX_Radio(pDX, IDC_SYSTEM_SPEED, m_nSpeedType);
	DDX_Text(pDX, IDC_THERMAL_TEMPERATURE, m_dThermalTemperatureValue);
	DDV_MinMaxDouble(pDX, m_dThermalTemperatureValue, LIM_MIN_TEMP, LIM_MAX_TEMP);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDrawMolsDlg, CDialog)
	//{{AFX_MSG_MAP(CDrawMolsDlg)
	ON_EN_SETFOCUS(IDC_FIXED_SPEED_VALUE, OnSetfocusFixedSpeedValue)
	ON_EN_SETFOCUS(IDC_THERMAL_TEMPERATURE, OnSetfocusThermalTemperature)
	ON_EN_SETFOCUS(IDC_DIRECTION_ANGLE, OnSetfocusDirectionAngle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDrawMolsDlg message handlers

void CDrawMolsDlg::OnSetfocusFixedSpeedValue()
{
	// User is editing fixed speed value, so set corresponding radio button
	UpdateData(TRUE);   // remember all user settings
	m_nSpeedType = DM_FIXED_SPEED;
	UpdateData(FALSE); 
}

void CDrawMolsDlg::OnSetfocusThermalTemperature()
{
	// User is editing specific thermal temp, so set corresponding radio button
	UpdateData(TRUE);   // remember all user settings
	m_nSpeedType = DM_SPECIFIC_THERMAL;
	UpdateData(FALSE); 	
}

void CDrawMolsDlg::OnSetfocusDirectionAngle()
{
	// Like others, set corresponding radio button to edit box being used
	UpdateData(TRUE);   // remember all user settings
	m_nDirectionType = DM_DIRECTION_FIXED;
	UpdateData(FALSE);	
}
