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

// sysprpdg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditSysPropsDlg dialog

class CEditSysPropsDlg : public CDialog
{
// Construction
public:
	CEditSysPropsDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CEditSysPropsDlg)
	enum { IDD = IDD_SYS_PROPS };
	double	m_dGravitationalAcceleration;
	double	m_dSystemTemperature;
	BOOL	m_nAutoRun;
	BOOL	m_nUseAutoTimestep;
	double	m_dTimestep;
	double	m_dAverageTimeConst;
	long	m_lRandSeed;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    virtual BOOL OnInitDialog();

    void UpdateEditBoxes();

	// Generated message map functions
	//{{AFX_MSG(CEditSysPropsDlg)
	afx_msg void OnHelpBtn();
	afx_msg void OnCheckAutoTimestep();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
