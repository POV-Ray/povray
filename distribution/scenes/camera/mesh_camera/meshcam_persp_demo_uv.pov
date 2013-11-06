// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

/* Persistence Of Vision Raytracer sample file.

   Demo for the Mesh Camera macros for use with the new mesh_camera:
  
   Simple demo of the UV version of the pinhole camera, for use with the smooth
   modifier of the distribution method #3. It gives the same results, but it is 
   independant of the resolution: it allows for much smaller mesh camera sizes, 
   and works with standard AA too. 
  
   --
   Jaime Vives Piqueres, Jan. 2011  <jaime@ignorancia.org> */

/**********************************************************************************************
 * $File: //depot/povray/smp/distribution/scenes/camera/mesh_camera/meshcam_persp_demo_uv.pov $
 * $Revision: #1 $
 * $Change: 5377 $
 * $DateTime: 2011/01/09 19:56:00 $
 * $Author: jholsenback $
 *********************************************************************************************/
#version 3.7;

// control center
#declare use_distortion=0.1; // 0=pinhole mesh, !0=lens mesh, controls distortion amount (>0=barrel, <=pincushion)

// common globals for all the demos
#include "demo_globals.inc"

// standard includes
#include "colors.inc"
#include "textures.inc"

// common test subjects and scenario
#include "demo_common.inc"

// mesh camera macros being demonstrated
#include "meshcam_macros.inc"

// camera parameters
#declare c_location=<4,2,-3>;   // location
#declare c_look_at=<0,1.1,1>;  // look at
#declare c_angle=54;         // angle

// create the camera with a small and fixed number of rows, columns depend on the aspect ratio
#if (use_distortion)
  #declare camera_mesh=
  meshcam_lens_uv((image_width/image_height)*30,30, c_angle, use_distortion, "")
#else
  #declare camera_mesh=
  meshcam_pinhole_uv((image_width/image_height)*30,30, c_angle, "")
#end

// create the camera with the generated mesh, using distrib.#3 and smooth
camera{
  mesh_camera{ 1 3 
    mesh{camera_mesh 
      meshcam_placement(c_location,c_look_at)
    }
    smooth
  }
  location <0,0,-.01> 
}
