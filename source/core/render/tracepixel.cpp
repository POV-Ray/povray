//******************************************************************************
///
/// @file core/render/tracepixel.cpp
///
/// Implementations related to the basics of raytracing a pixel.
///
/// This module implements the TracePixel class, which mostly pertains to
/// setting up the camera, the initial ray for each pixel rendered, and calling
/// TraceRay() for said pixels. It also contains focal blur code, as this is
/// part of camera and ray setup.
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
#include "core/render/tracepixel.h"

// C++ variants of C standard header files
#include <cstring>

// C++ standard header files
#include <vector>

// POV-Ray header files (base module)
#include <algorithm>

// POV-Ray header files (core module)
#include "core/material/normal.h"
#include "core/material/pigment.h"
#include "core/math/chi2.h"
#include "core/math/jitter.h"
#include "core/math/matrix.h"
#include "core/render/ray.h"
#include "core/render/trace.h"
#include "core/scene/object.h"
#include "core/scene/scenedata.h"
#include "core/shape/mesh.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using std::min;
using std::max;

#ifdef DYNAMIC_HASHTABLE
extern unsigned short *hashTable; // GLOBAL VARIABLE
#else
alignas(16) extern unsigned short hashTable[]; // GLOBAL VARIABLE
#endif

const int Grid1Size    = 4;
const int HexGrid2Size = 7;
const int HexGrid3Size = 19;
const int HexGrid4Size = 37;

// Grid size (n x n) used while jittering focal blur sub-pixel position.
const int SUB_PIXEL_GRID_SIZE = 16;

static const Vector2d Grid1[Grid1Size] =
{
    Vector2d(-0.25,  0.25),
    Vector2d( 0.25,  0.25),
    Vector2d(-0.25, -0.25),
    Vector2d( 0.25, -0.25)
};

static const DBL HexJitter2 = 0.144338;
static const int HexGrid2Samples[2] = { 7, 0 };
static const Vector2d HexGrid2[HexGrid2Size] =
{
    Vector2d(-0.288675,  0.000000),
    Vector2d( 0.000000,  0.000000),
    Vector2d( 0.288675,  0.000000),
    Vector2d(-0.144338,  0.250000),
    Vector2d(-0.144338, -0.250000),
    Vector2d( 0.144338,  0.250000),
    Vector2d( 0.144338, -0.250000)
};

static const DBL HexJitter3 = 0.096225;
static const int HexGrid3Samples[4] = { 7, 6, 6, 0 };
static const Vector2d HexGrid3[HexGrid3Size] =
{
    Vector2d(-0.192450,  0.333333),
    Vector2d(-0.192450, -0.333333),
    Vector2d( 0.192450,  0.333333),
    Vector2d( 0.192450, -0.333333),
    Vector2d( 0.384900,  0.000000),
    Vector2d(-0.384900,  0.000000),
    Vector2d( 0.000000,  0.000000),

    Vector2d( 0.000000,  0.333333),
    Vector2d( 0.000000, -0.333333),
    Vector2d(-0.288675,  0.166667),
    Vector2d(-0.288675, -0.166667),
    Vector2d( 0.288675,  0.166667),
    Vector2d( 0.288675, -0.166667),

    Vector2d(-0.096225,  0.166667),
    Vector2d(-0.096225, -0.166667),
    Vector2d( 0.096225,  0.166667),
    Vector2d( 0.096225, -0.166667),
    Vector2d(-0.192450,  0.000000),
    Vector2d( 0.192450,  0.000000)
};

static const DBL HexJitter4 = 0.0721688;
static const int HexGrid4Samples[9] = { 7, 6, 6, 4, 4, 4, 4, 2, 0 };
static const Vector2d HexGrid4[HexGrid4Size] =
{
    Vector2d( 0.000000,  0.000000),
    Vector2d(-0.216506,  0.375000),
    Vector2d( 0.216506, -0.375000),
    Vector2d(-0.216506, -0.375000),
    Vector2d( 0.216506,  0.375000),
    Vector2d(-0.433013,  0.000000),
    Vector2d( 0.433013,  0.000000),

    Vector2d(-0.144338,  0.250000),
    Vector2d( 0.144338, -0.250000),
    Vector2d(-0.144338, -0.250000),
    Vector2d( 0.144338,  0.250000),
    Vector2d(-0.288675,  0.000000),
    Vector2d( 0.288675,  0.000000),

    Vector2d(-0.072169,  0.125000),
    Vector2d( 0.072169, -0.125000),
    Vector2d(-0.072169, -0.125000),
    Vector2d( 0.072169,  0.125000),
    Vector2d(-0.144338,  0.000000),
    Vector2d( 0.144338,  0.000000),

    Vector2d(-0.360844,  0.125000),
    Vector2d(-0.360844, -0.125000),
    Vector2d( 0.360844,  0.125000),
    Vector2d( 0.360844, -0.125000),

    Vector2d(-0.288675,  0.250000),
    Vector2d(-0.288675, -0.250000),
    Vector2d( 0.288675,  0.250000),
    Vector2d( 0.288675, -0.250000),

    Vector2d(-0.072169,  0.375000),
    Vector2d(-0.072169, -0.375000),
    Vector2d( 0.072169,  0.375000),
    Vector2d( 0.072169, -0.375000),

    Vector2d(-0.216506,  0.125000),
    Vector2d(-0.216506, -0.125000),
    Vector2d( 0.216506,  0.125000),
    Vector2d( 0.216506, -0.125000),

    Vector2d( 0.000000,  0.250000),
    Vector2d( 0.000000, -0.250000),
};

inline int PseudoRandom(int v)
{
    return int(hashTable[int(v & 0x0fff)]);
}


bool HasInteriorPointObjectCondition::operator()(const Vector3d& point, ConstObjectPtr object) const
{
    return object->interior != nullptr;
}

bool ContainingInteriorsPointObjectCondition::operator()(const Vector3d& point, ConstObjectPtr object) const
{
    containingInteriors.push_back(object->interior.get());
    return true;
}


TracePixel::TracePixel(std::shared_ptr<SceneData> sd, const Camera* cam, TraceThreadData *td, unsigned int mtl, DBL adcb, const QualityFlags& qf,
                       CooperateFunctor& cf, MediaFunctor& mf, RadiosityFunctor& af, bool pt) :
                       Trace(sd, td, qf, cf, mf, af),
                       sceneData(sd),
                       threadData(td),
                       focalBlurData(nullptr),
                       maxTraceLevel(mtl),
                       adcBailout(adcb),
                       pretrace(pt)
{
    for (unsigned int i = 0; i < 3; ++i)
    {
        mpCameraLocationFn[i] = nullptr;
        mpCameraDirectionFn[i] = nullptr;
    }
    SetupCamera((cam == nullptr) ? sd->parsedCamera : *cam);
}

TracePixel::~TracePixel()
{
    if (focalBlurData != nullptr)
        delete focalBlurData;
    for (unsigned int i = 0; i < 3; ++i)
    {
        if (mpCameraLocationFn[i] != nullptr)
            delete mpCameraLocationFn[i];
        if (mpCameraDirectionFn[i] != nullptr)
            delete mpCameraDirectionFn[i];
    }
}

void TracePixel::SetupCamera(const Camera& cam)
{
    bool normalise = false;
    camera = cam;
    useFocalBlur = false;
    precomputeContainingInteriors = true;
    cameraDirection = camera.Direction;
    cameraRight = camera.Right;
    cameraUp =  camera.Up;
    cameraLocation =  camera.Location;
    aspectRatio = 4.0 / 3.0;
    cameraLengthRight = cameraRight.length();
    cameraLengthUp = cameraUp.length();

    switch(camera.Type)
    {
        case CYL_1_CAMERA:
        case CYL_3_CAMERA:
            aspectRatio = cameraLengthUp;
            normalise = true;
            break;
        case CYL_2_CAMERA:
        case CYL_4_CAMERA:
            aspectRatio = cameraLengthRight;
            normalise = true;
            break;
        case ULTRA_WIDE_ANGLE_CAMERA:
            aspectRatio = cameraLengthUp / cameraLengthRight;
            normalise = true;
            break;
        case OMNIMAX_CAMERA:
        case FISHEYE_CAMERA:
            aspectRatio = cameraLengthRight / cameraLengthUp;
            normalise = true;
            break;
        case USER_DEFINED_CAMERA:
            normalise = true;
            for (unsigned int i = 0; i < 3; ++i)
            {
                if (mpCameraLocationFn[i] != nullptr)
                    delete mpCameraLocationFn[i];
                if (camera.Location_Fn[i] != nullptr)
                    mpCameraLocationFn[i] = new GenericScalarFunctionInstance(camera.Location_Fn[i], threadData);
                if (mpCameraDirectionFn[i] != nullptr)
                    delete mpCameraDirectionFn[i];
                if (camera.Direction_Fn[i] != nullptr)
                    mpCameraDirectionFn[i] = new GenericScalarFunctionInstance(camera.Direction_Fn[i], threadData);
            }
            break;
        default:
            aspectRatio = cameraLengthRight / cameraLengthUp;
            break;
    }

    if(normalise == true)
    {
        cameraRight.normalize();
        cameraUp.normalize();
        cameraDirection.normalize();
    }

    if (focalBlurData != nullptr)
    {
        delete focalBlurData;
        focalBlurData = nullptr;
    }

    // TODO: there is little point in calculating the grid separately for each thread.
    // since all threads in a given render must have identical grids, we should calculate
    // it once, and then duplicate the calculated data when starting up the threads.
    // (Possibly we could store it in the view).
    useFocalBlur = ((camera.Aperture != 0.0) && (camera.Blur_Samples > 0));
    if(useFocalBlur == true)
        focalBlurData = new FocalBlurData(camera, threadData);
}

void TracePixel::operator()(DBL x, DBL y, DBL width, DBL height, RGBTColour& colour)
{
    if(useFocalBlur == false)
    {
        colour.Clear();
        int numTraced = 0;
        for (size_t rayno = 0; rayno < camera.Rays_Per_Pixel; rayno++)
        {
            TraceTicket ticket(maxTraceLevel, adcBailout, sceneData->outputAlpha);
            Ray ray(ticket);

            if (CreateCameraRay(ray, x, y, width, height, rayno) == true)
            {
                MathColour col;
                ColourChannel transm = 0.0;

                TraceRay(ray, col, transm, 1.0, false, camera.Max_Ray_Distance);
                colour += RGBTColour(ToRGBColour(col), transm);
                numTraced++;
            }
        }
        if (numTraced)
            colour /= (DBL) numTraced;
        else
            colour.transm() = 1.0;
    }
    else
        TraceRayWithFocalBlur(colour, x, y, width, height);
}

bool TracePixel::CreateCameraRay(Ray& ray, DBL x, DBL y, DBL width, DBL height, size_t ray_number)
{
    DBL x0 = 0.0, y0 = 0.0;
    DBL cx, sx, cy, sy, ty, rad, phi;
    Vector3d V1;
    TRANSFORM Trans;

    // Set ray flags
    ray.SetFlags(Ray::PrimaryRay, false, false, false, false, pretrace);

    // Create primary ray according to the camera used.
    ray.Origin = cameraLocation;

    switch(camera.Type)
    {
        // Perspective projection (Pinhole camera; POV standard).
        case PERSPECTIVE_CAMERA:
            // Convert the x coordinate to be a DBL from -0.5 to 0.5.
            x0 = x / width - 0.5;

            // Convert the y coordinate to be a DBL from -0.5 to 0.5.
            y0 = 0.5 - y / height;

            // Create primary ray.
            ray.Direction = cameraDirection + x0 * cameraRight + y0 * cameraUp;

            // Do focal blurring (by Dan Farmer).
            if(useFocalBlur)
                JitterCameraRay(ray, x, y, ray_number);

            InitRayContainerState(ray, useFocalBlur);
            break;

        // Orthographic projection.
        case ORTHOGRAPHIC_CAMERA:
            // Convert the x coordinate to be a DBL from -0.5 to 0.5.
            x0 = x / width - 0.5;

            // Convert the y coordinate to be a DBL from -0.5 to 0.5.
            y0 = 0.5 - y / height;

            // Create primary ray.
            ray.Direction = cameraDirection;

            ray.Origin = cameraLocation + x0 * cameraRight + y0 * cameraUp;

            if(useFocalBlur)
                JitterCameraRay(ray, x, y, ray_number);

            InitRayContainerState(ray, true);
            break;

        // Fisheye camera.
        case FISHEYE_CAMERA:
            // Convert the x coordinate to be a DBL from -1.0 to 1.0.
            x0 = 2.0 * (x / width - 0.5);

            // Convert the y coordinate to be a DBL from -1.0 to 1.0.
            y0 = 2.0 * (0.5 - y / height);

            // This code would do what Warp wants
            x0 *= cameraLengthRight;
            y0 *= cameraLengthUp;

            rad = sqrt(x0 * x0 + y0 * y0);

            // If the pixel lies outside the unit circle no ray is traced.

            if(rad > 1.0)
                return false;

            if(rad == 0.0)
                phi = 0.0;
            else if(x0 < 0.0)
                phi = M_PI - asin(y0 / rad);
            else
                phi = asin(y0 / rad);

            // Get spherical coordinates.
            x0 = phi;

            // Set vertical angle to half viewing angle.
            y0 = rad * camera.Angle * M_PI_360;

            // Create primary ray.
            cx = cos(x0);  sx = sin(x0);
            cy = cos(y0);  sy = sin(y0);

            ray.Direction = (cx * sy) * cameraRight + (sx * sy) * cameraUp + cy * cameraDirection;

            if(useFocalBlur)
                JitterCameraRay(ray, x, y, ray_number);

            InitRayContainerState(ray, useFocalBlur);
            break;

        // Omnimax camera.
        case OMNIMAX_CAMERA:
            // Convert the x coordinate to be a DBL from -1.0 to 1.0.
            x0 = 2.0 * (x / width - 0.5);

            // Convert the y coordinate to be a DBL from -1.0 to 1.0.
            y0 = 2.0 * (0.5 - y / height);

            // Get polar coordinates.
            if(aspectRatio > 1.0)
            {
                if(aspectRatio > 1.283458)
                {
                    x0 *= aspectRatio/1.283458;
                    y0 = (y0-1.0)/1.283458 + 1.0;
                }
                else
                    y0 = (y0-1.0)/aspectRatio + 1.0;
            }
            else
                y0 /= aspectRatio;

            rad = sqrt(x0 * x0 + y0 * y0);

            // If the pixel lies outside the unit circle no ray is traced.

            if(rad > 1.0)
                return false;

            if(rad == 0.0)
                phi = 0.0;
            else if (x0 < 0.0)
                phi = M_PI - asin(y0 / rad);
            else
                phi = asin(y0 / rad);

            // Get spherical coordinates.
            x0 = phi;

            y0 = 1.411269 * rad - 0.09439 * rad * rad * rad + 0.25674 * rad * rad * rad * rad * rad;

            cx = cos(x0);  sx = sin(x0);
            cy = cos(y0);  sy = sin(y0);

            // We can't see below 45 degrees under the projection axis.
            if (sx * sy < tan(135.0 * M_PI_180) * cy)
                return false;

            ray.Direction = (cx * sy) * cameraRight + (sx * sy) * cameraUp + cy * cameraDirection;

            if(useFocalBlur)
                JitterCameraRay(ray, x, y, ray_number);

            InitRayContainerState(ray, useFocalBlur);
            break;

        // Panoramic camera from Graphic Gems III.
        case PANORAMIC_CAMERA:
            // Convert the x coordinate to be a DBL from 0.0 to 1.0.
            x0 = x / width;

            // Convert the y coordinate to be a DBL from -1.0 to 1.0.
            y0 = 2.0 * (0.5 - y / height);

            // Get cylindrical coordinates.
            x0 = (1.0 - x0) * M_PI;
            y0 = M_PI_2 * y0;

            cx = cos(x0);
            sx = sin(x0);

            if(fabs(M_PI_2 - fabs(y0)) < EPSILON)
            {
                if (y0 > 0.0)
                    ty = BOUND_HUGE;
                else
                    ty = - BOUND_HUGE;
            }
            else
                ty = tan(y0);

            // Create primary ray.
            ray.Direction = cx * cameraRight + ty * cameraUp + sx * cameraDirection;

            if(useFocalBlur)
                JitterCameraRay(ray, x, y, ray_number);

            InitRayContainerState(ray, useFocalBlur);
            break;

        // Ultra wide angle camera written by Dan Farmer.
        case ULTRA_WIDE_ANGLE_CAMERA:
            // Convert the x coordinate to be a DBL from -0.5 to 0.5.
            x0 = x / width - 0.5;

            // Convert the y coordinate to be a DBL from -0.5 to 0.5.
            y0 = 0.5 - y / height;

            // Create primary ray.
            x0 *= camera.Angle * M_PI_180; // NK 1998 - changed to M_PI_180
            // 1999 July 10 Bugfix - as per suggestion of Gerald K. Dobiasovsky
            // added aspectRatio
            y0 *= camera.Angle * aspectRatio * M_PI_180; // NK 1998 - changed to M_PI_180

            cx = cos(x0);  sx = sin(x0);
            cy = cos(y0);  sy = sin(y0);

            ray.Direction = sx * cameraRight + sy * cameraUp + (cx * cy) * cameraDirection;

            if(useFocalBlur)
                JitterCameraRay(ray, x, y, ray_number);

            InitRayContainerState(ray, useFocalBlur);
            break;

        // Cylinder camera 1. Axis in "up" direction
        case CYL_1_CAMERA:
            // Convert the x coordinate to be a DBL from -0.5 to 0.5.
            x0 = x / width - 0.5;

            // Convert the y coordinate to be a DBL from -0.5 to 0.5.
            y0 = 0.5 - y / height;

            // Create primary ray.
            x0 *= camera.Angle * M_PI_180;
            y0 *= aspectRatio;

            cx = cos(x0);
            sx = sin(x0);

            ray.Direction = sx * cameraRight + y0 * cameraUp + cx * cameraDirection;

            if(useFocalBlur)
                JitterCameraRay(ray, x, y, ray_number);

            InitRayContainerState(ray, useFocalBlur);
            break;

        // Cylinder camera 2. Axis in "right" direction
        case CYL_2_CAMERA:
            // Convert the x coordinate to be a DBL from -0.5 to 0.5.
            x0 = x / width - 0.5;

            // Convert the y coordinate to be a DBL from -0.5 to 0.5.
            y0 = 0.5 - y / height;

            y0 *= camera.Angle * M_PI_180;
            x0 *= aspectRatio;

            cy = cos(y0);
            sy = sin(y0);

            ray.Direction = x0 * cameraRight + sy * cameraUp + cy * cameraDirection;

            if(useFocalBlur)
                JitterCameraRay(ray, x, y, ray_number);

            InitRayContainerState(ray, useFocalBlur);
            break;

        // Cylinder camera 3. Axis in "up" direction, orthogonal in "right"
        case CYL_3_CAMERA:
            // Convert the x coordinate to be a DBL from -0.5 to 0.5.
            x0 = x / width - 0.5;

            // Convert the y coordinate to be a DBL from -0.5 to 0.5.
            y0 = 0.5 - y / height;

            // Create primary ray.
            x0 *= camera.Angle * M_PI_180;
            y0 *= aspectRatio;

            cx = cos(x0);
            sx = sin(x0);

            ray.Direction = sx * cameraRight + cx * cameraDirection;

            ray.Origin = cameraLocation + y0 * cameraUp;

            if(useFocalBlur)
                JitterCameraRay(ray, x, y, ray_number);

            InitRayContainerState(ray, true);
            break;

        // Cylinder camera 4. Axis in "right" direction, orthogonal in "up"
        case CYL_4_CAMERA:
            // Convert the x coordinate to be a DBL from -0.5 to 0.5.
            x0 = x / width - 0.5;

            // Convert the y coordinate to be a DBL from -0.5 to 0.5.
            y0 = 0.5 - y / height;

            // Create primary ray.
            y0 *= camera.Angle * M_PI_180;
            x0 *= aspectRatio;

            cy = cos(y0);
            sy = sin(y0);

            ray.Direction = sy * cameraUp + cy * cameraDirection;

            ray.Origin = cameraLocation + x0 * cameraRight;

            if(useFocalBlur)
                JitterCameraRay(ray, x, y, ray_number);

            InitRayContainerState(ray, true);
            break;

        // spherical camera: x is horizontal, y vertical, V_Angle - vertical FOV, H_Angle - horizontal FOV
        case SPHERICAL_CAMERA:
            // Convert the x coordinate to be a DBL from -0.5 to 0.5.
            x0 = x / width - 0.5;

            // Convert the y coordinate to be a DBL from -0.5 to 0.5.
            y0 = 0.5 - y / height;

            // get angle in radians
            y0 *= (camera.V_Angle / 360) * TWO_M_PI;
            x0 *= (camera.H_Angle / 360) * TWO_M_PI;

            // find latitude for y in 3D space
            Compute_Axis_Rotation_Transform(&Trans, cameraRight, -y0);
            MTransPoint (V1, cameraDirection, &Trans);

            // Now take V1 and find longitude based on x
            Compute_Axis_Rotation_Transform(&Trans, cameraUp, x0);

            // Create primary ray.
            MTransPoint(ray.Direction, V1, &Trans);

            if(useFocalBlur)
                JitterCameraRay(ray, x, y, ray_number);

            InitRayContainerState(ray, useFocalBlur);
            break;

        case MESH_CAMERA:
            // in the case of the mesh camera, we don't want any pixel co-ordinates that are outside
            // the logical image boundaries (and particularly no negative ones), so we clip them here.
            if (camera.Face_Distribution_Method != 3)
            {
                x = max(0.0, min(floor(x), width - 1.0));
                y = max(0.0, min(floor(y), height - 1.0));
            }

            // Note: while it does not make sense to use AA with distribution methods 0, 1, or 2, we don't prohibit it.
            // The same goes for jitter and a few other non-mesh-camera specific effects. This is primarily because
            // these methods convert the X and Y positions to indexes, and in doing so first converts them to integers;
            // hence any sub-pixel positioning information gets lost.
            if (camera.Face_Distribution_Method == 0)
            {
                // this is single or multiple rays per pixel, with each additional ray going to the next mesh
                // in the sequence in which they were declared. we already know there is at least as many meshes
                // as needed, so we don't check it here.
                const Mesh *mesh = static_cast<const Mesh *>(camera.Meshes[ray_number]);
                unsigned int numFaces = mesh->Data->Number_Of_Triangles;
                unsigned int faceIndex = ((unsigned int) y * (unsigned int) width + (unsigned int) x);

                // if it's outside the mesh, don't trace the ray.
                if (faceIndex >= numFaces)
                    return false;

                // set the ray origin to the centriod of the triangle.
                const Mesh_Triangle_Struct& tr = mesh->Data->Triangles[faceIndex];
                ray.Origin = Vector3d(mesh->Data->Vertices[tr.P1] + mesh->Data->Vertices[tr.P2] + mesh->Data->Vertices[tr.P3]) / 3;

                // set the ray direction according to the normal of the face
                ray.Direction = Vector3d(mesh->Data->Normals[tr.Normal_Ind]);

                // we use the Z co-ordinate of the camera location to indicate how far, along
                // the ray's direction, we should move the ray's origin point. this allows the
                // ray origin to be set slightly above the face, for example.
                ray.Origin = ray.Evaluate(camera.Location[Z]);

                // we use the Z component of the camera direction to indicate whether or not
                // we should invert the ray direction. if the Z component is less than 0 (or
                // actually -EPSILON), then we shoot the ray in the opposite direction.
                if (camera.Direction[Z] < -EPSILON)
                    ray.Direction.invert();

                // apply any transformations needed
                if (mesh->Trans)
                {
                    MTransPoint (ray.Origin, ray.Origin, mesh->Trans);
                    MTransNormal (ray.Direction, ray.Direction, mesh->Trans);
                }

                // we're done
                InitRayContainerState(ray, true);
            }
            else if (camera.Face_Distribution_Method == 1)
            {
                // this is 1:1 distribution across the summed meshes, potentially with multiple rays per pixel
                unsigned int numPixels = width * height;
                unsigned int numFaces = *camera.Mesh_Index.rbegin();
                unsigned int faceIndex = ((unsigned int) y * (unsigned int) width + (unsigned int) x);
                unsigned int lastOffset = 0;
                unsigned int meshNo;

                // for distribution method 1, we take the origin for e.g. pixel 3, ray #3 (ray_number == 2) from
                // the face at (width * height) * 2 + 3. i.e. we add width * height * ray_number to the calculated
                // index. this allows pixels to be calculated using the summed result of multiple faces.
                faceIndex += ray_number * numFaces;

                // if the face index falls outside the number of faces, return false so the pixel will not be traced.
                // note that this is not the same as tracing a black pixel, since TracePixel will take into account
                // the fact the ray was not shot and takes that into account when dividing the summed color.
                if (faceIndex >= numFaces)
                    return false;

                // find the mesh that this face falls within
                for (meshNo = 0; meshNo < camera.Mesh_Index.size(); lastOffset = camera.Mesh_Index[meshNo++])
                {
                    if (camera.Mesh_Index[meshNo] > faceIndex)
                    {
                        faceIndex -= lastOffset;
                        const Mesh *mesh = static_cast<const Mesh *>(camera.Meshes[meshNo]);
                        Mesh_Triangle_Struct& tr = mesh->Data->Triangles[faceIndex];

                        // see comments for distribution method 0
                        ray.Origin = Vector3d(mesh->Data->Vertices[tr.P1] + mesh->Data->Vertices[tr.P2] + mesh->Data->Vertices[tr.P3]) / 3;
                        ray.Direction = Vector3d(mesh->Data->Normals[tr.Normal_Ind]);
                        ray.Origin = ray.Evaluate(camera.Location[Z]);
                        if (camera.Direction[Z] < -EPSILON)
                            ray.Direction.invert();
                        if (mesh->Trans)
                        {
                            MTransPoint (ray.Origin, ray.Origin, mesh->Trans);
                            MTransNormal (ray.Direction, ray.Direction, mesh->Trans);
                        }
                        InitRayContainerState(ray, true);
                        break;
                    }
                }
                if (meshNo == camera.Mesh_Index.size())
                    return false;
            }
            else if (camera.Face_Distribution_Method == 2)
            {
                // this is multiple logical cameras, placed side-by-size horizontally
                // currently, we ignore rays per pixel for this camera sub-type, and furthermore, we don't
                // sum the meshes: mesh #0 is the left-most camera, and mesh #n is the right-most (we don't
                // really care if the render width is not a multiple of the number of meshes).
                unsigned int meshNo = (unsigned int) (x / (width / camera.Meshes.size()));

                const Mesh *mesh = static_cast<const Mesh *>(camera.Meshes[meshNo]);
                unsigned int numFaces = mesh->Data->Number_Of_Triangles;
                unsigned int faceIndex = ((unsigned int) y * (unsigned int) ((unsigned int) width / camera.Meshes.size()) + (unsigned int) x);

                // if it's outside the mesh, don't trace the ray.
                if (faceIndex >= numFaces)
                    return false;

                // see comments for distribution method 0
                Mesh_Triangle_Struct& tr = mesh->Data->Triangles[faceIndex];
                ray.Origin = Vector3d(mesh->Data->Vertices[tr.P1] + mesh->Data->Vertices[tr.P2] + mesh->Data->Vertices[tr.P3]) / 3;
                ray.Direction = Vector3d(mesh->Data->Normals[tr.Normal_Ind]);
                ray.Origin = ray.Evaluate(camera.Location[Z]);
                if (camera.Direction[Z] < -EPSILON)
                    ray.Direction.invert();
                if (mesh->Trans)
                {
                    MTransPoint (ray.Origin, ray.Origin, mesh->Trans);
                    MTransNormal (ray.Direction, ray.Direction, mesh->Trans);
                }
                InitRayContainerState(ray, true);
            }
            else if (camera.Face_Distribution_Method == 3)
            {
                // this is for texture baking: we need to use the UV co-ordinates to position the camera.
                // it can also be used for non-baking purposes of course; e.g. a mesh camera where the
                // number of faces does not equal the number of pixels. in that case, the UV map would
                // presumably have been constructed to scale the pixels evenly across all the faces.

                // convert X and Y into UV co-ordinates
                double u = x / width;
                double v = 1.0 - y / height;

                // now we need to find the first face that that those co-ordinates fall within.
                // NB while it is of course possible for multiple faces to match a single UV co-ordinate,
                // we don't need to care about that as in this case we are seeking the color of the pixel
                // in the UV map, rather than the other way around. Hence, the first match is good enough.
                bool found = false;
                const Mesh *mesh = static_cast<const Mesh *>(camera.Meshes[0]);
                unsigned int count = (mesh->Data->Number_Of_Triangles + 31) / 32;

                // a face potentially has the given UV co-ordinate if it is in both the U column and V column
                unsigned int *up = &camera.U_Xref[min((unsigned int) (u * 10), 9U)][0];
                unsigned int *vp = &camera.V_Xref[min((unsigned int) (v * 10), 9U)][0];

                for (unsigned int idx = 0, intersection = *vp & *up; idx < count && found == false; idx++, intersection = *++vp & *++up)
                {
                    if (intersection != 0)
                    {
                        // there is at least one face that falls within the intersection of the two columns
                        for (unsigned int bit = 0, mask = 1; bit < 32 && found == false; bit++, mask <<= 1)
                        {
                            if ((intersection & mask) != 0)
                            {
                                const Mesh_Triangle_Struct *tr(mesh->Data->Triangles + idx * 32 + bit);
                                const double& P1u(mesh->Data->UVCoords[tr->UV1][U]);
                                const double& P2u(mesh->Data->UVCoords[tr->UV2][U]);
                                const double& P3u(mesh->Data->UVCoords[tr->UV3][U]);
                                const double& P1v(mesh->Data->UVCoords[tr->UV1][V]);
                                const double& P2v(mesh->Data->UVCoords[tr->UV2][V]);
                                const double& P3v(mesh->Data->UVCoords[tr->UV3][V]);

                                // derive the barycentric co-ordinates from the UV co-ords
                                double scale = (P2u - P1u) * (P3v - P1v) - (P3u - P1u) * (P2v - P1v);
                                double B1 = ((P2u - u) * (P3v - v) - (P3u - u) * (P2v - v)) / scale;
                                double B2 = ((P3u - u) * (P1v - v) - (P1u - u) * (P3v - v)) / scale;
                                double B3 = ((P1u - u) * (P2v - v) - (P2u - u) * (P1v - v)) / scale;

                                // if it's not within the triangle, we try the next one
                                if (B1 < 0 || B2 < 0 || B3 < 0)
                                    continue;

                                // now all we need to do is convert the barycentric co-ordinates back to a point in 3d space which is on the surface of the face
                                ray.Origin = Vector3d(mesh->Data->Vertices[tr->P1] * B1 + mesh->Data->Vertices[tr->P2] * B2 + mesh->Data->Vertices[tr->P3] * B3);

                                // we use the one normal for any location on the face, unless smooth is set
                                ray.Direction = Vector3d(mesh->Data->Normals[tr->Normal_Ind]);
                                if (camera.Smooth)
                                    mesh->Smooth_Mesh_Normal(ray.Direction, tr, ray.Origin);

                                found = true;
                                break;
                            }
                        }
                    }
                }
                if (!found)
                    return false;
                ray.Origin = ray.Evaluate(camera.Location[Z]);
                if (camera.Direction[Z] < -EPSILON)
                    ray.Direction.invert();
                if (mesh->Trans)
                {
                    MTransPoint (ray.Origin, ray.Origin, mesh->Trans);
                    MTransNormal (ray.Direction, ray.Direction, mesh->Trans);
                }
                InitRayContainerState(ray, true);
            }
            break;

        case USER_DEFINED_CAMERA:
            // Convert the x coordinate to be a DBL from -0.5 to 0.5.
            x0 = x / width - 0.5;

            // Convert the y coordinate to be a DBL from -0.5 to 0.5.
            y0 = 0.5 - y / height;

            for (unsigned int i = 0; i < 3; ++i)
            {
                if (camera.Location_Fn[i] != nullptr)
                    cameraLocation[i] = mpCameraLocationFn[i]->Evaluate(x0, y0);
                if (!IsFinite(cameraLocation[i]))
                    return false;
                if (camera.Direction_Fn[i] != nullptr)
                    cameraDirection[i] = mpCameraDirectionFn[i]->Evaluate(x0, y0);
                if (!IsFinite(cameraDirection[i]))
                    return false;
            }
            if (cameraDirection.IsNearNull(EPSILON))
                return false;
            ray.Origin    = cameraLocation;
            ray.Direction = cameraDirection;

            if(useFocalBlur)
                JitterCameraRay(ray, x, y, ray_number);

            InitRayContainerState(ray, true);
            break;

        default:
            throw POV_EXCEPTION_STRING("Unknown camera type in CreateCameraRay().");
    }

    if (camera.Tnormal != nullptr)
    {
        ray.Direction.normalize();
        V1 = Vector3d(x0, y0, 0.0);
        Perturb_Normal(ray.Direction, camera.Tnormal, V1, nullptr, nullptr, threadData);
    }

    ray.Direction.normalize();

    return true;
}

void TracePixel::InitRayContainerState(Ray& ray, bool compute)
{
    if((compute == true) || (precomputeContainingInteriors == true)) // TODO - check this logic, in particular that of compute!
    {
        precomputeContainingInteriors = false;
        containingInteriors.clear();

        if(sceneData->boundingMethod == 2)
        {
            HasInteriorPointObjectCondition precond;
            ContainingInteriorsPointObjectCondition postcond(containingInteriors);
            BSPInsideCondFunctor ifn(ray.Origin, sceneData->objects, threadData, precond, postcond);

            mailbox.clear();
            (*sceneData->tree)(ray.Origin, ifn, mailbox);

            // test infinite objects
            for(std::vector<ObjectPtr>::iterator object = sceneData->objects.begin() + sceneData->numberOfFiniteObjects; object != sceneData->objects.end(); object++)
                if (((*object)->interior != nullptr) && Inside_BBox(ray.Origin, (*object)->BBox) && (*object)->Inside(ray.Origin, threadData))
                    containingInteriors.push_back((*object)->interior.get());
        }
        else if ((sceneData->boundingMethod == 0) || (sceneData->boundingSlabs == nullptr))
        {
            for(std::vector<ObjectPtr>::iterator object = sceneData->objects.begin(); object != sceneData->objects.end(); object++)
                if (((*object)->interior != nullptr) && Inside_BBox(ray.Origin, (*object)->BBox) && (*object)->Inside(ray.Origin, threadData))
                    containingInteriors.push_back((*object)->interior.get());
        }
        else
        {
            InitRayContainerStateTree(ray, sceneData->boundingSlabs);
        }
    }

    ray.AppendInteriors(containingInteriors);
}

/*****************************************************************************
*
* METHOD
*
*   InitRayContainerStateTree
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Step down the bounding box hierarchy and test for all node wether
*   the ray's origin is inside or not. If it's inside a node descend
*   further. If a leaf is reached and the ray's origin is inside the
*   leaf object insert the objects data into the ray's containing lists.
*
* CHANGES
*
*   Mar 1996 : Creation.
*
******************************************************************************/

void TracePixel::InitRayContainerStateTree(Ray& ray, BBOX_TREE *node)
{
    /* Check current node. */
    if(!Inside_BBox(ray.Origin, node->BBox))
        return;
    if(node->Entries == 0)
    {
        /* This is a leaf so test contained object. */
        ObjectPtr object = ObjectPtr(node->Node);
        if ((object->interior != nullptr) && object->Inside(ray.Origin, threadData))
            containingInteriors.push_back(object->interior.get());
    }
    else
    {
        /* This is a node containing leaves to be checked. */
        for(int i = 0; i < node->Entries; i++)
            InitRayContainerStateTree(ray, node->Node[i]);
    }
}

void TracePixel::TraceRayWithFocalBlur(RGBTColour& colour, DBL x, DBL y, DBL width, DBL height)
{
    int nr;     // Number of current samples.
    int level;  // Index into number of samples list.
    int max_s;  // Number of samples to take before next confidence test.
    int dxi, dyi;
    int i;
    DBL dx, dy, n, randx, randy;
    RGBTColour C, V1, S1, S2;
    int seed = int((x-0.5) * 313.0 + 11.0) + int((y-0.5) * 311.0 + 17.0);

    TraceTicket ticket(maxTraceLevel, adcBailout, sceneData->outputAlpha);
    Ray ray(ticket);

    colour.Clear();
    V1.Clear();
    S1.Clear();
    S2.Clear();

    nr = 0;
    level = 0;

    do
    {
        // Trace number of rays given by the list Current_Number_Of_Samples[].
        max_s = 4;

        if (focalBlurData->Current_Number_Of_Samples != nullptr)
        {
            if(focalBlurData->Current_Number_Of_Samples[level] > 0)
            {
                max_s = focalBlurData->Current_Number_Of_Samples[level];
                level++;
            }
        }

        for(i = 0; (i < max_s) && (nr < camera.Blur_Samples); i++)
        {
            // Choose sub-pixel location.
            dxi = PseudoRandom(seed + nr) % SUB_PIXEL_GRID_SIZE;
            dyi = PseudoRandom(seed + nr + 1) % SUB_PIXEL_GRID_SIZE;

            dx = (DBL)(2 * dxi + 1) / (DBL)(2 * SUB_PIXEL_GRID_SIZE) - 0.5;
            dy = (DBL)(2 * dyi + 1) / (DBL)(2 * SUB_PIXEL_GRID_SIZE) - 0.5;

            Jitter2d(dx, dy, randx, randy);

            // Add jitter to sub-pixel location.
            dx += (randx - 0.5) / (DBL)(SUB_PIXEL_GRID_SIZE);
            dy += (randy - 0.5) / (DBL)(SUB_PIXEL_GRID_SIZE);

            // remove interiors accumulated from previous iteration (if any)
            ray.ClearInteriors();

            // Create and trace ray.
            if(CreateCameraRay(ray, x + dx, y + dy, width, height, nr))
            {
                // Increase_Counter(stats[Number_Of_Samples]);

                MathColour tempC;
                ColourChannel tempT = 0.0;
                TraceRay(ray, tempC, tempT, 1.0, false, camera.Max_Ray_Distance);
                C = RGBTColour(ToRGBColour(tempC), tempT);

                colour += C;
            }
            else
                C = RGBTColour(0.0, 0.0, 0.0, 1.0);

            // Add color to color sum.

            S1 += C;

            // Add color to squared color sum.

            S2 += Sqr(C);

            nr++;
        }

        // Get variance of samples.

        n = (DBL)nr;

        V1 = (S2 / n - Sqr(S1 / n)) / n;

        // Exit if samples are likely too be good enough.

        if((nr >= camera.Blur_Samples_Min) &&
           (V1.IsNearZero(focalBlurData->Sample_Threshold[nr - 1])))
            break;
    }
    while(nr < camera.Blur_Samples);

    colour /= (DBL)nr;
}

void TracePixel::JitterCameraRay(Ray& ray, DBL x, DBL y, size_t ray_number)
{
    DBL xjit, yjit, xlen, ylen, r;
    Vector3d temp_xperp, temp_yperp, deflection;

    r = camera.Aperture * 0.5;

    Jitter2d(x, y, xjit, yjit);
    xjit *= focalBlurData->Max_Jitter * 2.0;
    yjit *= focalBlurData->Max_Jitter * 2.0;

    xlen = r * (focalBlurData->Sample_Grid[ray_number].x() + xjit);
    ylen = r * (focalBlurData->Sample_Grid[ray_number].y() + yjit);

    // Deflect the position of the eye by the size of the aperture, and in
    // a direction perpendicular to the current direction of view.

    temp_xperp = focalBlurData->XPerp * xlen;
    temp_yperp = focalBlurData->YPerp * ylen;

    deflection = temp_xperp - temp_yperp;

    ray.Origin += deflection;

    // Deflect the direction of the ray in the opposite direction we deflected
    // the eye position.  This makes sure that we are looking at the same place
    // when the distance from the eye is equal to "Focal_Distance".

    ray.Direction *= focalBlurData->Focal_Distance;
    ray.Direction -= deflection;

    ray.Direction.normalize();
}

TracePixel::FocalBlurData::FocalBlurData(const Camera& camera, TraceThreadData* threadData)
{
    // Create list of thresholds for confidence test.
    Sample_Threshold = new DBL[camera.Blur_Samples];
    if(camera.Blur_Samples > 1)
    {
        DBL T1 = camera.Variance / chdtri((DBL)(camera.Blur_Samples-1), camera.Confidence);
        for(int i = 0; i < camera.Blur_Samples; i++)
            Sample_Threshold[i] = T1 * chdtri((DBL)(i + 1), camera.Confidence);
    }
    else
        Sample_Threshold[0] = 0.0;

    // Create list of sample positions.
    Sample_Grid = new Vector2d[camera.Blur_Samples];

    if (camera.Bokeh)
    {
        Current_Number_Of_Samples = nullptr;
        Max_Jitter = 0.5 / sqrt((DBL)camera.Blur_Samples);

        double weightSum = 0.0;
        double weightMax = 0.0;
        size_t tries = 0; // safeguard against infinite loop
        double max_tries = Sqr((double)camera.Blur_Samples);

        SequentialVector2dGeneratorPtr vgen(GetSubRandom2dGenerator(0, -0.5, 0.5, -0.5, 0.5));
        SequentialDoubleGeneratorPtr randgen(GetRandomDoubleGenerator(0.0, 1.0));
        for (int i = 0; i < camera.Blur_Samples; i++)
        {
            Vector2d v;
            TransColour c;
            double weight;
            do
            {
                v = (*vgen)();
                Compute_Pigment(c, camera.Bokeh, Vector3d(v.x() + 0.5, v.y() + 0.5, 0.0), nullptr, nullptr, threadData);
                weight = c.colour().Greyscale();
                weightSum += weight;
                weightMax = max(weightMax, weight);
                weight += tries / max_tries; // safeguard against infinite loops
                tries++;
            }
            while ((*randgen)() > weight);

            Sample_Grid[i] = v;
        }

        double weightAvg = weightSum/tries;

        // TODO - generate a warning if weightMax > 1.0, or weightAvg particularly low
    }
    else
    {

        // Choose sample list and the best standard grid to use.

        // Default is 4x4 standard grid.
        const Vector2d *Standard_Sample_Grid = Grid1;
        int Standard_Sample_Grid_Size = 4;
        Current_Number_Of_Samples = nullptr;

        // Check for 37 samples hexgrid.
        if(camera.Blur_Samples >= HexGrid4Size)
        {
            Standard_Sample_Grid = HexGrid4;
            Standard_Sample_Grid_Size = HexGrid4Size;
            Current_Number_Of_Samples = HexGrid4Samples;
        }
        // Check for 19 samples hexgrid.
        else if(camera.Blur_Samples >= HexGrid3Size)
        {
            Standard_Sample_Grid = HexGrid3;
            Standard_Sample_Grid_Size = HexGrid3Size;
            Current_Number_Of_Samples = HexGrid3Samples;
        }
        // Check for 7 samples hexgrid.
        else if(camera.Blur_Samples >= HexGrid2Size)
        {
            Standard_Sample_Grid = HexGrid2;
            Standard_Sample_Grid_Size = HexGrid2Size;
            Current_Number_Of_Samples = HexGrid2Samples;
        }

        // Get max. jitter.
        switch(camera.Blur_Samples)
        {
            case HexGrid2Size:
                Max_Jitter = HexJitter2;
                break;
            case HexGrid3Size:
                Max_Jitter = HexJitter3;
                break;
            case HexGrid4Size:
                Max_Jitter = HexJitter4;
                break;
            default:
                Max_Jitter = 0.5 / sqrt((DBL)camera.Blur_Samples);
                break;
        }

        // Copy standard grid to sample grid.
        for(int i = 0; i < min(Standard_Sample_Grid_Size, camera.Blur_Samples); i++)
            Sample_Grid[i] = Standard_Sample_Grid[i];

        // Choose remaining samples from a uniform grid to get "best" coverage.
        if(camera.Blur_Samples > Standard_Sample_Grid_Size)
        {
            // Get sub-pixel grid size (I want it to be odd).
            double minGridRadius = sqrt(camera.Blur_Samples / M_PI);
            int Grid_Size = 2 * (int)ceil(minGridRadius) + 1;

            // Allocate temporary grid.
            std::unique_ptr<char[]> Grid_Data (new char [Grid_Size * Grid_Size]);
            char *p = Grid_Data.get();
            std::memset(p, 0, Grid_Size * Grid_Size);
            std::vector<char*> Grid(Grid_Size);
            for(int i = 0; i < Grid_Size; i++, p += Grid_Size)
                Grid[i] = p;

            // Mark sub-pixels already covered.
            for(int i = 0; i < Standard_Sample_Grid_Size; i++)
            {
                int xi = (int)((Sample_Grid[i].x() + 0.5) * (DBL)Grid_Size);
                int yi = (int)((Sample_Grid[i].y() + 0.5) * (DBL)Grid_Size);
                Grid[yi][xi]++;
            }

            size_t remain = camera.Blur_Samples * 10;
            SequentialVector2dGeneratorPtr randgen(GetSubRandomOnDiscGenerator(0, 0.5, remain));

            // Distribute remaining samples.
            for(int i = Standard_Sample_Grid_Size; i < camera.Blur_Samples; i++)
            {
                Vector2d v = (*randgen)();
                int xi = min((int)((v.x() + 0.5) * (DBL)Grid_Size), Grid_Size - 1);
                int yi = min((int)((v.y() + 0.5) * (DBL)Grid_Size), Grid_Size - 1);
                remain --;
                while ((Grid[yi][xi] || (v.lengthSqr() > 0.25)) && (remain > camera.Blur_Samples - i))
                {
                    v = (*randgen)();
                    xi = min((int)((v.x() + 0.5) * (DBL)Grid_Size), Grid_Size - 1);
                    yi = min((int)((v.y() + 0.5) * (DBL)Grid_Size), Grid_Size - 1);
                    remain --;
                }

                Sample_Grid[i] = v;

                Grid[yi][xi]++;
            }
        }
    }

    // Calculate vectors perpendicular to the optical axis
    // We're making a "+" (crosshair) on the film plane.

    // XPerp = vector perpendicular to y/z plane
    XPerp = cross(camera.Up, camera.Direction).normalized();

    // YPerp = vector perpendicular to x/z plane
    YPerp = cross(camera.Direction, XPerp).normalized();

    // Get adjusted distance to focal plane.
    DBL len;
    len = camera.Direction.length();
    Focal_Distance = camera.Focal_Distance / len;
}

TracePixel::FocalBlurData::~FocalBlurData()
{
    delete[] Sample_Grid;
    delete[] Sample_Threshold;
}

}
// end of namespace pov
