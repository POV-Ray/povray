// Persistence Of Vision raytracer version 3.1 sample file.


global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "textures.inc"
#include "stones.inc"

#declare PREC = 10;

camera {
    location <4.0, 3.0, -1.0>
    up   y
    right x * 1.33
    direction z
    look_at <0,0,0>
}

background { rgb 0.5 }

light_source { <4.3, 4.5, -3.0> rgb <0.7, 0.75, 0.7> }
light_source { <-3.3, 5.5, -1.0> rgb <0.4, 0.45, 0.3> }

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
    scale 0.5
    rotate <-23, 213, -153>
    translate <1, 0.5, 1>
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
            reflection 0.4
        }
    }
    rotate <40, -30, 120>
    translate <0, 1.12, 0>
    scale 0.5
    translate <-1, 0.5, 1>

}

julia_fractal {
    <0.33, 0.54, 0.52, 0.32>
    max_iteration  7
    precision PREC
    cube
    quaternion
 
    interior {ior 1.2}
    texture {
        pigment { rgbf <0.8, 0.3, 0.2, 0.9>  }
        finish {
            phong 0.3
            phong_size 200
            reflection 0.4
        }
    }
    scale 0.5
    rotate <-80, 30, -120>
    translate <1, 0.5, -1>
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
            finish { reflection 0.4 }
        }
    }
    box { <-3, -3, -3>, <3, 3, 0.2> texture { T_Stone11 } }

    rotate <0, 30, 110>
    scale 0.5
    translate <-1, 0.5, -1>
}


plane { y, -0.5
    texture {
//        T_Stone13
        pigment { White }
        finish { diffuse 0.5 ambient 0.2 }
        scale 0.5
    }
}

