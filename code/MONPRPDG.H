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

// monprpdg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditMonitorPropsDlg dialog

class CEditMonitorPropsDlg : public CDialog
{
// Construction
public:
	CEditMonitorPropsDlg(CWnd* pParent = NULL);	// standard constructor
    
    COLORREF m_cColour;
    
// Dialog Data
	//{{AFX_DATA(CEditMonitorPropsDlg)
	enum { IDD = IDD_EDIT_MONITOR_PROPS };
	int		m_nControlType;
	CString	m_sName;
	BOOL	m_nHatch;
	BOOL	m_nLabel;
	int		m_nLineThick;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    void UpdatePreview();
    
	// Generated message map functions
	//{{AFX_MSG(CEditMonitorPropsDlg)
	afx_msg void OnCheckHatch();
	afx_msg void OnCheckLabel();
	afx_msg void OnButtonColour();
	afx_msg void OnPaint();
	afx_msg void OnButtonPreview();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
