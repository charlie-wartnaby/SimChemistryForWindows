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

class CSObjTextbox;

class CObjTypeTextbox : public CObjType
{                    
public:       
	int m_nFontSize;
	CString m_sFontName; 
	CString m_sUserTypedText;

	CObjTypeTextbox();
	void DrawTextFromString(CDC* pDC, CString& string, int xLeft, int yTop);
	void DrawText(CDC* pDC, CNumLink* pFirstLink, int xLeft, int yTop, CSObjTextbox* pTextbox);
	CNumLink* ParseTextToLinks(CString& string);   
	
	void EscapeBadChars(CString& string);  // replace newlines etc with escape sequences
    void ExpandBadChars(CString& string);  // expands newlines back again
    static int CalcTargetFontSize(double dScaleFactor, int nFontSize); // allows for screen scaling
	virtual int GetStateParam(int command);
	virtual CMD_ENTRY* GetCmdList(); 
	virtual int AcceptCommandLine(CString& remainingLine, CString& errString);
	virtual int DrawCommand();                                
	virtual void PropertiesCommand();
	virtual void DrawAllSObjs(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2);
	virtual void UserDrawing(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2);
	
	CPoint DrawCrosshair(CDC* pDC); // overridden to do special textbox positioning cursor 
	virtual void OnLButtonUp();  // overridden to issue command to create textbox 
	
	virtual void Prepare();    // reference numeric quantities when ready before sim start
	
	static CMD_ENTRY m_celCObjTypeTextboxCommands[MAX_COMMANDS_PER_OBJ];
};

class CSObjTextbox : public CSObj
{   
public:    
	CSObjTextbox();
	virtual ~CSObjTextbox();      
	
	void DeleteLinks();  // deletes NumLink objects it owns
		
	virtual int GetStateParam(int command);
	virtual CMD_ENTRY* GetCmdList(); 
	virtual int AcceptCommandLine(CString& remainingLine, CString& errString);   

	virtual int HitTest(double dXLeft, double dYBottom, 
	                           double dXRight, double dYTop); 
	                           
	virtual void CreateTracker();  // overridden to disallow resizing 
	virtual void DrawTracker(CDC* pDC);  
	virtual void HandleMove(double dDeltaX, double dDeltaY);
	                           
	virtual void ObjectProperties();
	                           	
	double m_dXLeft;
	double m_dYTop;   
	
	int m_nXLeftLogical;
	int m_nYTopLogical;
	int m_nMaxXLogical;
	int m_nMinYLogical;

#ifdef _DEBUG
	int m_nDrawn;
#endif

	CString m_sText;     
	CNumLink* m_pFirstNumLink;
	
	// need to keep track of what valid and what not:
	union 
	{
		unsigned all;         // all the flags in one go
		struct               // bf for bit flag
		{
		    unsigned xleft:   1; 
		    unsigned ytop:    1;
		    unsigned text:    1;
		} bf; 
	}
	m_fsValid;


	static CMD_ENTRY m_celCSObjTextboxCommands[MAX_COMMANDS_PER_OBJ];  
	
};                                                       
