// Insert menu illustration scene "Random.pov"
// Author Friedrich A. Lohmueller, June-2012 
// update Feb-2013
#version 3.7;
global_settings{ assumed_gamma 1.0 } 

#default{ finish{ ambient 0.1 diffuse 0.9 }}
#include "shapes.inc"
#include "shapes2.inc"
#include "colors.inc"
#include "textures.inc"
#include "stones.inc"
#include "glass.inc"
#include "transforms.inc"
#include "math.inc"
#include "functions.inc"
// #while + #for loops, sphere_sweep, spline curves

#declare In_Path  = "60 - Transformations/"

  //#declare Typ =13; // for tests

#switch (Typ)  //----------------------------------------------------------
// 60 - Transformations//
#case(10)  #declare Txt_Path="10 - matrix shear x to y.txt" #break
#case(11)  #declare Txt_Path="11 - matrix shear x to z.txt" #break
#case(12)  #declare Txt_Path="12 - matrix shear y to x.txt" #break
#case(13)  #declare Txt_Path="13 - matrix shear y to z.txt" #break
#case(14)  #declare Txt_Path="14 - matrix shear z to x.txt" #break
#case(15)  #declare Txt_Path="15 - matrix shear z to y.txt" #break

#case(20)  #declare Txt_Path="20 - Shear_Trans example.txt" #break
#case(21)  #declare Txt_Path="21 - Matrix_Trans example.txt" #break
#case(22)  #declare Txt_Path="22 - Axial_Scale_Trans.txt" #break
#case(23)  #declare Txt_Path="23 - Rotate_Around_Trans.txt" #break
#case(24)  #declare Txt_Path="24 - Axis_Rotate_Trans.txt" #break
#case(25)  #declare Txt_Path="25 - Reorient_Trans.txt" #break
#case(26)  #declare Txt_Path="26 - vaxis_rotate.txt" #break
#case(27)  #declare Txt_Path="27 - vrotate.txt" #break

#end // of '#switch (Typ)' ------------------------------------------------
//-------------------------------------------------------------------------

//---------------------------


// sun -------------------------------------------------------------------
light_source{<-1500,2500,-2500> color White}
// sky -------------------------------------------------------------------
sky_sphere{ pigment{ gradient <0,1,0>
                     color_map{ [0   color rgb<1,1,1>         ]//White
                                [0.4 color rgb<0.14,0.14,0.56>*0.7]//~Navy
                                [0.6 color rgb<0.14,0.14,0.56>*0.7]//~Navy
                                [1.0 color rgb<1,1,1>         ]//White
                              }
                     scale 2 }
           } // end of sky_sphere
//------------------------------------------------------------------------

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
 pigment{ color rgb<1,0.45,0>}
 finish { phong 1}
}
#declare T_Light =
texture {
 pigment{ color rgb<1,1,1>}
 finish { phong 1}
}
//------------------------------------------------------------------------
// ground -----------------------------------------------------------------
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


plane { <0,1,0>, 0    // plane with layered textures
        texture { pigment{color White*1.1}
                  finish {ambient 0.45 diffuse 0.85}}
        texture { Raster(RasterScale,RasterHalfLine ) rotate<0,0,0> }
        texture { Raster(RasterScale,RasterHalfLineZ) rotate<0,90,0>}
        rotate<0,0,0>
      }
//------------------------------------------------ end of squared plane XZ
//-----------------------------------------------  macro "Vector(Start,End,Radius)"!
#macro Vector(P_start,P_end, R_Vector)
union{

cylinder{ P_start, P_end - ( vnormalize(P_end - P_start)*9.5*R_Vector), R_Vector  }
cone    { P_end - ( vnormalize(P_end - P_start)*10*R_Vector), 3*R_Vector, P_end, 0 }
}// end of union
#end //-------------------------------------------------------------------------- end of macro

//--------------------------------------------------- Segment_of_CylinderRing macro
#macro  Segment_of_CylinderRing ( R_out, R_in, Height, Segment_Angle_)
#local Segment_Angle = Segment_Angle_;
#local D = 0.00001; // just a little bit

 #if (Height = 0 ) #local  Height = D; #end
 #if (Height < 0 ) #local  D = -D; #end

 #if (R_out < R_in) #local X=R_out; #local R_out=R_in; #local R_in=X; #end
 #if (R_in <= 0) #local R_in = D; #end

 #if (Segment_Angle < 0)
      #local Negativ_Flag = 1;
      #local Segment_Angle = -Segment_Angle;
 #else
      #local Negativ_Flag = 0;
 #end


 #if (Segment_Angle >= 360) #local Segment_Angle = mod (Segment_Angle, 360); #end

 intersection{
   cylinder { <0,0,0>,<0,Height,0>, R_out
            } // end of outer cylinder  ----------
   cylinder { <0,-D,0>,<0,Height+D,0>, R_in
              inverse
            } // end of inner cylinder  ----------


  #if (Segment_Angle > 0) // ------------------------------------------------------
  #if (Segment_Angle >= 180)
  union{
  #end // then use union!

   box { <-R_out+D,-D,0>,< R_out+D, Height+D, R_out+D>
         rotate<0,0,0>
       }// end of box

   box { <-R_out+D,-D,-R_out+D>,< R_out+D, Height+D,0>
         rotate<0,  Segment_Angle,0>
       }// end of box

  #if (Segment_Angle >= 180)
   } // end of union
  #end // end of union, if union is used!

 #if (Negativ_Flag = 1)  rotate<0,-Segment_Angle,0>   #end
 scale<-1,1,-1>
 #end // of "#if (Segment_Angle > 0)" --------------------------------------------

} // end of intersection


#end // end of macro ------------------------- end of Segment_of_CylinderRing macro ------

// ---------------------------
// ---------------------------
// ---------------------------
// ---------------------------
// ---------------------------
// Base wireframe
#declare Base_Wire_Frame_Box =
object { // Wire_Box(A, B, WireRadius, UseMerge)
         Wire_Box(<-0.035,0,-0.035>,<1.035,1.035,1.035>, 0.035   , 0)

         texture{ pigment{ color rgb<1,0.65,0>} finish{ phong 1}
                }
         scale<1,1,1>  rotate<0, 0,0> translate<0,0.00,0>
       } // ---------------------------------------------
// base shape
#declare Base_Box = box{<0,0,0>,<1,1,1>
 texture{ pigment{ color rgb<0.7,0.5,1>*0.9}
                }
         scale<1,1,1>  rotate<0, 0,0> translate<0,0,0>
       } // ---------------------------------------------

//------------------------------------------------------------------------------------
// Shear_Transform.pov/     50 - Shearing and transform
/*
#case(10)  #declare Txt_Path="10 - matrix shear x to y.txt" #break
#case(11)  #declare Txt_Path="11 - matrix shear x to z.txt" #break
#case(12)  #declare Txt_Path="12 - matrix shear y to x.txt" #break
#case(13)  #declare Txt_Path="13 - matrix shear y to z.txt" #break
#case(14)  #declare Txt_Path="14 - matrix shear z to x.txt" #break
#case(15)  #declare Txt_Path="15 - matrix shear z to y.txt" #break

#case(20)  #declare Txt_Path="20 - Shear_Trans example.txt" #break
#case(21)  #declare Txt_Path="21 - Matrix_Trans example.txt" #break
#case(22)  #declare Txt_Path="22 - Axial_Scale_Trans.txt" #break
#case(23)  #declare Txt_Path="23 - Rotate_Around_Trans.txt" #break
#case(24)  #declare Txt_Path="24 - Axis_Rotate_Trans.txt" #break
#case(25)  #declare Txt_Path="25 - Reorient_Trans.txt" #break
#case(26)  #declare Txt_Path="26 - vaxis_rotate.txt" #break
#case(27)  #declare Txt_Path="27 - vrotate.txt" #break
*/

#if (Typ=10) // In_Path,"31 - random linear height.txt"
camera{ angle 35 location < 3.50, 3.00, -3.00> look_at< 0.70, 0.70,  0.30> right x*image_width/image_height}
object{ Base_Wire_Frame_Box }
object{ Base_Box  #include concat(In_Path,Txt_Path) }
object{ AxisXYZ_( 2.5, 2.5, 7.5, T_Dark, T_Light,20,-45,   0.65,0.65,1.05) scale 0.5}
#end

#if (Typ=11)
camera{ angle 35 location < 3.50, 3.00, -3.00> look_at< 0.70, 0.70,  0.30> right x*image_width/image_height}
object{ Base_Wire_Frame_Box }
object{ Base_Box #include concat(In_Path,Txt_Path) }
object{ AxisXYZ_( 2.5, 2.5, 7.5, T_Dark, T_Light,20,-45,   0.65,0.65,1.05) scale 0.5}
#end

#if (Typ=12)
camera{ angle 35 location < 3.50, 3.00, -3.00> look_at< 0.70, 0.70,  0.30> right x*image_width/image_height}
object{ Base_Wire_Frame_Box }
object{ Base_Box #include concat(In_Path,Txt_Path) }
object{ AxisXYZ_( 2.5, 2.5, 7.5, T_Dark, T_Light,20,-45,   0.65,0.65,1.05) scale 0.5}
#end

#if (Typ=13)
camera{ angle 35 location < 3.50, 3.00, -3.00> look_at< 0.70, 0.70,  0.30> right x*image_width/image_height}
object{ Base_Wire_Frame_Box }
object{ Base_Box #include concat(In_Path,Txt_Path) }
object{ AxisXYZ_( 2.5, 2.5, 7.5, T_Dark, T_Light,20,-45,   0.65,0.65,1.05) scale 0.5}
#end

#if (Typ=14)// 41 - VRand_In_Sphere.txt
camera{ angle 35 location < 3.50, 3.00, -3.00> look_at< 0.70, 0.70,  0.30> right x*image_width/image_height}
object{ Base_Wire_Frame_Box }
object{ Base_Box #include concat(In_Path,Txt_Path) }
object{ AxisXYZ_( 2.5, 2.5, 7.5, T_Dark, T_Light,20,-45,   0.65,0.65,1.05) scale 0.5}
#end

#if (Typ=15)// 42 - VRand_On_Sphere.txt
camera{ angle 35 location < 3.50, 3.00, -3.00> look_at< 0.70, 0.70,  0.30> right x*image_width/image_height}
object{ Base_Wire_Frame_Box }
object{ Base_Box #include concat(In_Path,Txt_Path) }
object{ AxisXYZ_( 2.5, 2.5, 7.5, T_Dark, T_Light,20,-45,   0.65,0.65,1.05) scale 0.5}
#end



#if (Typ=20)// 20 - Shear_Trans example.txt
camera{ angle 35 location < 3.50, 3.00, -3.00> look_at< 1.00, 0.70,  0.30> right x*image_width/image_height}
box { <0.00, 0.00, 0.00>,< 1.00, 1.00, 1.00>
      texture { pigment{ color rgb<1.0,0.25,0.5>*0.75} finish { phong 0.7 }}
      Shear_Trans(<2,0.5,0>, <0,0.5,0>, <0,0.5,0.5>)
    } // end of box --------------------------------------
object { Wire_Box(<-0.035,0,-0.035>,<1.035,1.035,1.035>, 0.035   , 0)
         texture{ pigment{ color rgb<1,0.65,0>} finish{ phong 1} }
       }
object{ Base_Wire_Frame_Box }
object{ AxisXYZ_( 2.5, 2.5, 7.5, T_Dark, T_Light,20,-45,   0.65,0.65,1.05) scale 0.5}
#end
// ---
#if (Typ=21)// 21 - Matrix_Trans example.txt
camera{ angle 35 location < 3.50, 3.00, -3.00> look_at< 1.00, 0.70,  0.30> right x*image_width/image_height}
box { <0.00, 0.00, 0.00>,< 1.00, 1.00, 1.00>
      texture { pigment{ color rgb<1.0,0.35,0.6>*0.75} finish {phong 0.7}}
      Matrix_Trans(<2,0.5,0>, <0,0.5,0>, <0,0.5,0.5>, <-0.5,0,0>)
    } // end of box --------------------------------------
object { Wire_Box(<-0.01,-0.01,-0.01>,<1.01,1.01,1.01>, 0.02   , 0)
         texture{ pigment{ color rgb<1,0.65,0.0>} finish { phong 1}}
       } // ---------------------------------------------------------
box { <-0.01,-0.01,-0.01>,<1.01,1.01,1.01>
      texture { pigment{ color rgbf<1,0.65,0.0,0.8>} finish { phong 0.7}}
    } // end of box --------------------------------------
object{ Base_Wire_Frame_Box }
object{ AxisXYZ_( 2.5, 2.5, 7.5, T_Dark, T_Light,20,-45,   0.65,0.65,1.05) scale 0.5}
#end
// ---
#if (Typ=22)// 22 - Axial_Scale_Trans.txt
camera{ angle 28 location < 2.00, 4.00, -4.50> look_at< 0.80, 0.00,  0.90> right x*image_width/image_height}
#local Object_Trans = < 1,0.0,0>;
box { <0.00, 0.00,-0.00>,< 1.0, 0.25,  1>
      texture { pigment{ color rgb<1,0.35,0.6>*0.95} finish {phong 0.7}}
      translate Object_Trans
      Axial_Scale_Trans(  <2,0,2>, 0.5)
    } // end of box --------------------------------------
object{ Vector( o , <2,0,2> , 0.04)
        texture { pigment{ color rgb<0.2,1,0.0>} finish { phong 0.7}}
        } //----------------------------------------------
object{ Wire_Box(<-0.010,-0.01,-0.01>,<1.01,0.251, 1.01>, 0.02   , 0)
        texture{ pigment{ color rgb<1,0.65,0.0>} finish { phong 1}}
        translate Object_Trans
       } // ---------------------------------------------------------
box { <-0.01,-0.01, 0.01>,<1.01,0.251, 1.01>
      texture { pigment{ color rgbf<1,0.85,0.4,0.9>} finish { phong 0.7 }}
      translate Object_Trans //
    } // end of box --------------------------------------
object{ AxisXYZ_( 4.3, 2.5, 4.5, T_Dark, T_Light,20,-15,   0.65,0.65,0.75) scale 0.5}
#end

// ---
#if (Typ=23)// 23 - Rotate_Around_Trans.txt
camera{ angle 28 location <3.80, 4.00, -4.50> look_at< 0.95, 0.30,  0.90> right x*image_width/image_height}
#local Object_Trans = <1.5,0.5,1>;
box { <0.00, 0.00, 0.00>,< 1.00, 0.50, 0.50>
      texture { pigment{ color rgb<1,0.3,0.5>*0.95} finish { phong 0.7 }}
      translate Object_Trans
      // Rotate_Around_Trans( <Rotate_x,Rotate_y,Rotate_z>, Center_of_Rotation)
      Rotate_Around_Trans( <0,0,20>, <1.5,0.5,1>)
    } // end of box --------------------------------------
object { Wire_Box(<-0.00,-0.00,-0.00>,<1.5,0.5,1>, 0.02   , 0)
         texture{ pigment{ color rgb<1,0.65,0.0>} finish { phong 1}}
       } // ---------------------------------------------------------
sphere { <0,0,0>, 0.075
         texture { pigment{ color rgb< 1,0.1, 0.0>} finish { phong 1 }}
            translate Object_Trans
       }  // end of sphere -----------------------------------
//#macro Segment_of_CylinderRing ( R_out, R_in, Height, Segment_Angle)
object{  Segment_of_CylinderRing ( 0, 0.80 ,0.02, -20)
         texture { pigment{ color  rgb< 1, .1, 0.0>} finish { phong 1}}
         rotate<-90,0,0> translate Object_Trans
      }//----------------------------------------------------------
object{ AxisXYZ_( 3.7, 2.5, 7, T_Dark, T_Light,10,-0,   0.65,0.55,0.75) scale 0.5}
#end


// ---
#if (Typ=24)// 24 - Axis_Rotate_Trans.txt
camera{ angle 28 location < 0.80, 4.00, -4.50> look_at< 0.80, 0.50,  0.90> right x*image_width/image_height}
#local Object_Trans = < 1,0.0,0>;
box { <0.00, 0.00,-0.40>,< 1.50, 0.25, -1>
      texture { pigment{ color rgb<1,0.3,0.5>*0.95} finish { phong 0.7}}
      translate Object_Trans   rotate<0,-degrees(atan2(2,1)),0>
      // Reorient_Trans(Axis1, Axis2)
      Axis_Rotate_Trans(  <1,0,2>, 110)
    } // end of box --------------------------------------
object{ Vector( o , <1.5,0,3> , 0.04)
        texture { pigment{ color rgb<0.2,1,0.0>} finish { phong 0.7 }}
        } //----------------------------------------------
object{ Wire_Box(<-0.010,-0.01,-0.401>,<1.51,0.251,-1.01>, 0.02   , 0)
        texture{ pigment{ color rgb<1,0.65,0.0>} finish { phong 1 }}
        translate Object_Trans  rotate<0,-degrees(atan2(2,1)),0>
      } // ---------------------------------------------------------
box { <-0.01,-0.01,-0.401>,<1.51,0.251,-1.01>
      texture { pigment{ color rgbf<1,0.85,0.3,0.9> } finish { phong 0.7 }}
      translate Object_Trans  rotate<0,-degrees(atan2(2,1)),0>
    } // end of box --------------------------------------
//#macro Segment_of_CylinderRing ( R_out, R_in, Height, Segment_Angle)
object{  Segment_of_CylinderRing ( 0.0, 0.50 ,0.02, -110)
         texture { pigment{ color  rgbf< 0.2,1,0.2,0.8>} finish { diffuse 0.9 phong 1} }
         rotate<-90,90,0>
         translate Object_Trans  rotate<0,-degrees(atan2(2,1)),0>
      }//----------------------------------------------------------
object{ AxisXYZ_( 3.3, 2.5, 7, T_Dark, T_Light,10,-0,   0.65,0.55,0.75) scale 0.5}
#end
//---------------------------------------------------- end shearing and transform


// ---
#if (Typ=25)// 25 - Reorient_Trans.txt
camera{ angle 27 location < 4.50, 4.50, -4.50> look_at< 0.90, 0.00,  0.80> right x*image_width/image_height}
#local Object_Trans = < 1,0.0,0>;
box { <0.00, 0.00,0>,< 1.50, 0.25,  1>
      texture { pigment{ color rgb<1,0.4,0.5>*0.75} finish { phong 0.7 }}
      translate Object_Trans
      // Reorient_Trans(Axis1, Axis2)
      Reorient_Trans(  <1,0,0>, <1,0.0,2>)
    } // end of box --------------------------------------
object{ Vector( o , <1,0.0,2> , 0.04)
        texture { pigment{ color rgb<0.2,1,0.0> } finish { phong 0.7 }}
        } //----------------------------------------------
object{ Vector( o , <1,0,0> , 0.04)
        texture { pigment{ color rgb<0.2,1,0.0>*0.5 } finish { phong 0.7 }}
        } //----------------------------------------------
object{ Wire_Box(<-0.010,-0.01,-0.01>,<1.51,0.251, 1.01>, 0.02   , 0)
        texture{ pigment{ color rgb<1,0.65,0.0>} finish { phong 1}}
        translate Object_Trans
        Reorient_Trans(  <1,0,0>, <1,0.0,2>)
      } // ---------------------------------------------------------

object{ Wire_Box(<-0.010,-0.01,-0.01>,<1.51,0.251, 1.01>, 0.02   , 0)
        texture{ pigment{ color rgb<1,0.65,0.0>} finish { phong 1}}
        translate Object_Trans
      } // ---------------------------------------------------------

box { <-0.010,-0.01,-0.01>,<1.51,0.251, 1.01>
      texture { pigment{ color rgbf<1,0.95,0.5,0.9> } finish { phong 0.7 }}
      translate Object_Trans
    } // end of box --------------------------------------
//#macro Segment_of_CylinderRing ( R_out, R_in, Height, Segment_Angle)
object{  Segment_of_CylinderRing ( 0.0, 0.50 ,0.02, -60)
         texture { pigment{ color  rgbf< 0.5,1,  0.3, 0.8>} finish { diffuse 0.9 phong 1}}
         translate< 0,0.04,0>
      }//----------------------------------------------------------
object{ AxisXYZ_( 5.2, 2.25, 4.5, T_Dark, T_Light,20,-15,   0.65,0.65,0.75) scale 0.5}
#end

// ---
#if (Typ=26)// 26 - vaxis_rotate.txt
camera{ angle 28 location < 3.00, 3.70, -1.20> look_at< 0.30, 0.50,  0.30> right x*image_width/image_height}
#declare V1 = < 0,1,0.5>;
#declare V2 = < 1,1.5,0>  ;
#declare Angle = 60; // degrees
// vaxis_rotate(V1,V2,A) = rotate V1 around V2 by A degrees
//vaxis_rotate( < 0,1,0.5>, < 0,1,0.5>, 70 )
object{ Vector( o ,V1 , 0.03)
        texture { pigment{ color rgb<0.2,1,0.0>*0.7}  finish { phong 0.7 }}
        } //----------------------------------------------
object{ Vector( o ,vaxis_rotate( V1,V2, 80)  , 0.03)
        texture { pigment{ color rgb<0.5,1,0.0>*0.7} finish { phong 0.7 }}
        } //----------------------------------------------
object{ Vector( o ,V2 , 0.04)
        texture { pigment{ color rgb<1,0,0.0>*0.7 } finish { phong 0.7 }}
        } //----------------------------------------------
//#macro Segment_of_CylinderRing ( R_out, R_in, Height, Segment_Angle)
object{  Segment_of_CylinderRing ( 1.00, 0.95 ,0.02, -65)
         texture { pigment{ color  rgb< 0.5,0.8,  0.0>*0.8} finish { phong 1}}
         rotate<0,-65,0> translate<0,0.5,0>
         Point_At_Trans(V2)
      }//----------------------------------------------------------
object{ AxisXYZ_( 1.00, 2.00, 3.85, T_Dark, T_Light,20,-65,   0.65,0.65,0.75) scale 0.5}
#end


// ---
#if (Typ=27)// 27 - vrotate.txt
camera{ angle 48 location < 2.50, 2.2, -2.80> look_at< 1.20, 0.70,  0.30> right x*image_width/image_height}
#declare V1 = < 2.5,1,0>;
#declare V2 = vrotate(V1,< 0,-90,0>);
#declare Angle = 90; // degrees
object{ Vector( o ,V1 , 0.03)
        texture { pigment{ color rgb<1,0.40,0.0>*0.7}  finish { phong 0.7 }}
        } //----------------------------------------------
object{ Vector( o, V2, 0.05)
        texture { pigment{ color rgb<0.5,1,0.0>*0.7} finish { phong 0.7 }}
        } //----------------------------------------------
//#macro Segment_of_CylinderRing ( R_out, R_in, Height, Segment_Angle)
object{  Segment_of_CylinderRing ( 2.50, 2.45 ,0.05, -90)
         texture { pigment{ color  rgb< 1,0.0,  0.2>*0.8} finish { phong 1}}
           translate<0,1,0> no_shadow
         //Point_At_Trans(V2)
      }//----------------------------------------------------------
cone{<0,0,0>,0,<0.35,0,0>,0.15
       texture { pigment{ color  rgb< 1,0.0,  0.2>*0.8} finish { phong 1}}
       translate V2
      }

object{ AxisXYZ_( 2.25, 2.25, 4.85, T_Dark, T_Light,0,0,   0.55,0.65,0.75) scale 0.5}
#end


//---------------------------------------------------- end shearing and transform