// Persistence Of Vision raytracer version 3.1 sample file.
// Pigment_map example

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
