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


// piston constants
#define PT_FIXED                0
#define PT_SPRING               1    
#define PT_VERTICAL             0
#define PT_HORIZONTAL           1

#define PD_PISTON               0
#define PD_REST_POSITION        1    

#define PISTON_DEFAULT_SIZE (0.5)     

#define OBJTYPE_PISTON_CMDS     9


class CObjTypePiston : public CObjType
{                    
public:       
	CObjTypePiston(); 

	virtual int WriteState(CString& workingLine);
	virtual int GetStateParam(int command);
	virtual CMD_ENTRY* GetCmdList(); 
	virtual int AcceptCommandLine(CString& remainingLine, CString& errString);
	virtual void ApplyAccels();
    virtual void MoveSObjs();    
    virtual double PrepStep(); 
   
	
	virtual void DrawAllSObjs(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2);
	virtual void UserDrawing(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2);
	virtual void OnLButtonUp();     
	       
	int m_nDrawingPhase;
    double m_dDrawingPistonXLeft;
    double m_dDrawingPistonYTop;
    double m_dDrawingPistonXRight;
    double m_dDrawingPistonYBottom;
    CPoint m_ptDrawingPistonTopLeft;
    CPoint m_ptDrawingPistonBottomRight; 
    double m_dDrawingRestLeft;
    double m_dDrawingRestTop;
        
    CPoint DrawRestCursor(CDC* pDC);
    void IssueNewPistonCmd();
    
	virtual int DrawCommand();  
	virtual void PropertiesCommand(); 
	virtual void ContributeNumerics();
	
	static CMD_ENTRY m_celCObjTypePistonCommands[OBJTYPE_PISTON_CMDS];    
	
	int CmdWallType(CString& remainingLine, CString& errString);
	
    int m_nDirectionType;
	double m_dFixedPressure;    // N/m
	CNumeric m_numFixedPressure;
	double m_dForceConst;       // N/m/nm
	CNumeric m_numForceConst;
	double m_dMass;             // amu           
	CNumeric m_numMass;                          
	double m_dDampingConst;    // /ns
	CNumeric m_numDampingConst;
	int m_nPressureType;
	CObjTypeWall* m_pWallType;

	// need to keep track of what valid and what not:
	union 
	{
		unsigned all;         // all the flags in one go
		struct               // bf for bit flag
		{
		    unsigned walltype:      1; 
		    unsigned name:          1;  
		    unsigned mass:          1;
		    unsigned forceconst:    1;
		    unsigned fixedpressure: 1; 
		    unsigned dampingconst:  1;
		    unsigned direction:     1;
		} bf; 
	}
	m_fsValid;

};

class CSObjPiston : public CSObj
{   
public:      
	CSObjPiston();  
	~CSObjPiston();
	
	double m_dXLeft;      
	CNumeric m_numXLeft;
	double m_dYTop; 
	CNumeric m_numYTop;    
	double m_dXRight;  
	CNumeric m_numXRight;
	double m_dYBottom; 
	CNumeric m_numYBottom;
	double m_dXLRest;  // top-left equil co-ord
	CNumeric m_numXLRest;
	double m_dYTRest;  // top-left equil co-ord 
	CNumeric m_numYTRest;   
	double m_dXDisplacement; // from rest pos to where it is
	CNumeric m_numXDisplacement;
	double m_dYDisplacement;
	CNumeric m_numYDisplacement;
	double m_dVx;
	double m_dVy;       
	
	virtual void ContributeNumerics();
	
	// useful also to have direction cosines, length, normal vector, offset from O:
	double m_dCxHorizontal;                                                                  
	double m_dCyHorizontal;
	double m_dNxHorizontal;
	double m_dNyHorizontal;
	double m_dLengthHorizontal;     
	double m_dCxVertical;                                                                  
	double m_dCyVertical;
	double m_dNxVertical;
	double m_dNyVertical;
	double m_dLengthVertical;     

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
		    unsigned xlrest:   1;
		    unsigned ytrest:   1;   
		    unsigned vx:       1;
		    unsigned vy:       1;
		    unsigned cosines:  1;
		    unsigned normals:  1;
		    unsigned lengths:  1; 
		} bf; 
	}
	m_fsValid;
	
	virtual int GetStateParam(int command);
	virtual CMD_ENTRY* GetCmdList(); 
	virtual int AcceptCommandLine(CString& remainingLine, CString& errString);

	virtual int HitTest(double dXLeft, double dYBottom, 
	                           double dXRight, double dYTop);       

	virtual void ObjectProperties();	                           
    
    CGridTracker* m_pRestTracker;  // extra tracking rectangle for rest position

	int m_nHitRestTracker;         // remember which tracker hit by mouse
	
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

	virtual void HandleMove(double dDeltaX, double dDeltaY);	                            
	                           
	                           
	static CMD_ENTRY m_celCSObjPistonCommands[MAX_COMMANDS_PER_OBJ];      
	
	void ClipSpeed();
};                                                       

