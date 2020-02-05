// Persistence Of Vision raytracer version 3.1 sample file.
// CRATER.POV
// Render CRAT_DAT.POV to create CRAT_DAT.PNG and then render CRATER.POV


global_settings { assumed_gamma 2.2 }

#include "colors.inc"

camera{location <0,8,-20> direction z*5 look_at 0}

light_source{<1000,1000,-1000> White}

height_field { 
  png "crat_dat.png"
  smooth
  pigment {White}
  translate <-.5, 0, -.5>
  scale <17, 1.75, 17>
}  
