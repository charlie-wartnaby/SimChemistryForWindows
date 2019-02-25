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

class CMolMolHardInt : public CInt
{                           
public:
	CMolMolHardInt();	
	virtual CInt* Clone();

	virtual void BuildNeighList();
	virtual void CalcAllHardEvents();
	virtual void CalcHardEvent(CNP* pNP);
	virtual void ExecHardEvent(CNP* pNP);

	double m_dSafetySqdIncRadii;
	double m_dRadiusSumSqd;


    // common functionality for this and reactive hard interaction
    // is factored out into the following functions

    // Moves mols until (almost) touching prior to calculating
    // how their vels will change as a result of colliding:
    void MoveToTouching(CObjTypeMol* pMolType1,
                        CSObjMol* pMol1,
                        CObjTypeMol* pMolType2,
                        CSObjMol* pMol2,
                        CNP* pNP);

    // Calcs new trajectories after elastic collision, and forces
    // reconsideration of any affected events
    void CMolMolHardInt::DoCollisionAndCleanUp(CObjTypeMol* pMolType1, CSObjMol* pMol1,
                                           CObjTypeMol* pMolType2, CSObjMol* pMol2,
                                           CNP* pNP);


};

