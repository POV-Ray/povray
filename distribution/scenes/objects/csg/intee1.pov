// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
// Intersection of two cylinders,  with holes cut from each one, in pink
// marblized stone.
// File by Dan Farmer.
// Demonstrates CSG intersection,  layered stone texture.
// POV-Ray 2.0 Time: about 3 hours @640x480+a0.2 on 486/33
//
// -w320 -h240
// -w800 -h600 +a0.3
#version  3.7; 
global_settings { assumed_gamma 1.5 }

#include "colors.inc"           // Standard colors library
#include "shapes.inc"           // Commonly used object shapes
#include "textures.inc"         // LOTS of neat textures.  

camera {
   location <0.0, 10, -26>
   angle 69
   right     x*image_width/image_height
   look_at   <0,-0.8, 0>
}

// Light source
light_source { <-10, 20, -25> color White }
light_source { <0, 0, 0> color White }

#declare Color1A = color DustyRose;
#declare Color1B = color DarkSlateGray;

intersection {
   object { Cylinder_X }
   object { Cylinder_Y }
   object { Cylinder_X inverse scale <1, 0.5, 0.5> }  // small hole
   object { Cylinder_Y inverse scale <0.5, 1, 0.5> }  // ditto

   // A great looking pinkish marble.  Except that it doesn't use marble
   // or agate... it's all done with bozo and granite!

   // Underlying surface is very subtly mottled with bozo in pinks and
   // grays.

   texture {
      finish {ambient 0.25 }
      pigment {
         bozo
         turbulence 0.25
         color_map {
            [0.0 1.0 color red 0.9 green 0.75 blue 0.75
                     color red 0.6 green 0.6 blue 0.6 ]
         }
         scale 0.4
      }
   }

   // Second layer texture has some filter values, yet a fair amount of color
   // Viening is kept quite thin.
   texture {
      finish {
         Glossy
         phong 0.25
         phong_size 75
         brilliance 4
      }
      pigment {
         granite
         color_map {
            [0.0 0.9  color Color1A filter 1 color Color1A filter 0.5 ]
            [0.9 1.0  color Color1B color Color1B ]
         }
         scale 2
      }
   }

   scale 10
   rotate y*45
}
