// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer Scene Description File
// File: t_stone1_.pov
// Vers: 3.5
// Desc: Render via t_stones1.ini,
//       generates html files and images for all the
//       textures in stones1.inc
// Date: 2001/07/28
// Auth: Ingo Janssen
#version 3.6;
global_settings { 
  assumed_gamma 1.0
}

#include "colors.inc"
#include "stones1.inc"
#include "html_gen.inc"

#declare Generate_HTML=yes;
#declare Generate_Images=yes;

// all texture names extracted from stones1.inc,
// as strings for the generation of the html-files
// and as identfiers for the generation of the images.
#declare str_textureArr=array[83] {
  "T_Grnt0",  "T_Grnt1",  "T_Grnt2",  "T_Grnt3",  "T_Grnt4",
  "T_Grnt5",  "T_Grnt6",  "T_Grnt7",  "T_Grnt8",  "T_Grnt9",
  "T_Grnt10",  "T_Grnt11",  "T_Grnt12",  "T_Grnt13",  "T_Grnt14",
  "T_Grnt15",  "T_Grnt16",  "T_Grnt17",  "T_Grnt18",  "T_Grnt19",
  "T_Grnt20",  "T_Grnt21",  "T_Grnt22",  "T_Grnt23",  "T_Grnt24",
  "T_Grnt25",  "T_Grnt26",  "T_Grnt27",  "T_Grnt28",  "T_Grnt29",
  "T_Grnt0a",  "T_Grnt1a",  "T_Grnt2a",  "T_Grnt3a",  "T_Grnt4a",
  "T_Grnt5a",  "T_Grnt6a",  "T_Grnt7a",  "T_Grnt8a",  "T_Grnt9a",
  "T_Grnt10a",  "T_Grnt11a",  "T_Grnt12a",  "T_Grnt13a",  "T_Grnt14a",
  "T_Grnt15a",  "T_Grnt16a",  "T_Grnt17a",  "T_Grnt18a",  "T_Grnt19a",
  "T_Grnt20a",  "T_Grnt21a",  "T_Grnt22a",  "T_Grnt23a",  "T_Grnt24a",
  "T_Crack1",  "T_Crack2",  "T_Crack3",  "T_Crack4",  "T_Stone1",
  "T_Stone2",  "T_Stone3",  "T_Stone4",  "T_Stone5",  "T_Stone6",
  "T_Stone7",  "T_Stone8",  "T_Stone9",  "T_Stone10",  "T_Stone11",
  "T_Stone12",  "T_Stone13",  "T_Stone14",  "T_Stone15",  "T_Stone16",
  "T_Stone17",  "T_Stone18",  "T_Stone19",  "T_Stone20",  "T_Stone21",
  "T_Stone22",  "T_Stone23",  "T_Stone24",
}
#declare textureArr=array[83] {
  T_Grnt0,  T_Grnt1,  T_Grnt2,  T_Grnt3,  T_Grnt4,
  T_Grnt5,  T_Grnt6,  T_Grnt7,  T_Grnt8,  T_Grnt9,
  T_Grnt10,  T_Grnt11,  T_Grnt12,  T_Grnt13,  T_Grnt14,
  T_Grnt15,  T_Grnt16,  T_Grnt17,  T_Grnt18,  T_Grnt19,
  T_Grnt20,  T_Grnt21,  T_Grnt22,  T_Grnt23,  T_Grnt24,
  T_Grnt25,  T_Grnt26,  T_Grnt27,  T_Grnt28,  T_Grnt29,
  T_Grnt0a,  T_Grnt1a,  T_Grnt2a,  T_Grnt3a,  T_Grnt4a,
  T_Grnt5a,  T_Grnt6a,  T_Grnt7a,  T_Grnt8a,  T_Grnt9a,
  T_Grnt10a,  T_Grnt11a,  T_Grnt12a,  T_Grnt13a,  T_Grnt14a,
  T_Grnt15a,  T_Grnt16a,  T_Grnt17a,  T_Grnt18a,  T_Grnt19a,
  T_Grnt20a,  T_Grnt21a,  T_Grnt22a,  T_Grnt23a,  T_Grnt24a,
  T_Crack1,  T_Crack2,  T_Crack3,  T_Crack4,  T_Stone1,
  T_Stone2,  T_Stone3,  T_Stone4,  T_Stone5,  T_Stone6,
  T_Stone7,  T_Stone8,  T_Stone9,  T_Stone10,  T_Stone11,
  T_Stone12,  T_Stone13,  T_Stone14,  T_Stone15,  T_Stone16,
  T_Stone17,  T_Stone18,  T_Stone19,  T_Stone20,  T_Stone21,
  T_Stone22,  T_Stone23,  T_Stone24,
}

#if (Generate_HTML)
   #if (clock=0)                         // generate the html-files for showing the images in.
      #declare FromFileName="stones1.inc"// the name of the include file the data came from.
      #declare OutName="t_1stones"      // the OutName should match with Output_File_Name in the ini-file!!!!
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
      difference {
         sphere{0,1}
         box{<-1,0.9,-1>,<1,1.1,1>}
      }
      texture{textureArr[frame_number-1]}
   }
#end
