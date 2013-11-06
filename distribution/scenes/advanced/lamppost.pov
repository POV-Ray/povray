// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Description: Old fashioned lamppost on a reflective floor
// File by Anton Raves
//
//  Updated: 2013/02/15 for 3.7
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

global_settings {
  assumed_gamma 1.0
  max_trace_level 5
}

#include "shapes.inc"
#include "colors.inc"

#declare Use_Arealights = true;
#if(Use_Arealights=false)
    #debug "Try Use_Arealights=true\n"
#end

camera
{
  location  <0, 95, -475>
  angle 50 // direction 1.4*z
  up        y
  right     x*image_width/image_height
  look_at   <0, 95, 0>
}


light_source { <190, 679, -250> color White
#if(Use_Arealights)
  area_light <25, 0, 0>, <0, 0, 25>, 5, 5
  adaptive 1
  jitter
#end
}

// To make things a little more interesting the light comes from ... a lightbulb
// This should use "looks_like"
light_source { <100, 179, -160> color Yellow * 1.5
#if(Use_Arealights)
  area_light <4, 0, 0>, <0, 0, 4>, 4, 4
  adaptive 1
  jitter
#end
}
sphere { <100, 179, -160>, 3
  texture { pigment { rgb<1, 1, 0.9> } finish { ambient 1 diffuse 1 } }
  no_shadow
}

// This is the glass2-texture from the TEXTURES.INC-file that I tweaked a little
#declare Glass_tex =
texture {
  pigment { color red 1 green 1 blue 1 filter .4 }
  finish { ambient .1 diffuse .1 reflection 0.5  phong 0.3 phong_size 60 }
}

// This is the gold-texture Mike Miller used in his SLUG1-scene
#declare Gold_tex =
texture {
  pigment { color red .64 green .51 blue .05 }
  finish { crand .02 ambient .3 diffuse .5 reflection .6 brilliance 8 specular .5 }
}

#declare Lamp_tex =
texture {
  pigment { color Gray20 }
  finish { ambient .1 phong .7 }
}

#declare Vent_cutout =
box { <-.5, -1.5, -1> <.5, 1.5, 1> }

#declare Collar_piece =
union {
  cylinder { .5*z, -.5*z, 1.5 translate <-4, 0, 0> }
  cylinder { .5*z, -.5*z, 1.5 translate <4, 0, 0> }
  box { <-4, -1.5, -.5> <4, 1.5, .5> }
}

#declare Collar =
union {
  object { Collar_piece }
  object { Collar_piece rotate -45*y }
  object { Collar_piece rotate -90*y }
  object { Collar_piece rotate -135*y }
  object { Collar_piece rotate -180*y }
  object { Collar_piece rotate -225*y }
  object { Collar_piece rotate -270*y }
  object { Collar_piece rotate -315*y }
}

#declare Little_feature =
union {
  sphere { <0, 0, 0>, 1 }
  triangle { <0, 2, 0> <0, 0, -1> <1, 0, 0> }
  triangle { <0, 2, 0> <0, 0, -1> <-1, 0, 0> }
}

#declare Little =
union {
  object { Little_feature translate <3.5, 0, 0> }
  object { Little_feature translate <3.5, 0, 0> rotate -45*y }
  object { Little_feature translate <3.5, 0, 0> rotate -90*y }
  object { Little_feature translate <3.5, 0, 0> rotate -135*y }
  object { Little_feature translate <3.5, 0, 0> rotate -180*y }
  object { Little_feature translate <3.5, 0, 0> rotate -225*y }
  object { Little_feature translate <3.5, 0, 0> rotate -270*y }
  object { Little_feature translate <3.5, 0, 0> rotate -315*y }
}

#declare support_piece =
difference {
  union {
    cylinder { .5*z, -.5*z 1.5 }
    box { <-1.5, -1.5, -.5> <0, 0, .5> }
  }
  cylinder { z, -z, .5 }
}

#declare support =
union {
  difference {
    cylinder { .5*z, -.5*z, 6 }
    cylinder { z, -z, 5 }
    plane { y, 0 inverse }
    plane { x, 0 }
  }
  object { support_piece translate <6.5, 1.5, 0> }
}

#declare Lamppost_stem =
union {
  cylinder { y, -y, 7 translate <0, 1, 0> }
  cylinder { .5*y, -.5*y, 6 translate <0, 2.5, 0> }
  cylinder { y, -y, 6.75 translate <0, 4, 0> }
  cone { y, 5, -y, 6.75 translate <0, 6, 0> }
  cylinder { 17*y, -17*y, 4.5 translate <0, 24, 0> }
  cylinder { y, -y, 5 translate <0, 41, 0> }
  cylinder { y, -y, 6.75 translate <0, 43, 0> }
  cylinder { y, -y, 5 translate <0, 45, 0> }
  cylinder { y, -y, 4 translate <0, 47.5, 0> }
  cylinder { 2*y, -2*y, 3.5 translate <0, 51, 0> }
  cylinder { .5*y, -.5*y, 4.5 translate <0, 53.5, 0> }
  cylinder { 53*y, -53*y, 3.5 translate <0, 107, 0> }
  object { Little translate <0, 69.5, 0> }
  object { Collar translate <0, 135.5, 0> }
  texture { Lamp_tex }
}

#declare Lamppost_head =
union {
  sphere { <0, 0, 0>, .5 texture { Gold_tex } translate <0, 29.5, 0> }
  cone { 1.5*y, 0, -1.5*y, 1 texture { Gold_tex } translate <0, 27.5, 0> }
  cylinder { .5*y, -.5*y, 2 translate <0, 25.5, 0> }
  cylinder { .5*y, -.5*y, 1 translate <0, 24.5, 0> }
  cone { 1.5*y, 1, -1.5*y, 4 translate <0, 22.5, 0> }
  cylinder { .5*y, -.5*y, 6 translate <0, 20.5, 0> }
  difference {
    cylinder { 2.5*y, -2.5*y, 4 translate <0, 17.5, 0> }
    object { Vent_cutout translate <4, 17.5, 0> }
    object { Vent_cutout translate <4, 17.5, 0> rotate -45*y }
    object { Vent_cutout translate <4, 17.5, 0> rotate -90*y }
    object { Vent_cutout translate <4, 17.5, 0> rotate -135*y }
    object { Vent_cutout translate <4, 17.5, 0> rotate -180*y }
    object { Vent_cutout translate <4, 17.5, 0> rotate -225*y }
    object { Vent_cutout translate <4, 17.5, 0> rotate -270*y }
    object { Vent_cutout translate <4, 17.5, 0> rotate -315*y }
  }
  cylinder { .5*y, -.5*y, 5 texture { Gold_tex } translate <0, 14.5, 0> }
  cylinder { .5*y, -.5*y, 6 texture { Gold_tex } translate <0, 13.5, 0> }
  cone { 3.5*y, 6, -3.5*y, 14 open translate <0, 9.5, 0> }
  difference {
    cylinder { y, -y, 16 translate <0, 5, 0> }
    cylinder { 2*y, -2*y, 12 translate <0, 5, 0> }
  }
  cone { 11.5*y, 14, -11.5*y, 10 interior {ior 1.5} texture { Glass_tex } translate <0, -7.5, 0> }
  object {
    difference {
      cylinder { y, -y, 10 translate <0, -20, 0> }
      cylinder { 2*y, -2*y, 8 translate <0, -20, 0> }
    }
  }
  box { <-9, -21, -1> <9, -19, 1> no_shadow }
  box { <-1, -21, -9> <1, -19, 9> no_shadow }
  object { support translate <3.5, -24, 0> no_shadow }
  object { support translate <3.5, -24, 0> rotate 90*y no_shadow }
  object { support translate <3.5, -24, 0> rotate 180*y no_shadow }
  object { support translate <3.5, -24, 0> rotate 270*y no_shadow }
  cylinder { 4.5*y, -4.5*y, 1.5 texture { Gold_tex } translate <0, 9.5, 0> }
  texture { Lamp_tex }
}

#declare Lamppost =
union {
  object { Lamppost_stem }
  object { Lamppost_head translate <0, 189, 0> }
}

object { Lamppost translate <100, -10, -160> }
plane { y, -10
  texture {
    pigment { color Gray30 }
    finish { ambient .2 reflection .4 }
  }
}
