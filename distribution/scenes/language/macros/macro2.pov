// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Ray Tracer POV-Ray sample Scene
// by Chris Young
// MACRO2.POV demonstrates basic use of a macro to modify an
// identifier parameter, not just do something based upon the
// parameter.  Defines a macro called Turn_Me which takes
// an object identifier and re-declares it turned a specified
// amount about a particular axis. The result is passed back
// through the parameter.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0}
#default{ finish{ ambient 0.1 diffuse 0.9 }} 

#include "colors.inc"

light_source { <1000,1000,-1000>, White}

camera { location <3,3,-10> 
         right    x*image_width/image_height
         angle 35 // direction 2*z 
         look_at <0,0,0>
       }

union {
 plane{y,-2} plane{-z,-10} plane{x,-10}
 pigment{checker  color rgb<1,1,1>*0.9 color rgb<1,1,1>*1.2 }
}

// Define the macro.  Parameters are:
//   Stuff:    The stuff to be rotated.  This identifier is
//             actually re-declared and the new object is passed
//             back to the calling module.
//   Degrees:  Number of degrees to rotate
//   Axis:     The axis about which we'll rotate
#macro Turn_Me(Stuff,Degrees,Axis)
    #declare Stuff=object{Stuff rotate Axis*Degrees}
#end

#declare Thing = cone{0,1/2,y,0}

object{Thing               // Display the original Thing
  pigment{rgb<1,0,0>}
  translate -2.25*x
}

Turn_Me(Thing,-90,x)       // Turn -90 about x

object{Thing               // Thing was changed by Turn_Me
  pigment{rgb<0,1,1>}
}

Turn_Me(Thing,-90,y)       // Turn -90 about y

object{Thing               // Thing was changed again
  pigment{rgb<1,1,0>}
  translate 2.25*x
}

