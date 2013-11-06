// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
// File by Dan Farmer
// Specular values demonstration

#version 3.6;

global_settings {
  assumed_gamma 2.2
  }

#include "colors.inc"
#include "stdcam.inc"

#declare S = clock;

#declare Font="cyrvetic.ttf"
text{ ttf Font
    concat("specular=",str(S,1,1)),0.1,0
    rotate<0,-5,0> 
    scale <1.25, 1.5, 4>
    translate <-3.25, 0.5, -1>
    pigment { rgb <1, 0.5, 0.2> }
    finish { specular S roughness 0.1 }
}

sphere { <-1.0, 0.4, -2.9>, 0.4
    pigment { LimeGreen }
    finish { specular S roughness 0.1 }
}

