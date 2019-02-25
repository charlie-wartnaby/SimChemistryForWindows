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

// edintsdg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditIntsDlg dialog

class CEditIntsDlg : public CDialog
{
// Construction
public:
	CEditIntsDlg(CWnd* pParent = NULL);	// standard constructor
	CSim* m_pSim;  
	int m_nActiveIntIndex;      
	CPtrArray m_intArray;
	
// Dialog Data
	//{{AFX_DATA(CEditIntsDlg)
	enum { IDD = IDD_EDIT_INTS_DLG };
	CString	m_sObjType2;   
	CString	m_sObjType1;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support   
	virtual BOOL OnInitDialog();  
	
	void UpdateIntList();

	// Generated message map functions
	//{{AFX_MSG(CEditIntsDlg)
	afx_msg void OnEditDetailsBtn();
	afx_msg void OnShowFirstBtn();
	afx_msg void OnShowNextBtn();
	afx_msg void OnSelchangeObj2Combo();
	afx_msg void OnSelchangeObj1Combo();
	afx_msg void OnMakeSelActive();
	afx_msg void OnDblclkIntTypeList();
	afx_msg void OnSelchangeIntTypeList();
	afx_msg void OnHelpBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
