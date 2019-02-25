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

// ReactiveIntPropsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "simobjs.h"
#include "sim.h"
#include "baseint.h"
#include "lennard.h"
#include "mol.h"
#include "ReactiveIntPropsDlg.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReactiveIntPropsDlg dialog


CReactiveIntPropsDlg::CReactiveIntPropsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReactiveIntPropsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReactiveIntPropsDlg)
	m_dBarrier = 0.0;
	m_nDoReverse = FALSE;
	m_nTwoProducts = FALSE;
	m_sProduct1 = _T("");
	m_sProduct2 = _T("");
	//}}AFX_DATA_INIT

    m_pSim = NULL;
}


void CReactiveIntPropsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReactiveIntPropsDlg)
	DDX_Text(pDX, IDC_ACTIVATION_ENERGY, m_dBarrier);
	DDV_MinMaxDouble(pDX, m_dBarrier, LIM_MIN_BARRIER, LIM_MAX_BARRIER);
	DDX_Check(pDX, IDC_CHECK_DO_REVERSE, m_nDoReverse);
	DDX_Check(pDX, IDC_CHECK_TWO_PRODS, m_nTwoProducts);
	DDX_CBString(pDX, IDC_COMBO_PROD1, m_sProduct1);
	DDV_MaxChars(pDX, m_sProduct1, LIM_MAX_CHARS_OBJTYPE);
	DDX_CBString(pDX, IDC_COMBO_PROD2, m_sProduct2);
    DDV_MaxChars(pDX, m_sProduct2, LIM_MAX_CHARS_OBJTYPE);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReactiveIntPropsDlg, CDialog)
	//{{AFX_MSG_MAP(CReactiveIntPropsDlg)
	ON_BN_CLICKED(IDC_CHECK_TWO_PRODS, OnCheckTwoProds)
	ON_BN_CLICKED(IDC_HELP_BTN, OnHelpBtn)
	ON_EN_KILLFOCUS(IDC_ACTIVATION_ENERGY, OnKillfocusActivationEnergy)
	ON_CBN_SELCHANGE(IDC_COMBO_PROD1, OnSelchangeComboProd1)
	ON_CBN_SELCHANGE(IDC_COMBO_PROD2, OnSelchangeComboProd2)
	ON_BN_CLICKED(ID_MY_OK, OnMyOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CReactiveIntPropsDlg::OnInitDialog()
{
    CDialog::OnInitDialog(); // recommended in MSVC help

	// Here the dropdown list boxes are initialised with the names
	// of the molecule types we know about
	
	CComboBox* pObjType1Box = (CComboBox*) GetDlgItem(IDC_COMBO_PROD1);
	CComboBox* pObjType2Box = (CComboBox*) GetDlgItem(IDC_COMBO_PROD2);
	
	pObjType1Box->ResetContent();
	pObjType2Box->ResetContent();

    int nFoundProd1 = 0;
    int nFoundProd2 = 0;
    
    if (NULL == m_pSim)
    {
        ASSERT(FALSE);
        return TRUE;
    }

    int index;
	for (index = 0; index <= m_pSim->m_typeArray.GetUpperBound(); index++)
	{
		CObjType* pType = (CObjType*) m_pSim->m_typeArray[index];
		
		if (CI_MOL == pType->m_nBaseType)
		{
			pObjType1Box->AddString(pType->m_sName);
			pObjType2Box->AddString(pType->m_sName);

            if (pType->m_sName == m_sProduct1) nFoundProd1 = index;
            if (pType->m_sName == m_sProduct2) nFoundProd2 = index;
		}
	}
   
	pObjType1Box->SetCurSel(nFoundProd1);
	pObjType2Box->SetCurSel(nFoundProd2);

    if (!m_nTwoProducts || m_sProduct2 == "")
    {
        m_nTwoProducts = FALSE;
        m_nDoReverse = FALSE;
    }
    else
    {
        m_nTwoProducts = TRUE;
        m_nDoReverse = TRUE; // arbitrarily
    }

    UpdateData(FALSE);

    UpdateControls();

	return TRUE; // leaves it to Windows to highlight first control
}

void CReactiveIntPropsDlg::UpdateControls()
{
    UpdateData(TRUE); // retrieve state of variables

    // If there are two products, we want to enable the second
    // drop-down box etc
	
    GetDlgItem(IDC_COMBO_PROD2)->EnableWindow(m_nTwoProducts);
    GetDlgItem(IDC_CHECK_DO_REVERSE)->EnableWindow(m_nTwoProducts);
    GetDlgItem(IDC_SEL_PROD2_TEXT)->EnableWindow(m_nTwoProducts);

    CString sEqn;

    sprintf(buff, "Equation: %s + %s --> %s",
                 (const char*) m_pInt->m_pType1->m_sName,
                 (const char*) m_pInt->m_pType2->m_sName,
                 m_sProduct1);
    sEqn = buff;

    if (m_nTwoProducts)
    {
        sEqn += " + ";
        sEqn += m_sProduct2;
    }

    GetDlgItem(IDC_EQN_TEXT)->SetWindowText(sEqn);

    double dEnergyChange;
    double dMassChange;
    double dProdRadius1 = -1, dProdRadius2 = -1;

    CObjTypeMol* pProd1Type = (CObjTypeMol*) m_pSim->GetObjTypeByName(m_sProduct1);

    if (NULL == pProd1Type)
    {
        ASSERT(FALSE);
    }
    else
    {
        ASSERT(pProd1Type->m_nBaseType == CI_MOL);

        CObjTypeMol* pProd2Type;

        if (m_nTwoProducts)
        {
            pProd2Type = (CObjTypeMol*) m_pSim->GetObjTypeByName(m_sProduct2);

            ASSERT(pProd2Type != FALSE);
            ASSERT(pProd2Type->m_nBaseType == CI_MOL);
        }

        dEnergyChange = pProd1Type->m_dFormationEnergy;
        dMassChange = pProd1Type->m_dMass;
        dProdRadius1 = pProd1Type->m_dHSRadius;

        if (m_nTwoProducts && (NULL != pProd2Type))
        {
            dEnergyChange += pProd2Type->m_dFormationEnergy;
            dMassChange += pProd2Type->m_dMass;
            dProdRadius2 = pProd2Type->m_dHSRadius;
        }
    }

    dEnergyChange -= ((CObjTypeMol*) m_pInt->m_pType1)->m_dFormationEnergy;
    dEnergyChange -= ((CObjTypeMol*) m_pInt->m_pType2)->m_dFormationEnergy;

    dMassChange -= ((CObjTypeMol*) m_pInt->m_pType1)->m_dMass;
    dMassChange -= ((CObjTypeMol*) m_pInt->m_pType2)->m_dMass;

    sprintf(buff, "Overall energy change in reaction = %g kJ/mol",
                  dEnergyChange);
    
    CString sEnergyChange = buff;

    if (dEnergyChange > NEARLY_ZERO)
    {
        sEnergyChange += " (i.e. endothermic)";
    }
    else if (dEnergyChange < -NEARLY_ZERO)
    {
        sEnergyChange += " (i.e. exothermic)";
    }
    else 
    {
        sEnergyChange += " (i.e. no change)";
    }

    GetDlgItem(IDC_ENERGY_CHANGE_TEXT)->SetWindowText(sEnergyChange);

    double dTotalBarrier = m_dBarrier;
    
    if (dEnergyChange > 0)
    {
        dTotalBarrier += dEnergyChange;
    }

    sprintf(buff, "Total energy barrier from reactants to products = %g kJ/mol",
            dTotalBarrier);

    CString sTotBarrier(buff);

    GetDlgItem(IDC_ENERGY_BARRIER_TEXT)->SetWindowText(sTotBarrier);

    CString sNotes;

    if (fabs(dMassChange) > NEARLY_ZERO)
    {
        sNotes += "Warning: mass is not conserved by this reaction. ";
    }

    int nOneProdFitsReactant1 = !m_nTwoProducts && dProdRadius1 <= ((CObjTypeMol*) m_pInt->m_pType1)->m_dHSRadius;
    int nOneProdFitsReactant2 = !m_nTwoProducts && dProdRadius1 <= ((CObjTypeMol*) m_pInt->m_pType2)->m_dHSRadius;
    
    int nTwoProdsFitWay1 = FALSE;
    int nTwoProdsFitWay2 = FALSE;

    if (m_nTwoProducts)
    {   
        nTwoProdsFitWay1 = (dProdRadius1 <= ((CObjTypeMol*) m_pInt->m_pType1)->m_dHSRadius
                          && dProdRadius2 <= ((CObjTypeMol*) m_pInt->m_pType2)->m_dHSRadius);
        nTwoProdsFitWay2 = (dProdRadius1 <= ((CObjTypeMol*) m_pInt->m_pType2)->m_dHSRadius
                          && dProdRadius2 <= ((CObjTypeMol*) m_pInt->m_pType1)->m_dHSRadius);
    }

    if (nOneProdFitsReactant1 || nOneProdFitsReactant2
        || nTwoProdsFitWay1 || nTwoProdsFitWay2)
    {
        // products can fit into space left by reactants
    }
    else
    {
        sNotes += "Warning: products cannot fit into space left by reactants, "
                  "and may be created overlapping each other or other objects.";
    }
    
    GetDlgItem(IDC_NOTES_TEXT)->SetWindowText(sNotes);
}

/////////////////////////////////////////////////////////////////////////////
// CReactiveIntPropsDlg message handlers

void CReactiveIntPropsDlg::OnCheckTwoProds() 
{
    UpdateControls();
}

void CReactiveIntPropsDlg::OnHelpBtn() 
{
#if (_MSC_VER <= 1299) /* MSVC 6 = 1200 */
	AfxGetApp()->WinHelp(0x20000 + IDD_REACTIVE_INT_PROPS);
#else
	::HtmlHelp(this->m_hWnd, "Lennard.chm", HH_DISPLAY_TOC, NULL);
#endif

}

void CReactiveIntPropsDlg::OnKillfocusActivationEnergy() 
{
	UpdateControls();
}

void CReactiveIntPropsDlg::OnSelchangeComboProd1() 
{
	UpdateControls();
}

void CReactiveIntPropsDlg::OnSelchangeComboProd2() 
{
	UpdateControls();
}

void CReactiveIntPropsDlg::OnOK() 
{
    // do nothing if user presses return after entering barrier
    // value etc
}

void CReactiveIntPropsDlg::OnMyOk() 
{
    // overridden so doesn't happen when press return
    CDialog::OnOK();
}
