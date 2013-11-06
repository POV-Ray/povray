// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

/* Persistence Of Vision Raytracer sample file.

   Demo for the Mesh Camera macros for use with the new mesh_camera:
  
   Shows how to use the macros to create a pure orthographic mesh camera

   --
   Jaime Vives Piqueres, Jan. 2011  <jaime@ignorancia.org> */

/*******************************************************************************************
 * $File: //depot/povray/smp/distribution/scenes/camera/mesh_camera/meshcam_ortho_demo.pov $
 * $Revision: #1 $
 * $Change: 5377 $
 * $DateTime: 2011/01/09 19:56:00 $
 * $Author: jholsenback $
 ******************************************************************************************/
#version 3.7;

// control center
#declare meshcam_file=1;      // 0=off, 1=save mesh to file, 2=load mesh from file

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
#declare c_location=<0,3,-8>;   // location
#declare c_look_at=<0,1.1,1>;  // look at
#declare c_up_length=4;             // up_length (controls viewing area as in pure orthographic camera)

// create a mesh simulating an orthographic camera, with optional load/save mechanism
#if (meshcam_file>0)
  // get the file name
  #declare prefix="meshcam-ortho_"; 
  #declare mesh_file=concat(concat(concat(concat(prefix,str(image_width,0,0)),"x"),str(image_height,0,0)),".inc");
#else
  // ..wich should be empty to turn the mesh file off
  #declare mesh_file="";
#end
#if (meshcam_file=2)
  // loading a mesh from file
  #declare camera_mesh=
  #include mesh_file
#else
  // create a new mesh
  #declare camera_mesh=meshcam_orthographic(image_width, image_height, c_up_length, mesh_file)
#end

// create the camera with the generated or loaded mesh
camera{
  mesh_camera{ 1 0  // 1 ray per pixel, distribution 0 
    mesh{camera_mesh
      meshcam_placement(c_location,c_look_at)
    }
  }
  location <0,0,-.01> // look at the face slighty off along the normal
}

