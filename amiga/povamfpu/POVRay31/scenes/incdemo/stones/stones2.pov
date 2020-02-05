// POV-Ray 3.1 scene file

global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"
#include "stones.inc"

camera {
   location <0, 0, -125>
   direction z*9.75
   right x*1
}

light_source {<500, 500, -1000> color White * 1.5}
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
    object { Test texture { T_Stone25 } translate <-4.2, 4.7, 0> }
    object { Test texture { T_Stone26 } translate <-2.1, 4.7, 0> }
    object { Test texture { T_Stone27 } translate < 0.0, 4.7, 0> }
    object { Test texture { T_Stone28 } translate < 2.1, 4.7, 0> }
    object { Test texture { T_Stone29 } translate < 4.2, 4.7, 0> }
}

union {
    object { Test texture { T_Stone30 } translate <-4.2, 1.6, 0> }
    object { Test texture { T_Stone31 } translate <-2.1, 1.6, 0> }
    object { Test texture { T_Stone32 } translate < 0.0, 1.6, 0> }
    object { Test texture { T_Stone33 } translate < 2.1, 1.6, 0> }
    object { Test texture { T_Stone34 } translate < 4.2, 1.6, 0> }
}

union {
    object { Test texture { T_Stone35 } translate <-4.2,-1.6, 0> }
    object { Test texture { T_Stone36 } translate <-2.1,-1.6, 0> }
    object { Test texture { T_Stone37 } translate < 0.0,-1.6, 0> }
    object { Test texture { T_Stone38 } translate < 2.1,-1.6, 0> }
    object { Test texture { T_Stone39 } translate < 4.2,-1.6, 0> }
}

union {
    object { Test texture { T_Stone40 } translate <-4.2,-4.7, 0> }
    object { Test texture { T_Stone41 } translate <-2.1,-4.7, 0> }
    object { Test texture { T_Stone42 } translate < 0.0,-4.7, 0> }
    object { Test texture { T_Stone43 } translate < 2.1,-4.7, 0> }
    object { Test texture { T_Stone44 } translate < 4.2,-4.7, 0> }
}

