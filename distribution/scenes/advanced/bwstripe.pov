// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// "Black and White Stripes"
// Copyright 2001 Rune S. Johansen
//
//  Updated: 2013/02/15 for 3.7

// -w320 -h240
// -w800 -h600 +a0.3

// This scene makes use of a contrast enhancing filter.
// For more information, see the demo scene transmitfx.pov
// located in the folder \scenes\textures\pigments\

// The idea is to create a woodcut look where dark areas are
// made of thick black stripes and bright areas are made of
// thin black stripes. This effect is achieved by giving all
// objects a grayish gradient pigment. The brighter parts of
// the gradient becomes white and the darker parts black.
// Because the light from the light_source also makes the
// gradient brighter, the white stripes are more dominant in
// the illuminated areas on the objects.

#version 3.6;

global_settings {assumed_gamma 1.0}

camera {
   location <0,6,-10>
   right x*image_width/image_height // keep propotions with any aspect ratio
   look_at 4*y
}

// A small sphere around the camera location with a
// contrast enhasing texture.
// (Also try to render with this sphere commented out
// to see how the scene looks without the filter.)
sphere {
   <0,6,-10>, 0.001 hollow
   pigment {color rgb 0.5 transmit 100}
   finish {ambient 1 diffuse 0}
}

// A light source
light_source {<0,10,0>, color 1}

// The grayish gradient pigment which is applied to all
// objects in the scene by making it the default.
#default {
   texture {
      pigment {
         gradient y
         triangle_wave
         color_map {[0, rgb 0.5][1, rgb 1.0]}
         scale 0.3 rotate 20
      }
      finish {ambient 0.55 diffuse 0.25}
   }
}

#declare Column =
union {
   cylinder {0.0*y, 0.4*y, 1.2}
   cylinder {0.4*y, 3.6*y, 0.8}
   cylinder {3.6*y, 4.0*y, 1.2}
   torus {0.8, 0.4 translate 0.4*y}
   torus {0.8, 0.4 translate 3.6*y}
}

// Four columns
object {Column translate -4*x rotate 000*y}
object {Column translate -4*x rotate 060*y}
object {Column translate -4*x rotate 120*y}
object {Column translate -4*x rotate 180*y}

// One object hovering over each column
union {
   sphere {0, 1                             translate -4*x rotate 000*y}
   box {-0.9, 0.9 rotate <45,10,45>         translate -4*x rotate 060*y}
   cylinder {-x, x, 0.9 rotate <-20,20,-10> translate -4*x rotate 120*y}
   torus {0.7, 0.4 rotate <50,45,0>         translate -4*x rotate 180*y}

   translate 5.5*y
}

// A quick room
box {<-8,0,-12>, <8,12,8> hollow}
