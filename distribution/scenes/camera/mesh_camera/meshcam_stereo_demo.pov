// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

/* Persistence Of Vision Raytracer sample file.

   Demo for the Mesh Camera macros :
  
   How to create an stereo pair witht the mesh_camera using distribution #2
   
   This shows how to use the mesh camera, not how to setup a stereo pair... I know 
   this is not the most correct way, but it was the quickest.
   
   Note you have to render it twice as wide as the scene, as the output contains the two images
   side by side.
  
   --
   Jaime Vives Piqueres, Jan. 2011  <jaime@ignorancia.org> */

/********************************************************************************************
 * $File: //depot/povray/smp/distribution/scenes/camera/mesh_camera/meshcam_stereo_demo.pov $
 * $Revision: #1 $
 * $Change: 5377 $
 * $DateTime: 2011/01/09 19:56:00 $
 * $Author: jholsenback $
 *******************************************************************************************/
#version 3.7;

// control center
#declare meshcam_file=1;    // 0=off, 1=save mesh to file, 2=load mesh from file

// common globals for all the demos
#include "demo_globals.inc"

// standard includes
#include "colors.inc"
#include "textures.inc"

// common test subjects and scenario
#include "demo_common.inc"

// mesh camera macros being demonstrated
#include "meshcam_macros.inc"

// common camera parameters
#declare c_location=<4,2,-3>; // location
#declare c_look_at=<0,1.1,1>; // look at
#declare c_angle=54;          // angle
#declare c_stereo_base=.3;    // distance between cameras

// create a mesh simulating a pinhole camera (perspective) with optional load/save mechanism
// we create the mesh half as wide as the rendering, because we will use two copies side by side
#if (meshcam_file>0)
  // get the file name
  #declare mesh_file=concat(concat(concat(concat(concat(concat("meshcam-pinhole_",str(image_width*.5,0,0)),"x"),str(image_height,0,0)),"-angle_"),str(c_angle,0,0)),".inc");
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
  // the mesh width is half of the rendering width, as this is a stereo pair
  #declare camera_mesh=
  meshcam_pinhole(image_width*.5, image_height, c_angle, mesh_file)
#end

// create the camera with the generated or loaded mesh, 
// using two copies placed side by side (moved along the right vector of the camera)
#declare c_location_left =c_location+c_stereo_base*.5*vtransform(x,Reorient_Trans(z,c_look_at-c_location));  
#declare c_location_right=c_location-c_stereo_base*.5*vtransform(x,Reorient_Trans(z,c_look_at-c_location)); 
camera{
  mesh_camera{ 1 2 // distribution #2 is the appropiate on this case
    mesh{camera_mesh
      meshcam_placement(c_location_left,c_look_at)
    }
    mesh{camera_mesh
      meshcam_placement(c_location_right,c_look_at)
    }
  }
  location <0,0,-.01>
}

