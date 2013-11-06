// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence of Vision Raytracer Scene Description File
// File: hollow2.pov
// Author: Dieter Bayer
// Description:
// This scene shows the effect of solid vs. hollow objects.
//
// Both spheres in this scene are enclosed by a media container object.
//
// The left sphere is solid, i.e. it's not filled with the media.
// The right sphere is hollow, i.e. it's filled with the media.
//
// Note that you can see some discontinuities in the media of the
// right sphere. This is caused be the refraction. After the rays
// are bent they travel through the media on a different path
// resulting in a different result than that for an unbent ray,
// which would show no discontinuities.
//
// -w320 -h240
// -w800 -h600 +a0.3
//
//*******************************************

#version 3.6;

global_settings {
  assumed_gamma 1.0
  max_trace_level 5
}

#include "colors.inc"

camera {
	location  <0, 20,-100>
	direction <0, 0, 1>
}

//
// Put down the beloved famous raytrace blue/green checkered floor
//

plane {y, -10
	pigment {
		checker Blue, Green
		scale 20
	}
	finish {
		ambient 0.2
		diffuse 0.8
	}
	hollow
}

//
// Use beloved famous raytrace blue/green checkered wall
//

plane {z, 50
	pigment {
		checker Blue, Green
		scale 20
	}
	finish {
		ambient 0.2
		diffuse 0.8
	}
	hollow
}

//
// Declare media.
//

#declare Media =
media {
	emission 0.05
	intervals 1
	samples 5
	method 3
	density {
		spherical
		ramp_wave
		turbulence 0.1
		color_map {
			[0.0 color rgb <0, 0, 0>]
			[0.1 color rgb <1, 0, 0>]
			[1.0 color rgb <1, 1, 0>]
		}
	}
}

//
// Solid, translucent sphere enclosed by media.
//

sphere {<-29, 20, 0>, 25
	pigment { rgbt <1, 1, 1, 0.9> }
	finish {
		ambient 0.0
		diffuse 0.0
		phong 1.0
		phong_size 200
	}
	interior { ior 1.1 }
	hollow no
}

sphere {< 0, 0, 0>, 1
	pigment { color rgbf <1, 1, 1, 1> }
	finish { ambient 0 diffuse 0 }
	interior { media { Media } }
	scale 28
	translate <-29, 20, 0>
	hollow
}

//
// Hollow, translucent sphere enclosed by media.
//

sphere {<29, 20, 0>, 25
	pigment { rgbt <1, 1, 1, 0.9> }
	finish {
		ambient 0.0
		diffuse 0.0
		phong 1.0
		phong_size 200
	}
	interior { ior 1.1 }
	hollow yes
}

sphere {< 0, 0, 0>, 1
	pigment { color rgbf <1, 1, 1, 1> }
	finish { ambient 0 diffuse 0 }
	interior { media { Media } }
	scale 28
	translate <29, 20, 0>
	hollow
}

//
// Cast some light.
//

light_source {<500, 600, -200>, color White}

