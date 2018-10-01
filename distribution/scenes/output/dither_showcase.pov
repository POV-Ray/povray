// POV-Ray 3.8 Scene File "dither_showcase.pov"
// author:  Christoph Lipka
// date:    2018-09-30
//
//--------------------------------------------------------------------------
#version 3.8;

#ifndef (Glow)
  #declare Glow = on;
#end
#ifndef (Brightness)
  #declare Brightness = 4.0;
#end

global_settings {
  max_trace_level 5
  assumed_gamma 1.0
  radiosity {
    pretrace_start 0.08
    pretrace_end   0.01
    count 150
    nearest_count 20
    error_bound 0.5
    recursion_limit 2
    low_error_factor .5
    gray_threshold 0.0
    minimum_reuse 0.015
    brightness 1
    adc_bailout 0.01/2
  }
}

#default {
  texture {
    pigment {rgb 1}
    finish {
      ambient 0.0
      diffuse 0.8
      specular albedo 1.0 roughness 0.001
      reflection { 1.0 fresnel on }
      conserve_energy
      fresnel on
    }
  }
}

// ----------------------------------------

#local TestRed   = <1.0,.03,.03>;
#local TestGreen = <.03,1.0,.03>;
#local TestBlue  = <.03,.03,1.0>;

#local CameraFocus = <0,1,1>;
#local CameraDist  = 8;
#local CameraDepth = 3.0;
#local CameraTilt  = 5;

camera {
  location  <0,0,0>
  direction z*CameraDepth
  right     x*image_width/image_height
  up        y
  translate <0,0,-CameraDist>
  rotate    x*CameraTilt
  translate CameraFocus
}

#macro LightSource(Pos,Color)
  light_source {
    Pos
    color Color
    area_light x*vlength(Pos)/10, y*vlength(Pos)/10, 9,9 adaptive 1 jitter circular orient
  }
  
#end

LightSource(<-500,500,-500>, rgb Brightness)

// ----------------------------------------

plane {
  y, 0
  texture { pigment { color rgb 0.2 } }
  interior { ior 1.5 }
}

#macro TestSphere(Pos,Radius,TargetColor,Hole)
  #if (Hole)
    union {
      #local Th = 20;
      #local R = 0.05;
      #local SinTh = sin(Th*pi/180);
      #local CosTh = cos(Th*pi/180);
      difference {
        sphere { <0,0,0>, 1 }
        cylinder { y, y*(1-R)*CosTh, SinTh }
        cylinder {-y,-y*(1-R)*CosTh, SinTh }
        cylinder { y,-y,(1-R)*SinTh-R }
      }
      torus { (1-R)*SinTh, R translate  y*(1-R)*CosTh }
      torus { (1-R)*SinTh, R translate -y*(1-R)*CosTh }
  #else
    sphere { <0,0,0>, 1
  #end
    texture { pigment { color  TargetColor }
      finish { emission Glow * Brightness * 0.5 }
    }
    interior { ior 1.5 }
    rotate z*30
    rotate y*clock*360 - y*45
    scale Radius
    translate Pos + y*Radius
  }
#end

TestSphere(<-2,0,1>, 1, TestRed,   false)
TestSphere(< 0,0,1>, 1, TestBlue,  true)
TestSphere(< 2,0,1>, 1, TestGreen, false)
