// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer Scene Description File
// File: allpatterns_.pov
// Vers: 3.5
// Desc: Render via allpatterns.ini,
//       generates html files and images for all the
//       patterns in allpatterns.pov
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

#declare Agate         = pigment {agate colour_map {[0, rgb 0][1, rgb 1]}}
#declare Average       = pigment {average pigment_map{[1, gradient x][1, gradient y][1, gradient z]}translate 0.01 scale 0.5}
#declare Boxed         = pigment {boxed colour_map {[0, rgb 0][1, rgb 1]} translate<-0.6,0.6,0>}
#declare Bozo          = pigment {bozo colour_map {[0, rgb 0][1, rgb 1]}scale 0.5}
#declare Brick         = pigment {brick colour rgb 0.8, colour rgb 0.2 scale 0.05}
#declare Bumps         = pigment {bumps colour_map {[0, rgb 0][1, rgb 1]} scale 0.5}
#declare Cells         = pigment {cells colour_map {[0, rgb 0][1, rgb 1]} scale 0.2}
#declare Checker       = pigment {checker color rgb 1 color rgb 0.1 scale 0.5}
#declare Crackle       = pigment {crackle colour_map {[0, rgb 0][1, rgb 1]} scale 0.5}
#declare Cylindrical   = pigment {cylindrical colour_map {[0, rgb 0][1, rgb 1]}scale 0.8}
#declare Dents         = pigment {dents colour_map {[0, rgb 0][1, rgb 1]}scale 0.5}
#declare Mandel        = pigment {mandel 50 interior 2,.5  exterior 1,.01 colour_map {[0, rgb 0][1, rgb 1]}}
#declare Julia         = pigment {julia <0.353, 0.288>, 30 interior 1, 1 color_map {[0 rgb 0][1 rgb 1]}scale 1.2}
#declare Function      = pigment {function {(sin(x*x)-cos(y*y))} scale 0.6 colour_map {[0, rgb 0][1, rgb 1]}}  
#declare Gradient      = pigment {gradient x colour_map {[0, rgb 0][1, rgb 1]}scale 0.5}
#declare Granite       = pigment {granite colour_map {[0, rgb 0][1, rgb 1]} scale 0.5}
#declare Hexagon       = pigment {hexagon color rgb 1, colour rgb 0, color rgb 0.5 rotate <90,0,0> scale 0.5}
#declare Image_pattern = pigment {image_pattern { png "mtmandj.png" }}
#declare Leopard       = pigment {leopard colour_map {[0, rgb 0][1, rgb 1]}scale 0.1}
#declare Marble        = pigment {marble colour_map {[0, rgb 0][1, rgb 1]}scale 0.5}
#declare Onion         = pigment {onion colour_map {[0, rgb 0][1, rgb 1]}scale 0.5}
#declare Planar        = pigment {planar colour_map {[0, rgb 0][1, rgb 1]}scale 0.8}
#declare Quilted       = pigment {quilted colour_map {[0, rgb 0][1, rgb 1]}scale 0.5}
#declare Radial        = pigment {radial frequency 6 colour_map {[0, rgb 0][1, rgb 1]}rotate<90,0,0> scale 0.5}
#declare Ripples       = pigment {ripples colour_map {[0, rgb 0][1, rgb 1]} scale 0.5}
#declare Spherical     = pigment {spherical colour_map {[0, rgb 0][1, rgb 1]}}
#declare Spiral1       = pigment {spiral1 5 colour_map {[0, rgb 0][1, rgb 1]}}
#declare Spiral2       = pigment {spiral2 5 colour_map {[0, rgb 0][1, rgb 1]}}
#declare Spotted       = pigment {spotted colour_map {[0, rgb 0][1, rgb 1]}scale 0.5}
#declare Waves         = pigment {waves colour_map {[0, rgb 0][1, rgb 1]}scale 0.5}
#declare Wood          = pigment {wood colour_map {[0, rgb 0][1, rgb 1]}scale 0.5}
#declare Wrinkles      = pigment {wrinkles colour_map {[0, rgb 0][1, rgb 1]}scale 0.5}

// all texture names extracted from allpatterns.pov,
// as strings for the generation of the html-files
// and as identfiers for the generation of the images.
#declare str_patternArr=array[32] {
   "Agate","Average","Boxed","Bozo","Brick","Bumps"        
   "Cells","Checker","Crackle","Cylindrical",
   "Dents","Mandel", "Julia",       
   "Function","Gradient","Granite","Hexagon","Image_pattern"
   "Leopard","Marble","Onion","Planar","Quilted"
   "Radial","Ripples","Spherical","Spiral1","Spiral2"
   "Spotted","Waves","Wood","Wrinkles"

}
#declare patternArr=array[32] {
   Agate,Average,Boxed,Bozo,Brick,Bumps        
   Cells,Checker,Crackle,Cylindrical,
   Dents,Mandel, Julia,       
   Function,Gradient,Granite,Hexagon,Image_pattern
   Leopard,Marble,Onion,Planar,Quilted      
   Radial,Ripples,Spherical,Spiral1,Spiral2      
   Spotted,Waves,Wood,Wrinkles     
}


#if (Generate_HTML)
   #if (clock=0)                         // generate the html-files for showing the images in.
      #declare FromFileName="allpatterns.pov"   // the name of the include file the data came from.
      #declare OutName="allpatterns"          // the OutName should match with Output_File_Name in the ini-file!!!!
      #declare Keyword="pattern"           // the stuff represented in the array: texture, pigment, material, color etc.
      #declare DataArray=str_patternArr    // the array containing the strings of identifiers
      #declare NumPicHorizonal=3;        // the amount of images per row in the table
      #declare NumPicVertical=2;         // the amount of images per collumn in the table
      #declare IW=image_width;           // the dimesions of the image, these are set in the ini-file!
      #declare IH=image_height;
      #declare Comment="<p>The patterns, 'boxed', 'cylindrical' and 'spherical' are shown as emitting media</p> "
      HTMLgen(FromFileName, OutName, Keyword, DataArray, NumPicHorizonal, NumPicVertical, IW, IH, Comment)
   #end
#end

#if(Generate_Images)
   #switch(frame_number-1)
      #case(2)//2 boxed
         box {
            -1.5,1.5
            hollow
            material {
               texture {
                  pigment {rgbf 1}
                  finish {
                     ambient 0 
                     diffuse 0
                  }
               }
               interior {
                  media {
                     emission 0.3
                     samples 10,10 
                     intervals 10
                     method 2
                     density {
                        boxed
                        color_map {
                           [0, rgb 0]
                           [0, rgb <0.5,0.5,0.4>]
                           [1, rgb <1,0,0>]
                        }
                     }
                  }
               }
            }
            rotate <0,-35,0>
            rotate <-25,0,0>
            translate<0,0,-1.5>   
         }
      #break
      #case(9)//9 cylindrical
         box {
            -1.5,1.5
            hollow
            material {
               texture {
                  pigment {rgbf 1}
                  finish {
                     ambient 0 
                     diffuse 0
                  }
               }
               interior {
                  media {
                     emission 0.3
                     samples 10,10 
                     intervals 10
                     method 2
                     density {
                        cylindrical
                        color_map {
                           [0, rgb 0]
                           [0, rgb <0.5,0.5,0.4>]
                           [1, rgb <1,0,0>]
                        }
                     }
                  }
               }
            }
            rotate <0,-35,0>
            rotate <-25,0,0>
            translate<0,0,-1.5>   
         }
      #break
      #case(25)//25 spherical
         box {
            -1.5,1.5
            hollow
            material {
               texture {
                  pigment {rgbf 1}
                  finish {
                     ambient 0 
                     diffuse 0
                  }
               }
               interior {
                  media {
                     emission 0.3
                     samples 10,10 
                     intervals 10
                     method 2
                     density {
                        spherical
                        color_map {
                           [0, rgb 0]
                           [0, rgb <0.5,0.5,0.4>]
                           [1, rgb <1,0,0>]
                        }
                     }
                  }
               }
            }
            rotate <0,-35,0>
            rotate <-25,0,0>
            translate<0,0,-1.5>   
         }
      #break
   #else
      #declare T=texture {
         pigment {patternArr[frame_number-1]}
      }
      box {
         -1.5,1.5
         translate<0,0,1.5>
         texture{T}
         translate<0,0,-1.5>   
         rotate <0,-35,0>
         rotate <-25,0,0>
      }
   #end
   
   camera {
     right x*image_width/image_height
     location  <0,0,-8>
     look_at   <0,-0.25, 0>
     angle 35
   }
   light_source{<500,10,-500> rgb 1}
   light_source{<-500,500,-500> rgb <0.6,0.6,0.8>}
#end

