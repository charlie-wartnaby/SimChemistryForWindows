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

// mncsetdg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMonitorCtrlSetDlg dialog

class CMonitorCtrlSetDlg : public CDialog
{
// Construction
public:
	CMonitorCtrlSetDlg(CWnd* pParent = NULL);	// standard constructor   

    ~CMonitorCtrlSetDlg();
	
	CSim* m_pSim;    
    CObjTypeMonitor* m_pMonitorType;     
    
    CSObjMonitor* m_pMonitor;  // captive example monitor for dialogue use  
    
    int m_nClone;               // if copying existing one 
    CSObjMonitor* m_pCloneMonitor; // one to copy

// Dialog Data
	//{{AFX_DATA(CMonitorCtrlSetDlg)
	enum { IDD = IDD_MON_CTRL_SET_DLG };
	BOOL	m_bHeaterActive;
	CString	m_sCtrlSummary;
	double	m_dDensitySelected;
	double	m_dHeaterPower;
	double	m_dTemperatureSelected;
	BOOL	m_nTemperatureFixedCheck;
	BOOL	m_nDensityFixedCheck;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    virtual void OnOK();
	virtual BOOL OnInitDialog();
		
	void EnableControls();   // will grey out bits depending on current monitor props     
	void UpdateMolList();    // fills in list box with mol types and current settings
	void UpdateEditBoxes();  // copies attributes for selected mol type to controls beneath
	void ApplyEdits();       // copies edit-box attributes into CSObjMonitor
	
	int m_molListTabstops[4]; 
	int m_nOldMolSeln;       // mol type selected before change
	
	// Generated message map functions
	//{{AFX_MSG(CMonitorCtrlSetDlg)
	afx_msg void OnSelchangeFixerList();
	afx_msg void OnCheckDensity();
	afx_msg void OnCheckTemperature();
	afx_msg void OnKillfocusEditDensity();
	afx_msg void OnKillfocusEditTemperature();
	afx_msg void OnTypePropsBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
