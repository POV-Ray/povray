// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Ray Tracer Scene Description File
// File: landscape.pov
// Desc: Use of 'trace' for placing objects on isosurfaces/heightfields
//       Furthermore showing the following techniques:
//          - use of piment functions for creatig isosurface terrain
//          - use function image type for heightfields
//          - slope pattern
//          - random variation of objects using rand() and macros
//          - nested #while loops for placing objects in a grid
//          - creation of simple trees using partly transparent textures
//
// Date: June 2001
// Auth: Christoph Hormann
//  Updated: 2013/02/15 for 3.7
//
// -w320 -h240
// -w512 -h384 +a0.3
// ------------------------------------------

#version 3.7;
global_settings { assumed_gamma 1.0 } 

#declare Test_Render=false;   // use simplified trees if true
#declare use_iso=false;       // use an isosurface object (HF with function image type otherwise)
#declare Viewpoint=1;

global_settings {
  assumed_gamma 1.0
  max_trace_level 25
  noise_generator 2  
}
//-------------------------------------------------------------------------
// This scene uses a non-standard camera set-up. 
// (See CAMERA in the included documentation for details.) 
// If you are new to POV-Ray, you might want to try a different demo scene.
//-------------------------------------------------------------------------
                
               
#if (Viewpoint=1)
  camera {
    //location    <3.7, -0.55, 0.3>    
    location    <3.0, 2.3, 0.3>
    direction   y
    sky         z
    up          z
    right       x*image_width/image_height // keep propotions with any aspect ratio
    look_at     <0.0, 0.0, 0.1>
    angle       32
  }
#else
  camera {
    location    <7, 14, 7>
    direction   y
    sky         z
    up          z
    right       x*image_width/image_height // keep propotions with any aspect ratio
    look_at     <0.0, 0.0, 0.0>
    angle       32
  }
#end

light_source {               
  <1.0, 4.0, 1.3>*100000
  color rgb <1.5, 1.4, 1.1>
}

// -------- Sky -------------
sphere {
  0,1
  texture {
    pigment {
      gradient z
      color_map {
        [0.03 color rgb <0.55, 0.65, 1.0> ]
        [0.12 color rgb <0.30, 0.40, 1.0> ]
      }
    }
    finish { ambient 1 diffuse 0 }
  }
  scale 1000
  no_shadow
  hollow on
}

// -------- random seeds for the Trees -------------
#declare Seed=seed(2);
#declare Seed2=seed(1);


// -------- Tree textures -------------
#if (Test_Render)
  // simple textures for test renders
  #declare T_Wood=
  texture {
    pigment { color rgb <1, 0, 0> }
  }

  #declare T_Tree=
  texture {
    pigment { color rgb <1, 0, 0> }
  }

#else
  #declare T_Wood=
  texture {
    pigment { color rgb <0.4, 0.2, 0.05> }
    finish {
      specular 0.3
      diffuse 0.5
    }
    normal {
      bozo 0.6
      scale <0.1, 0.1, 0.5>
    }
  }

  #declare T_Tree=
  texture {
    pigment {
      agate
      color_map {
        [0.77 color rgbt 1]
        [0.77 color rgb <0.2, 0.5, 0.10> ]
        [0.85 color rgb <0.4, 0.6, 0.15> ]
        [0.97 color rgb <0.4, 0.6, 0.15> ]
        [0.99 color rgb <0.4, 0.2, 0.05> ]
      }
      scale 0.5
      warp { turbulence 0.4 }
    }
    finish {
      diffuse 0.5
      brilliance 1.5
      ambient 0.07
    }
    normal {
      wrinkles 0.6
      scale 0.5
    }
  }
#end

// -------- Tree macro -------------
#macro TreeA (Size)
  union {
    cylinder { 0, Size*z, Size*0.04 }       // Trunk
    union {                                 // Roots
      cylinder { 0, -Size*0.30*z, Size*0.025 rotate (40+rand(Seed)*20)*x rotate rand(Seed2)*360*z }
      cylinder { 0, -Size*0.25*z, Size*0.020 rotate (40+rand(Seed)*20)*x rotate rand(Seed2)*360*z }
      cylinder { 0, -Size*0.27*z, Size*0.022 rotate (40+rand(Seed)*20)*x rotate rand(Seed2)*360*z }
    }

    union {                                 // Branches
      cylinder {
        0, Size*0.35*z, Size*0.025
        rotate (40+rand(Seed)*35)*x
        rotate rand(Seed2)*360*z
        translate Size*(0.7+0.3*rand(Seed))*z
      }
      cylinder {
        0, Size*0.40*z, Size*0.026
        rotate (40+rand(Seed)*35)*x
        rotate rand(Seed2)*360*z
        translate Size*(0.7+0.3*rand(Seed))*z
      }
      cylinder {
        0, Size*0.27*z, Size*0.022
        rotate (40+rand(Seed)*35)*x
        rotate rand(Seed2)*360*z
        translate Size*(0.7+0.3*rand(Seed))*z
      }
    }

    #if (Test_Render)                       // Foliage
      sphere {
        Size*z, Size*(0.4+rand(Seed)*0.15)
        scale <rand(Seed)*0.5+0.5, rand(Seed)*0.5+0.5, 1>
        texture { T_Tree scale Size translate rand(Seed)*6 }
      }
    #else
      union {
        sphere {
          Size*z, Size*(0.4+rand(Seed)*0.15)
          scale <rand(Seed)*0.5+0.5, rand(Seed)*0.5+0.5, 1>
          texture { T_Tree scale Size translate rand(Seed)*6 }
        }
        sphere {
          Size*z, Size*(0.3+rand(Seed)*0.15)
          scale <rand(Seed)*0.5+0.5, rand(Seed)*0.5+0.5, 1>
          texture { T_Tree scale Size translate rand(Seed)*6 }
        }
        sphere {
          Size*z, Size*(0.2+rand(Seed)*0.15)
          scale <rand(Seed)*0.5+0.5, rand(Seed)*0.5+0.5, 1>
          texture { T_Tree scale Size translate rand(Seed)*6 }
        }
        sphere {
          Size*z, Size*(0.3+rand(Seed)*0.15)
          scale <rand(Seed)*0.5+0.5, rand(Seed)*0.5+0.5, 1>
          texture { T_Tree scale Size translate rand(Seed)*6 }
        }
        sphere {
          Size*z, Size*(0.2+rand(Seed)*0.15)
          scale <rand(Seed)*0.5+0.5, rand(Seed)*0.5+0.5, 1>
          texture { T_Tree scale Size translate rand(Seed)*6 }
        }
        sphere {
          Size*z, Size*(0.3+rand(Seed)*0.15)
          scale <rand(Seed)*0.5+0.5, rand(Seed)*0.5+0.5, 1>
          texture { T_Tree scale Size translate rand(Seed)*6 }
        }
      }
    #end

    texture { T_Wood scale Size }
  }
#end


// -------- Terrain textures -------------
#declare T_Sand=
texture {
  pigment { color rgb <1.1, 0.7, 0.3> }
  finish { 
    specular 0.06 
    ambient <0.8, 0.9, 1.4>*0.1
  }
  normal {
    granite 0.3
    scale 0.1
  }
}

#declare T_Grass=
texture {
  pigment { color rgb <0.5, 1.15, 0.3> }
  finish {
    specular 0.1
    diffuse 0.3
    brilliance 1.6
    ambient <0.8, 0.9, 1.4>*0.03
  }
  normal {
    granite 0.5
    accuracy 0.01
    scale 0.12
  }
}

#declare T_Rock=
texture {
  pigment {
    agate
    color_map {
      [0.2 color rgb <0.55, 0.50, 0.50> ]
      [0.6 color rgb <0.75, 0.50, 0.60> ]
      [1.0 color rgb <0.70, 0.60, 0.60> ]
    }
    scale 0.2
    warp { turbulence 0.5 }
  }
  finish {
    specular 0.2
    diffuse 0.4
    ambient <0.8, 0.9, 1.4>*0.06
  }
  normal {
    granite 0.6
    scale 0.1
  }
}

#declare T_Terrain=
texture {
  slope { -z*0.8 altitude z }
  texture_map {
    [0.05 T_Grass ]
    [0.09 T_Sand  ]
    [0.19 T_Sand  ]
    [0.23 T_Rock  ]
  }
  translate -0.05*z
}

#if (use_iso)
  // -------- Use an isosurface for the terrain -------------

  #declare fnPig=   // Terrain shape function
  function{
    pigment {
      agate
      color_map {
        [0.0 color rgb 0.0]
        [0.3 color rgb 0.2]
        [0.7 color rgb 0.8]
        [1.0 color rgb 1.0]
      }
      warp { turbulence 0.01 }
      scale 5 
      translate <1.8, -6.7, 0>
    }
  }

  #declare Terrain_Obj=
  isosurface {

    function { z-fnPig(x, -y, 0).gray*0.4 }

    max_gradient 2.8
    //evaluate 1, 10, 0.99    // for evaluating max_gradient
    accuracy 0.02

    contained_by { box { <-3, -3, -0.1>, <3, 3, 0.41> } }

  }

#else
  // -------- Use a heightfield for the terrain -------------

  #declare Terrain_Obj=
  height_field {
    function 500,500 {
      pigment {
        agate
        color_map {
          [0.0 color rgb 0.0]
          [0.3 color rgb 0.2]
          [0.7 color rgb 0.8]
          [1.0 color rgb 1.0]
        }
        warp { turbulence 0.01 }
        scale 5 
        translate <1.8, -6.7, 0>
     
        translate <3, 3, 0>
        scale 1/6
        
        translate -0.5*y    
        scale <1, -1, 1>
        translate 0.5*y
      } 
    }

    rotate -90*x
    scale <6, 6, -0.4>
    translate <-3, -3, 0>
  }

#end

object {
  Terrain_Obj
  texture { T_Terrain }
}


// -------- Placing the trees -------------

#declare Spacing=0.24;
#declare Cnt=0;


#declare PosX=-3;

#while (PosX < 3)

  #declare PosY=-3;

  #while (PosY < 3)

    // trace function
    #declare Norm = <0, 0, 0>;
    #declare Start = <PosX+(rand(Seed)-0.5)*Spacing, PosY+(rand(Seed)-0.5)*Spacing, 1.0>;
    #declare Pos = trace (
                  Terrain_Obj,     // object to test
                  Start,           // starting point
                  -z,              // direction
                  Norm );          // normal


    #if (Norm.x != 0 | Norm.y != 0 | Norm.z != 0)   // if intersection is found, normal differs from 0
      #if ((vdot(Norm, z)>0.85) & (Pos.z < 0.25))
      // criteria for placing trees: not too steep and not too high
        object {
          TreeA (0.05+rand(Seed)*0.02)
          translate Pos
        }
        #declare Cnt=Cnt+1;
      #end
    #end

    #declare PosY=PosY+Spacing;

  #end

  #declare PosX=PosX+Spacing;
#end

#debug concat("Placed ", str(Cnt,0,0), " Trees\n")

