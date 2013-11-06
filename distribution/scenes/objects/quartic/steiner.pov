// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3

// Sample quartic file
// by Alexander Enzmann
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
#declare Steiner_Orientation = <50, -25, 0>;

object {
   Steiner_Surface

   texture { pigment { color rgb<0.9,0.1,0.2>  } finish { phong 1 phong_size 10 } }

   bounded_by { sphere { <0, 0, 0>, 1 } }

   scale 6
   rotate Steiner_Orientation
}

// Show coordinate axes
object {
   Axes
   rotate Steiner_Orientation
}

// The viewer is eight units back along the z-axis.
camera {
   location  <0.0, 0.0, -8.0>
   right     x*image_width/image_height
   angle 65 
}

// Put in some light sources so that highlighting can give visual clues
//  as to the shape of the surface.
light_source { <500, 100, -300> colour White }

light_source { <-200, 100, -300> colour White }

background { color rgb<1,1,1>*0.03 } 
 
