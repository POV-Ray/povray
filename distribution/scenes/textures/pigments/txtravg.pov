// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Weighted averaged textures example
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 2.2}

#include "colors.inc"
#include "textures.inc"

camera {
  location <0,3,-31>
  right   x*image_width/image_height
  angle 23 
  look_at  <0,3,0>
}


plane {  z, 4.01  hollow on pigment  { color rgb<1,1,1>}}

light_source { <300, 500, -500> color Gray65}
light_source { <-50,  10, -500> color Gray65}

#declare Thing = plane{z,0.1 hollow on clipped_by{box{-2,2}}}


object{Thing
  texture {
    average
    texture_map {
      [pigment{Jade} finish{ambient .2}]
      [pigment{radial frequency 10} finish{phong 1} rotate x*90]
    }
  }
  translate <-3,5.5,0>
}

object{Thing
  texture {
    average
    texture_map {
      [pigment{DMFWood4} scale 3 rotate x*80]
      [pigment{radial frequency 10} finish{phong 1} rotate x*90]
    }
  }
  translate <3,5.5,0>
}

object{Thing
  texture {
    average
    texture_map {
      [3.0 pigment{Jade} finish{ambient .2}]
      [1.0 pigment{radial frequency 10} finish{phong 1} rotate x*90]
    }
  }
  translate <-3,1,0>
}

object{Thing
  texture {
    average
    texture_map {
      [pigment{radial frequency 10} finish{phong 1} rotate x*90 translate < 1, 1,0>]
      [pigment{radial frequency 10} finish{phong 1} rotate x*90 translate <-1,-1,0>]
    }
  }
  translate <3,1,0>
}

