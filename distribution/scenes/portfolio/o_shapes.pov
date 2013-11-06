// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer Scene Description File
// File:  o_shapes_.pov
// Vers: 3.5
// Desc: Render via o_shapes.ini,
//       generates html files and images for all the
//       objects in shapes.inc
// Date: 2001/08/13
// Auth: Ingo Janssen
#version 3.6;
global_settings { 
  assumed_gamma 1.0
}

#include "colors.inc"
#include "shapes.inc"
#include "functions.inc"
#include "html_gen.inc"

#declare Generate_HTML=yes;
#declare Generate_Images=yes;

// all object names extracted from shapes.inc,
// as strings for the generation of the html-files
// and as identfiers for the generation of the images.
#declare str_Arr=array[17] {
 "Bevelled_Text", "Circle_Text", "Supercone",
 "Connect_Spheres", "Wire_Box","Round_Box", "Round_Cylinder",
 "Round_Cone", "Round_Cone2", "Round_Cone3",
 "HF_Square", "HF_SPhere", "HF_Cylinder", "HF_Torus", "Wedge", "Spheroid",
 "Supertorus"
}

#if (Generate_HTML)
   #if (clock=0)                         // generate the html-files for showing the images in.
      #declare FromFileName="shapes.inc"   // the name of the include file the data came from.
      #declare OutName="o_shapes"          // the OutName should match with Output_File_Name in the ini-file!!!!
      #declare Keyword="object"           // the stuff represented in the array: texture, pigment, material, color etc.
      #declare DataArray=str_Arr         // the array containing the strings of identifiers
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
            Bevelled_Text("crystal.ttf", "ABC", 10, 35, 0.1, 0.3, 0, 0)
            scale 2
            translate <-1.4,-0.5,0>
            pigment {rgb 1}
            finish{specular 0.4}
         }
      #break
      #case(1)
         object {
            Circle_Text("crystal.ttf", "Circle Text", 1, 0, 0.3, 0.95, 0, Align_Center, 10)
            scale 0.75
            pigment {rgb <1,0.1,0>}
            finish{specular 0.4}
         }
      #break
      #case(2)
         object { 
            Supercone(<0,-1.5,0>,1,1.5, <0,1.5,0>,1,.4)
            rotate <0,33,0>
            scale 0.6
            pigment {rgb 1}
            finish{specular 0.4}
         }
      #break
      #case(3)
         object{
            Connect_Spheres(<-0.5,-0.5,0>, 0.5, <0.7,0.7,0.3>, 0.25)
            pigment {rgb 1}
            finish {specular 0.4}
         }
         sphere { <-0.5,-0.5,0>, 0.5 pigment {rgbf <1,1,0.7,0.9>}finish {specular 0.3}}
         sphere { <0.7,0.7,0.3>, 0.25 pigment {rgbf <1,1,0.7,0.9>}finish {specular 0.3}}
      #break
      #case(4)
         object {
            Wire_Box(<-1,-0.75,-0.5>, <1,0.75,3>, 0.1, 0)
            pigment {rgb 1}finish {specular 0.3}
         } 
      #break
      #case(5)
         object{
            Round_Box(<-0.75,-0.75,-0.75>, <0.75,0.75,1.5>, 0.1, 0)
            rotate <35,45,0>
            pigment {rgb 1}finish {specular 0.3}
         }
      #break
      #case(6)
          object{
            Round_Cylinder(<-0.5,-0.5,0>, <0.5,0.6,-1>,0.5, 0.1, 0)
            pigment {rgb 1}finish {specular 0.3}
          }
      #break
      #case(7)
         object {
            Round_Cone(<0,-0.5,0>, 0.7, <0.5,0.6,-0.8>, 0.3, 0.1, 0)
            pigment {rgb 1}finish {specular 0.3}
         }
      #break
      #case(8)
         object {
            Round_Cone2(<0,-0.5,0>, 0.7, <0.5,0.6,-0.8>, 0.3, 0)
            pigment {rgb 1}finish {specular 0.3}
         }
      #break
      #case(9)
         object {
            Round_Cone3(<0,-0.5,0>, 0.7, <0.5,0.6,-0.8>, 0.3, 0)
            pigment {rgb 1}finish {specular 0.3}
         }
      #break
      #case(10)
         #declare HF_Function=function {pattern{crackle scale 0.3}}
          object {
             HF_Square (HF_Function, off, off <100,100>, on, "", <0.0, 0.1, 0.0>, <3, 0.3, 3>)
             texture {
                pigment {color rgb < 1.0, 1.0, 1.0>}
                finish {specular 0.3}
             }
             rotate<-45,0,0>
             translate <-1.5,-1,0>
          }
      #break
      #case(11)
         #declare HF_Function=function {pattern{crackle scale 0.5}}
          object {
             HF_Sphere (HF_Function, off, off, <100,100>, on, "", <0.0, 0.0, 0.0>, 0.75, 0.2)
             texture {
                pigment {color rgb < 1.0, 1.0, 1.0>}
                finish {specular 0.3}
             }
          }
      #break
      #case(12)
         #declare HF_Function=function {pattern{crackle scale 0.3}}
        object {
           HF_Cylinder (HF_Function, off, off, <200,200>, on, "", <0,-1, 0>, <0, 1, 0>, 0.5, 0.3)
           texture {
              pigment {color rgb < 1.0, 1.0, 1.0>}
              finish {specular 0.3}
           }
        }
      #break 
      #case(13)
         #declare HF_Function=function {pattern{crackle scale 0.3}}
          object {
             HF_Torus (HF_Function, off, off,<100,100>, on, "", 0.75, 0.25, 0.2)
             texture {
                pigment {color rgb < 1.0, 1.0, 1.0>}
                finish {specular 0.3}
             }
             rotate<-45.0,0,0>
          }
      #break 
      #case(14)
         object {
            Wedge(45)
            rotate <60,0,30>
            pigment {rgb 1}
         }
      #break
      #case(15)
         object {
            Spheroid(0,<1,0.76,0.23>)
            rotate <32,64,24>
            pigment {rgb 1}
            finish {specular 0.4 roughness .4}
         }
      #break
      #case(16)
         object {
            Supertorus(0.6, 0.3, 0.50, 0.2, 0.0001, 0)
            rotate <-45,0,0>
            pigment {rgb 1}
            finish {specular 0.4 roughness .4}
         }
      #break
   #end
   
   camera {
     right x*image_width/image_height
     location  <0,0,-3>
     look_at   <0,0,0>
   }
   light_source {<500,500,-500> rgb 1}
   light_source {<-500,500,-500> rgb <0.1,0.1,0.3> shadowless}
   sky_sphere {
      pigment {
         planar
         color_map {  [0.0 color rgb 1][1 color <0.22,0.2,0.7>] }
      }
   }

#end

