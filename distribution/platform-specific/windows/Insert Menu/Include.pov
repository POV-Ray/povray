// Insert menu illustration scene
// Created June-August 2001 by Christoph Hormann
// Updated to 3.7 by Friedrich A. Lohmueller, June-2012.

// ----- include files submenu -----

// -w72 -h72 +a0.1 +am2 -j +r3

#version 3.7;
global_settings{ assumed_gamma 1.0 } 

#include "colors.inc"
#include "finish.inc"
#include "glass.inc"
#include "metals.inc"
#include "stones1.inc"
#include "stones2.inc"
#include "woods.inc"

 
//#declare Typ=1;     // colors
//#declare Typ=2;     // finish
//#declare Typ=3;     // glass
//#declare Typ=4;     // metals
//#declare Typ=5;     // stones
//#declare Typ=6;     // woods


global_settings {
 // assumed_gamma 1
  max_trace_level 5
}

light_source {
  <1.5, 2.5, 2.5>*10000
  color rgb 1.0
}

camera {
  orthographic
  location <0,0,1>
  look_at  <0,0,0>
  
  #if (Typ=3)   // glass
    right 2*x
    up 2*y
  #else
    right 1*x
    up 1*y
  #end
 
}

sky_sphere {
  pigment {
    gradient z
    color_map {
      [0.0 rgb <0.6,0.7,0.9>]
      [1.0 rgb <0.5,0.5,0.7>]
    }
  }
}

// ----------------------------------------

plane
{
  z, -2
  texture
  {
    pigment { color rgb 1 }
    finish {
      diffuse 0.8
      specular 0.4
      roughness 0.01

      reflection { 0.5 , 1.0
        fresnel on
        metallic 0.8
      }
      conserve_energy

    }
  }
}


// =============================================

#if (Typ=1)

#declare colorArr=array[130] {
   Red, Green, Blue, Yellow, Cyan,
   Magenta, Clear, White, Black, Gray05,
   Gray10, Gray15, Gray20, Gray25, Gray30,
   Gray35, Gray40, Gray45, Gray50, Gray55,
   Gray60, Gray65, Gray70, Gray75, Gray80,
   Gray85, Gray90, Gray95, DimGray, DimGrey,
   Gray, Grey, LightGray, LightGrey, VLightGray,
   VLightGrey, Aquamarine, BlueViolet, Brown, CadetBlue,
   Coral, CornflowerBlue, DarkGreen, DarkOliveGreen, DarkOrchid,
   DarkSlateBlue, DarkSlateGray, DarkSlateGrey, DarkTurquoise, Firebrick,
   ForestGreen, Gold, Goldenrod, GreenYellow, IndianRed,
   Khaki, LightBlue, LightSteelBlue, LimeGreen, Maroon,
   MediumAquamarine, MediumBlue, MediumForestGreen, MediumGoldenrod, MediumOrchid,
   MediumSeaGreen, MediumSlateBlue, MediumSpringGreen, MediumTurquoise, MediumVioletRed,
   MidnightBlue, Navy, NavyBlue, Orange, OrangeRed,
   Orchid, PaleGreen, Pink, Plum, Salmon,
   SeaGreen, Sienna, SkyBlue, SlateBlue, SpringGreen,
   SteelBlue, Tan, Thistle, Turquoise, Violet,
   VioletRed, Wheat, YellowGreen, SummerSky, RichBlue,
   Brass, Copper, Bronze, Bronze2, Silver,
   BrightGold, OldGold, Feldspar, Quartz, Mica,
   NeonPink, DarkPurple, NeonBlue, CoolCopper, MandarinOrange,
   LightWood, MediumWood, DarkWood, SpicyPink, SemiSweetChoc,
   BakersChoc, Flesh, NewTan, NewMidnightBlue, VeryDarkBrown,
   DarkBrown, DarkTan, GreenCopper, DkGreenCopper, DustyRose,
   HuntersGreen, Scarlet, Med_Purple, Light_Purple, Very_Light_Purple,
}

#declare Cnt=0;

#declare YCnt=0;

#while (YCnt < 1)

  #declare XCnt=0;

  #while (XCnt < 1)

    #declare Tex1=
    texture {
      pigment {
        color colorArr[Cnt]
      }
      finish { diffuse 0 ambient 1 }
    }

    #declare Cnt=Cnt+1;

    object {
      box {<-1,-1,-1>, <1,1,1> }
      scale 0.042
      texture { Tex1 }
      no_shadow
      translate <-0.46+XCnt, -0.46+YCnt, 0>
    }
    #declare XCnt=XCnt+(1/11);
  #end
  #declare YCnt=YCnt+(1/11);
#end

#end

// =============================================

#if (Typ=2)

#declare finishArr=array[8] {
 Dull, Shiny, Phong_Dull, Phong_Shiny, Glossy,
 Phong_Glossy, Luminous, Mirror,
}

#declare Cnt=0;

#declare YCnt=0;

#while (YCnt < 1)

  #declare XCnt=0;

  #while (XCnt < 1)

    #if (Cnt < 8)
      #declare Tex1=
      texture {
        pigment {
          color rgb 0.8
        }
        finish { finishArr[Cnt] }
      }

      #declare Cnt=Cnt+1;

      object {
        sphere {0, 1 }
        scale 0.18
        texture { Tex1 }
        translate <-0.35+XCnt, -0.35+YCnt, 0>
      }
    #end

    #declare XCnt=XCnt+(1/3);
  #end
  #declare YCnt=YCnt+(1/3);
#end

#end

// =============================================

#if (Typ=3)

#declare MaxCnt = 25;
#declare MArr = array[MaxCnt]


#declare T_01 = texture { pigment { color Clear } finish { F_Glass6 } }
#declare T_02 = texture { pigment { color Clear } finish { F_Glass7 } }
#declare T_03 = texture { pigment { color Clear } finish { F_Glass8 } }
#declare T_04 = texture { pigment { color Clear } finish { F_Glass9 } }
#declare T_05 = texture { pigment { color Clear } finish { F_Glass10 } }


#declare MArr[0]  = material { texture { T_01 } interior { I_Glass fade_color Col_Blue_01 } }
#declare MArr[1]  = material { texture { T_02 } interior { I_Glass fade_color Col_Green_01 } }
#declare MArr[2]  = material { texture { T_03 } interior { I_Glass fade_color Col_Red_01 } }
#declare MArr[3]  = material { texture { T_04 } interior { I_Glass fade_color Col_Yellow_01 } }
#declare MArr[4]  = material { texture { T_05 } interior { I_Glass fade_color Col_Amethyst_06 } }

#declare MArr[5]  = material { texture { T_01 } interior { I_Glass1 fade_color Col_Blue_01 } }
#declare MArr[6]  = material { texture { T_02 } interior { I_Glass1 fade_color Col_Green_01 } }
#declare MArr[7]  = material { texture { T_03 } interior { I_Glass1 fade_color Col_Red_01 } }
#declare MArr[8]  = material { texture { T_04 } interior { I_Glass1 fade_color Col_Yellow_01 } }
#declare MArr[9]  = material { texture { T_05 } interior { I_Glass1 fade_color Col_Amethyst_06 } }

#declare MArr[10] = material { texture { T_01 } interior { I_Glass2 fade_color Col_Blue_01 } }
#declare MArr[11] = material { texture { T_02 } interior { I_Glass2 fade_color Col_Green_01 } }
#declare MArr[12] = material { texture { T_03 } interior { I_Glass2 fade_color Col_Red_01 } }
#declare MArr[13] = material { texture { T_04 } interior { I_Glass2 fade_color Col_Yellow_01 } }
#declare MArr[14] = material { texture { T_05 } interior { I_Glass2 fade_color Col_Amethyst_06 } }

#declare MArr[15] = material { texture { T_01 } interior { I_Glass3 fade_color Col_Blue_01 } }
#declare MArr[16] = material { texture { T_02 } interior { I_Glass3 fade_color Col_Green_01 } }
#declare MArr[17] = material { texture { T_03 } interior { I_Glass3 fade_color Col_Red_01 } }
#declare MArr[18] = material { texture { T_04 } interior { I_Glass3 fade_color Col_Yellow_01 } }
#declare MArr[19] = material { texture { T_05 } interior { I_Glass3 fade_color Col_Amethyst_06 } }

#declare MArr[20] = material { texture { T_01 } interior { I_Glass4 fade_color Col_Blue_01 } }
#declare MArr[21] = material { texture { T_02 } interior { I_Glass4 fade_color Col_Blue_01 } }
#declare MArr[22] = material { texture { T_03 } interior { I_Glass4 fade_color Col_Blue_01 } }
#declare MArr[23] = material { texture { T_04 } interior { I_Glass4 fade_color Col_Blue_01 } }
#declare MArr[24] = material { texture { T_05 } interior { I_Glass4 fade_color Col_Blue_01 } }

#declare Cnt=0;

#declare YCnt=0;

#while (YCnt < 2)

  #declare XCnt=0;

  #while (XCnt < 2)

    #if (Cnt < MaxCnt)

      object {
        sphere {0, 1 }
        scale 0.25
        material { MArr[Cnt] }
        translate <-0.75+XCnt, -0.75+YCnt, 0>
      }

      #declare Cnt=Cnt+1;

    #end

    #declare XCnt=XCnt+(2/4);
  #end
  #declare YCnt=YCnt+(2/4);
#end

#end

// =============================================

#if (Typ=4)

#declare textureArr=array[100] {
   T_Brass_1A, T_Brass_1B, T_Brass_1C, T_Brass_1D, T_Brass_1E,
   T_Brass_2A, T_Brass_2B, T_Brass_2C, T_Brass_2D, T_Brass_2E,
   T_Brass_3A, T_Brass_3B, T_Brass_3C, T_Brass_3D, T_Brass_3E,
   T_Brass_4A, T_Brass_4B, T_Brass_4C, T_Brass_4D, T_Brass_4E,
   T_Brass_5A, T_Brass_5B, T_Brass_5C, T_Brass_5D, T_Brass_5E,
   T_Copper_1A, T_Copper_1B, T_Copper_1C, T_Copper_1D, T_Copper_1E,
   T_Copper_2A, T_Copper_2B, T_Copper_2C, T_Copper_2D, T_Copper_2E,
   T_Copper_3A, T_Copper_3B, T_Copper_3C, T_Copper_3D, T_Copper_3E,
   T_Copper_4A, T_Copper_4B, T_Copper_4C, T_Copper_4D, T_Copper_4E,
   T_Copper_5A, T_Copper_5B, T_Copper_5C, T_Copper_5D, T_Copper_5E,
   T_Chrome_1A, T_Chrome_1B, T_Chrome_1C, T_Chrome_1D, T_Chrome_1E,
   T_Chrome_2A, T_Chrome_2B, T_Chrome_2C, T_Chrome_2D, T_Chrome_2E,
   T_Chrome_3A, T_Chrome_3B, T_Chrome_3C, T_Chrome_3D, T_Chrome_3E,
   T_Chrome_4A, T_Chrome_4B, T_Chrome_4C, T_Chrome_4D, T_Chrome_4E,
   T_Chrome_5A, T_Chrome_5B, T_Chrome_5C, T_Chrome_5D, T_Chrome_5E,
   T_Silver_1A, T_Silver_1B, T_Silver_1C, T_Silver_1D, T_Silver_1E,
   T_Silver_2A, T_Silver_2B, T_Silver_2C, T_Silver_2D, T_Silver_2E,
   T_Silver_3A, T_Silver_3B, T_Silver_3C, T_Silver_3D, T_Silver_3E,
   T_Silver_4A, T_Silver_4B, T_Silver_4C, T_Silver_4D, T_Silver_4E,
   T_Silver_5A, T_Silver_5B, T_Silver_5C, T_Silver_5D, T_Silver_5E,
}

#declare Cnt=0;

#declare YCnt=0;

#while (YCnt < 1)

  #declare XCnt=0;

  #while (XCnt < 1)

      #declare Tex1=
      texture {
        textureArr[Cnt*3]
      }

      #declare Cnt=Cnt+1;

      object {
        sphere {0, 1 }
        scale 0.1
        texture { Tex1 }
        translate <-0.4+XCnt, -0.4+YCnt, 0>
      }

    #declare XCnt=XCnt+(1/5);
  #end
  #declare YCnt=YCnt+(1/5);
#end

#end

// =============================================

#if (Typ=5)

#declare textureArr=array[44] {
  T_Stone1,
  T_Stone2,  T_Stone3,  T_Stone4,  T_Stone5,  T_Stone6,
  T_Stone7,  T_Stone8,  T_Stone9,  T_Stone10,  T_Stone11,
  T_Stone12,  T_Stone13,  T_Stone14,  T_Stone15,  T_Stone16,
  T_Stone17,  T_Stone18,  T_Stone19,  T_Stone20,  T_Stone21,
  T_Stone22,  T_Stone23,  T_Stone24,
  T_Stone25,  T_Stone26,  T_Stone27,  T_Stone28,  T_Stone29,
  T_Stone30,  T_Stone31,  T_Stone32,  T_Stone33,  T_Stone34,
  T_Stone35,  T_Stone36,  T_Stone37,  T_Stone38,  T_Stone39,
  T_Stone40,  T_Stone41,  T_Stone42,  T_Stone43,  T_Stone44,
}

#declare Cnt=0;

#declare YCnt=0;

#while (YCnt < 1)

  #declare XCnt=0;

  #while (XCnt < 1)

      #declare Tex1=
      texture {
        textureArr[Cnt]
      }

      #declare Cnt=Cnt+1;

      object {
        sphere {0, 1 }
        scale 0.1
        texture { Tex1 }
        translate <-0.4+XCnt, -0.4+YCnt, 0>
      }

    #declare XCnt=XCnt+(1/5);
  #end
  #declare YCnt=YCnt+(1/5);
#end

#end

// =============================================

#if (Typ=6)

#declare textureArr=array[35] {
   T_Wood1, T_Wood2, T_Wood3, T_Wood4, T_Wood5,
   T_Wood6, T_Wood7, T_Wood8, T_Wood9, T_Wood10,
   T_Wood11, T_Wood12, T_Wood13, T_Wood14, T_Wood15,
   T_Wood16, T_Wood17, T_Wood18, T_Wood19, T_Wood20,
   T_Wood21, T_Wood22, T_Wood23, T_Wood24, T_Wood25,
   T_Wood26, T_Wood27, T_Wood28, T_Wood29, T_Wood30,
   T_Wood31, T_Wood32, T_Wood33, T_Wood34, T_Wood35,
}

#declare Cnt=0;

#declare YCnt=0;

#while (YCnt < 1)

  #declare XCnt=0;

  #while (XCnt < 1)

      #declare Tex1=
      texture {
        textureArr[Cnt]
      }

      #declare Cnt=Cnt+1;

      object {
        sphere {0, 1 }
        scale 0.1
        texture { Tex1 }
        translate <-0.4+XCnt, -0.4+YCnt, 0>
      }

    #declare XCnt=XCnt+(1/5);
  #end
  #declare YCnt=YCnt+(1/5);
#end



#end
