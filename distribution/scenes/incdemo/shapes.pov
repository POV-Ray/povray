// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//   Persistence of Vision Raytracer Scene Description File
//   File: shapes.pov
//   Author: Chris Huff and Rune S. Johansen
//   Description:
// This scene demonstrates some of the new macros in shapes.inc.
//
// -w320 -h240
// -w800 -h600 +a0.3
//
//*******************************************

#version 3.7;

#include "stdinc.inc"

//-------------------------------------------

global_settings {
   assumed_gamma 1.0
   max_trace_level 10
}

#default {finish {ambient 0}}

camera {
   location <-2.0, 3.5,-8.5>
   up y  right x*image_width/image_height
   angle 45
   look_at <0.0, 0.7, 0.0>
}

light_source {<-150, 200,-100>, color 0.8 * <1.0, 1.0, 0.6>} // yellowish light from the left
light_source {< 150, 100,-100>, color 0.7 * <0.6, 0.6, 1.0>} // blueish light from the right
light_source {< -50, 200, 100>, color 0.5 * <1.0, 1.0, 1.0>} // dim light from the back of the objects.

box {
   <-10,-1,-10>, < 10, 0, 10>
   texture {
      pigment {checker color <0.1, 0.3, 0.4>, color <0.2, 0.5, 0.7>}
      finish {diffuse 0.7 reflection 0.2}
   }
}

//*******************************************

#declare ShinyFinish =
finish {
   diffuse 0.7
   reflection {0.1, 1.0 fresnel}
   specular 1 roughness 0.0035 metallic
}
#declare Int =
interior {ior 1.7}


object {
   Align_Object( // Bevelled_Text(Font, String, Cuts, BevelAng, BevelDepth, Depth, Offset)
      Center_Object(Bevelled_Text("crystal.ttf", "POV", 3, 45, 0.025, 0.25, 0, no), x)
      -y, < 0, 0, 0>
   )
   texture {
      pigment {color <0.5,0.3,0.1>}
      finish {
         diffuse 0.8 brilliance 2
         reflection {0.3 metallic}
         specular 1 roughness 0.1 metallic
      }
   }
   scale 2
   translate <-1.5, 0.0,-0.5>
}

object {
   Supercone(< 0, 0, 0>, 0.5, 1.00, < 0, 2, 0>, 1.0, 0.25)
   texture {
      pigment {color rgb 0.5}
      finish {ShinyFinish}
   }
   interior {Int}
   translate < 1.5, 0.0, 3.5>
}

object {
   Round_Cone(< 0, 0, 0>, 1.0, < 0, 2, 0>, 0.5, 0.15, no)
   texture {
      pigment {color rgb 1}
      finish {ShinyFinish}
   }
   interior {Int}
   translate < 2.5, 0.0, 2.0>
}

// The Round_Cone2 macro below makes a rounded cone out of a cone and two spheres,
// where the cone in the middle is made to fit the spheres in the ends.
union {
   sphere {< 0, 1, 0>, 1.0001}
   sphere {< 0, 2, 0>, 0.5001}
   texture {
      pigment {color rgb 0.5}
      finish {ShinyFinish}
   }
   interior {Int}
   translate < 3, 0, 0>
}
object {
   Round_Cone2(< 0, 1, 0>, 1.0, < 0, 2, 0>, 0.5, yes)
   texture {
      pigment {color rgb 1}
      finish {ShinyFinish}
   }
   interior {Int}
   translate < 3, 0, 0>
}

// The Round_Cone3 macro below makes a rounded cone out of a cone and two spheres,
// where the spheres in the ends are made to fit the cone in the middle.
cone {
   <-1, 0, 0>, 0.5001, < 1, 1, 0>, 0.1001
   texture {
      pigment {color rgb 0.5}
      finish {ShinyFinish}
   }
   interior {Int}
   translate <-2, 2, 3>
}
object {
   Round_Cone3(<-1, 0, 0>, 0.5, < 1, 1, 0>, 0.1, yes)
   texture {
      pigment {color rgb 1}
      finish {ShinyFinish}
   }
   interior {Int}
   translate <-2, 2, 3>
}

object {
   Round_Cylinder(< 0, 0, 0>, < 0, 1.5, 0>, 0.75, 0.25, no)
   texture {
      pigment {color rgb 1}
      finish {ShinyFinish}
   }
   interior {Int}
   translate <-0.5, 0.0, 3.5>
}
object {
   Wire_Box(<-1.1, 0.0,-0.1>, < 1.1, 1.6, 1.1>, 0.05, no)
   texture {
      pigment {color rgb 1}
      finish {ShinyFinish}
   }
   interior {Int}
   translate <-2.5, 0.0, 2.0>
}
object {
   Round_Box(<-1.0, 0.0, 0.0>, < 1.0, 1.5, 1.0>, 0.25, no)
   texture {
      pigment {color rgb 0.5}
      finish {ShinyFinish}
   }
   interior {Int}
   translate <-2.5, 0.0, 2.0>
}

//*******************************************

#declare MyFunction = function{pattern {bumps scale 0.2}}

object {
   HF_Square (MyFunction, off, off, <60,60>, on, "", <0.0, 0.1, 0.0>, <1.5, 0.5, 1.5>)
   texture {
      pigment {color rgb 1}
      finish {ShinyFinish}
   }
   translate <-3.0, 0.0,-3.2>
}

object {
   HF_Sphere (MyFunction, off, off, <60,60>, on, "", <0.0, 0.6, 0.0>, 0.3, 0.3)
   texture {
      pigment {color rgb 1}
      finish {ShinyFinish}
   }
   translate <-0.7, 0.0,-3.3>
}

object {
   HF_Cylinder (MyFunction, off, off, <60,60>, on, "", <0.5, 0.0, 0.5>, <0.5, 1.0, 0.5>, 0.4, 0.3)
   texture {
      pigment {color rgb 1}
      finish {ShinyFinish}
   }
   translate < 0.7, 0.0,-3.5>
}

object {
   HF_Torus (MyFunction, off, off, <60,60>, on, "", 0.5, 0.1, 0.3)
   texture {
      pigment {color rgb 1}
      finish {ShinyFinish}
   }
   translate < 0.5, 0.3,-1.0>
}
