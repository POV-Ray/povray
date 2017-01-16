// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.
//
// Persistence Of Vision Ray Tracer ('POV-Ray') sample file.
//
// Slope pattern example slope.pov.
//
// Date: August 30 2001
// Auth: Rune S. Johansen
// Updated: January 2017
//
// -w800 -h600 +a0.3

#version 3.71;
global_settings { assumed_gamma 1.0 }

camera {
    location <0,1,-40>
    right    x*image_width/image_height
    angle    50
    look_at  <0,3,0>
}

// A bright blue sky.
#declare White      = srgb <1,1,1>;
#declare SkyColor00 = srgb <0.8,0.9,1.0>;
#declare SkyColor01 = srgb <0.5,0.6,0.8>;
sky_sphere {
    pigment {
        gradient y
        color_map {
            blend_mode 2 blend_gamma 2.5
            [0.0, White]
            [0.2, SkyColor00]
            [1.0, SkyColor01]
        }
    }
}

// Some light.
light_source {<200,200,-200>, color rgb 1.2}

// And some simple water.
#declare Black = srgb <0,0,0>;
plane {
    y, 0
    pigment { Black }
    normal {bumps 0.03 scale 0.05}
    finish {reflection 0.7}
}


// Here are the basic textures.

// Plain yellowish sand for the beaches.
#declare BeachColor00 = srgb <0.9,0.8,0.5>;
#declare Sand_Texture = texture {
    pigment { BeachColor00 }
}

// A green texture for grass, plants, trees, etc.
#declare VegeColor00 = srgb <0.1,0.6,0.1>;
#declare Vege_Texture = texture {
    pigment { VegeColor00 }
}

// A brown texture for the soil, where the plants can't grow.
#declare SoillColor00 = srgb <0.7,0.6,0.5>;
#declare Soil_Texture = texture {
    pigment { SoillColor00 }
}

// A grey texture for the rock, where there is not even any soil.
#declare RockColor00 = srgb <0.6,0.6,0.6>;
#declare Rock_Texture = texture {
    pigment { RockColor00 }
}

// A white texture for the snow.
#declare Snow_Texture = texture {
    pigment { White }
    finish {ambient 0.006 diffuse 0.8}
}

// Now for the slope pattern textures.
// Before looking into these, try having a look at the
// texture of the height_field at the bottom of this file.

// A texture that has vegetation on the flat and low areas,
// soil on the steeper and higher areas, and rock on the
// steepest and highest areas.
// Notice how the altitude values fit to the pattern values
// in the texture_map of the height_field below.
#declare Vege2Rock_Texture = texture {
    slope {
        -y*3, 0, 0.5
        altitude y, 0.005, 0.400
    }
    texture_map {
        [0.45, Vege_Texture]
        [0.55, Soil_Texture]
        [0.65, Soil_Texture]
        [0.65, Rock_Texture]
    }
}

// A texture that has snow on the flat and high areas,
// and rock on the steeper and lower areas.
// Notice how the altitude values fit to the pattern values
// in the texture_map of the height_field below.
// The altitude values have been switched around because we
// want the snow near the top and not near the bottom.
#declare Rock2Snow_Texture = texture {
    slope {
        -y*2, 0, 0.5
        altitude y, 1.000, 0.400
    }
    texture_map {
        [0.75, Snow_Texture]
        [0.75, Rock_Texture]
    }
}


// Here is the mountain. height_field passes x & y,
// (u,v unit square) coordinates to Fn01 function.
#include "functions.inc"
#declare Fn00 = function (x,y,z) {
    pow(max(0,1-(x*x+y*y+(f_wrinkles(x,y,z)*2.5))),2.5)
}
#declare Fn01 = function (x,y,z) {
    Fn00((x-0.5)*2,(y-0.5)*2,z)
}
height_field {
   function 1200, 1200 { Fn01(x,y,z) }
   scale <80,12,80>
   translate <-40,-2,-30>
   texture {
      gradient y
      scale 10
      texture_map { // Notice the values in this texture_map.
                    // Then compare them to the altitude values
                    // in the slope pattern textures above.

         [0.000, Sand_Texture] // At the foot of the mountain
         [0.005, Sand_Texture] // we want some plain sand.

         [0.005, Vege2Rock_Texture] // Then some vegetation,
         [0.400, Vege2Rock_Texture] // soil and rock.

         [0.400, Rock2Snow_Texture] // Then from rock to snow
         [1.000, Rock2Snow_Texture] // at the top.

      }
   }
}
