// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

#ifndef(Metals_Inc_Temp)
#declare Metals_Inc_Temp=version;
#version 3.5;

#ifdef(View_POV_Include_Stack)
#   debug "including metals.inc\n"
#end

/*
              Persistence of Vision Raytracer Version 3.1
               Metallic pigments, finishes, and textures.


*****************************************************************************
                           BASIC METAL COLORS
*****************************************************************************
 Note: Describing metallic textures is difficult, at best.  The reflectivity
 and other qualities make them highly dependant on lighting and surroundings.

 The following notes are intended as a rough guide only.
 P_Brass1: Dark brown bronze.
 P_Brass2: Somewhat lighter brown than Brass4. Old penny, in soft finishes.
 P_Brass3: Used by Steve Anger's Polished_Brass. Slightly coppery.
 P_Brass4: A little yellower than Brass1.
 P_Brass5: Very light bronze, ranges from med tan to almost white.

 P_Copper1: bronze-like.  Best in finish #C.
 P_Copper2: slightly brownish copper/bronze.  Best in finishes #B-#D.
 P_Copper3: reddish-brown copper.  Best in finishes #C-#E.
 P_Copper4: pink copper, like new tubing.  Best in finishes #C-#E.
 P_Copper5: Bronze in softer finishes, gold in harder finishes.

 P_Chrome1: 20% Gray. Used in Steve Anger's Polished_Chrome.
 P_Chrome2: Slightly blueish 60% gray. Good steel w/finish #A
 P_Chrome3: 50% neutral gray.
 P_Chrome4: 75% neutral gray.
 P_Chrome5: 95% neutral gray.

 P_Silver1: Yellowish silverplate.  Somewhat tarnished looking.
 P_Silver2: Not quite as yellowish as Silver1 but more so than Silver3.
 P_Silver3: Reasonably neutral silver.
 P_Silver4: REDUNDANT (unused)
 P_Silver5: REDUNDANT (unused)

*/

#include "golds.inc"

#declare P_Brass1    = color rgb <0.30, 0.20, 0.10>;
#declare P_Brass2    = color rgb <0.50, 0.35, 0.25>;
#declare P_Brass3    = color rgb <0.58, 0.42, 0.20>;
#declare P_Brass4    = color rgb <0.65, 0.50, 0.25>;
#declare P_Brass5    = color rgb <0.70, 0.55, 0.40>;

#declare P_Copper1   = color rgb <0.40, 0.20, 0.15>;
#declare P_Copper2   = color rgb <0.50, 0.25, 0.15>;
#declare P_Copper3   = color rgb <0.60, 0.30, 0.15>;
#declare P_Copper4   = color rgb <0.70, 0.25, 0.15>;
#declare P_Copper5   = color rgb <0.65, 0.35, 0.15>;

#declare P_Chrome1   = color rgb <0.20, 0.20, 0.20>;
#declare P_Chrome2   = color rgb <0.39, 0.41, 0.43>;
#declare P_Chrome3   = color rgb <0.50, 0.50, 0.50>;
#declare P_Chrome4   = color rgb <0.75, 0.75, 0.75>;
#declare P_Chrome5   = color rgb <0.95, 0.95, 0.95>;

#declare P_Silver1   = color rgb <0.94, 0.93, 0.80>;
#declare P_Silver2   = color rgb <0.94, 0.93, 0.85>;
#declare P_Silver3   = color rgb <0.94, 0.93, 0.90>;
#declare P_Silver4   = color rgb <0.95, 0.91, 0.91>;             
#declare P_Silver5   = color rgb <0.91, 0.95, 0.91>;   

//*****************************************************************************
//                           BASIC METAL FINISHES
//*****************************************************************************

// F_MetalA  :  Very soft and dull.  
// F_MetalB  :  Fairly soft and dull.
// F_MetalC  :  Medium reflectivity. Holds color well.
// F_MetalD  :  Highly hard and polished. High reflectivity.   
// F_MetalE  :  Very highly polished & reflective.

#declare F_MetalA  =
finish {
    ambient 0.35
    brilliance 2
    diffuse 0.3
    metallic
    specular 0.80
    roughness 1/20
    reflection 0.1
}

#declare F_MetalB  = 
finish {
    ambient 0.30
    brilliance 3
    diffuse 0.4
    metallic
    specular 0.70
    roughness 1/60
    reflection 0.25
}

#declare F_MetalC  =
finish {
    ambient 0.25
    brilliance 4
    diffuse 0.5
    metallic
    specular 0.80
    roughness 1/80
    reflection 0.5
}

#declare F_MetalD  =
finish {
    ambient 0.15
    brilliance 5
    diffuse 0.6
    metallic
    specular 0.80
    roughness 1/100
    reflection 0.65
}

#declare F_MetalE  =
finish {
    ambient 0.1
    brilliance 6
    diffuse 0.7
    metallic
    specular 0.80
    roughness 1/120
    reflection 0.8
}

//*****************************************************************************
//                           METAL TEXTURES
//*****************************************************************************

//                              BRASSES
#declare T_Brass_1A = texture { pigment { P_Brass1 } finish { F_MetalA  } }
#declare T_Brass_1B = texture { pigment { P_Brass1 } finish { F_MetalB  } }
#declare T_Brass_1C = texture { pigment { P_Brass1 } finish { F_MetalC  } }
#declare T_Brass_1D = texture { pigment { P_Brass1 } finish { F_MetalD  } }
#declare T_Brass_1E = texture { pigment { P_Brass1 } finish { F_MetalE  } }

#declare T_Brass_2A = texture { pigment { P_Brass2 } finish { F_MetalA  } }
#declare T_Brass_2B = texture { pigment { P_Brass2 } finish { F_MetalB  } }
#declare T_Brass_2C = texture { pigment { P_Brass2 } finish { F_MetalC  } }
#declare T_Brass_2D = texture { pigment { P_Brass2 } finish { F_MetalD  } }
#declare T_Brass_2E = texture { pigment { P_Brass2 } finish { F_MetalE  } }

#declare T_Brass_3A = texture { pigment { P_Brass3 } finish { F_MetalA  } }
#declare T_Brass_3B = texture { pigment { P_Brass3 } finish { F_MetalB  } }
#declare T_Brass_3C = texture { pigment { P_Brass3 } finish { F_MetalC  } }
#declare T_Brass_3D = texture { pigment { P_Brass3 } finish { F_MetalD  } }
#declare T_Brass_3E = texture { pigment { P_Brass3 } finish { F_MetalE  } }

#declare T_Brass_4A = texture { pigment { P_Brass4 } finish { F_MetalA  } }
#declare T_Brass_4B = texture { pigment { P_Brass4 } finish { F_MetalB  } }
#declare T_Brass_4C = texture { pigment { P_Brass4 } finish { F_MetalC  } }
#declare T_Brass_4D = texture { pigment { P_Brass4 } finish { F_MetalD  } }
#declare T_Brass_4E = texture { pigment { P_Brass4 } finish { F_MetalE  } }

#declare T_Brass_5A = texture { pigment { P_Brass5 } finish { F_MetalA  } }
#declare T_Brass_5B = texture { pigment { P_Brass5 } finish { F_MetalB  } }
#declare T_Brass_5C = texture { pigment { P_Brass5 } finish { F_MetalC  } }
#declare T_Brass_5D = texture { pigment { P_Brass5 } finish { F_MetalD  } }
#declare T_Brass_5E = texture { pigment { P_Brass5 } finish { F_MetalE  } }

//                           COPPERS & BRONZES
#declare T_Copper_1A = texture { pigment { P_Copper1 } finish { F_MetalA  } }
#declare T_Copper_1B = texture { pigment { P_Copper1 } finish { F_MetalB  } }
#declare T_Copper_1C = texture { pigment { P_Copper1 } finish { F_MetalC  } }
#declare T_Copper_1D = texture { pigment { P_Copper1 } finish { F_MetalD  } }
#declare T_Copper_1E = texture { pigment { P_Copper1 } finish { F_MetalE  } }
                                           
#declare T_Copper_2A = texture { pigment { P_Copper2 } finish { F_MetalA  } }
#declare T_Copper_2B = texture { pigment { P_Copper2 } finish { F_MetalB  } }
#declare T_Copper_2C = texture { pigment { P_Copper2 } finish { F_MetalC  } }
#declare T_Copper_2D = texture { pigment { P_Copper2 } finish { F_MetalD  } }
#declare T_Copper_2E = texture { pigment { P_Copper2 } finish { F_MetalE  } }
                                           
#declare T_Copper_3A = texture { pigment { P_Copper3 } finish { F_MetalA  } }
#declare T_Copper_3B = texture { pigment { P_Copper3 } finish { F_MetalB  } }
#declare T_Copper_3C = texture { pigment { P_Copper3 } finish { F_MetalC  } }
#declare T_Copper_3D = texture { pigment { P_Copper3 } finish { F_MetalD  } }
#declare T_Copper_3E = texture { pigment { P_Copper3 } finish { F_MetalE  } }
                                           
#declare T_Copper_4A = texture { pigment { P_Copper4 } finish { F_MetalA  } }
#declare T_Copper_4B = texture { pigment { P_Copper4 } finish { F_MetalB  } }
#declare T_Copper_4C = texture { pigment { P_Copper4 } finish { F_MetalC  } }
#declare T_Copper_4D = texture { pigment { P_Copper4 } finish { F_MetalD  } }
#declare T_Copper_4E = texture { pigment { P_Copper4 } finish { F_MetalE  } }
                                           
#declare T_Copper_5A = texture { pigment { P_Copper5 } finish { F_MetalA  } }
#declare T_Copper_5B = texture { pigment { P_Copper5 } finish { F_MetalB  } }
#declare T_Copper_5C = texture { pigment { P_Copper5 } finish { F_MetalC  } }
#declare T_Copper_5D = texture { pigment { P_Copper5 } finish { F_MetalD  } }
#declare T_Copper_5E = texture { pigment { P_Copper5 } finish { F_MetalE  } }

//                           CHROMES & STEELS  
#declare T_Chrome_1A = texture { pigment { P_Chrome1 } finish { F_MetalA  } }
#declare T_Chrome_1B = texture { pigment { P_Chrome1 } finish { F_MetalB  } }
#declare T_Chrome_1C = texture { pigment { P_Chrome1 } finish { F_MetalC  } }
#declare T_Chrome_1D = texture { pigment { P_Chrome1 } finish { F_MetalD  } }
#declare T_Chrome_1E = texture { pigment { P_Chrome1 } finish { F_MetalE  } }
                                           
#declare T_Chrome_2A = texture { pigment { P_Chrome2 } finish { F_MetalA  } }
#declare T_Chrome_2B = texture { pigment { P_Chrome2 } finish { F_MetalB  } }
#declare T_Chrome_2C = texture { pigment { P_Chrome2 } finish { F_MetalC  } }
#declare T_Chrome_2D = texture { pigment { P_Chrome2 } finish { F_MetalD  } }
#declare T_Chrome_2E = texture { pigment { P_Chrome2 } finish { F_MetalE  } }
                                           
#declare T_Chrome_3A = texture { pigment { P_Chrome3 } finish { F_MetalA  } }
#declare T_Chrome_3B = texture { pigment { P_Chrome3 } finish { F_MetalB  } }
#declare T_Chrome_3C = texture { pigment { P_Chrome3 } finish { F_MetalC  } }
#declare T_Chrome_3D = texture { pigment { P_Chrome3 } finish { F_MetalD  } }
#declare T_Chrome_3E = texture { pigment { P_Chrome3 } finish { F_MetalE  } }
                                           
#declare T_Chrome_4A = texture { pigment { P_Chrome4 } finish { F_MetalA  } }
#declare T_Chrome_4B = texture { pigment { P_Chrome4 } finish { F_MetalB  } }
#declare T_Chrome_4C = texture { pigment { P_Chrome4 } finish { F_MetalC  } }
#declare T_Chrome_4D = texture { pigment { P_Chrome4 } finish { F_MetalD  } }
#declare T_Chrome_4E = texture { pigment { P_Chrome4 } finish { F_MetalE  } }
                                           
#declare T_Chrome_5A = texture { pigment { P_Chrome5 } finish { F_MetalA  } }
#declare T_Chrome_5B = texture { pigment { P_Chrome5 } finish { F_MetalB  } }
#declare T_Chrome_5C = texture { pigment { P_Chrome5 } finish { F_MetalC  } }
#declare T_Chrome_5D = texture { pigment { P_Chrome5 } finish { F_MetalD  } }
#declare T_Chrome_5E = texture { pigment { P_Chrome5 } finish { F_MetalE  } }

//                               SILVERS        
#declare T_Silver_1A = texture { pigment { P_Silver1 } finish { F_MetalA  } }
#declare T_Silver_1B = texture { pigment { P_Silver1 } finish { F_MetalB  } }
#declare T_Silver_1C = texture { pigment { P_Silver1 } finish { F_MetalC  } }
#declare T_Silver_1D = texture { pigment { P_Silver1 } finish { F_MetalD  } }
#declare T_Silver_1E = texture { pigment { P_Silver1 } finish { F_MetalE  } }
                                           
#declare T_Silver_2A = texture { pigment { P_Silver2 } finish { F_MetalA  } }
#declare T_Silver_2B = texture { pigment { P_Silver2 } finish { F_MetalB  } }
#declare T_Silver_2C = texture { pigment { P_Silver2 } finish { F_MetalC  } }
#declare T_Silver_2D = texture { pigment { P_Silver2 } finish { F_MetalD  } }
#declare T_Silver_2E = texture { pigment { P_Silver2 } finish { F_MetalE  } }
                                           
#declare T_Silver_3A = texture { pigment { P_Silver3 } finish { F_MetalA  } }
#declare T_Silver_3B = texture { pigment { P_Silver3 } finish { F_MetalB  } }
#declare T_Silver_3C = texture { pigment { P_Silver3 } finish { F_MetalC  } }
#declare T_Silver_3D = texture { pigment { P_Silver3 } finish { F_MetalD  } }
#declare T_Silver_3E = texture { pigment { P_Silver3 } finish { F_MetalE  } }
                                           
#declare T_Silver_4A = texture { pigment { P_Silver4 } finish { F_MetalA  } }
#declare T_Silver_4B = texture { pigment { P_Silver4 } finish { F_MetalB  } }
#declare T_Silver_4C = texture { pigment { P_Silver4 } finish { F_MetalC  } }
#declare T_Silver_4D = texture { pigment { P_Silver4 } finish { F_MetalD  } }
#declare T_Silver_4E = texture { pigment { P_Silver4 } finish { F_MetalE  } }
                                           
#declare T_Silver_5A = texture { pigment { P_Silver5 } finish { F_MetalA  } }
#declare T_Silver_5B = texture { pigment { P_Silver5 } finish { F_MetalB  } }
#declare T_Silver_5C = texture { pigment { P_Silver5 } finish { F_MetalC  } }
#declare T_Silver_5D = texture { pigment { P_Silver5 } finish { F_MetalD  } }
#declare T_Silver_5E = texture { pigment { P_Silver5 } finish { F_MetalE  } }


#version Metals_Inc_Temp;
#end
