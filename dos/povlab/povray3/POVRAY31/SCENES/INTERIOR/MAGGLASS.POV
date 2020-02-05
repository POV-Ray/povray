// Persistence Of Vision raytracer version 3.1 sample file.
// Magnifying glass created using POV-Ray's refraction.
// A convex lens created with CSG
// (and something to view through it)
// This example doesn't work very well, but it gives a good
// starting point for a better use of the magnifying glass.


global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"
#include "glass.inc"
#include "consts.inc"                  // Index of refraction constants

camera {
   location <0.0, 2, -10>
   direction <0.0, 0.0, 1>
   angle 40
   up  <0.0, 1.0, 0.0>
   right <4/3, 0.0, 0.0>
   look_at <0, 0, 0>
}

light_source { <30, 50, -50> color White }

light_source { <-30, 10, 20> color Gray50 }

fog { color Gray50 distance 200 }  // This fog reaches max density at 200z

// Background sphere
sphere { <0, 0, 0>, 1
     hollow on
     finish { crand 0.015 }
     pigment {
        gradient y
        color_map {
            [0.0 1.0 color Gray80 color Gray30]
        }
        scale 10000
    }
}

// A lens.  This uses the Ellipsoid quadric to make it independantly
// scalable, but it would be faster to use spheres.
// It is designed "sideways" so you can see the thickness.
// It is then rotated 90o on Y so the viewer is looking through the lens.
#declare Lens_Thickness = 0.35;
#declare Lens_Diameter = 1.5;

#declare Lens =
intersection {
   sphere { <0, 0, 0>, 1.5 translate <0.75, 0, 0> }
   sphere { <0, 0, 0>, 1.5 translate <-0.75, 0, 0> }

   interior{ior Flint_Glass_Ior}
   texture {
       T_Glass3
       finish {
          reflection 0                  // Over-ride reflection
       }
   }

   scale <Lens_Thickness, Lens_Diameter, Lens_Diameter>
}


plane { y, -4
    pigment {
       checker color HuntersGreen color SummerSky
       scale <3, 1, 3>
    }
    finish {
       ambient 0.2
       diffuse 0.6
    }
}

object { Lens rotate 80*y }

// A sphere in the distance
sphere { <3, 1, 30>, 2 finish {Phong_Shiny} pigment {Orange} }

object { Cylinder_X
   finish {
      Phong_Shiny
      ambient 0.25
      diffuse 0.6
   }
   pigment {
      granite
      scale 2
   }

    rotate -75*y
    translate <0 ,-3, 25>
}
