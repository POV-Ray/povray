// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer Scene Description File
// File:  o_shapes_.pov
// Vers: 3.5
// Desc: Render via o_shapes_old.ini,
//       generates html files and images for all the
//       shapes in shapes_old.inc
// Date: 2001/07/30
// Auth: ...
#version 3.6;
global_settings { 
  assumed_gamma 1.0
}

#include "colors.inc"
#include "shapes_old.inc"
#include "html_gen.inc"

#declare Generate_HTML=yes;
#declare Generate_Images=yes;

// all object names extracted from shapes_old.inc,
// as strings for the generation of the html-files
// and as identfiers for the generation of the images.
#declare str_Arr=array[21] {
//box
 "UnitBox", "Cube",
//cone
 "Cone_X", "Cone_Y", "Cone_Z",
//cylinder
 "Disk_X", "Disk_Y", "Disk_Z",
//quadric
 "Cylinder_X", "Cylinder_Y", "Cylinder_Z", "QCone_X",
 "QCone_Y", "QCone_Z", "Paraboloid_X", "Paraboloid_Y", "Paraboloid_Z",
 "Hyperboloid", "Hyperboloid_Y",
//sphere
 "Ellipsoid", "Sphere",
}
#declare Arr=array[21] {
//box
 UnitBox, Cube,
//cone
 Cone_X, Cone_Y, Cone_Z,
//cylinder
 Disk_X, Disk_Y, Disk_Z,
//quadric
 Cylinder_X, Cylinder_Y, Cylinder_Z, QCone_X,
 QCone_Y, QCone_Z, Paraboloid_X, Paraboloid_Y, Paraboloid_Z,
 Hyperboloid, Hyperboloid_Y,
//sphere
 Ellipsoid, Sphere,
}


#if (Generate_HTML)
   #if (clock=0)                         // generate the html-files for showing the images in.
      #declare FromFileName="shapes_old.inc"   // the name of the include file the data came from.
      #declare OutName="o_shapes_old"          // the OutName should match with Output_File_Name in the ini-file!!!!
      #declare Keyword="object"           // the stuff represented in the array: texture, pigment, material, color etc.
      #declare DataArray=str_Arr         // the array containing the strings of identifiers
      #declare NumPicHorizonal=3;        // the amount of images per row in the table
      #declare NumPicVertical=2;         // the amount of images per collumn in the table
      #declare IW=image_width;           // the dimesions of the image, these are set in the ini-file!
      #declare IH=image_height;
      #declare Comment=""
      HTMLgen(FromFileName, OutName, Keyword, DataArray, NumPicHorizonal, NumPicVertical, IW, IH, Comment)
   #end
#end

#if(Generate_Images)
   camera {
     right x*image_width/image_height
     location  <3,3,-3>
     look_at   <0,0,0>
     //angle 35
   }
   
   light_source {<500,500,-50> rgb 1}
   light_source {<-500,500,-500> rgb <0.1,0.1,0.3> shadowless}
   light_source {<3,3,-3> rgb <0.5,0.4,0.4>}

   sky_sphere {
      pigment {
         planar
         color_map { [0.0 color blue 0.5] [1.0 color rgb <0.7,0.7,1.0>] }
      }
   }
   object {
      Arr[frame_number-1]   // put the right arrray name here !!
      pigment {rgb 1}
   }
#end

