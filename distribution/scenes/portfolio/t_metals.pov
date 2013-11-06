// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer Scene Description File
// File:  t_metals_.pov
// Vers: 3.5
// Desc: Render via t_metals.ini,
//       generates html files and images for all the
//       textures in metals.inc
// Date: 2001/07/30
// Auth: Ingo Janssen
#version 3.6;
global_settings { 
  assumed_gamma 1.0
}

#include "colors.inc"
#include "metals.inc"
#include "golds.inc"
#include "html_gen.inc"

#declare Generate_HTML=yes;
#declare Generate_Images=yes;

// all texture names extracted from metals.inc,
// as strings for the generation of the html-files
// and as identfiers for the generation of the images.
#declare str_textureArr=array[125] {
   "T_Brass_1A", "T_Brass_1B", "T_Brass_1C", "T_Brass_1D", "T_Brass_1E",
   "T_Brass_2A", "T_Brass_2B", "T_Brass_2C", "T_Brass_2D", "T_Brass_2E",
   "T_Brass_3A", "T_Brass_3B", "T_Brass_3C", "T_Brass_3D", "T_Brass_3E",
   "T_Brass_4A", "T_Brass_4B", "T_Brass_4C", "T_Brass_4D", "T_Brass_4E",
   "T_Brass_5A", "T_Brass_5B", "T_Brass_5C", "T_Brass_5D", "T_Brass_5E",
   "T_Copper_1A", "T_Copper_1B", "T_Copper_1C", "T_Copper_1D", "T_Copper_1E",
   "T_Copper_2A", "T_Copper_2B", "T_Copper_2C", "T_Copper_2D", "T_Copper_2E",
   "T_Copper_3A", "T_Copper_3B", "T_Copper_3C", "T_Copper_3D", "T_Copper_3E",
   "T_Copper_4A", "T_Copper_4B", "T_Copper_4C", "T_Copper_4D", "T_Copper_4E",
   "T_Copper_5A", "T_Copper_5B", "T_Copper_5C", "T_Copper_5D", "T_Copper_5E",
   "T_Chrome_1A", "T_Chrome_1B", "T_Chrome_1C", "T_Chrome_1D", "T_Chrome_1E",
   "T_Chrome_2A", "T_Chrome_2B", "T_Chrome_2C", "T_Chrome_2D", "T_Chrome_2E",
   "T_Chrome_3A", "T_Chrome_3B", "T_Chrome_3C", "T_Chrome_3D", "T_Chrome_3E",
   "T_Chrome_4A", "T_Chrome_4B", "T_Chrome_4C", "T_Chrome_4D", "T_Chrome_4E",
   "T_Chrome_5A", "T_Chrome_5B", "T_Chrome_5C", "T_Chrome_5D", "T_Chrome_5E",
   "T_Silver_1A", "T_Silver_1B", "T_Silver_1C", "T_Silver_1D", "T_Silver_1E",
   "T_Silver_2A", "T_Silver_2B", "T_Silver_2C", "T_Silver_2D", "T_Silver_2E",
   "T_Silver_3A", "T_Silver_3B", "T_Silver_3C", "T_Silver_3D", "T_Silver_3E",
   "T_Silver_4A", "T_Silver_4B", "T_Silver_4C", "T_Silver_4D", "T_Silver_4E",
   "T_Silver_5A", "T_Silver_5B", "T_Silver_5C", "T_Silver_5D", "T_Silver_5E",
 "T_Gold_1A", "T_Gold_1B", "T_Gold_1C", "T_Gold_1D", "T_Gold_1E",
 "T_Gold_2A", "T_Gold_2B", "T_Gold_2C", "T_Gold_2D", "T_Gold_2E",
 "T_Gold_3A", "T_Gold_3B", "T_Gold_3C", "T_Gold_3D", "T_Gold_3E",
 "T_Gold_4A", "T_Gold_4B", "T_Gold_4C", "T_Gold_4D", "T_Gold_4E",
 "T_Gold_5A", "T_Gold_5B", "T_Gold_5C", "T_Gold_5D", "T_Gold_5E",
}
#declare textureArr=array[125] {
   T_Brass_1A, T_Brass_1B, T_Brass_1C, T_Brass_1D, T_Brass_1E,
   T_Brass_2A, T_Brass_2B, T_Brass_2C, T_Brass_2D, T_Brass_2E,
   T_Brass_3A, T_Brass_3B, T_Brass_3C, T_Brass_3D, T_Brass_3E,
   T_Brass_4A, T_Brass_4B, T_Brass_4C, T_Brass_4D, T_Brass_4E,
   T_Brass_5A, T_Brass_5B, T_Brass_5C, T_Brass_5D, T_Brass_5E,
   T_Copper_1A, T_Copper_1B, T_Copper_1C, T_Copper_1D, T_Copper_1E,
   T_Copper_2A, T_Copper_2B, T_Copper_2C, T_Copper_2D, T_Copper_2E,
   T_Copper_3A, T_Copper_3B, T_Copper_3C, T_Copper_3D, T_Copper_3E,
   T_Copper_4A, T_Copper_4B, T_Copper_4C, T_Copper_4D, T_Copper_4E,
   T_Copper_5A, T_Copper_5B, T_Copper_5C, T_Copper_5D, T_Copper_5E,
   T_Chrome_1A, T_Chrome_1B, T_Chrome_1C, T_Chrome_1D, T_Chrome_1E,
   T_Chrome_2A, T_Chrome_2B, T_Chrome_2C, T_Chrome_2D, T_Chrome_2E,
   T_Chrome_3A, T_Chrome_3B, T_Chrome_3C, T_Chrome_3D, T_Chrome_3E,
   T_Chrome_4A, T_Chrome_4B, T_Chrome_4C, T_Chrome_4D, T_Chrome_4E,
   T_Chrome_5A, T_Chrome_5B, T_Chrome_5C, T_Chrome_5D, T_Chrome_5E,
   T_Silver_1A, T_Silver_1B, T_Silver_1C, T_Silver_1D, T_Silver_1E,
   T_Silver_2A, T_Silver_2B, T_Silver_2C, T_Silver_2D, T_Silver_2E,
   T_Silver_3A, T_Silver_3B, T_Silver_3C, T_Silver_3D, T_Silver_3E,
   T_Silver_4A, T_Silver_4B, T_Silver_4C, T_Silver_4D, T_Silver_4E,
   T_Silver_5A, T_Silver_5B, T_Silver_5C, T_Silver_5D, T_Silver_5E,
 T_Gold_1A, T_Gold_1B, T_Gold_1C, T_Gold_1D, T_Gold_1E,
 T_Gold_2A, T_Gold_2B, T_Gold_2C, T_Gold_2D, T_Gold_2E,
 T_Gold_3A, T_Gold_3B, T_Gold_3C, T_Gold_3D, T_Gold_3E,
 T_Gold_4A, T_Gold_4B, T_Gold_4C, T_Gold_4D, T_Gold_4E,
 T_Gold_5A, T_Gold_5B, T_Gold_5C, T_Gold_5D, T_Gold_5E,
}


#if (Generate_HTML)
   #if (clock=0)                         // generate the html-files for showing the images in.
      #declare FromFileName="metals.inc, golds.inc"   // the name of the include file the data came from.
      #declare OutName="t_metals"          // the OutName should match with Output_File_Name in the ini-file!!!!
      #declare Keyword="texture"           // the stuff represented in the array: texture, pigment, material, color etc.
      #declare DataArray=str_textureArr    // the array containing the strings of identifiers
      #declare NumPicHorizonal=5;        // the amount of images per row in the table
      #declare NumPicVertical=5;         // the amount of images per collumn in the table
      #declare IW=image_width;           // the dimesions of the image, these are set in the ini-file!
      #declare IH=image_height;
      #declare Comment=""
      HTMLgen(FromFileName, OutName, Keyword, DataArray, NumPicHorizonal, NumPicVertical, IW, IH, Comment)
   #end
#end

#if(Generate_Images)
   camera {
     right x*image_width/image_height
     location  <0,4.5,0.01>
     look_at   <0,0, 0.0>
     angle 35
   }
   
   light_source {<-100,300,-500> rgb 1}
   light_source {<500,500,500> rgb 0.7 shadowless}
   
   sky_sphere {
      pigment {
         planar
         turbulence 0.5
         octaves 7
         lambda 1
         omega 0.5
      }
   }
   
   union {
      plane {y, 0}
      torus {
         1,0.2
         translate <0,-0.1,0>
      }
      texture{textureArr[frame_number-1]}   // put the right arrray name here !!
   }
#end
