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
#declare Col6 = 0;

#declare Row1 = 24;

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
   }}
   
#declare Dist = -4;
#declare Radius = 3;

// create centered text with a border
#macro centerText(Text)
	#local tx = text { ttf "cyrvetic.ttf", Text, 0.02, 0.0}
	#local minTx = min_extent(tx);
	#local maxTx = max_extent(tx)-minTx;
	#if (maxTx.x>Radius*2*.9)
		#local tx=object{tx scale (Radius*2*.9 / maxTx.x)}
		#local minTx = min_extent(tx);
		#local maxTx = max_extent(tx)-minTx;
	#end
	#local centerTx=object{tx translate<minTx.x-(maxTx.x/2),-maxTx.y/2,-Radius-.1>}
	union{
		object{centerTx translate -z*.1 pigment{rgb 1}}
		#local def=6; // definition of the border, higher value=best definition
		#for (nn,0,def-1)
			object{centerTx translate vrotate(<.1,0,0>,<0,0,360/def*nn>) pigment{rgb 0}}
		#end
	}
#end

plane {
  z,Radius + .1
  pigment {White}
  hollow  // Works the same as "hollow on"
}

#declare allColors = array[10][13]{
	{Gray05, Gray10, Gray15, Gray20, Gray25, Gray30, Gray35, Gray40, Gray45, Gray50, Gray55, Gray60, Gray65},
	{Gray70, Gray75, Gray80, Gray85, Gray90, Gray95, DimGray, Grey, LightGrey, VLightGrey, Clear, White, Red},
	{Green, Blue, Yellow, Cyan, Magenta, Black, Aquamarine, BlueViolet, Brown, CadetBlue, Coral, CornflowerBlue, DarkGreen},
	{DarkOliveGreen, DarkOrchid, DarkSlateBlue, DarkSlateGrey, DarkTurquoise, Firebrick, ForestGreen, Gold, Goldenrod, GreenYellow, IndianRed, Khaki, LightBlue},
	{LightSteelBlue, LimeGreen, Maroon, MediumAquamarine, MediumBlue, MediumForestGreen, MediumGoldenrod, MediumOrchid, MediumSeaGreen, MediumSlateBlue, MediumSpringGreen, 
		MediumTurquoise, MediumVioletRed,},
	{MidnightBlue,Navy,NavyBlue,Orange,OrangeRed,Orchid,PaleGreen,Pink,Plum,Salmon,SeaGreen,Sienna,SkyBlue},
	{SlateBlue, SpringGreen, SteelBlue, Tan, Thistle, Turquoise, Violet, VioletRed, Wheat, YellowGreen, SummerSky, RichBlue, Brass},
	{Copper, Bronze, Bronze2, Silver, BrightGold, OldGold, Feldspar, Quartz, Mica, NeonPink, DarkPurple, NeonBlue, CoolCopper}
	{MandarinOrange, LightWood, MediumWood, DarkWood, SpicyPink, SemiSweetChoc, BakersChoc, Flesh, NewTan, NewMidnightBlue, VeryDarkBrown, DarkBrown, DarkTan},
	{GreenCopper, DkGreenCopper, DustyRose, HuntersGreen, Scarlet, Med_Purple, Light_Purple, Very_Light_Purple, Clear, Clear, Clear, Clear, Clear}
}

#declare allColorsStr = array[10][13]{
	{"Gray05", "Gray10", "Gray15", "Gray20", "Gray25", "Gray30", "Gray35", "Gray40", "Gray45", "Gray50", "Gray55", "Gray60", "Gray65"},
	{"Gray70", "Gray75", "Gray80", "Gray85", "Gray90", "Gray95", "DimGray", "Grey", "LightGrey", "VLightGrey", "Clear", "White", "Red"},
	{"Green", "Blue", "Yellow", "Cyan", "Magenta", "Black", "Aquamarine", "BlueViolet", "Brown", "CadetBlue", "Coral", "CornflowerBlue", "DarkGreen"},
	{"DarkOliveGreen", "DarkOrchid", "DarkSlateBlue", "DarkSlateGrey", "DarkTurquoise", "Firebrick", "ForestGreen", "Gold", "Goldenrod", "GreenYellow", "IndianRed", "Khaki", "LightBlue"}
	{"LightSteelBlue","LimeGreen","Maroon","MediumAquamarine","MediumBlue","MediumForestGreen","MediumGoldenrod","MediumOrchid","MediumSeaGreen","MediumSlateBlue","MediumSpringGreen","MediumTurquoise","MediumVioletRed"}
	{"MidnightBlue", "Navy", "NavyBlue", "Orange", "OrangeRed", "Orchid", "PaleGreen", "Pink", "Plum", "Salmon", "SeaGreen", "Sienna", "SkyBlue"},
	{"SlateBlue", "SpringGreen", "SteelBlue", "Tan", "Thistle", "Turquoise", "Violet", "VioletRed", "Wheat", "YellowGreen", "SummerSky", "RichBlue", "Brass"},
	{"Copper", "Bronze", "Bronze2", "Silver", "BrightGold", "OldGold", "Feldspar", "Quartz", "Mica", "NeonPink", "DarkPurple", "NeonBlue", "CoolCopper"},
	{"MandarinOrange", "LightWood", "MediumWood", "DarkWood", "SpicyPink", "SemiSweetChoc", "BakersChoc", "Flesh", "NewTan", "NewMidnightBlue", "VeryDarkBrown", "DarkBrown", "DarkTan"},
	{"GreenCopper", "DkGreenCopper", "DustyRose", "HuntersGreen", "Scarlet", "Med_Purple", "Light_Purple", "Very_Light_Purple", "", "", "", "", ""},
}

#for (ny, 0, 9)
	#for (nx, 0, 12)
		#if (allColorsStr[ny][nx]!="")
		union{
			sphere {
				0, Radius
				pigment {allColors[ny][nx]}
			}
			centerText(allColorsStr[ny][nx])
			translate<Col0 + 6*nx, Row1-ny*6, Dist>
		}
		#end
	#end
#end

/*
sphere {<Col8, Row10, Dist>, Radius pigment{ }}

sphere {<Col10, Row10, Dist>, Radius pigment{ }}

sphere {<Col10, Row10, Dist>, Radius pigment{ }}

sphere {<Col11, Row10, Dist>, Radius pigment{ }}

sphere {<Col12, Row10, Dist>, Radius pigment{ }}

*/
