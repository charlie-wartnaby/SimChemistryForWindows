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

// consfrm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConsFrame frame with splitter

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CConsFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CConsFrame)
protected:
	CConsFrame();			// protected constructor used by dynamic creation

// Attributes
protected:
	CSplitterWnd	m_wndSplitter;  
	int m_nCmdWndHeight;  // make variable in case auto-change with font later
public:

// Operations
public:

// Implementation
public:
	virtual ~CConsFrame();
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

protected:
	void OnUpdateFrameTitle(BOOL bAddToTitle); // overridden to change win title
	// Generated message map functions
	//{{AFX_MSG(CConsFrame)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
