// Insert menu illustration scene "Woods.pov"
// Author: Friedrich A. Lohmueller, March-2013
#version 3.7;
global_settings{ assumed_gamma 1.0 } 
//--------------------------------------------------------------------------

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"
#include "glass.inc"
//------------------------------------

#declare In_Path1  = "E0 - Textures and Materials/25 - Wood samples/"   

  //    #declare Typ = 68 ; // for tests

#switch (Typ)  //----------------------------------------------------------
// In_Path1
#case(1)  #declare Txt_Path="01 - Cherry_Wood.txt" #break 
#case(2)  #declare Txt_Path="02 - Pine_Wood.txt" #break 
#case(3)  #declare Txt_Path="03 - Dark_Wood.txt" #break 
#case(4)  #declare Txt_Path="04 - Tan_Wood.txt" #break 
#case(5)  #declare Txt_Path="05 - White_Wood.txt" #break 
#case(6)  #declare Txt_Path="06 - Tom_Wood.txt" #break 
#case(7)  #declare Txt_Path="07 - Yellow_Pine.txt" #break 
#case(8)  #declare Txt_Path="08 - Rosewood.txt" #break 
#case(9)  #declare Txt_Path="09 - Sandalwood.txt" #break 


#case(10)  #declare Txt_Path="10 - EMBWood1.txt" #break 
#case(11)  #declare Txt_Path="11 - DMFWood1.txt" #break
#case(12)  #declare Txt_Path="11 - DMFWood2.txt" #break
#case(13)  #declare Txt_Path="11 - DMFWood3.txt" #break
#case(14)  #declare Txt_Path="11 - DMFWood4.txt" #break
#case(15)  #declare Txt_Path="11 - DMFWood5.txt" #break
#case(16)  #declare Txt_Path="11 - DMFWood6.txt" #break
#case(17)  #declare Txt_Path="12 - DMFDarkOak.txt" #break
#case(18)  #declare Txt_Path="12 - DMFLightOak.txt" #break
#case(19)  #declare Txt_Path="22 - Cork.txt" #break

#case(20)  #declare Txt_Path="22 - Cork bumps.txt" #break
/*
#case(25)  #declare Txt_Path=".txt" #break
*/
#case(31)  #declare Txt_Path="30 - PineWood1.txt" #break
#case(32)  #declare Txt_Path="31 - PineWood2.txt" #break
#case(33)  #declare Txt_Path="32 - PineWood3.txt" #break
#case(34)  #declare Txt_Path="33 - PineWood4.txt" #break

#case(35)  #declare Txt_Path="35 - T_Wood1.txt" #break
#case(36)  #declare Txt_Path="35 - T_Wood2.txt" #break
#case(37)  #declare Txt_Path="35 - T_Wood3.txt" #break
#case(38)  #declare Txt_Path="35 - T_Wood4.txt" #break
#case(39)  #declare Txt_Path="35 - T_Wood5.txt" #break

#case(40)  #declare Txt_Path="35 - T_Wood6.txt" #break
#case(41)  #declare Txt_Path="35 - T_Wood7.txt" #break
#case(42)  #declare Txt_Path="35 - T_Wood8.txt" #break
#case(43)  #declare Txt_Path="35 - T_Wood9.txt" #break
#case(44)  #declare Txt_Path="36 - T_Wood10.txt" #break
#case(45)  #declare Txt_Path="36 - T_Wood11.txt" #break
#case(46)  #declare Txt_Path="36 - T_Wood12.txt" #break
#case(47)  #declare Txt_Path="36 - T_Wood13.txt" #break
#case(48)  #declare Txt_Path="36 - T_Wood14.txt" #break
#case(49)  #declare Txt_Path="36 - T_Wood15.txt" #break

#case(50)  #declare Txt_Path="36 - T_Wood16.txt" #break
#case(51)  #declare Txt_Path="36 - T_Wood17.txt" #break
#case(52)  #declare Txt_Path="36 - T_Wood18.txt" #break
#case(53)  #declare Txt_Path="36 - T_Wood19.txt" #break
#case(54)  #declare Txt_Path="36 - T_Wood20.txt" #break
#case(55)  #declare Txt_Path="36 - T_Wood21.txt" #break
#case(56)  #declare Txt_Path="36 - T_Wood22.txt" #break
#case(57)  #declare Txt_Path="36 - T_Wood23.txt" #break
#case(58)  #declare Txt_Path="36 - T_Wood24.txt" #break
#case(59)  #declare Txt_Path="36 - T_Wood25.txt" #break

#case(60)  #declare Txt_Path="36 - T_Wood26.txt" #break
#case(61)  #declare Txt_Path="36 - T_Wood27.txt" #break
#case(62)  #declare Txt_Path="36 - T_Wood28.txt" #break
#case(63)  #declare Txt_Path="36 - T_Wood29.txt" #break
#case(64)  #declare Txt_Path="36 - T_Wood30.txt" #break
#case(65)  #declare Txt_Path="36 - T_Wood31.txt" #break
#case(66)  #declare Txt_Path="36 - T_Wood32.txt" #break
#case(67)  #declare Txt_Path="36 - T_Wood33.txt" #break
#case(68)  #declare Txt_Path="36 - T_Wood34.txt" #break
#case(69)  #declare Txt_Path="36 - T_Wood35.txt" #break


#end // of '#switch (Typ)' ------------------------------------------------
//-------------------------------------------------------------------------
#macro Base_Shape1( Tex ) // left box, right sphere
union{ 
 box{<-0.3,-1.9/2,0>,<0.3,1.9/2,7> rotate<0, 0,0> texture{Tex} translate<-0.95 ,0.1+1.9/2,-0.8> } 
 sphere{<0,0,0>,1.05 texture{Tex} translate<0.65,1.2,0> } 
 translate<0,0,-0.35>
 }//  
#end // macro
//------------------------------------------------------------------------- 
//------------------------------------------------------------------------- 
//------------------------------------------------------------------------- 
#if ( (Typ >=1) & (Typ <=70)) // 

#default{ finish{ ambient 0.1 diffuse 0.9 }} 
#include "woods.inc"

camera { angle 75      // front view
         location  <0.0 , 1.0 ,-3.0>
         right     x*image_width/image_height
         look_at   <0.0 , 1.0 , 0.0>}
light_source{< 1500,2000,-2500> color White}
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

#if ( ((Typ>=1) & (Typ<=20)) | ((Typ>=31) & (Typ<=69))       ) //  
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
//------------------------------------------------------------------------- 




