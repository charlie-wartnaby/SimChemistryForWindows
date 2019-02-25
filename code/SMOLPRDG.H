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

// smolprdg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSMolPropsDlg dialog

#define SMOLPROP_USE_SYS_TEMP      0
#define SMOLPROP_USE_SPECIFIC_TEMP 1       

class CSMolPropsDlg : public CDialog
{
// Construction
public:
	CSMolPropsDlg(CWnd* pParent = NULL);	// standard constructor

	CSim* m_pSim;   
	CObjTypeMol* m_pMolType;

	
// Dialog Data
	//{{AFX_DATA(CSMolPropsDlg)
	enum { IDD = IDD_SMOL_PROPS };
	double	m_dDirection;
	double	m_dSpeed;
	double	m_dVx;
	double	m_dVy;
	double	m_dX;
	double	m_dY;
	double	m_dSpecificTemperature;
	int		m_nUseSysTemp;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual  BOOL OnInitDialog();    
	virtual void OnOK();     // prevents return key closing dialogue
	
	void RoundNumbers();
	
	// Generated message map functions
	//{{AFX_MSG(CSMolPropsDlg)
	afx_msg void OnNewRandomDirnBtn();
	afx_msg void OnNewThermSpeedBtn();
	afx_msg void OnRadioSysTemp();
	afx_msg void OnRadioThisTemp();
	afx_msg void OnTypePropsBtn();
	afx_msg void OnKillfocusEditDirection();
	afx_msg void OnKillfocusEditSpeed();
	afx_msg void OnKillfocusEditVx();
	afx_msg void OnKillfocusEditVy();
	afx_msg void OnKillfocusThisTempEdit();
	afx_msg void OnOkbtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
