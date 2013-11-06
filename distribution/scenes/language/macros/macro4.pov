// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Ray Tracer POV-Ray sample Scene
// by Chris Young
// MACRO4.POV demonstrates basic use of a macro as a type
// of procedure that "returns" a value via a parameter but
// only when a lone identifier is passed.  If a constant
// or expression is passed, the value cannot be returned.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0}
#default{ finish{ ambient 0.1 diffuse 0.9 }} 

#include "colors.inc"

light_source { <100,1000,-1000>, White}

camera { location <0,-1,-16> 
         right    x*image_width/image_height
         angle 35 // direction 2*z 
         look_at <0,-1,0>
       }

plane{-z,-10  pigment{checker color rgb<1,1,1>*0.8 color rgb<1,1,1>} }

// Define the macro.  Parameters are:
//   V:  The value to be incremented.  New value
//       is returned via this parameter so it must be
//       a lone identifier.  It cannot be a constant or
//       an expression.
#macro Inc(V)
  #local V=V+1;
#end

#declare Value=5;

union{
 text{ttf "cyrvetic.ttf" "#macro Inc(V)",0.1,0 translate 2*y}
 text{ttf "cyrvetic.ttf" "  #local V=V+1;",0.1,0 translate y}
 text{ttf "cyrvetic.ttf" "#end",0.1,0 }

 text{ttf "cyrvetic.ttf" concat("#declare Value=",str(Value,0,0),";"),0.1,0 translate -y}

 Inc(Value+0)  // Expression won't work

 text{ttf "cyrvetic.ttf" concat("Inc(Value+0)=",str(Value,0,0)),0.1,0 translate -2*y}

 Inc(+Value)  // This too is an expression so it won't work

 text{ttf "cyrvetic.ttf" concat("Inc(+Value)=",str(Value,0,0)),0.1,0 translate -3*y}

 Inc(Value)   // Lone identifier works.  It accepts return value.

 text{ttf "cyrvetic.ttf" concat("Inc(Value)=",str(Value,0,0)),0.1,0 translate -4*y}

 pigment{Red*0.7}
 translate -3.75*x
}
