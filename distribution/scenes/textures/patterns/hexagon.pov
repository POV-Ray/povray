// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.
//
// Persistence Of Vision Ray Tracer ('POV-Ray') sample file.
//
// Hexagon pattern example hexagon.pov.
//
// -w800 -h600 +a0.3

#version 3.7;
global_settings { assumed_gamma 1.0 }
#default { finish { ambient 0.006 diffuse 0.456 } }

#declare Black  = srgb <0,0,0>;
#declare White  = srgb <1,1,1>;
#declare Gray50 = srgb <0.5,0.5,0.5>;

#declare T1 = texture {
    pigment {
        hexagon color Black color White color Gray50
        scale 0.124
        rotate <90,0,0>
    }
}

#declare T2 = texture {
    pigment { White }
    normal {
        hexagon 1.6
        scale 0.124
        rotate <90,0,0>
    }
    finish { phong 1 phong_size 400 reflection{ 0.1 } }
}

#include "pignorm.inc"
