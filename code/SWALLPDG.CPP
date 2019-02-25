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

// swallpdg.cpp : implementation file
//

#include "stdafx.h"
#include "lennard.h"    
#include "simobjs.h"
#include "wall.h"
#include "sim.h"
#include "swallpdg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSWallPropsDlg dialog


CSWallPropsDlg::CSWallPropsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSWallPropsDlg::IDD, pParent)
{    
	m_pSim = NULL;
	
	//{{AFX_DATA_INIT(CSWallPropsDlg)
	m_dX1 = 0;
	m_dX2 = 0;
	m_dY1 = 0;
	m_dY2 = 0;
	//}}AFX_DATA_INIT
}

void CSWallPropsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSWallPropsDlg)
	DDX_Text(pDX, IDC_EDIT_X1, m_dX1);
	DDV_MinMaxDouble(pDX, m_dX1, LIM_MIN_X, LIM_MAX_X);
	DDX_Text(pDX, IDC_EDIT_X2, m_dX2);
	DDV_MinMaxDouble(pDX, m_dX2, LIM_MIN_X, LIM_MAX_X);
	DDX_Text(pDX, IDC_EDIT_Y1, m_dY1);
	DDV_MinMaxDouble(pDX, m_dY1, LIM_MIN_Y, LIM_MAX_Y);
	DDX_Text(pDX, IDC_EDIT_Y2, m_dY2);
	DDV_MinMaxDouble(pDX, m_dY2, LIM_MIN_Y, LIM_MAX_Y);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSWallPropsDlg, CDialog)
	//{{AFX_MSG_MAP(CSWallPropsDlg)
	ON_BN_CLICKED(ID_OKBTN, OnOkbtn)
	ON_BN_CLICKED(IDC_GRID_SETTINGS_BTN, OnGridSettingsBtn)
	ON_BN_CLICKED(IDC_GRIDIFY_BTN, OnGridifyBtn)
	ON_BN_CLICKED(IDC_TYPE_PROPS_BTN, OnTypePropsBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSWallPropsDlg::OnInitDialog()
{ 
	CDialog::OnInitDialog();
	                      
	// Display wall type name
	(GetDlgItem(IDC_WAL_TYPE_TEXT))->SetWindowText("Wall Type:  " 
	                                               + m_pWallType->m_sName);

	DisplayGridSettings();	
	
	return TRUE; // Windows will then highlight first edit box
}

void CSWallPropsDlg::DisplayGridSettings()
{
	CString string("The system grid is currently ");
	
	if (m_pSim->m_nUseGrid)
	{
		string += "on ";
	}
	else
	{
		string += "off ";
	}
	
	string += "and set to a size of ";
	
	sprintf(buff, "%g nm:", m_pSim->m_dGridSize);
	
	string += buff;

	(GetDlgItem(IDC_GRID_SETTINGS_TEXT))->SetWindowText(string); 
}

/////////////////////////////////////////////////////////////////////////////
// CSWallPropsDlg message handlers

void CSWallPropsDlg::OnOkbtn()
{
	// Have a separate handler for this to prevent return dismissing
	// the dialogue normally
	
	CDialog::OnOK();
}

void CSWallPropsDlg::OnOK()
{
	// doesn't close dialogue if return pressed
}

void CSWallPropsDlg::OnGridSettingsBtn()
{
	m_pSim->OnGridSettings(); 
	
	DisplayGridSettings();
}

void CSWallPropsDlg::OnGridifyBtn()
{
	m_pSim->Round(m_dX1, m_pSim->m_dGridSize);
	m_pSim->Round(m_dY1, m_pSim->m_dGridSize);
	m_pSim->Round(m_dX2, m_pSim->m_dGridSize);
	m_pSim->Round(m_dY2, m_pSim->m_dGridSize);
	
	UpdateData(FALSE);   // update edit boxes
}

void CSWallPropsDlg::OnTypePropsBtn()
{
	m_pWallType->PropertiesCommand();
}
