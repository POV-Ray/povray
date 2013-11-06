// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Raytracer sample file.
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
   location <0, 0, -130>
   direction z*8
   right x*0.8
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
    object { Test texture { T_Stone21 } translate <-4.2,-6.2, 0> }
    object { Test texture { T_Stone22 } translate <-2.1,-6.2, 0> }
    object { Test texture { T_Stone23 } translate < 0.0,-6.2, 0> }
    object { Test texture { T_Stone24 } translate < 2.1,-6.2, 0> }
    object { Test texture { T_Stone25}  translate < 4.2,-6.2, 0> }
}

