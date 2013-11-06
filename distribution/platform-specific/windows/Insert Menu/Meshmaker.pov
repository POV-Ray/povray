// Insert menu illustration scene
// Author Friedrich A. Lohmueller, March-2013
//
#version 3.7;

    // #declare Typ = 163; // for tests

//#if(( Typ < 500 | Typ > 831 ) 
//   & (Typ !=110) & (Typ !=430) & (Typ !=431) ) 
global_settings{ assumed_gamma 1.0 } 
//#end 


#default{ finish{ ambient 0.1 diffuse 0.9 }}
#include "colors.inc"
#include "textures.inc"
#include "woods.inc"
#include "shapes.inc"
#include "functions.inc"
#include "math.inc"
#include "transforms.inc"
// --------------------------- the following paths must exist:
#declare In_Path = "40 - Special shapes/"
/*
#declare Sub_Path7 = "32 - Isosurfaces by pattern functions/"
#declare Sub_Path8 = "40 - Polynomial Quartic/"
#declare Sub_Path9 = "50 - Parametric/"
*/
#declare Sub_Path10 = "60 - Meshes by meshmaker.inc/"
//------------------------------------------------------------

//----------------------------------------------------------
#switch (Typ)  //----------------------------------------------------------
// 60 - Mesh_Generation/
#case(10)  #declare Txt_Path="10 - prism by spline.txt" #break
#case(15)  #declare Txt_Path="15 - prism1 by splines.txt" #break
#case(20)  #declare Txt_Path="20 - mesh by array of splines.txt" #break

#case(30)  #declare Txt_Path="30 - lathe mesh by spline.txt" #break

#case(35)  #declare Txt_Path="35 - p_lathe 1.txt" #break
#case(36)  #declare Txt_Path="36 - p_lathe 2.txt" #break

#case(40)  #declare Txt_Path="40 - mesh by 4 splines.txt" #break

#case(50)  #declare Txt_Path="50 - mesh by 2varfunction 1.txt" #break
#case(52)  #declare Txt_Path="52 - mesh by 2varfunction 2.txt" #break
#case(54)  #declare Txt_Path="54 - mesh by 2varfunction 3.txt" #break
// parametrics -----------------------------------------
#case(60)  #declare Txt_Path="60 - p_square_uv.txt" #break

#case(65)  #declare Txt_Path="65 - p_sphere_uv.txt" #break

#case(70)  #declare Txt_Path="70 - p_cylinder.txt" #break
#case(71)  #declare Txt_Path="71 - p_cylinder_uv_in_out.txt" #break
#case(73)  #declare Txt_Path="73 - p_cylinder_crackle.txt" #break
#case(75)  #declare Txt_Path="75 - p_cylinder_crackle_2.txt" #break

#case(80)  #declare Txt_Path="80 - p_shell 1.txt" #break
#case(82)  #declare Txt_Path="82 - p_shell 2.txt" #break
#case(84)  #declare Txt_Path="84 - p_shell 3.txt" #break

#case(86)  #declare Txt_Path="86 - p_dini 1.txt" #break
#case(87)  #declare Txt_Path="87 - p_dini 2.txt" #break


#case(90)  #declare Txt_Path="90 - p_moebius 1.txt" #break
#case(92)  #declare Txt_Path="92 - p_moebius 2.txt" #break
#case(94)  #declare Txt_Path="94 - p_moebius 3.txt" #break 

#case(101)  #declare Txt_Path="A1 - p_klein8 1.txt" #break
#case(102)  #declare Txt_Path="A2 - p_klein8 2.txt" #break
#case(103)  #declare Txt_Path="A3 - p_klein8 3.txt" #break

#case(105)  #declare Txt_Path="A5 - p_klein 1.txt" #break

#case(108)  #declare Txt_Path="A8 - p_kleinbottle 1.txt" #break
#case(109)  #declare Txt_Path="A9 - p_kleinbottle 2.txt" #break

#case(110)  #declare Txt_Path="B0 - p_bretzel 1.txt" #break
#case(111)  #declare Txt_Path="B1 - p_bretzel 2.txt" #break

//----------------------------------------------------------
#case(130)  #declare Txt_Path="D0 - p_umbilic 0.txt" #break
#case(131)  #declare Txt_Path="D1 - p_umbilic 1.txt" #break
#case(132)  #declare Txt_Path="D2 - p_umbilic 2.txt" #break

#case(140)  #declare Txt_Path="E0 - p_steiner 1.txt" #break

#case(150)  #declare Txt_Path="F0 - p_Spherical_Harmonics 0.txt" #break
#case(151)  #declare Txt_Path="F1 - p_Spherical_Harmonics 1.txt" #break
#case(152)  #declare Txt_Path="F2 - p_Spherical_Harmonics 2.txt" #break
#case(153)  #declare Txt_Path="F3 - p_Spherical_Harmonics 3.txt" #break
#case(154)  #declare Txt_Path="F4 - p_Spherical_Harmonics 4.txt" #break
#case(155)  #declare Txt_Path="F5 - p_Spherical_Harmonics 5.txt" #break
#case(156)  #declare Txt_Path="F6 - p_Spherical_Harmonics 6.txt" #break
#case(157)  #declare Txt_Path="F7 - p_Spherical_Harmonics 7.txt" #break
#case(158)  #declare Txt_Path="F8 - p_Spherical_Harmonics 8.txt" #break

#case(160)  #declare Txt_Path="V0 - SweepSpline1 1.txt" #break
#case(161)  #declare Txt_Path="V0 - SweepSpline2 1.txt" #break
#case(162)  #declare Txt_Path="V1 - SweepSpline1 2.txt" #break
#case(163)  #declare Txt_Path="V1 - SweepSpline2 2.txt" #break
//----------------------------------------------------------
//----------------------------------------------------------
//don't re-render this ! >15min with 8 threats !
#case(951)  #declare Txt_Path="51 - parametric conical spiral.txt" #break
#case(952)  #declare Txt_Path="52 - parametric Dini surface.txt" #break



#end
// ------------------------------------------------------------------------

// ---------------------------
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
         rotate<Rot_X,Rot_Y,0> scale ScaleX translate <AxisLenX+0.10,0.4,-0.10> no_shadow}
#end // of #if
#if (AxisLenY != 0)
 object { Axis__(AxisLenY, Tex_Dark, Tex_Light,ScaleY)   rotate< 0,0,  0>}// y-Axis
 text   { ttf "arial.ttf",  "y",  0.15,  0  texture{Tex_Dark}
          rotate<Rot_X,0,0> scale ScaleY translate <-0.55,AxisLenY+0.25,-0.20> rotate<0,Rot_Y,0> no_shadow}
#end // of #if
#if (AxisLenZ != 0)
 object { Axis__(AxisLenZ, Tex_Dark, Tex_Light,ScaleY)   rotate<90,0,  0>}// z-Axis
 text   { ttf "arial.ttf",  "z",  0.15,  0  texture{Tex_Dark}
          rotate<Rot_X,Rot_Y,0> scale ScaleZ translate <-0.35,0.4,AxisLenZ+0.10> no_shadow}
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
//-------------------------------------------------- end of coordinate axes

// sky ---------------------------------------------------------------
#macro MySkyP()
sky_sphere{ pigment{ gradient <0,1,0>
                     color_map{ [0   color rgb<0.24,0.34,0.56>*0.7] 
                                [0.5 color rgb<0.24,0.34,0.56>*0.4] 
                                [0.5 color rgb<0.24,0.34,0.56>*0.4] 
                                [1.0 color rgb<0.24,0.34,0.56>*0.7] 
                              }
                     
                      rotate< 0,0, 0>  
                   
                     scale 2 }
           } // end of sky_sphere
#end // of macro 
//---------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------





#if (Typ=801)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) 
#end
// ----

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

//------------------ Mesh_Generation
// ---- meshes by splines -------
#if (Typ=10)
MySkyP()
camera{ angle 49 location <1.5 , 2.5 ,-2.75> look_at<0.1 , 0.60 , 0.1>}
light_source{<1000,2500,-2500> color rgb<1,1,1>*0.9}
light_source{<2.0 , 2.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_(  2.75, 2.95, 10, T_Dark, T_Light, 15,-45,   0.6,0.45,0.95) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "10 - prism by spline.txt"
#end
// ----
#if (Typ=15)
MySkyP()
camera{ angle 58 location  <2.5 , 2.5 ,-2.5> look_at<0.1 , 0.8 , 0.1>}
light_source{<1000,2500,-2500> color rgb<1,1,1>*0.9}
light_source{<2.0 , 2.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 3, 4.0, 13, T_Dark, T_Light, 15,-25,   0.6,0.6,0.95 ) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "15 - prism1 by splines.txt"
#end
// ----
#if (Typ=20)
MySkyP()
camera{ angle 56 location  <4.0 , 2.5 ,-7.0> look_at <0.7 , 1.75 , 0.1>}
light_source{<-1000,1500,-2500> color rgb<1,1,1>*0.9}
light_source{<10.0 , 0.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_(3.7, 3.5, 10, T_Dark, T_Light, 5,-15,   0.6,0.6,0.85 ) scale 1}
#include concat(In_Path,Sub_Path10,Txt_Path) // "20 - mesh by array of splines.txt"
#end
// ----
#if (Typ=30)
MySkyP()
camera{ angle 57 location  <3.0 , 2.0 ,-5.0> look_at <0.2 , 1.5 , 0.1>}
light_source{< 1000,1500,-2500> color rgb<1,1,1>*0.9}
light_source{<10.0 , 0.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_(2.0, 3.0, 10, T_Dark, T_Light, 5,-15,   0.6,0.6,0.85 ) scale 1}
#include concat(In_Path,Sub_Path10,Txt_Path) // "30 - lathe mesh by spline.txt"
#end
// ----
#if (Typ=35)
MySkyP()
camera{ angle 45 location  <3.0 , 3.0 ,-3.0> look_at <0.20 , 1.00 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_(2.25, 4 ,5, T_Dark, T_Light, 15,-45,   0.6,0.6,0.85 ) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "35 - p_lathe 1.txt"
#end
// ----
#if (Typ=36)
MySkyP()
camera{ angle 45 location  <3.0 , 3.0 ,-3.0> look_at <0.20 , 1.00 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_(2.25, 4 ,5, T_Dark, T_Light, 15,-45,   0.6,0.6,0.85 ) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "36 - p_lathe 2.txt"
#end
// ----
#if (Typ=40)
MySkyP()
camera{ angle 37 location  <2.0 , 5.0 ,-3.0> look_at <0.40 , 0.15 , 0.1>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_(4, 2.7, 3.25, T_Dark, T_Light,  25,-25,   0.6,0.6,0.75 ) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "40 - mesh by 4 splines.txt"
#end
// ----
#if (Typ=50)
MySkyP()
camera{ angle 42 location <8.0 , 12.0 ,-15.0> look_at <0.70 , 0.00 ,-2.0>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_(6.5, 3.7, 8, T_Dark, T_Light,   25,-25,   0.9,0.9,1.15 ) scale 1}
#include concat(In_Path,Sub_Path10,Txt_Path) // "50 - mesh by 2varfunction 1.txt"
#end
// ----
#if (Typ=52)
MySkyP()
camera{ angle 42 location <8.0 , 12.0 ,-15.0> look_at <0.70 , 0.00 ,-2.0>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_(6.5, 3.7, 8, T_Dark, T_Light,   25,-25,   0.9,0.9,1.15 ) scale 1}
#include concat(In_Path,Sub_Path10,Txt_Path) // "50 - mesh by 2varfunction 2.txt"
#end
// ----
#if (Typ=54)
MySkyP()
camera{ angle 42 location <8.0 , 12.0 ,-15.0> look_at <0.70 , 0.00 ,-2.0>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_(6.5, 3.7, 8, T_Dark, T_Light,   25,-25,   0.9,0.9,1.15 ) scale 1}
#include concat(In_Path,Sub_Path10,Txt_Path) // "50 - mesh by 2varfunction 3.txt"
#end



// ----
// ---- meshes by parametric --
#if (Typ=60)
MySkyP()
camera{ angle 35 location <4 , 4.0 ,-4.0> look_at< 0.00, 0.15,  0.00>}
light_source{< 1500, 2500,-1500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.2}  // flash light
object{ AxisXYZ_(  3.5,2.75, 7, T_Dark, T_Light, 25,-45,   0.9,0.9,1.15) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "55 - p_square_uv.txt"
#end
// ----
#if (Typ=65)
MySkyP()
camera{ angle 35 location <4 , 4.0 ,-4.0> look_at< 0.00, 0.35,  0.00>}
light_source{< 1500, 2500,-1500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.2}  // flash light
object{ AxisXYZ_(  3.5,2.95, 7, T_Dark, T_Light, 25,-45,   0.9,0.9,1.15) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "55 - p_sphere_uv.txt"
#end
// ----
#if (Typ=70)
MySkyP()
camera{ angle 37 location <4 , 4.0 ,-4.0> look_at< 0.00, 0.15,  0.00>}
light_source{< 1500, 2500,-1500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.2}  // flash light
object{ AxisXYZ_(  3.25,2.95, 7, T_Dark, T_Light, 25,-45,   0.9,0.9,1.15) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "63 - p_cylinder_crackle.txt"
#end
// ----
#if (Typ=71)
MySkyP()
camera{ angle 37 location <4 , 4.0 ,-4.0> look_at< 0.00, 0.15,  0.00>}
light_source{< 1500, 2500,-1500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.2}  // flash light
object{ AxisXYZ_(  3.25,2.95, 7, T_Dark, T_Light, 25,-45,   0.9,0.9,1.15) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "61 - p_cylinder_uv_in_out.txt"
#end
// ----
#if (Typ=73)
MySkyP()
camera{ angle 44 location <4 , 4.0 ,-4.0> look_at< 0.00, 1,  0.00>}
light_source{<2500, 2500,-2000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_(  3.5, 4.75, 8, T_Dark, T_Light, 25,-25,   0.9,0.9,1.15) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "63 - p_cylinder_crackle.txt"
#end
// ----
#if (Typ=75)
MySkyP()
camera{ angle 44 location <4 , 4.0 ,-4.0> look_at< 0.00, 1,  0.00>}
light_source{<2500, 2500,-2000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_(  3.5, 4.75, 8, T_Dark, T_Light, 25,-25,   0.9,0.9,1.15) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "63 - p_cylinder_crackle_2.txt"
#end
// ----
 
// ----
#if (Typ=80)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at<0.20 , 0.50 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.5}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.5}  //shadowless}
object{ AxisXYZ_( 3.75, 2.8,7 , T_Dark, T_Light, 15,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "80 - p_shell 1.txt"
#end
// ----
#if (Typ=82)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at <0.20 , 0.55 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.5}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.5}  //shadowless}
object{ AxisXYZ_( 3.75, 3.3,7 , T_Dark, T_Light, 15,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "82 - p_shell 2.txt"
#end
// ----
#if (Typ=84)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at <0.20 , 0.55 ,0.00>}
light_source{<2000,1500,-1500> color rgb<1,1,1>*0.5}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.5}// shadowless}
object{ AxisXYZ_( 3.75, 3.3,7 , T_Dark, T_Light, 15,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "84 - p_shell 3.txt"
#end


// ----
#if (Typ=86)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at <0.20 , 0.10 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 2.75, 2.5,5 , T_Dark, T_Light, 15,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "86 - p_dini 1.txt"
#end
// ----
#if (Typ=87)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at <0.20 , 0.10 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 2.75, 2.5,5 , T_Dark, T_Light, 15,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "87 - p_dini 2.txt"
#end
// ----


// ----
#if (Typ=90)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at<0.20 , 0.30 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_(  3.75, 2.8,7, T_Dark, T_Light, 15,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "90 - p_moebius 1.txt"
#end
// ----
#if (Typ=92)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at<0.20 , 0.00 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.5}
light_source{<0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.5 } //shadowless}
object{ AxisXYZ_( 3.95, 2.4,7, T_Dark, T_Light, 25,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "92 - p_moebius 2.txt"
#end
// ----
#if (Typ=94)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at<0.20 ,- 0.10 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
//object{ AxisXYZ_( 3.95, 2.4,7, T_Dark, T_Light, 25,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "94 - p_moebius 3.txt"
#end
// ----
#if (Typ=101)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at<0.20 ,-0.10 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 3.75, 2.2,6, T_Dark, T_Light, 25,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "A1 - p_klein8 1.txt"
#end
// ----
#if (Typ=102)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at<0.20 ,-0.10 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 3.75, 2.2,6, T_Dark, T_Light, 25,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "A2 - p_klein8 2.txt"
#end
// ----
#if (Typ=103)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at<0.20 ,-0.10 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 3.75, 2.2,6, T_Dark, T_Light, 25,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "A3 - p_klein8 3.txt"
#end
// ----
#if (Typ=105)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at  <0.20 , 0.30 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 3.75, 2.2,6, T_Dark, T_Light, 0,-65,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "A8 - p_klein 1.txt"
#end
// ----



#if (Typ=108)
MySkyP()
camera{ angle 45 location <4.5, 1.0 ,-2.5> look_at <0.50 , 0.00 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 4.65, 2.2,6, T_Dark, T_Light, 0,-65,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "A8 - p_kleinbottle 1.txt"
#end
// ----
#if (Typ=109)
MySkyP()
camera{ angle 45 location <4.5, 1.0 ,-2.5> look_at <0.80 , 0.00 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
//object{ AxisXYZ_( 4.65, 2.2,6, T_Dark, T_Light, 0,-65,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "A9 - p_kleinbottle 2.txt"
#end
// ----
#if (Typ=110)
MySkyP()
camera{ angle 45 location <3,3,-3> look_at <0.50 , 0.00 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 4, 2.2,6, T_Dark, T_Light,25,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "B0 - p_bretzel 1.txt"
#end
// ----
#if (Typ=111)
MySkyP()
camera{ angle 45 location <3,3,-3> look_at <0.50 , 0.00 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 4, 2.2,6, T_Dark, T_Light,25,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "B0 - p_bretzel 2.txt"
#end
// ----

//----------------------------------------------------------
#if (Typ=130)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at<0.20 ,-0.10 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 3.75, 2.2,6, T_Dark, T_Light, 25,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "D0 - p_umbilic 0.txt"
#end
// ----
#if (Typ=131)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at<0.20 ,-0.10 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 3.75, 2.2,6, T_Dark, T_Light, 25,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "D1 - p_umbilic 1.txt"
#end
// ----
#if (Typ=132)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at<0.20 ,-0.10 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 3.25, 3.2,5, T_Dark, T_Light, 15,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "D2 - p_umbilic 2.txt"
#end
// ----

// ----
#if (Typ=140)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at <0.20 , 0.50 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 3.25, 3.2,5, T_Dark, T_Light, 15,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "E0 - p_steiner 1.txt"
#end
// ----
#if (Typ=150)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at<0.20 , 0.20 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 3.75, 2.5,7, T_Dark, T_Light, 15,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "F0 - p_Spherical_Harmonics 0.txt"
#end
// ----
#if (Typ=151)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at<0.20 , 0.20 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 3.75, 2.5,7, T_Dark, T_Light, 15,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "F1 - p_Spherical_Harmonics 1.txt"
#end
// ----
#if (Typ=152)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at<0.20 , 0.20 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 3.75, 2.5,7, T_Dark, T_Light, 15,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "F2 - p_Spherical_Harmonics 2.txt"
#end
// ----
#if (Typ=153)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at<0.20 , 0.20 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 3.75, 2.5,7, T_Dark, T_Light, 15,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "F3 - p_Spherical_Harmonics 3.txt"
#end
// ----
#if (Typ=154)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at<0.20 , 0.20 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 3.75, 2.5,7, T_Dark, T_Light, 15,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "F4 - p_Spherical_Harmonics 4.txt"
#end
// ----
#if (Typ=155)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at<0.20 , 0.20 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 3.75, 2.5,7, T_Dark, T_Light, 15,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "F5 - p_Spherical_Harmonics 5.txt"
#end
// ----
#if (Typ=156)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at<0.20 , 0.20 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 3.75, 2.5,7, T_Dark, T_Light, 15,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "F6 - p_Spherical_Harmonics 6.txt"
#end
// ----
#if (Typ=157)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at<0.20 , 0.20 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 3.75, 2.5,7, T_Dark, T_Light, 15,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "F7 - p_Spherical_Harmonics 7.txt"
#end
// ----
#if (Typ=158)
MySkyP()
camera{ angle 45 location <3.0 , 3.0 ,-3.0> look_at<0.20 , 0.20 ,0.00>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 3.75, 2.5,7, T_Dark, T_Light, 15,-45,   0.6,0.6,0.85) scale 0.5}
#include concat(In_Path,Sub_Path10,Txt_Path) // "F8 - p_Spherical_Harmonics 8.txt"
#end
// ----

#if (Typ=160)
MySkyP()
camera{ angle 45 location  <4.0 , 1.5 ,-8.0> look_at<3.0 , 2.0 , 0.0>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 4.8, 3.0, 5, T_Dark, T_Light, 5,-35,   0.6,0.9,0.95) scale 1}
#include concat(In_Path,Sub_Path10,Txt_Path) // "V0 - SweepSpline1 1.txt"
#end
// ----
#if (Typ=161)
MySkyP()
camera{ angle 45 location  <4.0 , 1.5 ,-8.0> look_at<3.0 , 2.0 , 0.0>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_(4.8, 3.0, 5, T_Dark, T_Light, 5,-35,   0.6,0.9,0.95) scale 1}
#include concat(In_Path,Sub_Path10,Txt_Path) // "V1 - SweepSpline2 1.txt"
#end
// ----
#if (Typ=162)
MySkyP()
camera{ angle 40 location <1.5 , 1 ,-2.5> look_at<0.25 , 0.2 , 0.0>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 4.5, 2.5, 8, T_Dark, T_Light, 5,-35,   0.6,0.9,0.95) scale 0.25}
#include concat(In_Path,Sub_Path10,Txt_Path) // "V1 - SweepSpline1 2.txt"
#end
// ----
#if (Typ=163)
MySkyP()
camera{ angle 40 location <1.5 , 1 ,-2.5> look_at<0.25 , 0.2 , 0.0>}
light_source{<2000,2500,-1500> color rgb<1,1,1>*0.9}
light_source{< 0.0 , 20.5 ,-2.0> color rgb<1,1,1>*0.1 shadowless}
object{ AxisXYZ_( 4.5, 2.5, 8, T_Dark, T_Light, 5,-35,   0.6,0.9,0.95) scale 0.25}
#include concat(In_Path,Sub_Path10,Txt_Path) // "V1 - SweepSpline2 2.txt"
#end
// ----
