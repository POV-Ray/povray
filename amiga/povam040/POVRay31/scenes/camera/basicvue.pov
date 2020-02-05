// Persistence Of Vision raytracer version 3.1 sample file.
// Use copies of this file for starting your own scenes.

global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "textures.inc"

camera {
   location  <0, 3,-10>
   direction <0, 0,  1>
   up        <0, 1,  0>
   right   <4/3, 0,  0>
   look_at   <0, 2, 0>
}

light_source {<10, 20, -30> color White}

sky_sphere { pigment { Blue } }

// Floor
plane { y, 0
   pigment {NeonBlue}
   finish {ambient 0.15 diffuse 0.8}
}

// Sphere object
sphere { <0, 3, 0>, 3
   pigment {Orange}
   finish {
      ambient 0.2
      diffuse 0.7
      phong 1
      phong_size 80
      brilliance 2
   }
}
