// POVLAB version 4.00 SP4 - Scene for raytracing.
// (C) 1994-1998, Denis Olivier.
// All rights reserved.
// Generated for POV-Ray (C) POV-Team, USA.
// Date=Tuesday 04-24-2001.
// Time=15:48:11.
// user_name=USER

// Render # 2

#include "..\POVLAB\RENDER\LIGHT.INC"

global_settings {
  adc_bailout 0.003922
  // ambient_light <1,1,1>
  assumed_gamma 2.2
  irid_wavelength rgb <0.25,0.18,0.14>
  max_intersections 64
  max_trace_level 3
  number_of_waves 10
}

// ---- Camera and environment.

#declare location_vector = <0,4.782,-6.724>
#declare look_at_vector  = <0,0.9679,0>

// ---- if Lens Flare used.

#declare cam_loc = location_vector
#declare lookat = look_at_vector
#declare sky_vect = <0,1,0>

camera {
  location location_vector
  direction <0,0,1.497>
  sky <0,1,0>
  right <1.333333,0,0>
  look_at look_at_vector
}

background { color rgb <0.1961,0.1961,0.2353> }

light_source { // #01
  <-1.961,3.5,-2.347>
  color rgb <0,0,1>
  atmosphere off
  shadowless
}

light_source { // #02
  <2.931,3.5,-0.1424>
  color rgb <0,1,0>
  atmosphere off
  shadowless
}

light_source { // #03
  <-1.329,3.474,2.658>
  color rgb <1,0,0>
  atmosphere off
  shadowless
}

#declare Default = texture {
  pigment { color rgb <1,1,1> }
  finish { phong 1.0 }
}

// Object 00001: Sol

plane {
  <0,1,0>,0
  texture {
    White_RPlastic
  }
}

// Object 00002: Sphère

sphere {
  <0,0,0>,1
  texture {
    White_RPlastic
  }
  translate <0,0.9962,0>
}

