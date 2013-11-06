// Insert menu illustration scene
// Author Friedrich A. Lohmueller, June 2012, update Feb-2013
#version 3.7;

   //  #declare Typ = 61; // for testing

#if ( (Typ > 2) & (Typ != 160))
global_settings{ assumed_gamma 1.0 } 
#end 

#default{ finish{ ambient 0.1 diffuse 0.9 }} 
#include "colors.inc"
#include "textures.inc"
#include "woods.inc"
// --------------------------- 
#declare In_Path  = "20 - Basic shapes round/" // round
#declare In_Path2 = "21 - Basic shapes angular/" // angular 


#switch (Typ)  //----------------------------------------------------------
#case(1)  #declare Txt_Path="01 - sphere.txt"  #break
#case(2)  #declare Txt_Path="02 - sphere with bumps.txt"  #break
#case(3)  #declare Txt_Path="03 - ellipsoid Polished Chrome.txt"  #break
#case(4)  #declare Txt_Path="04 - ellipsoid flat.txt"  #break

#case(10)  #declare Txt_Path="10 - ovus.txt"  #break

#case(21)  #declare Txt_Path="21 - cylinder y.txt"  #break
#case(23)  #declare Txt_Path="23 - cylinder x.txt"  #break
#case(24)  #declare Txt_Path="24 - cylinder z.txt"  #break
#case(25)  #declare Txt_Path="25 - cylinder open.txt"  #break

#case(30)  #declare Txt_Path="30 - cone truncated.txt"  #break
#case(31)  #declare Txt_Path="31 - cone open.txt"  #break
#case(32)  #declare Txt_Path="32 - cone open texture_map.txt"  #break
#case(33)  #declare Txt_Path="33 - cone double.txt"  #break

#case(35)  #declare Txt_Path="35 - cone 1.txt"  #break
#case(37)  #declare Txt_Path="37 - cone truncated thin.txt"  #break

#case(40)  #declare Txt_Path="40 - torus.txt"  #break  
#case(41)  #declare Txt_Path="41 - torus 1.txt"  #break  
#case(43)  #declare Txt_Path="43 - torus 2.txt"  #break  

// new: Feb-2013
#case(36)  #declare Txt_Path="36 - cone texture bumps.txt"  #break
#case(38)  #declare Txt_Path="38 - torus small.txt"  #break

#case(61)  #declare Txt_Path="61 - sor Surface Of Revolution.txt"  #break
#case(62)  #declare Txt_Path="62 - sor Surface Of Revolution.txt"  #break
//-------- 

#case(63)  #declare Txt_Path="63 - sor Surface Of Revolution.txt"  #break

#case(70)  #declare Txt_Path="70 - lathe object linear_spline.txt"  #break
#case(71)  #declare Txt_Path="71 - lathe object quadratic_spline.txt"  #break
#case(72)  #declare Txt_Path="72 - lathe object cubic_spline.txt"  #break
// -----------------------------------------------
// angular


#case(101)  #declare Txt_Path="A1 - box symmetric.txt"  #break
#case(102)  #declare Txt_Path="A2 - box xyz_positive.txt"  #break

#case(104)  #declare Txt_Path="A4 - box street.txt"  #break
#case(105)  #declare Txt_Path="A5 - box wood board.txt"  #break

#case(120)  #declare Txt_Path="B0 - prism X.txt"  #break
#case(121)  #declare Txt_Path="B1 - prism Y.txt"  #break
#case(130)  #declare Txt_Path="B2 - prism Z house1.txt"  #break
#case(131)  #declare Txt_Path="B3 - prism Z roof1.txt"  #break
// new: Feb-2013
#case(132)  #declare Txt_Path="B4 - prism Z house2.txt"  #break
#case(133)  #declare Txt_Path="B5 - prism Z roof2.txt"  #break
#case(134)  #declare Txt_Path="B6 - prism X prism Z union.txt"  #break
#case(135)  #declare Txt_Path="B7 - prism X prism Z union2.txt"  #break
#case(136)  #declare Txt_Path="B8 - prism X prism Z union difference.txt"  #break
#case(137)  #declare Txt_Path="B9 - prism X prism Z intersection.txt"  #break
//----------

#case(150)  #declare Txt_Path="C0 - prism Y linear_spline.txt"  #break
#case(152)  #declare Txt_Path="C2 - prism Y cubic_spline.txt"  #break

#case(160)  #declare Txt_Path="D0 - plane.txt"  #break

#end
// -----------------------------------------------------------------------------------------------------
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
#declare Ground_1 = 
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
 
#declare Ground_2 = 
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
#declare Sand_Ground  =
plane{ <0,1,0>, 0 
       texture{ pigment{ color rgb <1.00,0.95,0.8>}
                normal { bumps 0.75 scale 0.025  }
                finish { phong 0.1 } 
              } // end of texture
     } // end of plane

//#include  BaseScene0  
#declare BaseScene0 ="10 - Ready made scenes/50 - Basic Scene 05 - Grass with small clouds in sky.txt"
// sky -------------------------------------------------------------- 
#macro BaseScene0_o()
light_source{<-1500,2000,-2500> color White}
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
plane { <0,1,0>, 0 
        texture{ pigment{ color rgb<0.35,0.65,0.0>*0.72 }
                 normal { bumps 0.75 scale 0.015 }
                 finish { phong 0.1 }
               } // end of texture
      } // end of plane
#end // of macro


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
                                [0.2 color rgb<0.24,0.34,0.56>*0.8]//~Navy
                                [0.7 color rgb<0.24,0.34,0.56>*0.8]//~Navy
                                [1.0 color rgb<1,1,1>         ]//White
                              }
                     rotate<-30,0,0>
                     scale 2 }
           } // end of sky_sphere
fog{fog_type   2
    distance   50
    color      White
    fog_offset 0.1
    fog_alt    1.0
    turbulence 0.8}
 
#end // call: MySky()
#macro MySkyBlueish(Rot_X)
 sky_sphere{ pigment{ gradient <0,1,0>
                       color_map{ [0   color rgb<1,1,1>         ]//White
                                  [0.25 color rgb<0.24,0.34,0.56>*0.8]//~Navy
                                  [0.75 color rgb<0.24,0.34,0.56>*0.8]//~Navy
                                  [1.0 color rgb<1,1,1>         ]//White
                              }
                     rotate< Rot_X,0,0>
                     scale 2 }
           } // end of sky_sphere
#end //
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
// -----------------------------------------------
// -----------------------------------------------












// --------
#if (Typ=1)
#include  BaseScene0
object{ AxisXYZ_( 3.25, 3.2, 2, T_Dark, T_Light, 0, 0,   0.75,0.75,0.75) scale 0.5}
#include concat(In_Path,Txt_Path) // "10 - Basic shapes/01 - sphere.txt"
#end
// --------
#if (Typ=2)
#include  BaseScene0
light_source{ < 0.00, 1.00, -3.00>  color rgb<0.9,0.9,1>*0.05 shadowless}  // flash 
object{ AxisXYZ_( 3.25, 4.5, 2, T_Dark, T_Light, 0,  0,   0.8,0.75,0.75) scale 0.5}
#include concat(In_Path,Txt_Path) // "10 - Basic shapes/01 - sphere.txt"
#end
// --------
#if (Typ=3)
BaseScene0_o()
camera{ angle 65 location<-0.5,1.5,-2.5> look_at<0,0.75,0> }
object{ AxisXYZ_( 2.75, 3.2, 7, T_Dark, T_Light, 10, 0,   0.75,0.65,0.75) scale 0.5}
#include concat(In_Path,Txt_Path)  // "10 - Basic shapes/02 - ellipsoid Polished Chrome.txt"
#end
// --------
#if (Typ=4)
BaseScene0_o()
camera{ angle 65 location<-1,1.20,-2.5> look_at<0,0.5,0> }
object{ AxisXYZ_( 4.25, 2.2, 7, T_Dark, T_Light, 0, 0,   0.75,0.75,0.75) scale 0.5}
#include  concat(In_Path,Txt_Path) // "10 - Basic shapes/03 - ellipsoid flat.txt"
#end
// --------
// --------
#if (Typ=10)
BaseScene0_o()
camera{ angle 54 location < 1.00, 1.50, -2.00> look_at< 0.05, 0.80,  0.05>}
object{ AxisXYZ_( 1.20, 2.8, 7, T_Dark, T_Light,10,-35, 0.55,0.55,0.8) scale 0.5}
#include  concat(In_Path,Txt_Path) // "10 - Basic shapes/10 - ovus.txt"
#end
// --------



#if (Typ=21)
BaseScene0_o()
camera{ angle 63 location< 1.0,2.75,-2.5> look_at<0,1.35,0> }
object{ Ground_2 } 
object{ AxisXYZ_( 2.25, 4.5, 7, T_Dark, T_Light, 0, 0,   0.75,0.55,1) scale 0.5}
#include  concat(In_Path,Txt_Path) // "10 - Basic shapes/21 - cylinder y.txt"
#end
// --------
#if (Typ=23)
BaseScene0_o()
camera{ angle 50 location< 1.0,2.50,-2.5> look_at<0.2,0.5,0> }
object{ Ground_2 } 
object{ AxisXYZ_( 2.75, 2.2, 7, T_Dark, T_Light, 0, 0,   0.75,0.75,1) scale 0.5}
#include concat(In_Path,Txt_Path) //  "10 - Basic shapes/23 - cylinder x.txt"
#end
// --------
#if (Typ=24)
BaseScene0_o()
camera{ angle 45 location< 1.0,2.50,-2.5> look_at<0.2,0.65,0> }
object{ Ground_2 } 
object{ AxisXYZ_( 2.25, 2.2, 7, T_Dark, T_Light, 0, 0,   0.75,0.75,1) scale 0.5}
#include  concat(In_Path,Txt_Path) // "10 - Basic shapes/24 - cylinder z.txt"
#end
// --------
#if (Typ=25)
camera{ angle 38 location < 2.50, 3.00, -2.50> look_at< 0.05, 0.30,  0.05>}
light_source{< 1500, 1500, -200> color rgb<1,1,1>*0.9}     // sun light
light_source{< 1.50, 2.00,-3.00> color rgb<0.9,0.9,1>*0.1}  // flash 
MySkyBlueish( 0 ) // sky  
//------------------------------------------------------------------------
object{ AxisXYZ_(  2.75, 2.25, 5, T_Dark, T_Light,10,-35, 0.55,0.65,0.8) scale 0.5}
//------------------------------------------------------------------------------------
object{ Vector( <0,0,0>,<1.00,0,0>, 0.015) texture{ T_Red } no_shadow 
        translate<0,0.75,0> }
sphere{ <0,0,0>, 0.065 texture{ T_Red }  no_shadow
         translate<0,0,0> }  
sphere{ <0,0,0>, 0.065 texture{ T_Red }   no_shadow
         translate<0,0.75,0> }  
text { ttf "arialbd.ttf", "R", 0.02, 0.0  texture{ T_DarkRed } no_shadow 
       scale<1,1.5,1>*0.175
       rotate<30,-45,0>
       translate<0.70,0.80,-0.40> }      
text { ttf "arialbd.ttf", "P1", 0.02, 0.0 texture{ T_DarkRed } no_shadow 
       scale<1,1.5,1>*0.165
       translate<-0.30,0.05,0.0>      
       rotate<30,-45,0> }
text { ttf "arialbd.ttf", "P2", 0.02, 0.0 texture{ T_DarkRed } no_shadow  
       scale<1,1.5,1>*0.165
       translate<-0.30,0.55,0.0>     
       rotate<30,-45,0> }
///-----------------------
#include concat(In_Path,Txt_Path) //  "10 - Basic shapes/25 - cylinder open.txt"
#end
// --------




// -------- cones 
#if (Typ=30)
camera{ angle 42 location< 1.50, 2.00, -3.00>  look_at< 0.15, 0.40,  0.05>}
light_source{< 2500, 500,-1000> color rgb<1,1,1>*0.9}     // sun light
light_source{ < 1.50, 2.00, -3.00>  color rgb<0.9,0.9,1>*0.1}  // flash light
MySkyBlueish( 0 ) // sky  
//------------------------------------------------------------------------
object{ AxisXYZ_( 2.35 , 2.20 , 8 , T_Dark, T_Light, 0, 0,   0.65,0.65,1.1) scale 0.5}
object{ Vector( <0,0,0>,<1.00,0,0>, 0.025) pigment{ color rgb< 0.9,0.0,0.0> } rotate<0,-25,0>  no_shadow }
object{ Vector( <0,0,0>,<0.60,0,0>, 0.02) pigment{ color rgb< 0.8,0.0,0.0> } rotate<0,-25,0> translate<0,0.80,0> no_shadow }
//------------------------------------------------------------------------------------
text { ttf "arialbd.ttf", "R1", 0.02, 0.0 texture{ T_DarkRed }  no_shadow 
       scale<1,1.5,1>*0.145
       translate<1.05,0.01,0.0> rotate<0,-25,0>}    
text { ttf "arialbd.ttf", "R2", 0.02, 0.0 texture{ T_DarkRed } no_shadow  
       scale<1,1.5,1>*0.135
       translate<0.65,0.81,0.0> rotate<0,-25,0>}
sphere{ <0,0,0>, 0.065 texture{ T_Red } no_shadow 
        translate<0,0,0> }
sphere{ <0,0,0>, 0.065 texture{ T_Red } no_shadow 
        translate<0,0.80,0> }  
text { ttf "arialbd.ttf", "Center 1", 0.02, 0 texture{ T_DarkRed } no_shadow  
       scale<1,1.5,1>*0.175
       translate<-0.8,0.02,0.0> rotate<0,-25,0>}
text { ttf "arialbd.ttf", "Center 2", 0.02, 0 texture{ T_DarkRed } no_shadow  
       scale<1,1.5,1>*0.165
       translate<-0.8,0.82,0.0> rotate<0,-25,0> }
torus{ 1.0,0.015 texture { T_Orange } no_shadow  
       translate<0,0.0,0>} 
torus{ 0.60,0.015 texture { T_Orange } no_shadow 
       translate<0,0.8,0>}
///-----------------------
#include concat(In_Path,Txt_Path) //  "10 - Basic shapes/30 - cone truncated.txt"
#end
// --------
#if (Typ=31)
camera{ angle 42 location< 1.50, 2.00, -3.00>  look_at< 0.15, 0.40,  0.05>}
light_source{< 2500, 1500,-1000> color rgb<1,1,1>*0.9}     // sun light
light_source{ < 1.50, 2.00, -3.00>  color rgb<0.9,0.9,1>*0.1}  // flash light
MySkyBlueish( 0 ) // sky  
//------------------------------------------------------------------------
object{ AxisXYZ_( 2.35 , 2.20 , 3 , T_Dark, T_Light, 0, 0,   0.65,0.65,1.1) scale 0.5}
//---------------------------------------------------------------------------------
object{ Vector( <0,0,0>,<0.50,0,0>, 0.015) pigment{ color rgb< 0.9,0.0,0.0> } rotate<0,-25,0>  no_shadow }
object{ Vector( <0,0,0>,<1.00,0,0>, 0.015) pigment{ color rgb< 0.8,0.0,0.0> } rotate<0,-25,0> translate<0,1,0> no_shadow }
sphere{ <0,0,0>, 0.055 texture{ T_Red } no_shadow 
        translate<0,0,0> }
sphere{ <0,0,0>, 0.055 texture{ T_Red } no_shadow 
        translate<0,1.00,0> }  
//------------------------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // "31 - cone open.txt
#end
// --------
#if (Typ=32)
#include  BaseScene0
//------------------------------------------------------------------------
object{ AxisXYZ_( 3 , 4.70 , 3 , T_Dark, T_Light, 0, 0,   0.75,0.65,1.1) scale 0.5}
//---------------------------------------------------------------------------------
sphere{ <0,0,0>, 0.055 texture{ T_Red } no_shadow 
        translate<0,1.15,0> }  
//------------------------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // "31 - cone open2.txt
#end
// --------
#if (Typ=33)
camera{ angle 42 location< 1.50, 2.00, -3.00>  look_at< 0.15, 0.40,  0.05>}
light_source{< 2500, 1500,-1000> color rgb<1,1,1>*0.9}     // sun light
light_source{ < 1.50, 2.00, -3.00>  color rgb<0.9,0.9,1>*0.1}  // flash light
MySkyBlueish(-15 ) // sky  
//------------------------------------------------------------------------
object{ AxisXYZ_( 2.35 , 2.20 , 3 , T_Dark, T_Light, 0, 0,   0.65,0.65,1.1) scale 0.5}
//---------------------------------------------------------------------------------
object{ Vector( <0,0,0>,<-0.50,0,0>, 0.015) pigment{ color rgb< 0.9,0.0,0.0> } rotate<0,-25,0> translate<0.23,0.25,-0.4>  no_shadow }
object{ Vector( <0,0,0>,<1.00,0,0>, 0.02) pigment{ color rgb< 1,0.0,0.0> } rotate<0,-25,0> translate<0,1,0> no_shadow }
sphere{ <0,0,0>, 0.045 texture{ T_Red } no_shadow 
        translate<0,0,0> translate<0.23,0.25,-0.4> }
sphere{ <0,0,0>, 0.055 texture{ T_Red } no_shadow 
        translate<0,1.00,0> }  
//------------------------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // "33 - cone doble.txt
#end
// --------

#if (Typ=35)
BaseScene0_o()
camera{ angle 70 location <1.5, 1.30, -2.50> look_at< 0.0, 0.90,  0.0>}
//--------------------------------------------------------------------------------
object{ AxisXYZ_(  2.50, 3.45, 5, T_Dark, T_Light, 5, -20, 0.65,0.75,1.2) scale 0.5}
///-------------------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // "35 - cone 1.txt"
#end
// -------
#if (Typ=36)
BaseScene0_o()
camera{ angle 70 location <1.5, 1.30, -2.50> look_at< 0.0, 0.90,  0.0>}
//--------------------------------------------------------------------------------
object{ AxisXYZ_(  2.50, 3.45, 5, T_Dark, T_Light, 5, -20, 0.65,0.75,1.2) scale 0.5}
///-------------------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // "36 - cone texture bumps.txt"
#end
// --------
#if (Typ=37)
BaseScene0_o()
camera{ angle 70 location <1.5, 1.30, -2.50> look_at< 0.0, 1.20,  0.0>}
//--------------------------------------------------------------------------------
object{ AxisXYZ_(  1.50, 4.65, 5, T_Dark, T_Light, 0, -20, 0.55,0.65,1.1) scale 0.5}
///-------------------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // "37 - cone truncated thin.txt"
#end
// --------
#if (Typ=38)
BaseScene0_o()
camera{ angle 70 location <1.5, 1.30, -2.50> look_at< 0.0, 1.20,  0.0>}
//--------------------------------------------------------------------------------
object{ AxisXYZ_(  1.50, 4.65, 5, T_Dark, T_Light, 0, -20, 0.55,0.65,1.1) scale 0.5}
///-------------------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // "38 - torus small.txt"
#end
// --------



// --------  // torus
#if (Typ=40)
camera{ angle 39 location < 2.50, 2.50, -2.50> look_at< 0.05, 0.00,  0.05>}
light_source{< 2500, 500,-1000> color rgb<1,1,1>*0.9}     // sun light
light_source{ < 1.50, 2.00, -3.00>  color rgb<0.9,0.9,1>*0.1}  // flash light
MySkyBlueish( 0 ) // sky  
//------------------------------------------------------------------------
object{ AxisXYZ_(  2.75, 1.65, 4, T_Dark, T_Light,10,-35, 0.55,0.65,0.8) scale 0.5}
//------------------------------------------------------------------------------------
object{ Vector( <0,0,0>,<1.00,0,0>, 0.02) texture{ T_Red } no_shadow  
         translate<0,0.,-0.015> }
object{ Vector( <0,0,0>,<0.25,0,0>, 0.015) texture{ T_Red } no_shadow
       rotate<0,0,110> translate<1,0,0>}
//------------------------------------------------------------------------------------  
text { ttf "arialbd.ttf", "R_major", 0.02, 0 texture{ T_DarkRed } no_shadow 
       scale<1,1.5,1>*0.165
       translate<0.175, 0.09,0.0> rotate<0,-45,0>} 
text { ttf "arialbd.ttf", "R_minor", 0.02, 0 texture{T_DarkRed }  no_shadow 
       rotate<0,-45,0> 
       scale<1,1.5,1>*0.155
       translate<0.9,0.27,0.0> } 
//------------------------------------------------------
sphere { <0,0,0>, 0.065 texture{ T_Red } no_shadow 
         translate<0,0,0>}   
sphere { <0,0,0>, 0.045 texture{ T_Red } no_shadow 
         translate<1,0,0>}   
//------------------------
torus{  1.0,0.01   rotate<0,0,0>  texture{ T_Red } no_shadow  
        translate<0,0. ,0>}
torus{  0.250,0.01   rotate<90,0,0> texture{ T_Red } no_shadow 
        translate<1,0. ,0>}
///--------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // "10 - Basic shapes/40 - torus.txt"
#end
// --------  // torus
#if (Typ=41) 
#include  BaseScene0
//------------------------------------------------------------------------
object{ AxisXYZ_(  4.00, 2.65, 15, T_Dark, T_Light,10,30, 0.85,0.75,1.4) scale 0.5}
///--------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // "10 - Basic shapes/41 - torus 1.txt"
#end
// --------
#if (Typ=43)
BaseScene0_o()
camera{ angle 60 location <0, 0.50, -2.00> look_at< 0.0, 0.50,  0.0>}
//------------------------------------------------------------------------
sphere { <0,0,0>, 0.065 texture{ T_Red } no_shadow 
         translate<0,0.5,0>}   
object{ AxisXYZ_(  2.00, 2.35, 15, T_Dark, T_Light,  0,10, 0.55,0.55,0.4) scale 0.45}
///--------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // "10 - Basic shapes/43 - torus 2.txt"
#end


// ----   
#if (Typ=61) //  
object{ Sand_Ground translate<0,0.0001,0> } 
camera{ angle 35 location < 2.50, 1.70, -3.00> look_at< 0.05, 0.75,  0.05>}
light_source{< 2500, 500,-1000> color rgb<1,1,1>*0.9}     // sun light
light_source{ < 1.50, 2.00, -3.00>  color rgb<0.9,0.9,1>*0.1 shadowless}  // flash 
MySky() // sky  
//------------------------------------------------------------------------
object{ AxisXYZ_( 2*1.55, 2*3.15, 2*3.00, T_Dark, T_Light, 5,-40, 0.65,0.75,0.9) scale 0.25}
//------------------------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // "61 - sor Surface Of Revolution.txt"
#end
// --------
#if (Typ=62)
object{ Sand_Ground translate<0,0.0001,0> } 
camera{ angle 35 location < 2.50, 1.50, -3.00> look_at< 0.05, 0.42,  0.05>}
light_source{< 2500, 500,-1000> color rgb<1,1,1>*0.9}     // sun light
light_source{ < 1.50, 2.00, -3.00>  color rgb<0.9,0.9,1>*0.1 shadowless}  // flash 
MySky() // sky  
//------------------------------------------------------------------------
object{ AxisXYZ_( 2*1.75, 2*2.20, 2*3.00, T_Dark, T_Light, 5,-40, 0.65,0.65,0.8) scale 0.25}
//------------------------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // "62 - sor Surface Of Revolution.txt 
#end
// --------
#if (Typ=63)
camera{ angle 30 location < 1.50, 1.60, -3.00> look_at< 0.05, 0.52,  0.05>}
light_source{< 2500, 500,-1000> color rgb<1,1,1>*0.9}     // sun light
light_source{< 2.50, 2.00, -2.50>  color rgb<0.9,0.9,1>*0.1 shadowless}  // flash 
MySky() // sky  
//------------------------------------------------------------------------
object{ AxisXYZ_( 2*1.00, 2*2.15, 2*1.5, T_Dark, T_Light,10,-35, 0.65,0.65,0.75) scale 0.25}
//------------------------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // "63 - sor Surface Of Revolution.txt 
#end
// --------


// --------
#if (Typ=70)
camera{ angle 26 location <1.50, 1.8, -2.20> look_at< 0.05, 0.05,  0.05>}
light_source{< 2500, 1500,-1000> color rgb<1,1,1>*0.9}     // sun light
light_source{ < 1.50, 2.00, -3.00>  color rgb<0.9,0.9,1>*0.1 shadowless}  // flash 
MySky() // sky  
//------------------------------------------------------------------------
object{ AxisXYZ_(  2*1.25, 2*1.0, 2*1.80, T_Dark, T_Light,10,-35, 0.55,0.55,0.75) scale 0.25}
//------------------------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // "70 - lathe object linear_spline.txt 
#end
// --------
#if (Typ=71)
camera{ angle 26 location <1.50, 2.5, -2.00> look_at< 0.05, 0.37,  0.05>}
light_source{< 2500, 1500,-1000> color rgb<1,1,1>*0.9}     // sun light
light_source{ < 1.50, 2.00, -3.00>  color rgb<0.9,0.9,1>*0.1 shadowless}  // flash 
MySky() // sky  
//------------------------------------------------------------------------
object{ AxisXYZ_(  2*0.95, 2*1.8, 2*1.50, T_Dark, T_Light,10,-35, 0.55,0.55,0.75) scale 0.25}
//------------------------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // "71 - lathe object quadratic_spline.txt 
#end
// --------
#if (Typ=72)
camera{ angle 16 location <1.50, 1.70, -2.20> look_at< 0.05, 0.14,  0.05>}
light_source{< 2500, 1500,-1000> color rgb<1,1,1>*0.9}     // sun light
light_source{ < 1.50, 2.00, -3.00>  color rgb<0.9,0.9,1>*0.1 shadowless}  // flash 
MySky() // sky  
//------------------------------------------------------------------------
object{ AxisXYZ_(  2*0.75, 2*0.7, 2*1.20, T_Dark, T_Light,10,-35, 0.4,0.4,0.45) scale 0.25}
//------------------------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // "72 - lathe object cubic_spline.txt 
#end
// --------



// ---------------------------------------------------------------------------------------- 
#if (Typ=101) 
BaseScene0_o()
camera{ angle 50 location <-3.5, 3.50, -3.50> look_at< 0.05, 1.00,  0.05>}
light_source{ < 2.5, 2.50, -4.50>  color rgb<0.9,0.9,1>*0.2 shadowless}  // flash 
//------------------------------------------------------------------------
object{ AxisXYZ_(  7.00, 4.75, 10, T_Dark, T_Light,45, 45, 1.05,0.95,1.2) scale 0.5}
///--------------------------------------------------------------------
#include concat(In_Path2,Txt_Path) // "A1 - box symmetric.txt"
#end
// --------
#if (Typ=102) 
BaseScene0_o()
camera{ angle 34 location < 2.0, 3.50, -3.50> look_at< 0.6, 0.80,  0.3>}
light_source{ < 2.5, 2.50, -4.50>  color rgb<0.9,0.9,1>*0.2 shadowless}  // flash 
//------------------------------------------------------------------------
object{ AxisXYZ_(  3.00, 3.25, 8, T_Dark, T_Light,45,-15,  0.65,0.65,1.0) scale 0.5}
///--------------------------------------------------------------------
#include concat(In_Path2,Txt_Path) // "A2 - box xyz_positive.txt"
#end
// --------


#if (Typ=104)  // box 
BaseScene0_o()
camera{ angle 45 location <0.2, 1.00, -3.00> look_at< 0.0, 1.00,  0.0>}
light_source{ < 2.5, 2.50, -4.50>  color rgb<0.9,0.9,1>*0.2 shadowless}  // flash 
//------------------------------------------------------------------------
//object{ AxisXYZ_(  2.10, 3.65, 10.5, T_Dark, T_Light, 0, 0, 0.55,0.55,1.2) scale 0.5}
///--------------------------------------------------------------------
#include concat(In_Path2,Txt_Path) // "A4 - box street.txt"
#end
// --------  //                                                                                 
#if (Typ=105)  // box wood board
BaseScene0_o()
object{ Grass_Ground_Small  translate<0,0.0001,0> }
//plane{<0,1,0>,0   texture{pigment{color rgb<1,1,1>*1.3}}} 
camera{ angle 35 location <-0.30, 1.00, -0.70> look_at< 0.20, 0.00,  0.38>}
light_source{ < 2.5, 2.50, -4.50>  color rgb<0.9,0.9,1>*0.2 shadowless}  // flash 
//------------------------------------------------------------------------
object{ AxisXYZ_(  3.80, 3.65, 10.5, T_Dark, T_Light, 35,  25, 0.85,0.55,1.2) scale 0.1}
///--------------------------------------------------------------------
#include concat(In_Path2,Txt_Path) // "A5 - box wood board.txt"
#end


#if (Typ=120)  // prism x
BaseScene0_o()
camera{ angle 55 location <2, 3.00, -3.00> look_at< 0.0, 0.60,  0.0>}
light_source{ < 2.5, 2.50, -4.50>  color rgb<0.9,0.9,1>*0.2 shadowless}  // flash 
//------------------------------------------------------------------------
object{ AxisXYZ_(  3.50, 3.65, 10.5, T_Dark, T_Light, 40, -35, 0.75,0.75,1.2) scale 0.5}
///--------------------------------------------------------------------
#include concat(In_Path2,Txt_Path) // "B0 - prism X.txt"
#end
// ----
#if (Typ=121)  // prism y
BaseScene0_o()
camera{ angle 52 location <1.2, 3.00, -4.00> look_at< 0.0, 0.90,  0.0>}
light_source{ < 2.5, 2.50, -4.50>  color rgb<0.9,0.9,1>*0.2 shadowless}  // flash 
//------------------------------------------------------------------------
object{ AxisXYZ_(  3.50, 4.25, 15.5, T_Dark, T_Light, 50, -35, 0.85,0.75,1.2) scale 0.5}
///--------------------------------------------------------------------
#include concat(In_Path2,Txt_Path) // "B1 - prism Y.txt"
#end

// ---- house shapes
#if (Typ=130)  // prism Z roof1
BaseScene0_o()
camera{ angle 55 location <1.5, 2.20, -3.50> look_at< 0.0, 0.78,  0.0>}
light_source{ <1.5, 2.20, -3.50>  color rgb<0.9,0.9,1>*0.2 shadowless}  // flash 
//------------------------------------------------------------------------
object{ AxisXYZ_(  3.50, 4.15, 16.5, T_Dark, T_Light, 10, -25, 0.65,0.55,1.2) scale 0.5}
///--------------------------------------------------------------------
#include concat(In_Path2,Txt_Path) // "B2 - prism Z house1.txt"
#end
// ---- house shapes
#if (Typ=131) // prism Z roof1 
BaseScene0_o()
camera{ angle 57 location <1.5, 2.20, -3.50> look_at< 0.0, 0.78,  0.0>}
light_source{ <1.5, 2.20, -3.50>  color rgb<0.9,0.9,1>*0.2 shadowless}  // flash 
//------------------------------------------------------------------------
object{ AxisXYZ_(  3.50, 4.15, 1.5, T_Dark, T_Light, 10, -25, 0.65,0.55,0.8) scale 0.5}
///--------------------------------------------------------------------
#include concat(In_Path2,Txt_Path) // "B3 - prism Z roof1.txt"
#end
/*
// new: Feb-2013
#case(132)  #declare Txt_Path="B4 - prism Z house2.txt"  #break
#case(133)  #declare Txt_Path="B5 - prism Z roof2.txt"  #break
#case(134)  #declare Txt_Path="B6 - prism X prism Z union.txt"  #break
#case(135)  #declare Txt_Path="B7 - prism X prism Z union2.txt"  #break
#case(136)  #declare Txt_Path="B8 - prism X prism Z union difference.txt"  #break
#case(137)  #declare Txt_Path="B9 - prism X prism Z intersection.txt"  #break
*/
// ---- house shapes
#if (Typ=132) // prism 
BaseScene0_o()
camera{ angle 57 location <1.5, 2.20, -3.50> look_at< 0.0, 0.78,  0.0>}
light_source{ <1.5, 2.20, -3.50>  color rgb<0.9,0.9,1>*0.2 shadowless}  // flash 
//------------------------------------------------------------------------
object{ AxisXYZ_(  3.50, 4.15, 1.5, T_Dark, T_Light, 10, -25, 0.65,0.55,0.8) scale 0.5}
///--------------------------------------------------------------------
#include concat(In_Path2,Txt_Path) // "B4 - prism Z house2.txt"
#end
#if (Typ=133) // prism 
BaseScene0_o()
camera{ angle 57 location <1.5, 2.20, -3.50> look_at< 0.0, 0.78,  0.0>}
light_source{ <1.5, 2.20, -3.50>  color rgb<0.9,0.9,1>*0.2 shadowless}  // flash 
//------------------------------------------------------------------------
object{ AxisXYZ_(  3.50, 4.15, 1.5, T_Dark, T_Light, 10, -25, 0.65,0.55,0.8) scale 0.5}
///--------------------------------------------------------------------
#include concat(In_Path2,Txt_Path) // "B5 - prism Z roof2.txt"
#end
#if (Typ=134) // prism 
BaseScene0_o()
camera{ angle 57 location <1.5, 2.20, -3.50> look_at< 0.0, 0.78,  0.0>}
light_source{ <1.5, 2.20, -3.50>  color rgb<0.9,0.9,1>*0.2 shadowless}  // flash 
//------------------------------------------------------------------------
object{ AxisXYZ_(  3.50, 4.15, 1.5, T_Dark, T_Light, 10, -25, 0.65,0.55,0.8) scale 0.5}
///--------------------------------------------------------------------
#include concat(In_Path2,Txt_Path) // "B6 - prism X prism Z union.txt"
#end
#if (Typ=135) // prism 
BaseScene0_o()
camera{ angle 57 location <1.5, 2.20, -3.50> look_at< 0.0, 0.78,  0.0>}
light_source{ <1.5, 2.20, -3.50>  color rgb<0.9,0.9,1>*0.2 shadowless}  // flash 
//------------------------------------------------------------------------
object{ AxisXYZ_(  3.50, 4.15, 1.5, T_Dark, T_Light, 10, -25, 0.65,0.55,0.8) scale 0.5}
///--------------------------------------------------------------------
#include concat(In_Path2,Txt_Path) // "B7 - prism X prism Z union2.txt"
#end
#if (Typ=136) // prism 
BaseScene0_o()
camera{ angle 57 location <1.5, 2.20, -3.50> look_at< 0.0, 0.78,  0.0>}
light_source{ <1.5, 2.20, -3.50>  color rgb<0.9,0.9,1>*0.2 shadowless}  // flash 
//------------------------------------------------------------------------
object{ AxisXYZ_(  3.50, 4.15, 1.5, T_Dark, T_Light, 10, -25, 0.65,0.55,0.8) scale 0.5}
///--------------------------------------------------------------------
#include concat(In_Path2,Txt_Path) // "B8 - prism X prism Z union difference.txt"
#end
#if (Typ=137) // prism 
BaseScene0_o()
camera{ angle 57 location <1.5, 2.20, -3.50> look_at< 0.0, 0.78,  0.0>}
light_source{ <1.5, 2.20, -3.50>  color rgb<0.9,0.9,1>*0.2 shadowless}  // flash 
//------------------------------------------------------------------------
object{ AxisXYZ_(  3.50, 4.15, 1.5, T_Dark, T_Light, 10, -25, 0.65,0.55,0.8) scale 0.5}
///--------------------------------------------------------------------
#include concat(In_Path2,Txt_Path) // "B9 - prism X prism Z intersection.txt"
#end
// end house shapes








// ----  50 - prism Y linear_sweep
#if (Typ=150) // prism sweeps splines 
object{ Sand_Ground translate<0,0.0001,0> } 
camera{ angle 50 location <0.0, 1.50, -3.50> look_at< 0.0, 0.6,  0.0>}
light_source{< 1500, 1500,-2500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 1.50, 2.00, -3.00>  color rgb<0.9,0.9,1>*0.2 shadowless}  // flash 
MySky() // sky  
//--------------------------------------------------------------------
#include concat(In_Path2,Txt_Path) // "C0 - prism Y linear_spline.txt"
#end
// ----  50 - prism Y linear_sweep
#if (Typ=152) // prism sweeps splines 
object{ Sand_Ground translate<0,0.0001,0> } 
camera{ angle 50 location <0.0, 1.50, -3.50> look_at< 0.0, 0.6,  0.0>}
light_source{< 1500, 1500,-2500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 1.50, 2.00, -3.00>  color rgb<0.9,0.9,1>*0.2 shadowless}  // flash 
MySky() // sky  
//--------------------------------------------------------------------
#include concat(In_Path2,Txt_Path) // "C2 - prism Y cubic_spline.txt"
#end

// --------  // plane                                                                                
#if (Typ=160) 
#include "10 - Ready made scenes/M2 - Basic Scene M2 - Squared plane XZ 3D far.txt"
#include concat(In_Path2,Txt_Path) //  "D0 - plane.txt"
#end







