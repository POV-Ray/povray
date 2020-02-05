// Persistence Of Vision raytracer version 3.1 sample file.

#version 3.1;
global_settings { assumed_gamma 2.2 }

#include "stage1.inc"

#declare Radius =2.5;
#declare RowSpace=1.35;
#declare ColSpace=1.25;
#declare Dist=0.9-Radius;
#declare Row3=-Dist;
#declare Row2=Row3+Radius*RowSpace*2;
#declare Row1=Row2+Radius*RowSpace*2;
#declare Col1= -Radius*ColSpace*4;
#declare Col2= Col1+Radius*ColSpace*2;
#declare Col3= Col2+Radius*ColSpace*2;
#declare Col4= Col3+Radius*ColSpace*2;
#declare Col5= Col4+Radius*ColSpace*2;

camera{Camera1 translate <0,Row2,-180>}

#declare Thing=
intersection {
   sphere {<0, 0, 0>, 1}
   object {UnitBox translate z*0.4 rotate 45*y}
   bounded_by {UnitBox}
   scale Radius
}


object {Thing pigment {agate}       translate <Col1 ,Row1 ,Dist>}
object {Thing pigment {bozo}        translate <Col2 ,Row1 ,Dist>}
object {Thing pigment {checker}     translate <Col3 ,Row1 ,Dist>}
object {Thing pigment {color White} translate <Col4 ,Row1 ,Dist>}
object {Thing pigment {gradient x}  translate <Col5 ,Row1 ,Dist>}

object {Thing pigment {granite}    translate <Col1 ,Row2 ,Dist>}
object {Thing pigment {hexagon rotate -x*90}    translate <Col2 ,Row2 ,Dist>}
object {Thing pigment {image_map{png "test.png"}
        translate -(x+y)/2
        scale 2*Radius}  translate <Col3 ,Row2 ,Dist>}
object {Thing pigment {leopard scale .3}    translate <Col4 ,Row2 ,Dist>}
object {Thing pigment {mandel 256} translate <Col5 ,Row2 ,Dist>}

object {Thing pigment {marble turbulence .8}  translate <Col1 ,Row3 ,Dist>}
object {Thing pigment {onion}   translate <Col2 ,Row3 ,Dist>}
object {Thing pigment {radial rotate -x*90}  translate <Col3 ,Row3 ,Dist>}
object {Thing pigment {spotted} translate <Col4 ,Row3 ,Dist>}
object {Thing pigment {wood turbulence .15 scale .5} translate <Col5 ,Row3 ,Dist>}
