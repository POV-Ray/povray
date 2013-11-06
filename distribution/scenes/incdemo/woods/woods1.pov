// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Raytracer sample file.
// Woods.inc demonstration on cubes.  Faster than woods2.pov but
// doesn't give as good an idea of how the textures look on other
// shapes.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.6;
global_settings {assumed_gamma 2.2}

// Set default finish for wood textures before including the file
#default { finish { specular 0.15 roughness 0.025 ambient 0.25 } }

#include "colors.inc"
#include "woods.inc"

camera {
   location <0, 30, -47>
   direction <0, 0,  3.0>
   right x*1.33
   look_at 3*y
}

light_source {<-50, 50, -1000> color Gray75}
light_source {< 15, 30, -10> color White}

background { color Gray30 }

#declare Thing =
box {-1, 1 scale <1, 2, 1> translate y * 2
   no_shadow
}

#declare T01 = texture { T_Wood1  rotate x*90 }
#declare T02 = texture { T_Wood2  rotate x*90 }
#declare T03 = texture { T_Wood3  rotate x*90 }
#declare T04 = texture { T_Wood4  rotate x*90 }
#declare T05 = texture { T_Wood5  rotate x*90 }

#declare T06 = texture { T_Wood6  rotate x*90 }
#declare T07 = texture { T_Wood7  rotate x*90 }
#declare T08 = texture { T_Wood8  rotate x*90 }
#declare T09 = texture { T_Wood9  rotate x*90 }
#declare T10 = texture { T_Wood10 rotate x*90 }

#declare T11 = texture { T_Wood11  rotate x*90 }
#declare T12 = texture { T_Wood12  rotate x*90 }
#declare T13 = texture { T_Wood13  rotate x*90 }
#declare T14 = texture { T_Wood14 rotate x*90 }
#declare T15 = texture { T_Wood15 rotate x*90 }

#declare T16 = texture { T_Wood16 rotate x*90 }
#declare T17 = texture { T_Wood17 rotate x*90 }
#declare T18 = texture { T_Wood18 rotate x*90 }
#declare T19 = texture { T_Wood19 rotate x*90 }
#declare T20 = texture { T_Wood20 rotate x*90 }

#declare T21 = texture { T_Wood21 rotate x*90 }
#declare T22 = texture { T_Wood22 rotate x*90 }
#declare T23 = texture { T_Wood23 rotate x*90 }
#declare T24 = texture { T_Wood24 rotate x*90 }
#declare T25 = texture { T_Wood25 rotate x*90 }

#declare T26 = texture { T_Wood26 rotate x*90 }
#declare T27 = texture { T_Wood27 rotate x*90 }
#declare T28 = texture { T_Wood28 rotate x*90 }
#declare T29 = texture { T_Wood29 rotate x*90 }
#declare T30 = texture { T_Wood30 rotate x*90 }

#declare T31 = texture { T_Wood31 rotate x*90 }
#declare T32 = texture { T_Wood32 rotate x*90 }
#declare T33 = texture { T_Wood33 rotate x*90 }
#declare T34 = texture { T_Wood34 rotate x*90 }
#declare T35 = texture { T_Wood35 rotate x*90 }



#declare Height = 1;

#declare Dist1 =  -8;
#declare Dist2 =  -4;
#declare Dist3 =   0;
#declare Dist4 =   4;
#declare Dist5 =   8;
#declare Dist6 =  12;
#declare Dist7 =  16;

#declare Col1 =  -8;
#declare Col2 =  -4;
#declare Col3 =   0;
#declare Col4 =   4;
#declare Col5 =   8;

// 1st row, left to right
union {
object { Thing texture{T01} translate <Col1 Height Dist1 >}
object { Thing texture{T02} translate <Col2 Height Dist1 >}
object { Thing texture{T03} translate <Col3 Height Dist1 >}
object { Thing texture{T04} translate <Col4 Height Dist1 >}
object { Thing texture{T05} translate <Col5 Height Dist1 >}
translate -x
}

// 2nd row, left to right
union {
object { Thing texture{T06} translate <Col1 Height Dist2 >}
object { Thing texture{T07} translate <Col2 Height Dist2 >}
object { Thing texture{T08} translate <Col3 Height Dist2 >}
object { Thing texture{T09} translate <Col4 Height Dist2 >}
object { Thing texture{T10} translate <Col5 Height Dist2 >}
translate  x
}

// 3rd row, left to right
union {
object { Thing texture{T11} translate <Col1 Height Dist3 >}
object { Thing texture{T12} translate <Col2 Height Dist3 >}
object { Thing texture{T13} translate <Col3 Height Dist3 >}
object { Thing texture{T14} translate <Col4 Height Dist3 >}
object { Thing texture{T15} translate <Col5 Height Dist3 >}
translate -x
}

// 4th row, left to right
union {
object { Thing texture{T16} translate <Col1 Height Dist4 >}
object { Thing texture{T17} translate <Col2 Height Dist4 >}
object { Thing texture{T18} translate <Col3 Height Dist4 >}
object { Thing texture{T19} translate <Col4 Height Dist4 >}
object { Thing texture{T20} translate <Col5 Height Dist4 >}
translate  x
}

// 5th row, left to right
union {
object { Thing texture{T21} translate <Col1 Height Dist5 >}
object { Thing texture{T22} translate <Col2 Height Dist5 >}
object { Thing texture{T23} translate <Col3 Height Dist5 >}
object { Thing texture{T24} translate <Col4 Height Dist5 >}
object { Thing texture{T25} translate <Col5 Height Dist5 >}
translate -x
}

// 6th row, left to right
union {
object { Thing texture{T26} translate <Col1 Height Dist6 >}
object { Thing texture{T27} translate <Col2 Height Dist6 >}
object { Thing texture{T28} translate <Col3 Height Dist6 >}
object { Thing texture{T29} translate <Col4 Height Dist6 >}
object { Thing texture{T30} translate <Col5 Height Dist6 >}
translate x
}

// 7th row, left to right
union {
object { Thing texture{T31} translate <Col1 Height Dist7 >}
object { Thing texture{T32} translate <Col2 Height Dist7 >}
object { Thing texture{T33} translate <Col3 Height Dist7 >}
object { Thing texture{T34} translate <Col4 Height Dist7 >}
object { Thing texture{T35} translate <Col5 Height Dist7 >}
translate -x
}


