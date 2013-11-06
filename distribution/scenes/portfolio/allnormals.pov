// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer Scene Description File
// File: allnormals_.pov
// Vers: 3.5
// Desc: Render via allnormals.ini,
//       generates html files and images for all the
//       patterns in allnormals.pov
// Date: 2001/08/08
// Auth: ingo Janssen
#version 3.6;
global_settings { 
  assumed_gamma 1.0
}

#include "colors.inc"
#include "html_gen.inc"


#declare Generate_HTML=yes;
#declare Generate_Images=yes;

#declare Agate         = normal {pigment_pattern{agate colour_map {[0, rgb 0][1, rgb 1]}}1}
#declare Average       = normal {pigment_pattern{average pigment_map{[1, gradient x sine_wave][1, gradient y sine_wave][3, bumps]}translate 0.01 scale 0.5}2}
#declare Boxed         = normal {pigment_pattern{boxed colour_map {[0, rgb 0][1, rgb 1]} translate<-0.6,0.6,0>}2}
#declare Bozo          = normal {pigment_pattern{bozo colour_map {[0, rgb 0][1, rgb 1]}scale 0.5}2}
#declare Brick         = normal {pigment_pattern{brick colour rgb 0.2, colour rgb 0.8 scale 0.05}2}
#declare Bumps         = normal {pigment_pattern{bumps colour_map {[0, rgb 0][1, rgb 1]} scale 0.5}2}
#declare Cells         = normal {pigment_pattern{cells colour_map {[0, rgb 0][1, rgb 1]} scale 0.2}2}
#declare Checker       = normal {pigment_pattern{checker color rgb 1 color rgb 0.1 scale 0.5}2}
#declare Crackle       = normal {pigment_pattern{crackle colour_map {[0, rgb 0][1, rgb 1]} scale 0.5}2}
#declare Cylindrical   = normal {pigment_pattern{cylindrical colour_map {[0, rgb 0][1, rgb 1]}scale 0.8}2}
#declare Dents         = normal {pigment_pattern{dents colour_map {[0, rgb 0][1, rgb 1]}scale 0.5}2}
#declare Mandel        = normal {pigment_pattern{mandel 50 interior 2,.5  exterior 1,.01 colour_map {[0, rgb 0][1, rgb 1]}}2}
#declare Julia         = normal {pigment_pattern{julia <0.353, 0.288>, 30 interior 1, 1 color_map {[0 rgb 0][1 rgb 1]}scale 1.2}2}
#declare Facets        = normal {facets coords 0.4}
#declare Function      = normal {pigment_pattern{function {(sin(x*x)-cos(y*y))} poly_wave 3 scale 0.6 colour_map {[0, rgb 0][1, rgb 1]}}2}
#declare Gradient      = normal {pigment_pattern{gradient x colour_map {[0, rgb 0][1, rgb 1]}scale 0.5}2}
#declare Granite       = normal {pigment_pattern{granite colour_map {[0, rgb 0][1, rgb 1]} scale 0.5}}
#declare Hexagon       = normal {pigment_pattern{hexagon color rgb 1, colour rgb 0, color rgb 0.5 rotate <90,0,0> scale 0.5}2}
#declare Image_pattern = normal {pigment_pattern{image_pattern { png "mtmandj.png" }}2}
#declare Leopard       = normal {pigment_pattern{leopard colour_map {[0, rgb 0][1, rgb 1]}scale 0.1}2}
#declare Marble        = normal {pigment_pattern{marble colour_map {[0, rgb 0][1, rgb 1]}scale 0.5}2}
#declare Onion         = normal {pigment_pattern{onion colour_map {[0, rgb 0][1, rgb 1]}scale 0.5}2}
#declare Planar        = normal {pigment_pattern{planar colour_map {[0, rgb 0][1, rgb 1]}scale 0.82}}
#declare Quilted       = normal {pigment_pattern{quilted colour_map {[0, rgb 0][1, rgb 1]}scale 0.5}2}
#declare Radial        = normal {pigment_pattern{radial frequency 6 colour_map {[0, rgb 0][1, rgb 1]}rotate<90,0,0> scale 0.5}2}
#declare Ripples       = normal {pigment_pattern{ripples colour_map {[0, rgb 0][1, rgb 1]} scale 0.5}2}
#declare Spherical     = normal {pigment_pattern{spherical colour_map {[0, rgb 0][1, rgb 1]}}2}
#declare Spiral1       = normal {pigment_pattern{spiral1 5 colour_map {[0, rgb 0][1, rgb 1]}}2}
#declare Spiral2       = normal {pigment_pattern{spiral2 5 colour_map {[0, rgb 0][1, rgb 1]}}2}
#declare Spotted       = normal {pigment_pattern{spotted colour_map {[0, rgb 0][1, rgb 1]}scale 0.5}2}
#declare Waves         = normal {pigment_pattern{waves colour_map {[0, rgb 0][1, rgb 1]}scale 0.5}2}
#declare Wood          = normal {pigment_pattern{wood colour_map {[0, rgb 0][1, rgb 1]}scale 0.5}2}
#declare Wrinkles      = normal {pigment_pattern{wrinkles colour_map {[0, rgb 0][1, rgb 1]}scale 0.5}2}

// all texture names extracted from allnormals.pov,
// as strings for the generation of the html-files
// and as identfiers for the generation of the images.
#declare str_normalArr=array[33] {
   "Agate","Average""Boxed","Bozo","Brick","Bumps"        
   "Cells","Checker","Crackle","Cylindrical",
   "Dents","Mandel", "Julia", "Facets",      
   "Function","Gradient","Granite","Hexagon","Image_pattern"
   "Leopard","Marble","Onion","Planar","Quilted"
   "Radial","Ripples","Spherical","Spiral1","Spiral2"
   "Spotted","Waves","Wood","Wrinkles"

}
#declare normalArr=array[33] {
   Agate,Average,Boxed,Bozo,Brick,Bumps        
   Cells,Checker,Crackle,Cylindrical,
   Dents,Mandel, Julia, Facets,       
   Function,Gradient,Granite,Hexagon,Image_pattern
   Leopard,Marble,Onion,Planar,Quilted      
   Radial,Ripples,Spherical,Spiral1,Spiral2      
   Spotted,Waves,Wood,Wrinkles     
}


#if (Generate_HTML)
   #if (clock=0)                         // generate the html-files for showing the images in.
      #declare FromFileName="allnormals.pov"   // the name of the include file the data came from.
      #declare OutName="allnormals"          // the OutName should match with Output_File_Name in the ini-file!!!!
      #declare Keyword="pattern as normal"   // the stuff represented in the array: texture, pigment, material, color etc.
      #declare DataArray=str_normalArr    // the array containing the strings of identifiers
      #declare NumPicHorizonal=3;        // the amount of images per row in the table
      #declare NumPicVertical=2;         // the amount of images per collumn in the table
      #declare IW=image_width;           // the dimesions of the image, these are set in the ini-file!
      #declare IH=image_height;
      #declare Comment="<p>Most of the patterns, used as a normal</p>"
      HTMLgen(FromFileName, OutName, Keyword, DataArray, NumPicHorizonal, NumPicVertical, IW, IH, Comment)
   #end
#end

#if(Generate_Images)
   camera {
     right x*image_width/image_height
     location  <0,0,-8>
     look_at   <0,-0.25, 0>
     angle 35
   }
   #declare T=texture {
      pigment {rgb 1}
      normal {normalArr[frame_number-1]}
      finish {phong 0.5 phong_size 20}
   }
   light_source{<500,50,20> rgb 1}
   light_source{<-500,500,-500> rgb <0.6,0.6,0.8>}
   box {
      -1.5,1.5
      translate<0,0,1.5>
      texture{T}
      translate<0,0,-1.5>   
      rotate <0,-35,0>
      rotate <-15,0,0>
   }
#end

