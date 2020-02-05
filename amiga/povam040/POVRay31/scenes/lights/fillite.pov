// Persistence Of Vision raytracer version 3.1 sample file.
// File by Dan Farmer
// Shadowless lighting example


global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"

background { Blue }
camera {
  location <0, 3, -5>
  direction z * 1.25
  right x*1.3333
  look_at 0
}

light_source { <20, 40, -30> White
    shadowless
}
light_source { <-5, 2, -3> White
    shadowless
}

union {
    box { <-1, -1, -1> <1, 1, 1> }
    sphere { <0,1,0>, 1 }
    rotate -y*45
    pigment { Scarlet }
    finish {
        Shiny
        ambient 0
        diffuse 0.8
    }
}
plane { y,-1
    pigment { checker Yellow, Blue scale 0.3}
    finish { ambient 0 diffuse 1 }
}
