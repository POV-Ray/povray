// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Demo scene for the "no_reflection" object modifier
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0}

#include "colors.inc"
#include "thingy.inc"

camera { location <100,100,100> 
         right     x*image_width/image_height
         angle 65 //  direction z*1 
         look_at <0,0,0>
       }

plane {y,0 pigment {SteelBlue}}

object {Thingy
        scale .4 translate <-40,0,30> pigment {rgb <1,1,.5>}
        no_reflection
        }

object {Thingy
        scale .4 translate <40,0,30> pigment {rgb <1,.5,.5>}
        }

plane {z,0 pigment {SteelBlue} finish {reflection {.5}} normal {bumps .02 scale 5}}

text {ttf "cyrvetic","no_reflection !",.05,0 scale 15 rotate <0,-180,0> translate <-10,30,.5> pigment {rgb <1,1,.5>} finish {ambient 1 diffuse 0}}


light_source {<400,500,130> White*2}
