// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Iridescence "amount" example
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0}

#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"

camera {
  location <0, 0, -10>
  right   x*image_width/image_height
  angle 23
  look_at  <0, 0, 0>
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

