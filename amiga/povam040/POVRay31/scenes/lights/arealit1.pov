// Persistence Of Vision raytracer version 3.1 sample file.
// Illustrates how area light grid size affects soft shadows

// Left   shadow - area_light <8,0,0>, <0,0,8>,  3,  3 (renders fastest)
// Middle shadow - area_light <8,0,0>, <0,0,8>,  5,  5
// Right  shadow - area_light <8,0,0>, <0,0,8>, 17, 17 (renders slowest)

#version 3.1;
global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "textures.inc"

// A back wall to cast shadows onto
plane { -z, -20
    pigment { Gray70 }
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
    direction 2*z
    look_at <0, -15, 0>
}
