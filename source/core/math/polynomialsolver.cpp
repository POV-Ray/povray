//******************************************************************************
///
/// @file core/math/polynomialsolver.cpp
///
/// Implementations related to solving polynomial equations.
///
/// @author Alexander Enzmann
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
#include "core/math/polynomialsolver.h"

#include "core/support/statistics.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

/*                  WARNING     WARNING    WARNING

   The following three constants have been defined so that quartic equations
   will properly render on fpu/compiler combinations that only have 64 bit
   IEEE precision.  Do not arbitrarily change any of these values.

   If you have a machine with a proper fpu/compiler combo - like a Mac with
   a 68881, then use the native floating format (96 bits) and tune the
   values for a little less tolerance: something like: factor1 = 1.0e15,
   factor2 = -1.0e-7, factor3 = 1.0e-10.

   The meaning of the three constants are:
      factor1 - the magnitude of difference between coefficients in a
                quartic equation at which the Sturmian root solver will
                kick in.  The Sturm solver is quite a bit slower than
                the algebraic solver, so the bigger this is, the faster
                the root solving will go.  The algebraic solver is less
                accurate so the bigger this is, the more likely you will
                get bad roots.

      factor2 - Tolerance value that defines how close the quartic equation
                is to being a square of a quadratic.  The closer this can
                get to zero before roots disappear, the less the chance
                you will get spurious roots.

      factor3 - Similar to factor2 at a later stage of the algebraic solver.
*/

#ifndef FUDGE_FACTOR1
    #define FUDGE_FACTOR1 1.0e12
#endif
#ifndef FUDGE_FACTOR2
    #define FUDGE_FACTOR2 -1.0e-5
#endif
#ifndef FUDGE_FACTOR3
    #define FUDGE_FACTOR3 1.0e-7
#endif

/* Constants. */
const DBL TWO_M_PI_3  = 2.0943951023931954923084;
const DBL FOUR_M_PI_3 = 4.1887902047863909846168;

/* Max number of iterations. */
const int MAX_ITERATIONS = 50;

/* A coefficient smaller than SMALL_ENOUGH is considered to be zero (0.0). */
const DBL SMALL_ENOUGH = 1.0e-10;

/* Smallest relative error we want. */
const DBL RELERROR = 1.0e-12;


/*****************************************************************************
* Local typedefs
******************************************************************************/

struct polynomial
{
    int ord;
    DBL coef[MAX_ORDER+1];
};


/*****************************************************************************
* Static functions
******************************************************************************/

static int solve_quadratic (const DBL *x, DBL *y);
static int solve_cubic (const DBL *x, DBL *y);
static int solve_quartic (const DBL *x, DBL *y);
static int polysolve (int order, const DBL *Coeffs, DBL *roots);
static int modp (const polynomial *u, const polynomial *v, polynomial *r);
static int regula_falsa (int order, const DBL *coef, DBL a, DBL b, DBL *val);
static int sbisect (int np, const polynomial *sseq, DBL min, DBL max, int atmin, int atmax, DBL *roots);
static int numchanges (int np, const polynomial *sseq, DBL a);
static DBL polyeval (DBL x, int n, const DBL *Coeffs);
static int buildsturm (int ord, polynomial *sseq);
static int visible_roots (int np, const polynomial *sseq, int *atneg, int *atpos);
static int difficult_coeffs (int n, const DBL *x);


/*****************************************************************************
*
* FUNCTION
*
*   modp
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Calculates the modulus of u(x) / v(x) leaving it in r.
*   It returns 0 if r(x) is a constant.
*
*   NOTE: This function assumes the leading coefficient of v is 1 or -1.
*
* CHANGES
*
*   Okt 1996 : Added bug fix by Heiko Eissfeldt. [DB]
*
******************************************************************************/

static int modp(const polynomial *u, const polynomial *v, polynomial *r)
{
    int k, j;

    *r = *u;

    if (v->coef[v->ord] < 0.0)
    {
        for (k = u->ord - v->ord - 1; k >= 0; k -= 2)
        {
            r->coef[k] = -r->coef[k];
        }

        for (k = u->ord - v->ord; k >= 0; k--)
        {
            for (j = v->ord + k - 1; j >= k; j--)
            {
                r->coef[j] = -r->coef[j] - r->coef[v->ord + k] * v->coef[j - k];
            }
        }
    }
    else
    {
        for (k = u->ord - v->ord; k >= 0; k--)
        {
            for (j = v->ord + k - 1; j >= k; j--)
            {
                r->coef[j] -= r->coef[v->ord + k] * v->coef[j - k];
            }
        }
    }

    k = v->ord - 1;

    while (k >= 0 && fabs(r->coef[k]) < SMALL_ENOUGH)
    {
        r->coef[k] = 0.0;

        k--;
    }

    r->ord = (k < 0) ? 0 : k;

    return(r->ord);
}



/*****************************************************************************
*
* FUNCTION
*
*   buildsturm
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Build the sturmian sequence for a polynomial.
*
* CHANGES
*
*   -
*
******************************************************************************/

static int buildsturm(int ord, polynomial *sseq)
{
    int i;
    DBL f;
    DBL *fp, *fc;
    polynomial *sp;

    sseq[0].ord = ord;
    sseq[1].ord = ord - 1;

    /* calculate the derivative and normalize the leading coefficient. */

    f = fabs(sseq[0].coef[ord] * ord);

    fp = sseq[1].coef;
    fc = sseq[0].coef + 1;

    for (i = 1; i <= ord; i++)
    {
        *fp++ = *fc++ * i / f;
    }

    /* construct the rest of the Sturm sequence */

    for (sp = sseq + 2; modp(sp - 2, sp - 1, sp); sp++)
    {
        /* reverse the sign and normalize */

        f = -fabs(sp->coef[sp->ord]);

        for (fp = &sp->coef[sp->ord]; fp >= sp->coef; fp--)
        {
            *fp /= f;
        }
    }

    /* reverse the sign */

    sp->coef[0] = -sp->coef[0];

    return(sp - sseq);
}



/*****************************************************************************
*
* FUNCTION
*
*   visible_roots
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Find out how many visible intersections there are.
*
* CHANGES
*
*   -
*
******************************************************************************/

static int visible_roots(int np, const polynomial *sseq, int *atzer, int  *atpos)
{
    int atposinf, atzero;
    const polynomial *s;
    DBL f, lf;

    atposinf = atzero = 0;

    /* changes at positve infinity */

    lf = sseq[0].coef[sseq[0].ord];

    for (s = sseq + 1; s <= sseq + np; s++)
    {
        f = s->coef[s->ord];

        if (lf == 0.0 || lf * f < 0)
        {
            atposinf++;
        }

        lf = f;
    }

    /* Changes at zero */

    lf = sseq[0].coef[0];

    for (s = sseq+1; s <= sseq + np; s++)
    {
        f = s->coef[0];

        if (lf == 0.0 || lf * f < 0)
        {
            atzero++;
        }

        lf = f;
    }

    *atzer = atzero;
    *atpos = atposinf;

    return(atzero - atposinf);
}



/*****************************************************************************
*
* FUNCTION
*
*   numchanges
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Return the number of sign changes in the Sturm sequence in
*   sseq at the value a.
*
* CHANGES
*
*   -
*
******************************************************************************/

static int numchanges(int np, const polynomial *sseq, DBL a)
{
    int changes;
    DBL f, lf;
    const polynomial *s;
    changes = 0;

    lf = polyeval(a, sseq[0].ord, sseq[0].coef);

    for (s = sseq + 1; s <= sseq + np; s++)
    {
        f = polyeval(a, s->ord, s->coef);

        if (lf == 0.0 || lf * f < 0)
        {
            changes++;
        }

        lf = f;
    }

    return(changes);
}



/*****************************************************************************
*
* FUNCTION
*
*   sbisect
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Uses a bisection based on the sturm sequence for the polynomial
*   described in sseq to isolate intervals in which roots occur,
*   the roots are returned in the roots array in order of magnitude.
*
*   NOTE: This routine has one severe bug: When the interval containing the
*         root [min, max] has a root at one of its endpoints, as well as one
*         within the interval, the root at the endpoint will be returned
*         rather than the one inside.
*
* CHANGES
*
*   -
*
******************************************************************************/

static int sbisect(int np, const polynomial *sseq, DBL min_value, DBL  max_value, int atmin, int  atmax, DBL *roots)
{
    DBL mid;
    int n1, n2, its, atmid;

    if ((atmin - atmax) == 1)
    {
        /* first try using regula-falsa to find the root.  */

        if (regula_falsa(sseq->ord, sseq->coef, min_value, max_value, roots))
        {
            return(1);
        }
        else
        {
            /* That failed, so now find it by bisection */

            for (its = 0; its < MAX_ITERATIONS; its++)
            {
                mid = (min_value + max_value) / 2;

                atmid = numchanges(np, sseq, mid);

                /* The follow only happens if there is a bug.  And
                   unfortunately, there is. CEY 04/97
                 */
                if ((atmid<atmax) || (atmid>atmin))
                {
                    return(0);
                }

                if (fabs(mid) > RELERROR)
                {
                    if (fabs((max_value - min_value) / mid) < RELERROR)
                    {
                        roots[0] = mid;

                        return(1);
                    }
                }
                else
                {
                    if (fabs(max_value - min_value) < RELERROR)
                    {
                        roots[0] = mid;

                        return(1);
                    }
                }

                if ((atmin - atmid) == 0)
                {
                    min_value = mid;
                }
                else
                {
                    max_value = mid;
                }
            }

            /* Bisection took too long - just return what we got */

            roots[0] = mid;

            return(1);
        }
    }

    /* There is more than one root in the interval.
       Bisect to find new intervals. */

    for (its = 0; its < MAX_ITERATIONS; its++)
    {
        mid = (min_value + max_value) / 2;

        atmid = numchanges(np, sseq, mid);

        /* The follow only happens if there is a bug.  And
           unfortunately, there is. CEY 04/97
         */
        if ((atmid<atmax) || (atmid>atmin))
        {
            return(0);
        }

        if (fabs(mid) > RELERROR)
        {
            if (fabs((max_value - min_value) / mid) < RELERROR)
            {
                roots[0] = mid;

                return(1);
            }
        }
        else
        {
            if (fabs(max_value - min_value) < RELERROR)
            {
                roots[0] = mid;

                return(1);
            }
        }

        n1 = atmin - atmid;
        n2 = atmid - atmax;

        if ((n1 != 0) && (n2 != 0))
        {
            n1 = sbisect(np, sseq, min_value, mid, atmin, atmid, roots);
            n2 = sbisect(np, sseq, mid, max_value, atmid, atmax, &roots[n1]);

            return(n1 + n2);
        }

        if (n1 == 0)
        {
            min_value = mid;
        }
        else
        {
            max_value = mid;
        }
    }

    /* Took too long to bisect.  Just return what we got. */

    roots[0] = mid;

    return(1);
}



/*****************************************************************************
*
* FUNCTION
*
*   polyeval
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Evaluate the value of a polynomial at the given value x.
*
*   The coefficients are stored in c in the following order:
*
*     c[0] + c[1] * x + c[2] * x ^ 2 + c[3] * x ^ 3 + ...
*
* CHANGES
*
*   -
*
******************************************************************************/

static DBL polyeval(DBL x, int n, const DBL *Coeffs)
{
    register int i;
    DBL val;

    val = Coeffs[n];

    for (i = n-1; i >= 0; i--)
    {
        val = val * x + Coeffs[i];
    }

    return(val);
}



/*****************************************************************************
*
* FUNCTION
*
*   regular_falsa
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Close in on a root by using regula-falsa.
*
* CHANGES
*
*   -
*
******************************************************************************/

static int regula_falsa(int order, const DBL *coef, DBL a, DBL b, DBL *val)
{
    int its;
    DBL fa, fb, x, fx, lfx;

    fa = polyeval(a, order, coef);
    fb = polyeval(b, order, coef);

    if (fa * fb > 0.0)
    {
        return 0;
    }

    if (fabs(fa) < SMALL_ENOUGH)
    {
        *val = a;

        return(1);
    }

    if (fabs(fb) < SMALL_ENOUGH)
    {
        *val = b;

        return(1);
    }

    lfx = fa;

    for (its = 0; its < MAX_ITERATIONS; its++)
    {
        x = (fb * a - fa * b) / (fb - fa);

        fx = polyeval(x, order, coef);

        if (fabs(x) > RELERROR)
        {
            if (fabs(fx / x) < RELERROR)
            {
                *val = x;

                return(1);
            }
        }
        else
        {
            if (fabs(fx) < RELERROR)
            {
                *val = x;

                return(1);
            }
        }

        if (fa < 0)
        {
            if (fx < 0)
            {
                a = x;

                fa = fx;

                if ((lfx * fx) > 0)
                {
                    fb /= 2;
                }
            }
            else
            {
                b = x;

                fb = fx;

                if ((lfx * fx) > 0)
                {
                    fa /= 2;
                }
            }
        }
        else
        {
            if (fx < 0)
            {
                b = x;

                fb = fx;

                if ((lfx * fx) > 0)
                {
                    fa /= 2;
                }
            }
            else
            {
                a = x;

                fa = fx;

                if ((lfx * fx) > 0)
                {
                    fb /= 2;
                }
            }
        }

        /* Check for underflow in the domain */

        if (fabs(b-a) < RELERROR)
        {
            *val = x;

            return(1);
        }

        lfx = fx;
    }

    return(0);
}



/*****************************************************************************
*
* FUNCTION
*
*   solve_quadratic
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Solve the quadratic equation:
*
*     x[0] * x^2 + x[1] * x + x[2] = 0.
*
*   The value returned by this function is the number of real roots.
*   The roots themselves are returned in y[0], y[1].
*
* CHANGES
*
*   -
*
******************************************************************************/

static int solve_quadratic(const DBL *x, DBL *y)
{
    DBL d, t, a, b, c;

    a = x[0];
    b = -x[1];
    c = x[2];

    if (a == 0.0)
    {
        if (b == 0.0)
        {
            return(0);
        }

        y[0] = c / b;

        return(1);
    }

    // normalize the coefficients
    b /= a;
    c /= a;
    a  = 1.0;

    d = b * b - 4.0 * a * c;

    /* Treat values of d around 0 as 0. */

    if ((d > -SMALL_ENOUGH) && (d < SMALL_ENOUGH))
    {
        y[0] = 0.5 * b / a;

        return(1);
    }
    else
    {
        if (d < 0.0)
        {
            return(0);
        }
    }

    d = sqrt(d);

    t = 2.0 * a;

    y[0] = (b + d) / t;
    y[1] = (b - d) / t;

    return(2);
}



/*****************************************************************************
*
* FUNCTION
*
*   solve_cubic
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*
*   Solve the cubic equation:
*
*     x[0] * x^3 + x[1] * x^2 + x[2] * x + x[3] = 0.
*
*   The result of this function is an integer that tells how many real
*   roots exist.  Determination of how many are distinct is up to the
*   process that calls this routine.  The roots that exist are stored
*   in (y[0], y[1], y[2]).
*
*   NOTE: This function relies very heavily on trigonometric functions and
*         the square root function.  If an alternative solution is found
*         that does not rely on transcendentals this code will be replaced.
*
* CHANGES
*
*   -
*
******************************************************************************/

static int solve_cubic(const DBL *x, DBL *y)
{
    DBL Q, R, Q3, R2, sQ, d, an, theta;
    DBL A2, a0, a1, a2, a3;

    a0 = x[0];

    if (a0 == 0.0)
    {
        return(solve_quadratic(&x[1], y));
    }
    else
    {
        if (a0 != 1.0)
        {
            a1 = x[1] / a0;
            a2 = x[2] / a0;
            a3 = x[3] / a0;
        }
        else
        {
            a1 = x[1];
            a2 = x[2];
            a3 = x[3];
        }
    }

    A2 = a1 * a1;

    Q = (A2 - 3.0 * a2) / 9.0;

    /* Modified to save some multiplications and to avoid a floating point
       exception that occured with DJGPP and full optimization. [DB 8/94] */

    R = (a1 * (A2 - 4.5 * a2) + 13.5 * a3) / 27.0;

    Q3 = Q * Q * Q;

    R2 = R * R;

    d = Q3 - R2;

    an = a1 / 3.0;

    if (d >= 0.0)
    {
        /* Three real roots. */

        d = R / sqrt(Q3);

        theta = acos(d) / 3.0;

        sQ = -2.0 * sqrt(Q);

        y[0] = sQ * cos(theta) - an;
        y[1] = sQ * cos(theta + TWO_M_PI_3) - an;
        y[2] = sQ * cos(theta + FOUR_M_PI_3) - an;

        return(3);
    }
    else
    {
        sQ = pow(sqrt(R2 - Q3) + fabs(R), 1.0 / 3.0);

        if (R < 0)
        {
            y[0] = (sQ + Q / sQ) - an;
        }
        else
        {
            y[0] = -(sQ + Q / sQ) - an;
        }

        return(1);
    }
}

#ifdef USE_NEW_DIFFICULT_COEFFS

/*****************************************************************************
*
* FUNCTION
*
*   difficult_coeffs
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Test to see if any coeffs are more than 6 orders of magnitude
*   larger than the smallest.
*
* CHANGES
*
*   -
*
******************************************************************************/

static int difficult_coeffs(int n, DBL *x)
{
    int i, flag = 0;
    DBL t, biggest;

    biggest = fabs(x[0]);

    for (i = 1; i <= n; i++)
    {
        t = fabs(x[i]);

        if (t > biggest)
        {
            biggest = t;
        }
    }

    /* Everything is zero no sense in doing any more */

    if (biggest == 0.0)
    {
        return(0);
    }

    for (i = 0; i <= n; i++)
    {
        if (x[i] != 0.0)
        {
            if (fabs(biggest / x[i]) > FUDGE_FACTOR1)
            {
                x[i] = 0.0;
                flag = 1;
            }
        }
    }

    return(flag);
}
#else
/*****************************************************************************
*
* FUNCTION
*
*   difficult_coeffs
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Test to see if any coeffs are more than 6 orders of magnitude
*   larger than the smallest (function from POV 1.0) [DB 8/94].
*
* CHANGES
*
*   -
*
******************************************************************************/

static int difficult_coeffs(int n, const DBL *x)
{
    int i;
    DBL biggest;

    biggest = 0.0;

    for (i = 0; i <= n; i++)
    {
        if (fabs(x[i]) > biggest)
        {
            biggest = x[i];
        }
    }

    /* Everything is zero no sense in doing any more */

    if (biggest == 0.0)
    {
        return(0);
    }

    for (i = 0; i <= n; i++)
    {
        if (x[i] != 0.0)
        {
            if (fabs(biggest / x[i]) > FUDGE_FACTOR1)
            {
                return(1);
            }
        }
    }

    return(0);
}

#endif

#ifdef TEST_SOLVER
/*****************************************************************************
*
* FUNCTION
*
*   solve_quartic
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   The old way of solving quartics algebraically.
*   This is an adaptation of the method of Lodovico Ferrari (Circa 1731).
*
* CHANGES
*
*   -
*
******************************************************************************/

static int solve_quartic(const DBL *x, DBL *results)
{
    DBL cubic[4], roots[3];
    DBL a0, a1, y, d1, x1, t1, t2;
    DBL c0, c1, c2, c3, c4, d2, q1, q2;
    int i;

    c0 = x[0];

    if (c0 != 1.0)
    {
        c1 = x[1] / c0;
        c2 = x[2] / c0;
        c3 = x[3] / c0;
        c4 = x[4] / c0;
    }
    else
    {
        c1 = x[1];
        c2 = x[2];
        c3 = x[3];
        c4 = x[4];
    }

    /* The first step is to take the original equation:

         x^4 + b*x^3 + c*x^2 + d*x + e = 0

       and rewrite it as:

         x^4 + b*x^3 = -c*x^2 - d*x - e,

       adding (b*x/2)^2 + (x^2 + b*x/2)y + y^2/4 to each side gives a
       perfect square on the lhs:

         (x^2 + b*x/2 + y/2)^2 = (b^2/4 - c + y)x^2 + (b*y/2 - d)x + y^2/4 - e

       By choosing the appropriate value for y, the rhs can be made a perfect
       square also.  This value is found when the rhs is treated as a quadratic
       in x with the discriminant equal to 0.  This will be true when:

         (b*y/2 - d)^2 - 4.0 * (b^2/4 - c*y)*(y^2/4 - e) = 0, or

         y^3 - c*y^2 + (b*d - 4*e)*y - b^2*e + 4*c*e - d^2 = 0.

       This is called the resolvent of the quartic equation.  */

    a0 = 4.0 * c4;

    cubic[0] = 1.0;
    cubic[1] = -1.0 * c2;
    cubic[2] = c1 * c3 - a0;
    cubic[3] = a0 * c2 - c1 * c1 * c4 - c3 * c3;

    i = solve_cubic(&cubic[0], &roots[0]);

    if (i > 0)
    {
        y = roots[0];
    }
    else
    {
        return(0);
    }

    /* What we are left with is a quadratic squared on the lhs and a
       linear term on the right.  The linear term has one of two signs,
       take each and add it to the lhs.  The form of the quartic is now:

         a' = b^2/4 - c + y,    b' = b*y/2 - d, (from rhs quadritic above)

         (x^2 + b*x/2 + y/2) = +sqrt(a'*(x + 1/2 * b'/a')^2), and
         (x^2 + b*x/2 + y/2) = -sqrt(a'*(x + 1/2 * b'/a')^2).

       By taking the linear term from each of the right hand sides and
       adding to the appropriate part of the left hand side, two quadratic
       formulas are created.  By solving each of these the four roots of
       the quartic are determined.
    */

    i = 0;

    a0 = c1 / 2.0;
    a1 = y / 2.0;

    t1 = a0 * a0 - c2 + y;

    if (t1 < 0.0)
    {
        if (t1 > FUDGE_FACTOR2)
        {
            t1 = 0.0;
        }
        else
        {
            /* First Special case, a' < 0 means all roots are complex. */

            return(0);
         }
     }

    if (t1 < FUDGE_FACTOR3)
    {
        /* Second special case, the "x" term on the right hand side above
           has vanished.  In this case:

             (x^2 + b*x/2 + y/2) = +sqrt(y^2/4 - e), and
             (x^2 + b*x/2 + y/2) = -sqrt(y^2/4 - e).  */

        t2 = a1 * a1 - c4;

        if (t2 < 0.0)
        {
            return(0);
        }

        x1 = 0.0;
        d1 = sqrt(t2);
    }
    else
    {
        x1 = sqrt(t1);
        d1 = 0.5 * (a0 * y - c3) / x1;
    }

    /* Solve the first quadratic */

    q1 = -a0 - x1;
    q2 = a1 + d1;
    d2 = q1 * q1 - 4.0 * q2;

    if (d2 >= 0.0)
    {
        d2 = sqrt(d2);

        results[0] = 0.5 * (q1 + d2);
        results[1] = 0.5 * (q1 - d2);

        i = 2;
    }

    /* Solve the second quadratic */

    q1 = q1 + x1 + x1;
    q2 = a1 - d1;
    d2 = q1 * q1 - 4.0 * q2;

    if (d2 >= 0.0)
    {
        d2 = sqrt(d2);

        results[i++] = 0.5 * (q1 + d2);
        results[i++] = 0.5 * (q1 - d2);
    }

    return(i);
}
#else
/*****************************************************************************
*
* FUNCTION
*
*   solve_quartic
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Solve a quartic using the method of Francois Vieta (Circa 1735).
*
* CHANGES
*
*   -
*
******************************************************************************/

static int solve_quartic(const DBL *x, DBL *results)
{
    DBL cubic[4];
    DBL roots[3];
    DBL c12, z, p, q, q1, q2, r, d1, d2;
    DBL c0, c1, c2, c3, c4;
    int i;

    /* Make sure the quartic has a leading coefficient of 1.0 */

    c0 = x[0];

    if (c0 != 1.0)
    {
        c1 = x[1] / c0;
        c2 = x[2] / c0;
        c3 = x[3] / c0;
        c4 = x[4] / c0;
    }
    else
    {
        c1 = x[1];
        c2 = x[2];
        c3 = x[3];
        c4 = x[4];
    }

    /* Compute the cubic resolvant */

    c12 = c1 * c1;
    p = -0.375 * c12 + c2;
    q = 0.125 * c12 * c1 - 0.5 * c1 * c2 + c3;
    r = -0.01171875 * c12 * c12 + 0.0625 * c12 * c2 - 0.25 * c1 * c3 + c4;

    cubic[0] = 1.0;
    cubic[1] = -0.5 * p;
    cubic[2] = -r;
    cubic[3] = 0.5 * r * p - 0.125 * q * q;

    i = solve_cubic(cubic, roots);

    if (i > 0)
    {
        z = roots[0];
    }
    else
    {
        return(0);
    }

    d1 = 2.0 * z - p;

    if (d1 < 0.0)
    {
        if (d1 > -SMALL_ENOUGH)
        {
            d1 = 0.0;
        }
        else
        {
            return(0);
        }
    }

    if (d1 < SMALL_ENOUGH)
    {
        d2 = z * z - r;

        if (d2 < 0.0)
        {
            return(0);
        }

        d2 = sqrt(d2);
    }
    else
    {
        d1 = sqrt(d1);
        d2 = 0.5 * q / d1;
    }

    /* Set up useful values for the quadratic factors */

    q1 = d1 * d1;
    q2 = -0.25 * c1;

    i = 0;

    /* Solve the first quadratic */

    p = q1 - 4.0 * (z - d2);

    if (p == 0)
    {
        results[i++] = -0.5 * d1 - q2;
    }
    else
    {
        if (p > 0)
        {
            p = sqrt(p);
            results[i++] = -0.5 * (d1 + p) + q2;
            results[i++] = -0.5 * (d1 - p) + q2;
        }
    }

    /* Solve the second quadratic */

    p = q1 - 4.0 * (z + d2);

    if (p == 0)
    {
        results[i++] = 0.5 * d1 - q2;
    }
    else
    {
        if (p > 0)
        {
            p = sqrt(p);
            results[i++] = 0.5 * (d1 + p) + q2;
            results[i++] = 0.5 * (d1 - p) + q2;
        }
    }

    return(i);
}
#endif



/*****************************************************************************
*
* FUNCTION
*
*   polysolve
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
*   Alexander Enzmann
*
* DESCRIPTION
*
*   Root solver based on the Sturm sequences for a polynomial.
*
* CHANGES
*
*   Okt 1996 : Added bug fix by Heiko Eissfeldt. [DB]
*
******************************************************************************/

static int polysolve(int order, const DBL *Coeffs, DBL *roots)
{
    polynomial sseq[MAX_ORDER+1];
    DBL min_value, max_value;
    int i, nroots, np, atmin, atmax;

    /* Put the coefficients into the top of the stack. */

    for (i = 0; i <= order; i++)
    {
        sseq[0].coef[order-i] = Coeffs[i] / Coeffs[0];
    }

    /* Build the Sturm sequence */

    np = buildsturm(order, &sseq[0]);

    /* Get the total number of visible roots */

    if ((nroots = visible_roots(np, sseq, &atmin, &atmax)) == 0)
    {
        return(0);
    }

    /* Bracket the roots */

    min_value = 0.0;
    max_value = MAX_DISTANCE;

    atmin = numchanges(np, sseq, min_value);
    atmax = numchanges(np, sseq, max_value);

    nroots = atmin - atmax;

    if (nroots == 0)
    {
        return(0);
    }

    /* perform the bisection. */

    return(sbisect(np, sseq, min_value, max_value, atmin, atmax, roots));
}



/*****************************************************************************
*
* FUNCTION
*
*   Solve_Polynomial
*
* INPUT
*
*   n       - order of polynomial
*   c       - coefficients
*   r       - roots
*   sturm   - true, if sturm should be used for n=3,4
*   epsilon - Tolerance to discard small root
*
* OUTPUT
*
*   r
*
* RETURNS
*
*   int - number of roots found
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Solve the polynomial equation
*
*     c[0] * x ^ n + c[1] * x ^ (n-1) + ... + c[n-1] * x + c[n] = 0
*
*   If the equation has a root r, |r| < epsilon, this root is eliminated
*   and the equation of order n-1 will be solved. This will avoid the problem
*   of "surface acne" in (most) cases while increasing the speed of the
*   root solving (polynomial's order is reduced by one).
*
*   WARNING: This function can only be used for polynomials if small roots
*   (i.e. |x| < epsilon) are not needed. This is the case for ray/object
*   intersection tests because only intersecions with t > 0 are valid.
*
*   NOTE: Only one root at x = 0 will be eliminated.
*
*   NOTE: If epsilon = 0 no roots will be eliminated.
*
*
*   The method and idea for root elimination was taken from:
*
*     Han-Wen Nienhuys, "Polynomials", Ray Tracing News, July 6, 1994,
*     Volume 7, Number 3
*
*
* CHANGES
*
*   Jul 1994 : Creation.
*
******************************************************************************/

int Solve_Polynomial(int n, const DBL *c0, DBL *r, int sturm, DBL epsilon, RenderStatistics& stats)
{
    int roots, i;
    const DBL *c;

    stats[Polynomials_Tested]++;

    roots = 0;

    /*
     * Determine the "real" order of the polynomial, i.e.
     * eliminate small leading coefficients.
     */

    i = 0;

    while ((fabs(c0[i]) < SMALL_ENOUGH) && (i < n))
    {
        i++;
    }

    n -= i;

    c = &c0[i];

    switch (n)
    {
        case 0:

            break;

        case 1:

            /* Solve linear polynomial. */

            if (c[0] != 0.0)
            {
                r[roots++] = -c[1] / c[0];
            }

            break;

        case 2:

            /* Solve quadratic polynomial. */

            roots = solve_quadratic(c, r);

            break;

        case 3:

            /* Root elimination? */

            if (epsilon > 0.0)
            {
                if ((c[2] != 0.0) && (fabs(c[3]/c[2]) < epsilon))
                {
                    stats[Roots_Eliminated]++;

                    roots = solve_quadratic(c, r);

                    break;
                }
            }

            /* Solve cubic polynomial. */

            if (sturm)
            {
                roots = polysolve(3, c, r);
            }
            else
            {
                roots = solve_cubic(c, r);
            }

            break;

        case 4:

            /* Root elimination? */

            if (epsilon > 0.0)
            {
                if ((c[3] != 0.0) && (fabs(c[4]/c[3]) < epsilon))
                {
                    stats[Roots_Eliminated]++;

                    if (sturm)
                    {
                        roots = polysolve(3, c, r);
                    }
                    else
                    {
                        roots = solve_cubic(c, r);
                    }

                    break;
                }
            }

            /* Test for difficult coeffs. */

            if (difficult_coeffs(4, c))
            {
                sturm = true;
            }

            /* Solve quartic polynomial. */

            if (sturm)
            {
                roots = polysolve(4, c, r);
            }
            else
            {
                roots = solve_quartic(c, r);
            }

            break;

        default:

            if (epsilon > 0.0)
            {
                if ((c[n-1] != 0.0) && (fabs(c[n]/c[n-1]) < epsilon))
                {
                    stats[Roots_Eliminated]++;

                    roots = polysolve(n-1, c, r);
                }
            }

            /* Solve n-th order polynomial. */

            roots = polysolve(n, c, r);

            break;
    }

    return(roots);
}

}
