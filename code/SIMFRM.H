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

// simfrm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSimFrm frame

class CSimFrm : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CSimFrm)
protected:
	CSimFrm();			// protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Implementation
protected:      
	void OnUpdateFrameTitle(BOOL bAddToTitle); // overridden to change win title
	virtual void ActivateFrame(int nCmdShow = -1); // overridden to auto maximise
    int m_nHaveMaximisedSelf;
    
	virtual ~CSimFrm();

	// Generated message map functions
	//{{AFX_MSG(CSimFrm)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
