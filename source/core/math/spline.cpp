//******************************************************************************
///
/// @file core/math/spline.cpp
///
/// Implementation of spline-related maths.
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
#include "core/math/spline.h"

// C++ variants of C standard header files
#include <cmath>
#include <cstdio>
#include <cstring>

// C++ standard header files
#include <limits>

// POV-Ray header files (base module)
#include "base/pov_err.h"
#include "base/povassert.h"
#include "base/types.h"
#include "core/coretypes.h"

// POV-Ray header files (core module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Global Variables
******************************************************************************/


/*****************************************************************************
* Local preprocessor defines
******************************************************************************/


/*****************************************************************************
* Local typedefs
******************************************************************************/


/*****************************************************************************
* Local variables
******************************************************************************/


/*****************************************************************************
* Local functions
******************************************************************************/

DBL linear_interpolate(const SplineEntryList& se, SplineEntryList::size_type i, int k, DBL p);
DBL quadratic_interpolate(const SplineEntryList& se, SplineEntryList::size_type i, int k, DBL p);
DBL natural_interpolate(const SplineEntryList& se, const SplineCoeffList& ce, SplineEntryList::size_type i, int k, DBL p);
DBL catmull_rom_interpolate(const SplineEntryList& se, SplineEntryList::size_type i, int k, DBL p);
SplineEntryList::size_type findt(const GenericSpline * sp, DBL Time);


DBL basic_x_interpolate(const SplineEntryList& se, int i, int k, DBL p, DBL fd);
DBL extended_x_interpolate(const SplineEntryList& se, int i, int k, DBL p, int N);
DBL general_x_interpolate(const SplineEntryList& se, int i, int k, DBL p, int N);


/*****************************************************************************
*
* FUNCTION
*
*       Precompute_SOR_Coeffs
*
* INPUT
*
*       sp : a pointer to the spline to compute interpolation coefficients for
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*       ABX (abx@abx.art.pl)
*
* DESCRIPTION
*
*       Computes the coefficients used in sor_interpolate.
*
* CHANGES
*
*       2002.08.09 - Initial version by ABX
*
******************************************************************************/

void SorSpline::Precompute()
{
    SorSpline * sp = this;
    int i, k;
    // temporary variables for operations
    // I hope that compilers remove part of it
    DBL b0,b1,b2,b3;
    DBL M00,M01,M02;
    DBL M10,M11,M12;
    DBL M20,M21,M30,M31;
    DBL M2131,M1101,M1202;
    DBL M1101_1,M1101_2;
    DBL A,B,C,D;

    SplinePreComputed.resize(SplineEntries.size());
    for(k = 0; k < 5; k++)
    {
        for(i = 2; i < SplineEntries.size() - 1; i++)
        {
            b0=pow(sp->SplineEntries[i-1].vec[k],2);
            b1=pow(sp->SplineEntries[i].vec[k],2);
            b2=2*sp->SplineEntries[i-1].vec[k]*(sp->SplineEntries[i].vec[k]-sp->SplineEntries[i-2].vec[k])/
               (sp->SplineEntries[i].par-sp->SplineEntries[i-2].par);
            b3=2*sp->SplineEntries[i].vec[k]*(sp->SplineEntries[i+1].vec[k]-sp->SplineEntries[i-1].vec[k])/
               (sp->SplineEntries[i+1].par-sp->SplineEntries[i-1].par);
            M00=pow(sp->SplineEntries[i-1].par,3);
            M01=pow(sp->SplineEntries[i-1].par,2);
            M02=sp->SplineEntries[i-1].par;
            M10=pow(sp->SplineEntries[i].par,3);
            M11=pow(sp->SplineEntries[i].par,2);
            M12=sp->SplineEntries[i].par;
            M20=3*pow(sp->SplineEntries[i-1].par,2);
            M21=2*sp->SplineEntries[i-1].par;
            M30=3*pow(sp->SplineEntries[i].par,2);
            M31=2*sp->SplineEntries[i].par;
            M2131=M21-M31;
            M1101=M11-M01;
            M1202=M12-M02;
            M1101_1=M1101-M31*M1202;
            M1101_2=M21*M1202-M1101;
            A=((b0-b1)*M2131+b2*M1101_1+b3*M1101_2)/
              ((M00-M10)*M2131+M20*M1101_1+M30*M1101_2);
            B=(b2-b3+A*(M30-M20))/M2131;
            C=b3-M30*A-M31*B;
            D=b1-M10*A-M11*B-M12*C;

            sp->SplinePreComputed[i].coeff[k][0]=A;
            sp->SplinePreComputed[i].coeff[k][1]=B;
            sp->SplinePreComputed[i].coeff[k][2]=C;
            sp->SplinePreComputed[i].coeff[k][3]=D;
        }
    }
    sp->Coeffs_Computed = true;
}

/*****************************************************************************
*
* FUNCTION
*
*       SOR_interpolate
*
* INPUT
*
*       se : a pointer to the entries in the spline
*       i  : the first point in the interpolation interval
*       k  : which dimension of the 5D vector to interpolate in
*       p  : the parameter to interpolate the value for
*
* OUTPUT
*
* RETURNS
*
*       The value of the kth dimension of the SOR interpolation of the
*        vector at p
*
* AUTHOR
*
*       ABX (abx@abx.art.pl)
*
* DESCRIPTION
*
* CHANGES
*
*       2002.08.09 initial version
*
******************************************************************************/

DBL SorSpline::interpolate(int i, int k, DBL p)const
{
    DBL Result=
      sqrt(
        SplinePreComputed[i+1].coeff[k][0]*pow(p,3)
       +SplinePreComputed[i+1].coeff[k][1]*pow(p,2)
       +SplinePreComputed[i+1].coeff[k][2]*p
       +SplinePreComputed[i+1].coeff[k][3]
      );
    return Result;
}



/*****************************************************************************
*
* FUNCTION
*
*       Precompute_TCB_Coeffs
*
* INPUT
*
*       sp : a pointer to the spline to compute interpolation coefficients for
*
* AUTHOR
*
*       ABX (abx@abx.art.pl)
*
* DESCRIPTION
*
*       Computes the coefficients used in tcb spline also known as Kochanek-Bartels.
*       Helpers tcb_in_T() and tcb_out_T() calculates components of incoming and outgoing tangents
*
* REFERENCE
*
*       http://www.magic-software.com/Documentation/KBSplines.pdf
*
* CHANGES
*
*       2003.04.10 - Initial version by ABX
*
******************************************************************************/

inline DBL tcb_in_T(SplineEntry *se, SplineTcbParam *pe, int k)
{
  DBL p1=(1-pe[1].tension)*
         (1+pe[1].continuity)*
         (1-pe[1].bias)*
         (se[2].vec[k]-se[1].vec[k])/2;
  DBL p2=(1-pe[1].tension)*
         (1-pe[1].continuity)*
         (1+pe[1].bias)*
         (se[1].vec[k]-se[0].vec[k])/2;
  return p1+p2;
}

inline DBL tcb_out_T(SplineEntry *se, SplineTcbParam* pe, int k)
{
  DBL p1=(1-pe[1].tension)*
         (1-pe[1].continuity)*
         (1-pe[1].bias)*
         (se[2].vec[k]-se[1].vec[k])/2;
  DBL p2=(1-pe[1].tension)*
         (1+pe[1].continuity)*
         (1+pe[1].bias)*
         (se[1].vec[k]-se[0].vec[k])/2;
  return p1+p2;
}

void TcbSpline::Precompute()
{
    TcbSpline * sp = this;
    SplinePreComputedIn.resize( sp->SplineEntries.size());
    SplinePreComputedOut.resize( sp->SplineEntries.size());
    for(int i = 1; i < sp->SplineEntries.size() - 2; i++)
    {
        for(int k = 0; k < 5; k++)
        {
            // incoming tangent from next key
            sp->SplinePreComputedIn[i].coeff[k] =
                  tcb_in_T( &(sp->SplineEntries[i]), &(sp->in[i]), k )
                  * ( sp->SplineEntries[i+2].par - sp->SplineEntries[i+1].par );

            // outgoing tangent from current key
            sp->SplinePreComputedOut[i].coeff[k] =
                  tcb_out_T( &(sp->SplineEntries[i-1]), &(sp->out[i-1]), k )
                  * ( sp->SplineEntries[i+1].par - sp->SplineEntries[i].par );
        }
    }
    sp->Coeffs_Computed = true;
}

inline DBL H0(DBL s){return (2*s*s-3*s)*s+1;}
inline DBL H1(DBL s){return (3-2*s)*s*s;}
inline DBL H2(DBL s){return (s-2)*s*s+s;}
inline DBL H3(DBL s){return (s-1)*s*s;}

DBL TcbSpline::interpolate(int i, int k, DBL p)const
{
    const DBL t=(p-SplineEntries[i].par)/(SplineEntries[i+1].par-SplineEntries[i].par);
    return (H0(t)*SplineEntries[i].vec[k]+
            H1(t)*SplineEntries[i+1].vec[k]+
            H2(t)*SplinePreComputedOut[i].coeff[k]+
            H3(t)*SplinePreComputedIn[i].coeff[k]);
}



/*****************************************************************************
*
* FUNCTION
*
*       Precompute_Akima_Coeffs
*
* INPUT
*
*       sp : a pointer to the spline to compute interpolation coefficients for
*
* AUTHOR
*
*       ABX (abx@abx.art.pl)
*
* DESCRIPTION
*
*       Computes the coefficients used in akima_spline
*
* CHANGES
*
*       2003.04.15 - Initial version by ABX
*
******************************************************************************/

void AkimaSpline::Precompute()
{
    AkimaSpline * sp = this;
    const int N = SplineEntries.size();
    DBL* Slope;
    DBL* Der;

    SplinePreComputed.resize(N);
    Slope = (DBL *)POV_MALLOC((N+3)*sizeof(DBL), "Spline coefficient storage");
    Der = (DBL *)POV_MALLOC(N*sizeof(DBL), "Spline coefficient storage");

    for (int k = 0; k < 5; k++)
    {
        for (int i = 0; i < N-1; i++)
        {
            Slope[i+2] =
                (SplineEntries[i+1].vec[k] - SplineEntries[i].vec[k]) /
                (SplineEntries[i+1].par - SplineEntries[i].par);
        }

        Slope[1  ] = 2.0 * Slope[2  ] - Slope[3  ];
        Slope[0  ] = 2.0 * Slope[1  ] - Slope[2  ];
        Slope[N+1] = 2.0 * Slope[N  ] - Slope[N-1];
        Slope[N+2] = 2.0 * Slope[N+1] - Slope[N  ];

        for (int i = 0; i < N; i++)
        {
            if ( Slope[i+1] != Slope[i+2] )
            {
                const bool compare = ( Slope[i+2] != Slope[i+3] );

                if ( Slope[i] != Slope[i+1] )
                {
                    if ( compare )
                    {
                        const DBL d0 = fabs(Slope[i+3] - Slope[i+2]);
                        const DBL d1 = fabs(Slope[i] - Slope[i+1]);
                        Der[i] = (d0*Slope[i+1]+d1*Slope[i+2])/(d0+d1);
                    }
                    else
                    {
                        Der[i] = Slope[i+2];
                    }
                }
                else
                {
                    Der[i] = ( compare ) ? ( Slope[i+1] ) : ( (Slope[i+1]+Slope[i+2])/2 ) ;
                }
            }
            else
            {
                Der[i] = Slope[i+1];
            }
        }
        for (int i = 0; i < N-1; i++)
        {
            const DBL d0 = SplineEntries[i+1].vec[k] - SplineEntries[i].vec[k];
            const DBL d1 = SplineEntries[i+1].par - SplineEntries[i].par;
            const DBL d2 = d1*d1;

            SplinePreComputed[i].coeff[k][0] = SplineEntries[i].vec[k];
            SplinePreComputed[i].coeff[k][1] = Der[i];
            SplinePreComputed[i].coeff[k][2] = (3.0*d0-d1*(Der[i+1]+2.0*Der[i]))/d2;
            SplinePreComputed[i].coeff[k][3] = (Der[i]+Der[i+1]-2.0*d0/d1)/d2;
        }
    }

    POV_FREE(Der);
    POV_FREE(Slope);

    sp->Coeffs_Computed = true;
}

DBL AkimaSpline::interpolate(int i, int k, DBL p)const
{
    const DBL d=p-(DBL)SplineEntries[i].par;
    return SplinePreComputed[i].coeff[k][0]+d*(SplinePreComputed[i].coeff[k][1]+d*(SplinePreComputed[i].coeff[k][2]+d*SplinePreComputed[i].coeff[k][3]));
}


  inline DBL x_f(DBL p, DBL u){return u*u*u*(10.0-p+(2.0*p-15.0)*u+(6.0-p)*u*u);};


DBL BasicXSpline::interpolate( int i, int k, DBL p, DBL fd)const
{
    const DBL d1=SplineEntries[i+2].par-SplineEntries[i].par;
    const DBL d2=SplineEntries[i+3].par-SplineEntries[i+1].par;
    const DBL A0=x_f(fd,(SplineEntries[i+2].par-p)/d1);
    const DBL A1=x_f(fd,(SplineEntries[i+3].par-p)/d2);
    const DBL A2=x_f(fd,(p-SplineEntries[i+0].par)/d1);
    const DBL A3=x_f(fd,(p-SplineEntries[i+1].par)/d2);
    return (A0*SplineEntries[i  ].vec[k]+
            A1*SplineEntries[i+1].vec[k]+
            A2*SplineEntries[i+2].vec[k]+
            A3*SplineEntries[i+3].vec[k])/
           (A0+A1+A2+A3);
}



DBL ExtendedXSpline::interpolate(int i, int k, DBL p, int N)const
{
    const int i0=std::max(std::min(i  ,N),0);
    const int i1=std::max(std::min(i+1,N),0);
    const int i2=std::max(std::min(i+2,N),0);
    const int i3=std::max(std::min(i+3,N),0);

    const DBL pp=std::max(std::min(p,SplineEntries[N].par),SplineEntries[0].par);

    const DBL d0=SplineEntries[i1].par-SplineEntries[i0].par;
    const DBL d1=SplineEntries[i2].par-SplineEntries[i1].par;
    const DBL d2=SplineEntries[i3].par-SplineEntries[i2].par;

    const DBL d02=(SplineEntries[i2].par-SplineEntries[i0].par)/2;
    const DBL d13=(SplineEntries[i3].par-SplineEntries[i1].par)/2;

    const DBL T0=SplineEntries[i1].par+node[i1].freedom_degree*d1;
    const DBL p0=2*Sqr((SplineEntries[i2].par-T0)/d02);
    const DBL A0=(pp>T0)?0:x_f(p0,(T0-pp)/(T0-SplineEntries[i0].par));

    const DBL T1=SplineEntries[i2].par+node[i2].freedom_degree*d2;
    const DBL p1=2*Sqr((SplineEntries[i3].par-T1)/d13);
    const DBL A1=(pp>T1)?0:x_f(p1,(pp-T1)/(SplineEntries[i1].par-T1));

    const DBL T2=SplineEntries[i1].par-node[i1].freedom_degree*d0;
    const DBL p2=2*Sqr((T2-SplineEntries[i0].par)/d02);
    const DBL A2=(pp<T2)?0:x_f(p2,(pp-T2)/(SplineEntries[i2].par-T2));

    const DBL T3=SplineEntries[i2].par-node[i2].freedom_degree*d1;
    const DBL p3=2*Sqr((T3-SplineEntries[i1].par)/d13);
    const DBL A3=(pp<T3)?0:x_f(p3,(pp-T3)/(SplineEntries[i3].par-T3));

    return (A0*SplineEntries[i0].vec[k]+A1*SplineEntries[i1].vec[k]+A2*SplineEntries[i2].vec[k]+A3*SplineEntries[i3].vec[k])/
           (A0+A1+A2+A3);
}



inline DBL x_g(DBL p, DBL q, DBL u){return q*u+2*q*u*u+(10-12*q-p)*u*u*u+(2*p+14*q-15)*u*u*u*u+(6-5*q-p)*u*u*u*u*u;};

inline DBL x_h(DBL q, DBL u){return q*u+2*q*u*u-2*q*u*u*u*u-q*u*u*u*u*u;};

DBL GeneralXSpline::interpolate(int i, int k, DBL p, int N)const
{
    const int i0=std::max(std::min(i  ,N),0);
    const int i1=std::max(std::min(i+1,N),0);
    const int i2=std::max(std::min(i+2,N),0);
    const int i3=std::max(std::min(i+3,N),0);

    const DBL pp=std::max(std::min(p,SplineEntries[N].par),SplineEntries[0].par);

    const DBL d0=SplineEntries[i1].par-SplineEntries[i0].par;
    const DBL d1=SplineEntries[i2].par-SplineEntries[i1].par;
    const DBL d2=SplineEntries[i3].par-SplineEntries[i2].par;

    const DBL d02=(SplineEntries[i2].par-SplineEntries[i0].par)/2;
    const DBL d13=(SplineEntries[i3].par-SplineEntries[i1].par)/2;

    const DBL T0=SplineEntries[i1].par+std::max(node[i1].freedom_degree,0.0)*d1;
    const DBL q0=(node[i1].freedom_degree<0)?-node[i1].freedom_degree/2.0:0.0;
    const DBL p0=(d02!=0.0)?2*Sqr((SplineEntries[i2].par-T0)/d02):0.0;
    const DBL A0=(i0==i1)?0.0:((pp>T0)?((q0>0)?x_h(q0,(SplineEntries[i2].par-pp)/d1-1):0.0):x_g(p0,q0,(pp-T0)/(SplineEntries[i0].par-T0)));

    const DBL T1=SplineEntries[i2].par+std::max(node[i2].freedom_degree,0.0)*d2;
    const DBL q1=(node[i2].freedom_degree<0)?-node[i2].freedom_degree/2.0:0.0;
    const DBL p1=(d13!=0.0)?2*Sqr((SplineEntries[i3].par-T1)/d13):0.0;
    const DBL A1=(i1==i2)?0.0:((pp>T1)?0.0:x_g(p1,q1,(pp-T1)/(SplineEntries[i1].par-T1)));

    const DBL T2=SplineEntries[i1].par-std::max(node[i1].freedom_degree,0.0)*d0;
    const DBL q2=(node[i1].freedom_degree<0)?-node[i1].freedom_degree/2.0:0.0;
    const DBL p2=(d02!=0.0)?2*Sqr((T2-SplineEntries[i0].par)/d02):0.0;
    const DBL A2=(i1==i2)?0.0:((pp<T2)?0.0:x_g(p2,q2,(pp-T2)/(SplineEntries[i2].par-T2)));

    const DBL T3=SplineEntries[i2].par-std::max(node[i2].freedom_degree,0.0)*d1;
    const DBL q3=(node[i2].freedom_degree<0)?-node[i2].freedom_degree/2.0:0.0;
    const DBL p3=(d13!=0.0)?2*Sqr((T3-SplineEntries[i1].par)/d13):0.0;
    const DBL A3=(i2==i3)?0.0:((pp<T3)?((q3>0)?x_h(q3,(pp-SplineEntries[i1].par)/d1-1):0.0):x_g(p3,q3,(pp-T3)/(SplineEntries[i3].par-T3)));

    return (A0*SplineEntries[i0].vec[k]+A1*SplineEntries[i1].vec[k]+A2*SplineEntries[i2].vec[k]+A3*SplineEntries[i3].vec[k])/
           (A0+A1+A2+A3);
}

/*****************************************************************************
*
* FUNCTION
*
*       Precompute_Cubic_Coeffs
*
* INPUT
*
*       sp : a pointer to the spline to compute interpolation coefficients for
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Mark Wagner
*
* DESCRIPTION
*
*       Computes the coefficients used in cubic_interpolate.
*
* CHANGES
*
******************************************************************************/

void NaturalSpline::Precompute()
{
    SplineEntryList::size_type i, k;
    DBL *h;
    DBL *b;
    DBL *u;
    DBL *v;
    NaturalSpline * sp = this;

    SplineEntryList::size_type numEntries = sp->SplineEntries.size();
    POV_ASSERT(numEntries >= 2);
    sp->SplinePreComputed.resize( numEntries );
    
    h = new DBL[numEntries];
    b = new DBL[numEntries];
    u = new DBL[numEntries];
    v = new DBL[numEntries];

    for(k = 0; k < 5; k++)
    {
        for(i = 0; i <= numEntries - 2; i++)
        {
            h[i] = sp->SplineEntries[i+1].par - sp->SplineEntries[i].par;
            b[i] = (sp->SplineEntries[i+1].vec[k] - sp->SplineEntries[i].vec[k])/h[i];
        }
        u[1] = 2*(h[0]+h[1]);
        v[1] = 6*(b[1]-b[0]);
        for(i = 2; i <= numEntries - 2; i++)
        {
            u[i] = 2*(h[i]+h[i-1]) - (h[i-1]*h[i-1])/u[i-1];
            v[i] = 6*(b[i]-b[i-1]) - (h[i-1]*v[i-1])/u[i-1];
        }
        sp->SplinePreComputed.back().coeff[k] = 0;
        for(i = numEntries-2; i > 0; i--)
        {
            sp->SplinePreComputed[i].coeff[k] = (v[i] - h[i]*sp->SplinePreComputed[i+1].coeff[k])/u[i];
        }
        sp->SplinePreComputed[0].coeff[k] = 0;
    }
    sp->Coeffs_Computed = true;

    delete[] h;
    delete[] b;
    delete[] u;
    delete[] v;
}


/*****************************************************************************
*
* FUNCTION
*
*       linear_interpolate
*
* INPUT
*
*       se : a pointer to the entries in the spline
*       i  : the first point to interpolate between
*       k  : which dimension of the 5D vector to interpolate in
*       p  : the parameter to interpolate the value for
*
* OUTPUT
*
* RETURNS
*
*       The value of the kth dimension of the vector linearly interpolated at p
*
* AUTHOR
*
*   Wolfgang Ortmann
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL linear_interpolate(const SplineEntryList& se, SplineEntryList::size_type i, int k, DBL p)
{
    DBL p1, p2, v1, v2;
    p1 = se[i].par;
    p2 = se[i+1].par;
    v1 = se[i].vec[k];
    v2 = se[i+1].vec[k];
    return (p-p1)*(v2-v1)/(p2-p1)+v1;
}


/*****************************************************************************
*
* FUNCTION
*
*       quadratic_interpolate
*
* INPUT
*
*       se : a pointer to the entries in the spline
*       i  : the second point of three to interpolate between
*       k  : which dimension of the 5D vector to interpolate in
*       p  : the parameter to interpolate the value for
*
* OUTPUT
*
* RETURNS
*
*       The value of the kth dimension of the quadratic interpolation of the
*        vector at p
*
* AUTHOR
*
*   Wolfgang Ortmann
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL quadratic_interpolate(const SplineEntryList& se, SplineEntryList::size_type i, int k, DBL p)
{
    /* fit quadratic function to three point*/
    DBL n;
    DBL p1, p2, p3,
        v1, v2, v3,
        a, b, c;
    /* assignments to make life easier */
    p1=se[i-1].par;    p2=se[i].par;    p3=se[i+1].par;
    v1=se[i-1].vec[k]; v2=se[i].vec[k]; v3=se[i+1].vec[k];

    n=(p2-p1)*(p3-p1)*(p3-p2);

    /* MWW NOTE: I'm assuming these are correct.  I didn't write them */
    a=(-p2*v1+p3*v1
       +p1*v2-p3*v2
       -p1*v3+p2*v3) /n;
    b=( p2*p2*v1 - p3*p3*v1
       -p1*p1*v2 + p3*p3*v2
       +p1*p1*v3 - p2*p2*v3) /n;
    c=(-p2*p2*p3*v1+p2*p3*p3*v1
       +p1*p1*p3*v2-p1*p3*p3*v2
       -p1*p1*p2*v3+p1*p2*p2*v3) /n;
    return (a*p+b)*p+c; /* Fast way of doing ap^2+bp+c */
}


/*****************************************************************************
*
* FUNCTION
*
*       natural_interpolate
*
* INPUT
*
*       se : reference to the entries in the spline
*       ce : reference to pre computed coefficients in the spline
*       i  : the first point in the interpolation interval
*       k  : which dimension of the 5D vector to interpolate in
*       p  : the parameter to interpolate the value for
*
* OUTPUT
*
* RETURNS
*
*       The value of the kth dimension of the natural cubic interpolation
*        of the vector at p
*
* AUTHOR
*
*       Mark Wagner
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL natural_interpolate(const SplineEntryList& se, const SplineCoeffList& ce, SplineEntryList::size_type i, int k, DBL p)
{
    DBL h, tmp;
    h = se[i+1].par - se[i].par;
    tmp = ce[i].coeff[k]/2.0 + ((p - se[i].par)*(ce[i+1].coeff[k] - ce[i].coeff[k]))/(6.0*h);
    tmp = -(h/6.0)*(ce[i+1].coeff[k] + 2.0*ce[i].coeff[k]) + (se[i+1].vec[k] - se[i].vec[k])/h + (p - se[i].par)*tmp;
    return se[i].vec[k] + (p - se[i].par)*tmp;
}


/*****************************************************************************
*
* FUNCTION
*
*       catmull_rom_interpolate
*
* INPUT
*
*       se : a pointer to the entries in the spline
*       i  : the first point in the interpolation interval
*       k  : which dimension of the 5D vector to interpolate in
*       p  : the parameter to interpolate the value for
*
* OUTPUT
*
* RETURNS
*
*       The value of the kth dimension of the Catmull-Rom interpolation of the
*        vector at p
*
* AUTHOR
*
*       Mark Wagner
*
* DESCRIPTION
*
* CHANGES
*
******************************************************************************/

DBL catmull_rom_interpolate(const SplineEntryList& se, SplineEntryList::size_type i, int k, DBL p)
{
    DBL dt = (se[i+1].par - se[i].par); /* Time between se[i] and se[i+1] */
    DBL u = (p - se[i].par)/dt;         /* Fractional time from se[i] to p */
    DBL dp0 = ((se[i].vec[k] - se[i-1].vec[k])/(se[i].par - se[i-1].par) + (se[i+1].vec[k] - se[i].vec[k])/(se[i+1].par - se[i].par))/2.0 * dt;
    DBL dp1 = ((se[i+2].vec[k] - se[i+1].vec[k])/(se[i+2].par - se[i+1].par) + (se[i+1].vec[k] - se[i].vec[k])/(se[i+1].par - se[i].par))/2.0 * dt;

    return (se[i].vec[k] * (2*u*u*u - 3*u*u + 1) +
            se[i+1].vec[k] * (3*u*u - 2*u*u*u) +
            dp0 * (u*u*u - 2*u*u + u) +
            dp1 * (u*u*u - u*u) );
}


/*****************************************************************************
*
* FUNCTION
*
*       findt
*
* INPUT
*
*       sp   : a pointer to a spline
*       Time : The parameter to search for
*
* OUTPUT
*
* RETURNS
*
*       The first spline entry with a parameter greater than Time
*
* AUTHOR
*
*   Wolfgang Ortmann
*
* DESCRIPTION
*
* CHANGES
*
*       Mark Wagner  6 Nov 2000 : Changed from linear search to binary search
*
******************************************************************************/

SplineEntryList::size_type findt(const GenericSpline * sp, DBL Time)
{
    SplineEntryList::size_type i, i2;
    const SplineEntryList& se = sp->SplineEntries;
    if(sp->SplineEntries.empty()) return 0;
    SplineEntryList::size_type numEntries = se.size();

    if(Time <= se.front().par) return 0;

    if(Time >= se.back().par) return numEntries;

    i = numEntries / 2;
    /* Bracket the proper entry */
    if( Time > se[i].par ) /* i is lower, i2 is upper */
    {
        i2 = numEntries-1;
        while(i2 - i > 1)
        {
            if(Time > se[i+(i2-i)/2].par)
                i = i+(i2-i)/2;
            else
                i2 = i+(i2-i)/2;
        }
        return i2;
    }
    else /* i is upper, i2 is lower */
    {
        i2 = 0;
        while(i - i2 > 1)
        {
            if(Time > se[i2+(i-i2)/2].par)
                i2 = i2+(i-i2)/2;
            else
                i = i2+(i-i2)/2;
        }
        return i;
    }
}


/*****************************************************************************
*
* FUNCTION
*
*       Create_Spline
*
* INPUT
*
*       Type : the type of the new spline
*
* OUTPUT
*
* RETURNS
*
*       A pointer to the newly-created spline
*
* AUTHOR
*
*   Wolfgang Ortmann
*
* DESCRIPTION
*
* CHANGES
*
*       Mark Wagner  6 Nov 2000 : Added support for dynamic resizing of the
*               SplineEntry array.
*       Mark Wagner 25 Aug 2001 : Added support for pre-computing coefficients
*               of cubic splines
*
******************************************************************************/

GenericSpline::GenericSpline() :
    Coeffs_Computed(false),
    Terms(2),
    ref_count(1)
{}

LinearSpline::LinearSpline() : GenericSpline()
{}

QuadraticSpline::QuadraticSpline() : GenericSpline()
{}

NaturalSpline::NaturalSpline() : GenericSpline()
{}

CatmullRomSpline::CatmullRomSpline() : GenericSpline()
{}

SorSpline::SorSpline(): GenericSpline()
{}

AkimaSpline::AkimaSpline(): GenericSpline()
{}

TcbSpline::TcbSpline(): GenericSpline()
{}

XSpline::XSpline(): GenericSpline()
{}

BasicXSpline::BasicXSpline(): GenericSpline()
{}

ExtendedXSpline::ExtendedXSpline(): XSpline()
{}

GeneralXSpline::GeneralXSpline(): XSpline()
{}

/*****************************************************************************
*
* FUNCTION
*
*       Copy_Spline
*
* INPUT
*
*       Old : A pointer to the old spline
*
* OUTPUT
*
* RETURNS
*
*       A pointer to the new spline
*
* AUTHOR
*
*   Wolfgang Ortmann
*
* DESCRIPTION
*
* CHANGES
*
*       Mark Wagner  6 Nov 2000 : Added support for dynamic resizing of the
*               SplineEntry array
*       Mark Wagner 25 Aug 2001 : Added support for pre-computing coefficients
*               of cubic splines
*
******************************************************************************/

GenericSpline::GenericSpline(const GenericSpline& o) :
    SplineEntries(o.SplineEntries),
    Coeffs_Computed(false),
    Terms(o.Terms),
    ref_count(1)
{}

LinearSpline::LinearSpline(const GenericSpline& o) : GenericSpline(o)
{}

QuadraticSpline::QuadraticSpline(const GenericSpline& o) : GenericSpline(o)
{}

NaturalSpline::NaturalSpline(const GenericSpline& o) : GenericSpline(o)
{}

CatmullRomSpline::CatmullRomSpline(const GenericSpline& o) : GenericSpline(o)
{}

SorSpline::SorSpline( const GenericSpline& o ): GenericSpline(o)
{}

AkimaSpline::AkimaSpline( const GenericSpline& o ): GenericSpline(o)
{}

TcbSpline::TcbSpline( const GenericSpline& o ): GenericSpline(o)
{
  in.resize( SplineEntries.size() );
  out.resize( SplineEntries.size() );
}

XSpline::XSpline( const GenericSpline& o ): GenericSpline(o)
{
  node.resize( SplineEntries.size() );
}

BasicXSpline::BasicXSpline( const GenericSpline& o ): GenericSpline(o)
{}

ExtendedXSpline::ExtendedXSpline( const GenericSpline& o ): XSpline(o)
{}

GeneralXSpline::GeneralXSpline( const GenericSpline& o ): XSpline(o)
{}

GenericSpline * Copy_Spline(const GenericSpline * Old)
{
    return Old->Clone();
}


void GenericSpline::AcquireReference()
{
    if (ref_count >= std::numeric_limits<SplineRefCount>::max())
        throw POV_EXCEPTION_STRING("Too many unresolved references to single spline\n");
    ref_count ++;
}

void Acquire_Spline_Reference(GenericSpline* sp)
{
    if (sp)
        sp->AcquireReference();
}

void GenericSpline::ReleaseReference()
{
    if (ref_count <= 0)
        throw POV_EXCEPTION_STRING("Internal error: Invalid spline reference count\n");
    ref_count --;
}

void Release_Spline_Reference(GenericSpline* sp)
{
    if (sp)
        Destroy_Spline(sp);
}

/*****************************************************************************
*
* FUNCTION
*
*       Destroy_Spline
*
* INPUT
*
*       Spline : A pointer to the spline to delete
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Wolfgang Ortmann
*
* DESCRIPTION
*
*       Deletes the SplineEntry array
*
* CHANGES
*
******************************************************************************/

GenericSpline::~GenericSpline()
{}

void Destroy_Spline(GenericSpline* sp)
{
    sp->ReleaseReference();
    if (sp->ref_count == 0)
        delete sp;
}


/*****************************************************************************
*
* FUNCTION
*
*       Insert_Spline_Entry
*
* INPUT
*
*       sp : a pointer to the spline to insert the new value in
*       p  : the value of the parameter at that point
*       v  : a 5D vector that is the value of the spline at that parameter
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Wolfgang Ortmann
*
* DESCRIPTION
*
*       Inserts a value into the given spline, sorting the entries in order by
*               increasing p
*       If a value of the parameter already exists, that value is overwritten
*
* CHANGES
*
*       Mark Wagner  8 Nov 2000 : Added dynamic sizing of the SplineEntry array
*          If a value of the parameter already exists, that value is overwritten
*       Mark Wagner 25 Aug 2001 : Modified for compatibility with new method of
*          computing cubic splines.
*       Mark Wagner 30 Aug 2001 : Fixed a bug with over-writing of parameter
*          values.
*
******************************************************************************/

void Insert_Spline_Entry(GenericSpline * sp, DBL p, const EXPRESS& v)
{
    SplineEntryList::size_type i;
    int k;

    /* Reset the Coeffs_Computed flag.  Inserting a new point invalidates
     *  pre-computed coefficients */
    sp->Coeffs_Computed = false;
    i = findt(sp, p);
    // If p is already in the spline, replace it.
    bool replace = false;
    if (!sp->SplineEntries.empty())
    {
        // If p matches the _last_ spline entry, `findt()` is implemented to return `sp->SplineEntries.size()`
        // instead of the index of the matching entry; this has some benefits in the other places `findt()` is used,
        // but requires the following workaround here.
        if (i < sp->SplineEntries.size())
            replace = (sp->SplineEntries[i].par == p);
        else if (sp->SplineEntries[i-1].par == p)
        {
            --i;
            replace = true;
        }
    }
    if(replace)
    {
        for(k=0; k<5; k++)
            sp->SplineEntries[i].vec[k] = v[k];
    }
    else
    {
        SplineEntry fresh;
        fresh.par = p;
        for(k=0; k<5; k++)
            fresh.vec[k] = v[k];
        SplineEntryList::iterator pos = sp->SplineEntries.begin();
        pos += i;
        sp->SplineEntries.insert( pos, fresh );
        /*
        mkfree(sp, i);
        sp->SplineEntries[i].par = p;

        for(k=0; k<5; k++)
            sp->SplineEntries[i].vec[k] = v[k];
            */
    }
}

void Insert_Spline_Entry(GenericSpline * sp, DBL p, const EXPRESS& v, const SplineFreedom& f)
{
    XSpline * ssp = dynamic_cast<XSpline *>(sp);
    BasicXSpline * bsp = dynamic_cast<BasicXSpline *>(sp);
    SplineEntryList::size_type i;
    int k;

    /* Reset the Coeffs_Computed flag.  Inserting a new point invalidates
     *  pre-computed coefficients */
    sp->Coeffs_Computed = false;
    if (bsp)
    {
        bsp->freedom = f;
    }
  
    i = findt(sp, p);
    // If p is already in the spline, replace it.
    bool replace = false;
    if (!sp->SplineEntries.empty())
    {
        // If p matches the _last_ spline entry, `findt()` is implemented to return `sp->SplineEntries.size()`
        // instead of the index of the matching entry; this has some benefits in the other places `findt()` is used,
        // but requires the following workaround here.
        if (i < sp->SplineEntries.size())
            replace = (sp->SplineEntries[i].par == p);
        else if (sp->SplineEntries[i-1].par == p)
        {
            --i;
            replace = true;
        }
    }
    if(replace)
    {
        for(k=0; k<5; k++)
            sp->SplineEntries[i].vec[k] = v[k];
        if ( ssp)
            ssp->node[i] = f;
    }
    else
    {
        SplineEntry fresh;
        fresh.par = p;
        for(k=0; k<5; k++)
            fresh.vec[k] = v[k];
        SplineEntryList::iterator pos = sp->SplineEntries.begin();
        pos += i;
        sp->SplineEntries.insert( pos, fresh );
        if ( ssp)
        {
            SplineFreedomList::iterator posf = ssp->node.begin();
            posf += i;
            ssp->node.insert( posf, f );
        }
    }
}


void Insert_Spline_Entry(GenericSpline * sp, DBL p, const EXPRESS& v, const SplineTcbParam& intcb, const SplineTcbParam& outtcb )
{
    TcbSpline * tsp = dynamic_cast<TcbSpline *>(sp);
    SplineEntryList::size_type i;
    int k;

    /* Reset the Coeffs_Computed flag.  Inserting a new point invalidates
     *  pre-computed coefficients */
    sp->Coeffs_Computed = false;
    i = findt(sp, p);
    // If p is already in the spline, replace it.
    bool replace = false;
    if (!sp->SplineEntries.empty())
    {
        // If p matches the _last_ spline entry, `findt()` is implemented to return `sp->SplineEntries.size()`
        // instead of the index of the matching entry; this has some benefits in the other places `findt()` is used,
        // but requires the following workaround here.
        if (i < sp->SplineEntries.size())
            replace = (sp->SplineEntries[i].par == p);
        else if (sp->SplineEntries[i-1].par == p)
        {
            --i;
            replace = true;
        }
    }
    if(replace)
    {
        for(k=0; k<5; k++)
            sp->SplineEntries[i].vec[k] = v[k];
        if (tsp)
        {
            tsp->in[i] = intcb;
            tsp->out[i] = outtcb;
        }
    }
    else
    {
        SplineEntry fresh;
        fresh.par = p;
        for(k=0; k<5; k++)
            fresh.vec[k] = v[k];
        SplineEntryList::iterator pos = sp->SplineEntries.begin();
        pos += i;
        sp->SplineEntries.insert( pos, fresh );
        if (tsp)
        {
            SplineTcbParamList::iterator posi = tsp->in.begin();
            SplineTcbParamList::iterator poso = tsp->out.begin();
            posi += i;
            poso += i;
            tsp->in.insert( posi, intcb );
            tsp->out.insert( poso, outtcb );
        }
    }
}

/*****************************************************************************
*
* FUNCTION
*
*       Get_Spline_Value
*
* INPUT
*
*       sp : a pointer to the spline to interpolate
*       p  : the parameter to interpolate the value for
*       v  : a pointer to a 5D vector to store the interpolated values in
*
* OUTPUT
*
*       v  : a pointer to a 5D vector to store the interpolated values in
*
* RETURNS
*
*       The value of the first element of the interpolated vector
*
* AUTHOR
*
*   Wolfgang Ortmann
*
* DESCRIPTION
*
*       Interpolates the spline at the given point.  If the point is outside the
*               range of parameters of the spline, returns the value at the
*               appropriate endpoint (ie. no extrapolation).  If there are not
*               enough points in the spline to do the desired type of
*               interpolation, does the next best type (cubic->quadratic->linear).
*
* CHANGES
*
*       Mark Wagner  8 Nov 2000 : Complete overhaul.  I am apalled at the
*               problems I found in the original implementation
*       Mark Wagner  25 Aug 2001 : Changed interpolation method to pre-compute
*               coefficients for cubic splines.
*       Mark Wagner  24 Feb 2002 : Added support for Catmull-Rom interpolation
*               Re-arranged the code to make future additions cleaner by moving
*                more of the code into the "switch" statement
*
******************************************************************************/

void LinearSpline::Get(DBL p, EXPRESS& v)
{
    if (SplineEntries.size() == 1)
        memcpy(&v, &SplineEntries.front().vec, sizeof(EXPRESS));
    else
    {
        /* Find which spline segment we're in.  i is the control point at the end of the segment */
        SplineEntryList::size_type i = findt(this, p);
        for(int k=0; k<5; k++)
        {
            /* If outside spline range, return first or last point */
            if(i == 0)
                v[k] = SplineEntries.front().vec[k];
            else if(i >= SplineEntries.size())
                v[k] = SplineEntries.back().vec[k];
            /* Else, normal case */
            else
                v[k] = linear_interpolate(SplineEntries, i-1, k, p);
        }
    }
}

void QuadraticSpline::Get(DBL p, EXPRESS& v)
{
    if (SplineEntries.size() == 1)
        memcpy(&v, &SplineEntries.front().vec, sizeof(EXPRESS));
    else
    {
        /* Find which spline segment we're in.  i is the control point at the end of the segment */
        SplineEntryList::size_type i = findt(this, p);
        for(int k=0; k<5; k++)
        {
            /* If outside the spline range, return the first or last point */
            if(i == 0)
                v[k] = SplineEntries.front().vec[k];
            else if(i >= SplineEntries.size())
                v[k] = SplineEntries.back().vec[k];
            /* If not enough points, reduce order */
            else if(SplineEntries.size() == 2)
                v[k] = linear_interpolate(SplineEntries, i-1, k, p);
            else
            {
                /* Normal case: between the second and last points */
                if(i > 1)
                    v[k] = quadratic_interpolate(SplineEntries, i-1, k, p);
                else /* Special case: between first and second points */
                    v[k] = quadratic_interpolate(SplineEntries, i, k, p);
            }
        }
    }
}

void NaturalSpline::Get(DBL p, EXPRESS& v)
{
    if (SplineEntries.size() == 1)
        memcpy(&v, &SplineEntries.front().vec, sizeof(EXPRESS));
    else
    {
        if (!Coeffs_Computed)
            Precompute();
        /* Find which spline segment we're in.  i is the control point at the end of the segment */
        SplineEntryList::size_type i = findt(this, p);
        for(int k=0; k<5; k++)
        {
            /* If outside the spline range, return the first or last point */
            if(i == 0)
                v[k] = SplineEntries.front().vec[k];
            else if(i >= SplineEntries.size())
                v[k] = SplineEntries.back().vec[k];
            /* Else, normal case.  cubic_interpolate can handle the case of not enough points */
            else
                v[k] = natural_interpolate(SplineEntries, SplinePreComputed, i-1, k, p);
        }
    }
}

void SorSpline::Get(DBL p, EXPRESS& v)
{
    if (SplineEntries.size() == 1)
        memcpy(&v, &SplineEntries.front().vec, sizeof(EXPRESS));
    else
    {
        if (!Coeffs_Computed)
            Precompute();
        SplineEntryList::size_type last = SplineEntries.size()-1;
        SplineEntryList::size_type i = findt(this, p);
        for(int k=0; k<5; k++)
        {
            if(i <= 1)
                v[k] = SplineEntries[1].vec[k];
            else if(i >= last)
                v[k] = SplineEntries[last-1].vec[k];
            else
                v[k] = interpolate( i-1, k, p);
        }
    }
}

void AkimaSpline::Get(DBL p, EXPRESS& v)
{
    if (SplineEntries.size() == 1)
        memcpy(&v, &SplineEntries.front().vec, sizeof(EXPRESS));
    else
    {
        if (!Coeffs_Computed)
            Precompute();
        SplineEntryList::size_type last = SplineEntries.size()-1;
        SplineEntryList::size_type i = findt(this, p);
        for(int k=0; k<5; k++)
        {
            if(i == 0)
                v[k] = SplineEntries.front().vec[k];
            else if(i > last )
                v[k] = SplineEntries.back().vec[k];
            else
                v[k] = interpolate( i-1, k, p);
        }
    }
}

void TcbSpline::Get(DBL p, EXPRESS& v)
{
    if (SplineEntries.size() == 1)
        memcpy(&v, &SplineEntries.front().vec, sizeof(EXPRESS));
    else
    {
        if (!Coeffs_Computed)
            Precompute();
        SplineEntryList::size_type last = SplineEntries.size()-1;
        SplineEntryList::size_type i = findt(this, p);
        for(int k=0; k<5; k++)
        {
            if(i <= 1)
                v[k] = SplineEntries[1].vec[k];
            else if(i >= last)
                v[k] = SplineEntries[last-1].vec[k];
            else
                v[k] = interpolate( i-1, k, p);
        }
    }
}

void BasicXSpline::Get(DBL p, EXPRESS& v)
{
    if (SplineEntries.size() == 1)
        memcpy(&v, &SplineEntries.front().vec, sizeof(EXPRESS));
    else
    {
        SplineEntryList::size_type last = SplineEntries.size()-1;
        SplineEntryList::size_type i = findt(this, p);
        for(int k=0; k<5; k++)
        {
            if(i <= 1)
                v[k] = interpolate( 0, k, SplineEntries[1].par, freedom.freedom_degree );
            else if(i >= last)
                v[k] = interpolate( last-3, k, SplineEntries[last-1].par, freedom.freedom_degree );
            else
                v[k] = interpolate( i-2, k, p, freedom.freedom_degree );
        }
    }
}


void XSpline::Get(DBL p, EXPRESS& v)
{
    if (SplineEntries.size() == 1)
        memcpy(&v, &SplineEntries.front().vec, sizeof(EXPRESS));
    else
    {
        SplineEntryList::size_type last = SplineEntries.size()-1;
        SplineEntryList::size_type i = findt(this, p);
        for(int k=0; k<5; k++)
        {
            if(i == 0)
                v[k] = SplineEntries.front().vec[k];
            else if(i > last )
                v[k] = SplineEntries.back().vec[k];
            else
                v[k] = interpolate( std::max(int(i-2),int(-1)), k, p, last);
        }
    }
}


void CatmullRomSpline::Get(DBL p, EXPRESS& v)
{
    if (SplineEntries.size() == 1)
        memcpy(&v, &SplineEntries.front().vec, sizeof(EXPRESS));
    else
    {
        /* Find which spline segment we're in.  i is the control point at the end of the segment */
        SplineEntryList::size_type i = findt(this, p);
        for(int k=0; k<5; k++)
        {
            /* If only two points, return their average */
            if(SplineEntries.size() == 2)
                v[k] = (SplineEntries[0].vec[k] + SplineEntries[1].vec[k])/2.0;
            /* Catmull-Rom: If only three points, return the second one */
            else if(i < 2)
                v[k] = SplineEntries[1].vec[k];
            /* Catmull-Rom: Can't interpolate before second point or after next-to-last */
            else if(i >= SplineEntries.size()-1)
                v[k] = SplineEntries[SplineEntries.size()-2].vec[k];
            /* Else, normal case */
            else
                v[k] = catmull_rom_interpolate(SplineEntries, i-1, k, p);
        }
    }
}



DBL Get_Spline_Val(GenericSpline *sp, DBL p, EXPRESS& v, int *Terms)
{
    *Terms = sp->Terms;
    sp->Get(p, v);
    return v[0];
}

}
// end of namespace pov
