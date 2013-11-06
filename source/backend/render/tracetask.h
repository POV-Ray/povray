/*******************************************************************************
 * tracetask.h
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
 * $File: //depot/public/povray/3.x/source/backend/render/tracetask.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef POVRAY_BACKEND_TRACETASK_H
#define POVRAY_BACKEND_TRACETASK_H

#include <vector>

#include <boost/thread.hpp>

#include "backend/frame.h"
#include "backend/povray.h"
#include "backend/scene/view.h"
#include "backend/scene/scene.h"
#include "backend/render/rendertask.h"
#include "backend/render/tracepixel.h"

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
		TraceTask(ViewData *vd, unsigned int tm, DBL js, DBL aat, unsigned int aad, GammaCurvePtr& aag, unsigned int ps, bool psc, bool final, bool hr);
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

				void SetSample(size_t x, size_t y, const Colour& col);
				bool Sampled(size_t x, size_t y);

				Colour& operator()(size_t x, size_t y);

				void Clear();
			private:
				vector<Colour> colors;
				vector<bool> sampled;
				size_t size;
		};

		unsigned int tracingMethod;
		DBL jitterScale;
		DBL aaThreshold;
		unsigned int aaDepth;
		unsigned int previewSize;
		bool previewSkipCorner;
		bool finalTrace;
		bool highReproducibility;
		GammaCurvePtr aaGamma;

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

		void NonAdaptiveSupersamplingForOnePixel(DBL x, DBL y, Colour& leftcol, Colour& topcol, Colour& curcol, bool& sampleleft, bool& sampletop, bool& samplecurrent);
		void SupersampleOnePixel(DBL x, DBL y, Colour& col);
		void SubdivideOnePixel(DBL x, DBL y, DBL d, size_t bx, size_t by, size_t bstep, SubdivisionBuffer& buffer, Colour& result, int level);
};

}

#endif // POVRAY_BACKEND_TRACETASK_H
