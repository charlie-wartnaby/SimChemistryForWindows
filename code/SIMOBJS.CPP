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
#include "tracker.h"
#include "lennard.h"  
#include "lendoc.h" 
#include "simvw.h"
#include "baseint.h"

   
#include <stdlib.h>                                   
#include <limits.h>     
#include <time.h>                       
#include <math.h>  


// limits.h doesn't seem to have these, in debug anyway:
#ifndef INT_MIN
#define INT_MIN (-32768)
#define INT_MAX 32767
#endif


#ifdef _DEBUG
// If some anomalous situation occurs in simulation, can
// set breakpoint on this counter reaching a value just before the
// bad thing happens (by trial and error) to catch it.
long global_debug_counter = 0;
#endif

//////////////////////////////////////////////////////////////////////
// shared static parameter values for object saving and dumping       
//////////////////////////////////////////////////////////////////////                                 
double        CSimObj::m_dArg;    
CString       CSimObj::m_sArg;                         
unsigned char CSimObj::m_bArg;
int           CSimObj::m_nArg;
long          CSimObj::m_lArg;

//////////////////////////////////////////////////////////////////////                                 
// generic CSimObj functionality
//////////////////////////////////////////////////////////////////////                                 
CSimObj::~CSimObj()
{
}

int CSimObj::WriteState(CString& workingLine)
{                                            
	// Go through the list of state commands known to this object
	// and list their names and properties 
	
	CMD_ENTRY* celList = GetCmdList();       
	
	int ret = SR_OK;            
	CString string;
	int i =  0;    

	while (CI_ENDLIST != celList[i].cmd_id)
	{         
		if (CT_STATE == celList[i].cmd_type)
		{  
			if (SR_OK == GetStateParam(celList[i].cmd_id))  // should be, unless command not handled by mistake
			{
				workingLine += celList[i].command;
				workingLine += " ";
				switch (celList[i].arg_type)
				{
					case CA_DOUBLE:
						sprintf(buff, "%g ", m_dArg);
						workingLine += buff;
						break;
					
					case CA_NONE:
						break;        
						
					case CA_STRING:
						workingLine += "\"" + m_sArg + "\" ";
						break;    
						
					case CA_BYTE:
						sprintf(buff, "%d ", m_bArg);
						workingLine += buff;
						break;
						
					case CA_INT:
						sprintf(buff, "%d ", m_nArg);
						workingLine += buff;
						break;
						
					case CA_LONG:
						sprintf(buff, "%ld ", m_lArg);
						workingLine += buff;
						break;
						
					default:
						ASSERT(FALSE);
						break;
				}
			}    
			else
			{
				// problem with a command
				ret = SR_ERROR;
			}
		
		}       
		i++;
	}           
	
	return ret;
}

int CSimObj::WriteCmdToDisk(CStdioFile& file, CString& outString)
{            
	// This function should trap disk exceptions and format
	// the file either for easy e-mailing or easy spreadsheet
	// importation. For now it simply writes. 
	
	CString copyString(outString);
	
	if (copyString.IsEmpty())
	{
		// blank line
		copyString = "\n";
	}
	else
	{	
		// append semicolon to command and write it 
		copyString += ";\n";
	}

	file.WriteString(copyString);                
	
	return SR_OK;
}

void CSimObj::ContributeNumerics()
{
	// default is to have no numerics to add to master list
}
                                        
int CSimObj::GrabCommand(CString& remainingLine, CString& errString,
                         int& cmd_id_ret, CMD_ENTRY** ppCmdEntry)
{
	CMD_ENTRY* celList = GetCmdList();
	int i = 0;
	CString listedCommand;
	int nCmdFound = FALSE;     
	int ret = SR_OK;           
	CString word;  
	
	
	PopWord(remainingLine, word);  
	
	if (-1 == word.FindOneOf("abcdefghijklmnopqrstuvwxyz"
	                         "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	                         "0123456789\""))
	{
		nCmdFound = FALSE;  // nothing there  
		cmd_id_ret = CI_EMPTY;
	}
	else
	{
		nCmdFound = FALSE;  
		cmd_id_ret = CI_UNIDENTIFIED;
		ret = SR_ERROR;

		while (CI_ENDLIST != celList[i].cmd_id)
		{         
			// for now, commands must be exact match, but in future
			// could allow abbreviation up to the point of ambiguity
			if (celList[i].command == word)
			{  
				cmd_id_ret = celList[i].cmd_id;  
				nCmdFound = TRUE;   
				ret = SR_OK;
				if (NULL != ppCmdEntry)
				{
					*ppCmdEntry = &(celList[i]);
				}
				break;
			}       
			i++;
		}
	} 

	double value;   
	int nLength;  
	int nMaxLength;
	int nMinLength;
		
	if (nCmdFound)
	{
		switch (celList[i].arg_type)
		{    
			case CA_DOUBLE:
				PopWord(remainingLine, word);
				value = atof(word);
				if (value > celList[i].hi_lim)
				{           
					ret = SR_WARN;
					m_dArg = celList[i].hi_lim;
					sprintf(buff, "Warning: %f clipped to %f after %s; ",
					        value, celList[i].hi_lim, celList[i].command);
					errString += buff;
				}
				else if (value < celList[i].lo_lim)
				{
					ret = SR_WARN;
					m_dArg = celList[i].lo_lim;
					sprintf(buff, "Warning: %f clipped to %f after %s; ",
					        value, celList[i].lo_lim, celList[i].command);
					errString += buff;
				}
				else
				{
					m_dArg = value;
				}
				break;
			
			case CA_STRING:
				PopWord(remainingLine, word);
				nLength = word.GetLength();  
				nMaxLength = (int) celList[i].hi_lim;
				nMinLength = (int) celList[i].lo_lim;
				if (nLength > nMaxLength)
				{           
					ret = SR_WARN;
					m_sArg = word.Left(nMaxLength);
					sprintf(buff, "Warning: \"%s\" clipped to \"%s\" after %s; ",
					        (const char*) word, (const char*) m_sArg, celList[i].command);
					errString += buff;
				}
				else if (nLength < nMinLength)
				{
					ret = SR_WARN; 
					m_sArg = word;
					while (m_sArg.GetLength() < nMinLength)
					{
						m_sArg += "z";
					}
					sprintf(buff, "Warning: \"%s\" extended to \"%s\" after %s; ",
					        (const char*) word, (const char*) m_sArg, celList[i].command);
					errString += buff;
				}
				else
				{
					m_sArg = word;
				}
				break; 
				
			case CA_BYTE:
				PopWord(remainingLine, word);
				value = atof(word);   
				ASSERT (celList[i].hi_lim <= 255);
				ASSERT (celList[i].lo_lim >= 0);
				if (value > celList[i].hi_lim)
				{
					ret = SR_WARN;
					m_bArg = (unsigned char) celList[i].hi_lim;
					sprintf(buff, "Warning: %f clipped to %f after %s; ",
					        value, celList[i].hi_lim, celList[i].command);
					errString += buff;
				}
				else if (value < celList[i].lo_lim)
				{
					ret = SR_WARN;
					m_bArg = (unsigned char) celList[i].lo_lim;
					sprintf(buff, "Warning: %f clipped to %f after %s; ",
					        value, celList[i].lo_lim, celList[i].command);
					errString += buff;
				}
				else
				{
					m_bArg = (unsigned char) value;
				}
				break;     
				
			case CA_INT:
				PopWord(remainingLine, word);
				value = atof(word);   
				ASSERT (celList[i].hi_lim <= INT_MAX);
				ASSERT (celList[i].lo_lim >= INT_MIN);
				if (value > celList[i].hi_lim)
				{
					ret = SR_WARN;
					m_nArg = (int) celList[i].hi_lim;
					sprintf(buff, "Warning: %f clipped to %f after %s; ",
					        value, celList[i].hi_lim, celList[i].command);
					errString += buff;
				}
				else if (value < celList[i].lo_lim)
				{
					ret = SR_WARN;
					m_nArg = (int) celList[i].lo_lim;
					sprintf(buff, "Warning: %f clipped to %f after %s; ",
					        value, celList[i].lo_lim, celList[i].command);
					errString += buff;
				}
				else
				{
					m_nArg = (int) value;
				}
				break;     
				
			case CA_LONG:
				PopWord(remainingLine, word);
				value = atof(word);   
				ASSERT (celList[i].hi_lim <= LONG_MAX);
				ASSERT (celList[i].lo_lim >= LONG_MIN);
				if (value > celList[i].hi_lim)
				{
					ret = SR_WARN;
					m_lArg = (long) celList[i].hi_lim;
					sprintf(buff, "Warning: %f clipped to %f after %s; ",
					        value, celList[i].hi_lim, celList[i].command);
					errString += buff;
				}
				else if (value < celList[i].lo_lim)
				{
					ret = SR_WARN;
					m_lArg = (long) celList[i].lo_lim;
					sprintf(buff, "Warning: %f clipped to %f after %s; ",
					        value, celList[i].lo_lim, celList[i].command);
					errString += buff;
				}
				else
				{
					m_lArg = (long) value;
				}
				break;     
				
			case CA_NONE:
				break;
				
			default:              
				errString += "internal error!";
				ASSERT(FALSE);
				ret = SR_ERROR;
				break;	
		}
	}
	
	return ret;
}               

int CSimObj::FindGrabCommand(CString& commandLine, int nCmdId)
{                
	// Used to check for existence of 'id 4' etc in commands that
	// make objects, so we can add them if they don't exist.
	
	// We repeatedly call GrabCommand until we find the command
	// we were looking for, and return TRUE if we found it.

	CString sLineCopy = commandLine;  
	CString sDummyErrString;
	
    int nFound = FALSE;                                             
    int nCmdFound = 0;
    
    do
    {   
    	// We ignore error returns from GrabCommand, as we may well encounter
    	// commands this obj doesn't know about (eg sim searching a line for 'id 3'
    	// won't understand 'xleft 2.3' and should skip over it)
    	
    	GrabCommand(sLineCopy, sDummyErrString, nCmdFound);
    	
    	if (nCmdFound == nCmdId) nFound = TRUE;
    }
    while (sLineCopy != "" && !nFound);
    
	return nFound;
}                

int CSimObj::MatchCommand(CString& commandLine, int nCmdId)
{                
	// MatchCommand is like FindGrabCommand but only looks for first command
	// in string (to save time)

	CString sLineCopy = commandLine;  
	CString sDummyErrString;
	
    int nCmdFound = 0;
    
   	GrabCommand(sLineCopy, sDummyErrString, nCmdFound);
    	
    return (nCmdFound == nCmdId);
}                

int CSimObj::SnipCommand(CString& commandLine, int nCmdId)
{     
	// Used to find a command, eg 'first_id 4' in a string, and removes it
	// if found (so that it can be replaced). Used in CSim::CmdFillMol.
	
	// We repeatedly call GrabCommand until we find the command
	// we were looking for, and return TRUE if we found it. But 
	// unlike FindGrabCommand, we tack together the remaining line and the
	// string up to the point at which it was found so as to return
	// what is left minus the command we found.
	           
	
	CString sLineCopy = commandLine;  
	CString sDummyErrString;
	CString sPreGrab;    
	CString sSnipRemains;
	
    int nFound = FALSE;                                             
    int nCmdFound = 0;
    
    do
    {   
    	// We ignore error returns from GrabCommand, as we may well encounter
    	// commands this obj doesn't know about (eg sim searching a line for 'id 3'
    	// won't understand 'xleft 2.3' and should skip over it)
    	
    	sPreGrab = sLineCopy;
    	GrabCommand(sLineCopy, sDummyErrString, nCmdFound);
    	
    	if (nCmdFound == nCmdId)
    	{
    		nFound = TRUE;            
    		sSnipRemains += " " + sLineCopy;  // keep leftovers after what we wanted found
    	}
    	else
    	{
    		// keep bits that have been snipped off by GrabCommand that weren't
    		// what we were looking for
    		
    		sSnipRemains += sPreGrab.Left(sPreGrab.GetLength() - sLineCopy.GetLength());
    	}
    }
    while (sLineCopy != "" && !nFound);
    
	if (nFound)
	{   
		commandLine = sSnipRemains;
		return TRUE;               
	}
	else
	{
		return FALSE;
	}
}                

void CSimObj::PopWord(CString& remainingLine, CString& word)
{
	// extracts the next token from the remaining line and removes
	// it; treats quoted string as one token
	                                                 
	int breakpoint;
	
	do
	{
		while (0 == (breakpoint = remainingLine.FindOneOf(" ,\t\n;")))
		{                                                                     
			// removing leading whitespace if there is any
			remainingLine = remainingLine.Right(remainingLine.GetLength() -1); 
		}
				
		if (-1 == breakpoint)
		{   
			// no delimeters found
			word = remainingLine;
			remainingLine = "";
		}
		else                      
		{          
			if ("\"" != remainingLine.Left(1))
			{         
				// simple word
				word = remainingLine.Left(breakpoint);
				remainingLine = remainingLine.Right(remainingLine.GetLength()
			                    - breakpoint - 1);
			}
			else
			{              
				// remove first quote
				remainingLine = remainingLine.Right(remainingLine.GetLength() - 1);
				// add everything up to next quote, if there is one
				word = "";
				while (!remainingLine.IsEmpty() && !("\"" == remainingLine.Left(1)))
				{
					word += remainingLine.Left(1);
					remainingLine = remainingLine.Right(remainingLine.GetLength() - 1);
				}                
				// remove last quote
				remainingLine = remainingLine.Right(remainingLine.GetLength() - 1);
			}				
		}               

	    // remove quotes, if any
	    if ("\"" == word.Left(1))
	    {
	    	word = word.Right(word.GetLength() - 1);
	    	if  ("\"" == word.Right(1)) 
	    	{
	    		 word = word.Left(word.GetLength() - 1);
	    	}
	    }

	}
	while ((0 == breakpoint) && (remainingLine.GetLength() > 0)); 
	// that should handle consecutive delimeters
}
                                                                            
int CSimObj::RectsOverlap(double xl1, double yb1, double xr1, double yt1, 
                           double  xl2, double yb2, double xr2, double yt2)
{
	// Checks if two rectangles overlap; handy for checking if piston, monitor,
	// or textbox lie within selection rectangle.
	
	// Can overlap in the following ways:
	//   * rect1 encloses rect2
	//   * rect2 encloses rect1
	//   * rect1 and rect2 overlap such as one corner of each lies within other
	//   * rect1 overlaps rect2 such that one of its edges passes through two of the other
	// I think all of these are covered by checking if any corners are contained
	// by the other rectangle (including just being on edge) 
	// Aha! Also case in which two edges cross two perpendicular edges but no
	// corners enclosed.
	
	NormaliseRect(xl1, yt1, xr1, yb1);
	NormaliseRect(xl2, yt2, xr2, yb2);
	
	int nXL1 = (xl1 >= xl2 && xl1 <= xr2);  // left edge of 1 between edges of 2
	int nXR1 = (xr1 >= xl2 && xr1 <= xr2);  // right edge of 1 between edges of 2
	int nYB1 = (yb1 >= yb2 && yb1 <= yt2);
	int nYT1 = (yt1 >= yb2 && yt1 <= yt2);
	int nXL2 = (xl2 >= xl1 && xl2 <= xr1);  // left edge of 2 between edges of 1
	int nXR2 = (xr2 >= xl1 && xr2 <= xr1);  // right edge of 2 between edges of 1
	int nYB2 = (yb2 >= yb1 && yb2 <= yt1);
	int nYT2 = (yt2 >= yb1 && yt2 <= yt1);
	
	if ((nXL1 && nYB1)  // left bottom corner of 1 contained by 2 
	  || (nXR1 && nYB1)  // right bottom corner of 1 contained by 2
	  || (nXL1 && nYT1)
	  || (nXR1 && nYT1)
	  || (nXL2 && nYB2)
	  || (nXR2 && nYB2) 
	  || (nXL2 && nYT2)
	  || (nXR2 && nYT2)) 
	{
		return TRUE;
	}
	else if ((nXL1 && nXR1 && yt1 >= yt2 && yb1 <= yb2)
	       || (nYB1 && nYT1 && xl1 <= xl2 && xr1 >= xr2))
	{  
		// cases where no corners enclosed but pairs of edges cross  
		return TRUE;
	}
	else
	{			
		return FALSE;
	}
}

int CSimObj::RectCircleOverlap(double xl, double yb, double xr, double yt, 
	                      double x, double y, double r)
{
	NormaliseRect(xl, yt, xr, yb);
	 
	// This code is taken directly from "Graphics Gems" p656
	// (original scanned to start with).
	
	double Rad2;
	
	Rad2 = r * r;
	/* Translate coordinates, placing C at the origin.*/
	xr -= x; yt -= y;
	xl -= x; yb -= y;
	
	if (xr < 0)               /* R to left of circle center */
		if (yt < 0)           /* R in lower left corner */
		  return ((xr * xr + yt * yt) <    Rad2);
		else if (yb > 0)      /* R in upper left corner */
		  return ((xr * xr + yb * yb) <    Rad2);
		else                        /* R due West of circle */
		  return(fabs(xr) < r);
		else if (xl > 0)      /* R to right of circle center*/
		  if (yt < 0)      /* R in lower right corner */
		      return ((xl * xl + yb * yb) < Rad2);
		else if (yb > 0)      /* R in upper right corner */
		  return ((xl  * xl + yb * yb) < Rad2);
		else                        /* R due East of circle */
		  return (xl  < r);
		else                        /* R on circle vertical centerline */
		  if (yt < 0)      /* R due South of circle*/
		  return (fabs(yt)  <  r);
		else if (yb > 0)      /* R due North of circle*/
		  return (yb < r);
		else                        /* R contains circle centerpoint*/
		  return (TRUE);
} 

int CSimObj::LinesIntersect(double xl1, double yb1, double xr1, double yt1, 
	                        double xl2, double yb2, double xr2, double yt2)
{   
    // To get intersection of two lines, defined by 
    //   L1 = r1 + lambda.d1
    //   L2 = r2 + mu.d2
    // with r1=(r1x, r1y), d1=(d1x, d1y) etc, r = vector to beginning
    // of line, d = vector from there to other end.
    //
    // Solving for lambda and mu such that L1 = L2, get
    //
    //    lambda = (r1y.d2x - r2y.d2x - d2y.r1x + d2y.r2x) / denom
    //        mu = (r1y.d1x - r1x.d1y - r2y.d1x + r2x.d1y) / denom
    // 
    // where
    //
    //     denom = (d1x.d2y - d1y.d2x)    
    //
    // If a solution (0 <= lambda <= |d1|) and (0 <= mu <= |d2|) exists, then
    // the lines intersect at that point. 
    
    double dx1 = xr1 - xl1;  // d1x in comments above, xl1 is r1x above
    double dy1 = yt1 - yb1;
    double dx2 = xr2 - xl2;
    double dy2 = yt2 - yb2;
    
    double d1 = sqrt(dx1 * dx1 + dy1 * dy1); // line length |d1|
    double d2 = sqrt(dx2 * dx2 + dy2 * dy2);            
    
    double denom = dx1 * dy2 - dy1 * dx2;
    
    if (fabs(denom) < NEARLY_ZERO || d1 < NEARLY_ZERO || d2 < NEARLY_ZERO)
    {                
    	// lines pretty well parallel
    	return FALSE;
    }
    else
    {
    	double lambda = (yb1 * dx2 - yb2 * dx2 - dy2 * xl1 + dy2 * xl2)
    	                    / denom;
    	                    
    	double mu     = (yb1 * dx1 - xl1 * dy1 - yb2 * dx1 + xl2 * dy1)
    	                    / denom;            
    	                    
    	return (0 <= lambda && lambda <= 1 && 0 <= mu && mu <= 1);
    }
}

int CSimObj::PointInInclinedRect(double xorg, double yorg,   // origin of rectangle
   	                             double x1,   double y1,     // one corner on side from org
	                             double x2,   double y2,     // other corner on side from org
	                             double x,    double y)      // point to test 	                     
{
	// The two vectors (xorg, yorg)->(x1, y1) and (xorg, yorg)->(x2, y2)
	// should be perpendicular and define two edges of the inclined
	// rectangle. We will find the coordinates of the point (x, y)
	// relative to (xorg, yorg) using the rectangle vectors as axes,
	// and then test if those coordinates are within the rectangle.
	
	// Get rectangle vectors:
	double dx1 = x1 - xorg;
	double dy1 = y1 - yorg;
	double dx2 = x2 - xorg;
	double dy2 = y2 - yorg;
	
	double len1 = sqrt(dx1 * dx1 + dy1 * dy1);
	double len2 = sqrt(dx2 * dx2 + dy2 * dy2);
	
	if (len2 < NEARLY_ZERO || len2 < NEARLY_ZERO)
	{
		// rectangle is really a line or point, as it has one or
		// both edges of zero length, so point can't lie in it
		
		return FALSE;
	}
	
	// Get vector from new origin (rectangle corner) to test point
	double dx = x - xorg;
	double dy = y - yorg;         
	
	double len = sqrt(dx * dx + dy * dy);
	
	// If point happens to lie at origin, that length will be zero:
	if (len < NEARLY_ZERO)
	{
		return TRUE;
	}
	
	// Debug test that rectangle edges are perpendicular (dot
	// product should be zero):
	ASSERT(fabs(dx1 * dx2 + dy1 * dy2) < 1e-6); // NEARLY_ZERO too exacting!
	
	// Get angle between vector to test point and our first axis
	// (dx1, dy1). If those vectors were a and b, then a.b = ab cos theta
	double dotprod = dx * dx1 + dy * dy1;
	double theta = acos(dotprod / (len1 * len));  // divide by zero already checked
	
	double xcoord = len * cos(theta);  // coord rel to edge 1
	double ycoord = len * sin(theta);  // coord rel to edge 2
	
	return (xcoord >= 0 && xcoord <= len1 && ycoord >= 0 && ycoord <= len2);
}

void CSimObj::NormaliseRect(double& xleft, double& ytop, double& xright, double& ybottom)
{      
	// ensure co-ords in order
	double dSwap;  
	
	if (xleft > xright)
	{
		dSwap = xleft;
		xleft = xright;
		xright = dSwap;
	}
	
	if (ybottom > ytop)
	{
		dSwap = ybottom;
		ybottom = ytop;
		ytop = dSwap;
	}    
}
                                                                            
////////////////////////////////////////////////////////////////////////////
// CNumeric
////////////////////////////////////////////////////////////////////////////
CNumeric::CNumeric()
{
	m_pSim = NULL;  // indicates unused 
	m_pdValue = NULL;
	m_nType = 0;
	m_dMin = 0;
	m_dMax = 1;
}

CNumeric::~CNumeric()
{
	// When a numeric quantity is destroyed, it must tell
	// each referencing object (eg in a textbox) so, and
	// remove itself from the master list held by CSim
	
	// remove all references 
	CNumLink* pLink;
	
	while (!m_refList.IsEmpty())
	{
		pLink = (CNumLink*) m_refList.RemoveHead(); 
		pLink->m_nPointerValid = FALSE;
	}
	
	// delete self from big list if has ever been added
	if (NULL != m_pSim)
	{
		m_pSim->RemoveNumeric(this);
	}
}                             

void CNumeric::RemoveRef(CNumLink* pLink)
{   
	POSITION pos = m_refList.Find(pLink);   
	
	if (NULL != pos)
	{
		m_refList.RemoveAt(pos);
	}
	else
	{
		ASSERT(FALSE);  // wasn't in our list anyway
	}
}

////////////////////////////////////////////////////////////////////////////
// CNumLink
////////////////////////////////////////////////////////////////////////////
CNumLink::CNumLink()
{
	// initialise pointer members etc to safe state
	m_nPointerValid = FALSE;
	m_pNumeric = NULL;
	m_nType = NL_TEXT; 
	m_pNextLink = NULL;
}

CNumLink::~CNumLink()
{
	// remove references to self held by previously
	// referenced CNumeric objects
	
	if (m_nPointerValid)
	{
		m_pNumeric->RemoveRef(this);
	}
}

///////////////////////////////////////////////////////////////////////////
// Generic CObjType functionality
/////////////////////////////////////////////////////////////////////////
CObjType::CObjType()
{          
	m_pFirstSObj = NULL;
	m_pLastSObj = NULL;   
	m_sName = "no_name";
	m_pSim = NULL;
	m_lLastIdUsed = 0;
	m_nUseGridWhenGridOn = TRUE;
}        

void CObjType::Prepare()
{
	// called when sim about to start, in case needs
	// to do initialisations; default is to do nothing
}

void CObjType::UserDrawing(CDC* pDC, CDC* pSpareDC1, CDC* pSpareDC2)
{                                    
	// This is where we draw cursors, rectangle representing obj in process
	// of being drawn etc; this default version just does black rectangle
	
    CPoint currentPt;
    
    CPen wallPen(PS_SOLID, 1, 
                 (COLORREF) RGB(0, 0, 0));
                                
    CPen* pOldPen;
    
	switch (m_pSim->m_nDrawingState)
	{                     
		case SM_AWAITING_CLICK1:     
		case SM_BUTDN_NOT_MOVED1:      
			currentPt = DrawCrosshair(pDC);    
			break;
			
		case SM_DRAGGING:
		case SM_AWAITING_CLICK2:
			currentPt = DrawCrosshair(pDC);    
			pOldPen = pDC->SelectObject(&wallPen);
			pDC->MoveTo(m_pSim->m_ptLogDragStart);
			pDC->LineTo(m_pSim->m_ptLogDragStart.x, currentPt.y); 
			pDC->LineTo(currentPt);
			pDC->LineTo(currentPt.x, m_pSim->m_ptLogDragStart.y);
			pDC->LineTo(m_pSim->m_ptLogDragStart);
			pDC->SelectObject(pOldPen);
			break;

		case SM_NOT_DRAWING:			
		default:
			// unknown state
			ASSERT(FALSE);
			break;
	}
}


void CObjType::Delete()
{
	// remove all CSObjs of this type
	DelObjs();	    
	  
	// remove all interactions belonging to this type,
	// if it is a type that has interactions
	
	int nThisTypeIndex = m_pSim->GetObjTypeIndexByName(m_sName); 
		
	if (SR_NO_SUCH_TYPE != nThisTypeIndex)
	{                                    
		m_pSim->m_intArray.RemoveIndex(nThisTypeIndex);
	}
	else
	{
		// can't delete interactions as we don't seem to exist...
		ASSERT(FALSE);
	}
			
	// remove type from type list  
	
	m_pSim->m_typeArray.RemoveAt(nThisTypeIndex);
}

void CObjType::DelObjs()
{
	// Deletes all CSObjs of this type
	CSObj* pLastSObj = m_pFirstSObj;
	CSObj* pNextSObj = NULL;
		
	if (NULL != pLastSObj)
	{
		pNextSObj = pLastSObj->m_pNextSObj;
	}
			
	while (NULL != pNextSObj)
	{   
		delete pLastSObj;
		pLastSObj = pNextSObj;
		pNextSObj = pNextSObj->m_pNextSObj;
	}
		
	if (NULL != pLastSObj)
	{
		delete pLastSObj;
	}
	  
}

int CObjType::GetSelectedObjects(double dXLeft, double dYBottom,
                       double dXRight, double dYTop, int nDragged, int nSingle)
{                       
	// select all objs that overlap the given rectangle
	
	// If the user is dragging over a region (with shift or ctrl pressed),
	// don't want to toggle selection of objects that are already
	// selected, only want to add them. So only issue 'select' command
	// if not in dragging state, ie nDragged = FALSE.
	
	CSObj* pSObj = m_pFirstSObj;     
	int sel = FALSE;
		
	while ((NULL != pSObj) && !(nSingle && sel))
    {
		if (pSObj->HitTest(dXLeft, dYBottom, dXRight, dYTop)
		            && !(nDragged && pSObj->m_nSelected))
		{   
			sprintf(buff, "%ld", pSObj->m_lId);   
				
			CString cmd;
			cmd = "select \"" + m_sName;
			cmd += "\" ";
			cmd += buff;
			m_pSim->HandleNewCommand(cmd);
			
			sel = TRUE;
		}
        
   		pSObj = pSObj->m_pNextSObj;
    }	     
    
    return sel;
}

void CObjType::MoveSObjs()
{                
	// default is not to move anything (valid for walls, for example)
}                  

void CObjType::ApplyAccels()
{
	// should not get called for anything that cannot move, and should
	// be overridden for things that can
	ASSERT(FALSE);
}

double CObjType::PrepStep()
{
	// called before forces are calculated etc for mobile object types;
	// should return fastest speed squared
	return 0;
}

void CObjType::Initialise()
{
	// Do any special processing at beginning, or if properties have
	// been modified, that is required before the object type is used 
}

int CObjType::DrawCommand()
{
	// By default, don't open any dialogues or anything, go straight
	// into user drawing:
	
	return TRUE;
}

CPoint CObjType::DrawCrosshair(CDC* pDC)
{
	// helper function to draw crosshair cursor, snapping to
	// grid if required
    CPoint currentPt;
    double gridX;
    double gridY;

	gridX = m_pSim->m_dSimMouseX;
	gridY = m_pSim->m_dSimMouseY;
	if (m_pSim->m_nUseGrid)
	{
		m_pSim->Gridify(&gridX, &gridY);  
	}
	currentPt = m_pSim->SimToLogical(gridX, gridY); 
	pDC->SelectStockObject(BLACK_PEN);
	currentPt.x -= DW_CROSSHAIR_RADIUS;
	pDC->MoveTo(currentPt.x, currentPt.y);
	pDC->LineTo(currentPt.x + 2 * DW_CROSSHAIR_RADIUS, currentPt.y); 
	currentPt.x += DW_CROSSHAIR_RADIUS;
	currentPt.y -= DW_CROSSHAIR_RADIUS;
	pDC->MoveTo(currentPt.x, currentPt.y);
	pDC->LineTo(currentPt.x, currentPt.y + 2 * DW_CROSSHAIR_RADIUS);
	currentPt.y += DW_CROSSHAIR_RADIUS;
	
	return currentPt;
}

void CObjType::OnMouseMove()
{     
	switch (m_pSim->m_nDrawingState)
	{
		case SM_AWAITING_CLICK1:
			// Will just move cursor, no change to state
			break;
			
		case SM_BUTDN_NOT_MOVED1:
			// Have moved mouse with button down, so dragging, if have
			// moved significantly
			
			if ((abs(m_pSim->m_ptLogMouse.x - m_pSim->m_ptLogDragStart.x) > DRAG_THRESH_PIXELS)
			   || (abs(m_pSim->m_ptLogMouse.y - m_pSim->m_ptLogDragStart.y) > DRAG_THRESH_PIXELS))
			{
				m_pSim->m_nDrawingState = SM_DRAGGING;  
			}
			break;
			
		default:
			break;
	}
}

void CObjType::OnLButtonDown()
{  
	switch (m_pSim->m_nDrawingState)
	{
		case SM_AWAITING_CLICK1:
			// Have pressed left button for first time, not yet moved
			m_pSim->m_nDrawingState = SM_BUTDN_NOT_MOVED1;
			break;   
			
		default:
			break; 
	}
}

void CObjType::OnLButtonUp()
{
	ChangeDrawingStateOnLButtonUp();
}

void CObjType::ChangeDrawingStateOnLButtonUp()
{
	switch (m_pSim->m_nDrawingState)
	{          
		case SM_AWAITING_CLICK1:
			// Have managed to unclick before we registered the
			// button down (the second half of a double-click event,
			// probably)
			m_pSim->m_nDrawingState = SM_AWAITING_CLICK2;
			TRACE("Instantaneous click 1 done, now waiting for click2\n");
			break;

		case SM_DRAGGING:
			// Have released button after dragging a rectangle
			// in the same place; now moving freely for click 2
			m_pSim->m_nDrawingState = SM_AWAITING_CLICK2;
			TRACE("Click 1 dragging finished, now waiting for click2\n");
			break;

		case SM_BUTDN_NOT_MOVED1:
			// Have just clicked once
			// in the same place; now moving freely for click 2
			m_pSim->m_nDrawingState = SM_AWAITING_CLICK2;
			TRACE("Click 1 over, didn't drag, now waiting for click 2\n");
			break;
			
		default:
			break; 
	}
}
//////////////////////////////////////////////////////////////////////////
// CObjType generic functionality
//////////////////////////////////////////////////////////////////////////
int CObjType::AddObj(CSObj* pObj)
{
	if (m_pSim->m_lTotalNumObjs >= LIM_TOTAL_NUM_OBJS)
	{
		// can't make any more!
		
		if (!m_pSim->m_nIssuedTooManyObjsWarning)
		{                  
			sprintf(buff, "I'm sorry, you are not allowed to create a total of"
			              " more than %ld objects simultaneously. You will not"
			              " see this warning again, but no more objects will be"
			              " made until some others are deleted.", (long) LIM_TOTAL_NUM_OBJS);
			              
			AfxMessageBox(buff);
			
			m_pSim->m_nIssuedTooManyObjsWarning = TRUE;
		}
		
		return FALSE;
	}
	else
	{                            
		// add obj to end of doubly-linked list maintained by obj type object
		if (NULL != m_pLastSObj) 
		{
			m_pLastSObj->m_pNextSObj = pObj;
			pObj->m_pPrevSObj = m_pLastSObj;
		}
		else
		{
			m_pFirstSObj = pObj;
			pObj->m_pPrevSObj = NULL;
		}
		pObj->m_pNextSObj = NULL; 
		m_pLastSObj = pObj;
		
		pObj->m_nLinked = TRUE;
		
		if (0 == pObj->m_lId)
		{
			// initialise obj id number to last used plus one
			pObj->m_lId = ++m_lLastIdUsed;
		}
		else
		{
			// update biggest used according to specified id, unless in some special
            // scheme like the IDs generated by monitors or reactive interactions
			if (pObj->m_lId > m_lLastIdUsed && pObj->m_lId < FIRST_SPECIAL_ID)
            {
                m_lLastIdUsed = pObj->m_lId;
            }
		}               
		
		pObj->ContributeNumerics();  
		
		m_pSim->m_lTotalNumObjs++;
		
		ASSERT(m_pSim->m_lTotalNumObjs <= LIM_TOTAL_NUM_OBJS);
		
		return TRUE; 
	}
}	      

void CObjType::RemoveObj(CSObj* pObj)
{
	// Removes obj from linked list we maintain of such objs

	if (NULL == pObj->m_pNextSObj)
	{
		// this is last in list
			
		m_pLastSObj = pObj->m_pPrevSObj;
	}
	else
	{
		// not last
			
		pObj->m_pNextSObj->m_pPrevSObj = pObj->m_pPrevSObj;
	}
	
	if (NULL == pObj->m_pPrevSObj)
	{
		// this is first in list
			
		m_pFirstSObj = pObj->m_pNextSObj;
	}
	else
	{
		// not first
			
		pObj->m_pPrevSObj->m_pNextSObj = pObj->m_pNextSObj;
	}             
	
/* Removed this for time being -- always force rebuild of neighbour
   list if delete an obj, so should never get CNP referencing a 
   deleted object:
    // delete any CNP objects attached via the objects handle
    if (NULL != pObj->m_pFirstHandle)
    {
        CListHandle* pHandle = pObj->m_pFirstHandle;
        CListHandle* pNewHandle;

        do
        {
            pNewHandle = pHandle->m_pNextHandle;
            delete pHandle->m_pNP; // deletes handle too
            pHandle = pNewHandle;
        }
        while (NULL != pHandle);
    }
*/

	m_pSim->m_lTotalNumObjs--;
	
	ASSERT(m_pSim->m_lTotalNumObjs >= 0);
}        

void CObjType::DrawSelnBlob(CDC* pDC, int nXLogical, int nYLogical)
{     
	CBrush brush; 
	
	brush.CreateSolidBrush(0);
	
	CPoint point(nXLogical - 3, nYLogical - 3);
	CSize size(6, 6);
	CRect rect(point, size);
	
	pDC->FillRect(&rect, &brush);
}

CSObj* CObjType::FetchSObj(long lId)
{
	CSObj* pObj = m_pFirstSObj;
	
	while ((NULL != pObj) && (pObj->m_lId != lId))
	{
		pObj = pObj->m_pNextSObj;
	}
	
	return pObj;  // will always be null unless matched id found
}         

/////////////////////////////////////////////////////////////////
// CSObj
/////////////////////////////////////////////////////////////////

CSObj::CSObj()
{      
	m_nLinked = FALSE;
	m_pPrevSObj = NULL;
	m_pNextSObj = NULL; 
    m_dTimeDoneAlready = 0;
    m_nTouched = FALSE;
	m_pSim = NULL;
	m_lId = 0;
	m_pFirstHandle = NULL;
	m_nSelected = FALSE;    
	m_pTracker = NULL;
	m_nTouched = FALSE;         
	m_pType = NULL;

    // for classes derived from CSObj, their own constructors
	// are called automatically after this
}

CSObj::~CSObj()
{                                                                   
	// This will be called automatically after any derived
	// classes overridden destructors have been executed
	
	// unlink self from doubly-linked list maintained by parent type
	
	if (m_nLinked)
	{
		m_pType->RemoveObj(this);
		
	}   
	
	if (m_nSelected)
	{
		// Remove self from selected list maintained by sim
		
		m_pSim->Unselect(this);
	} 
	
	delete m_pTracker;  // should be done by Unselect anyway
}

void CSObj::ObjectProperties()
{                 
	sprintf(buff, "Object properties for obj at %p", this);
	
	AfxMessageBox(buff);
}

void CSObj::CreateTracker()
{
	ASSERT(m_pTracker == NULL);

	m_pTracker = new CGridTracker;
	
	m_pTracker->m_pObj = this;
	m_pTracker->m_pSim = m_pSim;
}  

void CSObj::DeleteTracker()
{
	delete m_pTracker;
	
	m_pTracker = NULL;
}  

void CSObj::DrawTracker(CDC* pDC)
{              
	ASSERT(FALSE); // needs overriding
	
	m_pTracker->Draw(pDC);
}

BOOL CSObj::SetCursor(CWnd* pWnd, UINT nHitTest, int nMultiple) const
{
	// Called to see if should change cursor as moved over
	// tracking rectangle on selected obj. Usage copied from
	// CRectTracker, and by default pass it straight on to our
	// tracker. Except extra parameter so we know whether we
	// have multiple selection (which bars resizes).
	
	if (NULL != m_pTracker)
	{
		return m_pTracker->SetCursor(pWnd, nHitTest, nMultiple);
	}
	else
	{
		ASSERT(FALSE); // shouldn't have got called 
		
		return FALSE;
	}
}
	
int CSObj::TrackerHitTest(CPoint point)  
{       
	// Called to see if mouse moved over or clicked on tracking rectangle(s)
	// belonging to this object 
	
	if (NULL != m_pTracker)
	{
		return (CRectTracker::hitNothing != m_pTracker->HitTest(point));
	}
	else
	{
		ASSERT(FALSE); // shouldn't have got called 
		
		return FALSE;
	}
}

int CSObj::Track(int nMultiple, CWnd* pWnd, CPoint point)
{
	// If click on tracking rectangle of selected object, this function called
	// to allow user to drag it about (and resize if appropriate)  
	
	if (NULL != m_pTracker)
	{
		if (m_pTracker->Track(nMultiple, pWnd, point, TRUE /* allow invert */))
		{
			// Selection was moved; get co-ords of new rectangle and displacement
			// vector from original position,all in simulation co-ords:
			
			double x1, y1;  // old top left position
			CPoint point1(m_pTracker->m_startRect.left, m_pTracker->m_startRect.top); // where started
			double x2, y2;  // new top left position
			CPoint point2(m_pTracker->m_rect.left, m_pTracker->m_rect.top); // top left of where ended up
            double xr, yb;  // new bottom right position
            CPoint pointBR(m_pTracker->m_rect.right, m_pTracker->m_rect.bottom);
            ((CSimView*) pWnd)->SimifyMousePoint(point1, &x1, &y1);
            ((CSimView*) pWnd)->SimifyMousePoint(point2, &x2, &y2);
			((CSimView*) pWnd)->SimifyMousePoint(pointBR, &xr, &yb);  
			
			if (m_pType->m_nUseGridWhenGridOn && m_pSim->m_nUseGrid)
			{	
				m_pSim->Gridify(&x2, &y2);
				m_pSim->Gridify(&xr, &yb);
			}
			
			if (nMultiple)
			{
				// Did do move, multiple selection, handle generically
				
				sprintf(buff, "seln_move x %g y %g", x2 - x1, y2 - y1);
				
				m_pSim->HandleNewCommand(buff);
			}
			else
			{
				// single selection, obj type specific behaviour 
				
				OnMoveOrResize(x2, y2, xr, yb, x1, y1, 
				                   m_pTracker->m_nHaveResized);
			}
			
			return TRUE;
		}
		else
		{
			// no move or pressed Esc or right button to cancel tracking
			return FALSE;
		}
	}
	else
	{
		ASSERT(FALSE); // shouldn't have got called
		
		return FALSE;
	}	
}

void CSObj::OnMoveOrResize(double dNewXLeft, double dNewYTop,
	                       double dNewXRight, double dNewYBottom,
	                       double dOldXLeft, double dOldYTop,
	                       int nResized)
{    
	// When single selection has been manipulated by mouse, end up
	// here to issue command to sim to alter it. Needs to be overridable
	// eg for piston to handle separate rest position and actual piston,
	// and for resizable things to issue 'change' commmand on themselves.
	
	// For default, just do move as for multiple selection (OK for non-resizable
	// objects):                          
	
	sprintf(buff, "seln_move x %g y %g", dNewXLeft - dOldXLeft, dNewYTop - dOldYTop);
				
	m_pSim->HandleNewCommand(buff);
}
	                                  
void CSObj::PrepareOtherTrackersToFollow()
{
	// when leading multiple selection drag
	
	m_pSim->PrepareTrackersToFollow(this);
}

void CSObj::UpdateFollowingTrackers(int nDeltaXL, int nDeltaYT,
	                                int nDeltaXR, int nDeltaYB,
	                                CWnd* pWnd)
{                                                                                      
	m_pSim->UpdateFollowingTrackers(nDeltaXL, nDeltaYT, this, pWnd);
}

void CSObj::UpdateTrackerAsFollowing(int nDeltaX, int nDeltaY, CWnd* pWnd)
{
	if (NULL != m_pTracker)
	{                                               
		// Should only be a move, so set delta x right = delta x left etc
		m_pTracker->UpdateTrackerAsFollowing(nDeltaX, nDeltaY, nDeltaX, nDeltaY, pWnd);
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CSObj::PrepareTrackerForFollowing()
{             
	if (NULL != m_pTracker)
	{
		m_pTracker->PrepareToFollow();
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CSObj::HandleMove(double dDeltaX, double dDeltaY)
{
	// Should be overridden
	
	ASSERT(FALSE);
}

///////////////////////////////////////////////////////////////////////////////
// CHotSpot
///////////////////////////////////////////////////////////////////////////////

CHotSpot::CHotSpot()
{
	m_pOwner = NULL;
	m_nId = 0;
	m_rect.left = 0;
	m_rect.right = 0;
	m_rect.top = 0;
	m_rect.bottom = 0;
}             

CHotSpot::~CHotSpot()
{
	m_pOwner->m_pSim->RemoveHotSpotRef(this);
}

int CSObj::HotSpotSetCursor(CHotSpot* pHotSpot, CWnd* pWnd, UINT nHitTest)
{
	ASSERT(FALSE); // should be overridden if obj has hotspots
	
	return FALSE;
}

int CSObj::OnHotSpotLButtonDown(CHotSpot* pHotSpot, UINT nFlags, CPoint point)
{
	pHotSpot->m_ptLBDown = point;
	pHotSpot->m_trackingRect = pHotSpot->m_rect;
	
	return HSLBD_TRACKING_STARTED;
} 

void CSObj::OnHotSpotMouseMove(CHotSpot* pHotSpot, UINT nFlags, CPoint point)
{  
	pHotSpot->m_trackingRect = pHotSpot->m_rect + CPoint(point.x - pHotSpot->m_ptLBDown.x,
	                                                     point.y - pHotSpot->m_ptLBDown.y);
}	
                                        
void CSObj::OnHotSpotLButtonUp(CHotSpot* pHotSpot, UINT nFlags, CPoint point)
{
}