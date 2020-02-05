// Persistence Of Vision Raytracer version 3.1 sample file.
#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"

#declare Camera1=
camera {
   location  <0, 0, -60>
   direction <0, 0,  10>
   look_at   <0, 0,   0>
}

camera { Camera1 }

light_source { <1000, 1000, -2000> color White}

plane { z, 1.01 pigment {checker color White color rgb <1,.8,.8>}hollow on }

