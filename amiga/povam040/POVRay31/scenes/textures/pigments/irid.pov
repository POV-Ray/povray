// Persistence Of Vision raytracer version 3.1 sample file.
// Iridescence "amount" example


global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"

camera {
  location <0, 0, -10>
  up y
  right x*1.3333
  direction z*3
  look_at 0
}
light_source { < 150, 150, -2500> colour Gray90 }

// Top row: varies irid amount
union {
    sphere { <-1, 0, 0>, 0.45
        pigment { White }
        finish {
            Shiny
            diffuse 0.2
            irid {0.25 thickness 2/4 turbulence 0.75}
        }
    }
    sphere { < 0, 0, 0>, 0.45
        pigment { White }
        finish {
            Shiny
            diffuse 0.2
            irid {0.5 thickness 2/4 turbulence 0.75}
        }
    }
    sphere { < 1, 0, 0>, 0.45
        pigment { White }
        finish {
            Shiny
            diffuse 0.2
            irid  {0.75 thickness 2/4 turbulence 0.75}
        }
    }
translate y*1
}

// Center row: varies irid thickness
union {
    sphere { <-1, 0, 0>, 0.45
        pigment { White }
        finish {
            Shiny
            diffuse 0.2
            irid {0.5 thickness 0.5 turbulence 0.5}
        }
    }
    sphere { < 0, 0, 0>, 0.45
        pigment { White }
        finish {
            Shiny
            diffuse 0.2
            irid {0.5 thickness 2 turbulence 0.5}
        }
    }
    sphere { < 1, 0, 0>, 0.45
        pigment { White }
        finish {
            Shiny
            diffuse 0.2
            irid  {0.5 thickness 8 turbulence 0.5}
        }
    }
}

// Right bottom: varies irid turbulence
union {
    sphere { <-1, 0, 0>, 0.45
        pigment { White }
        finish {
            Shiny
            diffuse 0.2
            irid {0.5 thickness 2 turbulence 0.1}
        }
    }
    sphere { < 0, 0, 0>, 0.45
        pigment { White }
        finish {
            Shiny
            diffuse 0.2
            irid {0.5 thickness 2 turbulence 1}
        }
    }
    sphere { < 1, 0, 0>, 0.45
        pigment { White }
        finish {
            Shiny
            diffuse 0.2
            irid  {0.5 thickness 2 turbulence 2}
        }
    }
translate -y*1
}

