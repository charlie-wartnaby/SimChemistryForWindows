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

// molprpdg.cpp : implementation file
//

#include "stdafx.h"
#include "simobjs.h"
#include "sim.h"
#include "mol.h"
#include "lennard.h"
#include "molprpdg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditMolPropsDlg dialog

CEditMolPropsDlg::CEditMolPropsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditMolPropsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditMolPropsDlg)
	m_dMass = 20;
	m_sName = "";
	m_dRadius = 0.1;
	m_dLineThick = 0.1;
	m_dFormationEnergy = 0.0;
	//}}AFX_DATA_INIT
}

void CEditMolPropsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditMolPropsDlg)
	DDX_Text(pDX, IDC_MASS, m_dMass);
	DDV_MinMaxDouble(pDX, m_dMass, LIM_MIN_MOL_MASS, LIM_MAX_MOL_MASS);
	DDX_Text(pDX, IDC_NAME, m_sName);
	DDV_MaxChars(pDX, m_sName, LIM_MAX_CHARS_OBJTYPE);
	DDX_Text(pDX, IDC_RADIUS, m_dRadius);
	DDV_MinMaxDouble(pDX, m_dRadius, LIM_MIN_MOL_RADIUS, LIM_MAX_MOL_RADIUS);
	DDX_Text(pDX, IDC_LINE_THICKNESS, m_dLineThick);
	DDV_MinMaxDouble(pDX, m_dLineThick, 0., 1.);
	DDX_Text(pDX, IDC_FORM_ENERGY, m_dFormationEnergy);
	DDV_MinMaxDouble(pDX, m_dFormationEnergy, LIM_MIN_FORMATION_ENERGY, LIM_MAX_FORMATION_ENERGY);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditMolPropsDlg, CDialog)
	//{{AFX_MSG_MAP(CEditMolPropsDlg)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_FILL_COLOUR, OnFillColour)
	ON_BN_CLICKED(IDC_LINE_COLOUR, OnLineColour)
	ON_EN_KILLFOCUS(IDC_LINE_THICKNESS, OnKillfocusLineThickness)
	ON_BN_CLICKED(IDC_UPDATE_PREVIEW, OnUpdatePreview)
	ON_EN_KILLFOCUS(IDC_RADIUS, OnKillfocusRadius)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEditMolPropsDlg message handlers

void CEditMolPropsDlg::OnPaint()
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
    
    int nMolScreenRadius;
	double radius = (m_dRadius * m_pSim->m_dScaleFactor);
	if (radius <= MAX_SCREEN_RADIUS)
	{      
		nMolScreenRadius = (int) radius;
	}
	else
	{    
		nMolScreenRadius = MAX_SCREEN_RADIUS;
	}
	
	int nMolScreenDiameter = nMolScreenRadius * 2;
    int nScreenLineThick = int (m_dLineThick * nMolScreenRadius);

	CPen pen;  
	BOOL success = pen.CreatePen(PS_SOLID, nScreenLineThick,
	                      m_cLineColour);        
	CBrush brush;                                                
	success &= brush.CreateSolidBrush(m_cFillColour);  
    
    if (success)
    {
	    CPen* pOldPen = pControlDC->SelectObject(&pen);
	    CBrush* pOldBrush = pControlDC->SelectObject(&brush);
	
		// Want to put the molecule in the centre of this little window
		// if it is small enough, or just touching the centre of the top
		// edge if it is too big (so that at least we'll see part of it).
		
		pControlDC->DPtoLP(&clientRect);
		
		int x, y;
		
		if ((nMolScreenDiameter > -clientRect.Height()) // minus sign for MM_LOENGLISH
		     || (nMolScreenDiameter > clientRect.Width()))
		{
			// too big to fit in window, position touching centre-top
			x = clientRect.Width() / 2;
			y = -nMolScreenRadius;
		}
		else
		{
			// fits in window, position centrally
			x = clientRect.Width() / 2;
			y = clientRect.Height() / 2;
		}
		
		// adjust x,y from centre of mol to top left:
		x = x - nMolScreenRadius;
		y = y + nMolScreenRadius;                    
		
		pControlDC->Ellipse(x + nScreenLineThick / 2,
		                   y - nScreenLineThick / 2,
		                   x + nMolScreenDiameter - nScreenLineThick / 2,
		                   y + nScreenLineThick / 2 - nMolScreenDiameter);    
		
	    pControlDC->SelectObject(pOldPen);
	    pControlDC->SelectObject(pOldBrush);     
	}
	else
	{
		pWnd->SetWindowText("Graphics exhausted");
	}
	
	pWnd->ReleaseDC(pControlDC);	
}

void CEditMolPropsDlg::OnFillColour()
{
	CColorDialog colDlg(m_cFillColour); 

	if (IDOK == colDlg.DoModal())
	{
		m_cFillColour = colDlg.GetColor();
	}

    UpdatePreview();
}

void CEditMolPropsDlg::OnLineColour()
{
	CColorDialog colDlg(m_cLineColour);     

	if (IDOK == colDlg.DoModal())
	{
		m_cLineColour = colDlg.GetColor();
	}

    UpdatePreview();
}

void CEditMolPropsDlg::OnKillfocusLineThickness()
{
	TRACE("OnKillfocusLineThickness\n");
	UpdatePreview();	
}

void CEditMolPropsDlg::OnUpdatePreview()
{
	TRACE("OnUpdatePreview\n");
	UpdatePreview();	
}

void CEditMolPropsDlg::OnKillfocusRadius()
{
	TRACE("OnKillfocusRadius\n");
	UpdatePreview();	
}

void CEditMolPropsDlg::UpdatePreview()
{
	UpdateData(TRUE);  // does data exchange
	Invalidate();  // causes redraw
}
