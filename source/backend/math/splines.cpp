/*******************************************************************************
 * splines.cpp
 *
 * This module implements splines.
 *
 * ---------------------------------------------------------------------------
 * Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
 * Copyright 1991-2013 Persistence of Vision Raytracer Pty. Ltd.
 *
 * POV-Ray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * POV-Ray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------------
 * POV-Ray is based on the popular DKB raytracer version 2.12.
 * DKBTrace was originally written by David K. Buck.
 * DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
 * ---------------------------------------------------------------------------
 * $File: //depot/public/povray/3.x/source/backend/math/splines.cpp $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "backend/math/splines.h"
#include "base/pov_err.h"


#include <math.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Global Variables
******************************************************************************/


/*****************************************************************************
* Local preprocessor defines
******************************************************************************/


/*****************************************************************************
* Local typedefs
******************************************************************************/


/*****************************************************************************
* Local variables
******************************************************************************/


/*****************************************************************************
* Local functions
******************************************************************************/

DBL linear_interpolate(const SPLINE_ENTRY * se, int i, int k, DBL p);
DBL quadratic_interpolate(const SPLINE_ENTRY * se, int i, int k, DBL p);
DBL natural_interpolate(const SPLINE_ENTRY * se, int i, int k, DBL p);
DBL catmull_rom_interpolate(const SPLINE_ENTRY * se, int i, int k, DBL p);
int findt(const SPLINE * sp, DBL Time);
void mkfree(SPLINE * sp, int i);
void Precompute_Cubic_Coeffs(SPLINE *sp);

/*****************************************************************************
*
* FUNCTION
*
*       Precompute_Cubic_Coeffs
*
* INPUT
*
*       sp : a pointer to the spline to compute interpolation coefficients for
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Mark Wagner
*
* DESCRIPTION
*
*       Computes the coefficients used in cubic_interpolate.
*
* CHANGES
*
******************************************************************************/

void Precompute_Cubic_Coeffs(SPLINE *sp)
{
	int i, k;
	DBL *h;
	DBL *b;
	DBL *u;
	DBL *v;

	h = (DBL *)POV_MALLOC(sp->Number_Of_Entries*sizeof(DBL), "Spline coefficient storage");
	b = (DBL *)POV_MALLOC(sp->Number_Of_Entries*sizeof(DBL), "Spline coefficient storage");
	u = (DBL *)POV_MALLOC(sp->Number_Of_Entries*sizeof(DBL), "Spline coefficient storage");
	v = (DBL *)POV_MALLOC(sp->Number_Of_Entries*sizeof(DBL), "Spline coefficient storage");

	for(k = 0; k < 5; k++)
	{
		for(i = 0; i <= sp->Number_Of_Entries - 2; i++)
		{
			h[i] = sp->SplineEntries[i+1].par - sp->SplineEntries[i].par;
			b[i] = (sp->SplineEntries[i+1].vec[k] - sp->SplineEntries[i].vec[k])/h[i];
		}
		u[1] = 2*(h[0]+h[1]);
		v[1] = 6*(b[1]-b[0]);
		for(i = 2; i <= sp->Number_Of_Entries - 2; i++)
		{
			u[i] = 2*(h[i]+h[i-1]) - (h[i-1]*h[i-1])/u[i-1];
			v[i] = 6*(b[i]-b[i-1]) - (h[i-1]*v[i-1])/u[i-1];
		}
		sp->SplineEntries[sp->Number_Of_Entries-1].coeff[k] = 0;
		for(i = sp->Number_Of_Entries-2; i > 0; i--)
		{
			sp->SplineEntries[i].coeff[k] = (v[i] - h[i]*sp->SplineEntries[i+1].coeff[k])/u[i];
		}
		sp->SplineEntries[0].coeff[k] = 0;
	}
	sp->Coeffs_Computed = true;

	POV_FREE(h);
	POV_FREE(b);
	POV_FREE(u);
	POV_FREE(v);
}


/*****************************************************************************
*
* FUNCTION
*
*       linear_interpolate
*
* INPUT
*
*       se : a pointer to the entries in the spline
*       i  : the first point to interpolate between
*       k  : which dimension of the 5D vector to interpolate in
*       p  : the parameter to interpolate the value for
*
* OUTPUT
*
* RETURNS
*
*       The value of the kth dimension of the vector linearly interpolated at p
*
* AUTHOR
*
*   Wolfgang Ortmann
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL linear_interpolate(const SPLINE_ENTRY * se, int i, int k, DBL p)
{
	DBL p1, p2, v1, v2;
	p1 = se[i].par;
	p2 = se[i+1].par;
	v1 = se[i].vec[k];
	v2 = se[i+1].vec[k];
	return (p-p1)*(v2-v1)/(p2-p1)+v1;
}


/*****************************************************************************
*
* FUNCTION
*
*       quadratic_interpolate
*
* INPUT
*
*       se : a pointer to the entries in the spline
*       i  : the second point of three to interpolate between
*       k  : which dimension of the 5D vector to interpolate in
*       p  : the parameter to interpolate the value for
*
* OUTPUT
*
* RETURNS
*
*       The value of the kth dimension of the quadratic interpolation of the
*        vector at p
*
* AUTHOR
*
*   Wolfgang Ortmann
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL quadratic_interpolate(const SPLINE_ENTRY * se, int i, int k, DBL p)
{
	/* fit quadratic function to three point*/
	DBL n;
	DBL p1, p2, p3,
	    v1, v2, v3,
	    a, b, c;
	/* assignments to make life easier */
	p1=se[i-1].par;    p2=se[i].par;    p3=se[i+1].par;
	v1=se[i-1].vec[k]; v2=se[i].vec[k]; v3=se[i+1].vec[k];

	n=(p2-p1)*(p3-p1)*(p3-p2);

	/* MWW NOTE: I'm assuming these are correct.  I didn't write them */
	a=(-p2*v1+p3*v1
	   +p1*v2-p3*v2
	   -p1*v3+p2*v3) /n;
	b=( p2*p2*v1 - p3*p3*v1
	   -p1*p1*v2 + p3*p3*v2
	   +p1*p1*v3 - p2*p2*v3) /n;
	c=(-p2*p2*p3*v1+p2*p3*p3*v1
	   +p1*p1*p3*v2-p1*p3*p3*v2
	   -p1*p1*p2*v3+p1*p2*p2*v3) /n;
	return (a*p+b)*p+c; /* Fast way of doing ap^2+bp+c */
}


/*****************************************************************************
*
* FUNCTION
*
*       natural_interpolate
*
* INPUT
*
*       se : a pointer to the entries in the spline
*       i  : the first point in the interpolation interval
*       k  : which dimension of the 5D vector to interpolate in
*       p  : the parameter to interpolate the value for
*
* OUTPUT
*
* RETURNS
*
*       The value of the kth dimension of the natural cubic interpolation
*        of the vector at p
*
* AUTHOR
*
*       Mark Wagner
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL natural_interpolate(const SPLINE_ENTRY * se, int i, int k, DBL p)
{
	DBL h, tmp;
	h = se[i+1].par - se[i].par;
	tmp = se[i].coeff[k]/2.0 + ((p - se[i].par)*(se[i+1].coeff[k] - se[i].coeff[k]))/(6.0*h);
	tmp = -(h/6.0)*(se[i+1].coeff[k] + 2.0*se[i].coeff[k]) + (se[i+1].vec[k] - se[i].vec[k])/h + (p - se[i].par)*tmp;
	return se[i].vec[k] + (p - se[i].par)*tmp;
}


/*****************************************************************************
*
* FUNCTION
*
*       catmull_rom_interpolate
*
* INPUT
*
*       se : a pointer to the entries in the spline
*       i  : the first point in the interpolation interval
*       k  : which dimension of the 5D vector to interpolate in
*       p  : the parameter to interpolate the value for
*
* OUTPUT
*
* RETURNS
*
*       The value of the kth dimension of the Catmull-Rom interpolation of the
*        vector at p
*
* AUTHOR
*
*       Mark Wagner
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL catmull_rom_interpolate(const SPLINE_ENTRY * se, int i, int k, DBL p)
{
	DBL dt = (se[i+1].par - se[i].par); /* Time between se[i] and se[i+1] */
	DBL u = (p - se[i].par)/dt;         /* Fractional time from se[i] to p */
	DBL dp0 = ((se[i].vec[k] - se[i-1].vec[k])/(se[i].par - se[i-1].par) + (se[i+1].vec[k] - se[i].vec[k])/(se[i+1].par - se[i].par))/2.0 * dt;
	DBL dp1 = ((se[i+2].vec[k] - se[i+1].vec[k])/(se[i+2].par - se[i+1].par) + (se[i+1].vec[k] - se[i].vec[k])/(se[i+1].par - se[i].par))/2.0 * dt;

	return (se[i].vec[k] * (2*u*u*u - 3*u*u + 1) +
	        se[i+1].vec[k] * (3*u*u - 2*u*u*u) +
	        dp0 * (u*u*u - 2*u*u + u) +
	        dp1 * (u*u*u - u*u) );
}


/*****************************************************************************
*
* FUNCTION
*
*       findt
*
* INPUT
*
*       sp   : a pointer to a spline
*       Time : The parameter to search for
*
* OUTPUT
*
* RETURNS
*
*       The first spline entry with a parameter greater than Time
*
* AUTHOR
*
*   Wolfgang Ortmann
*
* DESCRIPTION
*
* CHANGES
*
*       Mark Wagner  6 Nov 2000 : Changed from linear search to binary search
*
******************************************************************************/

int findt(const SPLINE * sp, DBL Time)
{
	int i, i2;
	SPLINE_ENTRY * se;
	se = sp->SplineEntries;
	if(sp->Number_Of_Entries == 0) return 0;

	if(Time <= se[0].par) return 0;

	if(Time >= se[sp->Number_Of_Entries-1].par) return sp->Number_Of_Entries;

	i = sp->Number_Of_Entries / 2;
	/* Bracket the proper entry */
	if( Time > se[i].par ) /* i is lower, i2 is upper */
	{
		i2 = sp->Number_Of_Entries-1;
		while(i2 - i > 1)
		{
			if(Time > se[i+(i2-i)/2].par)
				i = i+(i2-i)/2;
			else
				i2 = i+(i2-i)/2;
		}
		return i2;
	}
	else /* i is upper, i2 is lower */
	{
		i2 = 0;
		while(i - i2 > 1)
		{
			if(Time > se[i2+(i-i2)/2].par)
				i2 = i2+(i-i2)/2;
			else
				i = i2+(i-i2)/2;
		}
		return i;
	}
}


/*****************************************************************************
*
* FUNCTION
*
*       mkfree
*
* INPUT
*
*       sp : a pointer to the entries in the spline
*       i  : the index of the entry to make available
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Wolfgang Ortmann
*
* DESCRIPTION
*
*       Makes the spline entry at index i available for inserting a new entry
*       by moving all entries starting with that index down by one slot in the
*       array.
*
* CHANGES
*
******************************************************************************/

void mkfree(SPLINE * sp, int i)
{
	int j;
	SPLINE_ENTRY * se;
	se = sp->SplineEntries;

	for (j=sp->Number_Of_Entries; j>i; j--)
		se[j] = se[j-1];
}


/*****************************************************************************
*
* FUNCTION
*
*       Create_Spline
*
* INPUT
*
*       Type : the type of the new spline
*
* OUTPUT
*
* RETURNS
*
*       A pointer to the newly-created spline
*
* AUTHOR
*
*   Wolfgang Ortmann
*
* DESCRIPTION
*
* CHANGES
*
*       Mark Wagner  6 Nov 2000 : Added support for dynamic resizing of the
*               SplineEntry array.
*       Mark Wagner 25 Aug 2001 : Added support for pre-computing coefficients
*               of cubic splines
*
******************************************************************************/

SPLINE * Create_Spline(int Type)
{
	SPLINE * New;
	New = (SPLINE *)POV_MALLOC(sizeof(SPLINE), "spline");
	New->SplineEntries = (SPLINE_ENTRY *)POV_MALLOC(INIT_SPLINE_SIZE*sizeof(SPLINE_ENTRY), "spline entry");
	New->Max_Entries = INIT_SPLINE_SIZE;
	New->Number_Of_Entries = 0;
	New->Type = Type;
	New->Coeffs_Computed = false;
	New->Terms = 2;
	// New->Cache_Valid = false; [JG] flyspray #294, cache is not thread-safe
	New->ref_count = 1;
	int i;
	for (i=0; i< New->Max_Entries; i++)
	{
		New->SplineEntries[i].par=-1e6;   //this should be a negative large number
	}

	return New;
}


/*****************************************************************************
*
* FUNCTION
*
*       Copy_Spline
*
* INPUT
*
*       Old : A pointer to the old spline
*
* OUTPUT
*
* RETURNS
*
*       A pointer to the new spline
*
* AUTHOR
*
*   Wolfgang Ortmann
*
* DESCRIPTION
*
* CHANGES
*
*       Mark Wagner  6 Nov 2000 : Added support for dynamic resizing of the
*               SplineEntry array
*       Mark Wagner 25 Aug 2001 : Added support for pre-computing coefficients
*               of cubic splines
*
******************************************************************************/

SPLINE * Copy_Spline(const SPLINE * Old)
{
	SPLINE * New;
	New = (SPLINE *)POV_MALLOC(sizeof(SPLINE), "spline");

	New->SplineEntries = (SPLINE_ENTRY *)POV_MALLOC(Old->Number_Of_Entries*sizeof(SPLINE_ENTRY), "spline entry");
	POV_MEMCPY(New->SplineEntries, Old->SplineEntries, Old->Number_Of_Entries*sizeof(SPLINE_ENTRY));

	New->Max_Entries = Old->Number_Of_Entries;
	New->Number_Of_Entries = Old->Number_Of_Entries;
	New->Type = Old->Type;
	New->Coeffs_Computed = Old->Coeffs_Computed;
	New->Terms = Old->Terms;
	//[JG] flyspray #294, cache is not thread-safe
	// New->Cache_Valid = false; // we don't copy the cache so mark it as invalid
	New->ref_count = 1;

	return New;
}


void Acquire_Spline_Reference(SPLINE * Spline)
{
	if (Spline)
	{
		if (Spline->ref_count >= INT_MAX)
			throw POV_EXCEPTION_STRING("Too many unresolved references to single spline\n");
		Spline->ref_count ++;
	}
}

void Release_Spline_Reference(SPLINE * Spline)
{
	if (Spline)
		Destroy_Spline(Spline);
}

/*****************************************************************************
*
* FUNCTION
*
*       Destroy_Spline
*
* INPUT
*
*       Spline : A pointer to the spline to delete
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Wolfgang Ortmann
*
* DESCRIPTION
*
*       Deletes the SplineEntry array
*
* CHANGES
*
******************************************************************************/

void Destroy_Spline(SPLINE * Spline)
{
	if (Spline->ref_count <= 0)
		throw POV_EXCEPTION_STRING("Internal error: Invalid spline reference count\n");

	Spline->ref_count --;
	if (Spline->ref_count == 0)
	{
		POV_FREE(Spline->SplineEntries);
		POV_FREE(Spline);
	}
}


/*****************************************************************************
*
* FUNCTION
*
*       Insert_Spline_Entry
*
* INPUT
*
*       sp : a pointer to the spline to insert the new value in
*       p  : the value of the parameter at that point
*       v  : a 5D vector that is the value of the spline at that parameter
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Wolfgang Ortmann
*
* DESCRIPTION
*
*       Inserts a value into the given spline, sorting the entries in order by
*               increasing p
*       If a value of the parameter already exists, that value is overwritten
*
* CHANGES
*
*       Mark Wagner  8 Nov 2000 : Added dynamic sizing of the SplineEntry array
*          If a value of the parameter already exists, that value is overwritten
*       Mark Wagner 25 Aug 2001 : Modified for compatibility with new method of
*          computing cubic splines.
*       Mark Wagner 30 Aug 2001 : Fixed a bug with over-writing of parameter 
*          values.
*
******************************************************************************/

void Insert_Spline_Entry(SPLINE * sp, DBL p, const EXPRESS v)
{
	int i, k;

	/* Reset the Coeffs_Computed flag.  Inserting a new point invalidates 
	 *  pre-computed coefficients */
	sp->Coeffs_Computed = false;
	// sp->Cache_Valid = false;[JG] flyspray #294, cache is not thread-safe
	/* If all space is used, reallocate */
	if(sp->Number_Of_Entries >= sp->Max_Entries)
	{
		sp->Max_Entries += INIT_SPLINE_SIZE;
		sp->SplineEntries = (SPLINE_ENTRY *)POV_REALLOC(sp->SplineEntries, sp->Max_Entries * sizeof(SPLINE_ENTRY), "Temporary Spline Entries");
		for (i = sp->Number_Of_Entries; i < sp->Max_Entries; i++)
		{
			sp->SplineEntries[i].par=-1e6;
		}
	}
	i = findt(sp, p);
	/* If p is already in spline, replace */
	/* The clause after the || is needed because findt returns sp->Number_Of_Entries
	 * if p is greater than OR EQUAL TO the highest par in the spline */
	if(sp->Number_Of_Entries != 0 && ((sp->SplineEntries[i].par == p) || (i == sp->Number_Of_Entries && sp->SplineEntries[i-1].par == p)))
	{
		for(k=0; k<5; k++)
			sp->SplineEntries[i].vec[k] = v[k];
	}
	else
	{
		mkfree(sp, i);
		sp->SplineEntries[i].par = p;

		for(k=0; k<5; k++)
			sp->SplineEntries[i].vec[k] = v[k];

		sp->Number_Of_Entries += 1;
	}
}


/*****************************************************************************
*
* FUNCTION
*
*       Get_Spline_Value
*
* INPUT
*
*       sp : a pointer to the spline to interpolate
*       p  : the parameter to interpolate the value for
*       v  : a pointer to a 5D vector to store the interpolated values in
*
* OUTPUT
*
*       v  : a pointer to a 5D vector to store the interpolated values in
*
* RETURNS
*
*       The value of the first element of the interpolated vector
*
* AUTHOR
*
*   Wolfgang Ortmann
*
* DESCRIPTION
*
*       Interpolates the spline at the given point.  If the point is outside the
*               range of parameters of the spline, returns the value at the
*               appropriate endpoint (ie. no extrapolation).  If there are not
*               enough points in the spline to do the desired type of
*               interpolation, does the next best type (cubic->quadratic->linear).
*
* CHANGES
*
*       Mark Wagner  8 Nov 2000 : Complete overhaul.  I am apalled at the
*               problems I found in the original implementation
*       Mark Wagner  25 Aug 2001 : Changed interpolation method to pre-compute
*               coefficients for cubic splines.
*       Mark Wagner  24 Feb 2002 : Added support for Catmull-Rom interpolation
*               Re-arranged the code to make future additions cleaner by moving
*                more of the code into the "switch" statement
*
******************************************************************************/

DBL Get_Spline_Val(SPLINE *sp, DBL p, EXPRESS v, int *Terms)
{
	int i, k;
	int last;
	SPLINE_ENTRY * se;

	*Terms = sp->Terms;

	if(!sp->Coeffs_Computed)
	{
		switch(sp->Type)
		{
			case NATURAL_SPLINE:
				Precompute_Cubic_Coeffs(sp);
				break;
			default:
				break;
		}
	}
/* [JG] flyspray #294, cache is not thread-safe
	// check if the value is in the cache
	if((sp->Cache_Point == p) && (sp->Cache_Type == sp->Type))
	{
		if(sp->Cache_Valid == true) // doing this here is more efficient as it is rarely false [trf]
		{
			Assign_Express(v, sp->Cache_Data);
			return sp->Cache_Data[0];
		}
	}

	// init some cache data
	sp->Cache_Valid = false;
	sp->Cache_Type = sp->Type;
	sp->Cache_Point = p;
 */
	last = sp->Number_Of_Entries-1;
	se = sp->SplineEntries;

	if(last == 0)
	{/* if only one entry then return this */
		for(k=0; k<5; k++)
			v[k] = se[0].vec[k];
		return se[0].vec[0];
	}

	/* Find which spline segment we're in.  i is the control point at the end of the segment */
	i = findt(sp, p);

	switch(sp->Type)
	{
		case LINEAR_SPLINE:
			for(k=0; k<5; k++)
			{
				/* If outside spline range, return first or last point */
				if(i == 0)
					v[k] = se[0].vec[k];
				else if(i > last)
					v[k] = se[last].vec[k];
				/* Else, normal case */
				else
					v[k] = linear_interpolate(se, i-1, k, p);
			}
			break;
		case QUADRATIC_SPLINE:
			for(k=0; k<5; k++)
			{
				/* If outside the spline range, return the first or last point */
				if(i == 0)
					v[k] = se[0].vec[k];
				else if(i > last)
					v[k] = se[last].vec[k];
				/* If not enough points, reduce order */
				else if(last == 1)
					v[k] = linear_interpolate(se, i-1, k, p);
				/* Normal case: between the second and last points */
				else if(i > 1)
				{
					v[k] = quadratic_interpolate(se, i-1, k, p);
				}
				else /* Special case: between first and second points */
				{
					v[k] = quadratic_interpolate(se, i, k, p);
				}
			}
			break;
		case NATURAL_SPLINE:
			for(k=0; k<5; k++)
			{
				/* If outside the spline range, return the first or last point */
				if(i == 0)
					v[k] = se[0].vec[k];
				else if(i > last)
					v[k] = se[last].vec[k];
				/* Else, normal case.  cubic_interpolate can handle the case of not enough points */
				else
					v[k] = natural_interpolate(se, i-1, k, p);
			}
			break;
		case CATMULL_ROM_SPLINE:
			for(k=0; k<5; k++)
			{
				/* If only two points, return their average */
				if(last == 1)
					v[k] = (se[0].vec[k] + se[1].vec[k])/2.0;
				/* Catmull-Rom: If only three points, return the second one */
				/* Catmull-Rom: Can't interpolate before second point or after next-to-last */
				else if(i < 2)
					v[k] = se[1].vec[k];
				else if(i >= last)
					v[k] = se[last-1].vec[k];
				/* Else, normal case */
				else
					v[k] = catmull_rom_interpolate(se, i-1, k, p);
			}
			break;
		default:
			throw POV_EXCEPTION_STRING("Unknown spline type found.\n");

	}

/* [JG] flyspray #294, cache is not thread-safe
	// put data in cache
	Assign_Express(sp->Cache_Data, v);
	sp->Cache_Valid = true;
 */

	return v[0];
}

}
