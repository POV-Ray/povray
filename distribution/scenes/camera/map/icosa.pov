
#version 3.7;
global_settings{ assumed_gamma 1.0 }

camera { 
	icosa
	location <0,0,-1000>
	direction 1000*z
		right x
		up y
rotate 93.0*y
}

#include "scene.inc"
/*
+Iicosa.pov
+H600
+W1270
+A0.001
*/
