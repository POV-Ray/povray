// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File by Dan Farmer
// Radiosity demonstration

// updated to 3.5 radiosity by Christoph Hormann

#version 3.7;

#include "colors.inc"
#include "textures.inc"

#include "rad_def.inc"

global_settings {
  assumed_gamma 2.2
  
  radiosity {
    Rad_Settings(Radiosity_Default, off, off)
    //Rad_Settings(Radiosity_Debug, off, off)
    //Rad_Settings(Radiosity_Fast, off, off)
    //Rad_Settings(Radiosity_Normal, off, off)
    //Rad_Settings(Radiosity_2Bounce, off, off)
    //Rad_Settings(Radiosity_Final, off, off)

    //Rad_Settings(Radiosity_OutdoorLQ, off, off)
    //Rad_Settings(Radiosity_OutdoorHQ, off, off)
    //Rad_Settings(Radiosity_OutdoorLight, off, off)
    //Rad_Settings(Radiosity_IndoorLQ, off, off)
    //Rad_Settings(Radiosity_IndoorHQ, off, off)
  }
}

camera {
  location <-1.5, 2, -29.9>
  direction z * 1.75
  up y
  right     x*image_width/image_height
  look_at <0.5, -1.0, 0.0>
}

#declare Dist=15;
//#declare L = 0.65;
//#declare L = 0.35;
#declare L = 0.45;

light_source { <0, 9.5, 0>
  color rgb L
  fade_distance Dist fade_power 2
  shadowless
}

light_source { <-5, 7.5, 10.>
  color rgb L
  fade_distance Dist fade_power 2
  shadowless
}


//#declare Ambient = 0.35;
#declare Ambient = 0.0;

box { -1, 1
    scale <10, 10, 30>
    pigment { White }
    finish { ambient Ambient }
    inverse
}

box { -1, 1 scale <9, 8, 0.2>
    pigment {
        gradient z
        color_map {
            [0.0 color Red ]
            [0.5 color Red ]
            [0.5 color Blue ]
            [1.0 color Blue ]
        }
        translate -z*0.5
    }
    finish { ambient Ambient }
    rotate y*90
    rotate y*(clock*360)
    translate z*10
}

