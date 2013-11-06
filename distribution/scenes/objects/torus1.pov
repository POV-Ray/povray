// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
// Demo showing several torii ... Dieter Bayer, June 1994
//
// -w320 -h240
// -w800 -h600 +a0.3
#version  3.7;
global_settings { 
  assumed_gamma 1.8
  max_trace_level 5
}

#include "colors.inc"
#include "textures.inc"


camera {
  location <0, 80, -200>
  right     x*image_width/image_height
  angle 42 
  look_at <0, 15, 0>
}

light_source { <50, 200, -100> colour Gray70 }
light_source { <-20, 40, -20> colour Gray70 }
light_source { <100, 80, -200> colour Gray70 }

background { color MidnightBlue }

plane { y, 0
   pigment {
      checker colour Blue colour Green
      scale 20
   }
   finish {
      ambient 0.2
      diffuse 0.8
   }
}

plane { z, 250
   hollow on
   pigment { colour White }
   finish {
     ambient 0
     diffuse 0.1
     specular 1
     roughness 0.00001
     reflection 0.8
   }
}

#declare Torus1 = torus { 10, 1.5 rotate 90*x }
#declare Torus2 = torus { 15, 5 }
#declare Torus3 = torus { 10, 3 rotate 90*x }

#declare Ring = union {
  object { Torus1 translate 35*x rotate  0*y }
  object { Torus1 translate 35*x rotate  15*y }
  object { Torus1 translate 35*x rotate  30*y }
  object { Torus1 translate 35*x rotate  45*y }
  object { Torus1 translate 35*x rotate  60*y }
  object { Torus1 translate 35*x rotate  75*y }
  object { Torus1 translate 35*x rotate  90*y }
  object { Torus1 translate 35*x rotate 105*y }
  object { Torus1 translate 35*x rotate 120*y }
  object { Torus1 translate 35*x rotate 135*y }
  object { Torus1 translate 35*x rotate 150*y }
  object { Torus1 translate 35*x rotate 165*y }
  object { Torus1 translate 35*x rotate 180*y }
  object { Torus1 translate 35*x rotate 195*y }
  object { Torus1 translate 35*x rotate 210*y }
  object { Torus1 translate 35*x rotate 225*y }
  object { Torus1 translate 35*x rotate 240*y }
  object { Torus1 translate 35*x rotate 255*y }
  object { Torus1 translate 35*x rotate 270*y }
  object { Torus1 translate 35*x rotate 285*y }
  object { Torus1 translate 35*x rotate 300*y }
  object { Torus1 translate 35*x rotate 315*y }
  object { Torus1 translate 35*x rotate 330*y }
  object { Torus1 translate 35*x rotate 345*y }
}

#declare Stack = union {
  object { Torus2 translate  0*y scale <0.5, 1, 0.5> }
  object { Torus2 translate  5*y scale <0.5, 1, 0.5> }
  object { Torus2 translate 10*y scale <0.5, 1, 0.5> }
  object { Torus2 translate 15*y scale <1, 1, 0.5> }
  object { Torus2 translate 20*y scale <1, 1, 0.75> }
  object { Torus2 translate 25*y scale <1, 1, 1> }
  object { Torus2 translate 30*y scale <0.75, 1, 1> }
  object { Torus2 translate 35*y scale <0.5, 1, 1> }
}

#declare Queue = union {
  object { Torus3 translate  200*z }
  object { Torus3 translate  180*z }
  object { Torus3 translate  160*z }
  object { Torus3 translate  140*z }
  object { Torus3 translate  120*z }
  object { Torus3 translate  100*z }
  object { Torus3 translate   80*z }
  object { Torus3 translate   60*z }
  object { Torus3 translate   40*z }
  object { Torus3 translate   20*z }
  object { Torus3 translate    0*z }
  object { Torus3 translate  -20*z }
  object { Torus3 translate  -40*z }
  object { Torus3 translate  -60*z }
  object { Torus3 translate  -80*z }
  object { Torus3 translate -100*z }
  object { Torus3 translate -120*z }
  object { Torus3 translate -140*z }
  object { Torus3 translate -160*z }
  object { Torus3 translate -180*z }
  object { Torus3 translate -200*z }
  object { Torus3 translate -220*z }
  object { Torus3 translate -240*z }
  object { Torus3 translate -260*z }
  object { Torus3 translate -280*z }
  object { Torus3 translate -300*z }
  object { Torus3 translate -320*z }
  object { Torus3 translate -340*z }
  object { Torus3 translate -360*z }
  object { Torus3 translate -380*z }
  object { Torus3 translate -400*z }
  object { Torus3 translate -420*z }
  object { Torus3 translate -440*z }
  object { Torus3 translate -460*z }
  object { Torus3 translate -480*z }
  object { Torus3 translate -500*z }
  object { Torus3 translate -520*z }
  object { Torus3 translate -540*z }
  object { Torus3 translate -560*z }
  object { Torus3 translate -580*z }
  object { Torus3 translate -600*z }
}

object {
  Queue

  texture { Copper_Metal }

  rotate -20*y
  translate <40, 15, 40>
}

object {
  Stack

  texture { pigment { White_Marble } scale <2, 2, 2> }

  translate <-70, 5, 80>
}

object {
  Ring

  pigment {
    color Red
  }
  finish {
    ambient 0.1
    diffuse 0.6
    phong 0.6
    phong_size 7
  }

  translate <-20, 15, -20>
}

torus { 35, 5 translate <-20, 15, -20>
  pigment { color Green }
  finish {
    ambient 0.1
    diffuse 0.6
    phong 0.6
    phong_size 7
  }
}

