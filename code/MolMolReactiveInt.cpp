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
#include "mol.h"    
#include "mmhrdint.h" 
#include "lennard.h"
#include "MolMolReactiveInt.h"
#include "ReactiveIntPropsDlg.h"

#include <math.h>

#ifdef _DEBUG
extern long global_debug_counter;
#endif



CMD_ENTRY CMolMolReactiveInt::m_celCMolMolReactiveIntCommands[MAX_COMMANDS_PER_OBJ] = 
{                                        
	{"objtype1",        CA_STRING, CT_STATE,   CI_OBJTYPE1,          LIM_MIN_CHARS_OBJTYPE,  LIM_MAX_CHARS_OBJTYPE},
	{"objtype2",        CA_STRING, CT_STATE,   CI_OBJTYPE2,          LIM_MIN_CHARS_OBJTYPE,  LIM_MAX_CHARS_OBJTYPE},
    {"products",        CA_INT,    CT_STATE,   CI_PRODUCTS,          1, 2      },
	{"barrier",         CA_DOUBLE, CT_STATE,   CI_BARRIER,           LIM_MIN_BARRIER,    LIM_MAX_BARRIER},
	{"product1",        CA_STRING, CT_STATE,   CI_PRODUCT1,          LIM_MIN_CHARS_OBJTYPE,  LIM_MAX_CHARS_OBJTYPE},
	{"product2",        CA_STRING, CT_STATE,   CI_PRODUCT2,          LIM_MIN_CHARS_OBJTYPE,  LIM_MAX_CHARS_OBJTYPE}, 
	{"end",             CA_NONE,   CT_ENDLIST, CI_ENDLIST,           0,    0   }
};
   

CMolMolReactiveInt::CMolMolReactiveInt()
{
	m_nBaseType1 = CI_MOL;
	m_nBaseType2 = CI_MOL;

	m_nType1Mobile = TRUE;
	m_nType2Mobile = TRUE;

	m_nIsDefault = FALSE;
	m_nIsNull = FALSE;      
	m_nHasDetails = TRUE;
	m_pFirstNP = NULL;
	m_pLastNP = NULL;
	m_pFirstSpareNP = NULL; 
	m_pType1 = NULL;
	m_pType2 = NULL;

	m_sName = "mol_mol_reactive_int";
	m_sDescription = "Molecules bounce off each other like billiard balls"
	                 " but can react to form different molecules if they"
                     " collide with sufficient energy.";

	m_nIsHard = TRUE;
	m_nIsSoft = FALSE;

    m_pProduct1Type = NULL;
    m_pProduct2Type = NULL;
    m_dBarrier = 0;
    m_nProducts = 1;
}

CInt* CMolMolReactiveInt::Clone()
{
	CMolMolReactiveInt* pInt = new CMolMolReactiveInt;
	pInt->m_pSim = m_pSim;
    pInt->m_pProduct1Type = m_pProduct1Type;
    pInt->m_pProduct2Type = m_pProduct2Type;
    pInt->m_sProduct1Name = m_sProduct1Name;
    pInt->m_sProduct2Name = m_sProduct2Name;
    pInt->m_nProducts = m_nProducts;
    pInt->m_dBarrier = m_dBarrier;
    pInt->m_dTotalBarrier = m_dTotalBarrier;
 
	return pInt;
}


CMD_ENTRY* CMolMolReactiveInt::GetCmdList()
{
	return &(m_celCMolMolReactiveIntCommands[0]);
}

int CMolMolReactiveInt::AcceptCommandLine(CString& remainingLine, CString& errString)
{
	int command;
	int ret;                                   
	CObjType* pType;
    
    ret = GrabCommand(remainingLine, errString, command);            
    while ((CI_EMPTY != command) && (SR_ERROR != ret))
    {
		switch (command)
		{     
			case CI_OBJTYPE1:
			case CI_OBJTYPE2:

				// check given name against current objects and set object
				// pointer as reqd  
				pType = m_pSim->GetObjTypeByName(m_sArg);
				
				if (NULL != pType)
				{
					if ((m_nIsNull || (pType->m_nBaseType == m_nBaseType1)) && (NULL == m_pType1))
					{
						// OK for our type 1  
						m_pType1 = pType;
					}
					else if ((m_nIsNull || (pType->m_nBaseType == m_nBaseType2)) && (NULL == m_pType2))
					{
						// OK for our type 2 
						m_pType2 = pType;
					}
					else
					{
						// we can't accept an object of that type   
						ret |= SR_ERROR;   
						errString += "can't use this type for this interaction:  "
						               + m_sArg;
					}
				}
				else
				{
					// no such type
					ret |= SR_ERROR;
					errString += "no such object type: " + m_sArg;
				}
				
				break;

            case CI_PRODUCTS:
                m_nProducts = m_nArg;
                break;

            case CI_PRODUCT1:
                pType = m_pSim->GetObjTypeByName(m_sArg);
                if (NULL == pType)
                {
                    ret |= SR_ERROR;
                    errString += "no such object type: " + m_sArg;
                }
                else if (pType->m_nBaseType != CI_MOL)
                {
                    ret |= SR_ERROR;
                    errString += "not a molecular type: " + m_sArg;
                }
                else
                {
                    m_pProduct1Type = (CObjTypeMol*) pType;
                    m_sProduct1Name = m_sArg;
                }
                break;

            case CI_PRODUCT2:
                pType = m_pSim->GetObjTypeByName(m_sArg);
                if (NULL == pType)
                {
                    ret |= SR_ERROR;
                    errString += "no such object type: " + m_sArg;
                }
                else if (pType->m_nBaseType != CI_MOL)
                {
                    ret |= SR_ERROR;
                    errString += "not a molecular type: " + m_sArg;
                }
                else
                {
                    m_pProduct2Type = (CObjTypeMol*) pType;
                    m_sProduct2Name = m_sArg;
                }
                break;
                
			case CI_BARRIER:
				m_dBarrier = m_dArg;
				break;
							
			default:
				ret |= SR_ERROR;
				errString += "confused near: " + remainingLine;
				break;
		}
		
        ret |= GrabCommand(remainingLine, errString, command);
    
	}              
	
	if ((NULL == m_pType1) || (NULL == m_pType2))
	{
		ret |= SR_ERROR;
		errString += "need both reactant types specified for interaction";
	}    
	else if (NULL == m_pProduct1Type)
    {
        ret |= SR_ERROR;
        errString += "at least product1 must be specified";
    }
    else
	{
		// get the reactant types in correct order
		int type1 = m_pSim->GetObjTypeIndexByName(m_pType1->m_sName);
		int type2 = m_pSim->GetObjTypeIndexByName(m_pType2->m_sName);
			
		if (type2 < type1)
		{
			CObjType* pSpareType = m_pType1;
			m_pType1 = m_pType2;
			m_pType2 = pSpareType;
		}

        if ((m_nProducts > 1) && (NULL == m_pProduct2Type))
        {
            ret |= SR_WARN;
            m_nProducts = 1;
            errString += "changed to 1 product as only one specified";
        }
	}
	  
	return ret;

}

int CMolMolReactiveInt::GetStateParam(int command)
{
	int ret = SR_OK;
	
	switch(command)
	{               
		case CI_OBJTYPE1:
			m_sArg = m_pType1->m_sName;
			break;
			
		case CI_OBJTYPE2:
			m_sArg = m_pType2->m_sName;
			break;    

        case CI_PRODUCTS:
            m_nArg = m_nProducts;
            break;

        case CI_PRODUCT1:
            m_sArg = m_pProduct1Type->m_sName;
            break;

        case CI_PRODUCT2:
            if (NULL != m_pProduct2Type)
            {
			    m_sArg = m_pProduct2Type->m_sName;
            }
            else
            {
                m_sArg = "";
            }
            break;

		case CI_BARRIER:
			m_dArg = m_dBarrier;
			break;
			
		default:        
			TRACE("Asked for param for unknown command in CInt::GetStateParam\n");
			ret = SR_ERROR;
			break;
	}             
	
	return ret;

}


void CMolMolReactiveInt::ExecHardEvent(CNP* pNP)
{
    // Suggested algorithm for A + B -> C + D, mass conserved:
    //
    // A and B positioned so just touching but still with original velocities.
    // Assuming that A will become C and B D, and that C is lighter than A,
    // the excess mass is broken off A and clumped onto B in an inelastic
    // absorption. Momentum is conserved and B slows down relative to A.
    // A's velocity does not change. The reaction has happened.
    // Then solve the resulting collision such that the final kinetic
    // energy is correct, still conserving momentum.
    //
    // Get to this point in normal hard collision:
  	// Multiplying out the brackets and using cos2 theta + sin2 theta = 1,
	// [but not now dividing through by p],
	// -2E = p^2/m1 + p^2/m2 + 2p(v2.rh) - 2p(v1.rh)  (rh = r / r, ie r hat)
    // leaving a quadratic to solve in p, where E is the potential energy
    // we have added after the collision (i.e. KE before = KE after + E)
    //
    // What does it mean if no solution? Rearranging to ax2 + bx + c = 0,
    // get
    //
    //     -2(v2-v1).rh +/- sqrt(4[(v2-v1).rh]^2 - 4.(1/m1 + 1/m2).2E
    // p = ----------------------------------------------------------
    //                          2(1/m1 + 1/m2)
    //
    //   (note: soln to p does not depend on ref frame, only relative vels reqd)
    // Considering when square root touches zero, for case where m1=m2
    // and v1 = -v2, and rh = 1 (i.e. head-on in x-direction)
    // 4[2v]^2 = 4.(1/m + 1/m).2E
    // 8v^2    = 16E/m
    //       E = mV^2 / 2       -- i.e. when E gets as big as total KE available
    //
    // But so long as we compute collision energy in zero-momentum frame,
    // won't do reaction unless sufficient energy.

    //
    // Suggested algorithm for A + B -> C:
    //
    // Cannot preserve energy and momentum in this case, in general.
    // For example, if A and B have equal mass and collide head-on with
    // equal and opposite velocities, C's final vel must be zero to
    // preserve momentum. But unless its increased potential energy happened
    // to equate with the absorbed kinetic energy, this cannot be so.
    // Just produce C with combined original momentum, then scale velocity
    // such that desired kinetic energy achieved.
    //
    // For users' experiments, probably better to conserve energy than
    // momentum, because may well be measuring heating effect in a closed volume
    // of substance, but less likely to be looking at details of individual
    // reactive scattering trajectories. Will just scale existing vel to
    // achieve desired energy change.
    //
    // For A + B -> C + D (+ E...?), mass not conserved:
    //
    // Want to keep energy if not momentum conservation. If fiddle vels before
    // collision to preserve energy, may have mols not colliding at all!
    // (eg if A just catching up B, then A goes very heavy, would have to
    // slow down to preserve KE, then doesn't hit B at all!)
    //
    // Could do standard elastic collision, fiddle masses and vels afterwards
    // to achieve correct energy. Or do mass-preserving reaction as far as
    // possible (but with mass transfer); e.g. aim firstly to get final masses
    // in correct ratio but with mass preserved, and afterwards scale both
    // masses to achieve correct total.

	CObjTypeMol* pMolType1 = (CObjTypeMol*) m_pType1;
	CObjTypeMol* pMolType2 = (CObjTypeMol*) m_pType2;

	CSObjMol* pMol1 = (CSObjMol*) pNP->m_pSObj1;
	CSObjMol* pMol2 = (CSObjMol*) pNP->m_pSObj2;

    MoveToTouching(pMolType1, pMol1, pMolType2, pMol2, pNP);

    // Work out if colliding with necessary violence to react. Calculate
    // total KE in zero momentum frame, taking only velocity components in direction
    // of collision, and compare with barrier to reaction.

    double m1 = pMolType1->m_dMass;
    double m2 = pMolType2->m_dMass;
    double totmass = m1 + m2;

    double totxmtm = pMol1->m_dVx * m1 + pMol2->m_dVx * m2; 
    double totymtm = pMol1->m_dVy * m1 + pMol2->m_dVy * m2;

    double zmtm_xvel = totxmtm / totmass;
    double zmtm_yvel = totymtm / totmass;

    double zmtm_xv1 = pMol1->m_dVx - zmtm_xvel;
    double zmtm_yv1 = pMol1->m_dVy - zmtm_yvel;
    double zmtm_xv2 = pMol2->m_dVx - zmtm_xvel;
    double zmtm_yv2 = pMol2->m_dVy - zmtm_yvel;

	// r is the vector from 1 to 2:
	double rx = pMol2->m_dX - pMol1->m_dX;
	double ry = pMol2->m_dY - pMol1->m_dY;	
	double rLength = sqrt(rx * rx + ry * ry);
	
	double nrx, nry;  // normalised version of r
	
	double dvx, dvy;  // vel of 2 rel to 1
	
	if (rLength > NEARLY_ZERO) // mols must not be coincident
    {
        nrx = rx / rLength;
        nry = ry / rLength;
    }
    else
    {
        // mols coincident, shouldn't get here anyway
        ASSERT(FALSE);
        nrx = 0;
        nry = 0;
    }

    // calc total kinetic energy in kJ/mol, taking dot prod of vels with
    // collision vector to get component only in normal direction between
    // mols

    double zmtm_v1_norm = zmtm_xv1 * nrx + zmtm_yv1 * nry;
    double zmtm_v2_norm = zmtm_xv2 * nrx + zmtm_yv2 * nry;

    TRACE("zmtm_v1_norm = %g, zmtm_v2_norm = %g\n", zmtm_v1_norm, zmtm_v2_norm);

    // energies in kJ/mol
    double totke = 0.5 * ONE_AMU * AVAGADRO * 1e-3 *
                          (m1 * zmtm_v1_norm * zmtm_v1_norm
                         + m2 * zmtm_v2_norm * zmtm_v2_norm);

    double totke_simframe = 0.5 * ONE_AMU * AVAGADRO * 1e-3 *
                          (m1 * (pMol1->m_dVx * pMol1->m_dVx + pMol1->m_dVy * pMol1->m_dVy)
                         + m2 * (pMol2->m_dVx * pMol2->m_dVx + pMol2->m_dVy * pMol2->m_dVy));

    TRACE("m1=%g pMol1->m_dVx=%g pMol1->m_dVy=%g m2=%g pMol2->m_dVx=%g pMol2->m_dVy=%g\n",
          m1, pMol1->m_dVx, pMol1->m_dVy, m2, pMol2->m_dVx, pMol2->m_dVy);

    double oldpe = pMolType1->m_dFormationEnergy + pMolType2->m_dFormationEnergy;
    double newpe = m_pProduct1Type->m_dFormationEnergy;

    if (m_nProducts > 1)
    {
        if (NULL == m_pProduct2Type)
        {
            ASSERT(FALSE);
            return;
        }

        newpe += m_pProduct2Type->m_dFormationEnergy;
    }

    double dpe = newpe - oldpe; // increase in potl energy if react

    double totbarrier = m_dBarrier;

    if (dpe > 0)
    {
        totbarrier += dpe;
    }

    if (totke < totbarrier)
    {
        // not enough energy to react, so just do normal elastic hard collision
        DoCollisionAndCleanUp(pMolType1, pMol1, pMolType2, pMol2, pNP);
    }
    else
    {
        // mols have sufficient energy in z-mtm frame to react so do it
        TRACE("About to do a molecular reaction:\n");
        TRACE("Mol1 m=%g x=%g y=%g vx=%g vy=%g r=%g pe=%g\n",
              pMolType1->m_dMass,
              pMol1->m_dX,
              pMol1->m_dY,
              pMol1->m_dVx,
              pMol1->m_dVy,
              pMolType1->m_dHSRadius,
              pMolType1->m_dFormationEnergy);
        TRACE("Mol2 m=%g x=%g y=%g vx=%g vy=%g r=%g pe=%g\n",
              pMolType2->m_dMass,
              pMol2->m_dX,
              pMol2->m_dY,
              pMol2->m_dVx,
              pMol2->m_dVy,
              pMolType2->m_dHSRadius,
              pMolType2->m_dFormationEnergy);

        // Calc spare energy in simulation reference frame
        //          J      =     kJ/mol    / (N/mol)  * 1000
        double molec_spare_energy = (totke_simframe - dpe) / AVAGADRO * 1000;

        TRACE("totke(zmtm) = %g totke(sim) = %g molec_spare_energy (sim) = %g\n",
               totke, totke_simframe, molec_spare_energy);

        if (molec_spare_energy < 0)
        {
            // shouldn't have reacted in the first place!
            ASSERT(FALSE);
            molec_spare_energy = 0;
        }

        // To minimise risk of creating mol
        // overlapping Cartesian-aligned walls etc, may need to place 
        // product molecule(s) in centre of
        // rectangle defined by extreme coordinates of reactants:
        double xmin1 = pMol1->m_dX - pMolType1->m_dHSRadius;
        double xmin2 = pMol2->m_dX - pMolType2->m_dHSRadius;
        double xmax1 = pMol1->m_dX + pMolType1->m_dHSRadius;
        double xmax2 = pMol2->m_dX + pMolType2->m_dHSRadius;
        double ymin1 = pMol1->m_dY - pMolType1->m_dHSRadius;
        double ymin2 = pMol2->m_dY - pMolType2->m_dHSRadius;
        double ymax1 = pMol1->m_dY + pMolType1->m_dHSRadius;
        double ymax2 = pMol2->m_dY + pMolType2->m_dHSRadius;

        double xcentre = (MIN(xmin1, xmin2) + MAX(xmax1, xmax2)) / 2;
        double ycentre = (MIN(ymin1, ymin2) + MAX(ymax1, ymax2)) / 2;

        TRACE("Centre of collision rectangle at (%g,%g)\n", xcentre, ycentre);

        switch (m_nProducts) // room for future expansion!
        {

        case 1:  // i.e. only one product molecule
            {
                // new block to allow local vars in this case clause


                // m/s    = sqrt (2 * J / kg)
                double new_speed = sqrt(2 * molec_spare_energy / (m_pProduct1Type->m_dMass * ONE_AMU));
                TRACE("1 product mol, speed will be %g\n", new_speed);

                // work out where to put new molecule; best if can place where
                // one of the reactants was, so guaranteeing not to overlap
                // any other objects (assuming the reactant wasn't already)
                double x, y;

                if (m_pProduct1Type->m_dHSRadius <= pMolType1->m_dHSRadius)
                {
                    // will fit where product 1 was
                    x = pMol1->m_dX;
                    y = pMol1->m_dY;
                    TRACE("Fitting it where mol 1 was\n");
                }
                else if (m_pProduct1Type->m_dHSRadius <= pMolType2->m_dHSRadius)
                {
                    // will fit where product 1 was
                    x = pMol2->m_dX;
                    y = pMol2->m_dY;
                    TRACE("Fitting it where mol 2 was\n");
                }
                else
                {
                    // doesn't fit either one. To minimise risk of creating mol
                    // overlapping Cartesian-aligned walls etc, place in centre of
                    // rectangle defined by extreme coordinates of reactants

                    x = xcentre;
                    y = ycentre;
                    TRACE("Doesn't fit where either mol was, going for centre\n");
                }

                // create new mol with energy conserved and going in direction of
                // original zero-momentum frame wrt simulation frame; momentum will
                // not generally be conserved
                CSObjMol* pNewMol = m_pProduct1Type->CreateNewMol(x, y, new_speed,
                                              atan2(zmtm_yvel, zmtm_xvel));

                TRACE("Product mol m=%g x=%g y=%g vx=%g vy=%g r=%g pe=%g\n",
                      m_pProduct1Type->m_dMass,
                      pNewMol->m_dX,
                      pNewMol->m_dY,
                      pNewMol->m_dVx,
                      pNewMol->m_dVy,
                      m_pProduct1Type->m_dFormationEnergy);

               if (NULL != pNewMol)
               {
                   pNewMol->m_nTouched = TRUE;
                   pNewMol->m_dTimeDoneAlready = m_pSim->m_dTimeSoFar;
               }
            }
            break;

        case 2: // i.e. two product molecules

            { // new block for local vars within case clause
                
                // Firstly transfer mass from one molecule to another, if necessary,
                // and have it inelastically absorbed by the recipient. If mass cannot
                // be conserved for this reaction, aim for the two product molecules
                // to have the desired ratio of final masses in the first instance.
                // Which reactant turns into which product is first decided by
                // seeing which fits into which space; if the products fit into the
                // holes left by the reactants, so much the better. If neither does
                // anyway, have the heavier reactant turn into the heavier product.

                // see what fits where
                int nProd1FitsReactant1 = m_pProduct1Type->m_dHSRadius <= pMolType1->m_dHSRadius;
                int nProd1FitsReactant2 = m_pProduct1Type->m_dHSRadius <= pMolType2->m_dHSRadius;
                int nProd2FitsReactant1 = m_pProduct2Type->m_dHSRadius <= pMolType1->m_dHSRadius;
                int nProd2FitsReactant2 = m_pProduct2Type->m_dHSRadius <= pMolType2->m_dHSRadius;

                // count which permutation gets most fitting of prods into reactants
                int nFitsIf11and22 = nProd1FitsReactant1 + nProd2FitsReactant2;
                int nFitsIf12and21 = nProd1FitsReactant2 + nProd2FitsReactant1;

                enum {TIE_SOME_FIT, R_BETTER_1122, R_BETTER_1221, NO_FITS} nRadiusFit;

                if (nFitsIf11and22 > nFitsIf12and21)
                {
                    nRadiusFit = R_BETTER_1122;
                }
                else if (nFitsIf12and21 > nFitsIf11and22)
                {
                    nRadiusFit = R_BETTER_1221;
                }
                else
                {
                    if (nFitsIf11and22 > 0)
                    {
                        nRadiusFit = TIE_SOME_FIT;
                    }
                    else
                    {
                        nRadiusFit = NO_FITS;
                    }
                }
                
                enum {M_BETTER_1122, M_BETTER_1221, DONT_CARE} nMassFit;

                if ((fabs(m1 - m2) < NEARLY_ZERO) ||
                    (fabs(m_pProduct1Type->m_dMass - m_pProduct2Type->m_dMass) < NEARLY_ZERO))
                {
                    nMassFit = DONT_CARE;
                }
                else if (m1 > m2)
                {
                    if (m_pProduct1Type->m_dMass > m_pProduct2Type->m_dMass)
                    {
                        nMassFit = M_BETTER_1122;
                    }
                    else
                    {
                        nMassFit = M_BETTER_1221;
                    }
                }
                else
                {
                    if (m_pProduct1Type->m_dMass < m_pProduct2Type->m_dMass)
                    {
                        nMassFit = M_BETTER_1122;
                    }
                    else
                    {
                        nMassFit = M_BETTER_1221;
                    }
                }

                int nMap1To1;

                switch (nRadiusFit)
                {
                case R_BETTER_1122:
                    nMap1To1 = TRUE;
                    break;
                case R_BETTER_1221:
                    nMap1To1 = FALSE;
                    break;
                case TIE_SOME_FIT:
                case NO_FITS:
                    nMap1To1 = (M_BETTER_1122 == nMassFit) || (DONT_CARE == nMassFit);
                    break;
                default:
                    ASSERT(FALSE);
                    nMap1To1 = TRUE;
                    break;
                }

                TRACE("Two product mols, nMap1To1=%d\n", nMap1To1);

                double dProdMassRatio = m_pProduct1Type->m_dMass / m_pProduct2Type->m_dMass;
                double dReactantMassRatio = nMap1To1 ? m1 / m2 : m2 / m1;

                TRACE("Prod mass ratio 1:2 = %g, reactants 1:2/2:1 = %g\n", 
                        dProdMassRatio, dReactantMassRatio);

                // KE of reactants after any mass transfer (kJ/mol):
                double new_totke_simframe = 0;

                if (fabs(dProdMassRatio - dReactantMassRatio) > NEARLY_ZERO)
                {
                    // the mass ratios of products and reactants are unequal, so do
                    // mass transfer just before collision to get reactant ratio right.
                    // Notionally transfer a fragment of over-heavy molecule to lighter one,
                    // transferring the momentum of that fragment.
                    //
                    // If desired mass ratio is k and need to subtract d from m1 and add
                    // it to m2, then
                    //
                    //      (m1 - d)/(m2 + d) = k
                    //
                    //       ->             d = (m1 - km2)/(k + 1)

                    double dTargetReactant1Mass = dProdMassRatio * m2;
                    int nTransferFrom1To2 = (dReactantMassRatio > dProdMassRatio);

                    double dDeltaMass;
                    double dFragmentXMtm, dFragmentYMtm;
                    double dTotXMtm, dTotYMtm;

                    if (nTransferFrom1To2)
                    {
                        dDeltaMass = (m1 - dProdMassRatio * m2) / (dProdMassRatio + 1);
                        TRACE("Transferring mass from 1 to 2, dm = %g\n", dDeltaMass);
                        dFragmentXMtm = pMol1->m_dVx * dDeltaMass;
                        dFragmentYMtm = pMol1->m_dVy * dDeltaMass;
                        dTotXMtm = pMol2->m_dVx * m2 + dFragmentXMtm;
                        dTotYMtm = pMol2->m_dVy * m2 + dFragmentYMtm;
                        m2 += dDeltaMass;
                        pMol2->m_dVx = dTotXMtm / m2;
                        pMol2->m_dVy = dTotYMtm / m2;
                        m1 -= dDeltaMass;
                    }
                    else
                    {
                        dDeltaMass = -(m1 - dProdMassRatio * m2) / (dProdMassRatio + 1);
                        TRACE("Transferring mass from 2 to 1, dm = %g\n", dDeltaMass);
                        dFragmentXMtm = pMol2->m_dVx * dDeltaMass;
                        dFragmentYMtm = pMol2->m_dVy * dDeltaMass;
                        dTotXMtm = pMol1->m_dVx * m1 + dFragmentXMtm;
                        dTotYMtm = pMol1->m_dVy * m1 + dFragmentYMtm;
                        m1 += dDeltaMass;
                        pMol1->m_dVx = dTotXMtm / m1;
                        pMol1->m_dVy = dTotYMtm / m1;
                        m2 -= dDeltaMass;
                    }

                    // In general we will have lost kinetic energy in that inelastic
                    // mass transfer, so using up some of the spare KE we had to play
                    // with in the collision. Calc new KE:
                    new_totke_simframe = 0.5 * ONE_AMU * AVAGADRO * 1e-3 *
                          (m1 * (pMol1->m_dVx * pMol1->m_dVx + pMol1->m_dVy * pMol1->m_dVy)
                         + m2 * (pMol2->m_dVx * pMol2->m_dVx + pMol2->m_dVy * pMol2->m_dVy));
                }

                double dSpentEnergy = totke_simframe - new_totke_simframe;
                ASSERT(dSpentEnergy >= 0);
                TRACE("After any mass transfer, spent %g reducing KE from %g to %g, now:\n",
                       dSpentEnergy, totke_simframe, new_totke_simframe);

                TRACE("Mol1 m=%g x=%g y=%g vx=%g vy=%g r=%g pe=%g\n",
                      m1,
                      pMol1->m_dX,
                      pMol1->m_dY,
                      pMol1->m_dVx,
                      pMol1->m_dVy,
                      pMolType1->m_dHSRadius,
                      pMolType1->m_dFormationEnergy);
                TRACE("Mol2 m=%g x=%g y=%g vx=%g vy=%g r=%g pe=%g\n",
                      m2,
                      pMol2->m_dX,
                      pMol2->m_dY,
                      pMol2->m_dVx,
                      pMol2->m_dVy,
                      pMolType2->m_dHSRadius,
                      pMolType2->m_dFormationEnergy);

                ASSERT(fabs(m1/m2 - dProdMassRatio) < 1e-6 /* NEARLY_ZERO seems to strict here */);

                // Reactant mols now have correct mass ratio (m1 : m2) to map to products
                // and the lighter one has absorbed the momentum of a fragment of the heavier
                // one to achieve this if necessary. Now do inelastic collision so that
                // momentum and energy are conserved and the correct final potential energy is
                // achieved.

                //     -2(v2-v1).rh +/- sqrt(4[(v2-v1).rh]^2 - 4.(1/m1 + 1/m2).2E
                // p = ----------------------------------------------------------
                //                          2(1/m1 + 1/m2)

	            
		        dvx = pMol1->m_dVx - pMol2->m_dVx;
		        dvy = pMol1->m_dVy - pMol2->m_dVy;
                
                double inv_mass_sum = 1.0 / (ONE_AMU * m1) + 1.0 / (ONE_AMU * m2);
                double dot_prod = dvx * nrx + dvy * nry;
                double root_term = dot_prod * dot_prod -
                             inv_mass_sum * 2 * (newpe - oldpe - dSpentEnergy) / AVAGADRO * 1000;

                if (root_term < 0)
                {
                    ASSERT(FALSE);
                    root_term = NEARLY_ZERO;
                }

                // impulse here in kg.m/s (c.f. mol-mol hard int, amu.m/s)
                double p = (-dot_prod - sqrt(root_term)) / inv_mass_sum;

                double dv1 = p / (m1 * ONE_AMU);
	            double dv2 = -p / (m2 * ONE_AMU);  
	            
                TRACE("inv_mass_sum=%g, dot_prod=%g, root_term=%g, p=%g, dv1=%g, dv2=%g\n",
                       inv_mass_sum,    dot_prod,    root_term,    p,    dv1,    dv2);

	            pMol1->m_dVx += dv1 * nrx;
	            pMol1->m_dVy += dv1 * nry;
	            pMol2->m_dVx += dv2 * nrx;
	            pMol2->m_dVy += dv2 * nry;

                TRACE("Mol1 m=%g x=%g y=%g vx=%g vy=%g r=%g pe=%g\n",
                      m1,
                      pMol1->m_dX,
                      pMol1->m_dY,
                      pMol1->m_dVx,
                      pMol1->m_dVy,
                      pMolType1->m_dHSRadius,
                      pMolType1->m_dFormationEnergy);
                TRACE("Mol2 m=%g x=%g y=%g vx=%g vy=%g r=%g pe=%g\n",
                      m2,
                      pMol2->m_dX,
                      pMol2->m_dY,
                      pMol2->m_dVx,
                      pMol2->m_dVy,
                      pMolType2->m_dHSRadius,
                      pMolType2->m_dFormationEnergy);

               // position new mols to fit into reactant space if poss, and
               // if not so as to fit in rectangle defined by reactants
               // now transmute the mols into the product species

               double x1, y1, x2, y2;
               double dSpeed1, dSpeed2;
               double dAngle1, dAngle2;

               if (nMap1To1)
               {
                    dSpeed1 = sqrt(pMol1->m_dVx * pMol1->m_dVx + pMol1->m_dVy * pMol1->m_dVy);
                    dSpeed2 = sqrt(pMol2->m_dVx * pMol2->m_dVx + pMol2->m_dVy * pMol2->m_dVy);

                    if (nProd1FitsReactant1)
                    {
                        x1 = pMol1->m_dX;
                        y1 = pMol1->m_dY;
                    }
                    else
                    {
                        x1 = xcentre;
                        y1 = ycentre;
                    }

                    if (nProd2FitsReactant2)
                    {
                        x2 = pMol2->m_dX;
                        y2 = pMol2->m_dY;
                    }
                    else
                    {
                        x2 = xcentre;
                        y2 = ycentre;
                    }

                    dAngle1 = atan2(pMol1->m_dVy, pMol1->m_dVx);
                    dAngle2 = atan2(pMol2->m_dVy, pMol2->m_dVx);
               }
               else
               {
                    dSpeed2 = sqrt(pMol1->m_dVx * pMol1->m_dVx + pMol1->m_dVy * pMol1->m_dVy);
                    dSpeed1 = sqrt(pMol2->m_dVx * pMol2->m_dVx + pMol2->m_dVy * pMol2->m_dVy);

                    if (nProd1FitsReactant2)
                    {
                        x1 = pMol2->m_dX;
                        y1 = pMol2->m_dY;
                    }
                    else
                    {
                        x1 = xcentre;
                        y1 = ycentre;
                    }

                    if (nProd2FitsReactant1)
                    {
                        x2 = pMol1->m_dX;
                        y2 = pMol1->m_dY;
                    }
                    else
                    {
                        x2 = xcentre;
                        y2 = ycentre;
                    }

                    dAngle2 = atan2(pMol1->m_dVy, pMol1->m_dVx);
                    dAngle1 = atan2(pMol2->m_dVy, pMol2->m_dVx);
               }

               
               // see how much KE we've ended up with and speed up or slow
               // down the new mols to conserve energy (shouldn't have to
               // if mass conserved by the reaction)

               //   J / molec pair = ...
               double new_molec_ke = 0.5 * ONE_AMU *
                                 (m_pProduct1Type->m_dMass * dSpeed1 * dSpeed1 +
                                        m_pProduct2Type->m_dMass * dSpeed2 * dSpeed2);

               double dSurplusKEFactor = sqrt(new_molec_ke / molec_spare_energy);

               TRACE("New KE=%g, old KE=%g, surplus factor=%g\n",
                      new_molec_ke, molec_spare_energy, dSurplusKEFactor);

               dSpeed1 /= dSurplusKEFactor;
               dSpeed2 /= dSurplusKEFactor;

               
               // Get MSVC 5.0 internal compiler error if next two lines in original order!!
               // (only for release build)
               CSObjMol* pProdMol2 = m_pProduct2Type->CreateNewMol(x2, y2, dSpeed2, dAngle2);
               CSObjMol* pProdMol1 = m_pProduct1Type->CreateNewMol(x1, y1, dSpeed1, dAngle1);

               TRACE("Reactive Int created mols %p and %p\n", pProdMol1, pProdMol2);
                TRACE("Mol1 m=%g x=%g y=%g vx=%g vy=%g r=%g pe=%g\n",
                      m_pProduct1Type->m_dMass,
                      pProdMol1->m_dX,
                      pProdMol1->m_dY,
                      pProdMol1->m_dVx,
                      pProdMol1->m_dVy,
                      m_pProduct1Type->m_dHSRadius,
                      m_pProduct1Type->m_dFormationEnergy);
                TRACE("Mol2 m=%g x=%g y=%g vx=%g vy=%g r=%g pe=%g\n",
                      m_pProduct2Type->m_dMass,
                      pProdMol2->m_dX,
                      pProdMol2->m_dY,
                      pProdMol2->m_dVx,
                      pProdMol2->m_dVy,
                      m_pProduct2Type->m_dHSRadius,
                      m_pProduct2Type->m_dFormationEnergy);

               // calc impulsive contribution to pressure (will be a bit meaningless
               // if radii or masses not preserved, but never mind because won't be
               // expecting system to behave well in that case):
               double dImpulseContrib = p / ONE_AMU * (nrx * rx + nry * ry) / 2;

               // having created mols, must take account of them appearing part way through
               // timestep, otherwise will move impossibly far when shifted to synchronize with
               // other sim objects:


               if (NULL != pProdMol1)
               {
                   pProdMol1->m_nTouched = TRUE;
                   pProdMol1->m_dTimeDoneAlready = m_pSim->m_dTimeSoFar;
                   pProdMol1->m_dImpulseSum = dImpulseContrib;
               }

               if (NULL != pProdMol2)
               {
                   pProdMol2->m_nTouched = TRUE;
                   pProdMol2->m_dTimeDoneAlready = m_pSim->m_dTimeSoFar;
                   pProdMol2->m_dImpulseSum = dImpulseContrib;
               }


            } // end of 2 prod mols block
            break;

        default:  // unhandled number of product molecules
            ASSERT(FALSE);
            break;

        } // end of switch (number of prod molecules)

        // now destroy reactant molecules
        TRACE("Reactive int destroying mols %p and %p\n", pMol1, pMol2);

        delete pMol1;
        delete pMol2;

        // reacting species will probably have been referenced by other
        // CNP objects etc, so force rethink of all interactions
        m_pSim->m_dSafetyDistLeft = 0; // kludge to force neighbour rebuild
        m_pSim->InvalidateAllHardEvents(); 

    }  // end of if (have enough energy to react)

}

int CMolMolReactiveInt::CheckNotReferencingDeadTypes()
{
    // Interactions normally reference only those object types involved
    // directly in the interaction, but this is an exception; we may
    // be referring to mol types as products which may get deleted. Check
    // if this is the case, and if so replace ourselves with standard
    // mol-mol hard interaction.

    if ((NULL == m_pSim->GetObjTypeByName(m_sProduct1Name)) ||
        ((2 == m_nProducts) && (NULL == m_pSim->GetObjTypeByName(m_sProduct2Name))))
    {
        // Our product types are no longer valid

        CString cmd("interaction mol_mol_hard_int objtype1 \"");

        cmd += m_pType1->m_sName;

        cmd += "\" objtype2 \"";

        cmd += m_pType2->m_sName;

        cmd += "\"";

        m_pSim->HandleNewCommand(cmd);

        return SR_WARN;
    }
    else
    {
        return SR_OK;
    }

}

void CMolMolReactiveInt::ContributeNumerics()
{                 
	m_numBarrier.m_pSim = m_pSim;
	m_numBarrier.m_sName = "interaction:" + m_pType1->m_sName + ":"
	                            + m_pType2->m_sName + ":barrier";
	m_numBarrier.m_sUnits = "kJ/mol";
	m_numBarrier.m_sDescription = "activation barrier";
	m_numBarrier.m_pdValue = &m_dBarrier; 
	m_numBarrier.m_nType = NUM_USERWRITE | NUM_USERREAD;
	m_numBarrier.m_dMin = LIM_MIN_BARRIER;
	m_numBarrier.m_dMax = LIM_MAX_BARRIER;
	m_pSim->AcceptNewNumeric(&m_numBarrier);
}

int CMolMolReactiveInt::EditDetails()
{
    CReactiveIntPropsDlg dlg;

    dlg.m_pSim = m_pSim;
    dlg.m_pInt = this;

    if (NULL != m_pProduct1Type)
    {
        dlg.m_sProduct1 = m_pProduct1Type->m_sName;
    }
    else
    {
        dlg.m_sProduct1 = "";
    }

    if (NULL != m_pProduct2Type)
    {
        dlg.m_sProduct2 = m_pProduct2Type->m_sName;
    }
    else
    {
        dlg.m_sProduct2 = "";
    }

    dlg.m_nTwoProducts = (2 == m_nProducts);

    dlg.m_dBarrier = m_dBarrier;

    int ret = dlg.DoModal();

    if (IDOK == ret)
    {
        m_dBarrier = dlg.m_dBarrier;

        m_pProduct1Type = (CObjTypeMol*) m_pSim->GetObjTypeByName(dlg.m_sProduct1);
        m_pProduct2Type = (CObjTypeMol*) m_pSim->GetObjTypeByName(dlg.m_sProduct2);

        ASSERT(NULL != m_pProduct1Type);
        ASSERT(NULL != m_pProduct2Type);

        m_nProducts = (dlg.m_nTwoProducts ? 2 : 1);

        if ((NULL == m_pProduct1Type)
            || (NULL == m_pProduct1Type && 2 == m_nProducts))
        {
            ret = IDCANCEL;
        }
        else
        {
            // Work out possible reverse command first, because this instance of
            // the interaction is destroyed when command passed to CSim!!

            CString cmd_reverse;
            CSim* pSim = m_pSim;

            if (2 == m_nProducts && dlg.m_nDoReverse)
            {
                CMolMolReactiveInt* pReverseInt = (CMolMolReactiveInt*) Clone();

                pReverseInt->m_pType1 = m_pProduct1Type;
                pReverseInt->m_pType2 = m_pProduct2Type;
                pReverseInt->m_pProduct1Type = (CObjTypeMol*) m_pType1;
                pReverseInt->m_pProduct2Type = (CObjTypeMol*) m_pType2;

                cmd_reverse = "interaction " + pReverseInt->m_sName + " ";
                pReverseInt->WriteState(cmd_reverse);

                delete pReverseInt;
            }

            CString cmd = "interaction " + m_sName + " ";    
		    WriteState(cmd);
		    m_pSim->HandleNewCommand(cmd); // this now destroyed
            
            if (cmd_reverse != "")
            {
                pSim->HandleNewCommand(cmd_reverse);
            }

        }
    }

    return ret;
}
