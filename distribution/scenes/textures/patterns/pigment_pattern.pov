// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// this scene demonstrates a possible use of the pigment_pattern pattern

// first, we'll define a reasonably complex pigment, made of wrinkles
// and leopard pigments mapped within a bozo pattern in another pigment.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

global_settings { assumed_gamma 1.0 }

#include "colors.inc"

//the two basic pigments
#declare Pig1 = pigment {
 leopard
 color_map {
  [0 SteelBlue]
  [1 Yellow]
 }
 sine_wave
 scale .05
}

#declare Pig2 = pigment {
 wrinkles
 color_map {
  [0.0 Orange]
  [0.5 White]
  [1.0 White]
 }
}

//the complex pigment
#declare Pig3 = pigment {
 bozo
 pigment_map {
 [0 Pig1]
 [1 Pig2]
 }
 triangle_wave
 scale 1.5
}

//the first, greenish, sphere shows the complex pigment as is
sphere {<0,4,0>,4 pigment {Pig3}
                  normal {pigment_pattern {Pig3} 10 }
       }

//the second, gray, sphere shows how the complex pigment becomes
//a new pattern with values from 0 to 1
sphere {<0,4,0>,4
        pigment {pigment_pattern {Pig3}}
        normal {pigment_pattern {Pig3} 3.4}
        translate z*12
        }

//and the big torus shows the new pattern with an orange color_map,
//and an added normal using the same pattern to create visible craters
//on the surface, following the complex pattern.
torus {7 2
        pigment {pigment_pattern {Pig3}
                 color_map {
                  [0 OrangeRed*.2]
                  [1 OrangeRed*1.5]
                  }
                 }
        normal {pigment_pattern {Pig3} 3.4}
        finish {phong .8 phong_size 10}
        translate y*2
        }

//some usual scene elements
camera { location  <10,20,10> 
         right     x*image_width/image_height
         look_at   <0,2,4> 
         angle     50
       }
plane {y,0 pigment {rgb <.2,.4,.3>}}
light_source {<20,30,40> White*1.2}
light_source {<-20,30,-40> Wheat*.5 shadowless}
