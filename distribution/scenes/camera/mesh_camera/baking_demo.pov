// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

/* Persistence Of Vision Raytracer sample file.


   Demo for the new mesh_camera:
  
   Baking meshes to textures and using these back on the same scene  
  
   --
   Jaime Vives Piqueres, Jan. 2011  <jaime@ignorancia.org> */

/************************************************************************************
 * $File: //depot/povray/smp/distribution/scenes/camera/mesh_camera/baking_demo.pov $
 * $Revision: #4 $
 * $Change: 5378 $
 * $DateTime: 2011/01/09 21:25:00 $
 * $Author: jholsenback $
 ***********************************************************************************/
#version 3.7;

// standard includes
#include "colors.inc"
#include "textures.inc"

// control center
#ifndef(use_baking) // do not set it if  it is already set on the command line by the bake.sh script
#declare use_baking=0;  // 0=off, 1=save, 2=load
#end

// use radiosity only when baking or when rendering normally
#if (use_baking<2)
global_settings{
 assumed_gamma 1.0
 radiosity{
  count 80
  error_bound .5
  nearest_count 10
  recursion_limit 1
 }
}
#default{texture{finish{ambient 0 diffuse 1}}}
#else
// ...otherwise, just use emission on all the finishes
#default{texture{finish{ambient 0 emission 1 diffuse 0}}}
#end

// common test subject and scenario for all the demos
#include "demo_common.inc"

// bake textures
#if (use_baking=1)

// to bake all the textures, just use the included script bake.sh (bake.bat still missing, volunteers welcome).
// for individual baking, render with +Kn, where n is the number of the mesh on the switch below, 
// also +O with the corresponding texture name, and +FP for PNG output
camera{
  mesh_camera{ 1 3 // distribution 3 is what we want here
  #switch (clock)
    #case (1)
      mesh{vase1 
          scale scl_vase1
          translate pos_vase1
      }
      #break
    #case (2)
      mesh{vase2 
          scale scl_vase2
          translate pos_vase2
      }
      #break
    #case (3)
      mesh{room 
          scale scl_room
          translate pos_room
      }
      #break
  #end
  }
  // usually the normals on the mesh point outside, so we reverse them to look at the faces
  location <0,0,.01>
  direction <0,0,-1>
}

// render with a regular camera (when rendering directly the scene, or the baked version)
#else

  #declare c_location=<4,2,-3>;   // location
  #declare c_look_at=<0,1.1,1>;  // look at
  #declare c_angle=54;         // angle
  camera{
    perspective
    location c_location
    up 1*y right (image_width/image_height)*x
    angle c_angle
    look_at c_look_at
    // test focal blur with the baked render... it's very fast!
    //aperture 1/3 blur_samples 49 focal_point <1.1,1.1,1> variance 0
  }

#end





