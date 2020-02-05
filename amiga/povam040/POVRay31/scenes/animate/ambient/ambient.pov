// Persistence Of Vision raytracer version 3.1 sample file.
// File by Dan Farmer
// Ambient values example

global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "stdcam.inc"

#declare A = clock;

#declare Font="cyrvetic.ttf"
text{ ttf Font
    concat("ambient=",str(A,1,2)),0.1,0
    scale <1.25, 1.25, 4>
    translate <-4, 0, 0>
    pigment { rgb <1, 0.5, 0.2> }
    finish { ambient A }
}

sphere { <-1.5, 0.4, -2.5>, 0.4
    pigment { LimeGreen }
    finish { ambient A }
}
