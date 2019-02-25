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


#define PT_NO_HIT                    0
#define PT_HITS_LEFT_FACE            1
#define PT_HITS_RIGHT_FACE           2
#define PT_HITS_BOTTOM_FACE          3
#define PT_HITS_TOP_FACE             4
#define PT_OVERLAPS                  5
#define PT_HITS_TOP_LEFT_CORNER      6
#define PT_HITS_TOP_RIGHT_CORNER     7
#define PT_HITS_BOTTOM_RIGHT_CORNER  8
#define PT_HITS_BOTTOM_LEFT_CORNER   9

class CMolPistonHardInt : public CInt
{                           
public:
	CMolPistonHardInt();	
	virtual CInt* Clone();

	virtual void BuildNeighList();
	virtual void CalcAllHardEvents();
	virtual void CalcHardEvent(CNP* pNP);
	virtual void ExecHardEvent(CNP* pNP);

	double m_dMolRadiusSqd;  
	double m_dMolRadiusIncSafety;
	double m_dSafetySqdIncRadius;
};
