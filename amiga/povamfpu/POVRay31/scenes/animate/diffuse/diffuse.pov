// Persistence Of Vision raytracer version 3.1 sample file.
// File by Dan Farmer
// Diffuse values demonstration

global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "stdcam.inc"

#declare D = clock;

#declare Font="cyrvetic.ttf"
text{ ttf Font
    concat("diffuse=",str(D,1,2)),0.1,0
    scale <1.25, 1.25, 4>
    translate <-4, 0, 0>
    pigment { rgb <1, 0.5, 0.2> }
    finish { diffuse D }
}

sphere { <-1.5, 0.4, -2.5>, 0.4
    pigment { LimeGreen }
    finish { diffuse D }
}
