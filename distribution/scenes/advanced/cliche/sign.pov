// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File used for cliche.pov by Rune S. Johansen
// 
// +w400 +h400 +a0.3

#version 3.6;

global_settings {assumed_gamma 1.0}

camera {
   orthographic
   location -10*z
   up 2.2*y
   right 2.2*x
   look_at 0
}

background {color rgb 1}

#default {finish {ambient 1 diffuse 0}}

cylinder {
   -0.9*y, 0.9*y, 0.15 translate -8*z
   matrix <1,0,0,1,1,0,0,0,1,0,0,0>
   pigment {color <1,0,0>}
}
cylinder {
   -0.9*y, 0.9*y, 0.25 translate -6*z
   matrix <1,0,0,1,1,0,0,0,1,0,0,0>
   pigment {color <1,1,1>}
}
torus {
   0.8, 0.10 rotate 90*x
   pigment {color rgb 0}
}
torus {
   0.2, 0.05 rotate 45*x translate 0.4*y rotate 25*z
   pigment {color rgb 0}
}
