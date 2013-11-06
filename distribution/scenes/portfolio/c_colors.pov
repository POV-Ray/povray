// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer Scene Description File
// File: stone2_.pov
// Vers: 3.5
// Desc: Render via colors.ini,
//       generates html files and images for all the
//       colors in colors.inc
// Date: 2001/07/28
// Auth: Ingo Janssen
#version 3.6;
global_settings { 
  assumed_gamma 1.0
}

#include "colors.inc"
#include "html_gen.inc"


#declare Generate_HTML=yes;
#declare Generate_Images=yes;

#declare str_colorArr=array[130] {
   "Red", "Green", "Blue", "Yellow", "Cyan",
   "Magenta", "Clear", "White", "Black", "Gray05",
   "Gray10", "Gray15", "Gray20", "Gray25", "Gray30",
   "Gray35", "Gray40", "Gray45", "Gray50", "Gray55",
   "Gray60", "Gray65", "Gray70", "Gray75", "Gray80",
   "Gray85", "Gray90", "Gray95", "DimGray", "DimGrey",
   "Gray", "Grey", "LightGray", "LightGrey", "VLightGray",
   "VLightGrey", "Aquamarine", "BlueViolet", "Brown", "CadetBlue",
   "Coral", "CornflowerBlue", "DarkGreen", "DarkOliveGreen", "DarkOrchid",
   "DarkSlateBlue", "DarkSlateGray", "DarkSlateGrey", "DarkTurquoise", "Firebrick",
   "ForestGreen", "Gold", "Goldenrod", "GreenYellow", "IndianRed",
   "Khaki", "LightBlue", "LightSteelBlue", "LimeGreen", "Maroon",
   "MediumAquamarine", "MediumBlue", "MediumForestGreen", "MediumGoldenrod", "MediumOrchid",
   "MediumSeaGreen", "MediumSlateBlue", "MediumSpringGreen", "MediumTurquoise", "MediumVioletRed",
   "MidnightBlue", "Navy", "NavyBlue", "Orange", "OrangeRed",
   "Orchid", "PaleGreen", "Pink", "Plum", "Salmon",
   "SeaGreen", "Sienna", "SkyBlue", "SlateBlue", "SpringGreen",
   "SteelBlue", "Tan", "Thistle", "Turquoise", "Violet",
   "VioletRed", "Wheat", "YellowGreen", "SummerSky", "RichBlue",
   "Brass", "Copper", "Bronze", "Bronze2", "Silver",
   "BrightGold", "OldGold", "Feldspar", "Quartz", "Mica",
   "NeonPink", "DarkPurple", "NeonBlue", "CoolCopper", "MandarinOrange",
   "LightWood", "MediumWood", "DarkWood", "SpicyPink", "SemiSweetChoc",
   "BakersChoc", "Flesh", "NewTan", "NewMidnightBlue", "VeryDarkBrown",
   "DarkBrown", "DarkTan", "GreenCopper", "DkGreenCopper", "DustyRose",
   "HuntersGreen", "Scarlet", "Med_Purple", "Light_Purple", "Very_Light_Purple",
}
#declare colorArr=array[130] {
   Red, Green, Blue, Yellow, Cyan,
   Magenta, Clear, White, Black, Gray05,
   Gray10, Gray15, Gray20, Gray25, Gray30,
   Gray35, Gray40, Gray45, Gray50, Gray55,
   Gray60, Gray65, Gray70, Gray75, Gray80,
   Gray85, Gray90, Gray95, DimGray, DimGrey,
   Gray, Grey, LightGray, LightGrey, VLightGray,
   VLightGrey, Aquamarine, BlueViolet, Brown, CadetBlue,
   Coral, CornflowerBlue, DarkGreen, DarkOliveGreen, DarkOrchid,
   DarkSlateBlue, DarkSlateGray, DarkSlateGrey, DarkTurquoise, Firebrick,
   ForestGreen, Gold, Goldenrod, GreenYellow, IndianRed,
   Khaki, LightBlue, LightSteelBlue, LimeGreen, Maroon,
   MediumAquamarine, MediumBlue, MediumForestGreen, MediumGoldenrod, MediumOrchid,
   MediumSeaGreen, MediumSlateBlue, MediumSpringGreen, MediumTurquoise, MediumVioletRed,
   MidnightBlue, Navy, NavyBlue, Orange, OrangeRed,
   Orchid, PaleGreen, Pink, Plum, Salmon,
   SeaGreen, Sienna, SkyBlue, SlateBlue, SpringGreen,
   SteelBlue, Tan, Thistle, Turquoise, Violet,
   VioletRed, Wheat, YellowGreen, SummerSky, RichBlue,
   Brass, Copper, Bronze, Bronze2, Silver,
   BrightGold, OldGold, Feldspar, Quartz, Mica,
   NeonPink, DarkPurple, NeonBlue, CoolCopper, MandarinOrange,
   LightWood, MediumWood, DarkWood, SpicyPink, SemiSweetChoc,
   BakersChoc, Flesh, NewTan, NewMidnightBlue, VeryDarkBrown,
   DarkBrown, DarkTan, GreenCopper, DkGreenCopper, DustyRose,
   HuntersGreen, Scarlet, Med_Purple, Light_Purple, Very_Light_Purple,
}

#if (Generate_HTML)
   #if (clock=0)                         // generate the html-files for showing the images in.
      #declare FromFileName="colors.inc" // the name of the include file the data came from.
      #declare OutName="c_colors"       // the OutName should match with Output_File_Name in the ini-file!!!!
      #declare Keyword="color"           // the stuff represented in the array: texture, pigment, material, color etc.
      #declare DataArray=str_colorArr    // the array containing the strings of identifiers
      #declare NumPicHorizonal=4;        // the amount of images per row in the table
      #declare NumPicVertical=4;         // the amount of images per collumn in the table
      #declare IW=image_width;           // the dimesions of the image, these are set in the ini-file!
      #declare IH=image_height;
      #declare Comment=""
      HTMLgen(FromFileName, OutName, Keyword, DataArray, NumPicHorizonal, NumPicVertical, IW, IH, Comment)
   #end
#end

#if (Generate_Images)
   camera {
     right x*image_width/image_height
     location  <0,6,0.01>
     look_at   <0,0, 0.0>
     angle 35
   }
   
   //the colour in light
   light_group {
      light_source {<-50,500,-200> rgb 1}
      union {
         plane {y, 0 no_shadow}
         difference {
            sphere{0,1}
            box{<-1,0.9,-1>,<1,1.1,1>}
         }
         translate <0.7,0,0>
         pigment{colorArr[frame_number-1]}
      }
   }
   
   //the pure colour
   box {
      -1,1
      scale <0.5,0.01,1>
      no_shadow
      translate<-1.2,0.01,0>
      pigment {colorArr[frame_number-1]}
      finish {ambient 1 diffuse 0}
   }
   
   //background plane for transparent colors   
   plane {
      y,-1
      no_shadow
      pigment{
         checker 
         color rgb 1 
         color blue 1 
         scale 0.25 
         rotate <0,45,0>
      }
      finish {ambient 1}
   }
#end
