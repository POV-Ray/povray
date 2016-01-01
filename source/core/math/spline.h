//******************************************************************************
///
/// @file core/math/spline.h
///
/// Declarations for spline-related maths.
///
/// @note   This file currently contains only stuff for the SDL's function-alike
///         spline feature; as such, it would naturally belong in the parser
///         module. However, it is planned for the polymorphic type hierarchy
///         herein to also absorb the spline-specific maths for the geometric
///         primtitives (which is currently embedded in the respective
///         primitives' code), and the file has already been moved to the core
///         module in preparation.
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

#ifndef POVRAY_CORE_SPLINE_H
#define POVRAY_CORE_SPLINE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

namespace pov
{

typedef DBL EXPRESS[5];

struct SplineEntry
{
    DBL par;      // Parameter
    EXPRESS vec;  // Value at the parameter
    DBL coeff[5]; // Interpolating coefficients at the parameter
};

typedef vector<SplineEntry> SplineEntryList;

typedef int SplineRefCount;

struct GenericSpline
{
    GenericSpline();
    GenericSpline(const GenericSpline& o);
    virtual ~GenericSpline();
    SplineEntryList SplineEntries;
    bool Coeffs_Computed;
    int Terms;
    SplineRefCount ref_count;

    virtual void Get(DBL p, EXPRESS& v) = 0;
    virtual GenericSpline* Clone() const = 0;
    void AcquireReference();
    void ReleaseReference();
};

struct LinearSpline : public GenericSpline
{
    LinearSpline();
    LinearSpline(const GenericSpline& o);
    virtual void Get(DBL p, EXPRESS& v);
    virtual GenericSpline* Clone() const { return new LinearSpline(*this); }
};

struct QuadraticSpline : public GenericSpline
{
    QuadraticSpline();
    QuadraticSpline(const GenericSpline& o);
    virtual void Get(DBL p, EXPRESS& v);
    virtual GenericSpline* Clone() const { return new QuadraticSpline(*this); }
};

struct NaturalSpline : public GenericSpline
{
    NaturalSpline();
    NaturalSpline(const GenericSpline& o);
    virtual void Get(DBL p, EXPRESS& v);
    virtual GenericSpline* Clone() const { return new NaturalSpline(*this); }
};

struct CatmullRomSpline : public GenericSpline
{
    CatmullRomSpline();
    CatmullRomSpline(const GenericSpline& o);
    virtual void Get(DBL p, EXPRESS& v);
    virtual GenericSpline* Clone() const { return new CatmullRomSpline(*this); }
};


// TODO FIXME - Some of the following are higher-level functions and should be moved to the parser, others should be made part of the class.

GenericSpline* Copy_Spline(const GenericSpline* Old);
void Acquire_Spline_Reference(GenericSpline* sp);
void Release_Spline_Reference(GenericSpline* sp);
void Destroy_Spline(GenericSpline* sp);
void Insert_Spline_Entry(GenericSpline* sp, DBL p, const EXPRESS& v);
DBL Get_Spline_Val(GenericSpline* sp, DBL p, EXPRESS& v, int *Terms);

}

#endif // POVRAY_CORE_SPLINE_H
