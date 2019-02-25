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

// monprpdg.cpp : implementation file
//

#include "stdafx.h"
#include "lennard.h"
#include "monprpdg.h" 
#include "simobjs.h"
#include "monitor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditMonitorPropsDlg dialog


CEditMonitorPropsDlg::CEditMonitorPropsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditMonitorPropsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditMonitorPropsDlg)
	m_nControlType = 0;
	m_sName = "";
	m_nHatch = FALSE;
	m_nLabel = TRUE;
	m_nLineThick = 2;
	//}}AFX_DATA_INIT
	
	m_cColour = RGB(255, 0, 0); // red
}

void CEditMonitorPropsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditMonitorPropsDlg)
	DDX_Radio(pDX, IDC_RADIO_MOL_CONTROLS, m_nControlType);
	DDX_Text(pDX, IDC_EDIT_NAME, m_sName);
	DDV_MaxChars(pDX, m_sName, LIM_MAX_CHARS_OBJTYPE);
	DDX_Check(pDX, IDC_CHECK_HATCH, m_nHatch);
	DDX_Check(pDX, IDC_CHECK_LABEL, m_nLabel);
	DDX_Text(pDX, IDC_EDIT_THICKNESS, m_nLineThick);
	DDV_MinMaxInt(pDX, m_nLineThick, LIM_MIN_MONITOR_THICK, LIM_MAX_MONITOR_THICK);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditMonitorPropsDlg, CDialog)
	//{{AFX_MSG_MAP(CEditMonitorPropsDlg)
	ON_BN_CLICKED(IDC_CHECK_HATCH, OnCheckHatch)
	ON_BN_CLICKED(IDC_CHECK_LABEL, OnCheckLabel)
	ON_BN_CLICKED(IDC_BUTTON_COLOUR, OnButtonColour)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_PREVIEW, OnButtonPreview)
	ON_BN_CLICKED(IDC_HELP_BTN, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CEditMonitorPropsDlg::UpdatePreview()
{
	UpdateData(TRUE);  // does data exchange
	Invalidate();      // causes redraw
}

/////////////////////////////////////////////////////////////////////////////
// CEditMonitorPropsDlg message handlers

void CEditMonitorPropsDlg::OnCheckHatch()
{
	UpdatePreview();
}

void CEditMonitorPropsDlg::OnCheckLabel()
{
	UpdatePreview();
}

void CEditMonitorPropsDlg::OnButtonColour()
{
	CColorDialog colDlg(m_cColour);     

	if (IDOK == colDlg.DoModal())
	{
		m_cColour = colDlg.GetColor();
	}               
	
	UpdatePreview();
}

void CEditMonitorPropsDlg::OnPaint()
{
	CWnd* pWnd = GetDlgItem(IDC_EDIT_PREVIEW);
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

	pControlDC->DPtoLP(&clientRect);            
	
	CBrush* pOutlineBrush = CObjTypeMonitor::CreateOutlineBrush(pControlDC, m_cColour, 0);
	CFont* pLabelFont = CObjTypeMonitor::CreateLabelFont();
	
	CString sLabel = m_sName + ":1";
	
	CObjTypeMonitor::DrawMonitor(pControlDC,
	                             clientRect.left + 15,
	                             clientRect.top - 15,
	                             clientRect.right - 15,
	                             clientRect.bottom + 15, 
	                             m_nLineThick,
	                             m_nHatch,
	                             m_nLabel,
	                             sLabel,
	                             pOutlineBrush,
	                             pLabelFont,
	                             m_cColour);    
	                             
	delete pOutlineBrush;	       
	delete pLabelFont;                      
		
	pWnd->ReleaseDC(pControlDC);	
}

void CEditMonitorPropsDlg::OnButtonPreview()
{
	UpdatePreview();
}

void CEditMonitorPropsDlg::OnHelp()
{
#if (_MSC_VER <= 1299) /* MSVC 6 = 1200 */
	AfxGetApp()->WinHelp(IDD_EDIT_MONITOR_PROPS);
#else
	::HtmlHelp(this->m_hWnd, "Lennard.chm", HH_DISPLAY_TOC, NULL);
#endif

}
