//******************************************************************************
///
/// @file core/scene/camera.h
///
/// Declarations related to cameras.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
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
class TracePixelCameraData;

//##############################################################################
///
/// @defgroup PovCoreSceneCamera Cameras
/// @ingroup PovCore
///
/// @{

/*****************************************************************************
* Global preprocessor defines
******************************************************************************/

// Available camera types. [DB 8/94]

enum CameraType
{
     PERSPECTIVE_CAMERA,
     ORTHOGRAPHIC_CAMERA,
     FISHEYE_CAMERA,
     ULTRA_WIDE_ANGLE_CAMERA,
     OMNIMAX_CAMERA,
     PANORAMIC_CAMERA,
     CYL_1_CAMERA,
     CYL_2_CAMERA,
     CYL_3_CAMERA,
     CYL_4_CAMERA,
     SPHERICAL_CAMERA,
     MESH_CAMERA,
     USER_DEFINED_CAMERA,
     PROJ_TETRA_CAMERA,
     PROJ_CUBE_CAMERA,
     PROJ_OCTA_CAMERA,
     PROJ_ICOSA_CAMERA,
     PROJ_MERCATOR_CAMERA,
     PROJ_LAMBERT_CYL_CAMERA,
     PROJ_BEHRMANN_CAMERA,
     PROJ_CRASTER_CAMERA,
     PROJ_EDWARDS_CAMERA,
     PROJ_HOBO_DYER_CAMERA,
     PROJ_PETERS_CAMERA,
     PROJ_GALL_CAMERA,
     PROJ_BALTHASART_CAMERA,
     PROJ_AITOFF_CAMERA,
     PROJ_MOLLWEIDE_CAMERA,
     PROJ_LAMBERT_AZI_CAMERA,
     PROJ_VAN_DER_GRINTEN_CAMERA,
     PROJ_PLATECARREE_CAMERA,
     PROJ_ECKERT4_CAMERA,
     PROJ_ECKERT6_CAMERA,
     PROJ_MILLER_CAMERA,
     STEREOSCOPIC_CAMERA,
     FISHEYE_ORTHOGRAPHIC_CAMERA,
     FISHEYE_EQUISOLIDANGLE_CAMERA,
     FISHEYE_STEREOGRAPHIC_CAMERA,
     OMNI_DIRECTIONAL_STEREO_CAMERA,
     GRID_CAMERA
};

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
    CameraType Type;                // Camera type.
    DBL Angle;                      // Viewing angle.
    DBL H_Angle;                    // Spherical horizontal viewing angle
    DBL V_Angle;                    // Spherical verticle viewing angle
    TNORMAL *Tnormal;               // Primary ray pertubation.
    TRANSFORM *Trans;               // Used only to record the user's input
    PIGMENT *Bokeh;                 // Pigment to use for the bokeh
    DBL Eye_Distance;               // for Stereoscopic camera
    DBL Parallaxe;                  // for Stereoscopic camera
    // the following declarations are used for the grid camera
    unsigned int GridSize[2];// division of picture in part
    vector<Camera> Cameras;// list of camera as sub-part, recursive
    mutable vector<TracePixelCameraData> TracePixels;

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

/// @}
///
//##############################################################################

}

#endif // POVRAY_CORE_CAMERA_H
