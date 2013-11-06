// Insert menu illustration scene "Random.pov"
// Author Friedrich A. Lohmueller, June-2012,
// expanded  March 2013

#version 3.7; 

  // #declare Typ =71; // for tests

#if ((Typ > 35) & ( Typ < 70))
global_settings{ assumed_gamma 1.0 } 
#end 

#default{ finish{ ambient 0.1 diffuse 0.9 }}
#include "shapes.inc"
#include "shapes2.inc"
#include "colors.inc"
#include "textures.inc"
#include "stones.inc"
#include "glass.inc"

// Random

#declare In_Path  = "B0 - Random/"
#declare In_Path2 = ""


#switch (Typ)  //----------------------------------------------------------
// 45 - Random/
#case(31)  #declare Txt_Path="31 - random linear height.txt" #break
#case(32)  #declare Txt_Path="32 - random linear size.txt" #break
#case(33)  #declare Txt_Path="33 - random quadratic position.txt" #break
#case(34)  #declare Txt_Path="34 - random cubic position.txt" #break
#case(35)  #declare Txt_Path="35 - random tuft of grass.txt" #break

#case(41)  #declare Txt_Path="41 - VRand_In_Sphere.txt" #break
#case(42)  #declare Txt_Path="42 - VRand_On_Sphere.txt" #break
#case(43)  #declare Txt_Path="43 - VRand_In_Obj.txt" #break

#case(71)  #declare Txt_Path="71 - makegrass 1.txt" #break
#case(72)  #declare Txt_Path="72 - makegrass 2.txt" #break
#case(73)  #declare Txt_Path="73 - makegrass 3.txt" #break
#case(75)  #declare Txt_Path="75 - grass meadow.txt" #break

#end // of '#switch (Typ)' ------------------------------------------------
//-------------------------------------------------------------------------

//---------------------------



//------------------------------ the Axes --------------------------------
//------------------------------------------------------------------------
#macro Axis__( AxisLen, Dark_Texture,Light_Texture,Scale)
 union{
    cylinder { <0,-AxisLen,0>,<0,AxisLen,0>,0.05
               texture{checker texture{Dark_Texture }
                               texture{Light_Texture}
                       translate<0.1,0,0.1>}
             }
    cone{<0,0,0>,0.25,<0,1,0>,0 scale Scale translate <0,AxisLen,0>
          texture{Dark_Texture}
         }
     } // end of union
#end // of macro "Axis()"
//------------------------------------------------------------------------
#macro AxisXYZ_( AxisLenX, AxisLenY, AxisLenZ, Tex_Dark, Tex_Light,
                 Rot_X,Rot_Y, ScaleX,ScaleY,ScaleZ)
//--------------------- drawing of 3 Axes --------------------------------

union{
#if (AxisLenX != 0)
 object { Axis__(AxisLenX, Tex_Dark, Tex_Light, ScaleX)   rotate< 0,0,-90>}// x-Axis
 text   { ttf "arial.ttf",  "x",  0.15,  0  texture{Tex_Dark}
         rotate<Rot_X,Rot_Y,0> scale ScaleX translate <AxisLenX+0.05,0.3,-0.10> no_shadow}
#end // of #if
#if (AxisLenY != 0)
 object { Axis__(AxisLenY, Tex_Dark, Tex_Light,ScaleY)   rotate< 0,0,  0>}// y-Axis
 text   { ttf "arial.ttf",  "y",  0.15,  0  texture{Tex_Dark}
          rotate<Rot_X,0,0> scale ScaleY translate <-0.55,AxisLenY+0.15,-0.20> rotate<0,Rot_Y,0> no_shadow}
#end // of #if
#if (AxisLenZ != 0)
 object { Axis__(AxisLenZ, Tex_Dark, Tex_Light,ScaleY)   rotate<90,0,  0>}// z-Axis
 text   { ttf "arial.ttf",  "z",  0.15,  0  texture{Tex_Dark}
          rotate<Rot_X,Rot_Y,0> scale ScaleZ translate <-0.35,0.3,AxisLenZ+0.10> no_shadow}
#end // of #if
} // end of union
#end// of macro "AxisXYZ( ... )"
//------------------------------------------------------------------------
#declare T_Dark  =
texture {
 pigment{ color rgb<1,0.55,0>}
 finish { phong 1}
}
#declare T_Light =
texture {
 pigment{ color rgb<1,1,1>}
 finish { phong 1}
}
//------------------------------------------------------------------------
#macro Blue_Background ()
sky_sphere{ pigment{ gradient <0,1,0>
                     color_map{ [0   color rgb<0.34,0.34,0.36>*1.5]
                                [0.5 color rgb<0.24,0.34,0.56>*0.5]
                                [0.5 color rgb<0.24,0.34,0.56>*0.5]
                                [1.0 color rgb<0.34,0.34,0.36>*1.5]
                              }
                      rotate< -30,0, 0>
                      scale 2 }
           } // end of sky_sphere
#end
// ---------------------------
// ---------------------------
// ---------------------------
// ---------------------------
// ---------------------------





//------------------------------------------------------------------------------------
// 45 - Random/

#if (Typ=31) // In_Path,"31 - random linear height.txt"
#include "10 - Ready made scenes/40 - Basic Scene 04 - Grass with partly cloudy sky.txt"
#include concat(In_Path,Txt_Path)
#end

#if (Typ=32)
#include "10 - Ready made scenes/40 - Basic Scene 04 - Grass with partly cloudy sky.txt"
#include concat(In_Path,Txt_Path)
#end

#if (Typ=33)
#include "10 - Ready made scenes/40 - Basic Scene 04 - Grass with partly cloudy sky.txt"
#include concat(In_Path,Txt_Path)
#end

#if (Typ=34)
#include "10 - Ready made scenes/40 - Basic Scene 04 - Grass with partly cloudy sky.txt"
#include concat(In_Path,Txt_Path)
#end

#if (Typ=35)
light_source{<-1500,2500,-2500> color White}
plane{ <0,1,0>, 0 
       texture{ pigment{ color rgb <1.00,0.95,0.8>}
                normal { bumps 0.75 scale 0.025  }
                finish { phong 0.1 } 
              } // end of texture
     } // end of plane
//#include "10 - Ready made scenes/30 - Basic Scene 03 - White sands with blue sky.txt"
camera{ angle 20 location < 3.00, 2.00, -4.50> look_at< 0.05, 0.55,  0.00> right x*image_width/image_height}
#include concat(In_Path,Txt_Path)
#end


#if (Typ=41)// 41 - VRand_In_Sphere.txt
Blue_Background ()
camera{ angle 22 location < 3.00, 4.00, -4.50> look_at< 0.05, 0.30,  0.00> right x*image_width/image_height}
light_source{<2500, 2500,    0> color rgb<1,1,1>*0.9}     // sun light
light_source{<4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1 shadowless}  // flash
object{ AxisXYZ_( 2.25, 1.85, 3.5, T_Dark, T_Light,30,-45,   0.65,0.65,0.65) scale 0.5}
#include concat(In_Path,Txt_Path)
#end

#if (Typ=42)// 42 - VRand_On_Sphere.txt
Blue_Background ()
camera{ angle 29 location < 3.00, 4.00, -4.50> look_at< 0.05, 0.00,  0.00> right x*image_width/image_height}
light_source{<2500, 2500,    0> color rgb<1,1,1>*0.9}     // sun light
light_source{<4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1 shadowless}  // flash
//object{ AxisXYZ_( 2.25, 1.85, 3.5, T_Dark, T_Light,30,-45,   0.65,0.65,0.65) scale 0.5}
#include concat(In_Path,Txt_Path)
#end

#if (Typ=43)// 43 - VRand_In_Obj.txt
Blue_Background ()
camera{ angle 27 location < 3.00, 4.00, -4.50> look_at< 0.05, 0.10,  0.00> right x*image_width/image_height}
light_source{<2500, 2500,    0> color rgb<1,1,1>*0.9}     // sun light
light_source{<4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1 shadowless}  // flash
object{ AxisXYZ_( 2.75, 2.00, 3.25, T_Dark, T_Light,30,-45,   0.65,0.65,0.65) scale 0.5}
#include concat(In_Path,Txt_Path)
#end


#if (Typ=71) // 71 - makegrass 1.tx
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
#include concat(In_Path,Txt_Path)
#end
#if (Typ=72) // 72 - makegrass 2.tx
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
#include concat(In_Path,Txt_Path)
#end
#if (Typ=73) // 73 - makegrass 3.tx
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
#include concat(In_Path,Txt_Path)
#end

#if (Typ=75) // 75 - grass meadow.tx
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
#include concat(In_Path,Txt_Path)
#end


//---------------------------------------------------- end random