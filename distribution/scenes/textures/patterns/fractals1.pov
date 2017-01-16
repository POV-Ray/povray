// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.
//
// Persistence Of Vision Ray Tracer ('POV-Ray') sample file.
//
// Fractals pattern example fractals1.pov.
//
// Last updated: January 2017
// Author: Juha Nieminen
// Description:
// Demonstrates the use of fractal patterns.
// The fractals used are:
// - Floor: magnet1m as both pigment and normal with
//   interior type 1 in both and exterior type 5 in pigment.
// - Left box: A julia pigment with interior type 1.
// - Middle box: A mandelbrot pigment with interior type 5
//   and exterior type 6.
// - Right box: A mandel4 pigment with interior type 1
//   and exterior type 5.
// - Sphere: A julia4 normal.
//
// -w800 -h600 +a0.3

#version 3.71;
global_settings {
    assumed_gamma 1.0
    max_trace_level 5
}
#default { finish { ambient 0.006 diffuse 0.456 } }

camera {
    location <-1,2,-5>*1.2
    right    x*image_width/image_height
    look_at  <0,0.2,0>
    angle    35
}

light_source { <10,50,-30>, 1 }

#declare FloorColor00 = srgb <1.0,0.8,0.5>;
#declare FloorColor01 = srgb <0.8,0.5,0.4>;
// Floor made with magnet1m:
plane {
    -z,0
    texture {
        pigment {
            magnet 1 mandel 50
            color_map {
                blend_mode 2 blend_gamma 2.5
                [0 FloorColor00]
                [1 FloorColor01]
            }
            interior 1,200
            exterior 5,1
        }
        normal {
            magnet 1 mandel 50 -0.2
            slope_map {
                [0 <1,0>]
                [1 <0,-1>]
            }
            interior 1,200
            accuracy .0005
        }
        finish { specular 0.73 reflection 0.5 }
        translate <-1.9,-1,0>
        scale 20
    }
    rotate x*90
}

#declare JuliaColor00 = srgb <0.29412,0.18824,0.07451>;
#declare JuliaColor01 = srgb <1,0,0>;
#declare JuliaColor02 = srgb <1,1,0>;
#declare JuliaColor03 = srgb <1,1,1>;
// Julia:
box {
    <-2,-2,0><2,2,-0.1>
    texture {
        pigment {
            julia <0.3,0.44> 30
            interior 1,1
            color_map {
                blend_mode 2 blend_gamma 2.5
                [0.00 JuliaColor00]
                [0.25 JuliaColor01]
                [0.50 JuliaColor02]
                [1.00 JuliaColor03]
            }
            scale 1.3
        }
    }
    translate y*2
    scale 0.4
    rotate <30,-20,0>
    translate <-1.1,0,2>
}

#declare MandelColor00 = srgb <0,0,0>;
#declare MandelColor01 = srgb <0,0,1>;
#declare MandelColor02 = srgb <0.07451,0.80392,1>;
#declare MandelColor03 = srgb <1,1,1>;
// Mandel:
box {
    <-2,-2,0><2,2,-0.1>
    texture {
        pigment {
            mandel 10
            color_map {
                blend_mode 2 blend_gamma 2.5
                [0.00 MandelColor00]
                [0.25 MandelColor01]
                [0.50 MandelColor02]
                [1.00 MandelColor03]
            }
            interior 5,2
            exterior 6,0.05
            translate x*0.6
            scale 1.3
        }
    }
    translate y*2
    scale 0.4
    rotate <30,0,0>
    translate <0.5,0,2.2>
}

#declare Mandel4Color00 = srgb <0,0,0>;
#declare Mandel4Color01 = srgb <0,1,0>;
#declare Mandel4Color02 = srgb <0.50196,1,0>;
#declare Mandel4Color03 = srgb <1,1,0>;
// Mandel4:
box {
    <-2,-2,0><2,2,-.1>
    texture {
        pigment {
            mandel 50
            exponent 4
            interior 1,.5
            exterior 5,.01
            color_map {
                blend_mode 2 blend_gamma 2.5
                [0.0 Mandel4Color00]
                [0.3 Mandel4Color01]
                [0.6 Mandel4Color02]
                [1.0 Mandel4Color03]
            }
            scale 1.3
        }
    }
    translate y*2
    scale 0.4
    rotate <30,20,0>
    translate <2,0,1.6>
}

#declare Julia4Color00 = srgb <1,1,1>;
// Julia4:
sphere {
    0,2
    texture {
        pigment { Julia4Color00 }
        normal
        { julia <-.5,.5> 10 1
          exponent 4
          slope_map {
              [0.0 <0.0,0>]
              [0.5 <0.5,1>]
              [1.0 <1.0,0>]
          }
          scale 1.5
          rotate x*30
        }
        finish { specular 0.73 reflection 0.4 }
    }
    translate y*2
    scale 0.15
    translate <-0.2,0,-1>
}
