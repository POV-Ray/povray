// Persistence Of Vision raytracer version 3.1 sample file.

// Sample quartic file 
// by Alexander Enzmann

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"
#include "shapesq.inc"

// Get the declaration of the coordinate axes 
#include "axisbox.inc"

// Declare the orientation of the surface 
#declare Steiner_Orientation = <50, -20, 0>;

object {
   Steiner_Surface

   texture { pigment { Red } finish { phong 1.0 } }

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
   right     <4/3, 0.0,  0.0>
   up        <0.0, 1.0,  0.0>
   direction <0.0, 0.0,  1.0>
}

// Put in some light sources so that highlighting can give visual clues
//  as to the shape of the surface. 
light_source { <200, 100, -300> colour White }

light_source { <-200, 100, -300> colour White }
