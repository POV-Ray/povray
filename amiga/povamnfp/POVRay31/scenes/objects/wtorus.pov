// Persistence Of Vision raytracer version 3.1 sample file.
// A single wooden torus
// Illustrates what is possible with POV-Raywood textures.
// File by Dan Farmer Jan 1992

global_settings { assumed_gamma 2.2 }

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"


// Wooden torus
torus { 7.0, 3.0
   // Bottom layer texture.  Uses a "stretched" bozo for fine porous grain
   texture {
      pigment {
         bozo
         color_map {
            [0.0 0.4 color BakersChoc  color BakersChoc ]
            [0.4 1.01 color Tan color Tan]
         }
         scale <4, 0.05, 0.05>
      }
   }

   // Overlaying ring grain texture
   texture {
      finish {
         phong 1
         phong_size 100
         brilliance 3
         ambient 0.2
         diffuse 0.8
      }
      pigment {
         wood
         turbulence 0.025

         color_map {
            [0.0 0.15 color SemiSweetChoc color CoolCopper ]
            [0.15 0.40 color CoolCopper color Clear ]
            [0.40 0.80 color Clear  color CoolCopper ]
            [0.80 1.01 color CoolCopper color SemiSweetChoc ]
         }

         scale <3.5, 1, 1>
         translate -50*y
         rotate 1.5*z
      }
   }
}


// Main light source
light_source { <-50.0, 100, -80.0> colour White }

// Dim side light to fill shadows
light_source { <250.0, 25.0, -100.0> colour DimGray }


camera {
   location <0.0, 20.0, -15.0>
   direction <0.0, 0.0, 1.0>
   up  <0.0, 1.0, 0.0>
   right <4/3, 0.0, 0.0>
   look_at <0, 0, 0>
}
