// Insert menu illustration scene "Sky.pov"
// Author: Friedrich A. Lohmueller, June-2012
#version 3.7;
#if( Typ > 42 ) 
global_settings{ assumed_gamma 1.0 } 
#end

#default{ finish{ ambient 0.1 diffuse 0.9 }} 
#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"
#include "glass.inc"
//------------------------------------
#declare In_Path1  = "E0 - Textures and Materials/10 - Texture samples/"   
#declare In_Path2  = "E0 - Textures and Materials/20 - Mirrors and Glasses/"   

     // #declare Typ = 41; // for tests

#switch (Typ)  //----------------------------------------------------------
// In_Path1
#case(1)  #declare Txt_Path="01 - checker.txt" #break 
#case(2)  #declare Txt_Path="02 - checker_spherical_warp.txt" #break
#case(3)  #declare Txt_Path="03 - hexagon.txt" #break
#case(4)  #declare Txt_Path="04 - brick - 1.txt" #break
#case(5)  #declare Txt_Path="05 - brick - 2.txt" #break
#case(6)  #declare Txt_Path="06 - brick spherical_warp.txt" #break
#case(7)  #declare Txt_Path="07 - crackle with colors.txt" #break
#case(8)  #declare Txt_Path="08 - crackle with textures.txt" #break
#case(9)  #declare Txt_Path="09 - crackle normal pigment_pattern.txt" #break
#case(10)  #declare Txt_Path="10 - gradient_xy candy_cane.txt" #break
#case(11)  #declare Txt_Path="11 - spiral1 candy_cane.txt" #break
#case(12)  #declare Txt_Path="12 - Radial color_map.txt" #break
#case(13)  #declare Txt_Path="13 - Radial.txt" #break
#case(14)  #declare Txt_Path="14 - gradient_peel.txt" #break
#case(15)  #declare Txt_Path="15 - spiral_peel.txt" #break
// In_Path2 
#case(40)  #declare Txt_Path="40 - Polished_Chrome.txt" #break
#case(41)  #declare Txt_Path="41 - Polished_Chrome bumps.txt" #break
#case(42)  #declare Txt_Path="42 - Polished_Chrome crackle.txt" #break

#case(80)  #declare Txt_Path="80 - Window Glass no IOR.txt" #break
#case(81)  #declare Txt_Path="81 - Glass with Refraction.txt" #break
#case(82)  #declare Txt_Path="82 - NBglass with Refraction.txt" #break
#case(83)  #declare Txt_Path="83 - Glass3 with Refraction.txt" #break
#case(84)  #declare Txt_Path="84 - NBoldglass with Refraction.txt" #break
#case(85)  #declare Txt_Path="85 - Green_Glass with Refraction.txt" #break
#case(86)  #declare Txt_Path="86 - NBwinebottle with Refraction.txt" #break
#case(87)  #declare Txt_Path="87 - Yellow_Glass with Refraction.txt" #break
#case(88)  #declare Txt_Path="88 - Orange_Glass with Refraction.txt" #break
#case(89)  #declare Txt_Path="89 - NBbeerbottle with Refraction.txt" #break

#case(90)  #declare Txt_Path="90 - Ruby_Glass with Refraction.txt" #break
#case(91)  #declare Txt_Path="91 - Vicks_Bottle_Glass with Refraction.txt" #break
#case(92)  #declare Txt_Path="92 - Blue Glass with Refraction.txt" #break

#end // of '#switch (Typ)' ------------------------------------------------
//-------------------------------------------------------------------------
#macro Base_Shape0( Tex )
 sphere{<0,0,0>,1 texture{Tex} translate<0,1,-0.35>
 }//  
#end // macro

#macro Base_Shape1( Tex )
union{ 
 box{<-1.0,0,0>,<0,2,2> texture{Tex} translate<-0.80,0,0> } 
 sphere{<0,0,0>,1 texture{Tex} translate<0.65,1,0> } 
 translate<0,0,-0.35>
 }//  
#end // macro

#macro Base_Shape2( Tex )
union{ 
 cylinder{<0,0,0>,<0,2,0>,0.35 texture{Tex rotate<0,0,0>} translate<1.20,0,0> } 
 sphere{<0,0,0>,1 texture{Tex} rotate<-30,0,0> translate<-0.45,1,0> } 
 translate<0,0,-0.5>
 }//  
#end // macro

#macro Base_Shape3( Tex )
union{ 
 box{<-0.75,0,-1>,<0.75,0.1,1> texture{Tex} translate<-0.80,0,0.5> } 
 sphere{<0,0,0>,1 texture{Tex} translate<0.65,1,0> } 
 translate<0,0,-0.35>
 }//  
#end // macro


#macro Base_ShapeM( Tex )
 sphere{<0,0,0>,1 material{Tex} translate<0,1,-0.35>
 }//  
#end // macro
//--------------------------------------------------------------------------------------------
#macro Base_Scene_2 ()

 
camera {perspective angle 75 location  <0.0 , 1.0 ,-3.0>
                            right     x*image_width/image_height
                            look_at   <0.0 , 1.0 , 0.0>}
light_source{< 3000,3000,-3000> color White}
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
       texture{ pigment{ checker color rgb<1,1,1>*1.2 color rgb<0.25,0.15,0.1>*0}
                finish { phong 0.1}
              } // end of texture
     } // end of plane
 
#end 
//------------------------------------------------------------------------- 
//----------
#if (Typ=1) // 
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00>} 
#end
//----------
#if (Typ=2) // 
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
object{ Base_Shape0( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.20> } 
#end
//----------
#if (Typ=3) // 
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end
//----------
#if (Typ=4) // 
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end
//----------
#if (Typ=5) // 
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
object{ Base_Shape1( #include concat(In_Path1,Txt_Path)) translate<0,0,-0.00> } 
#end
//----------
#if (Typ=6) // 
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
object{ Base_Shape0( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.20> } 
#end
//----------
#if (Typ=7) // 
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) rotate<0,10,0> translate<0,0,-0.00> } 
#end
//----------
#if (Typ=8) // 
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
#declare Texture_W =
 texture{ pigment{ color White*0.9}
          finish{ specular 1} // shiny
        } // end of texture
#declare Texture_S =
 texture{ T_Stone10 scale 0.15
          normal { agate 0.25 scale 0.25 rotate<0,0,0> }
          finish { phong 1 }
          scale 0.5 translate<0,0,0>
        } // end of texture
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) rotate<0,10,0>  translate<0,0,-0.00> } 
#end
//----------
#if (Typ=9) // 
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) rotate<0,0,0>  translate<0,0,-0.00> } 
#end
//----------
#if (Typ=10) // 
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end
//----------
#if (Typ=11) // 
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
object{ Base_Shape2( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end
//----------
#if (Typ=12) // 
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
object{ Base_Shape3( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end
//----------
#if (Typ=13) // 
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
object{ Base_Shape3( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end
//----------
#if (Typ=14) // 
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
object{ Base_Shape1( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end
//----------
#if (Typ=15) // 
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
object{ Base_Shape2( #include concat(In_Path1,Txt_Path) ) translate<0,0,-0.00> } 
#end


//---------- Mirrors + Glasses
#if (Typ=40) // 
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
object{ Base_Shape0( #include concat(In_Path2,Txt_Path) ) translate<0, 0.20,-0.00> } 
#end
//----------
#if (Typ=41) // 
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
object{ Base_Shape0( #include concat(In_Path2,Txt_Path) ) translate<0, 0.20,-0.00> } 
#end
//----------
#if (Typ=42) // 
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
object{ Base_Shape0( #include concat(In_Path2,Txt_Path) ) translate<0, 0.20,0> } 
#end

//---------- 
#if (Typ=80) //  
Base_Scene_2()
object{ Base_Shape1( #include concat(In_Path2,Txt_Path) ) scale 0.95 translate<0, 0.50,-0.00> } 
#end
//----------
#if (Typ=81) // 
Base_Scene_2()
object{ Base_ShapeM( #include concat(In_Path2,Txt_Path) ) translate<0, 0.10,-0.10> } 
#end
//----------
#if (Typ=82) // 
Base_Scene_2()
object{ Base_ShapeM( #include concat(In_Path2,Txt_Path) ) translate<0, 0.10,-0.10> } 
#end
//----------
#if (Typ=83) // 
Base_Scene_2()
object{ Base_ShapeM( #include concat(In_Path2,Txt_Path) ) translate<0, 0.10,-0.10> } 
#end
//----------
#if (Typ=84) // 
Base_Scene_2()
object{ Base_ShapeM( #include concat(In_Path2,Txt_Path) ) translate<0, 0.10,-0.10> } 
#end
//----------
#if (Typ=85) // 
Base_Scene_2()
object{ Base_ShapeM( #include concat(In_Path2,Txt_Path) ) translate<0, 0.10,-0.10> } 
#end
//----------
#if (Typ=86) // 
Base_Scene_2()
object{ Base_ShapeM( #include concat(In_Path2,Txt_Path) ) translate<0, 0.10,-0.10> } 
#end
//----------
#if (Typ=87) // 
Base_Scene_2()
object{ Base_ShapeM( #include concat(In_Path2,Txt_Path) ) translate<0, 0.10,-0.10> } 
#end
//----------
#if (Typ=88) // 
Base_Scene_2()
object{ Base_ShapeM( #include concat(In_Path2,Txt_Path) ) translate<0, 0.10,-0.10> } 
#end
//----------
#if (Typ=89) // 
Base_Scene_2()
object{ Base_ShapeM( #include concat(In_Path2,Txt_Path) ) translate<0, 0.10,-0.10> } 
#end
//----------
#if (Typ=90) // 
Base_Scene_2()
object{ Base_ShapeM( #include concat(In_Path2,Txt_Path) ) translate<0, 0.10,-0.10> } 
#end
//----------
#if (Typ=91) // 
Base_Scene_2()
object{ Base_ShapeM( #include concat(In_Path2,Txt_Path) ) translate<0, 0.10,-0.10> } 
#end
//----------
#if (Typ=92) // 
Base_Scene_2()
object{ Base_ShapeM( #include concat(In_Path2,Txt_Path) ) translate<0, 0.10,-0.10> } 
#end

//---------- End Mirrors + Glasses