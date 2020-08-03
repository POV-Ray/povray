#version 3.7;
global_settings {assumed_gamma 1.0}
#default{ finish{ ambient 0.1 diffuse 0.9 }} 

#include "colors.inc"

light_source { <100,1000,-1000>, White}

camera { location <0,-1,-16> 
         right    x*image_width/image_height
         angle 35 
         look_at <0,0,0>
       }

plane{-z,-10  pigment{checker color rgb<1,1,1>*0.8 color rgb<1,1,1>} }

#declare CSG= union{
 sphere{ 0,1 texture { pigment{ color red 1 } } }
 box{ -0.8,0.8 texture { pigment{ color green 1 } } }
 cylinder { -z,z, 0.3 texture { pigment { color blue 1 } } }
}

object {CSG translate y}
#local Counter=0;
#while(Counter<children(CSG))
object{ child(CSG, Counter) translate (3*Counter-3)*x-2*y }
#local Counter=Counter+1;
#end
