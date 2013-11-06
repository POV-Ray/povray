// Insert menu illustration scene
// Author Friedrich A. Lohmueller, Feb-2013
#version 3.7;
global_settings{ assumed_gamma 1.0 } 

#default{ finish{ ambient 0.1 diffuse 0.9 }}
#include "shapes.inc"
#include "shapes2.inc"
#include "colors.inc"
#include "textures.inc"
#include "glass.inc"

#declare In_Path = "35 - Shapes3/"
//-------------------------------------------------------------- 
// samples for  
// #include "shapes3.inc"
// -------------------------------------------------------------   

 
 //  #declare Typ =20; // for tests

#switch (Typ)  //-----------------------------------------------------------------------------
#case( 1)  #declare Txt_Path="01 - Segment_of_Torus.txt"  #break
#case( 3)  #declare Txt_Path="03 - Segment_of_CylinderRing.txt"  #break
#case( 5)  #declare Txt_Path="05 - Segment_of_Object.txt"  #break
#case(12)  #declare Txt_Path="12 - Egg.txt"  #break
#case(14)  #declare Txt_Path="14 - Egg_Shape.txt"  #break

#case(15)  #declare Txt_Path="15 - Facetted_Egg.txt"  #break
#case(16)  #declare Txt_Path="16 - Facetted_Egg_Shape.txt"  #break
#case(17)  #declare Txt_Path="17 - Facetted_Sphere.txt"  #break
#case(18)  #declare Txt_Path="18 - Facetted_Sphere 2.txt"  #break
#case(19)  #declare Txt_Path="18 - Facetted_Sphere 3.txt"  #break
#case(20)  #declare Txt_Path="18 - Facetted_Sphere 4.txt"  #break
#case(21)  #declare Txt_Path="18 - Ring_Sphere_2.txt"  #break

#case(22)  #declare Txt_Path="20 - Column_N.txt"  #break
#case(23)  #declare Txt_Path="23 - Column_N_AB.txt"  #break

#case(24)  #declare Txt_Path="24 - Pyramid_N_AB.txt"  #break
#case(25)  #declare Txt_Path="25 - Pyramid_N.txt"  #break       
#case(26)  #declare Txt_Path="26 - Pyramid_N truncated.txt"  #break 

#case(30)  #declare Txt_Path="30 - Round_Pyramid_N_in truncated.txt"  #break
#case(31)  #declare Txt_Path="31 - Round_Pyramid_N_in truncated.txt"  #break
#case(32)  #declare Txt_Path="32 - Round_Pyramid_N_out truncated.txt"  #break
#case(33)  #declare Txt_Path="33 - Round_Pyramid_N_out truncated.txt"  #break
#case(34)  #declare Txt_Path="34 - Round_Pyramid_N_in columnar.txt"  #break
#case(35)  #declare Txt_Path="35 - Round_Pyramid_N_out.txt"  #break
#case(36)  #declare Txt_Path="36 - Round_Pyramid_N_in wireframe.txt"  #break
#case(37)  #declare Txt_Path="37 - Round_Pyramid_N_out wireframe.txt"  #break
#case(38)  #declare Txt_Path="38 - Round_Pyramid_N_out transparent.txt"  #break

#case(40)  #declare Txt_Path="40 - Round_Cylinder_Tube open.txt"  #break
#case(42)  #declare Txt_Path="42 - Round_Cylinder_Tube filled.txt"  #break
#case(45)  #declare Txt_Path="45 - Rounded_Tube open.txt"  #break
#case(47)  #declare Txt_Path="47 - Rounded_Tube_AB.txt"  #break

#case(55)  #declare Txt_Path="55 - Round_N_Tube_Polygon octagonal.txt"  #break
#case(56)  #declare Txt_Path="56 - Round_N_Tube_Polygon hexagonal.txt"  #break
#case(57)  #declare Txt_Path="57 - Round_N_Tube_Polygon transparent.txt"  #break

#case(60)  #declare Txt_Path="60 - Round_Conic_Torus transparent.txt"  #break
#case(61)  #declare Txt_Path="61 - Round_Conic_Torus.txt"  #break
#case(65)  #declare Txt_Path="65 - Round_Conic_Prism.txt"  #break

#case(71)  #declare Txt_Path="71 - Half_Hollowed_Rounded_Cylinder1.txt"  #break
#case(72)  #declare Txt_Path="72 - Half_Hollowed_Rounded_Cylinder2.txt"  #break

#end
// -------------------------------------------------------------------------------------------
// --------
// #include concat(In_Path,Txt_Path) 
// right x*image_width/image_height
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
          P_end   - ( vnormalize(P_end - P_start)*9.5*R_Vector), R_Vector  }
cone    { P_end   - ( vnormalize(P_end - P_start)*10*R_Vector), 3*R_Vector, P_end, 0 }
cone    { P_start + ( vnormalize(P_end - P_start)*10*R_Vector), 3*R_Vector, P_start, 0 }
}// end of union
#end //-------------------------------------------------------------------------- end of macro

//------------------------------ the Axes --------------------------------
//------------------------------------------------------------------------
#macro Axis_( AxisLen, Dark_Texture,Light_Texture) 
 union{
    cylinder { <0,-AxisLen,0>,<0,AxisLen,0>,0.035
               texture{checker texture{Dark_Texture } 
                               texture{Light_Texture}
                       translate<0.1,0,0.1>}
             }
    cone{<0,AxisLen,0>,0.12 ,<0,AxisLen+0.45,0>,0
          texture{Dark_Texture}
         }
     } // end of union                   
#end // of macro "Axis()"
//------------------------------------------------------------------------
#macro AxisXYZ( AxisLenX, AxisLenY, AxisLenZ, Tex_Dark, Tex_Light)
//--------------------- drawing of 3 Axes --------------------------------
#declare Text_Rotate =<20,-45,0>; 
union{
#if (AxisLenX != 0)
 object { Axis_(AxisLenX, Tex_Dark, Tex_Light)   rotate< 0,0,-90>}// x-Axis
 text   { ttf "arial.ttf",  "x",  0.15,  0  texture{Tex_Dark} 
          rotate Text_Rotate scale 0.6 translate <AxisLenX+0.10,0.3,0.00>}
#end // of #if 
#if (AxisLenY != 0)
 object { Axis_(AxisLenY, Tex_Dark, Tex_Light)   rotate< 0,0,  0>}// y-Axis
 text   { ttf "arial.ttf",  "y",  0.15,  0  texture{Tex_Dark}    
          rotate <Text_Rotate.x,0,0> scale 0.65 translate <-0.55,AxisLenY+0.10,-0.10> rotate <0,Text_Rotate.y,0>}
#end // of #if 
#if (AxisLenZ != 0)
 object { Axis_(AxisLenZ, Tex_Dark, Tex_Light)   rotate<90,0,  0>}// z-Axis
 text   { ttf "arial.ttf",  "z",  0.15,  0  texture{Tex_Dark}
          rotate Text_Rotate scale 0.75 translate <-0.55,0.2,AxisLenZ+0.10>}
#end // of #if 
} // end of union
#end// of macro "AxisXYZ( ... )"
//------------------------------------------------------------------------
#declare Texture_A_Dark  = texture {
                               pigment{ color rgb<1,0.45,0>}
                               finish { phong 1}
                             }
#declare Texture_A_Light = texture { 
                               pigment{ color rgb<1,1,1>}
                               finish { phong 1}
                             }

//-------------------------------------------------- end of coordinate axes
//------------------------------------------------------------------------

// sky -------------------------------------------------------------------
#declare Sky_01 = 
sphere{<0,0,0>,1     pigment{ gradient <0,1,0>
                     color_map{ [0   color rgb<0.24,0.34,0.56>*0.6]        
                                [0.5 color rgb<0.24,0.34,0.56>*0.3] 
                                [0.5 color rgb<0.24,0.34,0.56>*0.3] 
                                [1.0 color rgb<0.24,0.34,0.56>*0.6]  }
                     scale 2 }
                     finish{ ambient 1 diffuse 0 }
        scale 10000
      } // end of sky_sphere
//------------------------------------------------------------------------
#declare Sky_02 =
union{ 
sphere{<0,0,0>,1    
               pigment{ //color rgb<1,1,1> } //
                     gradient <0,1,0>
                     color_map{ [0   color rgb<0.24,0.34,0.56>*1.0]
                                [0.5 color rgb<0.24,0.34,0.56>*0.3]
                                [0.5 color rgb<0.24,0.34,0.56>*0.3]
                                [1.0 color rgb<0.24,0.34,0.56>*1.0]
                              }
                     scale 2 }
                     finish{ ambient 1 diffuse 0 }
        scale 50000
           } // end of  sphere

plane{<0,1,0>,1 hollow  
       texture{ pigment{ bozo turbulence 0.92
                         color_map { [0.00 rgb <0.24, 0.32, 1.0>*0.7]
                                     [0.50 rgb <0.24, 0.32, 1.0>*0.7]
                                     [0.70 rgb <1,1,1>]
                                     [0.85 rgb <0.25,0.25,0.25>]
                                     [1.0 rgb <0.5,0.5,0.5>]}
                        scale<1,1,1.5>*1.25  translate< 0,0,0>
                       }
                finish {ambient 1 diffuse 0} }      
       scale 5000}
} // end union 
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// --------
#if (Typ=1) // 01 - Segment_of_Torus.txt"
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0 , 0.25 , 0.0>}
light_source{<1500,2500,-2500> color White}
object{ AxisXYZ( 3.35, 2.75, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
#include concat(In_Path,Txt_Path) // 01 - Segment_of_Torus.txt"
#end

// --------
#if (Typ=3) // 03 - Segment_of_CylinderRing.txt
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at<0.0 , 0.25 , 0.0>}
light_source{<1500,2500,-2500> color White}
object{ AxisXYZ( 3.35, 2.75, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
#include concat(In_Path,Txt_Path) // 03 - Segment_of_CylinderRing.txt
#end




// --------
#if (Typ=5) // 05 - Segment_of_Object.txt
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0 , 0.25 , 0.0>}
light_source{<1500,2500,-2500> color White}
object{ AxisXYZ( 3.35, 2.75, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
#include concat(In_Path,Txt_Path) // 05 - Segment_of_Object.txt
#end 


// --------                                                  
#if (Typ=12) // 12 - Egg.txt
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0, 1.00, 0.0>}
light_source{<1200,2500,-3000> color White}
object{ AxisXYZ( 1.35, 4.0, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
#include concat(In_Path,Txt_Path) // 12 - Egg.txt
#end
// --------
#if (Typ=14) // 14 - Egg_Shape.txt
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0, 1.00, 0.0>}
light_source{<1200,2500,-3000> color White}
object{ AxisXYZ( 1.35, 4.0, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
#include concat(In_Path,Txt_Path) // 14 - Egg_Shape.txt
#end
// --------
#if (Typ=15) // 15 - Facetted_Egg.txt
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0, 1.00, 0.0>}
light_source{<1200,2500,-3000> color White}
object{ AxisXYZ( 1.35, 4.0, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
#include concat(In_Path,Txt_Path) // 15 - Facetted_Egg.txt
#end
// --------
#if (Typ=16) // 16 - Facetted_Egg_Shape.txt
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0 , 0.00 , 0.0>}
light_source{<1500,2500,-2500> color White}
object{ AxisXYZ( 2.5, 2.25, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
#include concat(In_Path,Txt_Path) // 16 - Facetted_Egg_Shape.txt
#end
// --------
#if (Typ=17) // 17 - Facetted_Sphere.txt
camera{ angle 35 location <3.5 , 2.5 ,-3.0> right x*image_width/image_height look_at <0.0 , 0.15 , 0.0>}
light_source{<-1200,1500,-2500> color rgb<1,1,1>*0.8}
light_source{ <3.5 , 2.5 ,-3.0> color rgb<1,1,1>*0.2 shadowless }
object{ AxisXYZ( 3.0, 2.2, 4.5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
#include concat(In_Path,Txt_Path) // 17 - Facetted_Sphere.txt
#end
// --------
#if (Typ=18) // 18 - Facetted_Sphere.txt
camera{ angle 38 location  < 2.50, 3.00, -2.70> right x*image_width/image_height look_at <0.0 , 0.10, 0.0>}
light_source{<-500,500,-2500> color rgb<1,1,1>*0.8}                // sun
light_source{ <1000,2500,-1000>  color rgb<0.9,0.9,1>*0.2 shadowless}// flash
object{ AxisXYZ( 3.0, 2.2, 4.5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
object{ Sky_02   } 
#include concat(In_Path,Txt_Path) // 18 - Facetted_Sphere 2.txt
#end
// --------
#if (Typ=19) // 19 - Facetted_Sphere.txt
camera{ angle 38 location  < 2.50, 3.00, -2.70> right x*image_width/image_height look_at <0.0 , 0.10, 0.0>}
light_source{<-500,500,-2500> color rgb<1,1,1>*0.8}                // sun
light_source{ <1000,2500,-1000>  color rgb<0.9,0.9,1>*0.2 shadowless}// flash
object{ AxisXYZ( 3.0, 2.2, 4.5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
object{ Sky_02   } 
#include concat(In_Path,Txt_Path) // 18 - Facetted_Sphere 3.txt
#end
// --------
#if (Typ=20) // 17 - Facetted_Sphere 3.txt
camera{ angle 38 location  < 2.50, 3.00, -2.70> right x*image_width/image_height look_at <0.0 , 0.10, 0.0>}
light_source{<-500,500,-2500> color rgb<1,1,1>*0.8}                // sun
light_source{ <1000,2500,-1000>  color rgb<0.9,0.9,1>*0.2 shadowless}// flash
object{ AxisXYZ( 3.0, 2.2, 4.5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
object{ Sky_02   } 
#include concat(In_Path,Txt_Path) // 18 - Facetted_Sphere 4.txt
#end                                                 
// --------
// --------
#if (Typ=21) // 18 - Ring_Sphere.txt
camera{ angle 35 location<3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0 , 0.15 , 0.0>}
light_source{<-500,1500,-2500> color rgb<1,1,1>*0.9}
light_source{ <3.5 , 2.5 ,-3.0> color rgb<1,1,1>*0.1 shadowless }
object{ AxisXYZ( 3.35, 2.6, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
#include concat(In_Path,Txt_Path) // 18 - Ring_Sphere.txt
#end
// --------
#if (Typ=22) // 20 - Column_N.txt
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0, 0.25, 0.0>}
light_source{< 600,2500,-3000> color White}
object{ AxisXYZ( 3.35, 2.75, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
#include concat(In_Path,Txt_Path) // 20 - Column_N.txt
#end
// --------
#if (Typ=23) // 23 - Column_N_AB.txt
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0, 0.25, 0.0>}
light_source{<-1500,2500,-2500> color White*0.9}
light_source{<3.5 , 3.5 ,-3.0> color White*0.1 shadowless}
object{ AxisXYZ( 3.35, 2.75, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
#include concat(In_Path,Txt_Path) // 23 - Column_N_AB.txt
#end
// --------
#if (Typ=24) // 24 - Pyramid_N_AB.txt
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0, 0.25, 0.0>}
light_source{<500,2500,-2500> color White}
object{ AxisXYZ( 3.35, 2.75, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
// ------------------------------------------------------------------
sphere{<0,0,0>,0.05 texture{ pigment{rgb<0.6,1,0>} finish{phong 1}} translate<0,1,-1>}
text{ttf "arial.ttf","A", 0.025, 0.0 scale 0.25  rotate<20,-45,0>  
                    texture{ pigment{rgb<0.6,1,0>} finish{phong 1}} translate<0+1,1+0.4,-1-1>}
sphere{<0,0,0>,0.04 texture{ pigment{rgb<1,0.18 ,0>} finish{phong 1}} translate<1.5,1.5,-0.5>}
text{ttf "arial.ttf","B", 0.025, 0.0 scale 0.3 rotate<20,-45,2>  
                    texture{ pigment{rgb<1,0.3,0>} finish{phong 1}} translate<1.5,1.5+0.3,-0.5>}
// ------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // 24 - Pyramid_N_AB.txt
#end
// --------
#if (Typ=25) // 25 - Pyramid_N.txt
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0, 0.25, 0.0>}
light_source{<-1500,2500,-2500> color White*0.9}
light_source{<3.5 , 3.5 ,-3.0> color White*0.1 shadowless}
object{ AxisXYZ( 3.35, 2.75, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
#include concat(In_Path,Txt_Path) // 25 - Pyramid_N.txt //0 
#end
// --------
#if (Typ=26) // 26 - Pyramid_N truncated.txt
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0, 0.25, 0.0>}
light_source{<-1500,2500,-2500> color White*0.9}
light_source{<3.5 , 3.5 ,-3.0> color White*0.1 shadowless}
object{ AxisXYZ( 3.35, 2.75, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
#include concat(In_Path,Txt_Path) // 26 - Pyramid_N truncated.txt    //1
#end
// --------
#if (Typ=30) // 30 - Round_Pyramid_N_in truncated.txt 
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0 , 0.25 , 0.0>}
light_source{<-1500,2500,-2500> color White*0.9}
light_source{<3.5 , 3.5 ,-3.0> color White*0.1 shadowless}
object{ AxisXYZ( 3.35, 2.75, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
#include concat(In_Path,Txt_Path) // 30 - Round_Pyramid_N_in truncated.txt //_15 
#end
// --------                                                          
#if (Typ=31) // 31 - Round_Pyramid_N_in truncated.txt
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0 , 0.25 , 0.0>}
light_source{<1200,2500,-3000> color White}
object{ AxisXYZ( 3.35, 2.75, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
// ------------------------------------------------------------------
object{ Vector( <0,0,0>,<0.70,0,0>, 0.03) rotate<0,0,0> translate<0,0.05,0> pigment{ color rgb<0.4,0.7,0.0> } }
object{ Vector( <0,0,0>,<0.55,0,0>, 0.03) rotate<0,0,0> translate<0,0.82,0>  pigment{ color rgb<1,0.8,0.0> } }
// ------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // 31 - Round_Pyramid_N_in truncated.txt // 06
#end
// --------
#if (Typ=32) // 32 - Round_Pyramid_N_out truncated.txt  
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0, 0.25, 0.0>}
light_source{<1200,2500,-3000> color White}
object{ AxisXYZ( 3.35, 2.75, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
// ------------------------------------------------------------------
object{ Vector( <0,0,0>,<1.20,0,0>, 0.03) translate<0,0.08,0> pigment{ color rgb<0.4,0.7,0.0> } }
object{ Vector( <0,0,0>,<0.95,0,0>, 0.03) translate<0,0.8,0> pigment{ color rgb<1,0.8,0.0> } }
// ------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // 32 - Round_Pyramid_N_out truncated.txt //out 06 
#end
// --------
#if (Typ=33) // 33 - Round_Pyramid_N_out truncated.txt
camera{ angle 35 location <3.5, 3.5,-3.0> right x*image_width/image_height look_at <0.0, 0.25, 0.0>}
light_source{<500,2500,-3000> color White}
object{ AxisXYZ( 3.35, 2.75, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
object{ Vector( <0,0,0>,<0.70,0,0>, 0.03) rotate<0,0,-52> rotate<20,0,0> translate<1.0,1.1,0> pigment{ color rgb<0.4,0.7,0.0>}}
sphere { <0,0,0>, 0.04 
         texture{ pigment{ color rgb< 0.4,0.7, 0.0>}
                  finish { phong 1 } 
                }   
         translate  <1,1.1,0>
       }  // end of sphere  
// ------------------------------------------------------------------
#include concat(In_Path,Txt_Path) //  33 - Round_Pyramid_N_out truncated.txt // out_08
#end
// --------
#if (Typ=34) // 34 - Round_Pyramid_N_in columnar.txt 
camera{ angle 40 location<3.2 , 2 ,-3.00> right x*image_width/image_height look_at <0.0 , 0.50 , 0.0>}
light_source{<- 500,2500,-2500> color rgb<1,1,1>*0.8}
light_source{ <3.5 , 2.5 ,-3.0> color rgb<1,1,1>*0.2 shadowless }
object{ AxisXYZ( 2.85, 2.8, 6.0, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
//------------------------------------------------------------------- 
object{ Vector( <0,0,0>,<1.20,0,0>, 0.03) translate<0,0.72,0> pigment{ color rgb< 0.4,0.7,0.0>}}
#include concat(In_Path,Txt_Path) // 34 - Round_Pyramid_N_in columnar.txt // in _12
//------------------------------------------------------------------- 
object{ Round_Pyramid_N_in( // defined by circumcircle radii 
                             8 , // number of side faces 
                             <0,00,0>, 1.21, <0,0.70,0>, 1.21 , // A, radius A, B, radius B, 
                             0.015, // wire radius or border radius 
                             0,  //  1 = Filled
                             0   // 0 = union, 1 = merge for transparent materials           
                       )//-----------------------------------------------------------------
        texture{ pigment{ color rgb< 1,0.7,0> }
                 finish { phong 1}
               } // end of texture
       } // end of object ------------------------------------------------------------------   
//------------------------------------------------------------------- 
#end
// --------
#if (Typ=35) // 35 - Round_Pyramid_N_out_12.txt  
camera{ angle 40 location<3.2 , 2 ,-3.00> right x*image_width/image_height look_at <0.0 , 0.50 , 0.0>}
light_source{<-2500,2500,-2500> color rgb<1,1,1>*1}
object{ AxisXYZ( 2.85, 2.8, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
#include concat(In_Path,Txt_Path) // 35 - Round_Pyramid_N_out_12.txt  //   out_12
#end
// --------
#if (Typ=36) // 36 - Round_Pyramid_N_in wireframe.txt
camera{ angle 40 location <3.2 , 2 ,-3.00> right x*image_width/image_height look_at <0.0, 0.50, 0.0>}
light_source{< 500,2500,-2500> color rgb<1,1,1>*0.9}
light_source{ <3.5 , 2.5 ,-3.0> color rgb<1,1,1>*0.1 shadowless }
object{ AxisXYZ( 2.85, 2.8, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
// ------------------------------------------------------------------
object{ Vector( <0,0,0>,<1.02,0,0>, 0.02) translate<0,0.11,0> pigment{ color rgb<0.7,0,0.0> } }
//------------------------------------------------------------------------------------
#include concat(In_Path,Txt_Path) //  36 - Round_Pyramid_N_in wireframe.txt  // in_11
#end
// --------
#if (Typ=37) // 37 - Round_Pyramid_N_out wireframe.txt 
camera{ angle 40 location <3.2 , 2 ,-3.00> right x*image_width/image_height look_at <0.0, 0.50, 0.0>}
light_source{< 500,2500,-2500> color rgb<1,1,1>*0.9}
light_source{ <3.5 , 2.5 ,-3.0> color rgb<1,1,1>*0.1 shadowless }
object{ AxisXYZ( 2.85, 2.8, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
// ------------------------------------------------------------------
object{ Vector( <0,0,0>,<1.30,0,0>, 0.02)  translate<0,0.11,0> pigment{ color rgb< 0.7,1,0.0> } }
//-------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // 37 - Round_Pyramid_N_out wireframe.txt // out _11.
#end
// --------


#if (Typ=38) // 38 - Round_Pyramid_N_out transparent.txt
camera{ angle 40 location<3.2 , 2 ,-3.00> right x*image_width/image_height look_at<0.0, 0.50, 0.0>}
light_source{< 00,2500,-2500> color rgb<1,1,1>*0.8}
light_source{ <3.5 , 2.5 ,-3.0> color rgb<1,1,1>*0.2 shadowless }
object{ AxisXYZ( 2.85, 2.8, 6.0, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
#include concat(In_Path,Txt_Path) // 38 - Round_Pyramid_N_out transparent.txt    //out_14
#end
// --------
#if (Typ=40) //  40 - Round_Cylinder_Tube open.txt
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0 , 0.25 , 0.0>}
light_source{<  0,2500,-2500> color White}
object{ AxisXYZ( 3.35, 2.75, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
// ------------------------------------------------------------------
object{ Vector( <0,0,0>,<0,-0.95,0>, 0.03) rotate<50,0,25> translate<1.0,0.9,-0.3> pigment{color rgb<1,0.48,0>}}
sphere{<0,0,0>,0.05 texture{ pigment{rgb<0.6,1,0>} finish{phong 1}} translate<-1,0.2,-0.2>}
text{ttf "arial.ttf","A", 0.035, 0.0 scale 0.25  rotate<20,-45,0>  
                    texture{ pigment{rgb<0.6,1,0>} finish{phong 1}} translate<-1.4,0.6,-0.85>}
sphere{<0,0,0>,0.05 texture{ pigment{rgb<1,0.48 ,0>} finish{phong 1}} translate<1.0,0.9,-0.3>}
text{ttf "arial.ttf","B", 0.025, 0.0 scale 0.25 rotate<20,-45,2>  
                    texture{ pigment{rgb<1,0.48,0>} finish{phong 1}} translate<1.45,0.9+0.4,-0.5>}
//------------------------------------------------------------------- 
#include concat(In_Path,Txt_Path) // 40 - Round_Cylinder_Tube open.txt  // 1 
#end
// --------
#if (Typ=42) // 42 - Round_Cylinder_Tube filled.txt 
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0,0.25,0.0>}
light_source{<  0,2500,-2500> color White}
object{ AxisXYZ( 3.35, 2.8, 5.5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
// ------------------------------------------------------------------
object{ Vector( <0,0,0>,<0,-0.85,0>, 0.03) rotate<50,0,25> translate<1.0,0.9,-0.3> pigment{color rgb<1,0.48,0>}}
sphere{<0,0,0>,0.05 texture{ pigment{rgb<0.6,1,0>} finish{phong 1}} translate<-1,-0.2,-0.2>}
text{ttf "arial.ttf","A", 0.035, 0.0 scale 0.25  rotate<20,-45,0>  
                    texture{ pigment{rgb<0.6,1,0>} finish{phong 1}} translate<-1.4,0.6,-0.85>}
sphere{<0,0,0>,0.05 texture{ pigment{rgb<1,0.18 ,0>} finish{phong 1}} translate<1.0,0.9,-0.3>}
text{ttf "arial.ttf","B", 0.025, 0.0 scale 0.25 rotate<20,-45,2>  
                    texture{ pigment{rgb<1,0.3,0>} finish{phong 1}} translate<1.4,0.9+0.4,-0.5>}
//------------------------------------------------------------------------------ 
#include concat(In_Path,Txt_Path) // 42 - Round_Cylinder_Tube filled.txt  // _3
#end
// --------
#if (Typ=45) // 45 - Rounded_Tube open.txt 
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0,0.25,0.0>}
light_source{<  00,2500,-2500> color White}
object{ AxisXYZ( 3.35, 2.75, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
// ------------------------------------------------------------------
object{ Vector( <0,0,0>,<0.80,0,0>, 0.03) translate<0,0.70,0> pigment{color rgb<0.7,1,0>}}
sphere{<0,0,0>,0.05 texture{ pigment{rgb<0.6,1,0>} finish{phong 1}} translate<0,0.7,0>}
object{ Vector( <0,0,0>,<1.50,0,0>, 0.03) rotate<0,-25,0> translate<0,0.70,0.05> pigment{color rgb<1,0.48,0>}}
object{ Distance_Marker( <0,0,0>,<0,0.70,0>, 0.02 ) translate<1.52,0,0> rotate<0, 0,0> pigment{color rgb<1,0.28,0>}}
//------------------------------------------------------------------- 
#include concat(In_Path,Txt_Path) // 45 - Rounded_Tube open.txt                          // _1
#end
// --------
#if (Typ=47) // 47 - Rounded_Tube_AB.txt 
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0,0.25,0.0>}
light_source{<  00,2500,-2500> color White}
object{ AxisXYZ( 3.35, 2.75, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
// ------------------------------------------------------------------
object{ Vector( <0,0,0>,<0,-0.50,0>, 0.025) rotate<90,0,0> translate<1.0,0.9,-0.3> pigment{color rgb<0.7,1,0>}}
sphere{<0,0,0>,0.05 texture{ pigment{rgb<0.6,1,0>} finish{phong 1}} translate<0,0.7,0>}
object{ Vector( <0,0,0>,<0,-0.95,0>, 0.03) rotate<50,0,25> translate<1.0,0.9,-0.3> pigment{color rgb<1,0.48,0>}}
sphere{<0,0,0>,0.05 texture{ pigment{rgb<0.6,1,0>} finish{phong 1}} translate<-1,0.2,-0.2>}
text{ttf "arial.ttf","A", 0.035, 0.0 scale 0.25  rotate<20,-45,0>  
                    texture{ pigment{rgb<0.6,1,0>} finish{phong 1}} translate<-1.4,0.6,-0.85>}
sphere{<0,0,0>,0.05 texture{ pigment{rgb<1,0.18 ,0>} finish{phong 1}} translate<1.0,0.9,-0.3>}
text{ttf "arial.ttf","B", 0.025, 0.0 scale 0.25 rotate<20,-45,2>  
                    texture{ pigment{rgb<1,0.3,0>} finish{phong 1}} translate<1.4,0.9+0.4,-0.5>}
//----------------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // 47 - Rounded_Tube_AB.txt                            // _1
#end
// --------
#if (Typ=55) // 55 - Round_N_Tube_Polygon octagonal.txt
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0,0.25,0.0>}
light_source{<1500,2500,-2500> color White}
object{ AxisXYZ( 3.35, 2.70, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
#include concat(In_Path,Txt_Path) //  55 - Round_N_Tube_Polygon octagonal.txt         // _3
#end
// --------
#if (Typ=56) // 56 - Round_N_Tube_Polygon hexagonal.jpg 
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0,0.25,0.0>}
light_source{<1500,2500,-2500> color White}
object{ AxisXYZ( 3.35, 2.70, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
#include concat(In_Path,Txt_Path)//  56 - Round_N_Tube_Polygon hexagonal.jpg      // _1
#end
// --------
#if (Typ=57) // 57 - Round_N_Tube_Polygon transparent.txt 
camera{ angle 35 location <3.5 , 3.5 ,-3.0> right x*image_width/image_height look_at <0.0,0.25,0.0>}
light_source{<1500,2500,-2500> color White}
object{ AxisXYZ( 3.35, 2.70, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
#include concat(In_Path,Txt_Path) // 57 - Round_N_Tube_Polygon transparent.txt    // _2
#end
// --------
#if (Typ=60) // 60 - Round_Conic_Torus transparent.txt 
camera{ angle 45 location<0.0 , 0.25 ,-4.0>right x*image_width/image_height look_at <0.0,0.25,0.0>}
light_source{< 0,2500,-2500> color White}
object{ AxisXYZ( 2.25, 2.4 , 0, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
// ------------------------------------------------------------------
object{ Vector( <0,0,0>,<0.50, 0,0>, 0.025) rotate< 0,0,20> translate<0.0,0.8,-0.13> pigment{color rgb<0.7,1,0>}}
object{ Vector( <0,0,0>,<0.80,0,0>, 0.025) rotate< 0,0,20> translate<0,0,-0.13> pigment{color rgb<0.7,1,0>}}
object{ Distance_Marker( <0,0,0>,<0,0.8,0>, 0.03 ) translate<0,0,-0.13> rotate<0, 0,0> pigment{color rgb<0.2, 1, 0>}}
sphere{<0,0,0>,0.05 texture{ pigment{rgb<0.6,1,0>} finish{phong 1}} translate<0,0,0>}
sphere{<0,0,0>,0.05 texture{ pigment{rgb<1,0.18 ,0>} finish{phong 1}} translate<0.0,0.80,-0.12>}
#include concat(In_Path,Txt_Path) // 60 - Round_Conic_Torus transparent.txt               //_2
#end
// --------
#if (Typ=61) // 61 - Round_Conic_Torus.txt 
camera{ angle 41 location<3.5 , 2.5 ,-3.0> right x*image_width/image_height look_at<0.0, 0.80, 0.0>}
light_source{< 0,2500,-2500> color White}
object{ AxisXYZ( 2.25, 3.8, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
   // ------------------------------------------------------------------
object{ Vector( <0,0,0>,<0.80, 0,0>, 0.025) rotate<90,0,0> translate<0.0,1,-0.13> pigment{color rgb<0.7,1,0>}}
object{ Vector( <0,0,0>,<0.50,0,0>, 0.025) rotate< 0,0,-30> translate<0,0,-0.13> pigment{color rgb<0.7,1,0>}}
object{ Distance_Marker( <0,0,0>,<0,1,0>, 0.02 ) translate<0,0,-0.13> rotate<0, 0,0> pigment{color rgb<0.2, 1, 0>}}
sphere{<0,0,0>,0.05 texture{ pigment{rgb<0.6,1,0>} finish{phong 1}} translate<0,0,0>}
sphere{<0,0,0>,0.05 texture{ pigment{rgb<1,0.18 ,0>} finish{phong 1}} translate<0.0,1.00,-0.12>}
#include concat(In_Path,Txt_Path) // 61 - Round_Conic_Torus.txt               //_1
#end
// --------
#if (Typ=65) // 65 - Round_Conic_Prism.txt 
camera{ angle 41 location<3.5, 2.5,-3.0> right x*image_width/image_height look_at  <0.0,0.80,0.0>}
light_source{< 0,2500,-2500> color White}
object{ AxisXYZ( 2.25, 3.8, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
// ------------------------------------------------------------------
object{ Vector( <0,0,0>,<0.90, 0,0>, 0.025) rotate<0,0,-20> translate<0.0,0.95,-0.83> pigment{color rgb<0.5,1,0>}}
object{ Vector( <0,0,0>,<0.40,0,0>, 0.025) rotate< 0,0,-20> translate<0,0,-0.83> pigment{color rgb<0.5,1,0>}}
object{ Distance_Marker( <0,0,0>,<0,0.95,0>, 0.02 ) translate<0,0,-0.83> rotate<0, 0,0> pigment{color rgb<0.2, 1, 0>}}
sphere{<0,0,0>,0.05 texture{ pigment{rgb<0.6,1,0>} finish{phong 1}} translate<0,0,-0.83>}
sphere{<0,0,0>,0.05 texture{ pigment{rgb<1,0.18 ,0>} finish{phong 1}} translate<0.0,1.00,-0.83>}
//----------------------------------------------------------------------------
#include concat(In_Path,Txt_Path) // 65 - Round_Conic_Prism.txt 
#end
// --------
#if (Typ=71) // 71 - Half_Hollowed_Rounded_Cylinder1.txt
camera{ angle 41 location<3.5, 2.5,-3.0> right x*image_width/image_height look_at <0.0,0.80,0.0>}
light_source{< 0,2500,-2500> color White}
object{ AxisXYZ( 2.55, 3.8, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
#include concat(In_Path,Txt_Path) // 71 - Half_Hollowed_Rounded_Cylinder1.txt 
#end
// --------


#if (Typ=72) // 72 - Half_Hollowed_Rounded_Cylinder2.txt 
camera{ angle 41 location<3.5, 2.5,-3.0> right x*image_width/image_height look_at <0.0,0.80,0.0>}
light_source{< 0,2500,-2500> color White}
object{ AxisXYZ( 2.55, 3.8, 5, Texture_A_Dark, Texture_A_Light) scale 0.5}
object{ Sky_01 } 
#include concat(In_Path,Txt_Path) // 72 - Half_Hollowed_Rounded_Cylinder2.txt 
#end
// --------

// end of file








