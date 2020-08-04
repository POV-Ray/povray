//******************************************************************************
///
/// @file core/shape/rationalbezierpatch.h
///
/// This module implements the header for the rational_bezier_patch primitive.
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

#ifndef POVRAY_CORE_RATIONALBEZIERPATCH_H
#define POVRAY_CORE_RATIONALBEZIERPATCH_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

#include "core/scene/object.h"

#include "core/shape/uvmeshable.h"


namespace pov
{

/*****************************************************************************
 * Global preprocessor defines
 ******************************************************************************/

#define RATIONAL_BEZIER_PATCH_OBJECT (PATCH_OBJECT)

/*****************************************************************************
 * Global typedefs
 ******************************************************************************/

class RationalBezierPatch : public NonsolidObject, public UVMeshable
{
private:
    class Grid
    {
    public:
        Grid();
        Grid( const size_t x, const size_t y );
        ~Grid();
        void deCasteljauX( Grid& first, Grid& second, const DBL w, const DBL dw )const;
        void deCasteljauY( Grid& first, Grid& second, const DBL w, const DBL dw )const;
        void deCasteljauXF( Grid& first, const DBL w, const DBL dw )const;
        void deCasteljauYF( Grid& first, const DBL w, const DBL dw )const;
        void deCasteljauXB( Grid& second, const DBL w, const DBL dw )const;
        void deCasteljauYB( Grid& second, const DBL w, const DBL dw )const;
        bool getMinMaxX( DBL& min, DBL& max )const;
        bool getMinMaxY( DBL& min, DBL& max )const;
        DBL get( const size_t x, const size_t y )const;
        DBL set( const size_t x, const size_t y, const DBL& v );
        DBL getBottomLeft()const{return content.at(ysize-1).at(0);}
        DBL getBottomRight()const{return content.at(ysize-1).at(xsize-1);}
        DBL getTopLeft()const{return content.at(0).at(0);}
        DBL getTopRight()const{return content.at(0).at(xsize-1);}
        size_t dimX()const {return xsize;}
        size_t dimY()const {return ysize;}
        bool planePointsDistances( const Vector3d& normal, const DBL d, const Grid& x, const Grid& y, const Grid& z, const Grid& w );
        void linePointsDistances( const Vector2d& line, const Grid& a, const Grid& b );
        void point( const DBL u, const DBL v, bool vFirst, DBL & p0, DBL & p10, DBL & p11 )const;
        bool bounds(Vector2d& l)const;

    private:
        size_t xsize;
        size_t ysize;
        std::vector<std::vector<DBL> > content;
        void deCasteljauForward( const std::vector<DBL>& in, std::vector<DBL>& out, const DBL w, const DBL dw )const;
        void deCasteljauBackward( const std::vector<DBL>& in, std::vector<DBL>& out, const DBL w, const DBL dw )const;
        void checkIntersection( const DBL p, const DBL n, const DBL pw, const DBL nw, DBL& min, DBL & max )const;
        void reduceX(){xsize = 1;}
        void reduceY(){ysize = 1;}
    };

    Grid wc[3];/**< weigthed X coordinates */
    Grid weight;/**< original weights */
    DBL accuracy;/**< limit for solver */
    Vector3d minbox;
    Vector3d maxbox;
    /** for copy */
    RationalBezierPatch(): NonsolidObject( RATIONAL_BEZIER_PATCH_OBJECT ) {}
    bool lineU(const Grid& a, const Grid& b, Vector2d& l)const;
    bool lineV(const Grid& a, const Grid& b, Vector2d& l)const;
    bool bounds(const Grid& a, const Grid&b, Vector2d& la, Vector2d& lb)const;
    bool addSolution(const DBL u, const DBL v, const BasicRay& ray, IStack& Depth_Stack, TraceThreadData* Thread);
    bool findSolution( Grid& a, Grid& b, Vector2d& i0, Vector2d& i1, Vector2d& b0, Vector2d& b1, const BasicRay& ray, IStack& Depth_Stack, TraceThreadData* Thread);
    void findPlanes( const BasicRay& ray, Vector3d& n0, DBL& d0, Vector3d& n1, DBL& d1)const;
public:
    RationalBezierPatch( const size_t x, const size_t y );
    void set( const size_t x, const size_t y, const VECTOR_4D& v );
    void setAccuracy( const DBL a ) {accuracy = a;}
    virtual void evalVertex( Vector3d& r, const DBL u, const DBL v, TraceThreadData *Thread )const override;
    virtual void evalNormal( Vector3d& r, const DBL u, const DBL v, TraceThreadData *Thread )const override;
    virtual void minUV( Vector2d& r )const override;
    virtual void maxUV( Vector2d& r )const override;
    virtual ~RationalBezierPatch();

    virtual ObjectPtr Copy() override;

    virtual bool All_Intersections( const Ray&, IStack&, TraceThreadData* ) override;
    virtual bool Inside( const Vector3d&, TraceThreadData* ) const override;
    virtual void Normal( Vector3d&, Intersection*, TraceThreadData* ) const override;
    virtual void UVCoord( Vector2d&, const Intersection* ) const override;
    virtual void Translate( const Vector3d&, const TRANSFORM* ) override;
    virtual void Rotate( const Vector3d&, const TRANSFORM* ) override;
    virtual void Scale( const Vector3d&, const TRANSFORM* ) override;
    virtual void Transform( const TRANSFORM* ) override;
    virtual void Compute_BBox() override;
};

}

#endif
