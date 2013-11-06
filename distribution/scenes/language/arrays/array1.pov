// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Ray Tracer POV-Ray sample Scene
// by Chris Young
// ARRAY1.POV demonstrates basic use of an array.  A one dimension
// array is filled with 10 text objects that are a random digit.
// These objects are displayed in a union object in reverse order.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0}
#default{ finish{ ambient 0.1 diffuse 0.9 }} 

#include "debug.inc"
#include "colors.inc"

light_source { <100,1000,-1000>, White}

camera { location <2,1,-10> 
         right     x*image_width/image_height
         angle 35 // direction 2*z 
         look_at <0,0,0>
       }

union {
 plane{y,-2} plane{-z,-10} plane{x,-10}
 pigment{checker   color rgb<1,1,1>*0.9 color rgb<1,1,1>*1.2 }
}

#declare Digit=array[10]

#declare S=seed(123);

#declare I=0;

#while (I<=9)
  #declare Digit[I]=text{ttf "cyrvetic.ttf",str(I,1,0),0.1,0}
  #declare I=I+1;
#end

union{
 #declare I=9;
 #while (I>=0)
  #declare D=int(rand(S)*10);
  object{Digit[D] translate x*I*0.6}
  #declare I=I-1;
 #end
 pigment{Red}
 translate <-3,0,0>
}
