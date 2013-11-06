// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File by Drew Wells
// Title-"Not a Trace of Reality"
// Compuserve Hall Of Fame award winner
// This one is hard to describe and easy to look at. Have fun with it!
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.6;

global_settings {
  assumed_gamma 2.2
  max_trace_level 5
}

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"

#declare It =  sphere { <0, 0, 0>, 1 scale <4.0, 0.3, 0.3> }

/* Camera/Viewer */

// This camera is different from any of the other sample scene files
// The camera is looking in the negative Z direction
// and right is in the negative X direction.
camera {
   angle 55 // direction <0.0, 0.0,  1.5>
   up  <0.0,  1.0,  0.0>
   right -x*image_width/image_height
   location < -15.0,  5.0,  120.0>
   look_at <10.0, 12.0, 55.0>
}

#declare Purple_Clouds = pigment {
   bozo
   turbulence 0.6
   colour_map {
      [0.0 0.5   colour red 0.9 green 0.5  blue 0.6
                 colour red 0.4 green 0.0  blue 0.4]
      [0.5 0.6   colour red 1.0 green 1.0  blue 1.0 filter 1.0
                 colour red 1.0 green 1.0  blue 1.0 filter 1.0 ]
      [0.6 1.001 colour red 1.0 green 1.0  blue 1.0 filter 1.0
                 colour red 1.0 green 1.0  blue 1.0 filter 1.0]
   }
}

#declare Sunset_Sky = pigment {
   gradient y

   colour_map {
      [0.0 0.4   colour red 0.8 green 0.0 blue 0.0
                 colour red 0.4 green 0.0 blue 0.4]
      [0.4 0.6   colour red 0.4 green 0.0 blue 0.4
                 colour red 0.0 green 0.0 blue 0.2]
      [0.6 1.001 colour red 0.0 green 0.0 blue 0.2
                 colour red 0.0 green 0.0 blue 0.0]
   }
   scale 700.0
}

#declare Twister = union {
   #include "ntreal.inc"

   pigment {
      White_Wood
      scale 3.0
   }
   finish {
      crand 0.05
      ambient 0.1
      diffuse 0.99
   }
}

#declare Slice = sphere {
   <0, 0, 0>, 1
   translate <0.0, 0.0, 3.0>
   scale <0.25, 1.00, 0.25>
}

#declare Thing = union {
   object { Slice }
   object { Slice rotate -20.0*y }
   object { Slice rotate -40.0*y }
   object { Slice rotate -60.0*y }
   object { Slice rotate -80.0*y }
   object { Slice rotate -100.0*y }
   object { Slice rotate -120.0*y }
   object { Slice rotate -140.0*y }
   object { Slice rotate -160.0*y }
   object { Slice rotate  180.0*y }
   object { Slice rotate -200.0*y }
   object { Slice rotate -220.0*y }
   object { Slice rotate -240.0*y }
   object { Slice rotate -260.0*y }
   object { Slice rotate -280.0*y }
   object { Slice rotate -300.0*y }
   object { Slice rotate -320.0*y }
   object { Slice rotate -340.0*y }

   pigment { color red 0.8 green 0.22 blue 0.1 }
   normal {
      bumps 0.3
      scale 0.1
   }
   finish {
      ambient 0.1
      diffuse 0.9
      phong 0.75
      phong_size 30.0
   }
}

#declare Slice2 = sphere { <0.0, 0.0, 0.5>, 0.1 }

#declare Thing2 = union {
   object { Slice2 }
   object { Slice2 rotate -20.0*y }
   object { Slice2 rotate -40.0*y }
   object { Slice2 rotate -60.0*y }
   object { Slice2 rotate -80.0*y }
   object { Slice2 rotate -100.0*y }
   object { Slice2 rotate -120.0*y }
   object { Slice2 rotate -140.0*y }
   object { Slice2 rotate -160.0*y }
   object { Slice2 rotate  180.0*y }
   object { Slice2 rotate -200.0*y }
   object { Slice2 rotate -220.0*y }
   object { Slice2 rotate -240.0*y }
   object { Slice2 rotate -260.0*y }
   object { Slice2 rotate -280.0*y }
   object { Slice2 rotate -300.0*y }
   object { Slice2 rotate -320.0*y }
   object { Slice2 rotate -340.0*y }

   pigment { color red 0.1 green 0.22 blue 0.8 }
   finish {
      ambient 0.1
      diffuse 0.9
      phong 0.75
      phong_size 30.0
   }
}


/*******************************************/
/*******************************************/

object {
   Twister
   rotate <-15.0, 30.0, 0.0>
   translate <-16.0, 7.7, 61.5>
}

object {
   Twister
   rotate <-15.0, 0.0, -10.0>
   translate <0.0, 1.0, 88.0>
}

object {
   Twister
   rotate <0.0, 45.0, 0.0>
   translate <13.0, 25.0, 40.0>
}

object {
   Twister
   rotate <-15.0, 0.0, -10.0>
   translate <26.0, 14.0, 70.0>
}

/* Little Things */
object {
   Thing
   scale <1.5, 3.0, 1.5>
   translate < -11.0, 1.55, 95.0>
}

object {
   Thing2
   scale 6.0
   rotate <-10.0, 30.0, 0.0>
   translate <-11.0, 1.55, 95.0>
}

object {
   Thing
   scale <1.5, 3.5, 1.5>
   translate <-10.0, 10.55, 95.0>
}

object {
   Thing2
   scale 10.0
   rotate <0.0, 0.0, -35.0>
   translate < -10.0, 10.55, 95.0>
}

object {
   Thing
   scale <1.5, 3.5, 1.5>
   translate < -4.0, 4.0, 80.0>
}

object {
   Thing2
   scale 6.0
   rotate <-30.0, 0.0, 20.0>
   translate < -4.6, 5.55, 80.0>
}


object {
   Thing
   scale <1.5, 3.5, 1.5>
   translate < 11.0, 1.6, 90.0>
}

object {
   Thing2
   scale <8.0, 8.0, 8.0>
   rotate <0.0, 45.0, 20.0>
   translate < 10.0, 7.0, 90.0>
}

/*cloud hills*/
object {
   Paraboloid_Y
   scale <40.0, 10.0, 77.0>
   rotate 180.0*z
   translate <0.0, 21.0, -28.0>

   texture {
      pigment {
         Purple_Clouds
         scale 5.0
      }
      finish {
         ambient 0.5
         diffuse 0.9
      }
   }
}

/*cloud hill to right*/
object {
   Paraboloid_Y
   scale <30.0, 10.0, 40.0>
   rotate 180.0*z
   translate <40.0, 14.0, 50.0>

   texture {
      pigment {
         Purple_Clouds
         scale < 7.0, 5.0, 5.0>
      }
      finish {
         ambient 0.5
         diffuse 0.9
      }
   }
}

/*The Sun*/
light_source { <150.0, 40.0, 1200.0> colour White }

/*sky*/
sphere {
   <0.0, 0.0, 0.0>, 2000.0
   inverse

   texture {
      pigment {
         Sunset_Sky
         translate 200.0*y
         scale 1.2
      }
      finish {
         ambient 0.6
         diffuse 0.0
      }
   }
}

