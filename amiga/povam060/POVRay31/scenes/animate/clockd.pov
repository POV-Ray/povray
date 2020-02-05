// Persistence of Vision Ray Tracer POV-Ray 3.1 Sample Scene
// by Chris Young
// CLOCKD.POV demonstrates basic use of the clock_delta identifier.
// Run this scene with various start & end clock values or frames.  

#include "colors.inc"

light_source { <100,1000,-1000>, White}

camera { location <0,1,-16> direction 2*z look_at <0,1,0>}

union{
 cylinder{0,z,2 pigment{radial color_map{[0.5 Green][0.5 Cyan]} frequency 12 rotate x*90}}
 cone{0,0.25,y*2,0 pigment{Blue} rotate -z*clock*360}
 translate y*3
}

text{ttf "timrom.ttf" concat("clock_delta=",str(clock_delta,8,4)),0.1,0 
 pigment{Red}
 translate -5*x
}

union { 
 plane{y,-2} plane{-z,-10} plane{x,-10}
 pigment{checker Cyan,Yellow}
}

