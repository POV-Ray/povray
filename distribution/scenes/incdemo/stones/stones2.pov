// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.6;
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

