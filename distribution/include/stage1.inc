// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

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

