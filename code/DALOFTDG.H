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

// daloftdg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDelAllOfTypeDlg dialog

class CDelAllOfTypeDlg : public CDialog
{
// Construction
public:
	CDelAllOfTypeDlg(CWnd* pParent = NULL);	// standard constructor
	CSim* m_pSim;  

// Dialog Data
	//{{AFX_DATA(CDelAllOfTypeDlg)
	enum { IDD = IDD_DEL_ALL_OF_TYPE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();    
	void UpdateTypeList();

	// Generated message map functions
	//{{AFX_MSG(CDelAllOfTypeDlg)
	afx_msg void OnDelSelnBtn();
	afx_msg void OnDblclkTypeList();
	afx_msg void OnSelchangeTypeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
