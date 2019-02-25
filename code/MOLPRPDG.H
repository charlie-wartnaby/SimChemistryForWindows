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

// molprpdg.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CEditMolPropsDlg dialog

class CEditMolPropsDlg : public CDialog
{
// Construction
public:
	CEditMolPropsDlg(CWnd* pParent = NULL);	// standard constructor

	CSim* m_pSim;  
	COLORREF m_cFillColour;
	COLORREF m_cLineColour;
	
// Dialog Data
	//{{AFX_DATA(CEditMolPropsDlg)
	enum { IDD = IDD_EDIT_MOL_PROPS };
	double	m_dMass;
	CString	m_sName;
	double	m_dRadius;
	double	m_dLineThick;
	double	m_dFormationEnergy;
	//}}AFX_DATA

// Implementation
protected:   
	void UpdatePreview();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CEditMolPropsDlg)
	afx_msg void OnPaint();
	afx_msg void OnFillColour();
	afx_msg void OnLineColour();
	afx_msg void OnKillfocusLineThickness();
	afx_msg void OnUpdatePreview();
	afx_msg void OnKillfocusRadius();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
