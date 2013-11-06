// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Drums, the new improved version  Feb. 1992
// Copyright Dan Farmer 1992
// Time: a little over 3 hours no anti-aliasing, 640,480 on 33/486
//       7 hrs, 22 min +a0.05
//
// -w320 -h240
// -w800 -h600 +a0.3

/*
Fractint 17 parameter file to reconstruct REDNEWT.PNG:

Clip the Red Newton block below and save to a file named REDNEWT.PAR.
Run FRACTINT, press @ to get to the par file screen, press F6 to select
REDNEWT.PAR. Make it as large as you think your memory will afford... I used
1024x768 myself.

Red Newton         { ; Used as backdrop in DRUMS2.PNG
  reset type=complexnewton passes=1
  corners=-4.316552/-6.725229/9.54158/9.251383/-5.04438/10.512019
  params=8/0/4/8 maxiter=32767 inside=0 periodicity=0
  colors=000700<13>000000000000000<124>y00z00y00<108>800
  }

*/

#version 3.6;

global_settings {
  assumed_gamma 2.2
  max_trace_level 20
}

#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"
#include "drums.inc"


/*----------------------------- Viewer and lights -----------------------------*/
camera {
   location <0.0, 3.0, -90.0>
   direction <0.0, 0.0, 2.0>
   //   direction <0.0, 0.0,  3.0>  (Close up view, nice, too.)
   up  <0.0,  1.0,  0.0>
   right x*image_width/image_height
   look_at <0.0, 8.0, 0.0>
}


#declare Spacing = 15;
#declare Brightness = 0.5;



// Light source #1
union {
   light_source { <-Spacing, 0.0, 0.0> color red Brightness }
   light_source { <0.0, Spacing, 0.0>  color green Brightness }
   light_source { <Spacing, 0.0, 0.0>  color blue Brightness }

   pigment { White } // Doesn't do anything but suppresses a warning

   translate -100*z
   rotate 80*x
}


// Light source #2
union {
   light_source { <-Spacing, 0.0, 0.0> color red Brightness }
   light_source { <0.0, Spacing, 0.0>  color green Brightness }
   light_source { <Spacing, 0.0, 0.0>  color blue Brightness }

   pigment { White }

   translate -100*z
   rotate <30, 30, 0>
}

// Light source #3
union {
   light_source { <-Spacing, 0.0, 0.0> color red Brightness }
   light_source { <0.0, Spacing, 0.0> color green Brightness }
   light_source { <Spacing, 0.0, 0.0> color blue Brightness }

   pigment { White }

   translate -100*z
   rotate <30, -30, 0>
}

sphere {
   <0, 0, 0>, 1000
   hollow on

   pigment {
      image_map { png "rednewt.png" interpolate 4 }
      translate <-0.5, -0.5, 0.0>
      scale 800
   }
   finish {
      ambient 1
      diffuse 0.5
   }
}

// Floor
plane {
   y, -10.0

   pigment { Black }
   finish {
      ambient 0.2
      diffuse 0.8
      reflection 0.25
   }
}


// Left drumset

object {
   HalfSet
   rotate 20.0*y
   translate -13.0*x
}


// Right drumset
object {
   HalfSet
   rotate -20.0*y
   translate 13.0*x
}
