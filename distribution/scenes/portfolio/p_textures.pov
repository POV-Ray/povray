// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer Scene Description File
// File:  p_textures_.pov
// Vers: 3.5
// Desc: Render via p_textures.ini,
//       generates html files and images for all the
//       pigments in textures.inc
// Date: 2001/07/30
// Auth: Ingo Janssen
#version 3.6;
global_settings { 
  assumed_gamma 1.0
}

#include "colors.inc"
#include "textures.inc"
#include "html_gen.inc"

#declare Generate_HTML=yes;
#declare Generate_Images=yes;

// all pigment names extracted from texture.inc,
// as strings for the generation of the html-files
// and as identfiers for the generation of the images.
#declare str_pigmentArr=array[33] {
 "Jade", "Red_Marble", "White_Marble", "Blood_Marble", "Blue_Agate",
 "Sapphire_Agate", "Brown_Agate", "Pink_Granite", "Blue_Sky", "Bright_Blue_Sky",
 "Blue_Sky2", "Blue_Sky3", "Blood_Sky", "Apocalypse", "Clouds",
 "FBM_Clouds", "Cherry_Wood", "Pine_Wood", "Dark_Wood", "Tan_Wood",
 "White_Wood", "Tom_Wood", "DMFWood1", "DMFWood2", "DMFWood3",
 "DMFWood4", "DMFWood5", "DMFLightOak", "DMFDarkOak", "Candy_Cane",
 "Peel", "Y_Gradient", "X_Gradient",
}
#declare pigmentArr=array[33] {
 Jade, Red_Marble, White_Marble, Blood_Marble, Blue_Agate,
 Sapphire_Agate, Brown_Agate, Pink_Granite, Blue_Sky, Bright_Blue_Sky,
 Blue_Sky2, Blue_Sky3, Blood_Sky, Apocalypse, Clouds,
 FBM_Clouds, Cherry_Wood, Pine_Wood, Dark_Wood, Tan_Wood,
 White_Wood, Tom_Wood, DMFWood1, DMFWood2, DMFWood3,
 DMFWood4, DMFWood5, DMFLightOak, DMFDarkOak, Candy_Cane,
 Peel, Y_Gradient, X_Gradient,
}


#if (Generate_HTML)
   #if (clock=0)                         // generate the html-files for showing the images in.
      #declare FromFileName="textures.inc"   // the name of the include file the data came from.
      #declare OutName="p_textures"          // the OutName should match with Output_File_Name in the ini-file!!!!
      #declare Keyword="pigment"           // the stuff represented in the array: texture, pigment, material, color etc.
      #declare DataArray=str_pigmentArr    // the array containing the strings of identifiers
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
      pigment{pigmentArr[frame_number-1]}   // put the right arrray name here !!
   }
#end
