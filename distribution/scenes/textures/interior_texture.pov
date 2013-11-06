// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// This file demonstrates the "interior_texture" feature
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.6;
global_settings {assumed_gamma 1.0}

#include "colors.inc"

camera { location <100,50,50> 
         right     x*image_width/image_height
         angle 58  
         look_at <-5,25,0>
       }

plane {y,0 pigment {White}}

//--------------declaring two "peel" textures
#declare Texture_1 =
texture {
  pigment {
   spiral1 1
   color_map {
    [0.0 White]
    [0.2 Wheat]
    [0.4 Orange]
    [0.4 Clear]
    [1.0 Clear]
   }
   scale 5
   }
  normal {bumps .3 scale .2}
  }
#declare Texture_2 =
texture {
  pigment {
   spiral1 1
   color_map {
    [0.0 Blue]
    [0.2 Red]
    [0.6 YellowGreen]
    [0.6 Clear]
    [1.0 Clear]
   }
   scale 5
   }
  normal {bumps .3 scale .2}
  }

//--------making spheres with different inside/outside textures

sphere {<0,25,-30>,25
        texture {Texture_1}
        interior_texture {Texture_2}
        }

sphere {<0,25,30>,25
        texture {pigment {rgb <0.5,0.4,1>}}
        interior_texture {pigment {rgb <1,0.7,0.0>} normal {bumps .7 scale .15} }
        clipped_by {box {<-50,20,-50>,<50,35,50> inverse}}
        }

light_source {<400,5000,3000> White*1.5 }
fog {White distance 1700}
