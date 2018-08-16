
#version 3.7;
global_settings{ assumed_gamma 1.0 }

camera { 
	edwards
	location <0,0,-1000>
	direction 1000*z
		right x
		up y
}

#include "scene.inc"
/*
+Iedwards.pov
+H600
+W1190
+A0.001
*/
