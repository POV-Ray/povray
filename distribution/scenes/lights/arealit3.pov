// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Illustrates how adaptive level can affect soft shadows
//
// -w320 -h240
// -w800 -h600 +a0.3

// The shadow on the left uses "adaptive 0" and shows some major shadow
// artifacts causes by undersampling. As the adaptive level increases
// the shadow become much more accurate. Note: This is a worst-case
// example.

// Left   shadow - adaptive 0 (renders fastest)
// Middle shadow - adaptive 1
// Right  shadow - adaptive 2 (renders slowest)

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

#declare Object = difference {
    box { <-6, -6, 0>, <6, 6, 0.5> rotate 45*z }
    box { <-2.5, -2.5, -1>, <2.5, 2.5, 1> rotate 45*z }
    pigment { Red }
    finish { Shiny }
}

object { Object translate -SpacingX*x }

light_source {
    <0, LightY, LightZ> color White
    area_light <15, 0, 0>, <0, 15, 0>, 17, 17
    adaptive 0
    jitter

    spotlight
    point_at <-SpacingX, 0, 0>
    tightness 0
    radius SRadius
    falloff SFalloff
}

object { Object translate 0*x }

light_source {
    <0, LightY, LightZ> color White
    area_light <15, 0, 0>, <0, 15, 0>, 17, 17
    adaptive 1
    jitter

    spotlight
    point_at <0, 0, 0>
    tightness 0
    radius SRadius
    falloff SFalloff
}

object { Object translate SpacingX*x }

light_source {
    <0, LightY, LightZ> color White
    area_light <15, 0, 0>, <0, 15, 0>, 17, 17
    adaptive 2
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
