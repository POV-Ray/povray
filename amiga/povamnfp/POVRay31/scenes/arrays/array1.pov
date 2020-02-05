// Persistence of Vision Ray Tracer POV-Ray 3.1 Sample Scene
// by Chris Young
// ARRAY1.POV demonstrates basic use of an array.  A one dimension
// array is filled with 10 text objects that are a random digit.
// These objects are displayed in a union object in reverse order.

#include "debug.inc"
#include "colors.inc"

light_source { <100,1000,-1000>, White}

camera { location <2,1,-10> direction 2*z look_at <0,0,0>}

union { 
 plane{y,-2} plane{-z,-10} plane{x,-10}
 pigment{checker Cyan,Yellow}
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
