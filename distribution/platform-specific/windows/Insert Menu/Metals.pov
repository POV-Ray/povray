// Insert menu illustration scene "Metals.pov"
// Author: Friedrich A. Lohmueller, March-2013
#version 3.7;
global_settings{ assumed_gamma 1.0 } 
//--------------------------------------------------------------------------

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"
#include "glass.inc"
//------------------------------------

#declare In_Path1  = "E0 - Textures and Materials/35 - Metals/"   

     //  #declare Typ =  35  ; // for tests

#switch (Typ)  //----------------------------------------------------------
// In_Path1
#case(1)  #declare Txt_Path="01 - Rust.txt" #break 
#case(2)  #declare Txt_Path="02 - Rusty_Iron.txt" #break
#case(3)  #declare Txt_Path="03 - Brass_Valley.txt" #break
#case(4)  #declare Txt_Path="04 - Silver1.txt" #break
#case(5)  #declare Txt_Path="05 - Silver2.txt" #break
#case(6)  #declare Txt_Path="06 - Gold_Metal.txt" #break
#case(7)  #declare Txt_Path="07 - Copper_Metal.txt" #break
#case(8)  #declare Txt_Path="08 - New_Brass.txt" #break
#case(9)  #declare Txt_Path="09 - Polished_Brass.txt" #break

#case(10)  #declare Txt_Path="10 - Brass_Metal.txt" #break
#case(11)  #declare Txt_Path="11 - Bronze_Metal.txt" #break
#case(12)  #declare Txt_Path="12 - Spun_Brass.txt" #break
#case(13)  #declare Txt_Path="13 - Brass_Valley.txt" #break
#case(14)  #declare Txt_Path="14 - Brushed_Aluminum.txt" #break
#case(15)  #declare Txt_Path="15 - Silver_Metal.txt" #break
#case(16)  #declare Txt_Path="16 - Chrome_Metal.txt" #break
#case(17)  #declare Txt_Path="17 - Polished_Chrome.txt" #break
#case(18)  #declare Txt_Path="18 - Polished_Chrome red.txt" #break
#case(19)  #declare Txt_Path="19 - Polished_Chrome violet.txt" #break

#case(20)  #declare Txt_Path="32 - T_Copper_1A.txt" #break
#case(21)  #declare Txt_Path="32 - T_Copper_2A.txt" #break
#case(22)  #declare Txt_Path="32 - T_Copper_3A.txt" #break
#case(23)  #declare Txt_Path="32 - T_Copper_4A.txt" #break
#case(24)  #declare Txt_Path="32 - T_Copper_5A.txt" #break
#case(25)  #declare Txt_Path="32 - T_Copper_5C.txt" #break
#case(26)  #declare Txt_Path="32 - T_Copper_5E.txt" #break

#case(27)  #declare Txt_Path="34 - T_Silver_1A.txt" #break
#case(28)  #declare Txt_Path="34 - T_Silver_3A.txt" #break
#case(29)  #declare Txt_Path="34 - T_Silver_5A.txt" #break

#case(30)  #declare Txt_Path="36 - T_Chrome_1A.txt" #break
#case(31)  #declare Txt_Path="36 - T_Chrome_3A.txt" #break
#case(32)  #declare Txt_Path="36 - T_Chrome_5A.txt" #break
#case(33)  #declare Txt_Path="36 - T_Chrome_5C.txt" #break
#case(34)  #declare Txt_Path="36 - T_Chrome_5E.txt" #break

#case(35)  #declare Txt_Path="37 - T_Gold_1A.txt" #break
#case(36)  #declare Txt_Path="37 - T_Gold_1C.txt" #break
#case(37)  #declare Txt_Path="37 - T_Gold_1E.txt" #break
#case(38)  #declare Txt_Path="37 - T_Gold_2A.txt" #break
#case(39)  #declare Txt_Path="37 - T_Gold_3A.txt" #break
#case(40)  #declare Txt_Path="37 - T_Gold_4A.txt" #break
#case(41)  #declare Txt_Path="37 - T_Gold_5A.txt" #break
#case(42)  #declare Txt_Path="37 - T_Gold_5C.txt" #break
#case(43)  #declare Txt_Path="37 - T_Gold_5E.txt" #break







//#case(50)  #declare Txt_Path=".txt" #break



#end // of '#switch (Typ)' ------------------------------------------------
//-------------------------------------------------------------------------
#macro Base_Shape1( Tex ) // left box, right sphere
union{ 
  object{ Round_Box( <-0.5,-0.5,-0.5>,<0.5,0.5,0.5>, 0.1, 0) texture{Tex}
          scale 1.1 rotate<0,10,0> translate<-0.4 ,0.9,-0.8> } 
// box{<-0.3,-1.9/2,0>,<0.3,1.9/2,7> rotate<0, 0,0> texture{Tex} translate<-0.95 ,0.1+1.9/2,-0.8> } 
 sphere{<0,0,0>,1.1 texture{Tex} translate<0.55,1.2,0> } 
 translate<0,0,-0.35>
 }//  
#end // macro
//------------------------------------------------------------------------- 
//------------------------------------------------------------------------- 
//------------------------------------------------------------------------- 
#if ( (Typ >=1) & (Typ <=43)) // 

#default{ finish{ ambient 0.1 diffuse 0.9 }} 
#include "metals.inc"

camera { angle 75      // front view
         location  <0.0 , 1.0 ,-3.0>
         right     x*image_width/image_height
         look_at   <0.0 , 1.0 , 0.0>}
light_source{< 2500,2000,-2500> color White}
// sky 
plane{<0,1,0>,1 hollow  
       texture{ pigment{ bozo turbulence 0.92
                         color_map { [0.00 rgb <0.20, 0.20, 1.0>*0.9]
                                     [0.50 rgb <0.20, 0.20, 1.0>*0.9]
                                     [0.70 rgb <1,1,1>]
                                     [0.85 rgb <0.25,0.25,0.25>]
                                     [1.0 rgb <0.5,0.5,0.5>]}
                        scale<1,1,1.5>*2.5  translate< 0,0,0>
                       }
                finish {ambient 1 diffuse 0} }      
       scale 10000}
// fog on the ground ------------------------- 
fog { fog_type   2
      distance   50
      color      White  
      fog_offset 0.1
      fog_alt    1.5
      turbulence 1.8
    }
// ground ------------------------------------ 
plane { <0,1,0>, 0 
        texture{ pigment{ color rgb<0.35,0.65,0.0>*0.72 }
	         normal { bumps 0.75 scale 0.015 }
                 finish { phong 0.1 }
               } // end of texture
      } // end of plane
//plane{ <0,1,0>, 0.001 texture{ pigment{  color rgb<0.55,0.45,0.43>*0.3} finish { phong 0.1} } }
#end //--------------------------------------------------------------------
//------------------------------------------------------------------------- 

#if ( (Typ>=1) & (Typ<=43) ) //  
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
//------------------------------------------------------------------------- 




