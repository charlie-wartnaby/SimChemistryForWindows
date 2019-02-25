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

// SimSizeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lennard.h"
#include "simobjs.h"
#include "sim.h"
#include "SimSizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimSizeDlg dialog


CSimSizeDlg::CSimSizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSimSizeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSimSizeDlg)
	m_dBottom = 0.0;
	m_dHeight = 0.0;
	m_dLeft = 0.0;
	m_dWidth = 0.0;
	//}}AFX_DATA_INIT
}


void CSimSizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSimSizeDlg)
	DDX_Text(pDX, IDC_EDIT_BOTTOM, m_dBottom);
	DDV_MinMaxDouble(pDX, m_dBottom, LIM_MIN_Y, LIM_MAX_SIM_BOTTOM);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_dHeight);
	DDV_MinMaxDouble(pDX, m_dHeight, LIM_MIN_SIM_HEIGHT, LIM_MAX_SIM_HEIGHT);
	DDX_Text(pDX, IDC_EDIT_LEFT, m_dLeft);
	DDV_MinMaxDouble(pDX, m_dLeft, LIM_MIN_X, LIM_MAX_SIM_LEFT);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_dWidth);
	DDV_MinMaxDouble(pDX, m_dWidth, LIM_MIN_SIM_WIDTH, LIM_MAX_SIM_WIDTH);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSimSizeDlg, CDialog)
	//{{AFX_MSG_MAP(CSimSizeDlg)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_BN_CLICKED(IDC_LIKE_V10, OnLikeV10)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_HELP_BTN, OnHelpBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimSizeDlg message handlers

void CSimSizeDlg::OnDefault() 
{
    // make old default area viewable

	m_dLeft = TRAD_SCREEN_LEFT_NM;
    m_dBottom = TRAD_SCREEN_BOTTOM_NM;
    m_dWidth = TRAD_SCREEN_WIDTH_NM;
    m_dHeight = TRAD_SCREEN_HEIGHT_NM;

    UpdateData(FALSE);

    OnApply();
}

void CSimSizeDlg::OnLikeV10() 
{
	// behave like SimChemistry v1.0, i.e. show larger sim area when
    // screen made bigger, keeping scale factor the same; need to ask
    // sim for current window co-ords
    double dScale = TRAD_SCREEN_SF;

    if (dScale < NEARLY_ZERO)
    {
        ASSERT(FALSE);
    }
    else
    {
        m_dLeft = TRAD_SCREEN_LEFT_NM;
        m_dWidth = m_pSim->m_ScreenLogicalRect.Width() / dScale;
        m_dHeight = -m_pSim->m_ScreenLogicalRect.Height() / dScale;
        m_dBottom = TRAD_SCREEN_HEIGHT_NM + TRAD_SCREEN_BOTTOM_NM - m_dHeight;
    }
	
    UpdateData(FALSE);

    OnApply();
}

void CSimSizeDlg::OnApply() 
{
    UpdateData(TRUE);

    sprintf(buff, "sim_size left %g bottom %g width %g height %g",
                  m_dLeft, m_dBottom, m_dWidth, m_dHeight);

    m_pSim->HandleNewCommand(buff);
}

void CSimSizeDlg::OnOK() 
{
    OnApply();
	
	CDialog::OnOK();
}

void CSimSizeDlg::OnHelpBtn() 
{
#if (_MSC_VER <= 1299) /* MSVC 6 = 1200 */
		AfxGetApp()->WinHelp(0x10000 + ID_SIMULATION_SIZE_SCALING);
#else
		::HtmlHelp(this->m_hWnd, "Lennard.chm", HH_DISPLAY_TOC, NULL);
#endif

}
