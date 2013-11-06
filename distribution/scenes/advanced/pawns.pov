// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
// "Pawns", a study in wood... three pawns on a chessboard
// File by Douglas Otwell
//
// -w320 -h240
// -w800 -h600 +a0.3
#version 3.7;
global_settings { assumed_gamma 1.8 }

#include "colors.inc"
#include "shapes.inc"
//#include "textures.inc"

//
//   Yellow pine, close grained
//
#declare Yellow_Pine = texture {
   pigment {
      wood
      turbulence 0.02
      color_map {
         [0.000, 0.222  color red  0.808  green  0.671  blue  0.251  filter  0.000
                        color red  0.808  green  0.671  blue  0.251  filter  0.000]
         [0.222, 0.342  color red  0.808  green  0.671  blue  0.251  filter  0.000
                        color red  0.600  green  0.349  blue  0.043  filter  0.000]
         [0.342, 0.393  color red  0.600  green  0.349  blue  0.043  filter  0.000
                        color red  0.808  green  0.671  blue  0.251  filter  0.000]
         [0.393, 0.709  color red  0.808  green  0.671  blue  0.251  filter  0.000
                        color red  0.808  green  0.671  blue  0.251  filter  0.000]
         [0.709, 0.821  color red  0.808  green  0.671  blue  0.251  filter  0.000
                        color red  0.533  green  0.298  blue  0.027  filter  0.000]
         [0.821, 1      color red  0.533  green  0.298  blue  0.027  filter  0.000
                        color red  0.808  green  0.671  blue  0.251  filter  0.000]
      }
      scale 0.1
      translate 10*x
   }
}

// Yellow_Pine layer 2
texture {
   pigment {
      wood
      turbulence 0.01
      color_map {
         [0.000, 0.120   color red  1.000  green  1.000  blue  1.000  filter  1.000
                         color red  0.702  green  0.412  blue  0.118  filter  0.608]
         [0.120, 0.231   color red  0.702  green  0.412  blue  0.118  filter  0.608
                         color red  0.702  green  0.467  blue  0.118  filter  0.608]
         [0.231, 0.496   color red  0.702  green  0.467  blue  0.118  filter  0.608
                         color red  1.000  green  1.000  blue  1.000  filter  1.000]
         [0.496, 0.701   color red  1.000  green  1.000  blue  1.000  filter  1.000
                         color red  1.000  green  1.000  blue  1.000  filter  1.000]
         [0.701, 0.829   color red  1.000  green  1.000  blue  1.000  filter  1.000
                         color red  0.702  green  0.467  blue  0.118  filter  0.608]
         [0.829, 1       color red  0.702  green  0.467  blue  0.118  filter  0.608
                         color red  1.000  green  1.000  blue  1.000  filter  1.000]
      }
      scale 0.5
      translate 10*x
   }
}

//
//   Rosewood
//
#declare Rosewood = texture {
   pigment {
      bozo
      turbulence 0.04
      color_map {
         [0.000, 0.256   color red  0.204  green  0.110  blue  0.078  filter  0.000
                         color red  0.231  green  0.125  blue  0.090  filter  0.000]
         [0.256, 0.393   color red  0.231  green  0.125  blue  0.090  filter  0.000
                         color red  0.247  green  0.133  blue  0.090  filter  0.000]
         [0.393, 0.581   color red  0.247  green  0.133  blue  0.090  filter  0.000
                         color red  0.204  green  0.110  blue  0.075  filter  0.000]
         [0.581, 0.726   color red  0.204  green  0.110  blue  0.075  filter  0.000
                         color red  0.259  green  0.122  blue  0.102  filter  0.000]
         [0.726, 0.983   color red  0.259  green  0.122  blue  0.102  filter  0.000
                         color red  0.231  green  0.125  blue  0.086  filter  0.000]
         [0.983, 1       color red  0.231  green  0.125  blue  0.086  filter  0.000
                         color red  0.204  green  0.110  blue  0.078  filter  0.000]
      }
      scale <0.5, 0.5, 1>
      translate 10*x
   }
   finish {
      ambient 0.5
      diffuse 0.8
   }
}

// Rosewood layer 2
texture {
   pigment {
      wood
      turbulence 0.04
      color_map {
         [0.000, 0.139   color red  0.545  green  0.349  blue  0.247  filter  1.000
                         color red  0.000  green  0.000  blue  0.000  filter  0.004]
         [0.139, 0.148   color red  0.000  green  0.000  blue  0.000  filter  0.004
                         color red  0.000  green  0.000  blue  0.000  filter  0.004]
         [0.148, 0.287   color red  0.000  green  0.000  blue  0.000  filter  0.004
                         color red  0.545  green  0.349  blue  0.247  filter  1.000]
         [0.287, 0.443   color red  0.545  green  0.349  blue  0.247  filter  1.000
                         color red  0.545  green  0.349  blue  0.247  filter  1.000]
         [0.443, 0.626   color red  0.545  green  0.349  blue  0.247  filter  1.000
                         color red  0.000  green  0.000  blue  0.000  filter  0.004]
         [0.626, 0.635   color red  0.000  green  0.000  blue  0.000  filter  0.004
                         color red  0.000  green  0.000  blue  0.000  filter  0.004]
         [0.635, 0.843   color red  0.000  green  0.000  blue  0.000  filter  0.004
                         color red  0.545  green  0.349  blue  0.247  filter  1.000]
         [0.843, 1       color red  0.545  green  0.349  blue  0.247  filter  1.000
                         color red  0.545  green  0.349  blue  0.247  filter  1.000]
      }
      scale <0.5, 0.5, 1>
      translate <10, 0, 0>
   }
   finish {
      ambient 0.5
      diffuse 0.8
   }
}

//
//   Sandalwood ( makes a great burled maple, too)
//
#declare Sandalwood = texture {
   pigment {
      bozo
      turbulence 0.2
      color_map {
         [0.000, 0.171   color red  0.725  green  0.659  blue  0.455  filter  0.000
                         color red  0.682  green  0.549  blue  0.420  filter  0.000]
         [0.171, 0.274   color red  0.682  green  0.549  blue  0.420  filter  0.000
                         color red  0.557  green  0.451  blue  0.322  filter  0.000]
         [0.274, 0.393   color red  0.557  green  0.451  blue  0.322  filter  0.000
                         color red  0.725  green  0.659  blue  0.455  filter  0.000]
         [0.393, 0.564   color red  0.725  green  0.659  blue  0.455  filter  0.000
                         color red  0.682  green  0.549  blue  0.420  filter  0.000]
         [0.564, 0.701   color red  0.682  green  0.549  blue  0.420  filter  0.000
                         color red  0.482  green  0.392  blue  0.278  filter  0.000]
         [0.701, 1       color red  0.482  green  0.392  blue  0.278  filter  0.000
                         color red  0.725  green  0.659  blue  0.455  filter  0.000]
      }
      scale <0.2, 0.2, 1>
      scale 2
   }
}
// Sandalwood layer 2
texture {
   pigment {
      bozo
      turbulence 0.8
      color_map {
         [0.000, 0.087   color red  0.682  green  0.604  blue  0.380  filter  1.000
                         color red  0.761  green  0.694  blue  0.600  filter  0.020]
         [0.087, 0.226   color red  0.761  green  0.694  blue  0.600  filter  0.020
                         color red  0.635  green  0.553  blue  0.325  filter  1.000]
         [0.226, 0.348   color red  0.635  green  0.553  blue  0.325  filter  1.000
                         color red  0.761  green  0.694  blue  0.600  filter  0.020]
         [0.348, 0.496   color red  0.761  green  0.694  blue  0.600  filter  0.020
                         color red  0.682  green  0.604  blue  0.380  filter  1.000]
         [0.496, 0.565   color red  0.682  green  0.604  blue  0.380  filter  1.000
                         color red  0.761  green  0.694  blue  0.600  filter  0.020]
         [0.565, 0.661   color red  0.761  green  0.694  blue  0.600  filter  0.020
                         color red  0.682  green  0.604  blue  0.380  filter  1.000]
         [0.661, 0.835   color red  0.682  green  0.604  blue  0.380  filter  1.000
                         color red  0.761  green  0.694  blue  0.600  filter  0.020]
         [0.835, 1       color red  0.761  green  0.694  blue  0.600  filter  0.020
                         color red  0.682  green  0.604  blue  0.380  filter  1.000]
      }
      scale 0.2
      scale 2
   }
}

//
//   Lights, Camera ...
//
camera {
   location  <-8.0, 4.0, -14.0>
   angle 40 // direction <0.0, 0.0, 2.0>
   up        <0.0, 1.0, 0.0>
   right     x*image_width/image_height
   look_at   <-2.0, 0.0, -4.0>
}

light_source { <100.0, 400.0, -600.0> color White }

//   a back-light to create a highlight on the board
light_source { <12.0, 4.0, 12.0> color White }

//  We'll build our chessboard out of one big pine block and 32
// little rosewood ones
#declare Fours = union {
   object { UnitBox
      texture {
         Rosewood
         finish {
            phong 0.3
            ambient 0.5
            diffuse 0.7
            reflection 0.3
         }
      }
      translate <-1, -1, 1>
   }

   object { UnitBox
      texture {
         Rosewood
         finish {
            phong 0.3
            ambient 0.5
            diffuse 0.7
            reflection 0.3
         }
      }
      translate <1, -1, -1>
   }


}

#declare Eights = union {
   object { Fours
      translate <-2, 0, 2>
   }
   object { Fours
      translate <2, 0, 2>
   }
   object { Fours
      translate <-2, 0, -2>
   }
   object { Fours
      translate <2, 0, -2>
   }

 }

//   Add another wood texture around the edges
#declare rail = intersection {
   object { UnitBox
      scale <10, 0.25, 1>
   }

   plane { -x, 0
      rotate 45*y
      translate -9*x
   }

   plane { x, 0
      rotate -45*y
      translate 9*x
   }

   translate <0, -0.25, -9>

   texture {
      Sandalwood
      finish { phong 0.4 }
   }
}

//
//   Chessboard
//
#declare Chessboard = union {
   object { Eights
      translate <-4, 0, 4>
   }
   object { Eights
      translate <4, 0, 4>
   }
   object { Eights
      translate <-4, 0, -4>
   }
   object { Eights
      translate <4, 0, -4>
   }

   object { UnitBox
      scale <8, 0.25, 8>
      translate -0.25*y
   }

   object { rail rotate 0*y }
   object { rail rotate 90*y }
   object { rail rotate 180*y }
   object { rail rotate 270*y }

   texture {
      Yellow_Pine
      finish {
         phong 0.5
         reflection 0.3
      }
      scale 2
   }


}

//
//   Pawn
//
#declare pawn = union {
   difference {
      object { Disk_Y scale <8, 12.7468, 8> }
      quartic {
         < 1.0,  0.0,  0.0,   0.0,    2.0,  0.0,  0.0,  2.0,  0.0,-738.0,
         0.0,  0.0,  0.0,   0.0,    0.0,  0.0,  0.0,  0.0,  0.0,   0.0,
         1.0,  0.0,  0.0,   2.0,    0.0, 162.0,  0.0,  0.0,  0.0,   0.0,
         1.0,   0.0, -738.0,   0.0,   6561.0>
         sturm
      }
   }

   quartic {
      < 1.0,  0.0,  0.0,   0.0,    2.0,  0.0,  0.0,  2.0,  0.0, -132.5,
      0.0,  0.0,  0.0,   0.0,    0.0,  0.0,  0.0,  0.0,  0.0,   0.0,
      1.0,  0.0,  0.0,   2.0,    0.0, 123.5,  0.0,  0.0,  0.0,   0.0,
      1.0,  0.0, -132.5,  0.0,  3813.0625 >
      sturm
      translate -11.2468*y
   }

   quartic {
      < 1.0,  0.0,  0.0,   0.0,    2.0,  0.0,  0.0,  2.0,  0.0, -132.5,
      0.0,  0.0,  0.0,   0.0,    0.0,  0.0,  0.0,  0.0,  0.0,   0.0,
      1.0,  0.0,  0.0,   2.0,    0.0, 123.5,  0.0,  0.0,  0.0,   0.0,
      1.0,  0.0, -132.5,  0.0,  3813.0625>
      sturm
      translate 11.2468*y
   }

   //   Base
   intersection {
      object { Disk_Y
         scale <12, 3, 12>
         translate -15.7468*y
      }
      object { QCone_Y
         translate -2*y
      }
   }

   //   Ball on top
   sphere { <0, 17.7468, 0>, 7 }

   bounded_by { object { Disk_Y scale <14, 26, 14> } }

   translate 18.7468*y
   scale 0.06
}


//   Now let's put the pieces together

object { Chessboard }

//   Pawn 1
object { pawn
   texture {
      Yellow_Pine
      finish { phong 0.8 }
   }

   rotate 60*y
   translate <-5, 0, -7>
}

//   Pawn 2
object { pawn
   texture {
      Yellow_Pine
      finish { phong 0.8 }
   }

   rotate 30*y
   translate <1, 0, -1>
}

//   Pawn 3
object { pawn
   texture {
      Rosewood
      finish {
         phong 1.0
         ambient 0.5
         diffuse 0.7
      }
   }

   rotate 30*y
   translate <0.72, -0.24, 0>
   rotate 96.2052*z
   translate <1, 0, -5>
}

//   a background glow to add interest
sphere { <0, 0, 0>, 1000
   inverse
   hollow on

   pigment {
      gradient y
      color_map {
         [0.0  0.25  color Goldenrod color Goldenrod]
         [0.25 0.46  color Goldenrod color Black]
         [0.46 1.001 color Black color Black]
      }
      scale 2000
      translate -1000*y
   }
}
