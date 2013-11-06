// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence of Vision Raytracer Scene Description File
// File: hollow1.pov
// Author: Dieter Bayer
// Description:
// This scene shows the effect of solid vs. hollow objects.
//
// The left sphere is solid, i.e. it's not filled with fog.
// The right sphere is hollow, i.e. it's filled with fog.
//
// The left sphere doesn't attenuate light passing through it.
// Thus the background seen through it and its shadow is lighter
// than for the right sphere.
//
// The right sphere attenuates light in the same way like fog does
// because it is filled with fog. Thus the background seen through
// it and its shadow is darker.
//
// -w320 -h240
// -w800 -h600 +a0.3
//
//*******************************************

#version 3.6;

global_settings {
  assumed_gamma 1.0
}

#include "colors.inc"

camera {
	location <0, 20, -100>
}

//
// Standard fog
//

fog {
	color Gray50
	distance 80
}

//
// Put down the beloved famous raytrace green/yellow checkered floor
//

plane {y, -10
	pigment {
		checker Yellow, Green
		scale 20
	}
	finish {
		ambient 0.2
		diffuse 0.8
	}
	hollow
}

//
// Use beloved famous raytrace green/yellow checkered wall
//

plane {z, 50
	pigment {
		checker Yellow, Green
		scale 20
	}
	finish {
		ambient 0.2
		diffuse 0.8
	}
	hollow
}

//
// Solid, translucent sphere
//

sphere {<-26, 20, 0>, 25
	pigment { rgbt<1, 1, 1, 0.8> }
	finish {
		ambient 0.0
		diffuse 0.0
		phong 1.0
		phong_size 200
	}
	interior { ior 1.05 }
	hollow no
}

//
// Hollow, translucent sphere
//

sphere {<26, 20, 0>, 25
  pigment {rgbt <1, 1, 1, 0.8>}
  finish {
    ambient 0.0
    diffuse 0.0
    phong 1.0
    phong_size 200
  }
  interior { ior 1.05 }
  hollow yes
}

//
// Due to the atmospheric attenuation and the large distance to
// the light source it has to be very bright.
//

light_source {<500, 600, -200>, color 40000*White
	media_attenuation on
}

