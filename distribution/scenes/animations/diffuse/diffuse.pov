// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracersample file.
// File by Dan Farmer
// Diffuse values demonstration

#version 3.7;

global_settings { assumed_gamma 1.0 }


#include "colors.inc"
#include "stdcam.inc"

#declare D = clock;

#declare Font="cyrvetic.ttf"
text{ ttf Font
    concat("diffuse=",str(D,1,2)),0.1,0
    rotate<0,-5,0> 
    scale <1.25, 1.5, 4>
    translate <-3.2, 0, -1>
    pigment { rgb <1, 0.5, 0.2> }
    finish { diffuse D }
}

sphere { <-1.0, 0.4, -3.0>, 0.4
    pigment { LimeGreen }
    finish { diffuse D }
}
