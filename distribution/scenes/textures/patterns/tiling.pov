// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Raytracer sample file.
// A tiling pattern demo scene by Samuel Benge

/******************************************************************************
 * $File: //depot/povray/smp/distribution/scenes/textures/patterns/tiling.pov $
 * $Revision: #2 $
 * $Change: 5430 $
 * $DateTime: 2011/03/21 10:42:00 $
 * $Author: jholsenback $
 *****************************************************************************/

#version 3.7;

global_settings {assumed_gamma 1.0}
background {rgb 1}

camera {location <0,10,-10> look_at <0,0,0> right  x*image_width/image_height}
light_source {<0, 30, -30> color rgb 1}

#include "colors.inc"
#include "math.inc"

// see the documentaion for possible patterns and types
#declare TilingPattern = 5;
#declare TileTypes    = 3;

// change to yes to write the color_map to a text file
#declare WriteMap = no;

// 0 = caulk off
// 1 = caulk at every color
// TileTypes = caulk at n TileTypes
#declare CaulkAt    = 1;
#declare CaulkSize  = 0.2;
#declare CaulkColor = White;

#declare ColorArray =
  array[12]{
    Red,        White,
    Green,      Yellow,
    Blue,       Cyan,
    IndianRed,  Pink,
    DarkGreen,  Turquoise
    Violet,     BlueViolet
  };

#if (WriteMap)
  #fopen MapFile "tilemap.txt" write
  #write (MapFile, "texture {\n  pigment {\n   tiling ",TilingPattern, "\n   color_map {\n")
#end

plane{y,0
  texture{
    pigment{
      tiling TilingPattern
      color_map{
        #declare V=0;
        #declare V2 = 0;
        #while(V<TileTypes*2)
          #if(mod(V,CaulkAt*2) = CaulkAt*2-1 & CaulkAt!=0)
            #declare ThisColor = CaulkColor;
           #else
            #declare ThisColor =
              ColorArray[mod(V,dimension_size(ColorArray,1))];
          #end
          #declare incre = (1.0-CaulkSize)*even(V)+CaulkSize*odd(V);
          [ V2/TileTypes ThisColor]
          [ (V2+incre)/TileTypes ThisColor]

          #if (WriteMap)
            #write (MapFile, "    [", str(V2/TileTypes,5,4)," rgb <", vstr(3, ThisColor,", ", 0,4),">]\n")
            #write (MapFile, "    [", str((V2+incre)/TileTypes,5,4)," rgb <", vstr(3, ThisColor,", ", 0,4),">]\n")
          #end

          #declare V = V + 1;
          #declare V2 = V2 + incre;
        #end
      }
    }
  }
}

#if (WriteMap)
  #write (MapFile, "    }\n  }\n}\n")
  #fclose MapFile
#end

//----------------------- Reference code --------------------------
// Useful scalings if using the tiling pattern in combination with
// other functions, patterns or warps. Usually the scale required
// to fit core repeated tile into one unit square size.
#switch (TilingPattern)
  #case (1)
    #declare TileScale = <1,1,1>;
  #break
  #case (2) #case (5)
    #declare TileScale = <2/3,1,1/sqrt(3)>;
  #break
  #case (3) #case (4)
    #declare TileScale = <2/3,1,1/sqrt(3)>;
  #break
  #case (6) #case (18)
    #declare TileScale = <1/4,1,1/4>;
  #break
  #case (7)
    #declare TileScale = <1/(sqrt(2)+1),1,1/(sqrt(2)+1)>;
  #break
  #case (8)
    #declare TileScale = <1,1,1/(sqrt(3)+2)>;
  #break
  #case (9)
    #declare TileScale = <1/2,1,1/(2*sqrt(3))>;
  #break
  #case (10)
    #declare TileScale = <1,1,1/2>;
  #break
  #case (11) #case (12)
    #declare TileScale = <1/3,1,1/3>;
  #break
  #case (13) #case (14)
    #declare TileScale = <1/(sqrt(3)+1),1,1/(sqrt(3)+1)>;
  #break
  #case (15)
    #declare TileScale = <1/5,1,1/5>;
  #break
  #case (16) #case (17)
    #declare TileScale = <1/(sqrt(3)+3),1,1/((sqrt(3)+1.0)*2.0)>;
  #break
  #case (19) #case (20)
    #declare TileScale = <1/2,1,1/((sqrt(3)/2)*3)>;
  #break
  #case (21)
    #declare TileScale = <1/(1+sqrt(3)),1,1/(1+sqrt(3))>;
  #break
  #case (22)
    #declare TileScale = <1/(2+sqrt(3)),1,1/(3+(2*sqrt(3)))>;
  #break
  #case (23) #case (24)
    #declare TileScale = <1/(3+sqrt(3)),1,1/(3+(3*sqrt(3)))>;
  #break
  // Unit square scaling not possible with Penrose tiling.
  // Scaling given useful to see all tiles in a way symmetrical in z
  #case (25) #case (26) #case (27)
    #declare TileScale = <1/(4*sqrt(2)),1,1/(4*sqrt(2))>;
  #break
  #else
    #declare TileScale = <1,1,1>;
#end

