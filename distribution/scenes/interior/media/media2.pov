// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence of Vision Raytracer Scene Description File
// File: media2.pov
// Author:
// Description:
// Participating media with spotlights.
//
// -w320 -h240
// -w800 -h600 +a0.3
//
//*******************************************

#version 3.6;
global_settings {assumed_gamma 1.0}

//
// The camera.
//

camera {
  location <5, 6, -18>
  look_at <0, 4, 0>
  angle 65
}

//
// Add media.
//

media {
  scattering {1, rgb 0.02}
  intervals 1
  samples 5
  method 3
//  intervals 40
//  samples 1, 10
//  confidence 0.9999
//  variance 1/1000
//  ratio 0.9
}

//
// Light source not interacting with the atmosphere.
//

light_source { <0, 15, 0> color rgb .3
  media_interaction off
  shadowless
}

//
// Spotlights pointing at shaft.
//

#declare Spot = light_source {
  <0, 0, 0> color rgb<1, 1, 1> * 2
  spotlight
  point_at <0, -1, 0>
  radius 2
  falloff 3
  media_attenuation on
}

#declare Spots = union {
  object { Spot rotate <0, 0,  15> rotate <0,   0, 0> }
  object { Spot rotate <0, 0,  15> rotate <0,  30, 0> }
  object { Spot rotate <0, 0,  15> rotate <0,  60, 0> }
  object { Spot rotate <0, 0,  15> rotate <0,  90, 0> }
  object { Spot rotate <0, 0,  15> rotate <0, 120, 0> }
  object { Spot rotate <0, 0,  15> rotate <0, 150, 0> }
  object { Spot rotate <0, 0,  15> rotate <0, 180, 0> }
  object { Spot rotate <0, 0,  15> rotate <0, 210, 0> }
  object { Spot rotate <0, 0,  15> rotate <0, 240, 0> }
  object { Spot rotate <0, 0,  15> rotate <0, 270, 0> }
  object { Spot rotate <0, 0,  15> rotate <0, 300, 0> }
  object { Spot rotate <0, 0,  15> rotate <0, 330, 0> }
  object { Spot rotate <0, 0,  30> rotate <0,  10, 0> }
  object { Spot rotate <0, 0,  30> rotate <0,  40, 0> }
  object { Spot rotate <0, 0,  30> rotate <0,  70, 0> }
  object { Spot rotate <0, 0,  30> rotate <0, 100, 0> }
  object { Spot rotate <0, 0,  30> rotate <0, 130, 0> }
  object { Spot rotate <0, 0,  30> rotate <0, 160, 0> }
  object { Spot rotate <0, 0,  30> rotate <0, 190, 0> }
  object { Spot rotate <0, 0,  30> rotate <0, 220, 0> }
  object { Spot rotate <0, 0,  30> rotate <0, 250, 0> }
  object { Spot rotate <0, 0,  30> rotate <0, 280, 0> }
  object { Spot rotate <0, 0,  30> rotate <0, 310, 0> }
  object { Spot rotate <0, 0,  30> rotate <0, 340, 0> }
  object { Spot rotate <0, 0,  45> rotate <0,  20, 0> }
  object { Spot rotate <0, 0,  45> rotate <0,  50, 0> }
  object { Spot rotate <0, 0,  45> rotate <0,  80, 0> }
  object { Spot rotate <0, 0,  45> rotate <0, 110, 0> }
  object { Spot rotate <0, 0,  45> rotate <0, 140, 0> }
  object { Spot rotate <0, 0,  45> rotate <0, 170, 0> }
  object { Spot rotate <0, 0,  45> rotate <0, 200, 0> }
  object { Spot rotate <0, 0,  45> rotate <0, 230, 0> }
  object { Spot rotate <0, 0,  45> rotate <0, 260, 0> }
  object { Spot rotate <0, 0,  45> rotate <0, 290, 0> }
  object { Spot rotate <0, 0,  45> rotate <0, 320, 0> }
  object { Spot rotate <0, 0,  45> rotate <0, 350, 0> }
}

//
// Declare steps.
//

#declare Step = prism {
  linear_spline
  linear_sweep
  0, 1, 9
  <cos(radians(0*45)), sin(radians(0*45))>,
  <cos(radians(1*45)), sin(radians(1*45))>,
  <cos(radians(2*45)), sin(radians(2*45))>,
  <cos(radians(3*45)), sin(radians(3*45))>,
  <cos(radians(4*45)), sin(radians(4*45))>,
  <cos(radians(5*45)), sin(radians(5*45))>,
  <cos(radians(6*45)), sin(radians(6*45))>,
  <cos(radians(7*45)), sin(radians(7*45))>,
  <cos(radians(0*45)), sin(radians(0*45))>
  rotate 22.5*y
}

#declare Stair = union {
  object {
    Step
    scale <10, 0.5, 10>
    translate <0, 0, 0>
  }
  object {
    Step
    scale <8, 0.5, 8>
    translate <0, 0.5, 0>
  }
  object {
    Step
    scale <6, 0.5, 6>
    translate <0, 1, 0>
  }
}

//
// Declare shaft.
//

#declare Shaft1 = union {
  cylinder { <0, 0.0, 0>, <0, 4.0, 0>, 0.6 }
  cylinder { <0, 4.0, 0>, <0, 5.0, 0>, 0.8 }
  cylinder { <0, 5.0, 0>, <0, 6.0, 0>, 0.4 }
  cylinder { <0, 6.0, 0>, <0, 7.0, 0>, 0.2 }
}

//
// Position objects.
//

object {
  Spots
  translate <0, 20, 0>
}

object {
  Stair
  pigment { color red 1 green 0.3 blue 0.3 }
  finish { ambient 0.2 diffuse 0.5 }
}

object {
  Shaft1
  translate <0, 1.5, 0>
  pigment { color red 0.3 green 1 blue 0.3 }
  finish { ambient 0.2 diffuse 0.5 phong 1 phong_size 20 }
}

//
// Room.
//

box { <-25, 0, -25>, <25, 25, 25>
  pigment { color red 1 green 1 blue 1 }
  finish { ambient 0.2 diffuse 0.5 }
  hollow
}

