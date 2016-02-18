//******************************************************************************
///
/// @file core/shape/heightfield.cpp
///
/// Implementation of the height field geometric primitive.
///
/// @author Doug Muir
/// @author David Buck
/// @author Drew Wells
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

/****************************************************************************
*
*  Explanation:
*
*    -
*
*  Syntax:
*
*  ---
*
*  Aug 1994 : Merged functions for CSG height fields into functions for
*             non-CSG height fiels. Moved all height field map related
*             data into one data structure. Fixed memory problems. [DB]
*
*  Feb 1995 : Major rewrite of the height field intersection tests. [DB]
*
*****************************************************************************/

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/shape/heightfield.h"

#include <algorithm>

#include "base/pov_err.h"

#include "core/math/matrix.h"
#include "core/render/ray.h"
#include "core/scene/tracethreaddata.h"
#include "core/shape/box.h"
#include "core/support/imageutil.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

#define sign(x) (((x) >= 0.0) ? 1.0 : -1.0)

#define Get_Height(x, z) ((DBL)Data->Map[(z)][(x)])

/* Small offest. */

const DBL HFIELD_OFFSET = 0.001;

const DBL HFIELD_TOLERANCE = 1.0e-6;



/*****************************************************************************
*
* FUNCTION
*
*   All_HField_Intersections
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Feb 1995 : Modified to work with new intersection functions. [DB]
*
******************************************************************************/

bool HField::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    int Side1, Side2;
    Vector3d Start;
    BasicRay Temp_Ray;
    DBL depth1, depth2;

    Thread->Stats()[Ray_HField_Tests]++;

    MInvTransRay(Temp_Ray, ray, Trans);

#ifdef HFIELD_EXTRA_STATS
    Thread->Stats()[Ray_HField_Box_Tests]++;
#endif

    if (!Box::Intersect(Temp_Ray,NULL,bounding_corner1,bounding_corner2,&depth1,&depth2,&Side1,&Side2))
    {
        return(false);
    }

#ifdef HFIELD_EXTRA_STATS
    Thread->Stats()[Ray_HField_Box_Tests_Succeeded]++;
#endif

    if (depth1 < HFIELD_TOLERANCE)
    {
        depth1 = HFIELD_TOLERANCE;

        if (depth1 > depth2)
        {
            return(false);
        }
    }

    Start = Temp_Ray.Evaluate(depth1);

    if (block_traversal(Temp_Ray, Start, Depth_Stack, ray, depth1, depth2, Thread))
    {
        Thread->Stats()[Ray_HField_Tests_Succeeded]++;

        return(true);
    }
    return(false);
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_HField
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool HField::Inside(const Vector3d& IPoint, TraceThreadData *Thread) const
{
    int px, pz;
    DBL x,z,y1,y2,y3,water, dot1, dot2;
    Vector3d Local_Origin, H_Normal, Test;

    MInvTransPoint(Test, IPoint, Trans);

    water = bounding_corner1[Y];

    if ((Test[X] < 0.0) || (Test[X] >= bounding_corner2[X]) ||
        (Test[Z] < 0.0) || (Test[Z] >= bounding_corner2[Z]))
    {
        return(Test_Flag(this, INVERTED_FLAG));
    }

    if (Test[Y] >= bounding_corner2[Y])
    {
        return(Test_Flag(this, INVERTED_FLAG));
    }

    if (Test[Y] < water)
    {
        return(!Test_Flag(this, INVERTED_FLAG));
    }

    px = (int)Test[X];
    pz = (int)Test[Z];

    x = Test[X] - (DBL)px;
    z = Test[Z] - (DBL)pz;

    if ((x+z) < 1.0)
    {
        y1 = max(Get_Height(px,   pz), water);
        y2 = max(Get_Height(px+1, pz), water);
        y3 = max(Get_Height(px,   pz+1), water);

        Local_Origin = Vector3d((DBL)px,y1,(DBL)pz);

        H_Normal = Vector3d(y1-y2, 1.0, y1-y3);
    }
    else
    {
        px = (int)ceil(Test[X]);
        pz = (int)ceil(Test[Z]);

        y1 = max(Get_Height(px,   pz), water);
        y2 = max(Get_Height(px-1, pz), water);
        y3 = max(Get_Height(px,   pz-1), water);

        Local_Origin = Vector3d((DBL)px,y1,(DBL)pz);

        H_Normal = Vector3d(y2-y1, 1.0, y3-y1);
    }

    dot1 = dot(Test, H_Normal);
    dot2 = dot(Local_Origin, H_Normal);

    if (dot1 < dot2)
    {
        return(!Test_Flag(this, INVERTED_FLAG));
    }

    return(Test_Flag(this, INVERTED_FLAG));
}



/*****************************************************************************
*
* FUNCTION
*
*   HField_Normal
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   2000 : NK crash bugfix
*
******************************************************************************/

void HField::Normal(Vector3d& Result, Intersection *Inter, TraceThreadData *Thread) const
{
    int px,pz, i;
    DBL x,z,y1,y2,y3,u,v;
    Vector3d Local_Origin;
    Vector3d n[4];

    if(Inter->haveNormal == true)
    {
        Result = Inter->INormal;
        return;
    }

    MInvTransPoint(Local_Origin, Inter->IPoint, Trans);

    px = (int)Local_Origin[X];
    pz = (int)Local_Origin[Z];

    if (px>Data->max_x)
        px=Data->max_x;
    if (pz>Data->max_z)
        pz=Data->max_z;

    x = Local_Origin[X] - (DBL)px;
    z = Local_Origin[Z] - (DBL)pz;

    if (Test_Flag(this, SMOOTHED_FLAG))
    {
        n[0][X] = Data->Normals[pz][px][0];
        n[0][Y] = Data->Normals[pz][px][1];
        n[0][Z] = Data->Normals[pz][px][2];
        n[1][X] = Data->Normals[pz][px+1][0];
        n[1][Y] = Data->Normals[pz][px+1][1];
        n[1][Z] = Data->Normals[pz][px+1][2];
        n[2][X] = Data->Normals[pz+1][px][0];
        n[2][Y] = Data->Normals[pz+1][px][1];
        n[2][Z] = Data->Normals[pz+1][px][2];
        n[3][X] = Data->Normals[pz+1][px+1][0];
        n[3][Y] = Data->Normals[pz+1][px+1][1];
        n[3][Z] = Data->Normals[pz+1][px+1][2];

        for (i = 0; i < 4; i++)
        {
            MTransNormal(n[i], n[i], Trans);
            n[i].normalize();
        }

        u = (1.0 - x);
        v = (1.0 - z);

        Result = v*(u*n[0] + x*n[1]) + z*(u*n[2] + x*n[3]);
    }
    else
    {
        if ((x+z) <= 1.0)
        {
            /* Lower triangle. */

            y1 = Get_Height(px,   pz);
            y2 = Get_Height(px+1, pz);
            y3 = Get_Height(px,   pz+1);

            Result = Vector3d(y1-y2, 1.0, y1-y3);
        }
        else
        {
            /* Upper triangle. */

            y1 = Get_Height(px+1, pz+1);
            y2 = Get_Height(px,   pz+1);
            y3 = Get_Height(px+1, pz);

            Result = Vector3d(y2-y1, 1.0, y3-y1);
        }

        MTransNormal(Result, Result, Trans);
    }

    Result.normalize();
}



/*****************************************************************************
*
* FUNCTION
*
*   normalize
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

DBL HField::normalize(Vector3d& A, const Vector3d& B)
{
    DBL tmp;

    tmp = B.length();

    if (fabs(tmp) > EPSILON)
    {
        A = B / tmp;
    }
    else
    {
        A = Vector3d(0.0, 1.0, 0.0);
    }

    return(tmp);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_pixel
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   2000 : NK crash bugfix
*
******************************************************************************/

bool HField::intersect_pixel(int x, int z, const BasicRay &ray, DBL height1, DBL height2, IStack& HField_Stack, const BasicRay &RRay, DBL mindist, DBL maxdist, TraceThreadData *Thread)
{
    int Found;
    DBL dot1, depth1, depth2;
    DBL s, t, y1, y2, y3, y4;
    DBL min_y2_y3, max_y2_y3;
    DBL max_height, min_height;
    Vector3d P;
    Vector3d N, V1;

#ifdef HFIELD_EXTRA_STATS
    Thread->Stats()[Ray_HField_Cell_Tests]++;
#endif

    if (z>Data->max_z) z = Data->max_z;
    if (x>Data->max_x) x = Data->max_x;

    y1 = Get_Height(x,   z);
    y2 = Get_Height(x+1, z);
    y3 = Get_Height(x,   z+1);
    y4 = Get_Height(x+1, z+1);

    /* Do we hit this cell at all? */

    if (y2 < y3)
    {
        min_y2_y3 = y2;
        max_y2_y3 = y3;
    }
    else
    {
        min_y2_y3 = y3;
        max_y2_y3 = y2;
    }

    min_height = min(min(y1, y4), min_y2_y3);
    max_height = max(max(y1, y4), max_y2_y3);

    if ((max_height < height1) || (min_height > height2))
    {
        return(false);
    }

#ifdef HFIELD_EXTRA_STATS
    Thread->Stats()[Ray_HField_Cell_Tests_Succeeded]++;
#endif

    Found = false;

    /* Check if we'll hit first triangle. */

    min_height = min(y1, min_y2_y3);
    max_height = max(y1, max_y2_y3);

    if ((max_height >= height1) && (min_height <= height2))
    {
#ifdef HFIELD_EXTRA_STATS
        Thread->Stats()[Ray_HField_Triangle_Tests]++;
#endif

        /* Set up triangle. */

        P = Vector3d((DBL)x, y1, (DBL)z);

        /*
         * Calculate the normal vector from:
         *
         * N = V2 x V1, with V1 = <1, y2-y1, 0>, V2 = <0, y3-y1, 1>.
         */

        N = Vector3d(y1-y2, 1.0, y1-y3);

        /* Now intersect the triangle. */

        dot1 = dot(N, ray.Direction);

        if ((dot1 > EPSILON) || (dot1 < -EPSILON))
            // (Rays virtually parallel to the triangle's plane are asking for mathematical trouble,
            // so they're always presumed to be a no-hit.)
        {
            V1 = P - ray.Origin;

            depth1 = dot(N, V1);

            depth1 /= dot1;

            if ((depth1 >= mindist) && (depth1 <= maxdist))
                // (More expensive check to make sure the ray isn't near-parallel to the triangle's plane.)
            {
                s = ray.Origin[X] + depth1 * ray.Direction[X] - (DBL)x;
                t = ray.Origin[Z] + depth1 * ray.Direction[Z] - (DBL)z;

                if ((s >= -0.0001) && (t >= -0.0001) && ((s+t) <= 1.0001))
                    // (Check whether the point of intersection with the plane is within the triangle)
                {
#ifdef HFIELD_EXTRA_STATS
                    Thread->Stats()[Ray_HField_Triangle_Tests_Succeeded]++;
#endif

                    P = RRay.Evaluate(depth1);

                    if (Clip.empty() || Point_In_Clip(P, Clip, Thread))
                        // (Check whether the point of intersection is within the clipped-by volume)
                    {
                        if (Test_Flag(this, SMOOTHED_FLAG))
                            // Smoothed height field;
                            // computation of surface normal is still non-trivial from here,
                            // so defer it until we know it's needed.
                            HField_Stack->push(Intersection(depth1, P, this));
                        else
                        {
                            // Non-smoothed height field;
                            // we've already computed the surface normal by now, so just convert it into
                            // world coordinate space, so we don't need to re-compute it in case it's indeed needed later.
                            Vector3d tmp = N;
                            MTransNormal(tmp,tmp,Trans);
                            tmp.normalize();
                            HField_Stack->push(Intersection(depth1, P, tmp, this));
                        }

                        Found = true;
                    }
                }
            }
        }
    }

    /* Check if we'll hit second triangle. */

    min_height = min(y4, min_y2_y3);
    max_height = max(y4, max_y2_y3);

    if ((max_height >= height1) && (min_height <= height2))
    {
#ifdef HFIELD_EXTRA_STATS
        Thread->Stats()[Ray_HField_Triangle_Tests]++;
#endif

        /* Set up triangle. */

        P = Vector3d((DBL)(x+1), y4, (DBL)(z+1));

        /*
         * Calculate the normal vector from:
         *
         * N = V2 x V1, with V1 = <-1, y3-y4, 0>, V2 = <0, y2-y4, -1>.
         */

        N = Vector3d(y3-y4, 1.0, y2-y4);

        /* Now intersect the triangle. */

        dot1 = dot(N, ray.Direction);

        if ((dot1 > EPSILON) || (dot1 < -EPSILON))
            // (Rays virtually parallel to the triangle's plane are asking for mathematical trouble,
            // so they're always presumed to be a no-hit.)
        {
            V1 = P - ray.Origin;

            depth2 = dot(N, V1);

            depth2 /= dot1;

            if ((depth2 >= mindist) && (depth2 <= maxdist))
                // (More expensive check to make sure the ray isn't near-parallel to the triangle's plane.)
            {
                s = ray.Origin[X] + depth2 * ray.Direction[X] - (DBL)x;
                t = ray.Origin[Z] + depth2 * ray.Direction[Z] - (DBL)z;

                if ((s <= 1.0001) && (t <= 1.0001) && ((s+t) >= 0.9999))
                    // (Check whether the point of intersection with the plane is within the triangle)
                {
#ifdef HFIELD_EXTRA_STATS
                    Thread->Stats()[Ray_HField_Triangle_Tests_Succeeded]++;
#endif

                    P = RRay.Evaluate(depth2);

                    if (Clip.empty() || Point_In_Clip(P, Clip, Thread))
                        // (Check whether the point of intersection is within the clipped-by volume)
                    {
                        if (Test_Flag(this, SMOOTHED_FLAG))
                            // Smoothed height field;
                            // computation of surface normal is still non-trivial from here,
                            // so defer it until we know it's needed.
                            HField_Stack->push(Intersection(depth2, P, this));
                        else
                        {
                            // Non-smoothed height field;
                            // we've already computed the surface normal by now, so just convert it into
                            // world coordinate space, so we don't need to re-compute it in case it's indeed needed later.
                            Vector3d tmp = N;
                            MTransNormal(tmp,tmp,Trans);
                            tmp.normalize();
                            HField_Stack->push(Intersection(depth2, P, tmp, this));
                        }

                        Found = true;
                    }
                }
            }
        }
    }

    return(Found);
}



/*****************************************************************************
*
* FUNCTION
*
*   add_single_normal
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

int HField::add_single_normal(HF_VAL **data, int xsize, int zsize, int x0, int z0, int x1, int z1, int x2, int z2, Vector3d& N)
{
    Vector3d v0, v1, v2;
    Vector3d t0, t1, Nt;

    if ((x0 < 0) || (z0 < 0) ||
        (x1 < 0) || (z1 < 0) ||
        (x2 < 0) || (z2 < 0) ||
        (x0 > xsize) || (z0 > zsize) ||
        (x1 > xsize) || (z1 > zsize) ||
        (x2 > xsize) || (z2 > zsize))
    {
        return(0);
    }
    else
    {
        v0 = Vector3d(x0, (DBL)data[z0][x0], z0);
        v1 = Vector3d(x1, (DBL)data[z1][x1], z1);
        v2 = Vector3d(x2, (DBL)data[z2][x2], z2);

        t0 = v2 - v0;
        t1 = v1 - v0;

        Nt = cross(t0, t1);

        Nt.normalize();

        if (Nt[Y] < 0.0)
        {
            Nt.invert();
        }

        N += Nt;

        return(1);
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   smooth_height_field
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
*
* DESCRIPTION
*
*   Given a height field that only contains an elevation grid, this
*   routine will walk through the data and produce averaged normals
*   for all points on the grid.
*
* CHANGES
*
*   -
*
******************************************************************************/

void HField::smooth_height_field(int xsize, int zsize)
{
    int i, j, k;
    Vector3d N;
    HF_VAL **map = Data->Map;

    /* First off, allocate all the memory needed to store the normal information */

    Data->Normals_Height = zsize+1;

    Data->Normals = reinterpret_cast<HF_Normals **>(POV_MALLOC((zsize+1)*sizeof(HF_Normals *), "height field normals"));

    for (i = 0; i <= zsize; i++)
    {
        Data->Normals[i] = reinterpret_cast<HF_Normals *>(POV_MALLOC((xsize+1)*sizeof(HF_Normals), "height field normals"));
    }

    /*
     * For now we will do it the hard way - by generating the normals
     * individually for each elevation point.
     */

    for (i = 0; i <= zsize; i++)
    {
        for (j = 0; j <= xsize; j++)
        {
            N = Vector3d(0.0, 0.0, 0.0);

            k = 0;

            k += add_single_normal(map, xsize, zsize, j, i, j+1, i, j, i+1, N);
            k += add_single_normal(map, xsize, zsize, j, i, j, i+1, j-1, i, N);
            k += add_single_normal(map, xsize, zsize, j, i, j-1, i, j, i-1, N);
            k += add_single_normal(map, xsize, zsize, j, i, j, i-1, j+1, i, N);

            if (k == 0)
            {
                throw POV_EXCEPTION_STRING("Failed to find any normals at.");
            }

            N.normalize();

            Data->Normals[i][j][0] = (short)(32767 * N[X]);
            Data->Normals[i][j][1] = (short)(32767 * N[Y]);
            Data->Normals[i][j][2] = (short)(32767 * N[Z]);
        }
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_HField
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
*
* DESCRIPTION
*
*   Copy image data into height field map. Create bounding blocks
*   for the block traversal. Calculate normals for smoothed height fields.
*
* CHANGES
*
*   Feb 1995 : Modified to work with new intersection functions. [DB]
*
******************************************************************************/

void HField::Compute_HField(const ImageData *image)
{
    int x, z, max_x, max_z;
    HF_VAL min_y, max_y, temp_y;

    /* Get height field map size. */

    max_x = image->iwidth;
    max_z = image->iheight;

    /* Allocate memory for map. */

    Data->Map = reinterpret_cast<HF_VAL **>(POV_MALLOC(max_z*sizeof(HF_VAL *), "height field"));

    for (z = 0; z < max_z; z++)
    {
        Data->Map[z] = reinterpret_cast<HF_VAL *>(POV_MALLOC(max_x*sizeof(HF_VAL), "height field"));
    }

    /* Copy map. */

    min_y = 65535L;
    max_y = 0;

    for (z = 0; z < max_z; z++)
    {
        for (x = 0; x < max_x; x++)
        {
            temp_y = image_height_at(image, x, max_z - z - 1);

            Data->Map[z][x] = temp_y;

            min_y = min(min_y, temp_y);
            max_y = max(max_y, temp_y);
        }
    }

    /* Resize bounding box. */

    Data->min_y = min_y;
    Data->max_y = max_y;

    bounding_corner1[Y] = max((DBL)min_y, bounding_corner1[Y]) - HFIELD_OFFSET;
    bounding_corner2[Y] = (DBL)max_y + HFIELD_OFFSET;

    /* Compute smoothed height field. */

    if (Test_Flag(this, SMOOTHED_FLAG))
    {
        smooth_height_field(max_x-1, max_z-1);
    }

    Data->max_x = max_x-2;
    Data->max_z = max_z-2;

    build_hfield_blocks();
}



/*****************************************************************************
*
* FUNCTION
*
*   build_hfield_blocks
*
* INPUT
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
*   Create the bounding block hierarchy used by the block traversal.
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

void HField::build_hfield_blocks()
{
    int x, z, nx, nz, wx, wz;
    int i, j;
    int xmin, xmax, zmin, zmax;
    DBL y, ymin, ymax, water;

    /* Get block size. */

    nx = max(1, (int)(sqrt((DBL)Data->max_x)));
    nz = max(1, (int)(sqrt((DBL)Data->max_z)));

    /* Get dimensions of sub-block. */

    wx = (int)ceil((DBL)(Data->max_x + 2) / (DBL)nx);
    wz = (int)ceil((DBL)(Data->max_z + 2) / (DBL)nz);

    /* Increase number of sub-blocks if necessary. */

    if (nx * wx < Data->max_x + 2)
    {
        nx++;
    }

    if (nz * wz < Data->max_z + 2)
    {
        nz++;
    }

    if (!Test_Flag(this, HIERARCHY_FLAG) || ((nx == 1) && (nz == 1)))
    {
        /* We don't want a bounding hierarchy. Just use one block. */

        Data->Block = reinterpret_cast<HFIELD_BLOCK **>(POV_MALLOC(sizeof(HFIELD_BLOCK *), "height field blocks"));

        Data->Block[0] = reinterpret_cast<HFIELD_BLOCK *>(POV_MALLOC(sizeof(HFIELD_BLOCK), "height field blocks"));

        Data->Block[0][0].xmin = 0;
        Data->Block[0][0].xmax = Data->max_x;
        Data->Block[0][0].zmin = 0;
        Data->Block[0][0].zmax = Data->max_z;

        Data->Block[0][0].ymin = bounding_corner1[Y];
        Data->Block[0][0].ymax = bounding_corner2[Y];

        Data->block_max_x = 1;
        Data->block_max_z = 1;

        Data->block_width_x = Data->max_x + 2;
        Data->block_width_z = Data->max_y + 2;

//      Debug_Info("Height field: %d x %d (1 x 1 blocks)\n", Data->max_x+2, Data->max_z+2);

        return;
    }

//  Debug_Info("Height field: %d x %d (%d x %d blocks)\n", Data->max_x+2, Data->max_z+2, nx, nz);

    /* Allocate memory for blocks. */

    Data->Block = reinterpret_cast<HFIELD_BLOCK **>(POV_MALLOC(nz*sizeof(HFIELD_BLOCK *), "height field blocks"));

    /* Store block information. */

    Data->block_max_x = nx;
    Data->block_max_z = nz;

    Data->block_width_x = wx;
    Data->block_width_z = wz;

    water = bounding_corner1[Y];

    for (z = 0; z < nz; z++)
    {
        Data->Block[z] = reinterpret_cast<HFIELD_BLOCK *>(POV_MALLOC(nx*sizeof(HFIELD_BLOCK), "height field blocks"));

        for (x = 0; x < nx; x++)
        {
            /* Get block's borders. */

            xmin = x * wx;
            zmin = z * wz;

            xmax = min((x + 1) * wx - 1, Data->max_x);
            zmax = min((z + 1) * wz - 1, Data->max_z);

            /* Find min. and max. height in current block. */

            ymin = BOUND_HUGE;
            ymax = -BOUND_HUGE;

            for (i = xmin; i <= xmax+1; i++)
            {
                for (j = zmin; j <= zmax+1; j++)
                {
                    y = Get_Height(i, j);

                    ymin = min(ymin, y);
                    ymax = max(ymax, y);
                }
            }

            /* Store block's borders. */

            Data->Block[z][x].xmin = xmin;
            Data->Block[z][x].xmax = xmax;
            Data->Block[z][x].zmin = zmin;
            Data->Block[z][x].zmax = zmax;

            Data->Block[z][x].ymin = max(ymin, water) - HFIELD_OFFSET;
            Data->Block[z][x].ymax = ymax + HFIELD_OFFSET;
        }
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_HField
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void HField::Translate(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_HField
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void HField::Rotate(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_HField
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void HField::Scale(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_HField
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void HField::Transform(const TRANSFORM *tr)
{
    Compose_Transforms(Trans, tr);

    Compute_BBox();
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_HField
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
*
* DESCRIPTION
*
*   Allocate and intialize a height field.
*
* CHANGES
*
*   Feb 1995 : Modified to work with new intersection functions. [DB]
*
******************************************************************************/

HField::HField() : ObjectBase(HFIELD_OBJECT)
{
    Trans = Create_Transform();

    bounding_corner1 = Vector3d(-1.0, -1.0, -1.0);
    bounding_corner2 = Vector3d( 1.0,  1.0,  1.0);

    /* Allocate height field data. */

    Data = reinterpret_cast<HFIELD_DATA *>(POV_MALLOC(sizeof(HFIELD_DATA), "height field"));

    Data->References = 1;

    Data->Normals_Height = 0;

    Data->Map     = NULL;
    Data->Normals = NULL;

    Data->max_x = 0;
    Data->max_z = 0;

    Data->block_max_x = 0;
    Data->block_max_z = 0;

    Data->block_width_x = 0;
    Data->block_width_z = 0;

    Set_Flag(this, HIERARCHY_FLAG);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_HField
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
*
* DESCRIPTION
*
*   NOTE: The height field data is not copied, only the number of references
*         is counted, so that Destray_HField() knows if it can be destroyed.
*
* CHANGES
*
*   -
*
******************************************************************************/

ObjectPtr HField::Copy()
{
    HField *New = new HField();

    POV_FREE (New->Data);

    /* Copy height field. */

    Destroy_Transform(New->Trans);
    *New = *this;
    New->Trans = Copy_Transform(Trans);

    New->bounding_corner1 = bounding_corner1;
    New->bounding_corner2 = bounding_corner2;

    New->Data = Data;
    New->Data->References++;

    return(New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_HField
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Doug Muir, David Buck, Drew Wells
*
* DESCRIPTION
*
*   NOTE: The height field data is destroyed if it's no longer
*         used by any copy.
*
* CHANGES
*
*   Feb 1995 : Modified to work with new intersection functions. [DB]
*
******************************************************************************/

HField::~HField()
{
    int i;

    if (--(Data->References) == 0)
    {
        if (Data->Map != NULL)
        {
            for (i = 0; i < Data->max_z+2; i++)
            {
                if (Data->Map[i] != NULL)
                {
                    POV_FREE (Data->Map[i]);
                }
            }

            POV_FREE (Data->Map);
        }

        if (Data->Normals != NULL)
        {
            for (i = 0; i < Data->Normals_Height; i++)
            {
                POV_FREE (Data->Normals[i]);
            }

            POV_FREE (Data->Normals);
        }

        if (Data->Block != NULL)
        {
            for (i = 0; i < Data->block_max_z; i++)
            {
                POV_FREE(Data->Block[i]);
            }

            POV_FREE(Data->Block);
        }

        POV_FREE (Data);
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_HField_BBox
*
* INPUT
*
*   HField - Height field
*
* OUTPUT
*
*   HField
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the bounding box of a height field.
*
* CHANGES
*
*   Aug 1994 : Creation.
*
******************************************************************************/

void HField::Compute_BBox()
{
    BBox.lowerLeft = BBoxVector3d(bounding_corner1);

    BBox.size = BBoxVector3d(bounding_corner2 - bounding_corner1);

    if (Trans != NULL)
    {
        Recompute_BBox(&BBox, Trans);
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   dda_traversal
*
* INPUT
*
*   Ray    - Current ray
*   HField - Height field
*   Start  - Start point for the walk
*   Block  - Sub-block of the height field to traverse
*
* OUTPUT
*
* RETURNS
*
*   int - true if intersection was found
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Traverse the grid cell of the height field using a modified DDA.
*
*   Based on the following article:
*
*     Musgrave, F. Kenton, "Grid Tracing: Fast Ray Tracing for Height
*     Fields", Research Report YALEU-DCS-RR-39, Yale University, July 1988
*
*   You should note that there are (n-1) x (m-1) grid cells in a height
*   field of (image) size n x m. A grid cell (i,j), 0 <= i <= n-1,
*   0 <= j <= m-1, extends from x = i to x = i + 1 - epsilon and
*   y = j to y = j + 1 -epsilon.
*
* CHANGES
*
*   Feb 1995 : Creation.
*
******************************************************************************/

bool HField::dda_traversal(const BasicRay &ray, const Vector3d& Start, const HFIELD_BLOCK *Block, IStack &HField_Stack, const BasicRay &RRay, DBL mindist, DBL maxdist, TraceThreadData *Thread)
{
    const char *dda_msg = "Illegal grid value in dda_traversal().\n"
                          "The height field may contain dark spots. To eliminate them\n"
                          "moving the camera a tiny bit should help. For more information\n"
                          "read the user manual!";
    int found;
    int xmin, xmax, zmin, zmax;
    int x, z, signx, signz;
    DBL ymin, ymax, y1, y2;
    DBL px, pz, dx, dy, dz;
    DBL delta, error, x0, z0;
    DBL neary, fary, deltay;

    /* Setup DDA. */

    found = false;

    px = Start[X];
    pz = Start[Z];

    /* Get dimensions of current block. */

    xmin = Block->xmin;
    xmax = min(Block->xmax + 1, Data->max_x);
    zmin = Block->zmin;
    zmax = min(Block->zmax + 1, Data->max_z);

    ymin = min(Start[Y], Block->ymin) - EPSILON;
    ymax = max(Start[Y], Block->ymax) + EPSILON;

    /* Check for illegal grid values (caused by numerical inaccuracies). */

    if (px < (DBL)xmin)
    {
        if (px < (DBL)xmin - HFIELD_OFFSET)
        {
;// TODO MESSAGE      Warning(dda_msg);

            return(false);
        }
        else
        {
            px = (DBL)xmin;
        }
    }
    else
    {
        if (px > (DBL)xmax + 1.0 - EPSILON)
        {
            if (px > (DBL)xmax + 1.0 + EPSILON)
            {
;// TODO MESSAGE        Warning(dda_msg);

                return(false);
            }
            else
            {
                px = (DBL)xmax + 1.0 - EPSILON;
            }
        }
    }

    if (pz < (DBL)zmin)
    {
        if (pz < (DBL)zmin - HFIELD_OFFSET)
        {
;// TODO MESSAGE      Warning(dda_msg);

            return(false);
        }
        else
        {
            pz = (DBL)zmin;
        }
    }
    else
    {
        if (pz > (DBL)zmax + 1.0 - EPSILON)
        {
            if (pz > (DBL)zmax + 1.0 + EPSILON)
            {
;// TODO MESSAGE        Warning(dda_msg);

                return(false);
            }
            else
            {
                pz = (DBL)zmax + 1.0 - EPSILON;
            }
        }
    }

    dx = ray.Direction[X];
    dy = ray.Direction[Y];
    dz = ray.Direction[Z];

    /*
     * Here comes the DDA algorithm.
     */

    /* Choose algorithm depending on the driving axis. */

    if (fabs(dx) >= fabs(dz))
    {
        /*
         * X-axis is driving axis.
         */

        delta = fabs(dz / dx);

        x = (int)px;
        z = (int)pz;

        x0 = px - floor(px);
        z0 = pz - floor(pz);

        signx = sign(dx);
        signz = sign(dz);

        /* Get initial error. */

        if (dx >= 0.0)
        {
            if (dz >= 0.0)
            {
                error = z0 + delta * (1.0 - x0) - 1.0;
            }
            else
            {
                error = -(z0 - delta * (1.0 - x0));
            }
        }
        else
        {
            if (dz >= 0.0)
            {
                error = z0 + delta * x0 - 1.0;
            }
            else
            {
                error = -(z0 - delta * x0);
            }
        }

        /* Get y differential. */

        deltay = dy / fabs(dx);

        if (dx >= 0.0)
        {
            neary = Start[Y] - x0 * deltay;

            fary = neary + deltay;
        }
        else
        {
            neary = Start[Y] - (1.0 - x0) * deltay;

            fary = neary + deltay;
        }

        /* Step through the cells. */

        do
        {
            if (neary < fary)
            {
                y1 = neary;
                y2 = fary;
            }
            else
            {
                y1 = fary;
                y2 = neary;
            }

            if (intersect_pixel(x, z, ray, y1, y2, HField_Stack, RRay, mindist, maxdist, Thread))
            {
                if (Type & IS_CHILD_OBJECT)
                {
                    found = true;
                }
                else
                {
                    return(true);
                }
            }

            if (error > EPSILON)
            {
                z += signz;

                if ((z < zmin) || (z > zmax))
                {
                    break;
                }
                else
                {
                    if (intersect_pixel(x, z, ray, y1, y2, HField_Stack, RRay, mindist, maxdist, Thread))
                    {
                        if (Type & IS_CHILD_OBJECT)
                        {
                            found = true;
                        }
                        else
                        {
                            return(true);
                        }
                    }
                }

                error--;
            }
            else
            {
                if (error > -EPSILON)
                {
                    z += signz;

                    error--;
                }
            }

            x += signx;

            error += delta;

            neary = fary;

            fary += deltay;
        }
        while ((neary >= ymin) && (neary <= ymax) && (x >= xmin) && (x <= xmax) && (z >= zmin) && (z <= zmax));
    }
    else
    {
        /*
         * Z-axis is driving axis.
         */

        delta = fabs(dx / dz);

        x = (int)px;
        z = (int)pz;

        x0 = px - floor(px);
        z0 = pz - floor(pz);

        signx = sign(dx);
        signz = sign(dz);

        /* Get initial error. */

        if (dz >= 0.0)
        {
            if (dx >= 0.0)
            {
                error = x0 + delta * (1.0 - z0) - 1.0;
            }
            else
            {
                error = -(x0 - delta * (1.0 - z0));
            }
        }
        else
        {
            if (dx >= 0.0)
            {
                error = x0 + delta * z0 - 1.0;
            }
            else
            {
                error = -(x0 - delta * z0);
            }
        }

        /* Get y differential. */

        deltay = dy / fabs(dz);

        if (dz >= 0.0)
        {
            neary = Start[Y] - z0 * deltay;

            fary = neary + deltay;
        }
        else
        {
            neary = Start[Y] - (1.0 - z0) * deltay;

            fary = neary + deltay;
        }

        /* Step through the cells. */

        do
        {
            if (neary < fary)
            {
                y1 = neary;
                y2 = fary;
            }
            else
            {
                y1 = fary;
                y2 = neary;
            }

            if (intersect_pixel(x, z, ray, y1, y2, HField_Stack, RRay, mindist, maxdist, Thread))
            {
                if (Type & IS_CHILD_OBJECT)
                {
                    found = true;
                }
                else
                {
                    return(true);
                }
            }

            if (error > EPSILON)
            {
                x += signx;

                if ((x < xmin) || (x > xmax))
                {
                    break;
                }
                else
                {
                    if (intersect_pixel(x, z, ray, y1, y2, HField_Stack, RRay, mindist, maxdist, Thread))
                    {
                        if (Type & IS_CHILD_OBJECT)
                        {
                            found = true;
                        }
                        else
                        {
                            return(true);
                        }
                    }
                }

                error--;
            }
            else
            {
                if (error > -EPSILON)
                {
                    x += signx;

                    error--;
                }
            }

            z += signz;

            error += delta;

            neary = fary;

            fary += deltay;
        }
        while ((neary >= ymin-EPSILON) && (neary <= ymax+EPSILON) &&
               (x >= xmin) && (x <= xmax) &&
               (z >= zmin) && (z <= zmax));
    }

    return(found);
}



/*****************************************************************************
*
* FUNCTION
*
*   block_traversal
*
* INPUT
*
*   Ray    - Current ray
*   HField - Height field
*   Start  - Start point for the walk
*
* OUTPUT
*
* RETURNS
*
*   int - true if intersection was found
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Traverse the blocks of the height field.
*
* CHANGES
*
*   Feb 1995 : Creation.
*
*   Aug 1996 : Fixed bug as reported by Dean M. Phillips:
*              "I found a bug in the height field code which resulted
*              in "Illegal grid value in dda_traversal()." messages
*              along with dark vertical lines in the height fields.
*              This turned out to be caused by overlooking the units in
*              which some boundary tests in two different places were
*              made. It was easy to fix.
*
******************************************************************************/

bool HField::block_traversal(const BasicRay &ray, const Vector3d& Start, IStack &HField_Stack, const BasicRay &RRay, DBL mindist, DBL maxdist, TraceThreadData *Thread)
{
    int xmax, zmax;
    int x, z, nx, nz, signx, signz;
    int found = false;
    int dx_zero, dz_zero;
    DBL px, pz, dx, dy, dz;
    DBL maxdv;
    DBL ymin, ymax, y1, y2;
    DBL neary, fary;
    DBL k1, k2, dist;
    Vector3d nearP;
    Vector3d farP;
    HFIELD_BLOCK *Block;

    px = Start[X];
    pz = Start[Z];

    dx = ray.Direction[X];
    dy = ray.Direction[Y];
    dz = ray.Direction[Z];

    maxdv = (dx > dz) ? dx : dz;

    /* First test for 'perpendicular' rays. */

    if ((fabs(dx) < EPSILON) && (fabs(dz) < EPSILON))
    {
        x = (int)px;
        z = (int)pz;

        neary = Start[Y];

        if (dy >= 0.0)
        {
            fary = 65536.0;
        }
        else
        {
            fary = 0.0;
        }

        return intersect_pixel(x, z, ray, min(neary, fary), max(neary, fary), HField_Stack, RRay, mindist, maxdist, Thread);
    }

    /* If we don't have blocks we just step through the grid. */

    if ((Data->block_max_x <= 1) && (Data->block_max_z <= 1))
    {
        return dda_traversal(ray, Start, &Data->Block[0][0], HField_Stack, RRay, mindist, maxdist, Thread);
    }

    /* Get dimensions of grid. */

    xmax = Data->block_max_x;
    zmax = Data->block_max_z;

    ymin = (DBL)Data->min_y - EPSILON;
    ymax = (DBL)Data->max_y + EPSILON;

    dx_zero = (fabs(dx) < EPSILON);
    dz_zero = (fabs(dz) < EPSILON);

    signx = sign(dx);
    signz = sign(dz);

    /* Walk on the block grid. */

    px /= Data->block_width_x;
    pz /= Data->block_width_z;

    x = (int)px;
    z = (int)pz;

    nearP = Start;

    neary = Start[Y];

    /*
     * Here comes the block walk algorithm.
     */

    do
    {
#ifdef HFIELD_EXTRA_STATS
        Thread->Stats()[Ray_HField_Block_Tests]++;
#endif

        /* Get current block. */

        Block = &Data->Block[z][x];

        /* Intersect ray with bounding planes. */

        if (dx_zero)
        {
            k1 = BOUND_HUGE;
        }
        else
        {
            if (signx >= 0)
            {
                k1 = ((DBL)Block->xmax + 1.0 - ray.Origin[X]) / dx;
            }
            else
            {
                k1 = ((DBL)Block->xmin - ray.Origin[X]) / dx;
            }
        }

        if (dz_zero)
        {
            k2 = BOUND_HUGE;
        }
        else
        {
            if (signz >= 0)
            {
                k2 = ((DBL)Block->zmax + 1.0 - ray.Origin[Z]) / dz;
            }
            else
            {
                k2 = ((DBL)Block->zmin - ray.Origin[Z]) / dz;
            }
        }

        /* Figure out the indices of the next block. */

        if (dz_zero || ((!dx_zero) && (k1<k2 - EPSILON / maxdv) && (k1>0.0)))
/*      if ((k1 < k2 - EPSILON / maxdv) && (k1 > 0.0)) */
        {
            /* Step along the x-axis. */

            dist = k1;

            nx = x + signx;
            nz = z;
        }
        else
        {
            if (dz_zero || ((!dx_zero) && (k1<k2 + EPSILON / maxdv) && (k1>0.0)))
/*          if ((k1 < k2 + EPSILON / maxdv) && (k1 > 0.0))  */
            {
                /* Step along both axis (very rare case). */

                dist = k1;

                nx = x + signx;
                nz = z + signz;
            }
            else
            {
                /* Step along the z-axis. */

                dist = k2;

                nx = x;
                nz = z + signz;
            }
        }

        /* Get point where ray leaves current block. */

        farP = ray.Evaluate(dist);

        fary = farP[Y];

        if (neary < fary)
        {
            y1 = neary;
            y2 = fary;
        }
        else
        {
            y1 = fary;
            y2 = neary;
        }

        /* Can we hit current block at all? */

        if ((y1 <= (DBL)Block->ymax + EPSILON) && (y2 >= (DBL)Block->ymin - EPSILON))
        {
            /* Test current block. */

#ifdef HFIELD_EXTRA_STATS
            Thread->Stats()[Ray_HField_Block_Tests_Succeeded]++;
#endif

            if (dda_traversal(ray, nearP, &Data->Block[z][x], HField_Stack, RRay, mindist, maxdist, Thread))
            {
                if (Type & IS_CHILD_OBJECT)
                {
                    found = true;
                }
                else
                {
                    return(true);
                }
            }
        }

        /* Step to next block. */

        x = nx;
        z = nz;

        nearP = farP;

        neary = fary;
    }
    while ((x >= 0) && (x < xmax) && (z >= 0) && (z < zmax) && (neary >= ymin) && (neary <= ymax));

    return(found);
}

}
