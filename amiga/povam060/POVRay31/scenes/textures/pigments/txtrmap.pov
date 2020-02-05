// Persistence Of Vision raytracer version 3.1 sample file.
// Texture_map example

global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "textures.inc"


camera { 
  location <0,3,-31>
  direction 3*z
} 

plane {
  y, -1.01
  pigment {checker White, Magenta}
}

plane {
  z, 3.01
  hollow on
  pigment {checker White, Magenta}
}

light_source { <300, 500, -500> color Gray65}
light_source { <-50,  10, -500> color Gray65}


sphere{0,2
  texture {
    gradient x
    texture_map{
      [0.2 pigment{Jade} finish{ambient .2}]
      [0.4 pigment{radial frequency 10} finish{phong 1} rotate x*90]
      [0.6 pigment{radial frequency 10} finish{phong 1} rotate x*90]
      [0.8 pigment{DMFWood4} scale 3 rotate x*80]
    }
  }
  translate <-3,5.5,0>
}

sphere{0,2
  texture {
    wood
    texture_map{
      [0.2 pigment{Jade} finish{ambient .2}]
      [0.4 pigment{radial frequency 10} finish{phong 1} rotate x*90]
      [0.6 pigment{radial frequency 10} finish{phong 1} rotate x*90]
      [0.8 pigment{DMFWood4} scale 3 rotate x*80]
    }
  }
  translate <3,5.5,0>
}

sphere{0,2
  texture {
    checker
      texture { pigment{Jade} finish{ambient .2} }
      texture { pigment{radial frequency 10} finish{phong 1} rotate x*90}
  }
  translate <-3,1,0>
}

sphere{0,2
  texture {
    radial frequency 6 
    texture_map{
      [0.2 pigment{Jade} finish{ambient .2}]
      [0.4 pigment{radial frequency 10} finish{phong 1} ]
      [0.6 pigment{radial frequency 10} finish{phong 1} ]
      [0.8 pigment{DMFWood4} scale 3 rotate x*10]
    }
    rotate x*90
  }
  translate <3,1,0>
}
