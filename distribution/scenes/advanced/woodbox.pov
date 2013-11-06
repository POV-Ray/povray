// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// woodbox.pov
// POV-Ray scene file by Dan Farmer
// Copyright POV-Team, 1995
// Demonstrates some of the woods.inc textures.
// Several wooden spheres (and one glass one), in and around
// a wooden box.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings { assumed_gamma 1.2 } 

#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"
#include "metals.inc"
#include "glass.inc"
#include "woods.inc"

camera {
   location <-5, 10, -15>
   angle 45 // direction <0, 0,  1.7>
   right x*image_width/image_height
   look_at <0,0,0>
}

// Uncomment the area lights only if you've got lots of time.
#declare Dist=80.0;
light_source {< -50, 25, -50> color White
     fade_distance Dist fade_power 2
//   area_light <-40, 0, -40>, <40, 0, 40>, 3, 3
//   adaptive 1
//   jitter
}
light_source {< 50, 10,  -4> color Gray30
     fade_distance Dist fade_power 2
//   area_light <-20, 0, -20>, <20, 0, 20>, 3, 3
//   adaptive 1
//   jitter
}
light_source {< 0, 100,  0> color Gray30
     fade_distance Dist fade_power 2
//   area_light <-30, 0, -30>, <30, 0, 30>, 3, 3
//   adaptive 1
//   jitter
}

sky_sphere {
    pigment {
        gradient y
        color_map {
            [0, 1  color Gray50 color Gray80]
        }
    }
}

#declare M_Wood18B =
colour_map {
    [0.00 0.25   color rgbf < 0.50, 0.26, 0.12, 0.10>
                 color rgbf < 0.54, 0.29, 0.13, 0.20>]
    [0.25 0.40   color rgbf < 0.54, 0.29, 0.13, 0.20>
                 color rgbf < 0.55, 0.28, 0.10, 0.70>]
    [0.40 0.50   color rgbf < 0.55, 0.28, 0.10, 0.70>
                 color rgbf < 0.50, 0.23, 0.15, 0.95>]
    [0.50 0.70   color rgbf < 0.50, 0.23, 0.15, 0.95>
                 color rgbf < 0.56, 0.29, 0.17, 0.70>]
    [0.70 0.98   color rgbf < 0.56, 0.29, 0.17, 0.70>
                 color rgbf < 0.54, 0.29, 0.13, 0.20>]
    [0.98 1.00   color rgbf < 0.54, 0.29, 0.13, 0.20>
                 color rgbf < 0.50, 0.26, 0.12, 0.10>]
}


#declare Floor_Texture =
    texture { pigment { P_WoodGrain18A color_map { M_Wood18A }}}
    texture { pigment { P_WoodGrain12A color_map { M_Wood18B }}}
    texture {
        pigment { P_WoodGrain12B color_map { M_Wood18B }}
        finish { reflection 0.25 }
    }

#declare Floor =
plane { y,0
    texture { Floor_Texture
        scale 0.5
        rotate y*90
        rotate <10, 0, 15>
        translate z*4
    }
}

#declare T0 = texture { T_Wood19 }

#declare T =
texture { T0
    finish { specular 0.50 roughness 0.1 ambient 0.25 }
}

#declare T1 = texture { T translate  x*10 rotate <0, 87, 0> }
#declare T2 = texture { T translate  y*10 rotate <0,  1, 0> }
#declare T3 = texture { T translate -x*10 rotate  <0, 88, 0> translate z*100}

#declare Front_Panel =
box      { <-3.75, 0.00, -0.5>,
           < 3.75, 1.75,  0.0> }

#declare Front_Top_Edge   =
cylinder { <-3.75, 1.75,  0.0>,
           < 3.75, 1.75,  0.0>, 0.5 }

#declare Front_Right_Edge =
cylinder { < 3.75, 0.00,  0.0>,
           < 3.75, 1.75,  0.0>, 0.5 }

#declare Front_Left_Edge  =
cylinder { <-3.75, 0.00,  0.0>,
           <-3.75, 1.75,  0.0>, 0.5 }

#declare Front_UL_Corner  = sphere   { <-3.75, 1.75,  0.0>, 0.5 }
#declare Front_UR_Corner  = sphere   { < 3.75, 1.75,  0.0>, 0.5 }


#declare Left_Panel      = box { <-0.50, 0, -2.75>, <0.50, 1.75, 2.75> }
#declare Left_Top_Edge   = cylinder { <0, 1.75, -2.75>, <0, 1.75, 2.75>, 0.5 }

#declare Left_End =
intersection {
    union {
        object { Left_Panel       }
        object { Left_Top_Edge    }
    }
    plane { x, 0 }
    texture { T2 }
    bounded_by { box { <-0.501, 0.01, -3.251>, <0.01, 2.251, 3.251> } }
}

#declare Box_Front =
intersection {
    union {
        object { Front_Panel      }
        object { Front_Top_Edge   }
        object { Front_Left_Edge  }
        object { Front_Right_Edge }
        object { Front_UL_Corner  }
        object { Front_UR_Corner  }
    }
    plane { z, 0 }
    texture { T1}
    bounded_by { box { <-4.251, 0.01, -0.51>, <4.251, 2.251,  0.01> }}
}
#declare Box_Bottom = box {<-3.75, 0.0, -2.75> <3.75, 0.25, 2.75> texture {T3} }
#declare Box_Lid =    box {<-3.75, 0.0, -2.75> <3.75, 0.25, 2.75>
    translate -2.75*z    // put "hinge" at origin
    rotate x*25          // open the lid
    translate 2.75*z     // move "hinge" back
    translate y*2        // lift to top
    texture {T3}
}
#declare Box =
union {
    object { Box_Front translate -z*2.75}
    object { Box_Front scale <1,1,-1> translate z*2.75}
    object { Left_End translate -x*3.75 }
    object { Left_End scale <-1,1,1> translate x*3.75 }
    object { Box_Lid  }
    object { Box_Bottom }
}
#declare Spheres =
union {

    // Inside of box
    sphere { <1.5, 1.5, -0.75>, 1.25
        texture {
            T_Wood14
            finish { specular 0.35 roughness 0.05 ambient 0.3 }
            translate x*1
            rotate <15, 10, 0>
            translate y*2
        }
    }
    // Inside of box
    sphere { <-1.5, 1.25,  0.5>, 1
        texture { T_Wood18
            finish { specular 0.25 roughness 0.025 ambient 0.35 }
            scale 0.33
            translate x*1
            rotate <10, 20, 30>
            translate y*10
        }
    }
    // Inside of box
    sphere { <-0.75, 1.0, -1.5>, 0.75
        texture { T_Wood10
            finish { specular 0.5 roughness 0.005 ambient 0.35 }
            translate x*1
            rotate <30, 10, 20>
        }
    }

    // Outside of box
    sphere { <-0.75, 0.75, -4.25>, 0.75
        texture { T_Wood4
            finish { specular 0.25 roughness 0.015 ambient 0.2 }
        }
    }
    // Outside of box
    sphere { <-2.25, 0.45, -4.5>, 0.45
        texture { T_Wood20
            finish { specular 0.15 roughness 0.15 ambient 0.3 }
            rotate <45, 10, 45>
            translate x*10
        }
    }

    // Outside of box
    sphere { <-5.5, 0.95, 0.8>, 0.95
      // texture { T_Glass4 } interior {I_Glass caustics 1}
      // converted to material 26Sep2008 (jh)
      material {
        texture {
          pigment { color rgbf <0.98, 1.0, 0.99, 0.75> }
          finish { F_Glass4 }
          }
        interior {I_Glass caustics 1}
        }
      }
    // Outside of box
    sphere { <-5.00, 0.75, -2.0>, 0.75 texture { T_Copper_2B} }
    // Outside of box
    sphere { <-1.75, 0.40, -5.4>, 0.40 texture { T_Brass_3B} }
}
union {
    object { Floor }
    object { Box }
    object { Spheres }
    rotate -y*35
}

