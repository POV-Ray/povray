// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer Scene Description File
// File: grenadine.pov
// Desc: Glass with liquid
// Date: 1999/06/04
// Auth: Ingo Janssen
// Updated: 2013/02/15 for 3.7
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

#include "glass.inc"
#include "lemon.inc"

global_settings {
   assumed_gamma 1.0
   max_trace_level 5
   photons {
      spacing 0.01  // higher value 'lower' quality, faster parsing.
      autostop 0
      jitter 0.5
      max_trace_level 15
  }
}

light_source {
  <500, 550, -100>
  rgb <1, 1, 1>
  spotlight
  radius 1
  falloff 1.1
  tightness 1
  point_at  <-19,-4,7>
}

camera {
  location  <-0.5, 2.5, -7.0>
  right     x*image_width/image_height // keep propotions with any aspect ratio
  look_at   <-0.5, 0.5,  0.0>
}

sky_sphere {
  pigment {
    gradient y
    color_map { [0.0 rgb <0.2,0,1>] [1.0 color rgb 1] }
  }
}

union {                     //plane & background
   difference {
      box {<-20,-1,0>,<20,13,13>}
      cylinder{<-21,13,0>,<21,13,0>,13}
   }
   plane {y, 0}
   translate <0,-1.9999,7>
   pigment {rgb .5}
   finish {diffuse .5 ambient 0}
}

//====== The Glass ======

#declare Ri=0.95;

#declare Glass= merge {
   difference {
      cylinder { -y*2,y*2,1 }
      sphere {-y*0.8,Ri}
      cylinder { -y*0.8,y*2.01,Ri}
      sphere {-y*1.9,0.1}
   }
   torus {0.975, 0.026 translate <0,2,0>}
   // texture {T_Glass1}
   // interior {ior 1.5}
   // converted to material 26Sep2008 (jh)
   material {
     texture {
       pigment {color rgbf<1.0, 1.0, 1.0, 0.7>}
       finish {F_Glass1}
       }
     interior {ior 1.5}
     }
}


//====== The bubbles and the juce ======

#declare Bubble= difference {
   sphere {0,0.1}
   sphere {0,0.09999999}
}

#declare S= seed(7);
#declare I=0;
#declare Bubbles= intersection {
   union {
      #while (I<60)
         object {
            Bubble
            scale rand(S)
            scale <1,0.7,1>
            translate <1,0.6,0>
            rotate <0,360*rand(S),0>
         }
         object {
            Bubble
            scale rand(S)*0.5
            translate <rand(S),0.58,0>
            rotate <0,360*rand(S),0>
         }
         #declare I=I+1;
      #end //while
   }
   cylinder{y*0.5,y*0.85,Ri+0.00000001}
}

#declare Liquid= merge {
   sphere {-y*0.8,Ri+0.00000001}
   cylinder {-y*0.8,y*0.6,Ri+0.00000001}
   object {Bubbles}
   pigment {rgbf <0.9, 0.1, 0.2, 0.95>}
   finish {reflection 0.3}
   interior{ior 1.2}
}

//====== The glass and juice =====
union {
   object {Glass}
   object {Liquid}
   photons {
      target
      refraction on
      reflection on
      collect off
   }
}

object {
   LemonSlice
   scale <0.8,0.8,1>
   translate <-0.99,0,0>
   rotate <0,-30,0>
   translate <0,2,0>
   photons {
      pass_through
   }
}


