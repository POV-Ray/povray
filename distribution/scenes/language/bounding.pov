// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence of Vision Raytracer Scene Description File
// File: bounding.pov
// Author: Fabien Mosen
// Description:
// This file demonstrates the "min_extent" and "max_extent" functions.
//
// -w320 -h240
// -w800 -h600 +a0.3
//
//*******************************************

#version 3.6;
global_settings {assumed_gamma 1.0}

#include "colors.inc"

camera { location <100,70,-60> 
         right     x*image_width/image_height
         angle 20 // direction z*4 
         look_at <0,0,0>
       }
light_source {<400,500,-300> White*2}

#declare r1=seed(0);

//defining and placing the objects groups
#declare Group1 =
blob {
  threshold .6
  #declare I=0;
  #while (I < 1)
    sphere {<-5+rand(r1)*10,-5+rand(r1)*10,-5+rand(r1)*10>,2+rand(r1)*1,5}
    #declare I=I+.1;
  #end
  pigment {SteelBlue}
  finish {phong .7}
  translate z*-10
}

#declare Group2 =
union {
  #declare I=0;
  #while (I < 1)
    box {-1,1 rotate rand(r1)*360
              scale .5+rand(r1)*.5
              translate <-5+rand(r1)*10,-5+rand(r1)*10,-5+rand(r1)*10>
    }
    #declare I=I+.1;
  #end
  pigment {MediumForestGreen}
  translate z*+10
}

object {Group1}
object {Group2}

//a macro to make a box perimeter
#macro BarBox (StartPoint,EndPoint,Thick)
  #local SmallCorner=<min(StartPoint.x,EndPoint.x),min(StartPoint.y,EndPoint.y),min(StartPoint.z,EndPoint.z)>;
  #local BigCorner=<max(StartPoint.x,EndPoint.x),max(StartPoint.y,EndPoint.y),max(StartPoint.z,EndPoint.z)>;
  #local Amplitude=BigCorner-SmallCorner;
  union {
    box {<0,0,0>,<Amplitude.x,Thick,Thick>}
    box {<0,Amplitude.y,0>,<Amplitude.x,Amplitude.y-Thick,Thick>}
    box {<0,0,Amplitude.z>,<Amplitude.x,Thick,Amplitude.z-Thick>}
    box {<0,Amplitude.y,Amplitude.z>,<Amplitude.x,Amplitude.y-Thick,Amplitude.z-Thick>}
  
    box {<0,0,0>,<Thick,Amplitude.y,Thick> translate 0}
    box {<0,0,0>,<Thick,Amplitude.y,Thick> translate <Amplitude.x-Thick,0,0>}
    box {<0,0,0>,<Thick,Amplitude.y,Thick> translate <0,0,Amplitude.z-Thick>}
    box {<0,0,0>,<Thick,Amplitude.y,Thick> translate <Amplitude.x-Thick,0,Amplitude.z-Thick>}
  
    box {0,<Thick,Thick,Amplitude.z>}
    box {0,<Thick,Thick,Amplitude.z> translate <Amplitude.x-Thick,Amplitude.y-Thick,0>}
    box {0,<Thick,Thick,Amplitude.z> translate <0,Amplitude.y-Thick,0>}
    box {0,<Thick,Thick,Amplitude.z> translate <Amplitude.x-Thick,0,0>}
  
    translate SmallCorner
  }
#end

//a macro that uses the bounding info to display the bounding box of an object
#macro ShowBounds (Obj,Col1,Col2,Col3,Thk)
 #local Corner1 = min_extent (Obj);
 #local Corner2 = max_extent (Obj);
 object {BarBox (Corner1,Corner2,Thk) pigment {Col3}}
 sphere {Corner1,Thk*2 pigment {Col1}}
 sphere {Corner2,Thk*2 pigment {Col2}}
#end

//creating the visible bounding limits
ShowBounds (Group1,Yellow,White,OrangeRed,.2)
ShowBounds (Group2,Yellow,White,OrangeRed,.2)
