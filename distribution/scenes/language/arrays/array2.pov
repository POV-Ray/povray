// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Ray Tracer POV-Ray sample Scene
// by Chris Young
// ARRAY2.POV demonstrates basic use of a two dimension array.
// A 4 x 10 array is declared and initialized with one digit
// float values. This digits are displayed as 40 text objects.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0}
#default{ finish{ ambient 0.1 diffuse 0.9 }} 

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

#declare Digit =
 array[4][10]
 {
   {7,6,7,0,2,1,6,5,5,0},
   {1,2,3,4,5,6,7,8,9,0},
   {0,9,8,7,6,5,4,3,2,1},
   {1,1,2,2,3,3,4,4,5,5}
 }


union{
 #declare J=0;
 #while (J<4)
   #declare I=0;
   #while (I<10)
      text{ttf "cyrvetic.ttf",str(Digit[J][I],0,0),0.1,0
      translate <I*.6,-J*1,0>}
      #declare I=I+1;
   #end
   #declare J=J+1;
 #end
 pigment{Red}
 translate <-3,1,0>
}

