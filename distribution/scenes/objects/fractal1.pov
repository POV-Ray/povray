// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"

camera {
    location  <0.0, 0.0, -12.0>
    angle 40  
    right     x*image_width/image_height
    look_at   <0.0, 1.5, 0.00>
}

light_source { <-20.00, 30.00, -100.00> color Coral }
light_source { <200.00, 300.00, -500.00> color Wheat }

#default { pigment { White } finish { Shiny }}

#declare PREC = 15;

#declare Z2_1 =
julia_fractal {
    <-0.083,0.0,-0.83,-0.025>
    quaternion
    sqr
    max_iteration 8
    precision PREC
}

#declare Z2_2 =
julia_fractal {
    <-0.03,0.5,-0.2,-0.5>
    quaternion
    sqr
    max_iteration 8
    precision PREC
}

#declare Z3_1 =
julia_fractal {
    <-0.083,0.0,-0.83,-0.025>
    max_iteration 8
    precision PREC
    quaternion
    cube
}

#declare Z3_2 =
julia_fractal {
    <-0.03,0.5,-0.2,-0.5>
    max_iteration 8
    precision PREC
    quaternion
    cube
}

object { Z2_1             translate <-2, 3,  0> }
object { Z2_2 rotate y*90 translate < 2, 3,  0> }
object { Z3_1 scale 1.5   translate <-2, 0,  0>  }
object { Z3_2 scale 1.5   translate < 2, 0,  0>  }

