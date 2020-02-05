#ifdef(ShapesQ_Inc_Temp)
// do nothing
#else
#declare ShapesQ_Inc_Temp = version;

#ifdef(View_POV_Include_Stack)
#   debug "including shapesq.inc\n"
#end

/*
              Persistence of Vision Raytracer Version 3.1
                      Quartic shapes include file
              Several cubic and quartic shape definitions
                          by Alexander Enzmann

 In the following descriptions, multiplication of two terms is
 shown as the two terms next to each other (i.e. x y, rather than
 x*y.  The expression c(n, m) is the binomial coefficient, n!/m!(n-m)!.

*/

/* Bicorn
  This curve looks like the top part of a paraboloid, bounded
  from below by another paraboloid.  The basic equation is:
     y^2 - (x^2 + z^2) y^2 - (x^2 + z^2 + 2 y - 1)^2 = 0.  */
#declare Bicorn =
 quartic
  {< 1,   0,   0,   0,  1,   0,   4,   2,   0, -2,
     0,   0,   0,   0,  0,   0,   0,   0,   0,  0,
     0,   0,   0,   1,  0,   3,   0,   4,   0, -4,
     1,   0,  -2,   0,  1>
  }

/* Crossed Trough
  This is a surface with four pieces that sweep up from the x-z plane.
  The equation is: y = x^2 z^2.  */
#declare Crossed_Trough =
 quartic
  {< 0,   0,   0,   0,  0,   0,   0,   4,   0,  0,
     0,   0,   0,   0,  0,   0,   0,   0,   0,  0,
     0,   0,   0,   0,  0,   0,   0,   0,   0, -1,
     0,   0,   0,   0,  0>
  }

/* a drop coming out of water? This is a curve formed by using the equation
  y = 1/2 x^2 (x + 1) as the radius of a cylinder having the x-axis as
  its central axis. The final form of the equation is:
     y^2 + z^2 = 0.5 (x^3 + x^2) */
#declare Cubic_Cylinder =
 quartic 
  {< 0,   0,   0,   -0.5, 0,   0,   0,   0,   0, -0.5,
     0,   0,   0,    0,   0,   0,   0,   0,   0,  0,
     0,   0,   0,    0,   0,   1,   0,   0,   0,  0,
     0,   0,   1,    0,   0>
  }

/* a cubic saddle. The equation is: z = x^3 - y^3. */
#declare Cubic_Saddle_1 =
 quartic 
  {< 0,   0,   0,    1,   0,   0,   0,   0,   0,  0,
     0,   0,   0,    0,   0,   0,   0,   0,   0,  0,
     0,   0,  -1,    0,   0,   0,   0,   0,   0,  0,
     0,   0,   0,   -1,   0>
  }

/* Variant of a devil's curve in 3-space.  This figure has a top and
  bottom part that are very similar to a hyperboloid of one sheet,
  however the central region is pinched in the middle leaving two
  teardrop shaped holes. The equation is:
     x^4 + 2 x^2 z^2 - 0.36 x^2 - y^4 + 0.25 y^2 + z^4 = 0.  */
#declare Devils_Curve =
 quartic 
  {<-1,   0,   0,    0,  0,   0,    0,  -2,   0,  0.36,
     0,   0,   0,    0,  0,   0,    0,   0,   0,  0,
     1,   0,   0,    0,  0,  -0.25, 0,   0,   0,  0,
    -1,   0,   0,    0,  0>
   }

/* Folium
  This is a folium rotated about the x-axis.  The formula is:
     2 x^2 - 3 x y^2 - 3 x z^2 + y^2 + z^2 = 0. */
#declare Folium =
 quartic 
  {< 0,   0,   0,    0,  0,   0,   0,   0,   0,  2,
     0,   0,  -3,    0,  0,   0,   0,  -3,   0,  0,
     0,   0,   0,    0,  0,   1,   0,   0,   0,  0,
     0,   0,   1,    0,  0>
  }

/* Glob - sort of like basic teardrop shape. The equation is:
   y^2 + z^2 = 0.5 x^5 + 0.5 x^4. */
#declare Glob_5 =
 poly 
  {5,
   <-0.5, 0,   0,  -0.5, 0,   0,   0,   0,   0,  0,
     0,   0,   0,   0,   0,   0,   0,   0,   0,  0,
     0,   0,   0,   0,   0,   0,   0,   0,   0,  0,
     0,   0,   0,   0,   0,   0,   0,   0,   0,  0,
     0,   0,   0,   0,   1,   0,   0,   0,   0,  0,
     0,   0,   0,   1,   0,   0>
  }

/* Variant of a lemniscate - the two lobes are much more teardrop-like. */
#declare Twin_Glob =
 poly 
  {6,
   < 4,   0,   0,   0,   0,   0,   0,   0,   0, -4,
     0,   0,   0,   0,   0,   0,   0,   0,   0,  0,
     0,   0,   0,   0,   0,   0,   0,   0,   0,  0,
     0,   0,   0,   0,   0,   0,   0,   0,   0,  0,
     0,   0,   0,   0,   0,   0,   0,   0,   0,  0,
     0,   0,   0,   0,   0,   0,   0,   0,   0,  0,
     0,   0,   0,   0,   0,   0,   0,   0,   0,  0,
     1,   0,   0,   0,   0,   0,   0,   0,   0,  0,
     0,   1,   0,   0>
  }

/*  Approximation to the helix z = arctan(y/x).

   The helix can be approximated with an algebraic equation (kept to the
   range of a quartic) with the following steps:

      tan(z) = y/x   =>  sin(z)/cos(z) = y/x   =>

   (1) x sin(z) - y cos(z) = 0

   Using the taylor expansions for sin, cos about z = 0,

      sin(z) = z - z^3/3! + z^5/5! - ...
      cos(z) = 1 - z^2/2! + z^6/6! - ...

   Throwing out the high order terms, the expression (1) can be written as:

      x (z - z^3/6) - y (1 + z^2/2) = 0, or

  (2) -1/6 x z^3 + x z + 1/2 y z^2 - y = 0

  This helix (2) turns 90 degrees in the range 0 <= z <= sqrt(2)/2.  By using
  scale <2 2 2>, the helix defined below turns 90 degrees in the range
  0 <= z <= sqrt(2) = 1.4042.
*/
#declare Helix =
 quartic 
  {<  0,   0,   0,    0,  0,   0,   0,      0,   0,  0,
      0,   0,   0,    0,  0,   0,  -0.1666, 0,   1,  0,
      0,   0,   0,    0,  0,   0,   0,      0.5, 0, -1,
      0,   0,   0,    0,  0>
   clipped_by
    {object {Cylinder_Z scale 2}
     plane  { z, 1.4142}
     plane  {-z, 0}
    }
   bounded_by{clipped_by}
  }

/* This is an alternate Helix, using clipped_by instead of csg intersection. */
#declare Helix_1 = object {Helix}

/* Hyperbolic Torus having major radius sqrt(40), minor radius sqrt(12).
  This figure is generated by sweeping a circle along the arms of a
  hyperbola.  The equation is:

     x^4 + 2 x^2 y^2 - 2 x^2 z^2 - 104 x^2 + y^4 - 2 y^2 z^2 +
     56 y^2 + z^4 + 104 z^2 + 784 = 0.

  See the description for the torus below. */
#declare Hyperbolic_Torus_40_12 =
 quartic 
  {< 1,   0,   0,    0,     2,   0,   0,  -2,   0, -104,
     0,   0,   0,    0,     0,   0,   0,   0,   0,    0,
     1,   0,   0,   -2,     0,  56,   0,   0,   0,    0,
     1,   0, 104,    0,   784>
  }

/* Lemniscate of Gerono
  This figure looks like two teardrops with their pointed ends connected.
  It is formed by rotating the Lemniscate of Gerono about the x-axis.
  The formula is:
     x^4 - x^2 + y^2 + z^2 = 0. */
#declare Lemniscate =
 quartic 
  {< 1,   0,   0,   0,   0,   0,   0,   0,   0, -1,
     0,   0,   0,   0,   0,   0,   0,   0,   0,  0,
     0,   0,   0,   0,   0,   1,   0,   0,   0,  0,
     0,   0,   1,   0,   0>
  }

/* This is a figure with a bumpy sheet on one side and something that
  looks like a paraboloid (but with an internal bubble).  The formula
  is:
     (x^2 + y^2 + a c x)^2 - (x^2 + y^2)(c - a x)^2.

   -99*x^4+40*x^3-98*x^2*y^2-98*x^2*z^2+99*x^2+40*x*y^2+40*x*z^2+y^4+2*y^2*z^2
   -y^2+z^4-z^2

*/
#declare Quartic_Loop_1 =
 quartic 
  {<99,   0,   0, -40,  98,   0,   0,  98,   0, -99,
     0,   0, -40,   0,   0,   0,   0, -40,   0,   0,
    -1,   0,   0,  -2,   0,   1,   0,   0,   0,   0,
    -1,   0,   1,   0,   0>
  }

/* Monkey Saddle
  This surface has three parts that sweep up and three down.  This gives
  a saddle that has a place for two legs and a tail... The equation is:

     z = c (x^3 - 3 x y^2).

  The value c gives a vertical scale to the surface - the smaller the
  value of c, the flatter the surface will be (near the origin). */
#declare Monkey_Saddle =
 quartic 
  {< 0,   0,   0,   1,  0,   0,   0,   0,   0,  0,
     0,   0,  -3,   0,  0,   0,   0,   0,   0,  0,
     0,   0,   0,   0,  0,   0,   0,   0,   0,  0,
     0,   0,   0,  -1,  0>
  }

/* Parabolic Torus having major radius sqrt(40), minor radius sqrt(12).
  This figure is generated by sweeping a circle along the arms of a
  parabola.  The equation is:

     x^4 + 2 x^2 y^2 - 2 x^2 z - 104 x^2 + y^4 - 2 y^2 z +
     56 y^2 + z^2 + 104 z + 784 = 0.

  See the description for the torus below. */
#declare Parabolic_Torus_40_12 =
 quartic 
  {< 1,   0,   0,    0,     2,   0,   0,   0,  -2, -104,
     0,   0,   0,    0,     0,   0,   0,   0,   0,    0,
     1,   0,   0,    0,    -2,  56,   0,   0,   0,    0,
     0,   0,   1,  104,   784>
  }

/* Piriform
  This figure looks like a hersheys kiss. It is formed by sweeping
  a Piriform about the x-axis.  a basic form of the equation is:
     (x^4 - x^3) + y^2 + z^2 = 0.
*/
#declare Piriform =
 quartic 
  {< 4,   0,   0,   -4,  0,   0,   0,   0,   0,  0,
     0,   0,   0,    0,  0,   0,   0,   0,   0,  0,
     0,   0,   0,    0,  0,   1,   0,   0,   0,  0,
     0,   0,   1,    0,  0>
  }

/* n-Roll Mill
  This curve in the plane looks like several hyperbolas with their
  bumps arranged about the origin.  The general formula is:

     x^n - c(n,2) x^(n-2) y^2 + c(n,4) x^(n-4) y^4 - ... = a

  When rendering in 3-Space, the resulting figure looks like a
  cylinder with indented sides.
*/

/* Quartic parabola - a 4th degree polynomial (has two bumps at the bottom)
  that has been swept around the z axis. The equation is:
     0.1 x^4 - x^2 - y^2 - z^2 + 0.9 = 0. */
#declare Quartic_Paraboloid =
 quartic 
  {< 0.1, 0,   0,  0,   0,   0,   0,   0,   0,  -1,
     0,   0,   0,  0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,  0,   0,   0,   0,   0,   0,  -1,
     0,   0,  -1,  0,   0.9>
  }

/* Quartic Cylinder - a Space Needle?  */
#declare Quartic_Cylinder =
 quartic 
  {< 0,   0,   0,    0,   1,   0,   0,   0,   0,   0.01,
     0,   0,   0,    0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,    1,   0,   0,   0,   0,   0,   0,
     0,   0,   0.01, 0,  -0.01>
  }

/* Steiners quartic surface */
#declare Steiner_Surface =
 quartic 
  {< 0,   0,   0,  0,  1,   0,   0,   1,   0,   0,
     0,   0,   0,  0,  1,   0,   0,   0,   0,   0,
     0,   0,   0,  1,  0,   0,   0,   0,   0,   0,
     0,   0,   0,  0,  0>
  }

/* Torus having major radius sqrt(40), minor radius sqrt(12) */
#declare Torus_40_12 =
 quartic 
  {< 1,   0,   0,    0,     2,   0,   0,   2,   0, -104,
     0,   0,   0,    0,     0,   0,   0,   0,   0,    0,
     1,   0,   0,    2,     0,  56,   0,   0,   0,    0,
     1,   0, -104,   0,   784>
  }

/* Witch of Agnesi */
#declare Witch_Hat =
 quartic 
  {<  0,   0,   0,   0,   0,   0,   1,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   1,   0,   0.04,
      0,   0,   0,   0,   0.04>
  }

/* very rough approximation to the sin-wave surface z = sin(2 pi x y).
  In order to get an approximation good to 7 decimals at a distance of
  1 from the origin would require a polynomial of degree around 60.  This
  would require around 200k coefficients. For best results, scale by
  something like <1 1 0.2>. */
#declare Sinsurf =
 poly 
  {6,
   <    0,   0,   0,    0,  0,   0,   0,   0,   0,  0,
    -1116.226, 0, 0,    0,  0,   0,   0,   0,   0,  0,
        0,   0,   0,    0,  0,   0,   0,   0,   0,  0,
        0,   0,   0,    0,  0,   0,   0,   0,   0,  0,
        0,   0,   0,    0,  0,   0,   0,   0,   0, 18.8496,
        0,   0,   0,    0,  0,   0,   0,   0,   0,  0,
        0,   0,   0,    0,  0,   0,   0,   0,   0,  0,
        0,   0,   0,    0,  0,   0,   0,   0,   0,  0,
        0,   0,  -1,    0>
   }

/* Empty quartic equation.  Ready to be filled with numbers...
  quartic
   {< 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0>
   }
*/

#version ShapesQ_Inc_Temp;
#end
