// Persistence Of Vision raytracer version 3.1 sample file.
// Weighted averaged textures example


global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "textures.inc"

camera { 
  location <0,3,-31>
  direction 3*z
} 

plane {  y,-1.01  pigment {checker Yellow,White}}

plane {  z, 4.01  hollow on  pigment  {checker Yellow,White}}

light_source { <300, 500, -500> color Gray65}
light_source { <-50,  10, -500> color Gray65}

#declare Thing = plane{z,0.1 clipped_by{box{-2,2}}}


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

