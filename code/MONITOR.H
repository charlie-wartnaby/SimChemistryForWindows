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

// enumeration for what sort of controls applies to molecules:

#define MCNTRL_NONE           0
#define MCNTRL_DESTROY        1
#define MCNTRL_FIX_DENS_TEMP  2
#define MCNTRL_HEATER         3   

#define MAX_INSERTION_ATTEMPTS 20

#define MON_MAX_ENERGY_TO_GIVE 50000  // J/mol

#define MAX_SMON_COMMANDS_PER_OBJ  15

#define MON_OUTLINE_BITMAP_SIZE 8 // pow of 2 between 4 and 8   

#define MON_OBJTYPE_CMDS 12

class CObjTypeMol; 
class CMonMolNP;

class CObjTypeMonitor : public CObjType
{                    
public:       
    CObjTypeMonitor(); 
    ~CObjTypeMonitor();
    
	virtual int GetStateParam(int command);
	virtual int WriteState(CString& workingLine);
	virtual CMD_ENTRY* GetCmdList(); 
	virtual int AcceptCommandLine(CString& remainingLine, CString& errString);
	
	virtual void DrawAllSObjs(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2);
	virtual void UserDrawing(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2);
	virtual void OnLButtonUp();
	
	virtual void PropertiesCommand();   
	virtual void ContributeNumerics();  
	virtual void Initialise();
	
	static CMD_ENTRY m_celCObjTypeMonitorCommands[MON_OBJTYPE_CMDS];
	
	int  m_nControlType;           // none / fix dens&T / destructor / heater	

	// need an array maintained of mol type vs index number, for indexing
	// into list of stats for each mol type ref'd by a monitor:
	CPtrArray m_molTypesRefd;
	int m_nMolTypesRefd;                                       
	
	int FindStatIndex(CObjTypeMol* pMolType);  // get common array index for this type
	int FindStatIndex(CString& sMolType);
	
	virtual void Prepare();  // gets called by sim prior to running, get chance to
	                 // initialise list of mol types etc 
	                 
	void UpdateQuantities(); // called at end of each timestep
	
	       
	// The following functions, used to draw monitors on screen, are static because
	// they are needed by the monitor type properties dialogue before any monitor
	// types exist:
	
	static CBrush* CreateOutlineBrush(CDC* pDC, COLORREF cColour, int nOffset);
	static CFont* CreateLabelFont();
	
	// The following function draws one monitor, and is used both by the simulation
	// and by the dialogue box used to set monitor properties. It returns a
	// CSize object specifying where the bottom-right corner of the label text
	// is relative to the top-left corner; that is required for hit-testing
	// mouse clicks, when determining whether a monitor has been selected.
	static CSize DrawMonitor(CDC* pDC,
	                         int nXLeft, // logical co-ords
	                         int nYTop,
	                         int nXRight,
	                         int nYBottom,  
	                         int nThickness,
	                         int nHatch,
	                         int nLabel,
	                         const char* sLabel,
	                         CBrush* pOutlineBrush,
	                         CFont* pLabelFont,
	                         COLORREF cColour);               
	                        
	CBrush* m_ppOutlineBrush[MON_OUTLINE_BITMAP_SIZE]; // will have an array for animation
	CFont* m_pLabelFont;	         
	
	COLORREF m_cColour;
	int m_nLineThick;
	int m_nHatch;
	int m_nLabel;               
}; 

class CMolTypeStats  // one of these for each mol type monitor looks at
{                          
public:
	CMolTypeStats();  // zeroes totals etc and sets default (null) controls
	~CMolTypeStats();  // deletes attached list of NP objs  
	
	void Initialise();  // zeroes totals etc prior to run  
	void ContributeNumerics(CSim* pSim, CObjType* pType, long lId);  // though not actually a CSimObj
	
	void SetTemperature(double dNewTemperature); // modifies mol temps
	
    // called if parent interaction for NPs we own is being destroyed
    // so we can mark them as 'orphaned' from that int:
    void NoteIntDying();


	CObjTypeMol* m_pMolType;
	
	long m_lLastMolIdUsed;  // each mon gets to allocate own mol ids
	
	long m_lNumMolsInside;    
	double m_dNumMolsInside;  // double copy for CNumeric 
	CNumeric m_numNumMolsInside;           
	
	long m_lCountExitsTop;     // to measure flux through monitor,
	long m_lCountExitsBottom;  // count molecules as they leave through
	long m_lCountExitsLeft;    // each edge
	long m_lCountExitsRight;    
	double m_dCountExitsTop;
	double m_dCountExitsBottom;
	double m_dCountExitsLeft;
	double m_dCountExitsRight;
	CNumeric m_numCountExitsTop;
	CNumeric m_numCountExitsBottom;
	CNumeric m_numCountExitsLeft;
	CNumeric m_numCountExitsRight;
	
	double m_dTemperature;
	CNumeric m_numTemperature;

	double m_dTemperatureS;
	CNumeric m_numTemperatureS;

	double m_dAvgKE;
	CNumeric m_numAvgKE;  

	double m_dAvgKES;
	CNumeric m_numAvgKES;  

	double m_dAvgPE;
	CNumeric m_numAvgPE;   
	
	double m_dAvgPES;
	CNumeric m_numAvgPES;   

	double m_dTotalEnergy;
	CNumeric m_numTotalEnergy;            
	
	double m_dTotalEnergyS;
	CNumeric m_numTotalEnergyS;            
	
	double m_dPartialPressure; 
	CNumeric m_numPartialPressure;

	double m_dPartialPressureS; 
	CNumeric m_numPartialPressureS;

	double m_dDensity;        // number density in (nm2)^-1
	CNumeric m_numDensity;

	double m_dDensityS;     // smoothed version
	CNumeric m_numDensityS;         
	
	int m_nTemperatureFixed;
	double m_dFixedTemperature;   
	CNumeric m_numFixedTemperature;
	
	int m_nDensityFixed;
	double m_dFixedDensity;       // mols/nm2
	CNumeric m_numFixedDensity;
	
	double m_dMolsToCreate;   

	CMonMolNP* m_pFirstMonMolNP;  // maintains linked list of referenced mols 
	CMonMolNP* m_pLastMonMolNP;   
};

class CSObjMonitor : public CSObj
{   
public:                       
	CSObjMonitor();
	virtual ~CSObjMonitor();
	
	virtual int GetStateParam(int command);
	virtual int WriteState(CString& workingLine);
	virtual CMD_ENTRY* GetCmdList(); 
	virtual int AcceptCommandLine(CString& remainingLine, CString& errString);
	virtual void ContributeNumerics();

	virtual int HitTest(double dXLeft, double dYBottom, 
	                           double dXRight, double dYTop);

	virtual void ObjectProperties();   
		                           
	virtual void DrawTracker(CDC* pDC);  
	virtual void HandleMove(double dDeltaX, double dDeltaY);
	virtual void OnMoveOrResize(double dNewXLeft, double dNewYTop,
	                            double dNewXRight, double dNewYBottom,
	                            double dOldXLeft, double dOldYTop,
	                            int nResized);
		                           
	                           	
	CPtrList m_destroyList;  // of mols to kill if destructor

	static CMD_ENTRY m_celCSObjMonitorCommands[MAX_SMON_COMMANDS_PER_OBJ];  
	
	CPtrArray m_molStatsList;  // list of totals etc for each mol type      
	int m_nMolStatsBuilt;  // whether list initialised
	void InitialiseStatsArray(int nMolTypes);  // zeroes totals etc
	void BuildStatsArray(int nMolTypes);  // creates empty stats list                 
	void UpdateQuantities(); // called at end of each timestep
	void InsertNewMol(CObjTypeMol* pMolType, double dTemperature);
	
    // If interaction referencing this monitor type is destroyed, tells
    // us so that we can mark any NPs privately owned by us as being
    // orphaned from their parent interaction:
    void NoteIntDying(int nStatIndex);

	double m_dXLeft;
	double m_dYTop;
	double m_dXRight;
	double m_dYBottom;     
	
	double m_dTextCornerXOffset;  // these from top-left corner, so we
	double m_dTextCornerYOffset;  // can include text in mouse hit-test   
	
	int m_nDrawn;  // those offsets and thickness not valid until drawn
    
    double m_dHeaterPower;  // in kJ/mol/ns, per mol of monitors that is
    CNumeric m_numHeaterPower;
    double m_dHeaterOn;          
    CNumeric m_numHeaterOn;
	double m_dEnergyToGive;  // in J/mol
	void DoHeating();                
	
	int m_nOutline;  // index into array of outline brushes maintained
	                 // by obj type to do shimmying border
	    
	long m_lUniqueMonId; // in all of history, used to allocate mol ids
	                     // made by this monitor	
	void InitialiseMolIds();  // in stats array
	                     
	double m_dArea;  // in nm2    
	
	int m_nFirstPass;  // whether averages uninitialised
	
	long m_lTotalNumMols;   // of all types referenced
	double m_dTotalNumMols;
	CNumeric m_numTotalNumMols;
	
	double m_dTotalTemperature; 
	CNumeric m_numTotalTemperature;

	double m_dTotalTemperatureS; // smoothed out version
	CNumeric m_numTotalTemperatureS;         
	
	double m_dAvgKE;  // in J/mol
	CNumeric m_numAvgKE;     

	double m_dAvgKES;  // in J/mol
	CNumeric m_numAvgKES;     
	
	double m_dAvgPE;  // of all mols refd, J/mol
	CNumeric m_numAvgPE;

	double m_dAvgPES;  // of all mols refd, J/mol
	CNumeric m_numAvgPES;
	
	double m_dTotalEnergy; // of all mols refd, J/mol
	CNumeric m_numTotalEnergy;

	double m_dTotalEnergyS; // of all mols refd, J/mol
	CNumeric m_numTotalEnergyS;
	
	double m_dTotalPressure;  // of all mols refd, N/m
	CNumeric m_numTotalPressure;

	double m_dTotalPressureS;  // of all mols refd, N/m
	CNumeric m_numTotalPressureS;

	double m_dDensity;        // total number density in (nm2)^-1
	CNumeric m_numDensity;

	double m_dDensityS;     // smoothed version
	CNumeric m_numDensityS;            
	
	
	int m_nMolTypes;  // num mol types ref'd by monitor
	
	// need to keep track of what valid and what not:
	union 
	{
		unsigned all;         // all the flags in one go
		struct               // bf for bit flag
		{
		    unsigned xleft:    1; 
		    unsigned ytop:     1;
		    unsigned xright:   1;
		    unsigned ybottom:  1; 
		} bf; 
	}
	m_fsValid;

};                                                       
