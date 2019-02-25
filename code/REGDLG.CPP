/////////////////////////////////////////////////////////////////////////////
//                      SimChemistry for Windows
// www.simchemistry.co.uk    / www.wartnaby.org
// chem @ simchemistry.co.uk / charlie @ wartnaby.org
//
// Copyright 2016 Charles Wartnaby
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

// regdlg.cpp : implementation file
//

#include "stdafx.h"
#include "lennard.h"
#include "regdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegisterDlg dialog


CRegisterDlg::CRegisterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRegisterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegisterDlg)
	m_sUserName = "";
	m_sRegCode = "";
	m_lNumUsers = 1;
	//}}AFX_DATA_INIT
}

void CRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegisterDlg)
	DDX_Text(pDX, IDC_EDIT_NAME, m_sUserName);
	DDX_Text(pDX, IDC_EDIT_REG_KEY, m_sRegCode);
	DDX_Text(pDX, IDC_EDIT_NUM_USERS, m_lNumUsers);
	DDV_MinMaxLong(pDX, m_lNumUsers, 0, 1000000000);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRegisterDlg, CDialog)
	//{{AFX_MSG_MAP(CRegisterDlg)
	ON_BN_CLICKED(IDC_ORDER_BTN, OnOrderBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CRegisterDlg::OnOK()
{    
	UpdateData(TRUE); // get user-typed reg code and user name
	
	switch (theApp.AuthenticateRegCode(m_sUserName, m_lNumUsers, m_sRegCode))
	{
		case AUTH_PASSED:
			// signature verifies, legit
			
			AfxMessageBox("Registration code valid, thank you very much"
			               " for your order!");
			               
			theApp.AcceptValidatedRegInfo(m_sUserName, m_lNumUsers, m_sRegCode);		               
					
			CDialog::OnOK();
			
			break;

		case AUTH_REGCODE_CHECKSUM_FAILS:
		
			AfxMessageBox("The registration code is not valid for any user; please"
			              " check that it is typed exactly as supplied."); 
			
			break;
			
		case AUTH_FAILED_BUT_REGCODE_CHECKSUM_OK:
		
			AfxMessageBox("The registration code is of the correct form but does not"
			              " match the name and number of users you have supplied."
			              " Please ensure that all three"
			              " are typed exactly as supplied.");                      
			              
			break;
			
		default:
			ASSERT(FALSE);
			
			AfxMessageBox("The registration code does not match the name and number"
			              " of users supplied;"
			              " please ensure that all three are"
			              " typed exactly as supplied.");
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRegisterDlg message handlers

void CRegisterDlg::OnOrderBtn()
{
	AfxGetApp()->WinHelp(IDD_REGISTER_DLG);
}
