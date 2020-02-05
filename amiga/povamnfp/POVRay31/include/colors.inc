#ifdef(Colors_Inc_Temp)
// do nothing
#else
#declare Colors_Inc_Temp = version;


#ifdef(View_POV_Include_Stack)
#   debug "including colors.inc\n"
#end

/*
              Persistence of Vision Raytracer Version 3.1
            Many pre-defined colors for use in scene files.
*/
// COLORS:
#declare Red     = rgb <1, 0, 0>;
#declare Green   = rgb <0, 1, 0>;
#declare Blue    = rgb <0, 0, 1>;
#declare Yellow  = rgb <1,1,0>;
#declare Cyan    = rgb <0, 1, 1>;
#declare Magenta = rgb <1, 0, 1>;
#declare Clear   = rgbf 1;
#declare White   = rgb 1;
#declare Black   = rgb 0;

// These grays are useful for fine-tuning lighting color values
// and for other areas where subtle variations of grays are needed.
// PERCENTAGE GRAYS:
#declare Gray05 = White*0.05;
#declare Gray10 = White*0.10;
#declare Gray15 = White*0.15;
#declare Gray20 = White*0.20;
#declare Gray25 = White*0.25;
#declare Gray30 = White*0.30;
#declare Gray35 = White*0.35;
#declare Gray40 = White*0.40;
#declare Gray45 = White*0.45;
#declare Gray50 = White*0.50;
#declare Gray55 = White*0.55;
#declare Gray60 = White*0.60;
#declare Gray65 = White*0.65;
#declare Gray70 = White*0.70;
#declare Gray75 = White*0.75;
#declare Gray80 = White*0.80;
#declare Gray85 = White*0.85;
#declare Gray90 = White*0.90;
#declare Gray95 = White*0.95;

// OTHER GRAYS
#declare DimGray = color red 0.329412 green 0.329412 blue 0.329412;
#declare DimGrey = color red 0.329412 green 0.329412 blue 0.329412;
#declare Gray = color red 0.752941 green 0.752941 blue 0.752941;
#declare Grey = color red 0.752941 green 0.752941 blue 0.752941;
#declare LightGray = color red 0.658824 green 0.658824 blue 0.658824;
#declare LightGrey = color red 0.658824 green 0.658824 blue 0.658824;
#declare VLightGray = color red 0.80 green 0.80 blue 0.80;
#declare VLightGrey = color red 0.80 green 0.80 blue 0.80;

#declare Aquamarine = color red 0.439216 green 0.858824 blue 0.576471;
#declare BlueViolet = color red 0.62352 green 0.372549 blue 0.623529;
#declare Brown = color red 0.647059 green 0.164706 blue 0.164706;
#declare CadetBlue = color red 0.372549 green 0.623529 blue 0.623529;
#declare Coral = color red 1.0 green 0.498039 blue 0.0;
#declare CornflowerBlue = color red 0.258824 green 0.258824 blue 0.435294;
#declare DarkGreen = color red 0.184314 green 0.309804 blue 0.184314;
#declare DarkOliveGreen = color red 0.309804 green 0.309804 blue 0.184314;
#declare DarkOrchid = color red 0.6 green 0.196078 blue 0.8;
#declare DarkSlateBlue = color red 0.119608 green 0.137255 blue 0.556863;
#declare DarkSlateGray = color red 0.184314 green 0.309804 blue 0.309804;
#declare DarkSlateGrey = color red 0.184314 green 0.309804 blue 0.309804;
#declare DarkTurquoise = color red 0.439216 green 0.576471 blue 0.858824;
#declare Firebrick = color red 0.556863 green 0.137255 blue 0.137255;
#declare ForestGreen = color red 0.137255 green 0.556863 blue 0.137255;
#declare Gold = color red 0.8 green 0.498039 blue 0.196078;
#declare Goldenrod = color red 0.858824 green 0.858824 blue 0.439216;
#declare GreenYellow = color red 0.576471 green 0.858824 blue 0.439216;
#declare IndianRed = color red 0.309804 green 0.184314 blue 0.184314;
#declare Khaki = color red 0.623529 green 0.623529 blue 0.372549;
#declare LightBlue = color red 0.74902 green 0.847059 blue 0.847059;
#declare LightSteelBlue = color red 0.560784 green 0.560784 blue 0.737255;
#declare LimeGreen = color red 0.196078 green 0.8 blue 0.196078;
#declare Maroon = color red 0.556863 green 0.137255 blue 0.419608;
#declare MediumAquamarine = color red 0.196078 green 0.8 blue 0.6;
#declare MediumBlue = color red 0.196078 green 0.196078 blue 0.8;
#declare MediumForestGreen = color red 0.419608 green 0.556863 blue 0.137255;
#declare MediumGoldenrod = color red 0.917647 green 0.917647 blue 0.678431;
#declare MediumOrchid = color red 0.576471 green 0.439216 blue 0.858824;
#declare MediumSeaGreen = color red 0.258824 green 0.435294 blue 0.258824;
#declare MediumSlateBlue = color red 0.498039 blue 1.0;
#declare MediumSpringGreen = color red 0.498039 green 1.0;
#declare MediumTurquoise = color red 0.439216 green 0.858824 blue 0.858824;
#declare MediumVioletRed = color red 0.858824 green 0.439216 blue 0.576471;
#declare MidnightBlue = color red 0.184314 green 0.184314 blue 0.309804;
#declare Navy = color red 0.137255 green 0.137255 blue 0.556863;
#declare NavyBlue = color red 0.137255 green 0.137255 blue 0.556863;
#declare Orange = color red 1 green 0.5 blue 0.0;
#declare OrangeRed = color red 1.0 green 0.25;
#declare Orchid = color red 0.858824 green 0.439216 blue 0.858824;
#declare PaleGreen = color red 0.560784 green 0.737255 blue 0.560784;
#declare Pink = color red 0.737255 green 0.560784 blue 0.560784;
#declare Plum = color red 0.917647 green 0.678431 blue 0.917647;
#declare Salmon = color red 0.435294 green 0.258824 blue 0.258824;
#declare SeaGreen = color red 0.137255 green 0.556863 blue 0.419608;
#declare Sienna = color red 0.556863 green 0.419608 blue 0.137255;
#declare SkyBlue = color red 0.196078 green 0.6 blue 0.8;
#declare SlateBlue = color green 0.498039 blue 1.0;
#declare SpringGreen = color green 1.0 blue 0.498039;
#declare SteelBlue = color red 0.137255 green 0.419608 blue 0.556863;
#declare Tan = color red 0.858824 green 0.576471 blue 0.439216;
#declare Thistle = color red 0.847059 green 0.74902 blue 0.847059;
#declare Turquoise = color red 0.678431 green 0.917647 blue 0.917647;
#declare Violet = color red 0.309804 green 0.184314 blue 0.309804;
#declare VioletRed = color red 0.8 green 0.196078 blue 0.6;
#declare Wheat = color red 0.847059 green 0.847059 blue 0.74902;
#declare YellowGreen = color red 0.6 green 0.8 blue 0.196078;
#declare SummerSky = color red 0.22 green 0.69 blue 0.87;
#declare RichBlue = color red 0.35 green 0.35 blue 0.67;
#declare Brass =  color red 0.71 green 0.65 blue 0.26;
#declare Copper = color red 0.72 green 0.45 blue 0.20;
#declare Bronze = color red 0.55 green 0.47 blue 0.14;
#declare Bronze2 = color red 0.65 green 0.49 blue 0.24;
#declare Silver = color red 0.90 green 0.91 blue 0.98;
#declare BrightGold = color red 0.85 green 0.85 blue 0.10;
#declare OldGold =  color red 0.81 green 0.71 blue 0.23;
#declare Feldspar = color red 0.82 green 0.57 blue 0.46;
#declare Quartz = color red 0.85 green 0.85 blue 0.95;
#declare Mica = color Black;  // needed in textures.inc
#declare NeonPink = color red 1.00 green 0.43 blue 0.78;
#declare DarkPurple = color red 0.53 green 0.12 blue 0.47;
#declare NeonBlue = color red 0.30 green 0.30 blue 1.00;
#declare CoolCopper = color red 0.85 green 0.53 blue 0.10;
#declare MandarinOrange = color red 0.89 green 0.47 blue 0.20;
#declare LightWood = color red 0.91 green 0.76 blue 0.65;
#declare MediumWood = color red 0.65 green 0.50 blue 0.39;
#declare DarkWood = color red 0.52 green 0.37 blue 0.26;
#declare SpicyPink = color red 1.00 green 0.11 blue 0.68;
#declare SemiSweetChoc = color red 0.42 green 0.26 blue 0.15;
#declare BakersChoc = color red 0.36 green 0.20 blue 0.09;
#declare Flesh = color red 0.96 green 0.80 blue 0.69;
#declare NewTan = color red 0.92 green 0.78 blue 0.62;
#declare NewMidnightBlue = color red 0.00 green 0.00 blue 0.61;
#declare VeryDarkBrown = color red 0.35 green 0.16 blue 0.14;
#declare DarkBrown = color red 0.36 green 0.25 blue 0.20;
#declare DarkTan = color red 0.59 green 0.41 blue 0.31;
#declare GreenCopper = color red 0.32 green 0.49 blue 0.46;
#declare DkGreenCopper = color red 0.29 green 0.46 blue 0.43;
#declare DustyRose = color red 0.52 green 0.39 blue 0.39;
#declare HuntersGreen = color red 0.13 green 0.37 blue 0.31;
#declare Scarlet = color red 0.55 green 0.09 blue 0.09;

#declare Med_Purple =  color red 0.73 green 0.16 blue 0.96;
#declare Light_Purple = color red 0.87 green 0.58 blue 0.98;
#declare Very_Light_Purple = color red 0.94 green 0.81 blue 0.99;

#version Colors_Inc_Temp;
#end
