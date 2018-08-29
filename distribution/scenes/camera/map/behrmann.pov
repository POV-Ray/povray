
#version 3.7;
global_settings{ assumed_gamma 1.0 }

camera { 
	behrmann
	location <0,0,-1000>
	direction 1000*z
		right x
		up y
}

#include "scene.inc"
/*
+Ibehrmann.pov
+H600
+W1414
+A0.001
*/
