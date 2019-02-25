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

#if !defined(AFX_SIMSIZEDLG_H__0DC68654_AB17_11D3_B029_004095442A0D__INCLUDED_)
#define AFX_SIMSIZEDLG_H__0DC68654_AB17_11D3_B029_004095442A0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SimSizeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSimSizeDlg dialog

class CSimSizeDlg : public CDialog
{
// Construction
public:
	CSimSizeDlg(CWnd* pParent = NULL);   // standard constructor
    CSim* m_pSim;

// Dialog Data
	//{{AFX_DATA(CSimSizeDlg)
	enum { IDD = IDD_SIM_SIZE_DLG };
	double	m_dBottom;
	double	m_dHeight;
	double	m_dLeft;
	double	m_dWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimSizeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSimSizeDlg)
	afx_msg void OnDefault();
	afx_msg void OnLikeV10();
	afx_msg void OnApply();
	virtual void OnOK();
	afx_msg void OnHelpBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMSIZEDLG_H__0DC68654_AB17_11D3_B029_004095442A0D__INCLUDED_)
