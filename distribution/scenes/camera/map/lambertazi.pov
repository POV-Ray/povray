
#version 3.7;
global_settings{ assumed_gamma 1.0 }

camera { 
	lambert_azimuthal
	location <0,0,-1000>
	direction 1000*z
		right x
		up y
}

#include "scene.inc"
/*
+Ilambertazi.pov
+H1024
+W1024
+A0.001
*/
