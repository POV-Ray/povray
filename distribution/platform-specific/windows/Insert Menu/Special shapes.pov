// Insert menu illustration scene
// Author Friedrich A. Lohmueller, June-2012
// updated Feb-2013 
#version 3.7;

  //  #declare Typ = 350; // for tests

#if(( Typ < 500 | Typ > 831 ) 
   & (Typ !=110) & (Typ !=430) & (Typ !=431) ) 
global_settings{ assumed_gamma 1.0 } 
#end 


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
#declare Sub_Path1 = "10 - 3D text shapes/"
#declare Sub_Path2 = "15 - height_field and HF_macros/"
#declare Sub_Path3 = "20 - mesh and non CSG shapes/"
#declare Sub_Path4 = "25 - blob and fractal/"
#declare Sub_Path5 = "30 - Isosurfaces by basic functions/"
#declare Sub_Path6 = "31 - Isosurfaces by function.inc/"
#declare Sub_Path7 = "32 - Isosurfaces by pattern functions/"
#declare Sub_Path8 = "40 - Polynomial Quartic/"
#declare Sub_Path9 = "50 - Parametric/"
//------------------------------------------------------------

   

#switch (Typ)  //----------------------------------------------------------
// 10 - 3D text shapes/
#case(110)  #declare Txt_Path="10 - simple flat text ttf.txt" #break
#case(111)  #declare Txt_Path="11 - text truetypefont.txt" #break
#case(121)  #declare Txt_Path="21 - Bevelled_Text macro.txt" #break
#case(122)  #declare Txt_Path="22 - Circle_Text macro.txt" #break
#case(123)  #declare Txt_Path="23 - Circle_Text_Valigned macro.txt" #break
#case(151)  #declare Txt_Path="51 - unicode Math.txt" #break
#case(152)  #declare Txt_Path="52 - unicode Europe.txt" #break
#case(153)  #declare Txt_Path="53 - unicode Asia.txt" #break
#case(154)  #declare Txt_Path="54 - unicode German.txt" #break


// 15 - height_field and HF_macros/
#case(209)  #declare Txt_Path="10 - height_field mountains1.txt" #break
#case(210)  #declare Txt_Path="10 - height_field mountains2.txt" #break
#case(211)  #declare Txt_Path="10 - height_field mountains3.txt" #break

#case(220)  #declare Txt_Path="20 - HF_Square macro.txt" #break
#case(230)  #declare Txt_Path="30 - HF_Sphere macro.txt" #break
#case(240)  #declare Txt_Path="40 - HF_Cylinder macro.txt" #break
#case(250)  #declare Txt_Path="50 - HF_Torus macro.txt" #break


// 20 - mesh and non-CSG shapes/
#case(300)  #declare Txt_Path="00 - disc.txt" #break
#case(310)  #declare Txt_Path="10 - triangle.txt" #break
#case(320)  #declare Txt_Path="20 - smooth_triangle.txt" #break
#case(330)  #declare Txt_Path="30 - polygon.txt" #break
#case(340)  #declare Txt_Path="40 - bicubic_patch.txt" #break

#case(350)  #declare Txt_Path="50 - mesh teapot_tri.txt" #break
#case(351)  #declare Txt_Path="51 - mesh chess knight and rook.txt" #break

#case(371)  #declare Txt_Path="71 - mesh cube sample.txt" #break
#case(385)  #declare Txt_Path="85 - mesh2 sample.txt" #break

// 25 - blob and fractal
#case(430)  #declare Txt_Path="30 - blob object spheres.txt" #break
#case(431)  #declare Txt_Path="31 - blob object cylinders.txt" #break
#case(432)  #declare Txt_Path="32 - blob object mixed.txt" #break
#case(433)  #declare Txt_Path="33 - blob with negative strength.txt" #break
#case(434)  #declare Txt_Path="34 - blob for loop 3D.txt" #break
#case(440)  #declare Txt_Path="40 - julia fractal object1.txt" #break
#case(442)  #declare Txt_Path="42 - julia fractal object2.txt" #break
#case(444)  #declare Txt_Path="44 - julia fractal object3.txt" #break

// 30 - Isosurfaces by basic functions"
#case(500)  #declare Txt_Path="00 - isosurface syntax.txt" #break
#case(512)  #declare Txt_Path="12 - Isosurface cristal.txt" #break
#case(513)  #declare Txt_Path="13 - Isosurface cylinder by sqrt.txt" #break
#case(516)  #declare Txt_Path="16 - Isosurface hyperboloid.txt" #break
#case(519)  #declare Txt_Path="19 - Isosurface cross1.txt" #break
#case(520)  #declare Txt_Path="20 - Isosurface cross1 min blobed.txt" #break
#case(521)  #declare Txt_Path="21 - Isosurface cross2.txt" #break
#case(522)  #declare Txt_Path="22 - Isosurface sine.txt" #break
#case(524)  #declare Txt_Path="24 - Isosurface y sine concentric.txt" #break
#case(525)  #declare Txt_Path="25 - Isosurface y sine cylinder.txt" #break
#case(526)  #declare Txt_Path="26 - Isosurface sine sphere.txt" #break
#case(527)  #declare Txt_Path="27 - Isosurface sine sphere y.txt" #break
#case(528)  #declare Txt_Path="28 - Isosurface sine sphere double sine wave.txt" #break

// 31 - Isosurfaces by function.inc"
#case(600)  #declare Txt_Path="" #break
#case(640)  #declare Txt_Path="40 - Isosurface f_rounded_box.txt" #break
#case(641)  #declare Txt_Path="41 - Isosurface f_pillow.txt" #break
#case(642)  #declare Txt_Path="42 - Isosurface f_superellipsoid.txt" #break
#case(643)  #declare Txt_Path="43 - Isosurface f_torus.txt" #break
#case(650)  #declare Txt_Path="50 - Isosurface f_heart.txt" #break
#case(651)  #declare Txt_Path="51 - Isosurface f_comma.txt" #break
#case(652)  #declare Txt_Path="52 - Isosurface f_dupin_cyclid.txt" #break
#case(653)  #declare Txt_Path="53 - Isosurface f_helix1.txt" #break
#case(655)  #declare Txt_Path="55 - Isosurface f_helix2.txt" #break
#case(656)  #declare Txt_Path="56 - Isosurface f_spiral.txt" #break
#case(657)  #declare Txt_Path="57 - Isosurface f_mesh1.txt" #break
#case(658)  #declare Txt_Path="58 - Isosurface f_flange_cover.txt" #break
#case(659)  #declare Txt_Path="59 - Isosurface f_spikes.txt" #break
#case(660)  #declare Txt_Path="60 - Isosurface f_spikes_2d.txt" #break
// 32 - Isosurfaces by pattern functions"
#case(781)  #declare Txt_Path="81 - Isosurface f_leopard.txt" #break
#case(782)  #declare Txt_Path="82 - Isosurface f_leopard.txt" #break
#case(785)  #declare Txt_Path="85 - Isosurface Round_Box - f_crackle.txt" #break
#case(787)  #declare Txt_Path="87 - Isosurface sphere - f_noise3d .txt" #break
#case(788)  #declare Txt_Path="88 - Isosurface Round_Box - f_agate.txt" #break
#case(789)  #declare Txt_Path="89 - Isosurface Round_Box - f_agate.txt" #break
#case(791)  #declare Txt_Path="91 - Isosurface bozo 3d.txt" #break
#case(792)  #declare Txt_Path="92 - Isosurface Sphere - f_noise3d.txt" #break
#case(794)  #declare Txt_Path="94 - Isosurface Sphere - f_noise3d.txt" #break
#case(796)  #declare Txt_Path="96 - Isosurface Sphere - f_noise3d.txt" #break
#case(797)  #declare Txt_Path="97 - Isosurface Sphere - pigment.txt" #break
#case(798)  #declare Txt_Path="98 - Isosurface f_sphere - wood.txt" #break
// 40 - Polynomial Quartic"
//#case(800)  #declare Txt_Path="polynomial syntax.txt" #break
#case(801)  #declare Txt_Path="01 - Glob_5.txt" #break
#case(802)  #declare Txt_Path="02 - Twin_Glob.txt" #break
#case(803)  #declare Txt_Path="03 - Sinsurf.txt" #break

//#case(810)  #declare Txt_Path="quartic syntax.txt" #break
#case(813)  #declare Txt_Path="13 - Bicorn.txt" #break
#case(814)  #declare Txt_Path="14 - Crossed_Trough.txt" #break
#case(815)  #declare Txt_Path="15 - Cubic_Cylinder.txt" #break
#case(816)  #declare Txt_Path="16 - Cubic_Saddle_1.txt" #break
#case(817)  #declare Txt_Path="17 - Devils_Curve.txt" #break
#case(818)  #declare Txt_Path="18 - Folium.txt" #break
#case(819)  #declare Txt_Path="19 - Helix.txt" #break
#case(820)  #declare Txt_Path="20 - Helix_1.txt" #break
#case(821)  #declare Txt_Path="21 - Hyperbolic_Torus_40_12.txt" #break
#case(822)  #declare Txt_Path="22 - Lemniscate.txt" #break
#case(823)  #declare Txt_Path="23 - Quartic_Loop_1.txt" #break
#case(824)  #declare Txt_Path="24 - Monkey_Saddle.txt" #break
#case(825)  #declare Txt_Path="25 - Parabolic_Torus_40_12.txt" #break
#case(826)  #declare Txt_Path="26 - Piriform.txt" #break
#case(827)  #declare Txt_Path="27 - Quartic_Paraboloid.txt" #break
#case(828)  #declare Txt_Path="28 - Quartic_Cylinder.txt" #break
#case(829)  #declare Txt_Path="29 - Steiner_Surface.txt" #break
#case(830)  #declare Txt_Path="30 - Torus_40_12.txt" #break
#case(831)  #declare Txt_Path="31 - Witch_Hat.txt" #break
// 50 - Parametric"
#case(900)  #declare Txt_Path="00 - parametric sample.txt" #break
#case(910)  #declare Txt_Path="10 - parametric flower.txt" #break
#case(920)  #declare Txt_Path="20 - parametric ornamental.txt" #break
#case(940)  #declare Txt_Path="40 - parametric sin cylinder.txt" #break
#case(945)  #declare Txt_Path="45 - parametric sin surface.txt" #break
#case(950)  #declare Txt_Path="50 - parametric screw.txt" #break
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


//-------------------------------------------------- end of coordinate axes
//---------------------------------<<< settings of squared plane dimensions
#declare RasterScale = 1.0;
#declare RasterHalfLine  = 0.025;
#declare RasterHalfLineZ = 0.025;
//-------------------------------------------------------------------------
#macro Raster(RScale, HLine)
       pigment{ gradient x scale RScale
                color_map{[0.000   color rgbt<1,1,1,0>*0.6]
                          [0+HLine color rgbt<1,1,1,0>*0.6]
                          [0+HLine color rgbt<1,1,1,1>]
                          [1-HLine color rgbt<1,1,1,1>]
                          [1-HLine color rgbt<1,1,1,0>*0.6]
                          [1.000   color rgbt<1,1,1,0>*0.6]} }
 #end// of Raster(RScale, HLine)-macro
//-------------------------------------------------------------------------
#declare Raster_Ground =
plane { <0,1,0>, 0    // plane with layered textures
        texture { pigment{color rgb<1,1,1>*1.1}
                  finish {ambient 0.45 diffuse 0.85}}
        texture { Raster(RasterScale,RasterHalfLine ) rotate<0,0,0> }
        texture { Raster(RasterScale,RasterHalfLineZ) rotate<0,90,0>}
        translate<0,0.001,0>
      }
//-------------------------------------------------------------------------
#declare RasterScale = 1.0;
#declare RasterHalfLine  = 0.015;
#declare RasterHalfLineZ = 0.025;
//-------------------------------------------------------------------------
#macro Raster(RScale, HLine)
       pigment{ gradient x scale RScale
                color_map{[0.000   color rgbt<1,1,1,0>*1.1]
                          [0+HLine color rgbt<1,1,1,0>*1.1]
                          [0+HLine color rgbt<1,1,1,1>]
                          [1-HLine color rgbt<1,1,1,1>]
                          [1-HLine color rgbt<1,1,1,0>*1.1]
                          [1.000   color rgbt<1,1,1,0>*1.1]} }
 #end// of Raster(RScale, HLine)-macro

#declare Raster_Ground2 =
plane { <0,1,0>, 0    // plane with layered textures
        texture { pigment{color rgb<1,1,1>*0.1 } }
        texture { Raster(RasterScale,RasterHalfLine ) rotate<0,0,0> }
        texture { Raster(RasterScale,RasterHalfLineZ) rotate<0,90,0>}
        translate<0,0.001,0>
      }
//------------------------------------------------ end of squared plane XZ
#declare Grass_Ground_Small =
plane { <0,1,0>, 0
        texture{ pigment{ color rgb<0.35,0.65,0.0>*0.72 }
                 normal { bumps 0.75 scale 0.015 }
                 finish { phong 0.1 }
                 scale 0.33
               } // end of texture
      } // end of plane
#declare Grass_Ground  =
plane { <0,1,0>, 0
        texture{ pigment{ color rgb<0.35,0.65,0.0>*0.72 }
                 normal { bumps 0.75 scale 0.015 }
                 finish { phong 0.1 }

               } // end of texture
      } // end of plane
#declare Sand_Ground  =
plane{ <0,1,0>, 0
       texture{ pigment{ color rgb <1.00,0.95,0.8>}
                normal { bumps 0.75 scale 0.025  }
                finish { phong 0.1 }
              } // end of texture
     } // end of plane



//-----------------------------------------------  macro "Vector(Start,End,Radius)"!
#macro Vector(P_start,P_end, R_Vector)
union{

cylinder{ P_start, P_end - ( vnormalize(P_end - P_start)*9.5*R_Vector), R_Vector  }
cone    { P_end - ( vnormalize(P_end - P_start)*10*R_Vector), 3*R_Vector, P_end, 0 }
}// end of union
#end //-------------------------------------------------------------------------- end of macro

//-----------------------------------------------  macro "Distance_Marker(Start,End,Radius)"!
#macro Distance_Marker(P_start,P_end, R_Vector)
union{

cylinder{ P_start + ( vnormalize(P_end - P_start)*9.5*R_Vector),
          P_end - ( vnormalize(P_end - P_start)*9.5*R_Vector), R_Vector  }
cone    { P_end - ( vnormalize(P_end - P_start)*10*R_Vector), 3*R_Vector, P_end, 0 }
cone    { P_start + ( vnormalize(P_end - P_start)*10*R_Vector), 3*R_Vector, P_start, 0 }
}// end of union
#end //-------------------------------------------------------------------------- end of macro
// -----------------------------------------------
#macro MySky()
 sky_sphere{ pigment{ gradient <0,1,0>
                       color_map{ [0   color rgb<1,1,1>         ]//White
                                [0.3 color rgb<0.24,0.34,0.56>*0.8]//~Navy
                                [0.7 color rgb<0.24,0.34,0.56>*0.8]//~Navy
                                [1.0 color rgb<1,1,1>         ]//White
                              }
                     rotate<0,0,0>
                     scale 2 }
           } // end of sky_sphere
fog{fog_type   2
    distance   50
    color      White
    fog_offset 0.1
    fog_alt    1.0
    turbulence 0.8}

#end // call: MySky()
// sky -------------------------------------------------------------------
#macro MySky2 (Turn)
sky_sphere{ pigment{ gradient <0,1,0>
                     color_map{ [0   color rgb<1,1,1>         ]//White
                                [0.5 color rgb<0.24,0.34,0.56>*0.7]//~Navy
                                [0.5 color rgb<0.24,0.34,0.56>*0.7]//~Navy
                                [1.0 color rgb<1,1,1>         ]//White
                              }

                     #if(Turn=1)rotate<-50,0,-50> #end
                     #if(Turn=2)rotate< 50,110,0> #end
                     scale 2 }
           } // end of sky_sphere
#end // of macro
//------------------------------------------------------------------------
// sky -------------------------------------------------------------- 
#macro MySky3()
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
      distance   150
      color      White  
      fog_offset 0.1
      fog_alt    1.5
      turbulence 1.8
    }
#end // of macro

// -----------------------------------------------
#declare T_Orange =
texture{ pigment{ color rgb<1.00,0.65,0.15>}
         finish { phong 1 }
       } // end of texture
#declare T_DarkRed =
texture{ pigment{ color rgb<1,0,0>*0.3 }
         finish { phong 0.1 }
       } // end of texture
#declare T_Red =
texture{ pigment{ color rgb<1,0,0> }
         finish { phong 0.1 }
       } // end of texture
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------





// 10 - 3D_text_shapes/
// ----
#if (Typ=110)
//object{ Raster_Ground }
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
#include concat(In_Path,Sub_Path1,Txt_Path) // "10 - 3D_text_shapes/10 - simple flat text ttf.txt"
#end
// ----
#if (Typ=111)
MySky3()
object{ Grass_Ground }
camera{ angle 27 location < 1.50, 3.00, -10.50> look_at< 0.15, 1.75,  0.15>}
light_source{<-1500,2000,-2500>  color rgb<1,1,1>*0.8}     // sun light
light_source{ < 1.50, 3.00, -10.50> color rgb<0.9,0.9,1>*0.1 shadowless}  // flash light
#include concat(In_Path,Sub_Path1,Txt_Path) // "10 - 3D_text_shapes/11 - text truetypefont.txt"
#end
// ----
#if (Typ=121)
object{ Sand_Ground } MySky()
//#include "10 - Ready made scenes/30 - Basic Scene 03 - White sands with blue sky.txt"
camera{ angle 39 location < -1.20, 2.80, -2.2> look_at<0.80, 0.80,  0.00>}
light_source{<1500, 2500,-1000> color rgb<1,1,1>*0.8}     // sun light
light_source{< -1.20, 2.80, -2.2>color rgb<0.9,0.9,1>*0.1}  // flash light
#include concat(In_Path,Sub_Path1,Txt_Path) // "10 - 3D_text_shapes/21 - Bevelled_Text macro.txt"
#end
// ----
#if (Typ=122)
object{ Grass_Ground } MySky()
camera{ angle 59 location < -0.00, 0.40, -3.5> look_at<0.00, 1.10,  0.00>}
light_source{<- 500, 3000,-2500> color rgb<1,1,1>*0.5}     // sun light
light_source{< -0.00, 1.50, -3.5> color rgb<0.9,0.9,1>*0.7}  // flash light
#include concat(In_Path,Sub_Path1,Txt_Path) // "10 - 3D_text_shapes/22 - Circle_Text macro.txt"
#end
// ----4
#if (Typ=123)
object{ Grass_Ground } MySky()
camera{ angle  50 location <2.0 , 1.0 ,-2.0> look_at <0.0 , 0.3 , 0.0>}
light_source{< 500,2000,-2500> color rgb<1,1,1>*0.9}     // sun light
light_source{< 2.00, 1.50, -2.5> color rgb<0.9,0.9,1>*0.1 shadowless}  // flash light
#include concat(In_Path,Sub_Path1,Txt_Path) // "10 - 3D_text_shapes/23 - Circle_Text_Valigned macro.txt"
#end

// ----
#if (Typ=151)
MySky2(2)
camera{ angle 71 location <0.0 , 1.0 ,-3.0> look_at< 0, 1.0,  0.00>}
light_source{<2500, 2500,-1000> color rgb<1,1,1>*0.9}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 6.65, 3.7, 1.5, T_Dark, T_Light, 0, 0,   0.65,0.65,0.15) scale 0.5}
#include concat(In_Path,Sub_Path1,Txt_Path) // "10 - 3D_text_shapes/51 - unicode Math.txt"
#end
// ----
#if (Typ=152)
MySky2(2)
camera{ angle 78 location < 3.35, 2.50, -5.00> look_at< 3.35, 2.5,  0.00>}
light_source{<2500, 2500,-1000> color rgb<1,1,1>*0.9}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 6.65, 4.7, 1.5, T_Dark, T_Light, 0, 0,   0.65,0.65,0.15) scale 1}
#include concat(In_Path,Sub_Path1,Txt_Path) // "10 - 3D_text_shapes/52 - unicode Europe.txt"
#end
// ----
#if (Typ=153)
MySky2(2)
camera{ angle 71 location < 0, 1.30, -3.00> look_at< 0, 1.3,  0.00>}
light_source{<2500, 2500,-1000> color rgb<1,1,1>*0.9}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 3.65, 4.7, 1.5, T_Dark, T_Light, 0, 0,   0.65,0.65,0.15) scale 0.5}
#include concat(In_Path,Sub_Path1,Txt_Path) // "10 - 3D_text_shapes/53 - unicode Asia.txt"
#end
// ----
#if (Typ=154)
MySky2(2)
camera{ angle 68 location < 1.65, 1.00, -3.00> look_at< 1.65, 1.0,  0.00>}
light_source{<2500, 2500,-1000> color rgb<1,1,1>*0.9}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 6.65, 3.7, 1.5, T_Dark, T_Light, 0, 0,   0.65,0.65,0.15) scale 0.5}
#include concat(In_Path,Sub_Path1,Txt_Path) // "10 - 3D_text_shapes/51 - unicode German.txt"
#end
// ----
/*
#case(110)  #declare Txt_Path="10 - simple flat text ttf.txt" #break
#case(111)  #declare Txt_Path="11 - text truetypefont.txt" #break
#case(121)  #declare Txt_Path="21 - Bevelled_Text macro.txt" #break
#case(122)  #declare Txt_Path="22 - Circle_Text macro.txt" #break
#case(123)  #declare Txt_Path="23 - Circle_Text_Valigned macro.txt" #break
#case(151)  #declare Txt_Path="51 - unicode German.txt" #break
#case(152)  #declare Txt_Path="52 - unicode Europe.txt" #break
#case(153)  #declare Txt_Path="53 - unicode Asia.txt" #break
*/
// ----
// 15 - height_field and HF_macros/
#if (Typ=209)
object{ Raster_Ground scale 1}  MySky()
camera{ angle 65 location<0.0 , 1.0 ,-3.0> look_at<0.0 , 1.0 , 0.0>}   
light_source{<-2500,1500,-1500> color rgb<1,1,1>*1}     // sun light
#include concat(In_Path,Sub_Path2,Txt_Path) // "15 - height_field and HF_macros/10 - height_field mountains1.txt"
#end
// ----
#if (Typ=210)
object{ Raster_Ground scale 1}  MySky()
camera{ angle 65 location<0.0 , 1.0 ,-3.0> look_at<0.0 , 1.0 , 0.0>}   
light_source{<-2500,1500,-2500> color rgb<1,1,1>*1}     // sun light
#include concat(In_Path,Sub_Path2,Txt_Path) // "15 - height_field and HF_macros/10 - height_field mountains2.txt"
#end
// ----
#if (Typ=211)
object{ Raster_Ground scale 1}  MySky()
camera{ angle 75 location <0.00, 1, -3> look_at< 0.00, 1,  0>}
light_source{<2500, 2500,-1000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
#include concat(In_Path,Sub_Path2,Txt_Path) // "15 - height_field and HF_macros/10 - height_field mountains3.txt"
#end
// ----
#if (Typ=220)
object{ Raster_Ground }
camera{ angle 46 location < 4.0, 4.00, -4.00> look_at< 0.0, 0.20,  0.0>}
light_source{<2500, 2500,-1000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 2.35, 1.75, 3.5, T_Dark, T_Light, 25, -45,   0.45,0.55,0.75) scale 1}
#include concat(In_Path,Sub_Path2,Txt_Path) // "15 - height_field and HF_macros/20 - HF_Square macro.txt"
#end
// ----
#if (Typ=230)
object{ Raster_Ground  translate<0,-2,0>}
camera{ angle 50 location < 4.0, 4.00, -4.00> look_at< 0.0, 0.20,  0.0>}
light_source{<2500, 2500,-1000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 2.35, 2.25, 4.0, T_Dark, T_Light, 25, -45,   0.45,0.55,0.75) scale 1}
#include concat(In_Path,Sub_Path2,Txt_Path) // "15 - height_field and HF_macros/30 - HF_Sphere macro.txt"
#end
// ----
#if (Typ=240)
object{ Raster_Ground }
camera{ angle 48 location < 4.0, 4.00, -4.00> look_at< 0.0, 0.50,  0.0>}
light_source{<2500, 2500,-1000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 2.45, 2.25, 5.5, T_Dark, T_Light, 25, -45,   0.45,0.55,0.75) scale 1}
#include concat(In_Path,Sub_Path2,Txt_Path) // "15 - height_field and HF_macros/40 - HF_Cylinder macro.txt"
#end
// ----
#if (Typ=250)
object{ Raster_Ground }
camera{ angle 47 location < 4.5, 4.00, -4.50> look_at< 0.02, 0.10,  0.02>}
light_source{<2500, 2500,-1000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 3.05, 1.75, 4.5, T_Dark, T_Light, 25, -45,   0.45,0.55,0.75) scale 1}
#include concat(In_Path,Sub_Path2,Txt_Path) // "15 - height_field and HF_macros/50 - HF_Torus macro.txt"
#end
// ----
/*
#case(209)  #declare Txt_Path="10 - height_field mountains1.txt" #break
#case(210)  #declare Txt_Path="10 - height_field mountains2.txt" #break
#case(211)  #declare Txt_Path="10 - height_field mountains3.txt" #break

#case(220)  #declare Txt_Path="20 - HF_Square macro.txt" #break
#case(230)  #declare Txt_Path="30 - HF_Sphere macro.txt" #break
#case(240)  #declare Txt_Path="40 - HF_Cylinder macro.txt" #break
#case(250)  #declare Txt_Path="50 - HF_Torus macro.txt" #break
*/

// ------- 20 - mesh and non-CSG shapes/

/*
#case(300)  #declare Txt_Path="00 - disc.txt" #break
#case(310)  #declare Txt_Path="10 - triangle.txt" #break
#case(320)  #declare Txt_Path="20 - smooth_triangle.txt" #break
#case(330)  #declare Txt_Path="30 - polygon.txt" #break
#case(340)  #declare Txt_Path="40 - bicubic_patch.txt" #break

#case(371)  #declare Txt_Path="71 - mesh cube sample.txt" #break
#case(385)  #declare Txt_Path="85 - mesh2 sample.txt" #break

*/ 
// ----
#if (Typ=300)
object{ Raster_Ground }
camera{ angle 26 location < 3.50, 3.00, -4.50> look_at< 0.15, 0.55,  0.15>}
light_source{<2500, 2500,-1000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 2.75, 2.7, 3.5, T_Dark, T_Light, 20, -36,   0.55,0.55,0.75) scale 0.5}
#include concat(In_Path,Sub_Path3,Txt_Path) // "00 - disc.txt"
#end
// ----
#if (Typ=310)
object{ Raster_Ground }
camera{ angle 26 location < 3.50, 3.00, -4.50> look_at< 0.15, 0.55,  0.15>}
light_source{<2500, 2500,-1000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 2.75, 2.7, 3.5, T_Dark, T_Light, 20, -36,   0.55,0.55,0.75) scale 0.5}
#include concat(In_Path,Sub_Path3,Txt_Path) // "10 - triangle.txt"
#end
// ----
#if (Typ=320)
object{ Raster_Ground }
MySky()
camera{ angle 52 location < 5.50, 5.00, -5.50> look_at< 0.5, 1.75,  0.5>}
light_source{<1500, 2500,-2500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 5.50, 3.00, -4.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 3.35, 4.25, 4.5, T_Dark, T_Light, 10, -45,   0.55,0.55,0.85) scale 1}
#include concat(In_Path,Sub_Path3,Txt_Path) // "20 - smooth_triangle.txt"
#end
// ----
#if (Typ=330)
object{ Raster_Ground }
camera{ angle 45 location < 3.50, 3.50, -4.50> look_at< 1.5, 1.20,  0.15>}
light_source{<2500, 2500,-2500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 3.50, 3.00, -4.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 6.75, 4.9, 3.5, T_Dark, T_Light, 15, -21,   0.75,0.75,1.05) scale 0.5}
#include concat(In_Path,Sub_Path3,Txt_Path) // "30 - polygon.txt"
#end
// ----
#if (Typ=340)
object{ Raster_Ground }
camera{ angle 32 location < 5.50, 6.00, -8.50> look_at<-0.0, 1.95,  -1.20>}
light_source{<2500, 2500,-1000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 2.35, 3.2, 3.5, T_Dark, T_Light, 20, -36,   0.55,0.55,0.75) scale 1}
#include concat(In_Path,Sub_Path3,Txt_Path) // "40 - bicubic_patch.txt"
#end
// ----
#if (Typ=371)
object{ Raster_Ground }
camera{ angle 35 location < 5.50, 6.00, -8.50> look_at< 0.5, 1.75,  -1.20>}
light_source{<2500, 2500,-1000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 2.55, 3.2, 8.5, T_Dark, T_Light, 20, -36,   0.75,0.75,1.15) scale 1}
#include concat(In_Path,Sub_Path3,Txt_Path) //71 - mesh cube sample.txt
#end 
#if (Typ=385)
object{ Raster_Ground }
camera{ angle 35 location < 2.50, 4.50, -3.70> look_at< 0.5, 0.0,  1.40>}
light_source{<2500, 2500,-1000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 1.95, 1.8, 3.5, T_Dark, T_Light, 30, -06,   0.45,0.45,0.65) scale 1}
 #include concat(In_Path,Sub_Path3,Txt_Path)//85 - mesh2 sample.txt 
#end 
// --------20 - mesh and non-CSG shapes///
#if (Typ=350)
MySky2(1)
camera{ angle 26 location < 2.50, 3.00, -4.50> look_at< 0.15, 0.63,  0.15>}
light_source{<2500, 2500,-0000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 2.25, 2.8, 6, T_Dark, T_Light, 20, -36,   0.55,0.55,0.75) scale 0.5}
#include concat(In_Path,Sub_Path3,Txt_Path) // "50 - mesh teapot_tri.txt"
#end
// ----
#if (Typ=351)
MySky2(1)
camera{ angle 26 location < 3.50, 3.00, -4.50> look_at< 0.15, 0.55,  0.15>}
light_source{<2500, 2500,-1000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 2.25, 2.7, 6, T_Dark, T_Light, 20, -36,   0.55,0.55,0.75) scale 0.5}
#declare WHITE_KNIGHT_1 = texture { pigment{ color rgb<1,1,1>*0.15 } finish{ phong 1}}
#declare BLACK_ROOK_1   = texture { pigment{ color rgb<1,1,1>*1.10 } finish{ phong 1}}
#include concat(In_Path,Sub_Path3,Txt_Path) // "51 - mesh chess knight and rook.txt"
#end
// ----





// --------25 - blob and fractal/
#if (Typ=430)
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
object{ AxisXYZ_( 3.25, 3.2, 2, T_Dark, T_Light, 0, 0,   0.75,0.75,0.75) scale 0.5}
#include concat(In_Path,Sub_Path4,Txt_Path) // "30 - blob object spheres.txt"
#end
// ----
#if (Typ=431)
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
object{ AxisXYZ_( 3.25, 3.2, 2, T_Dark, T_Light, 0, 0,   0.75,0.75,0.75) scale 0.5}
#include concat(In_Path,Sub_Path4,Txt_Path) // "31 - blob object cylinders.txt"
#end
// ----
#if (Typ=432)
MySky3()
object{ Grass_Ground }
light_source{<-1500,2000,-2500>  color rgb<1,1,1>*0.8}     // sun light
light_source{ < 1.50, 3.00, -10.50> color rgb<0.9,0.9,1>*0.1}  // flash light
camera{ angle 40 location < 3.50, 3.00, -4.50> look_at< 0.75, 0.55,  0.40>}
object{ AxisXYZ_( 5.45, 3.2, 6, T_Dark, T_Light, 10, -25,   0.75,0.85,0.95) scale 0.5}
#include concat(In_Path,Sub_Path4,Txt_Path) // "32 - blob object mixed.txt"
#end
// ----
#if (Typ=433)
MySky3()
object{ Grass_Ground }
light_source{<-1500,2000,-2500>  color rgb<1,1,1>*0.8}     // sun light
light_source{ < 1.50, 3.00, -10.50> color rgb<0.9,0.9,1>*0.1}  // flash light
camera{ angle 40 location < 2.50, 5.50, -5.50> look_at< 0.30, 0.50,  0.10>}
object{ AxisXYZ_( 2.750, 2.0, 4, T_Dark, T_Light, 30, -25,   0.45,0.45,0.75) scale 1}
#include concat(In_Path,Sub_Path4,Txt_Path) // "33 - blob with negative strength.txt"
#end
// ----
#if (Typ=434)
MySky3()
object{ Grass_Ground }
light_source{<-1500,2000,-2500>  color rgb<1,1,1>*0.8}     // sun light
light_source{ < 1.50, 3.00, -10.50> color rgb<0.9,0.9,1>*0.1}  // flash light
camera{ angle 40 location < 5.00, 6.00, -7.50> look_at< 1.75, 1.50,  1.40>}
object{ AxisXYZ_( 4.00, 4.0, 8, T_Dark, T_Light, 10, -15,   0.65,0.65,2.25) scale 1}
#include concat(In_Path,Sub_Path4,Txt_Path) // "34 - blob for loop 3D.txt"
#end
// ----
#if (Typ=440)
MySky2(1)
//#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
camera{ angle 27 location < 3.50, 3.00, -4.50> look_at< 0.05, 0.15,  0.05>}
light_source{<2500, 2500,-2000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 2.8, 2.1, 3.5, T_Dark, T_Light, 20, -45,   0.55,0.55,0.75) scale 0.5}
#include concat(In_Path,Sub_Path4,Txt_Path) // "40 - julia fractal object1.txt"
#end
// ----
#if (Typ=442)
MySky2(1)
//#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
camera{ angle 27 location < 3.50, 3.00, -4.50> look_at< 0.05, 0.15,  0.05>}
light_source{<2500, 2500,-2000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 2.8, 2.1, 3.5, T_Dark, T_Light, 20, -45,   0.55,0.55,0.75) scale 0.5}
#include concat(In_Path,Sub_Path4,Txt_Path) // "42 - julia fractal object2.txt"
#end
// ----
#if (Typ=444)
MySky2(1)
//#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
camera{ angle 27 location < 3.50, 3.00, -4.50> look_at< 0.05, 0.200,  0.05>}
light_source{<2500, 2500,-2000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 2.8, 2.2, 3.5, T_Dark, T_Light, 20, -45,   0.45,0.45,0.65) scale 0.5}
#include concat(In_Path,Sub_Path4,Txt_Path) // "44 - julia fractal object3.txt"
#end
// ----
/*
#case(430)  #declare Txt_Path="30 - blob object spheres.txt" #break
#case(431)  #declare Txt_Path="31 - blob object cylinders.txt" #break
#case(432)  #declare Txt_Path="32 - blob object mixed.txt" #break
#case(433)  #declare Txt_Path="33 - blob with negative strength.txt" #break
#case(434)  #declare Txt_Path="34 - blob for loop 3D.txt" #break
#case(440)  #declare Txt_Path="40 - julia fractal object1.txt" #break
#case(442)  #declare Txt_Path="42 - julia fractal object2.txt" #break
#case(444)  #declare Txt_Path="44 - julia fractal object3.txt" #break
*/


//--------------------------------------------------------------------------------- Isosurfaces
//  Isosurfaces  by_basic_functions
// ----
#if (Typ>=500 & Typ< 800)     // 50 - Basic Scene 05 - Grass with small clouds in sky
#include "10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
#end
#if (Typ=500) #include concat(In_Path,Sub_Path5,Txt_Path) #end
#if (Typ=512) #include concat(In_Path,Sub_Path5,Txt_Path) #end
#if (Typ=513) #include concat(In_Path,Sub_Path5,Txt_Path) #end
#if (Typ=516) #include concat(In_Path,Sub_Path5,Txt_Path) #end
#if (Typ=519) #include concat(In_Path,Sub_Path5,Txt_Path) #end
#if (Typ=520) #include concat(In_Path,Sub_Path5,Txt_Path) #end
#if (Typ=521) #include concat(In_Path,Sub_Path5,Txt_Path) #end
#if (Typ=522) #include concat(In_Path,Sub_Path5,Txt_Path) #end
#if (Typ=524) #include concat(In_Path,Sub_Path5,Txt_Path) #end
#if (Typ=525) #include concat(In_Path,Sub_Path5,Txt_Path) #end
#if (Typ=526) #include concat(In_Path,Sub_Path5,Txt_Path) #end
#if (Typ=527) #include concat(In_Path,Sub_Path5,Txt_Path) #end
#if (Typ=528) #include concat(In_Path,Sub_Path5,Txt_Path) #end
// ----
/*
#case(500)  #declare Txt_Path="00 - isosurface syntax.txt" #break
#case(510)  #declare Txt_Path="10 - Isosurface house.txt" #break
#case(512)  #declare Txt_Path="12 - Isosurface cristal.txt" #break
#case(513)  #declare Txt_Path="13 - Isosurface cylinder by sqrt.txt" #break
#case(516)  #declare Txt_Path="16 - Isosurface hyperboloid.txt" #break
#case(519)  #declare Txt_Path="19 - Isosurface cross1.txt" #break
#case(520)  #declare Txt_Path="20 - Isosurface cross1 min_blobed.txt" #break
#case(521)  #declare Txt_Path="21 - Isosurface cross2.txt" #break
#case(522)  #declare Txt_Path="22 - Isosurface sine.txt" #break
#case(524)  #declare Txt_Path="24 - Isosurface y sine concentric.txt" #break
#case(525)  #declare Txt_Path="25 - Isosurface y sine cylinder.txt" #break
#case(526)  #declare Txt_Path="26 - Isosurface sine sphere.txt" #break
#case(527)  #declare Txt_Path="27 - Isosurface sine sphere y.txt" #break
#case(528)  #declare Txt_Path="28 - Isosurface sine sphere double sine wave.txt" #break
*/
// Isosurfaces_by_function 
#if (Typ=640) #include concat(In_Path,Sub_Path6,Txt_Path) #end
#if (Typ=641) #include concat(In_Path,Sub_Path6,Txt_Path) #end
#if (Typ=642) #include concat(In_Path,Sub_Path6,Txt_Path) #end
#if (Typ=643) #include concat(In_Path,Sub_Path6,Txt_Path) #end

#if (Typ=650) #include concat(In_Path,Sub_Path6,Txt_Path) #end
#if (Typ=651) #include concat(In_Path,Sub_Path6,Txt_Path) #end
#if (Typ=652) #include concat(In_Path,Sub_Path6,Txt_Path) #end
#if (Typ=653) #include concat(In_Path,Sub_Path6,Txt_Path) #end
#if (Typ=655) #include concat(In_Path,Sub_Path6,Txt_Path) #end
#if (Typ=656) #include concat(In_Path,Sub_Path6,Txt_Path) #end
#if (Typ=657) #include concat(In_Path,Sub_Path6,Txt_Path) #end
#if (Typ=658) #include concat(In_Path,Sub_Path6,Txt_Path) #end
#if (Typ=659) #include concat(In_Path,Sub_Path6,Txt_Path) #end
#if (Typ=660) #include concat(In_Path,Sub_Path6,Txt_Path) #end
/*
#case(640)  #declare Txt_Path="40 - Isosurface f_rounded_box.txt" #break
#case(641)  #declare Txt_Path="41 - Isosurface f_pillow.txt" #break
#case(642)  #declare Txt_Path="42 - Isosurface f_superellipsoid.txt" #break
#case(643)  #declare Txt_Path="43 - Isosurface f_torus.txt" #break
#case(644)  #declare Txt_Path="44 - Isosurface f_torus2.txt" #break
#case(650)  #declare Txt_Path="50 - Isosurface f_heart.txt" #break
#case(651)  #declare Txt_Path="51 - Isosurface f_comma.txt" #break
#case(652)  #declare Txt_Path="52 - Isosurface f_dupin_cyclid.txt" #break
#case(653)  #declare Txt_Path="53 - Isosurface f_helix1.txt" #break
#case(655)  #declare Txt_Path="55 - Isosurface f_helix2.txt" #break
#case(656)  #declare Txt_Path="56 - Isosurface f_spiral.txt" #break
#case(657)  #declare Txt_Path="57 - Isosurface f_mesh1.txt" #break
#case(658)  #declare Txt_Path="58 - Isosurface f_flange_cover.txt" #break
#case(659)  #declare Txt_Path="59 - Isosurface f_spikes.txt" #break
#case(660)  #declare Txt_Path="60 - Isosurface f_spikes_2d.txt" #break
*/
//  Isosurfaces by pattern_functions 
#if (Typ=781) #include concat(In_Path,Sub_Path7,Txt_Path) #end
#if (Typ=782) #include concat(In_Path,Sub_Path7,Txt_Path) #end
#if (Typ=785) #include concat(In_Path,Sub_Path7,Txt_Path) #end
#if (Typ=787) #include concat(In_Path,Sub_Path7,Txt_Path) #end
#if (Typ=788) #include concat(In_Path,Sub_Path7,Txt_Path) #end
#if (Typ=789) #include concat(In_Path,Sub_Path7,Txt_Path) #end
#if (Typ=791) #include concat(In_Path,Sub_Path7,Txt_Path) #end
#if (Typ=792) #include concat(In_Path,Sub_Path7,Txt_Path) #end
#if (Typ=794) #include concat(In_Path,Sub_Path7,Txt_Path) #end
#if (Typ=796) #include concat(In_Path,Sub_Path7,Txt_Path) #end
#if (Typ=797) #include concat(In_Path,Sub_Path7,Txt_Path) #end
#if (Typ=798) #include concat(In_Path,Sub_Path7,Txt_Path) #end
/*
#case(781)  #declare Txt_Path="81 - Isosurface f_leopard.txt" #break
#case(782)  #declare Txt_Path="82 - Isosurface f_leopard.txt" #break
#case(785)  #declare Txt_Path="85 - Isosurface Round_Box - f_crackle.txt" #break
#case(787)  #declare Txt_Path="87 - Isosurface sphere - f_noise3d .txt" #break
#case(788)  #declare Txt_Path="88 - Isosurface Round_Box - f_agate.txt" #break
#case(789)  #declare Txt_Path="89 - Isosurface Round_Box - f_agate.txt" #break
#case(791)  #declare Txt_Path="91 - Isosurface bozo 3d.txt" #break
#case(792)  #declare Txt_Path="92 - Isosurface Sphere - f_noise3d.txt" #break
#case(793)  #declare Txt_Path="93 - Isosurface Sphere - f_noise3d.txt" #break
#case(794)  #declare Txt_Path="94 - Isosurface Sphere - f_noise3d.txt" #break
#case(796)  #declare Txt_Path="96 - Isosurface Sphere - f_noise3d.txt" #break
#case(797)  #declare Txt_Path="97 - Isosurface Sphere - pigment.txt" #break
#case(798)  #declare Txt_Path="98 - Isosurface f_sphere - wood.txt" #break
*/


// 40 - Polynomial Quartic"
/*
#case(801)  #declare Txt_Path="01 - Glob_5.txt" #break
#case(802)  #declare Txt_Path="02 - Twin_Glob.txt" #break
#case(803)  #declare Txt_Path="03 - Sinsurf.txt" #break

#case(813)  #declare Txt_Path="13 - Bicorn.txt" #break
#case(814)  #declare Txt_Path="14 - Crossed_Trough.txt" #break
#case(815)  #declare Txt_Path="15 - Cubic_Cylinder.txt" #break
#case(816)  #declare Txt_Path="16 - Cubic_Saddle_1.txt" #break
#case(817)  #declare Txt_Path="17 - Devils_Curve.txt" #break
#case(818)  #declare Txt_Path="18 - Folium.txt" #break
#case(819)  #declare Txt_Path="19 - Helix.txt" #break
#case(820)  #declare Txt_Path="20 - Helix_1.txt" #break
#case(821)  #declare Txt_Path="21 - Hyperbolic_Torus_40_12.txt" #break
#case(822)  #declare Txt_Path="22 - Lemniscate.txt" #break
#case(823)  #declare Txt_Path="23 - Quartic_Loop_1.txt" #break
#case(824)  #declare Txt_Path="24 - Monkey_Saddle.txt" #break
#case(825)  #declare Txt_Path="25 - Parabolic_Torus_40_12.txt" #break
#case(826)  #declare Txt_Path="26 - Piriform.txt" #break
#case(827)  #declare Txt_Path="27 - Quartic_Paraboloid.txt" #break
#case(828)  #declare Txt_Path="28 - Quartic_Cylinder.txt" #break
#case(829)  #declare Txt_Path="29 - Steiner_Surface.txt" #break
#case(830)  #declare Txt_Path="30 - Torus_40_12.txt" #break
#case(831)  #declare Txt_Path="31 - Witch_Hat.txt" #break
*/
#if (Typ=801)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----
#if (Typ=802)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----
#if (Typ=803)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----
#if (Typ=813)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----
#if (Typ=814)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----
#if (Typ=815)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----
#if (Typ=816)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----
#if (Typ=817)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----
#if (Typ=818)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----
#if (Typ=819)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----
#if (Typ=820)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----
#if (Typ=821)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----
#if (Typ=822)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----
#if (Typ=823)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----
#if (Typ=824)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----
#if (Typ=825)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----
#if (Typ=826)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----
#if (Typ=827)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----
#if (Typ=828)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----
#if (Typ=829)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----
#if (Typ=830)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----
#if (Typ=831)
#include "10 - Ready made scenes/M0 - Basic Scene M0 - DarkBlueSky + Axes.txt"  #include concat(In_Path,Sub_Path8,Txt_Path) #end
// ----

// 50 - Parametric"
/*
#case(900)  #declare Txt_Path="00 - parametric sample.txt" #break
#case(910)  #declare Txt_Path="10 - parametric flower.txt" #break
#case(915)  #declare Txt_Path="15 - parametric ornamental.txt" #break
#case(940)  #declare Txt_Path="40 - parametric sin cylinder.txt" #break
#case(945)  #declare Txt_Path="45 - parametric sin surface.txt" #break
#case(950)  #declare Txt_Path="50 - parametric screw.txt" #break
#case(951)  #declare Txt_Path="51 - parametric conical spiral.txt" #break
#case(952)  #declare Txt_Path="52 - parametric Dini surface.txt" #break
*/
#if (Typ=900)
MySky2(1)
camera{ angle 26 location < 3.50, 2.00, -3.50> look_at< 0.05, 0.40,  0.05>}
light_source{<2500, 2500,-2000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 2.5, 2.0, 3, T_Dark, T_Light, 10, -45,   0.45,0.45,0.55) scale 0.5}
#include concat(In_Path,Sub_Path9,Txt_Path) // "00 - parametric sample.txt"
#end
// ----
#if (Typ=910)
MySky2(1)
camera{ angle 33 location < 3.50, 3.00, -3.50> look_at< 0.05, 0.90,  0.05>}
light_source{<2500, 2500,-2000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 1.15, 3.55, 2, T_Dark, T_Light, 10, -45,   0.65,0.55,0.75) scale 0.5}
#include concat(In_Path,Sub_Path9,Txt_Path) // "10 - parametric flower.txt"
#end
// ----
#if (Typ=920)
MySky2(1)
camera{ angle 26 location < 3.50, 2.00, -3.50> look_at< 0.05, 0.20,-0.20>}
light_source{<2500, 2500,-2000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 2.1, 1.5, 3, T_Dark, T_Light, 10, -45,   0.45,0.45,0.55) scale 0.5}
#include concat(In_Path,Sub_Path9,Txt_Path) // "20 - parametric ornamental.txt"
#end
// ----
#if (Typ=940)
MySky2(1)
camera{ angle 26 location < 3.50, 2.00, -3.50> look_at< 0.05, 0.05,  0.05>}
light_source{<2500, 2500,-2000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 2.5, 1.5, 4, T_Dark, T_Light, 10, -45,   0.45,0.45,0.55) scale 0.5}
#include concat(In_Path,Sub_Path9,Txt_Path) // "40 - parametric sin cylinder.txt"
#end
// ----
#if (Typ=945)
MySky2(1)
camera{ angle 21 location < 2.50, 2.50, -2.50> look_at< 0.05, 0.10,  0.05>}
light_source{<2500, 2500,-2000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 3, 2.5, 4, T_Dark, T_Light, 10, -45,   0.55,0.55,0.65) scale 0.25}
#include concat(In_Path,Sub_Path9,Txt_Path) // "45 - parametric sin surface.txt"
#end
// ----
#if (Typ=950)
MySky2(1)
camera{ angle 28 location < 2.50, 1.50, -2.50> look_at< 0.05, 0.45,  0.05>}
light_source{<2500, 2500,-2000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 2.5, 4.0, 4, T_Dark, T_Light, 10, -45,   0.55,0.55,0.65) scale 0.25}
#include concat(In_Path,Sub_Path9,Txt_Path) // "50 - parametric screw.txt"
#end
// ----
#if (Typ=951)
MySky2(1)
camera{ angle 30 location < 2.50, 1.50, -1.50> look_at< 0.05, 0.45,  0.05>}
light_source{<2500, 2500,-2000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 1.5, 3.5, 2.1, T_Dark, T_Light, 10, -45,   0.45,0.45,0.55) scale 0.25}
#include concat(In_Path,Sub_Path9,Txt_Path) // "51 - parametric conical spiral.txt"
#end
// ----
#if (Typ=952)
MySky2(1)
camera{ angle 38 location < 2.50, 1.50, -2.50> look_at< 0.05, 0.45,  0.05>}
light_source{<2500, 2500,-2000> color rgb<1,1,1>*0.8}     // sun light
light_source{< 2.65,0.00,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 3.00, 5.0, 4.1, T_Dark, T_Light, 10, -45,   0.75,0.75,0.95) scale 0.25}
#include concat(In_Path,Sub_Path9,Txt_Path) // "52 - parametric Dini surface.txt"
#end
// ----

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------