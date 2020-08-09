#version 3.7;
global_settings{ assumed_gamma 1.0 }

camera {orthographic 
  location <4,4,-10>
  direction z
  up 10*y
  right 10*x*image_width/image_height
}
#include "colors.inc"
background { White }
light_source { -20*z, 1 }
#declare A=<0,3,0>;
#declare B=<6,3,0>;
#declare C=<4.5,1,0>;
#declare D=<7,2,0>;
#declare E=<4.5,6,0>;
#declare F=<7,4,0>;
#declare G=<2,4,0>;
#declare H=<4,6,0>;
#declare I=<4,0,0>;
#declare J=<5,0,0>;
#declare K=<5,8,0>;

polyline
{
  K, J, I, H,
  G, F, E, D,
  C, B, A, K
  range { 1, 2 }
  texture { pigment { color srgb 0.5 }}
}
