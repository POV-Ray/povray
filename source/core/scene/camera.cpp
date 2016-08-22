//******************************************************************************
///
/// @file core/scene/camera.cpp
///
/// Implementations related to cameras.
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
#include "core/scene/camera.h"

#include "core/material/normal.h"
#include "core/material/pigment.h"
#include "core/math/matrix.h"
#include "core/scene/object.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
*
* FUNCTION
*
*   Translate_Camera
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Camera::Translate(const Vector3d& Vector)
{
    Location += Vector;
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Camera
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Camera::Rotate(const Vector3d& Vector)
{
    TRANSFORM Trans;

    Compute_Rotation_Transform(&Trans, Vector);
    Transform(&Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Camera
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Camera::Scale(const Vector3d& Vector)
{
    TRANSFORM Trans;

    Compute_Scaling_Transform(&Trans, Vector);
    Transform(&Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Camera
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Camera::Transform(const TRANSFORM *Trans)
{
    MTransPoint(Location, Location, Trans);
    MTransDirection(Direction, Direction, Trans);
    MTransDirection(Up, Up, Trans);
    MTransDirection(Right, Right, Trans);
}



/*****************************************************************************
*
* METHOD
*
*   Camera::Init
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Camera::Init()
{
    Location    = Vector3d(0.0,  0.0, 0.0);
    Direction   = Vector3d(0.0,  0.0, 1.0);
    Up          = Vector3d(0.0,  1.0, 0.0);
    Right       = Vector3d(1.33, 0.0, 0.0); // TODO FIXME
    Sky         = Vector3d(0.0,  1.0, 0.0);
    Look_At     = Vector3d(0.0,  0.0, 1.0);
    Focal_Point = Vector3d(0.0,  0.0, 1.0);

    /* Init focal blur stuff (not used by default). */
    Blur_Samples        = 0;
    Blur_Samples_Min    = 0;
    Confidence          = 0.9;
    Variance            = 1.0 / 10000.0;
    Aperture            = 0.0;
    Focal_Distance      = -1.0;

    /* Set default camera type and viewing angle. [DB 7/94] */
    Type = PERSPECTIVE_CAMERA;
    Angle = 90.0;

    /* Default view angle for spherical camera. [MH 6/99] */
    H_Angle = 360;
    V_Angle = 180;

    /* Do not perturb primary rays by default. [DB 7/94] */
    Tnormal = NULL;

    Bokeh = NULL; // no user-defined bokeh by default

    Trans = Create_Transform();

    Rays_Per_Pixel = 1;
    Face_Distribution_Method = 0;
    Smooth = false;
    Max_Ray_Distance = 0.0;

    for (unsigned int i = 0; i < 3; ++i)
    {
        Location_Fn[i]  = NULL;
        Direction_Fn[i] = NULL;
    }
}

/*****************************************************************************
*
* FUNCTION
*
*   Create_Camera
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

Camera::Camera()
{
    Init();
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Camera
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

Camera& Camera::operator=(const Camera& src)
{
    Location    = src.Location;
    Direction   = src.Direction;
    Up          = src.Up;
    Right       = src.Right;
    Sky         = src.Sky;
    Look_At     = src.Look_At;
    Focal_Point = src.Focal_Point;

    Focal_Distance = src.Focal_Distance;
    Aperture = src.Aperture;
    Blur_Samples = src.Blur_Samples;
    Blur_Samples_Min = src.Blur_Samples_Min;
    Confidence = src.Confidence;
    Variance = src.Variance;
    Type = src.Type;
    Angle = src.Angle;
    H_Angle = src.H_Angle;
    V_Angle = src.V_Angle;

    if (Tnormal != NULL)
        Destroy_Tnormal(Tnormal);
    Tnormal = src.Tnormal ? Copy_Tnormal(src.Tnormal) : NULL;
    if (Trans != NULL)
        Destroy_Transform(Trans);
    Trans = src.Trans ? Copy_Transform(src.Trans) : NULL;

    if (Bokeh != NULL)
        Destroy_Pigment(Bokeh);
    Bokeh = src.Bokeh ? Copy_Pigment(src.Bokeh) : NULL;

    for (std::vector<ObjectPtr>::iterator it = Meshes.begin(); it != Meshes.end(); it++)
        Destroy_Object(*it);
    Meshes.clear();
    for (std::vector<ObjectPtr>::const_iterator it = src.Meshes.begin(); it != src.Meshes.end(); it++)
        Meshes.push_back(Copy_Object(*it));
    Face_Distribution_Method = src.Face_Distribution_Method;
    Rays_Per_Pixel = src.Rays_Per_Pixel;
    Max_Ray_Distance = src.Max_Ray_Distance;
    Mesh_Index = src.Mesh_Index;
    for (int i = 0; i < 10; i++)
    {
        U_Xref[i] = src.U_Xref[i];
        V_Xref[i] = src.V_Xref[i];
    }
    Smooth = src.Smooth;

    for (unsigned int i = 0; i < 3; ++i)
    {
        if (Location_Fn[i] != NULL)
            delete Location_Fn[i];
        if (src.Location_Fn[i] == NULL)
            Location_Fn[i] = NULL;
        else
            Location_Fn[i] = src.Location_Fn[i]->Clone();

        if (Direction_Fn[i] != NULL)
            delete Direction_Fn[i];
        if (src.Direction_Fn[i] == NULL)
            Direction_Fn[i] = NULL;
        else
            Direction_Fn[i] = src.Direction_Fn[i]->Clone();

    }

    return *this;
}

Camera::Camera(const Camera& src)
{
    Tnormal = NULL;
    Trans = NULL;
    Bokeh = NULL;
    for (unsigned int i = 0; i < 3; ++i)
    {
        Location_Fn[i]  = NULL;
        Direction_Fn[i] = NULL;
    }
    operator=(src);
}

/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Camera
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

Camera::~Camera()
{
    Destroy_Tnormal(Tnormal);
    Destroy_Transform(Trans);
    Destroy_Pigment(Bokeh);
    for (std::vector<ObjectPtr>::iterator it = Meshes.begin(); it != Meshes.end(); it++)
        Destroy_Object(*it);
    Meshes.clear();
    for (unsigned int i = 0; i < 3; ++i)
    {
        if (Location_Fn[i] != NULL)
            delete Location_Fn[i];
        if (Direction_Fn[i] != NULL)
            delete Direction_Fn[i];
    }
}

}
