// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer Scene Description File
// File:  o_shapesq_.pov
// Vers: 3.5
// Desc: Render via o_shapesq.ini,
//       generates html files and images for all the
//       shapes in shapesq.inc
// Date: 2001/08/06
// Auth: Ingo Janssen
#version 3.6;
global_settings { 
  assumed_gamma 1.0
}

#include "colors.inc"
#include "shapes.inc"
#include "shapesq.inc"
#include "html_gen.inc"
#declare Generate_HTML=yes;
#declare Generate_Images=yes;

// all shape names extracted from shapesq.inc,
// as strings for the generation of the html-files
// and as identfiers for the generation of the images.
#declare str_Arr=array[22] {
//poly
 "Glob_5", "Twin_Glob", "Sinsurf",
//quartic
 "Bicorn", "Crossed_Trough", "Cubic_Cylinder", "Cubic_Saddle_1",
 "Devils_Curve", "Folium", "Helix", "Hyperbolic_Torus_40_12", "Lemniscate",
 "Quartic_Loop_1", "Monkey_Saddle", "Parabolic_Torus_40_12", "Piriform", "Quartic_Paraboloid",
 "Quartic_Cylinder", "Steiner_Surface", "Torus_40_12", "Witch_Hat",
//object
 "Helix_1",
}
#declare Arr=array[22] {
//poly
 Glob_5, Twin_Glob, Sinsurf,
//quartic
 Bicorn, Crossed_Trough, Cubic_Cylinder, Cubic_Saddle_1,
 Devils_Curve, Folium, Helix, Hyperbolic_Torus_40_12, Lemniscate,
 Quartic_Loop_1, Monkey_Saddle, Parabolic_Torus_40_12, Piriform, Quartic_Paraboloid,
 Quartic_Cylinder, Steiner_Surface, Torus_40_12, Witch_Hat,
//object
 Helix_1,
}


#if (Generate_HTML)
   #if (clock=0)                         // generate the html-files for showing the images in.
      #declare FromFileName="shapesq.inc"   // the name of the include file the data came from.
      #declare OutName="o_shapesq"          // the OutName should match with Output_File_Name in the ini-file!!!!
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
   #switch(frame_number-1)
      #case(0)
         object {
            Arr[frame_number-1]
            sturm
            rotate <0,0,-90> 
            rotate <0,25,0>
            pigment {rgb <1,1,1>}
         }
      #break
      #case(1)
         object {
            Arr[frame_number-1]
            sturm
            rotate <0,25,0>
            pigment {rgb <1,1,1>}
         }
      #break
      #case(2)
         object {
            Arr[frame_number-1]
            sturm
            rotate <0,55,0>
            rotate <0,0,20>
            translate <0,-3,10>
            pigment {rgb <1,1,1>}
         }
      #break
      #case(3)
         object {
            Arr[frame_number-1]
            sturm
            translate <0,0,-2>
            pigment {rgb <1,1,1>}
         }
      #break
      #case(4)
         object {
            Arr[frame_number-1]
            sturm
            rotate<0,0,90>
            rotate<0,65,0>
            translate <0,-1,5>
            pigment {rgb <1,1,1>}
         }
      #break
      #case(5)
         object {
            Arr[frame_number-1]
            sturm
            rotate<0,0,-90>
            pigment {rgb <1,1,1>}
         }
      #break
      #case(6)
         object {
            Arr[frame_number-1]
            sturm
            rotate<90,55,0>
            translate <0,-5,20>
            pigment {rgb <1,1,1>}
         }
      #break
      #case(7)
         object {
            Arr[frame_number-1]
            sturm
            rotate<0,0,0>
            translate <0,-2.5,5>
            pigment {rgb <1,1,1>}
         }
      #break
      #case(8)
         object {
            Arr[frame_number-1]
            sturm
            rotate<0,-105,0>
            pigment {rgb <1,1,1>}
         }
      #break
      #case(9)
         object {
            Arr[frame_number-1]
            sturm
            rotate<0,33,0>
            translate <0,-1,2>
            pigment {rgb <1,1,1>}
         }
      #break
      #case(10)
         object {
            Arr[frame_number-1]
            sturm
            rotate<33,33,0>
            translate <0,-30,125>
            pigment {rgb <1,1,1>}
         }
      #break
      #case(11)
         object {
            Arr[frame_number-1]
            sturm
            rotate<33,33,0>
            translate <0,1,-2>
            pigment {rgb <1,1,1>}
         }
      #break
      #case(12)
         object {
            Arr[frame_number-1]
            sturm
            rotate<33,0,45>
            translate <-1,-1,2>
            pigment {rgb <1,1,1>}
         }
      #break
      #case(13)
         object {
            Arr[frame_number-1]
            sturm
            rotate<10,180,0>
            translate <-1,-5,20>
            pigment {rgb <1,1,1>}
         }
      #break
      #case(14)
         object {
            Arr[frame_number-1]
            sturm
            rotate<10,180,0>
            translate <-1,-20,50>
            pigment {rgb <1,1,1>}
         }
      #break
      #case(15)
         object {
            Arr[frame_number-1]
            sturm
            rotate<0,-150,0>
            translate <0.5,0.5,-2>
            pigment {rgb <1,1,1>}
         }
      #break
      #case(16)
         object {
            Arr[frame_number-1]
            sturm
            rotate<0,0,0>
            pigment {rgb <1,1,1>}
         }
      #break
      #case(17)
         object {
            Arr[frame_number-1]
            sturm
            pigment {rgb <1,1,1>}
         }
      #break
      #case(18)
         object {
            Arr[frame_number-1]
            sturm
            rotate <1,1,1>
            translate <0,1,-3>
            pigment {rgb <1,1,1>}
         }
      #break
      #case(19)
         object {
            Arr[frame_number-1]
            sturm
            rotate <-45,1,1>
            translate <0,-11,30>
            pigment {rgb <1,1,1>}
         }
      #break
      #case(20)
         object {
            Arr[frame_number-1]
            sturm
            rotate <-45,-180,1>
            pigment {rgb <1,1,1>}
         }
      #break
      #case(21)
         object {
            Arr[frame_number-1]
            sturm
            rotate <15,33,1>
            translate <-0.5,0,2>
            pigment {rgb <1,1,1>}
         }
      #break
   #end

   camera {
     right x*image_width/image_height
     location  <0,2,-6>
     look_at   <0,0,0>
     angle 35
   }
   
   light_source {<500,500,-500> rgb 1}
   light_source {<-500,500,-500> rgb <0.1,0.1,0.3> shadowless}
   light_source {<3,3,-3> rgb <0.5,0.4,0.4> shadowless}

   sky_sphere {
      pigment {
         planar
         color_map { [0.0 color blue 0.5] [1.0 color rgb <0.7,0.7,1.0>] }
      }
   }
#end

