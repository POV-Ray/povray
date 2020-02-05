// Persistence Of Vision raytracer version 3.1 sample file.
// File by Douglas Muir
// Note: Original used 640 x 480 height field.
// This version is scaled down for distribution.
// Requires "fract003.png" plasma png for the height field.

global_settings { assumed_gamma 2.2 }

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"

camera {
   location <0.0, 80.0, -300.0>
   direction <0.0, 0.0, 1.0>
   up <0.0, 1.0, 0.0>
   right <4/3, 0.0, 0.0>
   look_at <0.0, 30.0, 200.0>
}

height_field {
   png "fract003.png" 
   water_level 0.4

   pigment {
      image_map { png "fract003.png" }
      quick_color White
      rotate 90*x
      scale <320.0, 1.0, 200.0>
      translate <0.0, 0.0, -1.0>
   }

   scale <320, 256, 200>
   scale <2.0, 0.5, 2.0>
   translate <-160.0, -63.5, -100.0>
   rotate 10*y
   translate <-80.0, 0.0, -30.0>
}

// Define the ocean surface 
plane { y, -10.0
   hollow on
   pigment { Aquamarine }
   normal {
      waves 0.06
      frequency 5000.0
      scale 1000.0
   }
   finish {
      ambient 0.1
      diffuse 0.1
      reflection 0.8
   }
}

// Put a floor underneath to catch any errant waves from the ripples 
plane { y, -11.0
   pigment { colour red 1.0 green 0.6 }
   finish {
      crand 0.05
      ambient 0.8
      diffuse 0.0
   }
}

// Now draw the sky 
sphere { <0.0, 0.0, 0.0>, 3500.0
   hollow on
   pigment {
      onion
      colour_map {
         [0.0 0.6  colour red 1.0 green 0.6 blue 0.0
                   colour red 0.3 green 0.6 blue 0.6]
         [0.6 1.0  colour red 0.3 green 0.6 blue 0.6
                   colour red 0.1 green 0.4 blue 0.6]
      }
      quick_colour red 0.7 green 0.7 blue 1.0
      scale <6000.0, 1700.0, 4000.0>
      translate <-1200.0, 220.0, 2500.0>
   }
   finish {
      ambient 0.8
      diffuse 0.0   /* we don't want clouds casting shadows on the sky */
   }
}


// Put in a few clouds 
plane { y, 300.0
   hollow on
   pigment {
      bozo
      turbulence 0.5
      colour_map {
         [0.0 0.6   colour red 1.0 green 1.0 blue 1.0 filter 1.0
                    colour red 1.0 green 1.0 blue 1.0 filter 1.0]
         [0.6 0.8   colour red 1.0 green 1.0 blue 1.0 filter 1.0
                    colour red 1.0 green 0.8 blue 0.1]
         [0.8 1.001 colour red 1.0 green 0.8 blue 0.1
                    colour red 0.8 green 0.4 blue 0.2]
      }
      quick_colour red 0.7 green 0.7 blue 1.0
      scale <1000.0, 200.0, 800.0>
   }
   finish {
      ambient 0.7
      diffuse 0.0
   }

   translate -450*x
   rotate 6*y
}

// Now to cast some light on the subject 
light_source { <-150.0, 250.0, -400.0> colour MediumGoldenrod }

// Now to cast some more light on the subject 
light_source {
   <0, 0, 0> colour red 1.0 green 0.7

   looks_like {
      sphere { <0.0, 0.0, 0.0>, 190.0
         pigment { colour red 1.0 green 0.6 filter 0.35 }
         finish { ambient 1.0 diffuse 0.0 }
      }
   }
       
   translate <-1300.0, 380.0, 2500.0>
}
