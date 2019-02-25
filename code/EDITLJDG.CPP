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

// editljdg.cpp : implementation file
//

#include "stdafx.h"
#include "lennard.h"
#include "editljdg.h"  
#include "simobjs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditLJDetailsDlg dialog


CEditLJDetailsDlg::CEditLJDetailsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditLJDetailsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditLJDetailsDlg)
	m_dCutoffDist = 1;
	m_dEqmSepn = 0.1;
	m_dMaxPE = 500;
	m_dWellDepth = 1;
	//}}AFX_DATA_INIT
}

void CEditLJDetailsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditLJDetailsDlg)
	DDX_Text(pDX, IDC_CUTOFF_DIST, m_dCutoffDist);
	DDV_MinMaxDouble(pDX, m_dCutoffDist, LIM_MIN_LJ_CUTOFF, LIM_MAX_LJ_CUTOFF);
	DDX_Text(pDX, IDC_EQM_SEPN, m_dEqmSepn);
	DDV_MinMaxDouble(pDX, m_dEqmSepn, LIM_MIN_LJ_EQM_SEPN, LIM_MAX_LJ_EQM_SEPN);
	DDX_Text(pDX, IDC_MAX_PE, m_dMaxPE);
	DDV_MinMaxDouble(pDX, m_dMaxPE, LIM_MIN_CUT_PE, LIM_MAX_CUT_PE);
	DDX_Text(pDX, IDC_WELL_DEPTH, m_dWellDepth);
	DDV_MinMaxDouble(pDX, m_dWellDepth, LIM_MIN_WELL_DEPTH, LIM_MAX_WELL_DEPTH);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditLJDetailsDlg, CDialog)
	//{{AFX_MSG_MAP(CEditLJDetailsDlg)
	ON_BN_CLICKED(IDC_HELP_BTN, OnHelpBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEditLJDetailsDlg message handlers

void CEditLJDetailsDlg::OnHelpBtn()
{
#if (_MSC_VER <= 1299) /* MSVC 6 = 1200 */
	AfxGetApp()->WinHelp(IDD_EDIT_LJ_DETAILS);
#else
	::HtmlHelp(this->m_hWnd, "Lennard.chm", HH_DISPLAY_TOC, NULL);
#endif
}
