// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Raytracer sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.6;
global_settings {assumed_gamma 1.0}
#default{ finish{ ambient 0.1 diffuse 0.9 }} 

#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"
#include "woods.inc"

camera {
   location <0, 10, -20>
   direction <0, 0,  3>
   right x*1.33
   look_at 1.5*y
}

light_source {<-50, 50, -1000> color White*0.1 }
light_source {< 50, 30, -30>   color White*0.9 }


background { color Gray05 }

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
//    , Correct_Gamma)
//------------------------------------------------
//------------------------------------------------



#declare Stack =
union {
   sphere{<0, 4, 0>, 1}
   cone { -y,1, y, 0.5 translate 2*y }
   object {UnitBox}
   no_shadow
}

#include "cedar.map"
object {
    Stack
    texture{
     Correct_Pigment_Gamma( // gamma correction
        pigment {
            P_WoodGrain6A
            color_map { M_Cedar }
        }
    , 2.2 ) //, New_Gamma
        normal {            // Copy of WoodGrain6A pigment
            wood 0.4
            turbulence <0.05, 0.08, 1>
            octaves 4
            scale <0.15, .15, 1>
            translate -x*100
        }
     scale 0.6
     rotate z*35
     rotate x*85
    }
}
