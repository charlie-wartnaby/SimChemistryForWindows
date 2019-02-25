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

// selnumdg.cpp : implementation file
//

#include "stdafx.h"
#include "lennard.h"
#include "simobjs.h"
#include "sim.h"
#include "selnumdg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelNumericDlg dialog


CSelNumericDlg::CSelNumericDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelNumericDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelNumericDlg)
	m_nSelNumeric = -1;
	//}}AFX_DATA_INIT     
	
	m_pListFont = new CFont;      
	m_nNumericType = NUM_USERWRITE | NUM_USERREAD;  // by default get all
}

CSelNumericDlg::~CSelNumericDlg()
{
	delete m_pListFont;
}

void CSelNumericDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelNumericDlg)
	DDX_LBIndex(pDX, IDC_NUM_LIST, m_nSelNumeric);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSelNumericDlg, CDialog)
	//{{AFX_MSG_MAP(CSelNumericDlg)
	ON_LBN_DBLCLK(IDC_NUM_LIST, OnDblclkNumList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSelNumericDlg::OnInitDialog() 
{              
	CDialog::OnInitDialog();

	// Set up tab stops in list box. The co-ordinates are shown in
	// App Studio for the headings above the list-box when they
	// are selected.
	
#define LISTBOX_MARGIN   3  // pixels from edge of dlg box
#define DESCRIPTION	   178
#define UNITS          295
#define INDENT           0
	
	DWORD lDlgBaseUnits = ::GetDialogBaseUnits();     
	WORD  wDlgWidthUnits =  LOWORD(GetDialogBaseUnits());
	m_nListTabstops[0] = ((DESCRIPTION - LISTBOX_MARGIN + INDENT) * wDlgWidthUnits) / 8;
	m_nListTabstops[1] = ((UNITS       - LISTBOX_MARGIN + INDENT) * wDlgWidthUnits) / 8;
    
    CListBox* pListBox = (CListBox*) GetDlgItem(IDC_NUM_LIST);       
    BOOL ret = pListBox->SetTabStops(sizeof(m_nListTabstops)/sizeof(m_nListTabstops[0]),
                                     m_nListTabstops);
    ASSERT(ret);

	
	// set narrow font for wordy names and descriptions
	
	m_pListFont->CreateFont(12, // int nHeight,
	                        0, // int nWidth,
	                        0, // int nEscapement, 
                            0, // int nOrientation, 
                            FW_DONTCARE, // int nWeight, 
                            FALSE, // BYTE bItalic, 
                            FALSE, // BYTE bUnderline, 
                            0, // BYTE cStrikeOut,
                            ANSI_CHARSET, // BYTE nCharSet, 
                            OUT_DEFAULT_PRECIS, // BYTE nOutPrecision, 
                            CLIP_DEFAULT_PRECIS, // BYTE nClipPrecision, 
                            DEFAULT_QUALITY, // BYTE nQuality, 
                            DEFAULT_PITCH, // BYTE nPitchAndFamily,  
                            "MS Sans Serif"); // LPCSTR lpszFacename
	
	pListBox->SetFont(m_pListFont);
	
	
	// fill list box with names of numerics

	CNumeric* pNumeric;
	int nSelnDone = FALSE;
	
	for (int i = 0; i <= m_pSim->m_numList.GetUpperBound(); i++)
	{
		pNumeric = (CNumeric*) m_pSim->m_numList[i];  
		
		if (m_nNumericType & pNumeric->m_nType)
		{
			// Add only CNumerics that are of correct type (eg writeable)
			
			pListBox->AddString(pNumeric->m_sName + "\t" + 
		                    pNumeric->m_sDescription + "\t" +
		                    pNumeric->m_sUnits);

			if (pNumeric->m_sName == m_sInitSeln)
			{
				// The one we just added is the one we should select to start
				// with
				
				pListBox->SetCurSel(pListBox->GetCount() - 1); 
				nSelnDone = TRUE;
			}
		}
		
	}                        
	
	if (!nSelnDone)
	{
		pListBox->SetCurSel(0);
	}
		
   	return TRUE; // leaves it to Windows to highlight first control
}

void CSelNumericDlg::OnOK()
{    
	CDialog::OnOK();  // do this first to do data exchange to get
	                  // index of list-box selection
	                  
	// Get selection (if any) copied to string, for caller to retrieve 
	// only want name up until first tab, ie have to change something
	// like "sim:temperature\tuser-controlled sys temp\tK" to just
	// "sim:temperature".

	CListBox* pListBox = (CListBox*) GetDlgItem(IDC_NUM_LIST);       	
	
	if (m_nSelNumeric >= 0)
	{
		pListBox->GetText(m_nSelNumeric, m_sNumeric);  
		int nTabPos = m_sNumeric.Find('\t');
		ASSERT(-1 != nTabPos);
		m_sNumeric = m_sNumeric.Left(nTabPos);
	}
	else
	{
		m_sNumeric = "";
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSelNumericDlg message handlers

void CSelNumericDlg::OnDblclkNumList()
{
	// double-clicking a list item should select
	// it and close the dialogue as if OK pressed
	
	OnOK();
}
