// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Ray Tracer version 3.7 Include File
//    File: colors_ral.inc
//    Last updated: 2016-12-24
//    Description: This file contains pre-defined colors from the German RAL Classic collection.

#ifndef(Colors_Ral_Inc_Temp)

#declare Colors_Ral_Inc_Temp = version;
#version 3.5;

#ifdef(View_POV_Include_Stack)
    #debug "including colors_ral.inc\n"
#end

#include "colors.inc"


// Source: Wikipedia, https://de.wikipedia.org/wiki/RAL-Farbe#Farbtabelle, 2016-12-22
// NOTE: Colours should be expected to differ slightly from official RAL publications, except for
// those marked with an asterisk.
// NOTE: These colour definitions require the use of "assumed_gamma 1.0".

// RAL 1xxx - YELLOW TONES

#declare RAL_1000 = rgb CLab2RGB_D65(<76.02, -0.37, 27.64>); //*Grünbeige         // Green beige
#declare RAL_1001 = rgb CLab2RGB_D65(<74.99,  5.10, 24.64>); // Beige             // Beige
#declare RAL_1002 = rgb CLab2RGB_D65(<73.45,  6.83, 33.80>); // Sandgelb          // Sand yellow
#declare RAL_1003 = rgb CLab2RGB_D65(<75.99, 18.80, 72.93>); // Signalgelb        // Signal yellow
#declare RAL_1004 = rgb CLab2RGB_D65(<71.42, 15.28, 69.28>); // Goldgelb          // Golden yellow
#declare RAL_1005 = rgb CLab2RGB_D65(<63.45, 13.38, 74.69>); //*Honiggelb         // Honey yellow
#declare RAL_1006 = rgb CLab2RGB_D65(<68.20, 21.13, 65.98>); // Maisgelb          // Maize yellow
#declare RAL_1007 = rgb CLab2RGB_D65(<68.38, 25.44, 67.13>); // Narzissengelb     // Daffodil yellow

#declare RAL_1011 = rgb CLab2RGB_D65(<59.92, 11.35, 29.17>); // Braunbeige        // Brown beige
#declare RAL_1012 = rgb CLab2RGB_D65(<75.04,  4.64, 61.31>); // Zitronengelb      // Lemon yellow
#declare RAL_1013 = rgb CLab2RGB_D65(<87.15,  0.27, 10.43>); //*Perlweiß          // Oyster white
#declare RAL_1014 = rgb CLab2RGB_D65(<81.22,  2.47, 22.88>); // Elfenbein         // Ivory
#declare RAL_1015 = rgb CLab2RGB_D65(<86.40,  2.06, 15.48>); // Hellelfenbein     // Light ivory
#declare RAL_1016 = rgb CLab2RGB_D65(<88.37, -9.78, 71.30>); // Schwefelgelb      // Sulfur yellow
#declare RAL_1017 = rgb CLab2RGB_D65(<76.32, 19.37, 51.02>); // Safrangelb        // Saffron yellow
#declare RAL_1018 = rgb CLab2RGB_D65(<83.35,  3.46, 75.83>); //*Zinkgelb          // Zinc yellow
#declare RAL_1019 = rgb CLab2RGB_D65(<62.62,  4.31, 12.94>); // Graubeige         // Grey beige
#declare RAL_1020 = rgb CLab2RGB_D65(<61.98,  0.39, 23.18>); // Olivgelb          // Olive yellow
#declare RAL_1021 = rgb CLab2RGB_D65(<78.88, 10.03, 82.04>); // Rapsgelb          // Colza yellow

#declare RAL_1023 = rgb CLab2RGB_D65(<79.07, 10.46, 80.50>); // Verkehrsgelb      // Traffic yellow
#declare RAL_1024 = rgb CLab2RGB_D65(<64.26,  8.49, 41.49>); //*Ockergelb         // Ochre yellow

#declare RAL_1026 = rgb CLab2RGB_D65(<95.36,-21.56,120.18>); // Leuchtgelb        // Luminous yellow
#declare RAL_1027 = rgb CLab2RGB_D65(<58.15,  5.83, 47.68>); // Currygelb         // Curry
#declare RAL_1028 = rgb CLab2RGB_D65(<74.97, 29.64, 79.69>); // Melonengelb       // Melon yellow

#declare RAL_1032 = rgb CLab2RGB_D65(<72.32, 12.16, 66.97>); // Ginstergelb       // Broom yellow
#declare RAL_1033 = rgb CLab2RGB_D65(<71.74, 27.78, 71.68>); //*Dahliengelb       // Dahlia yellow
#declare RAL_1034 = rgb CLab2RGB_D65(<72.73, 21.40, 45.09>); // Pastellgelb       // Pastel yellow
#declare RAL_1035 = rgb CLab2RGB_D65(<54.79,  0.35, 11.86>); // Perlbeige         // Pearl beige
#declare RAL_1036 = rgb CLab2RGB_D65(<48.95,  4.77, 26.69>); // Perlgold          // Pearl gold
#declare RAL_1037 = rgb CLab2RGB_D65(<70.28, 26.19, 64.79>); // Sonnengelb        // Sun yellow

// RAL 2xxx - ORANGE TONES

#declare RAL_2000 = rgb CLab2RGB_D65(<58.20, 37.30, 68.68>); //*Gelborange        // Yellow orange
#declare RAL_2001 = rgb CLab2RGB_D65(<49.41, 39.79, 35.29>); // Rotorange         // Red orange
#declare RAL_2002 = rgb CLab2RGB_D65(<47.74, 47.87, 33.73>); // Blutorange        // Vermilion
#declare RAL_2003 = rgb CLab2RGB_D65(<66.02, 41.22, 52.36>); // Pastellorange     // Pastel orange
#declare RAL_2004 = rgb CLab2RGB_D65(<56.89, 50.34, 49.81>); // Reinorange        // Pure orange
#declare RAL_2005 = rgb CLab2RGB_D65(<72.27, 87.78, 82.31>); //*Leuchtorange      // Luminous orange

#declare RAL_2007 = rgb CLab2RGB_D65(<76.86, 47.87, 97.63>); // Leuchthellorange  // Luminous bright orange
#declare RAL_2008 = rgb CLab2RGB_D65(<61.99, 44.64, 51.72>); // Hellrotorange     // Bright red orange
#declare RAL_2009 = rgb CLab2RGB_D65(<55.83, 47.79, 48.83>); // Verkehrsorange    // Traffic orange
#declare RAL_2010 = rgb CLab2RGB_D65(<55.39, 40.10, 42.42>); // Signalorange      // Signal orange
#declare RAL_2011 = rgb CLab2RGB_D65(<59.24, 40.86, 64.50>); //*Tieforange        // Deep orange
#declare RAL_2012 = rgb CLab2RGB_D65(<57.75, 40.28, 30.66>); // Lachsorange       // Salmon orange
#declare RAL_2013 = rgb CLab2RGB_D65(<40.73, 32.14, 34.92>); // Perlorange        // Pearl orange

// RAL 3xxx - RED TONES

#declare RAL_3000 = rgb CLab2RGB_D65(<42.40, 43.24, 25.00>); // Feuerrot          // Flame red
#declare RAL_3001 = rgb CLab2RGB_D65(<40.19, 41.21, 21.60>); // Signalrot         // Signal red
#declare RAL_3002 = rgb CLab2RGB_D65(<34.46, 48.83, 31.87>); //*Karminrot         // Carmine red
#declare RAL_3003 = rgb CLab2RGB_D65(<35.59, 35.87, 15.75>); // Rubinrot          // Ruby red
#declare RAL_3004 = rgb CLab2RGB_D65(<33.05, 25.61,  9.02>); // Purpurrot         // Purple red
#declare RAL_3005 = rgb CLab2RGB_D65(<30.96, 18.46,  5.76>); // Weinrot           // Wine red

#declare RAL_3007 = rgb CLab2RGB_D65(<28.34,  8.14,  2.22>); // Schwarzrot        // Black red

#declare RAL_3009 = rgb CLab2RGB_D65(<29.27, 24.59, 16.51>); //*Oxidrot           // Oxide red

#declare RAL_3011 = rgb CLab2RGB_D65(<34.52, 28.66, 13.44>); // Braunrot          // Brown red
#declare RAL_3012 = rgb CLab2RGB_D65(<63.81, 20.79, 20.45>); // Beigerot          // Beige red
#declare RAL_3013 = rgb CLab2RGB_D65(<40.70, 36.67, 21.37>); // Tomatenrot        // Tomato red
#declare RAL_3014 = rgb CLab2RGB_D65(<60.17, 32.49, 12.58>); // Altrosa           // Antique pink
#declare RAL_3015 = rgb CLab2RGB_D65(<71.23, 21.59,  4.98>); //*Hellrosa          // Light pink
#declare RAL_3016 = rgb CLab2RGB_D65(<44.70, 37.92, 23.96>); // Korallenrot       // Coral red
#declare RAL_3017 = rgb CLab2RGB_D65(<54.24, 44.26, 16.87>); // Rosé              // Rose
#declare RAL_3018 = rgb CLab2RGB_D65(<50.77, 49.15, 19.86>); // Erdbeerrot        // Strawberry red

#declare RAL_3020 = rgb CLab2RGB_D65(<44.66, 52.03, 32.26>); // Verkehrsrot       // Traffic red

#declare RAL_3022 = rgb CLab2RGB_D65(<56.06, 38.90, 29.70>); //*Lachsrot          // Salmon pink

#declare RAL_3024 = rgb CLab2RGB_D65(<51.32, 82.52, 71.62>); // Leuchtrot         // Luminous red

#declare RAL_3026 = rgb CLab2RGB_D65(<54.38, 86.26, 76.07>); // Leuchthellrot     // Luminous bright red
#declare RAL_3027 = rgb CLab2RGB_D65(<43.07, 46.96, 15.81>); // Himbeerrot        // Raspberry red
#declare RAL_3028 = rgb CLab2RGB_D65(<48.80, 54.42, 33.08>); // Reinrot           // Pure red

#declare RAL_3031 = rgb CLab2RGB_D65(<39.39, 47.09, 24.71>); //*Orientrot         // Orient red
#declare RAL_3032 = rgb CLab2RGB_D65(<26.88, 41.34, 19.40>); // Perlrubinrot      // Pearl ruby red
#declare RAL_3033 = rgb CLab2RGB_D65(<44.29, 45.11, 28.62>); // Perlrosa          // Pearl pink

// RAL 4xxx - PURPLE TONES

#declare RAL_4001 = rgb CLab2RGB_D65(<49.10, 17.35,-12.85>); // Rotlila           // Red lilac
#declare RAL_4002 = rgb CLab2RGB_D65(<41.91, 30.05,  5.67>); // Rotviolett        // Red violet
#declare RAL_4003 = rgb CLab2RGB_D65(<54.25, 44.66, -5.02>); //*Erikaviolett      // Heather violet
#declare RAL_4004 = rgb CLab2RGB_D65(<32.22, 24.83,  0.06>); // Bordeauxviolett   // Claret violet
#declare RAL_4005 = rgb CLab2RGB_D65(<50.92, 15.38,-23.06>); // Blaulila          // Blue lilac
#declare RAL_4006 = rgb CLab2RGB_D65(<42.38, 39.48,-14.94>); // Verkehrspurpur    // Traffic purple
#declare RAL_4007 = rgb CLab2RGB_D65(<30.05, 13.16, -5.10>); // Purpurviolett     // Purple violet
#declare RAL_4008 = rgb CLab2RGB_D65(<40.76, 32.53,-20.56>); //*Signalviolett     // Signal violet
#declare RAL_4009 = rgb CLab2RGB_D65(<60.59, 10.38, -2.88>); // Pastellviolett    // Pastel violet
#declare RAL_4010 = rgb CLab2RGB_D65(<50.39, 48.95, -4.24>); // Telemagenta       // Telemagenta
#declare RAL_4011 = rgb CLab2RGB_D65(<47.92, 18.89,-20.83>); // Perlviolett       // Pearl violet
#declare RAL_4012 = rgb CLab2RGB_D65(<46.33,  7.27,-11.94>); // Perlbrombeer      // Pearl blackberry

// RAL 5xxx - BLUE TONES

#declare RAL_5000 = rgb CLab2RGB_D65(<32.59, -1.28,-21.69>); //*Violettblau       // Violet blue
#declare RAL_5001 = rgb CLab2RGB_D65(<35.43, -7.52,-16.65>); // Grünblau          // Green blue
#declare RAL_5002 = rgb CLab2RGB_D65(<33.11,  8.43,-35.40>); // Ultramarinblau    // Ultramarine blue
#declare RAL_5003 = rgb CLab2RGB_D65(<30.53, -0.37,-16.68>); // Saphirblau        // Sapphire blue
#declare RAL_5004 = rgb CLab2RGB_D65(<26.56, -0.19, -4.07>); // Schwarzblau       // Black blue
#declare RAL_5005 = rgb CLab2RGB_D65(<32.45, -6.68,-37.20>); //*Signalblau        // Signal blue

#declare RAL_5007 = rgb CLab2RGB_D65(<46.37, -6.24,-21.71>); // Brillantblau      // Brilliant blue
#declare RAL_5008 = rgb CLab2RGB_D65(<32.00, -2.09, -6.07>); // Graublau          // Grey blue
#declare RAL_5009 = rgb CLab2RGB_D65(<41.22, -9.56,-18.34>); // Azurblau          // Azure blue
#declare RAL_5010 = rgb CLab2RGB_D65(<36.57, -5.81,-28.94>); // Enzianblau        // Gentian blue
#declare RAL_5011 = rgb CLab2RGB_D65(<16.97, -1.20,-13.15>); //*Stahlblau         // Steel blue
#declare RAL_5012 = rgb CLab2RGB_D65(<55.62,-13.84,-30.72>); // Lichtblau         // Light blue
#declare RAL_5013 = rgb CLab2RGB_D65(<29.81,  1.67,-17.20>); // Kobaltblau        // Cobalt blue
#declare RAL_5014 = rgb CLab2RGB_D65(<53.79, -2.64,-15.59>); // Taubenblau        // Pigeon blue
#declare RAL_5015 = rgb CLab2RGB_D65(<51.13,-12.69,-34.21>); // Himmelblau        // Sky blue

#declare RAL_5017 = rgb CLab2RGB_D65(<34.82,-13.49,-36.36>); //*Verkehrsblau      // Traffic blue
#declare RAL_5018 = rgb CLab2RGB_D65(<55.13,-27.27, -8.47>); // Türkisblau        // Turquoise blue
#declare RAL_5019 = rgb CLab2RGB_D65(<41.18, -9.97,-25.87>); // Capriblau         // Capri blue
#declare RAL_5020 = rgb CLab2RGB_D65(<32.30,-13.01, -9.39>); // Ozeanblau         // Ocean blue
#declare RAL_5021 = rgb CLab2RGB_D65(<47.15,-29.26, -9.32>); // Wasserblau        // Water blue
#declare RAL_5022 = rgb CLab2RGB_D65(<19.87, 10.63,-28.48>); //*Nachtblau         // Night blue
#declare RAL_5023 = rgb CLab2RGB_D65(<47.64, -2.96,-21.18>); // Fernblau          // Distant blue
#declare RAL_5024 = rgb CLab2RGB_D65(<60.50, -9.53,-17.38>); // Pastellblau       // Pastel blue
#declare RAL_5025 = rgb CLab2RGB_D65(<35.93,-11.81,-16.28>); // Perlenzian        // Pearl gentian blue
#declare RAL_5026 = rgb CLab2RGB_D65(<16.00,  7.84,-29.10>); // Perlnachtblau     // Pearl night blue

// RAL 6xxx - GREEN TONES

#declare RAL_6000 = rgb CLab2RGB_D65(<44.53,-23.65,  5.32>); //*Patinagrün        // Patina green
#declare RAL_6001 = rgb CLab2RGB_D65(<43.86,-23.57, 18.31>); // Smaragdgrün       // Emerald green
#declare RAL_6002 = rgb CLab2RGB_D65(<39.87,-19.39, 16.95>); // Laubgrün          // Leaf green
#declare RAL_6003 = rgb CLab2RGB_D65(<39.25, -4.36, 10.17>); // Olivgrün          // Olive green
#declare RAL_6004 = rgb CLab2RGB_D65(<33.40,-13.17, -3.07>); // Blaugrün          // Blue green
#declare RAL_6005 = rgb CLab2RGB_D65(<24.44,-20.57,  4.67>); //*Moosgrün          // Moss green
#declare RAL_6006 = rgb CLab2RGB_D65(<33.04, -1.11,  4.17>); // Grauoliv          // Grey olive
#declare RAL_6007 = rgb CLab2RGB_D65(<30.42, -3.85,  4.77>); // Flaschengrün      // Bottle green
#declare RAL_6008 = rgb CLab2RGB_D65(<29.82, -0.67,  4.34>); // Braungrün         // Brown green
#declare RAL_6009 = rgb CLab2RGB_D65(<29.81, -5.74,  3.12>); // Tannengrün        // Fir green
#declare RAL_6010 = rgb CLab2RGB_D65(<42.99,-22.87, 26.09>); //*Grasgrün          // Grass green
#declare RAL_6011 = rgb CLab2RGB_D65(<53.24,-11.61, 14.48>); // Resedagrün        // Reseda green
#declare RAL_6012 = rgb CLab2RGB_D65(<31.94, -4.36, -0.46>); // Schwarzgrün       // Black green
#declare RAL_6013 = rgb CLab2RGB_D65(<52.30, -2.08, 14.26>); // Schilfgrün        // Reed green
#declare RAL_6014 = rgb CLab2RGB_D65(<33.84,  0.46,  6.15>); // Gelboliv          // Yellow olive
#declare RAL_6015 = rgb CLab2RGB_D65(<25.48, -1.59,  4.15>); //*Schwarzoliv       // Black olive
#declare RAL_6016 = rgb CLab2RGB_D65(<42.92,-32.22,  6.72>); // Türkisgrün        // Turquoise green
#declare RAL_6017 = rgb CLab2RGB_D65(<52.33,-23.24, 26.15>); // Maigrün           // May green
#declare RAL_6018 = rgb CLab2RGB_D65(<59.83,-32.96, 37.72>); // Gelbgrün          // Yellow green
#declare RAL_6019 = rgb CLab2RGB_D65(<81.42,-12.57, 13.50>); // Weißgrün          // Pastel green
#declare RAL_6020 = rgb CLab2RGB_D65(<26.34, -8.37, 10   >); //*Chromoxidgrün     // Chrome green
#declare RAL_6021 = rgb CLab2RGB_D65(<63.69,-11.28, 14.13>); // Blassgrün         // Pale green
#declare RAL_6022 = rgb CLab2RGB_D65(<30.43,  0.54,  5.62>); // Braunoliv         // Olive drab

#declare RAL_6024 = rgb CLab2RGB_D65(<51.81,-38.02, 15.50>); // Verkehrsgrün      // Traffic green
#declare RAL_6025 = rgb CLab2RGB_D65(<47.45,-13.45, 21.37>); // Farngrün          // Fern green
#declare RAL_6026 = rgb CLab2RGB_D65(<34.35,-36.57,  0.83>); //*Opalgrün          // Opal green
#declare RAL_6027 = rgb CLab2RGB_D65(<72.80,-19.82, -3.62>); // Lichtgrün         // Light green
#declare RAL_6028 = rgb CLab2RGB_D65(<38.15,-12.86,  3.82>); // Kieferngrün       // Pine green
#declare RAL_6029 = rgb CLab2RGB_D65(<44.18,-39.06, 15.73>); // Minzgrün          // Mint green

#declare RAL_6032 = rgb CLab2RGB_D65(<50.67,-33.25, 14.76>); // Signalgrün        // Signal green
#declare RAL_6033 = rgb CLab2RGB_D65(<51.93,-22.93, -2.33>); //*Minttürkis        // Mint turquoise
#declare RAL_6034 = rgb CLab2RGB_D65(<69.16,-15.95, -5.10>); // Pastelltürkis     // Pastel turquoise
#declare RAL_6035 = rgb CLab2RGB_D65(<29.14,-29.19, 16.35>); // Perlgrün          // Pearl green
#declare RAL_6036 = rgb CLab2RGB_D65(<33.97,-29.04,  0.68>); // Perlopalgrün      // Pearl opal green
#declare RAL_6037 = rgb CLab2RGB_D65(<53.49,-46.77, 34.32>); // Reingrün          // Pure green
#declare RAL_6038 = rgb CLab2RGB_D65(<62.31,-84.29, 57.55>); //*Leuchtgrün        // Luminous green

// RAL 7xxx - GREY TONES

#declare RAL_7000 = rgb CLab2RGB_D65(<58.32, -3.14, -4.71>); // Fehgrau           // Squirrel grey
#declare RAL_7001 = rgb CLab2RGB_D65(<63.81, -2.22, -4.05>); // Silbergrau        // Silver grey
#declare RAL_7002 = rgb CLab2RGB_D65(<54.51, -0.09, 10.69>); // Olivgrau          // Olive grey
#declare RAL_7003 = rgb CLab2RGB_D65(<52.32, -1.18,  6.92>); // Moosgrau          // Moss grey
#declare RAL_7004 = rgb CLab2RGB_D65(<63.83,  0.19, -0.44>); //*Signalgrau        // Signal grey
#declare RAL_7005 = rgb CLab2RGB_D65(<50.00, -1.55,  0.82>); // Mausgrau          // Mouse grey
#declare RAL_7006 = rgb CLab2RGB_D65(<48.53,  2.15,  7.57>); // Beigegrau         // Beige grey

#declare RAL_7008 = rgb CLab2RGB_D65(<45.91,  3.34, 17.92>); // Khakigrau         // Khaki grey
#declare RAL_7009 = rgb CLab2RGB_D65(<43.19, -2.43,  3.87>); // Grüngrau          // Green grey
#declare RAL_7010 = rgb CLab2RGB_D65(<38.44, -2.33,  2.59>); //*Zeltgrau          // Tarpaulin grey
#declare RAL_7011 = rgb CLab2RGB_D65(<41.52, -1.68, -2.72>); // Eisengrau         // Iron grey
#declare RAL_7012 = rgb CLab2RGB_D65(<44.34, -1.77, -1.71>); // Basaltgrau        // Basalt grey
#declare RAL_7013 = rgb CLab2RGB_D65(<39.21,  0.59,  6.33>); // Braungrau         // Brown grey

#declare RAL_7015 = rgb CLab2RGB_D65(<40.50, -0.25, -3.40>); // Schiefergrau      // Slate grey
#declare RAL_7016 = rgb CLab2RGB_D65(<25.93, -1.85, -3.40>); //*Anthrazitgrau     // Anthracite grey

#declare RAL_7021 = rgb CLab2RGB_D65(<30.65, -0.43, -1.22>); // Schwarzgrau       // Black grey
#declare RAL_7022 = rgb CLab2RGB_D65(<37.75, -0.07,  2.23>); // Umbragrau         // Umbra grey
#declare RAL_7023 = rgb CLab2RGB_D65(<55.60, -1.45,  4.52>); // Betongrau         // Concrete grey
#declare RAL_7024 = rgb CLab2RGB_D65(<36.97, -0.13, -3.32>); // Graphitgrau       // Graphite grey

#declare RAL_7026 = rgb CLab2RGB_D65(<27.43, -4.01, -3.11>); //*Granitgrau        // Granite grey

#declare RAL_7030 = rgb CLab2RGB_D65(<61.31, -0.26,  4.53>); // Steingrau         // Stone grey
#declare RAL_7031 = rgb CLab2RGB_D65(<47.83, -2.96, -4.01>); // Blaugrau          // Blue grey
#declare RAL_7032 = rgb CLab2RGB_D65(<73.39, -0.93,  8.09>); // Kieselgrau        // Pebble grey
#declare RAL_7033 = rgb CLab2RGB_D65(<56.78, -3.36,  6.32>); // Zementgrau        // Cement grey
#declare RAL_7034 = rgb CLab2RGB_D65(<56.86,  0.03, 14.83>); //*Gelbgrau          // Yelow grey
#declare RAL_7035 = rgb CLab2RGB_D65(<81.29, -1.24,  0.79>); // Lichtgrau         // Light grey
#declare RAL_7036 = rgb CLab2RGB_D65(<63.49,  1.27,  0.78>); // Platingrau        // Platinum grey
#declare RAL_7037 = rgb CLab2RGB_D65(<55.30, -0.46,  0.22>); // Staubgrau         // Dust grey
#declare RAL_7038 = rgb CLab2RGB_D65(<72.97, -1.50,  2.97>); // Achatgrau         // Agate grey
#declare RAL_7039 = rgb CLab2RGB_D65(<43.50,  0.37,  5.56>); //*Quarzgrau         // Quartz grey
#declare RAL_7040 = rgb CLab2RGB_D65(<66.63, -1.17, -2.82>); // Fenstergrau       // Window grey

#declare RAL_7042 = rgb CLab2RGB_D65(<62.58, -1.51, -0.21>); // Verkehrsgrau A    // Traffic grey A
#declare RAL_7043 = rgb CLab2RGB_D65(<40.23, -1.28,  0.00>); // Verkehrsgrau B    // Traffic grey B
#declare RAL_7044 = rgb CLab2RGB_D65(<74.66, -0.04,  5.08>); // Seidengrau        // Silk grey
#declare RAL_7045 = rgb CLab2RGB_D65(<60.35, -1.43, -1.84>); //*Telegrau 1        // Telegrey 1
#declare RAL_7046 = rgb CLab2RGB_D65(<57.75, -1.60, -3.00>); // Telegrau 2        // Telegrey 2
#declare RAL_7047 = rgb CLab2RGB_D65(<81.43,  0.01,  0.10>); // Telegrau 4        // Telegrey 4
#declare RAL_7048 = rgb CLab2RGB_D65(<54.55, -0.45,  7.59>); // Perlmausgrau      // Pearl mouse grey

// RAL 8xxx - BROWN TONES

#declare RAL_8000 = rgb CLab2RGB_D65(<49.48,  5.14, 28.64>); // Grünbraun         // Green brown
#declare RAL_8001 = rgb CLab2RGB_D65(<47.08, 18.95, 39.87>); //*Ockerbraun        // Ochre brown
#declare RAL_8002 = rgb CLab2RGB_D65(<41.88, 14.45, 13.31>); // Signalbraun       // Signal brown
#declare RAL_8003 = rgb CLab2RGB_D65(<42.56, 15.59, 21.67>); // Lehmbraun         // Clay brown
#declare RAL_8004 = rgb CLab2RGB_D65(<43.78, 22.83, 20.22>); // Kupferbraun       // Copper brown

#declare RAL_8007 = rgb CLab2RGB_D65(<38.99, 12.62, 17.08>); // Rehbraun          // Fawn brown
#declare RAL_8008 = rgb CLab2RGB_D65(<35.15, 13.22, 28.50>); //*Olivbraun         // Olive brown

#declare RAL_8011 = rgb CLab2RGB_D65(<33.98, 10.04, 10.97>); // Nussbraun         // Nut brown
#declare RAL_8012 = rgb CLab2RGB_D65(<34.39, 17.06, 10.17>); // Rotbraun          // Red brown

#declare RAL_8014 = rgb CLab2RGB_D65(<31.99,  4.77,  7.71>); // Sepiabraun        // Sepia brown
#declare RAL_8015 = rgb CLab2RGB_D65(<33.52, 15.02,  9.25>); // Kastanienbraun    // Chestnut brown
#declare RAL_8016 = rgb CLab2RGB_D65(<21.40, 14.37, 13.84>); //*Mahagonibraun     // Mahogany brown
#declare RAL_8017 = rgb CLab2RGB_D65(<30.60,  5.99,  4.34>); // Schokoladen-braun // Chocolate brown

#declare RAL_8019 = rgb CLab2RGB_D65(<31.46,  2.12,  1.10>); // Graubraun         // Grey brown

#declare RAL_8022 = rgb CLab2RGB_D65(<25.08,  1.18,  0.67>); // Schwarzbraun      // Black brown
#declare RAL_8023 = rgb CLab2RGB_D65(<49.37, 24.91, 30.25>); // Orangebraun       // Orange brown
#declare RAL_8024 = rgb CLab2RGB_D65(<38.04, 14.14, 20.82>); //*Beigebraun        // Beige brown
#declare RAL_8025 = rgb CLab2RGB_D65(<44.00,  7.95, 11.73>); // Blassbraun        // Pale brown

#declare RAL_8028 = rgb CLab2RGB_D65(<34.19,  5.72,  8.58>); // Terrabraun        // Terra brown
#declare RAL_8029 = rgb CLab2RGB_D65(<35.06, 25.58, 27.32>); // Perlkupfer        // Pearl copper

// RAL 9xxx - WHITE AND BLACK TONES

#declare RAL_9001 = rgb CLab2RGB_D65(<90.40,  0.66,  6.64>); // Cremeweiß         // Cream
#declare RAL_9002 = rgb CLab2RGB_D65(<85.07, -1.04,  5.18>); //*Grauweiß          // Grey white
#declare RAL_9003 = rgb CLab2RGB_D65(<94.13, -0.55,  0.81>); // Signalweiß        // Signal white
#declare RAL_9004 = rgb CLab2RGB_D65(<28.66,  0.24, -0.66>); // Signalschwarz     // Signal black
#declare RAL_9005 = rgb CLab2RGB_D65(<25.33,  0.13, -0.16>); // Tiefschwarz       // Jet black
#declare RAL_9006 = rgb CLab2RGB_D65(<67.77, -0.58,  0.76>); // Weißaluminium     // White aluminium
#declare RAL_9007 = rgb CLab2RGB_D65(<55.55, -0.06,  2.14>); //*Graualuminium     // Grey aluminium

#declare RAL_9010 = rgb CLab2RGB_D65(<94.57, -0.47,  4.14>); // Reinweiß          // Pure white
#declare RAL_9011 = rgb CLab2RGB_D65(<26.54, -0.05, -1.13>); // Graphitschwarz    // Graphite black

#declare RAL_9016 = rgb CLab2RGB_D65(<95.26, -0.76,  2.11>); // Verkehrsweiß      // Traffic white
#declare RAL_9017 = rgb CLab2RGB_D65(<27.25,  0.44,  0.51>); // Verkehrsschwarz   // Traffic black
#declare RAL_9018 = rgb CLab2RGB_D65(<81.34, -2.29,  2.96>); //*Papyrusweiß       // Papyrus white

#declare RAL_9022 = rgb CLab2RGB_D65(<65.38, -0.43,  0.34>); // Perlhellgrau      // Pearl light grey
#declare RAL_9023 = rgb CLab2RGB_D65(<57.32, -0.31, -0.98>); // Perldunkelgrau    // Pearl dark grey

// RAL F9 COLOURS (Deutsche Bundeswehr)

//#declare RAL_6031 = rgb CLab2RGB_D65(<?????, ?????, ?????>); // Bronzegrün
//#declare RAL_8027 = rgb CLab2RGB_D65(<?????, ?????, ?????>); // Lederbraun
//#declare RAL_9021 = rgb CLab2RGB_D65(<?????, ?????, ?????>); // Teerschwarz

//#declare RAL_1039 = rgb CLab2RGB_D65(<?????, ?????, ?????>); // Sandbeige
//#declare RAL_1040 = rgb CLab2RGB_D65(<?????, ?????, ?????>); // Lehmbeige
//#declare RAL_6040 = rgb CLab2RGB_D65(<?????, ?????, ?????>); // Helloliv
//#declare RAL_7050 = rgb CLab2RGB_D65(<?????, ?????, ?????>); // Tarngrau
//#declare RAL_8031 = rgb CLab2RGB_D65(<?????, ?????, ?????>); // Sandbraun

// OBSOLETED RAL COLOURS (no longer part of official RAL colour collection)

//#declare RAL_4000 = rgb CLab2RGB_D65(<?????, ?????, ?????>); // (Violett)
//#declare RAL_7018 = rgb CLab2RGB_D65(<?????, ?????, ?????>); // (Blaugrau)
//#declare RAL_7027 = rgb CLab2RGB_D65(<?????, ?????, ?????>); // (Grau)
//#declare RAL_7028 = rgb CLab2RGB_D65(<?????, ?????, ?????>); // (Dunkelgelb)
//#declare RAL_8020 = rgb CLab2RGB_D65(<?????, ?????, ?????>); // (Gelbbraun)

#version Colors_Ral_Inc_Temp;
#end
