// Persistence Of Vision raytracer version 3.1 sample file.
// Demonstrates various new float math functions.
// Animate this scene with clock values +ki0.0 to +kf1.0

global_settings { assumed_gamma 2.2 }

#declare A = clock;
#declare B = sin(radians(A*360));
#declare C = cos(radians(A*360));
#declare D = min(B,C);
#declare E = max(B,C);

#include "colors.inc"

#declare Rad=1/3;
#declare Fudge=5;
#declare TFudge=<-6.25,0,0.2>;
#declare Font="cyrvetic.ttf"

camera {
   location  <0, 0, -120>
   direction <0, 0,  12>
   look_at   <0, 0,   0>
}

light_source { <5000, 10000, -20000> color White}
plane { z, Rad hollow on pigment {checker color rgb <1,.8,.8> color rgb <1,1,.8> scale <Fudge,1,1>} }

union{
 text{ttf Font concat("A=clock=",str(A,0,2)),0.1,0 translate TFudge}
 sphere {<A*Fudge,-0.5,0>,Rad}
 pigment{Red} translate 4*y
}

union{
 text{ttf Font concat("B=sin(radians(A*360)=",str(B,0,2)),0.1,0 translate TFudge}
 sphere {<B*Fudge,-0.5,0>,Rad}
 pigment{Green} translate 2*y
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

