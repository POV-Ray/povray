// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.
//
// Persistence Of Vision Ray Tracer ('POV-Ray') sample file.
//
// Aoi pattern example aoi.pov.
//
// +w800 +h800 +am2 +r4 +a0.1

#version 3.8;
global_settings { assumed_gamma 1 }
#declare Black = srgb <0,0,0>;
background { color Black }
#declare CameraPerspective = camera {
    perspective
    location <2.7,2.7,-2.701>
    sky <0,1,0>
    angle 35
    right x*(image_width/image_height)
    look_at <0,0,0>
}
// orthographic so camera rays parallel +y toward x,z plane.
#declare CameraOrthoY = camera {
    orthographic
    location <0,2,0>
    direction <0,-1,0>
    right 2.1*x*max(1,image_width/image_height)
    up 2.1*<0,0,max(1,image_height/image_width)>
}

// First object a sphere at origin
#declare Sphere00 = sphere { <0,0,0>, 0.5 }
#declare Red = srgb <1,0,0>;
#declare Azure = srgb <0,0.498,1>;
// Azure in map to show values not seen with aoi pattern.
#declare ColorMapSphere = color_map {
 // blend_mode 2 blend_gamma 0.5 // Uncomment one of these for
    blend_mode 2 blend_gamma 2.5 // fun with new 3.8 color_map
 // blend_mode 2 blend_gamma 5.5 // blend_mode, blend_gamma.
    [ 0 Azure ]
    [ 0.49 Azure ]
    [ 0.5 Black ]
    [ 0.75 Red ]
    [ 1 Black ]
}
#declare PigmentSphere = pigment {
    aoi
    color_map { ColorMapSphere }
}
#declare FinishEmission1 = finish {
    ambient srgb <0,0,0>
    diffuse 0
    emission srgb <1,1,1>
}
#declare TextureSphere = texture {
    pigment { PigmentSphere }
    finish { FinishEmission1 }
}
#declare ObjectSphere = object {
    Sphere00
    texture { TextureSphere }
}

// Second object is a tiling pattern based isosurface.
// Tiling normalization scalings are documented in tiling.pov example scene.
#declare VarTiling21_NrmScale = <1/(1+sqrt(3)),1,1/(1+sqrt(3))>;
#include "functions.inc"
#declare FnTiling = function {
    pattern {
        tiling 21 sine_wave frequency 1/sqrt(2)
        scale VarTiling21_NrmScale*0.5
    }
}
#declare FnDisplaceYbyTiling = function (x,y,z) {
    y+(FnTiling(x,y,z)*0.1)
}
#declare IsosurfaceTiling = isosurface {
    function { FnDisplaceYbyTiling(x,y,z) }
    contained_by { box { <-1,-0.2,-1>,<1,0.02,1> } }
    threshold 0
    accuracy 0.0005
    max_gradient 1.1
    max_trace 1
}
// Azure in map to show values not seen with aoi pattern.
#declare ColorMapIsosurfaceTiling = color_map {
    [ 0 Azure ]
    [ 0.49 Azure ]
    [ 0.5 Red ]
    [ 1 Black ]
}
#declare PigmentIsosurfaceTiling = pigment {
    aoi
    color_map { ColorMapIsosurfaceTiling }
}
#declare TextureIsosurfaceTiling = texture {
    pigment { PigmentIsosurfaceTiling }
    finish { FinishEmission1 }
}
#declare ObjectIsosurfaceTiling = object {
    IsosurfaceTiling
    texture { TextureIsosurfaceTiling }
}

//---
camera { CameraOrthoY }
object { ObjectSphere }
object { ObjectIsosurfaceTiling }

// Use CameraPerspective over CameraOrthoY to better see shapes.
// Note. Maximum gradient warning with CameraPerspective is expected.
// camera { CameraPerspective }

