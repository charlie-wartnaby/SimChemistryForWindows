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

// pisprpdg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditPistonPropsDlg dialog

class CEditPistonPropsDlg : public CDialog
{
// Construction
public:
	CEditPistonPropsDlg(CWnd* pParent = NULL);	// standard constructor
    
public:
	CObjTypeWall* m_pWallType;
	CSim* m_pSim;
	CPtrArray m_wallTypeList;
	
// Dialog Data
	//{{AFX_DATA(CEditPistonPropsDlg)
	enum { IDD = IDD_EDIT_PISTON_PROPS };
	double	m_dFixedPressure;
	double	m_dForceConst;
	double  m_dDampingConst;
	double	m_dMass;
	int		m_nPressureType;
	CString	m_sName;
	int		m_nWallTypeIndex;
	int		m_nDirectionType;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support    
	virtual BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(CEditPistonPropsDlg)
	afx_msg void OnSetfocusForceConstEdit();
	afx_msg void OnSetfocusFixedPressureEdit();
	afx_msg void OnSelchangeWallTypeCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
