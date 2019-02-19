//******************************************************************************
///
/// @file core/render/tracepixel.h
///
/// Declarations related to the basics of raytracing a pixel.
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

#ifndef POVRAY_CORE_TRACEPIXEL_H
#define POVRAY_CORE_TRACEPIXEL_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <memory>

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/render/trace.h"
#include "core/scene/camera.h"
#include "core/scene/scenedata.h"

namespace pov
{

//##############################################################################
///
/// @addtogroup PovCoreRender
/// @ingroup PovCore
///
/// @{

struct HasInteriorPointObjectCondition final : public PointObjectCondition
{
    virtual bool operator()(const Vector3d& point, ConstObjectPtr object) const override;
};

struct ContainingInteriorsPointObjectCondition final : public PointObjectCondition
{
    ContainingInteriorsPointObjectCondition(RayInteriorVector& ci) : containingInteriors(ci) {}
    virtual bool operator()(const Vector3d& point, ConstObjectPtr object) const override;
    RayInteriorVector &containingInteriors;
};

class TracePixel final : public Trace
{
    public:
        TracePixel(std::shared_ptr<SceneData> sd, const Camera* cam, TraceThreadData *td, unsigned int mtl, DBL adcb, const QualityFlags& qf,
                   CooperateFunctor& cf, MediaFunctor& mf, RadiosityFunctor& af, bool pt = false);
        virtual ~TracePixel() override;
        void SetupCamera(const Camera& cam);

        /// Trace a pixel or sub-pixel.
        /// @param[in]  x       X-coordinate of the (sub-)pixel's center, typically ranging from 0.5 (left) to width-0.5 (right).
        /// @param[in]  y       Y-coordinate of the (sub-)pixel's center, typically ranging from 0.5 (top) to height-0.5 (bottom).
        /// @param[in]  width   Horizontal size of the image in pixels.
        /// @param[in]  height  Vertical size of the image in pixels.
        /// @param[out] colour  Computed colour of the (sub-)pixel.
        void operator()(DBL x, DBL y, DBL width, DBL height, RGBTColour& colour);
    private:
        // Focal blur data
        class FocalBlurData final
        {
        public:
            FocalBlurData(const Camera& camera, TraceThreadData* threadData);
            ~FocalBlurData();

            // Direction to focal plane.
            DBL Focal_Distance;
            // Array of threshold for confidence test.
            DBL *Sample_Threshold;
            // Array giving number of samples to take before next confidence test.
            const int *Current_Number_Of_Samples;
            // Array of sample locations.
            Vector2d *Sample_Grid;
            // Maximum amount of jitter to use.
            DBL Max_Jitter;
            // Vectors in the viewing plane.
            Vector3d XPerp, YPerp;

        };

        bool useFocalBlur;
        FocalBlurData *focalBlurData;

        bool precomputeContainingInteriors;
        RayInteriorVector containingInteriors;

        Vector3d cameraDirection;
        Vector3d cameraRight;
        Vector3d cameraUp;
        Vector3d cameraLocation;
        /// length of current camera's 'right' vector prior to normalisation
        DBL cameraLengthRight;
        /// length of current camera's 'up' vector prior to normalisation
        DBL cameraLengthUp;
        /// aspect ratio for current camera
        DBL aspectRatio;
        /// camera
        Camera camera;
        /// scene data
        std::shared_ptr<SceneData> sceneData;
        /// thread data
        TraceThreadData *threadData;

        /// maximum trace recursion level allowed
        unsigned int maxTraceLevel;
        /// adc bailout
        DBL adcBailout;
        /// whether this is just a pretrace, allowing some computations to be skipped
        bool pretrace;

        /// Thread-local instances of user-defined camera functions
        GenericScalarFunctionInstancePtr mpCameraLocationFn[3];
        GenericScalarFunctionInstancePtr mpCameraDirectionFn[3];

        bool CreateCameraRay(Ray& ray, DBL x, DBL y, DBL width, DBL height, size_t ray_number);

        void InitRayContainerState(Ray& ray, bool compute = false);
        void InitRayContainerStateTree(Ray& ray, BBOX_TREE *node);

        void TraceRayWithFocalBlur(RGBTColour& colour, DBL x, DBL y, DBL width, DBL height);
        void JitterCameraRay(Ray& ray, DBL x, DBL y, size_t ray_number);
};

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_TRACEPIXEL_H
