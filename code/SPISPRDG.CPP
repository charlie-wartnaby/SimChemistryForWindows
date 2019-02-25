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

// spisprdg.cpp : implementation file
//

#include "stdafx.h"
#include "lennard.h"
#include "simobjs.h"
#include "sim.h"
#include "wall.h"
#include "piston.h"
#include "spisprdg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSPistonPropsDlg dialog


CSPistonPropsDlg::CSPistonPropsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSPistonPropsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSPistonPropsDlg)
	m_dHeight = 0;
	m_dVx = 0;
	m_dVy = 0;
	m_dWidth = 0;
	m_dXLeft = 0;
	m_dXLRest = 0;
	m_dXRight = 0;
	m_dYBottom = 0;
	m_dYTop = 0;
	m_dYTRest = 0;
	//}}AFX_DATA_INIT
}

void CSPistonPropsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSPistonPropsDlg)
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_dHeight);
	DDV_MinMaxDouble(pDX, m_dHeight, 0., 1000.);
	DDX_Text(pDX, IDC_EDIT_VX, m_dVx);
	DDV_MinMaxDouble(pDX, m_dVx, -LIM_MAX_PISTON_SPEED, LIM_MAX_PISTON_SPEED);
	DDX_Text(pDX, IDC_EDIT_VY, m_dVy);
	DDV_MinMaxDouble(pDX, m_dVy, -LIM_MAX_PISTON_SPEED, LIM_MAX_PISTON_SPEED);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_dWidth);
	DDV_MinMaxDouble(pDX, m_dWidth, 0., 1000.);
	DDX_Text(pDX, IDC_EDIT_XLEFT, m_dXLeft);
	DDV_MinMaxDouble(pDX, m_dXLeft, LIM_MIN_X, LIM_MAX_X);
	DDX_Text(pDX, IDC_EDIT_XLREST, m_dXLRest);
	DDV_MinMaxDouble(pDX, m_dXLRest, LIM_MIN_X, LIM_MAX_X);
	DDX_Text(pDX, IDC_EDIT_XRIGHT, m_dXRight);
	DDV_MinMaxDouble(pDX, m_dXRight, LIM_MIN_X, LIM_MAX_X);
	DDX_Text(pDX, IDC_EDIT_YBOT, m_dYBottom);
	DDV_MinMaxDouble(pDX, m_dYBottom, LIM_MIN_Y, LIM_MAX_Y);
	DDX_Text(pDX, IDC_EDIT_YTOP, m_dYTop);
	DDV_MinMaxDouble(pDX, m_dYTop, LIM_MIN_Y, LIM_MAX_Y);
	DDX_Text(pDX, IDC_EDIT_YTREST, m_dYTRest);
	DDV_MinMaxDouble(pDX, m_dYTRest, LIM_MIN_Y, LIM_MAX_Y);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSPistonPropsDlg, CDialog)
	//{{AFX_MSG_MAP(CSPistonPropsDlg)
	ON_BN_CLICKED(ID_OKBTN, OnOkbtn)
	ON_BN_CLICKED(IDC_GRID_SETTINGS_BTN, OnGridSettingsBtn)
	ON_BN_CLICKED(IDC_GRIDIFY_BTN, OnGridifyBtn)
	ON_BN_CLICKED(IDC_TYPE_PROPS_BTN, OnTypePropsBtn)
	ON_EN_KILLFOCUS(IDC_EDIT_HEIGHT, OnKillfocusEditHeight)
	ON_EN_KILLFOCUS(IDC_EDIT_WIDTH, OnKillfocusEditWidth)
	ON_EN_KILLFOCUS(IDC_EDIT_XRIGHT, OnKillfocusEditXright)
	ON_EN_KILLFOCUS(IDC_EDIT_YBOT, OnKillfocusEditYbot)
	ON_EN_KILLFOCUS(IDC_EDIT_XLEFT, OnKillfocusEditXleft)
	ON_EN_KILLFOCUS(IDC_EDIT_YTOP, OnKillfocusEditYtop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSPistonPropsDlg::OnInitDialog()
{                       
	// Round all of the numbers we have got, which would otherwise
	// be shown to some enormous precision in the edit boxes

	RoundNumbers();

	CDialog::OnInitDialog();

	DisplayGridSettings();
	
	DisableOneVelocity();
		
	// Display piston type name
	(GetDlgItem(IDC_PISTON_TYPE_TEXT))->SetWindowText("Piston Type:  " 
	                                                + m_pPistonType->m_sName);
	
	
	return TRUE; // Windows will then highlight first edit box
}

void CSPistonPropsDlg::DisplayGridSettings()
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

void CSPistonPropsDlg::DisableOneVelocity()
{                        
	// If this piston type has horizontal freedeom of movement, don't
	// want user to be able to put in finite vertical vel, etc
	
	if (PT_VERTICAL == m_pPistonType->m_nDirectionType)
	{
		m_dVx = 0; 
		(GetDlgItem(IDC_EDIT_VX))->EnableWindow(FALSE);
		(GetDlgItem(IDC_EDIT_VY))->EnableWindow(TRUE);
	}
	else
	{
		m_dVy = 0; 
		(GetDlgItem(IDC_EDIT_VX))->EnableWindow(TRUE);
		(GetDlgItem(IDC_EDIT_VY))->EnableWindow(FALSE);
	}        
	
	UpdateData(FALSE); // displays new values
}

void CSPistonPropsDlg::OnOK()
{
	// doesn't close dialogue if return pressed          
	
	// update as if user moved to another field and back	
	PrevDlgCtrl();
	NextDlgCtrl();	
}

void CSPistonPropsDlg::RoundNumbers()
{
	// In general a moving piston will not be at nice round
	// co-ords; round the numbers for display in edit boxes
	
	CSim::Round(m_dVx, 0.01);
	CSim::Round(m_dVy, 0.01);
	CSim::Round(m_dXLeft, 0.001);
	CSim::Round(m_dYBottom, 0.001);	
	CSim::Round(m_dXRight, 0.001);
	CSim::Round(m_dYTop, 0.001);	
	CSim::Round(m_dXLRest, 0.001);
	CSim::Round(m_dYTRest, 0.001);	

	m_dWidth = m_dXRight - m_dXLeft;
	m_dHeight = m_dYTop - m_dYBottom;
}
/////////////////////////////////////////////////////////////////////////////
// CSPistonPropsDlg message handlers

void CSPistonPropsDlg::OnOkbtn()
{
	// Have a separate handler for this to prevent return dismissing
	// the dialogue normally
	
	CDialog::OnOK();
}

void CSPistonPropsDlg::OnGridSettingsBtn()
{
	m_pSim->OnGridSettings(); 
	
	DisplayGridSettings();
}

void CSPistonPropsDlg::OnGridifyBtn()
{
	m_pSim->Round(m_dXLeft, m_pSim->m_dGridSize);
	m_pSim->Round(m_dYTop, m_pSim->m_dGridSize);
	m_pSim->Round(m_dXRight, m_pSim->m_dGridSize);
	m_pSim->Round(m_dYBottom, m_pSim->m_dGridSize);
	m_pSim->Round(m_dXLRest, m_pSim->m_dGridSize);
	m_pSim->Round(m_dYTRest, m_pSim->m_dGridSize);
	
	m_dWidth = m_dXRight - m_dXLeft;
	m_dHeight = m_dYTop - m_dYBottom;
	
	UpdateData(FALSE);   // update edit boxes
}

void CSPistonPropsDlg::OnTypePropsBtn()
{
	m_pPistonType->PropertiesCommand();	
	
	DisableOneVelocity();
}

void CSPistonPropsDlg::OnKillfocusEditHeight()
{
	UpdateData(TRUE);  // does data exchange
	
	// Height changed, so alter bottom y coord
	
	m_dYBottom = m_dYTop - m_dHeight; 
	
	RoundNumbers();

	UpdateData(FALSE); // displays new values	
}

void CSPistonPropsDlg::OnKillfocusEditWidth()
{
	UpdateData(TRUE);  // does data exchange
	
	// Width changed, so alter x right coord
	
	m_dXRight = m_dXLeft + m_dWidth;
	
	RoundNumbers();

	UpdateData(FALSE); // displays new values	
}

void CSPistonPropsDlg::OnKillfocusEditXright()
{
	UpdateData(TRUE);  // does data exchange
	
	m_dXLeft = m_dXRight - m_dWidth;
	
	RoundNumbers();

	UpdateData(FALSE); // displays new values	
}

void CSPistonPropsDlg::OnKillfocusEditYbot()
{
	UpdateData(TRUE);  // does data exchange
	
	m_dYTop = m_dYBottom + m_dHeight;
	
	RoundNumbers();

	UpdateData(FALSE); // displays new values	
}

void CSPistonPropsDlg::OnKillfocusEditXleft()
{
	UpdateData(TRUE);  // does data exchange
	
	m_dXRight = m_dXLeft + m_dWidth;
	
	RoundNumbers();

	UpdateData(FALSE); // displays new values
}

void CSPistonPropsDlg::OnKillfocusEditYtop()
{
	UpdateData(TRUE);  // does data exchange
	
	m_dYBottom = m_dYTop - m_dHeight;
	
	RoundNumbers();

	UpdateData(FALSE); // displays new values	
}
