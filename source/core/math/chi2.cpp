//******************************************************************************
///
/// @file core/math/chi2.cpp
///
/// Implementations related to the chi square distribution.
///
/// @author Stephen L. Moshier
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
/// ----------------------------------------------------------------------------
///
/// This software is derived from the Cephes Math Library and is
/// incorporated herein by permission of the author, Stephen L. Moshier.
///
///   Cephes Math Library Release 2.0:  April, 1987
///   Copyright 1984, 1987 by Stephen L. Moshier
///
/// The author reserves the right to distribute this material elsewhere under
/// different terms. This file is provided within POV-Ray under the following
/// terms:
///
/// Redistribution and use in source and binary forms, with or without
/// modification, are permitted provided that the following conditions are met:
///
///  1. Redistributions of source code must retain the above copyright notice,
///     this list of conditions and the following disclaimer.
///  2. Redistributions in binary form must reproduce the above copyright notice,
///     this list of conditions and the following disclaimer in the documentation
///     and/or other materials provided with the distribution.
///  3. Neither the names of the copyright holders nor the names of contributors
///     may be used to endorse or promote products derived from this software
///     without specific prior written permission.
///
///  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
///  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
///  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
///  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
///  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
///  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
///  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
///  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
///  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
///  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
///  POSSIBILITY OF SUCH DAMAGE.
///
/// @endparblock
///
//******************************************************************************

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/math/chi2.h"

// C++ variants of C standard header files
// C++ standard header files
//  (none at the moment)

// POV-Ray header files (base module)
#include "base/pov_err.h"

// POV-Ray header files (core module)
//  (none at the moment)

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*
Cephes Math Library Release 2.0:  April, 1987
Copyright 1984, 1987 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

const DBL MAXLGM = 2.556348e305;
const DBL BIG    = 1.44115188075855872E+17;
const DBL MACHEP = 1.38777878078144567553E-17; /* 2**-56 */
const DBL MAXLOG = 8.8029691931113054295988E1; /* log(2**127) */
const DBL MAXNUM = 1.701411834604692317316873e38;  /* 2**127 */
const DBL LOGPI  = 1.14472988584940017414;

/*
 * A[]: Stirling's formula expansion of log gamma
 * B[], C[]: log gamma function between 2 and 3
 */

const DBL A[] =
{
     8.11614167470508450300E-4,
    -5.95061904284301438324E-4,
     7.93650340457716943945E-4,
    -2.77777777730099687205E-3,
     8.33333333333331927722E-2
};

const DBL B[] =
{
    -1.37825152569120859100E3,
    -3.88016315134637840924E4,
    -3.31612992738871184744E5,
    -1.16237097492762307383E6,
    -1.72173700820839662146E6,
    -8.53555664245765465627E5
};

const DBL C[] =
{
     1.00000000000000000000E0,
    -3.51815701436523470549E2,
    -1.70642106651881159223E4,
    -2.20528590553854454839E5,
    -1.13933444367982507207E6,
    -2.53252307177582951285E6,
    -2.01889141433532773231E6
};

/* log(sqrt(2pi)) */

const DBL LS2PI = 0.91893853320467274178;

/* sqrt(2pi) */

const DBL s2pi = 2.50662827463100050242E0;

/* approximation for 0 <= |y - 0.5| <= 3/8 */

const DBL P0[5] =
{
    -5.99633501014107895267E1,
     9.80010754185999661536E1,
    -5.66762857469070293439E1,
     1.39312609387279679503E1,
    -1.23916583867381258016E0,
};

const DBL Q0[8] =
{
/*   1.00000000000000000000E0,*/
     1.95448858338141759834E0,
     4.67627912898881538453E0,
     8.63602421390890590575E1,
    -2.25462687854119370527E2,
     2.00260212380060660359E2,
    -8.20372256168333339912E1,
     1.59056225126211695515E1,
    -1.18331621121330003142E0,
};

/*
 * Approximation for interval z = sqrt(-2 log y ) between 2 and 8
 * i.e., y between exp(-2) = .135 and exp(-32) = 1.27e-14.
 */

const DBL P1[9] =
{
     4.05544892305962419923E0,
     3.15251094599893866154E1,
     5.71628192246421288162E1,
     4.40805073893200834700E1,
     1.46849561928858024014E1,
     2.18663306850790267539E0,
    -1.40256079171354495875E-1,
    -3.50424626827848203418E-2,
    -8.57456785154685413611E-4,
};

const DBL Q1[8] =
{
/*   1.00000000000000000000E0,*/
     1.57799883256466749731E1,
     4.53907635128879210584E1,
     4.13172038254672030440E1,
     1.50425385692907503408E1,
     2.50464946208309415979E0,
    -1.42182922854787788574E-1,
    -3.80806407691578277194E-2,
    -9.33259480895457427372E-4,
};

/*
 * Approximation for interval z = sqrt(-2 log y ) between 8 and 64
 * i.e., y between exp(-32) = 1.27e-14 and exp(-2048) = 3.67e-890.
 */

const DBL P2[9] =
{
    3.23774891776946035970E0,
    6.91522889068984211695E0,
    3.93881025292474443415E0,
    1.33303460815807542389E0,
    2.01485389549179081538E-1,
    1.23716634817820021358E-2,
    3.01581553508235416007E-4,
    2.65806974686737550832E-6,
    6.23974539184983293730E-9,
};

const DBL Q2[8] =
{
/*  1.00000000000000000000E0,*/
    6.02427039364742014255E0,
    3.67983563856160859403E0,
    1.37702099489081330271E0,
    2.16236993594496635890E-1,
    1.34204006088543189037E-2,
    3.28014464682127739104E-4,
    2.89247864745380683936E-6,
    6.79019408009981274425E-9,
};


/*****************************************************************************
* Static functions
******************************************************************************/

static DBL igami (DBL a, DBL y0);
static DBL lgam (DBL x, int *sgngam);
static DBL polevl (DBL x, const DBL * coef, int N);
static DBL p1evl (DBL x, const DBL * coef, int N);
static DBL igamc (DBL a, DBL x);
static DBL igam (DBL a, DBL x);



/*              chdtri()
 *
 *  Inverse of complemented Chi-square distribution
 *
 *
 *
 * SYNOPSIS:
 *
 * DBL df, x, y, chdtri();
 *
 * x = chdtri( df, y );
 *
 *
 *
 *
 * DESCRIPTION:
 *
 * Finds the Chi-square argument x such that the integral
 * from x to infinity of the Chi-square density is equal
 * to the given cumulative probability y.
 *
 * This is accomplished using the inverse gamma integral
 * function and the relation
 *
 *    x/2 = igami( df/2, y );
 *
 *
 *
 *
 * ACCURACY:
 *
 * See igami.c.
 *
 * ERROR MESSAGES:
 *
 *   message         condition      value returned
 * chdtri domain   y < 0 or y > 1        0.0
 *                     v < 1
 *
 */

DBL chdtri(DBL df, DBL  y)
{
    DBL x;

    if ((y < 0.0) || (y > 1.0) || (df < 1.0))
        throw POV_EXCEPTION_STRING("Illegal values in chdtri().");

    x = igami(0.5 * df, y);

    return (2.0 * x);
}



/*              lgam()
 *
 *  Natural logarithm of gamma function
 *
 *
 *
 * SYNOPSIS:
 *
 * DBL x, y, lgam();
 *
 * y = lgam( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the base e (2.718...) logarithm of the absolute
 * value of the gamma function of the argument.
 * The sign (+1 or -1) of the gamma function is returned in a
 * variable named sgngam.
 *
 * For arguments greater than 13, the logarithm of the gamma
 * function is approximated by the logarithmic version of
 * Stirling's formula using a polynomial approximation of
 * degree 4. Arguments between -33 and +33 are reduced by
 * recurrence to the interval [2,3] of a rational approximation.
 * The cosecant reflection formula is employed for arguments
 * less than -33.
 *
 * Arguments greater than MAXLGM return MAXNUM and an error
 * message.  MAXLGM = 2.035093e36 for DEC
 * arithmetic or 2.556348e305 for IEEE arithmetic.
 *
 *
 *
 * ACCURACY:
 *
 *
 * arithmetic      domain        # trials     peak         rms
 *    DEC     0, 3                  7000     5.2e-17     1.3e-17
 *    DEC     2.718, 2.035e36       5000     3.9e-17     9.9e-18
 *    IEEE    0, 3                 28000     5.4e-16     1.1e-16
 *    IEEE    2.718, 2.556e305     40000     3.5e-16     8.3e-17
 * The error criterion was relative when the function magnitude
 * was greater than one but absolute when it was less than one.
 *
 * The following test used the relative error criterion, though
 * at certain points the relative error could be much higher than
 * indicated.
 *    IEEE    -200, -4             10000     4.8e-16     1.3e-16
 *
 */

static DBL lgam(DBL x, int *sgngam)
{
    DBL p, q, w, z;
    int i;

    *sgngam = 1;

    if (x < -34.0)
    {
        q = -x;
        w = lgam(q, sgngam);  /* note this modifies sgngam! */
        p = floor(q);

        if (p == q)
        {
            goto loverf;
        }

        i = p;

        if ((i & 1) == 0)
        {
            *sgngam = -1;
        }
        else
        {
            *sgngam = 1;
        }

        z = q - p;

        if (z > 0.5)
        {
            p += 1.0;

            z = p - q;
        }

        z = q * sin(M_PI * z);

        if (z == 0.0)
        {
            goto loverf;
        }

/*      z = log(M_PI) - log( z ) - w;*/

        z = LOGPI - log(z) - w;

        return (z);
    }

    if (x < 13.0)
    {
        z = 1.0;

        while (x >= 3.0)
        {
            x -= 1.0;

            z *= x;
        }

        while (x < 2.0)
        {
            if (x == 0.0)
            {
                goto loverf;
            }

            z /= x;

            x += 1.0;
        }

        if (z < 0.0)
        {
            *sgngam = -1;

            z = -z;
        }
        else
        {
            *sgngam = 1;
        }

        if (x == 2.0)
        {
            return (log(z));
        }

        x -= 2.0;

        p = x * polevl(x, B, 5) / p1evl(x, C, 6);

        return (log(z) + p);
    }

    if (x > MAXLGM)
    {
        loverf:
/*
        mtherr("lgam", OVERFLOW);
*/
        return (*sgngam * MAXNUM);
    }

    q = (x - 0.5) * log(x) - x + LS2PI;

    if (x > 1.0e8)
    {
        return (q);
    }

    p = 1.0 / (x * x);

    if (x >= 1000.0)
    {
        q += ((7.9365079365079365079365e-4 * p -
               2.7777777777777777777778e-3) * p +
               0.0833333333333333333333) / x;
    }
    else
    {
        q += polevl(p, A, 4) / x;
    }

    return (q);
}



/*              igamc()
 *
 *  Complemented incomplete gamma integral
 *
 *
 *
 * SYNOPSIS:
 *
 * DBL a, x, y, igamc();
 *
 * y = igamc( a, x );
 *
 *
 *
 * DESCRIPTION:
 *
 * The function is defined by
 *
 *
 *  igamc(a,x)   =   1 - igam(a,x)
 *
 *                            inf.
 *                              -
 *                     1       | |  -t  a-1
 *               =   -----     |   e   t   dt.
 *                    -      | |
 *                   | (a)    -
 *                             x
 *
 *
 * In this implementation both arguments must be positive.
 * The integral is evaluated by either a power series or
 * continued fraction expansion, depending on the relative
 * values of a and x.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       0,30         2000       2.7e-15     4.0e-16
 *    IEEE      0,30        60000       1.4e-12     6.3e-15
 *
 */

static DBL igamc(DBL a, DBL  x)
{
    DBL ans, c, yc, ax, y, z;
    DBL pk, pkm1, pkm2, qk, qkm1, qkm2;
    DBL r, t;
    int sgngam = 0;

    if ((x <= 0) || (a <= 0))
    {
        return (1.0);
    }

    if ((x < 1.0) || (x < a))
    {
        return (1.0 - igam(a, x));
    }

    ax = a * log(x) - x - lgam(a, &sgngam);

    if (ax < -MAXLOG)
    {
/*
        mtherr("igamc", UNDERFLOW);
*/
        return (0.0);
    }

    ax = exp(ax);

/* continued fraction */

    y = 1.0 - a;
    z = x + y + 1.0;
    c = 0.0;

    pkm2 = 1.0;
    qkm2 = x;
    pkm1 = x + 1.0;
    qkm1 = z * x;

    ans = pkm1 / qkm1;

    do
    {
        c += 1.0;
        y += 1.0;
        z += 2.0;

        yc = y * c;

        pk = pkm1 * z - pkm2 * yc;
        qk = qkm1 * z - qkm2 * yc;

        if (qk != 0)
        {
            r = pk / qk;
            t = fabs((ans - r) / r);
            ans = r;
        }
        else
        {
            t = 1.0;
        }

        pkm2 = pkm1;
        pkm1 = pk;
        qkm2 = qkm1;
        qkm1 = qk;

        if (fabs(pk) > BIG)
        {
            pkm2 /= BIG;
            pkm1 /= BIG;
            qkm2 /= BIG;
            qkm1 /= BIG;
        }
    }
    while (t > MACHEP);

    return (ans * ax);
}



/*              igam.c
 *
 *  Incomplete gamma integral
 *
 *
 *
 * SYNOPSIS:
 *
 * DBL a, x, y, igam();
 *
 * y = igam( a, x );
 *
 *
 *
 * DESCRIPTION:
 *
 * The function is defined by
 *
 *                           x
 *                            -
 *                   1       | |  -t  a-1
 *  igam(a,x)  =   -----     |   e   t   dt.
 *                  -      | |
 *                 | (a)    -
 *                           0
 *
 *
 * In this implementation both arguments must be positive.
 * The integral is evaluated by either a power series or
 * continued fraction expansion, depending on the relative
 * values of a and x.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       0,30         4000       4.4e-15     6.3e-16
 *    IEEE      0,30        10000       3.6e-14     5.1e-15
 *
 */

/* left tail of incomplete gamma function:
 *
 *          inf.      k
 *   a  -x   -       x
 *  x  e     >   ----------
 *           -     -
 *          k=0   | (a+k+1)
 *
 */

static DBL igam(DBL a, DBL  x)
{
    DBL ans, ax, c, r;
    int sgngam = 0;

    if ((x <= 0) || (a <= 0))
    {
        return (0.0);
    }

    if ((x > 1.0) && (x > a))
    {
        return (1.0 - igamc(a, x));
    }

/* Compute  x**a * exp(-x) / gamma(a)  */
    ax = a * log(x) - x - lgam(a, &sgngam);

    if (ax < -MAXLOG)
    {
/*
        mtherr("igam", UNDERFLOW);
*/
        return (0.0);
    }

    ax = exp(ax);

/* power series */
    r = a;
    c = 1.0;
    ans = 1.0;

    do
    {
        r += 1.0;
        c *= x / r;
        ans += c;
    }
    while (c / ans > MACHEP);

    return (ans * ax / a);
}



/*              igami()
 *
 *      Inverse of complemented imcomplete gamma integral
 *
 *
 *
 * SYNOPSIS:
 *
 * DBL a, x, y, igami();
 *
 * x = igami( a, y );
 *
 *
 *
 * DESCRIPTION:
 *
 * Given y, the function finds x such that
 *
 *  igamc( a, x ) = y.
 *
 * Starting with the approximate value
 *
 *         3
 *  x = a t
 *
 *  where
 *
 *  t = 1 - d - ndtri(y) sqrt(d)
 *
 * and
 *
 *  d = 1/9a,
 *
 * the routine performs up to 10 Newton iterations to find the
 * root of igamc(a,x) - y = 0.
 *
 *
 * ACCURACY:
 *
 * Tested for a ranging from 0.5 to 30 and x from 0 to 0.5.
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       0,0.5         3400       8.8e-16     1.3e-16
 *    IEEE      0,0.5        10000       1.1e-14     1.0e-15
 *
 */

static DBL igami(DBL a, DBL  y0)
{
    DBL d, y, x0, lgm;
    int i;
    int sgngam = 0;

/* approximation to inverse function */
    d = 1.0 / (9.0 * a);
    y = (1.0 - d - ndtri(y0) * sqrt(d));

    x0 = a * y * y * y;

    lgm = lgam(a, &sgngam);

    for (i = 0; i < 10; i++)
    {
        if (x0 <= 0.0)
        {
/*
            mtherr("igami", UNDERFLOW);
*/
            return (0.0);
        }

        y = igamc(a, x0);

/* compute the derivative of the function at this point */
        d = (a - 1.0) * log(x0) - x0 - lgm;

        if (d < -MAXLOG)
        {
/*
            mtherr("igami", UNDERFLOW);
*/
            goto done;
        }

        d = -exp(d);

/* compute the step to the next approximation of x */
        if (d == 0.0)
        {
            goto done;
        }

        d = (y - y0) / d;

        x0 = x0 - d;

        if (i < 3)
        {
            continue;
        }

        if (fabs(d / x0) < 2.0 * MACHEP)
        {
            goto done;
        }
    }

done:

    return (x0);
}



/*              ndtri.c
 *
 *  Inverse of Normal distribution function
 *
 *
 *
 * SYNOPSIS:
 *
 * DBL x, y, ndtri();
 *
 * x = ndtri( y );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the argument, x, for which the area under the
 * Gaussian probability density function (integrated from
 * minus infinity to x) is equal to y.
 *
 *
 * For small arguments 0 < y < exp(-2), the program computes
 * z = sqrt( -2.0 * log(y) );  then the approximation is
 * x = z - log(z)/z  - (1/z) P(1/z) / Q(1/z).
 * There are two rational functions P/Q, one for 0 < y < exp(-32)
 * and the other for y up to exp(-2).  For larger arguments,
 * w = y - 0.5, and  x/sqrt(2pi) = w + w**3 R(w**2)/S(w**2)).
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain        # trials      peak         rms
 *    DEC      0.125, 1         5500       9.5e-17     2.1e-17
 *    DEC      6e-39, 0.135     3500       5.7e-17     1.3e-17
 *    IEEE     0.125, 1        20000       7.2e-16     1.3e-16
 *    IEEE     3e-308, 0.135   50000       4.6e-16     9.8e-17
 *
 *
 * ERROR MESSAGES:
 *
 *   message         condition    value returned
 * ndtri domain       x <= 0        -MAXNUM
 * ndtri domain       x >= 1         MAXNUM
 *
 */

DBL ndtri(DBL y0)
{
    DBL x, y, z, y2, x0, x1;
    int code;

    if (y0 <= 0.0)
    {
/*
        mtherr("ndtri", DOMAIN);
*/
        return (-MAXNUM);
    }

    if (y0 >= 1.0)
    {
/*
        mtherr("ndtri", DOMAIN);
*/
        return (MAXNUM);
    }

    code = 1;

    y = y0;

    if (y > (1.0 - 0.13533528323661269189)) /* 0.135... = exp(-2) */
    {
        y = 1.0 - y;
        code = 0;
    }

    if (y > 0.13533528323661269189)
    {
        y = y - 0.5;
        y2 = y * y;
        x = y + y * (y2 * polevl(y2, P0, 4) / p1evl(y2, Q0, 8));
        x = x * s2pi;

        return (x);
    }

    x = sqrt(-2.0 * log(y));
    x0 = x - log(x) / x;

    z = 1.0 / x;

    if (x < 8.0)  /* y > exp(-32) = 1.2664165549e-14 */
    {
        x1 = z * polevl(z, P1, 8) / p1evl(z, Q1, 8);
    }
    else
    {
        x1 = z * polevl(z, P2, 8) / p1evl(z, Q2, 8);
    }

    x = x0 - x1;

    if (code != 0)
    {
        x = -x;
    }

    return (x);
}



/*              polevl.c
 *              p1evl.c
 *
 *  Evaluate polynomial
 *
 *
 *
 * SYNOPSIS:
 *
 * int N;
 * DBL x, y, coef[N+1], polevl[];
 *
 * y = polevl( x, coef, N );
 *
 *
 *
 * DESCRIPTION:
 *
 * Evaluates polynomial of degree N:
 *
 *                     2          N
 * y  =  C  + C x + C x  +...+ C x
 *        0    1     2          N
 *
 * Coefficients are stored in reverse order:
 *
 * coef[0] = C  , ..., coef[N] = C  .
 *            N                   0
 *
 *  The function p1evl() assumes that coef[N] = 1.0 and is
 * omitted from the array.  Its calling arguments are
 * otherwise the same as polevl().
 *
 *
 * SPEED:
 *
 * In the interest of speed, there are no checks for out
 * of bounds arithmetic.  This routine is used by most of
 * the functions in the library.  Depending on available
 * equipment features, the user may wish to rewrite the
 * program in microcode or assembly language.
 *
 */

static DBL polevl(DBL x, const DBL coef[], int N)
{
    DBL ans;
    int i;
    DBL const *p;

    p = coef;
    ans = *p++;
    i = N;

    do
    {
        ans = ans * x + *p++;
    }
    while (--i);

    return (ans);
}

/*              p1evl() */
/*                                          N
 * Evaluate polynomial when coefficient of x  is 1.0.
 * Otherwise same as polevl.
 */

static DBL p1evl(DBL x, const DBL coef[], int N)
{
    DBL ans;
    DBL const *p;
    int i;

    p = coef;
    ans = x + *p++;
    i = N - 1;

    do
    {
        ans = ans * x + *p++;
    }
    while (--i);

    return (ans);
}

}
// end of namespace pov
