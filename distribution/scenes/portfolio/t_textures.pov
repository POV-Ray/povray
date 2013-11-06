// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer Scene Description File
// File:  t_textures_.pov
// Vers: 3.5
// Desc: Render via t_textures.ini,
//       generates html files and images for all the
//       textures in textures.inc
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

// all texture names extracted from textures.inc,
// as strings for the generation of the html-files
// and as identfiers for the generation of the images.
#declare str_textureArr=array[40] {
 "PinkAlabaster", "Shadow_Clouds", "DMFWood6", "EMBWood1", "Yellow_Pine",
 "Rosewood", "Sandalwood", "Chrome_Texture", "Brass_Texture", "Gold_Texture",
 "Bronze_Texture", "Copper_Texture", "Silver_Texture", "Chrome_Metal", "Brass_Metal",
 "Bronze_Metal", "Gold_Metal", "Silver_Metal", "Copper_Metal", "Polished_Chrome",
 "Polished_Brass", "New_Brass", "Spun_Brass", "Brushed_Aluminum", "Silver1",
 "Silver2", "Silver3", "Brass_Valley", "Rust", "Rusty_Iron",
 "Soft_Silver", "New_Penny", "Tinny_Brass", "Gold_Nugget", "Aluminum",
 "Bright_Bronze", "Cork", "Lightning1", "Lightning2", "Starfield",
}
#declare textureArr=array[40] {
 PinkAlabaster, Shadow_Clouds, DMFWood6, EMBWood1, Yellow_Pine,
 Rosewood, Sandalwood, Chrome_Texture, Brass_Texture, Gold_Texture,
 Bronze_Texture, Copper_Texture, Silver_Texture, Chrome_Metal, Brass_Metal,
 Bronze_Metal, Gold_Metal, Silver_Metal, Copper_Metal, Polished_Chrome,
 Polished_Brass, New_Brass, Spun_Brass, Brushed_Aluminum, Silver1,
 Silver2, Silver3, Brass_Valley, Rust, Rusty_Iron,
 Soft_Silver, New_Penny, Tinny_Brass, Gold_Nugget, Aluminum,
 Bright_Bronze, Cork, Lightning1, Lightning2, Starfield,
}


#if (Generate_HTML)
   #if (clock=0)                         // generate the html-files for showing the images in.
      #declare FromFileName="textures.inc"   // the name of the include file the data came from.
      #declare OutName="t_textures"          // the OutName should match with Output_File_Name in the ini-file!!!!
      #declare Keyword="texture"           // the stuff represented in the array: texture, pigment, material, color etc.
      #declare DataArray=str_textureArr    // the array containing the strings of identifiers
      #declare NumPicHorizonal=3;        // the amount of images per row in the table
      #declare NumPicVertical=2;         // the amount of images per collumn in the table
      #declare IW=image_width;           // the dimesions of the image, these are set in the ini-file!
      #declare IH=image_height;
      #declare Comment=""
      HTMLgen(FromFileName, OutName, Keyword, DataArray, NumPicHorizonal, NumPicVertical, IW, IH, Comment)
   #end
#end

#if(Generate_Images)
   sky_sphere {
      pigment{Clouds}
   }

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
      texture{textureArr[frame_number-1]}   // put the right arrray name here !!
   }
#end
