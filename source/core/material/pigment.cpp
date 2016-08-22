//******************************************************************************
///
/// @file core/material/pigment.cpp
///
/// Implementations related to pigments.
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
#include "core/material/pigment.h"

#include "base/pov_err.h"

#include "core/material/blendmap.h"
#include "core/material/pattern.h"
#include "core/material/warp.h"
#include "core/scene/scenedata.h"
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
* Local variables
******************************************************************************/



/*****************************************************************************
* Static functions
******************************************************************************/
static void Do_Average_Pigments (TransColour& colour, const PIGMENT *Pigment, const Vector3d& EPoint, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread);



/*****************************************************************************
*
* FUNCTION
*
*   Create_Pigment
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   pointer to the created pigment
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION   : Allocate memory for new pigment and initialize it to
*                 system default values.
*
* CHANGES
*
******************************************************************************/

PIGMENT *Create_Pigment ()
{
    PIGMENT *New;

    New = new PIGMENT;

    Init_TPat_Fields(New);

    New->colour.Clear();
    New->Quick_Colour.Invalidate();

    return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Pigment
*
* INPUT
*
*   Old -- point to pigment to be copied
*
* RETURNS
*
*   pointer to the created pigment
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION   : Allocate memory for new pigment and initialize it to
*                 values in existing pigment Old.
*
* CHANGES
*
******************************************************************************/

PIGMENT *Copy_Pigment (PIGMENT *Old)
{
    PIGMENT *New;

    if (Old != NULL)
    {
        New = Create_Pigment ();

        Copy_TPat_Fields (New, Old);
        New->Blend_Map = shared_ptr<GenericPigmentBlendMap> (Old->Blend_Map);

        if (Old->Type == PLAIN_PATTERN)
            New->colour = Old->colour;
        New->Quick_Colour = Old->Quick_Colour;
    }
    else
    {
        New = NULL;
    }

    return (New);
}

void Copy_Pigments (vector<PIGMENT*>& New, const vector<PIGMENT*>& Old)
{
    New.resize(0);
    New.reserve(Old.size());
    for (vector<PIGMENT*>::const_iterator i = Old.begin(); i != Old.end(); ++ i)
        New.push_back(Copy_Pigment(*i));
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Pigment
*
* INPUT
*
*   pointer to pigment to destroied
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION   : free all memory associated with given pigment
*
* CHANGES
*
******************************************************************************/

void Destroy_Pigment (PIGMENT *Pigment)
{
    if (Pigment != NULL)
        delete Pigment;
}



/*****************************************************************************
*
* FUNCTION
*
*   Post_Pigment
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Chris Young
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

void Post_Pigment(PIGMENT *Pigment, bool* pHasFilter)
{
    bool hasFilter;

    if (Pigment == NULL)
    {
        throw POV_EXCEPTION_STRING("Missing pigment");
    }

    if (Pigment->Flags & POST_DONE)
    {
        if ((pHasFilter != NULL) && (Pigment->Flags & HAS_FILTER))
            *pHasFilter = true;
        return;
    }

    if (Pigment->Type == NO_PATTERN)
    {
        Pigment->Type = PLAIN_PATTERN;

        Pigment->colour.Clear();

;// TODO MESSAGE    Warning(150, "No pigment type given.");
    }

    Pigment->Flags |= POST_DONE;

    switch (Pigment->Type)
    {
        case NO_PATTERN:

            POV_PATTERN_ASSERT(false); // should have been forced to PLAIN_PATTERN by now
            break;

        case PLAIN_PATTERN:
        case BITMAP_PATTERN:

            break;

        default:

            if (Pigment->Blend_Map == NULL)
            {
                switch (Pigment->Type)
                {
                    // NB: The const default blend maps are marked so that they will not be modified nor destroyed later.
                    case AVERAGE_PATTERN:
                        // TODO MESSAGE Error("Missing pigment_map in average pigment");
                        break;

                    default:
                        Pigment->Blend_Map = static_pointer_cast<GenericPigmentBlendMap, ColourBlendMap>(
                                                 const_pointer_cast<ColourBlendMap, const ColourBlendMap>(
                                                     Pigment->pattern->GetDefaultBlendMap()));
                        break;
                }
            }

            break;
    }

    /* Now we test wether this pigment is opaque or not. [DB 8/94] */

    hasFilter = false;

    if (!Pigment->colour.TransmittedColour().IsNearZero(EPSILON))
    {
        hasFilter = true;
    }

    if ((Pigment->Type == BITMAP_PATTERN) &&
        (dynamic_cast<ImagePattern*>(Pigment->pattern.get())->pImage != NULL))
    {
        // bitmaps are transparent if they are used only once, or the image is not opaque
        if ((dynamic_cast<ImagePattern*>(Pigment->pattern.get())->pImage->Once_Flag) || !is_image_opaque(dynamic_cast<ImagePattern*>(Pigment->pattern.get())->pImage))
            hasFilter = true;
    }

    GenericPigmentBlendMap* Map = Pigment->Blend_Map.get();

    if (Map != NULL)
    {
        Map->Post(hasFilter);
    }

    if (hasFilter)
    {
        Pigment->Flags |= HAS_FILTER;
        if (pHasFilter != NULL)
            *pHasFilter = true;
    }
}

void ColourBlendMap::Post(bool& rHasFilter)
{
    for(Vector::const_iterator i = Blend_Map_Entries.begin(); i != Blend_Map_Entries.end(); i++)
    {
        if (!i->Vals.TransmittedColour().IsNearZero(EPSILON))
        {
            rHasFilter = true;
            break;
        }
    }
}

void PigmentBlendMap::Post(bool& rHasFilter)
{
    for(Vector::const_iterator i = Blend_Map_Entries.begin(); i != Blend_Map_Entries.end(); i++)
    {
        Post_Pigment(i->Vals, &rHasFilter);
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Pigment
*
* INPUT
*
*   Pigment - Info about this pigment
*   EPoint  - 3-D point at which pattern is evaluated
*   Intersection - structure holding info about object at intersection point
*
* OUTPUT
*
*   colour  - Resulting color is returned here.
*
* RETURNS
*
*   int - true,  if a color was found for the given point
*         false, if no color was found (e.g. areas outside an image map
*                that has the once option)
*
* AUTHOR
*
*   POV-Ray Team
*
* DESCRIPTION
*   Given a 3d point and a pigment, compute colour from that layer.
*   (Formerly called "Colour_At", or "Add_Pigment")
*
* CHANGES
*   Added pigment map support [CY 11/94]
*   Added Intersection parameter for UV support NK 1998
*
******************************************************************************/

bool Compute_Pigment (TransColour& colour, const PIGMENT *Pigment, const Vector3d& EPoint, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread)
{
    bool Colour_Found;
    Vector3d TPoint;
    DBL value;

    if (Thread->qualityFlags.quickColour && Pigment->Quick_Colour.IsValid())
    {
        colour = Pigment->Quick_Colour;
        return (true);
    }

    if (Pigment->Type <= LAST_SPECIAL_PATTERN)
    {
        Colour_Found = true;

        switch (Pigment->Type)
        {
            case NO_PATTERN:

                POV_PATTERN_ASSERT(false); // should have been forced to PLAIN_PATTERN in Post_Pigment
                colour.Clear();

                break;

            case PLAIN_PATTERN:

                colour = Pigment->colour;

                break;

            case AVERAGE_PATTERN:

                Warp_EPoint (TPoint, EPoint, Pigment);

                Do_Average_Pigments(colour, Pigment, TPoint, Intersect, ray, Thread);

                break;

            case UV_MAP_PATTERN:
                if(Intersect == NULL)
                    throw POV_EXCEPTION_STRING("The 'uv_mapping' pattern cannot be used as part of a pigment function!");

                Colour_Found = Pigment->Blend_Map->ComputeUVMapped(colour, Intersect, ray, Thread);
                break;

            case BITMAP_PATTERN:

                Warp_EPoint (TPoint, EPoint, Pigment);

                colour.Clear();

                Colour_Found = image_map (TPoint, Pigment, colour);

                break;

            default:

                throw POV_EXCEPTION_STRING("Pigment type not yet implemented.");
        }

        return(Colour_Found);
    }

    /* NK 19 Nov 1999 added Warp_EPoint */
    Warp_EPoint (TPoint, EPoint, Pigment);
    value = Evaluate_TPat (Pigment, TPoint, Intersect, ray, Thread);

    return Pigment->Blend_Map->Compute (colour, value, TPoint, Intersect, ray, Thread);
}


void GenericPigmentBlendMap::Blend(TransColour& result, const TransColour& colour1, DBL weight1, const TransColour& colour2, DBL weight2, TraceThreadData *thread)
{
    switch (blendMode)
    {
        case 1:
            // linear blending
            if (GammaCurve::IsNeutral(thread->GetSceneData()->workingGamma))
                result = colour1 * weight1 + colour2 * weight2;
            else
            {
                result = GammaCurve::Encode(thread->GetSceneData()->workingGamma,
                                            GammaCurve::Decode(thread->GetSceneData()->workingGamma, colour1) * weight1
                                          + GammaCurve::Decode(thread->GetSceneData()->workingGamma, colour2) * weight2);
            }
            break;

        case 2:
            // gamma blending (except for filter and transm, which are blended linearly)
            if (GammaCurve::IsNeutral(blendGamma))
                result = colour1 * weight1 + colour2 * weight2;
            else
            {
                result.trans()  = GammaCurve::Encode(thread->GetSceneData()->workingGamma,
                                                     GammaCurve::Decode(thread->GetSceneData()->workingGamma, colour1.trans()) * weight1
                                                   + GammaCurve::Decode(thread->GetSceneData()->workingGamma, colour2.trans()) * weight2);
                result.colour() = GammaCurve::Decode(blendGamma,
                                                     GammaCurve::Encode(blendGamma, colour1.colour()) * weight1
                                                   + GammaCurve::Encode(blendGamma, colour2.colour()) * weight2);
            }
            break;

        case 3:
            // linear blending of colours combined with gamma blending of brightness
            if (GammaCurve::IsNeutral(blendGamma) && GammaCurve::IsNeutral(thread->GetSceneData()->workingGamma))
                result = colour1 * weight1 + colour2 * weight2;
            else
            {
                ColourChannel targetBrightness = GammaCurve::Decode(blendGamma,
                                                                    GammaCurve::Encode(blendGamma, colour1.colour().Greyscale()) * weight1
                                                                  + GammaCurve::Encode(blendGamma, colour2.colour().Greyscale()) * weight2);
                result = GammaCurve::Encode(thread->GetSceneData()->workingGamma,
                                            GammaCurve::Decode(thread->GetSceneData()->workingGamma, colour1) * weight1
                                          + GammaCurve::Decode(thread->GetSceneData()->workingGamma, colour2) * weight2);
                ColourChannel actualBrightness = result.colour().Greyscale();
                if (fabs(actualBrightness) >= EPSILON)
                    result.colour() *= targetBrightness / actualBrightness;
            }
            break;

        default:
            // blend in working gamma space
            result = colour1 * weight1 + colour2 * weight2;
            break;
    }
}


bool ColourBlendMap::Compute(TransColour& colour, DBL value, const Vector3d& TPoint, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread)
{
    const ColourBlendMapEntry* Prev;
    const ColourBlendMapEntry* Cur;
    double prevWeight;
    double curWeight;
    Search (value, Prev, Cur, prevWeight, curWeight);
    if (Prev == Cur)
    {
        colour = Cur->Vals;
    }
    else
    {
        Blend(colour, Prev->Vals, prevWeight, Cur->Vals, curWeight, Thread);
    }
    return true;
}

bool PigmentBlendMap::Compute(TransColour& colour, DBL value, const Vector3d& TPoint, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread)
{
    const PigmentBlendMapEntry* Prev;
    const PigmentBlendMapEntry* Cur;
    double prevWeight;
    double curWeight;
    bool found = false;
    Search (value, Prev, Cur, prevWeight, curWeight);
    if (Compute_Pigment(colour, Cur->Vals, TPoint, Intersect, ray, Thread))
        found = true;
    if (Prev != Cur)
    {
        TransColour Temp_Colour;
        if (Compute_Pigment(Temp_Colour, Prev->Vals, TPoint, Intersect, ray, Thread))
            found = true;
        Blend(colour, Temp_Colour, prevWeight, colour, curWeight, Thread);
    }
    return found;
}

void ColourBlendMap::ComputeAverage(TransColour& colour, const Vector3d& EPoint, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread)
{
    SNGL Total = 0.0;

    colour.Clear();
    for(vector<ColourBlendMapEntry>::const_iterator i = Blend_Map_Entries.begin(); i != Blend_Map_Entries.end(); i++)
    {
        colour += i->value * i->Vals;
        Total  += i->value;
    }
    colour /= Total;
}


void PigmentBlendMap::ComputeAverage(TransColour& colour, const Vector3d& EPoint, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread)
{
    SNGL Total = 0.0;
    TransColour tempColour;

    colour.Clear();
    for(vector<PigmentBlendMapEntry>::const_iterator i = Blend_Map_Entries.begin(); i != Blend_Map_Entries.end(); i++)
    {
        Compute_Pigment (tempColour, i->Vals, EPoint, Intersect, ray, Thread);

        colour += i->value * tempColour;
        Total  += i->value;
    }
    colour /= Total;
}

bool ColourBlendMap::ComputeUVMapped(TransColour& colour, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread)
{
    colour = Blend_Map_Entries[0].Vals;
    return true;
}

bool PigmentBlendMap::ComputeUVMapped(TransColour& colour, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread)
{
    Vector2d UV_Coords;
    Vector3d TPoint;
    const PigmentBlendMapEntry* Cur = &(Blend_Map_Entries[0]);

    /* Don't bother warping, simply get the UV vect of the intersection */
    Intersect->Object->UVCoord(UV_Coords, Intersect, Thread);
    TPoint[X] = UV_Coords[U];
    TPoint[Y] = UV_Coords[V];
    TPoint[Z] = 0;

    return Compute_Pigment(colour, Cur->Vals, TPoint, Intersect, ray, Thread);
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
*   Added Intersection parameter for UV support NK 1998
*
******************************************************************************/

static void Do_Average_Pigments (TransColour& colour, const PIGMENT *Pigment, const Vector3d& EPoint, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread)
{
    Pigment->Blend_Map->ComputeAverage(colour, EPoint, Intersect, ray, Thread);
}

void Evaluate_Density_Pigment(vector<PIGMENT*>& Density, const Vector3d& p, AttenuatingColour& c, TraceThreadData *ttd)
{
    TransColour lc;

    c = AttenuatingColour(1.0);

    // TODO - Reverse iterator may be less performant than forward iterator; we might want to
    //        compare performance with using forward iterators and decrement, or using random access.
    //        Alternatively, reversing the vector after parsing might be another option.
    for (vector<PIGMENT*>::reverse_iterator i = Density.rbegin(); i != Density.rend(); ++ i)
    {
        lc.Clear();

        Compute_Pigment(lc, *i, p, NULL, NULL, ttd);

        c *= lc.colour();
    }
}

//******************************************************************************

ColourBlendMap::ColourBlendMap() : BlendMap<ColourBlendMapData>(kBlendMapType_Colour) {}

ColourBlendMap::ColourBlendMap(int n, const ColourBlendMap::Entry aEntries[]) : BlendMap<ColourBlendMapData>(kBlendMapType_Colour)
{
    Blend_Map_Entries.reserve(n);
    for (int i = 0; i < n; i ++)
        Blend_Map_Entries.push_back(aEntries[i]);
}


PigmentBlendMap::PigmentBlendMap(BlendMapTypeId type) : BlendMap<PigmentBlendMapData>(type) {}

PigmentBlendMap::~PigmentBlendMap()
{
    for (Vector::iterator i = Blend_Map_Entries.begin(); i != Blend_Map_Entries.end(); i++)
        Destroy_Pigment(i->Vals);
}

}

