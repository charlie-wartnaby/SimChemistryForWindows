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

// drwgphdg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDrawGraphDlg dialog

class CDrawGraphDlg : public CDialog
{
// Construction
public:
	CDrawGraphDlg(CWnd* pParent = NULL);	// standard constructor
    
    CSim* m_pSim; 
    CString m_sX;
    CString m_sY;  
	COLORREF m_cPointColour;
    
// Dialog Data
	//{{AFX_DATA(CDrawGraphDlg)
	enum { IDD = IDD_DRAW_GRAPH_DLG };
	CString	m_sGraphTitle;
	CString	m_sXLabel;
	CString	m_sYLabel;
	BOOL	m_nCheckTitle;
	BOOL	m_nCheckXLabel;
	BOOL	m_nCheckYLabel;
	double	m_dXLeft;
	double	m_dXRight;
	double	m_dYBottom;
	double	m_dYTop;
	BOOL	m_nFilled;
	int		m_nStyle;
	int		m_nSize;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support 
	virtual  BOOL OnInitDialog();
	
    void UpdateFields(int nDoUpdateData);
    void UpdatePreview();                
    void OnButtonSelXOrY(CString& sNumeric, CString& sLabel);
    
	// Generated message map functions
	//{{AFX_MSG(CDrawGraphDlg)
	afx_msg void OnButtonSelX();
	afx_msg void OnButtonSelY();
	afx_msg void OnCheckTitle();
	afx_msg void OnCheckXlabel();
	afx_msg void OnCheckYlabel();
	afx_msg void OnButtonColour();
	afx_msg void OnPaint();
	afx_msg void OnSelchangeComboSize();
	afx_msg void OnSelchangeComboStyle();
	afx_msg void OnCheckFilled();
	afx_msg void OnButtonUpdatePreview();
	afx_msg void OnKillfocusEditSize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
