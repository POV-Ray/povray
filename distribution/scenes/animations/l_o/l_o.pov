// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File by Dan Farmer
// Lambda/omega demonstration

#version 3.7;

global_settings {
  assumed_gamma 1.5
  }

#include "colors.inc"

camera {
    location <0, 0, -10>
    right     x*image_width/image_height
    angle 55
    look_at 0
}

light_source { <-20, 30, -25> rgb 1 }

#declare L = clock * 3;

#declare Font="cyrvetic.ttf"

union {
    text{ ttf Font
        concat("lambda=",str(L,1,2)),0.1,0
        scale <1.25, 1.25, 2>
        translate <-3, 1, -1>
        pigment { Cyan }
    }
    box { <-4,-0.5, -1> <4, 0.5, 1>
        pigment {
            gradient y
            turbulence 1
            lambda L
            color_map {
                [0.2 SteelBlue ]
                [0.5 White ]
                [0.9 Red ]
            }
            scale 0.5
        }
    }
    translate y*1.25
}

#declare O = clock;
union {
    text{ ttf Font
        concat("omega=",str(O,1,2)),0.1,0
        scale <1.25, 1.25, 2>
        translate <-3, 1, -1>
        pigment { Cyan }
    }
    box { <-4,-0.5, -1> <4, 0.5, 1>
        pigment {
            gradient y
            turbulence 1
            omega O
            color_map {
                [0.2 SteelBlue ]
                [0.5 White ]
                [0.9 Red ]
            }
            scale 0.5
        }
    }
    translate -y*1.65
}

plane { z, 10 hollow on pigment { Plum }}
