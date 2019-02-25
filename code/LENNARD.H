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

// lennard.h : main header file for the LENNARD application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#define new DEBUG_NEW   // use always to track memory leaks in debug builds


#include "resource.h"       // main symbols

#define SIMCHEMISTRY_VER_STRING "2.22"
#define SIMCHEMISTRY_VER_NUMERIC 2.22

#define GLOBAL_BUFF_SIZE 4096


extern char buff[GLOBAL_BUFF_SIZE]; // used everywhere for string manipulation


class CLenApp : public CWinApp
{
public:
	CLenApp();

	CByteArray m_simExistenceFlags;
	
	CMultiDocTemplate* m_pConsTemplate;
	CMultiDocTemplate* m_pSimTemplate;  
	
	// Whether to display first-time encouragement dialogue:
	UINT m_nShowFirstTimeDlg;       

    // Whether to show multiple selection resolution dialogue automatically:
    UINT m_nAutoShowSelnResolutionDlg;       
	
	
private:
    BOOL CallBaseClassIdle(LONG lCount);

// Overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	
// Implementation

	//{{AFX_MSG(CLenApp)
	afx_msg void OnAppAbout();
	afx_msg void OnUpdateFileNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CLenApp theApp;
/////////////////////////////////////////////////////////////////////////////
