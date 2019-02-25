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

// hints for what sort of collision between mol and wall  
#define WT_NO_HIT            (-1)
#define WT_HITS_MIDDLE          0
#define WT_END1                 1
#define WT_END2                 2
#define WT_END_UNKNOWN (WT_END1 | WT_END2)


class CWallMolDefaultInt : public CInt
{                           
public:
	CWallMolDefaultInt();	
	virtual CInt* Clone();

	virtual void BuildNeighList();
	virtual void CalcAllHardEvents();
	virtual void CalcHardEvent(CNP* pNP);
	virtual void ExecHardEvent(CNP* pNP);

	double m_dMolRadiusSqd;
	double m_dSafetySqdIncRadius;
};

