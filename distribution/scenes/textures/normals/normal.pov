// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

global_settings { assumed_gamma 1.0 }

#include "colors.inc"
#include "textures.inc"

camera {
  location  <0,10,-95>
  right     x*image_width/image_height
  direction 3*z
}


light_source { < 500, 500, -200> White*0.9}
//light_source { <-500,  50, -200> White*0.1}

#declare Radius =5;
#declare RowSpace=1.35;
#declare ColSpace=1.25;
#declare Dist=0.9-Radius;
#declare Row2=-Dist;
#declare Row1=Row2+Radius*RowSpace*2;
#declare Col1= -Radius*ColSpace*2;
#declare Col2= Col1+Radius*ColSpace*2;
#declare Col3= Col2+Radius*ColSpace*2;


#declare Thing=
object {
   box{<-1,-1,-1>,<1,1,1>}  
   scale Radius
   pigment {color rgb<1,1,1> }
   finish  {phong 1 }
}

object { Thing normal {bumps  0.6} translate <Col1 ,Row1 ,Dist> }

object {
   Thing
   normal {
      bump_map { png "test.png" bump_size 1.0 use_index }
      translate -(x+y)/2
      scale 2*Radius
   }
   translate <Col2 ,Row1 ,Dist>
}

object { Thing normal {dents    1.5}  translate <Col3 ,Row1 ,Dist> }
object { Thing normal {ripples  1.5}  translate <Col1 ,Row2 ,Dist> }
object { Thing normal {waves    1.5}  translate <Col2 ,Row2 ,Dist> }
object { Thing normal {wrinkles 1.5}  translate <Col3 ,Row2 ,Dist> }

