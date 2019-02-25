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

// edtxtdlg.cpp : implementation file
//

#include "stdafx.h"
#include "lennard.h"
#include "simobjs.h"   
#include "sim.h"
#include "textbox.h"
#include "edtxtdlg.h"                          
#include "selnumdg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditTextboxDlg dialog


CEditTextboxDlg::CEditTextboxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditTextboxDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditTextboxDlg)
	//}}AFX_DATA_INIT
}

void CEditTextboxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditTextboxDlg)
	DDX_Text(pDX, IDC_EDIT_TEXT_CTRL, m_sUserTypedText);
	DDV_MaxChars(pDX, m_sUserTypedText, LIM_MAX_CHARS_TEXTBOX);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditTextboxDlg, CDialog)
	//{{AFX_MSG_MAP(CEditTextboxDlg)
	ON_BN_CLICKED(IDC_INSERT_NUM_BTN, OnInsertNumBtn)
	ON_EN_CHANGE(IDC_EDIT_TEXT_CTRL, OnChangeEditTextCtrl)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_TYPE_PROPS_BTN, OnTypePropsBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CEditTextboxDlg::UpdatePreview()
{
	UpdateData(TRUE);  // does data exchange
	Invalidate();  // causes redraw
}

/////////////////////////////////////////////////////////////////////////////
// CEditTextboxDlg message handlers

void CEditTextboxDlg::OnInsertNumBtn()
{
	CSelNumericDlg dlg;
	
	dlg.m_pSim = m_pSim;
	dlg.m_nNumericType = NUM_USERREAD;
	
	if (IDOK == dlg.DoModal())
	{
		// note may have clicked OK with no selection, so m_sNumeric may be null string
		// in which case won't be found by sim
		
		if (NULL != m_pSim->FetchNumeric(dlg.m_sNumeric))
		{
			CString numString = "$" + dlg.m_sNumeric + "$";
			CEdit* pEdit = (CEdit*) GetDlgItem(IDC_EDIT_TEXT_CTRL);
			pEdit->ReplaceSel(numString);
		}
	}
	else
	{
		// user cancelled 
	}
}

void CEditTextboxDlg::OnChangeEditTextCtrl()
{
	UpdatePreview();
}

void CEditTextboxDlg::OnPaint()
{
	CWnd* pWnd = GetDlgItem(IDC_PREVIEW_CTRL);
	CPaintDC dc(this); // device context for painting  
	CDC* pControlDC = pWnd->GetDC();
	pWnd->Invalidate();
	pWnd->UpdateWindow();
	pControlDC->SetMapMode(MM_LOENGLISH); 
	
	CRect clientRect;
	pWnd->GetClientRect(&clientRect);   
	clientRect.InflateRect(-1, -1);  // to leave line border intact
	CRgn rgn;
	rgn.CreateRectRgnIndirect(&clientRect);
	pControlDC->SelectClipRgn(&rgn);  
    
	m_pTextboxType->DrawTextFromString(pControlDC, m_sUserTypedText, 1, -1);

    
	pWnd->ReleaseDC(pControlDC);	
}

void CEditTextboxDlg::OnTypePropsBtn()
{
	m_pTextboxType->PropertiesCommand();
}
