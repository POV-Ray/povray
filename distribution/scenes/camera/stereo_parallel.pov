#version 3.7;
global_settings{assumed_gamma 1.0 max_trace_level 10
photons{count 2000000 }
}

camera
{ stereo distance (26)
parallaxe atan2(13,300)
location -300*z
right image_width/image_height*x
direction z
up y
angle 15
}

#include "finish.inc"
box { -1,1 scale 5/1.2 rotate 45*z rotate 66*y 
texture { pigment { rgb 1 } finish { Mirror } } photons{ target reflection on }}
superellipsoid { <0.3,0.3> scale 5 texture { pigment { rgb 1 } finish { Glossy } } translate -x*13 photons{ target } }
superellipsoid { <0.3,0.3> scale 5 texture { pigment { rgb 1 } finish { Shiny } } translate x*13  photons{ target }}

sphere { 0,1 scale 5 translate 13*y texture { pigment { rgb 1 } finish { Shiny } } photons{ target }}
sphere { 0,1 scale 5 translate 13*y texture { pigment { rgb 1 } finish { Glossy } } translate -x*13 photons{ target }}
sphere { 0,1 scale 5 translate 13*y texture { pigment { rgb 1 } finish { Phong_Shiny } } translate x*13 photons{ target }}

cone { -5*z,0,5*z,1 scale 5 translate -13*y texture { pigment { rgb 1 } finish { Shiny } } photons{ target }}
cone { -5*z,0,5*z,1 scale 5 translate -13*y texture { pigment { rgb 1 } finish { Glossy } } translate -x*13 photons{ target }}
cone { -5*z,0,5*z,1 scale 5 translate -13*y texture { pigment { rgb 1 } finish { Phong_Shiny } } translate x*13 photons{ target }}
plane { -z, -5 texture { pigment { color rgb <1,1,1> } finish { Dull }} photons { collect } }

light_source { <0,20,-20>*100, rgb 1 area_light 100*x,100*y,9,9 photons { area_light }}
light_source { <-50,20,-10>*100, red 0.91  area_light 100*x,100*y,9,9  photons { area_light }}
light_source { <50,20,-10>*100, green 0.91  area_light 100*x,100*y,9,9  photons { area_light }}

#include "colors.inc"

background { Yellow }
