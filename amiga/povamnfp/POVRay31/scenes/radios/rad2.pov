// Persistence Of Vision raytracer version 3.1 sample file.
// File by Dan Farmer
// Radiosity demonstration


global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"
#include "glass.inc"
#include "metals.inc"
#include "consts.inc"
#include "rad2.inc"

// Constants used by 'rad_def.inc' are set in 'const.inc'
//#declare Rad_Quality = Radiosity_Debug
//#declare Rad_Quality = Radiosity_Fast
//#declare Rad_Quality = Radiosity_Normal
//#declare Rad_Quality = Radiosity_2Bounce
//#declare Rad_Quality = Radiosity_Final

//#include "rad_def.inc"

#declare Area_Lights = on
#if(Area_Lights)
    #debug "\nArea lights are ON"
#else
    #debug "\nArea lights are OFF"
#end

background { White }
camera {  //  Camera Camera01
  location  <-1.500, -29.900, 2.000>
  direction <0.0,     0.0,  1.75>
  sky       <0.0,     0.0,  1.0>  // Use right handed-system!
  up        <0.0,     0.0,  1.0>  // Where Z is up
  right     <1.3333,  0.0,  0.0>
  look_at   <0.50,  0.000, -1.0>
}

#declare Dist=15;
#declare L = 0.7;

light_source {   // Light001
  <0.000, 0.000, 7.500>
  color rgb L
  #if(Area_Lights)
     #debug "\nDing! Area light #1 turned on."
     area_light <-6, -6, 0>, <6, 6, 0>, 5, 5
     adaptive 1
     jitter
  #end
  fade_distance Dist fade_power 2
}

light_source {   // Light002
  <4.000, 15.000, 7.500>
  color rgb L
  #if(Area_Lights)
     #debug "\nBing! Area light #2 turned on.\n"
     area_light <-1.5, -1.5, 0>, <1.5, 1.5, 0>, 5, 5
     adaptive 1
     jitter
  #end
  fade_distance Dist fade_power 2
}


// ********  O B J E C T S *******
#declare A_Ball =
object {   // Sphere001
  sphere {<0,0,0>,1}
  texture {
    Ball_Texture
  }
  scale <3.824621, 3.824621, 3.824621>
  translate <-5.000000, 20.000000, -3.000000>
}


// Extra objects to place inside
#declare A_Cone =
cone {   // Cone001
  <0,0,0>, 0.00000
  <0,0,1>, 1.00000
  texture {
    Cone_Texture
  }
  interior{ior 1.55}
  scale <2.000000, 2.000000, 12.000000>
  rotate <-180.000000, 0.000000, 0.000000>
  translate <-4.603342, 11.005570, -2>
}

#declare A_Super_El =
superellipsoid { <.3, .3>
  texture {
    Box_Texture
  }
  scale <2.000000, 2.000000, 4.000000>
  rotate z*45
  translate <6.000000, 10.000000, -6.000000>
}

#declare A_Torus =
torus {   // Torus001
  2.000, 1.000  // Major, minor radius
  rotate -x*90
  texture {
    Torus_Texture
  }
  rotate x*35
  translate <0,0, 1>
  translate <0, 10, -8>
}

#declare Room =
union {
    box {   // LeftWall
      <-1, -1, -1>, <1, 1, 1>
      texture {
        LeftWallTex
      }
      scale <0.010000, 30.000999, 10.001000>
      translate <-10.000000, 0.000000, 0.000000>
    }
    box {   // RightWall
      <-1, -1, -1>, <1, 1, 1>
      texture {
        RightWallTex
      }
      scale <0.010000, 30.000999, 10.001000>
      translate <10.000000, 0.000000, 0.000000>
    }
    box {   // Floor
      <-1, -1, -1>, <1, 1, 1>
      scale <20.010000, 30.000999,0.00001>
      translate <10.000000, 0.000000, -9.999999>
      texture {
        pigment { checker color ForestGreen color SteelBlue
          scale < 5 5 1 >
        }
        finish { reflection 0.15 }
      }
    }
    box {   // Room
      <-1, -1, -1>, <1, 1, 1>
      texture {
        Room_Texture
      }
      scale <10.000000, 30.000000, 10.000000>
    }
}

object { Room  hollow on}
object { A_Ball }
// object { A_Cone }
object { A_Super_El }
// object { A_Torus }
