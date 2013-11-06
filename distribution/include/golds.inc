// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

#ifndef(Gold_Inc_Temp)
#declare Gold_Inc_Temp=version;
#version 3.5;

#ifdef(View_POV_Include_Stack)
#debug "including golds.inc\n"
#end

/*
              Persistence of Vision Raytracer Version 3.1
                      Gold textures
*/

#declare GoldBase = <1.00, 0.875, 0.575>;  // mine again

// This set shifts the color toward orange by subtracting
// bluegreen from the base color.
#declare CVect1 = GoldBase  - <0.00, 0.20, 0.40>;
#declare CVect2 = GoldBase  - <0.00, 0.15, 0.30>;
#declare CVect3 = GoldBase  - <0.00, 0.10, 0.20>;
#declare CVect4 = GoldBase  - <0.00, 0.05, 0.1>;
#declare CVect5 = GoldBase  - <0.00, 0.00, 0.00>;

// Cast CVect as an rgb vector
#declare P_Gold1    = rgb CVect1;             // back row
#declare P_Gold2    = rgb CVect2;
#declare P_Gold3    = rgb CVect3;
#declare P_Gold4    = rgb CVect4;
#declare P_Gold5    = rgb CVect5;            // front row

// Reflection colors, derived from pigment color, "grayed down" a touch.
#declare R_GoldA =  P_Gold1 * 0.30 + <0.25,0.25,0.25>;        // left column (soft)
#declare R_GoldB =  P_Gold2 * 0.35 + <0.25,0.25,0.25>;
#declare R_GoldC =  P_Gold3 * 0.40 + <0.25,0.25,0.25>;
#declare R_GoldD =  P_Gold4 * 0.45 + <0.25,0.25,0.25>;
#declare R_GoldE =  P_Gold5 * 0.50 + <0.25,0.25,0.25>;       // right column (hard)


// Ambient colors, derived from base color
#declare A_GoldA    =  P_Gold1 * 0.12 + <0.1,0.1,0.1>;      // left column  (soft)
#declare A_GoldB    =  P_Gold2 * 0.10 + <0.1,0.1,0.1>;
#declare A_GoldC    =  P_Gold3 * 0.08 + <0.1,0.1,0.1>;
#declare A_GoldD    =  P_Gold4 * 0.05 + <0.1,0.1,0.1>;
#declare A_GoldE    =  P_Gold5 * 0.02 + <0.1,0.1,0.1>;     // right column (hard)

// Diffuse values
// Calculated as 1 - (ambient+reflective+specular values)
#declare D_GoldA    = 1-(((R_GoldA.red+R_GoldA.green+R_GoldA.blue)/3)
                       + ((A_GoldA.red+A_GoldA.green+A_GoldA.blue)/3));
#declare D_GoldB    = 1-(((R_GoldB.red+R_GoldB.green+R_GoldB.blue)/3)
                       + ((A_GoldB.red+A_GoldB.green+A_GoldB.blue)/3));
#declare D_GoldC    = 1-(((R_GoldC.red+R_GoldC.green+R_GoldC.blue)/3)
                       + ((A_GoldC.red+A_GoldC.green+A_GoldC.blue)/3));
#declare D_GoldD    = 1-(((R_GoldD.red+R_GoldD.green+R_GoldD.blue)/3)
                       + ((A_GoldD.red+A_GoldD.green+A_GoldD.blue)/3));
#declare D_GoldE    = 1-(((R_GoldE.red+R_GoldE.green+R_GoldE.blue)/3)
                       + ((A_GoldE.red+A_GoldE.green+A_GoldE.blue)/3));

#declare D_GoldA = max(D_GoldA, 0);
#declare D_GoldB = max(D_GoldB, 0);
#declare D_GoldC = max(D_GoldC, 0);
#declare D_GoldD = max(D_GoldD, 0);
#declare D_GoldE = max(D_GoldE, 0);


// #debug "\nReflection colors:"
// #debug concat("\nR_GoldA=<", str(R_GoldA.red,   5, 5), "," ,
//                              str(R_GoldA.green, 5, 5),"," ,
//                              str(R_GoldA.blue,  5, 5), ">")
// #debug concat("\nR_GoldB=<", str(R_GoldB.red,   5, 5), "," ,
//                              str(R_GoldB.green, 5, 5),"," ,
//                              str(R_GoldB.blue,  5, 5), ">")
// #debug concat("\nR_GoldC=<", str(R_GoldC.red,   5, 5), "," ,
//                              str(R_GoldC.green, 5, 5),"," ,
//                              str(R_GoldC.blue,  5, 5), ">")
// #debug concat("\nR_GoldD=<", str(R_GoldD.red,   5, 5), "," ,
//                              str(R_GoldD.green, 5, 5),"," ,
//                              str(R_GoldD.blue,  5, 5), ">")
// #debug concat("\nR_GoldE=<", str(R_GoldE.red,   5, 5), "," ,
//                              str(R_GoldE.green, 5, 5),"," ,
//                              str(R_GoldE.blue,  5, 5), ">")
//
// #debug "\n\nAmbient colors:"
// #debug concat("\nA_GoldA=<", str(A_GoldA.red,   5, 5), "," ,
//                              str(A_GoldA.green, 5, 5),"," ,
//                              str(A_GoldA.blue,  5, 5), ">")
// #debug concat("\nA_GoldB=<", str(A_GoldB.red,   5, 5), "," ,
//                              str(A_GoldB.green, 5, 5),"," ,
//                              str(A_GoldB.blue,  5, 5), ">")
// #debug concat("\nA_GoldC=<", str(A_GoldC.red,   5, 5), "," ,
//                              str(A_GoldC.green, 5, 5),"," ,
//                              str(A_GoldC.blue,  5, 5), ">")
// #debug concat("\nA_GoldD=<", str(A_GoldD.red,   5, 5), "," ,
//                              str(A_GoldD.green, 5, 5),"," ,
//                              str(A_GoldD.blue,  5, 5), ">")
// #debug concat("\nA_GoldE=<", str(A_GoldE.red,   5, 5), "," ,
//                              str(A_GoldE.green, 5, 5),"," ,
//                              str(A_GoldE.blue,  5, 5), ">")
//
//
// #debug "\n\nDiffuse values:"
// #debug concat("\nD_GoldA = ",str(D_GoldA, 5, 5))
// #debug concat("\nD_GoldB = ",str(D_GoldB, 5, 5))
// #debug concat("\nD_GoldC = ",str(D_GoldC, 5, 5))
// #debug concat("\nD_GoldD = ",str(D_GoldD, 5, 5))
// #debug concat("\nD_GoldE = ",str(D_GoldE, 5, 5))
// #debug "\n"

#declare M = 1;    //   Metallic amount

//*****Finishes****
// Left column, dullest & darkest
#declare F_MetalA  =
finish {
    brilliance 2
    diffuse D_GoldA
    ambient A_GoldA
    reflection R_GoldA
    metallic M
    specular 0.20
    roughness 1/20
}

#declare F_MetalB  =
finish {
    brilliance 3
    diffuse D_GoldB
    ambient A_GoldB
    reflection R_GoldB
    metallic M
    specular 0.30
    roughness 1/60
}

#declare F_MetalC  =
finish {
    brilliance 4
    diffuse D_GoldC
    ambient A_GoldC
    reflection R_GoldC
    metallic M
    specular 0.60
    roughness 1/80
}

#declare F_MetalD  =
finish {
    brilliance 5
    diffuse D_GoldD
    ambient A_GoldD
    reflection R_GoldD
    metallic M
    specular 0.70
    roughness 1/100
}

#declare F_MetalE  =
finish {
    brilliance 6
    diffuse D_GoldE
    ambient A_GoldE
    reflection R_GoldE
    metallic M
    specular 0.80
    roughness 1/120
}

//                              GOLDS
#declare T_Gold_1A = texture { pigment { P_Gold1 } finish { F_MetalA } }
#declare T_Gold_1B = texture { pigment { P_Gold1 } finish { F_MetalB } }
#declare T_Gold_1C = texture { pigment { P_Gold1 } finish { F_MetalC } }
#declare T_Gold_1D = texture { pigment { P_Gold1 } finish { F_MetalD } }
#declare T_Gold_1E = texture { pigment { P_Gold1 } finish { F_MetalE } }

#declare T_Gold_2A = texture { pigment { P_Gold2 } finish { F_MetalA } }
#declare T_Gold_2B = texture { pigment { P_Gold2 } finish { F_MetalB } }
#declare T_Gold_2C = texture { pigment { P_Gold2 } finish { F_MetalC } }
#declare T_Gold_2D = texture { pigment { P_Gold2 } finish { F_MetalD } }
#declare T_Gold_2E = texture { pigment { P_Gold2 } finish { F_MetalE } }

#declare T_Gold_3A = texture { pigment { P_Gold3 } finish { F_MetalA } }
#declare T_Gold_3B = texture { pigment { P_Gold3 } finish { F_MetalB } }
#declare T_Gold_3C = texture { pigment { P_Gold3 } finish { F_MetalC } }
#declare T_Gold_3D = texture { pigment { P_Gold3 } finish { F_MetalD } }
#declare T_Gold_3E = texture { pigment { P_Gold3 } finish { F_MetalE } }

#declare T_Gold_4A = texture { pigment { P_Gold4 } finish { F_MetalA } }
#declare T_Gold_4B = texture { pigment { P_Gold4 } finish { F_MetalB } }
#declare T_Gold_4C = texture { pigment { P_Gold4 } finish { F_MetalC } }
#declare T_Gold_4D = texture { pigment { P_Gold4 } finish { F_MetalD } }
#declare T_Gold_4E = texture { pigment { P_Gold4 } finish { F_MetalE } }

#declare T_Gold_5A = texture { pigment { P_Gold5 } finish { F_MetalA } }
#declare T_Gold_5B = texture { pigment { P_Gold5 } finish { F_MetalB } }
#declare T_Gold_5C = texture { pigment { P_Gold5 } finish { F_MetalC } }
#declare T_Gold_5D = texture { pigment { P_Gold5 } finish { F_MetalD } }
#declare T_Gold_5E = texture { pigment { P_Gold5 } finish { F_MetalE } }

//The following #declares are needed for stage_xz.pov

#declare T01 = texture { T_Gold_1A }
#declare T02 = texture { T_Gold_1B }
#declare T03 = texture { T_Gold_1C }
#declare T04 = texture { T_Gold_1D }
#declare T05 = texture { T_Gold_1E }

#declare T06 = texture { T_Gold_2A }
#declare T07 = texture { T_Gold_2B }
#declare T08 = texture { T_Gold_2C }
#declare T09 = texture { T_Gold_2D }
#declare T10 = texture { T_Gold_2E }

#declare T11 = texture { T_Gold_3A }
#declare T12 = texture { T_Gold_3B }
#declare T13 = texture { T_Gold_3C }
#declare T14 = texture { T_Gold_3D }
#declare T15 = texture { T_Gold_3E }

#declare T16 = texture { T_Gold_4A }
#declare T17 = texture { T_Gold_4B }
#declare T18 = texture { T_Gold_4C }
#declare T19 = texture { T_Gold_4D }
#declare T20 = texture { T_Gold_4E }

#declare T21 = texture { T_Gold_5A }
#declare T22 = texture { T_Gold_5B }
#declare T23 = texture { T_Gold_5C }
#declare T24 = texture { T_Gold_5D }
#declare T25 = texture { T_Gold_5E }

#version Gold_Inc_Temp;
#end
