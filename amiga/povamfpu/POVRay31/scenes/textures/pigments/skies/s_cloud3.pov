global_settings { assumed_gamma 2.2 }
#include "colors.inc"
#include "skies.inc"

camera {
    location <0, 1, -100>
    up y
    right x*1.33
    direction z
    look_at <0 20 0>
    angle 57
}

light_source { <100, 100, -50> White }

sky_sphere { S_Cloud3 }
plane { y, 0 pigment { color red 0.3 green 0.75 blue 0.5} }

