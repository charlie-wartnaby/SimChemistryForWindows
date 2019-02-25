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

#if !defined(AFX_REACTIVEINTPROPSDLG_H__76E2B284_DE71_11D3_B090_004095442A0D__INCLUDED_)
#define AFX_REACTIVEINTPROPSDLG_H__76E2B284_DE71_11D3_B090_004095442A0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ReactiveIntPropsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReactiveIntPropsDlg dialog

class CReactiveIntPropsDlg : public CDialog
{
// Construction
public:
	CReactiveIntPropsDlg(CWnd* pParent = NULL);   // standard constructor
    CSim* m_pSim;
    CInt* m_pInt;

// Dialog Data
	//{{AFX_DATA(CReactiveIntPropsDlg)
	enum { IDD = IDD_REACTIVE_INT_PROPS };
	double	m_dBarrier;
	BOOL	m_nDoReverse;
	BOOL	m_nTwoProducts;
	CString	m_sProduct1;
	CString	m_sProduct2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReactiveIntPropsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

    virtual BOOL CReactiveIntPropsDlg::OnInitDialog();
    virtual void OnOK();

// Implementation
protected:

    void UpdateControls();

	// Generated message map functions
	//{{AFX_MSG(CReactiveIntPropsDlg)
	afx_msg void OnCheckTwoProds();
	afx_msg void OnHelpBtn();
	afx_msg void OnKillfocusActivationEnergy();
	afx_msg void OnSelchangeComboProd1();
	afx_msg void OnSelchangeComboProd2();
	afx_msg void OnMyOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REACTIVEINTPROPSDLG_H__76E2B284_DE71_11D3_B090_004095442A0D__INCLUDED_)
