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

// Command mode of simulation
// (storing old history or actually executing)         

#define SIM_NUM_COMMANDS      60
              
#define SCM_ACTIVE             1  // execute new commands and add to history
#define SCM_HISTORY            2  // just add old commands to history
#define SCM_STATE              3  // just execute commands to rebuild state
                       
// Types of history string
#define HT_CMD                 1
#define HT_INFO                2
#define HT_ALL                 0xFF   
#define NUM_HTYPES             2

// Special position parameters for getting at history
#define HP_NEXT                (-1)
#define HP_PREVIOUS            (-2)
#define HP_FIRST               (-3)
#define HP_LAST                (-4)   
#define HP_THIS                (-5)

// simulation state machine values
#define SM_SIM_RUNNING         1
#define SM_SIM_PAUSED          2
#define SM_SIM_STOPPED         3

#define SM_NOT_DRAWING         1
#define SM_AWAITING_CLICK1     2 
#define SM_BUTDN_NOT_MOVED1    3
#define SM_DRAGGING            4
#define SM_AWAITING_CLICK2     5

#define SM_REPLAY_STOPPED      1
#define SM_REPLAY_PAUSED_PLAY  2
#define SM_REPLAY_PLAYING      3
#define SM_REPLAY_FFWDING      4
#define SM_REPLAY_PAUSED_STOP  6

#define CMD_IMMEDIATE          1
#define CMD_FROM_SCRIPT        2 

#define PSCALING            1000 // number of points in CSim::m_pdVelVsPForUnitMOverT

#define FFWD_DELAY         (0.2) // seconds wait for delays in ffwd mode     

#define TIMESTEP_FRACTION_FOR_OVERLAP_COLLN (0.01) // if converging hard objects overlap already, do collision in this fraction of timestep

#define SIM_PREF_SAFETY_DIST (0.5) // nm

#define MIN_GREY_BAR_WIDTH_PIXELS  5

class CLN;  // linked list node

class CHistoryEntry
{
public:
	int type;
	CString string;
};                      

class CTriPtrArray : public CPtrArray
{
	// This is a triangular array in which only the diagonal
	// and upper right portions are populated.
public:
	CTriPtrArray();
	void RemoveIndex(int newIndex);
	void AddIndex(int newIndex);
	int GetIndex(int row, int col, int edgeLength);   
	int GetIndex(int row, int col);   
	int m_nNumEdges;        
	CSim* m_pSim;
};            

class CIntListEl
{
	// This class used to build fast-access linked lists of
	// interactions
public:
	CIntListEl* m_pNextEl;
	CInt* m_pInt;
};

class CTypeListEl
{
	// This class used to build fast-access linked lists of
	// object types
public:
	CTypeListEl* m_pNextEl;
	CObjType* m_pType;
};

class CSim : public CSimObj
{                
public:                    
	CLenDoc* m_pDoc;
	
	// Command list
	virtual int GetStateParam(int command);
	virtual CMD_ENTRY* GetCmdList(); 
    virtual int AcceptCommandLine(CString& remainingLine, CString& errString);
	int AcceptCommandLine(CString& remainingLine, CString& errString, int* nNoHistory); // non-virtual overloaded override of CSimObj version
    int WriteState(CStdioFile& file); // non-virtual overloaded override of CSimObj version
	static CMD_ENTRY m_celCSimCommands[SIM_NUM_COMMANDS];
	int HandleNewCommand(const char* commandString); // for immediate commands
	int HandleNewCommand(const char* commandString, int type); // also for non-immediate
	void AddToHistory(const char* string, int type);
	int ReadFromHistory(CHistoryEntry*& entry,
	                          int type, int index, int& currentIndex);
	int Save(CStdioFile& file);
	int GetNumHistoryEntries(); 
	BOOL OnIdle();                    
	void SingleStep();           
	void ChooseDeltaTime();
	
	void ReplayScript();
	void PlayScript();
	void StopScript();   
	void FastFwdScript();
	int KillPauseIfPaused(); // returns TRUE if was paused 
	
	// pause is a user command handled in CmdPause

	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags); // called when key pressed in sim
	                                                    // view (but not Esc from draw)
	
	// for user selection; return TRUE if did tracking or displayed dialogue
    // box, in which case left button will be up again upon exit
	int OnLButtonDown(CPoint point, int nFlags, CWnd* pWnd);     
	
	void OnFinishedDragging(int nFlags);
	void OnLButtonDblClk(int nFlags);  // to edit properties of obj hit
	int OnRButtonDown(int nFlags); // should create floating menu (return TRUE to have it done)

    // returns TRUE if brought up dialogue box, hence button up afterwards:
	int GetSelection(int nFlags, int nSingle); // common to double and left clicks

	void ObjectProperties();

    // called when simulation screen is resized; rectangle supplied must be in
    // logical coordinates but may not be normalised:
	void OnSizeSimScreen(CRect rect);			

    // called to impose new sim area and redisplay immediately with that area,
    // params passed are in nm:
    void RedisplayWithNewScaling(double dLeft, double dBottom, double dWidth, double dHeight);

    // used internally during rescaling operations:
    void RecalcScalingParams();

	int CmdTemperature();        
	int CmdInt(CString& remainingLine, CString& errString);             
	int CmdBeginHistory();
	int CmdEndHistory();        
	int CmdBeginState();
	int CmdEndState();
	int CmdRun(); 
	int CmdStop();           
	
	int m_nFillmolInProgress; 
	int CmdFillmol(CString& remainingLine, CString& errString);    

	int CmdChange(CString& remainingLine, CString& errString);  // modify existing obj or type
	int CmdObjType(CString& remainingLine, CString& errString);  // new type obj
	int CmdNewObj(int nBaseType, CString& remainingLine, CString& errString); // new sobj
	int CmdChangeObj(int nBaseType, CString& remainingLine, CString& errString); // modify existing sobj
	int HandleObj(int nBaseType, CString& remainingLine, CString& errString, int nChangeExisting);
	int HandleObjType(CString& remainingLine, CString& errString, int nChangeExisting);  // whether new or not

	int CmdDelType(CString& remainingLine, CString& errString);
	int CmdReset();
	int CmdDelay();               
	int CmdSelnReset();    
	int CmdSelnMove(CString& remainingLine, CString& errString);
	int CmdSelect(CString& remainingLine, CString& errString);
	void Unselect(CSObj* pSObj);
	int CmdSelnDelete();    
	int CmdPause();  // waits for keypress in script replay
	int CmdDelObjs(CString& errString);  // deletes all objs of specified type, leaving type
	int CmdDelAll(CString& errString); // delets all objs of all types, leaving types
	int CmdSet(CString& remainingLine, CString& errString); // changes numeric values	
	int CmdClearGraph(CString& remainingLine, CString& errString);
    int CmdSimSize(CString& remainingLine, CString& errString);
	
	void OnLoad(); // called when loading from disk complete
		
	~CSim();
	CSim();      
	int m_nAppExistenceFlagIndex;  
	void Initialise(); 
	
	double  m_dDocSimChemistryVer; // software version found in loaded file

    // If mol type deleted, may be reactive ints that still refer to
    // it as a product (note: interactions in which that mol type
    // is one of the interacting parties deleted automatically when
    // object type destroyed). Check here that all OK:
    int CheckIntsNotReferencingDeadTypes();

	// Master list of public numeric quantities (declaration must
	// appear before any embedded CNumerics, as destructors will be
	// called in reverse order and nums will try and remove selves from list)
	CPtrArray m_numList;
	virtual void ContributeNumerics();  // add own public values to list
	void AcceptNewNumeric(CNumeric* pNumeric);  // for others to call to add one
    void RemoveNumeric(CNumeric* pNumeric); // so deleted objs can remove selves
    CNumeric* FetchNumeric(CString& string); // find one in list by name else NULL returned
    
	CPoint SimToLogical(double x, double y);  
	void LogicalToSim(CPoint point, double* x, double* y);    
	void Gridify(double* x, double* y);
    int OnGridSettings();  // does user dialogue to change settings, returns TRUE if altered
    
    // returns TRUE if brought up dialogue box, hence button up again:
	int GetSelectedObjects(double dXLeft, double dYBottom, double dXRight, double dYTop,
	                        int nDragged,  // flag at end because shift-clicking on clicked
	                                       // selection should toggle its selection, but shift-
	                                       // dragging over it should leave it selected
	                        int nSingle);  //  ... if only single selection wanted
	                                       
	CPtrList m_selectedObjects;
    
    // When one obj dragged in mult. selection, must prepare other
    // selected objs to be dragged with it, and they must all be told
    // at each mouse move of the displacement from the starting position:
	void PrepareTrackersToFollow(CSObj* pCallingObj);  // argument indicates caller
	void UpdateFollowingTrackers(int nDeltaX, int nDeltaY,
	                     CSObj* pCallingObj, CWnd* pWnd);


	// hot spots to take care of (bits of picture cursor should change for
	// that can be clicked on)
	CPtrArray m_hotSpots;
	void RemoveHotSpotRef(CHotSpot* pHotSpot);
	void AddHotSpotRef(CHotSpot* pHotSpot);  // should not mind if add existing one
				
	// simulation state machine 

	int m_nIsAutoRun;   // whether should go automatically when loaded
	int m_nWasAutoRun;  // whether was run automatically from startup by user
	int m_nSimState;      
	int m_nDoingHardEvents;
	int m_nDrawingState;    
	int m_nSimReady;
	int m_nReplayState;  // eg playing, paused, ffwding, etc         
	int m_nIsRecording;  // whether or not history being saved for replay
	int m_nDoingDelay;
	int m_nCurrentCmdIndex;
	CObjType* m_pDrawingType;
	
	double m_dSimMouseX; // where mouse pointer is now
	double m_dSimMouseY;  
	CPoint m_ptLogMouse;                    
	
	double m_dSimDragStartX; // where mouse pointer was first clicked
	double m_dSimDragStartY;
	CPoint m_ptLogDragStart;                                    
	
	BOOL m_bInClientRect;    // used by mols in user drawing phase                
	
	double m_dDeltaTime;  // ns 
	CNumeric m_numDeltaTime;
	
	double m_dUseAutoDeltaTime;  // 0 or 1, thresh 0.5
	CNumeric m_numUseAutoDeltaTime;
	
	double m_dSimTime;   // ns
	CNumeric m_numSimTime;
	
	double m_dScriptTime;  
	double m_dTimeThisLoop;
	double m_dAverageTimeConst; // for smoothing out temperature measurements etc, ns
	CNumeric m_numAverageTimeConst;
	double m_dTimeRatio;  // real time / sim time   
	double m_dDelayTimer;  // ns left to end of delay
	double m_dDelayTotal;  // ns for whole delay
	double m_dSimTimeLastCmd; // to measure delay since last thing done if sim runs  
	DWORD m_lIdleScreenUpdateMs;
	
	DWORD m_lNextLoopStartTicks;

	long m_lStepsBetweenUpdates;

	// History command strings and response strings 
	CPtrArray m_historyStrings;     
	// What sort of info being accepted
	int m_nCmdMode;
	// Current command string  
	CString m_currentCommand;
	// List of types   
	CPtrArray m_typeArray; // slower than linked list, but can 
	                       // make linked list internally anyway.   
	long m_lTotalNumObjs;      
	int m_nIssuedTooManyObjsWarning;                 
	CObjType* GetObjTypeByName(CString& objTypeName);
	int GetObjTypeIndexByName(CString& objTypeName);
	int GetObjTypeIndexByPtr(CObjType* pObjType);
	void GetTypeArray(CPtrArray& array, int baseType);
	void MakeUniqueTypeName(CString& sName);
	CString GetBaseTypeName(int nBaseType);
	
	// Search from current point in history onwards for CSObj ids belonging to
	// a (newly created) CObjType, so duplicate ids can be avoided	
	void ScanHistoryForUsedIds(CObjType* pType);

	// Monitors are special; they can make molecules by themselves, and those
	// molecules will be given ids related to the unique monitor id so that even
	// if the monitor makes an indeterminate number of mols, those created manually
	// will get reproducible ids for selection.	
	void ScanHistoryForMonitorIds();
	long m_lLastMonIdUsed;  // anywhere in history, top unique monitor id used 
	                        // monitor's responsibility to use it, ours to add to history
	
	BOOL OnDrawCommand(int typeId);
	BOOL OnPropertiesCommand(int typeId);
	
	double m_dScaleFactor;  // logical co-ord = (real co-ord * sf) - offset
	double m_dXOffset;
	double m_dYOffset;   

    double m_dScreenWidth_nm; // real-world screen area defining coords
    double m_dScreenHeight_nm;
    double m_dScreenLeft_nm;
    double m_dScreenBottom_nm;

    CRect m_ScreenLogicalRect;  // logical coords of total sim screen window

    // The currently displayed simulation area will not in general fit perfectly in
    // the window area, preserving aspect ratio. Normally either the window will be
    // too wide, requiring that grey stripes appear at each side and the sim area
    // goes right from top to bottom centred in the middle, or conversely the window
    // might be too tall and thin, so that the simulation area must appear letterbox
    // like with grey regions above and below it.
    int m_nNeedGreyBars;     // if screen is of correct aspect ratio, don't use grey bits
    CRect m_GreyLogicalRect1; // logical rectangle inaccessible off edge of sim area
    CRect m_GreyLogicalRect2;
    CRect m_SimLogicalRect;  // active area for simulation

	double m_dGridSize;   // for aligning walls, pistons etc neatly
	int m_nUseGrid;
	
public:
	// Triangular array of interactions between all of the object types    
	CTriPtrArray m_intArray; 
	
	// Keep lists of interactions of different categories that will need
	// to be called in same sorts of way such that this can be done at
	// run-time without further prevarication:
	CIntListEl* m_pFirstHardIntEl;
	CIntListEl* m_pFirstSoftIntEl;
	CIntListEl* m_pFirstNonNullIntEl;

	// Similarly lists of types that need to do particular actions:	
	CTypeListEl* m_pFirstMobileTypeEl;
	CTypeListEl* m_pFirstMonitorTypeEl;
	CTypeListEl* m_pFirstGraphTypeEl;
	 
	void RegisterInt(CInt* pInt);
	CPtrList m_intGallery;              
	int GetIntsByObjTypeNames(CString& sObjType1, CString& sObjType2,
	              CPtrArray* pIntList);
	CInt* GetDefaultIntByIndex(int nType1, int nType2);
	CInt* GetIntInUseByNames(CString& sObjType1, CString& sObjType2);
	CInt* GetIntInUseByIndex(int nType1, int nType2);
	CInt* GetIntByName(CString& sName);
	
	void BuildIntCatLists(); 
	void DeleteIntCatLists(); 
	void PrepareMonitors();
	void PrepareTextboxes();
	
	void PrepareSim();       
	void BuildTypeCatLists(); 
	void DeleteTypeCatLists();
			
	// Hard event tree
	CNP* m_pRootNode;         
	void InsertNode(CNP* pNP);  
	void DetachNode(CNP* pNP);
	void ExecHardEvents();   
	double m_dTimeSoFar; // within this timestep already done; can be modified in interaction events
    int m_nSynchronizeHappened; // whether all sim objs synchronised to time
                                // during hard event processing
    
    void InvalidateAllHardEvents(); // called if have to delete or create objects when
                                    // in middle of dealing with hard event list
    void SynchronizeToNow();  // gets sim and objs to catch up with current sim time done
    
#ifdef _DEBUG
	long m_lNumNodes;
#endif                         

	// link node store for general use
	CLN* m_pFirstSpareLN;
	inline CLN* GetLN();  // returns an LN for use
	inline void RecycleLNs(CLN*& pFirstLN, CLN*& pLastLN);
	
	// System properties
	double m_dTemperature;  
	CNumeric m_numTemperature;

	double m_dGravitationalAcceleration;   // in m/s/ns
	CNumeric m_numGravitationalAcceleration;
	int m_nGravityOn;                      // used to avoid unnecessary calcs when g = 0
	double m_dGravitationalYVelIncrement;  // all y-vels affected similarly for given g	
	                                       // units m/s/timestep
	
	double m_dSafetyDistLeft;   
	double m_dSafetyDist;
	double m_dInsuranceDist;  
	double m_dMaxVSquared;
	
	// utility functions

	static void Round(double& dNumber, double dPrecision);
	static void Round(double& dNumber, int nSigFigs);
	double RandomDirection();
	double ThermalSpeed(double temperature, double mass);  // Maxwellian for T
	double ThermalWallSpeed(double temperature, double mass); // exact speed for T
	double* m_pdVelVsPForUnitMOverT;  // table reqd for this, see also PSCALING #define 

    // The random number generator used in v2.21 and previously was
	// a "quality" algorithm from a well-known textbook which I had permission
	// to use in the binary, but not to distribute as source code. Now it
	// is replaced by the system function so that this source as a whole can
	// be distributed.
	long m_lRanSeed;
    inline double GetRandom()
    {
		return ((double) rand() / (RAND_MAX + 1));
	};
	void SeedRandom(long seed)
	{
		srand((unsigned int) seed);
	};


	void BuildThermalWallTable();

	double CosineDistributionAngle();
};

