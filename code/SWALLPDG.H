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

// swallpdg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSWallPropsDlg dialog

class CSWallPropsDlg : public CDialog
{
// Construction
public:
	CSWallPropsDlg(CWnd* pParent = NULL);	// standard constructor

	CSim* m_pSim;   
	CObjTypeWall* m_pWallType;
	
// Dialog Data
	//{{AFX_DATA(CSWallPropsDlg)
	enum { IDD = IDD_SWALL_PROPS };
	double	m_dX1;
	double	m_dX2;
	double	m_dY1;
	double	m_dY2;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual  BOOL OnInitDialog();    
	virtual void OnOK();     // prevents return key closing dialogue
	
	void DisplayGridSettings();
	
	// Generated message map functions
	//{{AFX_MSG(CSWallPropsDlg)
	afx_msg void OnOkbtn();
	afx_msg void OnGridSettingsBtn();
	afx_msg void OnGridifyBtn();
	afx_msg void OnTypePropsBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
