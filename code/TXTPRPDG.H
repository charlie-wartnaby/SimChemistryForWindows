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

// txtprpdg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTextboxPropsDlg dialog

class CTextboxPropsDlg : public CDialog
{
// Construction
public:
	CTextboxPropsDlg(CWnd* pParent = NULL);	// standard constructor
    ~CTextboxPropsDlg();
    
    CString m_sFontName;
	int m_nFontSize;  
	CFont* m_pExampleFont;
    double m_dScaleFactor; // nm to logicals to allow for current zoom level
	
// Dialog Data
	//{{AFX_DATA(CTextboxPropsDlg)
	enum { IDD = IDD_TEXTBOX_PROPS_DLG };
	CString	m_sName;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    virtual BOOL OnInitDialog();   
    
    void UpdateExampleTextFont();
    
	// Generated message map functions
	//{{AFX_MSG(CTextboxPropsDlg)
	afx_msg void OnEditFontBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
