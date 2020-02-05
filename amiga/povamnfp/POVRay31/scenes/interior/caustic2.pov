// Persistence Of Vision raytracer version 3.1 sample file.
// Caustics example

global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "textures.inc"

light_source { <0, 50, 0> color White }

camera {
    direction z
    location <0, 6, -15>
    look_at <0, 2, 0>
}

// The sea floor
plane { y, 0
    pigment { Gray60 }
    finish { ambient 0.1 diffuse 0.7 }
}

// The water surface
plane { y, 10
    hollow on
    pigment { red 0.7 green 0.7 blue 1.0 filter 0.9 }
    finish {reflection 0.7 }
    interior { ior 1.1 caustics 1.0 }
    translate <5, 0, -10>
    normal { bumps 0.5 }
}
