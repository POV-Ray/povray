// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Ray Tracer Scene Description File
// File: crackle2.pov
// Desc: crackle pattern demonstration scene
// Date: April 2001
// Auth: Christoph Hormann

// -w400 -h200
// -w512 -h256 +a0.3

#version 3.7;

global_settings { assumed_gamma 1.0 }

#include "colors.inc"

light_source {
   <1.6, 1.9, 2.7>*10000
   rgb 1.3
}

//-------------------------------------------------------------------------
// This scene uses a non-standard camera set-up. 
// (See CAMERA in the included documentation for details.) 
// If you are new to POV-Ray, you might want to try a different demo scene.
//-------------------------------------------------------------------------
camera {
   location    <7, 24, 12>
   direction   y
   sky         z
   up          z
   right       2.5*x
   look_at     <0.0, 0, 0.3>
   angle       30
}


sphere {
   <0, 0, 0>, 1
   texture {
      pigment {
         color rgb < 0.60, 0.70, 0.95 >
      }
      finish {
         diffuse 0
         ambient 1
      }
   }
   scale 100000
   no_shadow
   hollow on
}

plane {
   z, 0
   
   texture {
      pigment {
         crackle
         color_map {
            [ 0.0 color rgb<0.356, 0.321, 0.274> ]
            [ 0.1 color rgb<0.611, 0.500, 0.500> ]
            [ 0.4 color rgb<0.745, 0.623, 0.623> ]
            [ 1.0 color rgb<0.837, 0.782, 0.745> ]
         }
         warp { turbulence 0.6 }
      }
      finish {
         diffuse 0.6
         ambient 0.1
         specular 0.2
         reflection {
            0.2, 0.6
            fresnel on
         }
         conserve_energy
      }
    
   }
   
}

#macro Objs(Metr)

union {
   #local fn_Crack=
   function {
      pigment {
         crackle
         metric Metr
         color_map { [0 rgb 0][1 rgb 1] }
         scale 0.7
      }
   }
   
   
   #local T_Crack=
   texture {
      pigment {
         crackle
         metric Metr
         color_map {
            [ 0.0000 color rgb<0.2353, 0.1333, 0.4824> ]
            [ 0.1000 color rgb<0.5647, 0.4353, 0.8000> ]
            [ 0.2000 color rgb<0.6549, 0.4000, 0.8275> ]
            [ 0.3000 color rgb<0.7294, 0.4039, 0.8471> ]
            [ 0.7000 color rgb<0.9059, 0.5255, 0.8980> ]
            [ 0.8000 color rgb<1.0000, 0.6784, 0.8549> ]
            [ 0.9000 color rgb<0.9725, 0.6980, 0.5922> ]
            [ 0.9300 color rgb<0.9490, 0.7255, 0.4078> ]
            [ 0.9800 color rgb<0.9725, 0.7255, 0.3294> ]
            [ 1.0000 color rgb<0.3000, 0.2000, 0.8000> ]
         }
         scale 0.7
      }
      finish {
         ambient 0.1
         diffuse 0.7
         brilliance 1.3
         specular 0.3
      }
   }
   
   superellipsoid {
      <0.1, 0.1>
      scale <1.0, 1.0, 0.6>
      texture { T_Crack }
      translate <0.0, 3.0, 0.3>
   }
   
   superellipsoid {
      <0.1, 0.1>
      scale <1.0, 1.0, 0.6>
      
      texture {
         pigment { rgb <0.2, 0.8, 0.5> }
         finish {
            specular 0.5
            roughness 0.04
            diffuse 0.8
            brilliance 2.0
            ambient 0.1
         }
         normal {
            crackle 0.7
            metric Metr
            scale 0.7
         }
      }
      
      translate <0.0, -3.0, 0.3>
   }
   
   #if (Metr != 1)
      isosurface {
         function { z-fn_Crack(x, y, 0.6).red*0.35 }
         //evaluate 1, 1.2, 0.99
         max_gradient 1.5
         accuracy 0.001
         contained_by { box { <-1.0,-1.0,-0.6>,<1.0,1.0,1.1> } }
         
         texture {
            pigment { rgb <0.3, 0.2, 0.95> }
            finish {
               specular 0.5
               roughness 0.04
               diffuse 0.8
               brilliance 2.0
               ambient 0.1
            }
         }
         
         translate 0.6*z
      }
   #end
   
   
   object {
      text
      {
         ttf
         "crystal.ttf",
         concat("metric ", str(Metr,0,1)),
         0.1, 0
         
         rotate 90*x
         rotate 180*z
         scale 0.3
         translate <0.85, 4.01, 0.5>
         
         texture {
            pigment { color Red*0.8 }
         }
      }
   }
   
   translate -1*y
}
#end


object { Objs(1)   translate -5.0*x }
object { Objs(1.5) translate -2.5*x }
object { Objs(2)   translate  0.0*x }
object { Objs(2.2) translate  2.5*x }
object { Objs(3)   translate  5.0*x }

