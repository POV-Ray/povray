
#version 3.7;
global_settings{ assumed_gamma 1.0 }

camera { 
	gall
	location <0,0,-1000>
	direction 1000*z
		right x
		up y
}

#include "scene.inc"
/*
+Igall.pov
+H600
+W942
+A0.001
*/
