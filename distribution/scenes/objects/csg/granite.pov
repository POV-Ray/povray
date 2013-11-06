// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3
#version  3.7;
global_settings { assumed_gamma 1.9 }

#include "colors.inc"           // Standard colors library
#include "shapes.inc"           // Commonly used object shapes
#include "textures.inc"         // LOTS of neat textures.  Lots of NEW textures.

camera {
   location  <0, 3.5, -3.7>
   angle 69 
   right     x*image_width/image_height
   look_at   <0, -0.25,    0>
}

background{ color rgb<1,1,1>*0.15 } 

// Light source

light_source {<-30, 11, +20>  color White  }
light_source {< 31, 12, -20>  color White  }
light_source {< 32, 11, -20>  color LightGray }

#include "rdgranit.map"
#declare Pink_Gran_Texture =
texture {
   pigment {
      granite
      color_map { M_RedGranite }
      scale 0.4
      }
   finish {
      specular 0.75
      roughness 0.0085
      ambient 0.05
      reflection 0.2
   }
}

union {
   sphere {<0, 0, 0>, 1.75}
   difference {
      object {UnitBox scale 1.5}
      // Clip some sqr holes in the box to make a 3D box frame
      object {UnitBox scale <1.51, 1.25, 1.25> }   // "clip" x
      object {UnitBox scale <1.25, 1.51, 1.25> }   // "clip" y
      object {UnitBox scale <1.25, 1.25, 1.51> }   // "clip" z
   }
   texture { Pink_Gran_Texture scale 0.25 }
   rotate y*45
}

