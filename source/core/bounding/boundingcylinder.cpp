//******************************************************************************
///
/// @file core/bounding/boundingcylinder.cpp
///
/// Implementations related to bounding cylinders (used by lathe and sor).
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
#include "core/bounding/boundingcylinder.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/coretypes.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using std::vector;

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Static functions
******************************************************************************/

static int  intersect_thick_cylinder (const BCYL *BCyl, const vector<BCYL_INT>& rint, const vector<BCYL_INT>& hint, const BCYL_ENTRY *Entry, DBL *dist);
static void insert_hit (const BCYL_INT *Element, vector<BCYL_INT>& intervals);
static void intersect_bound_elements (const BCYL *BCyl, vector<BCYL_INT>& rint, vector<BCYL_INT>& hint, const Vector3d& P, const Vector3d& D);


/*****************************************************************************
* Local variables
******************************************************************************/



/*****************************************************************************
*
* FUNCTION
*
*   intersect_thick_cylinder
*
* INPUT
*
*   BCyl - Pointer to lathe structure
*   Entry - Segment whos bounding cylinder to intersect
*   dist  - List of sorted intersection depths
*
* OUTPUT
*
* RETURNS
*
*   int - number of intersections found
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Find all intersections of the current ray with the bounding
*   cylinder of the given segment. The intersection tests are
*   done in intersect_bound_elements() and the results stored
*   in the lathe structure are evaluated here.
*
* CHANGES
*
*   Oct 1996 : Creation.
*
******************************************************************************/

static int intersect_thick_cylinder(const BCYL *BCyl, const vector<BCYL_INT>& rint, const vector<BCYL_INT>& hint, const BCYL_ENTRY *Entry, DBL *dist)
{
    int i, j, n;
    DBL k, r, h;

    n = 0;

    /* Intersect ray with the cap-plane. */

    if (hint[Entry->h2].n)
    {
        r = hint[Entry->h2].w[0];

        if ((r >= BCyl->radius[Entry->r1]) &&
            (r <= BCyl->radius[Entry->r2]))
        {
            dist[n++] = hint[Entry->h2].d[0];
        }
    }

    /* Intersect ray with the base-plane. */

    if (hint[Entry->h1].n)
    {
        r = hint[Entry->h1].w[0];

        if ((r >= BCyl->radius[Entry->r1]) &&
            (r <= BCyl->radius[Entry->r2]))
        {
            dist[n++] = hint[Entry->h1].d[0];
        }
    }

    /* Intersect with inner cylinder. */

    if (rint[Entry->r1].n)
    {
        h = rint[Entry->r1].w[0];

        if ((h >= BCyl->height[Entry->h1]) &&
            (h <= BCyl->height[Entry->h2]))
        {
            dist[n++] = rint[Entry->r1].d[0];
        }

        h = rint[Entry->r1].w[1];

        if ((h >= BCyl->height[Entry->h1]) &&
            (h <= BCyl->height[Entry->h2]))
        {
            dist[n++] = rint[Entry->r1].d[1];
        }
    }

    /* Intersect with outer cylinder. */

    if (rint[Entry->r2].n)
    {
        h = rint[Entry->r2].w[0];

        if ((h >= BCyl->height[Entry->h1]) &&
            (h <= BCyl->height[Entry->h2]))
        {
            dist[n++] = rint[Entry->r2].d[0];
        }

        h = rint[Entry->r2].w[1];

        if ((h >= BCyl->height[Entry->h1]) &&
            (h <= BCyl->height[Entry->h2]))
        {
            dist[n++] = rint[Entry->r2].d[1];
        }
    }

    /* Sort intersections. */

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n - i - 1; j++)
        {
            if (dist[j] > dist[j+1])
            {
                k         = dist[j];
                dist[j]   = dist[j+1];
                dist[j+1] = k;
            }
        }
    }

    return(n);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_bound_elements
*
* INPUT
*
*   BCyl - Pointer to lathe structure
*   P, D  - Current ray
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Intersect all bounding discs and cylinders and store
*   the intersections found in the lathe structure.
*
*   By intersecting all different discs and cylinders once
*   we avoid testing the same cylinders and discs more than
*   once. This happened when we tested one bounding cylinder
*   after the other.
*
* CHANGES
*
*   Oct 1996 : Creation.
*
******************************************************************************/

static void intersect_bound_elements(const BCYL *BCyl, vector<BCYL_INT>& rint, vector<BCYL_INT>& hint, const Vector3d& P, const Vector3d& D)
{
    int i;
    DBL a, b, bb, b2, c, d, k;

    /* Init constants. */

    a = D[X] * D[X] + D[Z] * D[Z];

    b = P[X] * D[X] + P[Z] * D[Z];

    bb = b * b;

    b2 = 2.0 * b;

    c = P[X] * P[X] + P[Z] * P[Z];

    /* Intersect all rings. */

    if ((D[Y] < -EPSILON) || (D[Y] > EPSILON))
    {
        for (i = 0; i < BCyl->nheight; i++)
        {
            k = (BCyl->height[i] - P[Y]) / D[Y];

            hint[i].n = 1;

            hint[i].d[0] = k;

            hint[i].w[0] = k * (a * k + b2) + c;
        }
    }
    else
    {
        for (i = 0; i < BCyl->nheight; i++)
        {
            hint[i].n = 0;
        }
    }

    /* Intersect all cylinders. */

    for (i = 0; i < BCyl->nradius; i++)
    {
        rint[i].n = 0;

        if (BCyl->radius[i] > EPSILON)
        {
            d = bb - a * (c - BCyl->radius[i]);

            if (d > 0.0)
            {
                d = sqrt(d);

                k = (-b + d) / a;

                rint[i].n = 2;

                rint[i].d[0] = k;

                rint[i].w[0] = P[Y] + k * D[Y];

                k = (-b - d) / a;

                rint[i].d[1] = k;

                rint[i].w[1] = P[Y] + k * D[Y];
            }
        }
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   insert_hit
*
* INPUT
*
*   element   - Intersection to insert
*   intervals - List of intervals
*   cnt       - Number of elements in the list
*
* OUTPUT
*
*   intervals, cnt
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Insert an intersection into the depth sorted intersection list.
*
* CHANGES
*
*   Oct 1996 : Creation.
*
******************************************************************************/

static void insert_hit(const BCYL_INT *element, vector<BCYL_INT>& intervals)
{
    // TODO - a heap-based priority queue might be faster

    vector<BCYL_INT>::iterator k = intervals.begin();
    while ((k != intervals.end()) && (element->d[0] > k->d[0]))
        k ++;

    intervals.insert(k, *element);
}



/*****************************************************************************
*
* FUNCTION
*
*   Intersect_All_Bounds
*
* INPUT
*
*   BCyl     - Pointer to lathe structure
*   P, D      - Current ray
*   intervals - List of intervals
*   cnt       - Number of elements in the list
*
* OUTPUT
*
*   intervals, cnt
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Intersect given ray with all bounding cylinders of the given lathe
*   and return a sorted list of intersection depths and segments hit.
*
* CHANGES
*
*   Oct 1996 : Creation.
*
******************************************************************************/

int Intersect_BCyl(const BCYL *BCyl, vector<BCYL_INT>& intervals, vector<BCYL_INT>& rint, vector<BCYL_INT>& hint, const Vector3d& P, const Vector3d& D)
{
    int i;
    DBL dist[8];
    BCYL_INT Inter;
    BCYL_ENTRY *Entry;

    intervals.clear();

    Inter.d[1] = 0.0;

    /* Intersect all cylinder and plane elements. */

    intersect_bound_elements(BCyl, rint, hint, P, D);

    /* Intersect all spline segments. */
    for (i = 0; i < BCyl->number; i++)
    {
        Entry = &BCyl->entry[i];

        switch (intersect_thick_cylinder(BCyl, rint, hint, Entry, dist))
        {
            case 0:
                break;

            case 2:

                if (dist[0] > EPSILON)
                {
                    Inter.d[0] = dist[0];
                    Inter.n    = i;

                    insert_hit(&Inter, intervals);
                }
                else if (dist[1] > EPSILON)
                {
                    Inter.d[0] = 0.0;
                    Inter.n    = i;

                    insert_hit(&Inter, intervals);
                }

                break;

            case 4:

                if (dist[0] > EPSILON)
                {
                    Inter.d[0] = dist[0];
                    Inter.n    = i;

                    insert_hit(&Inter, intervals);
                }
                else if (dist[1] > EPSILON)
                {
                    Inter.d[0] = 0.0;
                    Inter.n    = i;

                    insert_hit(&Inter, intervals);
                }
                else if (dist[2] > EPSILON)
                {
                    Inter.d[0] = dist[2];
                    Inter.n    = i;

                    insert_hit(&Inter, intervals);
                }
                else if (dist[3] > EPSILON)
                {
                    Inter.d[0] = 0.0;
                    Inter.n    = i;

                    insert_hit(&Inter, intervals);
                }

                break;

            default:

                /*
                 * We weren't able to find an even number of intersections. Thus
                 * we can't tell where the ray enters and leaves the bounding
                 * cylinder. To avoid problems we assume that the ray is always
                 * inside the cylinder in that case.
                 */

                Inter.d[0] = dist[0];
                Inter.n    = i;

                insert_hit(&Inter, intervals);

                break;
        }
    }

    return(intervals.size());
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_BCyl
*
* INPUT
*
*   number - number of cylindrical segments
*   r1, r2 - list of segment radii
*   h1, h2 - list of segment heights
*
* OUTPUT
*
* RETURNS
*
*   BCYL * - created bounding cylinder.
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Create a bounding cylinder data structure from the given
*   radii and heights.
*
* CHANGES
*
*   Oct 1996 : Creation.
*
******************************************************************************/

BCYL *Create_BCyl(int number, const DBL *tmp_r1, const DBL *tmp_r2, const DBL *tmp_h1, const DBL *tmp_h2)
{
    int i, j, nr, nh;
    int *tmp_r1_index;
    int *tmp_r2_index;
    int *tmp_h1_index;
    int *tmp_h2_index;
    DBL *tmp_radius;
    DBL *tmp_height;
    BCYL *bcyl;

    /* Allocate bounding cylinder. */

    bcyl = new BCYL;

    /* Allocate entries. */

    bcyl->number = number;

    bcyl->entry = new BCYL_ENTRY[bcyl->number];

    /* Allocate temporary lists. */

    tmp_r1_index = new int[bcyl->number];
    tmp_r2_index = new int[bcyl->number];
    tmp_h1_index = new int[bcyl->number];
    tmp_h2_index = new int[bcyl->number];

    tmp_radius = new DBL[2 * bcyl->number];
    tmp_height = new DBL[2 * bcyl->number];

    /* Get different bounding radii and heights. */

    nr = 0;
    nh = 0;

    for (i = 0; i < bcyl->number; i++)
    {
        tmp_r1_index[i] = -1;
        tmp_r2_index[i] = -1;
        tmp_h1_index[i] = -1;
        tmp_h2_index[i] = -1;

        for (j = 0; j < nr; j++)
        {
            if (tmp_r1[i] == tmp_radius[j])
            {
                break;
            }
        }

        if (j == nr)
        {
            tmp_radius[nr++] = tmp_r1[i];
        }

        tmp_r1_index[i] = j;

        for (j = 0; j < nr; j++)
        {
            if (tmp_r2[i] == tmp_radius[j])
            {
                break;
            }
        }

        if (j == nr)
        {
            tmp_radius[nr++] = tmp_r2[i];
        }

        tmp_r2_index[i] = j;

        for (j = 0; j < nh; j++)
        {
            if (tmp_h1[i] == tmp_height[j])
            {
                break;
            }
        }

        if (j == nh)
        {
            tmp_height[nh++] = tmp_h1[i];
        }

        tmp_h1_index[i] = j;

        for (j = 0; j < nh; j++)
        {
            if (tmp_h2[i] == tmp_height[j])
            {
                break;
            }
        }

        if (j == nh)
        {
            tmp_height[nh++] = tmp_h2[i];
        }

        tmp_h2_index[i] = j;
    }

    /* Copy lists into the lathe. */

    bcyl->radius = new DBL[nr];
    bcyl->height = new DBL[nh];

    for (i = 0; i < nr; i++)
    {
        bcyl->radius[i] = Sqr(tmp_radius[i]);
    }

    for (i = 0; i < nh; i++)
    {
        bcyl->height[i] = tmp_height[i];
    }

    /* Assign height and radius indices. */

    bcyl->nradius = nr;
    bcyl->nheight = nh;

    for (i = 0; i < bcyl->number; i++)
    {
        bcyl->entry[i].r1 = tmp_r1_index[i];
        bcyl->entry[i].r2 = tmp_r2_index[i];
        bcyl->entry[i].h1 = tmp_h1_index[i];
        bcyl->entry[i].h2 = tmp_h2_index[i];
    }

/*
    fprintf(stderr, "number of different radii   = %d\n", nr);
    fprintf(stderr, "number of different heights = %d\n", nh);
*/

    /* Get rid of temp. memory. */

    delete[] tmp_height;
    delete[] tmp_radius;
    delete[] tmp_h2_index;
    delete[] tmp_h1_index;
    delete[] tmp_r2_index;
    delete[] tmp_r1_index;

    return(bcyl);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_BCyl
*
* INPUT
*
*   BCyl - bounding cylinder
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Destroy a bounding cylinder.
*
* CHANGES
*
*   Oct 1996 : Creation.
*
******************************************************************************/

void Destroy_BCyl(BCYL *BCyl)
{
    delete[] BCyl->entry;

    delete[] BCyl->radius;

    delete[] BCyl->height;

    delete BCyl;
}

}
// end of namespace pov
