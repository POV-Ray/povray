// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Recursive cluster of mirrored spheres
// After a classic, Sphereflake, by Eric Haines
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.6;

global_settings {
  assumed_gamma 2.2
  max_trace_level 20
}

#include "colors.inc"

//-------------------------------------------------------------------------
// This scene uses a non-standard camera set-up. 
// (See CAMERA in the included documentation for details.) 
// If you are new to POV-Ray, you might want to try a different demo scene.
//-------------------------------------------------------------------------
camera { // This is an unusual camera since it is a converted file
   location <2.31, 1.43, 1.65>
   up <0, 0, 1>          // The Z axis is up and
   right  -x*image_width/image_height
   direction <0, -1.60746, 0> // Field of view 45 degrees
   sky <0, 0, 1>
   look_at <0, 0, -0.1>
}

sphere { <0, 0, 0>, 10000
   hollow on
   pigment { SkyBlue }
   finish { ambient 1 diffuse .5}
}

light_source { <4, 3, 2 > color Gray30 }
light_source { <1, -4, 4> color Gray30 }
light_source { <-3, 1, 5> color Gray30 }

plane { z, -0.5
   pigment { color red 1.0 green 0.75 blue 0.33 }
   finish { diffuse 1 } //ambient <0.15,0.1,0 .045>}
} // ambient .15 .1 .045

/*
union {
   sphere { <0.7, 0, 0>, 0.2 texture { pigment { Red }   finish { diffuse 0.7} } }
   sphere { <0, 0.7, 0>, 0.2 texture { pigment { Green } finish { diffuse 0.7} } }
   sphere { <0, 0, 0.7>, 0.2 texture { pigment { Blue }  finish { diffuse 0.7} } }
}
*/

#declare Texture =
texture {
   finish {
      ambient 0.03
      diffuse 1
      reflection 0.4
      phong 1
      phong_size 3
   }
   pigment { color red 0.5 green 0.45 blue 0.35 }
}

union {
   sphere { <0, 0, 0>, 0.5
      texture {
         Texture
          normal {
             bumps 0.4
             sine_wave
             scale 0.025
          }
      }
   }

   union {
      sphere { <0.272166, 0.272166, 0.544331>, 0.166667  }
      sphere { <0.643951, 0.172546, 0>, 0.166667  }
      sphere { <0.172546, 0.643951, 0>, 0.166667 }
      sphere { <-0.371785, 0.0996195, 0.544331>, 0.166667  }
      sphere { <-0.471405, 0.471405, 0>, 0.166667  }
      sphere { <-0.643951, -0.172546, 0>, 0.166667  }
      sphere { <0.0996195, -0.371785, 0.544331>, 0.166667  }
      sphere { <-0.172546, -0.643951, 0>, 0.166667  }
      sphere { <0.471405, -0.471405, 0>, 0.166667  }
      texture { Texture }
   }
}

