// Persistence Of Vision raytracer version 3.1 sample file.
// Weighted averaged normals example

global_settings { 
 assumed_gamma 2.2
 number_of_waves 1
}

#include "colors.inc"
#include "textures.inc"

camera { 
  location <0,3,-31>
  direction 3*z
} 

plane {  y,-1.01  pigment {checker Yellow,White}}

plane {  z, 4.01 hollow on pigment {checker Yellow,White}}

light_source { < 500, 500, -500> White*.75}
light_source { <-500,  50, -500> White*.3}

#declare Thing = plane{z,0.1 clipped_by{box{-2,2}}}

#default {pigment{White} finish{phong 1  phong_size 100}}

object{Thing
  normal {
    average
    normal_map {
      [gradient x, -1.0 scallop_wave scale 0.5]
      [gradient y, -1.0 scallop_wave scale 0.5]
    }
  }
  translate <-3,5.5,0>
}

object{Thing
  normal {
    average
    normal_map {
      [gradient x, 1.0 triangle_wave scale 0.5]
      [gradient y, 1.0 triangle_wave scale 0.5]
    }
  }
  translate <3,5.5,0>
}

object{Thing
  normal {
    average
    normal_map {
      [3.0 gradient x, 1.0 triangle_wave scale 0.5]
      [1.0 gradient y, 1.0 triangle_wave scale 0.5]
    }
  }
  translate <-3,1,0>
}

object{Thing
  normal {
    average
    normal_map {
      [waves 1.0 frequency 3 translate < 1, 1,0>]
      [waves 1.0 frequency 3 translate <-1,-1,0>]
    }
  }
  translate <3,1,0>
}
