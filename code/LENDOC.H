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

// lendoc.h : interface of the CLenDoc class
//
/////////////////////////////////////////////////////////////////////////////

class CLenDoc : public CDocument
{
protected: // create from serialization only
	CLenDoc();
	DECLARE_DYNCREATE(CLenDoc)

// Attributes
public:
	CSim* m_pSim;    
	BOOL m_bConsShowing;
	BOOL m_bSimShowing;
	CMDIChildWnd* m_pConsWnd;
	CMDIChildWnd* m_pSimWnd;  
		
// Operations
public:

// Implementation
public:
	virtual ~CLenDoc();
	virtual BOOL OnOpenDocument(const char* pszPathName);
	virtual BOOL OnSaveDocument(const char* pszPathName);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnNewDocument();
	
	void OnSimulationStop(); // no longer a message handler but still used

// Generated message map functions
protected:
	//{{AFX_MSG(CLenDoc)
	afx_msg void OnObjectNewtypeofMolecule();
	afx_msg void OnObjectNewtypeofWall();
	afx_msg void OnObjectInteractions();
	afx_msg void OnUpdateObjectInteractions(CCmdUI* pCmdUI);
	afx_msg void OnSimulationRun();
	afx_msg void OnUpdateSimulationRun(CCmdUI* pCmdUI);
	afx_msg void OnObjectNewtypeofPiston();
	afx_msg void OnSimulationReplayScript();
	afx_msg void OnUpdateSimulationReplayScript(CCmdUI* pCmdUI);
	afx_msg void OnObjectDelete();
	afx_msg void OnUpdateObjectDelete(CCmdUI* pCmdUI);
	afx_msg void OnSimulationReset();
	afx_msg void OnObjectNewtypeofTextbox();
	afx_msg void OnObjectNewtypeofMonitor();
	afx_msg void OnSimulationSystemproperties();
	afx_msg void OnUpdateSimulationReset(CCmdUI* pCmdUI);
	afx_msg void OnUpdateObjectNewtypeofMolecule(CCmdUI* pCmdUI);
	afx_msg void OnUpdateObjectNewtypeofMonitor(CCmdUI* pCmdUI);
	afx_msg void OnUpdateObjectNewtypeofPiston(CCmdUI* pCmdUI);
	afx_msg void OnUpdateObjectNewtypeofTextbox(CCmdUI* pCmdUI);
	afx_msg void OnUpdateObjectNewtypeofWall(CCmdUI* pCmdUI);
	afx_msg void OnUpdateObjectTypeproperties(CCmdUI* pCmdUI);
	afx_msg void OnObjectDeleteselected();
	afx_msg void OnUpdateObjectDeleteselected(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnSimulationInsert();
	afx_msg void OnUpdateSimulationInsert(CCmdUI* pCmdUI);
	afx_msg void OnSimulationPauseScript();
	afx_msg void OnUpdateSimulationPauseScript(CCmdUI* pCmdUI);
	afx_msg void OnSimulationPlayScript();
	afx_msg void OnUpdateSimulationPlayScript(CCmdUI* pCmdUI);
	afx_msg void OnSimulationStopScript();
	afx_msg void OnUpdateSimulationStopScript(CCmdUI* pCmdUI);
	afx_msg void OnSimulationFfwd();
	afx_msg void OnUpdateSimulationFfwd(CCmdUI* pCmdUI);
	afx_msg void OnSimulationRecord();
	afx_msg void OnUpdateSimulationRecord(CCmdUI* pCmdUI);
	afx_msg void OnObjectDeletealloftype();
	afx_msg void OnUpdateObjectDeletealloftype(CCmdUI* pCmdUI);
	afx_msg void OnObjectDeleteAll();
	afx_msg void OnUpdateObjectDeleteAll(CCmdUI* pCmdUI);
	afx_msg void OnObjectNewtypeofGraph();
	afx_msg void OnUpdateObjectNewtypeofGraph(CCmdUI* pCmdUI);
	afx_msg void OnObjectProperties();
	afx_msg void OnUpdateObjectProperties(CCmdUI* pCmdUI);
	afx_msg void OnObjectGridsettings();
	afx_msg void OnObjectNewtypeofSlider();
	afx_msg void OnUpdateObjectNewtypeofSlider(CCmdUI* pCmdUI);
	afx_msg void OnSimulationStartboth();
	afx_msg void OnUpdateSimulationStartboth(CCmdUI* pCmdUI);
	afx_msg void OnSimulationStopboth();
	afx_msg void OnUpdateSimulationStopboth(CCmdUI* pCmdUI);
	afx_msg void OnSimulationSizeScaling();
	afx_msg void OnObjectResolvemultipleselection();
	afx_msg void OnUpdateObjectResolvemultipleselection(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
