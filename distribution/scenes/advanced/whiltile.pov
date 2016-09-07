// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File: WhilTile.POV 10/01/95  - updated 2013/02/15
// Auth: Eduard Schwan with modifications by Dan Farmer
// Desc: A tile floor, using the 'while' loop and 'if' statements
//       to create a unique pattern automatically.

// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

// Enable this next line to use a simpler test shape for debugging
// #declare DO_SIMPLE_SHAPE = 1;

global_settings { assumed_gamma 1.0  max_trace_level 5}

#include "colors.inc"
#include "woods.inc"

// ------------------------------------------------------------------
// set viewer's position in the scene
camera {
  location  <0.0, 22.0, -24.0> // position of camera <X Y Z>
  angle 35 // direction 4.0*z              // which way are we looking <X Y Z>
  up        y                  // which way is +up <X Y Z>
  right x*image_width/image_height // keep propotions with any aspect ratio
  look_at   <0.0, 0.0, -1.0>   // point center of view at this point <X Y Z>
}

// ------------------------------------------------------------------
// create a regular point light source
light_source { <10, 10, -10> color rgb 1 translate <-20, 40, -20> }


// ------------------------------------------------------------------
#declare TileShape =
#ifdef (DO_SIMPLE_SHAPE)
  // simpler shape for debugging
  sphere{0, 1 scale <0.5,0.1,0.5> translate 0.1*y}
#else
  // real tile shape
  intersection  {
    box { -1, 1 }  // square tile
    cylinder { -2*x, +2*x, 0.9 } // rounded edges along X axis
    cylinder { -2*z, +2*z, 0.9 } // rounded edges along Z axis
    sphere { 0, 2.0 translate 2.6*y inverse }  // concave top surface
//  sphere { 0, 1.1 } // rounded corners
//  bounded_by { box { -1, 1 } }
    scale <0.5,0.2,0.5>  // flatten the tile down on Y axis
//    translate 0.2*y  // move it up so its bottom is on origin
  }
#end

// ------------------------------------------------------------------
default {
  texture { pigment {rgb 1} finish {ambient 0.2 reflection 0.2 specular 0.7} }
}

// light tinted glass
#declare CrystalTex =
texture {
  pigment { rgbf <1.00, 0.70, 0.70, 0.95> }
  finish {roughness 0.001 }
}

#declare CrystalInt =
interior {ior 1.8}

// Black slate marble with white strata
#declare SlateTex =
texture {
  pigment {
    marble
    turbulence 0.8 rotate 60*y
    color_map {
      [0.10 rgb 0.01]
      [0.12 rgb 0.70]
      [0.15 rgb 0.01]
      [0.20 rgb 0.01]
      [0.30 rgb <0.9,0.7,0.6>]
      [0.50 rgb 0.01]
    }
    scale 0.4
  }
  finish {reflection 0.2}
}

// green crackle
#declare CrackleTex =
texture {
  pigment { color rgb <0.1, 0.5, 0.1> }
  normal {crackle 0.5 turbulence 0.2 scale 0.3}
}

// brown/blue pinwheel radial pattern
#declare PinwheelTex =
texture {
  pigment {
    radial
    frequency 8
    color_map {
      [0.00 blue 0.4]
      [0.50 blue 0.4]
      [0.60 rgb <0.5,0.6,0.9>]
    }
  }
}

#declare MSetTex =
texture {
  pigment {
    mandel 100
    color_map {
      [0.0  color MidnightBlue ]
      [0.2  color Yellow  ]
      [0.6  color Scarlet ]
      [0.8  color Magenta ]
      [1.0  color Gray10 ]
    }
    scale .25
    rotate x*90
    translate x*0.225
    //rotate y*45
  }
}

// ------------------------------------------------------------------
// declare the set of tiles as a union built by a loop
#declare Max  = 16;     // # of tiles across and down
#declare XMax = Max/2;
#declare ZMax = Max/2;

#declare ZCount = -ZMax;

#declare Tile_Set =
union {
    #while (ZCount <= ZMax)

      #declare XCount = -XMax;
      #while (XCount <= XMax)

      object {
        TileShape
        #if(XCount=0 & ZCount=0)
        // Center tile
          texture { MSetTex }
        #else
          #if (abs(XCount) = abs(ZCount))
            // An "X" pattern of tiles, diagonal through origin
            texture { CrystalTex }
            interior { CrystalInt }
          #else
            #if (abs(XCount)*abs(ZCount) < XMax)
              // A "fat plus" pattern, centered on origin
              texture {
                SlateTex
                rotate (XCount+ZCount)*10*y
                translate <-XCount, 0, -ZCount>  // alter texture per tile
              }
            #else
              #if (abs(mod(XCount,3)) = abs(mod(ZCount,2)))
              // An alternating sprinkle of remaining tiles
              texture {
                CrackleTex
                translate <-XCount, 0, -ZCount>  // alter texture per tile
              }
              #else
                texture { PinwheelTex }
              #end
            #end
          #end
        #end
        translate <XCount, 0, ZCount>
      }

      #declare XCount = XCount+1;
      #end  // Inner X loop

    #declare ZCount = ZCount+1;
    #end  // Outer Z loop
}

// ------------------------------------------------------------------
// make the tile object
object { Tile_Set rotate y*30}
