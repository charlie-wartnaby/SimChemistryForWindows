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

// cmdvw.cpp : implementation of the CCmdView class
//

#include "stdafx.h"
#include "simobjs.h"
#include "sim.h"
#include "lennard.h"

#include "lendoc.h"
#include "cmdvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif                        

static char BASED_CODE szClassName[] = "EDIT";


/////////////////////////////////////////////////////////////////////////////
// CCmdView

IMPLEMENT_DYNCREATE(CCmdView, CEditView)

BEGIN_MESSAGE_MAP(CCmdView, CEditView)
	//{{AFX_MSG_MAP(CCmdView)
	ON_WM_CHAR()
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdView construction/destruction

CCmdView::CCmdView()
{
	// TODO: add construction code here
}

CCmdView::~CCmdView()
{
}



/////////////////////////////////////////////////////////////////////////////
// CCmdView drawing

void CCmdView::OnDraw(CDC* pDC)
{
	CLenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CCmdView diagnostics

#ifdef _DEBUG
void CCmdView::AssertValid() const
{
	CEditView::AssertValid();
}

void CCmdView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}

CLenDoc* CCmdView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLenDoc)));
	return (CLenDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCmdView message handlers

void CCmdView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView); 
	if (bActivate)
	{ 
  		CLenDoc* pDoc = GetDocument();
		CString docTitle = pDoc->GetTitle();
		docTitle += ":Console";

		CFrameWnd* pFrame = GetParentFrame();	
		ASSERT_VALID(pFrame);
		pFrame->SetWindowText(docTitle);  
	}

}

void CCmdView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (VK_RETURN == nChar)
	{
		CLenDoc* pDoc = GetDocument();
		CSim* pSim = pDoc->m_pSim;  
		CString newCommand;
		GetWindowText(newCommand);
		if (!newCommand.IsEmpty())
		{
			int ret = pSim->HandleNewCommand(newCommand);
			if (SR_ERROR != ret)
			{
				SetWindowText(""); 
			}   
			else
			{
				AfxMessageBox("Command error or warning (see history)");
			}
			pDoc->UpdateAllViews(NULL);
		}
	}
	else
	{
		CEditView::OnChar(nChar, nRepCnt, nFlags);
	}
}


void CCmdView::OnEditPaste()
{   
	// have overidden this to allow checking for multiline paste  
/*	COleDataSource source;
	if (source.AttachClipboard();)
	{
		COleDataObject::IsDataAvailable to check whether a particular format is available. Use COleDataObject::BeginEnumFormats
*/	CEditView::OnEditPaste();
}
