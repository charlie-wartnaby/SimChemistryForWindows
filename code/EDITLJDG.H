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

// editljdg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditLJDetailsDlg dialog

class CEditLJDetailsDlg : public CDialog
{
// Construction
public:
	CEditLJDetailsDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CEditLJDetailsDlg)
	enum { IDD = IDD_EDIT_LJ_DETAILS };
	double	m_dCutoffDist;
	double	m_dEqmSepn;
	double	m_dMaxPE;
	double	m_dWellDepth;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CEditLJDetailsDlg)
	afx_msg void OnHelpBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
