//******************************************************************************
///
/// @file core/math/spline.cpp
///
/// Implementation of spline-related maths.
///
/// @note   This file currently contains only stuff for the SDL's function-alike
///         spline feature; as such, it would naturally belong in the parser
///         module. However, it is planned for the polymorphic type hierarchy
///         herein to also absorb the spline-specific maths for the geometric
///         primtitives (which is currently embedded in the respective
///         primitives' code), and the file has already been moved to the core
///         module in preparation.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd.
///
/// POV-Ray is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// POV-Ray is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
///
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///
/// ----------------------------------------------------------------------------
///
/// POV-Ray is based on the popular DKB raytracer version 2.12.
/// DKBTrace was originally written by David K. Buck.
/// DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
///
/// @endparblock
///
//******************************************************************************

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/math/spline.h"

#include <cmath>
#include <cstdio>
#include <cstring>

#include <limits>

#include "base/pov_err.h"
#include "base/types.h"

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

DBL linear_interpolate(const SplineEntryList& se, SplineEntryList::size_type i, int k, DBL p);
DBL quadratic_interpolate(const SplineEntryList& se, SplineEntryList::size_type i, int k, DBL p);
DBL natural_interpolate(const SplineEntryList& se, SplineEntryList::size_type i, int k, DBL p);
DBL catmull_rom_interpolate(const SplineEntryList& se, SplineEntryList::size_type i, int k, DBL p);
SplineEntryList::size_type findt(const GenericSpline * sp, DBL Time);
void mkfree(GenericSpline * sp, SplineEntryList::size_type i);
void Precompute_Cubic_Coeffs(GenericSpline *sp);

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

void Precompute_Cubic_Coeffs(GenericSpline *sp)
{
    SplineEntryList::size_type i, k;
    DBL *h;
    DBL *b;
    DBL *u;
    DBL *v;

    SplineEntryList::size_type numEntries = sp->SplineEntries.size();
    POV_ASSERT(numEntries >= 2);

    h = new DBL[numEntries];
    b = new DBL[numEntries];
    u = new DBL[numEntries];
    v = new DBL[numEntries];

    for(k = 0; k < 5; k++)
    {
        for(i = 0; i <= numEntries - 2; i++)
        {
            h[i] = sp->SplineEntries[i+1].par - sp->SplineEntries[i].par;
            b[i] = (sp->SplineEntries[i+1].vec[k] - sp->SplineEntries[i].vec[k])/h[i];
        }
        u[1] = 2*(h[0]+h[1]);
        v[1] = 6*(b[1]-b[0]);
        for(i = 2; i <= numEntries - 2; i++)
        {
            u[i] = 2*(h[i]+h[i-1]) - (h[i-1]*h[i-1])/u[i-1];
            v[i] = 6*(b[i]-b[i-1]) - (h[i-1]*v[i-1])/u[i-1];
        }
        sp->SplineEntries.back().coeff[k] = 0;
        for(i = numEntries-2; i > 0; i--)
        {
            sp->SplineEntries[i].coeff[k] = (v[i] - h[i]*sp->SplineEntries[i+1].coeff[k])/u[i];
        }
        sp->SplineEntries[0].coeff[k] = 0;
    }
    sp->Coeffs_Computed = true;

    delete[] h;
    delete[] b;
    delete[] u;
    delete[] v;
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

DBL linear_interpolate(const SplineEntryList& se, SplineEntryList::size_type i, int k, DBL p)
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

DBL quadratic_interpolate(const SplineEntryList& se, SplineEntryList::size_type i, int k, DBL p)
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

DBL natural_interpolate(const SplineEntryList& se, SplineEntryList::size_type i, int k, DBL p)
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

DBL catmull_rom_interpolate(const SplineEntryList& se, SplineEntryList::size_type i, int k, DBL p)
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

SplineEntryList::size_type findt(const GenericSpline * sp, DBL Time)
{
    SplineEntryList::size_type i, i2;
    const SplineEntryList& se = sp->SplineEntries;
    if(sp->SplineEntries.empty()) return 0;
    SplineEntryList::size_type numEntries = se.size();

    if(Time <= se.front().par) return 0;

    if(Time >= se.back().par) return numEntries;

    i = numEntries / 2;
    /* Bracket the proper entry */
    if( Time > se[i].par ) /* i is lower, i2 is upper */
    {
        i2 = numEntries-1;
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

void mkfree(GenericSpline * sp, SplineEntryList::size_type i)
{
    SplineEntryList::size_type j;
    SplineEntryList& se = sp->SplineEntries;

    se.resize(se.size()+1);
    for (j=sp->SplineEntries.size()-1; j>i; j--)
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

GenericSpline::GenericSpline() :
    Coeffs_Computed(false),
    Terms(2),
    ref_count(1)
{}

LinearSpline::LinearSpline() : GenericSpline()
{}

QuadraticSpline::QuadraticSpline() : GenericSpline()
{}

NaturalSpline::NaturalSpline() : GenericSpline()
{}

CatmullRomSpline::CatmullRomSpline() : GenericSpline()
{}


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

GenericSpline::GenericSpline(const GenericSpline& o) :
    SplineEntries(o.SplineEntries),
    Coeffs_Computed(false),
    Terms(o.Terms),
    ref_count(1)
{}

LinearSpline::LinearSpline(const GenericSpline& o) : GenericSpline(o)
{}

QuadraticSpline::QuadraticSpline(const GenericSpline& o) : GenericSpline(o)
{}

NaturalSpline::NaturalSpline(const GenericSpline& o) : GenericSpline(o)
{}

CatmullRomSpline::CatmullRomSpline(const GenericSpline& o) : GenericSpline(o)
{}

GenericSpline * Copy_Spline(const GenericSpline * Old)
{
    return Old->Clone();
}


void GenericSpline::AcquireReference()
{
    if (ref_count >= std::numeric_limits<SplineRefCount>::max())
        throw POV_EXCEPTION_STRING("Too many unresolved references to single spline\n");
    ref_count ++;
}

void Acquire_Spline_Reference(GenericSpline* sp)
{
    if (sp)
        sp->AcquireReference();
}

void GenericSpline::ReleaseReference()
{
    if (ref_count <= 0)
        throw POV_EXCEPTION_STRING("Internal error: Invalid spline reference count\n");
    ref_count --;
}

void Release_Spline_Reference(GenericSpline* sp)
{
    if (sp)
        Destroy_Spline(sp);
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

GenericSpline::~GenericSpline()
{}

void Destroy_Spline(GenericSpline* sp)
{
    sp->ReleaseReference();
    if (sp->ref_count == 0)
        delete sp;
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

void Insert_Spline_Entry(GenericSpline * sp, DBL p, const EXPRESS& v)
{
    SplineEntryList::size_type i;
    int k;

    /* Reset the Coeffs_Computed flag.  Inserting a new point invalidates
     *  pre-computed coefficients */
    sp->Coeffs_Computed = false;
    i = findt(sp, p);
    // If p is already in the spline, replace it.
    bool replace = false;
    if (!sp->SplineEntries.empty())
    {
        // If p matches the _last_ spline entry, `findt()` is implemented to return `sp->SplineEntries.size()`
        // instead of the index of the matching entry; this has some benefits in the other places `findt()` is used,
        // but requires the following workaround here.
        if (i < sp->SplineEntries.size())
            replace = (sp->SplineEntries[i].par == p);
        else if (sp->SplineEntries[i-1].par == p)
        {
            --i;
            replace = true;
        }
    }
    if(replace)
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

void LinearSpline::Get(DBL p, EXPRESS& v)
{
    if (SplineEntries.size() == 1)
        memcpy(&v, &SplineEntries.front().vec, sizeof(EXPRESS));
    else
    {
        /* Find which spline segment we're in.  i is the control point at the end of the segment */
        SplineEntryList::size_type i = findt(this, p);
        for(int k=0; k<5; k++)
        {
            /* If outside spline range, return first or last point */
            if(i == 0)
                v[k] = SplineEntries.front().vec[k];
            else if(i >= SplineEntries.size())
                v[k] = SplineEntries.back().vec[k];
            /* Else, normal case */
            else
                v[k] = linear_interpolate(SplineEntries, i-1, k, p);
        }
    }
}

void QuadraticSpline::Get(DBL p, EXPRESS& v)
{
    if (SplineEntries.size() == 1)
        memcpy(&v, &SplineEntries.front().vec, sizeof(EXPRESS));
    else
    {
        /* Find which spline segment we're in.  i is the control point at the end of the segment */
        SplineEntryList::size_type i = findt(this, p);
        for(int k=0; k<5; k++)
        {
            /* If outside the spline range, return the first or last point */
            if(i == 0)
                v[k] = SplineEntries.front().vec[k];
            else if(i >= SplineEntries.size())
                v[k] = SplineEntries.back().vec[k];
            /* If not enough points, reduce order */
            else if(SplineEntries.size() == 2)
                v[k] = linear_interpolate(SplineEntries, i-1, k, p);
            else
            {
                /* Normal case: between the second and last points */
                if(i > 1)
                    v[k] = quadratic_interpolate(SplineEntries, i-1, k, p);
                else /* Special case: between first and second points */
                    v[k] = quadratic_interpolate(SplineEntries, i, k, p);
            }
        }
    }
}

void NaturalSpline::Get(DBL p, EXPRESS& v)
{
    if (SplineEntries.size() == 1)
        memcpy(&v, &SplineEntries.front().vec, sizeof(EXPRESS));
    else
    {
        if (!Coeffs_Computed)
            Precompute_Cubic_Coeffs(this);
        /* Find which spline segment we're in.  i is the control point at the end of the segment */
        SplineEntryList::size_type i = findt(this, p);
        for(int k=0; k<5; k++)
        {
            /* If outside the spline range, return the first or last point */
            if(i == 0)
                v[k] = SplineEntries.front().vec[k];
            else if(i >= SplineEntries.size())
                v[k] = SplineEntries.back().vec[k];
            /* Else, normal case.  cubic_interpolate can handle the case of not enough points */
            else
                v[k] = natural_interpolate(SplineEntries, i-1, k, p);
        }
    }
}

void CatmullRomSpline::Get(DBL p, EXPRESS& v)
{
    if (SplineEntries.size() == 1)
        memcpy(&v, &SplineEntries.front().vec, sizeof(EXPRESS));
    else
    {
        /* Find which spline segment we're in.  i is the control point at the end of the segment */
        SplineEntryList::size_type i = findt(this, p);
        for(int k=0; k<5; k++)
        {
            /* If only two points, return their average */
            if(SplineEntries.size() == 2)
                v[k] = (SplineEntries[0].vec[k] + SplineEntries[1].vec[k])/2.0;
            /* Catmull-Rom: If only three points, return the second one */
            else if(i < 2)
                v[k] = SplineEntries[1].vec[k];
            /* Catmull-Rom: Can't interpolate before second point or after next-to-last */
            else if(i >= SplineEntries.size()-1)
                v[k] = SplineEntries[SplineEntries.size()-2].vec[k];
            /* Else, normal case */
            else
                v[k] = catmull_rom_interpolate(SplineEntries, i-1, k, p);
        }
    }
}

DBL Get_Spline_Val(GenericSpline *sp, DBL p, EXPRESS& v, int *Terms)
{
    *Terms = sp->Terms;
    sp->Get(p, v);
    return v[0];
}

}
