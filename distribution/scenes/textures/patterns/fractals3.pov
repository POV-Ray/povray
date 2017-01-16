// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.
//
// Persistence Of Vision Ray Tracer ('POV-Ray') sample file.
//
// Fractals pattern example fractals3.pov.
//
// Last updated: January 2017
// Author: Juha Nieminen
// Description:
// Demonstrates the use of fractal patterns.
// Three parts of the Mandelbrot fractal forming an acronym.
//
// +W800 +H267 +a0.1

#version 3.71;
global_settings { assumed_gamma 1.0 }
#default { finish { ambient 0.006 diffuse 0.456 } }

#declare Black    = srgb <0,0,0>;
#declare Cyan     = srgb <0,1,1>;
#declare Green    = srgb <0,1,0>;
#declare Red      = srgb <1,0,0>;
#declare White    = srgb <1,1,1>;
#declare Yellow   = srgb <1,1,0>;
#declare P = box {
    <-2,-2,0><2,2,0.1>
    texture {
        pigment {
            mandel 1000
            color_map {
                blend_mode 2 blend_gamma 2.5
                [0.0 rgb Black]
                [0.5 rgb Cyan]
                [1.0 rgb Yellow]
                [1.0 rgb Black]
            }
            translate -<-0.7653,0.1005>
            scale 1000
            scale <-1,1,1>
        }
        finish { ambient 0 emission 1 }
    }
}

#declare O = box {
    <-2,-2,0><2,2,.1>
    texture {
        pigment {
            mandel 10000
            color_map {
                blend_mode 2 blend_gamma 2.5
                [0.0 rgb Black]
                [0.5 rgb Red]
                [1.0 rgb Yellow]
                [1.0 rgb Black]
            }
            translate
            -<(-0.749979169204317290207435344827-
                0.749968488051973540207435344827)/2,
              (0.008640613399268010370560473549+
               0.008630313716650822870560473549)/2>
            scale 350000
            rotate z*-45
         }
         finish { ambient 0 emission 1 }
    }
}

#declare V = box {
    <-2,-2,0><2,2,.1>
    texture {
        pigment {
            mandel 300
            color_map {
                blend_mode 2 blend_gamma 2.0
                [0.1 rgb Black]
                [0.2 rgb Black]
                [0.5 rgb Green]
                [1.0 rgb White]
                [1.0 rgb Black]
            }
            translate
            -<(-0.596201137068877025831455721002-
                0.596168159011015697706455721002)/2,
              (0.665096856611702768182969470741+
              0.665064938141339606920170835929)/2>
            scale 130000
            rotate z*-40
        }
        finish { ambient 0 emission 1 }
    }
}

camera {
    right    x*3
    location -z*4
    look_at  0
}

object { P translate -x*4 }
object { O }
object { V translate x*4 }

