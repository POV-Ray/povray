// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Demo of extended light sources by Steve Anger
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.6;

global_settings {
  assumed_gamma 1.0
}

#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"


// A rather boring texture but it renders quick
#declare Text_Texture = texture {
   pigment { Red }
   finish {
      phong 2.0
      phong_size 80
      ambient 0.1
      diffuse 1.5
   }
}

#declare Letter_S = union {
    difference {
       torus { 4.0, 1.5 rotate 90*x }
       box { <0, -5.5, -1.5>, <5.5, 0, 1.5> }

       translate 4*y
    }

    difference {
       torus { 4.0, 1.5 rotate 90*x }
       box { <-5.5, 0, -1.5>, <0, 5.5, 1.5> }

       translate -4*y
    }

    sphere { <4, 4, 0>, 1.5 }
    sphere { <-4, -4, 0>, 1.5 }

}

#declare Letter_O = union {
    torus { 4.0, 1.5
        rotate 90*x

        clipped_by { box { <-5.5, 0, -1.5> <5.5, 5.5, 1.5> } }
        translate 4*y
    }

    torus { 4.0, 1.5
        rotate 90*x

        clipped_by { box { <-5.5, -5.5, -1.5> <5.5, 0, 1.5> } }
        translate -4*y
    }

    cylinder { <-4, -4, 0>, <-4, +4, 0>, 1.5 }
    cylinder { <+4, -4, 0>, <+4, +4, 0>, 1.5 }

}

#declare Letter_F = union {
    cylinder { <-4, -8, 0>, <-4, 8, 0>, 1.5 }
    cylinder { <-4, 0, 0>, <1.5, 0, 0>, 1.5 }
    cylinder { <-4, 8, 0>, <4, 8, 0>, 1.5 }

    sphere { <-4, -8, 0>, 1.5 }
    sphere { <-4, 8, 0>, 1.5 }
    sphere { <4, 8, 0>, 1.5 }
    sphere { <1.5, 0, 0>, 1.5 }

}

#declare Letter_T = union {
    cylinder { <0, -8, 0>, <0, 8, 0>, 1.5 }
    cylinder { <-4, 8, 0>, <4, 8, 0>, 1.5 }

    sphere { <-4, 8, 0>, 1.5 }
    sphere { <+4, 8, 0>, 1.5 }
    sphere { <0, -8, 0>, 1.5 }

}


// Put the letters together
union {
    object { Letter_S  translate -20*x }
    object { Letter_O  translate  -7*x }
    object { Letter_F  translate   7*x }
    object { Letter_T  translate  20*x }

    texture { Text_Texture }

    translate 9.5*y
}

// Floor
plane { y, 0
    pigment { Tan }
    finish {
        ambient 0.0
        diffuse 0.8
    }
}

// Something to light the front of the text
light_source { <0, 30, -90> color Gray30 }

// An extended area spotlight to backlight the letters
light_source {
   <0, 50, 100> color White

   // The spotlight parameters
   spotlight
   point_at <0, 0, -5>
   radius 6
   falloff 22

   // The extended area light paramaters
   area_light <6, 0, 0>, <0, 6, 0>, 9, 9
   adaptive 0
   jitter
}

camera {
    direction <0, 0, 1.5>
    location <0, 30, -90>
    look_at <0, 0, -2>
}
