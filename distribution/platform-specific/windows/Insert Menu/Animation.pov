// Insert menu illustration scene "Animation.pov"
// Author: Friedrich A. Lohmueller, June 2012
#version 3.7;
#if (( Typ != 10 ) & (Typ != 20))
global_settings{ assumed_gamma 1.0 } 
#end 

#default{ finish{ ambient 0.1 diffuse 0.9 }} 
#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"

#declare In_Path  = "C0 - Animation/"   

   //   #declare Typ =21; // for tests

#switch (Typ)  //----------------------------------------------------------

#case(10)  #declare Txt_Path="10 - animation1 ini_file.txt" #break
#case(11)  #declare Txt_Path="11 - animation1 scene_file.txt" #break

#case(20)  #declare Txt_Path="20 - animation2 ini_file.txt" #break
#case(21)  #declare Txt_Path="21 - animation2 scene_file.txt" #break

#end // of '#switch (Typ)' ------------------------------------------------
//-------------------------------------------------------------------------



//------------------------------------------------------------------------- 
//----------
#if (Typ=10) // In_Path,"10 - animation1 ini_file.txt"
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
//----
 #for (Nr, 0, 1, 1/20) 
  sphere{ <0,0,0>, 0.15 
          texture { pigment{ color rgb<1,0,0> } finish{ phong 1}}  
          translate<1,0.5,0>
          rotate<0,360*(Nr),0>    
        } 
 #end // --- 
//#include concat(In_Path,Sub_Path4,Txt_Path) 
#end
//----------
#if (Typ=11) // In_Path,"11 - animation1 scene_file.txt"
 #include concat(In_Path,Txt_Path) 
#end
//----------
//---------- 
#if (Typ=20)  //  In_Path,"20 - animation2 ini_file.txt"
 #include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
 //--------
 #declare Spline_1 =  
  spline {
    natural_spline
    
   -0.25, < 0, 2.1, 0.5>,  
   -0.15, <-1, 0.5, 0.0>,  
   
    0.00, <-1, 0.1,-1.0>,  
    0.25, < 0, 0.1,-0.5>,
    0.35, < 1, 0.1,-1.0>,
    0.50, < 1, 0.5, 1.0>,
    0.75, < 0, 2.1, 0.5>,
    0.85, <-1, 0.5, 0.0>,
    1.00, <-1, 0.1,-1.0>   
   
    1.25, < 0, 0.1, 0.0>   
    1.35, < 1, 0.1,-1.0>, 
  } //------------------- 
 #for (Nr, 0, 1, 1/30) 
  sphere{ <0,0,0>, 0.15 
          texture { pigment{ color rgb<1,0,0> } finish{ phong 1}}  
          translate Spline_1(Nr+0.25)+<0,0.15,0>
        } 
 #end // ------- 
#end
//----------
//----------
#if (Typ=21) // In_Path,"21 - animation2 scene_file."
 #include concat(In_Path,Txt_Path) 
#end
//----------
//----------

//---------- End Animation