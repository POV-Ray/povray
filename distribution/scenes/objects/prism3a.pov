// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3
#version 3.7;
global_settings { assumed_gamma 1.0 }
 

#include "prism3.inc"

#declare Prism = prism {
  linear_spline
  0.0,
  1.0,
  25,
  <-1.000000, -1.000000>,  // Outer polygon
  <1.000000, -1.000000>,
  <0.228311, 0.228311>,
  <-0.228311, 0.228311>,
  <-1.000000, -1.000000>
  <-0.228311, -0.456621>,  // 1st inner square
  <0.228311, -0.456621>,
  <0.228311, 0.000000>,
  <-0.228311, 0.000000>,
  <-0.228311, -0.456621>
  <-0.479452, -0.730594>,  // 2nd inner square
  <-0.022831, -0.730594>,
  <-0.022831, -0.273973>,
  <-0.479452, -0.273973>,
  <-0.479452, -0.730594>
  <0.027397, -0.730594>,   // 3rd inner square
  <0.484018, -0.730594>,
  <0.484018, -0.273973>,
  <0.027397, -0.273973>,
  <0.027397, -0.730594>
  <-0.228311, -0.958904>,  // 4th inner square
  <0.228311, -0.958904>,
  <0.228311, -0.502283>,
  <-0.228311, -0.502283>,
  <-0.228311, -0.958904>
}

object { Prism
  pigment { color rgb<1, 0.0, 0.0> }
  scale <2, 0.2, 2>
  translate 0.614*z
}

