// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer Scene Description File
// File:  f_fiinsh_.pov
// Vers: 3.5
// Desc: Render via f_finish.ini,
//       generates html files and images for all the
//       finishes in finish.inc
// Date: 2001/07/30
// Auth: Ingo Janssen
#version 3.6;
global_settings { 
  assumed_gamma 1.0
}

#include "colors.inc"
#include "finish.inc"
#include "html_gen.inc"

#declare Generate_HTML=yes;
#declare Generate_Images=yes;

// all finish names, extracted from finish.inc,
// as strings for the generation of the html-files
// and as identfiers for the generation of the images.
#declare str_finishArr=array[8] {
 "Dull", "Shiny", "Phong_Dull", "Phong_Shiny", "Glossy",
 "Phong_Glossy", "Luminous", "Mirror",
}
#declare finishArr=array[8] {
 Dull, Shiny, Phong_Dull, Phong_Shiny, Glossy,
 Phong_Glossy, Luminous, Mirror,
}

#if (Generate_HTML)
   #if (clock=0)                         // generate the html-files for showing the images in.
      #declare FromFileName="finish.inc"   // the name of the include file the data came from.
      #declare OutName="f_finish"          // the OutName should match with Output_File_Name in the ini-file!!!!
      #declare Keyword="finishe"           // the stuff represented in the array: texture, pigment, material, color etc.
      #declare DataArray=str_finishArr    // the array containing the strings of identifiers
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
      difference {
         sphere{0,1}
         box{<-1,0.9,-1>,<1,1.1,1>}
      }
      pigment {rgb 0.7}
      finish{finishArr[frame_number-1]}   // put the right arrray name here !!
   }
#end
