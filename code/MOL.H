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

// constants for drawing molecules
#define DM_SYSTEM_THERMAL       0
#define DM_SPECIFIC_THERMAL     1
#define DM_FIXED_SPEED          2
#define DM_DIRECTION_RANDOM     0
#define DM_DIRECTION_FIXED      1

// max radius of mol on screen
#define MAX_SCREEN_RADIUS    2000                           

#define MAX_COMMANDS_MOL       20


class CSObjMol;  // forward ref reqd as type in CObjTypeMol

class CObjTypeMol : public CObjType
{                    
public:       
	CObjTypeMol(); 

	virtual double PrepStep();
    virtual void MoveSObjs();	
 	virtual void ApplyAccels();
 	
 	virtual void ContributeNumerics();
	virtual int GetStateParam(int command);
	virtual CMD_ENTRY* GetCmdList(); 
	virtual int AcceptCommandLine(CString& remainingLine, CString& errString);
	virtual void Initialise();
	virtual void DrawAllSObjs(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2);
	virtual void UserDrawing(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2);
	virtual void OnLButtonUp(); 
	void GetMolFillCoords(CPtrList* ptrList, double left, double bottom,
	                         double right, double top, double radius);
	long m_lNumFillMols;  // count number to fill at drawing stage
	
	virtual int DrawCommand();      
	virtual void PropertiesCommand();                              
	
	static CMD_ENTRY m_celCObjTypeMolCommands[MAX_COMMANDS_MOL];
	
	int m_nMolScreenRadius; 
	int m_nMolScreenDiameter;
	int m_nScreenLineThick;   
	double m_dPackingStandoff;
	
	double m_dMass;     
	CNumeric m_numMass;
	double m_dHSRadius;       
	CNumeric m_numRadius;
    double m_dFormationEnergy; // kJ/mol
    CNumeric m_numFormationEnergy;
	double m_dLineThick;  
	unsigned char m_bLineRed;
	unsigned char m_bLineGreen;
	unsigned char m_bLineBlue;
	unsigned char m_bFillRed;
	unsigned char m_bFillGreen;
	unsigned char m_bFillBlue;
                                      
	// variables from draw molecule dialogue:                                      
	int m_nSpeedType;
	int m_nDirectionType;
	double m_dDirectionAngle;
	double m_dFixedSpeedValue;
	double m_dThermalTemperatureValue;	
	
	int CmdMass();
	int CmdHSRadius();   
	int CmdLineRed();
	int CmdLineGreen();
	int CmdLineBlue();
	int CmdFillRed();
	int CmdFillGreen();
	int CmdFillBlue();      
	int CmdLineThick();     
	
	// DoCmdFillMol fills a rectangle with close-packed molecules. It tries to
	// assign ids to those molecules within the range specified if they fit,
	// otherwise it makes a new range bigger than m_lLastUsedId:
	int DoCmdFillMol(CString& remainingLine, CString& errString,
	                  long& lFirstId, long& lLastId, int& nIdRangeOK);

	// used to auto-create new mols by monitors (they assign ID):	                  
	CSObjMol* CreateNewMol(double dX, double dY, double dTemperature, long lId);
    // and by reactive collisions (we assign ID):
    CSObjMol* CreateNewMol(double dX, double dY, double dSpeed, double dDirection);
    // common version if know speed, direction and ID; returns ptr to new mol
    // or NULL if fails:
    CSObjMol* CreateNewMol(double dX, double dY,  double dSpeed, double dDirection, long lId);

    long m_lNextIDForReactionProduct;
};

class CSObjMol : public CSObj
{   
public:                
    CSObjMol(); 
	
	// x, y, vel components -- primary reference
	double m_dX;
	double m_dY;
	double m_dVx;
	double m_dVy; 

	
	double m_dVSquared;  // v^2
		
	double m_dFx;
	double m_dFy;
	double m_dAx;
	double m_dAy;
	
	double m_dPE; // potential energy from interaction (not formation or gravity)
	double m_dVirial;   // used in pressure calc for LJ ints
	double m_dImpulseSum;  // used in pressure calc for hard ints	
	
	// sometimes handy to use angle or cosines intead:	
	double m_dDirection;   // in radians from east    
	double m_dCx;
	double m_dCy;
	double m_dSpeed;
	
	// need to keep track of what valid and what not:
	union 
	{
		unsigned all;         // all the flags in one go
		struct               // bf for bit flag
		{
		    unsigned x : 1; 
		    unsigned y : 1;
		    unsigned vx: 1;
		    unsigned vy: 1;
		    unsigned direction: 1;
		    unsigned cosines: 1;
		    unsigned speed: 1; 
		} bf; 
	}
	m_fsValid;     
	
	virtual int GetStateParam(int command);
	virtual CMD_ENTRY* GetCmdList(); 
	virtual int AcceptCommandLine(CString& remainingLine, CString& errString);

	virtual int HitTest(double dXLeft, double dYBottom, 
	                           double dXRight, double dYTop);

	virtual void ObjectProperties();                                         
	
	virtual void CreateTracker();  // overridden to disallow resizing of mols
	virtual void DrawTracker(CDC* pDC);  
	virtual void HandleMove(double dDeltaX, double dDeltaY);
		  	
	static CMD_ENTRY m_celCSObjMolCommands[MAX_COMMANDS_MOL];  
	
	int CmdX();
	int CmdY();
	int CmdVx();
	int CmdVy();     
	int CmdSpeedFixed();
	int CmdSpeedThermal();
	int CmdSpeedSysThermal();
	int CmdDirectionFixed();
	int CmdDirectionRandom();

	void ClipSpeed();
};  

