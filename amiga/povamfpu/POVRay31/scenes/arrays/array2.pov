// Persistence of Vision Ray Tracer POV-Ray 3.1 Sample Scene
// by Chris Young
// ARRAY2.POV demonstrates basic use of a two dimension array.  
// A 4 x 10 array is declared and initialized with one digit
// float values. This digits are displayed as 40 text objects.

#include "colors.inc"

light_source { <100,1000,-1000>, White}

camera { location <2,1,-10> direction 2*z look_at <0,0,0>}

union { 
 plane{y,-2} plane{-z,-10} plane{x,-10}
 pigment{checker Cyan,Yellow}
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

