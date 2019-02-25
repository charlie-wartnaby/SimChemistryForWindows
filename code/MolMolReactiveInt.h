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

class CMolMolReactiveInt : public CMolMolHardInt
{
public:
    CMolMolReactiveInt();

    virtual CInt* Clone();
    virtual int EditDetails();
    virtual void ExecHardEvent(CNP* pNP);
    virtual void ContributeNumerics();

    virtual int CheckNotReferencingDeadTypes();

	virtual int AcceptCommandLine(CString& remainingLine, CString& errString);
	virtual CMD_ENTRY* GetCmdList();
	virtual int GetStateParam(int command);  
	static CMD_ENTRY m_celCMolMolReactiveIntCommands[MAX_COMMANDS_PER_OBJ];  

    CString m_sProduct1Name;
    CString m_sProduct2Name;

    CObjTypeMol* m_pProduct1Type;
    CObjTypeMol* m_pProduct2Type; // may be specified even if only one prod,
                               // will then be default choice if switch to
                               // 2 prods later, or could be NULL

    int m_nProducts;   // either 1 or 2

    double m_dBarrier; // kJ/mol activation barrier
    CNumeric m_numBarrier;

    double m_dTotalBarrier; // kJ/mol including any potl energy increase
    
};
