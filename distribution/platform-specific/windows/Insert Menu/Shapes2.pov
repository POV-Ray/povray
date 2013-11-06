// Insert menu illustration scene
// Author Friedrich A. Lohmueller, June-2012
#version 3.7;
global_settings{ assumed_gamma 1.0 } 

#default{ finish{ ambient 0.1 diffuse 0.9 }}
#include "shapes.inc"
#include "shapes2.inc"
#include "colors.inc"
#include "textures.inc"
#include "glass.inc"

#declare In_Path = "30 - Shapes2/"

    // #declare Typ =51; // for tests

#switch (Typ)  //-----------------------------------------------------------------------------
#case(10)  #declare Txt_Path="10 - Wire_Box.txt"  #break
#case(11)  #declare Txt_Path="11 - Round_Box.txt"  #break
#case(12)  #declare Txt_Path="12 - Round_Cylinder.txt"  #break
#case(13)  #declare Txt_Path="13 - Round_Cone.txt"  #break
#case(14)  #declare Txt_Path="14 - Round_Cone2.txt"  #break
#case(15)  #declare Txt_Path="15 - Round_Cone3.txt"  #break

#case(17)  #declare Txt_Path="17 - Spheroid.txt"  #break
#case(19)  #declare Txt_Path="19 - Supercone.txt"  #break

#case(20)  #declare Txt_Path="20 - Hexagon.txt"  #break
#case(21)  #declare Txt_Path="21 - Rhomboid.txt"  #break
#case(22)  #declare Txt_Path="22 - Pyramid.txt"  #break
#case(23)  #declare Txt_Path="23 - Pyramid2.txt"  #break
#case(24)  #declare Txt_Path="24 - Tetrahedron.txt"  #break
#case(25)  #declare Txt_Path="25 - Octahedron.txt"  #break
#case(26)  #declare Txt_Path="26 - Dodecahedron.txt"  #break
#case(27)  #declare Txt_Path="27 - Icosahedron.txt"  #break

#case(41)  #declare Txt_Path="41 - Supertorus_1.txt"  #break
#case(42)  #declare Txt_Path="42 - Supertorus_2.txt"  #break
#case(43)  #declare Txt_Path="43 - Supertorus_3.txt"  #break
#case(44)  #declare Txt_Path="44 - Supertorus_4.txt"  #break

#case(50)  #declare Txt_Path="50 - superellipsoid_0.txt"  #break
#case(51)  #declare Txt_Path="51 - superellipsoid_1.txt"  #break
#case(52)  #declare Txt_Path="52 - superellipsoid_2.txt"  #break
#case(53)  #declare Txt_Path="53 - superellipsoid_3.txt"  #break
#case(54)  #declare Txt_Path="54 - superellipsoid_4.txt"  #break
#case(55)  #declare Txt_Path="55 - superellipsoid_5.txt"  #break

#case(70)  #declare Txt_Path="70 - Paraboloid_X.txt"  #break
#case(71)  #declare Txt_Path="71 - Paraboloid_Y.txt"  #break
#case(72)  #declare Txt_Path="72 - Paraboloid_Z.txt"  #break

#case(75)  #declare Txt_Path="75 - Hyperboloid.txt"  #break
#case(76)  #declare Txt_Path="76 - Hyperboloid_Y.txt"  #break
#case(77)  #declare Txt_Path="77 - Hyperboloid_Z.txt"  #break
#end
// -------------------------------------------------------------------------------------------
// --------
// #include concat(In_Path,Txt_Path) 

// ---------------------------
//------------------------------ the Axes --------------------------------
//------------------------------------------------------------------------
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
     } // end of union
#end // of macro "Axis()"
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
// sky -------------------------------------------------------------------
#macro MySky (Turn)
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
// ---------------------------
// ---------------------------
// ---------------------------
// ---------------------------
// ---------------------------





//------------------------------------------------------------------------------------


#if (Typ=10)
MySky (2)
camera{ angle 30 location < 4.50, 3.00, -4.50> look_at< 0.10, 0.65,  0.00>}
light_source{<2500, 2500,    0> color rgb<1,1,1>*0.8}     // sun light
light_source{<4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 3.55, 3.25, 5.5, T_Dark, T_Light,30,-45,   0.75,0.75,1.05) scale 0.5}
union{
sphere { <1,1,1>, 0.065
         texture{ T_Red }  no_shadow
       }  // end of sphere -------------------------------
sphere { <-1,0,-1>, 0.065
         texture{ T_Red }  no_shadow
       }  // end of sphere -------------------------------
object { // Wire_Box(A, B, WireRadius, UseMerge)
         Wire_Box(<-1,0,-1>,<1,1,1>, 0.015   , 0)
         texture{ pigment{ color rgb<1,1,1>*1.1}}
         scale<1,1,1>  rotate<0, 0,0> translate<0,0.0,0>
       } // ---------------------------------------------
translate<0,0.1,0>
}// end union
#include concat(In_Path,Txt_Path) // 10 - Wire_Box.txt"
#end

#if (Typ=11)
MySky (2)
camera{ angle 30 location < 4.50, 3.00, -4.50> look_at< 0.10, 0.65,  0.00>}
light_source{<2500, 2500,    0> color rgb<1,1,1>*0.8}     // sun light
light_source{<4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 3.25, 3.25, 6.1, T_Dark, T_Light,30,-45,   0.75,0.75,1.05) scale 0.5}
union{
sphere { <1,1,1>, 0.045
         texture{ T_Red }  no_shadow
       }  // end of sphere -------------------------------
sphere { <-1,0,-1>, 0.045
         texture{ T_Red }  no_shadow
       }  // end of sphere -------------------------------
object { // Wire_Box(A, B, WireRadius, UseMerge)
         Wire_Box(<-1,0,-1>,<1,1,1>, 0.015   , 0)
         texture{ pigment{ color rgb<1,1,1>*1.1} }

       } // ---------------------------------------------
translate<0,0.1,0>
}// end union
#include concat(In_Path,Txt_Path) // 11 - Round_Box.txt"
#end

#if (Typ=12)
MySky (2)
camera{ angle 28 location < 4.50, 4.00, -4.50> look_at< 0.10, 0.80,  0.00>}
light_source{<2500, 2500,    0> color rgb<1,1,1>*0.8}     // sun light
light_source{<4.50, 4.00,-4.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 2.15, 3.55, 4.5, T_Dark, T_Light,30,-45,   0.75,0.75,1.05) scale 0.5}

sphere { <0,0,0>, 0.045
         texture{ T_Red }  no_shadow
       }  // end of sphere -------------------------------
sphere { <0,1.5,0>, 0.045
         texture{ T_Red }  no_shadow
       }  // end of sphere -------------------------------
torus{0.50,0.01 translate<0,0,0> texture{ pigment{ color rgb<1,1,1>*1.1}} }
torus{0.50,0.01 translate<0,1.5,0> texture{ pigment{ color rgb<1,1,1>*1.1}} }
//------------------------------------------------------------------------------------
object{ Vector( <0,0,0>,<0.50,0,0>, 0.02) pigment{ color rgb< 0.8,0.0,0.0> }
       rotate<0,-30,0> translate<0,1.50,0> no_shadow }
object{ Vector( <0,0,0>,<0.50,0,0>, 0.02) pigment{ color rgb< 0.8,0.0,0.0> }
       rotate<0,-30,0> translate<0,0.00,0> no_shadow }
//------------------------------------------------------------------------------------

#include concat(In_Path,Txt_Path) // 12 - Round_Cylinder.txt"
#end

#if (Typ=13)
MySky (2)
camera{ angle 25 location < 4.50, 3.00, -4.50> look_at< 0.10, 0.70,  0.00>}
light_source{<2500, 2500,    0> color rgb<1,1,1>*0.8}     // sun light
light_source{<4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 2.15, 2.95, 4.5, T_Dark, T_Light,30,-45,   0.75,0.75,1.05) scale 0.5}
sphere { <0,1.20,0>, 0.045 texture{ T_Red }  no_shadow }
torus{0.70,0.01 translate<0,0,0> texture{ pigment{ color rgb<1,1,1>*1.1}} }
torus{0.40,0.01 translate<0,1.20,0> texture{ pigment{ color rgb<1,1,1>*1.1}} }
//------------------------------------------------------------------------------------
object{ Vector( <0,0,0>,<0.70,0,0>, 0.02) pigment{ color rgb< 0.8,0.0,0.0> }
       rotate<0,-25,0> translate<0,0.00,0> no_shadow }
object{ Vector( <0,0,0>,<0.40,0,0>, 0.02) pigment{ color rgb< 0.8,0.0,0.0> }
       rotate<0,-25,0> translate<0,1.20,0> no_shadow }
//------------------------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // 13 - Round_Cone.txt"
#end
#if (Typ=14)
MySky (2)
camera{ angle 30 location < 4.50, 3.00, -4.50> look_at< 0.10, 0.50,  0.00>}
light_source{<-1500, 2500,    0> color rgb<1,1,1>*0.8}     // sun light
light_source{< 4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 3.55, 2.95, 5.5, T_Dark, T_Light,30,-45,   0.75,0.75,1.05) scale 0.5}
sphere { <0,0.00,0>, 0.045 texture{ T_Red }  no_shadow  }
sphere { <0,1.00,0>, 0.045 texture{ T_Red }  no_shadow  }
//------------------------------------------------------------------------------------
object{ Vector( <0,0,0>,<0.70,0,0>, 0.02) pigment{ color rgb< 0.8,0.0,0.0> }
       rotate<0,-25,0> translate<0,0.00,0> no_shadow }
object{ Vector( <0,0,0>,<0.40,0,0>, 0.02) pigment{ color rgb< 0.8,0.0,0.0> }
       rotate<0,-25,0> translate<0,1.00,0> no_shadow }
torus{0.70,0.02 translate<0,0,0> texture{ pigment{ color rgb<1,1,1>*1.1}} }
torus{0.40,0.02 translate<0,1.00,0> texture{ pigment{ color rgb<1,1,1>*1.1}} }
//------------------------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // 14 - Round_Cone2.txt"
#end
#if (Typ=15)
MySky (2)
camera{ angle 30 location < 4.50, 3.00,-4.50> look_at< 0.10, 0.20,  0.00>}
light_source{<-1500, 2500,     0> color rgb<1,1,1>*0.8}     // sun light
light_source{ < 4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 3.15, 2.35, 4.5, T_Dark, T_Light,30,-45,   0.75,0.75,1.05) scale 0.5}
sphere { <0,0.00,0>, 0.045 texture{ T_Red }  no_shadow  }
sphere { <0,1.00,0>, 0.045 texture{ T_Red }  no_shadow  }
//------------------------------------------------------------------------------------
object{ Vector( <0,0,0>,<0.70,0,0>, 0.02) pigment{ color rgb< 0.8,0.0,0.0> }
       rotate<0,-25,0> translate<0,0.00,0> no_shadow }
object{ Vector( <0,0,0>,<0.40,0,0>, 0.02) pigment{ color rgb< 0.8,0.0,0.0> }
       rotate<0,-25,0> translate<0,0.800,0> no_shadow }
torus{0.70,0.02 translate<0,0,0> texture{ pigment{ color rgb<1,1,1>*1.1}} }
torus{0.40,0.02 translate<0,0.80,0> texture{ pigment{ color rgb<1,1,1>*1.1}} }
#include concat(In_Path,Txt_Path) // 15 - Round_Cone3.txt"
#end

#if (Typ=17)
MySky (1)
camera{ angle 45 location < 5.50, 4.00, -6.50> look_at<-1.80, 1.00,  -0.80>}
light_source{< 1000, 1000,-1500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 5.50, 4.00,-6.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 1.5, 3.00, 2.5, T_Dark, T_Light,30,-45,   0.65,0.65,0.85) scale 1}
sphere { <-1.50,2.00,-2.00>, 0.065 texture{ pigment{ color rgb< 0,1,1> }  }  no_shadow  }
//------------------------------------------------------------------------------------
object{ Vector( <0,0,0>,<-1.50,2.00,-2.00>, 0.04) pigment{ color rgb< 0,1,1> }
       rotate<0,0,0> translate<0,0.00,0> no_shadow }
object{ Vector( <0,0,0>,<2.0,0,0>, 0.03) pigment{ color rgb< 0.8,0.0,0.0> }
       rotate<0,0,0> translate<-1.50,2.00,-2.00> no_shadow }
object{ Vector( <0,0,0>,<0,1.2,0>, 0.03) pigment{ color rgb< 0.8,0.0,0.0> }
       rotate<0,0,0> translate<-1.50,2.00,-2.00> no_shadow }
object{ Vector( <0,0,0>,<0,0,2.5>, 0.03) pigment{ color rgb< 0.8,0.0,0.0> }
       rotate<0,0,0> translate<-1.50,2.00,-2.00> no_shadow }
torus{2.5,0.02 scale<2/2.5,1,1> translate<-1.50,2.00,-2.00> texture{ pigment{ color rgb<1,1,1>*1.1}} }
torus{2,0.02 rotate<90,0,0> scale<1,1.2/2,1>   translate<-1.50,2.00,-2.00> texture{ pigment{ color rgb<1,1,1>*1.1}} }
torus{2.5,0.02 rotate<0,0,90>scale<1,1.2/2.5,1> translate<-1.50,2.00,-2.00> texture{ pigment{ color rgb<1,1,1>*1.1}} }
// Spheroid(<-1.50,2.00,-2.00>, <2.0,1.2,2.5> )//   CenterVector,   RadiusVector Rx,Ry,Rz )
#include concat(In_Path,Txt_Path) // 17 - Spheroid.txt"
#end
#if (Typ=19)
MySky (1)
camera{ angle 30 location < 4.50, 4.00, -3.50> look_at< 0.10, 0.95,  0.00>}
light_source{< 1500, 2500,-2500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 4.50, 4.00,-3.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 1.55, 3.75, 3.85, T_Dark, T_Light,20,-45,   0.75,0.75,1.05) scale 0.5}
sphere { <0,0.00,0>, 0.055 texture{ T_Red }  no_shadow  }
sphere { <0,1.7,0.00>, 0.055 texture{ T_Red }  no_shadow  }
//------------------------------------------------------------------------------------
object{ Vector( <0,0,0>,<0.5,0,0>, 0.03) pigment{ color rgb< 0.8,0.0,0.0> }
        translate<0,0.00,0> no_shadow }
object{ Vector( <0,0,0>,<0,0,1.0>, 0.03) pigment{ color rgb< 0.8,0.0,0.0> }
        translate<0,0.00,0> no_shadow }
//------------------------------------------------------------------------------------
object{ Vector( <0,0,0>,<0.7,0,0>, 0.025) pigment{ color rgb< 0.8,0.0,0.0> }
        translate<0,1.70,0> no_shadow }
object{ Vector( <0,0,0>,<0,0,0.3>, 0.025) pigment{ color rgb< 0.8,0.0,0.0> }
        translate<0,1.70,0> no_shadow }
//Supercone(<0.00,0.00,0.00>, 0.5, 1.0, // point A, axis Ax, axis Az,
//          <0.00,1.70,0.00>, 0.7, 0.3) // point B, axis Bx, axis Bz)
#include concat(In_Path,Txt_Path) // 19 - Supercone.txt"
#end




// --------
#if (Typ=20)
MySky (1)
camera{ angle 62 location < 2.50, 1.50, -2.50> look_at< 0.00, 0.10,  0.00>}
light_source{<1500,2500,-1000> color rgb<1,1,1>*0.9}     // sun light
light_source{<2.50,1.50,-2.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_(  3.15, 2.65, 9, T_Dark, T_Light,10,-55,   0.75,0.75,1.25) scale 0.5}
#include concat(In_Path,Txt_Path) // 20 - Hexagon.txt"
#end
// --------
#if (Typ=21)
MySky (1)
camera{ angle 35 location < 4.50, 3.00, -4.50> look_at< 0.30, 0.15,  0.00>}
light_source{<1500,2500,-1000> color rgb<1,1,1>*0.9}     // sun light
light_source{<4.50,3.00,-4.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_(  3.35, 2.65, 8, T_Dark, T_Light,10,-55,   0.75,0.75,1.25) scale 0.5}
#include concat(In_Path,Txt_Path) // 21 - Rhomboid.txt"
#end
// --------
#if (Typ=22)
MySky (1)
camera{ angle 34 location < 4.50, 3.00, -4.50> look_at< 0.10,-0.10,  0.00>}
light_source{<1500,2500,-1000> color rgb<1,1,1>*0.9}     // sun light
light_source{<4.50,3.00,-4.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_(  3.15, 2.25, 3, T_Dark, T_Light,10,-55,   0.75,0.75,1.25) scale 0.5}
#include concat(In_Path,Txt_Path) // 22 - Pyramid.txt"
#end
// --------
#if (Typ=23)
MySky (1)
camera{ angle 38 location < 4.50, 3.00, -4.50> look_at< 0.10, 0.50,  0.00>}
light_source{< 1500,2500,-1000> color rgb<1,1,1>*0.9}     // sun light
light_source{< 4.50,3.00,-4.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_(  4.05, 3.65, 6.5, T_Dark, T_Light,10,-55,   0.75,0.75,1.05) scale 0.5}
#include concat(In_Path,Txt_Path) // 23 - Pyramid2.txt"
#end
// --------


#if (Typ=24)
MySky (1)
camera{ angle 36 location < 4.50, 3.00, -4.50> look_at< 0.10, 0.45,  0.00>}
light_source{< 1500,2500,-1000> color rgb<1,1,1>*0.9}     // sun light
light_source{< 4.50,3.00,-4.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_(  3.55, 3.55, 5.5, T_Dark, T_Light,10,-55,   0.75,0.75,1.05) scale 0.5}
#include concat(In_Path,Txt_Path) // 24 - Tetrahedron.txt"
#end
#if (Typ=25)
MySky (1)
camera{ angle 39 location < 4.50, 3.00, -4.50> look_at< 0.10, 0.40,  0.00>}
light_source{< 1500,2500,-1000> color rgb<1,1,1>*0.9}     // sun light
light_source{< 4.50,3.00,-4.50> color rgb<0.9,0.9,1>*0.1}  // flash light3
object{ AxisXYZ_(  3.35, 3.55, 5.5, T_Dark, T_Light,10,-55,   0.75,0.75,1.05) scale 0.5}
#include concat(In_Path,Txt_Path) // 25 - Octahedron.txt"
#end
#if (Typ=26)
MySky (1)
camera{ angle 35 location < 4.50, 3.00, -4.50> look_at< 0.10, 0.20,  0.00>}
light_source{< 1500,2500,-1000> color rgb<1,1,1>*0.9}     // sun light
light_source{< 4.50,3.00,-4.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 3.15, 2.65, 5.5, T_Dark, T_Light,10,-45,   0.75,0.75,1.05) scale 0.5}
#include concat(In_Path,Txt_Path) // 26 - Dodecahedron.txt"
#end
#if (Typ=27)
MySky (2)
camera{ angle 33 location < 4.50, 3.00, -4.50> look_at< 0.10, 0.35,  0.00>}
light_source{<2500, 2500,-0000> color rgb<1,1,1>*0.8}     // sun light
light_source{<4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 3.25, 2.95, 5.5, T_Dark, T_Light,30,-45,   0.75,0.75,1.05) scale 0.5}
#include concat(In_Path,Txt_Path) // 27 - Icosahedron.txt"
#end

#if (Typ=41)
MySky (1)
camera{ angle 30 location < 4.50, 3.00, -4.50> look_at< 0.10, 0.20,  0.00>}
light_source{<-1500, 2500,-2500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1} // flash light
object{ AxisXYZ_( 3.15, 2.35, 4.5, T_Dark, T_Light,30,-45,   0.75,0.75,1.05) scale 0.5}
#include concat(In_Path,Txt_Path) // 41 - Supertorus_1.txt"
#end
#if (Typ=42)
MySky (1)
camera{ angle 30 location < 4.50, 3.00, -4.50> look_at< 0.10, 0.20,  0.00>}
light_source{<-1500, 2500,-2500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1} // flash light
object{ AxisXYZ_( 3.15, 2.35, 4.5, T_Dark, T_Light,30,-45,   0.75,0.75,1.05) scale 0.5}
#include concat(In_Path,Txt_Path) // 42 - Supertorus_2.txt"
#end
#if (Typ=43)
MySky (1)
camera{ angle 30 location < 4.50, 3.00, -4.50> look_at< 0.10, 0.20  0.00>}
light_source{<-1500, 2500,-2500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1} // flash light
object{ AxisXYZ_( 3.15, 2.35, 4.5, T_Dark, T_Light,30,-45,   0.75,0.75,1.05) scale 0.5}
#include concat(In_Path,Txt_Path) // 43 - Supertorus_3.txt"
#end
#if (Typ=44)
MySky (1)
camera{ angle 30 location < 4.50, 3.00, -4.50> look_at< 0.10, 0.20,  0.00>}
light_source{<-1500, 2500,-2500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1} // flash light
object{ AxisXYZ_( 3.15, 2.35, 4.5, T_Dark, T_Light,30,-45,   0.75,0.75,1.05) scale 0.5}
#include concat(In_Path,Txt_Path) // 44 - Supertorus_4.txt"
#end



#if (Typ=50)
MySky (1)
camera{ angle 35 location < 4.50, 3.00, -4.50> look_at< 0.10, 0.00,  0.00>}
light_source{<-1500, 2500,-2500> color rgb<1,1,1>*0.8}      // sun light
light_source{< 4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1}  // flash light
object{ AxisXYZ_( 3.15, 2.45, 4.5, T_Dark, T_Light,30,-45,   0.75,0.75,1.05) scale 0.5}
object{ Wire_Box(<-1,-1,-1>,<1,1,1>, 0.015, 0) texture{ pigment{ color rgb<1,1,1>*1.1}} }
#include concat(In_Path,Txt_Path) // 50 - superellipsoid_0.txt"
#end
#if (Typ=51)
MySky (1)
camera{ angle 35 location < 4.50, 3.00, -4.50> look_at< 0.10, 0.00,  0.00>}
light_source{<-1500, 2500,-2500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1} // flash light
object{ AxisXYZ_( 3.15, 2.45, 4.5, T_Dark, T_Light,30,-45,   0.75,0.75,1.05) scale 0.5}
object{ Wire_Box(<-1,-1,-1>,<1,1,1>, 0.015, 0) texture{ pigment{ color rgb<1,1,1>*1.1}} }
#include concat(In_Path,Txt_Path) // 51 - superellipsoid_1.txt"
#end
#if (Typ=52)
MySky (1)
camera{ angle 35 location < 4.50, 3.00, -4.50> look_at< 0.10, 0.00,  0.00>}
light_source{<-1500, 2500,-2500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1} // flash light
object{ AxisXYZ_( 3.15, 2.45, 4.5, T_Dark, T_Light,30,-45,   0.75,0.75,1.05) scale 0.5}
object{ Wire_Box(<-1,-1,-1>,<1,1,1>, 0.015, 0) texture{ pigment{ color rgb<1,1,1>*1.1}} }
#include concat(In_Path,Txt_Path) // 52 - superellipsoid_2.txt"
#end
#if (Typ=53)
MySky (1)
camera{ angle 35 location < 4.50, 3.00, -4.50> look_at< 0.10, 0.00,  0.00>}
light_source{<-1500, 2500,-2500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1} // flash light
object{ AxisXYZ_( 3.15, 2.45, 4.5, T_Dark, T_Light,30,-45,   0.75,0.75,1.05) scale 0.5}
object { Wire_Box(<-1,-1,-1>,<1,1,1>, 0.015, 0) texture{ pigment{ color rgb<1,1,1>*1.1}} }
#include concat(In_Path,Txt_Path) // 53 - superellipsoid_3.txt"
#end
#if (Typ=54)
MySky (1)
camera{ angle 35 location < 4.50, 3.00, -4.50> look_at< 0.10, 0.00,  0.00>}
light_source{<-1500, 2500,-2500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1} // flash light
object{ AxisXYZ_( 3.15, 2.45, 4.5, T_Dark, T_Light,30,-45,   0.75,0.75,1.05) scale 0.5}
object{ Wire_Box(<-1,-1,-1>,<1,1,1>, 0.015, 0) texture{ pigment{ color rgb<1,1,1>*1.1}} }
#include concat(In_Path,Txt_Path) // 54 - superellipsoid_4.txt"
#end
#if (Typ=55)
MySky (1)
camera{ angle 35 location < 4.50, 3.00, -4.50> look_at< 0.10, 0.00,  0.00>}
light_source{<-1500, 2500,-2500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1} // flash light
object{ AxisXYZ_( 3.15, 2.45, 4.5, T_Dark, T_Light,30,-45,   0.75,0.75,1.05) scale 0.5}
object{ Wire_Box(<-1,-1,-1>,<1,1,1>, 0.015, 0) texture{ pigment{ color rgb<1,1,1>*1.1}} }
#include concat(In_Path,Txt_Path) // 55 - superellipsoid_5.txt"
#end





#if (Typ=70)
MySky (1)
camera{ angle 38 location < 4.50, 3.00, -4.50> look_at< 1.00,-0.06,  0.00>}
light_source{< 1500, 2500,-2500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1} // flash light
object{ AxisXYZ_( 5.25, 2.15, 6.5, T_Dark, T_Light,20,-45,   0.75,0.75,1.05) scale 0.5}
object{ Wire_Box(<0,-2,-2>,<4.0, 2,2>, 0.03, 0) scale 0.5 texture{ pigment{ color rgb<1,1,1>*1.1}} }
#include concat(In_Path,Txt_Path) // 70 - Paraboloid_X.txt"
#end
#if (Typ=71)
MySky (1)
camera{ angle 36 location < 4.50, 3.00, -4.50> look_at< 0.10, 1.00,  0.00>}
light_source{<-1500, 2500,-2500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1} // flash light
object{ AxisXYZ_( 2.85, 4.45, 6.75, T_Dark, T_Light,30,-45,   0.75,0.75,1.05) scale 0.5}
object{ Wire_Box(<-2,0,-2>,<2, 4.00,2>, 0.03, 0) scale 0.5 texture{ pigment{ color rgb<1,1,1>*1.1}} }
#include concat(In_Path,Txt_Path) // 71 - Paraboloid_Y.txt"
#end
#if (Typ=72)
MySky (1)
camera{ angle 31 location < 4.50, 3.00, -4.50> look_at< 0.10,-0.10,  1.00>}
light_source{<-1500, 2500,-2500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1} // flash light
object{ AxisXYZ_( 3.15, 2.45, 9.5, T_Dark, T_Light,30,-45,   0.75,0.75,1.05) scale 0.5}
object{ Wire_Box(<-2,-2,0>,<2,2, 4.00>, 0.03, 0) scale 0.5 texture{ pigment{ color rgb<1,1,1>*1.1}} }
#include concat(In_Path,Txt_Path) // 72 - Paraboloid_Z.txt"
#end
#if (Typ=75)
MySky (1)
camera{ angle 36 location < 4.50, 3.00, -4.50> look_at< 0.10, 0.75,  0.00>}
light_source{<-1500, 2500,-2500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1} // flash light
object{ AxisXYZ_( 3.55, 2.45, 7.5, T_Dark, T_Light,30,-45,   0.75,0.75,1.05) scale 0.5}
object{ Wire_Box(<-2.0, 0.0,-3.0>,<2.0, 4.0,3.0>, 0.03, 0) scale 0.5 texture{ pigment{ color rgb<1,1,1>*1.1}} }
#include concat(In_Path,Txt_Path) // 75 - Hyperboloid.txt"
#end
#if (Typ=76)
MySky (1)
camera{ angle 36 location < 4.50, 3.00, -4.50> look_at< 0.10,-0.10,  0.00>}
light_source{<-1500, 2500,-2500> color rgb<1,1,1>*0.8}     // sun light
light_source{< 4.50, 3.00,-4.50> color rgb<0.9,0.9,1>*0.1} // flash light
object{ AxisXYZ_( 4.15, 2.45, 7.75, T_Dark, T_Light,30,-45,   0.75,0.75,1.15) scale 0.5}
object{ Wire_Box(<-2.25,-2.0,-2.25>,<2.25, 2.00,2.25>, 0.03, 0) scale 0.5 texture{ pigment{ color rgb<1,1,1>*1.1}} }
#include concat(In_Path,Txt_Path) // 76 - Hyperboloid_Y.txt"
#end
#if (Typ=77)
MySky (1)
camera{ angle 39 location < 2.50, 2.00, -5.50> look_at< 0.10,-0.10,  0.00>}
light_source{<-1500, 2500,-2500> color rgb<1,1,1>*0.8}     // sun light
light_source{<2.50, 2.00, -5.50> color rgb<0.9,0.9,1>*0.1} // flash light
object{ AxisXYZ_( 3.55, 2.45, 16.5, T_Dark, T_Light,10,-25,   0.75,0.75,1.35) scale 0.5}
object{ Wire_Box(<-2.25,-2.25,-2.0>,<2.25, 2.25,2.0>, 0.03, 0) scale 0.5 texture{ pigment{ color rgb<1,1,1>*1.1}} }
#include concat(In_Path,Txt_Path) // 77 - Hyperboloid_Z.txt"
#end



