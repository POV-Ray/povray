// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// File by Dan Farmer
// Wine glass and chessboard
// Updated October, 1996
// Updated January, 1998  DCB
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings { assumed_gamma 1.0  max_trace_level 10 } 

global_settings {
  assumed_gamma 1.0
  number_of_waves 3
  max_trace_level 5
  }

#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"
#include "metals.inc"

camera {
   location <50.0, 55.0, -75.0>
   angle 65 //  direction z
   up y
   right x*image_width/image_height
   look_at <-10.0, 18.0, 0.0>
}

light_source { <10.0, 50.0, 35.0> colour White }
light_source { <-35.0, 30.0, -150.0> colour White }


#declare I_Glass1 =                  // Use with Liquid
   interior{
      ior 1.45
      caustics 2
      fade_distance 40                   // only for this scene
      fade_power 1
   }


#declare I_Glass2 =                    //Use with Bead
    interior{
       fade_distance 1.5              // only for this scene
       fade_power 1
       ior 1.45
       caustics 2
    }


#declare I_Glass3 =                  //Use with Rim
   interior{
      ior 1.51
      caustics 2
      fade_distance 0.025
      fade_power 1
   }

#declare I_Glass4 =                  //Use with Top & Stem
   interior{
      ior 1.51
      caustics 2
      fade_distance 1.5
      fade_power 1
   }

#declare I_Glass5 =                 // Use with Base
   interior{
       ior 1.51
       caustics 2
       fade_distance 3.25
       fade_power 1
     }


#declare T_Glass = texture {
   pigment { color red 1.0 green 1.0 blue 1.0 filter 0.95 }
   finish {
      ambient 0.0
      diffuse 0.0
      reflection 0.1
      phong 0.3
      phong_size 90
   }
}

#declare Wine = color red 1.0 filter 0.85;

#declare Liquid = finish { reflection 0.05 }

#declare Bead = object {
   sphere { <0, 0, 0>, 1 scale <1.65, 0.65, 1.65> }
   texture { T_Glass}
   interior {I_Glass2}

}

#declare Rim =
   torus {0.975, 0.025
   clipped_by { plane { -y, 0.0 } }
   scale <15.0, 10.0, 15.0>
   translate 24.0*y
   texture {T_Glass}
   interior{I_Glass3}
}

#declare Top = intersection {
   plane { y, 1.0  }
   object { QCone_Y }
   object { QCone_Y scale <0.97, 1.0, 0.97> inverse }

   clipped_by { plane { y, 0.0  inverse } }
   scale <15.0, 10.0, 15.0>
   translate 14.0*y
   texture { T_Glass}
   interior {I_Glass4}
}

#declare Stem =
cylinder { y*3.25, y*14, 1
  texture { T_Glass}
  interior {I_Glass4}
}


#declare Base = object {
   QCone_Y
   clipped_by {
      plane { y, 0.0  }
      plane { y, -1.0 inverse }
   }
   translate 1.0*y         /* This actually puts the base of the cone on y=0*/
   scale <12.0, 3.25, 12.0>
   texture { T_Glass}
   interior { I_Glass5}
}

#declare Wine2 =
union {
   cone { 0, 0, y, 0.95 open
      texture {
         finish { Liquid }
         pigment { Wine }
      }
   }
   disc { y, y, 0.95
      texture {
         finish { Liquid
            specular 1
            roughness 0.01
         }
         pigment { Wine }
         normal { onion 0.75 frequency 3 sine_wave turbulence 0.15 }
      }
      interior { I_Glass1}

   }
   scale <14.9, 9.5, 14.9>
   translate 14.0*y
   texture {
      finish { Liquid }
      pigment { Wine }
   }
   interior { I_Glass1}
}


#declare Frame =
union {
   // corners
   sphere { <-120, 0, -120>, 4.65 }
   sphere { < 120, 0, -120>, 4.65 }
   sphere { <-120, 0,  120>, 4.65 }
   sphere { < 120, 0,  120>, 4.65 }

   object {
      Disk_X    /* Front rounded edge */
      scale <120, 4.65, 4.65>
      translate -z*120
   }
   object {
      Disk_Z    /* Left rounded edge */
      scale <4.65, 4.65, 120>
      translate -x*120
   }
   object {
      Disk_X    /* Rear rounded edge */
      scale <120, 4.65, 4.65>
      translate  z*120
   }
   object {
      Disk_Z    /* Right rounded edge */
      scale <4.65, 4.65, 120>
      translate x*120
   }
   texture { Silver_Texture }
}

#declare ChessBoard = object {
   Cube
   scale <120.0, 4.0, 120.0>

   texture {
      tiles {
         texture {
           pigment {
               marble
               turbulence 1
               lambda 2.1
               omega 0.707
               scale <2, 1, 2>
               color_map {
                   [0.00 rgb 0.975 ]
                   [0.55 rgb <0.2, 0.25, 0.3>]
                   [0.75 rgb 0.2 ]
                   [0.85 rgb 0.5 ]
                   [1.00 rgb 0.975 ]
              }
           }
           finish { diffuse 1 }
         }
      tile2
         texture {
            pigment { Gray10 }
            finish { diffuse 1 specular 0.5 roughness 0.025}
         }
      }
   scale <30.0, 4.001, 30.0>
   }
}


#declare WineGlass_Without_Wine = merge {
   object { Rim   }
   object { Top   }
   object { Bead  translate 14.5*y }
   object { Bead  translate 10.0*y }
   object { Bead  translate  7.0*y }
   object { Bead  translate  3.0*y }
   object { Stem  }
   object { Base  }
   scale <1.0, 1.50, 1.0>
}
#declare WineGlass_With_Wine = union {
   object { WineGlass_Without_Wine }
   object { Wine2
      scale <1.0, 1.50, 1.0>
   }
}


fog { distance 200 color Black }

/* Ground plane */
plane {
   y, 0.0
   texture {
      pigment { RichBlue }
      normal {
         quilted 0.45
         control0 1 control1 1
         scale <0.45, 1, 0.45>
      }
   }
}

union {
    object { Frame }
    object { ChessBoard }
    translate <0.0, 4.0, 145.0>
}

// How did I figure out those wierd transformations below?
// Well, they'd be less obscure had I created the wine glass properly,
// but since I didn't, I simply put the camera a tiny bit above the
// floorplane and tried various values until it "fit".

object { WineGlass_Without_Wine translate -x*12.15 rotate -z*86.5 }

object { WineGlass_With_Wine translate <0, 0, -20>}

