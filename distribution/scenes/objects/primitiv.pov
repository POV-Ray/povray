// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3
#version 3.7;
global_settings { assumed_gamma 1.0 }
#default{ finish{ ambient 0.1 diffuse 0.9 }} 

#include "colors.inc"

#declare Radius =2.5;
#declare RowSpace=1.0;
#declare ColSpace=1.0;
#declare Dist=-1.25;
#declare Row3= 2;
#declare Row2=Row3+Radius*RowSpace*2;
#declare Row1=Row2+Radius*RowSpace*2;
#declare Col1= -Radius*ColSpace*3;
#declare Col2= Col1+Radius*ColSpace*2;
#declare Col3= Col2+Radius*ColSpace*2;
#declare Col4= Col3+Radius*ColSpace*2;
#declare Col5= Col4+Radius*ColSpace*2;

// perspective (default) camera
camera {
  location  <0.0, 5.0, -17.5>
  look_at   <0.0, 7.0,  0.0>
  right     x*image_width/image_height
  angle 65 
}
// create a regular point light source
light_source {
  0*x                  // light's position (translated below)
  color rgb <1,1,1>    // light's color
  translate <200, 500, -500>
}

background { color rgb<1,1,1>*0.35 } 

#declare ThisGreen = color rgb<0.6,1,0>; 

#declare Fade=
texture {
   pigment {
      gradient <1,1,0>
      color_map {
         [0.0 color ThisGreen]
         [0.3 color ThisGreen]
         [0.5 color Clear]
         [1.0 color Clear]
      }
      rotate z*45
   }
   finish {phong 1}
   scale 5
}

#declare Solid=
texture {
   pigment {ThisGreen}
   finish {phong 1}
}

bicubic_patch {
   type 1 flatness 0.01 u_steps 4 v_steps 4,
   <0, -1.5, 2>, <1, -1.5, 0>, <2, -1.5, 0>, <3, -1.5, -2>,
   <0, -0.5, 0>, <1, -0.5, 0>, <2, -0.5, 0>, <3, -0.5,  0>,
   <0,  0.5, 0>, <1,  0.5, 0>, <2,  0.5, 0>, <3,  0.5,  0>,
   <0,  1.5, 2>, <1,  1.5, 0>, <2,  1.5, 0>, <3,  1.5, -2>
   texture {Solid}
   rotate    -45*y
   translate <Col1, Row1, Dist>
}

blob {
   threshold 0.6
   component 1.0, 1.0, < 0.75,   0,       0>
   component 1.0, 1.0, <-0.375,  0.64952, 0>
   component 1.0, 1.0, <-0.375, -0.64952, 0>
   texture {Solid}
   translate <Col2, Row1, Dist>
}

box {
   <-1, -1, -1>, <1, 1, 1>
   texture {Solid}
   rotate <-25, 15, 0>
   translate <Col3, Row1, Dist>
}

cone {
   x,0.5,
   -x,1.0
   rotate  y*30
   texture {Solid}
   translate <Col4, Row1, Dist>
}

cylinder {
   x,-x,1.0
   rotate  y*30
   texture {Solid}
   translate <Col1, Row2, Dist>
}

cubic {
   //         x*x                + y*y*y + y*y           + z*z  -1 =0
   <0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.45, 0, 1, 0, 0, 0, 0, 1, 0, -1>
   sturm
   texture {Fade}
   clipped_by{box{<-1,-1,-1>,<1,1,1>  scale 2.5}}
   bounded_by{clipped_by}
   translate <Col2, Row2, Dist>
}

disc {
   <Col3, Row2, Dist>, //center location
   <-1, 3, -2>,        //normal vector
   1.5,                //radius
   0.5                 //hole radius (optional)
   texture {Solid}
}


height_field {
   png "plasma3.png"
   smooth
   texture {Solid}
   translate <-.5, -.5, -.5>
   scale <4,2,4>
   rotate <-35, -30, 0>
   translate <Col4, Row2, Dist>
}

plane {
   z, 0
   hollow on
   texture {Fade}
   clipped_by{box{<-1,-1,-1>,<1,1,1> scale 3}}
   translate <Col1, Row3, Dist>
}

//   x^4 - x^2 + y^2 + z^2 = 0
poly { 4
   < 1,   0,   0,   0,   0,   0,   0,   0,   0, -1,
   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,
   0,   0,   0,   0,   0,   1,   0,   0,   0,  0,
   0,   0,   1,   0,   0>
   sturm
   scale 2
   texture {Solid}
   translate <Col2, Row3, Dist>
}

// y - x*x + z*z = 0
quadric {
   <-1, 0, 1>, <0, 0, 0>, <0, 1, 0>, 0
   texture {Fade}
   clipped_by{box{<-1,-1,-1>,<1,1,1> scale 2}}
   translate <Col3, Row3, Dist>
}

quartic {
   //xxxx         xxyy    xxzz  -2(rr+RR)xx
   <1, 0, 0, 0, 2, 0, 0, 2, 0, -2.5,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

   //yyyy     yyzz  -2(rr-RR)yy
   1, 0, 0, 2, 0    1.5,     0, 0, 0,  0,

   //zzzz   -2(rr+RR)zz    (rr-RR)(rr-RR)
   1, 0,    -2.5,       0, 0.5625>
   texture {Solid}
   rotate -45*x
   translate <Col4, Row3, Dist>
}

