// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.
//
// Persistence Of Vision Ray Tracer ('POV-Ray') sample file.
//
// Brick pattern example brick.pov.
//
// -w800 -h600 +a0.3

#version 3.7;
global_settings { assumed_gamma 1.0 }
#default { finish { ambient 0.006 diffuse 0.456 } }

#declare Black = srgb <0,0,0>;
#declare White = srgb <1,1,1>;
#declare Brick = srgb <0.5,0.1,0>;
#declare Calk  = srgb <0.6,0.6,0.6>;

#declare T1 = texture {
    pigment {
        brick color Calk color Brick
        scale 0.1
    }
}

#declare T2 = texture {
    pigment { White }
    normal {
        pigment_pattern {
            brick color White color Black
            scale 0.1
        }
        , 0.5
    }
    finish { phong 0.8 phong_size 200 }
 }

#include "pignorm.inc"
