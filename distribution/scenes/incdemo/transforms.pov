// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Raytracer Scene Description File
// File: transforms.pov
// Last updated: 8/19/01
// Author: Chris Huff
// Description:
//
// -w320 -h240
// -w800 -h600 +a0.3
//
//*******************************************

#version 3.7;
global_settings { assumed_gamma 1.0 }

#include "stdinc.inc"

#default {finish {ambient 0 diffuse 1}}

#declare CamLoc = < 5, 5,-5>;

camera {
	up y  
	right x*image_width/image_height
	location CamLoc
	look_at < 0, 0, 0>
	angle 45
}

light_source {CamLoc color White*0.35}
light_source {<-50, 150,-75> color White}

box {<-10,-6,-10>, < 10,-5, 10>
	texture {
		pigment {checker color rgb < 0.15, 0.75, 0.15>, color White}
		finish {ambient 0 diffuse 1}
	}
}
box {<-10,-10, 5>, < 10, 10, 6>
	texture {
		pigment {checker color rgb < 0.15, 0.15, 0.75>, color White}
		finish {ambient 0 diffuse 1}
	}
}
box {<-6,-10,-10>, <-5, 10, 10>
	texture {
		pigment {checker color rgb < 0.75, 0.15, 0.15>, color White}
		finish {ambient 0 diffuse 1}
	}
}
cylinder {o, x*10, 0.1 texture {pigment {color Red}}}
cylinder {o, y*10, 0.1 texture {pigment {color Green}}}
cylinder {o, z*10, 0.1 texture {pigment {color Blue}}}
//-------------------------------------------

#declare BaseObj =
union {
	box {< 0, 0, 0>, < 1.5, 1.5, 1.5> texture {pigment {color White}}}
	sphere {< 0, 0, 0>, 0.5 texture {pigment {color White}}}

	sphere {< 1, 0, 0>, 0.5 texture {pigment {color rgb < 1, 0.5, 0.5>}}}
	cylinder {o, x*10, 0.075 texture {pigment {color rgb < 1, 0.5, 0.5>}}}

	sphere {< 0, 1, 0>, 0.5 texture {pigment {color rgb < 0.5, 1, 0.5>}}}
	cylinder {o, y*10, 0.075 texture {pigment {color rgb < 0.5, 1, 0.5>}}}

	sphere {< 0, 0, 1>, 0.5 texture {pigment {color rgb < 0.5, 0.5, 1>}}}
	cylinder {o, z*10, 0.075 texture {pigment {color rgb < 0.5, 0.5, 1>}}}
}

object {BaseObj
//	Shear_Trans(x, vrotate(y,-z*35), vrotate(z,-y*35))
//	Matrix_Trans(x, vrotate(y,-z*35), vrotate(z,-y*35), <1, 1, 1>)

//	Axial_Scale_Trans(< 1, 1, 1>, 1/3)

//	Axis_Rotate_Trans(< 1, 1, 1>, 35)

//	Rotate_Around_Trans(x*45, < 0, 0, 2.5>)

//	#local X = vrotate(x, < 25, 15, 25>);
//	#local Z = vrotate(z, < 25, 15, 25>);
//	Reorient_Trans(X, Z)
//	Point_At_Trans(< 1, 1, 1>)
}

//*******************************************

