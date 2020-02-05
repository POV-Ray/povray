// Persistence Of Vision Raytracer version 3.1 sample file.
// FLOAT5.POV
// Demonstrates various new float math functions and #while loop

global_settings { assumed_gamma 2.2 }

#include "colors.inc"

#declare Rad=1/6;
#declare Font="cyrvetic.ttf"

#declare Xval=-6.0;

#while (Xval <= 6.0)
  sphere{<Xval,exp(Xval),0>,Rad pigment{Red}}

  #if (Xval != 0.0)
    sphere{<Xval,pow(Xval,-1),0>,Rad pigment{Green}}
  #end

  sphere{<Xval,pow(Xval,2),0>,Rad pigment{Blue}}
  sphere{<Xval,pow(Xval,3),0>,Rad pigment{Cyan}}

  #if (Xval > 0.0)
    sphere{<Xval,log(Xval),0>,Rad pigment{Magenta}}
  #end

  #declare Xval=Xval+0.1;
#end

 text{ttf Font "Y=exp(X)",0.1,0    translate <-6.5, 0.5,0> pigment{Red}}
 text{ttf Font "Y=pow(X,-1)",0.1,0 translate <-6.5,-1.5,0> pigment{Green}}
 text{ttf Font "Y=pow(X,2)",0.1,0  translate <-6.5, 3,0>   pigment{Blue}}
 text{ttf Font "Y=pow(X,3)",0.1,0  translate <-6.5,-4,0>   pigment{Cyan}}
 text{ttf Font "Y=log(X)",0.1,0    translate < 2.5, 2,0>   pigment{Magenta}}

camera {
   location  <0, 0, -120>
   direction <0, 0,  12>
   look_at   <0, 0,   0>
}

light_source { <5000, 10000, -20000> color White}
plane { -z, -Rad pigment {checker color rgb <1,.8,.8> color rgb <1,1,.8>} }

union{ // X-axis
 cylinder{-x*5.5,x*5.5,.1}
 cone{-x*6.5,0,-x*5.5,.2}
 cone{ x*6.5,0, x*5.5,.2}
 translate z*Rad
 pigment{rgb<1,.8,1>}
}

union{ // Y-axis
 cylinder{-y*4,y*4,.1}
 cone{-y*5,0,-y*4,.2}
 cone{ y*5,0, y*4,.2}
 translate z*Rad
 pigment{rgb<.8,1,1>}
}

union{ // Axes labels
 text{ttf Font "X",0.1,0 translate <5.5,-1,0>}
 text{ttf Font "Y",0.1,0 translate <-.75,4,0>}
 pigment{rgb<1,.4,0>}
}

