// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Slope_map example
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

global_settings { assumed_gamma 1.0 }

#include "colors.inc"

camera {
     location  <0,0,-20>
     right     x*image_width/image_height
     direction 3*z
 }

#declare Amt=<0,0,0>;

 light_source { <200, 200, -100> color White}

 #default {
     pigment { White  }
     normal{
         onion .8
         scale .4
     }
     finish { phong 0.8 phong_size 200 }
 }

 #declare Thing =
 box{ <-1,-1,0>,<1,1,1> scale .85 }

 // top row, left to right
 object { Thing
     normal{
     }
     rotate Amt translate <-3,2,0>
 }
 object { Thing
     normal{
       slope_map {     // triangle_wave the hard way
         [0   <0, 1>]   // start at bottom and slope up
         [0.5 <1, 1>]   // halfway through reach top still climbing
         [0.5 <1,-1>]   // abruptly slope down
         [1   <0,-1>]   // finish on down slope at bottom
       }
     }
     rotate Amt translate <0,2,0>
 }
 object { Thing
     normal{
       slope_map {         // sine_wave the hard way
         [0    <0.5, 1>]   // start in middle and slope up
         [0.25 <1.0, 0>]   // flat slope at top of wave
         [0.5  <0.5,-1>]   // slope down at mid point
         [0.75 <0.0, 0>]   // flat slope at bottom
         [1    <0.5, 1>]   // finish in middle and slope up
       }
     }
     rotate Amt translate <3,2,0>
 }

 // middle row, left to right
 object { Thing
     normal{
       slope_map {      // reverse ramp wave
         [0   <1,-1>]   // start at top and slope down
         [1   <0,-1>]   // finish on down slope at bottom
       }
     }
     rotate Amt translate <-3,0,0>
 }
 object { Thing
     normal{
       slope_map {      // scallop_wave the hard way
         [0   <0, 1>]   // start at bottom and slope up
         [0.5 <1, 0>]   // halfway through reach top flat
         [1   <0,-1>]   // finish on down slope at bottom
       }
     }
     rotate Amt translate <0,0,0>
 }

 object { Thing
     normal{
       slope_map {      // scallop_wave with steep slopes
         [0   <0, 3>]   // 3.0 is suggested max
         [0.5 <1, 0>]   // halfway through reach top flat
         [1   <0,-3>]   // what goes up...
       }
     }
     rotate Amt translate <3,0,0>
 }

 // bottom row, left to right
 object { Thing
     normal{
       slope_map {      // Now let's get fancy
         [0.0  <0, 1>]   // Do tiny tringle here
         [0.2  <1, 1>]   //  down
         [0.2  <1,-1>]   //     to
         [0.4  <0,-1>]   //       here.
         [0.4  <0, 0>]   // Flat area
         [0.5  <0, 0>]   //   through here.
         [0.5  <1, 0>]   // Square wave leading edge
         [0.6  <1, 0>]   //   trailing edge
         [0.6  <0, 0>]   // Flat again
         [0.7  <0, 0>]   //   through here.
         [0.7  <0, 3>]   // Start scallop
         [0.8  <1, 0>]   //   flat on top
         [0.9  <0,-3>]   //     finish here.
         [0.9  <0, 0>]   // Flat remaining through 1.0
       }
       scale 2  // so you can see details
     }
     rotate Amt translate <-3,-2,0>
 }

 object { Thing
     normal{
       slope_map {      // Surf's up dude!
         [0   <0,  0.0>]   // start at bottom flat
         [0.7 <1,  0.0>]   // S-curv to flat top
         [0.7 <0,  0.0>]   // drop to bottom
         [1.0 <0,  0.0>]   //  flat
       }
     }
     rotate Amt translate <0,-2,0>
 }
 object { Thing
     normal{
       slope_map {      // inverse scallop_wave
         [0   <0, 0>]   // start at bottom flat
         [0.5 <1, 3>]   // halfway through reach bottom peak
         [0.5 <1,-3>]   // star down again
         [1   <0, 0>]   // finish on flat bottom
       }
     }
     rotate Amt translate <3,-2,0>
 }

