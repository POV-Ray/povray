// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.
//
// Persistence Of Vision Ray Tracer ('POV-Ray') sample file.
//
// Granite pattern example granite.pov.
//
// -w800 -h600 +a0.3

#version 3.7;
global_settings { assumed_gamma 1.0 }
#default { finish { ambient 0.006 diffuse 0.456 } }

#declare Black = srgb <0,0,0>;
#declare White = srgb <1,1,1>;

#declare T1= texture {
    pigment {
        granite
        color_map {
            [0.0 Black]
            [0.2 Black]
            [0.8 White]
            [1.0 White]
        }
    }
}

#declare T2 = texture {
    pigment { White }
    normal { granite 2.6 }
    finish { phong 1 phong_size 400 reflection{ 0.1 } }
 }

#include "pignorm.inc"
