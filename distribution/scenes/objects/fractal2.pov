// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3
#version 3.7;
global_settings { 
  assumed_gamma 1.0
  max_trace_level 5
}
#default{ finish{ ambient 0.1 diffuse 0.9 }} 

#include "colors.inc"
#include "textures.inc"
#include "stones.inc"

#declare PREC = 20;

camera {
    location <0.0, 0.0, -6.0>
    right     x*image_width/image_height
    angle 49  
    look_at <0,0.5,0>
}

background { rgb 0.5 }

light_source { <4.3, 4.5, -3.0>  rgb <0.7, 0.75, 0.7>*0.5 }
light_source { <-3.3, 5.5, -1.0> rgb <0.4, 0.45, 0.3>*0.5 }

julia_fractal {
    <0.2, 0.1, 0.59, -0.2>
    max_iteration 7
    precision PREC
    sqr
    hypercomplex
    texture {
        pigment { rgb <0.8, 0.65, 0.85> }
        finish { phong 0.7 phong_size 100 }
    }
    scale 1.2
    rotate < 83, 0, -70>
    translate <-1.3,1.5, 0>
}

julia_fractal {
    <0.02, -0.02, 0.8, 0>
    max_iteration 10
    precision PREC
    sqr
    quaternion

    interior {ior 1.5}
    texture {
        pigment { rgbf <0.8, 0.3, 0.2, 0.9>  }
        finish {
            phong 0.3
            phong_size 200
            reflection 0.1
        }
    }
    rotate <40, -30, 120>
    translate <0, 1.12, 0>
    scale 1
    translate <-1.2,-1.45,0>

}

julia_fractal {
    <0.33, 0.54, 0.52, 0.32>
    max_iteration  7
    precision PREC
    cube
    quaternion

    interior {ior 1.2}
    texture {
        pigment { rgbf <0.9, 0.6, 0.2, 0.9>  }
        finish {
            phong 0.3
            phong_size 200
            reflection 0.1
        }
    }
    scale 1
    rotate <-80, 30, -120>
    translate <1.35,1.45,0>
}
 
intersection {
    julia_fractal {
        <-0.54, 0.57, 0.0, -0.37>
        max_iteration 8
        precision PREC
        cube
        texture {
            pigment { rgb <0.5, 0.3, 0.5> }
            scale 0.7
            finish { reflection 0.1 }
        }
    }
    box { <-3, -3, -3>, <3, 3, 0.2> texture { T_Stone11 } }

    rotate <0, 30, 110>
    scale 1
    translate <1.2,-0.45,0>
}
   
background { color rgb<1,1,1>*1 } 

