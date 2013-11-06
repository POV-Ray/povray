// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Ray Tracer POV-Ray sample Scene
// by Chris Young
// MACRO3.POV demonstrates basic use of a macro as a type
// of "inline function" that "returns" a value like a built-in
// function.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0}
#default{ finish{ ambient 0.1 diffuse 0.9 }} 

#include "colors.inc"

light_source { <100,1000,-1000>, White}

camera { location <0,0,-15> 
         right    x*image_width/image_height
         angle 35 // direction 2*z 
         look_at <0,0,0>
       }

plane{-z,-1  pigment{checker  color rgb<1,1,1>*0.8 color rgb<1,1,1> }}

// Define the macro.  Parameters are:
//   T:  Middle value of time
//   T1: Initial time
//   T2: Final time
//   P1: Initial position (may be float, vector or color)
//   P2: Final position (may be float, vector or color)
//   Result is a value between P1 and P2 in the same proportion
//    as T is between T1 and T2.
#macro Interpolate(T,T1,T2,P1,P2)
   // Note: Without outermost parens this doesn't work as expected
   //       in the Location calculations.
   (P1+(T1+T/(T2-T1))*(P2-P1))

#end

#declare Here  = <-5,-2,0>;
#declare There = <5,3,0>;
#declare This_Color = rgb <1,1,0>;
#declare That_Color = rgb <1,0,1>;
#declare Size1 = 0.3;
#declare Size2 = 0.5;

#declare I=0;
#while (I<15)
  // Interpolate vector location from Here to There
  #declare Location=<0,0,-1> + Interpolate(I,0,15,Here,There) * 0.8;

  sphere{
    Location,
    // Interpolate float radius
    Interpolate(I,0,15,Size1,Size2)
    pigment{
      // Interpolate color
      color Interpolate(I,0,15,This_Color,That_Color)
    }
  }
  #declare I=I+1;
#end
