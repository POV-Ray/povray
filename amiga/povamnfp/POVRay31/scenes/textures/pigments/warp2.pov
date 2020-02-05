// Persistence Of Vision raytracer version 3.1 sample file.
// Texture warp example


global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "textures.inc"

camera { 
  location <0,3,-29>
  direction 3*z
} 

plane {  y,-1.01  pigment {checker Yellow,White}}

plane {  z, 4.01  hollow on pigment {checker Yellow,White}}

light_source { <300, 500, -500> color Gray65}
light_source { <-50,  10, -500> color Gray65}

#declare Thing = plane{z,0.1 clipped_by{box{<-2.75,-2,-2>,<2.75,2,2>}}}

#declare Tree = pigment{DMFWood4 scale 2 translate <1/2,0,1> 
                        rotate x*85 translate 10*y}

object{Thing
  pigment{ Tree }
  translate <-3,5.5,0>
}

object{Thing
  pigment{ Tree 
    warp{repeat x*2}
  }
  translate <3,5.5,0>
}

object{Thing
  pigment{ Tree 
    warp{repeat x*2 offset z*0.25}
  }
  translate <-3,1,0>
}

object{Thing
  pigment{ Tree 
    warp{repeat x*2 flip y}
  }
  translate <3,1,0>
}

