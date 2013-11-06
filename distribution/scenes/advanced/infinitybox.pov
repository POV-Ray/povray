// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Raytracer Scene Description File
// File: InfinityBox.pov
// Author: Chris Huff
//
// -w320 -h240
// -w800 -h600 +a0.3
//*******************************************

#version 3.6;
global_settings {
   assumed_gamma 1.0
   max_trace_level 25
}

#default {finish {ambient 0}}

#include "colors.inc"

//-------------------------------------------

#declare CamLoc = < 1.5, 3.5,-7>;

camera {
	location CamLoc
	up y*image_height/image_width  right x
	look_at < 0, 1, 0>
	angle 35
}
light_source {CamLoc, color rgb 0.3}
light_source {<-50, 150,-180>, color White}
light_source {< 20, 25, 75>, color rgb 0.5}
//*******************************************
box {<-50,-1,-50>, < 50, 0, 50>
	texture {
		pigment {checker color rgb < 0.15, 0.1, 0.5>, color White}
	}
}


#declare EdgeRad = 0.05;
union {
	box {<-1, 0,-1>, < 1, 2, 1>
		texture {
			pigment {color White filter 1}
		}
		interior_texture {
			pigment {color rgb 0}
			finish {reflection 0.8}
		}
		no_shadow
	}

	cylinder {<-1, 0,-1>, < 1, 0,-1>, EdgeRad}
	cylinder {< 1, 0,-1>, < 1, 0, 1>, EdgeRad}
	cylinder {< 1, 0, 1>, <-1, 0, 1>, EdgeRad}
	cylinder {<-1, 0, 1>, <-1, 0,-1>, EdgeRad}

	cylinder {<-1, 2,-1>, < 1, 2,-1>, EdgeRad}
	cylinder {< 1, 2,-1>, < 1, 2, 1>, EdgeRad}
	cylinder {< 1, 2, 1>, <-1, 2, 1>, EdgeRad}
	cylinder {<-1, 2, 1>, <-1, 2,-1>, EdgeRad}

	cylinder {<-1, 0,-1>, <-1, 2,-1>, EdgeRad}
	cylinder {< 1, 0,-1>, < 1, 2,-1>, EdgeRad}
	cylinder {< 1, 0, 1>, < 1, 2, 1>, EdgeRad}
	cylinder {<-1, 0, 1>, <-1, 2, 1>, EdgeRad}

	sphere {<-1, 0,-1>, EdgeRad*2}
	sphere {< 1, 0,-1>, EdgeRad*2}
	sphere {< 1, 0, 1>, EdgeRad*2}
	sphere {<-1, 0, 1>, EdgeRad*2}

	sphere {<-1, 2,-1>, EdgeRad*2}
	sphere {< 1, 2,-1>, EdgeRad*2}
	sphere {< 1, 2, 1>, EdgeRad*2}
	sphere {<-1, 2, 1>, EdgeRad*2}

	texture {
		pigment {color White}
	}
	translate y*EdgeRad
}


sphere {< 0.3, 1.5, 0.4>, 0.35
	texture {
		pigment {color rgb < 0.5, 0, 0>}
		finish {
			reflection {0.5 metallic}
			diffuse 0.4

			brilliance 2
			specular 1 roughness 0.01
			metallic
		}
	}
}

sphere {<-0.5, 0.5,-0.2>, 0.1
	texture {
		pigment {color rgb < 1, 1, 0>}
		finish {
			reflection {0.5 metallic}
			diffuse 0.4

			brilliance 2
			specular 1 roughness 0.01
			metallic
		}
	}
}
box {<-0.3,-0.1,-0.2>, < 0.5, 0.1, 0.2>
	texture {
		pigment {color rgb < 0, 0.8, 0>}
		finish {
			reflection {0.5 metallic}
			diffuse 0.4

			brilliance 2
			specular 1 roughness 0.01
			metallic
		}
	}
	translate < 0.5, 0.3,-0.5>
}

//*******************************************


//-------------------------------------------
