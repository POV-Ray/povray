// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File By Dan Farmer and Tim Wegner
//
// -w320 -h240
// -w800 -h600 +a0.3

// Low resolution versions of the images MTMAND.POT and MTMANDJ.PNG are
// included so that you can render this scene, however in order to really
// do the scene justice you should substitute higher resolution versions
// of these images. MTMAND.PAR contains the fractal parameters to generate
// both images using the DOS program FRACTINT.

#version 3.7;

global_settings {
  assumed_gamma 2.2
  }


#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"
#declare PlanetColor = color red 0.65 green 0.65 blue 1.00;

// The following constants simply make it easier to swap images of different
// scales. Change ScaleX and ScaleZ if you generated the MTMAND.POT file at
// a resolution different from 360 x 480 which Dan used.

#declare ScaleX = 0.5625;    // 360/(pot image width)
#declare ScaleZ = 1.0 ;      // 480/(pot image height)

camera {
   location  <-150.0, 300.0, -650.0>
   angle 20  //direction <   0.0,   0.0,    4.0> // "Telephoto" lens, "compresses" distance
   up        <0, 1, 0>        // The distance also seems to smooth the h-field
   right     x*image_width/image_height // keep propotions with any aspect ratio
   look_at   <-40.0, 150.0, 0.0>
}

// Define a couple of colors for the light sources.
#declare MainLight = color red 0.8 green 0.8 blue 0.8;
#declare FillLight = color red 0.23 green 0.23 blue 0.25;
// Light source (main)
light_source { <-400.0, 300.0, -60.0> color MainLight }
// Light source ( shadow filler )
light_source { <-50.0, 300.0, -60.0> color FillLight }

height_field  {
   // 16 bit continuous potential Fractint fractal,
   // floating point activated to allow a large bailout value
   // Fractint parameters are:
   //    type=mandel
   //    corners=-0.1992/-0.1099914/1.0000046/1.06707
   //    float=yes
   //    maxiter=1500
   //    potential=255/2200/1000/16bit
   //    savename=mtmand

   pot "mtmand.pot"
// pot "mtmand.pot" smooth // <== try this for high resolution renders
// png "mtmand.png" smooth // <== do this instead if no POT available
   water_level 0.0

   pigment { White }
   finish {
      crand 0.025         // dither  - not used often, but this image needs it.
      ambient 0.2         // Very dark shadows
      diffuse 0.8         // Whiten the whites
      phong 0.75          // Fairly shiny
      phong_size 100.0    // with tight highlights
      specular 1.0
      roughness 0.005
   }

   scale <640, 256, 480>
   scale <ScaleX, 0.5, ScaleZ> // Reduce the height, scale to 360 x 480
   translate <-180, 0.0, -240>  // Center the image by half of ScaleX and ScaleZ
}

// Sky sphere
sphere { <0.0, 0.0, 0.0>, 1200.0
   hollow on
   pigment {
      gradient y     // Fade from yellow to orange to red to black
      color_map {
         [0.00 0.10 color Yellow color Orange] // Yellow at horizon
         [0.10 0.15 color Orange color Red]    // Fade to orange to red
         [0.15 0.27 color Red   color Black ]  // then to dark red
         [0.27 1.01 color Black  color Black ] // to Black at zenith
      }
      quick_color SummerSky
      scale 1000                    // Big enough to surround the universe
      translate <0.0, -240.0, 0.0>  // This ajusts for the viewer position
   }
   finish {
      ambient 1.0                 // Keep objects from casting shadows
      diffuse 0.0                 // All light comes from ambient sources
   }
}


// Planet
sphere { <-95.0, 50.0, 600.0>, 35.0
   pigment { PlanetColor }
   normal {
      bump_map {  // Bump texture with corresponding julia image
         //    type=julia
         //    corners=-1.568/1.568/-1.176/1.176
         //    params=-0.1545957/1.0335373
         //    float=yes
         //    maxiter=256
         png  "mtmandj.png"
         bump_size 15.0
         interpolate 4.0  // Smooth the image
      }
       // mapped image is 1x1x1, with lower left corner at 0,0,0
      translate <-0.5, -0.5, 0.0>        // Center the image at origin
      scale <35.0, 35.0, 35.0>
      rotate 95*z                        // Tweak the positioning a little
   }
   finish {
      specular 0.35                    // Fairly "dull" surface
      roughness 0.5                    // spread the highlight
      ambient 0.0                      // Dark shadows
      diffuse 0.75
   }
}

// end of file
