#version 3.7;
global_settings{assumed_gamma 1.0}

#include "colors.inc"
#include "woods.inc"
#include "stones.inc"

camera { location <0,10,-8> direction z
 up y
 right image_width/image_height*x 
 look_at y 
angle 27
}

#declare My_pigment1=
  pigment { checker
  pigment { Aquamarine }
  pigment { Yellow }
}
#declare My_pigment2=
  pigment { checker
  pigment { Cyan }
  pigment { Red }
}
#declare My_pigment3=
  pigment { checker
  pigment { Black }
  pigment { White }
}
#declare My_pigment4=
  pigment { checker
  pigment { Blue}
  pigment { Magenta }
}

#declare My_pigmentA=pigment {
planar 
pigment_map
{
[0.0 My_pigment1]
[0.05 My_pigment1]
[0.25 My_pigment2]
[1.0 My_pigment2]
}
rotate 90*z
}
#declare My_pigmentB=pigment {
planar 
pigment_map
{
[0.0 My_pigment3]
[0.05 My_pigment3]
[0.25 My_pigment4]
[1.0 My_pigment4]
}
rotate 90*z
}
#declare My_pigmentC=pigment {
planar 
pigment_map
{
[0.0 My_pigment4]
[0.05 My_pigment4]
[0.25 My_pigment1]
[1.0 My_pigment1]
}
rotate 90*z
}
#declare My_pigmentD=pigment {
planar 
pigment_map
{
[0.0 My_pigment3]
[0.05 My_pigment3]
[0.25 My_pigment2]
[1.0 My_pigment2]
}
rotate 90*z
}
#declare My_pigmenth=pigment{
planar
pigment_map
{
[0.05 My_pigmentA]
[0.35 My_pigmentB]
}
rotate 90*x
}

#declare My_pigmentl=pigment{
planar
pigment_map
{
[0.05 My_pigmentC]
[0.35 My_pigmentD]
}
rotate 90*x
scale 1/2
}

#declare My_pigmentplus=pigment{
planar 
pigment_map
{
[0.5 My_pigmenth]
[0.5 My_pigmentl]
}
}
#declare My_pigmentmois=pigment
{
checker
pigment{ Orange }
pigment{ Green }
}
#declare Split=plane {z,-2};
#declare My_pigment=pigment{ object {  Split
pigment{My_pigmentplus}
pigment{My_pigmentmois}
}
}


//background { Gray50 }
light_source{ <10,10,-10> color rgb <1,1,1> }
light_source{ <-10,10,-10> color rgb <1,1,1> }
light_source{ <0,10,0> color rgb <1,1,1> }

box { <0,-1,-1>,<0.45,1,1> texture { My_pigment  scale <1/3,1/5,1/7>}
rotate 90*y
translate <1.5,1,0>
 }

#declare Off = -0.25*z;
union {
 cylinder { 0, 2*x, 0.01 texture { pigment { Red }}}
 //cylinder { 0, 2*y, 0.01 texture { pigment { Green }}}
 cylinder { Off, Off+2*x, 0.01 texture { pigment { Blue }}}
rotate 90*y
translate <1.5,1,0>
 }
 union{
 cylinder { 0, 2*x, 0.01 translate 0.55*x texture { pigment { Red }}}
 //cylinder { 0, 2*y, 0.01 translate 0.55*x texture { pigment { Green }}}
 cylinder { 0, 2*x, 0.01 rotate 45*y texture { pigment { Blue }}}
rotate 90*y
translate <-1.5,0.45,0>
 }

difference{
torus { 0.55,0.45 }
box{<0,-1,0>,<1,1,-1>}
 texture { pigment { My_pigment 
  warp { torus <0,0,0>,0.55,<3,5,7> }

}
rotate 90*y
}
translate <-1.5,0.45,0>
}


