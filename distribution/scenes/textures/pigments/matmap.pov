// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Material_map example
// File by Drew Wells
// NOTE: Uses povmap.png
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0}

#include "colors.inc"
#include "textures.inc"
#include "skies.inc"

camera {
   location  <0,  0,-120>
   right   x*image_width/image_height
   angle 35 
   look_at  <0,0,0>
}


sphere { <0, 0, 0>, 25
   texture {
      material_map {
         png "povmap.png"
         /* Now a list of textures to map with instead of colors */
         texture {
            pigment {color red 0.3 green 0.1 blue 1}
            normal  {ripples 0.85 frequency 10 }
            finish  {specular 0.75}
            scale 5
         }

         texture {
            pigment {White}
            finish {ambient 0 diffuse 0 reflection 0.9 specular 0.75}
         }

         texture {pigment{NeonPink} finish{Luminous}}

         texture {
            pigment {
               gradient y
               colour_map {
                  [0.00, 0.33  colour red 1 green 0 blue 0
                               colour red 0 green 0 blue 1]
                  [0.33, 0.66  colour red 0 green 0 blue 1
                               colour red 0 green 1 blue 0]
                  [0.66, 1.001 colour red 0 green 1 blue 0
                               colour red 1 green 0 blue 0]
               }
            }
            finish{specular 0.75}
            scale 8
         }
      }
      scale 30
      translate <-15, -15, 0>
   }
}

/*Sky*/
sky_sphere { S_Cloud2 }

plane { y,-25
   pigment {
      checker  color Gold color Firebrick
      scale 10
   }
   finish {
      ambient 0.1
      diffuse 0.8
      /*reflection 0.6*/
   }
}


light_source {<100, 140, -130> colour White}
