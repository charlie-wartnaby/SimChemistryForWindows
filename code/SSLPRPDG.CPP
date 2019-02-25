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

// sslprpdg.cpp : implementation file
//

#include "stdafx.h"
#include "lennard.h"  
#include "simobjs.h"
#include "sim.h"
#include "sslprpdg.h"
#include "selnumdg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSSliderPropsDlg dialog


CSSliderPropsDlg::CSSliderPropsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSSliderPropsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSSliderPropsDlg)
	m_nCheckTitle = TRUE;
	m_dScaleMax = LIM_MAX_TEMP;
	m_dScaleMin = LIM_MIN_TEMP;
	m_sTitle = "System Temperature Control";
	//}}AFX_DATA_INIT
	
	m_pSim = NULL;  
	m_sData = "sim:temperature"; 
	m_dAllowedMin = LIM_MIN_TEMP;
	m_dAllowedMax = LIM_MAX_TEMP; 
	m_nScaleLimsInit = FALSE;    
	m_nNumericSetOnceAlready = FALSE;
}

void CSSliderPropsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSSliderPropsDlg)
	DDX_Check(pDX, IDC_CHECK_TITLE, m_nCheckTitle);
	DDX_Text(pDX, IDC_EDIT_MAX_VALUE, m_dScaleMax);
	DDV_MinMaxDouble(pDX, m_dScaleMax, m_dAllowedMin, m_dAllowedMax);
	DDX_Text(pDX, IDC_EDIT_MIN_VALUE, m_dScaleMin);
	DDV_MinMaxDouble(pDX, m_dScaleMin, m_dAllowedMin, m_dAllowedMax);
	DDX_Text(pDX, IDC_EDIT_TITLE, m_sTitle);
	DDV_MaxChars(pDX, m_sTitle, LIM_MAX_CHARS_SLIDER_TITLE);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSSliderPropsDlg, CDialog)
	//{{AFX_MSG_MAP(CSSliderPropsDlg)
	ON_BN_CLICKED(IDC_CHECK_TITLE, OnCheckTitle)
	ON_BN_CLICKED(IDC_SELECT_BTN, OnSelectBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSSliderPropsDlg::OnInitDialog()
{                  
	CDialog::OnInitDialog();
	
	InitialiseMinMax(); 

	UpdateFields(); 
	
	return TRUE; // Windows highlights first control
}

void CSSliderPropsDlg::InitialiseMinMax()
{     
	CNumeric* pNumeric = m_pSim->FetchNumeric(m_sData);
	
	if (NULL != pNumeric)
	{
		m_dAllowedMin = pNumeric->m_dMin;
		m_dAllowedMax = pNumeric->m_dMax;

		if (m_nScaleLimsInit && !m_nNumericSetOnceAlready)
		{
			// preserve existing lims if OK
			if (m_dScaleMin < m_dAllowedMin || m_dScaleMin > m_dAllowedMax)
			{
				m_dScaleMin = m_dAllowedMin;
			}
			
			if (m_dScaleMax < m_dAllowedMin || m_dScaleMax > m_dAllowedMax)
			{
				m_dScaleMax = m_dAllowedMax;
			}
		}   
		else
		{    
			m_dScaleMin = m_dAllowedMin;  
			m_dScaleMax = m_dAllowedMax;
		}
 		
		GetDlgItem(IDC_EDIT_MIN_VALUE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_MAX_VALUE)->EnableWindow(TRUE);
	}    
	else
	{
		m_dScaleMin = 0;
		m_dScaleMax = 0;
		m_dAllowedMin = 0;
		m_dAllowedMax = 0;

		GetDlgItem(IDC_EDIT_MIN_VALUE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MAX_VALUE)->EnableWindow(FALSE);
	}
	
	UpdateData(FALSE); // copy numbers to edit boxes     

	// If come here again, will be because new numeric chosen,
	// in which case want to get new scale min and max limits:	
	m_nNumericSetOnceAlready = TRUE;
}

void CSSliderPropsDlg::UpdateFields()
{     
	// Enable those edit fields that should be, depending
	// on what options selected, etc
	
	UpdateData(TRUE);   // remember all user settings in variables
	
	GetDlgItem(IDC_EDIT_TITLE)->EnableWindow(m_nCheckTitle);

    if (m_sData != "")
    {
    	GetDlgItem(IDC_SELECTED_TEXT)->SetWindowText("Selected: " + m_sData);
    	
    	CNumeric* pNumeric = m_pSim->FetchNumeric(m_sData);
    	
    	if (NULL != pNumeric)
    	{
    		GetDlgItem(IDC_UNITS_TEXT)->SetWindowText("Units: " + pNumeric->m_sUnits);
    	}
    	else
    	{
    		ASSERT(FALSE);
    	}
    	
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
	else                                     
	{  
		GetDlgItem(IDC_SELECTED_TEXT)->SetWindowText("(Please select quantity to control)");                                      
		GetDlgItem(IDC_UNITS_TEXT)->SetWindowText("Units: (nothing selected)");
		
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSSliderPropsDlg message handlers

void CSSliderPropsDlg::OnCheckTitle()
{
	UpdateFields();
}

void CSSliderPropsDlg::OnSelectBtn()
{   
	UpdateData(TRUE);  // else user changes lost in call to 
	                    // InitialiseMinMax below
	                    
	CSelNumericDlg dlg;
	
	dlg.m_pSim = m_pSim;
	dlg.m_nNumericType = NUM_USERWRITE;  // allow selection only of writeable things 
	dlg.m_sInitSeln = m_sData;
	
	if (IDOK == dlg.DoModal())
	{
		m_sData = dlg.m_sNumeric;
		
		CNumeric* pNumeric = m_pSim->FetchNumeric(dlg.m_sNumeric);
		
		if (NULL == pNumeric)
		{
			m_sData = "";  // they selected nothing     
			ASSERT(FALSE); // should be impossible
		}
		else
		{
			InitialiseMinMax();   
			m_sTitle = pNumeric->m_sDescription;   
			
			if (m_sTitle.GetLength() > LIM_MAX_CHARS_SLIDER_TITLE)
			{      
				m_sTitle = m_sTitle.Left(LIM_MAX_CHARS_SLIDER_TITLE);
			}
			
			GetDlgItem(IDC_EDIT_TITLE)->SetWindowText(m_sTitle);
		}
	}     
	
	UpdateFields();
}
