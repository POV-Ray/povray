// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Demonstrates various new float math functions.
// Animate this scene with clock values +ki0.0 to +kf1.0

#version 3.7;

global_settings { assumed_gamma 1.0 }

#declare A = clock;
#declare B = sin(radians(A*360));
#declare C = cos(radians(A*360));
#declare D = min(B,C);
#declare E = max(B,C);

#include "colors.inc"

#declare Rad=1/3;
#declare Fudge=3.5;
#declare TFudge=<-6.25,0,0.2>;
#declare Font="cyrvetic.ttf"

camera {
   location  <0, 0, -120>
   right     x*image_width/image_height
   direction <0, 0,  12>
   look_at   <0, 0,   0>
}

light_source { <5000, 10000, -20000> color White}
plane { z, Rad hollow on pigment {checker color rgb <1,1,1>*1.2 color rgb <1,1,.8> scale <Fudge,1,1>}
        translate<0,-0.2,0> }

union{
 text{ttf Font concat("A=clock=",str(A,0,2)),0.1,0 translate TFudge}
 sphere {<A*Fudge,-0.5,0>,Rad}
 pigment{color rgb<1,0.1,0>} translate 4*y
}

union{
 text{ttf Font concat("B=sin(radians(A*360)=",str(B,0,2)),0.1,0 translate TFudge}
 sphere {<B*Fudge,-0.5,0>,Rad}
 pigment{color rgb<0.3,0.9,0>} translate 2*y
}

union{
 text{ttf Font concat("C=cos(radians(A*360)=",str(C,0,2)),0.1,0 translate TFudge}
 sphere {<C*Fudge,-0.5,0>,Rad}
 pigment{Blue}
}

union{
 text{ttf Font concat("D=min(B,C)=",str(D,0,2)),0.1,0 translate TFudge}
 sphere {<D*Fudge,-0.5,0>,Rad}
 pigment{Cyan} translate -2*y
}

union{
 text{ttf Font concat("E=max(B,C)=",str(E,0,2)),0.1,0 translate TFudge}
 sphere {<E*Fudge,-0.5,0>,Rad}
 pigment{Magenta} translate -4*y
}

