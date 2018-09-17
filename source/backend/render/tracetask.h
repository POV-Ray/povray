//******************************************************************************
///
/// @file backend/render/tracetask.h
///
/// @todo   What's in here?
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2018 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef POVRAY_BACKEND_TRACETASK_H
#define POVRAY_BACKEND_TRACETASK_H

#include <vector>

#include "base/image/colourspace.h"

#include "core/lighting/radiosity.h"
#include "core/material/media.h"
#include "core/render/tracepixel.h"

#include "backend/frame.h"
#include "backend/render/rendertask.h"

namespace pov
{

#ifdef PROFILE_INTERSECTIONS
    // NB not thread-safe (and not intended to be)
    extern POV_ULONG gIntersectionTime;
    extern vector <vector<POV_ULONG> > gBSPIntersectionTimes;
    extern vector <vector<POV_ULONG> > gBVHIntersectionTimes;
    extern vector <vector<POV_ULONG> > *gIntersectionTimes;
#endif

class TraceTask : public RenderTask
{
    public:
        TraceTask(ViewData *vd, unsigned int tm, DBL js,
                  DBL aat, DBL aac, unsigned int aad, pov_base::GammaCurvePtr& aag,
                  unsigned int ps, bool psc, bool contributesToImage, bool hr, size_t seed);
        virtual ~TraceTask();

        virtual void Run();
        virtual void Stopped();
        virtual void Finish();
    private:
        class CooperateFunction : public Trace::CooperateFunctor
        {
            public:
                CooperateFunction(Task& t) : task(t) { }
                virtual void operator()() { task.Cooperate(); }
            private:
                Task& task;
        };

        class SubdivisionBuffer
        {
            public:
                SubdivisionBuffer(size_t s);

                void SetSample(size_t x, size_t y, const RGBTColour& col);
                bool Sampled(size_t x, size_t y);

                RGBTColour& operator()(size_t x, size_t y);

                void Clear();
            private:
                vector<RGBTColour> colors;
                vector<bool> sampled;
                size_t size;
        };

        unsigned int tracingMethod;
        DBL jitterScale;
        DBL aaThreshold;
        DBL aaConfidence;
        unsigned int aaDepth;
        unsigned int previewSize;
        bool previewSkipCorner;
        bool passContributesToImage;    ///< Pass computes pixels for the final image.
        bool passCompletesImage;        ///< Pass is the last one computing pixels for the final image.
        bool highReproducibility;
        pov_base::GammaCurvePtr aaGamma;

        /// tracing core
        TracePixel trace;

        CooperateFunction cooperate;
        MediaFunction media;
        RadiosityFunction radiosity;
        PhotonGatherer photonGatherer;

        void SimpleSamplingM0();
        void SimpleSamplingM0P();
        void NonAdaptiveSupersamplingM1();
        void AdaptiveSupersamplingM2();
        void StochasticSupersamplingM3();

        void NonAdaptiveSupersamplingForOnePixel(DBL x, DBL y, RGBTColour& leftcol, RGBTColour& topcol, RGBTColour& curcol, bool& sampleleft, bool& sampletop, bool& samplecurrent);
        void SupersampleOnePixel(DBL x, DBL y, RGBTColour& col);
        void SubdivideOnePixel(DBL x, DBL y, DBL d, size_t bx, size_t by, size_t bstep, SubdivisionBuffer& buffer, RGBTColour& result, int level);
};

}

#endif // POVRAY_BACKEND_TRACETASK_H
