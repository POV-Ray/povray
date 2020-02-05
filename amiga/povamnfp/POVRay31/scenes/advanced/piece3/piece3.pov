// Persistence Of Vision raytracer version 3.1 sample file.

// piece 3
// by Truman Brown (12/91)

// Due to the large number of objects, you will probably have to
// have a lot of memory to render this scene.
// Rendering time using a 25Mhz 386 w/Cyrix fpu is approximately 60 hours.

// NOTE: The following comment by Truman is no longer relevant.  The
// gamma has been modified by the assumed_gamma setting below.  -dmf
//
//   The lighting is intentionally dim when the image is rendered.
//   You can enhance the image using PicLab's gamma, brightness, and
//   contrast command when you post-process the image.


global_settings { assumed_gamma 0.8 }

#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"


#declare YourTexture =
texture {  /* Dan Farmer's oak texture { */
   pigment {
      wood
      turbulence 0.05
      color_map {
         [0.00 0.10 color red 0.42 green 0.26 blue 0.15
         color red 0.42 green 0.26 blue 0.15]
         [0.10 0.90 color red 0.42 green 0.26 blue 0.15
         color red 0.52 green 0.37 blue 0.26]
         [0.90 1.01 color red 0.52 green 0.37 blue 0.26
         color red 0.52 green 0.37 blue 0.26]
      }
      rotate 90*x
      scale 3
   }
}

#declare BallTexture1 = /* Shiny sugar candy red */
texture {
   pigment { Firebrick }
   finish {
      ambient 0.0
      diffuse 0.6
      specular 1.00
      roughness 0.00001
   }
}

#declare RodTexture1 = /* Dim goldish */
texture {
   pigment { Sienna }
   finish {
      ambient 0.0
      diffuse 0.6
      specular 1.00
      roughness 0.00001
   }
}

#include "curly-q.inc" /* 1251 spheres rotating around in a ring    */
/* brought to you by Spiral 2.0 and ctds 1.6 */

camera {
   location  <  7.0,     50.0, -30.0 > /* Up high and in close.         */
   direction <  0.0,      0.0,   2.0 > /* Though this doesn't highlight */
   up        <  0.0,      1.0,   0.0 > /* the height of the piece, it   */
   right     <  4/3,      0.0,   0.0 > /* gives the effect i'm looking  */
   look_at   <  0.0,     15.0,   0.0 > /* for.  Feel free to change.    */
}

light_source { <99, 70, -90> color red 0.1 green 0.1 blue 0.1 }

light_source { <-35, 80, -80> color red 0.5 green 0.5 blue 0.5 }

#declare xrod = /* Horizontal left-right rod floating at <0 0 0> */
object {
   Cylinder_X
   clipped_by {
      plane {  x, 100.0 }
      plane { -x, 100.0 }
   }
   texture {
      RodTexture1
   }
   scale <1.0, 0.3, 0.3>
}

#declare zrod = /* Horizontal in-out rod floating at <0 0 0> */
object {
   Cylinder_Z
   clipped_by {
      plane {  z, 100.0 }
      plane { -z, 100.0 }
   }
   texture {
      RodTexture1
   }
   scale <0.3, 0.3, 1.0>
}

union {                                      /* Build a floor grid */
   object { zrod translate <-100, 0, 0> } /* extending beyond   */
   object { zrod translate < -92, 0, 0> } /* camera view to be  */
   object { zrod translate < -84, 0, 0> } /* captured in reflec-*/
   object { zrod translate < -76, 0, 0> } /* tions.             */
   object { zrod translate < -68, 0, 0> }
   object { zrod translate < -60, 0, 0> }
   object { zrod translate < -52, 0, 0> }
   object { zrod translate < -44, 0, 0> }
   object { zrod translate < -36, 0, 0> }
   object { zrod translate < -28, 0, 0> }
   object { zrod translate < -20, 0, 0> }
   object { zrod translate < -12, 0, 0> }
   object { zrod translate <  -4, 0, 0> }
   object { zrod translate <   4, 0, 0> }
   object { zrod translate <  12, 0, 0> }
   object { zrod translate <  20, 0, 0> }
   object { zrod translate <  28, 0, 0> }
   object { zrod translate <  36, 0, 0> }
   object { zrod translate <  44, 0, 0> }
   object { zrod translate <  52, 0, 0> }
   object { zrod translate <  60, 0, 0> }
   object { zrod translate <  68, 0, 0> }
   object { zrod translate <  76, 0, 0> }
   object { zrod translate <  84, 0, 0> }
   object { zrod translate <  92, 0, 0> }
   object { zrod translate < 100, 0, 0> }

   object { xrod translate < 0, 0, -100> }
   object { xrod translate < 0, 0,  -92> }
   object { xrod translate < 0, 0,  -84> }
   object { xrod translate < 0, 0,  -76> }
   object { xrod translate < 0, 0,  -68> }
   object { xrod translate < 0, 0,  -60> }
   object { xrod translate < 0, 0,  -52> }
   object { xrod translate < 0, 0,  -44> }
   object { xrod translate < 0, 0,  -36> }
   object { xrod translate < 0, 0,  -28> }
   object { xrod translate < 0, 0,  -20> }
   object { xrod translate < 0, 0,  -12> }
   object { xrod translate < 0, 0,   -4> }
   object { xrod translate < 0, 0,    4> }
   object { xrod translate < 0, 0,   20> }
   object { xrod translate < 0, 0,   36> }
   object { xrod translate < 0, 0,   52> }
   object { xrod translate < 0, 0,   64> }
   object { xrod translate < 0, 0,   72> }
   object { xrod translate < 0, 0,   80> }
   object { xrod translate < 0, 0,   88> }
   object { xrod translate < 0, 0,  100> }
//    bounded_by {
//       object { Cube scale <101.0, 0.5, 101.0> }
//    }
}

sphere { /* Big ellipsoid to reflect stuff */
   <0, 0, 0>, 1
   scale <6.25, 4.0, 6.25>
   translate <0.0, 14.5, 0.0>

   texture {
      pigment { SteelBlue }
      finish {
         ambient 0.0
         diffuse 0.6
         reflection 0.99
         specular 1.0
         roughness 0.00001
      }
   }
}

plane { /* Sub-plane { to reflect poles and curly-q */
   y, -1.5
   texture {
      pigment {
         marble
         turbulence 0.65
         color_map {
            [0.0   0.67   color Black
                          color DimGray]
            [0.67  0.775  color DimGray
                          color SteelBlue]
            [0.775 1.001  color SteelBlue
                          color Black]
         }
         scale <10, 8, 8>
         scale <20, 20, 20> /* Double scaled while debugging. */
      }
      finish {
         ambient 0.05
         diffuse 0.75
         specular 1.0
         roughness 0.00002432
         reflection 0.95
      }
   }                /* This gets the gray under the   */
}              /* curly-q and gets the blue to   */
/* reflect off the curly-q nicely.*/

object { spirals translate <0.0, 14.75, 0.0> } /* The curly-q! */

#declare ball = /* Ball to top poles placed inside of curly-q */
sphere {
   <0, 14.75, 0>, 2
   texture {
      pigment { Firebrick }
      finish {
         reflection 0.95
         ambient 0.0
         diffuse 0.5
         specular 1.0
         roughness 0.00001
      }
   }
}

#declare pole = /* Pole to be placed inside of curly-q */
object {
   Disk_Y
   scale <0.5, 14.75, 0.5>

   texture {
      pigment { Firebrick }
      finish {
         reflection 0.95
         ambient 0.0
         diffuse 0.5
         specular 1.0
         roughness 0.00001
      }
   }
}
/* Construct the poles. */
object { pole translate <11, 0, 0> rotate <0,  18, 0> } /* Could have made this */
object { pole translate <11, 0, 0> rotate <0,  54, 0> } /* a composite, but     */
object { pole translate <11, 0, 0> rotate <0,  90, 0> } /* didn't see the point.*/
object { pole translate <11, 0, 0> rotate <0, 126, 0> }
object { pole translate <11, 0, 0> rotate <0, 162, 0> }
object { pole translate <11, 0, 0> rotate <0, 198, 0> }
object { pole translate <11, 0, 0> rotate <0, 234, 0> }
object { pole translate <11, 0, 0> rotate <0, 270, 0> }
object { pole translate <11, 0, 0> rotate <0, 306, 0> }
object { pole translate <11, 0, 0> rotate <0, 342, 0> }
/* Construct the balls. */
object { ball translate <11, 0, 0> rotate <0,  18, 0> }
object { ball translate <11, 0, 0> rotate <0,  54, 0> }
object { ball translate <11, 0, 0> rotate <0,  90, 0> }
object { ball translate <11, 0, 0> rotate <0, 126, 0> }
object { ball translate <11, 0, 0> rotate <0, 162, 0> }
object { ball translate <11, 0, 0> rotate <0, 198, 0> }
object { ball translate <11, 0, 0> rotate <0, 234, 0> }
object { ball translate <11, 0, 0> rotate <0, 270, 0> }
object { ball translate <11, 0, 0> rotate <0, 306, 0> }
object { ball translate <11, 0, 0> rotate <0, 342, 0> }

#declare Connector_1 = cylinder {
   <0, 0, 0>, <1, 0, 0>, 1

   texture {
      pigment { DimGray }
      finish {
         ambient 0.0
         diffuse 0.6
         specular 1.0
         roughness 0.00001
         reflection 0.25
      }
   }
}

#include "links.inc" /* Poles linking interior balls and poles */
object { links rotate <0, 18, 0> translate <0.0, 14.75, 0.0> }
