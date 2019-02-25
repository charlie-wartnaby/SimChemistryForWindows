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

#include "stdafx.h"
#include "simobjs.h"   
#include "sim.h"
#include "baseint.h"
#include "nullint.h"          


#ifdef _DEBUG
extern long global_debug_counter;
#endif


CNullInt::CNullInt()
{
	m_nBaseType1 = 0;
	m_nBaseType2 = 0;     
	m_nType1Mobile = FALSE;
	m_nType2Mobile = FALSE;
	m_nIsDefault = FALSE;
	m_nIsNull = TRUE;  
	m_nHasDetails = FALSE;
	m_pFirstNP = NULL;
	m_pLastNP = NULL;
	m_pFirstSpareNP = NULL;
	m_sName = "null";
	m_sDescription = "Objects do not interact at all; they pass straight through"
	                 " each other.";

	m_nIsHard = FALSE;
	m_nIsSoft = FALSE;
	m_pType1 = NULL;
	m_pType2 = NULL;
}      	                      

CInt* CNullInt::Clone()
{
	CInt* pInt = new CNullInt;
	pInt->m_pSim = m_pSim;
	return pInt;
}                                          
