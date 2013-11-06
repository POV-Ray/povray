// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
// File by Chris Young
// Demonstrates various new vector math functions.
// Animate this scene with clock values +k0.0 to +k1.0

#version 3.7;

global_settings {
  assumed_gamma 1.0
  }

#include "colors.inc"

#declare Font="cyrvetic.ttf"

// Basic clock runs from 0.0 to 1.0 but we want to move more
//  than that.  Define a scaled version.

#declare Clock360 = 360*clock;
#declare ClockRot = Clock360*z;


#declare Vector_Arrow=
 union{
   cylinder{0,2.5*x,.10}
   cone{2.5*x,.3,3*x,0}
 }

#declare X_axis=
  union{
    object{Vector_Arrow}
    object{Vector_Arrow scale <-1,1,1>}
    scale <1.5, .3, .3>
    pigment{rgb<1,.4,0>}
  }
#declare Y_axis=object{X_axis rotate z*90}
#declare Z_axis=object{X_axis rotate y*90}

#declare A=object{Vector_Arrow pigment{Red}}
#declare B=object{Vector_Arrow rotate ClockRot translate -z/100 pigment{Green}}
#declare A_point   = x;
#declare B_point   = vrotate(x,ClockRot);
#declare A_dot_B   = vdot(A_point,B_point);   //float result
#declare C_point   = vcross(A_point,B_point); //vector result
#if (vlength(C_point) != 0.0)
  #declare C=object{Vector_Arrow rotate -y*90 scale <1,1,C_point.z> pigment{Blue}}
#end

union {
  object{A}
  object{B}
  #ifdef (C)
    object{C}
  #end
  object{X_axis}
  object{Y_axis}
  object{Z_axis}

  rotate <-20,35,0>
  translate <2.5,1,-3.25>
}

text{ttf Font
  concat("A=<",
         str(A_point.x,1,1),",",
         str(A_point.y,1,1),",",
         str(A_point.z,1,1),">"
        ),0.1,0
  pigment{Red}
  translate <-5,3,0>
}
text{ttf Font
  concat("B=<",
         str(B_point.x,1,1),",",
         str(B_point.y,1,1),",",
         str(B_point.z,1,1),">"
        ),0.1,0
  pigment{ Green }
  translate <-5,2,0>
}
text{ttf Font concat("vdot(A,B)=",str(A_dot_B,1,2)),0.1,0 pigment{Magenta*.7} translate <-5,-2,0>}
text{ttf Font "C=vcross(A,B)=", 0.1,0 pigment{Blue} translate <-5,-3,0>}
text{ttf Font
  concat("<",
         str(C_point.x,1,1),",",
         str(C_point.y,1,1),",",
         str(C_point.z,1,1),">"
        ),0.1,0
  pigment{Blue}
  translate <-4,-4,0>
}


camera {
   location  <0, 0, -100>
   right     x*image_width/image_height
   direction <0, 0,  11>
   look_at   <0, 0,   0>
}

light_source { <5000, 10000, -20000> color White*.5}
light_source { <-5000, -10000, -20000> color White*.5}
plane { -z, -.05 pigment {checker color rgb <1,1,1>*1.2 color rgb <1,1,.8>} }

