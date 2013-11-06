// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File  by Dan Farmer.
// File creates a pentagram made up of dodecahedrons.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 2.2 }

#include "shapes.inc"
#include "shapes2.inc"
#include "colors.inc"

#declare AnObject =
object {
   Dodecahedron
   bounded_by {sphere{<0, 0, 0>, 1.5}}    // Ver 3.0: leave this in. -dmf
   translate y*1.8
   rotate x*72
}

#declare VeryDarkWood1 = color red 0.30 green 0.15 blue 0.09;
#declare DarkWood1 =  color red 0.60 green 0.30 blue 0.18;

#default {
   finish {
      ambient 0.2
      diffuse 0.8
      specular 0.75
      roughness 0.008
      reflection 0.15
   }
}

#declare Wooden1 =
pigment {
   wood
   turbulence 0.04
   octaves 3
   scale <0.05, .05, 1>
   colour_map {
      [0.00, 0.10 color DarkWood1     color DarkWood1]
      [0.10, 0.90 color DarkWood1     color VeryDarkWood1]
      [0.90, 1.01 color VeryDarkWood1 color VeryDarkWood1]
   }
   rotate y*90
}

#declare DarkWood2 = color red 0.52 green 0.37 blue 0.26;
#declare VeryDarkWood2  = color red 0.42 green 0.26 blue 0.15;
#declare Wooden2 =
pigment {
   wood
   turbulence 0.03
   octaves 4
   scale <0.05, .05, 1>
   colour_map {
      [0.00, 0.10 color DarkWood2     color DarkWood2]
      [0.10, 0.90 color DarkWood2     color VeryDarkWood2]
      [0.90, 1.01 color VeryDarkWood2 color VeryDarkWood2]
   }
   rotate y*90
}

#declare DarkWood3  = colour red 0.4 green 0.133 blue 0.066;
#declare VeryDarkWood3  = colour red 0.2 green 0.065 blue 0.033;
#declare Wooden3 =
pigment {
   wood
   turbulence 0.05
   octaves 2
   scale <0.05, .05, 1>
   colour_map {
      [0.00, 0.10 color DarkWood3      color DarkWood3]
      [0.10, 0.90 color DarkWood3      color VeryDarkWood3]
      [0.90, 1.01 color VeryDarkWood3  color VeryDarkWood3]
   }
   rotate y*90
}

#declare DarkWood4 = colour red 0.888 green 0.600 blue 0.3;
#declare VeryDarkWood4  = colour red 0.6 green 0.4 blue 0.2;
#declare Wooden4 =
pigment {
   wood
   turbulence 0.04
   octaves 3
   scale <0.05, .05, 1>
   colour_map {
      [0.00, 0.10 color DarkWood4      color DarkWood4]
      [0.10, 0.90 color DarkWood4      color VeryDarkWood4]
      [0.90, 1.01 color VeryDarkWood4  color VeryDarkWood4]
   }
   rotate y*90
}

#declare DarkWood5  = colour red 0.3 green 0.1 blue 0.05;
#declare VeryDarkWood5  = colour red 0.25 green 0.07 blue 0.038;
#declare Wooden5 =
pigment {
   wood
   turbulence 0.05
   octaves 6
   scale <0.075, .075, 1>
   colour_map {
      [0.00, 0.10 color DarkWood5      color DarkWood5]
      [0.10, 0.90 color DarkWood5      color VeryDarkWood5]
      [0.90, 1.01 color VeryDarkWood5  color VeryDarkWood5]
   }
   rotate y*89
}

camera {
   location  <0, 0, -6>
   angle 40 
   right   x*image_width/image_height
   look_at   <0, 0,   0>
}

light_source {<5,   5, -30>  color White }
light_source {<0, -10, -10>  color Gray80 }


// Counter-clockwise, from top

object { AnObject texture {pigment{Wooden1} } }

object { AnObject texture {pigment{Wooden2} } rotate  z*72 }

object { AnObject texture {pigment{Wooden4} } rotate z*144 }

object { AnObject texture {pigment{Wooden3} } rotate z*216 }

object { AnObject texture {pigment{Wooden5} } rotate z*288 }

background { Gray20 }

// end of file

