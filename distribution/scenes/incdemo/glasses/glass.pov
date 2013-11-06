// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Raytracer Scene Description File
// File: glass.pov
// Desc: glass.inc sample scene (using sample.inc include file)
// Date: July 2001
// Auth: Christoph Hormann (modified by Chris Huff, adding photons)

// -w320 -h240 +a0.3 +sf1 +ef4 +kff5

#version 3.6;
global_settings { assumed_gamma 1.0 }


#if (!clock_on)
  #warning concat("This scene should be rendered as an animation\n",
                  "use '+sf1 +ef4' for rendering all versions.\n")
#end

global_settings {
	max_trace_level 8
}

#include "colors.inc"
#include "glass.inc"

// ---- For the sample include file ----

#declare MaxX=5;
#declare MaxZ=5;
#declare SpaceX=3.4;
#declare SpaceZ=3.4;
#declare SpaceY=1;
#declare Scale=1.4;

#declare MaxCnt = 25;
#declare MArr = array[MaxCnt]
#declare Type = 3;   // use '4' for photons
#declare Obj_Nbr = 1;

#declare Plane_Pigment=
  pigment {
    color rgb 1
    //checker color Gray40 color Gray80
    //scale <1, 1, 10000>
    //rotate y*45
}
// --------


#if ((frame_number=1) | (frame_number=2))
  #declare T_01 = texture { pigment { color Clear } finish { F_Glass1 } }
  #declare T_02 = texture { pigment { color Clear } finish { F_Glass2 } }
  #declare T_03 = texture { pigment { color Clear } finish { F_Glass3 } }
  #declare T_04 = texture { pigment { color Clear } finish { F_Glass4 } }
  #declare T_05 = texture { pigment { color Clear } finish { F_Glass5 } }
#else
  #declare T_01 = texture { pigment { color Clear } finish { F_Glass6 } }
  #declare T_02 = texture { pigment { color Clear } finish { F_Glass7 } }
  #declare T_03 = texture { pigment { color Clear } finish { F_Glass8 } }
  #declare T_04 = texture { pigment { color Clear } finish { F_Glass9 } }
  #declare T_05 = texture { pigment { color Clear } finish { F_Glass10 } }
#end

#if ((frame_number=0) | (frame_number=1) | (frame_number=3))
  #declare MArr[0]  = material { texture { T_01 } interior { I_Glass fade_color Col_Blue_01 } }
  #declare MArr[1]  = material { texture { T_02 } interior { I_Glass fade_color Col_Blue_01 } }
  #declare MArr[2]  = material { texture { T_03 } interior { I_Glass fade_color Col_Blue_01 } }
  #declare MArr[3]  = material { texture { T_04 } interior { I_Glass fade_color Col_Blue_01 } }
  #declare MArr[4]  = material { texture { T_05 } interior { I_Glass fade_color Col_Blue_01 } }

  #declare MArr[5]  = material { texture { T_01 } interior { I_Glass1 fade_color Col_Blue_01 } }
  #declare MArr[6]  = material { texture { T_02 } interior { I_Glass1 fade_color Col_Blue_01 } }
  #declare MArr[7]  = material { texture { T_03 } interior { I_Glass1 fade_color Col_Blue_01 } }
  #declare MArr[8]  = material { texture { T_04 } interior { I_Glass1 fade_color Col_Blue_01 } }
  #declare MArr[9]  = material { texture { T_05 } interior { I_Glass1 fade_color Col_Blue_01 } }

  #declare MArr[10] = material { texture { T_01 } interior { I_Glass2 fade_color Col_Blue_01 } }
  #declare MArr[11] = material { texture { T_02 } interior { I_Glass2 fade_color Col_Blue_01 } }
  #declare MArr[12] = material { texture { T_03 } interior { I_Glass2 fade_color Col_Blue_01 } }
  #declare MArr[13] = material { texture { T_04 } interior { I_Glass2 fade_color Col_Blue_01 } }
  #declare MArr[14] = material { texture { T_05 } interior { I_Glass2 fade_color Col_Blue_01 } }

  #declare MArr[15] = material { texture { T_01 } interior { I_Glass3 fade_color Col_Blue_01 } }
  #declare MArr[16] = material { texture { T_02 } interior { I_Glass3 fade_color Col_Blue_01 } }
  #declare MArr[17] = material { texture { T_03 } interior { I_Glass3 fade_color Col_Blue_01 } }
  #declare MArr[18] = material { texture { T_04 } interior { I_Glass3 fade_color Col_Blue_01 } }
  #declare MArr[19] = material { texture { T_05 } interior { I_Glass3 fade_color Col_Blue_01 } }

  #declare MArr[20] = material { texture { T_01 } interior { I_Glass4 fade_color Col_Blue_01 } }
  #declare MArr[21] = material { texture { T_02 } interior { I_Glass4 fade_color Col_Blue_01 } }
  #declare MArr[22] = material { texture { T_03 } interior { I_Glass4 fade_color Col_Blue_01 } }
  #declare MArr[23] = material { texture { T_04 } interior { I_Glass4 fade_color Col_Blue_01 } }
  #declare MArr[24] = material { texture { T_05 } interior { I_Glass4 fade_color Col_Blue_01 } }
#end

#if ((frame_number=2) | (frame_number=4))
  #declare MArr[0]  = material { texture { T_01 } interior { I_Glass fade_color Col_Blue_01 } }
  #declare MArr[1]  = material { texture { T_02 } interior { I_Glass fade_color Col_Blue_01 } }
  #declare MArr[2]  = material { texture { T_03 } interior { I_Glass fade_color Col_Blue_01 } }
  #declare MArr[3]  = material { texture { T_04 } interior { I_Glass fade_color Col_Blue_01 } }
  #declare MArr[4]  = material { texture { T_05 } interior { I_Glass fade_color Col_Blue_01 } }

  #declare MArr[5]  = material { texture { T_01 } interior { I_Glass_Dispersion1 fade_color Col_Blue_01 } }
  #declare MArr[6]  = material { texture { T_02 } interior { I_Glass_Dispersion1 fade_color Col_Blue_01 } }
  #declare MArr[7]  = material { texture { T_03 } interior { I_Glass_Dispersion1 fade_color Col_Blue_01 } }
  #declare MArr[8]  = material { texture { T_04 } interior { I_Glass_Dispersion1 fade_color Col_Blue_01 } }
  #declare MArr[9]  = material { texture { T_05 } interior { I_Glass_Dispersion1 fade_color Col_Blue_01 } }

  #declare MArr[10] = material { texture { T_01 } interior { I_Glass_Dispersion2 fade_color Col_Blue_01 } }
  #declare MArr[11] = material { texture { T_02 } interior { I_Glass_Dispersion2 fade_color Col_Blue_01 } }
  #declare MArr[12] = material { texture { T_03 } interior { I_Glass_Dispersion2 fade_color Col_Blue_01 } }
  #declare MArr[13] = material { texture { T_04 } interior { I_Glass_Dispersion2 fade_color Col_Blue_01 } }
  #declare MArr[14] = material { texture { T_05 } interior { I_Glass_Dispersion2 fade_color Col_Blue_01 } }

  #declare MArr[15] = material { texture { T_01 } interior { I_Glass_Caustics1 fade_color Col_Blue_01 } }
  #declare MArr[16] = material { texture { T_02 } interior { I_Glass_Caustics1 fade_color Col_Blue_01 } }
  #declare MArr[17] = material { texture { T_03 } interior { I_Glass_Caustics1 fade_color Col_Blue_01 } }
  #declare MArr[18] = material { texture { T_04 } interior { I_Glass_Caustics1 fade_color Col_Blue_01 } }
  #declare MArr[19] = material { texture { T_05 } interior { I_Glass_Caustics1 fade_color Col_Blue_01 } }

  #declare MArr[20] = material { texture { T_01 } interior { I_Glass_Caustics2 fade_color Col_Blue_01 } }
  #declare MArr[21] = material { texture { T_02 } interior { I_Glass_Caustics2 fade_color Col_Blue_01 } }
  #declare MArr[22] = material { texture { T_03 } interior { I_Glass_Caustics2 fade_color Col_Blue_01 } }
  #declare MArr[23] = material { texture { T_04 } interior { I_Glass_Caustics2 fade_color Col_Blue_01 } }
  #declare MArr[24] = material { texture { T_05 } interior { I_Glass_Caustics2 fade_color Col_Blue_01 } }
#end

#include "sample.inc"
