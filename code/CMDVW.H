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

// cmdvw.h : interface of the CCmdView class
//
/////////////////////////////////////////////////////////////////////////////

class CCmdView : public CEditView
{
protected: // create from serialization only
	CCmdView();
	DECLARE_DYNCREATE(CCmdView)

// Attributes
public:
	CLenDoc* GetDocument();

// Operations
public:

// Implementation
public:
	virtual ~CCmdView();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
// Generated message map functions
protected:
	//{{AFX_MSG(CCmdView)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditPaste();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in wibblvw.cpp
inline CLenDoc* CCmdView::GetDocument()
   { return (CLenDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
