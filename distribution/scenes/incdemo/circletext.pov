// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Raytracer Scene Description File
// File: circletext.pov
// Author: Ron Parker
// Description:
// A demonstration of the Circle_Text(), Align_Object(), and Center_Object macros.
//
// -w320 -h240
// -w800 -h600 +a0.3
//
//*******************************************
#version 3.7;
global_settings { assumed_gamma 2.2 }

#include "shapes.inc"

camera{ location -3*z 
        right     x*image_width/image_height
        look_at 0
      }
light_source {-20*z rgb 1 shadowless}

background {rgb 1}

cylinder {<0,-2,.01>,<0,2,.01>,.01 pigment {color rgb<.7,1,.7>}}
cylinder {<0,-2,.01>,<0,2,.01>,.01 pigment {color rgb<.7,.7,1>} rotate 45*z}
cylinder {<0,-2,.01>,<0,2,.01>,.01 pigment {color rgb<.7,.7,1>} rotate -45*z}

object {
	Circle_Text(
		"cyrvetic.ttf",
		"Centered text, not inverted",
		.2, 0.0, .01, 1.1, false, Align_Center, 90
	)
	pigment {color green 1}
}

object {
	Circle_Text(
		"cyrvetic.ttf",
		"Centered text, inverted",
		.2, 0.0, .01, 1.1, true, Align_Center, -90
	)
	pigment {color green 1}
}

object {
	Circle_Text(
		"cyrvetic.ttf",
		"left text",
		.15, 0.0, .01, 1.3, true, Align_Left, -135
	)
	pigment {color blue 1}
}

object {
	Circle_Text(
		"cyrvetic.ttf",
		"right text",
		.15, 0.0, .01, 1.3, true, Align_Right, -45
	)
	pigment {color blue 1}
}

object {
	Align_Object(text {ttf "cyrvetic.ttf", "left aligned", .01, 0}, -x, 0)
	scale <.15,.2,1>
	translate .1*y
	pigment {color red 1}
}

object {
	Center_Object(text {ttf "cyrvetic.ttf", "center aligned", .01, 0}, x)
	scale <.15,.2,1>
	translate -.1*y
	pigment {color red 1}
}

object {
	Align_Object(text {ttf "cyrvetic.ttf", "right aligned", .01, 0}, x, 0)
	scale <.15,.2,1>
	translate -.3*y
	pigment {color red 1}
}
