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

// walprpdg.cpp : implementation file
//

#include "stdafx.h" 
#include "simobjs.h"
#include "wall.h"
#include "lennard.h"
#include "walprpdg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditWallPropsDlg dialog


CEditWallPropsDlg::CEditWallPropsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditWallPropsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditWallPropsDlg)
	m_nLineThick = 0;
	m_dSpecialTemperature = 100;
	m_sName = "";
	m_nTemperatureType = 0;
	m_nMolIntType = -1;
	//}}AFX_DATA_INIT
}

void CEditWallPropsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditWallPropsDlg)
	DDX_Text(pDX, IDC_LINE_THICK_EDIT, m_nLineThick);
	DDV_MinMaxInt(pDX, m_nLineThick, LIM_MIN_WALL_THICK, LIM_MAX_WALL_THICK);
	DDX_Text(pDX, IDC_SPECIAL_TEMP_EDIT, m_dSpecialTemperature);
	DDV_MinMaxDouble(pDX, m_dSpecialTemperature, LIM_MIN_TEMP, LIM_MAX_TEMP);
	DDX_Text(pDX, IDC_TYPE_NAME, m_sName);
	DDV_MaxChars(pDX, m_sName, LIM_MAX_CHARS_OBJTYPE);
	DDX_Radio(pDX, IDC_SYS_TEMPERATURE, m_nTemperatureType);
	DDX_Radio(pDX, IDC_THERMAL, m_nMolIntType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditWallPropsDlg, CDialog)
	//{{AFX_MSG_MAP(CEditWallPropsDlg)
	ON_BN_CLICKED(IDC_UPDATE_PREVIEW, OnUpdatePreview)
	ON_BN_CLICKED(IDC_LINE_COLOUR_BTN, OnLineColourBtn)
	ON_WM_PAINT()
	ON_EN_SETFOCUS(IDC_SPECIAL_TEMP_EDIT, OnSetfocusSpecialTempEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEditWallPropsDlg message handlers

void CEditWallPropsDlg::OnUpdatePreview()
{
	UpdatePreview();
}

void CEditWallPropsDlg::OnLineColourBtn()
{
	CColorDialog colDlg(m_cLineColour);     

	if (IDOK == colDlg.DoModal())
	{
		m_cLineColour = colDlg.GetColor();
	}               
	
	UpdatePreview();
}

void CEditWallPropsDlg::UpdatePreview()
{
	UpdateData(TRUE);  // does data exchange
	Invalidate();      // causes redraw
}

void CEditWallPropsDlg::OnPaint()
{
	CWnd* pWnd = GetDlgItem(IDC_PREVIEW_EDITCTRL);
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
    
	CPen pen;  
	BOOL success = pen.CreatePen(PS_SOLID, m_nLineThick,
	                      m_cLineColour);        
    
    if (success)
    {
	    CPen* pOldPen = pControlDC->SelectObject(&pen);
		
		pControlDC->DPtoLP(&clientRect);

		pControlDC->MoveTo(clientRect.left, clientRect.top);
		pControlDC->LineTo(clientRect.right, clientRect.bottom);		
		
	    pControlDC->SelectObject(pOldPen);
	}
	else
	{
		pWnd->SetWindowText("Graphics exhausted");
	}
	
	pWnd->ReleaseDC(pControlDC);	
}

void CEditWallPropsDlg::OnSetfocusSpecialTempEdit()
{
	// If user is putting in a special temperature, also set corresponding
	// radio button
	UpdateData(TRUE); // remember user settings
	m_nTemperatureType = WT_USE_SPECIFIC_TEMP;
	UpdateData(FALSE); // will set radio button appropriately	
}
