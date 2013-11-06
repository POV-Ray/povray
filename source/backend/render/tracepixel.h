/*******************************************************************************
 * tracepixel.h
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
 * $File: //depot/public/povray/3.x/source/backend/render/tracepixel.h $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

#ifndef POVRAY_BACKEND_TRACEPIXEL_H
#define POVRAY_BACKEND_TRACEPIXEL_H

#include <vector>

#include <boost/thread.hpp>

#include "backend/frame.h"
#include "backend/povray.h"
#include "backend/scene/view.h"
#include "backend/scene/scene.h"
#include "backend/render/trace.h"

namespace pov
{

class BSPIntersectFunctor : public BSPTree::Intersect
{
	public:
		BSPIntersectFunctor(Intersection& bi, const Ray& r, vector<ObjectPtr>& objs, TraceThreadData *t) :
			found(false),
			bestisect(bi),
			ray(r),
			objects(objs),
			traceThreadData(t)
		{
			Vector3d tmp(1.0 / ray.GetDirection()[X], 1.0 / ray.GetDirection()[Y], 1.0 /ray.GetDirection()[Z]);
			Assign_Vector(origin, ray.Origin);
			Assign_Vector(invdir, *tmp);
			variant = (ObjectBase::BBoxDirection)((int(invdir[X] < 0.0) << 2) | (int(invdir[Y] < 0.0) << 1) | int(invdir[Z] < 0.0));
		}

		virtual bool operator()(unsigned int index, double& maxdist)
		{
			ObjectPtr object = objects[index];
			Intersection isect;

			if(Find_Intersection(&isect, object, ray, variant, origin, invdir, traceThreadData) && (isect.Depth <= maxdist))
			{
				if(isect.Depth < bestisect.Depth)
				{
					bestisect = isect;
					found = true;
					maxdist = bestisect.Depth;
				}
			}

			return found;
		}

		virtual bool operator()() const { return found; }
	private:
		bool found;
		vector<ObjectPtr>& objects;
		Intersection& bestisect;
		const Ray& ray;
		BBOX_VECT origin;
		BBOX_VECT invdir;
		ObjectBase::BBoxDirection variant;
		TraceThreadData *traceThreadData;
};

class BSPIntersectCondFunctor : public BSPTree::Intersect
{
	public:
		BSPIntersectCondFunctor(Intersection& bi, const Ray& r, vector<ObjectPtr>& objs, TraceThreadData *t,
		                        const RayObjectCondition& prec, const RayObjectCondition& postc) :
			found(false),
			bestisect(bi),
			ray(r),
			objects(objs),
			traceThreadData(t),
			precondition(prec),
			postcondition(postc)
		{
			Vector3d tmp(1.0 / ray.GetDirection()[X], 1.0 / ray.GetDirection()[Y], 1.0 /ray.GetDirection()[Z]);
			Assign_Vector(origin, ray.Origin);
			Assign_Vector(invdir, *tmp);
			variant = (ObjectBase::BBoxDirection)((int(invdir[X] < 0.0) << 2) | (int(invdir[Y] < 0.0) << 1) | int(invdir[Z] < 0.0));
		}

		virtual bool operator()(unsigned int index, double& maxdist)
		{
			ObjectPtr object = objects[index];

			if(precondition(ray, object, 0.0) == true)
			{
				Intersection isect;

				if(Find_Intersection(&isect, object, ray, variant, origin, invdir, postcondition, traceThreadData) && (isect.Depth <= maxdist))
				{
					if(isect.Depth < bestisect.Depth)
					{
						bestisect = isect;
						found = true;
						maxdist = bestisect.Depth;
					}
				}
			}

			return found;
		}

		virtual bool operator()() const { return found; }
	private:
		bool found;
		vector<ObjectPtr>& objects;
		Intersection& bestisect;
		const Ray& ray;
		BBOX_VECT origin;
		BBOX_VECT invdir;
		ObjectBase::BBoxDirection variant;
		TraceThreadData *traceThreadData;
		const RayObjectCondition& precondition;
		const RayObjectCondition& postcondition;
};

class BSPInsideCondFunctor : public BSPTree::Inside
{
	public:
		BSPInsideCondFunctor(Vector3d o, vector<ObjectPtr>& objs, TraceThreadData *t,
		                     const PointObjectCondition& prec, const PointObjectCondition& postc) :
			found(false),
			origin(o),
			objects(objs),
			precondition(prec),
			postcondition(postc),
			threadData(t)
		{
		}

		virtual bool operator()(unsigned int index)
		{
			ObjectPtr object = objects[index];
			if(precondition(origin, object))
				if(Inside_BBox(*origin, object->BBox) && object->Inside(*origin, threadData))
					if(postcondition(origin, object))
						found = true;
			return found;
		}

		virtual bool operator()() const { return found; }
	private:
		bool found;
		vector<ObjectPtr>& objects;
		Vector3d origin;
		const PointObjectCondition& precondition;
		const PointObjectCondition& postcondition;
		TraceThreadData *threadData;
};

struct HasInteriorPointObjectCondition : public PointObjectCondition
{
	virtual bool operator()(const Vector3d& point, const ObjectBase* object) const
	{
		return object->interior != NULL;
	}
};

struct ContainingInteriorsPointObjectCondition : public PointObjectCondition
{
	ContainingInteriorsPointObjectCondition(RayInteriorVector& ci) : containingInteriors(ci) {}
	virtual bool operator()(const Vector3d& point, const ObjectBase* object) const
	{
		containingInteriors.push_back(object->interior);
		return true;
	}
	RayInteriorVector &containingInteriors;
};

class TracePixel : public Trace
{
	public:
		TracePixel(ViewData *vd, TraceThreadData *td, unsigned int mtl, DBL adcb, unsigned int qf,
		           CooperateFunctor& cf, MediaFunctor& mf, RadiosityFunctor& af, bool pt = false);
		virtual ~TracePixel();
		void SetupCamera(const Camera& cam);

		void operator()(DBL x, DBL y, DBL width, DBL height, Colour& colour);
	private:
		// Focal blur data
		class FocalBlurData
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
			VECTOR XPerp, YPerp;

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
		shared_ptr<SceneData> sceneData;
		/// thread data
		TraceThreadData *threadData;

		/// maximum trace recursion level allowed
		unsigned int maxTraceLevel;
		/// adc bailout
		DBL adcBailout;
		/// whether this is just a pretrace, allowing some computations to be skipped
		bool pretrace;

		bool CreateCameraRay(Ray& ray, DBL x, DBL y, DBL width, DBL height, size_t ray_number);

		void InitRayContainerState(Ray& ray, bool compute = false);
		void InitRayContainerStateTree(Ray& ray, BBOX_TREE *node);

		void TraceRayWithFocalBlur(Colour& colour, DBL x, DBL y, DBL width, DBL height);
		void JitterCameraRay(Ray& ray, DBL x, DBL y, size_t ray_number);
};

}

#endif // POVRAY_BACKEND_TRACEPIXEL_H
