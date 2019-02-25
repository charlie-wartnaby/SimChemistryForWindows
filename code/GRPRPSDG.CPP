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

// grprpsdg.cpp : implementation file
//

#include "stdafx.h"
#include "lennard.h"   
#include "simobjs.h"
#include "grprpsdg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditGraphTypePropsDlg dialog


CEditGraphTypePropsDlg::CEditGraphTypePropsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditGraphTypePropsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditGraphTypePropsDlg)
	m_sName = "";
	//}}AFX_DATA_INIT
}

void CEditGraphTypePropsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditGraphTypePropsDlg)
	DDX_Text(pDX, IDC_EDIT_NAME, m_sName);
	DDV_MaxChars(pDX, m_sName, LIM_MAX_CHARS_OBJTYPE);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditGraphTypePropsDlg, CDialog)
	//{{AFX_MSG_MAP(CEditGraphTypePropsDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEditGraphTypePropsDlg message handlers
