// Insert menu illustration scene "Stones_and_Granites.pov"
// Author: Friedrich A. Lohmueller, March-2013
#version 3.7;
global_settings{ assumed_gamma 1.0 } 
//--------------------------------------------------------------------------

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"
#include "glass.inc"
//------------------------------------

#declare In_Path1  = "E0 - Textures and Materials/30 - Stones and Granites/"   

 // #declare Typ =  52  ; // for tests

#switch (Typ)  //----------------------------------------------------------
// In_Path1
#case(1)  #declare Txt_Path="05 - Pink_Granite.txt" #break 
#case(2)  #declare Txt_Path="05 - PinkAlabaster.txt" #break
#case(3)  #declare Txt_Path="22 - Blood_Marble.txt" #break
#case(4)  #declare Txt_Path="22 - Blue_Agate.txt" #break
#case(5)  #declare Txt_Path="22 - Brown_Agate.txt" #break
#case(6)  #declare Txt_Path="22 - Jade.txt" #break
#case(7)  #declare Txt_Path="22 - Red_Marble.txt" #break
#case(8)  #declare Txt_Path="22 - Sapphire_Agate.txt" #break
#case(9)  #declare Txt_Path="22 - White_Marble.txt" #break


// #case(10)  #declare Txt_Path=".txt" #break
#case(11)  #declare Txt_Path="40 - T_Grnt1.txt" #break
#case(12)  #declare Txt_Path="40 - T_Grnt2.txt" #break
#case(13)  #declare Txt_Path="40 - T_Grnt3.txt" #break
#case(14)  #declare Txt_Path="40 - T_Grnt4.txt" #break
#case(15)  #declare Txt_Path="40 - T_Grnt5.txt" #break
#case(16)  #declare Txt_Path="40 - T_Grnt6.txt" #break
#case(17)  #declare Txt_Path="40 - T_Grnt7.txt" #break
#case(18)  #declare Txt_Path="40 - T_Grnt8.txt" #break
#case(19)  #declare Txt_Path="40 - T_Grnt9.txt" #break

#case(20)  #declare Txt_Path="41 - T_Grnt10.txt" #break
#case(21)  #declare Txt_Path="41 - T_Grnt11.txt" #break
#case(22)  #declare Txt_Path="41 - T_Grnt12.txt" #break
#case(23)  #declare Txt_Path="41 - T_Grnt13.txt" #break
#case(24)  #declare Txt_Path="41 - T_Grnt14.txt" #break
#case(25)  #declare Txt_Path="41 - T_Grnt15.txt" #break
#case(26)  #declare Txt_Path="41 - T_Grnt16.txt" #break
#case(27)  #declare Txt_Path="41 - T_Grnt17.txt" #break
#case(28)  #declare Txt_Path="41 - T_Grnt18.txt" #break
#case(29)  #declare Txt_Path="41 - T_Grnt19.txt" #break

#case(30)  #declare Txt_Path="41 - T_Grnt20.txt" #break
#case(31)  #declare Txt_Path="41 - T_Grnt21.txt" #break
#case(32)  #declare Txt_Path="41 - T_Grnt22.txt" #break
#case(33)  #declare Txt_Path="41 - T_Grnt23.txt" #break
#case(34)  #declare Txt_Path="41 - T_Grnt24.txt" #break
#case(35)  #declare Txt_Path="41 - T_Grnt25.txt" #break
#case(36)  #declare Txt_Path="41 - T_Grnt26.txt" #break
#case(37)  #declare Txt_Path="41 - T_Grnt27.txt" #break
#case(38)  #declare Txt_Path="41 - T_Grnt28.txt" #break
#case(39)  #declare Txt_Path="41 - T_Grnt29.txt" #break

//#case(40)  #declare Txt_Path="50 - T_Stone1.txt" #break
#case(41)  #declare Txt_Path="50 - T_Stone1.txt" #break
#case(42)  #declare Txt_Path="50 - T_Stone2.txt" #break
#case(43)  #declare Txt_Path="50 - T_Stone3.txt" #break
#case(44)  #declare Txt_Path="50 - T_Stone4.txt" #break
#case(45)  #declare Txt_Path="50 - T_Stone5.txt" #break
#case(46)  #declare Txt_Path="50 - T_Stone6.txt" #break
#case(47)  #declare Txt_Path="50 - T_Stone7.txt" #break
#case(48)  #declare Txt_Path="50 - T_Stone8.txt" #break
#case(49)  #declare Txt_Path="50 - T_Stone9.txt" #break

#case(50)  #declare Txt_Path="51 - T_Stone10.txt" #break
#case(51)  #declare Txt_Path="51 - T_Stone11.txt" #break
#case(52)  #declare Txt_Path="51 - T_Stone12.txt" #break
#case(53)  #declare Txt_Path="51 - T_Stone13.txt" #break
#case(54)  #declare Txt_Path="51 - T_Stone14.txt" #break
#case(55)  #declare Txt_Path="51 - T_Stone15.txt" #break
#case(56)  #declare Txt_Path="51 - T_Stone16.txt" #break
#case(57)  #declare Txt_Path="51 - T_Stone17.txt" #break
#case(58)  #declare Txt_Path="51 - T_Stone18.txt" #break
#case(59)  #declare Txt_Path="51 - T_Stone19.txt" #break

#case(60)  #declare Txt_Path="51 - T_Stone20.txt" #break
#case(61)  #declare Txt_Path="51 - T_Stone21.txt" #break
#case(62)  #declare Txt_Path="51 - T_Stone22.txt" #break
#case(63)  #declare Txt_Path="51 - T_Stone23.txt" #break
#case(64)  #declare Txt_Path="51 - T_Stone24.txt" #break
#case(65)  #declare Txt_Path="51 - T_Stone25.txt" #break
#case(66)  #declare Txt_Path="51 - T_Stone26.txt" #break
#case(67)  #declare Txt_Path="51 - T_Stone27.txt" #break
#case(68)  #declare Txt_Path="51 - T_Stone28.txt" #break
#case(69)  #declare Txt_Path="51 - T_Stone29.txt" #break

#case(70)  #declare Txt_Path="51 - T_Stone30.txt" #break
#case(71)  #declare Txt_Path="51 - T_Stone31.txt" #break
#case(72)  #declare Txt_Path="51 - T_Stone32.txt" #break
#case(73)  #declare Txt_Path="51 - T_Stone33.txt" #break
#case(74)  #declare Txt_Path="51 - T_Stone34.txt" #break
#case(75)  #declare Txt_Path="51 - T_Stone35.txt" #break
#case(76)  #declare Txt_Path="51 - T_Stone36.txt" #break
#case(77)  #declare Txt_Path="51 - T_Stone37.txt" #break
#case(78)  #declare Txt_Path="51 - T_Stone38.txt" #break
#case(79)  #declare Txt_Path="51 - T_Stone39.txt" #break

#case(80)  #declare Txt_Path="51 - T_Stone40.txt" #break
#case(81)  #declare Txt_Path="51 - T_Stone41.txt" #break
#case(82)  #declare Txt_Path="51 - T_Stone42.txt" #break
#case(83)  #declare Txt_Path="51 - T_Stone43.txt" #break
#case(84)  #declare Txt_Path="51 - T_Stone44.txt" #break
/*
#case(25)  #declare Txt_Path=".txt" #break
*/


#end // of '#switch (Typ)' ------------------------------------------------
//-------------------------------------------------------------------------
#macro Base_Shape1( Tex ) // left box, right sphere
union{ 
  object{ Round_Box( <-0.5,-0.5,-0.5>,<0.5,0.5,0.5>, 0.1, 0) texture{Tex}
          scale 1.1 rotate<0,15,0> translate<-0.4 ,0.9,-0.8> } 
// box{<-0.3,-1.9/2,0>,<0.3,1.9/2,7> rotate<0, 0,0> texture{Tex} translate<-0.95 ,0.1+1.9/2,-0.8> } 
 sphere{<0,0,0>,1.1 texture{Tex} translate<0.55,1.2,0> } 
 translate<0,0,-0.35>
 }//  
#end // macro
//------------------------------------------------------------------------- 
//------------------------------------------------------------------------- 
//------------------------------------------------------------------------- 
#if ( (Typ >=1) & (Typ <=84)) // 

#default{ finish{ ambient 0.1 diffuse 0.9 }} 
#include "stones.inc"

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

#if ( (Typ>=1) & (Typ<=84) & (Typ!=10) & (Typ!=40) ) //  
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
//------------------------------------------------------------------------- 




