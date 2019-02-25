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

// drwmoldg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDrawMolsDlg dialog

class CDrawMolsDlg : public CDialog
{
// Construction
public:
	CDrawMolsDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDrawMolsDlg)
	enum { IDD = IDD_DRAW_MOLS };
	double	m_dDirectionAngle;
	int		m_nDirectionType;
	double	m_dFixedSpeedValue;
	int		m_nSpeedType;
	double	m_dThermalTemperatureValue;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CDrawMolsDlg)
	afx_msg void OnSetfocusFixedSpeedValue();
	afx_msg void OnSetfocusThermalTemperature();
	afx_msg void OnSetfocusDirectionAngle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
