// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.
//
// Persistence Of Vision Ray Tracer ('POV-Ray') sample file.
//
// Onion pattern example onion.pov.
//
// -w800 -h600 +a0.3

#version 3.7;
global_settings { assumed_gamma 1.0 }
#default { finish { ambient 0.006 diffuse 0.456 } }

#declare Black = srgb <0,0,0>;
#declare White = srgb <1,1,1>;

#declare T1 = texture {
    pigment {
        onion
        color_map {
            [0.00 Black]
            [0.05 Black]
            [0.90 White]
            [1.00 White]
        }
        scale 0.24
    }
}

#declare T2 = texture {
    pigment { White }
    normal {
        onion 3.6
        scale 0.24
    }
    finish { phong 1 phong_size 400 reflection{ 0.1 } }
}

#include "pignorm.inc"
