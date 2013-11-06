// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File: SwirlBox.POV
// Desc: Stacked square rings in a snail shell shape,
//       shows off the use of the while loop for creating objects,
//       and for smoothly changing colors.
// Date: 10/1/95
// Auth: Eduard Schwan
// Updated: 2013/02/15 for 3.7
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings { assumed_gamma 1.0 } 

// ------------------------------------------------------------------
// Look down at an angle at our creation
camera
{
  location  < 4, 9,-20>
  angle 65 //  direction  1*z
  look_at   < 0,-1, 8>
  right x*image_width/image_height // keep propotions with any aspect ratio
}


// ------------------------------------------------------------------
// A white marble floor
plane
{
  y, -0.1
  texture
  {
    pigment
    {
      marble
      turbulence 0.5 omega 0.7 rotate -40*y scale 6
      color_map
      {
        [0.50 color rgb 1.0]
        [0.57 color rgb 0.8]
        [0.60 color rgb <0.9,0.8,0.7>]
        [0.63 color rgb 1.0]
      }
    }
    finish {ambient 0.2 reflection 0.3}
  }
}


// ------------------------------------------------------------------
// Simple background for a simple scene
background { color rgb <0.0, 0.1, 0.2> }


// ------------------------------------------------------------------
// A light source
light_source { <50, 20, -50> color rgb 1 }


// ------------------------------------------------------------------
// create a simple square ring shape
#declare BasicShape = intersection
{
  // flat pizza box
  box { -1, +1 scale <1.0, 0.1, 1.0> }
  // remove square hole in middle
  box { -1, +1 scale <0.95, 1.1, 0.95> inverse }
}


// ------------------------------------------------------------------
// Set up the loop variables:
// the Counter variable will go from 0.0 to 1.0
// in NumIterations loops.
#declare NumIterations = 80;  // try different numbers of boxes (20,40,80...)
// don't change these
#declare Counter       = 0.0;
#declare Increment     = 1.0/NumIterations;
#declare NumTwists     = 360*2; // two full twists


// ------------------------------------------------------------------
// Create an iterated object built from our basic shape
union{
  #declare Flipper = 0;  // Flipper will switch between 0 and 1 each loop
  #while (Counter<=1.0)
    object
    {
      BasicShape
      // boxes get smaller as we stack them up
      scale <(1-Counter)*5,1,(1-Counter)*5>
      // boxes get closer to center and stack upward as we go
      translate <(1-Counter)*5, Counter*10, 0.0>
      // rotate each box a little more as they stack
      rotate NumTwists*Counter*y
      // put down the texture
      texture
      {
        pigment
        {
          // colors change as we stack them up
          color rgb <1-Counter, Flipper, Counter>
        }
        finish { ambient 0.2 specular 0.7 roughness 0.05 }
      }
    }
    #declare Flipper = 1 - Flipper; // flip its value (0->1 or 1->0)
    // manually increment our counter inside the loop
    #declare Counter=Counter+Increment;
  #end
}


