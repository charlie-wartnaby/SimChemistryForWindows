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

// sslprpdg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSSliderPropsDlg dialog

class CSSliderPropsDlg : public CDialog
{
// Construction
public:
	CSSliderPropsDlg(CWnd* pParent = NULL);	// standard constructor
    
    CSim* m_pSim;   
    
    CString m_sData;   // name of quantity to control, eg "sim:temperature"
    
    double m_dAllowedMin; // abs limits allowed according to CNumeric
    double m_dAllowedMax;                                            
    
    int m_nScaleLimsInit; // whether scalemin and max initialised for us  
    int m_nNumericSetOnceAlready; // but if change ref, will change lims anyway
    
// Dialog Data
	//{{AFX_DATA(CSSliderPropsDlg)
	enum { IDD = IDD_SLIDER_PROPS_DLG };
	BOOL	m_nCheckTitle;
	double	m_dScaleMax;
	double	m_dScaleMin;
	CString	m_sTitle;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    virtual BOOL OnInitDialog();
    
    void InitialiseMinMax();
    void UpdateFields();
    
	// Generated message map functions
	//{{AFX_MSG(CSSliderPropsDlg)
	afx_msg void OnCheckTitle();
	afx_msg void OnSelectBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
