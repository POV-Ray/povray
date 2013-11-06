// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Weighted averaged normals example
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

global_settings {
  assumed_gamma 1.0
  number_of_waves 1
}

#include "colors.inc"
#include "textures.inc"

camera {
  location  <0,3,-31>
  right     x*image_width/image_height
  direction 3*z
}


light_source { < 500, 500, -300> White*.75}
light_source { <-500,  50, -300> White*.25}

#declare Thing = plane{z,0.1 hollow on clipped_by{box{-2,2}}}

#default {pigment{White} finish{phong 1  phong_size 100}}

object{Thing
  normal {
    average
    normal_map {
      [gradient x, -5.0 scallop_wave scale 0.5]
      [gradient y, -5.0 scallop_wave scale 0.5]
    }
  }
  translate <-3,5.5,0>
}

object{Thing
  normal {
    average
    normal_map {
      [gradient x, 5.0 triangle_wave scale 0.5]
      [gradient y, 5.0 triangle_wave scale 0.5]
    }
  }
  translate <3,5.5,0>
}

object{Thing
  normal {
    average
    normal_map {
      [3.0 gradient x, 5.0 triangle_wave scale 0.5]
      [1.0 gradient y, 5.0 triangle_wave scale 0.5]
    }
  }
  translate <-3,1,0>
}

object{Thing
  normal {
    average
    normal_map {
      [waves 2.0 frequency 3 translate < 1, 1,0>]
      [waves 2.0 frequency 3 translate <-1,-1,0>]
    }
  }
  translate <3,1,0>
}
