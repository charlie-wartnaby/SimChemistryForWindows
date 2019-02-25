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

// drwgphdg.cpp : implementation file
//

#include "stdafx.h"
#include "lennard.h"
#include "simobjs.h"
#include "sim.h"
#include "drwgphdg.h"
#include "selnumdg.h"   
#include "graphx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawGraphDlg dialog


CDrawGraphDlg::CDrawGraphDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDrawGraphDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDrawGraphDlg)
	m_sGraphTitle = "Temperature versus Time";
	m_sXLabel = "Time (ns)";
	m_sYLabel = "System Temperature (K)";
	m_nCheckTitle = TRUE;
	m_nCheckXLabel = TRUE;
	m_nCheckYLabel = TRUE;
	m_dXLeft = 0;
	m_dXRight = 0;
	m_dYBottom = 0;
	m_dYTop = 0;
	m_nFilled = TRUE;
	m_nStyle = 0;
	m_nSize = 10;
	//}}AFX_DATA_INIT
	
	m_sX = "sim:time";
	m_sY = "sim:temperature";
	m_cPointColour = RGB(255, 0, 0);
}

void CDrawGraphDlg::DoDataExchange(CDataExchange* pDX)
{  
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDrawGraphDlg)
	DDX_Text(pDX, IDC_EDIT_GRAPH_TITLE, m_sGraphTitle);
	DDV_MaxChars(pDX, m_sGraphTitle, LIM_MAX_GR_TITLE_CHARS);
	DDX_Text(pDX, IDC_EDIT_X_LABEL, m_sXLabel);
	DDV_MaxChars(pDX, m_sXLabel, LIM_MAX_GR_LABEL_CHARS);
	DDX_Text(pDX, IDC_EDIT_Y_LABEL, m_sYLabel);
	DDV_MaxChars(pDX, m_sYLabel, LIM_MAX_GR_LABEL_CHARS);
	DDX_Check(pDX, IDC_CHECK_TITLE, m_nCheckTitle);
	DDX_Check(pDX, IDC_CHECK_XLABEL, m_nCheckXLabel);
	DDX_Check(pDX, IDC_CHECK_YLABEL, m_nCheckYLabel);
	DDX_Text(pDX, IDC_EDIT_XLEFT, m_dXLeft);
	DDX_Text(pDX, IDC_EDIT_XRIGHT, m_dXRight);
	DDV_MinMaxDouble(pDX, m_dXRight, LIM_MIN_X, LIM_MAX_X);
	DDX_Text(pDX, IDC_EDIT_YBOTTOM, m_dYBottom);
	DDX_Text(pDX, IDC_EDIT_YTOP, m_dYTop);
	DDV_MinMaxDouble(pDX, m_dYTop, LIM_MIN_Y, LIM_MAX_Y);
	DDX_Check(pDX, IDC_CHECK_FILLED, m_nFilled);
	DDX_CBIndex(pDX, IDC_COMBO_STYLE, m_nStyle);
	DDX_Text(pDX, IDC_EDIT_SIZE, m_nSize);
	DDV_MinMaxInt(pDX, m_nSize, LIM_MIN_GR_SYMB_SIZE, LIM_MAX_GR_SYMB_SIZE);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDrawGraphDlg, CDialog)
	//{{AFX_MSG_MAP(CDrawGraphDlg)
	ON_BN_CLICKED(IDC_BUTTON_SEL_X, OnButtonSelX)
	ON_BN_CLICKED(IDC_BUTTON_SEL_Y, OnButtonSelY)
	ON_BN_CLICKED(IDC_CHECK_TITLE, OnCheckTitle)
	ON_BN_CLICKED(IDC_CHECK_XLABEL, OnCheckXlabel)
	ON_BN_CLICKED(IDC_CHECK_YLABEL, OnCheckYlabel)
	ON_BN_CLICKED(IDC_BUTTON_COLOUR, OnButtonColour)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_COMBO_SIZE, OnSelchangeComboSize)
	ON_CBN_SELCHANGE(IDC_COMBO_STYLE, OnSelchangeComboStyle)
	ON_BN_CLICKED(IDC_CHECK_FILLED, OnCheckFilled)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_PREVIEW, OnButtonUpdatePreview)
	ON_EN_KILLFOCUS(IDC_EDIT_SIZE, OnKillfocusEditSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDrawGraphDlg::OnInitDialog()
{   
	CDialog::OnInitDialog(); 
	
	UpdateFields(FALSE);
	
	return TRUE;  // let Windows highlight first control
}

void CDrawGraphDlg::UpdateFields(int nDoUpdateData)
{     
	// Enable those edit fields that should be, depending
	// on what options selected, etc
	
	if (nDoUpdateData)
	{   
		// don't do this if called from InitDialog
		UpdateData(TRUE);   // remember all user settings in variables
	}
	
	GetDlgItem(IDC_EDIT_GRAPH_TITLE)->EnableWindow(m_nCheckTitle);

	GetDlgItem(IDC_EDIT_X_LABEL)->EnableWindow(m_nCheckXLabel);

	GetDlgItem(IDC_EDIT_Y_LABEL)->EnableWindow(m_nCheckYLabel);
    
    if (m_sX != "")
    {
    	GetDlgItem(IDC_X_DATA_TEXT)->SetWindowText(m_sX);
    }
    
    if (m_sY != "")
    {
    	GetDlgItem(IDC_Y_DATA_TEXT)->SetWindowText(m_sY);
    }
    
    if ((m_sX != "") && (m_sY != ""))
    {
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
	else                                     
	{                                        
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDrawGraphDlg message handlers

void CDrawGraphDlg::OnPaint()
{    
	// this paints the preview graph symbol
	
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
	
	CSObjGraph::DrawPoint(pControlDC,
	                      (clientRect.left + clientRect.right) / 2,
	                      (clientRect.top + clientRect.bottom) / 2,
	                      m_nStyle,
	                      m_nSize,
	                      m_nFilled,
	                      m_cPointColour);
		
	pWnd->ReleaseDC(pControlDC);	
}

void CDrawGraphDlg::OnButtonSelX()
{        
	OnButtonSelXOrY(m_sX, m_sXLabel);
}

void CDrawGraphDlg::OnButtonSelY()
{                                 
	OnButtonSelXOrY(m_sY, m_sYLabel);
}

void CDrawGraphDlg::OnButtonSelXOrY(CString& sNumeric, CString& sLabel)
{
	CSelNumericDlg dlg; 
	CNumeric* pNum;    
	CString sUnitsAppendage;
	
	dlg.m_pSim = m_pSim;
	dlg.m_nNumericType = NUM_USERREAD;

	if (IDOK == dlg.DoModal())
	{
		// note may have clicked OK with no selection, so m_sNumeric may be null string;
		// check valid and only copy to X data if a pukka CNumeric
		
		pNum = m_pSim->FetchNumeric(dlg.m_sNumeric);
		if (NULL != pNum)
		{
			sNumeric = dlg.m_sNumeric; 
			sLabel = pNum->m_sDescription;
			if (pNum->m_sUnits != "")
			{
				sUnitsAppendage = " (" + pNum->m_sUnits + ")";
			}        
			else
			{
				sUnitsAppendage = "";
			}
		}
		
		int nCharsLeft = LIM_MAX_GR_LABEL_CHARS - sUnitsAppendage.GetLength();
		
		if (sLabel.GetLength() > nCharsLeft)
		{      
			sLabel = sLabel.Left(nCharsLeft);
		}          
		
		sLabel += sUnitsAppendage;                           

		UpdateData(FALSE); // copy new label to control text
		UpdateFields(FALSE);  // enable/disable fields
	}
	else
	{
		// user cancelled 
	}
}

void CDrawGraphDlg::OnCheckTitle()
{
	UpdateFields(TRUE);
}

void CDrawGraphDlg::OnCheckXlabel()
{
	UpdateFields(TRUE);
}

void CDrawGraphDlg::OnCheckYlabel()
{
	UpdateFields(TRUE);
}

void CDrawGraphDlg::OnButtonColour()
{
	CColorDialog colDlg(m_cPointColour);     

	if (IDOK == colDlg.DoModal())
	{
		m_cPointColour = colDlg.GetColor();
	}               
	
	UpdatePreview();
}

void CDrawGraphDlg::UpdatePreview()
{
	UpdateData(TRUE);  // does data exchange
	Invalidate();  // causes redraw
}


void CDrawGraphDlg::OnSelchangeComboSize()
{
	UpdatePreview();
}

void CDrawGraphDlg::OnSelchangeComboStyle()
{
	UpdatePreview();
}

void CDrawGraphDlg::OnCheckFilled()
{
	UpdatePreview();
}

void CDrawGraphDlg::OnButtonUpdatePreview()
{
	UpdatePreview();
}

void CDrawGraphDlg::OnKillfocusEditSize()
{
	UpdatePreview();
}
