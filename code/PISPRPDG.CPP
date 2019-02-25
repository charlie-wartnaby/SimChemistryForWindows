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

// pisprpdg.cpp : implementation file
//

#include "stdafx.h"   
#include "simobjs.h"
#include "sim.h"
#include "wall.h"
#include "piston.h"
#include "lennard.h"
#include "pisprpdg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditPistonPropsDlg dialog


CEditPistonPropsDlg::CEditPistonPropsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditPistonPropsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditPistonPropsDlg)
	m_dFixedPressure = 0.1;
	m_dForceConst = 0.1;  
	m_dDampingConst = 0;
	m_dMass = 500;
	m_nPressureType = 0;
	m_sName = "unnamed piston";
	m_nWallTypeIndex = 0;   
	m_nDirectionType = 0;
	//}}AFX_DATA_INIT
	m_pWallType = NULL;

}

void CEditPistonPropsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditPistonPropsDlg)
	DDX_Text(pDX, IDC_FIXED_PRESSURE_EDIT, m_dFixedPressure);
	DDV_MinMaxDouble(pDX, m_dFixedPressure, -LIM_MAX_PISTON_PRESSURE, LIM_MAX_PISTON_PRESSURE);
	DDX_Text(pDX, IDC_FORCE_CONST_EDIT, m_dForceConst);
	DDV_MinMaxDouble(pDX, m_dForceConst, 0, LIM_MAX_PISTON_FCONST);
	DDX_Text(pDX, IDC_DAMPING_CONST, m_dDampingConst);
	DDV_MinMaxDouble(pDX, m_dDampingConst, 0, LIM_MAX_PISTON_DAMP);
	DDX_Text(pDX, IDC_PISTON_MASS, m_dMass);
	DDV_MinMaxDouble(pDX, m_dMass, LIM_MIN_PISTON_MASS, LIM_MAX_PISTON_MASS);
	DDX_Radio(pDX, IDC_PRESSURE_FIXED, m_nPressureType);
	DDX_Text(pDX, IDC_TYPE_NAME, m_sName);
	DDV_MaxChars(pDX, m_sName, LIM_MAX_CHARS_OBJTYPE);
	DDX_CBIndex(pDX, IDC_WALL_TYPE_COMBO, m_nWallTypeIndex);
	DDX_Radio(pDX, IDC_RADIO_VERTICAL, m_nDirectionType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditPistonPropsDlg, CDialog)
	//{{AFX_MSG_MAP(CEditPistonPropsDlg)
	ON_EN_SETFOCUS(IDC_FORCE_CONST_EDIT, OnSetfocusForceConstEdit)
	ON_EN_SETFOCUS(IDC_FIXED_PRESSURE_EDIT, OnSetfocusFixedPressureEdit)
	ON_CBN_SELCHANGE(IDC_WALL_TYPE_COMBO, OnSelchangeWallTypeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CEditPistonPropsDlg::OnInitDialog()
{
	// Here the dropdown list box is initialised with the names
	// of the wall types we know about

	UpdateData(FALSE); // fill in everything else

	m_wallTypeList.RemoveAll(); 
	m_pSim->GetTypeArray(m_wallTypeList, CI_WALL);   
	
	if (m_wallTypeList.GetSize() <= 0)
	{
		// this should never happen; the dialogue only gets invoked by
		// the document class if there are wall types
		AfxMessageBox("No wall types defined; cannot make a new piston type");
		OnCancel();
	}
	
	CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_WALL_TYPE_COMBO);
	
	pCombo->ResetContent();

	// If we have been given a wall type, make that initial
	// selection, otherwise select first by default. Should
	// be given wall type if editing properties of existing
	// piston type but not if creating new type.
	
		
	for (int i = 0; i <= m_wallTypeList.GetUpperBound(); i++)
	{ 
		CObjTypeWall* pWallType = (CObjTypeWall*) m_wallTypeList[i]; 
		pCombo->AddString(pWallType->m_sName);
		
		if (pWallType == m_pWallType)
		{                  
			pCombo->SetCurSel(i);  
		}	
	}  
	
	if (NULL == m_pWallType)
	{
		pCombo->SetCurSel(0);  
	
		m_pWallType = (CObjTypeWall*) m_wallTypeList[0];
	}

	return TRUE; // leaves it to Windows to highlight first control
}

/////////////////////////////////////////////////////////////////////////////
// CEditPistonPropsDlg message handlers

void CEditPistonPropsDlg::OnSetfocusForceConstEdit()
{   
	// if user is editing force constant, ensure corresponding radio button is checked
	UpdateData(TRUE);  // transfer user numbers to variables
	m_nPressureType = PT_SPRING; // set variable for spring pressure type
	UpdateData(FALSE);  // transfer variables to user numbers
}

void CEditPistonPropsDlg::OnSetfocusFixedPressureEdit()
{
	// if user is editing force constant, ensure corresponding radio button is checked   
	UpdateData(TRUE);
	m_nPressureType = PT_FIXED;
	UpdateData(FALSE);
}

void CEditPistonPropsDlg::OnSelchangeWallTypeCombo()
{
	CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_WALL_TYPE_COMBO);
	
	int selNum = pCombo->GetCurSel();
	                                                   
	ASSERT((selNum >= 0) && (selNum <= m_wallTypeList.GetUpperBound()));
	
	m_pWallType = (CObjTypeWall*) m_wallTypeList[selNum];
}
