// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence of Vision Raytracer Scene Description File
// File: media1.pov
// Author:
// Description:
// Participating media environment with spotlights.
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
  location <10, 6, -20>
  direction <0, 0, 1.5>
  look_at <0, 4, 0>
}

//
// Add media.
//

media {
  scattering { 1, rgb 0.03}
  intervals 1
  samples 5
  method 3
}

//
// Light source not interacting with the atmosphere.
//

light_source { <0, 15, 0> color rgb 0.7
  media_interaction off
  shadowless
}

//
// Spotlights pointing at balls.
//

#declare Intensity = 2;

light_source {
  <-10, 15, -5> color rgb<1, .3, .3> * Intensity
  spotlight
  point_at <0, 5, 0>
  radius 10
  falloff 15
  tightness 1
  media_attenuation on
}

light_source {
  <0, 15, -5> color rgb<.3, 1, .3> * Intensity
  spotlight
  point_at <0, 5, 0>
  radius 10
  falloff 15
  tightness 1
  media_attenuation on
}

light_source {
  <10, 15, -5> color rgb<.3, .3, 1> * Intensity
  spotlight
  point_at <0, 5, 0>
  radius 10
  falloff 15
  tightness 1
  media_attenuation on
}

//
// Room.
//

box { <-20, 0, -20>, <20, 20, 20>
  pigment { rgb 1 }
  finish { ambient 0.2 diffuse 0.5 }
  hollow
}

//
// Ball.
//

sphere { <0, 5, 0>, 1
  pigment { rgb 1 }
  finish { ambient 0.3 diffuse 0.7 phong 1 }
}

