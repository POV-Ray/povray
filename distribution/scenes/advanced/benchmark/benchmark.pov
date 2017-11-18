// This work is licensed under the POV-Ray v3.8 distribution license.
// To view a copy of this license, visit http://www.povray.org/licences/v3.8/.
//
// The following exception is granted to the above license terms:
// Permission is granted for this file and the output from it to be
// freely redistributed in an unmodified form for the purpose of
// generating and maintaining POV-Ray benchmarks.  Derivative works
// are permitted provided that they have a clearly different filename
// and do not claim to be the standard benchmark file.
//
// ==================================================================
//
// Persistence Of Vision Ray Tracer Scene Description File
//
// File:            benchmark.pov
// Benchmark Vers:  2.03 Scene File Version
// Desc:            POV-Ray benchmark scene
// Date:            October/November 2001 (initial release)
//
// Assembled by Christoph Hormann
// Contributions by:
//    - Ingo Janssen
//    - Mick Hazelgrove
//
// ==================================================================
//
//    Standard POV-Ray benchmark version 2.03 Scene File Version
//
// This is the official POV-Ray benchmark scene.  It is designed
// to test a variety of POV-Ray features and should render in a
// reasonable amount of time on current machines.  Note that the
// radiosity feature is still experimental and not very suitable for
// comparing results of different versions, therefore it is turned
// off by default.
//
// Please log all changes made to this file below.
//
// Also, please make sure the distribution copy of this file
// (`distribution/scenes/advanced/benchmark/benchmark.pov`) remains in
// sync with the inbuilt copy (`source/backend/control/benchmark_pov.cpp`)
// and the version number in `source/backend/control/benchmark.cpp`.
//
// Note that only results generated with the above options and the
// unchanged scene file are allowed to be published as 'official
// POV-Ray benchmark results'.  Feel free to do additional tests, but
// make sure the differences are made clear when publishing them.
//
// When publishing results, be sure to quote the exact version of the
// benchmark scene used (2.03 Scene File Version), and the exact
// version of POV-Ray.
//
// ==================================================================
//
// Change history
// --------------
//
// Nov. 2001   Initial release (version 3.5.0)
// Jan. 2002   using 'max()' instead of '&' in isosurface
// Apr. 2002   changed max_gradient of isosurface (new ver is 1.01)
// Jun. 2002   added photons pass_through to clouds (new ver is 1.02)
// Dec. 2010   made ready for v3.7 release (new ver is 2.00)
// Dec. 2010   last minute changes re: assumed_gamma (kept version the same)
// Oct. 2012   allow to run without installation (same ver : 2.00)
// Jan. 2013   change version to 2.01 to differentiate from beta.
// Feb  2013   Updated for v3.7
// Nov. 2017   Re-synced distribution copy with built-in copy (new ver is 2.02)
// Nov. 2017   Updated for v3.8 (new ver is 2.03)
//
// ==================================================================
//
// Suggested command line options if not using an INI file:
//
// -w512 -h512 +a0.3 +v -d -f -x
//
// The following INI options are used when the 'Run Benchmark' command
// is chosen on versions of POV-Ray that support the built-in version.
//
// All_Console=On
// All_File=
// Antialias_Depth=3
// Antialias=On
// Antialias_Gamma=2.5
// Antialias_Threshold=0.3
// Bits_Per_Color=8
// Bounding=On
// Bounding_Method=1
// Bounding_Threshold=3
// Clock=0
// Continue_Trace=Off
// Clockless_Animation=off
// Cyclic_Animation=Off
// Debug_Console=On
// Display=Off
// Display_Gamma=2.2
// Dither=off
// End_Column=1
// End_Row=1
// Fatal_Console=On
// Fatal_Error_Command=
// Fatal_Error_Return=I
// Field_Render=Off
// Final_Clock=1
// Final_Frame=1
// Grayscale_Output=off
// Height=512
// High_Reproducibility=off
// Initial_Clock=0
// Initial_Frame=1
// Include_Header=
// Input_File_Name=benchmark.pov
// Jitter_Amount=0.3
// Jitter=On
// Light_Buffer=On
// Odd_Field=Off
// Output_Alpha=Off
// Output_File_Name=
// Output_File_Type=n
// Output_To_File=Off
// Palette=3
// Pause_When_Done=Off
// Post_Frame_Command=
// Post_Frame_Return=I
// Post_Scene_Command=
// Post_Scene_Return=I
// Preview_End_Size=1
// Preview_Start_Size=1
// Pre_Frame_Command=
// Pre_Frame_Return=I
// Pre_Scene_Command=
// Pre_Scene_Return=I
// Quality=9
// Radiosity_From_File=off
// Radiosity_To_File=off
// Radiosity_Vain_Pretrace=on
// Real_Time_Raytracing=off
// Remove_Bounds=On
// Render_Block_Size=16
// Render_Block_Step=0
// Render_Console=On
// Render_Pattern=0
// Sampling_Method=1
// Split_Unions=Off
// Start_Column=0
// Start_Row=0
// Statistic_Console=On
// Subset_End_Frame=1
// Subset_Start_Frame=1
// Test_Abort_Count=0
// Test_Abort=Off
// User_Abort_Command=
// User_Abort_Return=I
// Verbose=On
// Version=3.8
// Warning_Console=On
// Width=512
//
// ==================================================================

#version 3.8;

#default { texture { finish { ambient 0.02 diffuse 1 }}}

#declare use_radiosity = false;

#declare use_photons = true;
#declare use_area_light = true;

#declare show_clouds = true;
#declare show_objects = true;

#declare Rad = 50000;

global_settings {
   assumed_gamma 1.0
   max_trace_level 12

   #if (use_radiosity=true)

      radiosity {
         pretrace_start 0.08
         pretrace_end 0.01
         count 80

         nearest_count 5
         error_bound 0.5
         recursion_limit 1

         low_error_factor .5
         gray_threshold 0.0
         minimum_reuse 0.015
         brightness 0.7

         adc_bailout 0.01/2

         normal on
      }

   #end

   #if (use_photons=true)
      photons { spacing 0.007 }
   #end

}

//====================================================================================

// This scene uses a non-standard camera set-up. 
// (See CAMERA in the included documentation for details.) 
// If you are new to POV-Ray, you might want to try a different demo scene.
camera {
   location  <3.2, 3.2, 1.8>
   direction y
   sky       z
   up        z
   right     -x*(image_width/image_height) // keep propotions with any aspect ratio
   look_at   <-1, -1, 0.9>
   angle     45
}

light_source {
   <-0.7, 0.83, 0.24>*150000
   color rgb <3.43,2.87,1.95>
   #if (use_area_light=true)
      area_light 4000*x 4000*y  4,4
      jitter
      orient
      circular
   #end

   media_attenuation on
   media_interaction on

   photons {
      reflection on
      refraction on
   }
}

#if (use_radiosity=false)
   light_source {
      <0.9, -0.6, 0.5>*150000
      color rgb 0.35

      shadowless
   }
#end

fog{
   fog_type 2
   fog_alt 1.5
   fog_offset 0
   color rgbt <0.75, 0.80, 0.86, 0.2>
   distance 400
   up z
}


//====================================================================================

// Copied from functions.inc for Oct 2012
#declare f_ridged_mf = function { internal(59) }
// Parameters: x, y, z
   // Six extra parameters required:
   // 1. H 
   // 2. Lacunarity 
   // 3. octaves
   // 4. offset 
   // 5. Gain 
   // 6. noise

#declare RMF = function{ f_ridged_mf(x, y, z, 0.07, 2.2,  7, 0.6, 0.9, 1)}

#declare M_Watx4 =
material {
   texture {
      pigment { color rgbt <0.21, 0.20, 0.3, 0.96> }
      finish {
         diffuse 0.0
         ambient 0.0

         reflection {
            0.1, 0.95
            fresnel on
            exponent 0.8
         }

         conserve_energy

         specular 0.1
         roughness 0.007
         metallic
      }
      normal{
         function { RMF(x, y, z) } 0.2
         scale 0.07
      }
   }
   interior {
      ior 1.31
      fade_distance 0.8
      fade_power 1001.0
      fade_color <0.02, 0.20, 0.06>
   }
}

box {              // --- Water ---
   <-1.95,  -1.65, 0.42>, < 1.95,  1.65, -5.5>

   material { M_Watx4 }
   hollow on

   photons { collect off }
}

#declare fn_RMF = function{ f_ridged_mf(x, y, z, 0.1, 3.1, 8 ,0.7, 0.8, 2) }


plane {            // --- floor ---
   z, -0.3

   texture {
      pigment { color rgb <1.0, 0.85, 0.6> }
      finish {
         diffuse 0.7
         specular 0.1
      }
      normal {
         function { fn_RMF(x, y, z) } 0.3
         scale 8
      }
   }

   clipped_by {
      box { <-1.95,  -1.65, 1>, < 1.95,  1.65, -1> inverse }
   }

   photons { collect off }
}


isosurface {
   function {
      z - fn_RMF(x, y, z)*0.07
   }

   max_gradient 1.3

   contained_by { box { <-15, -15, 0.0>, <15, 15, 0.2> } }

   texture {
      pigment { color rgb <1.0, 0.85, 0.6> }
      finish {
         diffuse 0.7
         specular 0.1
      }
   }

   scale 8
   translate -0.16*z

   clipped_by {
      box { <-1.95,  -1.65, 1>, < 1.95,  1.65, -1> inverse }
   }

   photons { collect off }
}


// ====================================================================================



sphere {           // --- Sky ---
   <0, 0, 0>, 1
   texture {
      pigment {
         gradient z
         pigment_map {
            [0.00 color rgb <0.6667, 0.7255, 0.7725>]
            [0.19
               spherical
               color_map {
                  [0.08 color rgb <0.33, 0.37, 0.90> ]
                  [0.14 color rgb <0.3210, 0.53, 0.9259> ]
                  [0.26 color rgb <0.3610, 0.57, 0.9259> ]
                  [0.50 color rgb < 0.880, 0.935, 0.976 > ]
               }
               scale 1.8
               translate <-0.7, 0.7, 0.24>
            ]
         }
      }
      finish {
         diffuse 0
         emission 1
      }
   }
   scale Rad*<20, 20, 4>
   translate -2*z
   no_shadow
   hollow on

   photons { collect off pass_through }
}

#if (show_clouds)

// from mick

difference {
   sphere { 0,Rad}
   sphere {0,Rad-15000}
   material{
      texture{
         pigment{ rgbf 1 }
         finish {
            ambient 0
            diffuse 0
         }
      }
      interior{
         media{
            scattering {2,<.013,.012,.008>/1.3 extinction 1/1.3 }
            method 3
            samples 7,7
            intervals 1

            density { // one
               wrinkles
               ramp_wave

               noise_generator 1

               color_map {
                  [0 rgb 0]
                  [0.5 rgb 0]
                  [0.7 rgb 0.275]
                  [1 rgb 0.475]
               } // color_map

               scale <10000,9000,5000>/3
               rotate z*29
               translate <1000,0,Rad-30000>
            } // density

            density { // two
               marble
               warp { turbulence 1.65 octaves 7 }
               noise_generator 1

               color_map {
                  [0 rgb 0]
                  [0.4 rgb 0]
                  [0.85 rgb 0.25]
                  [1 rgb 0.5]
               } // color_map

               scale <10000,7500,5000>*5
               rotate z*-25
               translate <0,0,Rad-30000>
            } // density

         } // media
      } // interior
   } // material
   hollow

   rotate z*70
   rotate y*15
   translate <0,0,32000-Rad>
   rotate -102*z

   scale <1,1,0.4>

   photons { collect off pass_through }

} // difference

#end

// ====================================================================================


#declare Metal_Texture =
texture {
   pigment { color rgb <0.65, 0.55, 0.5> }
   finish {
      ambient 0.0
      diffuse 0.15
      specular 0.3
      metallic
      roughness 0.01
      reflection {
         0.8
         metallic
      }
   }
}

#declare Stone_Tex =
texture {
   pigment {
      crackle
      pigment_map {
         [0.03
            bozo
            color_map {
               [0 color rgb <0.2, 0.14, 0.05>]
               [1 color rgb <0.2, 0.14, 0.05>]
            }
            warp { turbulence 0.6 }
         ]
         [0.055
            granite
            color_map {
               [0.0 color rgb <1, 0.95, 0.9>]
               [0.5 color rgb <0.6, 0.5, 0.52>]
               [1.0 color rgb <0.9, 0.8, 0.7>]
            }
            warp { turbulence 0.4 lambda 2.4 octaves 8 }
            scale 0.5
         ]
      }

      warp { turbulence 0.72 lambda 2.25 omega 0.53 octaves 9}

      scale 0.3
   }
   finish {
      diffuse 0.55
      specular 0.1
   }
   normal {
      granite 0.15
      scale 0.06
   }
}

#declare Stone_Tex2 =
texture {
   pigment {
      crackle
      pigment_map {
         [0.03
            bozo
            color_map {
               [0 color rgb <0.1, 0.08, 0.2>]
               [1 color rgb <0.1, 0.08, 0.2>]
            }
            warp { turbulence 0.6 }
         ]
         [0.055
            granite
            color_map {
               [0.0 color rgb <1, 0.95, 0.9>]
               [0.5 color rgb <0.6, 0.5, 0.52>]
               [1.0 color rgb <0.9, 0.8, 0.7>]
            }
            warp { turbulence 0.4 lambda 2.4 octaves 8 }
            scale 1.2
         ]
      }

      warp { turbulence 0.55 lambda 2.25 omega 0.53 octaves 9 }

      scale 0.24
   }
   finish {
      diffuse 0.55
      specular 0.1
   }
   normal {
      granite 0.15
      scale 0.06
   }
}

#declare Stone_Tex3 =
texture {
   pigment {
      agate
      pigment_map {
         [0.3
            crackle
            color_map {
               [0.1 color rgb <0.3, 0.28, 0.4>]
               [0.2 color rgb <0.8, 0.7, 0.4>]
            }
            warp { turbulence 0.5 lambda 2.2 omega 0.52 octaves 8 }
            scale 0.3
         ]
         [0.5
            granite
            color_map {
               [0.0 color rgb <1, 0.95, 0.9>]
               [0.5 color rgb <0.3, 0.6, 0.52>]
               [1.0 color rgb <0.3, 0.8, 0.7>]
            }
            warp { turbulence 0.4 lambda 2.4 octaves 8 }
         ]
      }

      warp { turbulence 0.55 }

      scale 0.24
   }
   finish {
      diffuse 0.55
      specular 0.1
   }
   normal {
      granite 0.15
      scale 0.06
   }
}

#declare Mat_Glass =
material {
   texture {
      pigment { color rgbt 1 }
      finish {
         diffuse 0
         ambient 0
         specular 0.6
         metallic 0.5
         roughness 0.005

         reflection {
            0.05, 0.95
            fresnel on
         }

         conserve_energy
      }
   }
   interior {
      ior 1.5
      fade_distance 0.12
      fade_power 1001
      fade_color <0.6, 0.5, 0.7>
   }
}


#declare Socket =
union {
   difference {
      cylinder { -5*z, -0.04*z, 0.8 }
      cylinder { -6*z, 0, 0.3 }

      #declare Cnt = 0;

      #while (Cnt<360)

         merge {
            cylinder { < 0.0,  0.0, -0.24>, < 1.0,  0.0, -0.24>, 0.1 }
            box { < 0.0,  -0.1, -0.24>, < 1.0,  0.1, -6> }

            rotate Cnt*z
         }

         #declare Cnt = Cnt+30;
      #end
   }

   #declare Cnt = 0;

   #while (Cnt<360)

      union {
         cylinder { < 0.8,  0.0, -0.04>, < 0.8,  0.0, -5>, 0.05 }
         cylinder { < 0.8,  0.0, -0.04>, < 0.8,  0.0, -0.01>, 0.07 }

         rotate 15*z
         rotate Cnt*z
      }

      #declare Cnt = Cnt+30;
   #end

   cylinder { -0.04*z, 0, 0.86 }
   torus {
      0.83, 0.03
      rotate 90*x
   }
}

#declare Pos1 = < 0.0,  0.0, 0.6>;
#declare Pos2 = <-2.4, -0.8, 1.0>;
#declare Pos3 = <-1.4, -2.8, 1.3>;
#declare Pos4 = < 1.4,  0.6, 0.5>;

#declare Pos5 = <-10, -5, 1.5>;

object { Socket translate Pos1 texture { Stone_Tex } }

object { Socket translate Pos2 texture { Stone_Tex } }

object { Socket scale 0.8 translate Pos3 texture { Stone_Tex } }

object { Socket scale 0.4 translate Pos4 texture { Stone_Tex } }

object { Socket scale 2 translate Pos5 texture { Stone_Tex } }

difference {
   box { <-2.0,  -1.7, 0.5>, < 2.0,  1.7, -6> }
   box { <-1.9,  -1.6, 1.0>, < 1.9,  1.6, -5> }

   texture {
      Stone_Tex2
   }
}

#if (show_objects)

#declare POV_Text =
text {
   internal 1 // ttf "timrom.ttf"
   "POV-Ray"
   0.25,0
   scale 0.3
   rotate 90*x
   rotate -90*z
}

#declare Version_Text =
text {
   internal 1 // ttf "timrom.ttf"
   "Version 3.8"
   0.25,0
   scale 0.3
   rotate 90*x
   rotate -90*z
}

object {
   POV_Text
   translate <-1.97,  0.995, 0.575>
   texture { Stone_Tex2 }
}

object {
   Version_Text
   rotate 90*z
   translate <-0.5, -1.5, 0.575>
   texture { Stone_Tex2 }
}

#end


height_field {

   function 300,300 {
      pigment {
         function { 1-(min(pow(x*x + y*y, 0.25), 1) -0.0001) }

         color_map {
            [0.0 rgb 0.0]
            [1.0 rgb 1.0]
         }

         translate <0.5,0.5,0>

         scale 0.45

         warp { turbulence 0.455 }

         scale 3
         warp { turbulence 0.2 lambda 2.2 octaves 8 }
         scale 1/3

      }
   }

   water_level 0.02

   rotate 90*x

   rotate 43*z

   scale <5, 5, 1.3>
   scale 2.6

   texture {
      pigment {
         bozo
         color_map {
            [0.40 color rgb <0.6, 0.6, 0.7>*0.6]
            [0.58 color rgb <0.9, 0.6, 0.3>*0.6]
            [0.62 color rgb <0.2, 0.6, 0.1>*0.4]
         }
         warp { turbulence 0.4 }
         scale <0.2, 0.2, 3>
      }
      finish {
         diffuse 0.6
         specular 0.2
      }
   }

   translate <-18, -13, -0.4>
}


#if (show_objects)

// ---------- Pos1 ----------

difference {
   cylinder { -0.0*z, 0.15*z, 0.7 }
   cylinder { -0.1*z, 0.25*z, 0.68 }

   texture { Metal_Texture }

   translate 0.6*z

   photons { target reflection on }
}

#declare fn_pigm =
function {
   pigment {
      bozo
      poly_wave 2
      color_map {
         [0 rgb 0][1 rgb 1]
      }
      warp {  turbulence 0.4 lambda 2.3 omega 0.52 }

      scale 0.2
   }
}


isosurface {
   function {
      (max(sqrt(x*x + y*y)-0.25, z-0.7))

      - fn_pigm(x, y, z).gray*0.07

   }

   max_gradient 2.4

   contained_by { box { <-0.35, -0.35, 0.0>, <0.35, 0.35, 0.8> } }

   texture {
      pigment { color rgb <1, 0.45, 0.2> }
      finish {
         diffuse 0.6
         specular 0.2
      }
   }

   translate Pos1
}

// Copied from logo.inc for Oct 2012
// The original version is made of various objects.
#declare Povray_Logo =
merge {
   sphere {2*y, 1}
   difference {
      cone {2*y, 1, -4*y, 0}
      sphere {2*y, 1.4 scale <1,1,2>}
   }
   difference {
      sphere {0, 1 scale <2.6, 2.2, 1>}
      sphere {0, 1 scale <2.3, 1.8, 2> translate <-0.35, 0, 0>}
      rotate z*30 translate 2*y
   }
   rotate <0, 0, -25>
   translate <-0.5,-0.35,0>
   scale 1/4
}


object {
   Povray_Logo

   rotate 90*x
   scale 0.4
   translate -0.2*y

   texture {
      pigment { color rgb <0.65, 0.55, 0.9> }
      finish {
         ambient 0.0
         diffuse 0.15
         specular 0.3
         metallic
         roughness 0.01
         reflection {
            0.8
            metallic
         }
      }
      normal {
         bumps 0.3
         scale 0.3
      }
   }

   rotate -25*z

   translate 0.96*z

   translate Pos1
}

#end

#if (show_objects)

// ---------- Pos2 ----------

#declare rd = seed(45);

union {

   #declare Cnt = 0;

   #while (Cnt<360)

      superellipsoid {
         <rand(rd)*2, rand(rd)*2>
         texture { Metal_Texture }
         scale 0.12
         translate <0.6, 0.0, 0.12>
         rotate (Cnt+30)*z
      }

      julia_fractal {
         < rand(rd), rand(rd)*0.6, -0.54, 0.2 >
         quaternion
         max_iteration 7
         precision 500
         scale 0.12
         translate <0.6, 0.0, 0.1>
         rotate Cnt*z

         texture {
            pigment { color rgb <1, 0.4, 0.8> }
            finish {
               ambient 0
               diffuse 0.6
               specular 0.2
               reflection 0.2
            }
         }
      }

      #declare Cnt = Cnt+60;
   #end

   translate Pos2
}

#end

#if (show_objects)

// ---------- Pos3 ----------

sphere {
   0, 0.24

   translate 0.24*z

   material { Mat_Glass }

   photons { target reflection on refraction on }

   translate Pos3
}

#end

#if (show_objects)


// ---------- Pos4 ----------


// from ingo

#declare Letter =
text {
   internal 1 // ttf "timrom.ttf"
   "X"
   1,0
   scale <1/0.7,1/0.66,1>
   translate <0,0,-0.5>
}

#declare xPigm =
function {
   pigment {
      object {
         Letter
         pigment {rgb 1}
         pigment {rgb 0}
      }
      warp {repeat x}
      warp {repeat y}
      scale 1.00002
      translate < 0.000001,-0.00001, 0>
   }
}

#declare XsinPigm =
function {
   pigment {
      function {xPigm(sin(x),pow(sin(y),2),z).gray}
   }
}

#declare Fn_Obj =
difference {
   cylinder {
      -2*y, 2*y, 2
      pigment {
         function{XsinPigm(x,y,z).gray}
         warp {planar}
         scale <0.5/pi, 1, 1>*0.5
         warp {
            cylindrical
            orientation z
            dist_exp 1
         }
         colour_map {
            [0, rgb 1]
            [1, rgbf 1]
         }
      }
      finish {
        diffuse 0.6
        specular 0.4
      }
   }
   cylinder {
      -1.9*y, 2.1*y, 1.9
      pigment {rgbf 1}
      finish {
        diffuse 0.6
        specular 0.4
      }
   }

   translate 1.96*y
   rotate 90*x
   scale 0.12
}

object {
   Fn_Obj
   translate Pos4
}

#end


#if (show_objects)


// ---------- Pos5 ----------


// from ingo

#macro BuildWriteMesh2(VecArr, NormArr, UVArr, U, V)

   #debug concat("\n\n Building mesh2: \n   - vertex_vectors\n")
   #local NumVertices = dimension_size(VecArr,1);
   mesh2 {
      vertex_vectors {
         NumVertices
         #local I = 0;
         #while (I<NumVertices)
            VecArr[I]
            #local I = I+1;
         #end
      }

      #debug concat("   - normal_vectors\n")
      #local NumVertices = dimension_size(NormArr,1);
      normal_vectors {
         NumVertices
         #local I = 0;
         #while (I<NumVertices)
            NormArr[I]
            #local I = I+1;
         #end
      }

      #debug concat("   - uv_vectors\n")
      #local NumVertices = dimension_size(NormArr,1);
      uv_vectors {
         NumVertices
         #local I = 0;
         #while (I<NumVertices)
            UVArr[I]
            #local I = I+1;
         #end
      }

      #debug concat("   - face_indices\n")
      #declare NumFaces = U*V*2;
      face_indices {
         NumFaces
         #local I = 0;
         #local H = 0;
         #while (I<V)
            #local J = 0;
            #while (J<U)
               #local Ind = (I*U)+I+J;
               <Ind, Ind+1, Ind+U+2>, <Ind, Ind+U+1, Ind+U+2>
               #local J = J+1;
               #local H = H+1;
            #end
            #local I = I+1;
         #end
      }
   }
#end

#macro FnA(X)
   #if (X<0.13)
      0.5+sin(X*14)*0.4
   #else
      0.5+sin((X-0.13)*7.2)*0.3
   #end
#end


// Build a two-dimensional array with vectors and normals retrieved from a function macro
// ResSpl: the amount of vectors to get from the macro
// based on ingo's code for splines

#macro L_GetVN(ResSpl)
   #local I = 0;
   #local A = array[ResSpl+1][2]
   #while (I<=ResSpl)
      #local P0 = 0+<FnA(I/ResSpl), I/ResSpl, 0>;
      #if (P0.x=0 & P0.z=0)
         #local P0 = <1e-25,P0.y,1e-25>;
      #end
      #if (I=0)
         #local P1 = 0+<FnA(((I-0.5)/ResSpl)), I/ResSpl, 0>;
         #local P2 = 0+<FnA(((I+0.5)/ResSpl)), I/ResSpl, 0>;
      #else
         #local P1 = P2;
         #local P2 = 0+<FnA(((I+0.5)/ResSpl)), I/ResSpl, 0>;
      #end
      #local P3 = vrotate(P0,<0,1,0>);
      #local P4 = vrotate(P0,<0,-1,0>);
      #local B1 = P4-P0;
      #local B2 = P2-P0;
      #local B3 = P3-P0;
      #local B4 = P1-P0;
      #local N1 = vcross(B1,B2);
      #local N2 = vcross(B2,B3);
      #local N3 = vcross(B3,B4);
      #local N4 = vcross(B4,B1);
      #local N = vnormalize((N1+N2+N3+N4)*-1);
      #local A[I][0] = P0;
      #local A[I][1] = N;
      #local I = I+1;
   #end
   A
#end

#macro FnLathe (Rot, ResRot, ResSpl)
   #declare VNArr = L_GetVN (ResSpl)
   #local VecArr = array[(ResRot+1)*(ResSpl+1)]
   #local NormArr = array[(ResRot+1)*(ResSpl+1)]
   #local UVArr = array[(ResRot+1)*(ResSpl+1)]
   #local R = Rot/ResRot;
   #local Dim = dimension_size(VNArr,1);
   #local Count = 0;
   #local I = 0;
   #while (I<=ResRot)
      #local J = 0;
      #while (J<Dim)
         #local VecArr[Count] = vrotate(VNArr[J][0],<0,R*I,0>);
         #local NormArr[Count] = vrotate(VNArr[J][1],<0,R*I,0>);
         #local UVArr[Count] = <I/ResRot,J/(Dim-1)>;
         #local J = J+1;
         #local Count = Count+1;
      #end
      #local I = I+1;
   #end
   BuildWriteMesh2(VecArr, NormArr, UVArr, ResSpl, ResRot)
#end


#declare MSH = FnLathe(360, 100, 100)

#declare Obj_Msh =
object {
   MSH

   uv_mapping

   texture {
      pigment{
         checker
         color rgb <1.0, 0.7, 0.5>,
         color rgb <0, 0, 0.15>

         scale 0.05
      }
      finish {
         diffuse 0.7
         specular 0.3
      }
   }

   rotate 90*x
   scale <1.9, 1.9, 2.8>


   translate Pos5
}


object { Obj_Msh }


#end
