// Insert menu illustration scene "Loops_sweep_spline.pov"
// Author Friedrich A. Lohmueller, June-2012
#version 3.7;
global_settings{ assumed_gamma 1.0 } 

#default{ finish{ ambient 0.1 diffuse 0.9 }}
#include "shapes.inc"
#include "shapes2.inc"
#include "colors.inc"
#include "textures.inc"
#include "stones.inc"
#include "glass.inc"

// #while + #for loops, sphere_sweep, spline curves

#declare In_Path  = "70 - Loop, sphere_sweep, spline/"
#declare In_Path2 = "70 - Loop, sphere_sweep, spline/"

 //#declare Typ = 107; // for tests

#switch (Typ)  //--------------------------------------------------------- 
// #while loops and #for loops/
#case(100)  #declare Txt_Path="00 - while loop linear.txt" #break
#case(102)  #declare Txt_Path="02 - while loop quadratic 2D nested.txt" #break
#case(103)  #declare Txt_Path="03 - while loop cubic 3D nested.txt" #break

#case(104)  #declare Txt_Path="04 - while loop circular.txt" #break
#case(107)  #declare Txt_Path="07 - while loop linear helix.txt" #break
#case(108)  #declare Txt_Path="08 - while loop circular single helix.txt" #break

#case(113)  #declare Txt_Path="13 - while loop spiral.txt" #break
#case(114)  #declare Txt_Path="14 - while loop snail.txt" #break

#case(120)  #declare Txt_Path="20 - for_loop linear.txt" #break
#case(122)  #declare Txt_Path="22 - for_loop quadratic_2D nested.txt" #break
#case(124)  #declare Txt_Path="24 - for_loop cubic_3D nested.txt" #break


// 42 - sphere_sweep and spline_curves/
#case(240)  #declare Txt_Path="40 - sphere_sweep linear_spline.txt" #break
#case(241)  #declare Txt_Path="41 - sphere_sweep cubic_spline.txt" #break
#case(242)  #declare Txt_Path="42 - sphere_sweep b_spline.txt" #break

#case(251)  #declare Txt_Path="51 - spline linear_spline.txt" #break
#case(252)  #declare Txt_Path="52 - spline quadratic_spline.txt" #break
#case(253)  #declare Txt_Path="53 - spline cubic_spline.txt" #break
#case(254)  #declare Txt_Path="54 - spline natural_spline.txt" #break
#case(255)  #declare Txt_Path="55 - spline closed natural_spline.txt" #break


#end // of '#switch (Typ)' ----------------------------------------------- 
//------------------------------------------------------------------------ 
//------------------------------ the Axes --------------------------------
#macro Axis_( AxisLen, Dark_Texture,Light_Texture)
 union{
    cylinder { <0,-AxisLen,0>,<0,AxisLen,0>,0.05
               texture{checker texture{Dark_Texture }
                               texture{Light_Texture}
                       translate<0.1,0,0.1>}
             }
    cone{<0,AxisLen,0>,0.2,<0,AxisLen+0.7,0>,0
          texture{Dark_Texture}
         }
     } 
#end // of macro 
//------------------------------------------------------------------------
#macro AxisXYZ_( AxisLenX, AxisLenY, AxisLenZ, Tex_Dark, Tex_Light,
                 Rot_X,Rot_Y, ScaleX,ScaleY,ScaleZ)
//--------------------- drawing of 3 Axes --------------------------------
union{
#if (AxisLenX != 0)
 object { Axis_(AxisLenX, Tex_Dark, Tex_Light)   rotate< 0,0,-90>}// x-Axis
 text   { ttf "arial.ttf",  "x",  0.15,  0  texture{Tex_Dark}
         rotate<Rot_X,Rot_Y,0> scale ScaleX translate <AxisLenX+0.05,0.4,-0.10> no_shadow}
#end // of #if
#if (AxisLenY != 0)
 object { Axis_(AxisLenY, Tex_Dark, Tex_Light)   rotate< 0,0,  0>}// y-Axis
 text   { ttf "arial.ttf",  "y",  0.15,  0  texture{Tex_Dark}
          rotate<Rot_X,0,0> scale ScaleY translate <-0.55,AxisLenY+0.15,-0.20> rotate<0,Rot_Y,0> no_shadow}
#end // of #if
#if (AxisLenZ != 0)
 object { Axis_(AxisLenZ, Tex_Dark, Tex_Light)   rotate<90,0,  0>}// z-Axis
 text   { ttf "arial.ttf",  "z",  0.15,  0  texture{Tex_Dark}
          rotate<Rot_X,Rot_Y,0> scale ScaleZ translate <-0.35,0.3,AxisLenZ+0.10> no_shadow}
#end // of #if
} // end of union
#end// of macro "AxisXYZ( ... )" // end AxisXYZ_ macro
//------------------------------------------------------------------------
#declare T_Dark  =                   // axis textures
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
//------------------------------------------------------------------------
#declare Blue_Background =
sky_sphere{ pigment{ gradient <0,1,0>
                     color_map{ [0   color rgb<0.24,0.34,0.56>*1.5]
                                [0.5 color rgb<0.24,0.34,0.56>*0.7]
                                [0.5 color rgb<0.24,0.34,0.56>*0.7]
                                [1.0 color rgb<0.24,0.34,0.56>*1.5]
                              }

                      rotate< 0,0, 0>

                     scale 2 }
           } // end of sky_sphere

#macro Small_Clouds ()
// sky --------------------------------------------------------------
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
      distance   250
      color      White
      fog_offset 0.1
      fog_alt    2.5
      turbulence 1.8
    }
#end //-------------------------------------------------------
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
//------------------------------------------------------------------------
#declare Grass_Ground  =
plane { <0,1,0>, 0
        texture{ pigment{ color rgb<0.35,0.65,0.0>*0.72 }
                 normal { bumps 0.75 scale 0.015 }
                 finish { phong 0.1 }

               } // end of texture
      } // end of plane
//------------------------------------------------------------------------
#declare Sand_Ground  =
plane{ <0,1,0>, 0
       texture{ pigment{ color rgb <1.00,0.95,0.8>}
                normal { bumps 0.75 scale 0.025  }
                finish { phong 0.1 }
              } // end of texture
     } // end of plane
//------------------------------------------------------------------------
#declare Desert_Ground  =
plane{ <0,1,0>, 0
       texture{ pigment{ color rgb <0.825,0.57,0.35>}
                normal { bumps 0.75 scale 0.025  }
                finish { phong 0.1 }
              } // end of texture
     } // end of plane

//------------------------------------------------------------------------





//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
#if (Typ=100) // In_Path,"00 - while loop linear.txt"
Small_Clouds ()
object{ Grass_Ground }
camera{ angle 50 location < 1.50, 1.50, -3.00> look_at< 0.0, 0.25,  3.00> right x*image_width/image_height}
light_source{<1500, 2500,-2500> color rgb<1,1,1>*0.9}     // sun light
light_source{<1.50, 1.50,-3.00> color rgb<0.9,0.9,1>*0.1 shadowless}  // flash
object{ AxisXYZ_( 2.85, 2.75, 20, T_Dark, T_Light,10,-30,   0.45,0.45,0.85) scale 0.5}
#include concat(In_Path,Txt_Path)
#end

#if (Typ=102)
Small_Clouds ()
object{ Desert_Ground }
camera{ angle 75 location < 0, 1.00, -3.00> look_at< 0.0,1.00,0> right x*image_width/image_height}
light_source{<-1500, 2500,-2500> color rgb<1,1,1>*0.9}     // sun light
light_source{<    0, 1.00,-3.00> color rgb<0.9,0.9,1>*0.1 shadowless}  // flash
object{ AxisXYZ_( 3.85, 2.75, 20, T_Dark, T_Light, 0, 0,   0.45,0.45,0.85) scale 0.5}
#include concat(In_Path,Txt_Path)
#end

#if (Typ=103)
Small_Clouds ()
object{ Sand_Ground }
camera{ angle 75 location < 0.00, 1.00, -3.00> look_at< 0.0,1.00,0> right x*image_width/image_height}
light_source{<-1500, 2500,-2500> color rgb<1,1,1>*0.9}     // sun light
light_source{< 0.00, 1.00,-3.00> color rgb<0.9,0.9,1>*0.1 shadowless}  // flash
#include concat(In_Path,Txt_Path)
#end

#if (Typ=104)//  circular
Small_Clouds ()
object { Sand_Ground  }
camera{ angle 24 location < 4.50, 3.00, -4.50> look_at< 0.05, 0.45,  0.00> right x*image_width/image_height}
light_source{<2500, 2500,-0000> color rgb<1,1,1>*0.9}     // sun light
light_source{<4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1 shadowless}  // flash
object{ AxisXYZ_( 2.85, 2.25, 4.5, T_Dark, T_Light,30,-45,   0.45,0.45,0.55) scale 0.5}
#include concat(In_Path,Txt_Path)
#end

#if (Typ=107)//  circular 2d
Small_Clouds ()
object { Sand_Ground  }
camera{ angle 45 location < 3.50, 2.50, -3.50> look_at< 0.05, 0.85,  0.00> right x*image_width/image_height}
light_source{<2500, 2500,-0000> color rgb<1,1,1>*0.9}     // sun light
light_source{<3.50, 2.50,-3.50> color rgb<0.9,0.9,1>*0.1 shadowless}  // flash
object{ AxisXYZ_( 2.85, 3.95, 5.5, T_Dark, T_Light,20,-50,   0.55,0.55,0.65) scale 0.5}
#include concat(In_Path,Txt_Path)
#end


#if (Typ=108)
object { Sand_Ground  }
Small_Clouds ()
camera{ angle 72 location < 2.00, 7.00, -6.50> look_at< 0.10, 0.90,  0.00> right x*image_width/image_height}
light_source{<-1500, 1500,-2500> color rgb<1,1,1>*0.9}     // sun light
light_source{< 2.00, 7.00,-6.50> color rgb<0.9,0.9,1>*0.1}  // flash light
#include concat(In_Path,Txt_Path)
#end

#if (Typ=113) // 13 - while loop spiral.txt
Small_Clouds ()
object{ Desert_Ground }
camera{ angle 64 location < 0, 1.00, -3> look_at< 0.00, 1,0.00> right x*image_width/image_height}
light_source{<-1500, 2500,-2500> color rgb<1,1,1>*0.9}     // sun light
light_source{<    0, 1.00,   -3> color rgb<0.9,0.9,1>*0.1 shadowless}  // flash
#include concat(In_Path,Txt_Path)
#end

#if (Typ=114) //  
object { Sand_Ground  }
Small_Clouds ()
camera{ angle 60 location < 2.00, 2.00,-2.00> look_at< 0.30, 0.95,  0.00> right x*image_width/image_height}
light_source{<-1000, 1800,-2500> color rgb<1,1,1>*0.9}     // sun light
light_source{< 2.00, 2.00,-2.00> color rgb<0.9,0.9,1>*0.1}  // flash light
#include concat(In_Path,Txt_Path)
#end
 

// #for loops
#if (Typ=120) // 20 - for_loop linear.txt
object { Raster_Ground }
Small_Clouds ()
camera{ angle 46 location < 7.00, 4.70,-5.00> look_at< 0, 0.7,  3.00> right x*image_width/image_height}
light_source{<-2000, 2000,-2500> color rgb<1,1,1>*0.9}     // sun light
light_source{< 2.65, 1.50,-2.10> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 2.75, 3.5,  14, T_Dark, T_Light,20,-45,   0.45,0.45,0.95) scale 1}
#include concat(In_Path,Txt_Path)
#end

// #for loops
#if (Typ=122) // 20 - for_loop linear.txt
object { Raster_Ground }
Small_Clouds ()
camera{ angle 46 location < 8.50, 7.70,-7.00> look_at< 3.5, 0.5,  2.00> right x*image_width/image_height}
light_source{<-2000, 2000,-2500> color rgb<1,1,1>*0.9}     // sun light
light_source{< 8.50, 7.70,-7.00> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 7.6, 2.5,  10, T_Dark, T_Light,20,-45,   0.65,0.65,1.25) scale 1}
#include concat(In_Path,Txt_Path)
#end


// #for loops
#if (Typ=124) // 20 - for_loop linear.txt
object { Raster_Ground }
Small_Clouds ()
camera{ angle 64 location < 6.50, 7.00,-5.00> look_at< 2.5, 2.0,  1.00> right x*image_width/image_height}
light_source{<-2000, 2000,-2500> color rgb<1,1,1>*0.9}     // sun light
light_source{< 6.50, 7.00,-5.00> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 5.2, 4.75,  15, T_Dark, T_Light,30,-45,   0.65,0.65,1.25) scale 1}
#include concat(In_Path,Txt_Path)
#end


// In_Path2,"42 - sphere_sweep and spline_curves/"
/*// 42 - sphere_sweep and spline_curves/
#case(240)  #declare Txt_Path="40 - sphere_sweep linear_spline.txt" #break
#case(241)  #declare Txt_Path="41 - sphere_sweep cubic_spline.txt" #break
#case(242)  #declare Txt_Path="42 - sphere_sweep b_spline.txt" #break

#case(251)  #declare Txt_Path="51 - spline linear_spline.txt" #break
#case(252)  #declare Txt_Path="52 - spline quadratic_spline.txt" #break
#case(253)  #declare Txt_Path="53 - spline cubic_spline.txt" #break
#case(254)  #declare Txt_Path="54 - spline natural_spline.txt" #break
#case(255)  #declare Txt_Path="55 - spline closed natural_spline.txt" #break
*/
// --- sphere_sweep
#if (Typ=240) // 40 - sphere_sweep linear_spline.txt
Small_Clouds ()
object{ Desert_Ground }
camera{ angle 48 location < 0.00, 1.00, -3.00> look_at< 0, 0.9, 0> right x*image_width/image_height}
light_source{<-1500, 2500, -2500> color rgb<1,1,1>*0.9}     // sun light
light_source{< 0.00, 1.00, -3.00> color rgb<0.9,0.9,1>*0.1 shadowless}  // flash
#include concat(In_Path2,Txt_Path)
#end
// ---
#if (Typ=241) // 40 - sphere_sweep linear_spline.txt
Small_Clouds ()
object{ Desert_Ground }
camera{ angle 48 location < 0.00, 1.00, -3.00> look_at< 0, 0.9, 0> right x*image_width/image_height }
light_source{<-1500, 2500, -2500> color rgb<1,1,1>*0.9}     // sun light
light_source{< 0.00, 1.00, -3.00> color rgb<0.9,0.9,1>*0.1 shadowless}  // flash
#include concat(In_Path2,Txt_Path)
#end
// ---
#if (Typ=242) // 40 - sphere_sweep linear_spline.txt
Small_Clouds ()
object{ Desert_Ground }
camera{ angle 44 location < 0.00, 1.00, -3.00> look_at< 0, 0.9, 0> right x*image_width/image_height}
light_source{<-1500, 2500, -2500> color rgb<1,1,1>*0.9}     // sun light
light_source{< 0.00, 1.00, -3.00> color rgb<0.9,0.9,1>*0.1 shadowless}  // flash
#include concat(In_Path2,Txt_Path)
#end
// ---  spline curves
#if (Typ=251) // 40 - sphere_sweep linear_spline.txt
Small_Clouds ()
object{ Sand_Ground }
camera{ angle 50 location < 0.00, 1.00, -3.00> look_at< 0, 1.05, 0> right x*image_width/image_height}
light_source{<-1500, 2500, -2500> color rgb<1,1,1>*0.9}     // sun light
light_source{< 0.00, 1.00, -3.00> color rgb<0.9,0.9,1>*0.1 shadowless}  // flash
#include concat(In_Path2,Txt_Path)
#end
// ---
#if (Typ=252) // 40 - sphere_sweep linear_spline.txt
Small_Clouds ()
object{ Sand_Ground }
camera{ angle 50 location < 0.00, 1.00, -3.00> look_at< 0, 1.05, 0> right x*image_width/image_height}
light_source{<-1500, 2500, -2500> color rgb<1,1,1>*0.9}     // sun light
light_source{< 0.00, 1.00, -3.00> color rgb<0.9,0.9,1>*0.1 shadowless}  // flash
#include concat(In_Path2,Txt_Path)
#end
// ---
#if (Typ=253) // 40 - sphere_sweep linear_spline.txt
Small_Clouds ()
object{ Sand_Ground }
camera{ angle 50 location < 0.00, 1.00, -3.00> look_at< 0, 1.05, 0> right x*image_width/image_height}
light_source{<-1500, 2500, -2500> color rgb<1,1,1>*0.9}     // sun light
light_source{< 0.00, 1.00, -3.00> color rgb<0.9,0.9,1>*0.1 shadowless}  // flash
#include concat(In_Path2,Txt_Path)
#end
// ---
#if (Typ=254) // 40 - sphere_sweep linear_spline.txt
Small_Clouds ()
object{ Grass_Ground }
camera{ angle 60 location < 0.00, 1.00, -3.00>look_at< 0, 1, 0> right x*image_width/image_height}
light_source{<-1500, 2500, -2500> color rgb<1,1,1>*0.9}     // sun light
light_source{< 0.00, 1.00, -3.00> color rgb<0.9,0.9,1>*0.1 shadowless}  // flash
#include concat(In_Path2,Txt_Path)
#end
// ---
#if (Typ=255) // 40 - sphere_sweep linear_spline.txt
Small_Clouds ()
object{ Grass_Ground }
camera{ angle 70 location < 0.00, 1.00, -3.00> look_at< 0, 0.95, 0> right x*image_width/image_height}
light_source{<-1500, 2500, -2500> color rgb<1,1,1>*0.9}     // sun light
light_source{< 0.00, 1.00, -3.00> color rgb<0.9,0.9,1>*0.1 shadowless}  // flash
#include concat(In_Path2,Txt_Path)
#end