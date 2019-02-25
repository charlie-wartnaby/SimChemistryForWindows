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

// For an array of commands attached to an object:
#define MAX_COMMAND_LEN        24                              
#define MAX_COMMANDS_PER_OBJ   10
#define MAX_OBJ_TYPES          50

// Command mode of simulation
// (storing old history or actually executing)                       
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

// return values from sim functions      
#define SR_OK                  0
#define SR_WARN                1              
#define SR_ERROR               3          
#define SR_NO_MORE             4 
#define SR_NO_SUCH_TYPE      (-1)   

// simulation state machine values
#define SM_SIM_RUNNING         1
#define SM_SIM_PAUSED          2
#define SM_SIM_STOPPED         3

#define SM_NOT_DRAWING         1
#define SM_AWAITING_CLICK1     2 
#define SM_BUTDN_NOT_MOVED1    3
#define SM_DRAGGING            4
#define SM_AWAITING_CLICK2     5

// command argument categories
#define CA_NONE                0
#define CA_DOUBLE              1                          
#define CA_STRING              2   
#define CA_BYTE                3 
#define CA_INT                 4 
#define CA_LONG                5

// command type categories
#define CT_ENDLIST             0
#define CT_STATE               1 
#define CT_CONTROL             2
#define CT_OBJECT              3

// command numerical identifiers; objects < 20
#define MAX_OBJ_CI             19

#define CI_MOL                  0  
#define CI_WALL                 1   
#define CI_PISTON               2
#define CI_MONITOR              3
#define CI_TEXTBOX              4
#define CI_GRAPH                5  
#define CI_SLIDER               6

#define CI_ENDLIST             20
#define CI_TEMPERATURE         21
#define CI_UNIDENTIFIED        22
#define CI_BEGIN_HISTORY       23
#define CI_END_HISTORY         24   
#define CI_BEGIN_STATE         25
#define CI_END_STATE           26 
#define CI_OBJTYPE             27  
#define CI_EMPTY               28 
#define CI_HSRADIUS            29
#define CI_MASS                30
#define CI_NAME                31
#define CI_X                   32
#define CI_Y                   33    
#define CI_VX                  34
#define CI_VY                  35 
#define CI_FILLRED             36     
#define CI_FILLBLUE            37
#define CI_FILLGREEN           38
#define CI_LINERED             39
#define CI_LINEBLUE            40
#define CI_LINEGREEN           41
#define CI_LINETHICK           42
#define CI_SPEED_FIXED         43             
#define CI_SPEED_THERMAL       44
#define CI_SPEED_SYS_THERMAL   45       
#define CI_DIRECTION_FIXED     46
#define CI_DIRECTION_RANDOM    47                     
#define CI_FILLMOL             48       
#define CI_LEFT                49
#define CI_RIGHT               50
#define CI_TOP                 51
#define CI_BOTTOM              52  
#define CI_ELASTIC             53
#define CI_THERMAL             54
#define CI_SPEED_THERMALISED   55
#define CI_SPEED_UNCHANGED     56
#define CI_SYSTEM_THERMAL      57   
#define CI_X1                  58
#define CI_X2                  59
#define CI_Y1                  60
#define CI_Y2                  61         
#define CI_RUN                 62              
#define CI_OBJTYPE1            63
#define CI_OBJTYPE2            64 
#define CI_INTERACTION         65   
#define CI_WELLDEPTH           66
#define CI_EQM_SEPN            67
#define CI_CUTOFF              68
#define CI_MAX_PE              69
#define CI_STOP                70
#define CI_WALLTYPE            71
#define CI_HORIZONTAL          72
#define CI_VERTICAL            73
#define CI_CONST_PRESS         74
#define CI_SPRING_PRESS        75    
#define CI_XLEFT               76
#define CI_XRIGHT              77
#define CI_YTOP                78
#define CI_YBOTTOM             79
#define CI_XLREST              80
#define CI_YTREST              81
#define CI_DELTYPE             82
#define CI_RESET               83
#define CI_DELAY               84
#define CI_SIZE                85
#define CI_FONT                86
#define CI_TEXT                87
#define CI_ID                  88
#define CI_NUMBER              89
#define CI_PRESSURE            90
#define CI_DESTRUCTOR          91
#define CI_FIXER               92
#define CI_HEATER              93
#define CI_FIXT                94
#define CI_NOFIXT              95
#define CI_FIXDENS             96
#define CI_NOFIXDENS           97
#define CI_MOLTYPE             98
#define CI_GRAVITY             99
#define CI_AUTORUN            100
#define CI_SELN_RESET         101
#define CI_SELECT             102
#define CI_SELN_DELETE        103
#define CI_PAUSE              104
#define CI_DEL_OBJS           105
#define CI_DEL_ALL            106
#define CI_XDATA              107
#define CI_YDATA              108
#define CI_TITLE              109
#define CI_XLABEL             110
#define CI_YLABEL             111 
#define CI_FIRST_ID           112
#define CI_LAST_ID            113
#define CI_MON_UID            114 
#define CI_CHANGE             115  
#define CI_SELN_MOVE          116
#define CI_GRID_ON            117
#define CI_GRIDSIZE           118
#define CI_DATA               119  
#define CI_SCALEMIN           120
#define CI_SCALEMAX           121
#define CI_SET                122  
#define CI_CLEAR_GRAPH        123
#define CI_POWER_ON           124
#define CI_POWER_OFF          125
#define CI_STYLE              126
#define CI_FILLED             127
#define CI_RED                128
#define CI_GREEN              129
#define CI_BLUE               130        
#define CI_HATCH              131
#define CI_LABEL              132  
#define CI_SIMCHEMISTRY_VER   133            
#define CI_DAMP_CONST         134
#define CI_SIM_SIZE           135
#define CI_WIDTH              136
#define CI_HEIGHT             137
#define CI_DELTATIME          138
#define CI_USE_AUTO_DELTATIME 139
#define CI_BARRIER            140
#define CI_PRODUCT1           141
#define CI_PRODUCT2           142
#define CI_FORM_ENERGY        143
#define CI_PRODUCTS           144
#define CI_AVERAGING_TC       145
#define CI_SET_RAND_SEED      146


#define FIRST_SPECIAL_ID 1000000L
#define MON_FIRST_MON_FIRST_MOL_ID    FIRST_SPECIAL_ID  // id of first mol made by monitor with mon_uid 1
#define MON_MOL_IDS_PER_MON 1000000L  // number of mol ids to allocate to each monitor
#define MOL_FIRST_REACTION_PRODUCT_ID 1000000000L // id of first mol of type created by any reaction

#define NR_EARLIER              0
#define NR_LATER                1
#define NR_ROOT                 2
#define NR_NOT_IN_TREE          3       

// types of numeric value (CNumeric types)
#define NUM_USERWRITE           1   // OR together if both (in fact
#define NUM_USERREAD            2   // can always read writeable quantity)

// types of CNumLink (esp see use in textboxes)
#define NL_NUMREF               1
#define NL_TEXT                 2   
#define NL_RETURNCHAR           3

// old default values of screen scaling etc
#define TRAD_SCREEN_WIDTH_NM   (3.765)
#define TRAD_SCREEN_HEIGHT_NM  (2.25)
#define TRAD_SCREEN_SF          (200.0)
#define TRAD_SCREEN_BOTTOM_NM  (0.25)
#define TRAD_SCREEN_LEFT_NM     (0.0)

// Limits imposed on user inputs       
#define LIM_MIN_X            (-10)
#define LIM_MAX_X             10
#define LIM_MIN_Y            (-10)
#define LIM_MAX_Y             10
#define LIM_MIN_SIM_WIDTH       1
#define LIM_MAX_SIM_WIDTH     (LIM_MAX_X - LIM_MIN_X)
#define LIM_MIN_SIM_HEIGHT      1
#define LIM_MAX_SIM_HEIGHT    (LIM_MAX_Y - LIM_MIN_Y)
#define LIM_MAX_SIM_BOTTOM    (LIM_MAX_Y - LIM_MIN_SIM_HEIGHT)
#define LIM_MAX_SIM_LEFT      (LIM_MAX_X - LIM_MIN_SIM_WIDTH)
#define LIM_TOTAL_NUM_OBJS    500L
#define LIM_MAX_TEMP         1000   // K
#define LIM_MIN_TEMP            5
#define LIM_MAX_GRAVITY     (1e7)
#define LIM_MAX_FILLMOLS      150 
#define LIM_MIN_MOL_MASS        1  // amu
#define LIM_MAX_MOL_MASS     1000  // amu 
#define LIM_MIN_MOL_RADIUS   0.01  // nm
#define LIM_MAX_MOL_RADIUS   0.50
#define LIM_MAX_MOL_SPEED   10000 
#define LIM_MAX_PISTON_SPEED (LIM_MAX_MOL_SPEED - 1000)  // hope means mols get away from piston
#define LIM_MIN_LJ_CUTOFF     0.1
#define LIM_MAX_LJ_CUTOFF      10      
#define LIM_MIN_LJ_EQM_SEPN  (LIM_MIN_MOL_RADIUS * 2) 
#define LIM_MAX_LJ_EQM_SEPN     5
#define LIM_MIN_CUT_PE          0 
#define LIM_MAX_CUT_PE        500
#define LIM_MIN_WELL_DEPTH      0
#define LIM_MAX_WELL_DEPTH     50
#define LIM_MIN_MON_POWER (-100000) // kJ/mol/ns
#define LIM_MAX_MON_POWER   100000
#define LIM_MIN_CTRL_DENSITY    0
#define LIM_MAX_CTRL_DENSITY  100
#define LIM_MAX_PISTON_PRESSURE 1  // N/m
#define LIM_MAX_PISTON_FCONST   1  // N/m/nm   
#define LIM_MIN_PISTON_MASS     1  // amu
#define LIM_MAX_PISTON_MASS 10000  // amu    
#define LIM_MAX_PISTON_DAMP 10000  // /ns
#define LIM_MIN_GR_SYMB_SIZE    1  // pixels
#define LIM_MAX_GR_SYMB_SIZE   50   
#define LIM_MAX_GR_LABEL_CHARS 30
#define LIM_MAX_GR_TITLE_CHARS 40  
#define LIM_MIN_GR_STYLE_NAME    1
#define LIM_MAX_GR_STYLE_NAME   10
#define LIM_MIN_WALL_THICK      1
#define LIM_MAX_WALL_THICK     20
#define LIM_MIN_MONITOR_THICK   1
#define LIM_MAX_MONITOR_THICK  20    
#define LIM_MIN_DELTA_TIME   1e-9
#define LIM_MAX_DELTA_TIME   1e-4  
#define LIM_MAX_CHARS_SLIDER_TITLE 32   
#define LIM_MIN_CHARS_OBJTYPE   1
#define LIM_MAX_CHARS_OBJTYPE  24   
#define LIM_MIN_CHARS_OBJTYPE_TYPE  3 // mol
#define LIM_MAX_CHARS_OBJTYPE_TYPE  7 // monitor, textbox
#define LIM_MAX_CHARS_NUMERIC_NAME  64
#define LIM_MIN_CHARS_NUMERIC_NAME  1
#define LIM_MIN_CHARS_INTERACTION   4 // null  
#define LIM_MAX_CHARS_INTERACTION  20 // not sure  
#define LIM_MAX_CHARS_FONTNAME    127 // no idea 
#define LIM_MAX_CHARS_TEXTBOX    1000
#define LIM_MIN_BARRIER  0.0
#define LIM_MAX_BARRIER  100.0  // no idea if good number
#define LIM_MIN_FORMATION_ENERGY  (-300) // guess
#define LIM_MAX_FORMATION_ENERGY  (300) // guess
#define LIM_MIN_AVERAGE_TC  1e-4
#define LIM_MAX_AVERAGE_TC  1e-1



// Return values from OnHotSpotLButtonDown()
#define HSLBD_ONE_OFF_ACTION_DONE      1  // performed an action on bdown
#define HSLBD_TRACKING_STARTED         2  // now want tracking following but down
#define HSLBD_ERROR                    3  // problem


// General macros        
#define MIN(x, y) ((x) <= (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y)) 

#define DRAG_THRESH             5 // how far in pixels before doing wall etc

#define REGISTER_INT(int_name) pInt = new (int_name); pInt->m_pSim = this; pInt->m_nIntTypeID = id++; RegisterInt(pInt)
              
#define COS60               (0.5)
#define SIN60         (0.8660254)  
#define COS30         (0.8660254)
#define SIN30               (0.5)   
#define PI         (3.1415926535)  
#define PI_BY_2       (1.5707963) 
#define KB          (1.38066e-23)      
#define NEARLY_ZERO       (1e-20)
#define NEARLY_INFINITY   (1e+30) 
#define TWO_TO_THE_ONE_SIXTH (1.122462048) 
#define ONE_AMU     (1.66056e-27)    
#define AVAGADRO  (6.02e23) 
#define ONE_SIXTH (0.16666666666667)         
#define SQRT_2_OVER_2   ( 0.7071067811865)

#define DW_CROSSHAIR_RADIUS    10      
#define DRAG_THRESH_PIXELS      5     
              
              
 // forward references required in following class declarations:
class CSimObj;     
class CSObj;
class CSim;
class CObjType;
class CLenDoc;
class CInt;           
class CListHandle;
class CNP;
class CNumeric;
class CNumLink;
class CLN;    
class CGridTracker;
class CHotSpot;

class CLN  // simple general purpose linked list node
{               
	// These aren't actually used anywhere now, but am loath to
	// delete them yet in case they come in handy in near future.
public:                                                 
	CLN* m_pNextLN;
	void* m_pObj;
};

class CNumeric
{ 
	// A CNumeric object is essentially a pointer to a numerical
	// value that belongs to some simulation object or object type.
	// It provides information such as the units, whether the user
	// should be able to modify it, and a description too.
public:       
	CNumeric();
	
	double*   m_pdValue;      // points to underlying variable whose value we want 
	int       m_nType;        // eg read, read/write, write
	CString   m_sName;        // eg "red wall:x1", for big list
	CString   m_sUnits;       // eg "N/m", use "" if unitless
	double    m_dMin;         // min expected, or allowed to set
	double    m_dMax;         // max expected, or allowed to set
	CString   m_sDescription; // eg "smoothed partial pressure"
	CPtrList  m_refList;      // to keep track of CNumLink objs referencing it 
	                          // so can tell them now invalid when destroyed 
	CSim*     m_pSim;         // so can find big list for self-removal when destroyed

    void RemoveRef(CNumLink* pLink);   // when link destructs, asks us to forget it
	~CNumeric();         // during which will remove self from list
};

class CNumLink
{             
	// A CNumLink object makes up each part of a textbox string. It may
	// include a pointer to a CNumeric quantity, and in turn the CNumeric
	// will have a pointer back to this CNumLink. If we destroy the CNumLink,
	// it will tell the CNumeric to forget it; if the CNumeric is destroyed,
	// it will tell the CNumLink that it no longer exists. All this allows
	// us to write links to things which might subsequently be deleted without
	// leaving any invalid pointers to nowhere.
public:       
	CNumLink();
	~CNumLink();     // during which will remove references to self
		
	CString   m_sText;         // eg "37.23 K" or "<not found>" or "Osmosis Expt"
	CString   m_sLinkText;     // eg "temperature" or "red monitor:3:oxygen:number"
	int       m_nPointerValid; // invalidated upon obj destruction or if nothing found
	CNumeric* m_pNumeric;      // pointer to CNumeric attached to value we want   
	int       m_nType;         // NL_NUMREF or NL_TEXT or NL_RETURNCHAR
	CNumLink* m_pNextLink;     // chain of these do all text for a textbox
};

class CHotSpot
{ 
	// Some objects have defined rectangles which the sim should change
	// cursor for if it is placed over them, and do things when they
	// are clicked
	
public:
	CHotSpot();
	~CHotSpot();  // will remove ref to self from sim during this
	
	CRect m_rect;  // device coords defining hot spot
	CRect m_trackingRect;  // when moved by mouse, if that sort of hotspot
	CPoint m_ptLBDown;   // point where mouse button first down
		
	CSObj* m_pOwner;
	int m_nId;   // one obj may have more than one, must know which hit
};

typedef struct CMD_ENTRYtag
{
	char command[MAX_COMMAND_LEN];
	int arg_type;
	int cmd_type;
	int cmd_id;    
	double lo_lim;
	double hi_lim;
    unsigned int nSpecialFlags;
}    
CMD_ENTRY;

// special flags for commands:
#define CE_DONT_RECORD_IN_HISTORY 1 // eg autorun or scaling, should just remember state

class CSimObj
{
public:       
	virtual ~CSimObj();
	
	// GrabCommand gets next command (eg 'xleft 3') from command string and
	// copies argument, eg 3, to shared m_xArg variable; only gets commands
	// that belong to this sort of simobj, and removes it from string when done
	int GrabCommand(CString& remainingLine, CString& errString, 
	                 int& cmd_id_ret, CMD_ENTRY** ppCmdEntry = NULL); 
	
	// PopWord gets the next word or number from the command string                 
	void PopWord(CString& remainingLine, CString& word);           
	
	// FindGrabCommand searches string for specified command, returns TRUE if
	// found it, leaves it in place, copies any argument to m_lArg etc
	int FindGrabCommand(CString& commandLine, int nCmdId);  
	
	// MatchCommand is like FindGrabCommand but only looks for first command
	// in string (to save time)
	int MatchCommand(CString& commandLine, int nCmdId);

	// SnipCommand removes a command from a string if it is present (ret TRUE if was)
	int SnipCommand(CString& commandLine, int nCmdId);
	
	int WriteCmdToDisk(CStdioFile& file, CString& outString);
	virtual int WriteState(CString& workingLine);
	virtual int AcceptCommandLine(CString& remainingLine, CString& errString) = 0;
	virtual CMD_ENTRY* GetCmdList() = 0;
	virtual int GetStateParam(int command) = 0;
	virtual void ContributeNumerics();  // add any public values to master list
//	CSimObj* m_pParent;
    
    // Utility functions useful for testing selections etc
	int RectsOverlap(double xl1, double yb1, double xr1, double yt1, 
	                     double xl2, double yb2, double xr2, double yt2); 

	int RectCircleOverlap(double xl, double yb, double xr, double yt, 
	                      double x, double y, double r);
	                      
	int LinesIntersect(double xl1, double yb1, double xr1, double yt1, 
	                     double xl2, double yb2, double xr2, double yt2); 
	                     
	int PointInInclinedRect(double xorg, double yorg,   // origin of rectangle
	                        double x1,   double y1,     // one corner on side from org
	                        double x2,   double y2,     // other corner on side from org
	                        double x,    double y);     // point to test 	                     
	
	void NormaliseRect(double& xleft, double& ytop, double& xright, double& ybottom);
		
	
	static double m_dArg;          // shared argument, type double           
	static CString m_sArg;         // shared argument, type string
	static unsigned char m_bArg;   // shared argument, type byte
	static int m_nArg;             // shared argument, type int
	static long m_lArg;            // shared argument, type long
};      

class CSObj : public CSimObj
{              
public:     
	CSObj();
    virtual ~CSObj();
    
	// Pointers to next and previous objects in list 
	CSObj* m_pNextSObj;
	CSObj* m_pPrevSObj;     
	int m_nLinked;  // whether in linked list at all

	// if moves in response to hard events, may be further forward in time
	// than other objects
	double m_dTimeDoneAlready;
	// note if has been moved in this way
	int m_nTouched;         
	
	CObjType* m_pType;
	CSim* m_pSim;    
	CListHandle* m_pFirstHandle;
	long m_lId;        // uniquely identifies this object instance eg mol:27
	int m_nSelected;  // whether selected on screen
	
	// Tracking rectangle used for resizing and moving when sim stopped  
	// lots of virtual functions here because need to handle at least one
	// special case (pistons) with two tracking rectangles
	CGridTracker* m_pTracker;
	virtual void CreateTracker();  // when first selected, or sim stops  
	virtual void DeleteTracker();  // when unselected, or sim runs
	virtual void DrawTracker(CDC* pDC);  
	virtual BOOL SetCursor(CWnd* pWnd, UINT nHitTest, int nMultiple) const; // like CRectTracker
	virtual int TrackerHitTest(CPoint point); 
	virtual int Track(int nMultiple, CWnd* pWnd, CPoint point);
	virtual void PrepareOtherTrackersToFollow(); // when leading multiple selection drag
	virtual void UpdateFollowingTrackers(int nDeltaXL, int nDeltaYT,
	                                     int nDeltaXR, int nDeltaYB,
	                                     CWnd* pWnd); 
	virtual void UpdateTrackerAsFollowing(int nDeltaX, int nDeltaY, CWnd* pWnd);
	virtual void PrepareTrackerForFollowing();   
	virtual void OnMoveOrResize(double dNewXLeft, double dNewYTop,
	                            double dNewXRight, double dNewYBottom,
	                            double dOldXLeft, double dOldYTop,
	                            int nResized);	                            
		
	// Used to see if selected by mouse click (sim coordinates):
	virtual int HitTest(double dXLeft, double dYBottom, double dXRight, double dYTop) = 0;

	// Handles user setting of object properties when object double-clicked on
	// or right click and chooses 'Properties':
	virtual void ObjectProperties();   
	
	// Handles move, ie when multiple selection dragged across screen
	virtual void HandleMove(double dDeltaX, double dDeltaY);
	
	// If has hotspots, must handle setting cursors over those too
	virtual int HotSpotSetCursor(CHotSpot* pHotSpot, CWnd* pWnd, UINT nHitTest);
	
	// Return value from next function indicates whether performed action or
	// if capture and tracking required (eg left button down on slider numeric
	// text opens dialogue as one-off action, lbdown on slider knob requires 
	// tracking until lbuttonup).
	virtual int OnHotSpotLButtonDown(CHotSpot* pHotSpot, UINT nFlags, CPoint point);
	
	// This is called if a hot spot is being dragged by user, eg slider knob:
	virtual void OnHotSpotMouseMove(CHotSpot* pHotSpot, UINT nFlags, CPoint point);
	
	// And when the button goes up...                                         
	virtual void OnHotSpotLButtonUp(CHotSpot* pHotSpot, UINT nFlags, CPoint point);
};

class CObjType : public CSimObj
{             
public:
	// Type properties
	CString m_sName;   // name of type, eg "oxygen molecule"
	int m_nBaseType;   // numeric ID for class of obj, eg wall            
	CSim* m_pSim;
	int m_nIsMobile;   // whether or not moves, eg mols do and walls don't
	int m_nHasInts;    // whether or not interacts with other obj types

	// List of actual object instances
	CSObj* m_pFirstSObj;
	CSObj* m_pLastSObj;                  
	long m_lLastIdUsed;
	
	
	virtual void Delete();  // does any type-specific internal deletion reqd
	
	CObjType();
	
	int AddObj(CSObj* pObj); // returns TRUE if successful, adds to linked list
	void RemoveObj(CSObj* pObj); // removes from linked list
	
	// Must draw all of own objects repeatedly to maintain animation:  
	virtual void DrawAllSObjs(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2) = 0;

	// called when user wants to draw obj(s) of this type; return TRUE if
	// going ahead with drawing, FALSE if they cancel:                                	
	virtual int DrawCommand();      
	
	// called so that user can edit type properties
	virtual void PropertiesCommand() = 0;          
	
	// called to add any of its objs within specified rectangle to sim's seln list:
	// (see also CSObj::HitTest); return TRUE if any selection made; if dragging,
	// don't toggle existing selection. If set to single, only select one obj.
	virtual int GetSelectedObjects(double dXLeft, double dYBottom, double dXRight,
	                              double dYTop, int nDragged, int nSingle);

	CSObj* FetchSObj(long lId);  // returns NULL if not found
	
	void DrawSelnBlob(CDC* pDC, int nXLogical, int nYLogical);
	
	// called during screen redraw when user drawing obj(s) of this type:                               
	virtual void UserDrawing(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2); 
	virtual void OnMouseMove();
	virtual void OnLButtonDown();
	virtual void OnLButtonUp();
	void ChangeDrawingStateOnLButtonUp();
	
	// Do any special processing at beginning, or if properties have
	// been modified, that is required before the object type is used
    // (eg if screen scaling params have changed, might have to fiddle
    // with bitmaps used for drawing).
	virtual void Initialise();
    
    
    // object deletion
    void DelObjs();
    
    
	CPoint DrawCrosshair(CDC* pDC);  // does so at current coords
	int m_nUseGridWhenGridOn;      // not true for mols
	           

	// simulation action routines
	
	// move all objects one timestep
    virtual void MoveSObjs();       
    
    // zero accels and return fastest speed squared
    virtual double PrepStep(); 
    
    // apply accelerations to particles from known forces
    virtual void ApplyAccels();
    
    
    // when sim gets going, may need to initialise things
    virtual void Prepare();
};

class CSimPoint
{
public:
	double m_dX;
	double m_dY;
};      



