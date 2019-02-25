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

class CLN;

class CListHandle
{      
	// Used to get at linked list of neighbour pair objects
	// in which a particular object is involved    
public:
    CListHandle();
	CListHandle* m_pNextHandle;
	CNP* m_pNP;
};

class CNP
{
	// neighbour pair
public:   
    CNP();
    CNP(CInt* pOwningInt);

	virtual ~CNP();  // in which unlinks itself from lists
	
	CSObj* m_pSObj1;
	CSObj* m_pSObj2;
	
	CNP* m_pLastNP;
	CNP* m_pNextNP; 
	
	CListHandle m_handle1;
	CListHandle m_handle2;
	
	double m_dTimeDue;  // rel to beginning of timestep, for hard event (ns)
	
    int m_nProximityHint;  // can be used to remember what sort of collisions
                           // possible before neighbour list rebuild
                           
	int m_nEventHint;      // can be used to remember what sort of collision
	                       // is scheduled to occur                             
    
	CInt* m_pInt;  
	
	CNP* m_pParentNode;
	CNP* m_pEarlierNode;
	CNP* m_pLaterNode;  
	int m_nParentRelationship;
};   

class CInt : public CSimObj
{ 
	// an interaction between two kinds of simulation object
public:                       
	// cloning function so sim can make more
	virtual CInt* Clone() = 0; 
	virtual ~CInt();
	     
	CSim* m_pSim;          
	int m_nIntTypeID;  // assigned in registration macro by sim
	
 	virtual void BuildNeighList();
 	virtual void CalcAllHardEvents(); 
	virtual void CalcHardEvent(CNP* pNP);
	virtual void CalcForces(); 
	virtual void ContributeNumerics();  // if it has any
	
    // Any types other than those two which are involved in interaction which
    // might be referenced (i.e. product types for reactive interaction) may
    // be deleted without the interaction noticing. This will be called if that
    // is a possibility:
    virtual int CheckNotReferencingDeadTypes();

	// define what types it knows about	
	int m_nBaseType1;
	int m_nBaseType2;                  
	
	// may be default type of int for that combination of basic types
	int m_nIsDefault;    
	
	// the null int, a special case, suits all combinations (but does nothing)
	int m_nIsNull;
	
	// categories (may not be mutually exclusive, in general)
	int m_nIsHard;
	int m_nIsSoft;
	
	// used to see if recalcs required if an obj may have moved
	int m_nType1Mobile;
	int m_nType2Mobile;                                          
	
	// Linked list of neighbour pairs to worry about
	CNP* m_pFirstNP;
	CNP* m_pLastNP;

#ifdef _DEBUG
	long m_lNumNP;
#endif
	
	// Keep stock of spare NP objects to reuse, to save reconstruction time
	CNP* m_pFirstSpareNP;
	
	// Name and description
	CString m_sName;
	CString m_sDescription;           
	int m_nHasDetails;   // will have dialogue for parameter editing:
	
    // This is called if there are further details (eg potentials,
    // reaction parameters) to edit. It should send a command to the sim
    // to change its state if the user does OK in the resulting dialogue.
	virtual int EditDetails(); // return IDOK if details changed
	                                                          
	// Pointers to actual object types for this particular instance
	CObjType* m_pType1;
	CObjType* m_pType2;
		                                                          
	// Methods to do collision detection, vel measurements etc
	virtual void ExecHardEvent(CNP* pNP);

	// Usual CSimObj command handlers; this CInt base class will handle
	// case for just the two object type names
	virtual int AcceptCommandLine(CString& remainingLine, CString& errString);
	virtual CMD_ENTRY* GetCmdList();
	virtual int GetStateParam(int command);  
	static CMD_ENTRY m_celCIntCommands[MAX_COMMANDS_PER_OBJ];  

};        
