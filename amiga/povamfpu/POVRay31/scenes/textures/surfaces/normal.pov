// Persistence Of Vision raytracer version 3.1 sample file.

#version 3.1;
global_settings { assumed_gamma 2.2 }

#include "stage1.inc"

#declare Radius =5;
#declare RowSpace=1.35;
#declare ColSpace=1.25;
#declare Dist=0.9-Radius;
#declare Row2=-Dist;
#declare Row1=Row2+Radius*RowSpace*2;
#declare Col1= -Radius*ColSpace*2;
#declare Col2= Col1+Radius*ColSpace*2;
#declare Col3= Col2+Radius*ColSpace*2;

camera{Camera1 translate <0,Row1-Radius*RowSpace,-240>}

#declare Thing=
object {
   UnitBox
   scale Radius
   pigment {Gray75}
   finish  {phong 0.8}
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

object { Thing normal {dents  0.6}    translate <Col3 ,Row1 ,Dist> }
object { Thing normal {ripples  0.6}  translate <Col1 ,Row2 ,Dist> }
object { Thing normal {waves    0.6}  translate <Col2 ,Row2 ,Dist> }
object { Thing normal {wrinkles 0.6}  translate <Col3 ,Row2 ,Dist> }

