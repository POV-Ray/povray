// Insert menu illustration scene "Normal.pov"
// Author: Friedrich A. Lohmueller, March-2013
#version 3.7;


#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"
#include "glass.inc"
//------------------------------------

#declare In_Path1  = "E0 - Textures and Materials/15 - Normal samples/"   
//#declare In_Path2  = "E0 - Textures and Materials/20 - Mirrors and Glasses/"   

    // #declare Typ = 58 ; // for tests

#switch (Typ)  //----------------------------------------------------------
// In_Path1
#case(10)  #declare Txt_Path="10 - white ripples.txt" #break 
#case(11)  #declare Txt_Path="11 - normal checker.txt" #break
#case(12)  #declare Txt_Path="12 - hexagon.txt" #break
#case(13)  #declare Txt_Path="13 - brick.txt" #break
#case(14)  #declare Txt_Path="14 - quilted.txt" #break
#case(15)  #declare Txt_Path="15 - average.txt" #break
#case(16)  #declare Txt_Path="16 - cells.txt" #break
#case(17)  #declare Txt_Path="17 - facets.txt" #break
#case(18)  #declare Txt_Path="18 - checker turbulence.txt" #break
#case(19)  #declare Txt_Path="19 - bricks turbulence.txt" #break

#case(20)  #declare Txt_Path="20 - bricks turbulence.txt" #break
#case(21)  #declare Txt_Path="21 - quilted turbulence.txt" #break
#case(22)  #declare Txt_Path="22 - crackle.txt" #break
#case(23)  #declare Txt_Path="23 - bumps.txt" #break
#case(24)  #declare Txt_Path="24 - wrinkles.txt" #break
#case(25)  #declare Txt_Path="25 - dents.txt" #break
#case(26)  #declare Txt_Path="26 - spotted.txt" #break
#case(27)  #declare Txt_Path="27 - bozo 1.txt" #break
#case(28)  #declare Txt_Path="28 - bozo 2.txt" #break
#case(29)  #declare Txt_Path="29 - agate.txt" #break

#case(30)  #declare Txt_Path="30 - spotted.txt" #break
#case(31)  #declare Txt_Path="31 - spotted.txt" #break
#case(32)  #declare Txt_Path="32 - mandel.txt" #break
#case(33)  #declare Txt_Path="33 - julia.txt" #break
#case(34)  #declare Txt_Path="34 - onion.txt" #break
#case(35)  #declare Txt_Path="35 - onion.txt" #break
#case(36)  #declare Txt_Path="36 - radial 1.txt" #break
#case(37)  #declare Txt_Path="37 - radial 2.txt" #break
#case(38)  #declare Txt_Path="38 - radial 3.txt" #break
#case(39)  #declare Txt_Path="39 - radial 4.txt" #break

#case(40)  #declare Txt_Path="40 - radial 5.txt" #break
#case(41)  #declare Txt_Path="41 - radial 6.txt" #break
#case(42)  #declare Txt_Path="42 - radial 7.txt" #break
#case(43)  #declare Txt_Path="43 - gradient y.txt" #break
#case(44)  #declare Txt_Path="44 - ripples.txt" #break
#case(45)  #declare Txt_Path="45 - wood.txt" #break
#case(46)  #declare Txt_Path="46 - ripples.txt" #break
#case(47)  #declare Txt_Path="47 - ripples.txt" #break
#case(48)  #declare Txt_Path="48 - waves.txt" #break
#case(49)  #declare Txt_Path="49 - spiral1.txt" #break

#case(50)  #declare Txt_Path="50 - spiral2.txt" #break
#case(51)  #declare Txt_Path="51 - spiral1.txt" #break
#case(52)  #declare Txt_Path="52 - spiral1.txt" #break
#case(53)  #declare Txt_Path="53 - gradient.txt" #break
#case(54)  #declare Txt_Path="54 - radial.txt" #break
#case(55)  #declare Txt_Path="55 - radial.txt" #break
#case(56)  #declare Txt_Path="56 - radial.txt" #break
#case(57)  #declare Txt_Path="57 - wood.txt" #break
#case(58)  #declare Txt_Path="58 - hexagon.txt" #break


#end // of '#switch (Typ)' ------------------------------------------------
//-------------------------------------------------------------------------
#macro Base_Shape0( Tex )
 sphere{<0,0,0>,1 texture{Tex} translate<0,1,-0.35>
 }//  
#end // macro

#macro Base_Shape1( Tex ) // left box, right sphere
union{ 
 box{<-1.0,0,0>,<0,2,2> rotate<0,-40,0> texture{Tex} translate<-0.40,0,0.1> } 
 sphere{<0,0,0>,1 texture{Tex} translate<0.65,1,0> } 
 translate<0,0,-0.35>
 }//  
#end // macro

#macro Base_Shape2( Tex ) // box + small sphere
union{ 
 box{<-1,-1,-1>,<1,1,1> scale 0.95 texture{Tex  rotate<90,0,0>} scale 0.8 rotate<30,25,0> translate<-0.30,0.9 ,0> } 
 sphere{<0,0,0>,1 texture{Tex  rotate<90,0,0>} scale 0.5 translate<0.90,1.3,-0.60> } 
 translate<0,0,-0.35>
 }//  
#end // macro
#macro Base_Shape2b( Tex ) // box + small sphere
union{ 
 box{<-1,-1,-1>,<1,1,1> scale 0.95 texture{Tex  rotate< 0,0,0>} scale 0.8 rotate<30,25,0> translate<-0.30,0.9 ,0> } 
 sphere{<0,0,0>,1 texture{Tex  rotate< 0,0,0>} scale 0.5 translate<0.90,1.3,-0.60> } 
 translate<0,0,-0.35>
 }//  
#end // macro


#macro Base_Shape3( Tex ) // box only
 box{<-1,-1,-1>,<1,1,1> texture{Tex rotate<90,0,0>} scale 0.8 rotate<45,-45,0> translate< 0.00,1.0 ,0>  
 translate<0,0,-0.35>
 }//  
#end // macro

#macro Base_Shape4( Tex ) // sphere only 
 sphere{<0,0,0>,1 texture{Tex  rotate< 15,-20,0> }  translate<-0.00,1,-0.10>  
 translate<0,0,-0.35>
 }//  
#end // macro
#macro Base_Shape4b( Tex ) // box only
 box{<-1,-1,-1>,<1,1,1> texture{Tex rotate<0,0,0>} scale 0.8 rotate< 0,-35,0> translate< 0.00,1.0 ,0>   
 translate<0,0,-0.35>
 }//  
#end // macro
#macro Base_Shape4c( Tex ) // cone only 
 cone{<0,0,0>,0.5,<0,1,0>,0  texture{Tex rotate< 0,0,0> }
 }//  
#end // macro
#macro Base_Shape4d( Tex ) // torus only 
torus{ 0.8,0.3  texture{Tex rotate< 0,0,0>}  }//  
#end // macro


#macro Base_Shape5( Tex )
union{ 
 cylinder{<0,0,0>,<0,2,0>,0.35 texture{Tex rotate<0,0,0>} translate<1.20,0,0> } 
 sphere{<0,0,0>,1 texture{Tex} rotate<-30,0,0> translate<-0.45,1,0> } 
 translate<0,0,-0.5>
 }//  
#end // macro

#macro Base_Shape6( Tex )
union{ 
 box{<-0.75,0,-1>,<0.75,0.1,1> texture{Tex} translate<-0.80,0,0.5> } 
 sphere{<0,0,0>,1 texture{Tex} translate<0.65,1,0> } 
 translate<0,0,-0.35>
 }//  
#end // macro


#macro Base_Shape7( Tex )
 sphere{<0,0,0>,1 material{Tex} translate<0,1,-0.35>
 }//  
#end // macro
//--------------------------------------------------------------------------------------------
#macro Base_Scene_2 ()
camera { perspective 
         angle 75 
         location  <0.0 , 1.0 ,-3.0>
         right     x*image_width/image_height
         look_at   <0.0 , 1.0 , 0.0> }
 
light_source{<-3000, 2000, -500> color rgb<1, 0.95, 0.9>*0.9 }
light_source{< 0.0, 1.0, -3.0>   color rgb<0.9,0.,9,1>*0.1  shadowless }
// sky ----------------------------------------------------------------------
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
// fog on the ground -------------------------------------------------
fog { fog_type   2
      distance   50
      color      White  
      fog_offset 0.1
      fog_alt    1.5
      turbulence 1.8
    }
// ground -------------------------------------------------------------------
plane{ <0,1,0>, 0 
       texture{ pigment{ checker color rgb<1,1,1>*1.2*0.8 color rgb<0.25,0.15,0.1>*0}
                finish { phong 0.1}
              } // end of texture
     } // end of plane
 
#end 
//------------------------------------------------------------------------- 
//----------
#if ( (Typ >=10) & (Typ <=60)) // 
#include "10 - Ready made scenes/80 - Basic Scene 08 - Sea with blue sky.txt"
plane{ <0,1,0>, 0.001 texture{ pigment{  color rgb<0.55,0.45,0.43>*0.3} finish { phong 0.1} } }
#end //----------


#if (Typ=10) // 
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=11) // 
light_source{< 0.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape3( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=12) // 
object{ Base_Shape2( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=13) // 
object{ Base_Shape2b( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=14) // 
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=15) // 
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=16) // 
light_source{< 0.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape3( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=17) // 
light_source{< 0.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=18) // 
light_source{< 0.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape2( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=19) // 
light_source{<5.0, 0.2, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape2b( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=20) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape2( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=21) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.15  shadowless }
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=22) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape2( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=23) // 
//light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=24) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.1  shadowless }
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=25) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape2( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=26) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=27) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=28) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape2( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=29) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=30) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=31) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=32) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape2b( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=33) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=34) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape4( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=35) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape4b( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=36) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape4( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end //----------
#if (Typ=37) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape4( #include concat(In_Path1,Txt_Path) )  scale<1,1,0.2> translate<0,0,-1.00> } 
#end //----------
#if (Typ=38) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape4( #include concat(In_Path1,Txt_Path) ) scale 1.1 translate<0,0,-0.00> } 
#end //----------
#if (Typ=39) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape4( #include concat(In_Path1,Txt_Path) ) scale 1.05 translate<0,0,-0.00> } 
#end //----------
#if (Typ=40) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape4( #include concat(In_Path1,Txt_Path) ) scale 1.05 scale 1.0 translate<0,0,-0.00> } 
#end //----------
#if (Typ=41) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape4( #include concat(In_Path1,Txt_Path) )  scale <1,1,0.5>*1.2  translate<0,0,-0.00> } 
#end //----------
#if (Typ=42) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape4( #include concat(In_Path1,Txt_Path) )   scale <1,1,0.5>*1.2 translate<0,0,-0.00> } 
#end //----------
#if (Typ=43) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.65  shadowless }
object{ Base_Shape4( #include concat(In_Path1,Txt_Path) ) scale  1.1 rotate<-50, 0,0> translate<0,0.6, 1.00> } 
#end //----------
#if (Typ=44) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape4( #include concat(In_Path1,Txt_Path) ) scale  1.05  rotate<-10, 0,0> translate<0,0.1,-0.00> } 
#end //----------
#if (Typ=45) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape4( #include concat(In_Path1,Txt_Path) ) scale  1.05  rotate<-10, 0,0> translate<0,0.15, 0.10> } 
#end //----------
#if (Typ=46) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.15  shadowless }
object{ Base_Shape4( #include concat(In_Path1,Txt_Path) ) scale  1.0   rotate<-30,-10,0> translate<-0.2,0.35, 0.10> } 
#end //----------
#if (Typ=47) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape4( #include concat(In_Path1,Txt_Path) )  scale  1.3   rotate< 60, 50,0> translate<-0.5, 0.1  ,-0.210> } 
#end //----------
#if (Typ=48) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.3  shadowless }
object{ Base_Shape4( #include concat(In_Path1,Txt_Path) )  scale  1.3   rotate< 60, 50,0> translate<-0.5, 0.1  ,-0.210> } 
#end //----------
#if (Typ=49) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape4( #include concat(In_Path1,Txt_Path) ) scale  1.0   rotate< -20,10,0> translate<-0.0, 0.2 ,-0.00>  } 
#end //----------
#if (Typ=50) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape4( #include concat(In_Path1,Txt_Path) ) scale  1.1  translate<0,0,-0.00> } 
#end //----------
#if (Typ=51) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape4( #include concat(In_Path1,Txt_Path) )  scale  1.1  translate<0,0,-0.00> } 
#end //----------
#if (Typ=52) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape4c( #include concat(In_Path1,Txt_Path) ) scale 2 rotate<-10,0,-20> translate<0,0.40,-0.30> } 
#end //----------
#if (Typ=53) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape4c( #include concat(In_Path1,Txt_Path) ) scale<1,0.8,1>*2 rotate<-50,-10,-20> translate<0,0.80,-0.40> } 
#end //----------
#if (Typ=54) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.05  shadowless }
object{ Base_Shape4d( #include concat(In_Path1,Txt_Path) ) scale 1 rotate<-90,-40,0> translate<0,1.1,-0.50> } 
#end //----------
#if (Typ=55) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.5  shadowless }
object{ Base_Shape4d( #include concat(In_Path1,Txt_Path) ) scale 1 rotate<-90,-30,0> translate<0,1.06,-0.70> } 
#end //----------
#if (Typ=56) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.25  shadowless }
object{ Base_Shape4d( #include concat(In_Path1,Txt_Path) ) scale<1,2,1>*0.95 rotate<-90,-40,0> translate<0,1.05,-0.70> } 
#end //----------
#if (Typ=57) // 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.05  shadowless }
object{ Base_Shape4d( #include concat(In_Path1,Txt_Path) ) scale<1,1.3,1>*1  rotate<-90,-50,0> translate<0,1.1,-0.50> } 
#end //----------
#if (Typ=58) // 
#default{ finish{ ambient 0.1 diffuse 0.6 }} 
light_source{<10.0, 1.0, -3.0>   color rgb<1,1,1>*0.1  shadowless }
object{ Base_Shape4d( #include concat(In_Path1,Txt_Path) ) scale<1,2,1>  rotate<-90,-40,0> translate<0,1.05,-0.50> } 
#end
