//******************************************************************************
///
/// @file core/scene/atmosphere.cpp
///
/// Implementations related to atmospheric effects and sky spheres.
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
#include "core/scene/atmosphere.h"

#include <algorithm>

#include "core/material/pattern.h"
#include "core/material/warp.h"
#include "core/material/pigment.h"
#include "core/material/texture.h"
#include "core/math/matrix.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

Fog_Struct::Fog_Struct() :
    Turb(NULL),
    Next(NULL)
{}

Fog_Struct::~Fog_Struct()
{
    if (Turb) delete Turb;
}

/*****************************************************************************
*
* FUNCTION
*
*   Create_Fog
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   FOG * - created fog
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Create a fog.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

FOG *Create_Fog()
{
    FOG *New;

    New = new FOG;

    New->Type = ORIG_FOG;

    New->Distance = 0.0;
    New->Alt      = 0.0;
    New->Offset   = 0.0;

    New->colour.Clear();

    New->Up = Vector3d(0.0, 1.0, 0.0);

    New->Turb = NULL;
    New->Turb_Depth = 0.5;

    New->Next = NULL;

    return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Fog
*
* INPUT
*
*   Old - fog to copy
*
* OUTPUT
*
* RETURNS
*
*   FOG * - new fog
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Copy a fog.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

FOG *Copy_Fog(const FOG *Old)
{
    FOG *New;

    New = Create_Fog();

    *New = *Old;

    New->Turb = new TurbulenceWarp();

    return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Fog
*
* INPUT
*
*   Fog - fog to destroy
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
*   Destroy a fog.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

void Destroy_Fog(FOG *Fog)
{
    if (Fog != NULL)
        delete Fog;
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Rainbow
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   RAINBOW * - created rainbow
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Create a rainbow.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

RAINBOW *Create_Rainbow()
{
    RAINBOW *New;

    New = new RAINBOW;

    New->Distance = MAX_DISTANCE;
    New->Jitter   = 0.0;
    New->Angle    = 0.0;
    New->Width    = 0.0;

    New->Falloff_Width  = 0.0;
    New->Arc_Angle      = 180.0;
    New->Falloff_Angle  = 180.0;

    New->Pigment = NULL;

    New->Antisolar_Vector = Vector3d(0.0, 0.0, 0.0);

    New->Right_Vector = Vector3d(1.0, 0.0, 0.0);
    New->Up_Vector = Vector3d(0.0, 1.0, 0.0);

    New->Next = NULL;

    return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Rainbow
*
* INPUT
*
*   Old - rainbow to copy
*
* OUTPUT
*
* RETURNS
*
*   RAINBOW * - new rainbow
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Copy a rainbow.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

RAINBOW *Copy_Rainbow(const RAINBOW *Old)
{
    RAINBOW *New;

    New = Create_Rainbow();

    *New = *Old;

    return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Rainbow
*
* INPUT
*
*   Rainbow - rainbow to destroy
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
*   Destroy a rainbow.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

void Destroy_Rainbow(RAINBOW *Rainbow)
{
    if (Rainbow != NULL)
        delete Rainbow;
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Skysphere
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
*   SKYSPHERE * - created skysphere
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Create a skysphere.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

SKYSPHERE *Create_Skysphere()
{
    SKYSPHERE *New;

    New = new SKYSPHERE;

    New->Emission = ColourModelRGB::Whitepoint::Colour; // TODO

    New->Trans = Create_Transform();

    return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Skysphere
*
* INPUT
*
*   Old - skysphere to copy
*
* OUTPUT
*
* RETURNS
*
*   SKYSPHERE * - copied skysphere
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Copy a skysphere.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

SKYSPHERE *Copy_Skysphere(const SKYSPHERE *Old)
{
    SKYSPHERE *New;

    New = Create_Skysphere();

    Destroy_Transform(New->Trans);

    *New = *Old;

    New->Trans = Copy_Transform(Old->Trans);

    // The standard assignment operator of SKYSPHERE has created a shallow copy of the Pigments vector, but we need a
    // deep copy in case Old gets destroyed.
    for (vector<PIGMENT*>::iterator i = New->Pigments.begin(); i != New->Pigments.end(); ++ i)
    {
        *i = Copy_Pigment(*i);
    }

    return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Skysphere
*
* INPUT
*
*   Skysphere - skysphere to destroy
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
*   Destroy a skysphere.
*
* CHANGES
*
*   Dec 1994 : Creation.
*
******************************************************************************/

void Destroy_Skysphere(SKYSPHERE *Skysphere)
{
    if (Skysphere != NULL)
        delete Skysphere;
}

Skysphere_Struct::~Skysphere_Struct()
{
    for (vector<PIGMENT*>::iterator i = Pigments.begin(); i != Pigments.end(); ++ i)
        delete *i;
    Destroy_Transform(Trans);
}


/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Skysphere
*
* INPUT
*
*   Vector - Rotation vector
*
* OUTPUT
*
*   Skysphere - Pointer to skysphere structure
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Rotate a skysphere.
*
* CHANGES
*
*   Feb 1996 : Creation.
*
******************************************************************************/

void Rotate_Skysphere(SKYSPHERE *Skysphere, const Vector3d& Vector)
{
    TRANSFORM Trans;

    Compute_Rotation_Transform(&Trans, Vector);

    Transform_Skysphere(Skysphere, &Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Skysphere
*
* INPUT
*
*   Vector - Scaling vector
*
* OUTPUT
*
*   Skysphere - Pointer to skysphere structure
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Scale a skysphere.
*
* CHANGES
*
*   Feb 1996 : Creation.
*
******************************************************************************/

void Scale_Skysphere(SKYSPHERE *Skysphere, const Vector3d& Vector)
{
    TRANSFORM Trans;

    Compute_Scaling_Transform(&Trans, Vector);

    Transform_Skysphere(Skysphere, &Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Skysphere
*
* INPUT
*
*   Vector - Translation vector
*
* OUTPUT
*
*   Skysphere - Pointer to skysphere structure
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Translate a skysphere.
*
* CHANGES
*
*   Feb 1996 : Creation.
*
******************************************************************************/

void Translate_Skysphere(SKYSPHERE *Skysphere, const Vector3d& Vector)
{
    TRANSFORM Trans;

    Compute_Translation_Transform(&Trans, Vector);

    Transform_Skysphere(Skysphere, &Trans);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Skysphere
*
* INPUT
*
*   Trans  - Pointer to transformation
*
* OUTPUT
*
*   Skysphere - Pointer to skysphere structure
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Transform a skysphere.
*
* CHANGES
*
*   Feb 1996 : Creation.
*
******************************************************************************/

void Transform_Skysphere(SKYSPHERE *Skysphere, const TRANSFORM *Trans)
{
    if (Skysphere->Trans == NULL)
    {
        Skysphere->Trans = Create_Transform();
    }

    Compose_Transforms(Skysphere->Trans, Trans);
}

}
