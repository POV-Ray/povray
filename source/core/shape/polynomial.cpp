//******************************************************************************
///
/// @file core/shape/polynomial.cpp
///
/// Implementation of the 3-variable polynomial geometric primitive.
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
#include "core/shape/polynomial.h"

#include "core/bounding/boundingbox.h"
#include "core/math/matrix.h"
#include "core/math/polynomialsolver.h"
#include "core/render/ray.h"
#include "core/scene/tracethreaddata.h"

// this must be the last file included
#include "base/povdebug.h"

/*
 * Basic form of a quartic equation:
 *
 *  a00*x^4    + a01*x^3*y   + a02*x^3*z   + a03*x^3     + a04*x^2*y^2 +
 *  a05*x^2*y*z+ a06*x^2*y   + a07*x^2*z^2 + a08*x^2*z   + a09*x^2     +
 *  a10*x*y^3  + a11*x*y^2*z + a12*x*y^2   + a13*x*y*z^2 + a14*x*y*z   +
 *  a15*x*y    + a16*x*z^3   + a17*x*z^2   + a18*x*z     + a19*x       + a20*y^4   +
 *  a21*y^3*z  + a22*y^3     + a23*y^2*z^2 + a24*y^2*z   + a25*y^2     + a26*y*z^3 +
 *  a27*y*z^2  + a28*y*z     + a29*y       + a30*z^4     + a31*z^3     + a32*z^2   + a33*z + a34
 *
 */

namespace pov
{

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/

const DBL DEPTH_TOLERANCE = 1.0e-4;
const DBL INSIDE_TOLERANCE = 1.0e-4;
const DBL ROOT_TOLERANCE = 1.0e-4;
const DBL COEFF_LIMIT = 1.0e-20;
// const int BINOMSIZE = 40;



/*****************************************************************************
* Local variables
******************************************************************************/

/* The following table contains the binomial coefficients up to 35 */
#if (MAX_ORDER > 35)
#error "Poly.cpp code would break loose due to a too short pascal triangle table."
#endif
/* this is a pascal's triangle : [k][j]=[k-1][j-1]+[k-1][j] ;
                                   [k][0]=1, [k][k]=1
 */
/* Max value in [35] is 0x8B18014C, so unsigned int is enough
 * If you want to go down to from [36] to [69],
 *  a 64 bits unsigned long must be used
 */
const unsigned int binomials[35][35] =
{

  {          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,          2u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,          3u,          3u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,          4u,          6u,          4u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,          5u,         10u,         10u,          5u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,          6u,         15u,         20u,         15u,          6u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,          7u,         21u,         35u,         35u,         21u,          7u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,          8u,         28u,         56u,         70u,         56u,         28u,          8u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,          9u,         36u,         84u,        126u,        126u,         84u,         36u,          9u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,         10u,         45u,        120u,        210u,        252u,        210u,        120u,         45u,         10u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,         11u,         55u,        165u,        330u,        462u,        462u,        330u,        165u,         55u,         11u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,         12u,         66u,        220u,        495u,        792u,        924u,        792u,        495u,        220u,         66u,         12u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,         13u,         78u,        286u,        715u,       1287u,       1716u,       1716u,       1287u,        715u,        286u,         78u,         13u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,         14u,         91u,        364u,       1001u,       2002u,       3003u,       3432u,       3003u,       2002u,       1001u,        364u,         91u,         14u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,         15u,        105u,        455u,       1365u,       3003u,       5005u,       6435u,       6435u,       5005u,       3003u,       1365u,        455u,        105u,         15u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,         16u,        120u,        560u,       1820u,       4368u,       8008u,      11440u,      12870u,      11440u,       8008u,       4368u,       1820u,        560u,        120u,         16u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,         17u,        136u,        680u,       2380u,       6188u,      12376u,      19448u,      24310u,      24310u,      19448u,      12376u,       6188u,       2380u,        680u,        136u,         17u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,         18u,        153u,        816u,       3060u,       8568u,      18564u,      31824u,      43758u,      48620u,      43758u,      31824u,      18564u,       8568u,       3060u,        816u,        153u,         18u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,         19u,        171u,        969u,       3876u,      11628u,      27132u,      50388u,      75582u,      92378u,      92378u,      75582u,      50388u,      27132u,      11628u,       3876u,        969u,        171u,         19u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,         20u,        190u,       1140u,       4845u,      15504u,      38760u,      77520u,     125970u,     167960u,     184756u,     167960u,     125970u,      77520u,      38760u,      15504u,       4845u,       1140u,        190u,         20u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,         21u,        210u,       1330u,       5985u,      20349u,      54264u,     116280u,     203490u,     293930u,     352716u,     352716u,     293930u,     203490u,     116280u,      54264u,      20349u,       5985u,       1330u,        210u,         21u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,         22u,        231u,       1540u,       7315u,      26334u,      74613u,     170544u,     319770u,     497420u,     646646u,     705432u,     646646u,     497420u,     319770u,     170544u,      74613u,      26334u,       7315u,       1540u,        231u,         22u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,         23u,        253u,       1771u,       8855u,      33649u,     100947u,     245157u,     490314u,     817190u,    1144066u,    1352078u,    1352078u,    1144066u,     817190u,     490314u,     245157u,     100947u,      33649u,       8855u,       1771u,        253u,         23u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,         24u,        276u,       2024u,      10626u,      42504u,     134596u,     346104u,     735471u,    1307504u,    1961256u,    2496144u,    2704156u,    2496144u,    1961256u,    1307504u,     735471u,     346104u,     134596u,      42504u,      10626u,       2024u,        276u,         24u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,         25u,        300u,       2300u,      12650u,      53130u,     177100u,     480700u,    1081575u,    2042975u,    3268760u,    4457400u,    5200300u,    5200300u,    4457400u,    3268760u,    2042975u,    1081575u,     480700u,     177100u,      53130u,      12650u,       2300u,        300u,         25u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,         26u,        325u,       2600u,      14950u,      65780u,     230230u,     657800u,    1562275u,    3124550u,    5311735u,    7726160u,    9657700u,   10400600u,    9657700u,    7726160u,    5311735u,    3124550u,    1562275u,     657800u,     230230u,      65780u,      14950u,       2600u,        325u,         26u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,         27u,        351u,       2925u,      17550u,      80730u,     296010u,     888030u,    2220075u,    4686825u,    8436285u,   13037895u,   17383860u,   20058300u,   20058300u,   17383860u,   13037895u,    8436285u,    4686825u,    2220075u,     888030u,     296010u,      80730u,      17550u,       2925u,        351u,         27u,          1u,          0u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,         28u,        378u,       3276u,      20475u,      98280u,     376740u,    1184040u,    3108105u,    6906900u,   13123110u,   21474180u,   30421755u,   37442160u,   40116600u,   37442160u,   30421755u,   21474180u,   13123110u,    6906900u,    3108105u,    1184040u,     376740u,      98280u,      20475u,       3276u,        378u,         28u,          1u,          0u,          0u,          0u,          0u,          0u,          0u},
  {          1u,         29u,        406u,       3654u,      23751u,     118755u,     475020u,    1560780u,    4292145u,   10015005u,   20030010u,   34597290u,   51895935u,   67863915u,   77558760u,   77558760u,   67863915u,   51895935u,   34597290u,   20030010u,   10015005u,    4292145u,    1560780u,     475020u,     118755u,      23751u,       3654u,        406u,         29u,          1u,          0u,          0u,          0u,          0u,          0u},
  {          1u,         30u,        435u,       4060u,      27405u,     142506u,     593775u,    2035800u,    5852925u,   14307150u,   30045015u,   54627300u,   86493225u,  119759850u,  145422675u,  155117520u,  145422675u,  119759850u,   86493225u,   54627300u,   30045015u,   14307150u,    5852925u,    2035800u,     593775u,     142506u,      27405u,       4060u,        435u,         30u,          1u,          0u,          0u,          0u,          0u},
  {          1u,         31u,        465u,       4495u,      31465u,     169911u,     736281u,    2629575u,    7888725u,   20160075u,   44352165u,   84672315u,  141120525u,  206253075u,  265182525u,  300540195u,  300540195u,  265182525u,  206253075u,  141120525u,   84672315u,   44352165u,   20160075u,    7888725u,    2629575u,     736281u,     169911u,      31465u,       4495u,        465u,         31u,          1u,          0u,          0u,          0u},
  {          1u,         32u,        496u,       4960u,      35960u,     201376u,     906192u,    3365856u,   10518300u,   28048800u,   64512240u,  129024480u,  225792840u,  347373600u,  471435600u,  565722720u,  601080390u,  565722720u,  471435600u,  347373600u,  225792840u,  129024480u,   64512240u,   28048800u,   10518300u,    3365856u,     906192u,     201376u,      35960u,       4960u,        496u,         32u,          1u,          0u,          0u},
  {          1u,         33u,        528u,       5456u,      40920u,     237336u,    1107568u,    4272048u,   13884156u,   38567100u,   92561040u,  193536720u,  354817320u,  573166440u,  818809200u, 1037158320u, 1166803110u, 1166803110u, 1037158320u,  818809200u,  573166440u,  354817320u,  193536720u,   92561040u,   38567100u,   13884156u,    4272048u,    1107568u,     237336u,      40920u,       5456u,        528u,         33u,          1u,          0u},
  {          1u,         34u,        561u,       5984u,      46376u,     278256u,    1344904u,    5379616u,   18156204u,   52451256u,  131128140u,  286097760u,  548354040u,  927983760u, 1391975640u, 1855967520u, 2203961430u, 2333606220u, 2203961430u, 1855967520u, 1391975640u,  927983760u,  548354040u,  286097760u,  131128140u,   52451256u,   18156204u,    5379616u,    1344904u,     278256u,      46376u,       5984u,        561u,         34u,          1u}

};

/*
*/
bool Poly::Set_Coeff(const unsigned int x, const unsigned int y, const unsigned int z, const DBL value)
{
    int a,b,c;
    unsigned int pos;
    a=Order-x;
    b=Order-x-y;
    c=Order-x-y-z;
    /* a bit overprotective */
    if ((x+y+z>Order)||(a<0)||(b<0)||(c<0))
    {
        return false;
    }
    /* pos = binomials[a+2][3]+binomials[b+1][2]+binomials[c][1];
     * rewriten to stay in bound (max is "Order", not Order+2)
     */
    pos =
        // binomials[a+2][3]
        //       binomials[a+1][3]
        binomials[a][3]+binomials[a][2]
        //      +binomials[a+1][2]
        +binomials[a][2]+binomials[a][1]
        // +binomials[b+1][2]
        +binomials[b][1]+binomials[b][2]
        +binomials[c][1];
    /* It's magic
     * Nah... a is the tetraedric sum to jump to get to the power of x index (first entry)
     * b is then the triangular sum to add to get to the power of y index (also first entry)
     * and c is the linear sum to add to get to the power of z index (that the one we want)
   *
     * Notice that binomials[c][1] == c, but the formula would loose its magic use of
     * pascal triangle everywhere.
     * triangular sum are in the third ([2] column)
     * tetraedric sum are in the fourth ([3] column)
     *
     * (and yes, the 0 at the start of each column is useful)
     */
    Coeffs[pos] = value;
    return true;
}


/*****************************************************************************
*
* FUNCTION
*
*   All_Poly_Intersections
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool Poly::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
    DBL Depths[MAX_ORDER];
    DBL len;
    Vector3d IPoint;
    int cnt, i, j, Intersection_Found, same_root;
    BasicRay New_Ray;

    /* Transform the ray into the polynomial's space */

    MInvTransRay(New_Ray, ray, Trans);

    len = New_Ray.Direction.length();
    New_Ray.Direction /= len;

    Intersection_Found = false;

    Thread->Stats()[Ray_Poly_Tests]++;

    switch (Order)
    {
        case 1:

            cnt = intersect_linear(New_Ray, Coeffs, Depths);

            break;

        case 2:

            cnt = intersect_quadratic(New_Ray, Coeffs, Depths);

            break;

        default:

            cnt = intersect(New_Ray, Order, Coeffs, Test_Flag(this, STURM_FLAG), Depths, Thread);
    }

    if (cnt > 0)
    {
        Thread->Stats()[Ray_Poly_Tests_Succeeded]++;
    }

    for (i = 0; i < cnt; i++)
    {
        if (Depths[i] > DEPTH_TOLERANCE)
        {
            same_root = false;

            for (j = 0; j < i; j++)
            {
                if (Depths[i] == Depths[j])
                {
                    same_root = true;

                    break;
                }
            }

            if (!same_root)
            {
                IPoint = New_Ray.Evaluate(Depths[i]);

                /* Transform the point into world space */

                MTransPoint(IPoint, IPoint, Trans);

                if (Clip.empty() || Point_In_Clip(IPoint, Clip, Thread))
                {
                    Depth_Stack->push(Intersection(Depths[i] / len,IPoint,this));

                    Intersection_Found = true;
                }
            }
        }
    }

    return (Intersection_Found);
}



/*****************************************************************************
*
* FUNCTION
*
*   evaluate_linear
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

/* For speedup of low order polynomials, expand out the terms
   involved in evaluating the poly. */
/* unused
DBL evaluate_linear(const Vector3d& P, DBL *a)
{
    return(a[0] * P[X]) + (a[1] * P[Y]) + (a[2] * P[Z]) + a[3];
}
*/



/*****************************************************************************
*
* FUNCTION
*
*   evaluate_quadratic
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

/*
DBL evaluate_quadratic(const Vector3d& P, DBL *a)
{
    DBL x, y, z;

    x = P[X];
    y = P[Y];
    z = P[Z];

    return(a[0] * x * x + a[1] * x * y + a[2] * x * z +
           a[3] * x     + a[4] * y * y + a[5] * y * z +
           a[6] * y     + a[7] * z * z + a[8] * z     + a[9]);
}
*/



/*****************************************************************************
*
* FUNCTION
*
*   factor_out
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
*   Remove all factors of i from n.
*
* CHANGES
*
*   -
*
******************************************************************************/
/*
int Poly::factor_out(int n, int i, int *c, int *s)
{
    while (!(n % i))
    {
        n /= i;

        s[(*c)++] = i;
    }

    return(n);
}
*/


/*****************************************************************************
*
* FUNCTION
*
*   factor1
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
*   Find all prime factors of n. (Note that n must be less than 2^15.
*
* CHANGES
*
*   -
*
******************************************************************************/
#if 0
void Poly::factor1(int n, int *c, int *s)
{
    int i,k;

    /* First factor out any 2s. */

    n = factor_out(n, 2, c, s);

    /* Now any odd factors. */

    k = (int)sqrt((DBL)n) + 1;

    for (i = 3; (n > 1) && (i <= k); i += 2)
    {
        if (!(n % i))
        {
            n = factor_out(n, i, c, s);
            k = (int)sqrt((DBL)n)+1;
        }
    }

    if (n > 1)
    {
        s[(*c)++] = n;
    }
}
#endif


/*****************************************************************************
*
* FUNCTION
*
*   binomial
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
*   Calculate the binomial coefficent of n, r.
*
* CHANGES
*
*   -
*
******************************************************************************/
#if 0
int Poly::binomial(int n, int  r)
{
    int h,i,j,k,l;
    unsigned int result;
    int stack1[BINOMSIZE], stack2[BINOMSIZE];

    if ((n < 0) || (r < 0) || (r > n))
    {
        result = 0L;
    }
    else
    {
        if (r == n)
        {
            result = 1L;
        }
        else
        {
            if ((r < 16) && (n < 16))
            {
                result = (int)binomials[n][r];
            }
            else
            {
                j = 0;

                for (i = r + 1; i <= n; i++)
                {
                    stack1[j++] = i;
                }

                for (i = 2; i <= (n-r); i++)
                {
                    h = 0;

                    factor1(i, &h, stack2);

                    for (k = 0; k < h; k++)
                    {
                        for (l = 0; l < j; l++)
                        {
                            if (!(stack1[l] % stack2[k]))
                            {
                                stack1[l] /= stack2[k];

                                goto l1;
                            }
                        }
                    }

                    // Error if we get here
//                  Debug_Info("Failed to factor\n");
l1:;
                }

                result = 1;

                for (i = 0; i < j; i++)
                {
                    result *= stack1[i];
                }
            }
        }
    }

    return(result);
}
#endif


/*****************************************************************************
*
* FUNCTION
*
*   inside
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

DBL Poly::inside(const Vector3d& IPoint, int Order, const DBL *Coeffs)
{
    DBL x[MAX_ORDER+1], y[MAX_ORDER+1], z[MAX_ORDER+1];
    DBL c, Result;
    int i, j, k, term;

    x[0] = 1.0;       y[0] = 1.0;       z[0] = 1.0;
    x[1] = IPoint[X]; y[1] = IPoint[Y]; z[1] = IPoint[Z];

    for (i = 2; i <= Order; i++)
    {
        x[i] = x[1] * x[i-1];
        y[i] = y[1] * y[i-1];
        z[i] = z[1] * z[i-1];
    }

    Result = 0.0;

    term = 0;

    for (i = Order; i >= 0; i--)
    {
        for (j=Order-i;j>=0;j--)
        {
            for (k=Order-(i+j);k>=0;k--)
            {
                if ((c = Coeffs[term]) != 0.0)
                {
                    Result += c * x[i] * y[j] * z[k];
                }
                term++;
            }
        }
    }

    return(Result);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect
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
*   Intersection of a ray and an arbitrary polynomial function.
*
* CHANGES
*
*   -
*
******************************************************************************/

int Poly::intersect(const BasicRay &ray, int Order, const DBL *Coeffs, int Sturm_Flag, DBL *Depths, TraceThreadData *Thread)
{
    DBL eqn_v[3][MAX_ORDER+1], eqn_vt[3][MAX_ORDER+1];
    DBL eqn[MAX_ORDER+1];
    DBL t[3][MAX_ORDER+1];
    Vector3d P, D;
    DBL val;
    int h, i, j, k, i1, j1, k1, term;
    int offset;

    /* First we calculate the values of the individual powers
       of x, y, and z as they are represented by the ray */

    P = ray.Origin;
    D = ray.Direction;

    for (i = 0; i < 3; i++)
    {
        eqn_v[i][0]  = 1.0;
        eqn_vt[i][0] = 1.0;
    }

    eqn_v[0][1] = P[X];
    eqn_v[1][1] = P[Y];
    eqn_v[2][1] = P[Z];

    eqn_vt[0][1] = D[X];
    eqn_vt[1][1] = D[Y];
    eqn_vt[2][1] = D[Z];

    for (i = 2; i <= Order; i++)
    {
        for (j=0;j<3;j++)
        {
            eqn_v[j][i]  = eqn_v[j][1] * eqn_v[j][i-1];
            eqn_vt[j][i] = eqn_vt[j][1] * eqn_vt[j][i-1];
        }
    }

    for (i = 0; i <= Order; i++)
    {
        eqn[i] = 0.0;
    }

    /* Now walk through the terms of the polynomial.  As we go
       we substitute the ray equation for each of the variables. */

    term = 0;

    for (i = Order; i >= 0; i--)
    {
        for (h = 0; h <= i; h++)
        {
            t[0][h] = binomials[i][h] * eqn_vt[0][i-h] * eqn_v[0][h];
        }

        for (j = Order-i; j >= 0; j--)
        {
            for (h = 0; h <= j; h++)
            {
                t[1][h] = binomials[j][h] * eqn_vt[1][j-h] * eqn_v[1][h];
            }

            for (k = Order-(i+j); k >= 0; k--)
            {
                if (Coeffs[term] != 0)
                {
                    for (h = 0; h <= k; h++)
                    {
                        t[2][h] = binomials[k][h] * eqn_vt[2][k-h] * eqn_v[2][h];
                    }

                    /* Multiply together the three polynomials. */

                    offset = Order - (i + j + k);

                    for (i1 = 0; i1 <= i; i1++)
                    {
                        for (j1=0;j1<=j;j1++)
                        {
                            for (k1=0;k1<=k;k1++)
                            {
                                h = offset + i1 + j1 + k1;
                                val = Coeffs[term];
                                val *= t[0][i1];
                                val *= t[1][j1];
                                val *= t[2][k1];
                                eqn[h] += val;
                            }
                        }
                    }
                }

                term++;
            }
        }
    }

    for (i = 0, j = Order; i <= Order; i++)
    {
        if (eqn[i] != 0.0)
        {
            break;
        }
        else
        {
            j--;
        }
    }

    if (j > 1)
    {
        return(Solve_Polynomial(j, &eqn[i], Depths, Sturm_Flag, ROOT_TOLERANCE, Thread->Stats()));
    }
    else
    {
        return(0);
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_linear
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
*   Intersection of a ray and a quadratic.
*
* CHANGES
*
*   -
*
******************************************************************************/

int Poly::intersect_linear(const BasicRay &ray, const DBL *Coeffs, DBL *Depths)
{
    DBL t0, t1;
    const DBL *a = Coeffs;

    t0 = a[0] * ray.Origin[X] + a[1] * ray.Origin[Y] + a[2] * ray.Origin[Z];
    t1 = a[0] * ray.Direction[X] + a[1] * ray.Direction[Y] +

    a[2] * ray.Direction[Z];

    if (fabs(t1) < EPSILON)
    {
        return(0);
    }

    Depths[0] = -(a[3] + t0) / t1;

    return(1);
}



/*****************************************************************************
*
* FUNCTION
*
*   intersect_quadratic
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
*   Intersection of a ray and a quadratic.
*
* CHANGES
*
*   -
*
******************************************************************************/

int Poly::intersect_quadratic(const BasicRay &ray, const DBL *Coeffs, DBL *Depths)
{
    DBL x, y, z, x2, y2, z2;
    DBL xx, yy, zz, xx2, yy2, zz2, ac, bc, cc, d, t;
    const DBL *a;

    x  = ray.Origin[X];
    y  = ray.Origin[Y];
    z  = ray.Origin[Z];

    xx = ray.Direction[X];
    yy = ray.Direction[Y];
    zz = ray.Direction[Z];

    x2  = x * x;    y2  = y * y;    z2 = z * z;
    xx2 = xx * xx;  yy2 = yy * yy;  zz2 = zz * zz;

    a = Coeffs;

    /*
     * Determine the coefficients of t^n, where the line is represented
     * as (x,y,z) + (xx,yy,zz)*t.
     */

    ac = (a[0]*xx2 + a[1]*xx*yy + a[2]*xx*zz + a[4]*yy2 + a[5]*yy*zz + a[7]*zz2);

    bc = (2*a[0]*x*xx + a[1]*(x*yy + xx*y) + a[2]*(x*zz + xx*z) +
          a[3]*xx + 2*a[4]*y*yy + a[5]*(y*zz + yy*z) + a[6]*yy +
          2*a[7]*z*zz + a[8]*zz);

    cc = a[0]*x2 + a[1]*x*y + a[2]*x*z + a[3]*x + a[4]*y2 +
         a[5]*y*z + a[6]*y + a[7]*z2 + a[8]*z + a[9];

    if (fabs(ac) < COEFF_LIMIT)
    {
        if (fabs(bc) < COEFF_LIMIT)
        {
            return(0);
        }

        Depths[0] = -cc / bc;

        return(1);
    }

    /*
     * Solve the quadratic formula & return results that are
     * within the correct interval.
     */

    d = bc * bc - 4.0 * ac * cc;

    if (d < 0.0)
    {
        return(0);
    }

    d = sqrt(d);

    bc = -bc;

    t = 2.0 * ac;

    Depths[0] = (bc + d) / t;
    Depths[1] = (bc - d) / t;

    return(2);
}



/*****************************************************************************
*
* FUNCTION
*
*   normal0
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
*   Normal to a polynomial (used for polynomials with order > 4).
*
* CHANGES
*
*   -
*
******************************************************************************/

void Poly::normal0(Vector3d& Result, int Order, const DBL *Coeffs, const Vector3d& IPoint)
{
    int i, j, k, term;
    DBL x[MAX_ORDER+1], y[MAX_ORDER+1], z[MAX_ORDER+1];
    DBL val;
    const DBL *a;

    x[0] = 1.0;
    y[0] = 1.0;
    z[0] = 1.0;

    x[1] = IPoint[X];
    y[1] = IPoint[Y];
    z[1] = IPoint[Z];

    for (i = 2; i <= Order; i++)
    {
        x[i] = IPoint[X] * x[i-1];
        y[i] = IPoint[Y] * y[i-1];
        z[i] = IPoint[Z] * z[i-1];
    }

    a = Coeffs;

    term = 0;

    Result = Vector3d(0.0, 0.0, 0.0);

    for (i = Order; i >= 0; i--)
    {
        for (j = Order-i; j >= 0; j--)
        {
            for (k = Order-(i+j); k >= 0; k--)
            {
                if (i >= 1)
                {
                    val = x[i-1] * y[j] * z[k];
                    Result[X] += i * a[term] * val;
                }

                if (j >= 1)
                {
                    val = x[i] * y[j-1] * z[k];
                    Result[Y] += j * a[term] * val;
                }

                if (k >= 1)
                {
                    val = x[i] * y[j] * z[k-1];
                    Result[Z] += k * a[term] * val;
                }

                term++;
            }
        }
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   nromal1
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
*   Normal to a polynomial (for polynomials of order <= 4).
*
* CHANGES
*
*   -
*
******************************************************************************/

void Poly::normal1(Vector3d& Result, int Order, const DBL *Coeffs, const Vector3d& IPoint)
{
    DBL x, y, z, x2, y2, z2, x3, y3, z3;
    const DBL *a;

    a = Coeffs;

    x = IPoint[X];
    y = IPoint[Y];
    z = IPoint[Z];

    switch (Order)
    {
        case 1:

            /* Linear partial derivatives */

            Result = Vector3d(a[0], a[1], a[2]);

            break;

        case 2:

            /* Quadratic partial derivatives */

            Result[X] = 2*a[0]*x+a[1]*y+a[2]*z+a[3];
            Result[Y] = a[1]*x+2*a[4]*y+a[5]*z+a[6];
            Result[Z] = a[2]*x+a[5]*y+2*a[7]*z+a[8];

            break;

        case 3:

            x2 = x * x;  y2 = y * y;  z2 = z * z;

            /* Cubic partial derivatives */

            Result[X] = 3*a[0]*x2 + 2*x*(a[1]*y + a[2]*z + a[3]) + a[4]*y2 +
                        y*(a[5]*z + a[6]) + a[7]*z2 + a[8]*z + a[9];
            Result[Y] = a[1]*x2 + x*(2*a[4]*y + a[5]*z + a[6]) + 3*a[10]*y2 +
                        2*y*(a[11]*z + a[12]) + a[13]*z2 + a[14]*z + a[15];
            Result[Z] = a[2]*x2 + x*(a[5]*y + 2*a[7]*z + a[8]) + a[11]*y2 +
                        y*(2*a[13]*z + a[14]) + 3*a[16]*z2 + 2*a[17]*z + a[18];

            break;

        case 4:

            /* Quartic partial derivatives */

            x2 = x * x;  y2 = y * y;  z2 = z * z;
            x3 = x * x2; y3 = y * y2; z3 = z * z2;

            Result[X] = 4*a[ 0]*x3+3*x2*(a[ 1]*y+a[ 2]*z+a[ 3])+
                        2*x*(a[ 4]*y2+y*(a[ 5]*z+a[ 6])+a[ 7]*z2+a[ 8]*z+a[ 9])+
                        a[10]*y3+y2*(a[11]*z+a[12])+y*(a[13]*z2+a[14]*z+a[15])+
                        a[16]*z3+a[17]*z2+a[18]*z+a[19];
            Result[Y] = a[ 1]*x3+x2*(2*a[ 4]*y+a[ 5]*z+a[ 6])+
                        x*(3*a[10]*y2+2*y*(a[11]*z+a[12])+a[13]*z2+a[14]*z+a[15])+
                        4*a[20]*y3+3*y2*(a[21]*z+a[22])+2*y*(a[23]*z2+a[24]*z+a[25])+
                        a[26]*z3+a[27]*z2+a[28]*z+a[29];
            Result[Z] = a[ 2]*x3+x2*(a[ 5]*y+2*a[ 7]*z+a[ 8])+
                        x*(a[11]*y2+y*(2*a[13]*z+a[14])+3*a[16]*z2+2*a[17]*z+a[18])+
                        a[21]*y3+y2*(2*a[23]*z+a[24])+y*(3*a[26]*z2+2*a[27]*z+a[28])+
                        4*a[30]*z3+3*a[31]*z2+2*a[32]*z+a[33];
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Poly
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool Poly::Inside(const Vector3d& IPoint, TraceThreadData *Thread) const
{
    Vector3d New_Point;
    DBL Result;

    /* Transform the point into polynomial's space */

    MInvTransPoint(New_Point, IPoint, Trans);

    Result = inside(New_Point, Order, Coeffs);

    if (Result < INSIDE_TOLERANCE)
    {
        return ((int)(!Test_Flag(this, INVERTED_FLAG)));
    }
    else
    {
        return ((int)Test_Flag(this, INVERTED_FLAG));
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Poly_Normal
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
*   Normal to a polynomial.
*
* CHANGES
*
*   -
*
******************************************************************************/

void Poly::Normal(Vector3d& Result, Intersection *Inter, TraceThreadData *Thread) const
{
    DBL val;
    Vector3d New_Point;

    /* Transform the point into the polynomials space. */

    MInvTransPoint(New_Point, Inter->IPoint, Trans);

    if (Order > 4)
    {
        normal0(Result, Order, Coeffs, New_Point);
    }
    else
    {
        normal1(Result, Order, Coeffs, New_Point);
    }

    /* Transform back to world space. */

    MTransNormal(Result, Result, Trans);

    /* Normalize (accounting for the possibility of a 0 length normal). */

    val = Result.lengthSqr();

    if (val > 0.0)
    {
        val = 1.0 / sqrt(val);

        Result *= val;
    }
    else
    {
        Result = Vector3d(1.0, 0.0, 0.0);
    }
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Poly
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Poly::Translate(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Poly
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Poly::Rotate(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Poly
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Poly::Scale(const Vector3d&, const TRANSFORM *tr)
{
    Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Poly
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void Poly::Transform(const TRANSFORM *tr)
{
    Compose_Transforms(Trans, tr);

    Compute_BBox();
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Poly
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

Poly::Poly(int order) : ObjectBase(POLY_OBJECT)
{
    Order = order;

    Trans = Create_Transform();

    Coeffs = reinterpret_cast<DBL *>(POV_MALLOC(term_counts(Order) * sizeof(DBL), "coefficients for POLY"));

    for (int i = 0; i < term_counts(Order); i++)
        Coeffs[i] = 0.0;
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Poly
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
*   Make a copy of a polynomial object.
*
* CHANGES
*
*   -
*
******************************************************************************/

ObjectPtr Poly::Copy()
{
    Poly *New = new Poly(Order);
    DBL *tmpCoeffs = New->Coeffs;
    int i;

    Destroy_Transform(New->Trans);
    *New = *this;
    New->Coeffs = tmpCoeffs;
    New->Trans = Copy_Transform(Trans);

    for(i = 0; i < term_counts(New->Order); i++)
        New->Coeffs[i] = Coeffs[i];

    return New;
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Poly
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
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

Poly::~Poly()
{
    POV_FREE(Coeffs);
}



/*****************************************************************************
*
* FUNCTION
*
*   Compute_Poly_BBox
*
* INPUT
*
*   Poly - Poly
*
* OUTPUT
*
*   Poly
*
* RETURNS
*
* AUTHOR
*
*   Dieter Bayer
*
* DESCRIPTION
*
*   Calculate the bounding box of a poly.
*
* CHANGES
*
*   Aug 1994 : Creation.
*
******************************************************************************/

void Poly::Compute_BBox()
{
    Make_BBox(BBox, -BOUND_HUGE/2, -BOUND_HUGE/2, -BOUND_HUGE/2, BOUND_HUGE, BOUND_HUGE, BOUND_HUGE);

    if(!Clip.empty())
        BBox = Clip[0]->BBox; // FIXME - does not seem to support more than one bounding object? [trf]
}

bool Poly::Intersect_BBox(BBoxDirection, const BBoxVector3d&, const BBoxVector3d&, BBoxScalar) const
{
    return true;
}

}
