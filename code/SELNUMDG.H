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

// selnumdg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelNumericDlg dialog

class CSelNumericDlg : public CDialog
{
// Construction
public:
	CSelNumericDlg(CWnd* pParent = NULL);	// standard constructor
	~CSelNumericDlg();
	
	virtual BOOL OnInitDialog();
	CSim* m_pSim;     
	CString m_sInitSeln;  // name of numeric to select to start with
		
	int m_nNumericType;  // whether want readable only, writeable only etc
	                     // or in appropriate NUM_* constants     
	                     
	CString m_sNumeric;  // text name of one selected
	
// Dialog Data
	//{{AFX_DATA(CSelNumericDlg)
	enum { IDD = IDD_SEL_NUMERIC_DLG };
	int		m_nSelNumeric;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    virtual void OnOK();
    
	int m_nListTabstops[2]; 
    CFont* m_pListFont;
    
	// Generated message map functions
	//{{AFX_MSG(CSelNumericDlg)
	afx_msg void OnDblclkNumList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
