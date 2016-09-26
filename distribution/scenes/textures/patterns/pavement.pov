//------------------------- pavement.pov ---------------------------
// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.
//
//------------------------------------------------------------------
//
// Renders all 112 pavement patterns with the exterior, interior and form
// settings specified.
//
// +A0.3 +H950 +W600

#version 3.7;
global_settings { assumed_gamma 1 }

#declare VarExterior = 0;    // 0,1,2   Exterior corner blunting/rounding.
#declare VarInterior = 0;    // 0,1,2   Interior corner blunting/rounding.
#declare VarForm     = 0;    // 0,1,2,3 Edge curvature in/out of corners.
#declare VarPatternScale = 1/2;
#declare VarCalkSize = 0.10;

//--- To scale the repeated pavement pattern to a unit square.
// #declare PavementTri_NrmScale  <1,1,2/sqrt(3)>   // (3 sides)
// #declare PavementQuad_NrmScale <1,1,1>           // (4 sides)
// #declare PavementHex_NrmScale  <1/3,1,1/sqrt(3)> // (6 sides)

//---
#declare White = srgbft <1,1,1,0,0>;
background { color White }
#declare Camera01y = camera {
    orthographic
    location <0,2,0>
    direction <0,-1,0>
    right 2*x*(image_width/image_height)
    up 2*<0,0,1>
}
#declare Light00 = light_source { <50,150,-250>, White }
#declare Box00 = box { <-1,-0.01,-1>,<1,0.01,1> }
#declare Red = srgbft <1,0,0,0,0>;
#declare Green = srgbft <0,1,0,0,0>;
#declare Azure = rgb <0,0.498,1>;
#declare Black = rgb <0,0,0>;
#declare Chartreuse_Green = rgb <0.5,1,0>;
#declare Spring_Green = rgb <0,1,0.5>;
#declare Yellow = rgb <1,1,0>;
#declare ColorMap6 = color_map {
    [ 0/6 Azure ]
    [ (1-VarCalkSize)/6 Azure ]
    [ (1-VarCalkSize)/6 Black ]
    [ 1/6 Black ]
    [ 1/6 Chartreuse_Green ]
    [ (2-VarCalkSize)/6 Chartreuse_Green ]
    [ (2-VarCalkSize)/6 Black ]
    [ 2/6 Black ]
    [ 2/6 Green ]
    [ (3-VarCalkSize)/6 Green ]
    [ (3-VarCalkSize)/6 Black ]
    [ 3/6 Black ]
    [ 3/6 Red ]
    [ (4-VarCalkSize)/6 Red ]
    [ (4-VarCalkSize)/6 Black ]
    [ 4/6 Black ]
    [ 4/6 Spring_Green ]
    [ (5-VarCalkSize)/6 Spring_Green ]
    [ (5-VarCalkSize)/6 Black ]
    [ 5/6 Black ]
    [ 5/6 Yellow ]
    [ (6-VarCalkSize)/6 Yellow ]
    [ (6-VarCalkSize)/6 Black ]
    [ 6/6 Black ]
}

#macro ThisPavementObj(_sides,_tiles,_pattern,_col,_row)
  object { Box00
    texture {
      pigment {
        pavement number_of_sides _sides number_of_tiles _tiles pattern _pattern
                 exterior VarExterior interior VarInterior form VarForm
        color_map { ColorMap6 }
        scale VarPatternScale
      }
    }
    translate <_col*2.2,0,_row*2.2>
  }
#end

#declare ObjPavement3_1_01 = ThisPavementObj(3,1,1,0,0)
#declare ObjPavement3_2_01 = ThisPavementObj(3,2,1,1,0)
#declare ObjPavement3_3_01 = ThisPavementObj(3,3,1,2,0)
#declare ObjPavement3_4_01 = ThisPavementObj(3,4,1,3,0)
#declare ObjPavement3_4_02 = ThisPavementObj(3,4,2,4,0)
#declare ObjPavement3_4_03 = ThisPavementObj(3,4,3,5,0)
#declare ObjPavement3_5_01 = ThisPavementObj(3,5,1,6,0)
#declare ObjPavement3_5_02 = ThisPavementObj(3,5,2,7,0)
#declare ObjPavement3_5_03 = ThisPavementObj(3,5,3,0,1)
#declare ObjPavement3_5_04 = ThisPavementObj(3,5,4,1,1)
#declare ObjPavement3_6_01 = ThisPavementObj(3,6,1,2,1)
#declare ObjPavement3_6_02 = ThisPavementObj(3,6,2,3,1)
#declare ObjPavement3_6_03 = ThisPavementObj(3,6,3,4,1)
#declare ObjPavement3_6_04 = ThisPavementObj(3,6,4,5,1)
#declare ObjPavement3_6_05 = ThisPavementObj(3,6,5,6,1)
#declare ObjPavement3_6_06 = ThisPavementObj(3,6,6,7,1)
#declare ObjPavement3_6_07 = ThisPavementObj(3,6,7,0,2)
#declare ObjPavement3_6_08 = ThisPavementObj(3,6,8,1,2)
#declare ObjPavement3_6_09 = ThisPavementObj(3,6,9,2,2)
#declare ObjPavement3_6_10 = ThisPavementObj(3,6,10,3,2)
#declare ObjPavement3_6_11 = ThisPavementObj(3,6,11,4,2)
#declare ObjPavement3_6_12 = ThisPavementObj(3,6,12,5,2)
#declare ObjPavement4_1_01 = ThisPavementObj(4,1,1,6,2)
#declare ObjPavement4_2_01 = ThisPavementObj(4,2,1,7,2)
#declare ObjPavement4_3_01 = ThisPavementObj(4,3,1,0,3)
#declare ObjPavement4_3_02 = ThisPavementObj(4,3,2,1,3)
#declare ObjPavement4_4_01 = ThisPavementObj(4,4,1,2,3)
#declare ObjPavement4_4_02 = ThisPavementObj(4,4,2,3,3)
#declare ObjPavement4_4_03 = ThisPavementObj(4,4,3,4,3)
#declare ObjPavement4_4_04 = ThisPavementObj(4,4,4,5,3)
#declare ObjPavement4_4_05 = ThisPavementObj(4,4,5,6,3)
#declare ObjPavement4_5_01 = ThisPavementObj(4,5,1,7,3)
#declare ObjPavement4_5_02 = ThisPavementObj(4,5,2,0,4)
#declare ObjPavement4_5_03 = ThisPavementObj(4,5,3,1,4)
#declare ObjPavement4_5_04 = ThisPavementObj(4,5,4,2,4)
#declare ObjPavement4_5_05 = ThisPavementObj(4,5,5,3,4)
#declare ObjPavement4_5_06 = ThisPavementObj(4,5,6,4,4)
#declare ObjPavement4_5_07 = ThisPavementObj(4,5,7,5,4)
#declare ObjPavement4_5_08 = ThisPavementObj(4,5,8,6,4)
#declare ObjPavement4_5_09 = ThisPavementObj(4,5,9,7,4)
#declare ObjPavement4_5_10 = ThisPavementObj(4,5,10,0,5)
#declare ObjPavement4_5_11 = ThisPavementObj(4,5,11,1,5)
#declare ObjPavement4_5_12 = ThisPavementObj(4,5,12,2,5)
#declare ObjPavement4_6_01 = ThisPavementObj(4,6,1,3,5)
#declare ObjPavement4_6_02 = ThisPavementObj(4,6,2,4,5)
#declare ObjPavement4_6_03 = ThisPavementObj(4,6,3,5,5)
#declare ObjPavement4_6_04 = ThisPavementObj(4,6,4,6,5)
#declare ObjPavement4_6_05 = ThisPavementObj(4,6,5,7,5)
#declare ObjPavement4_6_06 = ThisPavementObj(4,6,6,0,6)
#declare ObjPavement4_6_07 = ThisPavementObj(4,6,7,1,6)
#declare ObjPavement4_6_08 = ThisPavementObj(4,6,8,2,6)
#declare ObjPavement4_6_09 = ThisPavementObj(4,6,9,3,6)
#declare ObjPavement4_6_10 = ThisPavementObj(4,6,10,4,6)
#declare ObjPavement4_6_11 = ThisPavementObj(4,6,11,5,6)
#declare ObjPavement4_6_12 = ThisPavementObj(4,6,12,6,6)
#declare ObjPavement4_6_13 = ThisPavementObj(4,6,13,7,6)
#declare ObjPavement4_6_14 = ThisPavementObj(4,6,14,0,7)
#declare ObjPavement4_6_15 = ThisPavementObj(4,6,15,1,7)
#declare ObjPavement4_6_16 = ThisPavementObj(4,6,16,2,7)
#declare ObjPavement4_6_17 = ThisPavementObj(4,6,17,3,7)
#declare ObjPavement4_6_18 = ThisPavementObj(4,6,18,4,7)
#declare ObjPavement4_6_19 = ThisPavementObj(4,6,19,5,7)
#declare ObjPavement4_6_20 = ThisPavementObj(4,6,20,6,7)
#declare ObjPavement4_6_21 = ThisPavementObj(4,6,21,7,7)
#declare ObjPavement4_6_22 = ThisPavementObj(4,6,22,0,8)
#declare ObjPavement4_6_23 = ThisPavementObj(4,6,23,1,8)
#declare ObjPavement4_6_24 = ThisPavementObj(4,6,24,2,8)
#declare ObjPavement4_6_25 = ThisPavementObj(4,6,25,3,8)
#declare ObjPavement4_6_26 = ThisPavementObj(4,6,26,4,8)
#declare ObjPavement4_6_27 = ThisPavementObj(4,6,27,5,8)
#declare ObjPavement4_6_28 = ThisPavementObj(4,6,28,6,8)
#declare ObjPavement4_6_29 = ThisPavementObj(4,6,29,7,8)
#declare ObjPavement4_6_30 = ThisPavementObj(4,6,30,0,9)
#declare ObjPavement4_6_31 = ThisPavementObj(4,6,31,1,9)
#declare ObjPavement4_6_32 = ThisPavementObj(4,6,32,2,9)
#declare ObjPavement4_6_33 = ThisPavementObj(4,6,33,3,9)
#declare ObjPavement4_6_34 = ThisPavementObj(4,6,34,4,9)
#declare ObjPavement4_6_35 = ThisPavementObj(4,6,35,5,9)
#declare ObjPavement6_1_01 = ThisPavementObj(6,1,1,6,9)
#declare ObjPavement6_2_01 = ThisPavementObj(6,2,1,7,9)
#declare ObjPavement6_3_01 = ThisPavementObj(6,3,1,0,10)
#declare ObjPavement6_3_02 = ThisPavementObj(6,3,2,1,10)
#declare ObjPavement6_3_03 = ThisPavementObj(6,3,3,2,10)
#declare ObjPavement6_4_01 = ThisPavementObj(6,4,1,3,10)
#declare ObjPavement6_4_02 = ThisPavementObj(6,4,2,4,10)
#declare ObjPavement6_4_03 = ThisPavementObj(6,4,3,5,10)
#declare ObjPavement6_4_04 = ThisPavementObj(6,4,4,6,10)
#declare ObjPavement6_4_05 = ThisPavementObj(6,4,5,7,10)
#declare ObjPavement6_4_06 = ThisPavementObj(6,4,6,0,11)
#declare ObjPavement6_4_07 = ThisPavementObj(6,4,7,1,11)
#declare ObjPavement6_5_01 = ThisPavementObj(6,5,1,2,11)
#declare ObjPavement6_5_02 = ThisPavementObj(6,5,2,3,11)
#declare ObjPavement6_5_03 = ThisPavementObj(6,5,3,4,11)
#declare ObjPavement6_5_04 = ThisPavementObj(6,5,4,5,11)
#declare ObjPavement6_5_05 = ThisPavementObj(6,5,5,6,11)
#declare ObjPavement6_5_06 = ThisPavementObj(6,5,6,7,11)
#declare ObjPavement6_5_07 = ThisPavementObj(6,5,7,0,12)
#declare ObjPavement6_5_08 = ThisPavementObj(6,5,8,1,12)
#declare ObjPavement6_5_09 = ThisPavementObj(6,5,9,2,12)
#declare ObjPavement6_5_10 = ThisPavementObj(6,5,10,3,12)
#declare ObjPavement6_5_11 = ThisPavementObj(6,5,11,4,12)
#declare ObjPavement6_5_12 = ThisPavementObj(6,5,12,5,12)
#declare ObjPavement6_5_13 = ThisPavementObj(6,5,13,6,12)
#declare ObjPavement6_5_14 = ThisPavementObj(6,5,14,7,12)
#declare ObjPavement6_5_15 = ThisPavementObj(6,5,15,0,13)
#declare ObjPavement6_5_16 = ThisPavementObj(6,5,16,1,13)
#declare ObjPavement6_5_17 = ThisPavementObj(6,5,17,2,13)
#declare ObjPavement6_5_18 = ThisPavementObj(6,5,18,3,13)
#declare ObjPavement6_5_19 = ThisPavementObj(6,5,19,4,13)
#declare ObjPavement6_5_20 = ThisPavementObj(6,5,20,5,13)
#declare ObjPavement6_5_21 = ThisPavementObj(6,5,21,6,13)
#declare ObjPavement6_5_22 = ThisPavementObj(6,5,22,7,13)

#declare UnionObjs = union {
    object { ObjPavement3_1_01 }
    object { ObjPavement3_2_01 }
    object { ObjPavement3_3_01 }
    object { ObjPavement3_4_01 }
    object { ObjPavement3_4_02 }
    object { ObjPavement3_4_03 }
    object { ObjPavement3_5_01 }
    object { ObjPavement3_5_02 }
    object { ObjPavement3_5_03 }
    object { ObjPavement3_5_04 }
    object { ObjPavement3_6_01 }
    object { ObjPavement3_6_02 }
    object { ObjPavement3_6_03 }
    object { ObjPavement3_6_04 }
    object { ObjPavement3_6_05 }
    object { ObjPavement3_6_06 }
    object { ObjPavement3_6_07 }
    object { ObjPavement3_6_08 }
    object { ObjPavement3_6_09 }
    object { ObjPavement3_6_10 }
    object { ObjPavement3_6_11 }
    object { ObjPavement3_6_12 }
    object { ObjPavement4_1_01 }
    object { ObjPavement4_2_01 }
    object { ObjPavement4_3_01 }
    object { ObjPavement4_3_02 }
    object { ObjPavement4_4_01 }
    object { ObjPavement4_4_02 }
    object { ObjPavement4_4_03 }
    object { ObjPavement4_4_04 }
    object { ObjPavement4_4_05 }
    object { ObjPavement4_5_01 }
    object { ObjPavement4_5_02 }
    object { ObjPavement4_5_03 }
    object { ObjPavement4_5_04 }
    object { ObjPavement4_5_05 }
    object { ObjPavement4_5_06 }
    object { ObjPavement4_5_07 }
    object { ObjPavement4_5_08 }
    object { ObjPavement4_5_09 }
    object { ObjPavement4_5_10 }
    object { ObjPavement4_5_11 }
    object { ObjPavement4_5_12 }
    object { ObjPavement4_6_01 }
    object { ObjPavement4_6_02 }
    object { ObjPavement4_6_03 }
    object { ObjPavement4_6_04 }
    object { ObjPavement4_6_05 }
    object { ObjPavement4_6_06 }
    object { ObjPavement4_6_07 }
    object { ObjPavement4_6_08 }
    object { ObjPavement4_6_09 }
    object { ObjPavement4_6_10 }
    object { ObjPavement4_6_11 }
    object { ObjPavement4_6_12 }
    object { ObjPavement4_6_13 }
    object { ObjPavement4_6_14 }
    object { ObjPavement4_6_15 }
    object { ObjPavement4_6_16 }
    object { ObjPavement4_6_17 }
    object { ObjPavement4_6_18 }
    object { ObjPavement4_6_19 }
    object { ObjPavement4_6_20 }
    object { ObjPavement4_6_21 }
    object { ObjPavement4_6_22 }
    object { ObjPavement4_6_23 }
    object { ObjPavement4_6_24 }
    object { ObjPavement4_6_25 }
    object { ObjPavement4_6_26 }
    object { ObjPavement4_6_27 }
    object { ObjPavement4_6_28 }
    object { ObjPavement4_6_29 }
    object { ObjPavement4_6_30 }
    object { ObjPavement4_6_31 }
    object { ObjPavement4_6_32 }
    object { ObjPavement4_6_33 }
    object { ObjPavement4_6_34 }
    object { ObjPavement4_6_35 }
    object { ObjPavement6_1_01 }
    object { ObjPavement6_2_01 }
    object { ObjPavement6_3_01 }
    object { ObjPavement6_3_02 }
    object { ObjPavement6_3_03 }
    object { ObjPavement6_4_01 }
    object { ObjPavement6_4_02 }
    object { ObjPavement6_4_03 }
    object { ObjPavement6_4_04 }
    object { ObjPavement6_4_05 }
    object { ObjPavement6_4_06 }
    object { ObjPavement6_4_07 }
    object { ObjPavement6_5_01 }
    object { ObjPavement6_5_02 }
    object { ObjPavement6_5_03 }
    object { ObjPavement6_5_04 }
    object { ObjPavement6_5_05 }
    object { ObjPavement6_5_06 }
    object { ObjPavement6_5_07 }
    object { ObjPavement6_5_08 }
    object { ObjPavement6_5_09 }
    object { ObjPavement6_5_10 }
    object { ObjPavement6_5_11 }
    object { ObjPavement6_5_12 }
    object { ObjPavement6_5_13 }
    object { ObjPavement6_5_14 }
    object { ObjPavement6_5_15 }
    object { ObjPavement6_5_16 }
    object { ObjPavement6_5_17 }
    object { ObjPavement6_5_18 }
    object { ObjPavement6_5_19 }
    object { ObjPavement6_5_20 }
    object { ObjPavement6_5_21 }
    object { ObjPavement6_5_22 }
    translate <-7.70,0,-14.30>
    scale 0.06
}

//---
camera { Camera01y }
light_source { Light00 }
object { UnionObjs }

