
#version 3.7;
global_settings{ assumed_gamma 1.0 }

camera { 
	cube
	location <0,0,-1000>
	direction 1000*z
		right x
		up y
}

#include "scene.inc"
/*
+Icube.pov
+H640
+W1600
+A0.001
*/
