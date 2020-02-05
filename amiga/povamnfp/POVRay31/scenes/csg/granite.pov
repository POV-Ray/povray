// Persistence Of Vision raytracer version 3.1 sample file.

global_settings { assumed_gamma 2.2 }

#include "colors.inc"           // Standard colors library
#include "shapes.inc"           // Commonly used object shapes
#include "textures.inc"         // LOTS of neat textures.  Lots of NEW textures.

camera {
   location  <0, 3.5, -3.7>
   direction <0, 0,    1>
   up        <0, 1,    0>
   right   <4/3, 0,    0>
   look_at   <0, -0.25,    0>
}

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
      ambient 0.15
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

