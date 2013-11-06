// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer Scene Description File
// File:  o_shapes2_.pov
// Vers: 3.5
// Desc: Render via o_shapes2.ini,
//       generates html files and images for all the
//       shapes in shapes2.inc
// Date: 2001/08/06
// Auth: Ingo Janssen
#version 3.6;
global_settings { 
  assumed_gamma 1.0
}

#include "colors.inc"
#include "shapes.inc"
#include "shapes2.inc"
#include "html_gen.inc"

#declare Generate_HTML=yes;
#declare Generate_Images=yes;

// all shape names extracted from shapes2.inc,
// as strings for the generation of the html-files
// and as identfiers for the generation of the images.
#declare str_Arr=array[12] {
//intersection
 "Tetrahedron", "Octahedron", "Dodecahedron", "Icosahedron",
 "HalfCone_Y", "Hexagon", "Rhomboid", "Pyramid2",
//union
 "Pyramid", "Square_X", "Square_Y", "Square_Z",
}
#declare Arr=array[12] {
//intersection
 Tetrahedron, Octahedron, Dodecahedron, Icosahedron,
 HalfCone_Y, Hexagon, Rhomboid, Pyramid2,
//union
 Pyramid, Square_X, Square_Y, Square_Z,
}

#if (Generate_HTML)
   #if (clock=0)                         // generate the html-files for showing the images in.
      #declare FromFileName="shapes2.inc"   // the name of the include file the data came from.
      #declare OutName="o_2shapes"          // the OutName should match with Output_File_Name in the ini-file!!!!
      #declare Keyword="object"           // the stuff represented in the array: texture, pigment, material, color etc.
      #declare DataArray=str_Arr    // the array containing the strings of identifiers
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
     location  <6,6,-6>
     look_at   <0,0,0>
     angle 35
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

