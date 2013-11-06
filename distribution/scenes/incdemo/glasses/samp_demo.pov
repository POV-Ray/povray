// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Raytracer Scene Description File
// File: samp_demo.pov
// Desc: demonstrating use of 'sample.inc'
// Date: July/August 2001
// Auth: Christoph Hormann

// -w320 -h240 +sf1 +ef6 +kff10

#version 3.6;
global_settings { assumed_gamma 1.0 }

#if (!clock_on)
  #warning concat("This scene should be rendered as an animation\n",
                  "use '+sf1 +ef6' for rendering all versions.\n")
#end

global_settings {
  max_trace_level 5
}

#include "colors.inc"
#include "glass.inc"

#declare MaxX=12;
#declare MaxZ=9;
#declare SpaceX=2;
#declare SpaceZ=2;

#declare MaxCnt=111;
#declare MArr = array[MaxCnt]

#if (frame_number=1)
  #declare Scale=1.0;
  #declare Type=1;
  #declare Obj_Nbr=2;
#end

#if (frame_number=2)
  #declare SpaceX=2.3;
  #declare SpaceZ=2.3;
  #declare Scale=1.0;
  #declare Type=3;
  #declare Obj_Nbr=1;
#end

#if (frame_number=3)
  #declare SpaceX=2.3;
  #declare SpaceZ=2.3;
  #declare Scale=1.0;
  #declare Type=5;
  #declare Obj_Nbr=4;
#end

#if (frame_number=4)
  #declare SpaceX=2.3;
  #declare SpaceZ=2.3;
  #declare Scale=1.0;
  #declare Type=7;
  #declare Obj_Nbr=0;
#end

#if (frame_number=5)
  #declare SpaceX=2.3;
  #declare SpaceZ=2.3;
  #declare Scale=1.0;
  #declare Type=9;
  #declare Obj_Nbr=0;
#end

#if (frame_number=6)
  #declare Scale=1.0;
  #declare Type=11;
  #declare Obj_Nbr=4;
#end

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
#while (Cnt < MaxCnt)

  #declare MArr[Cnt]  = material { texture { finish { specular 0.4 } pigment {color colorArr[Cnt]} } }
  #declare Cnt=Cnt+1;
#end


#include "sample.inc"

