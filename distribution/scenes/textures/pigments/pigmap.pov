// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Pigment_map example
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
  angle 23
  look_at <0,3,01>
}

plane {
  y, -1.01
  pigment {checker White, Black rotate<0,45,0>}
}

plane {
  z, 3.01
  hollow on
  pigment {checker White, Black rotate<0,0,45>}
}

light_source { <300, 500, -500> color Gray65}
light_source { <-50,  10, -500> color Gray65}

box{<-2,-2,0>,<2,2,1>
  pigment {
    gradient x
    pigment_map{
      [0.2 Jade]
      [0.4 radial frequency 10 rotate x*90]
      [0.6 radial frequency 10 rotate x*90]
      [0.8 DMFWood4 scale 3 rotate x*80]
    }
  }
  translate <-3,5.50>
}

box{<-2,-2,0>,<2,2,1>
  pigment {
    wood
    pigment_map{
      [0.2 Jade]
      [0.4 radial frequency 10 rotate x*90]
      [0.6 radial frequency 10 rotate x*90]
      [0.8 DMFWood4 scale 3 rotate x*80]
    }
  }
  translate <3,5.50>
}
box{<-2,-2,0>,<2,2,1>
  pigment {
    checker
      pigment { Jade }
      pigment { radial frequency 10 rotate x*90}
  }
  translate <-3,1,0>
}
box{<-2,-2,0>,<2,2,1>
  pigment {
    radial frequency 6
    pigment_map{
      [0.2 Jade]
      [0.4 radial frequency 10]
      [0.6 radial frequency 10]
      [0.8 DMFWood4 scale 3 rotate x*10]
    }
    rotate x*90
  }
  translate <3,1,0>
}
