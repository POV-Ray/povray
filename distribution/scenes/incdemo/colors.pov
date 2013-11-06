// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// By Chris Young & Dan Farmer
// Illustrates many of the colors in standard include file "colors.inc"
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings { assumed_gamma 1.0 }

#include "colors.inc"

#declare Col0 = -36;
#declare Col1 = -30;
#declare Col2 = -24;
#declare Col3 = -18;
#declare Col4 = -12;
#declare Col5 = -6;
#declare Col6 = 0;
#declare Col7 = 6;
#declare Col8 = 12;
#declare Col9 = 18;
#declare Col10 = 24;
#declare Col11 = 30;
#declare Col12 = 36;

#declare Row1 = 24;
#declare Row2 = 18;
#declare Row3 = 12;
#declare Row4 = 6;
#declare Row5 = 0;
#declare Row6 = -6;
#declare Row7 = -12;
#declare Row8 = -18;
#declare Row9 = -24;
#declare Row10 = -30;


camera {
   location <Col6,  -3,  -350>
   right     x*image_width/image_height
   angle 14 
   look_at <Col6, -3, 0>
}

light_source {<-500, 500, -1500> color White}


default {
   finish {
      phong 1 phong_size 100
      ambient .15
      diffuse .7
   }
}

#declare Dist = -4;
#declare Radius = 3;

plane {
  z,3.1
  pigment {White}
  hollow  // Works the same as "hollow on"
}

sphere {
  <Col0, Row1, Dist>, Radius
  pigment {Gray05}
}

sphere {
  <Col1, Row1, Dist>, Radius
   pigment {Gray10}
}

sphere {
  <Col2, Row1, Dist>, Radius
   pigment {Gray15}
}

sphere {
  <Col3, Row1, Dist>, Radius
   pigment {Gray20}
}

sphere {
  <Col4, Row1, Dist>, Radius
   pigment {Gray25}
}

sphere {
  <Col5, Row1, Dist>, Radius
   pigment {Gray30}
}

sphere {
  <Col6, Row1, Dist>, Radius
   pigment {Gray35}
}

sphere {
  <Col7, Row1, Dist>, Radius
   pigment {Gray40}
}

sphere {
  <Col8, Row1, Dist>, Radius
   pigment {Gray45}
}

sphere {
  <Col9, Row1, Dist>, Radius
   pigment {Gray50}
}

sphere {
  <Col10, Row1, Dist>, Radius
   pigment {Gray55}
}

sphere {
  <Col11, Row1, Dist>, Radius
   pigment {Gray60}
}

sphere {
  <Col12, Row1, Dist>, Radius
   pigment {Gray65}
}

sphere {
  <Col0, Row2, Dist>, Radius
   pigment {Gray70}
}

sphere {
  <Col1, Row2, Dist>, Radius
   pigment {Gray75}
}

sphere {
  <Col2, Row2, Dist>, Radius
   pigment {Gray80}
}

sphere {
  <Col3, Row2, Dist>, Radius
   pigment {Gray85}
}

sphere {
  <Col4, Row2, Dist>, Radius
   pigment {Gray90}
}

sphere {
  <Col5, Row2, Dist>, Radius
   pigment {Gray95}
}

sphere {
  <Col6, Row2, Dist>, Radius
   pigment {DimGray}
}

sphere {
  <Col7, Row2, Dist>, Radius
   pigment {Grey}
}

sphere {
  <Col8, Row2, Dist>, Radius
   pigment {LightGrey}
}

sphere {
  <Col9, Row2, Dist>, Radius
   pigment {VLightGrey}
}

sphere {
  <Col10, Row2, Dist>, Radius
   pigment {Clear}
}

sphere {
  <Col11, Row2, Dist>, Radius
   pigment {White}
}

sphere {
  <Col12, Row2, Dist>, Radius
   pigment {Red}
}

sphere {
  <Col0, Row3, Dist>, Radius
   pigment {Green}
}

sphere {
  <Col1, Row3, Dist>, Radius
   pigment {Blue}
}

sphere {
  <Col2, Row3, Dist>, Radius
   pigment {Yellow}
}

sphere {
  <Col3, Row3, Dist>, Radius
   pigment {Cyan}
}

sphere {
  <Col4, Row3, Dist>, Radius
   pigment {Magenta}
}

sphere {
  <Col5, Row3, Dist>, Radius
   pigment {Black}
}

sphere {
  <Col6, Row3, Dist>, Radius
   pigment {Aquamarine}
}

sphere {
  <Col7, Row3, Dist>, Radius
   pigment {BlueViolet}
}

sphere {
  <Col8, Row3, Dist>, Radius
   pigment {Brown}
}

sphere {
  <Col9, Row3, Dist>, Radius
   pigment {CadetBlue}
}

sphere {
  <Col10, Row3, Dist>, Radius
   pigment {Coral}
}

sphere {
  <Col11, Row3, Dist>, Radius
   pigment {CornflowerBlue}
}

sphere {
  <Col12, Row3, Dist>, Radius
   pigment {DarkGreen}
}

sphere {
  <Col0, Row4, Dist>, Radius
   pigment {DarkOliveGreen}
}

sphere {
  <Col1, Row4, Dist>, Radius
   pigment {DarkOrchid}
}

sphere {
  <Col2, Row4, Dist>, Radius
   pigment {DarkSlateBlue}
}

sphere {
  <Col3, Row4, Dist>, Radius
   pigment {DarkSlateGrey}
}

sphere {
  <Col4, Row4, Dist>, Radius
   pigment {DarkTurquoise}
}

sphere {
  <Col5, Row4, Dist>, Radius
   pigment {Firebrick}
}

sphere {
  <Col6, Row4, Dist>, Radius
   pigment {ForestGreen}
}

sphere {
  <Col7, Row4, Dist>, Radius
   pigment {Gold}
}

sphere {
  <Col8, Row4, Dist>, Radius
   pigment {Goldenrod}
}

sphere {
  <Col9, Row4, Dist>, Radius
   pigment {GreenYellow}
}

sphere {
  <Col10, Row4, Dist>, Radius
   pigment {IndianRed}
}

sphere {
  <Col11, Row4, Dist>, Radius
   pigment {Khaki}
}

sphere {
  <Col12, Row4, Dist>, Radius
   pigment {LightBlue}
}

sphere {
  <Col0, Row5, Dist>, Radius
   pigment {LightSteelBlue}
}

sphere {
  <Col1, Row5, Dist>, Radius
   pigment {LimeGreen}
}

sphere {
  <Col2, Row5, Dist>, Radius
   pigment {Maroon}
}

sphere {
  <Col3, Row5, Dist>, Radius
   pigment {MediumAquamarine}
}

sphere {
  <Col4, Row5, Dist>, Radius
   pigment {MediumBlue}
}

sphere {
  <Col5, Row5, Dist>, Radius
   pigment {MediumForestGreen}
}

sphere {
  <Col6, Row5, Dist>, Radius
   pigment {MediumGoldenrod}
}

sphere {
  <Col7, Row5, Dist>, Radius
   pigment {MediumOrchid}
}

sphere {
  <Col8, Row5, Dist>, Radius
   pigment {MediumSeaGreen}
}

sphere {
  <Col9, Row5, Dist>, Radius
   pigment {MediumSlateBlue}
}

sphere {
  <Col10, Row5, Dist>, Radius
   pigment {MediumSpringGreen}
}

sphere {
  <Col11, Row5, Dist>, Radius
   pigment {MediumTurquoise}
}

sphere {
  <Col12, Row5, Dist>, Radius
   pigment {MediumVioletRed}
}

sphere {
  <Col0, Row6, Dist>, Radius
   pigment {MidnightBlue}
}

sphere {
  <Col1, Row6, Dist>, Radius
   pigment {Navy}
}

sphere {
  <Col2, Row6, Dist>, Radius
   pigment {NavyBlue}
}

sphere {
  <Col3, Row6, Dist>, Radius
   pigment {Orange}
}

sphere {
  <Col4, Row6, Dist>, Radius
   pigment {OrangeRed}
}

sphere {
  <Col5, Row6, Dist>, Radius
   pigment {Orchid}
}

sphere {
  <Col6, Row6, Dist>, Radius
   pigment {PaleGreen}
}

sphere {
  <Col7, Row6, Dist>, Radius
   pigment {Pink}
}

sphere {
  <Col8, Row6, Dist>, Radius
   pigment {Plum}
}

sphere {
  <Col9, Row6, Dist>, Radius
   pigment {Salmon}
}

sphere {
  <Col10, Row6, Dist>, Radius
   pigment {SeaGreen}
}

sphere {
  <Col11, Row6, Dist>, Radius
   pigment {Sienna}
}

sphere {
  <Col12, Row6, Dist>, Radius
   pigment {SkyBlue}
}

sphere {
  <Col0, Row7, Dist>, Radius
   pigment {SlateBlue}
}

sphere {
  <Col1, Row7, Dist>, Radius
   pigment {SpringGreen}
}

sphere {
  <Col2, Row7, Dist>, Radius
   pigment {SteelBlue}
}

sphere {
  <Col3, Row7, Dist>, Radius
   pigment {Tan}
}

sphere {
  <Col4, Row7, Dist>, Radius
   pigment {Thistle}
}

sphere {
  <Col5, Row7, Dist>, Radius
   pigment {Turquoise}
}

sphere {
  <Col6, Row7, Dist>, Radius
   pigment {Violet}
}

sphere {
  <Col7, Row7, Dist>, Radius
   pigment {VioletRed}
}

sphere {
  <Col8, Row7, Dist>, Radius
   pigment {Wheat}
}

sphere {
  <Col9, Row7, Dist>, Radius
   pigment {YellowGreen}
}

sphere {
  <Col10, Row7, Dist>, Radius
   pigment {SummerSky}
}

sphere {
  <Col11, Row7, Dist>, Radius
   pigment {RichBlue}
}

sphere {
  <Col12, Row7, Dist>, Radius
   pigment {Brass}
}

sphere {
  <Col0, Row8, Dist>, Radius
   pigment {Copper}
}

sphere {
  <Col1, Row8, Dist>, Radius
   pigment {Bronze}
}

sphere {
  <Col2, Row8, Dist>, Radius
   pigment {Bronze2}
}

sphere {
  <Col3, Row8, Dist>, Radius
   pigment {Silver}
}

sphere {
  <Col4, Row8, Dist>, Radius
   pigment {BrightGold}
}

sphere {
  <Col5, Row8, Dist>, Radius
   pigment {OldGold}
}

sphere {
  <Col6, Row8, Dist>, Radius
   pigment {Feldspar}
}

sphere {
  <Col7, Row8, Dist>, Radius
   pigment {Quartz}
}

sphere {
  <Col8, Row8, Dist>, Radius
   pigment {Mica}
}

sphere {
  <Col9, Row8, Dist>, Radius
   pigment {NeonPink}
}

sphere {
  <Col10, Row8, Dist>, Radius
   pigment {DarkPurple}
}

sphere {
  <Col11, Row8, Dist>, Radius
   pigment {NeonBlue}
}

sphere {
  <Col12, Row8, Dist>, Radius
   pigment {CoolCopper}
}

sphere {
  <Col0, Row9, Dist>, Radius
   pigment {MandarinOrange}
}

sphere {
  <Col1, Row9, Dist>, Radius
   pigment {LightWood}
}

sphere {
  <Col2, Row9, Dist>, Radius
   pigment {MediumWood}
}

sphere {
  <Col3, Row9, Dist>, Radius
    pigment {DarkWood}
}

sphere {
  <Col4, Row9, Dist>, Radius
    pigment {SpicyPink}
}

sphere {
  <Col5, Row9, Dist>, Radius
    pigment {SemiSweetChoc}
}

sphere {
  <Col6, Row9, Dist>, Radius
    pigment {BakersChoc}
}

sphere {
  <Col7, Row9, Dist>, Radius
    pigment {Flesh}
}

sphere {
  <Col8, Row9, Dist>, Radius
    pigment {NewTan}
}

sphere {
  <Col9, Row9, Dist>, Radius
    pigment {NewMidnightBlue}
}

sphere {
  <Col10, Row9, Dist>, Radius
    pigment {VeryDarkBrown}
}

sphere {
  <Col11, Row9, Dist>, Radius
    pigment {DarkBrown}
}

sphere {
  <Col12, Row9, Dist>, Radius
    pigment {DarkTan}
}

sphere {
  <Col0, Row10, Dist>, Radius
   pigment {GreenCopper}
}

sphere {
  <Col1, Row10, Dist>, Radius
   pigment {DkGreenCopper}
}

sphere {
  <Col2, Row10, Dist>, Radius
   pigment {DustyRose}
}

sphere {
  <Col3, Row10, Dist>, Radius
    pigment {HuntersGreen}
}

sphere {
  <Col4, Row10, Dist>, Radius
    pigment {Scarlet}
}
/*
sphere {
  <Col5, Row10, Dist>, Radius
    pigment { }
}

sphere {
  <Col6, Row10, Dist>, Radius
    pigment { }
}

sphere {
  <Col7, Row10, Dist>, Radius
    pigment { }
}

sphere {
  <Col8, Row10, Dist>, Radius
    pigment { }
}

sphere {
  <Col10, Row10, Dist>, Radius
    pigment { }
}

sphere {
  <Col10, Row10, Dist>, Radius
    pigment { }
}

sphere {
  <Col11, Row10, Dist>, Radius
    pigment { }
}

sphere {
  <Col12, Row10, Dist>, Radius
    pigment { }
}

*/
