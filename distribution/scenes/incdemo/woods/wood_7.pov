// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Raytracer sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.6;
global_settings {assumed_gamma 1.0}

#include "testcam.inc"

#declare Test1 =
    texture { pigment { P_WoodGrain7A color_map { M_Wood1A }}}
    texture { pigment { P_WoodGrain7B color_map { M_Wood1B }}}
#declare Test2 =
    texture { pigment { P_WoodGrain7A color_map { M_Wood2A }}}
    texture { pigment { P_WoodGrain7B color_map { M_Wood2B }}}
#declare Test3 =
    texture { pigment { P_WoodGrain7A color_map { M_Wood3A }}}
    texture { pigment { P_WoodGrain7B color_map { M_Wood3B }}}
#declare Test4 =
    texture { pigment { P_WoodGrain7A color_map { M_Wood4A }}}
    texture { pigment { P_WoodGrain7B color_map { M_Wood4B }}}
#declare Test5 =
    texture { pigment { P_WoodGrain7A color_map { M_Wood5A }}}
    texture { pigment { P_WoodGrain7B color_map { M_Wood5B }}}
#declare Test6 =
    texture { pigment { P_WoodGrain7A color_map { M_Wood6A }}}
    texture { pigment { P_WoodGrain7B color_map { M_Wood6B }}}
#declare Test7 =
    texture { pigment { P_WoodGrain7A color_map { M_Wood7A }}}
    texture { pigment { P_WoodGrain7B color_map { M_Wood7B }}}
#declare Test8 =
    texture { pigment { P_WoodGrain7A color_map { M_Wood8A }}}
    texture { pigment { P_WoodGrain7B color_map { M_Wood8B }}}
#declare Test9 =
    texture { pigment { P_WoodGrain7A color_map { M_Wood9A }}}
    texture { pigment { P_WoodGrain7B color_map { M_Wood9B }}}
#declare Test10 =
    texture { pigment { P_WoodGrain7A color_map { M_Wood10A }}}
    texture { pigment { P_WoodGrain7B color_map { M_Wood10B }}}
#declare Test11 =
    texture { pigment { P_WoodGrain7A color_map { M_Wood11A }}}
    texture { pigment { P_WoodGrain7B color_map { M_Wood11B }}}
#declare Test12 =
    texture { pigment { P_WoodGrain7A color_map { M_Wood12A }}}
    texture { pigment { P_WoodGrain7B color_map { M_Wood12B }}}
#declare Test13 =
    texture { pigment { P_WoodGrain7A color_map { M_Wood13A }}}
    texture { pigment { P_WoodGrain7B color_map { M_Wood13B }}}
#declare Test14 =
    texture { pigment { P_WoodGrain7A color_map { M_Wood14A }}}
    texture { pigment { P_WoodGrain7B color_map { M_Wood14B }}}
#declare Test15 =
    texture { pigment { P_WoodGrain7A color_map { M_Wood15A }}}
    texture { pigment { P_WoodGrain7B color_map { M_Wood15B }}}
#declare Test16 =
    texture { pigment { P_WoodGrain7A color_map { M_Wood16A }}}
    texture { pigment { P_WoodGrain7B color_map { M_Wood16B }}}
#declare Test17 =
    texture { pigment { P_WoodGrain7A color_map { M_Wood17A }}}
    texture { pigment { P_WoodGrain7B color_map { M_Wood17B }}}
#declare Test18 =
    texture { pigment { P_WoodGrain7A color_map { M_Wood18A }}}
    texture { pigment { P_WoodGrain7B color_map { M_Wood18B }}}

#include "testobjs.inc"
