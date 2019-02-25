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

// lennard.cpp : Defines the class behaviors for the application.
//              

#include "stdafx.h"
#include "simobjs.h"    
#include "sim.h"
#include "lennard.h"
#include "lendoc.h"
#include "mainfrm.h"  
#include "consfrm.h"    
#include "simfrm.h"
#include "consvw.h"    
#include "simvw.h"
#include "firstdlg.h" 
#include "regdlg.h"            
#include "time.h"   


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


char buff[GLOBAL_BUFF_SIZE];  // used everywhere for string manipulation, global



/////////////////////////////////////////////////////////////////////////////
// CLenApp

BEGIN_MESSAGE_MAP(CLenApp, CWinApp)
	//{{AFX_MSG_MAP(CLenApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLenApp construction

CLenApp::CLenApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CLenApp object

CLenApp theApp;  // was NEAR

/////////////////////////////////////////////////////////////////////////////
// CLenApp initialization

BOOL CLenApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	SetDialogBkColor();        // Set dialog background color to gray
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	m_nShowFirstTimeDlg = GetProfileInt("First-Time Dialogue", "Show", 1);
    m_nAutoShowSelnResolutionDlg  = GetProfileInt("Resolve Multiple Selection Dialogue", "Show", 1);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_LENNTYPE,
		RUNTIME_CLASS(CLenDoc),
		RUNTIME_CLASS(CSimFrm),        
		RUNTIME_CLASS(CSimView));
	AddDocTemplate(pDocTemplate);

    // Create doc templates for creating new views on demand. Note
    // that we have to delete these manually (in ExitInstance).                                                             
	m_pConsTemplate = new CMultiDocTemplate(
		IDR_LENNTYPE,
		RUNTIME_CLASS(CLenDoc),
		RUNTIME_CLASS(CConsFrame),        // console splitter frame
		RUNTIME_CLASS(CConsoleView));
		
	m_pSimTemplate = new CMultiDocTemplate(
		IDR_LENNTYPE,
		RUNTIME_CLASS(CLenDoc),
		RUNTIME_CLASS(CSimFrm),        // standard MDI child frame
		RUNTIME_CLASS(CSimView));

	// Register file types with Windows
	RegisterShellFileTypes(); 
	
	// Allow double-clicked docs to open
	EnableShellOpen();     
	
	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
    
    m_pMainWnd->DragAcceptFiles(); // allow drag & drop file open
    
    
	// For some reason I don't understand, if user double-clicks
	// document to open us, opens two copies unless open this dialogue
	// first:
	OnAppAbout();

    int nFileOpened;
    
	if (m_lpCmdLine[0] == '\0')
	{
		// create a new (empty) document
		OnFileNew();      
		nFileOpened = FALSE;
	}   
    else if ((m_lpCmdLine[0] == '-' || m_lpCmdLine[0] == '/') &&
        (m_lpCmdLine[1] == 'e' || m_lpCmdLine[1] == 'E'))
    {
        // program launched embedded - wait for DDE or OLE open
        nFileOpened = TRUE;
    }
    else
    {
        // open an existing document
        OpenDocumentFile(m_lpCmdLine);
        nFileOpened = TRUE;
    }

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	
	if (m_nShowFirstTimeDlg)
	{
		CFirstTimeDlg dlg;
		dlg.m_nAgain = TRUE;
		
		dlg.DoModal();
		
		m_nShowFirstTimeDlg = dlg.m_nAgain;
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg(); 
	~CAboutDlg();
	
	CBrush* m_pRedBrush; // for highlighting if trial expired

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual  BOOL OnInitDialog();
	
	CFont* m_pTitleFont;
	
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
	
	m_pTitleFont = new CFont;
	
	m_pRedBrush = new CBrush(RGB(255, 0, 0));
}

CAboutDlg::~CAboutDlg()
{
	delete m_pTitleFont;               
	delete m_pRedBrush;
}

BOOL CAboutDlg::OnInitDialog()      
{               
	CDialog::OnInitDialog(); 
	
	
	// set title text to nice font
	
	CEdit* pText = (CEdit*) GetDlgItem(IDC_PROG_TITLE);
	
	delete m_pTitleFont;
	m_pTitleFont = new CFont; // otherwise debug GDI code asserts
	
	m_pTitleFont->CreateFont(-28, // int nHeight,
	                           0, // int nWidth,
	                           0, // int nEscapement, 
                               0, // int nOrientation, 
                               FW_BOLD, // int nWeight, 
                               FALSE, // BYTE bItalic, 
                               FALSE, // BYTE bUnderline, 
                               0, // BYTE cStrikeOut,
                               ANSI_CHARSET, // BYTE nCharSet, 
                               OUT_DEFAULT_PRECIS, // BYTE nOutPrecision, 
                               CLIP_DEFAULT_PRECIS, // BYTE nClipPrecision, 
                               DEFAULT_QUALITY, // BYTE nQuality, 
                               DEFAULT_PITCH, // BYTE nPitchAndFamily,  
                               "Times New Roman"); // LPCSTR lpszFacename
	
	pText->SetFont(m_pTitleFont);
	                                                   
		                                                   
	pText = (CEdit*) GetDlgItem(IDC_BUILD_DATE);
	
	CString sBuildDate("Program built at ");
	sBuildDate += __TIME__;
	sBuildDate += " on ";
	sBuildDate += __DATE__;
	
	pText->SetWindowText(sBuildDate);

    CString sVerText("Version ");
    sVerText += (const char*) SIMCHEMISTRY_VER_STRING;
    GetDlgItem(IDC_VERSION)->SetWindowText(sVerText);
	
	return TRUE;
}                                


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CLenApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	
	aboutDlg.DoModal();
} 

int CLenApp::ExitInstance()
{    	
	delete m_pConsTemplate;
	delete m_pSimTemplate;
	
	WriteProfileInt("First-Time Dialogue", "Show", m_nShowFirstTimeDlg);
 	WriteProfileInt("Resolve Multiple Selection Dialogue", "Show", m_nAutoShowSelnResolutionDlg);

	return CWinApp::ExitInstance(); // saves profile settings
}                                   

BOOL CLenApp::CallBaseClassIdle(LONG lCount)
{
    return CWinApp::OnIdle(lCount);
}

BOOL CLenApp::OnIdle(LONG lCount)
{
	static int nFirstTime = TRUE;
	
	static DWORD dwTimeToSaveRegData;
	
//	if (nFirstTime)
//	{
//		nFirstTime = FALSE;
//		dwTimeToSaveRegData = GetTickCount() + TIME_BETWEEN_REG_SAVES_MS;
//	}
	   

	//BOOL needmore = CWinApp::OnIdle(lCount);  // first call the base class version to 
	                                          // let Windows update menus and the like
    BOOL needmore = CallBaseClassIdle(lCount);

	CMDIChildWnd* pActiveChild = ((CMainFrame*) m_pMainWnd)->MDIGetActive();
	CLenDoc* pDoc; 
	if ((pActiveChild == NULL)
	    || ((pDoc = (CLenDoc*) pActiveChild->GetActiveDocument()) == NULL)) 
	{
		// No active document, do nothing
	}
	else
	{
		CSim* pSim = pDoc->m_pSim;
		needmore = needmore || pSim->OnIdle();
	}

//	if (GetTickCount() > dwTimeToSaveRegData)
//	{
//		SaveRegistryData();            
//		dwTimeToSaveRegData = GetTickCount() + TIME_BETWEEN_REG_SAVES_MS;
//	} 
	
	return needmore;
}




/////////////////////////////////////////////////////////////////////////////
// CLenApp commands


void CLenApp::OnUpdateFileNew(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);	
}

void CLenApp::OnUpdateFileOpen(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}
