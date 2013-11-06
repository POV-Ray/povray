// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0}

camera {
  location <0,7,-100>
  right   x*image_width/image_height
  angle 20 //direction 4 *z
  look_at <0,7.5,0>
}

plane {
  z, 3.01
  hollow on
  pigment {checker color rgb<1,1,1>, color rgb<1,1,1>*0  rotate<0,0,45>}
}

light_source { <300, 500, -500> color rgb<1,1,1>*0.65}
light_source { <-50,  10, -500> color rgb<1,1,1>*0.65}

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


#declare Thing=
intersection {
   sphere {<0, 0, 0>, 1}
   box{<-1,-1,-1><1,1,1> translate z*0.4 rotate 45*y}
   scale Radius
}


object {Thing pigment {agate}       translate <Col1 ,Row1 ,Dist>}
object {Thing pigment {bozo}        translate <Col2 ,Row1 ,Dist>}
object {Thing pigment {checker}     translate <Col3 ,Row1 ,Dist>}
object {Thing pigment {color rgb<1,1,1>} translate <Col4 ,Row1 ,Dist>}
object {Thing pigment {gradient x}       translate <Col5 ,Row1 ,Dist>}

object {Thing pigment {granite}                 translate <Col1 ,Row2 ,Dist>}
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
