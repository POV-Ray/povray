// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Ray Tracer POV-Ray sample Scene
// by Chris Young
// MACRO1.POV demonstrates basic use of a macro.  Defines
// a macro called Make_Many which takes an object and
// makes many copies of it rotated about a particular axis.
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
//   Stuff:    The stuff to be multiplied and rotated
//   How_Many: Number of copies to make
//   Axis:     The axis about which we'll rotate the copies.
#macro Make_Many (Stuff,How_Many,Axis)
  #local Count=0;   // this identifier is local and
                    // temporary to this macro
  #while (Count<How_Many)
    object{Stuff rotate Axis*Count*(360/How_Many)}
    #local Count=Count+1;
  #end
#end

#declare Thing = cylinder{0,x,0.1}

union {
  Make_Many (Thing,4,z)  // Make 4 things rotated about z
  pigment{Red}
  translate -2.25*x
}

union {
  Make_Many (Thing,6,z)  // Make 6 things rotated about z
  pigment{Blue}
}

union {
  Make_Many (Thing,6,y)  // Make 6 things rotated about y
  pigment{Green}
  translate 2.25*x
}

