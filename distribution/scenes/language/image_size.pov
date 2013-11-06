// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence of Vision Raytracer Scene Description File
// File: image_size.pov
// Author: Fabien Mosen
// Description:
// This file demonstrates the "image_width" and "image_height" functions.
//
// -w320 -h240
// -w800 -h600 +a0.3
//
//*******************************************

#version 3.6;
global_settings {assumed_gamma 1.0}

#include "colors.inc"

camera { location <0,2,-10>
         right     x*image_width/image_height
         angle 50 // direction z*1.5 
         look_at <0,2,0>#
       }

#declare TotalPixels = image_width * image_height;
#declare PixString = str (TotalPixels,3,0)

union {
 text {ttf "cyrvetic.ttf","this image",.1,0 translate y*3}
 text {ttf "cyrvetic.ttf","contains exactly",.1,0 translate y*2}
 text {ttf "cyrvetic.ttf",PixString,.1,0 translate y*1 pigment {White}}
 text {ttf "cyrvetic.ttf","pixels",.1,0 translate y*0}
 text {ttf "cyrvetic.ttf","(unless you messed with it :-)",.1,0 scale .5 translate y*-1}
  pigment {SteelBlue}
  translate x*-3
}

plane {z,.1 hollow on pigment {spiral1 15 color_map {[0 Red*.3][1 Red*.2]} sine_wave warp {turbulence .1}}}

light_source {<4,5,-30> White*2}
