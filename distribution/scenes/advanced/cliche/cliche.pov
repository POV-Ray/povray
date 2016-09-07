// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// "No More Chrome Spheres Please!"
// Copyright 2001 Rune S. Johansen
// 
// The "chrome sphere over checkered plane" is an overused raytracing cliche.
// The last thing we need is another of those images...
// 
// ...but who cares! ;)
// 
// NOTE:
// First render the file SIGN.POV and then render this file.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.6;

global_settings {assumed_gamma 2.2 max_trace_level 5}

#declare ChromeFinish =
finish {brilliance 2.5 phong 1 reflection {0.5}}

#declare Sphere =
union {
   #declare Back =
   bicubic_patch {
      type 1 flatness 0 u_steps 5 v_steps 5
      <1,0.000, 0.000>, <1,0.553, 0.000>, <0.553,1.000, 0.000>, <0,1.000, 0.000>,
      <1,0.006,-0.003>, <1,0.553,-0.300>, <0.553,1.000,-0.553>, <0,1.000,-0.553>,
      <1,0.003,-0.006>, <1,0.300,-0.553>, <0.553,0.553,-1.000>, <0,0.553,-1.000>,
      <1,0.000, 0.000>, <1,0.000,-0.553>, <0.553,0.000,-1.000>, <0,0.000,-1.000>
   }
   #declare Upper =
   bicubic_patch {
      type 1 flatness 0 u_steps 5 v_steps 5
      <1.000,0.000,0>,<1.000,0.000,0.553>,<1.039,0.1,0.6>,<0.779,0.1,0.45>,
      <1.000,0.553,0>,<1.000,0.553,0.553>,<1.039,0.1,0.6>,<0.779,0.1,0.45>,
      <0.553,1.000,0>,<0.553,1.000,0.850>,<0.450,0.1,1.2>,<0.450,0.1,0.90>,
      <0.000,1.000,0>,<0.000,1.000,0.850>,<0.000,0.1,1.2>,<0.000,0.1,0.90>
   }
   #declare Lower =
   bicubic_patch {
      type 1 flatness 0 u_steps 5 v_steps 5
      <1.000, 0.000,0>,<1.000, 0.000,0.553>,<1.039, 0.1,0.6>,<0.779, 0.1,0.45>,
      <1.000,-0.553,0>,<1.000,-0.553,0.553>,<1.039,-0.6,0.6>,<0.779,-0.6,0.45>,
      <0.553,-1.000,0>,<0.553,-1.000,0.850>,<0.450,-0.4,1.2>,<0.450,-0.4,0.90>,
      <0.000,-1.000,0>,<0.000,-1.000,0.850>,<0.000,-0.4,1.2>,<0.000,-0.4,0.90>
   }
// Chrome "sphere"
   union {
      object {Back scale <-1,-1,1>}
      object {Back scale < 1,-1,1>}
      object {Back scale <-1, 1,1>}
      object {Back scale < 1, 1,1>}
      object {Upper scale <-1,1,1>}
      object {Upper scale < 1,1,1>}
      object {Lower scale <-1,1,1>}
      object {Lower scale < 1,1,1>}
      pigment {color rgb 0.7}
      finish {ChromeFinish}
   }
// Black "disc" to block view of inside of chrome sphere
   sphere {
      0, 1 scale <0.999,0.999,0.1>
      pigment {color 0}
   }
// Teeth
   union {
      torus {0.7, 0.05 scale <1,3,1> translate <0,+0.10,0.2>}
      torus {0.7, 0.05 scale <1,3,1> translate <0,-0.50,0.2>}
      pigment {color rgb 2}
      finish {ChromeFinish}
   }
// Tongue
   blob {
      threshold 1
      #declare X = 0;
      #declare Y = 40;
      #while (X<=Y)
         #declare V = X/Y;
         #declare W = 2*sin(acos(V))+0.001;
         cylinder {
            -x*W, x*W, 0.7, 0.5
            scale 0.2+0.1*V
            translate y
            rotate (-45+95*V)*x
            translate <0,-1.3,0.8>
         }
         #declare X = X+1;
      #end
      pigment {color <1,0,0>}
      finish {ChromeFinish}
   }
}

#declare Sign =
union {
   superellipsoid {
      0.15
      texture {
         pigment {image_map {"sign.jpg"} scale 2 translate -1}
         finish {ambient 0.1 diffuse 0.8 phong 1}
      }
      texture { // dirt layer
         pigment {
            bozo scale 0.3
            turbulence 1
            color_map {[0.5,rgbt <0.7,0.6,0.5,1.0>][1.0,rgbt <0.7,0.6,0.5,0.8>]}
         }
      }
      scale <0.7,0.7,0.02>
      translate <0,1.8,-0.12>
   }
   cylinder {
      0, 2.6*y, 0.1
      pigment {color rgb 0.8}
      normal {bumps 0.1 scale 0.01}
      finish {brilliance 2 phong 1 reflection {0.2}}
   }
}

#declare Plane =
plane {
   y, 0
   texture {
      pigment {
         checker
         pigment {
            gradient x triangle_wave turbulence 1
            color_map {[0, color rgb 0.0][1, color rgb 0.2]}
         }
         pigment {
            gradient z triangle_wave turbulence 1
            color_map {[0, color rgb 1.0][1, color rgb 0.8]}
         }
      }
      finish {ambient 0.1 diffuse 0.7 phong 1 reflection {0.2}}
   }
   texture {
      pigment {
         boxed translate <1,0,1> scale 0.5
         warp {repeat x} warp {repeat z}
         color_map {[0, color transmit 0][0.03, color transmit 1]}
      }
   }
}

camera {
   location <6,0.9,-3>
   right x*image_width/image_height
   angle 70
   look_at <0,2.4,-0.7>
}

light_source {<2,3,-1>*1000, color <0.8,0.9,1.0>}
light_source {<3,1,+1>*1000, color <0.2,0.2,0.2>}
light_source {<1,1,-3>*1000, color <0.4,0.4,0.3>}

sky_sphere {
   pigment {
      gradient y translate -0.5*y scale 4
      turbulence 0.05 octaves 7 lambda 5.0 omega 0.6
      color_map {
         [0.50, color <0.6,0.3,0.8>]
         [0.75, color <0.1,0.2,0.5>]
      }
   }
}

fog {
   fog_type 2
   color <0.6,0.3,0.8>
   distance 50
   fog_offset 0
   fog_alt 2
}

object {Sphere rotate 45*y translate <-1,2,-2>}
object {Sign translate 2*z}
object {Plane}
