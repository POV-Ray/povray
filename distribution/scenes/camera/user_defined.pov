//----------------------- user_defined.pov -------------------------
// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.
//
//----------------- Inward facing cylinderical ---------------------
//
// +w900 +h450 +a0.1

#version 3.71;
global_settings { assumed_gamma 1 }

#include "shapes.inc"
background { color rgb <0.5,0.5,0.5> }
#declare OurText = "Round and round we go."
#declare TextAboutY = object {
    Circle_Text_Valigned("crystal.ttf",OurText,
         0.275,0.001,0.02,0.5,0,Align_Left,-5,90)
    pigment { color rgb <0,0,0> }
    rotate x*-90
    translate <0,-0.05,0>
}
#declare CameraPerspective = camera {
    perspective
    location <2.3,2.3,-2.301>
    sky <0,1,0>
    angle 35
    right x*(image_width/image_height)
    look_at <0,0,0>
}
#declare CylY = cylinder { <0,-1,0>, <0,1,0>, 0.01
    texture { pigment { color rgb <0.859,0.910,0.831> }
              finish { ambient 0 diffuse 0 emission 1 }
    }
}
//------- Set up functional camera
#declare FnXrad = function (x) { (x+0.5)*tau }
#declare FnX    = function (x) { cos(FnXrad(x)) }
#declare FnZ    = function (x) { sin(FnXrad(x)) }
#declare CameraUserDefined = camera {
    user_defined
    location {
      function { FnX(x) }
      function { y }
      function { FnZ(x) }
    }
    direction {
      function { -FnX(x) }
      function { 0 }
      function { -FnZ(x) }
    }
}
//---
  camera { CameraUserDefined }
//camera { CameraPerspective } // Use for perspective scene view
object { CylY }
object { TextAboutY }

