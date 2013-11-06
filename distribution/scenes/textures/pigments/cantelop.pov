// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File by Dan Farmer
// Cantelope segments.  Uses onion for the cantelope interior and skin.
// Demonstrates intersection of spheres and planes, onion texture,
// color maps.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings { assumed_gamma 1.0 }

#include "colors.inc"

// Macro for the adjustment of images for POV-Ray 3.6.2
// for image_map with assumed_gamma = 1.0 ;
#macro Correct_Pigment_Gamma(Orig_Pig, New_G)
  #local Correct_Pig_fn =
      function{ pigment {Orig_Pig} }
  pigment{ average pigment_map{
   [function{ pow(Correct_Pig_fn(x,y,z).x, New_G)}
               color_map{[0 rgb 0][1 rgb<3,0,0>]}]
   [function{ pow(Correct_Pig_fn(x,y,z).y, New_G)}
               color_map{[0 rgb 0][1 rgb<0,3,0>]}]
   [function{ pow(Correct_Pig_fn(x,y,z).z, New_G)}
               color_map{[0 rgb 0][1 rgb<0,0,3>]}]
   }}
#end //
// "image_map" gamma corrected:
//    Correct_Pigment_Gamma(
//    pigment{ PIGMENT }
//    , Correct_Gamma )
//------------------------------------------------
//------------------------------------------------

#declare Melon = texture {
   finish { ambient 0.2 }
Correct_Pigment_Gamma( // gamma correction
   pigment {
      onion
      color_map {
         [0.0   0.95 color Orange       color Orange ]
         [0.95  0.96 color Orange       color GreenYellow ]
         [0.96  0.98 color GreenYellow  color Khaki  ]
         [0.98  1.00 color NewTan       color DarkTan ]
      }
   }
, 1.5 ) //, New_Gamma
}

camera {
   location <-2, 3, -3>
   angle 40
   right   x*image_width/image_height
   look_at <0, 0.10, 0>
}


// Light source
#declare Grayscale = 0.25;
#declare AmbientLight = color red Grayscale green Grayscale blue Grayscale;

light_source { <-20, 30, -100> color White }

light_source { <0, 50, 10> color AmbientLight }

box { <-6,-1,-5>,<8,0,8> 
      pigment{ color rgb<0.45,0.38,0.35>*0.2  } 
      translate<0,-1,0>
    } //      

// Flat-topped sphere/plane intersection
#declare MelonHalf = intersection {
   sphere { <0, 0, 0>, 1 }                // outer wall
   sphere { <0, 0, 0>, 0.65 inverse }     // inner wall
   plane { y, 0 }                         // top surface

   texture { Melon }
   bounded_by { sphere { <0, 0, 0>, 1.001 } }
}

// Quarter Wedge of above melon
#declare MelonWedge = intersection {
   sphere { <0, 0, 0>, 1 }                 // outer wall
   sphere { <0, 0, 0>, 0.65 inverse }      // inner wall
   plane { y, 0 rotate  45*x }             // top surface
   plane { y, 0 rotate -45*x }             // top surface

   texture { Melon }
   bounded_by { sphere { <0, 0, 0>, 1.001 } }
}

object { MelonHalf }
object { MelonWedge rotate 30*y translate <2, 0, 2> }
