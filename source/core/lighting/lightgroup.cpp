//******************************************************************************
///
/// @file core/lighting/lightgroup.cpp
///
/// Implements light group utility functions.
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
#include "core/lighting/lightgroup.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <vector>

// POV-Ray header files (base module)
//  (none at the moment)

// POV-Ray header files (core module)
#include "core/lighting/lightsource.h"
#include "core/scene/object.h"
#include "core/shape/csg.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

using std::vector;

void Promote_Local_Lights_Recursive(CompoundObject *Object, vector<LightSource *>& Lights);


/*****************************************************************************
*
* FUNCTION
*
*   Promote_Local_Lights
*
* INPUT
*
*   Object - CSG union object
*
* OUTPUT
*
*   Modified CSG union object with local lights added to each object
*
* RETURNS
*
* AUTHOR
*
*   Thorsten Froehlich [trf]
*
* DESCRIPTION
*
*   Collects all light sources in CSG union object (only those who are direct
*   children of the object, not childrens children light sources - this was
*   taken care of before) and adds them to local light list of every object
*   in the CSG union.
*   NOTE: Only pointers are changed, the original light source is still the
*   one in the CSG union, and once this light source has been deallocated,
*   the pointers will be invalid.  Note that because of this we do not need
*   to (and we may not!) free the LLight lights of each object!
*
* CHANGES
*
*   Jun 2000 : Creation.
*
******************************************************************************/

void Promote_Local_Lights(CSG *Object)
{
    vector<LightSource *> lights;

    if (Object == nullptr)
        return;

    // find all light sources in the light group and connect them to form a list
    int light_counter = 0;
    int object_counter = 0;
    for(vector<ObjectPtr>::iterator curObject = Object->children.begin();
        curObject != Object->children.end();
        curObject++)
    {
        if(((*curObject)->Type & LIGHT_GROUP_LIGHT_OBJECT) == LIGHT_GROUP_LIGHT_OBJECT)
        {
            lights.push_back(reinterpret_cast<LightSource *>(*curObject));
            light_counter++;
        }
        else
            object_counter++;
    }

    // if no lights have been found in the light group we don't need to continue, but the
    // user should know there are no lights (also it will continue to work as a union)
    if(light_counter <= 0)
    {
;// TODO MESSAGE        Warning("No light source(s) found in light group.");
        return;
    }
    // if no objects have been found nothing will happen at all (the light group is only wasting memory)
    if(object_counter <= 0)
    {
;// TODO MESSAGE        Warning("No object(s) found in light group.");
        return;
    }

    // allow easy promotion of lights (if this is part of another light group)
    Object->LLights = lights;

    // promote light recursively to all other objects in the CSG union
    Promote_Local_Lights_Recursive(reinterpret_cast<CompoundObject *>(Object), lights);
}

/*****************************************************************************
*
* FUNCTION
*
*   Promote_Local_Lights_Recursive
*
* INPUT
*
*   Object - compound object
*   Lights - local lights to add to children objects
*
* OUTPUT
*
*   Modified compound object with local lights added to each object
*
* RETURNS
*
* AUTHOR
*
*   Thorsten Froehlich [trf]
*
* DESCRIPTION
*
*   Adds input list of light sources to local light list of every object in
*   the compound object, recursively if there are other compound objects.
*   NOTE: Only pointers are changed and because of this we do not need to
*   (and we may not!) free the LLight lights of each object!
*
* CHANGES
*
*   Jun 2000 : Creation.
*
******************************************************************************/

void Promote_Local_Lights_Recursive(CompoundObject *Object, vector<LightSource *>& Lights)
{
    ObjectPtr curObject = nullptr;

    for(vector<ObjectPtr>::iterator curObject = Object->children.begin();
        curObject != Object->children.end();
        curObject++)
    {
        if(!(*curObject)->LLights.empty())
        {
            for(vector<LightSource *>::iterator i = Lights.begin(); i != Lights.end(); i++)
                (*curObject)->LLights.push_back(*i);
        }
        else if(((*curObject)->Type & IS_COMPOUND_OBJECT) == IS_COMPOUND_OBJECT)
        {
            // allow easy promotion of lights (if this is part of another light group)
            (*curObject)->LLights = Lights;

            Promote_Local_Lights_Recursive(reinterpret_cast<CompoundObject *>(*curObject), Lights);
        }
        else
        {
            (*curObject)->LLights = Lights;
        }
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Check_Photon_Light_Group
*
* INPUT
*
*   Object - any object
*
* OUTPUT
*
* RETURNS
*
*   True if this object is lit by the photon light (according to the light_group rules)
*
*
* AUTHOR
*
*   Nathan Kopp [NK]
*
* DESCRIPTION
*
*   If the photon light is a global light (not in a light group) as determined by
*   the photonOptions object, then we just check to see if the object interacts
*   with global lights.
*
*   Otherwise...
*
*   Checks to see if Light is one of Object's local lights (part of the light
*   group).
*
* CHANGES
*
*   Apr 2002 : Creation.
*
******************************************************************************/

bool Check_Photon_Light_Group(ConstObjectPtr Object)
{
/* TODO FIXME   if(photonOptions.Light_Is_Global)
    {
        if((Object->Flags & NO_GLOBAL_LIGHTS_FLAG) == NO_GLOBAL_LIGHTS_FLAG)
            return false;
        else
            return true;
    }
    else
    {
        for(vector<LightSource *>::iterator Test_Light = Object->LLights.begin(); Test_Light != Object->LLights.end(); Test_Light++)
        {
            if(*Test_Light == photonOptions.Light)
                return true;
        } */
        return true;
//  }
}

}
// end of namespace pov
