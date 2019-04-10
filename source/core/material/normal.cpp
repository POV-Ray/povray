//******************************************************************************
///
/// @file core/material/normal.cpp
///
/// Implementations related to surface normal perturbation.
///
/// The code in this file implements solid texturing functions that perturb the
/// surface normal to create a faux bumpy effect.
///
/// @copyright
/// @parblock
///
/// Some texture ideas garnered from SIGGRAPH '85 Volume 19 Number 3, "An Image
/// Synthesizer" By Ken Perlin. Further Ideas Garnered from "The RenderMan
/// Companion" (Addison Wesley).
///
/// ----------------------------------------------------------------------------
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
#include "core/material/normal.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/pov_err.h"
#include "base/povassert.h"

// POV-Ray header files (core module)
#include "core/material/blendmap.h"
#include "core/material/noise.h"
#include "core/material/pattern.h"
#include "core/material/pigment.h"
#include "core/material/warp.h"
#include "core/scene/object.h"
#include "core/scene/tracethreaddata.h"
#include "core/support/imageutil.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local constants
******************************************************************************/

static const Vector3d Pyramid_Vect [4]= { Vector3d( 0.942809041,-0.333333333, 0.0),
                                          Vector3d(-0.471404521,-0.333333333, 0.816496581),
                                          Vector3d(-0.471404521,-0.333333333,-0.816496581),
                                          Vector3d( 0.0        , 1.0        , 0.0) };

/*****************************************************************************
* Static functions
******************************************************************************/

static void ripples (const Vector3d& EPoint, const TNORMAL *Tnormal, Vector3d& Vector, const TraceThreadData *Thread);
static void waves (const Vector3d& EPoint, const TNORMAL *Tnormal, Vector3d& Vector, const TraceThreadData *Thread);
static void bumps (const Vector3d& EPoint, const TNORMAL *Tnormal, Vector3d& normal);
static void dents (const Vector3d& EPoint, const TNORMAL *Tnormal, Vector3d& normal, const TraceThreadData *Thread);
static void wrinkles (const Vector3d& EPoint, const TNORMAL *Tnormal, Vector3d& normal);
static void quilted (const Vector3d& EPoint, const TNORMAL *Tnormal, Vector3d& normal);
static DBL Hermite_Cubic (DBL T1, const Vector2d& UV1, const Vector2d& UV2);
static DBL Do_Slope_Map (DBL value, const SlopeBlendMap *Blend_Map);
static void Do_Average_Normals (const Vector3d& EPoint, const TNORMAL *Tnormal, Vector3d& normal, Intersection *Inter, const Ray *ray, TraceThreadData *Thread);
static void facets (const Vector3d& EPoint, const TNORMAL *Tnormal, Vector3d& normal, TraceThreadData *Thread);

/*****************************************************************************
*
* FUNCTION
*
*   ripples
*
* INPUT
*
* OUTPUT
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

static void ripples (const Vector3d& EPoint, const TNORMAL *Tnormal, Vector3d& normal, const TraceThreadData *Thread)
{
    unsigned int i;
    DBL length, scalar, index;
    Vector3d point;

    RipplesPattern* pPat = dynamic_cast<RipplesPattern*>(Tnormal->pattern.get());
    if (pPat == nullptr)
        throw POV_EXCEPTION_STRING("Invalid pattern type.");

    for (i = 0; i < Thread->numberOfWaves; i++)
    {
        point = EPoint - Thread->waveSources[i];
        length = point.length();

        if (length == 0.0)
            length = 1.0;

        index = length * pPat->waveFrequency + pPat->wavePhase;

        scalar = cycloidal(index) * Tnormal ->Amount;

        normal += (scalar / (length * (DBL)Thread->numberOfWaves)) * point;
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   waves
*
* INPUT
*
* OUTPUT
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

static void waves (const Vector3d& EPoint, const TNORMAL *Tnormal, Vector3d& normal, const TraceThreadData *Thread)
{
    unsigned int i;
    DBL length, scalar, index, sinValue;
    Vector3d point;

    WavesPattern* pPat = dynamic_cast<WavesPattern*>(Tnormal->pattern.get());
    if (pPat == nullptr)
        throw POV_EXCEPTION_STRING("Invalid pattern type.");

    for (i = 0; i < Thread->numberOfWaves; i++)
    {
        point = EPoint - Thread->waveSources[i];

        length = point.length();

        if (length == 0.0)
        {
            length = 1.0;
        }

        index = length * pPat->waveFrequency * Thread->waveFrequencies[i] + pPat->wavePhase;

        sinValue = cycloidal(index);

        scalar = sinValue * Tnormal->Amount / Thread->waveFrequencies[i];

        normal += (scalar / (length * (DBL)Thread->numberOfWaves)) * point;
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   bumps
*
* INPUT
*
* OUTPUT
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

static void bumps (const Vector3d& EPoint, const TNORMAL *Tnormal, Vector3d& normal)
{
    Vector3d bump_turb;

    /* Get normal displacement value. */

    DNoise (bump_turb, EPoint);

    /* Displace "normal". */

    normal += (DBL)Tnormal->Amount * bump_turb;
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
*   POV-Ray Team
*
* DESCRIPTION
*   Dents is similar to bumps, but uses noise() to control the amount of
*   dnoise() perturbation of the object normal...
*
* CHANGES
*
******************************************************************************/

static void dents (const Vector3d& EPoint, const TNORMAL *Tnormal, Vector3d& normal, const TraceThreadData *Thread)
{
    DBL noise;
    Vector3d stucco_turb;

    noise = Noise (EPoint, GetNoiseGen(Tnormal, Thread));

    noise = noise * noise * noise * Tnormal->Amount;

    /* Get normal displacement value. */

    DNoise(stucco_turb, EPoint);

    /* Displace "normal". */

    normal += noise * stucco_turb;
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
*   POV-Ray Team
*
* DESCRIPTION
*
*   Wrinkles - This is my implementation of the dented() routine, using
*   a surface iterative fractal derived from DTurbulence.
*
*   This is a 3-D version (thanks to DNoise()...) of the usual version
*   using the singular Noise()...
*
*   Seems to look a lot like wrinkles, however... (hmmm)
*
*   Idea garnered from the April 89 Byte Graphics Supplement on RenderMan,
*   refined from "The RenderMan Companion, by Steve Upstill of Pixar,
*   (C) 1990 Addison-Wesley.
*
* CHANGES
*
******************************************************************************/

static void wrinkles (const Vector3d& EPoint, const TNORMAL *Tnormal, Vector3d& normal)
{
    int i;
    DBL scale = 1.0;
    Vector3d result, value, value2;

    result = Vector3d(0.0, 0.0, 0.0);

    for (i = 0; i < 10; scale *= 2.0, i++)
    {
        value2 = EPoint * scale;
        DNoise(value, value2);

        result[X] += fabs(value[X] / scale);
        result[Y] += fabs(value[Y] / scale);
        result[Z] += fabs(value[Z] / scale);
    }

    /* Displace "normal". */

    normal += (DBL)Tnormal->Amount * result;
}


/*****************************************************************************
*
* FUNCTION
*
*   quilted
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dan Farmer '94
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

static void quilted (const Vector3d& EPoint, const TNORMAL *Tnormal, Vector3d& normal)
{
    Vector3d value;
    DBL t;

    value[X] = EPoint[X]-FLOOR(EPoint[X])-0.5;
    value[Y] = EPoint[Y]-FLOOR(EPoint[Y])-0.5;
    value[Z] = EPoint[Z]-FLOOR(EPoint[Z])-0.5;

    t = value.length();

    const QuiltedPattern *pattern = dynamic_cast<QuiltedPattern*>(Tnormal->pattern.get());
    POV_PATTERN_ASSERT(pattern);

    t = quilt_cubic(t, pattern->Control0, pattern->Control1);

    value *= t;

    normal += (DBL)Tnormal->Amount * value;
}

/*****************************************************************************
*
* FUNCTION
*
*   facets
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Ronald Parker '98
*
* DESCRIPTION
*
*   This pattern is based on the "Crackle" pattern and creates a faceted
*   look on a curved surface.
*
* CHANGES
*
******************************************************************************/

static void facets (const Vector3d& EPoint, const TNORMAL *Tnormal, Vector3d& normal, TraceThreadData *Thread)
{
    int      i;
    int      thisseed;
    DBL      sum, minsum;
    Vector3d sv, tv, dv, t1, add, newnormal, pert;
    DBL      scale;
    int      UseSquare;
    int      UseUnity;
    DBL      Metric;

    const FacetsPattern *pattern = dynamic_cast<FacetsPattern*>(Tnormal->pattern.get());
    POV_PATTERN_ASSERT(pattern);

    Vector3d *cv = Thread->Facets_Cube;
    Metric = pattern->facetsMetric;

    UseSquare = (Metric == 2 );
    UseUnity  = (Metric == 1 );

    normal.normalize();

    if (pattern->facetsCoords)
    {
        tv = EPoint;
    }
    else
    {
        tv = normal;
    }

    if (pattern->facetsSize < 1e-6)
    {
        scale = 1e6;
    }
    else
    {
        scale = 1. / pattern->facetsSize;
    }

    tv *= scale;

    /*
     * Check to see if the input point is in the same unit cube as the last
     * call to this function, to use cache of cubelets for speed.
     */

    thisseed = PickInCube(tv, t1);

    if (thisseed != Thread->Facets_Last_Seed)
    {
        /*
         * No, not same unit cube.  Calculate the random points for this new
         * cube and its 80 neighbours which differ in any axis by 1 or 2.
         * Why distance of 2?  If there is 1 point in each cube, located
         * randomly, it is possible for the closest random point to be in the
         * cube 2 over, or the one two over and one up.  It is NOT possible
         * for it to be two over and two up.  Picture a 3x3x3 cube with 9 more
         * cubes glued onto each face.
         */

        /* Now store a points for this cube and each of the 80 neighbour cubes. */

        int cvc = 0;

        for (add[X] = -2.0; add[X] < 2.5; add[X] +=1.0)
        {
            for (add[Y] = -2.0; add[Y] < 2.5; add[Y] += 1.0)
            {
                for (add[Z] = -2.0; add[Z] < 2.5; add[Z] += 1.0)
                {
                    /* For each cubelet in a 5x5 cube. */

                    if ((fabs(add[X])>1.5)+(fabs(add[Y])>1.5)+(fabs(add[Z])>1.5) <= 1.0)
                    {
                        /* Yes, it's within a 3d knight move away. */

                        sv = tv + add;

                        PickInCube(sv, t1);

                        cv[cvc] = t1;
                        cvc++;
                    }
                }
            }
        }

        Thread->Facets_Last_Seed = thisseed;
        Thread->Facets_CVC = cvc;
    }

    /*
     * Find the point with the shortest distance from the input point.
     */

    dv = cv[0] - tv;
    if ( UseSquare )
    {
        minsum  = dv.lengthSqr();
    }
    else if ( UseUnity )
    {
        minsum = dv[X]+dv[Y]+dv[Z];
    }
    else
    {
        minsum = pow(fabs(dv[X]), Metric)+
                 pow(fabs(dv[Y]), Metric)+
                 pow(fabs(dv[Z]), Metric);
    }

    newnormal = cv[0];

    /* Loop for the 81 cubelets to find closest. */

    for (i = 1; i < Thread->Facets_CVC; i++)
    {
        dv = cv[i] - tv;

        if ( UseSquare )
        {
            sum  = dv.lengthSqr();
        }
        else
        {
            if ( UseUnity )
            {
                sum = dv[X]+dv[Y]+dv[Z];
            }
            else
            {
                sum = pow(fabs(dv[X]), Metric)+
                      pow(fabs(dv[Y]), Metric)+
                      pow(fabs(dv[Z]), Metric);
            }
        }

        if (sum < minsum)
        {
            minsum = sum;
            newnormal = cv[i];
        }
    }

    if (pattern->facetsCoords)
    {
        DNoise( pert, newnormal );
        sum = dot(pert, normal);
        newnormal = normal * sum;
        pert -= newnormal;
        normal += pattern->facetsCoords * pert;
    }
    else
    {
        normal = newnormal;
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   Create_Tnormal
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   pointer to the created Tnormal
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION   : Allocate memory for new Tnormal and initialize it to
*                 system default values.
*
* CHANGES
*
******************************************************************************/


TNORMAL *Create_Tnormal ()
{
    TNORMAL *New;

    New = new TNORMAL;

    Init_TPat_Fields(New);

    New->Amount = 0.5;

    /* NK delta */
    New->Delta = (float)0.02; /* this is a good starting point for delta */

    return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Tnormal
*
* INPUT
*
* OUTPUT
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

TNORMAL *Copy_Tnormal (TNORMAL *Old)
{
    TNORMAL *New;

    if (Old != nullptr)
    {
        New = Create_Tnormal();

        Copy_TPat_Fields (New, Old);
        New->Blend_Map = Copy_Blend_Map(Old->Blend_Map);

        New->Amount = Old->Amount;
        New->Delta = Old->Delta;
    }
    else
    {
        New = nullptr;
    }

    return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Tnormal
*
* INPUT
*
* OUTPUT
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

void Destroy_Tnormal(TNORMAL *Tnormal)
{
    if (Tnormal != nullptr)
        delete Tnormal;
}



/*****************************************************************************
*
* FUNCTION
*
*   Post_Tnormal
*
* INPUT
*
* OUTPUT
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

void Post_Tnormal (TNORMAL *Tnormal)
{
    GenericNormalBlendMapPtr Map;

    if (Tnormal != nullptr)
    {
        if (Tnormal->Flags & POST_DONE)
        {
            return;
        }

        if (Tnormal->Type == NO_PATTERN)
        {
            throw POV_EXCEPTION_STRING("No normal type given.");
        }

        Tnormal->Flags |= POST_DONE;

        if ((Map = Tnormal->Blend_Map) != nullptr)
        {
            Map->Post((Tnormal->Flags & DONT_SCALE_BUMPS_FLAG) != 0);
        }
    }
}

void SlopeBlendMap::Post(bool dontScaleBumps)
{
    POV_BLEND_MAP_ASSERT(Type == kBlendMapType_Slope);
}

void NormalBlendMap::Post(bool dontScaleBumps)
{
    POV_BLEND_MAP_ASSERT(Type == kBlendMapType_Normal);
    for(Vector::iterator i = Blend_Map_Entries.begin(); i != Blend_Map_Entries.end(); i++)
    {
        if (dontScaleBumps)
            i->Vals->Flags |= DONT_SCALE_BUMPS_FLAG;
        Post_Tnormal(i->Vals);
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Perturb_Normal
*
* INPUT
*
* OUTPUT
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
*    Added intersectin parameter for UV mapping - NK 1998
*
******************************************************************************/

void Perturb_Normal(Vector3d& Layer_Normal, const TNORMAL *Tnormal, const Vector3d& EPoint, Intersection *Intersection, const Ray *ray, TraceThreadData *Thread)
{
    Vector3d TPoint,P1;
    DBL value1,Amount;
    int i;
    std::shared_ptr<NormalBlendMap> Blend_Map;

    if (Tnormal == nullptr)
    {
        return;
    }

    /* If normal_map present, use it and return */

    Blend_Map = std::dynamic_pointer_cast<NormalBlendMap>(Tnormal->Blend_Map);
    if (Blend_Map != nullptr)
    {
        if (Tnormal->Type == UV_MAP_PATTERN)
        {
            Vector2d UV_Coords;

            /* Don't bother warping, simply get the UV vect of the intersection */
            Intersection->Object->UVCoord(UV_Coords, Intersection);
            TPoint[X] = UV_Coords[U];
            TPoint[Y] = UV_Coords[V];
            TPoint[Z] = 0;

            Perturb_Normal(Layer_Normal,Blend_Map->Blend_Map_Entries[0].Vals,TPoint,Intersection,ray,Thread);
            Layer_Normal.normalize();
            Intersection->PNormal = Layer_Normal; /* -hdf- June 98 */

            return;
        }
        else if (Tnormal->Type != AVERAGE_PATTERN)
        {
            const NormalBlendMapEntry *Prev, *Cur;
            DBL prevWeight, curWeight;

            /* NK 19 Nov 1999 added Warp_EPoint */
            Warp_EPoint (TPoint, EPoint, Tnormal);
            value1 = Evaluate_TPat(Tnormal, TPoint, Intersection, ray, Thread);

            Blend_Map->Search (value1,Prev,Cur,prevWeight,curWeight);

            Warp_Normal(Layer_Normal,Layer_Normal, Tnormal, Test_Flag(Tnormal,DONT_SCALE_BUMPS_FLAG));
            P1 = Layer_Normal;

            Warp_EPoint (TPoint, EPoint, Tnormal);

            Perturb_Normal(Layer_Normal,Cur->Vals,TPoint,Intersection,ray,Thread);

            if (Prev != Cur)
            {
                Perturb_Normal(P1,Prev->Vals,TPoint,Intersection,ray,Thread);

                Layer_Normal = prevWeight * P1 + curWeight * Layer_Normal;
            }

            UnWarp_Normal(Layer_Normal,Layer_Normal, Tnormal, Test_Flag(Tnormal,DONT_SCALE_BUMPS_FLAG));

            Layer_Normal.normalize();

            Intersection->PNormal = Layer_Normal; /* -hdf- June 98 */

            return;
        }
        // TODO - what if Tnormal->Type == AVERAGE_PATTERN?
    }

    /* No normal_map. */

    if (Tnormal->Type <= LAST_SPECIAL_NORM_PATTERN)
    {
        Warp_Normal(Layer_Normal,Layer_Normal, Tnormal,
                    Test_Flag(Tnormal,DONT_SCALE_BUMPS_FLAG));

        Warp_EPoint (TPoint, EPoint, Tnormal);

        switch (Tnormal->Type)
        {
            case BITMAP_PATTERN:    bump_map    (TPoint, Tnormal, Layer_Normal);            break;
            case BUMPS_PATTERN:     bumps       (TPoint, Tnormal, Layer_Normal);            break;
            case DENTS_PATTERN:     dents       (TPoint, Tnormal, Layer_Normal, Thread);    break;
            case RIPPLES_PATTERN:   ripples     (TPoint, Tnormal, Layer_Normal, Thread);    break;
            case WAVES_PATTERN:     waves       (TPoint, Tnormal, Layer_Normal, Thread);    break;
            case WRINKLES_PATTERN:  wrinkles    (TPoint, Tnormal, Layer_Normal);            break;
            case QUILTED_PATTERN:   quilted     (TPoint, Tnormal, Layer_Normal);            break;
            case FACETS_PATTERN:    facets      (TPoint, Tnormal, Layer_Normal, Thread);    break;
            case AVERAGE_PATTERN:   Do_Average_Normals (TPoint, Tnormal, Layer_Normal, Intersection, ray, Thread); break;
            default:
                throw POV_EXCEPTION_STRING("Normal pattern not yet implemented.");
        }

        UnWarp_Normal(Layer_Normal,Layer_Normal, Tnormal,
                      Test_Flag(Tnormal,DONT_SCALE_BUMPS_FLAG));
    }
    else
    {
        std::shared_ptr<SlopeBlendMap> slopeMap = std::dynamic_pointer_cast<SlopeBlendMap>(Tnormal->Blend_Map);

        Warp_Normal(Layer_Normal,Layer_Normal, Tnormal,
                    Test_Flag(Tnormal,DONT_SCALE_BUMPS_FLAG));

        // TODO FIXME - two magic fudge factors
        Amount=Tnormal->Amount * -5.0; /*fudge factor*/
        Amount*=0.02/Tnormal->Delta; /* NK delta */

        /* warp the center point first - this is the last warp */
        Warp_EPoint(TPoint,EPoint,Tnormal);

        for(i=0; i<=3; i++)
        {
            P1 = TPoint + (DBL)Tnormal->Delta * Pyramid_Vect[i]; /* NK delta */
            value1 = Do_Slope_Map(Evaluate_TPat(Tnormal, P1, Intersection, ray, Thread), slopeMap.get());
            Layer_Normal += (value1*Amount) * Pyramid_Vect[i];
        }

        UnWarp_Normal(Layer_Normal,Layer_Normal,Tnormal,
                      Test_Flag(Tnormal,DONT_SCALE_BUMPS_FLAG));
    }

    if ( Intersection )
        Intersection->PNormal = Layer_Normal; /* -hdf- June 98 */
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

static DBL Do_Slope_Map (DBL value, const SlopeBlendMap *Blend_Map)
{
    DBL prevWeight, curWeight;
    const SlopeBlendMapEntry *Prev, *Cur;

    if (Blend_Map == nullptr)
    {
        return(value);
    }

    Blend_Map->Search (value, Prev, Cur, prevWeight, curWeight);

    if (Prev == Cur)
    {
        return(Cur->Vals[0]);
    }

    return(Hermite_Cubic(curWeight, Prev->Vals, Cur->Vals));
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

static DBL Hermite_Cubic(DBL T1, const Vector2d& UV1, const Vector2d& UV2)
{
    DBL TT=T1*T1;
    DBL TTT=TT*T1;
    DBL rv;        /* simplified equation for poor Symantec */

    rv  = TTT*(UV1[1]+UV2[1]+2.0*(UV1[0]-UV2[0]));
    rv += -TT*(2.0*UV1[1]+UV2[1]+3.0*(UV1[0]-UV2[0]));
    rv += T1*UV1[1] +UV1[0];

    return (rv);
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
*    Added intersection parameter for UV mapping - NK 1998
*
******************************************************************************/

static void Do_Average_Normals (const Vector3d& EPoint, const TNORMAL *Tnormal, Vector3d& normal, Intersection *Inter, const Ray *ray, TraceThreadData *Thread)
{
    Tnormal->Blend_Map->ComputeAverage(EPoint, normal, Inter, ray, Thread);
}


//******************************************************************************

SlopeBlendMap::SlopeBlendMap() : BlendMap<Vector2d>(kBlendMapType_Slope) {}

SlopeBlendMap::~SlopeBlendMap()
{
    POV_BLEND_MAP_ASSERT(Type == kBlendMapType_Slope);
}


NormalBlendMap::NormalBlendMap() : BlendMap<TNORMAL*>(kBlendMapType_Normal) {}

NormalBlendMap::~NormalBlendMap()
{
    POV_BLEND_MAP_ASSERT(Type == kBlendMapType_Normal);
    for (Vector::iterator i = Blend_Map_Entries.begin(); i != Blend_Map_Entries.end(); i++)
        Destroy_Tnormal(i->Vals);
}

void SlopeBlendMap::ComputeAverage (const Vector3d& EPoint, Vector3d& normal, Intersection *Inter, const Ray *ray, TraceThreadData *Thread)
{
    POV_BLEND_MAP_ASSERT(false);
}

void NormalBlendMap::ComputeAverage (const Vector3d& EPoint, Vector3d& normal, Intersection *Inter, const Ray *ray, TraceThreadData *Thread)
{
    SNGL Value;
    SNGL Total = 0.0;
    Vector3d V1,V2;

    POV_BLEND_MAP_ASSERT(Type == kBlendMapType_Normal);

    V1 = Vector3d(0.0, 0.0, 0.0);

    for(Vector::const_iterator i = Blend_Map_Entries.begin(); i != Blend_Map_Entries.end(); i++)
    {
        Value = i->value;

        V2 = normal;

        Perturb_Normal(V2,i->Vals,EPoint,Inter,ray,Thread);

        V1 += (DBL)Value * V2;

        Total += Value;
    }

    normal = V1 / Total;
}

}
// end of namespace pov
