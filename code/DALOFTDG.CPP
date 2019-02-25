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

// daloftdg.cpp : implementation file
//

#include "stdafx.h"    
#include "simobjs.h"
#include "sim.h"
#include "lennard.h"
#include "daloftdg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDelAllOfTypeDlg dialog


CDelAllOfTypeDlg::CDelAllOfTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDelAllOfTypeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDelAllOfTypeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CDelAllOfTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDelAllOfTypeDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDelAllOfTypeDlg, CDialog)
	//{{AFX_MSG_MAP(CDelAllOfTypeDlg)
	ON_BN_CLICKED(IDC_DEL_SELN_BTN, OnDelSelnBtn)
	ON_LBN_DBLCLK(IDC_TYPE_LIST, OnDblclkTypeList)
	ON_LBN_SELCHANGE(IDC_TYPE_LIST, OnSelchangeTypeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CDelAllOfTypeDlg::OnInitDialog()
{
	// Need to fill list box with names of current object types
	
	UpdateTypeList();  

   	return TRUE; // leaves it to Windows to highlight first control
}

void CDelAllOfTypeDlg::UpdateTypeList()
{
	CListBox* pTypeList = (CListBox*) GetDlgItem(IDC_TYPE_LIST);   
	
	pTypeList->ResetContent();
	
    int index;
	for (index = 0; index <= m_pSim->m_typeArray.GetUpperBound(); index++)
	{
		CObjType* pType = (CObjType*) m_pSim->m_typeArray[index];
		pTypeList->AddString(pType->m_sName);
	}                                 
	
	OnSelchangeTypeList();       
}    

/////////////////////////////////////////////////////////////////////////////
// CDelAllOfTypeDlg message handlers

void CDelAllOfTypeDlg::OnDelSelnBtn()
{
	// Tell sim to delete all of the objects of the selected types,
	// if there are any

	CListBox* pTypeList = (CListBox*) GetDlgItem(IDC_TYPE_LIST);
	
	int seln_indices[256];         
	int nSelns;
	CString cmd;
		
	// get list of selected indices
	
	nSelns = pTypeList->GetSelItems(256, (LPINT) seln_indices);
	
	for (int i = 0; i < nSelns; i++)
	{            
		cmd = " del_objs \"";   
		
		CObjType* pType = (CObjType*) m_pSim->m_typeArray[seln_indices[i]];
		cmd += pType->m_sName;
		
		cmd += "\"";
		
		m_pSim->HandleNewCommand(cmd);
	}                            
	
	UpdateTypeList();
}

void CDelAllOfTypeDlg::OnDblclkTypeList()
{
	// If user double-clicks on selection, do deletion without
	// requiring them to click delete button.  
	
	OnDelSelnBtn();
}

void CDelAllOfTypeDlg::OnSelchangeTypeList()
{
	// Enable or disable delete button depending on whether
	// any object types are selected
	
	CListBox* pTypeList = (CListBox*) GetDlgItem(IDC_TYPE_LIST);
	CButton* pDelBtn = (CButton*) GetDlgItem(IDC_DEL_SELN_BTN);
		
	pDelBtn->EnableWindow(0 != pTypeList->GetSelCount());
}
