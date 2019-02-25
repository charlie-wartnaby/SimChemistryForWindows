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

// newsvdg.cpp : implementation file
//

#include "stdafx.h"
#include "lennard.h"
#include "newsvdg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewSliderValueDlg dialog


CNewSliderValueDlg::CNewSliderValueDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewSliderValueDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewSliderValueDlg)
	m_dValue = 0;
	//}}AFX_DATA_INIT
}

void CNewSliderValueDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewSliderValueDlg)
	DDX_Text(pDX, IDC_EDIT_VALUE, m_dValue);
	DDV_MinMaxDouble(pDX, m_dValue, m_dMinValue, m_dMaxValue);
	//}}AFX_DATA_MAP
}

BOOL CNewSliderValueDlg::OnInitDialog()
{  
	CDialog::OnInitDialog();

	sprintf(buff, "Minimum Value = %g ", m_dMinValue);
		
	(GetDlgItem(IDC_MIN_VALUE_TEXT))->SetWindowText(buff + m_sUnits);

	sprintf(buff, "Maximum Value = %g ", m_dMaxValue);
		
	(GetDlgItem(IDC_MAX_VALUE_TEXT))->SetWindowText(buff + m_sUnits);
	
	return TRUE; // Windows highlights control
}

BEGIN_MESSAGE_MAP(CNewSliderValueDlg, CDialog)
	//{{AFX_MSG_MAP(CNewSliderValueDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNewSliderValueDlg message handlers
