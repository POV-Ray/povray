// Persistence Of Vision raytracer version 3.1 sample file.


global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"

camera {
    location <0.0, 0.0, -12.0>
    direction z*1.3
    up y
    right x*1.33
    look_at <0.0, 0.0, 0.00>
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

object { Z2_1             translate <-3, 3,  0> }
object { Z2_2 rotate y*90 translate < 3, 3,  0> }
object { Z3_1 scale 1.5   translate <-3, 0,  0>  }
object { Z3_2 scale 1.5   translate < 3, 0,  0>  }

