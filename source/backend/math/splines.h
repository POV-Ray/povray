//******************************************************************************
///
/// @file backend/math/splines.h
///
/// This module contains all defines, typedefs, and prototypes for
/// `splines.cpp`.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2015 Persistence of Vision Raytracer Pty. Ltd.
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

#ifndef SPLINE_H
#define SPLINE_H
/* Generic header for spline modules */

#include "parser/parser.h" // TODO - avoid this (pulled in for EXPRESS)

namespace pov
{

struct SplineEntry
{
    DBL par;      // Parameter
    EXPRESS vec;  // Value at the parameter
    DBL coeff[5]; // Interpolating coefficients at the parameter
};

typedef vector<SplineEntry> SplineEntryList;

struct GenericSpline
{
    GenericSpline();
    GenericSpline(const GenericSpline& o);
    virtual ~GenericSpline();
    SplineEntryList SplineEntries;
    bool Coeffs_Computed;
    int Terms;
    int ref_count;

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

#endif

