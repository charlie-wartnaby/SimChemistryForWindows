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

// spisprdg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSPistonPropsDlg dialog

class CSPistonPropsDlg : public CDialog
{
// Construction
public:
	CSPistonPropsDlg(CWnd* pParent = NULL);	// standard constructor
    
    CSim* m_pSim;
    CObjTypePiston* m_pPistonType;
    
// Dialog Data
	//{{AFX_DATA(CSPistonPropsDlg)
	enum { IDD = IDD_SPISTON_PROPS_DLG };
	double	m_dHeight;
	double	m_dVx;
	double	m_dVy;
	double	m_dWidth;
	double	m_dXLeft;
	double	m_dXLRest;
	double	m_dXRight;
	double	m_dYBottom;
	double	m_dYTop;
	double	m_dYTRest;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual  BOOL OnInitDialog();    
	virtual void OnOK();     // prevents return key closing dialogue
	
	void RoundNumbers();    
    void DisplayGridSettings();
    void DisableOneVelocity();
    
	// Generated message map functions
	//{{AFX_MSG(CSPistonPropsDlg)
	afx_msg void OnOkbtn();
	afx_msg void OnGridSettingsBtn();
	afx_msg void OnGridifyBtn();
	afx_msg void OnTypePropsBtn();
	afx_msg void OnKillfocusEditHeight();
	afx_msg void OnKillfocusEditWidth();
	afx_msg void OnKillfocusEditXright();
	afx_msg void OnKillfocusEditYbot();
	afx_msg void OnKillfocusEditXleft();
	afx_msg void OnKillfocusEditYtop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
