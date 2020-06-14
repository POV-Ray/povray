//******************************************************************************
///
/// @file core/shape/nurbs.cpp
///
/// This module implements the nurbs primitive.
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


#include "core/shape/nurbs.h"


#include "core/bounding/boundingbox.h"
#include "core/math/matrix.h"
#include "core/render/ray.h"
#include "core/scene/tracethreaddata.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/


Nurbs::Point4D::Point4D()
{
  coordinate[X]=0;
  coordinate[Y]=0;
  coordinate[Z]=0;
  coordinate[W]=0;
}
Nurbs::Point4D::Point4D( const VECTOR_4D& v)
{
  coordinate[X]=v[X];
  coordinate[Y]=v[Y];
  coordinate[Z]=v[Z];
  coordinate[W]=v[W];
}
Nurbs::Point4D Nurbs::Point4D::operator=(const Nurbs::Point4D v )
{
  coordinate[X]=v.coordinate[X];
  coordinate[Y]=v.coordinate[Y];
  coordinate[Z]=v.coordinate[Z];
  coordinate[W]=v.coordinate[W];
  return *this;
}
Nurbs::Point4D Nurbs::Point4D::operator+(const Nurbs::Point4D v )const
{
  Point4D temp;
  temp.coordinate[X] = coordinate[X] + v.coordinate[X];
  temp.coordinate[Y] = coordinate[Y] + v.coordinate[Y];
  temp.coordinate[Z] = coordinate[Z] + v.coordinate[Z];
  temp.coordinate[W] = coordinate[W] + v.coordinate[W];
  return temp;
}
Nurbs::Point4D Nurbs::Point4D::operator*(const DBL m )const
{
  Point4D temp;
  temp.coordinate[X] = m*coordinate[X];
  temp.coordinate[Y] = m*coordinate[Y];
  temp.coordinate[Z] = m*coordinate[Z];
  temp.coordinate[W] = m*coordinate[W];
  return temp;
}
  
Nurbs::Point4D Nurbs::Point4D::operator/(const DBL m )const
{
  Point4D temp;
  temp.coordinate[X] = coordinate[X]/m;
  temp.coordinate[Y] = coordinate[Y]/m;
  temp.coordinate[Z] = coordinate[Z]/m;
  temp.coordinate[W] = coordinate[W]/m;
  return temp;
}

void Nurbs::Point4D::asVector( Vector3d& res) const
{
  res[X] = coordinate[X]/coordinate[W];
  res[Y] = coordinate[Y]/coordinate[W];
  res[Z] = coordinate[Z]/coordinate[W];
}
Nurbs::Point4D Nurbs::deBoor( int k, int order, int i, DBL x, const std::vector< DBL > & knots, const std::vector< Point4D > & ctrlPoints )const

{
    if( k == 0 )
    {
        return ctrlPoints[i];
    }
    else
    {
       DBL alpha = (x-knots.at(i))/(knots.at(i+order-k)-knots.at(i));
       return (deBoor(k-1, order, i-1, x, knots, ctrlPoints)*(1-alpha)+deBoor(k-1, order, i, x, knots, ctrlPoints )*alpha);
    }
}
Nurbs::Nurbs( const size_t x, const size_t y, const size_t uo, const size_t vo  ): ObjectBase(NURBS_OBJECT),usize(x),vsize(y),uorder(uo),vorder(vo)
{
  Trans = Create_Transform();
  cp.resize(y);
  for(size_t i = 0; i < y; ++i)
  {
    cp.at(i).resize(x);
  }
  uknots.resize(x+uo);
  vknots.resize(y+vo);
}

void Nurbs::minUV( Vector2d& r) const
{
  r[U] = uknots[uorder-1];
  r[V] = vknots[vorder-1];
}
void Nurbs::maxUV( Vector2d& r) const
{
  r[U] = uknots[uknots.size()-uorder];
  r[V] = vknots[vknots.size()-vorder];
}
void Nurbs::computeCurveNormalizedDerivative( Vector3d& r, const int order, const DBL u, const std::vector< DBL > &knots, const std::vector< Point4D> & points )const
{
    /*
     * R'(u) = n * ((w_I,n-1(u)* w_I+1,n-1(u))/((w_I+1,n(u))^2)) * (( P_I+1,n-1(u)-P_I,n-1(u))/(u_I+1-u_I))
     *
     * n is irrelevant, we normalize the normal
     * the computation of weight is also irrelevant, because... we normalize the normal
     *
     * Remain R'(u) = (P_I+1,n-1(u) - P_I,n-1(u))/(u_I+1-u_I)
     *
     * But, as we normalize the normal, and u_I+1 >= u_I (with special treatment of nurb: 1/0 = 1)
     * we can also drop the knot part (u_X) 
     *
     * Remain R'(u) = (P_I+1,n-1(u) - P_I,n-1(u))
     */
  int interval = whichInterval( u, order, knots);
  Point4D pnext = deBoor( order-2, order , interval, u, knots, points);
  Point4D pbase = deBoor( order -2, order, interval-1, u, knots, points);
  Vector3d next,base;
  pnext.asVector( next );
  pbase.asVector( base );
  r = next - base;
  r.normalize();
}
void Nurbs::evalNormal( Vector3d& r, const DBL u, const DBL v, TraceThreadData * )const
{
// compute the normal : cross product of the 2 first derivatives
    Vector3d n1, n2, value;
    /**
     * for each coordinates axis (u,v), compute the nurb curve along the axis
     * then compute the derivative
     *
     * R'(u) = n * ((w_I,n-1(u)* w_I+1,n-1(u))/((w_I+1,n(u))^2)) * (( P_I+1,n-1(u)-P_I,n-1(u))/(u_I+1-u_I))
     *
     * ( From Michael S. Floater, Evaluation and Properties of the Derivative of a NURBS Curve
     *  in 
     *  Mathematical Methods in CAGD and Image Processing, Tom Lyche and L. L. Schumaler (eds)
     *  Copyright 1992 by Academic Press, Boston
     * 
     *  Read it, it's worth the time)
     *
     * R(u) = P_I+1,n
     */
    /* first axis, easy */
    std::vector< Point4D > temp;
    temp.resize( vsize );
    int interval = whichInterval( u, uorder, uknots );

    for( size_t i = 0; i < vsize; ++i )
    {
        temp[i] = deBoor( uorder - 1, uorder, interval, u , uknots, cp[i] );
    }

    computeCurveNormalizedDerivative( n1, vorder, v, vknots, temp );
    /* second axis, need to transpose cp as tcp */
    temp.resize( usize );
    std::vector< std::vector< Point4D > > tcp;
    size_t y = cp[0].size();
    size_t x = cp.size();
    tcp.resize( y );

    for( size_t i = 0; i < y; ++i )
    {
        tcp.at( i ).resize( x );

        for( size_t j = 0; j < x; ++j )
        {
            tcp.at( i ).at( j ) = cp.at( j ).at( i );
        }
    }
    interval = whichInterval( v, vorder, vknots );
    for( size_t i = 0; i < vsize; ++i )
    {
        temp[i] = deBoor( vorder - 1, vorder, interval, v , vknots, tcp[i] );
    }

    computeCurveNormalizedDerivative( n2, uorder, u, uknots, temp );
    /* time for cross product, of normalized vectors, so no need to normalize again */
    value = cross( n1, n2 );
    MTransNormal( r, value, Trans );
}
void Nurbs::evalVertex( Vector3d& Real_Pt, const DBL u, const DBL v, TraceThreadData * )const
{
// TODO optimise to not compute all the points of temp
    Vector3d Point;
    std::vector< Point4D > temp;
    temp.resize( vsize );
    int interval = whichInterval( u, uorder, uknots );

    for( size_t i = 0; i < vsize; ++i )
    {
        temp[i] = deBoor( uorder - 1, uorder, interval, u , uknots, cp[i] );
    }

    interval = whichInterval( v, vorder, vknots );
    Point4D value = deBoor( vorder - 1, vorder, interval, v , vknots, temp );
    value.asVector( Point );
    MTransPoint( Real_Pt, Point, Trans );
}
int Nurbs::whichInterval( DBL x,size_t order,  const std::vector< DBL > & knots )const
{
// we could detect when x is not in the minUV;maxUV range, but useless
  for(int i = order; i < (knots.size()-order); ++i)
  {
    if (x < knots[i])
    {
      return i-1;
    }
  }
  return knots.size()-order-1;
}
void Nurbs::setControlPoint(const size_t x, const size_t y, const VECTOR_4D& v)
{
  VECTOR_4D tmp;
  tmp[X] = v[X]*v[W];
  tmp[Y] = v[Y]*v[W];
  tmp[Z] = v[Z]*v[W];
  tmp[W] = v[W];
  cp[y][x]=tmp;
}
void Nurbs::setUKnot(const size_t i, const DBL v )
{
  uknots[i] = v;
}
void Nurbs::setVKnot(const size_t i, const DBL v )
{
  vknots[i] = v;
}

Nurbs::~Nurbs()
{
}

ObjectPtr Nurbs::Copy()
{
    Nurbs* New = new Nurbs();
    Destroy_Transform( New->Trans );
    * New = *this;
    New->Trans = Copy_Transform( Trans );
    return ( New );
}

bool Nurbs::All_Intersections( const Ray&, IStack&, TraceThreadData* )
{
    return false;
}
bool Nurbs::Inside( const Vector3d&, TraceThreadData* ) const
{
    return false;
}
void Nurbs::Normal( Vector3d&, Intersection*, TraceThreadData* ) const
{
}
void Nurbs::UVCoord( Vector2d&, const Intersection* ) const
{
}
void Nurbs::Translate( const Vector3d&, const TRANSFORM* tr )
{
  Transform( tr );
}
void Nurbs::Rotate( const Vector3d&, const TRANSFORM* tr )
{
  Transform( tr );
}
void Nurbs::Scale( const Vector3d&, const TRANSFORM* tr )
{
  Transform( tr );
}
void Nurbs::Transform( const TRANSFORM* tr )
{
    if( Trans == NULL )
    { Trans = Create_Transform(); }

    Compose_Transforms( Trans, tr );
    Compute_BBox();
}
void Nurbs::Compute_BBox()
{
  Make_BBox(BBox, 0, 0,0 , 0, 0,0);
}


}
