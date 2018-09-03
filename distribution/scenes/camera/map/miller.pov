
#version 3.7;
global_settings{ assumed_gamma 1.0 }

camera { 
	miller_cylindrical
	location <0,0,-1000>
	direction 1000*z
		right x
		up y
}

#include "scene.inc"
/*
+Imiller.pov
+H600
+W817
+A0.001
*/
