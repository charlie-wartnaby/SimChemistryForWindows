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

// mainfrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "lennard.h"
#include "simobjs.h"  
#include "sim.h"
#include "lendoc.h"
#include "mainfrm.h"   

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_SIMULATION, OnViewSimulation)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SIMULATION, OnUpdateViewSimulation)
	ON_COMMAND(ID_VIEW_CONSOLE, OnViewConsole)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CONSOLE, OnUpdateViewConsole)
	ON_WM_INITMENUPOPUP()
	//}}AFX_MSG_MAP
	// Global help commands
#if (_MSC_VER <= 1299) /* MSVC 6 = 1200 */
	ON_COMMAND(ID_HELP_INDEX, CMDIFrameWnd::OnHelpIndex)
	ON_COMMAND(ID_HELP_USING, CMDIFrameWnd::OnHelpUsing)
	ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpIndex)
#else
	ON_COMMAND(ID_HELP_INDEX, DisplayHtmlHelp)
	ON_COMMAND(ID_HELP_USING, DisplayHtmlHelp)
	ON_COMMAND(ID_HELP, DisplayHtmlHelp)
	ON_COMMAND(ID_CONTEXT_HELP, DisplayHtmlHelp)
	ON_COMMAND(ID_DEFAULT_HELP, DisplayHtmlHelp)
#endif
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars

// toolbar buttons - IDs are command buttons
static UINT BASED_CODE buttons[] =
{
	// same order as in the bitmap 'toolbar.bmp'
	ID_FILE_NEW,
	ID_FILE_OPEN,
	ID_FILE_SAVE,    
	ID_FILE_PRINT,
		ID_SEPARATOR,
	ID_EDIT_CUT,
	ID_EDIT_COPY,
	ID_EDIT_PASTE,
		ID_SEPARATOR, 
	ID_SIMULATION_RUN, 
		ID_SEPARATOR,        
	ID_SIMULATION_STOPBOTH,
	ID_SIMULATION_STARTBOTH,
		ID_SEPARATOR,
	ID_SIMULATION_REPLAY_SCRIPT,      
	ID_SIMULATION_FFWD,
	ID_SIMULATION_PLAY_SCRIPT,
	ID_SIMULATION_RECORD,
	ID_SIMULATION_PAUSE_SCRIPT,
	ID_SIMULATION_STOP_SCRIPT,
		ID_SEPARATOR,
	ID_APP_ABOUT,
	ID_CONTEXT_HELP,
};

static UINT BASED_CODE indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadBitmap(IDR_MAINFRAME) ||
		!m_wndToolBar.SetButtons(buttons,
		  sizeof(buttons)/sizeof(UINT)))
	{
		TRACE("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE("Failed to create status bar\n");
		return -1;      // fail to create
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


void CMainFrame::SetStatusText(CString& sStatus)
{
    m_wndStatusBar.SetWindowText(sStatus);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	CMDIChildWnd* pActiveChild = MDIGetActive();
	CLenDoc* pDoc; 
	
	if ((pActiveChild == NULL)
	    || ((pDoc = (CLenDoc*) pActiveChild->GetActiveDocument()) == NULL)
	    || (wParam < IDM_OBJECT_DRAW_BASE)
	    || (wParam >= IDM_OBJECT_DRAW_BASE + 2 * MAX_OBJ_TYPES)) 
	{
		return CMDIFrameWnd::OnCommand(wParam, lParam);        // default
    }               
    else
    {
    	// Corresponds to Object...Draw...[objtype] or properties [objtype]
    	// menu command         
        
        
    	if (wParam < IDM_OBJECT_DRAW_BASE + MAX_OBJ_TYPES)
    	{
    		// Draw command
    		return pDoc->m_pSim->OnDrawCommand((int) (wParam - IDM_OBJECT_DRAW_BASE));
    	}
    	else
    	{
    		// Edit Properties command
    		return pDoc->m_pSim->OnPropertiesCommand((int)
    		                 (wParam - IDM_OBJECT_DRAW_BASE - MAX_OBJ_TYPES));
    	}
	}    	
}

void CMainFrame::OnViewSimulation()
{
	OnWindowNew(WT_SIM);	
}

void CMainFrame::OnUpdateViewSimulation(CCmdUI* pCmdUI)
{
	CMDIChildWnd* pActiveChild = MDIGetActive();
	
	CLenDoc* pDocument;
	if (pActiveChild == NULL ||
	  (pDocument = (CLenDoc*) pActiveChild->GetActiveDocument()) == NULL) 
	{
		// no active document
		pCmdUI->SetCheck(FALSE);
	}
	else
	{
		pCmdUI->SetCheck(pDocument->m_bSimShowing);
	}
}

void CMainFrame::OnViewConsole()
{
	OnWindowNew(WT_CONSOLE);	
}

void CMainFrame::OnUpdateViewConsole(CCmdUI* pCmdUI)
{
	CMDIChildWnd* pActiveChild = MDIGetActive();
	
	CLenDoc* pDocument;
	if (pActiveChild == NULL ||
	  (pDocument = (CLenDoc*) pActiveChild->GetActiveDocument()) == NULL) 
	{
		// no active document
		pCmdUI->SetCheck(FALSE);
	}
	else
	{
		pCmdUI->SetCheck(pDocument->m_bConsShowing);
	}
}                           

void CMainFrame::OnWindowNew(int winType)
{
	CLenApp* pApp = (CLenApp*) AfxGetApp();
	CMDIChildWnd* pActiveChild = MDIGetActive();
	
	CLenDoc* pDocument;
	if (pActiveChild == NULL ||
	  (pDocument = (CLenDoc*) pActiveChild->GetActiveDocument()) == NULL)
	{
		TRACE0("Warning: No active document for WindowNew command\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return;     // command failed
	}
	
	CMultiDocTemplate* pTemplate = NULL; 

	if ((WT_CONSOLE == winType) && (!pDocument->m_bConsShowing))
	{                                                                   
		pTemplate = pApp->m_pConsTemplate;
	}
	else if ((WT_SIM == winType) && (!pDocument->m_bSimShowing))   
	{
		pTemplate = pApp->m_pSimTemplate;  
	}
	else if ((WT_CONSOLE == winType) && (pDocument->m_bConsShowing))   
	{          
		pDocument->m_pConsWnd->MDIDestroy();
	}
	else if ((WT_SIM == winType) && (pDocument->m_bSimShowing))   
	{          
		pDocument->m_pSimWnd->MDIDestroy();
	}
	else
	{
		// do nothing 
	}
	
	if (NULL != pTemplate)
	{
		ASSERT_VALID(pTemplate);
		CFrameWnd* pFrame = pTemplate->CreateNewFrame(pDocument, pActiveChild);
		if (pFrame == NULL)
		{
			TRACE0("Warning: failed to create new frame\n");
			return;     // command failed
		}    
	
		pTemplate->InitialUpdateFrame(pFrame, pDocument);
		
	}	
}

void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CMDIFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	
	CLenApp* pApp = (CLenApp*) AfxGetApp();
	CMDIChildWnd* pActiveChild = MDIGetActive();
		
	CLenDoc* pDocument; 
	CSim* pSim;
	if (NULL != pActiveChild)
	{
		pDocument = (CLenDoc*) pActiveChild->GetActiveDocument();
	 	pSim = pDocument->m_pSim;
	}
	else
	{
		pDocument = NULL;
		pSim = NULL;
	}
	
	CObjType* pType;
	CString name;    
			
	// Gives us a chance to change lists of current object types
	// for current document before the menu becomes visible.

	char item0[20];
	char item1[20];


	// find names of first two menu items to confirm Object menu
	pPopupMenu->GetMenuString(0, item0, 20, MF_BYPOSITION);
	pPopupMenu->GetMenuString(1, item1, 20, MF_BYPOSITION);
		
	int objmenu =  ((CString("&Draw") == item0)
	                && (CString("Type &Properties") == item1));

	if (pActiveChild == NULL || !objmenu)
	{                  
		// No active document, or another menu, so give up
	}
	else
	{
		// We have an active document and the Object menu
		// has been clicked. Ensure it is up-to-date for this sim.
			
		// build up submenus to insert (can't use same one twice
		// because ID's of items need to be unique)		
		CMenu popupObjTypeMenu[2];    
		popupObjTypeMenu[0].CreateMenu();
		popupObjTypeMenu[1].CreateMenu();
				
		int lastTypeIndex = pSim->m_typeArray.GetUpperBound();
		for (int index = 0; index <= lastTypeIndex; index++)
		{
			pType = (CObjType*) pSim->m_typeArray[index];  
			name = pType->m_sName;
			popupObjTypeMenu[0].AppendMenu(MF_STRING, IDM_OBJECT_DRAW_BASE + index, name);	
			popupObjTypeMenu[1].AppendMenu(MF_STRING, IDM_OBJECT_DRAW_BASE + MAX_OBJ_TYPES + index, name);
        }
				
		for (int subMenu = 0; subMenu <= 1; subMenu++)
		{
			// Loop over Draw and Type Properties submenus  
			char szString[256];     // don't change the string

			pPopupMenu->GetMenuString(subMenu,
			                          szString,
			                          sizeof(szString),
			                          MF_BYPOSITION);
			pPopupMenu->ModifyMenu(subMenu, 
			                       MF_BYPOSITION | MF_POPUP,
			                       (UINT)popupObjTypeMenu[subMenu].m_hMenu,
			                       szString);
		}      
				
		//SetMenu(pPopupMenu); 
				
		popupObjTypeMenu[0].Detach();
		popupObjTypeMenu[1].Detach();

	}   // end of if (doc exists)

 
	
	// If this is one of our dynamic popups about to appear, want to
	// enable all of the items.
	// Used to do this by comparing the address of the menu we are
	// dealing with (pPopupMenu) with that of the Object...Draw and
	// Object...Type Properties menus as found by searching from the
	// menus available from the main window. But this doesn't work
	// for floating copy of Object menu, so instead check if first
	// object name is same as expected. NB Could get into trouble if
	// someone chooses an object type name like "Paste"... 
   
	if ((NULL != pSim ) && (pSim->m_typeArray.GetUpperBound() > -1)
        && (SM_SIM_RUNNING != pSim->m_nSimState))
	{
		pType = (CObjType*) pSim->m_typeArray[0];
        
        pPopupMenu->GetMenuString(0, buff, 64, MF_BYPOSITION);
        
    	if (pType->m_sName == buff)
    	{
			for (int menuItem = 0; 
			  menuItem <= (int) pPopupMenu->GetMenuItemCount(); menuItem++)	
			{     
				pPopupMenu->EnableMenuItem(menuItem, MF_ENABLED | MF_BYPOSITION);	
			}	
		}
	}

}

void CMainFrame::DisplayHtmlHelp()
{
#if (_MSC_VER <= 1299) /* MSVC 6 = 1200 */
	AfxGetApp()->WinHelp(0);
#else
	::HtmlHelp(this->m_hWnd, "Lennard.chm", HH_DISPLAY_TOC, NULL);
#endif
}
