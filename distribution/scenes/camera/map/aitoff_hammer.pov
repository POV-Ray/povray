
#version 3.7;
global_settings{ assumed_gamma 1.0 }

camera { 
	aitoff_hammer
	location <0,0,-1000>
	direction 1000*z
		right x
		up y
}

#include "scene.inc"
/*
+Iaitoff_hammer.pov
+H600
+W1200
+A0.001
*/
