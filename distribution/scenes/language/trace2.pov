// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence of Vision Raytracer Scene Description File
// File: trace2.pov
// Last updated: 8/25/01
// Author: Chris Huff
// Description: pins on a height field...animated.
//
// -w320 -h240
// -w800 -h600 +a0.3
//
//*******************************************

#version 3.6;

#declare GridXRes = 16;
#declare GridZRes = 16;
// Total # of pins will be GridXRes*GridZRes...be careful
//not to use too high of a number.
#declare PinHeight = 0.25;

#include "functions.inc"
#include "math.inc"
#include "consts.inc"
#include "colors.inc"

//-------------------------------------------

global_settings {
  assumed_gamma 1.0
}

#default {finish {ambient 0 diffuse 1}}

#declare CamLoc = < 3, 4,-5>;

camera {
	location CamLoc
        right     x*image_width/image_height
	look_at < 0, 0, 0>
	angle 24
}

light_source {CamLoc color White*0.35}
light_source {<-50, 150,-75> color White}
//-------------------------------------------

#declare Land =
isosurface {
	function {y + f_noise3d(x, y+clock*2, z)}
	threshold 0
	max_gradient 1.75
	contained_by {box {<-1.1,-1,-1.1>, < 1.1, 1, 1.1>}}
	translate y*0.75
}

union {
	#declare J = 0;
	#while(J < GridXRes)
		#declare K = 0;
		#while(K < GridZRes)
			#declare stPt = <2*J/(GridXRes-1) - 1, 10, 2*K/(GridZRes-1) - 1>;
			#declare iNorm = y;
			#declare iPt = trace(Land, stPt, -y, iNorm);
			#if(!VZero(iNorm))
				cylinder {iPt, iPt + iNorm*PinHeight, 0.01}
			#end
			#declare K = K + 1;
		#end
		#declare J = J + 1;
	#end
	texture {
		pigment {color Green}
		finish {
			specular 1
		}
	}
}

object {Land
	texture {
		pigment {color White}
		finish {
			specular 1
		}
	}
}
//*******************************************

