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

// walprpdg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditWallPropsDlg dialog

class CEditWallPropsDlg : public CDialog
{
// Construction
public:
	CEditWallPropsDlg(CWnd* pParent = NULL);	// standard constructor
	
	COLORREF m_cLineColour;


// Dialog Data
	//{{AFX_DATA(CEditWallPropsDlg)
	enum { IDD = IDD_EDIT_WALL_PROPS };
	int		m_nLineThick;
	double	m_dSpecialTemperature;
	CString	m_sName;
	int		m_nTemperatureType;
	int		m_nMolIntType;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    void UpdatePreview();
    
	// Generated message map functions
	//{{AFX_MSG(CEditWallPropsDlg)
	afx_msg void OnUpdatePreview();
	afx_msg void OnLineColourBtn();
	afx_msg void OnPaint();
	afx_msg void OnSetfocusSpecialTempEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
