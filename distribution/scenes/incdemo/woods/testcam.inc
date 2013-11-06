// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Raytracer sample file.
// Camera and default test object definition for the wood_x.pov files.

#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"
#include "woods.inc"

camera {
   location <0, 0, -120>
   angle 5.4 // direction z*10
   right x*image_width/image_height
   look_at<0,1.1,0>
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
