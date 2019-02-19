//******************************************************************************
///
/// @file core/material/warp.cpp
///
/// Implementation of warps.
///
/// The code in this file implements functions that warp or modify the point at
/// which a texture pattern is evaluated.
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
#include "core/material/warp.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/pov_err.h"

// POV-Ray header files (core module)
#include "core/material/noise.h"
#include "core/material/pattern.h"
#include "core/material/texture.h"
#include "core/math/matrix.h"
#include "core/math/randomsequence.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

const DBL COORDINATE_LIMIT = 1.0e17;

/*****************************************************************************
* Static functions
******************************************************************************/

static RandomDoubleSequence WarpRands(0.0, 1.0, 32768);

/*****************************************************************************
*
* FUNCTION
*
*   Warp_EPoint
*
* INPUT
*
*   EPoint -- The original point in 3d space at which a pattern
*   is evaluated.
*   TPat   -- Texture pattern struct
*
* OUTPUT
*
*   TPoint -- Point after turbulence and transform
*   have been applied
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Warp_EPoint (Vector3d& TPoint, const Vector3d& EPoint, const TPATTERN *TPat)
{
    WarpList& warps=TPat->pattern->warps;

    TPoint = EPoint;

    for (WarpList::reverse_iterator iWarp = warps.rbegin(); iWarp != warps.rend(); iWarp ++)
    {
        WarpPtr& warp = *iWarp;
        warp->WarpPoint(TPoint);
    }

    for (int i=X; i<=Z; i++)
    {
        if (TPoint[i] > COORDINATE_LIMIT)
            TPoint[i]= COORDINATE_LIMIT;
        else if (TPoint[i] < -COORDINATE_LIMIT)
            TPoint[i] = -COORDINATE_LIMIT;
    }
}

bool BlackHoleWarp::WarpPoint(Vector3d& TPoint) const
{
    Vector3d C = Center;

    if (Repeat)
    {
        int blockX = 0, blockY = 0, blockZ = 0;

        /* first, get the block number we're in for each dimension  */
        /* block numbers are (currently) calculated relative to 0   */
        /* we use floor () since it correctly returns -1 for the
            first block below 0 in each axis                         */
        /* one final point - we could run into overflow problems if
            the repeat vector was small and the scene very large.    */
        if (Repeat_Vector[X] >= EPSILON)
            blockX = (int) floor (TPoint[X] / Repeat_Vector[X]);

        if (Repeat_Vector[Y] >= EPSILON)
            blockY = (int) floor (TPoint[Y] / Repeat_Vector[Y]);

        if (Repeat_Vector[Z] >= EPSILON)
            blockZ = (int) floor (TPoint[Z] / Repeat_Vector[Z]);

        if (Uncertain)
        {
            /* if the position is uncertain calculate the new one first */
            /* this will allow the same numbers to be returned by frand */

            int seed = Hash3d (blockX, blockY, blockZ);
            C[X] += WarpRands(seed)     * Uncertainty_Vector[X];
            C[Y] += WarpRands(seed + 1) * Uncertainty_Vector[Y];
            C[Z] += WarpRands(seed + 2) * Uncertainty_Vector[Z];
        }

        C[X] += Repeat_Vector[X] * blockX;
        C[Y] += Repeat_Vector[Y] * blockY;
        C[Z] += Repeat_Vector[Z] * blockZ;
    }

    Vector3d Delta = TPoint - C;
    DBL Length = Delta.length();

    /* Length is the distance from the centre of the black hole */
    if (Length >= Radius)
        return true;

    if (Type == 0)
    {
        /* now convert the length to a proportion (0 to 1) that the point
            is from the edge of the black hole. a point on the perimeter
            of the black hole will be 0.0; a point at the centre will be
            1.0; a point exactly halfway will be 0.5, and so forth. */
        Length = (Radius - Length) / Radius;

        /* Strength is the magnitude of the transformation effect. firstly,
            apply the Power variable to Length. this is meant to provide a
            means of controlling how fast the power of the Black Hole falls
            off from its centre. if Power is 2.0, then the effect is inverse
            square. increasing power will cause the Black Hole to be a lot
            weaker in its effect towards its perimeter.

            finally we multiply Strength with the Black Hole's Strength
            variable. if the resultant value exceeds 1.0 we clip it to 1.0.
            this means a point will never be transformed by more than its
            original distance from the centre. the result of this clipping
            is that you will have an 'exclusion' area near the centre of
            the black hole where all points whose final value exceeded or
            equalled 1.0 were moved by a fixed amount. this only happens
            if the Strength value of the Black Hole was greater than one. */

        DBL S = pow (Length, Power) * Strength;
        if (S > 1.0) S = 1.0;

        /* if the Black Hole is inverted, it gives the impression of 'push-
            ing' the pattern away from its centre. otherwise it sucks. */
        Delta *= (Inverted ? -S : S);

        /* add the scaled Delta to the input point to end up with TPoint. */
        TPoint += Delta;
    }

    return true;
}

bool CubicWarp::WarpPoint(Vector3d& EPoint) const
{
    DBL x = EPoint[X], y = EPoint[Y], z = EPoint[Z];
    const DBL ax = fabs(x), ay = fabs(y), az = fabs(z);

    if(x >= 0 && x >= ay && x >= az)
    {
        EPoint[X] = 0.75 - 0.25*(z/x+1.0)/2.0;
        EPoint[Y] = 1.0/3.0 + (1.0/3.0)*(y/x+1.0)/2.0;
        EPoint[Z] = x;
    }
    else if(y >= 0 && y >= ax && y >= az)
    {
        EPoint[X] = 0.25 + 0.25*(x/y+1.0)/2.0;
        EPoint[Y] = 1.0 - (1.0/3.0)*(z/y+1.0)/2.0;
        EPoint[Z] = y;
    }
    else if(z >= 0 && z >= ax && z >= ay)
    {
        EPoint[X] = 0.25 + 0.25*(x/z+1.0)/2.0;
        EPoint[Y] = 1.0/3.0 + (1.0/3.0)*(y/z+1.0)/2.0;
        EPoint[Z] = z;
    }
    else if(x < 0 && x <= -ay && x <= -az)
    {
        x = -x;
        EPoint[X] = 0.25*(z/x+1.0)/2.0;
        EPoint[Y] = 1.0/3.0 + (1.0/3.0)*(y/x+1.0)/2.0;
        EPoint[Z] = x;
    }
    else if(y < 0 && y <= -ax && y <= -az)
    {
        y = -y;
        EPoint[X] = 0.25 + 0.25*(x/y+1.0)/2.0;
        EPoint[Y] = (1.0/3.0)*(z/y+1.0)/2.0;
        EPoint[Z] = y;
    }
    else
    {
        z = -z;
        EPoint[X] = 1.0 - 0.25*(x/z+1.0)/2.0;
        EPoint[Y] = 1.0/3.0 + (1.0/3.0)*(y/z+1.0)/2.0;
        EPoint[Z] = z;
    }

    return 1;
}

bool CylindricalWarp::WarpPoint(Vector3d& EPoint) const
{
    DBL len, theta;
    DBL x = EPoint[X];
    DBL y = EPoint[Y];
    DBL z = EPoint[Z];

    // Determine its angle from the point (1, 0, 0) in the x-z plane.
    len = sqrt(x * x + z * z);

    if(len == 0.0)
        return false;
    else
    {
        if(z == 0.0)
        {
            if(x > 0)
                theta = 0.0;
            else
                theta = M_PI;
        }
        else
        {
            theta = acos(x / len);
            if(z < 0.0)
                theta = TWO_M_PI - theta;
        }

        theta /= TWO_M_PI;  // This will be from 0 to 1
    }

    if(DistExp == 1.0)
        theta *= len;
    else if (DistExp != 0.0)
        theta *= pow(len, DistExp);

    x = theta;
    z = len;

    if((Orientation_Vector[X] == 0.0) &&
       (Orientation_Vector[Y] == 0.0) &&
       (Orientation_Vector[Z] == 1.0))
    {
        EPoint[X] = x;
        EPoint[Y] = y;
        EPoint[Z] = z;
    }
    else
    {
        EPoint[X] = (Orientation_Vector[X] * z) +
                    (Orientation_Vector[Y] * x) +
                    (Orientation_Vector[Z] * x);
        EPoint[Y] = (Orientation_Vector[X] * y) +
                    (Orientation_Vector[Y] * -z) +
                    (Orientation_Vector[Z] * y);
        EPoint[Z] = (Orientation_Vector[X] * -x) +
                    (Orientation_Vector[Y] * y) +
                    (Orientation_Vector[Z] * z);
    }

    return true;
}

bool IdentityWarp::WarpPoint(Vector3d& TPoint) const
{
    return true;
}

bool PlanarWarp::WarpPoint(Vector3d& EPoint) const
{
    DBL x = EPoint[X];
    DBL z = OffSet;
    DBL y = EPoint[Y];

    if((Orientation_Vector[X] == 0.0) &&
       (Orientation_Vector[Y] == 0.0) &&
       (Orientation_Vector[Z] == 1.0))
    {
        EPoint[X] = x;
        EPoint[Y] = y;
        EPoint[Z] = z;
    }
    else
    {
        EPoint[X] = (Orientation_Vector[X] * z) +
                    (Orientation_Vector[Y] * x) +
                    (Orientation_Vector[Z] * x);
        EPoint[Y] = (Orientation_Vector[X] * y) +
                    (Orientation_Vector[Y] * -z) +
                    (Orientation_Vector[Z] * y);
        EPoint[Z] = (Orientation_Vector[X] * -x) +
                    (Orientation_Vector[Y] * y) +
                    (Orientation_Vector[Z] * z);
    }

    return true;
}

bool RepeatWarp::WarpPoint(Vector3d& TPoint) const
{
    SNGL BlkNum = (SNGL)floor(TPoint[Axis]/Width);

    TPoint[Axis] -= BlkNum*Width;

    if (((int)BlkNum) & 1)
    {
        TPoint *= Flip;
        if ( Flip[Axis] < 0 )
            TPoint[Axis] += Width;
    }

    TPoint += (DBL)BlkNum * Offset;

    return true;
}

bool SphericalWarp::WarpPoint(Vector3d& EPoint) const
{
    DBL len, phi, theta,dist;
    DBL x = EPoint[X];
    DBL y = EPoint[Y];
    DBL z = EPoint[Z];

    // Make sure this vector is on the unit sphere.

    dist = sqrt(x * x + y * y + z * z);

    if(dist == 0.0)
        return 0;
    else
    {
        x /= dist;
        y /= dist;
        z /= dist;
    }

    // Determine its angle from the x-z plane.
    phi = 0.5 + asin(y) / M_PI; // This will be from 0 to 1

    // Determine its angle from the point (1, 0, 0) in the x-z plane.
    len = sqrt(x * x + z * z);
    if(len == 0.0)
    {
        // This point is at one of the poles. Any value of xcoord will be ok...
        theta = 0;
    }
    else
    {
        if(z == 0.0)
        {
            if(x > 0)
                theta = 0.0;
            else
                theta = M_PI;
        }
        else
        {
            theta = acos(x / len);
            if (z < 0.0)
                theta = TWO_M_PI - theta;
        }
        theta /= TWO_M_PI;  /* This will be from 0 to 1 */
    }

    if(DistExp == 1.0)
    {
        theta *= dist;
        phi *= dist;
    }
    else if(DistExp != 0.0)
    {
        theta *= pow(dist, DistExp);
        phi *= pow(dist, DistExp);
    }

    x = theta;
    z = dist;
    y = phi;

    if((Orientation_Vector[X] == 0.0) &&
       (Orientation_Vector[Y] == 0.0) &&
       (Orientation_Vector[Z] == 1.0))
    {
        EPoint[X] = x;
        EPoint[Y] = y;
        EPoint[Z] = z;
    }
    else
    {
        EPoint[X] = (Orientation_Vector[X] * z) +
                    (Orientation_Vector[Y] * x) +
                    (Orientation_Vector[Z] * x);
        EPoint[Y] = (Orientation_Vector[X] * y) +
                    (Orientation_Vector[Y] * -z) +
                    (Orientation_Vector[Z] * y);
        EPoint[Z] = (Orientation_Vector[X] * -x) +
                    (Orientation_Vector[Y] * y) +
                    (Orientation_Vector[Z] * z);
    }

    return 1;
}

bool ToroidalWarp::WarpPoint(Vector3d& EPoint) const
{
    DBL len, phi, theta;
    DBL r0;
    DBL x = EPoint[X];
    DBL y = EPoint[Y];
    DBL z = EPoint[Z];

    r0 = MajorRadius;

    // Determine its angle from the x-axis.

    len = sqrt(x * x + z * z);

    if(len == 0.0)
        return false;
    else
    {
        if(z == 0.0)
        {
            if(x > 0)
                theta = 0.0;
            else
                theta = M_PI;
        }
        else
        {
            theta = acos(x / len);
            if(z < 0.0)
                theta = TWO_M_PI - theta;
        }
    }

    theta = 0.0 - theta;

    // Now rotate about the y-axis to get the point (x, y, z) into the x-y plane.

    x = len - r0;
    len = sqrt(x * x + y * y);
    phi = acos(-x / len);
    if (y > 0.0)
        phi = TWO_M_PI - phi;

    // Determine the parametric coordinates.

    theta /= (-TWO_M_PI);

    phi /= TWO_M_PI;

    if (DistExp == 1.0)
    {
        theta *= len;
        phi *= len;
    }
    else if (DistExp != 0.0)
    {
        theta *= pow(len, DistExp);
        phi *= pow(len, DistExp);
    }

    x = theta;
    z = len;
    y = phi;

    if((Orientation_Vector[X] == 0.0) &&
       (Orientation_Vector[Y] == 0.0) &&
       (Orientation_Vector[Z] == 1.0))
    {
        EPoint[X] = x;
        EPoint[Y] = y;
        EPoint[Z] = z;
    }
    else
    {
        EPoint[X] = (Orientation_Vector[X] * z) +
                    (Orientation_Vector[Y] * x) +
                    (Orientation_Vector[Z] * x);
        EPoint[Y] = (Orientation_Vector[X] * y) +
                    (Orientation_Vector[Y] * -z) +
                    (Orientation_Vector[Z] * y);
        EPoint[Z] = (Orientation_Vector[X] * -x) +
                    (Orientation_Vector[Y] * y) +
                    (Orientation_Vector[Z] * z);
    }

    return true;
}

bool TransformWarp::WarpPoint(Vector3d& TPoint) const
{
    MInvTransPoint(TPoint, TPoint, &Trans);
    return true;
}

bool GenericTurbulenceWarp::WarpPoint(Vector3d& TPoint) const
{
    Vector3d PTurbulence;
    DTurbulence (PTurbulence, TPoint, this);
    TPoint += PTurbulence * Turbulence;
    return true;
}



void Warp_Normal (Vector3d& TNorm, const Vector3d& ENorm, const TPATTERN *TPat, bool DontScaleBumps)
{
    const WarpList& warps = TPat->pattern->warps;

    if(!DontScaleBumps)
        TNorm = ENorm.normalized();
    else
        TNorm = ENorm;

    for (WarpList::const_reverse_iterator iWarp = warps.rbegin(); iWarp != warps.rend(); iWarp ++)
    {
        const WarpPtr& warp = *iWarp;
        warp->WarpNormal(TNorm);
    }

    if(!DontScaleBumps)
        TNorm.normalize();
}

bool IdentityWarp::WarpNormal(Vector3d& TNorm) const
{
    return true;
}

bool TransformWarp::WarpNormal(Vector3d& TNorm) const
{
    MInvTransNormal(TNorm, TNorm, &Trans);
    return true;
}

bool BlackHoleWarp::WarpNormal(Vector3d& TNorm) const           { /* Error("Black Hole Warp not yet implemented for normals"); */   return false; }
bool CubicWarp::WarpNormal(Vector3d& TNorm) const               { /* Error("Cubic Warp not yet implemented for normals"); */        return false; }
bool CylindricalWarp::WarpNormal(Vector3d& TNorm) const         { /* Error("Cylindrical Warp not yet implemented for normals"); */  return false; }
bool PlanarWarp::WarpNormal(Vector3d& TNorm) const              { /* Error("Planar Warp not yet implemented for normals"); */       return false; }
bool RepeatWarp::WarpNormal(Vector3d& TNorm) const              { /* Error("Repeat Warp not yet implemented for normals"); */       return false; }
bool SphericalWarp::WarpNormal(Vector3d& TNorm) const           { /* Error("Spherical Warp not yet implemented for normals"); */    return false; }
bool ToroidalWarp::WarpNormal(Vector3d& TNorm) const            { /* Error("Toroidal Warp not yet implemented for normals"); */     return false; }
bool GenericTurbulenceWarp::WarpNormal(Vector3d& TNorm) const   { /* Error("Turbulence Warp not yet implemented for normals"); */   return false; }


void UnWarp_Normal (Vector3d& TNorm, const Vector3d& ENorm, const TPATTERN *TPat, bool DontScaleBumps)
{
    const WarpList& warps = TPat->pattern->warps;

    if(!DontScaleBumps)
        TNorm = ENorm.normalized();
    else
        TNorm = ENorm;

    for (WarpList::const_iterator iWarp = warps.begin(); iWarp != warps.end(); iWarp ++)
    {
        const WarpPtr& warp = *iWarp;
        warp->UnwarpNormal(TNorm);
    }

    if(!DontScaleBumps)
        TNorm.normalize();
}

bool IdentityWarp::UnwarpNormal(Vector3d& TNorm) const
{
    return true;
}

bool TransformWarp::UnwarpNormal(Vector3d& TNorm) const
{
    MTransNormal(TNorm, TNorm, &Trans);
    return true;
}

bool BlackHoleWarp::UnwarpNormal(Vector3d& TNorm) const         { /* Error("Black Hole Warp not yet implemented for normals"); */   return false; }
bool CubicWarp::UnwarpNormal(Vector3d& TNorm) const             { /* Error("Cubic Warp not yet implemented for normals"); */        return false; }
bool CylindricalWarp::UnwarpNormal(Vector3d& TNorm) const       { /* Error("Cylindrical Warp not yet implemented for normals"); */  return false; }
bool PlanarWarp::UnwarpNormal(Vector3d& TNorm) const            { /* Error("Planar Warp not yet implemented for normals"); */       return false; }
bool RepeatWarp::UnwarpNormal(Vector3d& TNorm) const            { /* Error("Repeat Warp not yet implemented for normals"); */       return false; }
bool SphericalWarp::UnwarpNormal(Vector3d& TNorm) const         { /* Error("Spherical Warp not yet implemented for normals"); */    return false; }
bool ToroidalWarp::UnwarpNormal(Vector3d& TNorm) const          { /* Error("Toroidal Warp not yet implemented for normals"); */     return false; }
bool GenericTurbulenceWarp::UnwarpNormal(Vector3d& TNorm) const { /* Error("Turbulence Warp not yet implemented for normals"); */   return false; }


/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Destroy_Warps (WarpList& warps)
{
    for (WarpList::iterator iWarp = warps.begin(); iWarp != warps.end(); iWarp ++)
        delete *iWarp;
    warps.clear();
}



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Copy_Warps (WarpList& rNew, const WarpList& old)
{
    rNew.reserve(old.size());
    for (WarpList::const_iterator i = old.begin(); i != old.end(); i ++)
        rNew.push_back((*i)->Clone());
}

}
// end of namespace pov
