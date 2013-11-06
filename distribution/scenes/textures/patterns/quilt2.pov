// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Quilted pattern example
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

global_settings { assumed_gamma 1.0 }

#include "colors.inc"

camera {
     location  <0,0,-20>
     right     x*image_width/image_height
     direction 3*z
 }

 light_source { <300, 500, -500> color Gray65}
 light_source { <-50,  10, -500> color Gray45}

 #default {
     pigment { White }
     finish { phong 1 phong_size 400  reflection{ 0.1 } }
 }

 #declare Bump_Value = 5;
 #declare Pattern_Scale = .85;

 #declare Thing =
 box{ <-1,-1,-1>,<1,1,1> scale .8 }

 // top row, left to right
 object { Thing
     normal{
         quilted Bump_Value
         control0 0 control1 0
         scale Pattern_Scale
     }
     rotate <-30,30,0>
     translate <-3,2,0>
 }
 object { Thing
     normal{
         quilted Bump_Value
         control0 0 control1 0.5
         scale Pattern_Scale
     }
     rotate <-30,30,0>
     translate <0,2,0>
 }
 object { Thing
     normal{
         quilted Bump_Value
         control0 0 control1 1
         scale Pattern_Scale
     }
     rotate <-30,30,0>
     translate <3,2,0>
 }

 // middle row, left to right
 object { Thing
     normal{
         quilted Bump_Value
         control0 0.5 control1 0
         scale Pattern_Scale
     }
     rotate <-30,30,0>
     translate <-3,0,0>
 }
 object { Thing
     normal{
         quilted Bump_Value
         control0 0.5 control1 0.5
         scale Pattern_Scale
     }
     rotate <-30,30,0>
     translate <0,0,0>
 }

 object { Thing
     normal{
         quilted Bump_Value
         control0 0.5 control1 1
         scale Pattern_Scale
     }
     rotate <-30,30,0>
     translate <3,0,0>
 }

 // bottom row, left to right
 object { Thing
     normal{
         quilted Bump_Value
         control0 1 control1 0
         scale Pattern_Scale
     }
     rotate <-30,30,0>
     translate <-3,-2,0>
 }

 object { Thing
     normal{
         quilted Bump_Value
         control0 1 control1 0.5
         scale Pattern_Scale
     }
     rotate <-30,30,0>
     translate <0,-2,0>
 }
 object { Thing
     normal{
         quilted Bump_Value
         control0 1 control1 1
         scale Pattern_Scale
     }
     rotate <-30,30,0>
     translate <3,-2,0>
 }

