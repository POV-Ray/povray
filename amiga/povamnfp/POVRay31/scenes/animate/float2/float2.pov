// Persistence Of Vision raytracer version 3.1 sample file.
// Demonstrates various new float math functions.
// Animate this scene with clock values +ki0.0 to +kf1.0


global_settings { assumed_gamma 2.2 }

#declare F = clock*10-5;
#declare G = floor(F);
#declare H = ceil(F);
#declare I = int(F);
#declare J = abs(F);

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
plane { z, Rad hollow on pigment {checker color rgb <1,.8,.8> color rgb <1,1,.8>}}

union{
 text{ttf Font concat("F=clock*10-5=",str(F,0,2)),0.1,0 translate TFudge}
 sphere {<F,-0.5,0>,Rad}
 pigment{Red} translate 4*y
}

union{
 text{ttf Font concat("G=floor(F)=",str(G,0,2)),0.1,0 translate TFudge}
 sphere {<G,-0.5,0>,Rad}
 pigment{Green} translate 2*y
}

union{
 text{ttf Font concat("H=ceil(F)=",str(H,0,2)),0.1,0 translate TFudge}
 sphere {<H,-0.5,0>,Rad}
 pigment{Blue} translate 0*y
}

union{
 text{ttf Font concat("I=int(F)",str(I,0,2)),0.1,0 translate TFudge}
 sphere {<I,-0.5,0>,Rad}
 pigment{Cyan} translate -2*y
}

union{
 text{ttf Font concat("J=abs(F)=",str(J,0,2)),0.1,0 translate TFudge}
 sphere {<J,-0.5,0>,Rad}
 pigment{Magenta} translate -4*y
}

