// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// This file demonstrates some uses of the "crackle solid" pattern,
// with a fake-granite tiled floor, and a zinc bucket.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

global_settings { assumed_gamma 1.0 }

#include "colors.inc"
#include "woods.inc"

camera { location <70,100,60> 
         right   x*image_width/image_height
         angle 40 
         look_at <0,15,0>
       }

light_source {<40,350,300> White*1.5 spotlight point_at 0 radius 9 falloff 11}

//--textures definitions--------------------------------

#declare Stone1 = texture {
 pigment {crackle solid
          color_map {[0 Black][.2 Wheat*.5][.4 Black][.6 Wheat*.5][.8 Black][1 Wheat*.5]}
          }
 finish {phong .7 reflection {.4}}
 normal {bumps .1 scale .2}
 scale .1
}

#declare Stone2 = texture {
 pigment {crackle solid
          color_map {[0 Red][.2 Wheat][.4 Tan][.6 Wheat][.8 Red*.5][1 Wheat]}
          }
 scale .05
}

#declare Zinc1 = texture {
 pigment {Gray80}
 finish {phong .7 reflection {.3}}
 normal {bumps .002 scale <.1,.1,10>}
}

#declare Zinc2 = texture {
 pigment {Gray90}
 finish {phong .6 reflection {.05}}
 normal {bumps .002 scale <.1,.1,10> rotate y*90}
}

#declare Zinc3 = texture {
 pigment {Gray60}
 finish {phong .5 reflection {.3}}
 normal {bumps .002 scale <.1,.1,10> rotate y*180}
}

#declare Zinc = texture {
 crackle solid
 texture_map {
  [0.0 Zinc1]
  [0.5 Zinc2]
  [1.0 Zinc3]
 }
 scale 1
}

//--floor--------------------------------
plane {y,0
       texture {checker texture {Stone1} texture {Stone2} scale 20 translate y*10}
       }

//--bucket--------------------------------
union {
 cylinder {<0,0,0>,<0,2,0>,15}
 difference {
  cone {<0,2,0>,15,<0,40,0>,20}
  cone {<0,2,0>,14.9,<0,41,0>,19.9}
 }
 torus {20 .4 translate y*40}
  union {
   torus {22 .4 clipped_by {plane {x,0}}}
   cylinder {<0,0,18>,<0,0,22>,.4}
   sphere {<0,0,22>,.4}
   cylinder {<0,0,-18>,<0,0,-22>,.4}
   sphere {<0,0,-22>,.4}
   cylinder {<-21.5,0,-5>,<-21.5,0,5>,1.5 texture {T_Wood21 scale 20}}
   rotate z*150
   translate y*35}
 texture {Zinc}
 translate z*15
}
