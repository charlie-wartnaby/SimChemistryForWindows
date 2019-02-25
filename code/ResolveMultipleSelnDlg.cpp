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

// ResolveMultipleSelnDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lennard.h"
#include "simobjs.h"
#include "sim.h"
#include "ResolveMultipleSelnDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResolveMultipleSelnDlg dialog


CResolveMultipleSelnDlg::CResolveMultipleSelnDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CResolveMultipleSelnDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResolveMultipleSelnDlg)
	m_bAlwaysShow = FALSE;
	//}}AFX_DATA_INIT
}


void CResolveMultipleSelnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResolveMultipleSelnDlg)
	DDX_Check(pDX, IDC_ALWAYS_FOR_SINGLE_CLICK, m_bAlwaysShow);
	//}}AFX_DATA_MAP
}

BOOL CResolveMultipleSelnDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_bAlwaysShow = theApp.m_nAutoShowSelnResolutionDlg;
    UpdateData(FALSE);

    CListBox* pObjList = (CListBox*) GetDlgItem(IDC_OBJECT_LIST);

    if (NULL == pObjList)
    {
        ASSERT(FALSE);
    }
    else
    {
        m_nNumObjs = 0;

        CSObj* pObj = NULL;
	    POSITION pos;
        CString sObjIdentifier;
		    
	    pos = m_pSim->m_selectedObjects.GetHeadPosition();
				    
	    while (NULL != pos)
	    {   
		    pObj = (CSObj*) m_pSim->m_selectedObjects.GetAt(pos);
		    
            sObjIdentifier = "\"" + pObj->m_pType->m_sName;
            sObjIdentifier += "\" ";
            sprintf(buff, "%ld", pObj->m_lId);
            sObjIdentifier += buff;

            pObjList->AddString(sObjIdentifier);

            m_nNumObjs++;
			    
		    m_pSim->m_selectedObjects.GetNext(pos);
	    }

        pObjList->SetSel(-1, TRUE); // set all selected at once

        ASSERT(m_nNumObjs == m_pSim->m_selectedObjects.GetCount());

        m_wasSelectedArray = new int[m_nNumObjs];

        for (int i = 0; i < m_nNumObjs; i++)
        {
            m_wasSelectedArray[i] = TRUE;
        }
    
        m_nNumWereSelected = m_nNumObjs;
    }
    
    return TRUE;
}

CResolveMultipleSelnDlg::~CResolveMultipleSelnDlg()
{
    delete [] m_wasSelectedArray;
}

BEGIN_MESSAGE_MAP(CResolveMultipleSelnDlg, CDialog)
	//{{AFX_MSG_MAP(CResolveMultipleSelnDlg)
	ON_LBN_DBLCLK(IDC_OBJECT_LIST, OnDblclkObjectList)
	ON_LBN_SELCHANGE(IDC_OBJECT_LIST, OnSelchangeObjectList)
	ON_BN_CLICKED(IDC_UNSELECT_ALL, OnUnselectAll)
	ON_BN_CLICKED(IDC_ALWAYS_FOR_SINGLE_CLICK, OnAlwaysForSingleClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// If the selection changes to a single selection, probably most efficient
// to do a 'seln_reset' and then just select the required object.
// If ctrl- or shift-click used to modify selection list one or a few
// at a time, better to just select or unselect those individual objects.
// If double-click, want just that object selected and the dialogue box
// closed. (Double-click results in single selection.)
void CResolveMultipleSelnDlg::ApplySelnChanges()
{
    CListBox* pObjList = (CListBox*) GetDlgItem(IDC_OBJECT_LIST);

    if (NULL == pObjList)
    {
        ASSERT(FALSE);
        return;
    }

    int nNumNowSelected = pObjList->GetSelCount();
    int i;
    CString cmd;
    int oneSeln[1];

    if (m_nNumWereSelected > 1 && 1 == nNumNowSelected)
    {
        // clear all selections and then select just the one now added
        m_pSim->HandleNewCommand("seln_reset");
        pObjList->GetSelItems(1, oneSeln); 
        pObjList->GetText(oneSeln[0], buff);
        cmd = "select ";
        cmd += buff;
        m_pSim->HandleNewCommand(cmd);
    }
    else if (0 == nNumNowSelected)
    {
        // clear all selections
        m_pSim->HandleNewCommand("seln_reset");
    }
    else
    {
        // scan for changes and just do those
        for (i = 0; i < m_nNumObjs; i++)
        {
            int nNowSelected = pObjList->GetSel(i);
            if ((m_wasSelectedArray[i] && !nNowSelected)
                || (!m_wasSelectedArray[i] && nNowSelected))
            {
                // seln state of this obj toggled
                pObjList->GetText(i, buff);
                cmd = "select ";
                cmd += buff;
                m_pSim->HandleNewCommand(cmd);
            }
        }
    }

    for (i = 0; i < m_nNumObjs; i++)
    {
        m_wasSelectedArray[i] = pObjList->GetSel(i);
    }

    m_nNumWereSelected = nNumNowSelected;
}

/////////////////////////////////////////////////////////////////////////////
// CResolveMultipleSelnDlg message handlers



void CResolveMultipleSelnDlg::OnDblclkObjectList() 
{
	ApplySelnChanges();
	OnOK();
}

void CResolveMultipleSelnDlg::OnSelchangeObjectList() 
{
	ApplySelnChanges();
}

void CResolveMultipleSelnDlg::OnUnselectAll() 
{
    CListBox* pObjList = (CListBox*) GetDlgItem(IDC_OBJECT_LIST);

    pObjList->SetSel(-1, FALSE); // unselect all

    ApplySelnChanges();
}

void CResolveMultipleSelnDlg::OnAlwaysForSingleClick() 
{
    UpdateData(TRUE);
	theApp.m_nAutoShowSelnResolutionDlg = m_bAlwaysShow;
}
