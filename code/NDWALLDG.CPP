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

// ndwalldg.cpp : implementation file
//

#include "stdafx.h"
#include "lennard.h"
#include "ndwalldg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNeedWallsForPistonsDlg dialog


CNeedWallsForPistonsDlg::CNeedWallsForPistonsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNeedWallsForPistonsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNeedWallsForPistonsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CNeedWallsForPistonsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNeedWallsForPistonsDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNeedWallsForPistonsDlg, CDialog)
	//{{AFX_MSG_MAP(CNeedWallsForPistonsDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNeedWallsForPistonsDlg message handlers
