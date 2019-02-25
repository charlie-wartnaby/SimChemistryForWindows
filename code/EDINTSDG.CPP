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

// edintsdg.cpp : implementation file
//

#include "stdafx.h"
#include "simobjs.h"
#include "sim.h"
#include "baseint.h"
#include "lennard.h"
#include "edintsdg.h"
#include "htmlhelp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditIntsDlg dialog


CEditIntsDlg::CEditIntsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditIntsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditIntsDlg)
	m_sObjType2 = "";
	m_sObjType1 = "";
	//}}AFX_DATA_INIT
}

void CEditIntsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditIntsDlg)
	DDX_CBString(pDX, IDC_OBJ2_COMBO, m_sObjType2);
	DDV_MaxChars(pDX, m_sObjType2, LIM_MAX_CHARS_OBJTYPE);
	DDX_CBString(pDX, IDC_OBJ1_COMBO, m_sObjType1);
	DDV_MaxChars(pDX, m_sObjType1, LIM_MAX_CHARS_OBJTYPE);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditIntsDlg, CDialog)
	//{{AFX_MSG_MAP(CEditIntsDlg)
	ON_BN_CLICKED(IDC_EDIT_DETAILS_BTN, OnEditDetailsBtn)
	ON_BN_CLICKED(IDC_SHOW_FIRST_BTN, OnShowFirstBtn)
	ON_BN_CLICKED(IDC_SHOW_NEXT_BTN, OnShowNextBtn)
	ON_CBN_SELCHANGE(IDC_OBJ2_COMBO, OnSelchangeObj2Combo)
	ON_CBN_SELCHANGE(IDC_OBJ1_COMBO, OnSelchangeObj1Combo)
	ON_BN_CLICKED(IDC_MAKE_SEL_ACTIVE, OnMakeSelActive)
	ON_LBN_DBLCLK(IDC_INT_TYPE_LIST, OnDblclkIntTypeList)
	ON_LBN_SELCHANGE(IDC_INT_TYPE_LIST, OnSelchangeIntTypeList)
	ON_BN_CLICKED(IDC_HELP_BTN, OnHelpBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CEditIntsDlg::OnInitDialog()
{
	// Here the dropdown list boxes are initialised with the names
	// of the object types we know about
	
	CComboBox* pObjType1Box = (CComboBox*) GetDlgItem(IDC_OBJ1_COMBO);
	CComboBox* pObjType2Box = (CComboBox*) GetDlgItem(IDC_OBJ2_COMBO);
	
	pObjType1Box->ResetContent();
	pObjType2Box->ResetContent();
    
    int index;
	for (index = 0; index <= m_pSim->m_typeArray.GetUpperBound(); index++)
	{
		CObjType* pType = (CObjType*) m_pSim->m_typeArray[index];
		
		if (pType->m_nHasInts)
		{
			pObjType1Box->AddString(pType->m_sName);
			pObjType2Box->AddString(pType->m_sName);
		}
	}

	pObjType1Box->SetCurSel(0);
	pObjType2Box->SetCurSel(0);

	UpdateIntList();		
	
	return TRUE; // leaves it to Windows to highlight first control
}

void CEditIntsDlg::UpdateIntList()
{                  
	// The selected object types have changed and so we must update
	// the list of possible interactions
	
	CListBox* pIntListBox = (CListBox*) GetDlgItem(IDC_INT_TYPE_LIST);
	CEdit* pIntDescEdit = (CEdit*) GetDlgItem(IDC_INT_DESC_EDIT);	

	// Clear existing contents
	pIntListBox->ResetContent();
	pIntDescEdit->SetWindowText("");
	
	// Find out object type names by doing DDX
	UpdateData(TRUE);            
	
	CPtrList intList;
	int ret = m_pSim->GetIntsByObjTypeNames(m_sObjType1, m_sObjType2, &m_intArray); 
	CInt* pIntInUse = m_pSim->GetIntInUseByNames(m_sObjType1, m_sObjType2);
	
	CInt* pInt;    
	CString string;
	int index = 0;
	for (index = 0; index <= m_intArray.GetUpperBound(); index++)
	{
		pInt = (CInt*) m_intArray[index];
		string = pInt->m_sName;
		if ((NULL != pIntInUse) && (string == pIntInUse->m_sName))
		{
			string += "  (currently active)";
			m_nActiveIntIndex = index;
		}  
		pIntListBox->AddString(string); 
	}	
	
	// make the currently used interaction type selected	
	pIntListBox->SetCurSel(m_nActiveIntIndex);
		
	// This doesn't get called automatically by setting selection here:	
	OnSelchangeIntTypeList();	
}

/////////////////////////////////////////////////////////////////////////////
// CEditIntsDlg message handlers

void CEditIntsDlg::OnEditDetailsBtn()
{
	CListBox* pIntListBox = (CListBox*) GetDlgItem(IDC_INT_TYPE_LIST);
    int curSel = pIntListBox->GetCurSel();

	CInt* pIntInUse = m_pSim->GetIntInUseByNames(m_sObjType1, m_sObjType2);
	
	// Only allow editing of details if active int is selected and it
	// has details to edit	
	if ((curSel == m_nActiveIntIndex) && pIntInUse->m_nHasDetails)	
	{
		pIntInUse->EditDetails();
	}
}

void CEditIntsDlg::OnShowFirstBtn()
{
	// Set the selecte object types to the first combination available,
	// i.e. the zeroth listed for both
	
	CComboBox* pObjType1Box = (CComboBox*) GetDlgItem(IDC_OBJ1_COMBO);
	CComboBox* pObjType2Box = (CComboBox*) GetDlgItem(IDC_OBJ2_COMBO);
	
	pObjType1Box->SetCurSel(0);
	pObjType2Box->SetCurSel(0);

	UpdateIntList();		
}

void CEditIntsDlg::OnShowNextBtn()
{
	// Set the selecte object types to the next combination available,
	// making the id of the second >= to the first
	
	CComboBox* pObjType1Box = (CComboBox*) GetDlgItem(IDC_OBJ1_COMBO);
	CComboBox* pObjType2Box = (CComboBox*) GetDlgItem(IDC_OBJ2_COMBO);

	int numTypes = pObjType1Box->GetCount();

	if ((0 != numTypes) && (CB_ERR != numTypes))	
	{
		int oldSel1 = pObjType1Box->GetCurSel();
		int oldSel2 = pObjType2Box->GetCurSel();   
		
		if ((CB_ERR == oldSel1) || (CB_ERR == oldSel2))  
		{
			// Haven't got an existing selection, so go for first pair
			pObjType1Box->SetCurSel(0);
			pObjType2Box->SetCurSel(0);  
		}     
		else
		{
			// Get to actually go to next in list, we have selections
			int newSel1, newSel2;
			
			if (oldSel1 <= oldSel2)
			{
				newSel2 = oldSel2 + 1;
				newSel1 = oldSel1;
			}
			else
			{
				newSel1 = oldSel2;
				newSel2 = oldSel1 + 1;
			}
			
			// We are now sure that newSel1 < newSel2, but 2 may be off range:
			if (newSel2 > numTypes - 1)
			{
				newSel1++;
				newSel2 = newSel1;
				
				if (newSel1 > numTypes - 1)
				{
					newSel1 = 0;
					newSel2 = 0;
				}
			}

			pObjType1Box->SetCurSel(newSel1);
			pObjType2Box->SetCurSel(newSel2);
		}  
	}
	else
	{
		// empty list or error, leave it alone
	}
	
	UpdateIntList();		
}

void CEditIntsDlg::OnSelchangeObj2Combo()
{
	UpdateIntList();
}

void CEditIntsDlg::OnSelchangeObj1Combo()
{
	UpdateIntList();
}

void CEditIntsDlg::OnMakeSelActive()
{
	// Wish to make selected interaction the new active interaction
	// for this object type pair.

	CListBox* pIntListBox = (CListBox*) GetDlgItem(IDC_INT_TYPE_LIST);
    int curSel = pIntListBox->GetCurSel();
    
    if (LB_ERR != curSel) // no selection if true
    {
		// Only make it current if it is not already
		if (curSel != m_nActiveIntIndex)
		{
			CInt* pEditingInt = ((CInt*) m_intArray[curSel])->Clone();  
			int ret = IDOK;

			pEditingInt->m_pType1 = m_pSim->GetObjTypeByName(m_sObjType1);
			pEditingInt->m_pType2 = m_pSim->GetObjTypeByName(m_sObjType2);
			
			if (pEditingInt->m_nHasDetails)
			{
				ret = pEditingInt->EditDetails();
			}
			
			if (IDOK == ret)  // if user didn't cancel during detail editing
			{   
				m_nActiveIntIndex = curSel;

                if (!pEditingInt->m_nHasDetails)
                {
                    // int will have issued own command to sim if had a dialogue to
                    // edit details (done like that to allow reactive int to also
                    // set up reverse reaction if reqd). Otherwise need to do it here:
                    CString cmd = "interaction " + pEditingInt->m_sName + " ";    
				    pEditingInt->WriteState(cmd);
				    m_pSim->HandleNewCommand(cmd);
                }
			}             
			
			delete pEditingInt;
		}
	}
	
	UpdateIntList();	
}

void CEditIntsDlg::OnDblclkIntTypeList()
{
	// make selected interaction that has been double-clicked on active
	OnMakeSelActive();	
}

void CEditIntsDlg::OnSelchangeIntTypeList()
{
	// Need to update the description box to talk about selected int type
	
	CListBox* pIntListBox = (CListBox*) GetDlgItem(IDC_INT_TYPE_LIST);
	CEdit* pIntDescEdit = (CEdit*) GetDlgItem(IDC_INT_DESC_EDIT);	
    
    int curSel = pIntListBox->GetCurSel();

	CInt* pInt = NULL;    
    
	if (LB_ERR != curSel)
	{
		// fetch description for this sort of interaction
        
        int endList = m_intArray.GetUpperBound();
        CString string;
        
        if ((curSel <= endList) && (endList >= 0))
		{                                    
			pInt = (CInt*) m_intArray[curSel];
			string = pInt->m_sDescription;
		}
		else
		{
			string = "no description";
        }
        
		
		pIntDescEdit->SetWindowText(string);	
	}
	else
	{
		pIntDescEdit->SetWindowText("(none selected)");
	} 	                                          
	
	CButton* pMakeActiveBtn = (CButton*) GetDlgItem(IDC_MAKE_SEL_ACTIVE);
	CButton* pEditDetailsBtn = (CButton*) GetDlgItem(IDC_EDIT_DETAILS_BTN);

	// Dim out make active button if selected interaction is already
	// active.		
	pMakeActiveBtn->EnableWindow(!(curSel == m_nActiveIntIndex)
	                              && !(LB_ERR == curSel));
	
	// Dim out edit details button unless selected interaction is
	// active and it has details to edit
	pEditDetailsBtn->EnableWindow((curSel == m_nActiveIntIndex)
	                              && (NULL != pInt)
	                              && (pInt->m_nHasDetails));	
}

void CEditIntsDlg::OnHelpBtn()
{
#if (_MSC_VER <= 1299) /* MSVC 6 = 1200 */
	AfxGetApp()->WinHelp(ID_OBJECT_INTERACTIONS);
#else
	::HtmlHelp(this->m_hWnd, "Lennard.chm", HH_DISPLAY_TOC, NULL);
#endif
}
