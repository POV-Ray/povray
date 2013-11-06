// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//      Persistence of Vision Raytracer Scene Description File
//      File: math.pov
//      Last updated: 9. Sept. 2001
//      Authors: Chris Huff / Bob Hughes / Tor Olav Kristensen
//      Description: Graphing of functions
//*******************************************
//
// -w320 -h320
// -w800 -h800 +a0.3

#version 3.7;
global_settings { assumed_gamma 1.0 }

#include "stdinc.inc"

//-------------------------------------------

#default { finish { ambient 1 diffuse 0 } }

//-------------------------------------------

// Which graph to show.
// 0 for power, 1 for trig, 2 for inverse trig, 3 for various
#declare Graphic = 3;


// The aspect ratio of the image.
// (To get equal thickness of vertical and horizontal lines.)
// E.g.: 800/600 = 4/3, 320/200 = 8/5 
#declare AspectRatio = 4/3;

#declare vAR = <AspectRatio, 1>;


// Set to true to get a border.  S L O W.
#declare Border = false; 

// Half of the "thickness" of the lines
#declare CylRadius = 0.006;

//-------------------------------------------

#macro v2Dto3D(v0)

  <v0.u, v0.v, 0>

#end // macro v2Dto3D


#macro vNorm(v0)

  #local pGraphCtr = (pGraphMax + pGraphMin)/2;
  #local vA = (v0 - pGraphCtr)/(pGraphMax - pGraphMin);
  
  (vA*vAR)

#end // macro vNorm



#macro PlotPoint(p0)

  sphere { v2Dto3D(vNorm(p0)), CylRadius }

#end // macro PlotPoint



#macro PlotLine(p0, p1)

  cylinder { v2Dto3D(vNorm(p0)), v2Dto3D(vNorm(p1)), CylRadius }

#end // macro PlotLine



#macro Graph(Func, Num, Color)

  union {
    #local X = pGraphMin.u;
    #local pLast = <X, Func(X)>;
    PlotPoint(pLast)
    #local Xstep = (pGraphMax.u - pGraphMin.u)/(Num - 1);
    #local J = 1;
    #while(J < Num)
       #local X = X + Xstep;
       #local pNext = <X, Func(X)>;
       PlotLine(pLast, pNext)
       PlotPoint(pNext)
       #local pLast = pNext;
       #local J = J + 1;
    #end
    #if (Border)
      clipped_by {
        box {
          -<0.5, 0.5, CylRadius>,
           <0.5, 0.5, CylRadius>
          scale vAR + z
        }
      }
    #end // if
    pigment { color Color }
  }

#end // macro Graph


#macro GraphSetup()
  
  // Coordinate grid
  box {
    -<1, 1, 0>, <1, 1, 1>
    scale v2Dto3D(vAR)/2 + z/10000
    texture {
      pigment { checker color Gray60, color White }
      scale v2Dto3D(GraphScale*(vNorm(<1, 1>) - vNorm(<0, 0>))) + z
      translate v2Dto3D(vNorm(<0, 0>))
    }
  }

  // X-axis
  object {
    PlotLine(pGraphMin*u, pGraphMax*u)
    pigment { color Black }
  }

  // Y-axis
  object {
    PlotLine(pGraphMin*v, pGraphMax*v)
    pigment { color Black }
  }

  camera {
    orthographic
    right     x*image_width/image_height
  //  right x
    up y
    scale v2Dto3D(vAR)*(Border ? 1.08 : 1) + z
    location -z
    look_at <0, 0, 0>
  }

  #if (Border)
    background { color Gray10 + Blue/2 }
  #end // if

#end // macro GraphSetup

//-------------------------------------------

#switch(Graphic)
    #case(0) // power functions (^ operator)
        #declare pGraphMin = <-5,-5>;
        #declare pGraphMax = <5, 5>;
        #declare GraphScale = <1, 1>;
        Graph(function (x) { x }, 300, Red)
        Graph(function (x) { x*x }, 300, Green)
        Graph(function (x) { x*x*x }, 300, Blue)
    #break
    #case(1) // trigonometric functions
        #declare pGraphMin = <-90, -1.5>;
        #declare pGraphMax = <270, 1.5>;
        #declare GraphScale = <10, 0.1>;
        Graph(function (x) { sind(x) }, 300, Red)
        Graph(function (x) { cosd(x) }, 300, Green)
        Graph(function (x) { tand(x) }, 300, Blue)
    #break
    #case(2) // inverse trigonometric functions
        #declare pGraphMin = <-1,-180>;
        #declare pGraphMax = <1, 180>;
        #declare GraphScale = <0.1, 10>;
        Graph(function (x) { asind(x) }, 300, Red)
        Graph(function (x) { acosd(x) }, 300, Green)
        Graph(function (x) { atan2d(x, 1) }, 300, Blue)
    #break
    #case(3) // various functins
        #declare pGraphMin = <-10,-10>;
        #declare pGraphMax = <10, 10>;
        #declare GraphScale = <1, 1>;
        Graph(function (x) { sgn(x) }, 300, Red)
        Graph(function (x) { clip(x, -3, 2) }, 300, Green)
        Graph(function (x) { clamp(x, -3, 2) + 0.001 }, 300, Blue)// +0.001 to make it visible beside clip()
        Graph(function (x) { adj_range(x, 3, 5) }, 300, Yellow)
        Graph(function (x) { adj_range2(x, -3, 2, 3, 5) }, 300, Orange)
    #break
#end

GraphSetup()

//*******************************************
