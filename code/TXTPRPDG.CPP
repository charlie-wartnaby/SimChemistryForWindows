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

// txtprpdg.cpp : implementation file
//

#include "stdafx.h"
#include "lennard.h"    
#include "simobjs.h"
#include "textbox.h"
#include "txtprpdg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextboxPropsDlg dialog


CTextboxPropsDlg::CTextboxPropsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTextboxPropsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextboxPropsDlg)
	m_sName = "";
	//}}AFX_DATA_INIT 
	
	m_pExampleFont = new CFont;
}

CTextboxPropsDlg::~CTextboxPropsDlg()
{
	delete m_pExampleFont;
}

void CTextboxPropsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextboxPropsDlg)
	DDX_Text(pDX, IDC_TYPE_NAME, m_sName);
	DDV_MaxChars(pDX, m_sName, LIM_MAX_CHARS_OBJTYPE);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTextboxPropsDlg, CDialog)
	//{{AFX_MSG_MAP(CTextboxPropsDlg)
	ON_BN_CLICKED(IDC_EDIT_FONT_BTN, OnEditFontBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CTextboxPropsDlg::OnInitDialog() 
{                      
    CDialog::OnInitDialog();  // to do data exchange
	                       
	UpdateExampleTextFont();
	
	return TRUE; // leaves it to Windows to highlight first control
}

void CTextboxPropsDlg::UpdateExampleTextFont()
{
	// set example text to current font
	
    // slight problem with this: doesn't do drawing properly in correct
    // mapping mode, so font ends up slightly larger than on-screen.

	CEdit* pText = (CEdit*) GetDlgItem(IDC_EXAMPLE_TEXT);
	
	delete m_pExampleFont;
	m_pExampleFont = new CFont; // otherwise debug GDI code asserts

    int nTargetFontSize = CObjTypeTextbox::CalcTargetFontSize(m_dScaleFactor, m_nFontSize);

    LOGFONT logfont;

    logfont.lfHeight        = -nTargetFontSize; // height   
	logfont.lfWidth         = 0;   // width, use default
	logfont.lfEscapement    = 0;   // escapement           
	logfont.lfOrientation   = 0;   // orientation                
	logfont.lfWeight        = FW_NORMAL;
	logfont.lfItalic        = FALSE;   // italic                       
	logfont.lfUnderline     = FALSE;   // underline          
	logfont.lfStrikeOut     = 0;   // strikeout                    
	logfont.lfCharSet       = ANSI_CHARSET; 
	logfont.lfOutPrecision  = OUT_DEFAULT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality       = DEFAULT_QUALITY;    
	logfont.lfPitchAndFamily  = DEFAULT_PITCH;  
	strcpy(logfont.lfFaceName, m_sFontName);

	m_pExampleFont->CreateFontIndirect(&logfont);

	pText->SetFont(m_pExampleFont);
}
/////////////////////////////////////////////////////////////////////////////
// CTextboxPropsDlg message handlers

void CTextboxPropsDlg::OnEditFontBtn()
{
	CFontDialog fdlg;   
	
	// initialise to font currently stored in member variables
	
	strcpy(fdlg.m_cf.lpLogFont->lfFaceName, m_sFontName);
	fdlg.m_cf.lpLogFont->lfHeight = -m_nFontSize;   
	fdlg.m_cf.Flags |= CF_INITTOLOGFONTSTRUCT;
	
	if (IDOK == fdlg.DoModal())
	{
		// get new font parameters
		m_sFontName = fdlg.m_cf.lpLogFont->lfFaceName;
		m_nFontSize = -fdlg.m_cf.lpLogFont->lfHeight;
		
		UpdateExampleTextFont();
	}
}
