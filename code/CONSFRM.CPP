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

// consfrm.cpp : implementation file
//

#include "stdafx.h"
#include "simobjs.h"
#include "lennard.h"
#include "consfrm.h"       
#include "lendoc.h"
#include "consvw.h"
#include "cmdvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConsFrame

IMPLEMENT_DYNCREATE(CConsFrame, CMDIChildWnd)

CConsFrame::CConsFrame()
{                     
	m_nCmdWndHeight = 30;  // ought to fit to font
}

CConsFrame::~CConsFrame()
{
}

void CConsFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{                
	// This is based on the original in winmdi.cpp. It
	// is changed here to add ':console' to the document
	// title rather than ':2' etc.
	
	
	// update our parent window first
	GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);

	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave child window alone!

	CDocument* pDocument = GetActiveDocument();
	if (bAddToTitle && pDocument != NULL)
	{
		char szOld[256];
		GetWindowText(szOld, sizeof(szOld));
		char szText[256];

		lstrcpy(szText, pDocument->GetTitle());
		
		  
		// The following line is changed from the CMDIChildWnd version:
		lstrcat(szText, ":Console");

		// set title if changed, but don't remove completely
		if (lstrcmp(szText, szOld) != 0)
			SetWindowText(szText);
	}

}

BOOL CConsFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{     
	BOOL rtn;
	
	rtn = m_wndSplitter.CreateStatic(this,
		                             2,    // num rows
		                             1);   // num cols  
		                             
	CRect clientRect;
	m_wndSplitter.GetClientRect(&clientRect);
		
	rtn |= m_wndSplitter.CreateView(0,
	                                0,
	                                RUNTIME_CLASS(CConsoleView),
	                                CSize(lpcs->cx, lpcs->cy - 55),
	                                pContext);

	rtn |= m_wndSplitter.CreateView(1,
	                                0,
	                                RUNTIME_CLASS(CCmdView),
	                                CSize(lpcs->cx, 100),
	                                pContext);
	
	                                
	return rtn;	                                
}

BEGIN_MESSAGE_MAP(CConsFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CConsFrame)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CConsFrame message handlers


void CConsFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	
	// Want to keep command window fixed height (if it fits)
	CRect clientRect;
	m_wndSplitter.GetClientRect(&clientRect);  
	
	if ((!clientRect.IsRectEmpty())
	     && (clientRect.bottom > m_nCmdWndHeight))
	{
		 m_wndSplitter.SetRowInfo(0, clientRect.bottom - m_nCmdWndHeight, 0);
		 m_wndSplitter.SetRowInfo(1, m_nCmdWndHeight, 0);
		 m_wndSplitter.RecalcLayout();
	}
	
}
