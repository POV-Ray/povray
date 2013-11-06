// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3

// Sample Quartic file
// by Alexander Enzmann

// Bicorn
#version  3.7;
global_settings { 
  assumed_gamma 1.0
}

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"
#include "shapesq.inc"

// Get the declaration of the coordinate axes
#include "axisbox.inc"

// Declare the orientation of the surface
#declare Orient = <-20, -60, 0>;

// Bicorn
union {
   object {
      Bicorn
      sturm
      texture { pigment { Red } finish { phong 1.0 } }

      scale 3

      bounded_by { sphere { <0, 0 ,0>, 3 } }

      clipped_by {
         plane {  x, 0.5 }
         plane { -x, 0.5 }
      }
   }

   object {
      Bicorn
      sturm
      texture { pigment { color rgb<0.35,0.75,1> } finish { phong 1.0 } }

      scale 2.99
      translate 0.01*y

      bounded_by { sphere { <0, 0, 0>, 2.99 } }

      clipped_by {
         plane {  x, 0.49 }
         plane { -x, 0.49 }
      }
   }

   translate -1.5*y
   rotate Orient
}

// Show coordinate axes
object {
   Axes
   rotate Orient
}

// The viewer is eight units back along the z-axis.
camera {
   location  <0.0, 0.0, -8.0>
   right     x*image_width/image_height
   angle 65 
}

// Put in some light sources so that highlighting can give visual clues
//  as to the shape of the surface.
light_source { <200, 100, -300> colour White }

light_source { <-200, 100, -300> colour White }

background { color rgb<1,1,1>*0.03 } 
 
