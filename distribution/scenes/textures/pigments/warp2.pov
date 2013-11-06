// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Texture warp example
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0}

#include "colors.inc"
#include "textures.inc"

camera {
  location <0,3,-29>
  right   x*image_width/image_height
  direction 3.5*z
}

plane {  y,-1.01  hollow on pigment {checker Yellow,White}}

plane {  z, 4.01  hollow on pigment {checker Yellow,White}}

light_source { <300, 500, -500> color Gray65}
light_source { <-50,  10, -500> color Gray65}


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

#declare Thing = plane{z,0.1 hollow on clipped_by{box{<-2.75,-2,-2>,<2.75,2,2>}}}

#declare Tree = 
  Correct_Pigment_Gamma( // gamma correction
    pigment{ DMFWood4 scale 2 translate <1/2,0,1>
             rotate x*85 translate 10*y}
, 2.1 ) //, New_Gamma

object{Thing
  pigment{ Tree }
  translate <-3,5.25,0>
}

object{Thing
  pigment{ Tree
    warp{repeat x*2}
  }
  translate <3,5.25,0>
}

object{Thing
  pigment{ Tree
    warp{repeat x*2 offset z*0.25}
  }
  translate <-3,1,0>
}

object{Thing
  pigment{ Tree
    warp{repeat x*2 flip y}
  }
  translate <3,1,0>
}

