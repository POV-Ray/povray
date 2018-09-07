
#version 3.7;
global_settings{ assumed_gamma 1.0 }

camera { 
	plate_carree
	location <0,0,-1000>
	direction 1000*z
		right x
		up y
}

#include "scene.inc"
/*
+Iplatecarree.pov
+H942
+W1884
+A0.001
*/
