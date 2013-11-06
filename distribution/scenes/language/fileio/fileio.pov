// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Ray Tracer POV-Ray sample Scene
// by Chris Young
// FILEIO.POV demonstrates basic use of fopen, read and write directives.
// A string, float and vector are written to a file, read back in
// and displayed.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.6;
global_settings {assumed_gamma 1.0}

#include "colors.inc"

light_source { <100,1000,-1000>, White}

camera { location <0,1,-16> 
         angle 35  
         right    x*image_width/image_height
         look_at <0,1,0>
       }

#fopen MyFile "FILEIO.TXT" write

#write (MyFile,"\"Testing 123\",",5,",",<1,2,3>,"\n")

#fclose MyFile

#fopen MyFile "FILEIO.TXT" read

#read (MyFile,MyString,MyFloat,MyVect)

#fclose MyFile

union{
 text{ttf "timrom.ttf" concat("MyString='",MyString,"'"),0.1,0 translate y}
 text{ttf "timrom.ttf" concat("MyFloat=",str(MyFloat,0,0)),0.1,0 }
 text{ttf "timrom.ttf" concat("MyVector=<",
                               str(MyVect.x,0,0),",",
                               str(MyVect.y,0,0),",",
                               str(MyVect.z,0,0),">"),0.1,0 translate -y}

 pigment{Red}
 translate -5*x
}

union {
 plane{y,-2} plane{-z,-10} plane{x,-10}
 pigment{checker Cyan,Yellow}
}

