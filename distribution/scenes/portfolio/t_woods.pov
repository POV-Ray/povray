// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer Scene Description File
// File: t_woods_.pov
// Vers: 3.5
// Desc: Render via t_woods.ini,
//       generates html files and images for all the
//       textures in woods.inc
// Date: 2001/07/28
// Auth: Ingo Janssen
#version 3.6;
global_settings { 
  assumed_gamma 1.0
}

#include "colors.inc"
#include "woods.inc"
#include "html_gen.inc"


#declare Generate_HTML=yes;
#declare Generate_Images=yes;

// all texture names extracted from woods.inc,
// as strings for the generation of the html-files
// and as identfiers for the generation of the images.
#declare str_textureArr=array[35] {
   "T_Wood1", "T_Wood2", "T_Wood3", "T_Wood4", "T_Wood5",
   "T_Wood6", "T_Wood7", "T_Wood8", "T_Wood9", "T_Wood10",
   "T_Wood11", "T_Wood12", "T_Wood13", "T_Wood14", "T_Wood15",
   "T_Wood16", "T_Wood17", "T_Wood18", "T_Wood19", "T_Wood20",
   "T_Wood21", "T_Wood22", "T_Wood23", "T_Wood24", "T_Wood25",
   "T_Wood26", "T_Wood27", "T_Wood28", "T_Wood29", "T_Wood30",
   "T_Wood31", "T_Wood32", "T_Wood33", "T_Wood34", "T_Wood35",
}
#declare textureArr=array[35] {
   T_Wood1, T_Wood2, T_Wood3, T_Wood4, T_Wood5,
   T_Wood6, T_Wood7, T_Wood8, T_Wood9, T_Wood10,
   T_Wood11, T_Wood12, T_Wood13, T_Wood14, T_Wood15,
   T_Wood16, T_Wood17, T_Wood18, T_Wood19, T_Wood20,
   T_Wood21, T_Wood22, T_Wood23, T_Wood24, T_Wood25,
   T_Wood26, T_Wood27, T_Wood28, T_Wood29, T_Wood30,
   T_Wood31, T_Wood32, T_Wood33, T_Wood34, T_Wood35,
}

#if (Generate_HTML)
   #if (clock=0)                         // generate the html-files for showing the images in.
      #declare FromFileName="woods.inc"  // the name of the include file the data came from.
      #declare OutName="t_woods"        // the OutName should match with Output_File_Name in the ini-file!!!!
      #declare Keyword="texture"         // the stuff represented in the array: texture, pigment, material, color etc.
      #declare DataArray=str_textureArr  // the array containing the strings of identifiers
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
     location  <0,4,0.01>
     look_at   <0,0, 0.0>
     angle 35
   }
   
   light_source {<-500,300,-500> rgb 1}
   light_source {<500,500,500> rgb 0.7 shadowless}
   
   union {
      plane {y, 0}
      sphere{0, 1
         scale <0.6,0.3,0.6>
         translate<0.55,0,0.5>
      }
      texture{textureArr[frame_number-1]}
   }
   
   box {
      -1,1
      scale 0.5
      translate <-0.65,-0.4,0.5>
      texture{textureArr[frame_number-1] rotate<90,0,0>}
   }                             
#end
