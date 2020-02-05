// Persistence Of Vision raytracer version 3.1 sample file.
// Demonstrates various new float math functions.
// Animate this scene with clock values +ki0.0 to +kf1.0

global_settings { assumed_gamma 2.2 }

#declare K = clock*12-6;
#declare L = mod(K,2);
#declare M = div(K,2);
#declare N = mod(K,3);
#declare Oh = div(K,3);

#include "colors.inc"

#declare Rad=1/3;
#declare TFudge=<-6,0,0.2>;
#declare Font="cyrvetic.ttf"

camera {
   location  <0, 0, -120>
   direction <0, 0,  12>
   look_at   <0, 0,   0>
}

light_source { <5000, 10000, -20000> color White}
plane { z, Rad hollow on pigment {checker color rgb <1,.8,.8> color rgb <1,1,.8>} }

union{
 text{ttf Font concat("K=clock*12-6=",str(K,0,2)),0.1,0 translate TFudge}
 sphere {<K,-0.5,0>,Rad}
 pigment{Red} translate 4*y
}

union{
 text{ttf Font concat("L=mod(K,2)=",str(L,0,2)),0.1,0 translate TFudge}
 sphere {<L,-0.5,0>,Rad}
 pigment{Green} translate 2*y
}

union{
 text{ttf Font concat("M=div(K,2)=",str(M,0,2)),0.1,0 translate TFudge}
 sphere {<M,-0.5,0>,Rad}
 pigment{Blue} translate 0*y
}

union{
 text{ttf Font concat("N=mod(K,3)=",str(N,0,2)),0.1,0 translate TFudge}
 sphere {<N,-0.5,0>,Rad}
 pigment{Cyan} translate -2*y
}

union{
 text{ttf Font concat("O=div(K,3)=",str(Oh,0,2)),0.1,0 translate TFudge}
 sphere {<Oh,-0.5,0>,Rad}
 pigment{Magenta} translate -4*y
}
