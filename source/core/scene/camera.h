//******************************************************************************
///
/// @file core/scene/camera.h
///
/// Declarations related to cameras.
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

#ifndef POVRAY_CORE_CAMERA_H
#define POVRAY_CORE_CAMERA_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include "core/math/vector.h"
#include "core/material/normal.h"

namespace pov
{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

// Available camera types. [DB 8/94]

#define PERSPECTIVE_CAMERA      1
#define ORTHOGRAPHIC_CAMERA     2
#define FISHEYE_CAMERA          3
#define ULTRA_WIDE_ANGLE_CAMERA 4
#define OMNIMAX_CAMERA          5
#define PANORAMIC_CAMERA        6
#define CYL_1_CAMERA            7
#define CYL_2_CAMERA            8
#define CYL_3_CAMERA            9
#define CYL_4_CAMERA           10
#define SPHERICAL_CAMERA       11
#define MESH_CAMERA            12
#define USER_DEFINED_CAMERA    13

/*****************************************************************************
* Global typedefs
******************************************************************************/

class Camera
{
public:
    Vector3d Location;
    Vector3d Direction;
    Vector3d Up;
    Vector3d Right;
    Vector3d Sky;
    Vector3d Look_At;               // Used only to record the user's preference
    Vector3d Focal_Point;           // Used only to record the user's preference
    DBL Focal_Distance, Aperture;   // ARE 9/92 for focal blur.
    int Blur_Samples;               // ARE 9/92 for focal blur.
    int Blur_Samples_Min;           // Minimum number of blur samples to take regardless of confidence settings.
    DBL Confidence;                 // Probability for confidence test.
    DBL Variance;                   // Max. variance for confidence test.
    int Type;                       // Camera type.
    DBL Angle;                      // Viewing angle.
    DBL H_Angle;                    // Spherical horizontal viewing angle
    DBL V_Angle;                    // Spherical verticle viewing angle
    TNORMAL *Tnormal;               // Primary ray pertubation.
    TRANSFORM *Trans;               // Used only to record the user's input
    PIGMENT *Bokeh;                 // Pigment to use for the bokeh
    GenericScalarFunctionPtr Location_Fn[3];  // [USER_DEFINED_CAMERA] Set of functions defining the ray's origin for each screen position.
    GenericScalarFunctionPtr Direction_Fn[3]; // [USER_DEFINED_CAMERA] Set of functions defining the ray's direction for each screen position.

    // the following declarations are used for the mesh camera
    unsigned int Face_Distribution_Method;  // how to associate a pixel to a face within a mesh
    unsigned int Rays_Per_Pixel;            // cast this many rays per pixel; never less than 1
    bool Smooth;                            // if true, interpolate normals for dist #3
    vector<ObjectPtr> Meshes;               // list of the meshes to be used as the camera
    vector<unsigned int> Mesh_Index;        // used with distribution #1 to keep track of accumulated meshes
    vector<unsigned int> U_Xref[10];        // used to speed up location of a matching face for distribution #3
    vector<unsigned int> V_Xref[10];        // used to speed up location of a matching face for distribution #3
    DBL Max_Ray_Distance;                   // if not 0.0, then maximum distance to look along the ray for an intersection
    // end of mesh camera declarations

    Camera();
    Camera(const Camera& src);
    ~Camera();
    Camera& operator=(const Camera& rhs);
    void Transform(const TRANSFORM *Trans);
    void Scale(const Vector3d& Vector);
    void Rotate(const Vector3d& Vector);
    void Translate(const Vector3d& Vector);

private:
    void Init();
};

}

#endif // POVRAY_CORE_CAMERA_H
