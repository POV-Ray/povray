// Persistence Of Vision raytracer version 3.1 sample file.
// File by Dan Farmer
// Specular values demonstration


global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "stdcam.inc"

#declare S = clock;

#declare Font="cyrvetic.ttf"
text{ ttf Font
    concat("specular=",str(S,1,1)),0.1,0
    scale <1.25, 1.25, 4>
    translate <-3.75, 0, 0>
    pigment { rgb <1, 0.5, 0.2> }
    finish { specular S roughness 0.1 }
}

sphere { <-1.5, 0.4, -2.5>, 0.4
    pigment { LimeGreen }
    finish { specular S roughness 0.1 }
}

