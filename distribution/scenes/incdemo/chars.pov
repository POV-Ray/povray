// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Raytracer sample file.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings { assumed_gamma 1.0 }

#include "colors.inc"
#include "chars.inc"

// Camera definition
camera {
  location <0, 0, -500>
  right     x*image_width/image_height
  angle 6 
  look_at <0,5,0>
}

plane {z,3 pigment{checker color Yellow color White} hollow on}

light_source { <200, 200, -500> color White }

#default {pigment{Cyan}}

#declare ColWide=5;
#declare Col1=-22;
#declare Col2=Col1+ColWide;
#declare Col3=Col2+ColWide;
#declare Col4=Col3+ColWide;
#declare Col5=Col4+ColWide;
#declare Col6=Col5+ColWide;
#declare Col7=Col6+ColWide;
#declare Col8=Col7+ColWide;
#declare Col9=Col8+ColWide;
#declare Col10=Col9+ColWide;

#declare Row1=18;
#declare Row2=11;
#declare Row3= 4;
#declare Row4=-3;
#declare Row5=-10;
#declare Row6=-17;

object {char_A translate <Col1,Row1,0>}
object {char_B translate <Col2,Row1,0>}
object {char_C translate <Col3,Row1,0>}
object {char_D translate <Col4,Row1,0>}
object {char_E translate <Col5,Row1,0>}
object {char_F translate <Col6,Row1,0>}
object {char_G translate <Col7,Row1,0>}
object {char_H translate <Col8,Row1,0>}
object {char_I translate <Col9,Row1,0>}
object {char_J translate <Col10,Row1,0>}

object {char_K translate <Col1,Row2,0>}
object {char_L translate <Col2,Row2,0>}
object {char_M translate <Col3,Row2,0>}
object {char_N translate <Col4,Row2,0>}
object {char_O translate <Col5,Row2,0>}
object {char_P translate <Col6,Row2,0>}
object {char_Q translate <Col7,Row2,0>}
object {char_R translate <Col8,Row2,0>}
object {char_S translate <Col9,Row2,0>}
object {char_T translate <Col10,Row2,0>}

object {char_U translate <Col1,Row3,0>}
object {char_V translate <Col2,Row3,0>}
object {char_W translate <Col3,Row3,0>}
object {char_X translate <Col4,Row3,0>}
object {char_Y translate <Col5,Row3,0>}
object {char_Z translate <Col6,Row3,0>}
object {char_0 translate <Col7,Row3,0>}
object {char_1 translate <Col8,Row3,0>}
object {char_2 translate <Col9,Row3,0>}
object {char_3 translate <Col10,Row3,0>}

object {char_4 translate <Col1,Row4,0>}
object {char_5 translate <Col2,Row4,0>}
object {char_6 translate <Col3,Row4,0>}
object {char_7 translate <Col4,Row4,0>}
object {char_8 translate <Col5,Row4,0>}
object {char_9 translate <Col6,Row4,0>}
object {char_Dash translate <Col7,Row4,0>}
object {char_Plus translate <Col8,Row4,0>}
object {char_ExclPt translate <Col9,Row4,0>}
object {char_AtSign translate <Col10,Row4,0>}

object {char_Num  translate <Col1,Row5,0>}
object {char_Dol  translate <Col2,Row5,0>}
object {char_Perc translate <Col3,Row5,0>}
object {char_Hat  translate <Col4,Row5,0>}
object {char_Amps translate <Col5,Row5,0>}
object {char_Astr translate <Col6,Row5,0>}
object {char_LPar translate <Col7,Row5,0>}
object {char_RPar translate <Col8,Row5,0>}
object {char_LSqu translate <Col9,Row5,0>}
object {char_RSqu translate <Col10,Row5,0>}
