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

#if !defined(AFX_RESOLVEMULTIPLESELNDLG_H__82B4E2C4_B3FD_11D3_B035_004095442A0D__INCLUDED_)
#define AFX_RESOLVEMULTIPLESELNDLG_H__82B4E2C4_B3FD_11D3_B035_004095442A0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ResolveMultipleSelnDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CResolveMultipleSelnDlg dialog

class CResolveMultipleSelnDlg : public CDialog
{
// Construction
public:
	CResolveMultipleSelnDlg(CWnd* pParent = NULL);   // standard constructor
    CSim* m_pSim;
    ~CResolveMultipleSelnDlg();

// Dialog Data
	//{{AFX_DATA(CResolveMultipleSelnDlg)
	enum { IDD = IDD_MULTIPLE_SELN };
	BOOL	m_bAlwaysShow;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResolveMultipleSelnDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

    virtual BOOL OnInitDialog();

// Implementation
protected:

    int* m_wasSelectedArray;
    int m_nNumObjs;
    int m_nNumWereSelected;
    void ApplySelnChanges();

	// Generated message map functions
	//{{AFX_MSG(CResolveMultipleSelnDlg)
	afx_msg void OnDblclkObjectList();
	afx_msg void OnSelchangeObjectList();
	afx_msg void OnUnselectAll();
	afx_msg void OnAlwaysForSingleClick();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESOLVEMULTIPLESELNDLG_H__82B4E2C4_B3FD_11D3_B035_004095442A0D__INCLUDED_)
