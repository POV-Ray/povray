// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.
//
// Persistence Of Vision Ray Tracer ('POV-Ray') sample file.
//
// Crackle pattern example crackle2.pov.
//
// Date: April 2001
// Auth: Christoph Hormann
// Updated: January 2017
//
// -w512 -h256 +a0.3

#version 3.7;
global_settings { assumed_gamma 1.0 }
#default { finish { ambient 0.006 diffuse 0.456 } }

light_source {
    <1.6,1.9,2.7>*10000
    color rgb 1.3
}

//-------------------------------------------------------------------------
// This scene uses a non-standard camera set-up.
// (See CAMERA in the included documentation for details.)
// If you are new to POV-Ray, you might want to try a different demo scene.
//-------------------------------------------------------------------------
camera {
    location  <7,24,12>
    direction y
    sky       z
    up        z
    right     2.5*x
    look_at   <0,0,0.3>
    angle     30
}

#declare SkyColor = srgb <0.60,0.70,0.95>;
sphere {
    <0,0,0>, 1
    texture {
        pigment { SkyColor }
        finish {
            ambient  0
            diffuse  0
            emission 1
        }
    }
    scale 100000
    no_shadow
    hollow on
}

#declare FloorColorA = srgb <0.63137,0.60392,0.56078>;
#declare FloorColorB = srgb <0.80392,0.73725,0.73725>;
#declare FloorColorC = srgb <0.87843,0.81176,0.81176>;
#declare FloorColorD = srgb <0.92549,0.89804,0.87843>;
plane {
    z, 0
    texture {
        pigment {
            crackle
            color_map {
                [ 0.0 FloorColorA ]
                [ 0.1 FloorColorB ]
                [ 0.4 FloorColorC ]
                [ 1.0 FloorColorD ]
            }
            warp { turbulence 0.6 }
        }
        finish {
            diffuse 0.6
            ambient 0.006
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
      #local T_CrackColor00 = srgb <0.52157,0.40000,0.72549>;
      #local T_CrackColor01 = srgb <0.83137,0.66667,0.92157>;
      #local T_CrackColor02 = srgb <0.83137,0.66667,0.92157>;
      #local T_CrackColor03 = srgb <0.87059,0.66667,0.92941>;
      #local T_CrackColor04 = srgb <0.95686,0.75294,0.95294>;
      #local T_CrackColor05 = srgb <1.00000,0.84314,0.93333>;
      #local T_CrackColor06 = srgb <0.98823,0.85490,0.79216>;
      #local T_CrackColor07 = srgb <0.97647,0.86667,0.67059>;
      #local T_CrackColor08 = srgb <0.98823,0.86667,0.60784>;
      #local T_CrackColor09 = srgb <0.58431,0.48628,0.90588>;
      #local T_Crack = texture {
          pigment {
              crackle metric Metr
              color_map {
                  [ 0.0000 T_CrackColor00 ]
                  [ 0.1000 T_CrackColor01 ]
                  [ 0.2000 T_CrackColor02 ]
                  [ 0.3000 T_CrackColor03 ]
                  [ 0.7000 T_CrackColor04 ]
                  [ 0.8000 T_CrackColor05 ]
                  [ 0.9000 T_CrackColor06 ]
                  [ 0.9300 T_CrackColor07 ]
                  [ 0.9800 T_CrackColor08 ]
                  [ 1.0000 T_CrackColor09 ]
              }
              scale 0.7
          }
          finish {
              ambient 0.006
              diffuse 0.7
              brilliance 1.3
              specular 0.3
          }
      }

      superellipsoid {
          <0.1, 0.1>
          scale <1.0,1.0,0.6>
          texture { T_Crack }
          translate <0.0,3.0,0.3>
      }

      #local NormalPigmColor = srgb <0.48628,0.90588,0.73725>;
      superellipsoid {
          <0.1, 0.1>
          scale <1.0, 1.0, 0.6>
          texture {
              pigment { NormalPigmColor }
              finish {
                  specular 0.5
                  roughness 0.04
                  diffuse 0.8
                  brilliance 2.0
                  ambient 0.006
              }
              normal {
                  crackle 0.7 metric Metr
                  scale 0.7
              }
          }
          translate <0.0,-3.0,0.3>
      }

      #if (Metr != 1)
          #local IsoPigmColor = srgb <0.58431,0.48628,0.97647>;
          #local fn_Crack = function {
              pattern { crackle metric Metr scale 0.7 }
          }
          isosurface {
              function { z-fn_Crack(x,y,0.6)*0.35 }
              max_gradient 1.5
              accuracy 0.001
              contained_by { box { <-1.0,-1.0,-0.6>,<1.0,1.0,1.1> } }
              texture {
                  pigment { IsoPigmColor }
                  finish {
                      specular 0.2
                      roughness 0.04
                      diffuse 0.8
                      brilliance 2.0
                      ambient 0.006
                  }
              }
              translate 0.6*z
          }
      #end

      #local Red = srgb <1,0,0>;
      text {
          ttf "crystal.ttf", concat("metric ",str(Metr,0,1)), 0.1, 0
          rotate 90*x
          rotate 180*z
          scale 0.3
          translate <0.85,4.01,0.5>
          texture { pigment { color Red*0.8 } }
      }
      translate -1*y
  }
#end

object { Objs(1)   translate -5.0*x }
object { Objs(1.5) translate -2.5*x }
object { Objs(2)   translate  0.0*x }
object { Objs(2.2) translate  2.5*x }
object { Objs(3)   translate  5.0*x }

