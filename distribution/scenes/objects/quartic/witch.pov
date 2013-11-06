// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3

// By Alexander Enzmann
/* sample quartic scene file written by Alexander Enzmann */
#version  3.7;
global_settings { 
  assumed_gamma 1.0
}

#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"

/* Witch of Agnesi */
intersection {
   quartic {
      < 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  1.0,  0.0,  0.0,  0.0,
        0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
        0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  1.0,  0.0,  0.04,
        0.0,  0.0,  0.0,  0.0,  0.04 >

      texture {
         pigment{ color rgb<0.7,0.0,0.0>  }
         normal { radial 0.5 frequency 20 sine_wave }  
         finish {
            //specular 0.5 roughness 0.05
            phong 1 phong_size 30
            //ambient 0.2
            //diffuse 0.8
         }
      }
   }

   sphere {
      <0, 0, 0>, 1
      texture { pigment { Clear } }
   }

   bounded_by { sphere { <0, 0, 0>, 1.5 } }
   rotate <30, 0, 180>
}

//-------------------------------------------------------------------------
// This scene uses a non-standard camera set-up. 
// (See CAMERA in the included documentation for details.) 
// If you are new to POV-Ray, you might want to try a different demo scene.
//-------------------------------------------------------------------------
camera {
   location  <0.0, 0.0, -3.0>
   direction <0.0, 0.1,  1.5>
   up        <0.0, 1.0,  0.0>
   right     x*image_width/image_height
}

light_source { <1200, 30, -300> colour White }

light_source { <-200,1000, -300> colour White }

background { color rgb<1,1,1>*0.03 } 
 
