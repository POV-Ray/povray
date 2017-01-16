// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.
//
// Persistence Of Vision Ray Tracer ('POV-Ray') sample file.
//
// Fractals pattern example fractals2.pov.
//
// Last updated: January 2017
// Author: Juha Nieminen
// Description:
// Demonstrates the use of fractal patterns.
// The fractals used are:
// - Wall: A magnet2m pigment with interior type 1.
// - Floor: A mandelbrot pigment and normal with
//   interior type 1.
//
// -w800 -h600 +a0.3

#version 3.71;
global_settings {
    assumed_gamma 1.0
    max_trace_level 5
}
#default { finish { ambient 0.006 diffuse 0.456 } }

camera {
    location <-2,5,-10>*1.4
    look_at  <0,-1,0>
    right    x*image_width/image_height
    angle    35
}

light_source { <0,0,-.1>,<1,.95,.8> }

#declare Black  = srgb <0,0,0>;
#declare Red    = srgb <1,0,0>;
#declare White  = srgb <1,1,1>;
#declare Yellow = srgb <1,1,0>;
#declare Magnet2mColor00 = srgb <0.18824,0.50196,0.90196>;
// Magnet2m:
plane {
    -z,0
    texture {
        pigment {
            magnet 2 mandel 300
            color_map {
                blend_mode 2 blend_gamma 5.5
                [0.0 rgb Black] // Black
                [0.2 rgb Red]
                [0.3 rgb Yellow]
                [0.5 rgb Magnet2mColor00]
                [1.0 rgb White]
            }
            interior 1,200000
            translate <-1.693285,-.69524>
            scale 10000
        }
        finish { ambient 0 emission 1 diffuse albedo 0.456 }
    }
}

#declare MandelColor00 = srgb <0.40000,0.18824,0.07451>;
#declare MandelColor01 = srgb <0.80392,0.40000,0.07451>;
#declare MandelColor02 = srgb <1.00000,0.80392,0.40000>;
// Mandel:
plane {
    y,-1
    texture {
        pigment {
            mandel 50
            interior 1,5
            color_map {
                blend_mode 2 blend_gamma 2.5
                [0.0 rgb MandelColor00]
                [0.3 rgb MandelColor01]
                [0.6 rgb MandelColor02]
                [1.0 rgb White]
            }
          }
          normal {
              mandel 80 1
              interior 1,5
              slope_map {
                  [0.0 <0,0>]
                  [0.5 <.5,1>]
                  [1.0 <1,0>] }
              }
          finish { diffuse 0.2 specular 1 roughness 0.025 reflection 0.6 }
          translate <0.2,-1>
          scale 10
          rotate x*90
    }
}

#declare UnionColor00 = srgb <0.5,1,0.5>;
union {
    sphere { <+3.5,-0.5,-1>,0.5 }
    sphere { <-3.5,-0.5,-1>,0.5 }
    pigment { UnionColor00 }
    finish { diffuse 0.25 specular 0.7 reflection 0.5 }
}
