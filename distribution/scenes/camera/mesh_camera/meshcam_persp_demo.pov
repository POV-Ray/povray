// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

/* Persistence Of Vision Raytracer sample file.

   Demo for the Mesh Camera macros for use with the new mesh_camera:
  
   Shows how to use the macros to create a custom mesh camera and apply some effects, like antialiasing
   and vignetting, or to save it to a file for reuse. 

   --
   Jaime Vives Piqueres, Jan. 2011  <jaime@ignorancia.org> */

/*******************************************************************************************
 * $File: //depot/povray/smp/distribution/scenes/camera/mesh_camera/meshcam_persp_demo.pov $
 * $Revision: #1 $
 * $Change: 5377 $
 * $DateTime: 2011/01/09 19:56:00 $
 * $Author: jholsenback $
 ******************************************************************************************/
#version 3.7;

// control center
#declare meshcam_file=1;      // 0=off, 1=save mesh to file, 2=load mesh from file
#declare aa_samples=5;        // extra rays per pixel (one mesh copy per ray) (use 0 to turn aa off)
#declare aa_aperture=.02;     // max displacement along up and right vector
#declare aa_rand=seed(78);    // random seed for the fake aa 
#declare use_vignetting=1;    // vignetting effect: 0=off, 1=on, fixed amount
#declare use_distortion=.15;    // lens distortion: 0=off, >0=barrel, <0=pincushion 

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
// calculate up and right vector transforms for fake antialiasing 
#declare f_up=vtransform(y,Reorient_Trans(z,c_look_at-c_location));
#declare f_right=vtransform(x,Reorient_Trans(z,c_look_at-c_location));

// create a mesh simulating a perspective camera, with or without distortion, with optional load/save mechanism
#if (meshcam_file>0)
  // get the file name
  #if (use_distortion=0) 
    #declare prefix="meshcam-pinhole_"; 
  #else 
    #declare prefix="meshcam-lens_"; 
  #end
  #declare mesh_file=concat(concat(concat(concat(concat(concat(prefix,str(image_width,0,0)),"x"),str(image_height,0,0)),"-angle_"),str(c_angle,0,0)),".inc");
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
  #if (use_distortion=0)
    #declare camera_mesh=meshcam_pinhole(image_width, image_height, c_angle, mesh_file)
  #else
    #declare camera_mesh=meshcam_lens(image_width, image_height, c_angle, use_distortion, mesh_file)
  #end
#end

// create the camera with the generated or loaded mesh
camera{
  mesh_camera{ aa_samples+1 0  // distribution 1 will do the job too in this case, as all the meshes are the same
    mesh{camera_mesh
      meshcam_placement(c_location,c_look_at)
    }
    // additional copies of the mesh for fake antialiasing, randomly translated along the viewing plane
    #declare i_samples=0;
    #while (i_samples<aa_samples)
      #declare c_look_at_tmp=c_look_at+((-f_up*.5+f_up*rand(aa_rand))*aa_aperture*i_samples/aa_samples)
                                      +((-f_right*.5+f_right*rand(aa_rand))*aa_aperture*i_samples/aa_samples);       
      mesh{camera_mesh
        meshcam_placement(c_location,c_look_at_tmp)
      }
      #local i_samples=i_samples+1;
    #end
  }
  location <0,0,-.01> // look at the face slighty off along the normal
}

// add vignetting if enabled, by instancing the mesh camera on the scene with a tricky texture
#if (use_vignetting)
mesh{camera_mesh
  texture{
    pigment{
      cylindrical poly_wave 2
      color_map{
        [0.2 rgb 0 transmit 0]
        [0.5 rgb 0 transmit 1]
        [1.0 rgb 0 transmit 1]
      }
      rotate 90*x
      scale <image_width/image_height,1,1>
    } 
    finish{emission 1}
  }
  hollow no_shadow
  meshcam_placement(c_location,c_look_at)
}
#end



