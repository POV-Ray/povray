// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Ray Tracer Scene Description File
// File: object_pattern.pov
// Desc: Demo scene showing the following new features:
//          - object pattern
//          - cylindrical warp
//          - new radiosity
// Date: August 2001
// Auth: Christoph Hormann
// Updated: 2013/02/15 for 3.7

// -w320 -h240
// -w512 -h384 +a0.3

#version 3.7;
global_settings { assumed_gamma 1.0 } 

global_settings {
  assumed_gamma 1.0
  max_trace_level 5

  radiosity {
    pretrace_start 0.08
    pretrace_end   0.02
    count 50

    nearest_count 5
    error_bound 0.15
    recursion_limit 1

    low_error_factor .5
    gray_threshold 0.0
    minimum_reuse 0.015
    brightness 1

    adc_bailout 0.01/2

  }
}

camera {
  location    <6, 3, 6>
  right x*image_width/image_height // keep propotions with any aspect ratio
  look_at     <0,0.5,0>
  angle       36
}

light_source {
  <-3, 10, 18>
  color rgb <1.0, 0.9, 0.8>*2.4
}

sphere {
  <0, 0, 0>, 1
  texture {
   pigment {
     gradient y
     color_map {
       [0.0 color rgb < 1.0, 1.0, 1.0 >]
       [0.3 color rgb < 0.6, 0.7, 1.0 >]
     }
   }
   finish { diffuse 0 ambient 1 }
  }
  hollow on
  no_shadow
  scale 30000
}

#declare Tex_1=
texture {
  pigment {
    color rgb 0.6
  }
  finish {
    ambient 0.0
    diffuse 0.6
    specular 0.15
  }
}

plane {
  y, 0
  texture { Tex_1 }
}

#declare Text_Obj=
text {
  ttf
  "timrom.ttf",
  " POV-Ray 3.7", // changed to 3.7 17Aug2008 (jh)
  12, 0.0

  scale 0.26
  translate -0.5
  rotate -90*z
  translate 0.5
  translate 0.8*y
}

#declare Tex_Metal_A=
texture {
  pigment {
    gradient y
    color_map {
      [0.35 color rgb <0.65,0.2,0.1> ]
      [0.35 color rgb <0.15,0.2,0.4> ]
    }
    scale 2
  }
  finish {
    ambient 0.0
    diffuse 0.2
    specular 0.4
    roughness 0.1
    metallic

    reflection {
      0.8
      metallic
    }
  }
  normal {
    granite 0.05
    warp {turbulence 1}
    scale 0.1
  }
}


#declare Tex_Column=
texture {
  object {
    Text_Obj
    texture { Tex_1 },
    texture { Tex_Metal_A }
  }

  warp { cylindrical }
}


#declare Obj=
union {
  cylinder { 0, 2.1*y, 0.2 texture { Tex_Column } }
  torus { 0.2, 0.04 translate 0.04*y }
  torus { 0.2, 0.04 translate 2.06*y }
  texture { Tex_1 }
}

#macro Obj2(Angle, Flip)
  Obj
  rotate Angle*y
  #if (Flip)
    rotate 180*z
    translate 2.1*y
  #end
  rotate -90*x
  translate 0.24*y
#end

#macro Obj3(Angle)
  Obj
  rotate Angle*y
  rotate 90*x
  rotate 180*z
  scale <-1,-1,1>
  translate 0.24*y
  translate -2.1*z
  scale 0.45
  translate -0.4*z
#end

#macro Obj4(Angle)
  Obj
  rotate Angle*y
  scale 0.8
  translate 1.2*x
#end

object { Obj }

object { Obj2(0, false) translate <0.5,0,1> }

object { Obj3(190) rotate  90*y translate <-1,0,0.9> }
object { Obj3(190) rotate 110*y translate <-1,0,0.9> }
object { Obj3(190) rotate 130*y translate <-1,0,0.9> }
object { Obj3(190) rotate 150*y translate <-1,0,0.9> }
object { Obj3(190) rotate 170*y translate <-1,0,0.9> }
object { Obj3(190) rotate 190*y translate <-1,0,0.9> }

object { Obj rotate -20*y scale 0.6 translate <-1,0,0.9> }

object { Obj2(20, true) rotate 110*y translate <-1.4,0,-1.2> }

object { Obj2(20, true) scale 0.7 rotate 110*y translate <1.7,0,1.2> }

object { Obj4(260) rotate  20*y translate <0.6,0,-0.5> }
object { Obj4(260) rotate  40*y translate <0.6,0,-0.5> }
object { Obj4(260) rotate  60*y translate <0.6,0,-0.5> }
object { Obj4(260) rotate  80*y translate <0.6,0,-0.5> }
object { Obj4(260) rotate 100*y translate <0.6,0,-0.5> }
object { Obj4(260) rotate 120*y translate <0.6,0,-0.5> }

object { Obj2(-20, true) scale 0.42 rotate 160*y translate <1.5,0,-0.4> }
object { Obj2(-20, true) scale 0.42 rotate 160*y translate <1.7,0,-0.25> }
object { Obj2(-20, true) scale 0.42 rotate 160*y translate <1.9,0,-0.1> }
object { Obj2(-20, true) scale 0.42 rotate 160*y translate <2.1,0, 0.05> }
