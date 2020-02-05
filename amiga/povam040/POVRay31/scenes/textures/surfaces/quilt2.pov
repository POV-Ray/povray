// Persistence Of Vision raytracer version 3.1 sample file.
// Quilted pattern example

global_settings { assumed_gamma 2.2 }

#include "colors.inc"

camera {
     location <0,0,-20>
     direction 3*z
 }

 light_source { <300, 500, -500> color Gray65}
 light_source { <-50,  10, -500> color Gray45}

 #default {
     pigment { White }
     finish { phong 0.8 phong_size 200 }
 }

 #declare Bump_Value = 1;
 #declare Pattern_Scale = .75;

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

