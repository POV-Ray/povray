//******************************************************************************
///
/// @file backend/lighting/photonshootingtask.cpp
///
/// This module implements Photon Mapping.
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
#include "backend/lighting/photonshootingtask.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <vector>

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/bounding/boundingbox.h"
#include "core/lighting/lightgroup.h"
#include "core/lighting/lightsource.h"
#include "core/math/matrix.h"
#include "core/render/ray.h"
#include "core/scene/object.h"
#include "core/shape/csg.h"
#include "core/support/octree.h"
#include "core/support/statistics.h"

// POV-Ray header files (POVMS module)
#include "povms/povmscpp.h"
#include "povms/povmsid.h"
#include "povms/povmsutil.h"

// POV-Ray header files (backend module)
#include "backend/lighting/photonshootingstrategy.h"
#include "backend/scene/backendscenedata.h"
#include "backend/scene/view.h"
#include "backend/scene/viewthreaddata.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

PhotonShootingTask::PhotonShootingTask(ViewData *vd, PhotonShootingStrategy* strategy, size_t seed) :
    RenderTask(vd, seed, "Photon"),
    trace(vd->GetSceneData(), GetViewDataPtr(), vd->GetQualityFeatureFlags(), cooperate),
    rands(0.0, 1.0, 32768),
    randgen(&rands),
    strategy(strategy),
    cooperate(*this),
    maxTraceLevel(vd->GetSceneData()->photonSettings.Max_Trace_Level),
    adcBailout(vd->GetSceneData()->photonSettings.adcBailout)
{
}

PhotonShootingTask::~PhotonShootingTask()
{
}


PhotonMap* PhotonShootingTask::getMediaPhotonMap()
{
    return GetViewDataPtr()->mediaPhotonMap;
}

PhotonMap* PhotonShootingTask::getSurfacePhotonMap()
{
    return GetViewDataPtr()->surfacePhotonMap;
}

void PhotonShootingTask::SendProgress(void)
{
    if (timer.ElapsedRealTime() > 1000)
    {
        // TODO FIXME PHOTONS
        // with multiple threads shooting photons, the stats messages get confusing on the front-end.
        // this is because each thread sends its own count, and so varying numbers get displayed.
        // the totals should be combined and sent from a single thread.
        timer.Reset();
        POVMS_Object obj(kPOVObjectClass_PhotonProgress);
        obj.SetInt(kPOVAttrib_CurrentPhotonCount, GetViewDataPtr()->surfacePhotonMap->numPhotons + GetViewDataPtr()->mediaPhotonMap->numPhotons);
        RenderBackend::SendViewOutput(GetViewData()->GetViewId(), GetSceneData()->frontendAddress, kPOVMsgIdent_Progress, obj);
    }
}



void PhotonShootingTask::Run()
{
    // quit right away if photons not enabled
    if (!GetSceneData()->photonSettings.photonsEnabled) return;

    Cooperate();

    PhotonShootingUnit* unit = strategy->getNextUnit();
    while(unit)
    {
        //ShootPhotonsAtObject(unit->lightAndObject.target, unit->lightAndObject.light);
        ShootPhotonsAtObject(unit->lightAndObject);
        unit = strategy->getNextUnit();
    }


    // good idea to make sure all warnings and errors arrive frontend now [trf]
    SendProgress();
    Cooperate();
}

void PhotonShootingTask::Stopped()
{
    // nothing to do for now [trf]
}

void PhotonShootingTask::Finish()
{
    GetViewDataPtr()->timeType = TraceThreadData::kPhotonTime;
    GetViewDataPtr()->realTime = ConsumedRealTime();
    GetViewDataPtr()->cpuTime = ConsumedCPUTime();
}






void PhotonShootingTask::ShootPhotonsAtObject(LightTargetCombo& combo)
{
    MathColour colour;             /* light color */
    MathColour photonColour;       /* photon color */
    ColourChannel dummyTransm;
    int i;                         /* counter */
    DBL theta, phi;                /* rotation angles */
    DBL dphi;              /* deltas for theta and phi */
    DBL jittheta, jitphi;          /* jittered versions of theta and phi */
    DBL minphi,maxphi;
                                   /* these are minimum and maximum for theta and
                                       phi for the spiral shooting */
    DBL Attenuation;               /* light attenuation for spotlight */
    TRANSFORM Trans;               /* transformation for rotation */
    int mergedFlags=0;             /* merged flags to see if we should shoot photons */
    int notComputed=true;          /* have the ray containers been computed for this point yet?*/
    int hitAtLeastOnce = false;    /* have we hit the object at least once - for autostop stuff */
    ViewThreadData *renderDataPtr = GetViewDataPtr();

    /* get the light source colour */
    colour = combo.light->colour;

    /* set global variable stuff */
    renderDataPtr->photonSourceLight = combo.light;
    renderDataPtr->photonTargetObject = combo.target;

    /* first, check on various flags... make sure all is a go for this ObjectPtr */
    mergedFlags = combo.computeMergedFlags();

    if (!( ((mergedFlags & PH_RFR_ON_FLAG) && !(mergedFlags & PH_RFR_OFF_FLAG)) ||
           ((mergedFlags & PH_RFL_ON_FLAG) && !(mergedFlags & PH_RFL_OFF_FLAG)) ))
        /* it is a no-go for this object... bail out now */
        return;

    renderDataPtr->photonSpread = combo.photonSpread;

    /* ---------------------------------------------
           main ray-shooting loop
       --------------------------------------------- */
    i = 0;
    notComputed = true;
    for(theta=combo.mintheta; theta<combo.maxtheta; theta+=combo.dtheta)
    {
        Cooperate();
        SendProgress();
        renderDataPtr->hitObject = false;

        if (theta<EPSILON)
        {
            dphi=2*M_PI;
        }
        else
        {
            /* remember that for area lights, "theta" really means "radius" */
            if (combo.light->Parallel)
            {
                dphi = combo.dtheta / theta;
            }
            else
            {
                dphi=combo.dtheta/sin(theta);
            }
        }

        // FIXME: should copy from previously computed shootingdirection
        ShootingDirection shootingDirection(combo.light,combo.target);
        shootingDirection.compute();

        minphi = -M_PI + dphi*randgen()*0.5;
        maxphi = M_PI - dphi/2 + (minphi+M_PI);
        for(phi=minphi; phi<maxphi; phi+=dphi)
        {
            int x_samples,y_samples;
            int area_x, area_y;
            /* ------------------- shoot one photon ------------------ */

            /* jitter theta & phi */
            jitphi = phi + (dphi)*(randgen() - 0.5)*1.0*GetSceneData()->photonSettings.jitter;
            jittheta = theta + (combo.dtheta)*(randgen() - 0.5)*1.0*GetSceneData()->photonSettings.jitter;

            /* actually, shoot multiple samples for area light */
            if(combo.light->Area_Light && combo.light->Photon_Area_Light && !combo.light->Parallel)
            {
                x_samples = combo.light->Area_Size1;
                y_samples = combo.light->Area_Size2;
            }
            else
            {
                x_samples = 1;
                y_samples = 1;
            }

            for(area_x=0; area_x<x_samples; area_x++)
            {
                for(area_y=0; area_y<y_samples; area_y++)
                {
                    TraceTicket ticket(maxTraceLevel, adcBailout);
                    Ray ray(ticket);

                    ray.Origin = combo.light->Center;

                    if (combo.light->Area_Light && combo.light->Photon_Area_Light && !combo.light->Parallel)
                    {
                        shootingDirection.recomputeForAreaLight(ray,area_x,area_y);
                        /* we must recompute the media containers (new start point) */
                        notComputed = true;
                    }

                    DBL dist_of_initial_from_center;

                    if (combo.light->Parallel)
                    {
                        DBL a;
                        Vector3d v;
                        /* assign the direction */
                        ray.Direction = combo.light->Direction;

                        /* project ctr onto plane defined by Direction & light location */

                        a = dot(ray.Direction, shootingDirection.toctr);
                        v = ray.Direction * (-a*shootingDirection.dist); /* MAYBE NEEDS TO BE NEGATIVE! */

                        ray.Origin = shootingDirection.ctr + v;

                        /* move point along "left" distance theta (remember theta means rad) */
                        v = shootingDirection.left * jittheta;

                        /* rotate pt around ray.Direction by phi */
                        /* use POV funcitons... slower but easy */
                        Compute_Axis_Rotation_Transform(&Trans,combo.light->Direction,jitphi);
                        MTransPoint(v, v, &Trans);

                        ray.Origin += v;

                        // compute the length of "v" if we're going to use it
                        if (combo.light->Light_Type == CYLINDER_SOURCE)
                        {
                            Vector3d initial_from_center;
                            initial_from_center = ray.Origin - combo.light->Center;
                            dist_of_initial_from_center = initial_from_center.length();
                        }
                    }
                    else
                    {
                        DBL st,ct;                     /* cos(theta) & sin(theta) for rotation */
                        /* rotate toctr by theta around up */
                        st = sin(jittheta);
                        ct = cos(jittheta);
                        /* use fast rotation */
                        Vector3d v = -st * shootingDirection.left + ct * shootingDirection.toctr;

                        /* then rotate by phi around toctr */
                        /* use POV funcitons... slower but easy */
                        Compute_Axis_Rotation_Transform(&Trans,shootingDirection.toctr,jitphi);
                        MTransPoint(ray.Direction, v, &Trans);
                    }

                    /* ------ attenuation for spot/cylinder (copied from point.c) ---- */
                    Attenuation = computeAttenuation(combo.light, ray, dist_of_initial_from_center);

                    /* set up defaults for reflection, refraction */
                    renderDataPtr->passThruPrev = true;
                    renderDataPtr->passThruThis = false;

                    renderDataPtr->photonDepth = 0.0;
                    // GetViewDataPtr()->Trace_Level = 0;
                    // Total_Depth = 0.0;
                    renderDataPtr->Stats()[Number_Of_Photons_Shot]++;

                    /* attenuate for area light extra samples */
                    Attenuation/=(x_samples*y_samples);

                    /* compute photon color from light source & attenuation */

                    photonColour = colour * Attenuation;

                    if (Attenuation<0.00001) continue;

                    /* handle the projected_through object if it exists */
                    if (combo.light->Projected_Through_Object != nullptr)
                    {
                        /* try to intersect ray with projected-through ObjectPtr */
                        Intersection Intersect;

                        Intersect.Object = nullptr;
                        if ( trace.FindIntersection(combo.light->Projected_Through_Object, Intersect, ray) )
                        {
                            /* we must recompute the media containers (new start point) */
                            notComputed = true;

                            /* we did hit it, so find the 'real' starting point of the ray */
                            /* find the farthest intersection */
                            ray.Origin += (Intersect.Depth+EPSILON) * ray.Direction;
                            renderDataPtr->photonDepth += Intersect.Depth+EPSILON;
                            while(trace.FindIntersection( combo.light->Projected_Through_Object, Intersect, ray) )
                            {
                                ray.Origin += (Intersect.Depth+EPSILON) * ray.Direction;
                                renderDataPtr->photonDepth += Intersect.Depth+EPSILON;
                            }
                        }
                        else
                        {
                            /* we didn't hit it, so stop now */
                            continue;
                        }

                    }

                    /* As mike said, "fire photon torpedo!" */
                    //Initialize_Ray_Containers(&ray);
                    ray.ClearInteriors ();

                    for(std::vector<ObjectPtr>::iterator object = GetSceneData()->objects.begin(); object != GetSceneData()->objects.end(); object++)
                    {
                        if ((*object)->Inside(ray.Origin, renderDataPtr) && ((*object)->interior != nullptr))
                            ray.AppendInterior((*object)->interior.get());
                    }

                    notComputed = false;
                    //disp_elem = 0;   /* for dispersion */
                    //disp_nelems = 0; /* for dispersion */

                    ray.SetFlags(Ray::PrimaryRay, false, true);
                    trace.TraceRay(ray, photonColour, dummyTransm, 1.0, false);

                    /* display here */
                    if ((i++%100) == 0)
                    {
                        Cooperate();
                        SendProgress();
                    }

                } // for(area_y...)
            } // for(area_x...)
        }

        /* if we didn't hit anything and we're past the autostop angle, then
             we should stop

             as per suggestion from Smellenberg, changed autostop to a percentage
             of the object's bounding sphere. */

        /* suggested by Pabs, we only use autostop if we have it it once */
        if (renderDataPtr->hitObject) hitAtLeastOnce=true;

        if (hitAtLeastOnce && !renderDataPtr->hitObject && renderDataPtr->photonTargetObject)
            if (theta > GetSceneData()->photonSettings.autoStopPercent*combo.maxtheta)
                break;
    } /* end of rays loop */
}

DBL PhotonShootingTask::computeAttenuation(const LightSource* Light, const Ray& ray, DBL dist_of_initial_from_center)
{
    DBL costheta_spot;
    DBL Attenuation = 1.0;

    /* ---------- spot light --------- */
    if (Light->Light_Type == SPOT_SOURCE)
    {
        costheta_spot = dot(ray.Direction, Light->Direction);

        if (costheta_spot > 0.0)
        {
            Attenuation = pow(costheta_spot, Light->Coeff);

            if (Light->Radius > 0.0)
                Attenuation *= cubic_spline(Light->Falloff, Light->Radius, costheta_spot);

        }
        else
            Attenuation = 0.0;
    }
    /* ---------- cylinder light ----------- */
    else if (Light->Light_Type == CYLINDER_SOURCE)
    {
        DBL k, len;

        k = dot(ray.Direction, Light->Direction);

        if (k > 0.0)
        {
            len = dist_of_initial_from_center;

            if (len < Light->Falloff)
            {
                DBL dist = 1.0 - len / Light->Falloff;
                Attenuation = pow(dist, Light->Coeff);

                if (Light->Radius > 0.0 && len > Light->Radius)
                    Attenuation *= cubic_spline(0.0, 1.0 - Light->Radius / Light->Falloff, dist);

            }
            else
                Attenuation = 0.0;
        }
        else
            Attenuation = 0.0;
    }
    return Attenuation;
}

}
// end of namespace pov
