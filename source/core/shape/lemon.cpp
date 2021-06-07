//******************************************************************************
///
/// @file core/shape/lemon.cpp
///
/// Implementation of the lemon geometric primitive.
///
/// @author Jerome Grimbert
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2019 Persistence of Vision Raytracer Pty. Ltd.
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
#include "core/shape/lemon.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/messenger.h"

// POV-Ray header files (core module)
#include "core/math/matrix.h"
#include "core/math/polynomialsolver.h"
#include "core/render/ray.h"
#include "core/scene/tracethreaddata.h"
#include "core/support/statistics.h"

// this must be the last file included
#include "base/povdebug.h"


namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

// Tolerance used for degenerated torus detection and intersection of flat extremities
// as it is used only with low power, precision can be high
const DBL Lemon_Tolerance = 1.0e-10;
// from spheres.cpp, otherwise noisy sphere, used with square and root, so less precision
const DBL DEPTH_TOLERANCE = 1.0e-6;

// Tolerance used for order reduction during root finding.
// TODO FIXME - can we use EPSILON or a similar more generic constant instead?
const DBL ROOT_TOLERANCE = 1.0e-4;


/*****************************************************************************
*
* FUNCTION
*
*   All_Intersections
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Jerome Grimbert
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

bool Lemon::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    bool Intersection_Found;
    int cnt, i;
    Vector3d Real_Normal;
    Vector3d Real_Pt,INormal;
    LEMON_INT I[4];
    Vector3d P,D;
    DBL len;

    Thread->Stats()[Ray_Lemon_Tests]++;
    MInvTransPoint(P, ray.Origin, Trans);
    MInvTransDirection(D, ray.Direction, Trans);
    len = D.length();
    D /= len;

    Intersection_Found = false;

    if ((cnt = Intersect(P, D, I, Thread->Stats())) != 0)
    {
        for (i = 0; i < cnt; i++)
        {
            Real_Pt = ray.Origin + I[i].d/len * ray.Direction;

            if (Clip.empty() || Point_In_Clip(Real_Pt, Clip, Thread))
            {
                INormal = I[i].n;
                MTransNormal(Real_Normal, INormal, Trans);
                Real_Normal.normalize();

                Depth_Stack->push(Intersection(I[i].d/len,Real_Pt,Real_Normal,this));
                Intersection_Found = true;
            }
        }
    }
    if(Intersection_Found)
    {
        Thread->Stats()[Ray_Lemon_Tests_Succeeded]++;
    }
    return (Intersection_Found);
}


/*****************************************************************************
*
* FUNCTION
*
*   intersect
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Jerome Grimbert
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

int Lemon::Intersect(const Vector3d& P, const Vector3d& D, LEMON_INT *Intersection, RenderStatistics& stats) const
{
    int i = 0;
    DBL a, b, c[5], r[4];
    DBL d;
    DBL R2,r2, Pz2, Dz2, PDz2, k1, k2, horizontal, vertical, OCSquared;
    int n;

    Vector3d Padj, Second_Center,Ipoint, INormal;

    Second_Center = Vector3d(0, 0, VerticalPosition);
    r2 = Sqr(inner_radius);

    if (HorizontalPosition < -Lemon_Tolerance )
    { // use a torus
        Padj = P - Second_Center;
        R2 = Sqr(HorizontalPosition);

        Pz2 = Padj[Z] * Padj[Z];
        Dz2 = D[Z] * D[Z];
        PDz2 = Padj[Z] * D[Z];

        k1 = Padj[X] * Padj[X] + Padj[Y] * Padj[Y] + Pz2 - R2 - r2;
        k2 = Padj[X] * D[X] + Padj[Y] * D[Y] + PDz2;
        // this is just like a big torus
        c[0] = 1.0;

        c[1] = 4.0 * k2;

        c[2] = 2.0 * (k1 + 2.0 * (k2 * k2 + R2 * Dz2));

        c[3] = 4.0 * (k2 * k1 + 2.0 * R2 * PDz2);

        c[4] = k1 * k1 + 4.0 * R2 * (Pz2 - r2);

        n = Solve_Polynomial(4, c, r, Test_Flag(this, STURM_FLAG), ROOT_TOLERANCE, stats);
        while (n--)
        {
            // here we only keep the 'lemon' inside the torus
            // and dismiss the 'apple'
            // If you find a solution to resolve the rotation of
            //   (x + r)^2 + z^2 = R^2 around z (so replacing x by sqrt(x^2+y^2))
            // with something which is faster than a 4th degree polynome,
            // please feel welcome to update and share...

            Ipoint = P + r[n] * D;
            vertical = Ipoint[Z];
            if ((vertical >= 0.0) && (vertical <= 1.0))
            {
                horizontal = sqrt(Sqr(Ipoint[X]) + Sqr(Ipoint[Y]));
                OCSquared = Sqr((horizontal - HorizontalPosition)) + Sqr((vertical - VerticalPosition));
                if (fabs(OCSquared - r2 ) < ROOT_TOLERANCE)
                {
                    Intersection[i].d = r[n];
                    INormal = Ipoint;
                    INormal[Z] -= VerticalPosition;
                    INormal[X] -= (INormal[X] * HorizontalPosition / horizontal);
                    INormal[Y] -= (INormal[Y] * HorizontalPosition / horizontal);
                    INormal.normalize();
                    Intersection[i].n = INormal;
                    ++i;
                }
            }
        }
    }
    else
    {
        // use a sphere, as the center is on the axis
        // keeping a torus would trigger a problem of self-coincidence surface
        DBL OCSquared, t_Closest_Approach, Half_Chord, t_Half_Chord_Squared;
        DBL Depth;
        Vector3d Origin_To_Center;

        Origin_To_Center = Second_Center - P;

        OCSquared = Origin_To_Center.lengthSqr();

        t_Closest_Approach = dot(Origin_To_Center, D);

        if (!((OCSquared >= r2) && (t_Closest_Approach < EPSILON )))
        {

            t_Half_Chord_Squared = r2 - OCSquared + Sqr(t_Closest_Approach);

            if (t_Half_Chord_Squared > EPSILON)
            {
                Half_Chord = sqrt(t_Half_Chord_Squared);
                // first intersection
                Depth = t_Closest_Approach - Half_Chord;
                if((Depth > DEPTH_TOLERANCE) && (Depth < MAX_DISTANCE))
                {

                    Ipoint = P + Depth * D;
                    vertical = Ipoint[Z];
                    if ((vertical >= 0.0) && (vertical <= 1.0))
                    {
                        Intersection[i].d = Depth;
                        INormal = Ipoint;
                        INormal[Z] -= VerticalPosition;
                        INormal.normalize();
                        Intersection[i].n = INormal;
                        ++i;
                    }
                }
                // second intersection
                Depth = t_Closest_Approach + Half_Chord;
                if((Depth > DEPTH_TOLERANCE) && (Depth < MAX_DISTANCE))
                {
                    Ipoint = P + Depth * D;
                    vertical = Ipoint[Z];
                    if ((vertical >= 0.0) && (vertical <= 1.0))
                    {
                        Intersection[i].d = Depth;
                        INormal = Ipoint;
                        INormal[Z] -= VerticalPosition;
                        INormal.normalize();
                        Intersection[i].n = INormal;
                        ++i;
                    }
                }
            }
        }

    }

    // intersection with apex and base disc
    if (Test_Flag(this, CLOSED_FLAG) && (fabs(D[Z]) > EPSILON))
    {
        d = - P[Z] / D[Z];

        a = (P[X] + d * D[X]);

        b = (P[Y] + d * D[Y]);

        if (((Sqr(a) + Sqr(b)) <= Sqr(base_radius)) && (d > Lemon_Tolerance) && (d < MAX_DISTANCE))
        {
            Intersection[i].d   = d ;
            Intersection[i].n = Vector3d(0, 0, -1);
            ++i;
        }

        d = (1.0 - P[Z]) / D[Z];

        a = (P[X] + d * D[X]);

        b = (P[Y] + d * D[Y]);

        if (((Sqr(a) + Sqr(b)) <= Sqr(apex_radius))
                && (d > Lemon_Tolerance) && (d < MAX_DISTANCE))
        {
            Intersection[i].d   = d ;
            Intersection[i].n = Vector3d(0, 0, 1);
            ++i;
        }
    }

    return (i);
}


/*****************************************************************************
*
* FUNCTION
*
*   Inside_Lemon
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Jerome Grimbert
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

bool Lemon::Inside(const Vector3d& IPoint, TraceThreadData *Thread) const
{
    DBL OCSquared;
    DBL horizontal, vertical;
    bool INSide = false;
    Vector3d New_Point;
    MInvTransPoint(New_Point, IPoint, Trans);
    vertical = New_Point[Z];
    if ((vertical >= 0.0) && (vertical <= 1.0))
    {
        horizontal = sqrt(Sqr(New_Point[X]) + Sqr(New_Point[Y]));
        OCSquared = Sqr(horizontal - HorizontalPosition) + Sqr((vertical - VerticalPosition));
        if (OCSquared < Sqr(inner_radius))
        {
            INSide = true;
        }
    }
    if (Test_Flag(this, INVERTED_FLAG))
    {
        return !INSide;
    }
    else
    {
        return INSide;
    }
}


/*****************************************************************************
*
* FUNCTION
*
*   Lemon_Normal
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Jerome Grimbert
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Lemon::Normal(Vector3d& Result, Intersection *Inter, TraceThreadData *Thread) const
{
    Result = Inter->INormal;
}


/*****************************************************************************
*
* FUNCTION
*
*   Translate_Lemon
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Jerome Grimbert
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Lemon::Translate(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}


/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Lemon
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Jerome Grimbert
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Lemon::Rotate(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}


/*****************************************************************************
*
* FUNCTION
*
*   Scale_Lemon
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Jerome Grimbert
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Lemon::Scale(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Lemon
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Jerome Grimbert
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Lemon::Transform(const TRANSFORM *tr)
{
    if(Trans == nullptr)
        Trans = Create_Transform();

    Compose_Transforms(Trans, tr);

    Compute_BBox();
}


/*****************************************************************************
*
* FUNCTION
*
*   Create_Lemon
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Jerome Grimbert
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

Lemon::Lemon() : ObjectBase(LEMON_OBJECT)
{
    apex = Vector3d(0.0, 0.0, 1.0);
    base = Vector3d(0.0, 0.0, 0.0);

    apex_radius = 0.0;
    base_radius = 0.0;

    inner_radius = 0.5;

    Trans = Create_Transform();

    /* Lemon has capped ends by default. */

    Set_Flag(this, CLOSED_FLAG);

    /* Default bounds */

    Make_BBox(BBox, -1.0, -1.0, 0.0, 2.0, 2.0, 1.0);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Lemon
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Jerome Grimbert
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

ObjectPtr Lemon::Copy()
{
    Lemon *New = new Lemon();

    Destroy_Transform(New->Trans);
    *New = *this;
    New->Trans = Copy_Transform(Trans);

    return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Lemon_Data
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Jerome Grimbert
*
* DESCRIPTION
*
* CHANGES
*
*
******************************************************************************/

void Lemon::Compute_Lemon_Data(GenericMessenger& messenger, const MessageContext& context)
{
    DBL len;
    Vector3d axis;

    /* Process the primitive specific information */

    /* Find the axis and axis length */

    axis = apex - base;

    len = axis.length();

    if (len < EPSILON)
    {
        throw POV_EXCEPTION_STRING("Degenerate lemon.");
    }
    else
    {
        axis /= len; //normalize
    }
    /* adjust the various radius */
    apex_radius /= len;
    base_radius /= len;
    inner_radius /= len;
    /* Determine alignment and scale */
    Compute_Coordinate_Transform(Trans, base, axis, len, len);

    /* check constraint on inner_radius before solving the system of equation to find the
     * center of the minor circle of the torus
     *
     * base is at origin (0,0)
     * apex is at (0,1)
     * let's note base_radius as a, a>=0
     * let's note apex_radius as b, b>=0
     * let's note inner_radius as r, r>=0
     * center of minor circle is at (x,y), with x<=0
     *
     * (x-a)^2 + y^2 - r^2 = 0  : minor circle must pass on circle at base
     * (x-b)^2 + (1-y)^2 - r^2 = 0 : minor circle must pass on circle at apex
     *
     * r must be equal or greater than sqrt( a^4-2a^2(b^2-1)+(b^2+1)^2 ) / 2
     *
     * then with f = sqrt((a^2-2ab+b^2-4r^2+1)/(a^2-2ab+b^2+1))
     *
     *     x = (a+b-f)/2
     *     y = (f(b-a)+1)/2
     */
    DBL low = sqrt(base_radius*base_radius*base_radius*base_radius - 2*base_radius*base_radius*(apex_radius*apex_radius-1.0)+(apex_radius*apex_radius+1.0)*(apex_radius*apex_radius+1.0))/2.0;
    if (inner_radius < low )
    {
        inner_radius = low;
        messenger.WarningAt(kWarningGeneral, context,
                            "Inner (last) radius of lemon is too small. Minimal would be %g. Value has been adjusted.",
                            inner_radius * len);
    }

    DBL f = sqrt(-(base_radius*base_radius-2.0*base_radius*apex_radius+apex_radius*apex_radius-4.0*inner_radius*inner_radius+1.0)/(base_radius*base_radius-2.0*base_radius*apex_radius+apex_radius*apex_radius+1.0));
    /*
     * Attention: valid HorizontalPosition is negative, always (or null)
     * It is of particular importance when using with torus evaluation and normal computation
     */
    HorizontalPosition = (base_radius+apex_radius-f)/2.0;
    VerticalPosition = ((apex_radius-base_radius)*f+1.0)/2.0;
}


/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Lemon
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Jerome Grimbert
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

Lemon::~Lemon()
{}


/*****************************************************************************
*
* FUNCTION
*
*   Compute_Lemon_BBox
*
* INPUT
*
*   Lemon
*
* OUTPUT
*
*   Lemon
*
* RETURNS
*
* AUTHOR
*
*   Jerome Grimbert
*
* DESCRIPTION
*
*   Calculate the bounding box of a lemon
*
* CHANGES
*
*   May 2016 : Creation.
*
******************************************************************************/

void Lemon::Compute_BBox()
{
    DBL m = inner_radius+HorizontalPosition; //HorizontalPosition is negative
    if ( VerticalPosition < 0)
    {
      m = base_radius;
    }
    if ( VerticalPosition > 1)
    {
      m = apex_radius;
    }
    Make_BBox(BBox, -m, -m, 0.0, 2.0*m, 2.0*m, 1.0);

    Recompute_BBox(&BBox, Trans);
}


#ifdef POV_ENABLE_LEMON_UV

/*****************************************************************************
*
* FUNCTION
*
*   Lemon_UVCoord
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Jerome Grimbert
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Lemon::UVCoord(Vector2d& Result, const Intersection *Inter) const
{
    CalcUV(Inter->IPoint, Result);
}


/*****************************************************************************
*
* FUNCTION
*
*   CalcUV
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Jerome Grimbert
*
* DESCRIPTION
*
*   Calculate the u/v coordinate of a point on an lemon
*
* CHANGES
*
******************************************************************************/

void Lemon::CalcUV(const Vector3d& IPoint, Vector2d& Result) const
{
    DBL len, x, y, z;
    DBL phi, theta;
    Vector3d P;

    // Transform the ray into the lemon space.
    MInvTransPoint(P, IPoint, Trans);

    // the center of UV coordinate is <0, 0, 1/2>
    // No fancy translation of the center as the radii are changed
    x = P[X];
    y = P[Y];
    z = P[Z]-0.5;

    // Determine its angle from the point (1, 0, 0) in the x-y plane.
    len = x * x + y * y;

    if ((P[Z]>EPSILON)&&(P[Z]<(1.0-EPSILON)))
    {
    // when not on a face, the range 0.25 to 0.75 is used (just plain magic 25% for face, no other reason, but it makes C-Lipka happy)
        phi = 0.75-0.5*P[Z];
    }
    else if (P[Z]>EPSILON)
    {
    // aka P[Z] is 0, use the apex_radius, from 75% to 100% (at the very center)
        phi = 0.0;
        if (apex_radius)
        {
            phi = sqrt(len)/(apex_radius*4);
        }
    }
    else
    {
    // aka P[Z] is 1, use the base_radius, from 0% (at the very center) to 25%
       phi = 1.0;
       if (base_radius)
       {
           phi = 1.0-(sqrt(len)/(base_radius*4));
       }
    }


    if (len > EPSILON)
    {
        len = sqrt(len);
        if (y == 0.0)
        {
            if (x > 0)
                theta = 0.0;
            else
                theta = M_PI;
        }
        else
        {
            theta = acos(x / len);
            if (y < 0.0)
                theta = TWO_M_PI - theta;
        }

        theta /= TWO_M_PI; // This will be from 0 to 1
    }
    else
        // This point is at one of the poles. Any value of xcoord will be ok...
        theta = 0;

    Result[U] = theta;
    Result[V] = phi;

}

#endif // POV_ENABLE_LEMON_UV

}
// end of namespace pov
