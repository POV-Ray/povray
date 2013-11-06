// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence of Vision Raytracer Scene Description File
// File: galaxy.pov
// Author:
// Description: Density_file pattern example
//
// -w320 -h240
// -w800 -h600 +a0.3
//
//*******************************************

#version 3.6;
global_settings {assumed_gamma 1.0}

camera { angle 35 // direction z*2 
         location <0,0,-10>
       }

box {0, 1
	texture {
		pigment {rgbt 1}
	}
	interior {
		media {
			emission 1
			scattering {1, 0.1}
			intervals 1
			samples 5
			method 3
			density {
				density_file df3 "spiral.df3" interpolate 1
				color_map {
					[0    rgb 0]
					[0.1  rgb <0.5,0.5,0.7>]
					[0.5  rgb <1.0,0.5,0.7>]
					[0.7  rgb <1.0,1.0,0.7>]
					[1    rgb 1]
				}
			}
		}
	}
	hollow
	translate -0.5
	scale 5*<1,1,0.1>
	rotate <60,30,0>
}

light_source {<500,500,-500> rgb 1}
