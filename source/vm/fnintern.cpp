//******************************************************************************
///
/// @file vm/fnintern.cpp
///
/// This module implements built-in render-time functions.
///
/// This module is based on code by D. Skarda, T. Bily and R. Suzuki.
/// It includes functions based on code first introduced by many other
/// contributors.
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
#include "vm/fnintern.h"

#include <algorithm>

#include "base/mathutil.h"

#include "core/material/pigment.h"
#include "core/material/texture.h"
#include "core/material/warp.h"
#include "core/math/matrix.h"
#include "core/math/spline.h"
#include "core/scene/scenedata.h"
#include "core/scene/tracethreaddata.h"

#include "vm/fnpovfpu.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

/*
 * IMPORTANT NOTICE
 * Never ever use any PARAM_xxx macro after a call to POVFPU_SetLocal
 * because POVFPU_SetLocal may change the stack base address and thus
 * the "ptr" passed to the functions may be invalid after the call! [trf]
 */
#define PARAM_X (ptr[0])
#define PARAM_Y (ptr[1])
#define PARAM_Z (ptr[2])
#define PARAM(index) (ptr[index + 3])
#define PARAM_N_X(offset) (ptr[offset])
#define PARAM_N_Y(offset) (ptr[offset + 1])
#define PARAM_N_Z(offset) (ptr[offset + 2])
#define PARAM_N(index,offset) (ptr[index + offset + 3])

#define ROT2D(p,d,ang) if (p>0) {x2=sqrt(x2+PARAM_Z*PARAM_Z)- d; th=ang*M_PI_180; \
    if (th!=0){ PARAM_X= x2*cos(th)-PARAM_Y*sin(th); PARAM_Y= x2*sin(th)+PARAM_Y*cos(th);} else PARAM_X=x2; \
    x2=PARAM_X*PARAM_X; y2=PARAM_Y*PARAM_Y;}


/*****************************************************************************
* Global functions
******************************************************************************/

DBL f_algbr_cyl1(FPUContext *ctx, DBL *ptr, unsigned int fn); // 0
DBL f_algbr_cyl2(FPUContext *ctx, DBL *ptr, unsigned int fn); // 1
DBL f_algbr_cyl3(FPUContext *ctx, DBL *ptr, unsigned int fn); // 2
DBL f_algbr_cyl4(FPUContext *ctx, DBL *ptr, unsigned int fn); // 3
DBL f_bicorn(FPUContext *ctx, DBL *ptr, unsigned int fn); // 4
DBL f_bifolia(FPUContext *ctx, DBL *ptr, unsigned int fn); // 5
DBL f_blob(FPUContext *ctx, DBL *ptr, unsigned int fn); // 6
DBL f_blob2(FPUContext *ctx, DBL *ptr, unsigned int fn); // 7
DBL f_boy_surface(FPUContext *ctx, DBL *ptr, unsigned int fn); // 8
DBL f_comma(FPUContext *ctx, DBL *ptr, unsigned int fn); // 9
DBL f_cross_ellipsoids(FPUContext *ctx, DBL *ptr, unsigned int fn); // 10
DBL f_crossed_trough(FPUContext *ctx, DBL *ptr, unsigned int fn); // 11
DBL f_cubic_saddle(FPUContext *ctx, DBL *ptr, unsigned int fn); // 12
DBL f_cushion(FPUContext *ctx, DBL *ptr, unsigned int fn); // 13
DBL f_devils_curve(FPUContext *ctx, DBL *ptr, unsigned int fn); // 14
DBL f_devils_curve_2d(FPUContext *ctx, DBL *ptr, unsigned int fn); // 15
DBL f_dupin_cyclid(FPUContext *ctx, DBL *ptr, unsigned int fn); // 16
DBL f_ellipsoid(FPUContext *ctx, DBL *ptr, unsigned int fn); // 17
DBL f_enneper(FPUContext *ctx, DBL *ptr, unsigned int fn); // 18
DBL f_flange_cover(FPUContext *ctx, DBL *ptr, unsigned int fn); // 19
DBL f_folium_surface(FPUContext *ctx, DBL *ptr, unsigned int fn); // 20
DBL f_folium_surface_2d(FPUContext *ctx, DBL *ptr, unsigned int fn); // 21
DBL f_glob(FPUContext *ctx, DBL *ptr, unsigned int fn); // 22
DBL f_heart(FPUContext *ctx, DBL *ptr, unsigned int fn); // 23
DBL f_helical_torus(FPUContext *ctx, DBL *ptr, unsigned int fn); // 24
DBL f_helix1(FPUContext *ctx, DBL *ptr, unsigned int fn); // 25
DBL f_helix2(FPUContext *ctx, DBL *ptr, unsigned int fn); // 26
DBL f_hex_x(FPUContext *ctx, DBL *ptr, unsigned int fn); // 27
DBL f_hex_y(FPUContext *ctx, DBL *ptr, unsigned int fn); // 28
DBL f_hetero_mf(FPUContext *ctx, DBL *ptr, unsigned int fn); // 29
DBL f_hunt_surface(FPUContext *ctx, DBL *ptr, unsigned int fn); // 30
DBL f_hyperbolic_torus(FPUContext *ctx, DBL *ptr, unsigned int fn); // 31
DBL f_isect_ellipsoids(FPUContext *ctx, DBL *ptr, unsigned int fn); // 32
DBL f_kampyle_of_eudoxus(FPUContext *ctx, DBL *ptr, unsigned int fn); // 33
DBL f_kampyle_of_eudoxus_2d(FPUContext *ctx, DBL *ptr, unsigned int fn); // 34
DBL f_klein_bottle(FPUContext *ctx, DBL *ptr, unsigned int fn); // 35
DBL f_kummer_surface_v1(FPUContext *ctx, DBL *ptr, unsigned int fn); // 36
DBL f_kummer_surface_v2(FPUContext *ctx, DBL *ptr, unsigned int fn); // 37
DBL f_lemniscate_of_gerono(FPUContext *ctx, DBL *ptr, unsigned int fn); // 38
DBL f_lemniscate_of_gerono_2d(FPUContext *ctx, DBL *ptr, unsigned int fn); // 39
DBL f_mesh1(FPUContext *ctx, DBL *ptr, unsigned int fn); // 40
DBL f_mitre(FPUContext *ctx, DBL *ptr, unsigned int fn); // 41
DBL f_nodal_cubic(FPUContext *ctx, DBL *ptr, unsigned int fn); // 42
DBL f_odd(FPUContext *ctx, DBL *ptr, unsigned int fn); // 43
DBL f_ovals_of_cassini(FPUContext *ctx, DBL *ptr, unsigned int fn); // 44
DBL f_paraboloid(FPUContext *ctx, DBL *ptr, unsigned int fn); // 45
DBL f_parabolic_torus(FPUContext *ctx, DBL *ptr, unsigned int fn); // 46
DBL f_ph(FPUContext *ctx, DBL *ptr, unsigned int fn); // 47
DBL f_pillow(FPUContext *ctx, DBL *ptr, unsigned int fn); // 48
DBL f_piriform(FPUContext *ctx, DBL *ptr, unsigned int fn); // 49
DBL f_piriform_2d(FPUContext *ctx, DBL *ptr, unsigned int fn); // 50
DBL f_poly4(FPUContext *ctx, DBL *ptr, unsigned int fn); // 51
DBL f_polytubes(FPUContext *ctx, DBL *ptr, unsigned int fn); // 52
DBL f_quantum(FPUContext *ctx, DBL *ptr, unsigned int fn); // 53
DBL f_quartic_paraboloid(FPUContext *ctx, DBL *ptr, unsigned int fn); // 54
DBL f_quartic_saddle(FPUContext *ctx, DBL *ptr, unsigned int fn); // 55
DBL f_quartic_cylinder(FPUContext *ctx, DBL *ptr, unsigned int fn); // 56
DBL f_r(FPUContext *ctx, DBL *ptr, unsigned int fn); // 57
DBL f_ridge(FPUContext *ctx, DBL *ptr, unsigned int fn); // 58
DBL f_ridged_mf(FPUContext *ctx, DBL *ptr, unsigned int fn); // 59
DBL f_rounded_box(FPUContext *ctx, DBL *ptr, unsigned int fn); // 60
DBL f_sphere(FPUContext *ctx, DBL *ptr, unsigned int fn); // 61
DBL f_spikes(FPUContext *ctx, DBL *ptr, unsigned int fn); // 62
DBL f_spikes_2d(FPUContext *ctx, DBL *ptr, unsigned int fn); // 63
DBL f_spiral(FPUContext *ctx, DBL *ptr, unsigned int fn); // 64
DBL f_steiners_roman(FPUContext *ctx, DBL *ptr, unsigned int fn); // 65
DBL f_strophoid(FPUContext *ctx, DBL *ptr, unsigned int fn); // 66
DBL f_strophoid_2d(FPUContext *ctx, DBL *ptr, unsigned int fn); // 67
DBL f_superellipsoid(FPUContext *ctx, DBL *ptr, unsigned int fn); // 68
DBL f_th(FPUContext *ctx, DBL *ptr, unsigned int fn); // 69
DBL f_torus(FPUContext *ctx, DBL *ptr, unsigned int fn); // 70
DBL f_torus2(FPUContext *ctx, DBL *ptr, unsigned int fn); // 71
DBL f_torus_gumdrop(FPUContext *ctx, DBL *ptr, unsigned int fn); // 72
DBL f_umbrella(FPUContext *ctx, DBL *ptr, unsigned int fn); // 73
DBL f_witch_of_agnesi(FPUContext *ctx, DBL *ptr, unsigned int fn); // 74
DBL f_witch_of_agnesi_2d(FPUContext *ctx, DBL *ptr, unsigned int fn); // 75
DBL f_noise3d(FPUContext *ctx, DBL *ptr, unsigned int fn); // 76
DBL f_pattern(FPUContext *ctx, DBL *ptr, unsigned int fn); // 77
DBL f_noise_generator(FPUContext *ctx, DBL *ptr, unsigned int fn); // 78

void f_pigment(FPUContext *ctx, DBL *ptr, unsigned int fn, unsigned int sp); // 0
void f_transform(FPUContext *ctx, DBL *ptr, unsigned int fn, unsigned int sp); // 1
void f_spline(FPUContext *ctx, DBL *ptr, unsigned int fn, unsigned int sp); // 2


/*****************************************************************************
* Global variables
******************************************************************************/

const Trap POVFPU_TrapTable[] =
{
    { f_algbr_cyl1,              5 + 3 }, // 0
    { f_algbr_cyl2,              5 + 3 }, // 1
    { f_algbr_cyl3,              5 + 3 }, // 2
    { f_algbr_cyl4,              5 + 3 }, // 3
    { f_bicorn,                  2 + 3 }, // 4
    { f_bifolia,                 2 + 3 }, // 5
    { f_blob,                    5 + 3 }, // 6
    { f_blob2,                   4 + 3 }, // 7
    { f_boy_surface,             2 + 3 }, // 8
    { f_comma,                   1 + 3 }, // 9
    { f_cross_ellipsoids,        4 + 3 }, // 10
    { f_crossed_trough,          1 + 3 }, // 11
    { f_cubic_saddle,            1 + 3 }, // 12
    { f_cushion,                 1 + 3 }, // 13
    { f_devils_curve,            1 + 3 }, // 14
    { f_devils_curve_2d,         6 + 3 }, // 15
    { f_dupin_cyclid,            6 + 3 }, // 16
    { f_ellipsoid,               3 + 3 }, // 17
    { f_enneper,                 1 + 3 }, // 18
    { f_flange_cover,            4 + 3 }, // 19
    { f_folium_surface,          3 + 3 }, // 20
    { f_folium_surface_2d,       6 + 3 }, // 21
    { f_glob,                    1 + 3 }, // 22
    { f_heart,                   1 + 3 }, // 23
    { f_helical_torus,          10 + 3 }, // 24
    { f_helix1,                  7 + 3 }, // 25
    { f_helix2,                  7 + 3 }, // 26
    { f_hex_x,                   1 + 3 }, // 27
    { f_hex_y,                   1 + 3 }, // 28
    { f_hetero_mf,               6 + 3 }, // 29
    { f_hunt_surface,            1 + 3 }, // 30
    { f_hyperbolic_torus,        3 + 3 }, // 31
    { f_isect_ellipsoids,        4 + 3 }, // 32
    { f_kampyle_of_eudoxus,      3 + 3 }, // 33
    { f_kampyle_of_eudoxus_2d,   6 + 3 }, // 34
    { f_klein_bottle,            1 + 3 }, // 35
    { f_kummer_surface_v1,       1 + 3 }, // 36
    { f_kummer_surface_v2,       4 + 3 }, // 37
    { f_lemniscate_of_gerono,    1 + 3 }, // 38
    { f_lemniscate_of_gerono_2d, 6 + 3 }, // 39
    { f_mesh1,                   5 + 3 }, // 40
    { f_mitre,                   1 + 3 }, // 41
    { f_nodal_cubic,             1 + 3 }, // 42
    { f_odd,                     1 + 3 }, // 43
    { f_ovals_of_cassini,        4 + 3 }, // 44
    { f_paraboloid,              1 + 3 }, // 45
    { f_parabolic_torus,         3 + 3 }, // 46
    { f_ph,                      0 + 3 }, // 47
    { f_pillow,                  1 + 3 }, // 48
    { f_piriform,                1 + 3 }, // 49
    { f_piriform_2d,             7 + 3 }, // 50
    { f_poly4,                   5 + 3 }, // 51
    { f_polytubes,               6 + 3 }, // 52
    { f_quantum,                 1 + 3 }, // 53
    { f_quartic_paraboloid,      1 + 3 }, // 54
    { f_quartic_saddle,          1 + 3 }, // 55
    { f_quartic_cylinder,        3 + 3 }, // 56
    { f_r,                       0 + 3 }, // 57
    { f_ridge,                   6 + 3 }, // 58
    { f_ridged_mf,               6 + 3 }, // 59
    { f_rounded_box,             4 + 3 }, // 60
    { f_sphere,                  1 + 3 }, // 61
    { f_spikes,                  5 + 3 }, // 62
    { f_spikes_2d,               4 + 3 }, // 63
    { f_spiral,                  6 + 3 }, // 64
    { f_steiners_roman,          1 + 3 }, // 65
    { f_strophoid,               4 + 3 }, // 66
    { f_strophoid_2d,            7 + 3 }, // 67
    { f_superellipsoid,          2 + 3 }, // 68
    { f_th,                      0 + 3 }, // 69
    { f_torus,                   2 + 3 }, // 70
    { f_torus2,                  3 + 3 }, // 71
    { f_torus_gumdrop,           1 + 3 }, // 72
    { f_umbrella,                1 + 3 }, // 73
    { f_witch_of_agnesi,         2 + 3 }, // 74
    { f_witch_of_agnesi_2d,      6 + 3 }, // 75
    { f_noise3d,                 0 + 3 }, // 76
    { f_pattern,                 0 + 3 }, // 77
    { f_noise_generator,         1 + 3 }, // 78
    { NULL, 0 }
};

const TrapS POVFPU_TrapSTable[] =
{
    { f_pigment,                 0 + 3 }, // 0
    { f_transform,               0 + 3 }, // 1
    { f_spline,                  0 + 1 }, // 2
    { NULL, 0 }
};

const unsigned int POVFPU_TrapTableSize = 79;
const unsigned int POVFPU_TrapSTableSize = 3;


/*****************************************************************************
* Functions
******************************************************************************/

DBL f_algbr_cyl1(FPUContext *ctx, DBL *ptr, unsigned int) // 0
{
    DBL r, x2, y2, th;

    x2 = PARAM_X * PARAM_X;
    y2 = PARAM_Y * PARAM_Y;

    ROT2D(PARAM(2),PARAM(3),PARAM(4))

    PARAM_X = fabs(PARAM_X);

    r = -(x2 * PARAM_X - x2 + y2);

    return (-min(PARAM(1), max(PARAM(0) * r, -PARAM(1))));
}

DBL f_algbr_cyl2(FPUContext *ctx, DBL *ptr, unsigned int) // 1
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, th;
    ROT2D(PARAM(2),PARAM(3),PARAM(4))
    r=-( 2*x2*x2 -3*x2*PARAM_Y +y2 -2*y2*PARAM_Y +y2*y2 );
    return( min(PARAM(1), max(PARAM(0)*r,-PARAM(1))) );
}

DBL f_algbr_cyl3(FPUContext *ctx, DBL *ptr, unsigned int) // 2
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, th;
    ROT2D(PARAM(2),PARAM(3),PARAM(4))
    r=-( x2*x2 +x2*y2 -2*x2*PARAM_Y -PARAM_X*y2 +y2 );
    return( min(PARAM(1), max(PARAM(0)*r,-PARAM(1))) );
}

DBL f_algbr_cyl4(FPUContext *ctx, DBL *ptr, unsigned int) // 3
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, th;
    ROT2D(PARAM(2),PARAM(3),PARAM(4))
    r=-( x2*x2 +y2*y2 +2*x2*y2 +3*x2*PARAM_Y -y2*PARAM_Y );
    return( min(PARAM(1), max(PARAM(0)*r,-PARAM(1))) );
}

DBL f_bicorn(FPUContext *ctx, DBL *ptr, unsigned int) // 4
{
    DBL r, r2,x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z;
    r = PARAM(1)*PARAM(1);
    r2=(x2 + z2 + 2*PARAM(1)*PARAM_Y - r);
    r = ( y2*(r - (x2 + z2)) - r2*r2);
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_bifolia(FPUContext *ctx, DBL *ptr, unsigned int) // 5
{
    DBL r, x2=PARAM_X*PARAM_X, z2=PARAM_Z*PARAM_Z;
    r=x2+PARAM_Y*PARAM_Y+z2;
    r= -(r*r - PARAM(1)*(x2 + z2)*PARAM_Y);
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_blob(FPUContext *ctx, DBL *ptr, unsigned int) // 6
{
    DBL r,r2,y2,z2,temp;
    /*   blob  */
    y2=PARAM_Y*PARAM_Y; z2=PARAM_Z*PARAM_Z;
    r2= PARAM(0)*0.5;
    r=((PARAM_X+r2)*(PARAM_X+r2)+y2+z2)*PARAM(2)*PARAM(2);
    if (r>1.)
        r=1.;
    temp=( (PARAM_X - r2)*(PARAM_X - r2)+y2+z2)*PARAM(4)*PARAM(4);
    if (temp>1)
        temp=1.;
    return(-(PARAM(1) *(1.-r) *(1.-r)  +PARAM(3) *(1.-temp)*(1.-temp) ));
}

DBL f_blob2(FPUContext *ctx, DBL *ptr, unsigned int) // 7
{
    DBL r,x2,y2,z2;
    x2=PARAM_X*PARAM_X; y2=PARAM_Y*PARAM_Y; z2=PARAM_Z*PARAM_Z;
    /*  f= f1 + f2 */
    r=exp(-(x2+y2+z2)*PARAM(1)) + exp(-((PARAM_X-PARAM(0))*(PARAM_X-PARAM(0))+y2+z2)*PARAM(1));
    return(PARAM(3)-r*PARAM(2));
}

DBL f_boy_surface(FPUContext *ctx, DBL *ptr, unsigned int) // 8
{
    DBL r, r2,ph,x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z;
    r2=1-PARAM_Z; ph=x2+y2;
    r= PARAM(1)*(64*r2*r2*r2*z2*PARAM_Z- 48*r2*r2*z2*(3*x2+3*y2+2*z2)+
    12*r2*PARAM_Z*(27*ph*ph-24*z2*ph+ 36*sqrt(2.0)*PARAM_Y*PARAM_Z*(y2-3*x2)+4*z2*z2)+
    (9*x2+9*y2-2*z2)*(-81*ph*ph-72*z2*ph+
    108*sqrt(2.0)*PARAM_X*PARAM_Z*(x2-3*y2)+4*z2*z2) );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_comma(FPUContext *ctx, DBL *ptr, unsigned int) // 9
{
    DBL r,th,temp;
    if ((PARAM_X==0)&&(PARAM_Z==0))
        PARAM_X=0.0001;
    th=atan2(PARAM_Z,PARAM_X);
    r=sqrt( (PARAM_X + PARAM(0)*0.25)*(PARAM_X + PARAM(0)*0.25) + PARAM_Z*PARAM_Z);
    temp= cos(th*0.5) * PARAM(0)*0.5 -sqrt((r- PARAM(0)*0.75)*(r - PARAM(0)*0.75)+PARAM_Y*PARAM_Y);
    temp= min(PARAM_Z, temp);
    r= PARAM(0)*0.5   - sqrt( (PARAM_X - PARAM(0)*0.5)*(PARAM_X - PARAM(0)*0.5) + PARAM_Z*PARAM_Z + PARAM_Y*PARAM_Y);
    return(-(DBL)max(temp,r));
}

DBL f_cross_ellipsoids(FPUContext *ctx, DBL *ptr, unsigned int) // 10
{
    DBL r,x2,y2,z2;
    x2=PARAM_X*PARAM_X; y2=PARAM_Y*PARAM_Y; z2=PARAM_Z*PARAM_Z;
    /*  f= max (f1, f2, f3) */
    r=max(exp(-(x2*PARAM(0)+y2*PARAM(0)+z2)*PARAM(1)), exp(-(x2*PARAM(0)+y2+z2*PARAM(0))*PARAM(1)));
    r=max(r, exp(-(x2+y2*PARAM(0)+z2*PARAM(0))*PARAM(1)));
    return(PARAM(3)-r*PARAM(2));
}

DBL f_crossed_trough(FPUContext *ctx, DBL *ptr, unsigned int) // 11
{
    DBL r;
    r=( PARAM_X*PARAM_X * PARAM_Z*PARAM_Z - PARAM_Y  );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_cubic_saddle(FPUContext *ctx, DBL *ptr, unsigned int) // 12
{
    DBL r;
    r=-(PARAM_X*PARAM_X*PARAM_X - PARAM_Y*PARAM_Y*PARAM_Y - PARAM_Z);
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_cushion(FPUContext *ctx, DBL *ptr, unsigned int) // 13
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z;
    r= (z2*x2 - z2*z2 - 2*PARAM_Z*x2 + 2*z2*PARAM_Z + x2 - z2
    -(x2 - PARAM_Z)*(x2 - PARAM_Z) - y2*y2 - 2*x2*y2 - y2*z2 + 2*y2*PARAM_Z + y2);
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_devils_curve(FPUContext *ctx, DBL *ptr, unsigned int) // 14
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z;
    r=-(x2*x2 + 2*x2*z2 - 0.36*x2 - y2*y2 + 0.25*y2 + z2*z2);
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_devils_curve_2d(FPUContext *ctx, DBL *ptr, unsigned int) // 15
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, th;
    ROT2D(PARAM(3),PARAM(4),PARAM(5))
    r=-( x2 * (x2 - PARAM(1)*PARAM(1)) -  y2 * (y2 - PARAM(2)*PARAM(2)) );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_dupin_cyclid(FPUContext *ctx, DBL *ptr, unsigned int) // 16
{
    DBL r, r2,x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z, ph,th, p1,p2,p3,p4;
    r2=PARAM(5)*PARAM(5);
    ph=PARAM(2)*PARAM(2); th=PARAM(4)*PARAM(4);
    p1= ph - th - (PARAM(3) + PARAM(1))*(PARAM(3) + PARAM(1));
    p2= ph - th - (PARAM(3) - PARAM(1))*(PARAM(3) - PARAM(1));
    p3= PARAM(3)*PARAM(3); p4=PARAM(1)*PARAM(1);
    r=-( p1*p2* (x2*x2+y2*y2+z2*z2)+ 2*(p1*p2* (x2*y2+x2*z2+y2*z2))+
    2*r2*((-th-p3 + ph+p4)* (2*PARAM_X*PARAM(3)+2*PARAM_Y*PARAM(4)-r2)-4*PARAM(4)*ph*PARAM_Y)*
    (x2+y2+z2)+ 4*r2*r2*(PARAM(3)*PARAM_X+PARAM(4)*PARAM_Y)
    *(-r2+PARAM(4)*PARAM_Y+PARAM(3)*PARAM_X)+ 4*r2*r2*p4*y2+ r2*r2*r2*r2);
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_ellipsoid(FPUContext *ctx, DBL *ptr, unsigned int) // 17
{
    /*   sphere  */
    return(sqrt(PARAM_X*PARAM_X * PARAM(0)*PARAM(0) + PARAM_Y*PARAM_Y * PARAM(1)*PARAM(1) + PARAM_Z*PARAM_Z * PARAM(2)*PARAM(2)));
}

DBL f_enneper(FPUContext *ctx, DBL *ptr, unsigned int) // 18
{
    DBL r, r2,x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z;
    if (fabs(PARAM_Z)<0.2)
        PARAM_Z=0.2;
    r =((y2-x2)/(2*PARAM_Z)+2*z2/9+2/3);                  // TODO FIXME - was this supposed to be 2.0/3.0 ??
    r2=((y2-x2)/(4*PARAM_Z)-(1/4)*(x2+y2+(8/9)*z2)+2/9);  // TODO FIXME - was this supposed to be 1.0/4.0, 8.0/9.0 and 2.0/9.0 respectively ??
    r=-( r*r*r -6*r2*r2);
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_flange_cover(FPUContext *ctx, DBL *ptr, unsigned int) // 19
{
    DBL r,x2,y2,z2;
    x2=PARAM_X*PARAM_X; y2=PARAM_Y*PARAM_Y; z2=PARAM_Z*PARAM_Z;
    /*  f= f1 + f2 + f3 */
    r=exp(-(x2*PARAM(0)+y2*PARAM(0)+z2)*PARAM(1)) + exp(-(x2*PARAM(0)+y2+z2*PARAM(0))*PARAM(1)) +
    exp(-(x2+y2*PARAM(0)+z2*PARAM(0))*PARAM(1));
    return(PARAM(3)-r*PARAM(2));
}

DBL f_folium_surface(FPUContext *ctx, DBL *ptr, unsigned int) // 20
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z;
    r=((y2 + z2) * (1+(PARAM(2) - 4*PARAM(1))*PARAM_X)+x2*(1 + PARAM(2)));
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_folium_surface_2d(FPUContext *ctx, DBL *ptr, unsigned int) // 21
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z, th;
    ROT2D(PARAM(3),PARAM(4),PARAM(5))
    r = -( y2  * (1 + (PARAM(2) - 4*PARAM(1))*PARAM_X) + x2*(1 + PARAM(2)));
    return( min(10., max(PARAM(0)*r, -10.)) );
}

DBL f_glob(FPUContext *ctx, DBL *ptr, unsigned int) // 22
{
    DBL r, x2=PARAM_X*PARAM_X;
    r= ( 0.5*x2*x2*PARAM_X + 0.5*x2*x2 - (PARAM_Y*PARAM_Y + PARAM_Z*PARAM_Z)  );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_heart(FPUContext *ctx, DBL *ptr, unsigned int) // 23
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z;
    r=-( (2*x2+y2+z2-1)*(2*x2+y2+z2-1)*(2*x2+y2+z2-1)-
    0.1*x2*z2*PARAM_Z-y2*z2*PARAM_Z  );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_helical_torus(FPUContext *ctx, DBL *ptr, unsigned int) // 24
{
    DBL r,r2, temp,th,ph;
    r2=sqrt(PARAM_X*PARAM_X+PARAM_Z*PARAM_Z);
    if ((PARAM_X==0)&&(PARAM_Z==0))
        PARAM_X=0.000001;
    ph=atan2(PARAM_X,PARAM_Z);
    temp=atan2(r2 - PARAM(0), PARAM_Y);
    r=sqrt((r2-PARAM(0))*(r2-PARAM(0))+PARAM_Y*PARAM_Y);
    th=fmod(ph*PARAM(1)+temp*PARAM(2), TWO_M_PI);
    if (th<0)
        th+=TWO_M_PI;
    temp=atan2((th-M_PI)*PARAM(8), r-PARAM(5));
    temp=cos(temp*PARAM(7)+ph*PARAM(9))*PARAM(3)+PARAM(4);
    r2=PARAM(4)+PARAM(5)-PARAM(0)-r2;
    th=th - M_PI;
    temp=temp-min(sqrt((r-PARAM(5))*(r-PARAM(5))+th*th*PARAM(6)), (r+PARAM(5)));
    return(-max(r2,temp));
}

DBL f_helix1(FPUContext *ctx, DBL *ptr, unsigned int) // 25
{
    DBL r, r2, temp, th, ph, x2;
    /* helix                                     *
     *   p[0] : number of helix                  *
     *   p[1] : frequency                        *
     *   p[2] : minor radius                     *
     *   p[3] : major radius                     *
     *   p[4] : shape parameter                  *
     *   p[5] : cross section                    *
     *             p[5] = 1:  circle             *
     *                  = 2:  diamond            *
     *                  < 1:  rectangle(rounded) *
     *   p[6] : rotation angle for p[5]<1        */

    r = sqrt(PARAM_X * PARAM_X + PARAM_Z * PARAM_Z);
    if ((PARAM_X == 0) && (PARAM_Z == 0))
        PARAM_X = 0.000001;
    th = atan2(PARAM_Z, PARAM_X);
    th = fmod(th * PARAM(0) + PARAM_Y * PARAM(1) * PARAM(0), TWO_M_PI);
    if (th < 0)
        th += TWO_M_PI;
    PARAM_Z = (th - M_PI) / PARAM(4) / (PARAM(1) * PARAM(0));

    PARAM_X = r - PARAM(3);
    if (PARAM(5) == 1)
        r2 = sqrt(PARAM_X * PARAM_X + PARAM_Z * PARAM_Z);
    else
    {
        if (PARAM(6) != 0)
        {
            th = cos(PARAM(6) * M_PI_180);
            ph = sin(PARAM(6) * M_PI_180);
            x2 = PARAM_X * th - PARAM_Z * ph;
            PARAM_Z = PARAM_X * ph + PARAM_Z * th;
            PARAM_X = x2;
        }
        if (PARAM(5) != 0)
        {
            temp = 2. / PARAM(5);
            r2 = pow((pow(fabs(PARAM_X), temp) + pow(fabs(PARAM_Z), temp)), PARAM(5) *.5);
        }
        else
            r2 = max(fabs(PARAM_X), fabs(PARAM_Z));
    }

    return (-PARAM(2) + min((PARAM(3) + r), r2));
}

DBL f_helix2(FPUContext *ctx, DBL *ptr, unsigned int) // 26
{
    DBL th, ph, x2, z2, r2, temp;
    /* helical shape  for (minor radius>major radius  *
     *    cross section   p[5] same as NFUNCTION = 6      */
    th = PARAM_Y * PARAM(1);
    ph = cos(th);
    th = sin(th);
    x2 = PARAM_X - PARAM(3) * ph;
    z2 = PARAM_Z - PARAM(3) * th;
    PARAM_X = x2 * ph + z2 * th;
    PARAM_Z = (-x2 * th + z2 * ph);

    if (PARAM(5) == 1)
        return (sqrt(PARAM_X * PARAM_X + PARAM_Z * PARAM_Z) - PARAM(2));
    if (PARAM(5) != 0)
    {
        temp = 2. / PARAM(5);
        r2 = pow((pow(fabs(PARAM_X), temp) + pow(fabs(PARAM_Z), temp)), PARAM(5) *.5);
    }
    else
        r2 = max(fabs(PARAM_X), fabs(PARAM_Z));

    return (r2 - PARAM(2));
}

DBL f_hex_x(FPUContext *ctx, DBL *ptr, unsigned int) // 27
{
    DBL x1,y1,x2,y2, th;
    x1=fabs(fmod(fabs(PARAM_X), sqrt(3.0))-sqrt(3.0)/2);
    y1=fabs(fmod(fabs(PARAM_Y), 3)-1.5);
    x2=sqrt(3.0)/2-x1;
    y2=1.5-y1;
    if ((x1*x1+y1*y1)>(x2*x2+y2*y2))
    {
        x1=x2;
        y1=y2;
    }
    if ((x1==0)&&(y1==0))
        PARAM_X=0.000001;
    th=atan2(y1,x1);
    if (th<M_PI/6)
        return(x1);
    else
    {
        x1=cos(M_PI/3)*x1+sin(M_PI/3)*y1;
        return(x1);
    }
}

DBL f_hex_y(FPUContext *ctx, DBL *ptr, unsigned int) // 28
{
    DBL x1,y1,x2,y2, th;
    x1=fabs(fmod(fabs(PARAM_X), sqrt(3.0))-sqrt(3.0)/2);
    y1=fabs(fmod(fabs(PARAM_Y), 3)-1.5);
    x2=sqrt(3.0)/2-x1;
    y2=1.5-y1;
    if ((x1*x1+y1*y1)>(x2*x2+y2*y2))
    {
        x1=x2;
        y1=y2;
    }
    if ((x1==0)&&(y1==0))
        PARAM_X=0.000001;
    th=atan2(y1,x1);
    if (th<M_PI/6)
        return(y1);
    else
    {
        y1=-sin(M_PI/3)*x1+cos(M_PI/3)*y1;
        return(fabs(y1));
    }
}

DBL f_hetero_mf(FPUContext *ctx, DBL *ptr, unsigned int fn) // 29
{
    DBL signal;
    Vector3d V1;
    DBL rem;

    V1 = Vector3d(PARAM_X, PARAM_Y, PARAM_Z);

    int ngen = (int)PARAM(5) & 3;
    signal = (Noise(V1, ngen)*2.0 - 1.0) + PARAM(3);
    V1 *= PARAM(1);

    DBL p1_2_mp0 = pow(PARAM(1), -PARAM(0)), ea = p1_2_mp0;
    for (int i = 1; i < PARAM(2); i++)
    {
        // make a noisy increment and scale it by f^(-H)
        DBL inc = ((Noise(V1,ngen)*2.0 - 1.0) + PARAM(3)) * ea;

        // scale the increment by (the current signal)^PARAM(4) at V1
        //   so that PARAM(4)=0 gives 'straight' 1/f, PARAM(4)=1
        // gives heterogenous fractal, etc
        for (int p = (int) PARAM(4); p > 0; --p)
            inc *= signal;

        signal += inc;
        // go to next 'octave'
        V1 *= PARAM(1);
        ea *= p1_2_mp0;
    }

    rem = PARAM(2) - (int) PARAM(2);
    if(rem != 0.0)
    {
        // do something with fraction part of octave
        DBL inc = ((Noise(V1,ngen)*2.0-1.0) + PARAM(3)) * ea;
        signal += rem * inc * signal;
    }

    return signal;
}

DBL f_hunt_surface(FPUContext *ctx, DBL *ptr, unsigned int) // 30
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z;
    r=-( 4*(x2+y2+z2-13)*(x2+y2+z2-13)*(x2+y2+z2-13) +
    27*(3*x2+y2-4*z2-12)*(3*x2+y2-4*z2-12)  );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_hyperbolic_torus(FPUContext *ctx, DBL *ptr, unsigned int) // 31
{
    DBL r, ph,th,x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z;
    ph=PARAM(1)*PARAM(1); th=PARAM(2)*PARAM(2);
    r=-( x2*x2 + 2*x2*y2 - 2*x2*z2 - 2*(ph+th)*x2 + y2*y2 -
    2*y2*z2 + 2*(ph-th)*y2 + z2*z2 + 2*(ph+th)*z2 + (ph-th)*(ph-th));
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_isect_ellipsoids(FPUContext *ctx, DBL *ptr, unsigned int) // 32
{
    DBL r,x2,y2,z2;
    x2=PARAM_X*PARAM_X; y2=PARAM_Y*PARAM_Y; z2=PARAM_Z*PARAM_Z;
    /* f= min (f1, f2, f3) */
    r=min(exp(-(x2*PARAM(0)+y2*PARAM(0)+z2)*PARAM(1)), exp(-(x2*PARAM(0)+y2+z2*PARAM(0))*PARAM(1)));
    r=min(r, exp(-(x2+y2*PARAM(0)+z2*PARAM(0))*PARAM(1)));
    return(PARAM(3)-r*PARAM(2));
}

DBL f_kampyle_of_eudoxus(FPUContext *ctx, DBL *ptr, unsigned int) // 33
{
    DBL r, x2=PARAM_X*PARAM_X;
    r=-( (PARAM_Y*PARAM_Y + PARAM_Z*PARAM_Z) - PARAM(2)*PARAM(2) * x2*x2 +  PARAM(2)*PARAM(2) * PARAM(1)*PARAM(1) * x2  );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_kampyle_of_eudoxus_2d(FPUContext *ctx, DBL *ptr, unsigned int) // 34
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, th;
    ROT2D(PARAM(3),PARAM(4),PARAM(5))
    r=-( y2  - PARAM(2)*PARAM(2) * x2*x2 +    PARAM(2)*PARAM(2) * PARAM(1)*PARAM(2) * x2  );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_klein_bottle(FPUContext *ctx, DBL *ptr, unsigned int) // 35
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z;
    r=-( (x2+y2+z2+2*PARAM_Y-1)*((x2+y2+z2-2*PARAM_Y-1)*(x2+y2+z2-2*PARAM_Y-1)-8*z2)+
    16*PARAM_X*PARAM_Z*(x2+y2+z2-2*PARAM_Y-1) );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_kummer_surface_v1(FPUContext *ctx, DBL *ptr, unsigned int) // 36
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z;
    r=-( x2*x2+y2*y2+z2*z2-x2-y2-z2-x2*y2-x2*z2-y2*z2+1  );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_kummer_surface_v2(FPUContext *ctx, DBL *ptr, unsigned int) // 37
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z;
    r=-( x2*x2+y2*y2+z2*z2+PARAM(1)*(x2+y2+z2)+PARAM(2)*(x2*y2+x2*z2+y2*z2)+PARAM(3)*PARAM_X*PARAM_Y*PARAM_Z-1 );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_lemniscate_of_gerono(FPUContext *ctx, DBL *ptr, unsigned int) // 38
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z;
    r=-(  x2*x2 - x2 + y2 + z2  );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_lemniscate_of_gerono_2d(FPUContext *ctx, DBL *ptr, unsigned int) // 39
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, th;
    ROT2D(PARAM(3),PARAM(4),PARAM(5))
    r=-( y2 - PARAM(2)*PARAM(2) * PARAM(1)*PARAM(1) * x2 +   PARAM(2)*PARAM(2)*x2*x2  );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_mesh1(FPUContext *ctx, DBL *ptr, unsigned int) // 40
{
    DBL th, ph, r, r2, temp;
    th = M_PI / PARAM(0);
    ph = M_PI / PARAM(1);
    r = fmod(PARAM_X, PARAM(0) * 2);
    if (r < 0)
        r += PARAM(0) * 2;
    r = fabs(r - PARAM(0)) * PARAM(2);
    r2 = (PARAM_Y - cos(PARAM_Z * ph) * PARAM(3)) * PARAM(4);
    temp = -sqrt(r2 * r2 + r * r);

    r = fmod(PARAM_X - PARAM(0), PARAM(0) * 2);
    if (r < 0)
        r += PARAM(0) * 2;
    r = fabs(r - PARAM(0)) * PARAM(2);
    r2 = (PARAM_Y + cos(PARAM_Z * ph) * PARAM(3)) * PARAM(4);
    temp = max(-sqrt(r2 * r2 + r * r), temp);

    r = fmod(PARAM_Z, PARAM(1) * 2);
    if (r < 0)
        r += PARAM(1) * 2;
    r = fabs(r - PARAM(1)) * PARAM(2);
    r2 = (PARAM_Y + cos(PARAM_X * th) * PARAM(3)) * PARAM(4);
    temp = max(-sqrt(r2 * r2 + r * r), temp);

    r = fmod(PARAM_Z - PARAM(1), PARAM(1) * 2);
    if (r < 0)
        r += PARAM(1) * 2;
    r = fabs(r - PARAM(1)) * PARAM(2);
    r2 = (PARAM_Y - cos(PARAM_X * th) * PARAM(3)) * PARAM(4);

    return (-max(-sqrt(r2 * r2 + r * r), temp));
}

DBL f_mitre(FPUContext *ctx, DBL *ptr, unsigned int) // 41
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z;
    r=-( 4*x2*(x2 + y2 + z2) - y2*(1 - y2 - z2)  );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_nodal_cubic(FPUContext *ctx, DBL *ptr, unsigned int) // 42
{
    DBL r;
    r=-( PARAM_Y*PARAM_Y*PARAM_Y + PARAM_Z*PARAM_Z*PARAM_Z - 6*PARAM_Y*PARAM_Z );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_odd(FPUContext *ctx, DBL *ptr, unsigned int) // 43
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z;
    r=( z2*x2 - z2*z2 - 2*PARAM_Z*x2 + 2*z2*PARAM_Z + x2 - z2 -
    (x2 - PARAM_Z)*(x2 - PARAM_Z) - y2*y2 - 2*y2*x2 - y2*z2 + 2*y2*PARAM_Z + y2  );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_ovals_of_cassini(FPUContext *ctx, DBL *ptr, unsigned int) // 44
{
    DBL r, r2,x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z;
    r2 = (x2 + y2 + z2 + PARAM(1)*PARAM(1));
    r  = -(r2*r2 - PARAM(3)*PARAM(1)*PARAM(1)*(x2 + z2) - PARAM(2)*PARAM(2) );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_paraboloid(FPUContext *ctx, DBL *ptr, unsigned int) // 45
{
    DBL r;
    r=-( PARAM_X*PARAM_X - PARAM_Y + PARAM_Z*PARAM_Z);
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_parabolic_torus(FPUContext *ctx, DBL *ptr, unsigned int) // 46
{
    DBL r, ph,th, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z;
    ph=PARAM(1)*PARAM(1); th=PARAM(2)*PARAM(2);
    r=-(  x2*x2 + 2*x2*y2 - 2*x2*PARAM_Z - (ph+th)*x2 + y2*y2 - 2*y2*PARAM_Z +
    (ph-th)*y2 + z2 + (ph+th)*PARAM_Z +  (ph-th)* (ph-th)  );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_ph(FPUContext *ctx, DBL *ptr, unsigned int) // 47
{
    return( atan2(sqrt(PARAM_X*PARAM_X + PARAM_Z*PARAM_Z ),PARAM_Y) );
}

DBL f_pillow(FPUContext *ctx, DBL *ptr, unsigned int) // 48
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z;
    r=( x2*x2 + y2*y2 + z2*z2 - (x2 + y2 + z2)  );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_piriform(FPUContext *ctx, DBL *ptr, unsigned int) // 49
{
    DBL r, x2=PARAM_X*PARAM_X;
    r=-( (x2*x2 - x2*PARAM_X) + PARAM_Y*PARAM_Y + PARAM_Z*PARAM_Z  );
    return( -min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_piriform_2d(FPUContext *ctx, DBL *ptr, unsigned int) // 50
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y,th;
    ROT2D(PARAM(4),PARAM(5),PARAM(6))
    r=-( PARAM_Y*PARAM_Y - PARAM(1) * PARAM(3)* PARAM(3) * x2*PARAM_X -  PARAM(2) * PARAM(3)*PARAM(3) * x2*x2  );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_poly4(FPUContext *ctx, DBL *ptr, unsigned int) // 51
{
    /*  cylindrical shape   */
    DBL y2, temp;
    y2=PARAM_Y*PARAM_Y;
    temp=(PARAM(0)+PARAM(1)*PARAM_Y+PARAM(2)*y2 + PARAM(3)*y2*PARAM_Y +PARAM(4)*y2*y2);
    if (temp<-5.)
        temp=-5.;
    return (-temp+sqrt(PARAM_X*PARAM_X+PARAM_Z*PARAM_Z));
}

DBL f_polytubes(FPUContext *ctx, DBL *ptr, unsigned int) // 52
{
    DBL x2,y2,z2,r,r2;
    int i;
    y2=PARAM_Y*PARAM_Y;
    r2=PARAM(1)+PARAM(2)*PARAM_Y+PARAM(3)*y2+PARAM(4)*y2*PARAM_Y+  PARAM(5)*y2*y2;
    r=-10000;
    for (i=0;i<(int)PARAM(0);i++)
    {
        x2 = PARAM_X - r2*sin(2*M_PI/PARAM(0)*i);
        z2 = PARAM_Z - r2*cos(2*M_PI/PARAM(0)*i);
        r= max(r,-sqrt( x2*x2 + z2*z2 ) );
    }
    return(-r);
}

DBL f_quantum(FPUContext *ctx, DBL *ptr, unsigned int) // 53
{
    DBL r, th,temp;
    /*  well known function in quantum mechanics */
    if ((PARAM_X==0)&&(PARAM_Z==0))
        PARAM_X=1e-6;
    r=sqrt(PARAM_X*PARAM_X+PARAM_Z*PARAM_Z);
    th=cos(atan2(r,PARAM_Y));
    r=sqrt(r*r+PARAM_Y*PARAM_Y)*2.;
    temp=r*r*exp(-r*0.33333333)*(3.*th*th-1);
    return (temp*temp-12.0)*(-0.1);
}

DBL f_quartic_paraboloid(FPUContext *ctx, DBL *ptr, unsigned int) // 54
{
    DBL r, x2=PARAM_X*PARAM_X, z2=PARAM_Z*PARAM_Z;
    r=-( x2*x2 + z2*z2 - PARAM_Y  );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_quartic_saddle(FPUContext *ctx, DBL *ptr, unsigned int) // 55
{
    DBL r, x2=PARAM_X*PARAM_X, z2=PARAM_Z*PARAM_Z;
    r=-( x2*x2 - z2*z2 - PARAM_Y  );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_quartic_cylinder(FPUContext *ctx, DBL *ptr, unsigned int) // 56
{
    DBL r, x2=PARAM_X*PARAM_X, z2=PARAM_Z*PARAM_Z;
    r=-( (x2 + z2) * PARAM_Y*PARAM_Y + PARAM(2)*PARAM(2) * (x2 + z2) - PARAM(2)*PARAM(2) * PARAM(1)*PARAM(1)  );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_r(FPUContext *ctx, DBL *ptr, unsigned int) // 57
{
    return( sqrt(PARAM_X*PARAM_X + PARAM_Y*PARAM_Y + PARAM_Z*PARAM_Z ) );
}

DBL f_ridge(FPUContext *ctx, DBL *ptr, unsigned int) // 58
{
    Vector3d EPoint = Vector3d(PARAM_X, PARAM_Y, PARAM_Z);
    int i;
    DBL Lambda, Omega, l, o;
    DBL value, v, resid, tot = 1.0, off, ridge, scale, rscale;
    Vector3d temp;
    int Octaves;
    int ngen = (int)PARAM(5) & 0x03;

    Lambda = l = PARAM(0);
    Octaves = (int)PARAM(1);
    Omega = o = PARAM(2);
    off = PARAM(3);
    ridge = PARAM(4);

    rscale = 1.0 / max(ridge, 1.0 - ridge);
    scale = 1.0 / max(off, 1.0 - off);
    resid = PARAM(1) - (DBL)Octaves;

    v = fabs(Noise(EPoint,ngen) - ridge) * rscale;
    value = (v - off);

    for (i = 2; i <= Octaves; i++)
    {
        temp = EPoint * l;
        v = fabs(Noise(temp,ngen) - ridge) * rscale;
        value += o * (v - off);
        tot += o;
        l *= Lambda;
        o *= Omega;
    }
    if (0.0 != resid)
    {
        temp = EPoint * l;
        v = fabs(Noise(temp,ngen) - ridge) * rscale;
        value += o * (v - off) * resid;
        tot += o * resid;
    }

    return (value * scale / tot);
}

DBL f_ridged_mf(FPUContext *ctx, DBL *ptr, unsigned int fn) // 59
{
    FunctionCode *f = ctx->functionvm->GetFunction(fn);
    DBL *ea,freq,signal,weight,result;
    int i;
    Vector3d V1;
    int ngen = (int)PARAM(5) & 0x03;

    V1 = Vector3d(PARAM_X, PARAM_Y, PARAM_Z);
    if (f->private_data == NULL)
    {
        ea = reinterpret_cast<DBL *>(POV_MALLOC((PARAM(2) + 1)*sizeof(DBL), "exponent array"));
        freq = 1.0;
        for (i=0; i<=PARAM(2);i++)
        {
            ea[i]= pow(freq,-PARAM(0));
            freq *= PARAM(1);
        }
        f->private_data = reinterpret_cast<void *>(ea);
    }
    else
    {
        ea = reinterpret_cast<DBL *>(f->private_data);
    }
    signal = Noise(V1,ngen)*2.0-1.0;
    if (signal < 0.0 )
        signal = -signal;
    signal = PARAM(3) - signal;
    signal *= signal;
    result = signal;
    weight = 1.0;

    for (i=1; i<PARAM(2); i++)
    {
        V1 *= PARAM(1);
        weight = signal * PARAM(4);
        if (weight > 1.0)
            weight = 1.0;
        if (weight < 0.0)
            weight = 0.0;
        signal = Noise(V1,ngen)*2.0-1.0;
        if (signal < 0.0 )
            signal = -signal;
        signal = PARAM(3) - signal;
        signal *= signal;
        signal *= weight;
        result += signal * ea[i];
    }
    return (result);
}

DBL f_rounded_box(FPUContext *ctx, DBL *ptr, unsigned int) // 60
{
    DBL x2, y2, z2, x3, y3, z3;

    x2 = PARAM(1) - PARAM(0);
    y2 = PARAM(2) - PARAM(0);
    z2 = PARAM(3) - PARAM(0);
    x3 = (PARAM_X < x2) ? 0 : (PARAM_X - x2);
    y3 = (PARAM_Y < y2) ? 0 : (PARAM_Y - y2);
    z3 = (PARAM_Z < z2) ? 0 : (PARAM_Z - z2);
    x2 = PARAM(0) - PARAM(1);
    y2 = PARAM(0) - PARAM(2);
    z2 = PARAM(0) - PARAM(3);
    PARAM_X = (PARAM_X > x2) ? 0 : (x2 - PARAM_X);
    PARAM_Y = (PARAM_Y > y2) ? 0 : (y2 - PARAM_Y);
    PARAM_Z = (PARAM_Z > z2) ? 0 : (z2 - PARAM_Z);
    PARAM_X = max(PARAM_X, x3);
    PARAM_Y = max(PARAM_Y, y3);
    PARAM_Z = max(PARAM_Z, z3);

    return (-PARAM(0) + sqrt(PARAM_X * PARAM_X + PARAM_Y * PARAM_Y + PARAM_Z * PARAM_Z) - 1e-6);
}

DBL f_sphere(FPUContext *ctx, DBL *ptr, unsigned int) // 61
{
    return (-PARAM(0) + sqrt(PARAM_X * PARAM_X + PARAM_Y * PARAM_Y + PARAM_Z * PARAM_Z));
}

DBL f_spikes(FPUContext *ctx, DBL *ptr, unsigned int) // 62
{
    DBL r,x2,y2,z2;
    x2=PARAM_X*PARAM_X; y2=PARAM_Y*PARAM_Y; z2=PARAM_Z*PARAM_Z;
    r= exp(-(x2+y2+z2)*PARAM(3))*PARAM(4)          -exp(-(x2*PARAM(0)+y2*PARAM(0)+z2)*PARAM(1))
      -exp(-(x2*PARAM(0)+y2+z2*PARAM(0))*PARAM(1)) -exp(-(x2+y2*PARAM(0)+z2*PARAM(0))*PARAM(1));
    return(-r*PARAM(2));
}

DBL f_spikes_2d(FPUContext *ctx, DBL *ptr, unsigned int) // 63
{
    /*  2-D distribution */
    return(-( PARAM(0)* cos(PARAM(1)*PARAM_X) * cos(PARAM(2)*PARAM_Z) * exp(- PARAM(3)*(PARAM_X*PARAM_X+PARAM_Z*PARAM_Z)) -PARAM_Y));
}

DBL f_spiral(FPUContext *ctx, DBL *ptr, unsigned int) // 64
{
    DBL r, r2, th, temp;
    /* spiral shape                *
     *                             */
    r = sqrt(PARAM_X * PARAM_X + PARAM_Z * PARAM_Z);
    if ((PARAM_X == 0) && (PARAM_Z == 0))
        PARAM_X = 0.000001;
    th = atan2(PARAM_Z, PARAM_X);
    r = r + PARAM(0) * th / TWO_M_PI;
    r2 = fmod(r, PARAM(0)) - PARAM(0) * 0.5;

    if (PARAM(5) == 1)
        r2 = sqrt(r2 * r2 + PARAM_Y * PARAM_Y);
    else if (PARAM(5) != 0)
    {
        temp = 2 / PARAM(5);
        r2 = pow((pow(fabs(r2), temp) + pow(fabs(PARAM_Y), temp)), 1. / temp);
    }
    else
        r2 = max(fabs(r2), fabs(PARAM_Y));

    r = sqrt(PARAM_X * PARAM_X + PARAM_Y * PARAM_Y + PARAM_Z * PARAM_Z);

    return (-min(PARAM(2) - r, PARAM(1) - min(r2, r)));
}

DBL f_steiners_roman(FPUContext *ctx, DBL *ptr, unsigned int) // 65
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z;
    r=-( x2*y2 + x2*z2 + y2*z2 + PARAM_X*PARAM_Y*PARAM_Z  );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_strophoid(FPUContext *ctx, DBL *ptr, unsigned int) // 66
{
    DBL r, r2,x2=PARAM_X*PARAM_X;
    r2=PARAM(3)*PARAM(3);
    r=-((PARAM(2) - PARAM_X)*(PARAM_Y*PARAM_Y + PARAM_Z*PARAM_Z) - r2*PARAM(1)*x2 - r2*x2*PARAM_X );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_strophoid_2d(FPUContext *ctx, DBL *ptr, unsigned int) // 67
{
    DBL r, r2,x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, th;
    ROT2D(PARAM(4),PARAM(5),PARAM(6))
    r2=PARAM(3)*PARAM(3);
    r=-( (PARAM(2) - PARAM_X)*y2  - r2*PARAM(1)*x2 -  r2*x2*PARAM_X  );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_superellipsoid(FPUContext *ctx, DBL *ptr, unsigned int) // 68
{
    DBL p = 2 / PARAM(0), n = 1 / PARAM(1);

    return 1 - pow((pow((pow(fabs(PARAM_X), p) + pow(fabs(PARAM_Y), p)), PARAM(0) * n) + pow(fabs(PARAM_Z), 2 * n)), PARAM(1) *.5);
}

DBL f_th(FPUContext *ctx, DBL *ptr, unsigned int) // 69
{
    return( atan2(PARAM_X,PARAM_Z) );
}

DBL f_torus(FPUContext *ctx, DBL *ptr, unsigned int) // 70
{
    PARAM_X = sqrt(PARAM_X * PARAM_X + PARAM_Z * PARAM_Z) - PARAM(0);

    return -PARAM(1) + sqrt(PARAM_X * PARAM_X + PARAM_Y * PARAM_Y);
}

DBL f_torus2(FPUContext *ctx, DBL *ptr, unsigned int) // 71
{
    DBL r, ph,th,x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z;
    ph=PARAM(1)*PARAM(1); th=PARAM(2)*PARAM(2);
    r=-( x2*x2 + y2*y2 + z2*z2 + 2*x2*y2 + 2*x2*z2 + 2*y2*z2 -2* (ph + th)* x2 + 2* (ph - th)* y2 -2* (ph + th)* z2 + (ph - th)*(ph - th)  );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_torus_gumdrop(FPUContext *ctx, DBL *ptr, unsigned int) // 72
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, z2=PARAM_Z*PARAM_Z;
    r=-( 4*(x2*x2 + (y2 + z2)*(y2 + z2)) + 17 * x2 * (y2 + z2) - 20 * (x2 + y2 + z2) + 17);
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_umbrella(FPUContext *ctx, DBL *ptr, unsigned int) // 73
{
    DBL r;
    r=-( PARAM_X*PARAM_X - PARAM_Y*PARAM_Z*PARAM_Z );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_witch_of_agnesi(FPUContext *ctx, DBL *ptr, unsigned int) // 74
{
    DBL r;
    r=-( PARAM(1) * (PARAM_Y - 1) + (PARAM_X*PARAM_X + PARAM_Z*PARAM_Z) * PARAM_Y   );
    return( min(10., max(PARAM(0)*r,-10.)) );
}

DBL f_witch_of_agnesi_2d(FPUContext *ctx, DBL *ptr, unsigned int) // 75
{
    DBL r, x2=PARAM_X*PARAM_X, y2=PARAM_Y*PARAM_Y, th;
    ROT2D(PARAM(3),PARAM(4),PARAM(5))
    r=-( PARAM(1)*PARAM(1) * PARAM_Y + x2 * PARAM_Y - PARAM(2)  );
    return( min(10.0, max(PARAM(0)*r,-10.0)) );
}

DBL f_noise3d(FPUContext *ctx, DBL *ptr, unsigned int) // 76
{
    Vector3d Vec = Vector3d(PARAM_X, PARAM_Y, PARAM_Z);
    return Noise(Vec, ctx->threaddata->GetSceneData()->noiseGenerator);
}

DBL f_pattern(FPUContext *ctx, DBL *ptr, unsigned int fn) // 77
{
    Vector3d TPoint;
    Vector3d Vec = Vector3d(PARAM_X, PARAM_Y, PARAM_Z);
    FunctionCode *f = ctx->functionvm->GetFunction(fn);

    if(f->private_data == NULL)
        return 0.0;

    Warp_EPoint (TPoint, Vec, reinterpret_cast<const TPATTERN *>(f->private_data));

    return Evaluate_TPat(reinterpret_cast<const TPATTERN *>(f->private_data), TPoint, NULL, NULL, ctx->threaddata);
}

DBL f_noise_generator(FPUContext *ctx, DBL *ptr, unsigned int) // 78
{
    Vector3d Vec = Vector3d(PARAM_X, PARAM_Y, PARAM_Z);
    int ngen = (int)PARAM(0) & 0x03;

    return Noise(Vec, ngen);
}

void f_pigment(FPUContext *ctx, DBL *ptr, unsigned int fn, unsigned int sp) // 0
{
    Vector3d Vec = Vector3d(PARAM_N_X(5), PARAM_N_Y(5), PARAM_N_Z(5));
    TransColour Col;
    RGBFTColour rgbftCol;
    FunctionCode *f = ctx->functionvm->GetFunction(fn);

    if(f->private_data == NULL)
    {
        ctx->SetLocal(sp + pRED,    0.0);
        ctx->SetLocal(sp + pGREEN,  0.0);
        ctx->SetLocal(sp + pBLUE,   0.0);
        ctx->SetLocal(sp + pFILTER, 0.0);
        ctx->SetLocal(sp + pTRANSM, 0.0);
        return;
    }

    Compute_Pigment(Col, reinterpret_cast<const PIGMENT *>(f->private_data), Vec, NULL, NULL, ctx->threaddata);
    rgbftCol = RGBFTColour(Col);

    ctx->SetLocal(sp + pRED,    rgbftCol.red());
    ctx->SetLocal(sp + pGREEN,  rgbftCol.green());
    ctx->SetLocal(sp + pBLUE,   rgbftCol.blue());
    ctx->SetLocal(sp + pFILTER, rgbftCol.filter());
    ctx->SetLocal(sp + pTRANSM, rgbftCol.transm());
}

void f_transform(FPUContext *ctx, DBL *ptr, unsigned int fn, unsigned int sp) // 1
{
    Vector3d Vec = Vector3d(PARAM_N_X(3), PARAM_N_Y(3), PARAM_N_Z(3));
    Vector3d Result;
    FunctionCode *f = ctx->functionvm->GetFunction(fn);

    if(f->private_data == NULL)
    {
        ctx->SetLocal(sp + X, 0.0);
        ctx->SetLocal(sp + Y, 0.0);
        ctx->SetLocal(sp + Z, 0.0);
        return;
    }

    MTransPoint(Result, Vec, reinterpret_cast<const TRANSFORM *>(f->private_data));

    ctx->SetLocal(sp + X, Result[X]);
    ctx->SetLocal(sp + Y, Result[Y]);
    ctx->SetLocal(sp + Z, Result[Z]);
}

void f_spline(FPUContext *ctx, DBL *ptr, unsigned int fn, unsigned int sp) // 2
{
    EXPRESS Result;
    FunctionCode *f = ctx->functionvm->GetFunction(fn);
    int Terms;

    if(f->private_data == NULL)
    {
        ctx->SetLocal(sp + X, 0.0);
        ctx->SetLocal(sp + Y, 0.0);
        ctx->SetLocal(sp + Z, 0.0);
        return;
    }

    Terms = (reinterpret_cast<GenericSpline *>(f->private_data))->Terms;

    Get_Spline_Val(reinterpret_cast<GenericSpline *>(f->private_data), PARAM_N_X(Terms), Result, &Terms);

    ctx->SetLocal(sp + X, Result[X]);
    ctx->SetLocal(sp + Y, Result[Y]);
    if(Terms > 2)
    {
        ctx->SetLocal(sp + Z, Result[Z]);
        if(Terms > 3)
        {
            ctx->SetLocal(sp + T, Result[T]);
            if(Terms > 4)
                ctx->SetLocal(sp + T + 1, Result[T + 1]);
        }
    }
}

}
