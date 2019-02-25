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

// Orthogonal Boolean flags for mol's relationship with monitor

#define MNML_TOP      1
#define MNML_BOTTOM   2   
#define MNML_LEFT     4
#define MNML_RIGHT    8

#define MNML_INTERIOR 1
#define MNML_PROXIMAL 2

class CMolTypeStats;

class CMonMolNP : public CNP
{ 
public:    
    
    CMonMolNP::CMonMolNP(CInt* pOwningInt);

	// special NP because used in two lists; the normal list of
	// NPs maintained by a CInt, but a second list maintained
	// by the CSObjMonitor which tracks which mols lie within
	// it, hence the need for these extra list-pointer members:
	CMonMolNP* m_pNextInteriorNP; 
	CMonMolNP* m_pLastInteriorNP;  

	int m_nFlags;  // to record which lists in at present
		
	CMolTypeStats* m_pStats;  // stats entry belongs to, needed
	                          // if unlinks self from head or tail
	                          // end of list 

	void Unlink(); // from interior (stats) list only

	virtual ~CMonMolNP(); // in which unlinks itself
};

class CMonitorMolDefaultInt : public CInt
{                           
public:
    ~CMonitorMolDefaultInt();
	CMonitorMolDefaultInt();	
	virtual CInt* Clone();

	virtual void BuildNeighList();
	virtual void CalcAllHardEvents();
	virtual void CalcHardEvent(CNP* pNP);
	virtual void ExecHardEvent(CNP* pNP);
};

