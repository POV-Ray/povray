// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.
//
// Persistence Of Vision Ray Tracer ('POV-Ray') sample file.
//
// Aoi pattern example aoi.pov.
//
// +w800 +h600 +a0.3

#version 3.71;
global_settings { assumed_gamma 1 }
#default { finish { ambient 0.006 diffuse 0.456 } }
#declare Black = srgb <0,0,0>;
background { color Black }

// orthographic so camera rays parallel +y toward x,z plane.
#declare Camera01y = camera {
    orthographic
    location <0,2,0>
    direction <0,-1,0>
    right 2.1*x*max(1,image_width/image_height)
    up 2.1*<0,0,max(1,image_height/image_width)>
}
// First object a sphere at origin
#declare Sphere00 = sphere { <0,0,0>, 0.5
}
#declare Red = srgb <1,0,0>;
#declare Azure = srgb <0,0.5,1>;
#declare ColorMap00 = color_map {
    // blend_mode 2 blend_gamma 0.5 // Uncomment one of these
    // blend_mode 2 blend_gamma 2.5 // for fun with blend
    // blend_mode 2 blend_gamma 5.5
    [ 0 Azure ]
    [ 0.49 Azure ]
    [ 0.5 Black ]
    [ 0.75 Red ]
    [ 1 Black ]
}
#declare Pigment00 = pigment {
    aoi
    color_map { ColorMap00 }
}
#declare Finish00 = finish {
    ambient srgb <0,0,0>
    diffuse 0
    emission srgb <1,1,1>
}
#declare Texture00 = texture {
    pigment { Pigment00 }
    finish { Finish00 }
}
#declare Object00 = object {
    Sphere00
    texture { Texture00 }
}
// Second object tiling pattern based isosurface around x,z plane.
#declare VarTiling21_NrmScale = <1/(1+sqrt(3)),1,1/(1+sqrt(3))>;
#include "functions.inc"
#declare Fn01a = function {
    pattern { tiling 21 sine_wave frequency 1/sqrt(2)
              warp { turbulence 0.1 }
              scale VarTiling21_NrmScale*0.5
    }
}
#declare Fn01b = function (x,y,z) {
    y+(Fn01a(x,y,z)*0.1)
}
#declare Iso01 = isosurface {
    function { Fn01b(x,y,z) }
    contained_by { box { <-1,-0.2,-1>,<1,0.2,1> } }
    threshold 0
    accuracy 0.0005
    max_gradient 1.1
    max_trace 1
}
#declare ColorMap01 = color_map {
    [ 0 Azure ]
    [ 0.49 Azure ]
    [ 0.5 Red ]
    [ 1 Black ]
}
#declare Pigment01 = pigment {
    aoi
    color_map { ColorMap01 }
}
#declare Texture01 = texture {
    pigment { Pigment01 }
    finish { Finish00 }
}
#declare Object01 = object {
    Iso01
    texture { Texture01 }
}

//---
camera { Camera01y }
object { Object00 }
object { Object01 }
