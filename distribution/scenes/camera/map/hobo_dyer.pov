
#version 3.7;
global_settings{ assumed_gamma 1.0 }

camera { 
	hobo_dyer
	location <0,0,-1000>
	direction 1000*z
		right x
		up y
}

#include "scene.inc"
/*
+Ihobo_dyer.pov
+H600
+W1186
+A0.001
*/
