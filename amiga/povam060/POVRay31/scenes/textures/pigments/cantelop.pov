// Persistence Of Vision raytracer version 3.1 sample file.
// File by Dan Farmer
// Cantelope segments.  Uses onion for the cantelope interior and skin.
// Demonstrates intersection of spheres and planes, onion texture,
// color maps.

global_settings { assumed_gamma 2.2 }

#include "colors.inc"

#declare Melon = texture {
   finish { ambient 0.2 }
   pigment {
      onion
      color_map {
         [0.0   0.95 color Orange       color Orange ]
         [0.95  0.96 color Orange       color GreenYellow ]
         [0.96  0.98 color GreenYellow  color Khaki  ]
         [0.98  1.00 color NewTan       color DarkTan ]
      }
   }
}

camera {
   location <-2, 3, -3>
   direction <0.0, 0.0, 2.0>
   up  <0.0, 1.0, 0.0>
   right <4/3, 0.0, 0.0>
   look_at <0, 0, 0>
}


// Light source
#declare Grayscale = 0.25;
#declare AmbientLight = color red Grayscale green Grayscale blue Grayscale;

light_source { <-20, 30, -100> color White }

light_source { <0, 50, 10> color AmbientLight }

// Flat-topped sphere/plane intersection
#declare MelonHalf = intersection {
   sphere { <0, 0, 0>, 1 }                // outer wall
   sphere { <0, 0, 0>, 0.65 inverse }     // inner wall
   plane { y, 0 }                         // top surface

   texture { Melon }
   bounded_by { sphere { <0, 0, 0>, 1.001 } }
}

// Quarter Wedge of above melon
#declare MelonWedge = intersection {
   sphere { <0, 0, 0>, 1 }                 // outer wall
   sphere { <0, 0, 0>, 0.65 inverse }      // inner wall
   plane { y, 0 rotate  45*x }             // top surface
   plane { y, 0 rotate -45*x }             // top surface

   texture { Melon }
   bounded_by { sphere { <0, 0, 0>, 1.001 } }
}

object { MelonHalf }
object { MelonWedge rotate 30*y translate <2, 0, 2> }
