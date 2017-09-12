// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.
//
// Persistence Of Vision Ray Tracer ('POV-Ray') sample file.
//
// Potential pattern example potential.pov.
//
// +w800 +h600 +a0.3

#version 3.8;
global_settings { assumed_gamma 1 }
#default { finish { ambient 0.005 diffuse 0.45 } }
#declare Grey50 = srgb <0.5,0.5,0.5>;
background { color Grey50 }
#declare Camera00 = camera {
    perspective
    location <2.7,2.7,-2.701>
    sky <0,1,0>
    angle 35
    right x*(image_width/image_height)
    look_at <0,0,0>
}
#declare White = srgb <1,1,1>;
#declare Light00 = light_source {
    <50,150,-250>, White
}
#declare Orange = srgb <1,0.5,0>;
#declare CylinderX = cylinder {
    <-2,0,0>, <2,0,0>, 0.01
    pigment { color Orange }
}
#declare Cyan = srgb <0,1,1>;
#declare CylinderY = cylinder {
    <0,-2,0>, <0,2,0>, 0.01
    pigment { color Cyan }
}
#declare Blue = srgb <0,0,1>;
#declare CylinderZ = cylinder {
    <0,0,-2>, <0,0,2>, 0.01
    pigment { color Blue }
}

#declare Rose = srgb <1,0,0.498>;
#declare BlobLeft = blob {
    threshold 0.5
    cylinder { <-0.5,-0.5,-0.5>, <-0.5,0.5,-0.5>, 0.2, 1 }
    pigment { color Rose }
}
#declare BlobCenter = blob {
    threshold 0.5
    cylinder { <0.5,-0.5,-0.5>, <0.5,0.5,-0.5>, 0.2, 1 }
}
#declare BlobRight = blob {
    threshold 0.5
    cylinder { <0.5,-0.5,0.5>, <0.5,0.5,0.5>, 0.2, 1 }
}
// Add: object { UnionBlobs } at bottom and comment other
// objects there to see these three initial blobs.
#declare UnionBlobs = union {
    object { BlobLeft }
    object { BlobCenter }
    object { BlobRight }
    pigment { color Rose }
}


#declare Box00 = box {
    <-0.707,-0.707,-0.707>,<0.707,0.707,-0.407>
    rotate y*-45
}
#declare IntersectionCenter = intersection {
    object { Box00 }
    object { BlobCenter }
}
#declare Red = srgb <1,0,0>;
#declare Green = srgb <0,1,0>;
#declare Azure = srgb <0,0.498,1>;
// Color map must align with potential pattern 'threshold on'.
#declare ColorMap00 = color_map {
    [ 0 Red ]
    [ 0.5 Green ]
    [ 0.51 Azure ]
    [ 1 Azure ]
}
// Use center blob's potential to control pigment's color_map.
#declare PigmentBlobCenterPotential = pigment {
    potential { BlobCenter } threshold on
    color_map { ColorMap00 }
}
#declare TextureBlobCenterPotential = texture {
    pigment { PigmentBlobCenterPotential }
}
#declare ObjectBlobCenterPotential = object {
    IntersectionCenter
    texture { TextureBlobCenterPotential }
}

// Turning the right blob into an isosurface via
// the potential pattern.
#declare Brown = srgb <0.5882,0.2941,0>;
#include "functions.inc"
#declare FnBlobRightPotential = function {
    pattern { potential { BlobRight } threshold on }
}
#declare IsoOfBlobRightPotential = isosurface {
    function { FnBlobRightPotential(x,y,z) }
    contained_by { box { <0,-1.1,0>,<1.1,1.1,1.1> } }
    polarity 1
    threshold 0
    accuracy 0.0005
    max_gradient 8.1
    max_trace 1
    pigment { color Brown }
}

#declare VarSphereRadius = 0.1;
#declare FnSphere = function (x,y,z) {
    f_sphere(x-0.1,y-0.5,z+0.1,VarSphereRadius)
}
// Following isosurface used only in PigmentIsoSpherePotential below.
// Threshold is negated f_sphere radius - no isosurface shape forms.
#declare IsoSphere = isosurface {
    function { FnSphere(x,y,z) }
    contained_by { box { <-0.2,0.3,-0.2>,<0.2,0.7,0.2> } }
    threshold -VarSphereRadius
    accuracy 0.0005
    max_gradient 2.1
    max_trace 1
}
#declare Box01 = box {
    <-0.1707,-0.2707,-0.1707>,<0.1707,0.9707,-0.1706>
    rotate y*-45
}
// Access function FnSphere as potential via IsoSphere isosurface
// to control pigment's color_map for Box01.
#declare PigmentIsoSpherePotential = pigment {
    potential { IsoSphere } threshold on
    color_map { ColorMap00 }
}
#declare TextureIsoSpherePotential = texture {
    pigment { PigmentIsoSpherePotential }
}
#declare ObjectIsoSpherePotential = object {
    Box01
    texture { TextureIsoSpherePotential }
}

//---
camera { Camera00 }
light_source { Light00 }
object { CylinderX }
object { CylinderY }
object { CylinderZ }
object { BlobLeft } // Shown as match to IsoOfBlobRightPotential
object { ObjectBlobCenterPotential }
object { IsoOfBlobRightPotential }
object { ObjectIsoSpherePotential }

