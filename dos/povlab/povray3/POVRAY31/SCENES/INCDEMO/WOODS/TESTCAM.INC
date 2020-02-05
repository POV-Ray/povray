// POV-Ray 3.1 scene file
// Camera and default test object definition for the wood_x.pov files.

global_settings { assumed_gamma 2.2 }
#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"
#include "woods.inc"

camera {
   location <0, 0, -120>
   direction z*10
   right x*1
}

light_source {<500, 500, -1000> color White}
background { color Gray30 }

#declare Test =
intersection {
    intersection {
        object { Cube scale <1, 1, 0.95> }
        object { Disk_X scale 1.15 }
    }
    object { Disk_Y scale 1.15 }
}

#default {
    finish {
        specular 0.5
        roughness 0.0025
    }
}
#declare Angle = 87;   // Rotates the textures on the X axis.
