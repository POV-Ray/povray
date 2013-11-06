// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Raytracer Scene Description File
// Author: Juha Nieminen
// Description:
// Example scene demonstrating the use of the Sort_Array() macro.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings {assumed_gamma 1.0}

camera { orthographic
         location -z*10 
         look_at 0
       }
#default { finish { phong 1 phong_size 5 } }

light_source { < 100, 200,-300>, color 1.5 }
light_source { <-300, 100,-200>, color 1.0 }

#include "arrays.inc"

// Sorting an array of numbers:
// ---------------------------
#declare NumbersAmount = 20;
#declare Numbers = array[NumbersAmount];

#debug "\n*** Contents of the array before sorting: ***\n"

#declare S = seed(5);
#declare Ind = 0;
#while(Ind < NumbersAmount)
  #declare Numbers[Ind] = int(100*rand(S));
  #debug concat(" ", str(Numbers[Ind],0,0))
  #declare Ind = Ind+1;
#end

#debug "\n\n*** Contents of the array after sorting: ***\n"

Sort_Array(Numbers)

#declare Ind = 0;
#while(Ind < NumbersAmount)
  #debug concat(" ", str(Numbers[Ind],0,0))
  #declare Ind = Ind+1;
#end
#debug "\n\n"


// Sorting an array of colors:
// --------------------------
#declare ColorsAmount = 15;
#declare Colors = array[ColorsAmount];

#declare S = seed(4);
#declare Ind = 0;
#while(Ind < ColorsAmount)
  #declare Colors[Ind] = <rand(S), rand(S), rand(S)>;
  #declare Ind = Ind+1;
#end

#macro CreateBoxes(yCoord)
  #declare Width = 11/ColorsAmount;
  #declare Ind = 0;
  #while(Ind < ColorsAmount)
    cylinder
    { 0*y, 2.5*y, Width/2 rotate -20*x
      pigment { rgb  Colors[Ind] }
      translate <-6+12*Ind/ColorsAmount+Width/2, yCoord>
    }
    #declare Ind = Ind+1;
  #end
#end

text{
//ttf "timrom.ttf", "Colors before sorting:", .1, 0
  ttf "arial.ttf", "Colors before sorting:", .1, 0
  pigment { rgb 1 }
  scale .8
  translate <-6, 3.9>
}

CreateBoxes(1)

text{ 
// ttf "timrom.ttf", "Colors after sorting (by brightness):", .1, 0
  ttf "arial.ttf", "Colors after sorting (by brightness):", .1, 0
  pigment { rgb 1 }
  scale .8
  translate <-6, -1.1>
}

// Comparison macro which compares colors by brightness:
#macro Sort_Compare(Array, I1, I2)
  (Array[I1].gray < Array[I2].gray)
#end

Sort_Array(Colors)
CreateBoxes(-4)

