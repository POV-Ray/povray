// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Illustrates how area light grid size affects soft shadows
//
// -w320 -h240
// -w800 -h600 +a0.3

// Left   shadow - area_light <8,0,0>, <0,0,8>,  3,  3 (renders fastest)
// Middle shadow - area_light <8,0,0>, <0,0,8>,  5,  5
// Right  shadow - area_light <8,0,0>, <0,0,8>, 17, 17 (renders slowest)

#version 3.7;

global_settings {
  assumed_gamma 1
}

#include "colors.inc"
#include "textures.inc"

// A back wall to cast shadows onto
plane { -z, -20
    pigment { Black }
    finish { Dull }
}

#declare SpacingX = 20;
#declare Radius = 5;
#declare LightX = 15;
#declare LightY = 40;
#declare LightZ = -40;
#declare SRadius = 0;
#declare SFalloff = 11;

#declare SphereTexture = texture {
    pigment { Red }
    finish { Shiny }
}

sphere { <-SpacingX, 0, 0>, Radius
    texture { SphereTexture }
}

light_source {
    <0, LightY, LightZ> color White
    area_light <8, 0, 0>, <0, 8, 0>, 3, 3
    adaptive 0
    jitter

    spotlight
    point_at <-SpacingX, 0, 0>
    tightness 0
    radius SRadius
    falloff SFalloff
}

sphere { <0, 0, 0>, Radius
    texture { SphereTexture }
}

light_source {
    <0, LightY, LightZ> color White
    area_light <8, 0, 0>, <0, 8, 0>, 5, 5
    adaptive 0
    jitter

    spotlight
    point_at <0, 0, 0>
    tightness 0
    radius SRadius
    falloff SFalloff
}

sphere { <+SpacingX, 0, 0>, Radius
    texture { SphereTexture }
}

light_source {
    <0, LightY, LightZ> color White
    area_light <8, 0, 0>, <0, 8, 0>, 17, 17
    adaptive 0
    jitter

    spotlight
    point_at <+SpacingX, 0, 0>
    tightness 0
    radius SRadius
    falloff SFalloff
}

light_source { <0, -15, -120> color Gray10 }

camera {
    location <0, -15, -120>
    right     x*image_width/image_height
    angle 35 // direction 2*z
    look_at <0, -15, 0>
}
