// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Ray Tracer Scene Description File
// File: trace_dem.pov
// Desc: Basic Scene Example using trace(). This scene is
//intended to be run as an animation.
// Date: 2001/08/13
// Auth: Ingo Janssen
//

// +w320 +h240 +a0.3 +kfi0 +kff24

#version 3.6;

global_settings {
  assumed_gamma 1.0
}

#include "math.inc"

camera {
  location <0.0, 1.0, -12.0>
  right     x*image_width/image_height
  look_at  <0.0, 1.0,  0.0>
  angle 25
}

light_source {
  <500, 500, -500>
  color rgb <1, 1, 1>
}

#declare Ground=
difference {
   union {
      box{<-1.5,-0.6,-0.001>,<1.5,0,0.1>}
      cylinder{<-1,0,-0.003>,<-1,0,0.11>,0.5}
      cylinder{<1,0,-0.003>,<1,0,0.11>,0.5}
   }
   cylinder{<0,0,-0.003>,<0,0,0.11>,0.5}
   no_shadow
   pigment {rgb 1}
}

object {Ground}
#declare Norm=<0,0,0>;
#declare From=<-1.499+(clock*2.99),2,0>;
#declare Intersect=trace(Ground, From,<0,-1,0>,Norm);

union {
   sphere {From, 0.06}
   cylinder {From, Intersect+<0,0.5,0>, 0.025}
   cone {
      0,0,<0,0.5,0>,0.1
      translate Intersect
   }
   pigment {rgb <1,1,0>}
}

union {
   cylinder{
      Intersect, Intersect+(Norm/2), 0.025
   }
   cone {
      Intersect+(Norm)/2,0.1,Intersect+Norm,0
   }
   pigment {rgb <0,1,0>}
}

text {
  ttf
  "crystal.ttf",
  concat("Intersection: <",vstr(3,Intersect,",",0,2),">")
  0.1,
  0
  scale 0.3
  translate <-2.5,2.7,0>
  pigment {rgb <1,1,0>}
}
text {
  ttf
  "crystal.ttf",
  concat("Normal: <",vstr(3,Norm,",",0,2),">")
  0.1,
  0
  scale 0.3
  translate <-2.5,2.3,0>
  pigment {rgb <0,1,0>}
}
