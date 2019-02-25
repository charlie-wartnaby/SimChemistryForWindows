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

// simfrm.cpp : implementation file
//

#include "stdafx.h"
#include "lennard.h"
#include "simfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimFrm

IMPLEMENT_DYNCREATE(CSimFrm, CMDIChildWnd)

CSimFrm::CSimFrm()
{      
	m_nHaveMaximisedSelf = FALSE;
}

CSimFrm::~CSimFrm()
{
}

void CSimFrm::ActivateFrame(int nCmdShow)
{    
	if (m_nHaveMaximisedSelf)
	{
		// do not maximise self if user has 'restored' us since we
		// were first created                         
		
		CMDIChildWnd::ActivateFrame(nCmdShow);
	}
	else
	{                                       
		m_nHaveMaximisedSelf = TRUE;
		CMDIChildWnd::ActivateFrame(SW_SHOWMAXIMIZED);
    }
		
}

BEGIN_MESSAGE_MAP(CSimFrm, CMDIChildWnd)
	//{{AFX_MSG_MAP(CSimFrm)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSimFrm message handlers

void CSimFrm::OnUpdateFrameTitle(BOOL bAddToTitle)
{                
	// This is based on the original in winmdi.cpp. It
	// is changed here to add ':Simulation View' to the document
	// title rather than ':2' etc.
	
	
	// update our parent window first
	GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);

	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave child window alone!

	CDocument* pDocument = GetActiveDocument();
	if (bAddToTitle && pDocument != NULL)
	{
		char szOld[256];
		GetWindowText(szOld, sizeof(szOld));
		char szText[256];

		lstrcpy(szText, pDocument->GetTitle());  
		
		lstrcat(szText, ":Simulation View");

		// set title if changed, but don't remove completely
		if (lstrcmp(szText, szOld) != 0)
			SetWindowText(szText);
	}

}
