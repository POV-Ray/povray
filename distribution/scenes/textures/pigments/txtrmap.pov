// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Texture_map example
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0}

#include "colors.inc"
#include "textures.inc"


camera {
  location <0,3,-31>
  right   x*image_width/image_height
  angle 21 
  look_at  <0,3,0>
}

plane {
  y, -1.01
  pigment {checker White, Black}
}

plane {
  z, 3.01
  hollow on
  pigment {checker White, Black}
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
  translate <-3,5.25,0>
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
  translate <3,5.25,0>
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
