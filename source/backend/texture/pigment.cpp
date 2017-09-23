/*******************************************************************************
 * pigment.cpp
 *
 * This module implements solid texturing functions that modify the color
 * transparency of an object's surface.
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
 * $File: //depot/public/povray/3.x/source/backend/texture/pigment.cpp $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

/*
   Some texture ideas garnered from SIGGRAPH '85 Volume 19 Number 3, 
   "An Image Synthesizer" By Ken Perlin.
   Further Ideas Garnered from "The RenderMan Companion" (Addison Wesley).
*/

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "backend/texture/pigment.h"
#include "backend/pattern/pattern.h"
#include "backend/pattern/warps.h"
#include "backend/support/imageutil.h"
#include "backend/scene/threaddata.h"
#include "base/pov_err.h"

#include "povrayold.h" // TODO FIXME

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

static BLEND_MAP_ENTRY Black_White_Entries[2] =
	{{0.0, false, {{0.0, 0.0, 0.0, 0.0, 0.0}}},
	{1.0, false, {{1.0, 1.0, 1.0, 0.0, 0.0}}}};

const BLEND_MAP Gray_Default_Map =
	{ -1,  2,  false, COLOUR_TYPE,  Black_White_Entries};

static BLEND_MAP_ENTRY Bozo_Entries[6] =
	{{0.4, false, {{1.0, 1.0, 1.0, 0.0, 0.0}}},
	 {0.4, false, {{0.0, 1.0, 0.0, 0.0, 0.0}}},
	 {0.6, false, {{0.0, 1.0, 0.0, 0.0, 0.0}}},
	 {0.6, false, {{0.0, 0.0, 1.0, 0.0, 0.0}}},
	 {0.8, false, {{0.0, 0.0, 1.0, 0.0, 0.0}}},
	 {0.8, false, {{1.0, 0.0, 0.0, 0.0, 0.0}}}};

const BLEND_MAP Bozo_Default_Map =
	{ -1,  6,  false, COLOUR_TYPE,  Bozo_Entries};

static BLEND_MAP_ENTRY Wood_Entries[2] =
	{{0.6, false, {{0.666, 0.312,  0.2,   0.0, 0.0}}},
	 {0.6, false, {{0.4,   0.1333, 0.066, 0.0, 0.0}}}};

const BLEND_MAP Wood_Default_Map =
	{ -1,  2,  false, COLOUR_TYPE,  Wood_Entries};

static BLEND_MAP_ENTRY Mandel_Entries[5] =
	{{0.001, false, {{0.0, 0.0, 0.0, 0.0, 0.0}}},
	 {0.001, false, {{0.0, 1.0, 1.0, 0.0, 0.0}}},
	 {0.012, false, {{1.0, 1.0, 0.0, 0.0, 0.0}}},
	 {0.015, false, {{1.0, 0.0, 1.0, 0.0, 0.0}}},
	 {0.1,   false, {{0.0, 1.0, 1.0, 0.0, 0.0}}}};

const BLEND_MAP Mandel_Default_Map =
	{ -1,  5,  false, COLOUR_TYPE,  Mandel_Entries};

static BLEND_MAP_ENTRY Agate_Entries[6] =
	{{0.0, false, {{1.0,  1.0,  1.0,  0.0, 0.0}}},
	 {0.5, false, {{0.95, 0.75, 0.5,  0.0, 0.0}}},
	 {0.5, false, {{0.9,  0.7,  0.5,  0.0, 0.0}}},
	 {0.6, false, {{0.9,  0.7,  0.4,  0.0, 0.0}}},
	 {0.6, false, {{1.0,  0.7,  0.4,  0.0, 0.0}}},
	 {1.0, false, {{0.6,  0.3,  0.0,  0.0, 0.0}}}};

const BLEND_MAP Agate_Default_Map =
	{ -1,  6,  false, COLOUR_TYPE,  Agate_Entries};

static BLEND_MAP_ENTRY Radial_Entries[4] =
	{{0.0,   false, {{0.0, 1.0, 1.0, 0.0, 0.0}}},
	 {0.333, false, {{1.0, 1.0, 0.0, 0.0, 0.0}}},
	 {0.666, false, {{1.0, 0.0, 1.0, 0.0, 0.0}}},
	 {1.0,   false, {{0.0, 1.0, 1.0, 0.0, 0.0}}}};

const BLEND_MAP Radial_Default_Map =
	{ -1,  4,  false, COLOUR_TYPE,  Radial_Entries};

static BLEND_MAP_ENTRY Marble_Entries[3] =
	{{0.0, false, {{0.9, 0.8,  0.8,  0.0, 0.0}}},
	 {0.9, false, {{0.9, 0.08, 0.08, 0.0, 0.0}}},
	 {0.9, false, {{0.0, 0.0, 0.0, 0.0, 0.0}}}};

const BLEND_MAP Marble_Default_Map =
	{ -1,  3,  false, COLOUR_TYPE,  Marble_Entries};

static BLEND_MAP_ENTRY Brick_Entries[2] =
	{{0.0, false, {{0.5, 0.5,  0.5,  0.0, 0.0}}},
	 {1.0, false, {{0.6, 0.15, 0.15, 0.0, 0.0}}}};

const BLEND_MAP Brick_Default_Map =
	{ -1,  2,  false, COLOUR_TYPE,  Brick_Entries};

static BLEND_MAP_ENTRY Hex_Entries[3] =
	{{0.0, false, {{0.0, 0.0, 1.0, 0.0, 0.0}}},
	 {1.0, false, {{0.0, 1.0, 0.0, 0.0, 0.0}}},
	 {2.0, false, {{1.0, 0.0, 0.0, 0.0, 0.0}}}};

const BLEND_MAP Hex_Default_Map =
	{ -1, 3, false,COLOUR_TYPE, Hex_Entries};

// JN2007: Cubic pattern
static BLEND_MAP_ENTRY Cubic_Entries[6] =
	{{0.0, false, {{1.0, 0.0, 0.0, 0.0, 0.0}}},
	 {1.0, false, {{0.0, 1.0, 0.0, 0.0, 0.0}}},
	 {1.0, false, {{0.0, 0.0, 1.0, 0.0, 0.0}}},
	 {1.0, false, {{1.0, 1.0, 0.0, 0.0, 0.0}}},
	 {1.0, false, {{0.0, 1.0, 1.0, 0.0, 0.0}}},
	 {2.0, false, {{1.0, 0.0, 1.0, 0.0, 0.0}}}};
const BLEND_MAP Cubic_Default_Map =
	{ -1, 6, false,COLOUR_TYPE, Cubic_Entries};

const BLEND_MAP Check_Default_Map =
	{ -1, 2, false,COLOUR_TYPE, Hex_Entries}; /* Yes... Hex_Entries, not Check [CY] */

const BLEND_MAP Triangular_Default_Map =
	{ -1, 6, false,COLOUR_TYPE, Cubic_Entries}; /* Yes... Cubic_Entries, not Triangular [JG] */

const BLEND_MAP Square_Default_Map =
	{ -1, 4, false,COLOUR_TYPE, Cubic_Entries}; /* Yes... Cubic_Entries, not Square [JG] */



/*****************************************************************************
* Static functions
******************************************************************************/
static void Do_Average_Pigments (Colour& colour, const PIGMENT *Pigment, const VECTOR EPoint, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread);



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

	New = reinterpret_cast<PIGMENT *>(POV_MALLOC(sizeof (PIGMENT), "pigment"));

	Init_TPat_Fields(reinterpret_cast<TPATTERN *>(New));

	New->colour.clear();
	New->Quick_Colour = Colour(-1.0,-1.0,-1.0);
	New->Blend_Map = NULL;

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

PIGMENT *Copy_Pigment (const PIGMENT *Old)
{
	PIGMENT *New;

	if (Old != NULL)
	{
		New = Create_Pigment ();

		Copy_TPat_Fields (reinterpret_cast<TPATTERN *>(New), reinterpret_cast<const TPATTERN *>(Old));

		if (Old->Type == PLAIN_PATTERN)
			New->colour = Old->colour;
		New->Quick_Colour = Old->Quick_Colour;
		New->Next = reinterpret_cast<TPATTERN *>(Copy_Pigment(reinterpret_cast<const PIGMENT *>(Old->Next)));
	}
	else
	{
		New = NULL;
	}

	return (New);
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
	{
		Destroy_Pigment(reinterpret_cast<PIGMENT *>(Pigment->Next));

		Destroy_TPat_Fields (reinterpret_cast<TPATTERN *>(Pigment));

		POV_FREE(Pigment);
	}
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

int Post_Pigment(PIGMENT *Pigment)
{
	int i, Has_Filter;
	BLEND_MAP *Map;

	if (Pigment == NULL)
	{
		throw POV_EXCEPTION_STRING("Missing pigment");
	}

	if (Pigment->Flags & POST_DONE)
	{
		return(Pigment->Flags & HAS_FILTER);
	}

	if (Pigment->Type == NO_PATTERN)
	{
		Pigment->Type = PLAIN_PATTERN;

		Pigment->colour.clear() ;

;// TODO MESSAGE    Warning(150, "No pigment type given.");
	}

	Pigment->Flags |= POST_DONE;

	switch (Pigment->Type)
	{
		case PLAIN_PATTERN:

			Destroy_Warps (Pigment->Warps);

			Pigment->Warps = NULL;

			break;

		case NO_PATTERN:
		case BITMAP_PATTERN:

			break;

		default:

			if (Pigment->Blend_Map == NULL)
			{
				switch (Pigment->Type)
				{
					// NB: The const default blend maps are marked so that they will not be modified nor destroyed later.
					case BOZO_PATTERN:    Pigment->Blend_Map = const_cast<BLEND_MAP *>(&Bozo_Default_Map);  break;
					case BRICK_PATTERN:   Pigment->Blend_Map = const_cast<BLEND_MAP *>(&Brick_Default_Map); break;
					case WOOD_PATTERN:    Pigment->Blend_Map = const_cast<BLEND_MAP *>(&Wood_Default_Map);  break;
					case MANDEL_PATTERN:  Pigment->Blend_Map = const_cast<BLEND_MAP *>(&Mandel_Default_Map);break;
					case RADIAL_PATTERN:  Pigment->Blend_Map = const_cast<BLEND_MAP *>(&Radial_Default_Map);break;
					case AGATE_PATTERN:   Pigment->Blend_Map = const_cast<BLEND_MAP *>(&Agate_Default_Map); break;
					case MARBLE_PATTERN:  Pigment->Blend_Map = const_cast<BLEND_MAP *>(&Marble_Default_Map);break;
					case HEXAGON_PATTERN: Pigment->Blend_Map = const_cast<BLEND_MAP *>(&Hex_Default_Map);   break;
					case SQUARE_PATTERN:  Pigment->Blend_Map = const_cast<BLEND_MAP *>(&Square_Default_Map);break;
					case TRIANGULAR_PATTERN: Pigment->Blend_Map = const_cast<BLEND_MAP *>(&Triangular_Default_Map);break;
					case CUBIC_PATTERN:   Pigment->Blend_Map = const_cast<BLEND_MAP *>(&Cubic_Default_Map); break; // JN2007: Cubic pattern
					case CHECKER_PATTERN: Pigment->Blend_Map = const_cast<BLEND_MAP *>(&Check_Default_Map); break;
					case AVERAGE_PATTERN: break;// TODO MESSAGE Error("Missing pigment_map in average pigment"); break;
					case OBJECT_PATTERN:  Pigment->Blend_Map = const_cast<BLEND_MAP *>(&Check_Default_Map); break;
					default:              Pigment->Blend_Map = const_cast<BLEND_MAP *>(&Gray_Default_Map);  break;
				}
			}

			break;
	}

	/* Now we test wether this pigment is opaque or not. [DB 8/94] */

	Has_Filter = false;

	if ((fabs(Pigment->colour[pFILTER]) > EPSILON) ||
	    (fabs(Pigment->colour[pTRANSM]) > EPSILON))
	{
		Has_Filter = true;
	}

	if ((Pigment->Type == BITMAP_PATTERN) &&
	    (Pigment->Vals.image != NULL))
	{
		// bitmaps are transparent if they are used only once, or the image is not opaque
		Has_Filter |= (Pigment->Vals.image->Once_Flag) || !is_image_opaque(Pigment->Vals.image);
	}

	if ((Map = Pigment->Blend_Map) != NULL)
	{
		if ((Map->Type == PIGMENT_TYPE) || (Map->Type == DENSITY_TYPE))
		{
			for (i = 0; i < Map->Number_Of_Entries; i++)
			{
				Has_Filter |= Post_Pigment(Map->Blend_Map_Entries[i].Vals.Pigment);
			}
		}
		else
		{
			for (i = 0; i < Map->Number_Of_Entries; i++)
			{
				Has_Filter |= fabs(Map->Blend_Map_Entries[i].Vals.colour[pFILTER])>EPSILON;
				Has_Filter |= fabs(Map->Blend_Map_Entries[i].Vals.colour[pTRANSM])>EPSILON;
			}
		}
	}

	if (Has_Filter)
	{
		Pigment->Flags |= HAS_FILTER;
	}

	if (Pigment->Next != NULL)
	{
		Post_Pigment(reinterpret_cast<PIGMENT *>(Pigment->Next));
	}

	return(Has_Filter);
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

bool Compute_Pigment (Colour& colour, const PIGMENT *Pigment, const VECTOR EPoint, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread)
{
	int Colour_Found;
	VECTOR TPoint;
	DBL value;
	DBL fraction;
	const BLEND_MAP_ENTRY *Cur, *Prev;
	Colour Temp_Colour;
	const BLEND_MAP *Blend_Map = Pigment->Blend_Map;
	UV_VECT UV_Coords;

	if ((Thread->qualityFlags & Q_QUICKC) != 0 && Pigment->Quick_Colour[pRED] != -1.0 && Pigment->Quick_Colour[pGREEN] != -1.0 && Pigment->Quick_Colour[pBLUE] != -1.0)
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

				colour.clear();

				break;

			case PLAIN_PATTERN:

				colour = Pigment->colour;

				break;

			case AVERAGE_PATTERN:

				Warp_EPoint (TPoint, EPoint, reinterpret_cast<const TPATTERN *>(Pigment));

				Do_Average_Pigments(colour, Pigment, TPoint, Intersect, ray, Thread);

				break;

			case UV_MAP_PATTERN:
				if(Intersect == NULL)
					throw POV_EXCEPTION_STRING("The 'uv_mapping' pattern cannot be used as part of a pigment function!");

				Cur = &(Pigment->Blend_Map->Blend_Map_Entries[0]);

				if (Blend_Map->Type == COLOUR_TYPE)
				{
					Colour_Found = true;

					Assign_Colour(*colour, Cur->Vals.colour);
				}
				else
				{
					/* Don't bother warping, simply get the UV vect of the intersection */
					Intersect->Object->UVCoord(UV_Coords, Intersect, Thread);
					TPoint[X] = UV_Coords[U];
					TPoint[Y] = UV_Coords[V];
					TPoint[Z] = 0;

					if (Compute_Pigment(colour, Cur->Vals.Pigment,TPoint,Intersect, ray, Thread))
						Colour_Found = true;
				}

				break;

			case BITMAP_PATTERN:

				Warp_EPoint (TPoint, EPoint, reinterpret_cast<const TPATTERN *>(Pigment));

				colour.clear();

				Colour_Found = image_map (TPoint, Pigment, colour);

				break;

			default:

				throw POV_EXCEPTION_STRING("Pigment type not yet implemented.");
		}

		return(Colour_Found);
	}

	Colour_Found = false;

	/* NK 19 Nov 1999 added Warp_EPoint */
	Warp_EPoint (TPoint, EPoint, reinterpret_cast<const TPATTERN *>(Pigment));
	value = Evaluate_TPat (reinterpret_cast<const TPATTERN *>(Pigment),TPoint,Intersect, ray, Thread);

	Search_Blend_Map (value, Blend_Map, &Prev, &Cur);

	if (Blend_Map->Type == COLOUR_TYPE)
	{
		Colour_Found = true;

		Assign_Colour(*colour, Cur->Vals.colour);
	}
	else
	{
		Warp_EPoint (TPoint, EPoint, reinterpret_cast<const TPATTERN *>(Pigment));

		if (Compute_Pigment(colour, Cur->Vals.Pigment,TPoint,Intersect, ray, Thread))
			Colour_Found = true;
	}

	if (Prev != Cur)
	{
		if (Blend_Map->Type == COLOUR_TYPE)
		{
			Colour_Found = true;

			Assign_Colour(*Temp_Colour, Prev->Vals.colour);
		}
		else
		{
			if (Compute_Pigment(Temp_Colour, Prev->Vals.Pigment, TPoint, Intersect, ray, Thread))
				Colour_Found = true;
		}

		fraction = (value - Prev->value) / (Cur->value - Prev->value);

		colour = Temp_Colour + fraction * (colour - Temp_Colour);
	}

	return(Colour_Found);
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

static void Do_Average_Pigments (Colour& colour, const PIGMENT *Pigment, const VECTOR EPoint, const Intersection *Intersect, const Ray *ray, TraceThreadData *Thread)
{
	int i;
	Colour LC;
	BLEND_MAP *Map = Pigment->Blend_Map;
	SNGL Value;
	SNGL Total = 0.0;

	colour.clear();

	for (i = 0; i < Map->Number_Of_Entries; i++)
	{
		Value = Map->Blend_Map_Entries[i].value;

		Compute_Pigment (LC, Map->Blend_Map_Entries[i].Vals.Pigment, EPoint, Intersect, ray, Thread);

		colour += LC * Value;
		Total  += Value;
	}
	colour /= Total;
}

void Evaluate_Density_Pigment(const PIGMENT *pigm, const Vector3d& p, RGBColour& c, TraceThreadData *ttd)
{
	Colour lc;

	c.set(1.0);

	while(pigm != NULL)
	{
		lc.clear();

		Compute_Pigment(lc, pigm, *p, NULL, NULL, ttd);

		c.red()   *= lc.red();
		c.green() *= lc.green();
		c.blue()  *= lc.blue();

		pigm = reinterpret_cast<const PIGMENT *>(pigm->Next);
	}
}

}

