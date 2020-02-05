// Persistence Of Vision raytracer version 3.1 sample file.

global_settings {
    assumed_gamma 1.0
    number_of_waves 10
}

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"

fog{
    color Gray70
    fog_type 2
    fog_alt 0.35
    fog_offset 0
    distance 1.5
    turbulence <.15, .15, .15>
    omega 0.35
    lambda 1.25
    octaves 5
}


// Camera definition
camera {
  location <4, 1.5, -8>
  direction <0, 0, 1>
  up  <0, 1, 0>
  right <1.3333, 0, 0>
  look_at <-1,  1.5, 0>
}

light_source { <30, 50, -50> color Gray90 }
light_source { <-30, 50, -50> color Violet }

//Floor
plane { y, 0
    pigment{ color MidnightBlue }
    normal { ripples 0.75 frequency 7 scale 1.5}
    finish {
        reflection 0.15
        ambient 0.3
    }
}

// Sky
sky_sphere { pigment {color MidnightBlue}}

#declare Brick_Texture = texture {
    pigment {
        brick Gray70, Wheat
    }
    normal {
        brick 0.75
    }
    finish {
        crand 0.003
        diffuse 0.6 ambient 0.20
    }
    scale <0.1, 0.1, 0.05>
    translate <0.25, 0.30, 0.25>
}

#declare Span = difference {
    object { UnitBox }
    object { Disk_Z scale <0.85, 0.75, 1.5> } // cross-arches
    object { Disk_X scale <1.15, 0.7, 0.7> }  // lengthwise arches
    clipped_by { plane { -y,0 } }
}

#declare Bridge = union {
    object { Span }
    object { Span translate -x*2}
    object { Span translate  x*2}
    object { Span translate -x*4}
    object { Span translate  x*4}
    object { Span translate -x*6}
    object { Span translate  x*6}
    object { Cube scale <12, 0.15, 1.005> translate y*1.2}
    object { Cube scale <12, 0.05, 1.05> translate y*1.2}
    scale <1,1,2>
}

object {Bridge  scale <5.25,4,1>
    texture { Brick_Texture }
    rotate y*90
    translate <-1, 0, 25>
}

union {
    difference {
        union {
            object { Disk_Y  scale <10, 23, 10> }
            object { Disk_Y  scale <11, 1, 11> translate <0, 23, 0>}
        }
        object { Disk_Z scale <2.5, 10, 12> }
    }
    union {
    box { <-100, 0, 10> <10, 28, 20> }
    box { <-110, 28, 08> <10, 29, 22> }
    }

    translate <0, 0, 45>
    texture { Brick_Texture scale 2 }
}
