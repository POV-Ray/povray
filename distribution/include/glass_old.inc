// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Ray Tracer Include File
//    File: glass_old.inc
//    Last updated: 2001.8.9
//    Description: Glass finishes and textures (was glass.inc in versions prior to 3.5).
//    Changes in version 3.1: moved refraction and ior to the interior statment.
//    Use I_Glass in conjunction with each of the finish statments below.
//    Version 3.5: Renamed file as glass_old.inc.
//
//    Updated: 27Sep2008(jh)
//
//    To suppress warnings generated from using these deprecated textures you should consider
//    converting them to materials.
//    
//    In this example texture {T_Glass4} interior {I_Glass caustics 1} becomes
//
//    material {
//      texture {
//        pigment { color rgbf <0.98, 1.0, 0.99, 0.75> }
//        finish { F_Glass4 }
//        }
//      interior { I_Glass caustics 1 }
//      } 

#ifndef(Glass_Old_Inc_Temp)
#declare Glass_Old_Inc_Temp=version;
#version 3.5;

#ifdef(View_POV_Include_Stack)
    #debug "including glass_old.inc\n"
#end

// Glass Interior
#declare I_Glass =
interior {
    ior 1.5
}
  
// Glass Finishes
#declare F_Glass1 =
finish {
    specular 1
    roughness 0.001
    ambient 0
    diffuse 0
    reflection 0.1
}   

#declare F_Glass2 = 
finish {
    ambient 0
    diffuse 0
    reflection 0.5
    phong 0.3
    phong_size 60
}

#declare F_Glass3 =
finish  {
    ambient 0.1
    diffuse 0.1
    reflection 0.1
    specular 0.8
    roughness 0.003
    phong 1
    phong_size 400
}

#declare F_Glass4 =
finish {
    ambient 0.1
    diffuse 0.1
    reflection .25
    specular 1
    roughness 0.001
}

// Glass Textures
// Simple clear glass
#declare deprecated once "See notes in glass_old.inc regarding use of this deprecated texture: T_Glass1" T_Glass1 =
texture {
    pigment { color rgbf<1.0, 1.0, 1.0, 0.7> }
    finish  { F_Glass1 }
}

// More like an acrylic plastic
#declare deprecated once "See notes in glass_old.inc regarding use of this deprecated texture: T_Glass2" T_Glass2 =
texture {
    pigment { color rgbf<1.0, 1.0, 1.0, 1.0> }
    finish  { F_Glass2 }
}

// An excellent lead crystal glass!
#declare deprecated once "See notes in glass_old.inc regarding use of this deprecated texture: T_Glass3" T_Glass3 =
texture {
    pigment { color rgbf <0.98, 0.98, 0.98, 0.9> }
    finish { F_Glass3 }
}

#declare deprecated once "See notes in glass_old.inc regarding use of this deprecated texture: T_Glass4" T_Glass4 =
texture {
    pigment { color rgbf <0.98, 1.0, 0.99, 0.75> }
    finish { F_Glass4 }
}

#declare deprecated once "See notes in glass_old.inc regarding use of this deprecated texture: T_Old_Glass" T_Old_Glass =
texture {
    pigment { color rgbf <0.8, 0.9, 0.85, 0.85> }
    finish { F_Glass4 }
}

#declare deprecated once "See notes in glass_old.inc regarding use of this deprecated texture: T_Winebottle_Glass" T_Winebottle_Glass =
texture {
    pigment { color rgbf <0.4, 0.72, 0.4, 0.6> }
    finish { F_Glass4 }
}

#declare deprecated once "See notes in glass_old.inc regarding use of this deprecated texture: T_Beerbottle_Glass" T_Beerbottle_Glass =
texture {
    pigment { color rgbf <0.7, 0.5, 0.1, 0.6> }
    finish { F_Glass4 }
}

// A few color variations on Norm's glass
// Ruby glass
#declare deprecated once "See notes in glass_old.inc regarding use of this deprecated texture: T_Ruby_Glass" T_Ruby_Glass =
texture {
    pigment { color rgbf <0.9, 0.1, 0.2, 0.8> }
    finish { F_Glass4 }
}

#declare deprecated once "See notes in glass_old.inc regarding use of this deprecated texture: T_Green_Glass" T_Green_Glass =
texture {
    pigment { color rgbf <0.8, 1, 0.95, 0.9> }
    finish { F_Glass3 }
}

#declare deprecated once "See notes in glass_old.inc regarding use of this deprecated texture: T_Dark_Green_Glass" T_Dark_Green_Glass =
texture {
    pigment { color rgbf <0.1, 0.7, 0.8, 0.8> }
    finish { F_Glass4 }
}

#declare deprecated once "See notes in glass_old.inc regarding use of this deprecated texture: T_Yellow_Glass" T_Yellow_Glass =
texture {
    pigment { color rgbf <0.8, 0.8, 0.2, 0.8> }
    finish { F_Glass4 }
}

// Orange/Amber glass
#declare deprecated once "See notes in glass_old.inc regarding use of this deprecated texture: T_Orange_Glass" T_Orange_Glass =
texture {
    pigment { rgbf <1.0, 0.5, 0.0, 0.8> }
    finish { F_Glass4 }
}

// Vicks bottle glass
#declare deprecated once "See notes in glass_old.inc regarding use of this deprecated texture: T_Vicksbottle_Glass" T_Vicksbottle_Glass =
texture {
    pigment { color rgbf <0.1, 0.15, 0.5, 0.9> }
    finish { F_Glass4 }
}

#version Glass_Old_Inc_Temp;
#end
