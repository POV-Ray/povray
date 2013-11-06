// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Raytracer sample file.
// FLOAT4.POV
// Animate this scene with clock values +ki0.0 to +kf1.0
// Demonstrate sqrt, pow, degrees & atan2 as well as Pythagorean Theorm
// using a 3-4-5 triangle and some boxes.

#version 3.7;

global_settings { assumed_gamma 1.0 }

#include "colors.inc"

#declare Rad=1/6;
#declare Font="cyrvetic.ttf"

camera {
   location  <0, 0, -140>
   direction <0, 0,  11>
   look_at   <0, 0,   0>
}

light_source { <5000, 10000, -20000> color White}
plane { z, Rad hollow on pigment {checker color rgb <1,1,1>*1.2 color rgb <1,1,.8>}  
        translate<0,-0.2,0> }

#declare A=4;
#declare B=3*clock;
#declare C=sqrt(pow(A,2)+pow(B,2));
#declare Angle_b=atan2(B,A);
#declare b_Degrees=degrees(Angle_b);

union {
  box{0,<A,-A,1>
    pigment {checker rgb<1,1,1>*0.9 , color rgb<1,0.1,0>}
  }

  box{0,<B,B,1>
    pigment {checker rgb<1,1,1>*0.9  , color rgb<0.2,0.1,0.5,>}
    translate x*A
  }
  box{0,<C,C,1>
    pigment {checker rgb<1,1,1>*0.9  , color rgb<1,0.6,0>}
    rotate z*b_Degrees
  }

  intersection{
    box{0,2}
    cylinder{-z,z,2}
    cylinder{-z,z,1.75 inverse}
    translate z*.1
    pigment{Magenta*.7}
  }

  text{ttf Font "A=4",0.1,0 translate <1,-5,0> pigment{color rgb<1,0.1,0>}}
  text{ttf Font concat("B=",str(B,1,2)),0.1,0 translate <4.25,-1.25,0> pigment{color rgb<0.2,0.1,0.5,>}}
  text{ttf Font "C=sqrt(pow(A,2)",0.1,0  translate <-9,2,0> pigment{color rgb<0.1,0.5,0>}}
  text{ttf Font "+pow(B,2))",0.1,0  translate <-7,1,0> pigment{color rgb<0.1,0.5,0>}}
  text{ttf Font concat("C=",str(C,1,2)),0.1,0 pigment{color rgb<1,0.6,0>}
       translate (C+0.3)*y
       rotate z*b_Degrees
      }
  text{ttf Font concat("b=atan2(B,C)=",str(Angle_b,1,2)),0.1,0 translate <-9,-1,0> pigment{Magenta*.7}}
  text{ttf Font concat("degrees(b)=",str(b_Degrees,1,2)),0.1,0 translate <-8.5,-2,0> pigment{Magenta*.7}}

  translate x-y
}
