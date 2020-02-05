// Persistence Of Vision raytracer version 3.1 sample file.

// Sample quartic file
// by Alexander Enzmann

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"
#include "shapesq.inc"

/*
   Approximation to the helix z = arctan(y/x).

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

#declare Red_Helix =
object {
   Helix
   hollow on
   texture {
      pigment { Red }
      finish { phong 1.0 }
      /* scale <1, 1.4142, 1> */
   }
}

#declare Green_Helix =
object { 
   Helix
   hollow on
   texture {
      pigment { Green }
      finish { phong 1.0 }
      /* scale <1, 1.4142, 1> */
   }
}

// Glue a bunch of pieces together to make one long helix. 

object {
   Green_Helix
   translate -4.2426*z
   rotate 160*z
   rotate -90*x
   translate <0, -2, 5>
}

object {
   Red_Helix
   translate -2.8284*z
   rotate 70*z
   rotate -90*x
   translate <0, -2, 5>
}

object {
   Green_Helix
   translate -1.4142*z
   rotate 160*z
   rotate -90*x
   translate <0, -2, 5>
}

object {
   Red_Helix
   rotate 70*z
   rotate -90*x
   translate <0, -2, 5>
}

object {
   Green_Helix
   translate 1.4142*z
   rotate 160*z
   rotate -90*x
   translate <0, -2, 5>
}

object {
   Red_Helix
   translate 2.8284*z
   rotate 70*z
   rotate -90*x
   translate <0, -2, 5>
}

object {
   Green_Helix
   translate 4.2426*z
   rotate 160*z
   rotate -90*x
   translate <0, -2, 5>
}

object {
   Red_Helix
   translate 5.6569*z
   rotate 70*z
   rotate -90*x
   translate <0, -2, 5>
}

object {
   Green_Helix
   translate 7.0711*z
   rotate 160*z
   rotate -90*x
   translate <0, -2, 5>
}


camera {
   location  <0.0, 0.0, -10.0>
   direction <0.0, 0.0, 1.0>
   up        <0.0, 1.0, 0.0>
   right     <4/3, 0.0, 0.0>
}

// Toss in a couple of light sources. 
light_source { <200, 100, -300> colour red 1.0 green 1.0 blue 1.0 }

light_source { <-200, 100, -300> colour red 1.0 green 1.0 blue 1.0 }
