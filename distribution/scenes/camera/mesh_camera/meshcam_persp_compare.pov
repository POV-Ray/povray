// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

/* Persistence Of Vision Raytracer sample file.

   Test scene for the Mesh Camera macros
  
   Comparison of the meshcam pinhole cameras with a regular camera
   to test the correctness of the results
   
   --
   Jaime Vives Piqueres, Jan. 2011  <jaime@ignorancia.org> */

/**********************************************************************************************
 * $File: //depot/povray/smp/distribution/scenes/camera/mesh_camera/meshcam_persp_compare.pov $
 * $Revision: #1 $
 * $Change: 5377 $
 * $DateTime: 2011/01/09 19:56:00 $
 * $Author: jholsenback $
 *********************************************************************************************/
#version 3.7;

// control center:
#declare use_mesh_cam=1;  //  0=regular camera, 1=pinhole mesh, 2=pinhole mesh with UV 

// common globals for all the demos
#include "demo_globals.inc"

// standard includes
#include "colors.inc"
#include "textures.inc"

// common test subjects and scenario
#include "demo_common.inc"

// common camera parameters
#declare c_location=<4,2,-3>;   // location
#declare c_look_at=<0,1.1,1>;   // look at
#declare c_angle=54;            // angle

// test mesh camera
#if (use_mesh_cam)

  // mesh camera macros
  #include "meshcam_macros.inc"

  // create the pinhole mesh camera and
  // use it camera at the location, oriented to the look_at
  #if (use_mesh_cam=1)
    #declare camera_mesh=
    meshcam_pinhole(image_width, image_height, c_angle, "")
    camera{
      mesh_camera{ 1 0
        mesh{camera_mesh
          meshcam_placement(c_location,c_look_at)
        }
      }
      location <0,0,-.01>
    }
  #else
    #declare camera_mesh=
    meshcam_pinhole_uv(image_width*.1, image_height*.1, c_angle, "")
    camera{
      mesh_camera{ 1 3
        mesh{camera_mesh
          meshcam_placement(c_location,c_look_at)
        }
        smooth
      }
      location <0,0,-.01>
    } 
  #end

// test regular camera for comparison
#else

  camera{
    perspective   
    location c_location
    up 1*y right (image_width/image_height)*x
    angle c_angle
    look_at c_look_at
  }


#end

