// POV-Ray 3.1 scene file

global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"
#include "stones.inc"

camera {
   location <0, 0, -130>
   direction z*8
   right x*1
}

light_source {<500, 500, -1000> color White }
background { color Gray30 }

#declare Test =
intersection {
    intersection {
        object { Cube scale <1, 1, 0.95> }
        object { Disk_X scale 1.15 }
    }
    object { Disk_Y scale 1.15 }
    scale <1, 1.5, 1>
}

#default {
    finish {
        specular 0.35
        roughness 0.005
    }
}

union {
    object { Test texture { T_Stone1 } translate <-4.2, 6.2, 0> }
    object { Test texture { T_Stone2 } translate <-2.1, 6.2, 0> }
    object { Test texture { T_Stone3 } translate < 0.0, 6.2, 0> }
    object { Test texture { T_Stone4 } translate < 2.1, 6.2, 0> }
    object { Test texture { T_Stone5 } translate < 4.2, 6.2, 0> }
}

union {
    object { Test texture { T_Stone6 } translate <-4.2, 3.1, 0> }
    object { Test texture { T_Stone7 } translate <-2.1, 3.1, 0> }
    object { Test texture { T_Stone8 } translate < 0.0, 3.1, 0> }
    object { Test texture { T_Stone9 } translate < 2.1, 3.1, 0> }
    object { Test texture { T_Stone10} translate < 4.2, 3.1, 0> }
}

union {
    object { Test texture { T_Stone11 } translate <-4.2, 0.0, 0> }
    object { Test texture { T_Stone12 } translate <-2.1, 0.0, 0> }
    object { Test texture { T_Stone13 } translate < 0.0, 0.0, 0> }
    object { Test texture { T_Stone14 } translate < 2.1, 0.0, 0> }
    object { Test texture { T_Stone15}  translate < 4.2, 0.0, 0> }
}

union {
    object { Test texture { T_Stone16 } translate <-4.2,-3.1, 0> }
    object { Test texture { T_Stone17 } translate <-2.1,-3.1, 0> }
    object { Test texture { T_Stone18 } translate < 0.0,-3.1, 0> }
    object { Test texture { T_Stone19 } translate < 2.1,-3.1, 0> }
    object { Test texture { T_Stone20}  translate < 4.2,-3.1, 0> }
}

union {
    object { Test texture { T_Stone21 } translate <-4.2,-6.1, 0> }
    object { Test texture { T_Stone22 } translate <-2.1,-6.1, 0> }
    object { Test texture { T_Stone23 } translate < 0.0,-6.1, 0> }
    object { Test texture { T_Stone24 } translate < 2.1,-6.1, 0> }
    object { Test texture { T_Stone25}  translate < 4.2,-6.1, 0> }
}

