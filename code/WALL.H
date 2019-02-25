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

// constants for wall types
#define WT_USE_SYS_TEMP         0
#define WT_USE_SPECIFIC_TEMP    1
#define WT_THERMAL              0
#define WT_ELASTIC              1

// wall drawing constants
#define DW_DRAG_THRESH          3

class CObjTypeWall : public CObjType
{                    
public:       
	CObjTypeWall(); 

	virtual int WriteState(CString& workingLine);
	virtual int GetStateParam(int command);
	virtual CMD_ENTRY* GetCmdList(); 
	virtual int AcceptCommandLine(CString& remainingLine, CString& errString);
	virtual void Delete();
	
	virtual void DrawAllSObjs(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2);
	virtual void UserDrawing(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2);
	virtual void OnLButtonUp(); 

	virtual void PropertiesCommand();
    virtual void ContributeNumerics();
    
	static CMD_ENTRY m_celCObjTypeWallCommands[MAX_COMMANDS_PER_OBJ];
	
	unsigned char m_bLineRed;
	unsigned char m_bLineGreen;
	unsigned char m_bLineBlue;
	int m_nLineThick;
	double m_dTemperature;	  // special temperature if thermal and non-sys       
	CNumeric m_numTemperature;
	int		m_nTemperatureType;
	int		m_nMolIntType;
};

class CSObjWall : public CSObj
{   
public:
	double m_dX1;
	double m_dY1;
	double m_dX2;
	double m_dY2;  
	
	// useful also to have direction cosines, length, normal vector, offset from O:
	double m_dCx;                                                                  
	double m_dCy;
	double m_dNx;
	double m_dNy;
	double m_dLength;     
	double m_dDirection; 
	double m_dNormDirection;

	// need to keep track of what valid and what not:
	union 
	{
		unsigned all;         // all the flags in one go
		struct               // bf for bit flag
		{
		    unsigned x1:        1; 
		    unsigned y1:        1;
		    unsigned x2:        1;
		    unsigned y2:        1;
		    unsigned cosines:   1;
		    unsigned normal:    1;
		    unsigned length:    1; 
		    unsigned direction: 1;
		    unsigned ndirn:     1;
		} bf; 
	}
	m_fsValid;
	
	virtual int GetStateParam(int command);
	virtual CMD_ENTRY* GetCmdList(); 
	virtual int AcceptCommandLine(CString& remainingLine, CString& errString);

	virtual int HitTest(double dXLeft, double dYBottom, 
	                           double dXRight, double dYTop);  
	                           
	virtual void ObjectProperties();	                            
	                           
	virtual void CreateTracker();  // overridden to do line not box
	virtual void DrawTracker(CDC* pDC);  
	virtual void HandleMove(double dDeltaX, double dDeltaY);	                           
	virtual void OnMoveOrResize(double dNewXLeft, double dNewYTop,
	                            double dNewXRight, double dNewYBottom,
	                            double dOldXLeft, double dOldYTop,
	                            int nResized);
	                            	                           
	static CMD_ENTRY m_celCSObjWallCommands[MAX_COMMANDS_PER_OBJ];  
	
};                                                       
