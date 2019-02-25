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

class CLennardJonesInt : public CInt
{                           
public:
	CLennardJonesInt();	
	virtual CInt* Clone();

	virtual void BuildNeighList();   
	virtual void CalcForces(); 
	virtual void ContributeNumerics();
	  
	virtual int EditDetails();

	virtual int AcceptCommandLine(CString& remainingLine, CString& errString);
	virtual CMD_ENTRY* GetCmdList();
	virtual int GetStateParam(int command);  
	static CMD_ENTRY m_celCLennardJonesIntCommands[MAX_COMMANDS_PER_OBJ];  

public:
	// variables set by user	
	double m_dCutoffDist;  // nm      
	CNumeric m_numCutoffDist;
	double m_dEqmSepn;     // nm
	CNumeric m_numEqmSepn;
	double m_dMaxPE;       // kJ/mol
	CNumeric m_numMaxPE;
	double m_dWellDepth;  // kJ/mol
	CNumeric m_numWellDepth;
	
	int m_nEditedYet;     // whether params have been edited by user
		
protected:
	// derived variables for quick use	
	double m_dSafetySqdIncRadii;  // nm2
	double m_dCloseLimit;         // nm
	double m_dCloseLimitSqd;      // nm2
	double m_dCutoffSqd;          // nm2
	double m_dSigma;              // nm
	double m_dSigma6;             // nm2
	double m_dEta;                // J/mol 
	double m_dFMax;               // J/mol/nm

}; 
