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

// sysprpdg.cpp : implementation file
//

#include "stdafx.h"
#include "lennard.h"
#include "simobjs.h"
#include "sysprpdg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditSysPropsDlg dialog


CEditSysPropsDlg::CEditSysPropsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditSysPropsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditSysPropsDlg)
	m_dGravitationalAcceleration = 0;
	m_dSystemTemperature = 0;
	m_nAutoRun = FALSE;
	m_nUseAutoTimestep = FALSE;
	m_dTimestep = 0.0;
	m_dAverageTimeConst = 0.0;
	m_lRandSeed = 0;
	//}}AFX_DATA_INIT
}

#define MIN_GRAVITY (-LIM_MAX_GRAVITY) // keep MSVC class wizard happy

void CEditSysPropsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditSysPropsDlg)
	DDX_Text(pDX, IDC_EDIT_SYS_G, m_dGravitationalAcceleration);
	DDV_MinMaxDouble(pDX, m_dGravitationalAcceleration, MIN_GRAVITY, LIM_MAX_GRAVITY);
	DDX_Text(pDX, IDC_EDIT_SYS_T, m_dSystemTemperature);
	DDV_MinMaxDouble(pDX, m_dSystemTemperature, LIM_MIN_TEMP, LIM_MAX_TEMP);
	DDX_Check(pDX, IDC_CHECK_AUTORUN, m_nAutoRun);
	DDX_Check(pDX, IDC_CHECK_AUTO_TIMESTEP, m_nUseAutoTimestep);
	DDX_Text(pDX, IDC_EDIT_TIMESTEP, m_dTimestep);
	DDV_MinMaxDouble(pDX, m_dTimestep, LIM_MIN_DELTA_TIME, LIM_MAX_DELTA_TIME);
	DDX_Text(pDX, IDC_EDIT_AVERAGING_TC, m_dAverageTimeConst);
	DDV_MinMaxDouble(pDX, m_dAverageTimeConst, LIM_MIN_AVERAGE_TC, LIM_MAX_AVERAGE_TC);
	DDX_Text(pDX, IDC_EDIT_RAND_SEED, m_lRandSeed);
	DDV_MinMaxLong(pDX, m_lRandSeed, -2147483647, 2147483647);
	//}}AFX_DATA_MAP
}

BOOL CEditSysPropsDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    UpdateEditBoxes();

    return TRUE;
}

void CEditSysPropsDlg::UpdateEditBoxes()
{
    CEdit* pEditTimestep = (CEdit*) GetDlgItem(IDC_EDIT_TIMESTEP);

    pEditTimestep->EnableWindow(!m_nUseAutoTimestep);
}

BEGIN_MESSAGE_MAP(CEditSysPropsDlg, CDialog)
	//{{AFX_MSG_MAP(CEditSysPropsDlg)
	ON_BN_CLICKED(IDC_HELP_BTN, OnHelpBtn)
	ON_BN_CLICKED(IDC_CHECK_AUTO_TIMESTEP, OnCheckAutoTimestep)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEditSysPropsDlg message handlers

void CEditSysPropsDlg::OnHelpBtn()
{
#if (_MSC_VER <= 1299) /* MSVC 6 = 1200 */
	AfxGetApp()->WinHelp(ID_SIMULATION_SYSTEMPROPERTIES);
#else
	::HtmlHelp(this->m_hWnd, "Lennard.chm", HH_DISPLAY_TOC, NULL);
#endif

}

void CEditSysPropsDlg::OnCheckAutoTimestep() 
{
	UpdateData(TRUE);

	UpdateEditBoxes();
}

