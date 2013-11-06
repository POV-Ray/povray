// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer Scene Description File
// File: t_woods_.pov
// Vers: 3.5
// Desc: Render via t_woods.ini,
//       generates html files and images for all the
//       pigments in woods.inc
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

// all pigment names extracted from woods.inc,
// as strings for the generation of the html-files
// and as identfiers for the generation of the images.
#declare str_pigmentArr=array[38] {
 "P_WoodGrain1A", "P_WoodGrain1B", "P_WoodGrain2A", "P_WoodGrain2B", "P_WoodGrain3A",
 "P_WoodGrain3B", "P_WoodGrain4A", "P_WoodGrain4B", "P_WoodGrain5A", "P_WoodGrain5B",
 "P_WoodGrain6A", "P_WoodGrain6B", "P_WoodGrain7A", "P_WoodGrain7B", "P_WoodGrain8A",
 "P_WoodGrain8B", "P_WoodGrain9A", "P_WoodGrain9B", "P_WoodGrain10A", "P_WoodGrain10B",
 "P_WoodGrain11A", "P_WoodGrain11B", "P_WoodGrain12A", "P_WoodGrain12B", "P_WoodGrain13A",
 "P_WoodGrain13B", "P_WoodGrain14A", "P_WoodGrain14B", "P_WoodGrain15A", "P_WoodGrain15B",
 "P_WoodGrain16A", "P_WoodGrain16B", "P_WoodGrain17A", "P_WoodGrain17B", "P_WoodGrain18A",
 "P_WoodGrain18B", "P_WoodGrain19A", "P_WoodGrain19B",
}
#declare pigmentArr=array[38] {
 P_WoodGrain1A, P_WoodGrain1B, P_WoodGrain2A, P_WoodGrain2B, P_WoodGrain3A,
 P_WoodGrain3B, P_WoodGrain4A, P_WoodGrain4B, P_WoodGrain5A, P_WoodGrain5B,
 P_WoodGrain6A, P_WoodGrain6B, P_WoodGrain7A, P_WoodGrain7B, P_WoodGrain8A,
 P_WoodGrain8B, P_WoodGrain9A, P_WoodGrain9B, P_WoodGrain10A, P_WoodGrain10B,
 P_WoodGrain11A, P_WoodGrain11B, P_WoodGrain12A, P_WoodGrain12B, P_WoodGrain13A,
 P_WoodGrain13B, P_WoodGrain14A, P_WoodGrain14B, P_WoodGrain15A, P_WoodGrain15B,
 P_WoodGrain16A, P_WoodGrain16B, P_WoodGrain17A, P_WoodGrain17B, P_WoodGrain18A,
 P_WoodGrain18B, P_WoodGrain19A, P_WoodGrain19B,
}

#if (Generate_HTML)
   #if (clock=0)                         // generate the html-files for showing the images in.
      #declare FromFileName="woods.inc"  // the name of the include file the data came from.
      #declare OutName="p_woods"        // the OutName should match with Output_File_Name in the ini-file!!!!
      #declare Keyword="pigment"         // the stuff represented in the array: texture, pigment, material, color etc.
      #declare DataArray=str_pigmentArr  // the array containing the strings of identifiers
      #declare NumPicHorizonal=3;        // the amount of images per row in the table
      #declare NumPicVertical=2;         // the amount of images per collumn in the table
      #declare IW=image_width;           // the dimesions of the image, these are set in the ini-file!
      #declare IH=image_height;
      #declare Comment="<p>The pigments, that the wood textures are based on.</br> Some pigments use the default colour of the pattern, this can result in harsh colors</p>"
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
      sphere{
         0,1 
         scale <0.6,0.3,0.6>
         translate<0.55,0,0.5>
      }
      pigment{pigmentArr[frame_number-1]}
   }
   
   box {
      -1,1
      scale 0.5
      translate <-0.65,-0.4,0.5>
      pigment{pigmentArr[frame_number-1] rotate<90,0,0>}
   }                             
#end
