// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Demo scene for the "double_illuminate" object modifier
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0}

#include "colors.inc"
#include "thingy.inc"

camera { location <100,100,100> 
         right     x*image_width/image_height
         angle 65 // direction z*1 
         look_at <0,0,0>
       }

object {Thingy
        scale .4 translate <-60,0,-10> pigment {rgb <1,1,.5>}
        no_reflection
        }

box {<0,0,1>,<.1,60,60> pigment {White filter .7} double_illuminate}
box {<0,0,-1>,<.1,60,-60> pigment {White filter .7}}

plane {y,0 pigment {SteelBlue}}

text {ttf "cyrvetic","double_illuminate",.05,0 scale 10 rotate <90,-180,0> translate <80,.1,20> pigment {rgb <1,1,.5>} finish {ambient 1 diffuse 0}}
text {ttf "cyrvetic","(default)",.05,0 scale 10 rotate <90,-180,0> translate <45,.1,-20> pigment {rgb <1,1,.5>} finish {ambient 1 diffuse 0}}


light_source {<-200,30,0> White*1.5}
light_source {<400,500,300> White shadowless}
