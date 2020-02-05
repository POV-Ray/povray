// Persistence Of Vision raytracer version 3.1 sample file.

// Sample Quartic file
// by Alexander Enzmann

// Bicorn 

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
      texture { pigment { Blue } finish { phong 1.0 } }

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
   right     <4/3, 0.0,  0.0>
   up        <0.0, 1.0,  0.0>
   direction <0.0, 0.0,  1.0>
}

// Put in some light sources so that highlighting can give visual clues
//  as to the shape of the surface.
light_source { <200, 100, -300> colour White }

light_source { <-200, 100, -300> colour White }
