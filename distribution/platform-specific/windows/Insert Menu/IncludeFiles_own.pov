// Insert menu illustration scene "IncludeFiles_own.pov"
// Author: Friedrich A. Lohmueller, June-2012
#version 3.7;

  // #declare Typ = 4; // for tests


#if( Typ != 2 )  
global_settings{ assumed_gamma 1.0 } 
#end 

#default{ finish{ ambient 0.1 diffuse 0.9 }} 
#include "colors.inc"
#include "textures.inc"
//------------------------------------
#declare In_Path  = "A0 - Include files/"   
#declare Sub_Path = "20 - own include files/"


#switch (Typ)  //----------------------------------------------------------
// In_Path 
#case(1)  #declare Txt_Path="50 - Include_File Chair.txt" #break 
#case(2)  #declare Txt_Path="50 - Include_File_Chair_Use.txt" #break

#case(3)  #declare Txt_Path="20 - Include_File Socket.txt" #break 
#case(4)  #declare Txt_Path="30 - Include_File_Socket_Use.txt" #break


#case(10)  #declare Txt_Path="'chars.inc'.txt" #break
#case(11)  #declare Txt_Path="'logo.inc'.txt" #break
//#declare Typ=7; 'chars'.txt 


#end // of '#switch (Typ)' ------------------------------------------------
//-------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
#macro Base_Scene_1 ()

 
//------------------------------------------------------------------------
// sun -------------------------------------------------------------------
light_source{<500,2500,-2500> color White}
// sky -------------------------------------------------------------------
sky_sphere{ pigment{ gradient <0,1,0>
                     color_map{ [0   color rgb<1,1,1>         ]//White
                                [0.4 color rgb<0.14,0.14,0.56>]//~Navy
                                [0.6 color rgb<0.14,0.14,0.56>]//~Navy
                                [1.0 color rgb<1,1,1>         ]//White
                              }
                     scale 2 }
           } // end of sky_sphere 
//------------------------------------------------------------------------
// ground -----------------------------------------------------------------
//---------------------------------<<< settings of squared plane dimensions
#declare RasterScale = 0.3;
#declare RasterHalfLine  = 0.035;  
#declare RasterHalfLineZ = 0.035; 
//-------------------------------------------------------------------------
#macro Raster(RScale, HLine) 
       pigment{ gradient x scale RScale
                color_map{[0.000   color rgbt<1,1,1,0>*0.3]
                          [0+HLine color rgbt<1,1,1,0>*0.3]
                          [0+HLine color rgbt<1,1,1,1>]
                          [1-HLine color rgbt<1,1,1,1>]
                          [1-HLine color rgbt<1,1,1,0>*0.3]
                          [1.000   color rgbt<1,1,1,0>*0.3]} }
 #end// of Raster(RScale, HLine)-macro    
//-------------------------------------------------------------------------
box { <-2,-0.1,-1.5>,<1.5,0,2>     // plane with layered textures
        texture { pigment{color White*1.1}}
        texture { Raster(RasterScale,RasterHalfLine ) rotate<0,0,0> }
        texture { Raster(RasterScale,RasterHalfLineZ) rotate<0,90,0>}
        rotate<0,0,0>
      }
//------------------------------------------------ end of squared plane XZ
#end // "Base_Scene_1 ()"
//------------------------------------------------------------------------- 
//------------------------------------------------------------------------- 
//------------------------------------------------------------------------- 
//------------------------------------------------------------------------- 
//----------
#if (Typ=1) // 
Base_Scene_1 ()
camera { angle 12   // diagonal view
         location  <5.0 , 3.0 ,-5.0>
         right     x*image_width/image_height
         look_at   <0.0 , 0.45 , 0.0>}

#include concat(In_Path,Sub_Path,Txt_Path) // "50 - Include_File Chair.txt")
      //Chair( seat h, chair width, backrest h, feet d)
object{ Chair( 0.45, 0.45, 0.90, 0.04 )
        rotate<0,0,0>
        translate<0,0,0>
      }  
#end
//----------
#if (Typ=2) // 
/*
//Base_Scene_1 ()
camera { angle 10   // diagonal view
         location  <10.0 , 10.0 ,-10.0>
         right     x*image_width/image_height
         look_at   <0.0 , 0.5 , 0.0>}
*/
#include concat(In_Path,Sub_Path,Txt_Path) //"50 - Include_File Chair.txt")
      //Chair( seat h, chair width, backrest h, feet d)

object{ Chair( 0.45, 0.45, 0.90, 0.04 )
        rotate<0,-70,0>
        translate<-1,0, 0.1>
      }  
object{ Chair( 0.45, 0.45, 0.90, 0.04 )
        rotate<0,-20,0>
        translate<-0.5,0, 0.8>
      }  
object{ Chair( 0.45, 0.45, 0.90, 0.04 )
        rotate<0, 20,0>
        translate<0.75,0, 0.2>
      }  

object{ Chair( 0.45, 0.45, 0.90, 0.04 )
        rotate<0, -160,0>
        translate<-0.45,0,-1.0>
      }  

 
#end
//----------

#if (Typ=3) // 
Base_Scene_1 ()
camera { angle 12   // diagonal view
         location  <5.0 , 3.0 ,-5.0>
         right     x*image_width/image_height
         look_at   <0.0 , 0.45 , 0.0>}

#include concat(In_Path,Sub_Path,Txt_Path) // "50 - Include_File Chair.txt")
      //Chair( seat h, chair width, backrest h, feet d)
object{ Socket translate<0,0.5,0>
        scale 0.9
        rotate<0,0,0>
        
      }  
#end
//----------

#if (Typ=4) // 
Base_Scene_1 ()
camera { angle 20   // diagonal view
         location  <5.0 , 3.0 ,-5.0>
         right     x*image_width/image_height
         look_at   <0.1 , 0.45 , 0.0>}

#declare Socket_Tex_1 =  // sphere
//texture{ pigment{color rgb<1,0,0>}
texture{ Chrome_Metal  
         normal { bumps 0.85 scale 0.01 }
         finish { phong 0.5 } 
       }
#declare Socket_Tex_2 =  // box 
//texture{ pigment{color rgb<1,0.65,0>}
texture{ pigment{color rgb<1,1,1>*0.7}
         finish { phong 0.5 } 
       }
#declare Socket_Tex_3 =  // cylinders
// texture{ pigment{color rgb<0.5,1,0> }
texture{ Chrome_Metal
         finish { phong 1 } 
       }
#include concat(In_Path,Sub_Path,"20 - Include_File Socket.txt") //

object{ Socket  translate< 0,0.50, 0.00>
        scale <1,1,1>*1
        rotate<0,0,0>
        translate<0.750,0, 0.00>
      } //------------------------

// re-declared textures 
#declare Socket_Tex_1 =  // sphere
texture{ pigment{color rgb<1,0,0>}
         finish { phong 0.5 } 
       }
#declare Socket_Tex_2 =  // box 
texture{ pigment{color rgb<1,0.65,0>}
         finish { phong 0.5 } 
       }
#declare Socket_Tex_3 =  // cylinders
texture{ pigment{color rgb<0.5,1,0> }
         finish { phong 1 } 
       }
// loading object definition again ----------------------------------------------------//
#include concat(In_Path,Sub_Path,"20 - Include_File Socket.txt") //
object{ Socket  translate<0,0.50, 0.00>
        scale <1,1,1>*1
        rotate<0,0,0>
        translate< -0.750,0, 0.00>
      } //------------------------

#end
//----------





#if (Typ=10) // 
#include concat(In_Path,Txt_Path) //"50 - Include_File Chair.txt")
#end
//----------
#if (Typ=11) // 
#include concat(In_Path,Txt_Path) //"50 - Include_File Chair.txt")
#end
//----------


//---------- End Include file samples