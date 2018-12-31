//******************************************************************************
///
/// @file core/shape/nurbs.h
///
/// This module implements the header for the nurbs primitive.
///
/// @author Jerome Grimbert
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

#ifndef POVRAY_CORE_NURBS_H
#define POVRAY_CORE_NURBS_H

#include "core/configcore.h"

#include "core/scene/object.h"

#include "core/shape/uvmeshable.h"


namespace pov
{

/*****************************************************************************
 * Global preprocessor defines
 ******************************************************************************/

#define NURBS_OBJECT (PATCH_OBJECT)


/*****************************************************************************
 * Global typedefs
 ******************************************************************************/


class Nurbs: public ObjectBase, public UVMeshable
{
private:
    class Point4D
    {
    private:
      DBL coordinate[4];
    public:
      Point4D(); 
      Point4D( const VECTOR_4D& v ); 
      Point4D operator=(const Point4D pt);
      Point4D operator+(const Point4D pt)const;
      Point4D operator*(const DBL m)const;
      Point4D operator/(const DBL m)const;
      void asVector(Vector3d& res)const;
    };
    std::vector< std::vector< Point4D > > cp;
    std::vector< DBL > uknots;
    std::vector< DBL > vknots;
    size_t usize;
    size_t vsize;
    size_t uorder;
    size_t vorder;
    Point4D deBoor(int k, int order, int i, double x, const std::vector< DBL > & knots, const std::vector< Point4D > & ctrlPoints )const;
    int whichInterval( DBL x, size_t order, const std::vector< DBL > & knots)const;
    void computeCurveNormalizedDerivative( Vector3d& r, const int order, const DBL u, const std::vector< DBL > &knot, const std::vector< Point4D> & points )const;
public:
    virtual void evalVertex( Vector3d& r, const DBL u, const DBL v )const;
    virtual void evalNormal( Vector3d& r, const DBL u, const DBL v )const;
    virtual void minUV( Vector2d& r )const;
    virtual void maxUV( Vector2d& r )const;
    virtual ~Nurbs();
    Nurbs( const size_t x, const size_t y, const size_t uo, const size_t vo );
    void setControlPoint( const size_t x, const size_t y, const VECTOR_4D& v );
    void setUKnot( const size_t i, const DBL v );
    void setVKnot( const size_t i, const DBL v );
    /** for copy */
    Nurbs(): ObjectBase( NURBS_OBJECT ) {}


    virtual ObjectPtr Copy();

    virtual bool All_Intersections( const Ray&, IStack&, TraceThreadData* );
    virtual bool Inside( const Vector3d&, TraceThreadData* ) const;
    virtual void Normal( Vector3d&, Intersection*, TraceThreadData* ) const;
    virtual void UVCoord( Vector2d&, const Intersection*, TraceThreadData* ) const;
    virtual void Translate( const Vector3d&, const TRANSFORM* );
    virtual void Rotate( const Vector3d&, const TRANSFORM* );
    virtual void Scale( const Vector3d&, const TRANSFORM* );
    virtual void Transform( const TRANSFORM* );
    virtual void Compute_BBox();


};
}

#endif
