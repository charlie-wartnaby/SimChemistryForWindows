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

// newsvdg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewSliderValueDlg dialog

class CNewSliderValueDlg : public CDialog
{
// Construction
public:
	CNewSliderValueDlg(CWnd* pParent = NULL);	// standard constructor
    
    double m_dMinValue;
    double m_dMaxValue; 
    CString m_sUnits;
    
// Dialog Data
	//{{AFX_DATA(CNewSliderValueDlg)
	enum { IDD = IDD_NEW_SLIDER_VALUE_DLG };
	double	m_dValue;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support   
	virtual BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(CNewSliderValueDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
